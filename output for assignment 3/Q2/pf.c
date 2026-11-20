#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
int main(void){ size_t n=64u<<20; char*p=malloc(n); for(size_t i=0;i<n;i+=4096)p[i]=1;
 struct rusage r; getrusage(RUSAGE_SELF,&r); printf("touched 64 MB: minor faults=%ld major faults=%ld maxrss=%ld kB\n",r.ru_minflt,r.ru_majflt,r.ru_maxrss); free(p); return 0;}
