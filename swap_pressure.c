#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MB (1024UL * 1024UL)

int main(int argc, char *argv[]) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        perror("fopen /proc/meminfo");
        return 1;
    }

    unsigned long memtotal_kb = 0;
    if (fscanf(f, "MemTotal: %lu kB", &memtotal_kb) != 1) {
        fclose(f);
        fprintf(stderr, "Failed to parse MemTotal from /proc/meminfo\n");
        return 1;
    }
    fclose(f);

    unsigned long percent = 80UL;
    if (argc > 1) {
        percent = strtoul(argv[1], NULL, 10);
        if (percent == 0UL || percent > 95UL) {
            fprintf(stderr,
                    "Invalid percent '%s'. Use 1..95 (default: 80).\n",
                    argv[1]);
            return 1;
        }
    }

    unsigned long alloc_mb = (memtotal_kb / 1024UL) * percent / 100UL;
    printf("Total RAM: %lu MB, allocating %lu MB (%lu%%)\n", memtotal_kb / 1024UL,
           alloc_mb, percent);

    char *region = malloc(alloc_mb * MB);
    if (!region) {
        perror("malloc");
        return 1;
    }

    printf("Touching all pages...\n");
    for (unsigned long i = 0; i < alloc_mb * MB; i += 4096UL) {
        region[i] = (char)(i & 0xFFUL);
    }

    printf("Done. Sleeping 5s - run 'vmstat 1' in another terminal.\n");
    printf("PID: %d\n", getpid());
    fflush(stdout);
    sleep(5);

    printf("Re-reading all pages...\n");
    fflush(stdout);
    volatile char sink = 0;
    for (unsigned long i = 0; i < alloc_mb * MB; i += 4096UL) {
        sink += region[i];
    }
    (void)sink;

    printf("\nMemory status:\n");
    fflush(stdout);
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "grep -E 'VmRSS|VmSwap' /proc/%d/status",
             getpid());
    system(cmd);

    free(region);
    return 0;
}
