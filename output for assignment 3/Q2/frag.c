#include <stdio.h>
#include <stdlib.h>
int main(void){ srand(42); int n=100; long sizes[100]; long total=0;
 for(int i=0;i<n;i++){ sizes[i]=1024+rand()%(1024*1024-1024); total+=sizes[i]; }
 long ps[]={1024,4096,2097152}; const char*nm[]={"1 KB","4 KB","2 MB"};
 printf("100 random processes (1 KB..1 MB), total size %ld bytes\n",total);
 for(int k=0;k<3;k++){ long waste=0,pages=0; for(int i=0;i<n;i++){ long pg=(sizes[i]+ps[k]-1)/ps[k]; pages+=pg; waste+=pg*ps[k]-sizes[i]; }
  printf("page %-5s: pages=%7ld internal frag=%10ld bytes (avg %8.1f B/proc, %6.2f%% of memory used)  page-table entries=%ld\n",nm[k],pages,waste,waste/(double)n,100.0*waste/(total+waste),pages); }
 return 0;}
