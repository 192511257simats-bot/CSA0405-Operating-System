#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static long field(const char*k){ FILE*f=fopen("/proc/self/status","r"); char l[256]; long v=-1; size_t kl=strlen(k);
 while(fgets(l,sizeof l,f)) if(!strncmp(l,k,kl)){ v=atol(l+kl+1); break;} fclose(f); return v;}
static long pss(void){ FILE*f=fopen("/proc/self/smaps_rollup","r"); char l[256]; long v=-1; while(fgets(l,sizeof l,f)) if(!strncmp(l,"Pss:",4)){v=atol(l+4);break;} fclose(f); return v;}
int main(void){ size_t n=200u<<20; char*p=malloc(n); memset(p,1,n);
 printf("parent before fork: RSS=%ld kB, PSS=%ld kB\n",field("VmRSS"),pss()); fflush(stdout);
 struct timespec a,b; clock_gettime(CLOCK_MONOTONIC,&a); pid_t c=fork();
 if(c==0){ clock_gettime(CLOCK_MONOTONIC,&b); 
   fflush(stdout); printf("fork() of a 200 MB process took %.3f ms\n",((b.tv_sec-a.tv_sec)*1e9+(b.tv_nsec-a.tv_nsec))/1e6);
   printf("child after fork, before write: RSS=%ld kB, PSS=%ld kB (pages shared)\n",field("VmRSS"),pss());
   for(size_t i=0;i<n/2;i+=4096) ((volatile char*)p)[i]=2; fflush(stdout); /* write to half -> copy-on-write for 100 MB */
   printf("child after writing 100 MB    : RSS=%ld kB, PSS=%ld kB (100 MB copied)\n",field("VmRSS"),pss()); fflush(stdout); _exit(0);} 
 waitpid(c,NULL,0); return 0;}
