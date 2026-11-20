#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
int main(int argc,char**argv){ size_t mb=argc>1?atol(argv[1]):256; int rnd=argc>2?atoi(argv[2]):0;
 size_t n=mb<<20, pages=n/4096; char*p=malloc(n); srand(1);
 struct rusage a,b; getrusage(RUSAGE_SELF,&a); double t0=now();
 if(!rnd) for(size_t i=0;i<pages;i++) p[i*4096]=1;
 else { /* random order touch of every page exactly once (Fisher-Yates) */
   size_t*idx=malloc(pages*sizeof*idx); for(size_t i=0;i<pages;i++)idx[i]=i;
   for(size_t i=pages-1;i>0;i--){size_t j=((size_t)rand()*RAND_MAX+rand())%(i+1);size_t t=idx[i];idx[i]=idx[j];idx[j]=t;}
   t0=now(); for(size_t i=0;i<pages;i++) p[idx[i]*4096]=1; free(idx);} 
 double t1=now(); getrusage(RUSAGE_SELF,&b);
 printf("%5zu MB %-10s minor faults=%8ld major=%ld time=%.3fs (%.2f us/page)\n",mb,rnd?"random":"sequential",b.ru_minflt-a.ru_minflt,b.ru_majflt-a.ru_majflt,t1-t0,(t1-t0)*1e6/pages);
 return 0;}
