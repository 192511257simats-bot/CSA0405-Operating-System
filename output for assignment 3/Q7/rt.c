#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/resource.h>
static long ns(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec*1000000000L+t.tv_nsec;}
int main(int argc,char**argv){ int lock=argc>1&&!strcmp(argv[1],"lock"); size_t n=64u<<20,pages=n/4096;
 char*p=mmap(NULL,n,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
 if(lock){ if(mlockall(MCL_CURRENT|MCL_FUTURE)!=0){perror("mlockall");return 1;} memset(p,0,n); /* pre-fault while locked */ }
 struct rusage a,b; getrusage(RUSAGE_SELF,&a); long worst=0,sum=0;
 for(size_t i=0;i<pages;i++){ long t0=ns(); p[i*4096]++; long d=ns()-t0; sum+=d; if(d>worst)worst=d; }
 getrusage(RUSAGE_SELF,&b);
 printf("%-8s: loop of %zu page accesses: worst=%ld ns, mean=%ld ns, page faults in loop=%ld\n",lock?"mlocked":"unlocked",pages,worst,sum/(long)pages,b.ru_minflt-a.ru_minflt);
 return 0;}
