#include <stdio.h>
#include <stdlib.h>
#define MAXF 512
/* Page-replacement simulation: 2 GB device = 524288 frames of 4 KB. RT pinned (never evictable).
   Scaled model (1 frame = 1 MB => 2048 frames) for speed. Pages per task, LRU replacement for ML+Log only. */
#define TOTAL 2048
#define OS 300
#define RT 64
#define ML 1536
#define LOG 40
static int find(int*f,int n,int p){for(int i=0;i<n;i++)if(f[i]==p)return i;return -1;}
int main(void){ srand(5);
 int pinned=RT+OS; int avail=TOTAL-pinned; printf("Frames (1 MB each): total=%d, OS=%d, RT pinned=%d, left for ML+LOG=%d; ML+LOG demand=%d\n",TOTAL,OS,RT,avail,ML+LOG);
 static int fr[TOTAL],last[TOTAL]; int used=0; long faults=0,rtfaults=0,refs=0;
 for(long t=0;t<400000;t++){ int task=rand()%100; int page;
   if(task<20){ page=100000+rand()%RT; /* RT: always resident (pinned) */ refs++; continue; }
   else if(task<90) page=200000+rand()%ML; else page=300000+rand()%LOG;
   refs++; int k=find(fr,used,page); if(k>=0){last[k]=(int)t;continue;} faults++;
   if(used<avail){fr[used]=page;last[used]=(int)t;used++;} else {int v=0;for(int j=1;j<used;j++)if(last[j]<last[v])v=j;fr[v]=page;last[v]=(int)t;} }
 printf("references=%ld, faults(ML+LOG)=%ld, RT faults=%ld\n",refs,faults,rtfaults);
 return 0;}
