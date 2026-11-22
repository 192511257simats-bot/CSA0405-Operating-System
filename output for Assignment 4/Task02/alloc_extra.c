#include <stdio.h>
#include <string.h>

#define BLOCKS 32

char disk[BLOCKS]; /* '.' = free, otherwise the file letter */

/* Contiguous allocation, first fit: returns the start block or -1. */
int alloc_contiguous(char file, int size)
{
    /* TODO 1 (completed): scan for the first run of `size` consecutive free blocks */
    if (size <= 0) return -1;                 /* nothing sensible to allocate */
    int run = 0;                              /* length of the current free run */
    for (int b = 0; b < BLOCKS; b++) {
        run = (disk[b] == '.') ? run + 1 : 0; /* extend or break the run        */
        if (run == size) {                    /* first run that is long enough  */
            int start = b - size + 1;
            for (int i = start; i <= b; i++) disk[i] = file; /* mark blocks     */
            return start;
        }
    }
    return -1;                                /* no run long enough: external fragmentation */
}

/* Indexed allocation: one index block plus any free data blocks. */
int alloc_indexed(char file, int size, int index[])
{
    int got = 0, idx = -1;
    for (int b = 0; b < BLOCKS && got <= size; b++) {
        if (disk[b] != '.') continue;
        if (idx < 0) idx = b; /* first free block holds the index */
        else index[got++] = b;
        if (got == size) break;
    }
    if (idx < 0 || got < size) return -1;
    disk[idx] = (char)(file + 32); /* lowercase letter = index block */
    for (int i = 0; i < size; i++) disk[index[i]] = file;
    return idx;
}

void release(char file)
{
    for (int b = 0; b < BLOCKS; b++) if (disk[b] == file) disk[b] = '.';
}

static int largest_free_run(void)
{
    int best = 0, run = 0;
    for (int b = 0; b < BLOCKS; b++) { run = (disk[b] == '.') ? run + 1 : 0; if (run > best) best = run; }
    return best;
}
static int free_blocks(void) { int n = 0; for (int b = 0; b < BLOCKS; b++) n += (disk[b] == '.'); return n; }

int main(void)
{
    int index[32];
    memset(disk, '.', BLOCKS);
    alloc_contiguous('A', 6); alloc_contiguous('B', 8);
    alloc_contiguous('C', 5); alloc_contiguous('D', 7);
    release('B'); release('D');
    printf("%.32s  free=%d, largest free run=%d\n", disk, free_blocks(), largest_free_run());
    printf("Test 1: F  (15 blocks) contiguous -> %d (expect -1: external fragmentation)\n", alloc_contiguous('F', 15));
    printf("Test 2: F  (15 blocks) indexed    -> index block %d\n", alloc_indexed('F', 15, index));
    printf("%.32s  free=%d, largest free run=%d\n", disk, free_blocks(), largest_free_run());
    release('F'); release('f');   /* free data blocks AND the lowercase index block */
    printf("Test 3: G  (21 blocks) contiguous -> %d (expect -1)\n", alloc_contiguous('G', 21));
    printf("Test 4: G  (21 blocks) indexed    -> %d (expect -1: needs 21 data + 1 index = 22 > 21 free)\n", alloc_indexed('G', 21, index));
    printf("Test 5: G  (20 blocks) indexed    -> %d (largest indexed file = free - 1)\n", alloc_indexed('G', 20, index));
    printf("Test 6: H  (0 blocks)  contiguous -> %d (expect -1: invalid size)\n", alloc_contiguous('H', 0));
    printf("%.32s  free=%d\n", disk, free_blocks());
    return 0;
}
