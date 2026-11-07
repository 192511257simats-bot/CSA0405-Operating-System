/* Q2: FCFS, SJF (non-preemptive), SRTF, Priority (non-preemptive, small number = high), Round Robin
   on the same job set. Time-stepped simulator: prints Gantt, WT, TAT, RT. */
#include <stdio.h>
#include <string.h>
#define N 4
const char *nm[N] = {"P1","P2","P3","P4"};
int at[N] = {0,1,2,3}, bt[N] = {8,4,9,5}, pr[N] = {3,1,4,2};
enum {FCFS, SJF, SRTF, PRIO, RR};

void run(int pol, int q, const char *title)
{
    int left[N], fin[N], first[N], inq[N] = {0}, done = 0, t = 0, cur = -1, slice = 0;
    int queue[4096], h = 0, tl = 0, last = -1, seg_start = 0, sw = 0;
    for (int i = 0; i < N; i++) { left[i] = bt[i]; first[i] = -1; }
    printf("%s\nGantt:", title);
    while (done < N) {
        for (int i = 0; i < N; i++) if (pol == RR && !inq[i] && at[i] <= t) { queue[tl++] = i; inq[i] = 1; }
        int pick = -1;
        if (pol == RR) {
            if (cur >= 0 && left[cur] > 0 && slice < q) pick = cur;
            else {
                if (cur >= 0 && left[cur] > 0) queue[tl++] = cur;
                if (h < tl) pick = queue[h++];
                slice = 0;
            }
        } else if (pol == FCFS || pol == SJF || pol == PRIO) {
            if (cur >= 0 && left[cur] > 0) pick = cur;           /* non-preemptive */
            else for (int i = 0; i < N; i++) if (left[i] > 0 && at[i] <= t) {
                if (pick < 0) pick = i;
                else if (pol == FCFS && at[i] < at[pick]) pick = i;
                else if (pol == SJF && bt[i] < bt[pick]) pick = i;
                else if (pol == PRIO && pr[i] < pr[pick]) pick = i;
            }
        } else {                                                 /* SRTF */
            for (int i = 0; i < N; i++) if (left[i] > 0 && at[i] <= t)
                if (pick < 0 || left[i] < left[pick]) pick = i;
        }
        if (pick < 0) { t++; continue; }
        if (pick != last) {
            if (last >= 0) printf(" %s(%d-%d)", nm[last], seg_start, t);
            seg_start = t; last = pick; sw++;
        }
        if (first[pick] < 0) first[pick] = t;
        cur = pick; left[pick]--; t++; if (pol == RR) slice++;
        if (left[pick] == 0) { fin[pick] = t; done++; cur = -1; slice = 0; }
        if (pol == RR) for (int i = 0; i < N; i++) if (!inq[i] && at[i] <= t) { queue[tl++] = i; inq[i] = 1; }
    }
    printf(" %s(%d-%d)\n", nm[last], seg_start, t);
    double sw_t = 0, st = 0, sr = 0;
    printf("Process  TAT  WT  RT\n");
    for (int i = 0; i < N; i++) {
        int tat = fin[i] - at[i], wt = tat - bt[i], rt = first[i] - at[i];
        printf("%-7s %4d %3d %3d\n", nm[i], tat, wt, rt);
        st += tat; sw_t += wt; sr += rt;
    }
    printf("Average TAT = %.2f, WT = %.2f, RT = %.2f   (dispatches %d)\n\n", st/N, sw_t/N, sr/N, sw);
}
int main(void)
{
    puts("Jobs: P1(at0,bt8,pr3) P2(at1,bt4,pr1) P3(at2,bt9,pr4) P4(at3,bt5,pr2)\n");
    run(FCFS, 0, "== FCFS ==");
    run(SJF, 0, "== SJF (non-preemptive) ==");
    run(SRTF, 0, "== SRTF (preemptive SJF) ==");
    run(PRIO, 0, "== Priority (non-preemptive, 1 = highest) ==");
    run(RR, 3, "== Round Robin, q = 3 ==");
    return 0;
}
