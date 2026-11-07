/* Q1: cost of 2000 processes vs 2000 threads; context-switch cost (pipe ping-pong) */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#define N 2000
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static void *idle_thread(void *a){(void)a;return NULL;}
static void pin(void){cpu_set_t s;CPU_ZERO(&s);CPU_SET(0,&s);sched_setaffinity(0,sizeof s,&s);}
static long rss_kb(void){FILE*f=fopen("/proc/self/status","r");char l[256];long v=0;while(fgets(l,256,f))if(!strncmp(l,"VmRSS:",6))sscanf(l+6,"%ld",&v);fclose(f);return v;}
static int fd1[2],fd2[2];
static void *pong(void *a){(void)a;pin();char c;for(int i=0;i<100000;i++){read(fd1[0],&c,1);write(fd2[1],&c,1);}return NULL;}
int main(void)
{
    double t=now();
    for(int i=0;i<N;i++){pid_t p=fork();if(p==0)_exit(0);waitpid(p,NULL,0);}
    double tp=(now()-t)/N*1e6;
    t=now();
    for(int i=0;i<N;i++){pthread_t th;pthread_create(&th,NULL,idle_thread,NULL);pthread_join(th,NULL);}
    double tt=(now()-t)/N*1e6;
    printf("create+exit+wait of one process : %8.1f us (x%d = %.3f s)\n",tp,N,tp*N/1e6);
    printf("create+exit+join of one thread  : %8.1f us (x%d = %.3f s)\n",tt,N,tt*N/1e6);
    printf("process creation is %.1fx slower than thread creation\n",tp/tt);
    printf("resident memory of this tiny process: %ld kB\n",rss_kb());

    /* memory held by 200 live sleeping processes (sampled, scaled to 2000) */
    /* context switch: ping-pong through two pipes, both sides pinned to CPU 0 */
    char c='x';
    pipe(fd1);pipe(fd2);
    pid_t p=fork();
    if(p==0){pin();for(int i=0;i<100000;i++){read(fd1[0],&c,1);write(fd2[1],&c,1);}_exit(0);}
    pin();t=now();
    for(int i=0;i<100000;i++){write(fd1[1],&c,1);read(fd2[0],&c,1);}
    double cp=(now()-t)/200000*1e6; waitpid(p,NULL,0);
    pipe(fd1);pipe(fd2);
    pthread_t th;pthread_create(&th,NULL,pong,NULL);
    t=now();
    for(int i=0;i<100000;i++){write(fd1[1],&c,1);read(fd2[0],&c,1);}
    double ct=(now()-t)/200000*1e6; pthread_join(th,NULL);
    printf("process context switch (incl. pipe syscalls) : %.2f us\n",cp);
    printf("thread  context switch (incl. pipe syscalls) : %.2f us\n",ct);
    return 0;
}
