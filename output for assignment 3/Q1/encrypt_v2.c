#include <string.h>
int encrypt_buf(char *buf, int n, int key){ for(int i=0;i<n;i++) buf[i]=(char)((buf[i]^key)+1); return n; }
const char *enc_version(void){ return "ENC v2 (SECURITY FIX)"; }
