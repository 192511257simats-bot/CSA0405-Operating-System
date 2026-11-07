#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <unistd.h>
/* Reads the first 32 bytes of a file with raw system calls and echoes them. */
int main(void) {
    char buf[32];
    int fd = open("/etc/hostname", O_RDONLY);       /* openat() syscall */
    ssize_t n = read(fd, buf, sizeof buf);          /* read() syscall   */
    write(1, buf, (size_t)n);                       /* write() syscall  */
    close(fd);                                      /* close() syscall  */
    return 0;
}
