#include <stdio.h>
typedef struct { unsigned base, limit; const char *name; } Seg;
Seg st[] = {{1400,1000,"code"},{6300,400,"data"},{4300,400,"heap"},{3200,1100,"stack"},{4700,1000,"lib"}};
int main(void){
  struct { int s; unsigned o; } t[] = {{0,0},{0,999},{0,1000},{1,399},{1,400},{2,53},{3,1099},{4,500}};
  for(int i=0;i<8;i++){ int s=t[i].s; unsigned o=t[i].o;
    printf("(seg %d [%-5s], off %4u): ", s, st[s].name, o);
    if(o>=st[s].limit) printf("TRAP: offset >= limit %u (segmentation fault)\n", st[s].limit);
    else printf("physical = %u + %u = %u\n", st[s].base, o, st[s].base+o); }
  return 0; }
