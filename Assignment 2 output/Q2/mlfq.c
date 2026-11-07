/* Q2 (alternative): video service. 1 time unit = 1 second.
   2 films (1200 s, 900 s) + 8 thumbnails (2 s). Compare FCFS, SRTF, RR(q=20), MLFQ, MLFQ+ageing. */
#include <stdio.h>
#include <string.h>
#define J 10
int at[J]={0,5,10,20,30,40,50,60,70,80}, bt[J]={1200,900,2,2,2,2,2,2,2,2};
enum {FCFS,SRTF,RRQ,MLFQ,MLFQ_AGE};
#define LV 3
int qlev[LV]={2,10,50};
void sim(int pol,const char*title){
  int left[J],fin[J],lvl[J],first[J],t=0,done=0,cur=-1,slice=0,sw=0,last=-1;
  int rq[LV][100000],h[LV]={0},tl[LV]={0},inq[J]={0};
  for(int i=0;i<J;i++){left[i]=bt[i];lvl[i]=0;first[i]=-1;}
  while(done<J){
    for(int i=0;i<J;i++)if(!inq[i]&&at[i]<=t){inq[i]=1;rq[0][tl[0]++]=i;
        if(pol==MLFQ||pol==MLFQ_AGE){} }
    if(pol==MLFQ_AGE && t>0 && t%600==0){            /* ageing: periodic boost to top queue */
      for(int l=1;l<LV;l++)while(h[l]<tl[l]){int j=rq[l][h[l]++];lvl[j]=0;rq[0][tl[0]++]=j;}
      if(cur>=0&&lvl[cur]>0){rq[0][tl[0]++]=cur;lvl[cur]=0;cur=-1;slice=0;}
    }
    int pick=-1;
    if(pol==FCFS){ if(cur>=0)pick=cur; else{ for(int i=0;i<J;i++)if(left[i]>0&&at[i]<=t&&(pick<0||at[i]<at[pick]))pick=i; } }
    else if(pol==SRTF){ for(int i=0;i<J;i++)if(left[i]>0&&at[i]<=t&&(pick<0||left[i]<left[pick]))pick=i; }
    else if(pol==RRQ){
      if(cur>=0&&left[cur]>0&&slice<20)pick=cur;
      else{ if(cur>=0&&left[cur]>0)rq[0][tl[0]++]=cur; if(h[0]<tl[0])pick=rq[0][h[0]++]; slice=0; }
    } else {
      int best=-1; for(int l=0;l<LV;l++)if(h[l]<tl[l]){best=l;break;}
      if(cur>=0&&(best<0||best>=lvl[cur])&&left[cur]>0&&slice<qlev[lvl[cur]])pick=cur;       /* keep running */
      else{
        if(cur>=0&&left[cur]>0){ if(slice>=qlev[lvl[cur]]&&lvl[cur]<LV-1)lvl[cur]++; rq[lvl[cur]][tl[lvl[cur]]++]=cur; }
        for(int l=0;l<LV;l++)if(h[l]<tl[l]){pick=rq[l][h[l]++];break;}
        slice=0;
      }
    }
    if(pick<0){t++;continue;}
    if(pick!=last){sw++;last=pick;}
    if(first[pick]<0)first[pick]=t;
    cur=pick;left[pick]--;t++;slice++;
    if(left[pick]==0){fin[pick]=t;done++;cur=-1;slice=0;last=-1;}
  }
  double ws=0,wl=0,wa=0;int ns=0,nl=0;
  for(int i=0;i<J;i++){double w=fin[i]-at[i]-bt[i]; wa+=w; if(bt[i]<=2){ws+=w;ns++;}else{wl+=w;nl++;}}
  printf("%-22s short WT %8.1f s | long WT %8.1f s | overall %8.1f s | film1 finish %4d s | dispatches %d\n",
         title,ws/ns,wl/nl,wa/J,fin[0],sw);
}
int main(void){
  puts("Workload: films 1200 s @0, 900 s @5; eight 2 s thumbnails @10,20,...,80");
  sim(FCFS,"FCFS"); sim(SRTF,"SRTF (SJF preemptive)"); sim(RRQ,"Round Robin q=20");
  sim(MLFQ,"MLFQ (q=2,10,50)"); sim(MLFQ_AGE,"MLFQ + ageing boost");
  return 0;
}
