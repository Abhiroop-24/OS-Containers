#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE (4 * 1024 * 1024)
#define BLOCK_SIZE 4096
#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE)

int main(void) {
    char *pool = malloc(POOL_SIZE);
    if (!pool) {
        perror("malloc");
        return 1;
    }

    int allocated[NUM_BLOCKS];
    memset(allocated, 1, sizeof(allocated));

    printf("Pool: %d blocks of %d bytes = %d KB total\n", NUM_BLOCKS, BLOCK_SIZE,
           POOL_SIZE / 1024);

    int freed = 0;
    for (int i = 0; i < NUM_BLOCKS; i += 2) {
        allocated[i] = 0;
        freed++;
    }
    printf("Freed every other block: %d free blocks (%d KB free)\n", freed,
           freed * BLOCK_SIZE / 1024);

    int need = 128;
    int found = 0;
    int run = 0;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (!allocated[i]) {
            run++;
            if (run >= need) {
                found = 1;
                break;
            }
        } else {
            run = 0;
        }
    }

    printf("\nLooking for %d contiguous free blocks (%d KB)...\n", need,
           need * BLOCK_SIZE / 1024);
    if (found) {
        printf("SUCCESS - found contiguous run\n");
    } else {
        printf("FAILED - %d KB free total, but largest contiguous run < %d KB\n",
               freed * BLOCK_SIZE / 1024, need * BLOCK_SIZE / 1024);
    }
    printf("This is EXTERNAL FRAGMENTATION.\n");

    printf("\n--- Internal Fragmentation ---\n");
    char *tiny = malloc(1);
    char *tiny2 = malloc(1);
    if (!tiny || !tiny2) {
        perror("malloc tiny");
        free(tiny);
        free(tiny2);
        free(pool);
        return 1;
    }

    printf("malloc(1) at %p, malloc(1) at %p\n", (void *)tiny, (void *)tiny2);
    printf("Distance between them: %ld bytes (wasted: %ld bytes per alloc)\n",
           (long)(tiny2 - tiny), (long)(tiny2 - tiny) - 1L);

    free(tiny);
    free(tiny2);
    free(pool);
    return 0;
}
