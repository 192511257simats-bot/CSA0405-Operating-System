/* Q4: time to move 1, 10, 100 MB parent->child via (a) pipe, (b) shared memory + semaphores, (c) POSIX message queue */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#define CHUNK 8192
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
struct ring { sem_t empty, full; int len[16]; char buf[16][65536]; };

static double pipe_xfer(size_t total){
    int fd[2]; if(pipe(fd))exit(1); char *b=malloc(65536); memset(b,7,65536); double t=now();
    if(fork()==0){close(fd[1]);size_t got=0;ssize_t n;while((n=read(fd[0],b,65536))>0)got+=n;_exit(got==total?0:1);}
    close(fd[0]);size_t sent=0;while(sent<total){size_t c=total-sent>65536?65536:total-sent;write(fd[1],b,c);sent+=c;}
    close(fd[1]);int st;wait(&st);double d=now()-t;free(b);return WEXITSTATUS(st)?-1:d;}
static double shm_xfer(size_t total){
    struct ring *r=mmap(NULL,sizeof *r,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    sem_init(&r->empty,1,16);sem_init(&r->full,1,0);double t=now();
    if(fork()==0){size_t got=0;int out=0;volatile char sink=0;for(;;){sem_wait(&r->full);int l=r->len[out];if(l==0)break;sink+=r->buf[out][0];got+=l;out=(out+1)%16;sem_post(&r->empty);}_exit(got==total?0:1);}
    size_t sent=0;int in=0;while(sent<total){size_t c=total-sent>65536?65536:total-sent;sem_wait(&r->empty);memset(r->buf[in],7,c);r->len[in]=c;in=(in+1)%16;sem_post(&r->full);sent+=c;}
    sem_wait(&r->empty);r->len[in]=0;sem_post(&r->full);
    int st;wait(&st);double d=now()-t;munmap(r,sizeof *r);return WEXITSTATUS(st)?-1:d;}
static double mq_xfer(size_t total){
    struct mq_attr a={0,10,CHUNK,0};mq_unlink("/wx");mqd_t q=mq_open("/wx",O_CREAT|O_RDWR,0600,&a);if(q==(mqd_t)-1)return -1;
    char *b=malloc(CHUNK);memset(b,7,CHUNK);double t=now();
    if(fork()==0){size_t got=0;char *rb=malloc(CHUNK);for(;;){ssize_t n=mq_receive(q,rb,CHUNK,NULL);if(n<=0)break;got+=n;if(got>=total)break;}_exit(got==total?0:1);}
    size_t sent=0;while(sent<total){size_t c=total-sent>CHUNK?CHUNK:total-sent;mq_send(q,b,c,0);sent+=c;}
    int st;wait(&st);double d=now()-t;mq_close(q);mq_unlink("/wx");free(b);return WEXITSTATUS(st)?-1:d;}
int main(void){
    size_t mb[3]={1,10,100};
    puts("(best of 5 runs)");printf("%8s %12s %14s %14s\n","size","pipe (s)","shm+sem (s)","msg queue (s)");
    for(int i=0;i<3;i++){size_t n=mb[i]<<20;
        double p=1e9,s=1e9,m=1e9;for(int k=0;k<5;k++){double x;x=pipe_xfer(n);if(x<p)p=x;x=shm_xfer(n);if(x<s)s=x;x=mq_xfer(n);if(x<m)m=x;}
        printf("%5zu MB %12.4f %14.4f %14.4f\n",mb[i],p,s,m);}
    return 0;
}
