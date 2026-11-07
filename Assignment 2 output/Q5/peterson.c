/* Q5: Peterson's algorithm for 2 threads. Plain variables vs sequentially-consistent atomics. */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#define N 1000000
volatile int flag_v[2], turn_v; volatile long cnt_v = 0;           /* plain (volatile) version */
atomic_int flag_a[2], turn_a; long cnt_a = 0;                       /* atomic seq_cst version  */
static void *plain(void*a){int me=(int)(long)a,o=1-me;
  for(int i=0;i<N;i++){flag_v[me]=1;turn_v=o;while(flag_v[o]&&turn_v==o);cnt_v++;flag_v[me]=0;} return 0;}
static void *atom(void*a){int me=(int)(long)a,o=1-me;
  for(int i=0;i<N;i++){atomic_store(&flag_a[me],1);atomic_store(&turn_a,o);while(atomic_load(&flag_a[o])&&atomic_load(&turn_a)==o);cnt_a++;atomic_store(&flag_a[me],0);} return 0;}
int main(void){ pthread_t a,b;
  pthread_create(&a,0,plain,(void*)0);pthread_create(&b,0,plain,(void*)1);pthread_join(a,0);pthread_join(b,0);
  printf("Peterson, plain variables   : counter = %ld (expected %d)\n",cnt_v,2*N);
  pthread_create(&a,0,atom,(void*)0);pthread_create(&b,0,atom,(void*)1);pthread_join(a,0);pthread_join(b,0);
  printf("Peterson, atomic seq_cst    : counter = %ld (expected %d)\n",cnt_a,2*N); return 0;}
