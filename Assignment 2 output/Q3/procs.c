/* Q3 alternative: the same 64 frames processed by 4 child PROCESSES; results combined through a pipe (IPC) */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#define CAMERAS 64
#define WORK 12000000UL
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static unsigned long process_frame(int cam){unsigned long x=1469598103934665603UL^(unsigned long)cam;for(unsigned long i=0;i<WORK;i++){x^=i;x*=1099511628211UL;}return x;}
int main(void){
    int np[3]={4,8,16};
    for(int k=0;k<3;k++){
        int n=np[k], fd[2]; if(pipe(fd)) return 1; double t0=now();
        for(int p=0;p<n;p++) if(fork()==0){ close(fd[0]);
            for(int c=p;c<CAMERAS;c+=n){unsigned long r=process_frame(c); if(write(fd[1],&r,sizeof r)<0)_exit(1);}
            _exit(0);}
        close(fd[1]); unsigned long r,s=0;int cnt=0;
        while(read(fd[0],&r,sizeof r)==sizeof r){s^=r;cnt++;}
        while(wait(NULL)>0);
        printf("%2d processes: %6.3f s  results received through pipe: %d  checksum %lx\n",n,now()-t0,cnt,s);
    }
    return 0;
}
