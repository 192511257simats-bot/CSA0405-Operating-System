#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <spawn.h>
#include <time.h>
#include <sys/wait.h>
extern char **environ;
#define N 1000
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec/1e9; }
static void *tfn(void *a){ (void)a; return NULL; }
static long vmrss_kb(void){ FILE*f=fopen("/proc/self/status","r"); char l[128]; long v=0; while(fgets(l,sizeof l,f)) if(!strncmp(l,"VmRSS:",6)) sscanf(l+6,"%ld",&v); fclose(f); return v; }
static double t_fork(void){ double t=now(); for(int i=0;i<N;i++){ pid_t p=fork(); if(p==0)_exit(0); waitpid(p,NULL,0);} return now()-t; }
static double t_thread(void){ double t=now(); for(int i=0;i<N;i++){ pthread_t th; pthread_create(&th,NULL,tfn,NULL); pthread_join(th,NULL);} return now()-t; }
static double t_forkexec(void){ double t=now(); for(int i=0;i<N;i++){ pid_t p=fork(); if(p==0){execl("/bin/true","true",(char*)NULL);_exit(127);} waitpid(p,NULL,0);} return now()-t; }
static double t_spawn(void){ double t=now(); for(int i=0;i<N;i++){ pid_t p; char*av[]={"true",NULL}; posix_spawn(&p,"/bin/true",NULL,NULL,av,environ); waitpid(p,NULL,0);} return now()-t; }
int main(void){
    printf("%-44s %9s %12s\n","experiment (1,000 creations)","total ms","per item us");
    double r;
    r=t_fork();    printf("%-44s %9.1f %12.1f\n","fork()+_exit()+waitpid()  [small process]",r*1e3,r*1e6/N);
    r=t_thread();  printf("%-44s %9.1f %12.1f\n","pthread_create()+pthread_join()",r*1e3,r*1e6/N);
    r=t_forkexec();printf("%-44s %9.1f %12.1f\n","fork()+execl(/bin/true)+waitpid()",r*1e3,r*1e6/N);
    r=t_spawn();   printf("%-44s %9.1f %12.1f\n","posix_spawn(/bin/true)+waitpid()",r*1e3,r*1e6/N);
    size_t big=256u<<20; volatile char*m=malloc(big); for(size_t i=0;i<big;i+=4096) m[i]=1; /* touch every page */   /* 256 MiB resident -> shows page-table copy cost */
    printf("(parent now holds %ld MiB resident)\n", vmrss_kb()/1024);
    r=t_fork();    printf("%-44s %9.1f %12.1f\n","fork()+_exit()+waitpid()  [256 MiB parent]",r*1e3,r*1e6/N);
    r=t_thread();  printf("%-44s %9.1f %12.1f\n","pthread_create()+join()   [256 MiB parent]",r*1e3,r*1e6/N);
    /* copy-on-write: child writes to 10% of pages -> only those are copied */
    return 0;
}
