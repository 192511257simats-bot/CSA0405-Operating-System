#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
/* usage: bench_copy MODE src dst [bufsize]   MODE = sys | stdio | mmap */
int main(int argc, char **argv) {
    if (argc < 4) return 2;
    const char *mode = argv[1], *src = argv[2], *dst = argv[3];
    size_t b = argc > 4 ? (size_t)atol(argv[4]) : 4096;
    if (!strcmp(mode, "sys")) {
        char *buf = malloc(b);
        int in = open(src, O_RDONLY), out = open(dst, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        ssize_t n;
        while ((n = read(in, buf, b)) > 0) { if (write(out, buf, (size_t)n) != n) return 1; }
        close(in); close(out); free(buf);
    } else if (!strcmp(mode, "stdio")) {
        FILE *in = fopen(src, "rb"), *out = fopen(dst, "wb");
        char *buf = malloc(b); size_t n;
        while ((n = fread(buf, 1, b, in)) > 0) fwrite(buf, 1, n, out);
        fclose(in); fclose(out); free(buf);
    } else {                                   /* mmap: map input, one write() of whole file */
        int in = open(src, O_RDONLY), out = open(dst, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        struct stat st; fstat(in, &st);
        char *p = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, in, 0);
        size_t off = 0; while (off < (size_t)st.st_size) { ssize_t w = write(out, p+off, (size_t)st.st_size-off); if (w<=0) return 1; off += (size_t)w; }
        munmap(p, (size_t)st.st_size); close(in); close(out);
    }
    return 0;
}
