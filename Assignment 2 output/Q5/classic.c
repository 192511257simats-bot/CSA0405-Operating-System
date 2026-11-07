/* Q5: dining philosophers (semaphores, room limit N-1) and readers-writers (readers preference) */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define PH 5
#define MEALS 20000
sem_t fork_s[PH], room;
int meals[PH], eating_now[PH], violations = 0; pthread_mutex_t chk = PTHREAD_MUTEX_INITIALIZER;
void *phil(void*a){int i=(int)(long)a;
  for(int m=0;m<MEALS;m++){
    sem_wait(&room);                       /* at most 4 philosophers compete -> no circular wait */
    sem_wait(&fork_s[i]); sem_wait(&fork_s[(i+1)%PH]);
    pthread_mutex_lock(&chk); eating_now[i]=1;
    if(eating_now[(i+1)%PH]||eating_now[(i+PH-1)%PH])violations++;   /* neighbour eating with my fork? */
    pthread_mutex_unlock(&chk);
    meals[i]++;
    pthread_mutex_lock(&chk); eating_now[i]=0; pthread_mutex_unlock(&chk);
    sem_post(&fork_s[i]); sem_post(&fork_s[(i+1)%PH]); sem_post(&room);
  } return 0;}
sem_t rw_mutex, mtx; int readcount=0, data=0, bad_reads=0, active_writers=0;
void *reader(void*a){(void)a;for(int i=0;i<20000;i++){
  sem_wait(&mtx); if(++readcount==1)sem_wait(&rw_mutex); sem_post(&mtx);
  if(__atomic_load_n(&active_writers,__ATOMIC_SEQ_CST))bad_reads++;
  sem_wait(&mtx); if(--readcount==0)sem_post(&rw_mutex); sem_post(&mtx);} return 0;}
void *writer(void*a){(void)a;for(int i=0;i<5000;i++){
  sem_wait(&rw_mutex); __atomic_add_fetch(&active_writers,1,__ATOMIC_SEQ_CST); data++; __atomic_sub_fetch(&active_writers,1,__ATOMIC_SEQ_CST); sem_post(&rw_mutex);} return 0;}
int main(void){
  pthread_t t[PH]; sem_init(&room,0,PH-1); for(int i=0;i<PH;i++)sem_init(&fork_s[i],0,1);
  for(long i=0;i<PH;i++)pthread_create(&t[i],0,phil,(void*)i); for(int i=0;i<PH;i++)pthread_join(t[i],0);
  printf("Dining philosophers: meals =");for(int i=0;i<PH;i++)printf(" %d",meals[i]);printf("  neighbour-conflicts = %d  (no deadlock, finished)\n",violations);
  sem_init(&rw_mutex,0,1);sem_init(&mtx,0,1); pthread_t r[4],w[2];
  for(int i=0;i<4;i++)pthread_create(&r[i],0,reader,0); for(int i=0;i<2;i++)pthread_create(&w[i],0,writer,0);
  for(int i=0;i<4;i++)pthread_join(r[i],0); for(int i=0;i<2;i++)pthread_join(w[i],0);
  printf("Readers-writers: writes done = %d (expected 10000), reads overlapping a writer = %d\n",data,bad_reads); return 0;}
