#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MB (1024UL * 1024UL)

static void show_rss(const char *label) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "grep -E 'VmSize|VmRSS|VmSwap' /proc/%d/status",
             getpid());

    printf("\n[%s]\n", label);
    fflush(stdout);
    system(cmd);
}

int main(void) {
    show_rss("1. before malloc");

    char *big = malloc(256 * MB);
    if (!big) {
        perror("malloc");
        return 1;
    }

    show_rss("2. after malloc, before touch - virtual only!");

    memset(big, 'A', 64 * MB);
    show_rss("3. after touching 64 MB");

    memset(big, 'B', 256 * MB);
    show_rss("4. after touching all 256 MB");

    free(big);
    return 0;
}
