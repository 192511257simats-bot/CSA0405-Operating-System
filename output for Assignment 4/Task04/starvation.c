/* Starvation demo: head at 53, queue {98,183,37,122,14,124,65,67} plus a new request near cylinder 60 EVERY millisecond.
   Service = 1.0 ms + 0.01 ms per cylinder moved, so the server is never faster than the arrivals. */
#include <stdio.h>
#include <stdlib.h>
#define STREAM_MS 5000
#define MAXREQ (STREAM_MS + 16)
static int cyl[MAXREQ]; static double arr[MAXREQ]; static char done[MAXREQ];

static double run(int sstf, double *t_end_stream)
{
    int init[8] = {98, 183, 37, 122, 14, 124, 65, 67}, n = 0;
    for (int i = 0; i < 8; i++) { cyl[n] = init[i]; arr[n] = 0; done[n++] = 0; }
    srand(1);
    for (int ms = 1; ms <= STREAM_MS; ms++) { cyl[n] = 55 + rand() % 11; arr[n] = ms; done[n++] = 0; }   /* 55..65 */
    int head = 53, dir = 1, left = n; double t = 0, t183 = -1; (void)t_end_stream;
    while (left > 0) {
        int pick = -1, best = 1 << 30;
        for (int i = 0; i < n; i++) {
            if (done[i] || arr[i] > t) continue;
            if (sstf) { int d = abs(cyl[i] - head); if (d < best) { best = d; pick = i; } }
            else { int ahead = dir > 0 ? cyl[i] >= head : cyl[i] <= head; if (!ahead) continue; int d = abs(cyl[i] - head); if (d < best) { best = d; pick = i; } }
        }
        if (pick < 0) {                                    /* SCAN: nothing ahead -> turn round; or idle until next arrival */
            int any = 0; for (int i = 0; i < n; i++) if (!done[i] && arr[i] <= t) any = 1;
            if (any) dir = -dir; else t += 1;
            continue;
        }
        t += 1.0 + 0.01 * abs(cyl[pick] - head); head = cyl[pick]; done[pick] = 1; left--;
        if (cyl[pick] == 183 && arr[pick] == 0) t183 = t;
    }
    return t183;
}

int main(void)
{
    double e = 0;
    double s = run(1, &e), c = run(0, &e);
    printf("Arrivals: 1 new request/ms near cylinder 60 for %d ms\n", STREAM_MS);
    printf("SSTF: request at cylinder 183 (arrived at t=0) finally served at t = %.1f ms\n", s);
    printf("SCAN: request at cylinder 183 (arrived at t=0) served at        t = %.1f ms\n", c);
    return 0;
}
