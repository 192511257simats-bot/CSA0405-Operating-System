/* Disk-scheduling simulator: 10 000 random requests, cylinders 0-199.
   Time unit = 1 tick: the head moves 1 cylinder per tick, each request needs 5 ticks of rotation+transfer.
   waiting time = completion time - arrival time. Return jumps of C-SCAN / C-LOOK are counted as head movement. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NREQ 10000
#define MAXCYL 199
#define XFER 5
enum { FCFS, SSTF, SCAN, CSCAN, LOOK, CLOOK, NALG };
static const char *names[NALG] = {"FCFS", "SSTF", "SCAN", "C-SCAN", "LOOK", "C-LOOK"};

static double arrive[NREQ]; static int cyl[NREQ];
static double done_at[NREQ]; static char served[NREQ];

static unsigned long long rs = 88172645463325252ULL;
static double urand(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (rs >> 11) * (1.0 / 9007199254740992.0); }

static void gen(double mean_gap)
{
    double t = 0; rs = 88172645463325252ULL;
    for (int i = 0; i < NREQ; i++) { t += -mean_gap * log(1.0 - urand()); arrive[i] = t; cyl[i] = (int)(urand() * (MAXCYL + 1)); }
}

static void run(int alg, double *avg_move, double *max_wait, double *avg_wait)
{
    static int pend[NREQ]; int np = 0, next = 0, finished = 0;
    double t = 0; int head = 100, dir = 1; double move = 0;
    memset(served, 0, sizeof served);
    while (finished < NREQ) {
        while (next < NREQ && arrive[next] <= t) pend[np++] = next++;
        if (np == 0) { t = arrive[next]; continue; }            /* idle: head stays put */
        int pick = -1, target = -1;
        if (alg == FCFS) {
            for (int k = 0; k < np; k++) if (pick < 0 || pend[k] < pend[pick]) pick = k;
        } else if (alg == SSTF) {
            for (int k = 0; k < np; k++) if (pick < 0 || abs(cyl[pend[k]] - head) < abs(cyl[pend[pick]] - head)) pick = k;
        } else if (alg == SCAN || alg == LOOK) {
            for (int attempt = 0; attempt < 2 && pick < 0; attempt++) {
                for (int k = 0; k < np; k++) {
                    int c = cyl[pend[k]]; if (dir > 0 ? c < head : c > head) continue;
                    if (pick < 0 || abs(c - head) < abs(cyl[pend[pick]] - head)) pick = k;
                }
                if (pick < 0) {                                  /* nothing ahead: reverse */
                    if (alg == SCAN) { int edge = dir > 0 ? MAXCYL : 0; move += abs(edge - head); t += abs(edge - head); head = edge; }
                    dir = -dir;
                }
            }
        } else {                                                 /* C-SCAN / C-LOOK: always upwards */
            for (int k = 0; k < np; k++) { int c = cyl[pend[k]]; if (c >= head && (pick < 0 || c < cyl[pend[pick]])) pick = k; }
            if (pick < 0) {                                      /* nothing above: wrap round */
                int to;
                if (alg == CSCAN) { move += MAXCYL - head; t += MAXCYL - head; head = MAXCYL; to = 0; }
                else { to = MAXCYL; for (int k = 0; k < np; k++) if (cyl[pend[k]] < to) to = cyl[pend[k]]; }
                move += abs(head - to); t += abs(head - to); head = to;
                for (int k = 0; k < np; k++) { int c = cyl[pend[k]]; if (c >= head && (pick < 0 || c < cyl[pend[pick]])) pick = k; }
            }
        }
        target = pend[pick]; pend[pick] = pend[--np];
        int d = abs(cyl[target] - head); move += d; t += d + XFER; head = cyl[target];
        done_at[target] = t; served[target] = 1; finished++;
    }
    double mw = 0, sw = 0;
    for (int i = 0; i < NREQ; i++) { double w = done_at[i] - arrive[i]; sw += w; if (w > mw) mw = w; }
    *avg_move = move / NREQ; *max_wait = mw; *avg_wait = sw / NREQ;
}

/* fairness statistics computed from the last run(): spread of the waiting time and edge-vs-middle cylinders */
static void fairness(double *sd, double *edge, double *mid)
{
    double s = 0, s2 = 0, e = 0, m = 0; int ne = 0, nm = 0;
    for (int i = 0; i < NREQ; i++) {
        double w = done_at[i] - arrive[i]; s += w; s2 += w * w;
        if (cyl[i] < 20 || cyl[i] > 179) { e += w; ne++; } else if (cyl[i] >= 90 && cyl[i] <= 109) { m += w; nm++; }
    }
    *sd = sqrt(s2 / NREQ - (s / NREQ) * (s / NREQ)); *edge = e / ne; *mid = m / nm;
}

int main(void)
{
    const char *loads[3] = {"Light (gap 400)", "Medium (gap 100)", "Heavy (gap 45)"};
    double gaps[3] = {400, 100, 45};
    double fsd[NALG], fe[NALG], fm[NALG];
    printf("%d random requests, cylinders 0-%d, seek 1 tick/cylinder, +%d ticks per transfer\n\n", NREQ, MAXCYL, XFER);
    for (int l = 0; l < 3; l++) {
        gen(gaps[l]);
        printf("%s\n%-8s %-22s %-18s %-18s\n", loads[l], "Alg", "Avg head move/request", "Avg wait (ticks)", "Max wait (ticks)");
        for (int a = 0; a < NALG; a++) {
            double am, mw, aw; run(a, &am, &mw, &aw);
            printf("%-8s %-22.2f %-18.1f %-18.1f\n", names[a], am, aw, mw);
            fsd[a] = 0; fe[a] = 0; fm[a] = 0; fairness(&fsd[a], &fe[a], &fm[a]);
        }
        printf("  Fairness: %-8s %-14s %-24s %-24s\n", "Alg", "StdDev wait", "Mean wait, edge cyl", "Mean wait, middle cyl");
        for (int a = 0; a < NALG; a++) printf("            %-8s %-14.1f %-24.1f %-24.1f\n", names[a], fsd[a], fe[a], fm[a]);
        printf("\n");
    }
    return 0;
}
