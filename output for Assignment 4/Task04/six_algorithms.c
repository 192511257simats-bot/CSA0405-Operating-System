#include <stdio.h>
#include <stdlib.h>
#define N 8
#define MAXCYL 199
static int req[N] = {98, 183, 37, 122, 14, 124, 65, 67};
static int cmp(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static int report(const char *name, const int *seq, int n, int head, int jump_cost)
{   /* seq holds every cylinder the head visits, in order; jump_cost = return trip not part of a service move */
    int total = 0, pos = head;
    printf("%-7s %d", name, head);
    for (int i = 0; i < n; i++) { printf(" %d", seq[i]); total += abs(seq[i] - pos); pos = seq[i]; }
    printf("\n        total head movement = %d cylinders", total);
    if (jump_cost) printf("  (of which %d is the return jump)", jump_cost);
    printf("\n");
    return total;
}

int main(void)
{
    int head = 53, s[N], seq[N + 4], n;
    for (int i = 0; i < N; i++) s[i] = req[i];
    qsort(s, N, sizeof s[0], cmp);                      /* sorted: 14 37 65 67 98 122 124 183 */

    report("FCFS", req, N, head, 0);

    int done[N] = {0}, pos = head;                      /* SSTF */
    for (int k = 0; k < N; k++) {
        int best = -1;
        for (int i = 0; i < N; i++) if (!done[i] && (best < 0 || abs(req[i] - pos) < abs(req[best] - pos))) best = i;
        done[best] = 1; seq[k] = req[best]; pos = req[best];
    }
    report("SSTF", seq, N, head, 0);

    n = 0;                                              /* SCAN: down to 0, then up */
    for (int i = N - 1; i >= 0; i--) if (s[i] < head) seq[n++] = s[i];
    seq[n++] = 0;
    for (int i = 0; i < N; i++) if (s[i] > head) seq[n++] = s[i];
    report("SCAN", seq, n, head, 0);

    n = 0;                                              /* C-SCAN: up to 199, jump to 0, up again */
    for (int i = 0; i < N; i++) if (s[i] > head) seq[n++] = s[i];
    seq[n++] = MAXCYL; seq[n++] = 0;                    /* the 199 -> 0 move is the jump */
    for (int i = 0; i < N; i++) if (s[i] < head) seq[n++] = s[i];
    report("C-SCAN", seq, n, head, MAXCYL);

    n = 0;                                              /* LOOK: down to lowest request, then up to highest */
    for (int i = N - 1; i >= 0; i--) if (s[i] < head) seq[n++] = s[i];
    for (int i = 0; i < N; i++) if (s[i] > head) seq[n++] = s[i];
    report("LOOK", seq, n, head, 0);

    n = 0;                                              /* C-LOOK: up to highest request, jump to lowest, up again */
    for (int i = 0; i < N; i++) if (s[i] > head) seq[n++] = s[i];
    int jump = abs(s[N - 1] - s[0]);                    /* 183 -> 14 */
    for (int i = 0; i < N; i++) if (s[i] < head) seq[n++] = s[i];
    report("C-LOOK", seq, n, head, jump);
    return 0;
}
