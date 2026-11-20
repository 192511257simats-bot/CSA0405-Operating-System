#include <stdio.h>
int main(void){ int *p=(int*)0; printf("about to dereference NULL\n"); fflush(stdout); *p=42; return 0; }
