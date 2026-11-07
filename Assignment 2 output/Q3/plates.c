/* Q3: 64 camera frames processed sequentially, one-thread-per-camera, and thread pools of 4/8/16.
   Each "frame" = CPU-bound pseudo number-plate recognition (hash loop). Also shared-counter race demo. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#define CAMERAS 64
#define WORK 12000000UL
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static volatile unsigned long sink;
static unsigned long process_frame(int cam){
    unsigned long x = 1469598103934665603UL ^ (unsigned long)cam;
    for (unsigned long i = 0; i < WORK; i++) { x ^= i; x *= 1099511628211UL; }
    return x;
}
static unsigned long results[CAMERAS];
static atomic_int next_cam;
static void *pool_worker(void *a){ (void)a;
    int c; while ((c = atomic_fetch_add(&next_cam, 1)) < CAMERAS) results[c] = process_frame(c);
    return NULL; }
static void *one_cam(void *a){ int c=(int)(long)a; results[c]=process_frame(c); return NULL; }
static unsigned long checksum(void){unsigned long s=0;for(int i=0;i<CAMERAS;i++)s^=results[i];return s;}

/* race demo */
#define INC 2000000
static volatile long unsafe_counter = 0; static long safe_counter = 0; /* volatile stops the compiler collapsing the loop */
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static void *inc_unsafe(void*a){(void)a;for(int i=0;i<INC;i++)unsafe_counter++;return NULL;}
static void *inc_safe(void*a){(void)a;for(int i=0;i<INC;i++){pthread_mutex_lock(&m);safe_counter++;pthread_mutex_unlock(&m);}return NULL;}

int main(void){
    printf("CPU cores available: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    double t0=now(); for(int i=0;i<CAMERAS;i++)results[i]=process_frame(i);
    double seq=now()-t0; unsigned long ref=checksum();
    printf("%-26s %7.3f s   speed-up 1.00  checksum %lx\n","sequential (1 thread)",seq,ref);
    t0=now(); pthread_t th[CAMERAS];
    for(long i=0;i<CAMERAS;i++)pthread_create(&th[i],NULL,one_cam,(void*)i);
    for(int i=0;i<CAMERAS;i++)pthread_join(th[i],NULL);
    double d=now()-t0; printf("%-26s %7.3f s   speed-up %.2f  checksum %s\n","64 threads (1 per camera)",d,seq/d,checksum()==ref?"OK":"BAD");
    int sizes[3]={4,8,16};
    for(int s=0;s<3;s++){
        int n=sizes[s]; atomic_store(&next_cam,0); t0=now();
        for(int i=0;i<n;i++)pthread_create(&th[i],NULL,pool_worker,NULL);
        for(int i=0;i<n;i++)pthread_join(th[i],NULL);
        d=now()-t0; char lab[40]; snprintf(lab,40,"pool of %d threads",n);
        printf("%-26s %7.3f s   speed-up %.2f  checksum %s\n",lab,d,seq/d,checksum()==ref?"OK":"BAD");
    }
    pthread_t a,b;
    pthread_create(&a,NULL,inc_unsafe,NULL);pthread_create(&b,NULL,inc_unsafe,NULL);pthread_join(a,NULL);pthread_join(b,NULL);
    pthread_create(&a,NULL,inc_safe,NULL);pthread_create(&b,NULL,inc_safe,NULL);pthread_join(a,NULL);pthread_join(b,NULL);
    printf("\nShared counter, 2 threads x %d increments (expected %d):\n  no protection : %ld\n  with mutex    : %ld\n",INC,2*INC,unsafe_counter,safe_counter);
    return 0;
}
