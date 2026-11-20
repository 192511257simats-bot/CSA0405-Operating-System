#include <stdio.h>
#include <stdlib.h>
int g_init=5; int g_bss[1000];
int main(void){ char*h=malloc(1<<20); int local; printf("code=%p data=%p bss=%p heap=%p stack=%p\n",(void*)main,(void*)&g_init,(void*)g_bss,(void*)h,(void*)&local);
 FILE*f=fopen("/proc/self/maps","r"); char l[300]; while(fgets(l,sizeof l,f)) fputs(l,stdout); fclose(f); return 0;}
