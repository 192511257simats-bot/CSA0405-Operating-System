#include <stdio.h>
#define PAGE_SIZE 2048
#define PAGES 8
int page_table[PAGES] = {5, 2, -1, 7, 0, -1, 3, -1};
int main(void) {
    unsigned logical[] = {0, 1500, 3100, 6200, 9000, 2049};
    for (int i = 0; i < 6; i++) {
        unsigned a = logical[i];
        unsigned p = a / PAGE_SIZE, d = a % PAGE_SIZE;
        printf("%u -> page %u offset %4u : ", a, p, d);
        if (p >= PAGES) printf("invalid address\n");
        else if (page_table[p] == -1) printf("page fault\n");
        else printf("physical %u\n", (unsigned)page_table[p] * PAGE_SIZE + d);
    }
    return 0;
}
