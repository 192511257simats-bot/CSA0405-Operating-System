#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
int main(void){
  void *h=dlopen("./libenc.so",RTLD_NOW);
  if(!h){ fprintf(stderr,"dlopen failed: %s\n",dlerror()); return 1; }
  int (*enc)(char*,int,int)=(int(*)(char*,int,int))dlsym(h,"encrypt_buf");
  const char *(*ver)(void)=(const char*(*)(void))dlsym(h,"enc_version");
  if(!enc||!ver){ fprintf(stderr,"dlsym failed: %s\n",dlerror()); return 2; }
  char msg[]="ACCOUNT:1234567890"; enc(msg,(int)strlen(msg),0x5A);
  printf("dlopen OK: %s, first byte 0x%02x\n",ver(),(unsigned char)msg[0]);
  dlclose(h); return 0; }
