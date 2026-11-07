/* Q5: deployment decision - one global lock vs one lock per train vs one lock per coach.
   8 trains x 8 coaches x 64 seats. Each booking = lock, check+mark seat, ~1 us of work, unlock. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TR 8
#define CO 8
#define SE 64
#define OPS 200000
static char seat[TR][CO][SE]; static pthread_mutex_t gl=PTHREAD_MUTEX_INITIALIZER, tl[TR], cl[TR][CO];
static int mode; static long sold[16]; static long dbl[16];
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static void *agent(void*a){ long id=(long)a; unsigned s=12345+id*7919;
  for(int i=0;i<OPS;i++){ s=s*1103515245+12345; int t=(s>>8)%TR; s=s*1103515245+12345; int c=(s>>8)%CO; s=s*1103515245+12345; int q=(s>>8)%SE;
    pthread_mutex_t *m = mode==0?&gl : mode==1?&tl[t] : &cl[t][c];
    pthread_mutex_lock(m);
    if(!seat[t][c][q]){ seat[t][c][q]=1; sold[id]++; }
    for(volatile int w=0;w<150;w++);                    /* ~ payment/validation work inside the lock */
    pthread_mutex_unlock(m);} return 0;}
int main(void){
  for(int t=0;t<TR;t++){pthread_mutex_init(&tl[t],0);for(int c=0;c<CO;c++)pthread_mutex_init(&cl[t][c],0);}
  const char*nm[3]={"one global lock","one lock per train","one lock per coach"};
  int th[3]={1,2,4}; printf("%-20s","threads ->"); for(int k=0;k<3;k++)printf("%12d",th[k]); printf("   (bookings per second, millions)\n");
  for(mode=0;mode<3;mode++){ printf("%-20s",nm[mode]);
    for(int k=0;k<3;k++){ for(int t=0;t<TR;t++)for(int c=0;c<CO;c++)for(int q=0;q<SE;q++)seat[t][c][q]=0;
      for(int i=0;i<16;i++)sold[i]=0; pthread_t p[16]; double t0=now();
      for(long i=0;i<th[k];i++)pthread_create(&p[i],0,agent,(void*)i); for(int i=0;i<th[k];i++)pthread_join(p[i],0);
      double d=now()-t0; long tot=0,occ=0; for(int i=0;i<th[k];i++)tot+=sold[i];
      for(int t=0;t<TR;t++)for(int c=0;c<CO;c++)for(int q=0;q<SE;q++)occ+=seat[t][c][q];
      printf("%12.2f%s",th[k]*(double)OPS/d/1e6, tot==occ?"":"!"); }
    printf("\n"); }
  printf("(a '!' would mean a seat was sold twice; none shown = every seat sold at most once)\n"); return 0;}
