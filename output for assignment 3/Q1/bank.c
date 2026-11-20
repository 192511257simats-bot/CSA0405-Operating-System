#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int encrypt_buf(char *buf, int n, int key);
const char *enc_version(void);
int main(int argc,char**argv){
  (void)argv;
  char msg[]="ACCOUNT:1234567890 BAL:50000";
  encrypt_buf(msg,(int)strlen(msg),0x5A);
  printf("Library: %s | first byte after encryption: 0x%02x\n",enc_version(),(unsigned char)msg[0]);
  if(argc>1){ FILE*f=fopen("/proc/self/maps","r"); char l[512];
    while(fgets(l,sizeof l,f)){
      if(strstr(l,"bank")||strstr(l,"libenc")||strstr(l,"libc.")||strstr(l,"[stack]")||strstr(l,"[heap]")) fputs(l,stdout);
    }
    fclose(f);}
  return 0;}
