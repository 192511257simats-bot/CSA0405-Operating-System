#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
/* Demonstrates a zombie (child exited, parent has not wait()ed) and an orphan. */
int main(void) {
    pid_t z = fork();
    if (z == 0) _exit(0);                       /* child dies at once */
    sleep(1);                                   /* parent does NOT wait yet */
    char cmd[64]; snprintf(cmd, sizeof cmd, "ps -o pid,ppid,stat,comm -p %d", (int)z);
    printf("--- zombie child %d (state Z = defunct) ---\n", (int)z); fflush(stdout);
    system(cmd);
    waitpid(z, NULL, 0);                        /* reap: zombie disappears */
    printf("--- after waitpid() the entry is gone ---\n"); fflush(stdout);
    system(cmd);
    pid_t o = fork();
    if (o == 0) {                               /* orphan: parent exits first */
        printf("orphan: parent before = %d\n", (int)getppid()); fflush(stdout);
        sleep(2);
        printf("orphan: parent after  = %d (adopted by init/subreaper)\n", (int)getppid()); fflush(stdout);
        _exit(0);
    }
    usleep(300000);
    return 0;                                   /* parent exits, child orphaned */
}
