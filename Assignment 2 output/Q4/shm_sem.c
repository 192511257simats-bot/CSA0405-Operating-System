/* Q4 alternative: weather readings through POSIX shared memory + semaphores (ring buffer of ints) */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#define SLOTS 8
struct ring { sem_t empty, full; int buf[SLOTS]; };
int main(void)
{
    struct ring *r = mmap(NULL, sizeof *r, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    sem_init(&r->empty, 1, SLOTS); sem_init(&r->full, 1, 0);       /* pshared = 1 */
    int readings[] = {31, 33, 36, 34, 41, 38, -1};                 /* -1 = end marker */
    if (fork() == 0) {
        int count = 0, sum = 0, max = 0, out = 0;
        for (;;) {
            sem_wait(&r->full);
            int v = r->buf[out]; out = (out + 1) % SLOTS;
            sem_post(&r->empty);
            if (v < 0) break;
            count++; sum += v; if (v > max) max = v;
        }
        printf("analyser: %d readings, average %.1f, max %d\n", count, (double)sum / count, max); fflush(stdout);
        _exit(0);
    }
    int in = 0;
    for (int i = 0; i < 7; i++) {
        sem_wait(&r->empty); r->buf[in] = readings[i]; in = (in + 1) % SLOTS; sem_post(&r->full);
    }
    wait(NULL); printf("reader: done\n");
    return 0;
}
