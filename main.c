#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <stdint.h>
#include "my_malloc/my_malloc.h"

// test
int main(void)

// fyi main is made with ai for testing
{
    printf("--- malloc alignment stress test ---\n");

    size_t sizes[] = {
        1, 2, 3, 7, 8,
        13, 42, 103,
        512, 89,
        1023, 204,
        77, 15,
        500, 600,
        33, 1000,
        99, 1233,
        121, 92,
        1111, 1111,
        3, 8,
        1111, 496,
        17, 31, 63, 127
    };

    size_t count = sizeof(sizes) / sizeof(sizes[0]);

    void *ptrs[count];

    for (size_t i = 0; i < count; i++) {
        ptrs[i] = my_malloc(sizes[i]);

        printf(
            "Alloc %2zu | Req: %4zu | Ptr: %p | aligned: %s\n",
            i + 1,
            sizes[i],
            ptrs[i],
            ((uintptr_t)ptrs[i] % 8 == 0) ? "YES" : "NO"
        );

        memset(ptrs[i], 'A' + (i % 26), sizes[i]);
    }

    printf("--- verifying writes ---\n");

    for (size_t i = 0; i < count; i++) {
        char *p = ptrs[i];

        printf(
            "Block %2zu | first byte: %c | ptr: %p\n",
            i + 1,
            p[0],
            p
        );
    }

    printf("--- done ---\n");

    return 0;
}