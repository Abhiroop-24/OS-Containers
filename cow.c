#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MB (1024UL * 1024UL)
#define ALLOC_MB 128UL

static long get_minor_faults(void) {
    FILE *f = fopen("/proc/self/stat", "r");
    if (!f) {
        return -1;
    }

    char line[4096];
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return -1;
    }
    fclose(f);

    char *p = strrchr(line, ')');
    if (!p) {
        return -1;
    }
    p += 2;

    char state;
    int ppid, pgrp, session, tty_nr, tpgid;
    unsigned long flags;
    long minflt;
    int parsed = sscanf(p, "%c %d %d %d %d %d %lu %ld", &state, &ppid, &pgrp,
                        &session, &tty_nr, &tpgid, &flags, &minflt);
    if (parsed != 8) {
        return -1;
    }

    return minflt;
}

int main(void) {
    char *region = malloc(ALLOC_MB * MB);
    if (!region) {
        perror("malloc");
        return 1;
    }

    memset(region, 'P', ALLOC_MB * MB);

    printf("Parent: allocated and touched %lu MB (%lu pages)\n", ALLOC_MB,
           ALLOC_MB * 256UL);
        fflush(stdout);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        free(region);
        return 1;
    }

    if (pid == 0) {
        long faults_start = get_minor_faults();

        volatile char sink = 0;
        for (unsigned long i = 0; i < ALLOC_MB * MB; i += 4096UL) {
            sink += region[i];
        }
        (void)sink;
        long faults_after_read = get_minor_faults();

        printf("\nChild READ-ONLY scan:\n");
        printf("  Page faults: %ld  (should be ~0, pages are shared)\n",
               faults_after_read - faults_start);

        long faults_before_write = get_minor_faults();
        memset(region, 'C', ALLOC_MB * MB);
        long faults_after_write = get_minor_faults();

        printf("\nChild WRITE pass:\n");
        printf("  Page faults: %ld\n", faults_after_write - faults_before_write);
        printf("  Expected:    %lu  (one COW fault per page)\n", ALLOC_MB * 256UL);

        free(region);
        fflush(stdout);
        _exit(0);
    }

    wait(NULL);
    free(region);
    return 0;
}
