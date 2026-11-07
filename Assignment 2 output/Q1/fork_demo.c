/* Q1: create N child "student session" processes with fork(); observe with ps and /proc */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int n = argc > 1 ? atoi(argv[1]) : 5;
    int hold = argc > 2 ? atoi(argv[2]) : 6;   /* seconds children stay alive */
    pid_t kids[64];
    printf("parent (server) pid=%d creating %d sessions\n", getpid(), n);
    fflush(stdout);
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(1); }
        if (pid == 0) {                       /* child = one student session */
            printf("  session %d: pid=%d ppid=%d\n", i + 1, getpid(), getppid());
            fflush(stdout);
            sleep(hold);                      /* state: waiting (sleeping) */
            _exit(i);                         /* state: terminated */
        }
        kids[i] = pid;
    }
    for (int i = 0; i < n; i++) {
        int st;
        waitpid(kids[i], &st, 0);
        printf("parent: reaped pid=%d exit code=%d\n", kids[i], WEXITSTATUS(st));
    }
    return 0;
}
