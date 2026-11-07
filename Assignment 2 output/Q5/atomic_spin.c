/* Q5 alternative: mutex vs spin lock vs C11 atomic_fetch_add for a shared counter, 1/4/16 threads.
   Reports wall time per update and the CPU time burned (CPU/wall ratio shows spinning waste). */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#define TOTAL 4000000L
static long counter; static pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER; static atomic_flag sp=ATOMIC_FLAG_INIT; static atomic_long ac;
static int mode, nth;
static double now(clockid_t c){struct timespec t;clock_gettime(c,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static void *w(void*a){(void)a;long n=TOTAL/nth;for(long i=0;i<n;i++){
  if(mode==0){pthread_mutex_lock(&m);counter++;pthread_mutex_unlock(&m);}
  else if(mode==1){while(atomic_flag_test_and_set_explicit(&sp,memory_order_acquire));counter++;atomic_flag_clear_explicit(&sp,memory_order_release);}
  else atomic_fetch_add(&ac,1);} return 0;}
int main(void){ const char*nm[3]={"pthread mutex","spin lock","atomic_fetch_add"}; int ths[3]={1,4,16};
  printf("%-17s %7s %12s %10s %12s\n","method","threads","ns/update","wall (s)","CPU/wall");
  for(mode=0;mode<3;mode++)for(int k=0;k<3;k++){ nth=ths[k];counter=0;atomic_store(&ac,0);pthread_t t[16];
    double w0=now(CLOCK_MONOTONIC),c0=now(CLOCK_PROCESS_CPUTIME_ID);
    for(int i=0;i<nth;i++)pthread_create(&t[i],0,w,0);for(int i=0;i<nth;i++)pthread_join(t[i],0);
    double wd=now(CLOCK_MONOTONIC)-w0,cd=now(CLOCK_PROCESS_CPUTIME_ID)-c0; long v=mode==2?atomic_load(&ac):counter;
    printf("%-17s %7d %12.1f %10.3f %12.2f %s\n",nm[mode],nth,wd/TOTAL*1e9,wd,cd/wd,v==(TOTAL/nth)*nth?"":"WRONG");}
  return 0;}
