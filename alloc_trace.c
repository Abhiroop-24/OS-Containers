#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    void *brk_before = sbrk(0);
    printf("Initial break:       %p\n\n", brk_before);

    char *small = malloc(1024);
    if (!small) {
        perror("malloc small");
        return 1;
    }
    printf("After malloc(1KB):   break=%p  ptr=%p  (break moved up)\n", sbrk(0),
           (void *)small);

    char *large = malloc(256 * 1024);
    if (!large) {
        perror("malloc large");
        free(small);
        return 1;
    }
    printf("After malloc(256KB): break=%p  ptr=%p  (break unchanged!)\n",
           sbrk(0), (void *)large);

    char *small2 = malloc(512);
    if (!small2) {
        perror("malloc small2");
        free(small);
        free(large);
        return 1;
    }
    printf("After malloc(512B):  break=%p  ptr=%p\n", sbrk(0), (void *)small2);

    printf("\nsmall  at %p\n", (void *)small);
    printf("small2 at %p  (close to small - both on heap)\n", (void *)small2);
    printf("large  at %p  (far away - mmap'd separately)\n", (void *)large);

    free(small);
    free(small2);
    free(large);
    return 0;
}
