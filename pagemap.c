#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static void query_pagemap(const char *label, void *vaddr) {
    uint64_t entry = 0;
    long page_size = sysconf(_SC_PAGESIZE);
    unsigned long vpn = (unsigned long)vaddr / (unsigned long)page_size;

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        perror("open pagemap");
        return;
    }

    if (lseek(fd, (off_t)(vpn * 8UL), SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return;
    }

    if (read(fd, &entry, sizeof(entry)) != (ssize_t)sizeof(entry)) {
        perror("read");
        close(fd);
        return;
    }

    close(fd);

    int present = (int)((entry >> 63) & 1U);
    int swapped = (int)((entry >> 62) & 1U);
    uint64_t pfn = entry & 0x7FFFFFFFFFFFFFULL;

    printf("  %-14s VA=%p  present=%d  swapped=%d", label, vaddr, present,
           swapped);
    if (present) {
        printf("  PFN=0x%llx  ->  PA=0x%llx",
               (unsigned long long)pfn,
               (unsigned long long)(pfn * (uint64_t)page_size));
    } else {
        printf("  (no physical frame assigned)");
    }
    printf("\n");
}

int main(void) {
    const size_t region_size = 4096 * 4;
    char *p = mmap(NULL, region_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("=== Before touching any page ===\n");
    query_pagemap("page[0]:", p);
    query_pagemap("page[1]:", p + 4096);
    query_pagemap("page[2]:", p + 8192);

    p[0] = 'X';
    p[4096] = 'Y';

    printf("\n=== After touching pages 0 and 1 ===\n");
    query_pagemap("page[0]:", p);
    query_pagemap("page[1]:", p + 4096);
    query_pagemap("page[2]:", p + 8192);

    if (munmap(p, region_size) != 0) {
        perror("munmap");
        return 1;
    }

    return 0;
}
