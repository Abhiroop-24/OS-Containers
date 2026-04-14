#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MB (1024UL * 1024UL)
#define PAGE 4096UL

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

    unsigned long detected_ram_mb = memtotal_kb / 1024UL;
    unsigned long base_ram_mb = detected_ram_mb;
    unsigned long accesses = 100000UL;

    // Optional overrides for safer runs on low-RAM systems:
    //   ./thrash <base_ram_mb> <accesses>
    if (argc > 1) {
        base_ram_mb = strtoul(argv[1], NULL, 10);
    }
    if (argc > 2) {
        accesses = strtoul(argv[2], NULL, 10);
    }

    printf("Detected RAM: %lu MB\n", detected_ram_mb);
    printf("Benchmark base RAM: %lu MB\n", base_ram_mb);
    printf("Random accesses per test: %lu\n\n", accesses);
    printf("%-12s  %8s  %12s  %16s\n", "Alloc(MB)", "Time(s)", "Throughput",
           "Status");
    printf("-----------------------------------------------------------\n");

    int percents[] = {25, 50, 75, 100, 130, 160};
    int num_tests = (int)(sizeof(percents) / sizeof(percents[0]));

    for (int t = 0; t < num_tests; t++) {
        unsigned long target_mb = base_ram_mb * (unsigned long)percents[t] / 100UL;
        unsigned long target_bytes = target_mb * MB;

        char *region = malloc(target_bytes);
        if (!region) {
            printf("%-12lu  FAILED (malloc returned NULL)\n", target_mb);
            continue;
        }

        for (unsigned long i = 0; i < target_bytes; i += PAGE) {
            region[i] = 1;
        }

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        unsigned long num_pages = target_bytes / PAGE;
        volatile char sink = 0;
        unsigned long seed = 12345UL;
        for (unsigned long i = 0; i < accesses; i++) {
            seed = seed * 6364136223846793005UL + 1UL;
            unsigned long page_idx = (seed >> 16) % num_pages;
            sink += region[page_idx * PAGE];
        }
        (void)sink;

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (double)(end.tv_sec - start.tv_sec) +
                         (double)(end.tv_nsec - start.tv_nsec) / 1e9;
        double throughput = accesses / elapsed;

        const char *status = "THRASHING";
        if (elapsed < 1.0) {
            status = "OK (in-RAM)";
        } else if (elapsed < 10.0) {
            status = "SLOW (swapping)";
        }

        printf("%-12lu  %8.2f  %10.0f/s  %s\n", target_mb, elapsed, throughput,
               status);

        free(region);
        sleep(2);
    }

    return 0;
}
