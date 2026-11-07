/* Q2: effect of quantum on Round Robin: wait, response, context switches (quanta 1,3,5,20) */
#include <stdio.h>
#define N 4
int main(void)
{
    int at[N] = {0,1,2,3}, bt[N] = {8,4,9,5}, qs[4] = {1,3,5,20};
    printf("quantum  avgTAT  avgWT  avgRT  dispatches\n");
    for (int k = 0; k < 4; k++) {
        int q = qs[k], left[N], fin[N], first[N], inq[N] = {0}, queue[4096], h=0, tl=0, t=0, done=0, sw=0;
        for (int i=0;i<N;i++){left[i]=bt[i];first[i]=-1;}
        queue[tl++]=0; inq[0]=1;
        while (done < N) {
            int p = queue[h++];
            int run = left[p] < q ? left[p] : q;
            if (first[p] < 0) first[p] = t;
            t += run; left[p] -= run; sw++;
            for (int i=0;i<N;i++) if(!inq[i]&&at[i]<=t){queue[tl++]=i;inq[i]=1;}
            if (left[p]) queue[tl++]=p; else {fin[p]=t;done++;}
        }
        double a=0,w=0,r=0;
        for(int i=0;i<N;i++){a+=fin[i]-at[i];w+=fin[i]-at[i]-bt[i];r+=first[i]-at[i];}
        printf("%5d   %6.2f %6.2f %6.2f   %d\n", q, a/N, w/N, r/N, sw);
    }
    return 0;
}
