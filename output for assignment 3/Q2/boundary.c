#include <stdio.h>
#define PS 1024
#define PAGES 8
int pt[PAGES]={5,2,-1,7,0,-1,3,-1};
void t(unsigned a){unsigned p=a/PS,d=a%PS; printf("%5u (page %u, off %4u): ",a,p,d);
 if(p>=PAGES)puts("invalid (beyond last page)"); else if(pt[p]<0)puts("page fault"); else printf("physical %u\n",pt[p]*PS+d);}
int main(void){ t(0); t(1023); t(1024); t(7167); t(8191); t(8192); return 0;}
