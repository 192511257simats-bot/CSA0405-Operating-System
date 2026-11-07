/* Q6 D/E: time of the Banker's safety algorithm for n = 10, 100, 1000 processes (m = 3 and m = 6 resources).
   Prediction: T(n,m) = O(m * n^2): x10 processes -> ~x100 time; x2 resources -> ~x2 time. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
/* worst case: process i can only finish after i-1 has released -> every pass finds exactly one (scan order reversed) */
static int safe(int n,int m,int *avail,int *need,int *alloc){
    int *work=malloc(m*sizeof(int)),*fin=calloc(n,sizeof(int)),cnt=0; memcpy(work,avail,m*sizeof(int));
    while(cnt<n){int found=0;
        for(int p=0;p<n;p++){ if(fin[p])continue; int ok=1;
            for(int r=0;r<m;r++)if(need[p*m+r]>work[r]){ok=0;break;}
            if(ok){for(int r=0;r<m;r++)work[r]+=alloc[p*m+r];fin[p]=1;cnt++;found=1;} }
        if(!found){free(work);free(fin);return 0;} }
    free(work);free(fin);return 1;}
int main(void){
    int ns[3]={10,100,1000}, ms[2]={3,6};
    printf("%6s %3s %14s %14s %12s\n","n","m","time/check","ops (m*n^2)","ns per op");
    for(int a=0;a<2;a++)for(int b=0;b<3;b++){int n=ns[b],m=ms[a];
        int *avail=calloc(m,sizeof(int)),*need=malloc(n*m*sizeof(int)),*alloc=malloc(n*m*sizeof(int));
        for(int r=0;r<m;r++)avail[r]=1;
        /* worst-case chain: only the LAST resource ever blocks a process (so every check scans all m resources);
           need(p) = n-p, avail = 1, every finished process releases 1 -> exactly one process finishes per pass */
        for(int p=0;p<n;p++)for(int r=0;r<m;r++){ need[p*m+r]=(r<m-1)?0:(n-p); alloc[p*m+r]=1; }
        for(int r=0;r<m;r++)avail[r]=1;
        int reps=n==1000?3:n==100?300:30000; double t=now(); int ok=0;
        for(int k=0;k<reps;k++)ok=safe(n,m,avail,need,alloc);
        double d=(now()-t)/reps; double ops=(double)m*n*n/2;
        printf("%6d %3d %11.2f us %14.0f %12.2f %s\n",n,m,d*1e6,(double)m*n*n,d*1e9/((double)m*n*n),ok?"safe":"UNSAFE");
        free(avail);free(need);free(alloc);(void)ops;}
    return 0;}
