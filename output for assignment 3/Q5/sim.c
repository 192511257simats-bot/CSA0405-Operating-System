#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXF 16
#define NPAGES 200
static int find(const int f[],int n,int p){for(int i=0;i<n;i++)if(f[i]==p)return i;return -1;}
int fifo(const int*r,int len,int fr){int f[MAXF],u=0,nx=0,ft=0;for(int i=0;i<len;i++){if(find(f,u,r[i])>=0)continue;ft++;if(u<fr)f[u++]=r[i];else{f[nx]=r[i];nx=(nx+1)%fr;}}return ft;}
int lru(const int*r,int len,int fr){int f[MAXF],l[MAXF],u=0,ft=0;for(int i=0;i<len;i++){int k=find(f,u,r[i]);if(k>=0){l[k]=i;continue;}ft++;if(u<fr){f[u]=r[i];l[u]=i;u++;}else{int v=0;for(int j=1;j<fr;j++)if(l[j]<l[v])v=j;f[v]=r[i];l[v]=i;}}return ft;}
int clk(const int*r,int len,int fr){int f[MAXF],ref[MAXF],u=0,h=0,ft=0;for(int i=0;i<len;i++){int k=find(f,u,r[i]);if(k>=0){ref[k]=1;continue;}ft++;if(u<fr){f[u]=r[i];ref[u]=1;u++;}else{while(ref[h]){ref[h]=0;h=(h+1)%fr;}f[h]=r[i];ref[h]=1;h=(h+1)%fr;}}return ft;}
/* Optimal using precomputed next-use (O(len*frames)) */
int opt(const int*r,int len,int fr,const int*nu){int f[MAXF],n[MAXF],u=0,ft=0;for(int i=0;i<len;i++){int k=find(f,u,r[i]);if(k>=0){n[k]=nu[i];continue;}ft++;if(u<fr){f[u]=r[i];n[u]=nu[i];u++;}else{int v=0;for(int j=1;j<fr;j++)if(n[j]>n[v])v=j;f[v]=r[i];n[v]=nu[i];}}return ft;}
void nextuse(const int*r,int len,int*nu){int last[NPAGES];for(int i=0;i<NPAGES;i++)last[i]=len;for(int i=len-1;i>=0;i--){nu[i]=last[r[i]];last[r[i]]=i;}}
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
void gen_locality(int*r,int len){int base=0;for(int i=0;i<len;i++){if(rand()%100<2)base=rand()%(NPAGES-12);r[i]=base+(rand()%100<90?rand()%6:rand()%12);}}
void gen_random(int*r,int len){for(int i=0;i<len;i++)r[i]=rand()%NPAGES;}
int main(void){srand(2026);int len=100000;int*r=malloc(len*sizeof(int)),*nu=malloc(len*sizeof(int));
 for(int w=0;w<2;w++){ if(w==0)gen_locality(r,len);else gen_random(r,len); nextuse(r,len,nu);
  printf("# %s\nframes,FIFO,LRU,CLOCK,OPT\n",w==0?"locality":"random");
  for(int fr=3;fr<=10;fr++)printf("%d,%d,%d,%d,%d\n",fr,fifo(r,len,fr),lru(r,len,fr),clk(r,len,fr),opt(r,len,fr,nu));}
 gen_locality(r,len);nextuse(r,len,nu);printf("# timing (100,000 refs, 8 frames, locality string)\n");
 double t;int x=0;
 t=now();x+=fifo(r,len,8);printf("FIFO  %.3f ms\n",(now()-t)*1e3);
 t=now();x+=lru(r,len,8);printf("LRU   %.3f ms\n",(now()-t)*1e3);
 t=now();x+=clk(r,len,8);printf("CLOCK %.3f ms\n",(now()-t)*1e3);
 t=now();x+=opt(r,len,8,nu);printf("OPT   %.3f ms\n",(now()-t)*1e3);
 return x<0;}
