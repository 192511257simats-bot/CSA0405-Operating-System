#include <stdio.h>
#include <string.h>
int encrypt_buf(char *buf, int n, int key){ for(int i=0;i<n;i++) buf[i]^=key; return n; }
const char *enc_version(void){ return "ENC v1 (XOR)"; }
