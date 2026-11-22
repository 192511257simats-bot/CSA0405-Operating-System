#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define BLK 4096ULL                       /* block size                         */
#define PTRS (BLK / 4)                    /* 4-byte pointers -> 1024 per block  */

static double now_us(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1e6 + t.tv_nsec / 1e3; }

/* number of index-block reads needed for logical block k of an inode (12 direct, S, D, T) */
static int index_reads(unsigned long long k)
{
    if (k < 12) return 0;
    k -= 12; if (k < PTRS) return 1;
    k -= PTRS; if (k < PTRS * PTRS) return 2;
    return 3;
}

int main(void)
{
    unsigned long long nblocks = (4ULL << 30) / BLK;                 /* 4 GB file */
    unsigned long long last = nblocks - 1;
    printf("== Task 2.3  4 GB file, 4 KB blocks ==\n");
    printf("blocks in file                  : %llu\n", nblocks);
    printf("contiguous : disk reads to reach last block (data only)      : 1\n");
    printf("linked (no FAT): disk reads to reach last block              : %llu\n", nblocks);
    printf("FAT in RAM : memory look-ups %llu + 1 disk read\n", last);
    printf("indexed inode: index reads %d + 1 data read = %d disk reads\n", index_reads(last), index_reads(last) + 1);

    unsigned long long maxblk = 12ULL + PTRS + PTRS * PTRS + PTRS * PTRS * PTRS;
    printf("\nmax file size (12 direct + S + D + T indirect):\n");
    printf("  direct          : 12 x 4 KB            = %llu bytes\n", 12ULL * BLK);
    printf("  single indirect : 1024 x 4 KB          = %llu bytes\n", PTRS * BLK);
    printf("  double indirect : 1024^2 x 4 KB        = %llu bytes\n", PTRS * PTRS * BLK);
    printf("  triple indirect : 1024^3 x 4 KB        = %llu bytes\n", PTRS * PTRS * PTRS * BLK);
    printf("  TOTAL = %llu blocks = %llu bytes = %.6f TiB\n", maxblk, maxblk * BLK, (double)(maxblk * BLK) / (1ULL << 40));

    unsigned long long dblocks = (4ULL << 40) / BLK;
    printf("\n== Task 2.4  4 TB disk, 4 KB blocks ==\n");
    printf("blocks on disk                  : %llu (= 2^30)\n", dblocks);
    printf("bit vector (1 bit/block)        : %llu bytes = %llu MiB\n", dblocks / 8, dblocks / 8 >> 20);
    printf("FAT (4 bytes/entry)             : %llu bytes = %llu GiB\n", dblocks * 4, dblocks * 4 >> 30);
    printf("linked free list overhead       : 1 pointer per free block (4 B) held inside the free blocks themselves\n");

    /* Alternative concept: measure FAT chain walking (fragmented file => shuffled chain) */
    uint32_t *fat = malloc(nblocks * sizeof *fat);
    uint32_t *perm = malloc(nblocks * sizeof *perm);
    if (!fat || !perm) { fprintf(stderr, "out of memory\n"); return 1; }
    for (unsigned long long i = 0; i < nblocks; i++) perm[i] = (uint32_t)i;
    srand(7);
    for (unsigned long long i = nblocks - 1; i > 0; i--) { unsigned long long j = ((unsigned long long)rand() * 32768ULL + (unsigned long long)rand()) % (i + 1); uint32_t t = perm[i]; perm[i] = perm[j]; perm[j] = t; }
    for (unsigned long long i = 0; i + 1 < nblocks; i++) fat[perm[i]] = perm[i + 1];   /* file block i lives at disk block perm[i] */
    fat[perm[nblocks - 1]] = 0xFFFFFFFFu;

    printf("\n== Alternative concept: FAT chain walk (in RAM) vs inode index (measured) ==\n");
    printf("%-12s %-14s %-14s %-14s %-12s\n", "block k", "FAT look-ups", "FAT time (us)", "index reads", "index calc (us)");
    unsigned long long ks[] = {10, 1000, 100000, 524288, 1048575};
    for (int q = 0; q < 5; q++) {
        unsigned long long k = ks[q];
        double t0 = now_us();
        uint32_t cur = perm[0]; unsigned long long hops = 0;
        for (unsigned long long i = 0; i < k; i++) { cur = fat[cur]; hops++; }
        double t1 = now_us();
        volatile int r = index_reads(k);      /* index path: constant number of steps */
        double t2 = now_us();
        printf("%-12llu %-14llu %-14.1f %-14d %-12.3f   (reached disk block %u)\n", k, hops, t1 - t0, r, t2 - t1, cur);
    }
    free(fat); free(perm);
    return 0;
}
