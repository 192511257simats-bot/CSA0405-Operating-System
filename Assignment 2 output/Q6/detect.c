/* Q6 alternative: deadlock DETECTION (no avoidance). Same state as the Banker example but processes are
   only *requesting*; run detection and find the deadlocked set. State below creates a real deadlock. */
#include <stdio.h>
#include <string.h>
#define P 5
#define R 3
int main(void){
    int avail[R]={0,0,0};
    int alloc[P][R]={{0,1,0},{2,0,0},{0,0,3},{0,0,1},{0,0,2}};
    int req[P][R]  ={{2,0,0},{0,1,0},{0,0,0},{0,0,0},{0,0,0}};  /* P0 waits for P1's A, P1 waits for P0's B */
    int work[R],fin[P];memcpy(work,avail,sizeof work);
    for(int p=0;p<P;p++){int z=1;for(int r=0;r<R;r++)if(alloc[p][r])z=0;fin[p]=z;}
    for(int ch=1;ch;){ch=0;
        for(int p=0;p<P;p++){ if(fin[p])continue;int ok=1;
            for(int r=0;r<R;r++)if(req[p][r]>work[r])ok=0;
            if(ok){for(int r=0;r<R;r++)work[r]+=alloc[p][r];fin[p]=1;ch=1;} } }
    int dead=0;printf("deadlock detection result:");
    for(int p=0;p<P;p++)if(!fin[p]){printf(" P%d",p);dead++;}
    printf(dead?"  <- DEADLOCKED, abort one of them (cheapest victim)\n":" none, system not deadlocked\n");
    return 0;}
