#include <stdio.h>
/* Same job through the C library (fopen/fread/fwrite) for the alternative-concept comparison. */
int main(void) {
    char buf[32];
    FILE *f = fopen("/etc/hostname", "r");
    size_t n = fread(buf, 1, sizeof buf, f);
    fwrite(buf, 1, n, stdout);
    fclose(f);
    return 0;
}
