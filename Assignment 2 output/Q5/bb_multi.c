/* Q5: bounded buffer with NP producers and NC consumers; proves each item is consumed exactly once.
   Compile with -DNOMUTEX to remove the mutex (semaphores only) and watch it break. */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define SIZE 5
#define PER_PRODUCER 20000
static int NP, NC, buffer[SIZE], in = 0, out = 0;
static sem_t empty, full;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int *seen;                      /* seen[id] = how many times item id was consumed */
#ifdef NOMUTEX
#define LOCK()
#define UNLOCK()
#else
#define LOCK() pthread_mutex_lock(&lock)
#define UNLOCK() pthread_mutex_unlock(&lock)
#endif
static void *producer(void *a){ int id=(int)(long)a;
    for(int i=0;i<PER_PRODUCER;i++){ sem_wait(&empty); LOCK(); buffer[in]=id*PER_PRODUCER+i; in=(in+1)%SIZE; UNLOCK(); sem_post(&full);} return NULL;}
static void *consumer(void *a){ (void)a; int n=NP*PER_PRODUCER/NC;
    for(int k=0;k<n;k++){ sem_wait(&full); LOCK(); int v=buffer[out]; out=(out+1)%SIZE; UNLOCK(); sem_post(&empty);
        __atomic_fetch_add(&seen[v],1,__ATOMIC_RELAXED);} return NULL;}
int main(int argc,char**argv){
    NP=NC=argc>1?atoi(argv[1]):1; int total=NP*PER_PRODUCER; seen=calloc(total,sizeof(int));
    sem_init(&empty,0,SIZE); sem_init(&full,0,0);
    pthread_t p[16],c[16];
    for(long i=0;i<NP;i++)pthread_create(&p[i],NULL,producer,(void*)i);
    for(int i=0;i<NC;i++)pthread_create(&c[i],NULL,consumer,NULL);
    for(int i=0;i<NP;i++)pthread_join(p[i],NULL);
    for(int i=0;i<NC;i++)pthread_join(c[i],NULL);
    int once=0,lost=0,dup=0; for(int i=0;i<total;i++){ if(seen[i]==1)once++; else if(seen[i]==0)lost++; else dup++; }
    printf("%d producer(s), %d consumer(s): produced %d | consumed exactly once %d | lost %d | duplicated %d  -> %s\n",
           NP,NC,total,once,lost,dup,(once==total)?"CORRECT":"WRONG");
    return 0; }
