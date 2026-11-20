#include <stdio.h>
#include <stdlib.h>
#define MEM 4096   /* KB of physical memory, first-fit */
int main(void){ srand(7); int base[20],size[20],ok[20]; static char used[MEM];
 for(int i=0;i<20;i++){ size[i]=20+rand()%180; ok[i]=0; base[i]=-1;
  for(int s=0;s+size[i]<=MEM&&!ok[i];s++){ int f=1; for(int k=s;k<s+size[i];k++) if(used[k]){f=0;s=k;break;}
   if(f){ base[i]=s; ok[i]=1; for(int k=s;k<s+size[i];k++)used[k]=1; } } }
 int loaded=0,tot=0; for(int i=0;i<20;i++) if(ok[i]){loaded++;tot+=size[i];}
 printf("Loaded %d segments, %d KB used of %d KB\n",loaded,tot,MEM);
 for(int i=1;i<20;i+=2) if(ok[i]){ for(int k=base[i];k<base[i]+size[i];k++)used[k]=0; ok[i]=0; }  /* remove odd segments */
 int free_total=0,holes=0,largest=0,run=0;
 for(int k=0;k<MEM;k++){ if(!used[k]){free_total++;run++; if(run>largest)largest=run;} else { if(run>0)holes++; run=0; } }
 if(run>0){ holes++; }
 /* only count holes inside the used region (before last used KB) */
 int last=0; for(int k=0;k<MEM;k++) if(used[k]) last=k; int inner=0,irun=0,ilarge=0,iholes=0;
 for(int k=0;k<=last;k++){ if(!used[k]){inner++;irun++; if(irun>ilarge)ilarge=irun;} else { if(irun>0)iholes++; irun=0; } }
 printf("After removing every 2nd segment: free holes inside used area=%d, free KB in holes=%d, largest hole=%d KB\n",iholes,inner,ilarge);
 printf("External fragmentation = 1 - largest_hole/total_free = %.1f%%\n",100.0*(1.0-(double)largest/free_total));
 printf("(total free incl. tail=%d KB, largest free block incl. tail=%d KB)\n",free_total,largest);
 return 0;}
