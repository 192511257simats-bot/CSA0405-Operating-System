/* Task 6 Stage 5 - workload simulation of two storage designs for the lecture-video start-up.
   Workload : 2 000 videos x 1 GB, Zipf(1) popularity, S concurrent streams, each fetching a 4 MB chunk every 10.67 s
              (3 Mbit/s), 10 % of requests are "jump to a random minute / start-up" requests.
   Device models (assumptions, see report): HDD seek = 1 + 0.04*|dcyl| ms, rotation U(0,8.33) ms, 4 MB @ 180 MB/s = 22.2 ms
                                            NVMe 0.1 ms + 4 MB @ 2.5 GB/s = 1.7 ms ; RAM cache 0.05 ms
   Design A: all 2 000 videos on 40 striped HDDs, FCFS queue, ext3-style indirect index (2 extra index reads per jump)
   Design B: top 20 videos in RAM cache, next 180 on 2 NVMe SSDs, remaining 1 800 on 12 HDDs scheduled with LOOK, extent-based
             file system (index in the inode, 0 extra reads)                                                          */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define V 2000
#define CHUNKS 256
#define SIM_S 120.0
#define MAXREQ 400000

static unsigned long long rs = 0x9E3779B97F4A7C15ULL;
static double urand(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (rs >> 11) * (1.0 / 9007199254740992.0); }

typedef struct { double t; int v, chunk, jump, tier, dev, cyl; double lat; } Req;
static Req R[MAXREQ]; static int nreq;
static double zcum[V];

static int pick_video(void) { double u = urand() * zcum[V - 1]; int lo = 0, hi = V - 1; while (lo < hi) { int m = (lo + hi) / 2; if (zcum[m] < u) lo = m + 1; else hi = m; } return lo; }

static double hdd_seek(int d) { return d == 0 ? 0.0 : 1.0 + 0.04 * d; }
static double hdd_xfer = 22.2;

/* Single-server queue for one device; policy 0 = FCFS, 1 = LOOK. idx[] = requests of this device in arrival order */
static void serve_hdd(int *idx, int n, int policy, int design_a)
{
    static int pend[MAXREQ]; int np = 0, next = 0, done = 0, head = 0, dir = 1; double t = 0;
    while (done < n) {
        while (next < n && R[idx[next]].t <= t) pend[np++] = idx[next++];
        if (np == 0) { t = R[idx[next]].t; continue; }
        int pick = -1;
        if (policy == 0) { for (int k = 0; k < np; k++) if (pick < 0 || pend[k] < pend[pick]) pick = k; }
        else {
            for (int att = 0; att < 2 && pick < 0; att++) {
                for (int k = 0; k < np; k++) {
                    int c = R[pend[k]].cyl; if (dir > 0 ? c < head : c > head) continue;
                    if (pick < 0 || abs(c - head) < abs(R[pend[pick]].cyl - head)) pick = k;
                }
                if (pick < 0) dir = -dir;
            }
        }
        int r = pend[pick]; pend[pick] = pend[--np];
        double svc = hdd_seek(abs(R[r].cyl - head)) + urand() * 8.33 + hdd_xfer;
        if (design_a && R[r].jump) svc += 2 * (3.6 + urand() * 8.33);       /* two index-block reads (double indirect) */
        head = R[r].cyl; t += svc; R[r].lat = t - R[r].t; done++;
    }
}

static void serve_ssd(int *idx, int n)
{
    double t = 0;
    for (int i = 0; i < n; i++) { Req *q = &R[idx[i]]; if (t < q->t) t = q->t; t += 1.7; q->lat = t - q->t; }
}

static int cmpd(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return (x > y) - (x < y); }

static void report(const char *name, int streams, int *ndev, int ndev_total, double *sat_util, FILE *csv)
{
    static double all[MAXREQ], jmp[MAXREQ]; int na = 0, nj = 0, late = 0; double sum = 0;
    for (int i = 0; i < nreq; i++) { all[na++] = R[i].lat; sum += R[i].lat; if (R[i].lat > 500.0) late++; if (R[i].jump) jmp[nj++] = R[i].lat; }
    qsort(all, na, sizeof(double), cmpd); qsort(jmp, nj, sizeof(double), cmpd);
    printf("%-9s %7d %9.1f %9.1f %9.1f %10.1f %9.2f%% %9.1f %9.1f  %s\n", name, streams, sum / na, all[(int)(0.95 * na)], all[(int)(0.99 * na)], all[na - 1],
           100.0 * late / na, jmp[(int)(0.50 * nj)], jmp[(int)(0.95 * nj)], "");
    fprintf(csv, "%s,%d,%.2f,%.2f,%.2f,%.2f,%.3f,%.2f,%.2f\n", name, streams, sum / na, all[(int)(0.95 * na)], all[(int)(0.99 * na)], all[na - 1], 100.0 * late / na, jmp[(int)(0.50 * nj)], jmp[(int)(0.95 * nj)]);
    (void)ndev; (void)ndev_total; (void)sat_util;
}

static void simulate(int design, int streams, FILE *csv)
{
    double lambda = streams / 10.67;                       /* chunk requests per second */
    rs = 0x9E3779B97F4A7C15ULL; nreq = 0;
    double t = 0;
    while (1) {
        t += -log(1.0 - urand()) / lambda * 1000.0;        /* ms */
        if (t > SIM_S * 1000.0 || nreq >= MAXREQ) break;
        Req *q = &R[nreq++]; q->t = t; q->v = pick_video(); q->chunk = (int)(urand() * CHUNKS); q->jump = urand() < 0.10; q->lat = 0;
        int v = q->v, c = q->chunk;
        if (design == 0) { q->tier = 2; q->dev = (v + c) % 40; q->cyl = v / 10; }
        else if (v < 20)  { q->tier = 0; q->dev = 0; q->cyl = 0; }
        else if (v < 200) { q->tier = 1; q->dev = (v + c) % 2; q->cyl = 0; }
        else              { q->tier = 2; q->dev = (v + c) % 12; q->cyl = v / 10; }
    }
    static int idx[MAXREQ]; int n;
    int ndev_hdd = design == 0 ? 40 : 12;
    for (int d = 0; d < ndev_hdd; d++) { n = 0; for (int i = 0; i < nreq; i++) if (R[i].tier == 2 && R[i].dev == d) idx[n++] = i; if (n) serve_hdd(idx, n, design == 0 ? 0 : 1, design == 0); }
    if (design == 1) {
        for (int d = 0; d < 2; d++) { n = 0; for (int i = 0; i < nreq; i++) if (R[i].tier == 1 && R[i].dev == d) idx[n++] = i; if (n) serve_ssd(idx, n); }
        for (int i = 0; i < nreq; i++) if (R[i].tier == 0) R[i].lat = 0.05;
    }
    int cnt[3] = {0, 0, 0}; for (int i = 0; i < nreq; i++) cnt[R[i].tier]++;
    printf("   [%s, %d streams] requests=%d  RAM=%.1f%%  SSD=%.1f%%  HDD=%.1f%%\n", design ? "B" : "A", streams, nreq, 100.0 * cnt[0] / nreq, 100.0 * cnt[1] / nreq, 100.0 * cnt[2] / nreq);
    report(design ? "B tiered" : "A hdd-only", streams, NULL, 0, NULL, csv);
}

int main(void)
{
    double s = 0; for (int i = 0; i < V; i++) { s += 1.0 / (i + 1); zcum[i] = s; }
    FILE *csv = fopen("stream_results.csv", "w"); if (!csv) return 1;
    fprintf(csv, "design,streams,mean_ms,p95_ms,p99_ms,max_ms,pct_over_500ms,jump_p50_ms,jump_p95_ms\n");
    printf("Chunk-read latency in ms (4 MB chunk). SLO: p99 <= 500 ms.   Simulated time %.0f s per run\n", SIM_S);
    printf("%-9s %7s %9s %9s %9s %10s %10s %9s %9s\n", "Design", "streams", "mean", "p95", "p99", "max", ">500ms", "jump p50", "jump p95");
    int loads[3] = {5000, 10000, 15000};
    for (int l = 0; l < 3; l++) { simulate(0, loads[l], csv); simulate(1, loads[l], csv); }
    fclose(csv);
    return 0;
}
