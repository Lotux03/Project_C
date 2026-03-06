#include <stdio.h>
#include <stdlib.h>
#include "memtrack.h"

// Memory Tracker For Memory Allocate (malloc)
void* my_malloc(size_t size) {
    void* ptr = malloc(size);
    printf("Allocated %zu bytes at %p\n", size, ptr);
    return ptr;
}

// Memory Tracker For Memory Reallocate (realloc)
void* my_realloc(void* data, size_t size) {
    void* new_ptr = realloc(data, size);
    if (!new_ptr) {
        printf("Realloc failed!\n");
        return data;  // original memory still valid
    }

    printf("Reallocated to %zu bytes at %p\n", size, new_ptr);
    return new_ptr;
}

// Memory Tracker For Freeing Memory (free)
void my_free(void* data) {
    printf("Freed bytes at %p\n", data);
    free(data);
}

// Memory Tracker For memcpy
void* my_memcpy(void* dest, const void* src, size_t n)
{
    // call the real memcpy
    void* result = memcpy(dest, src, n);

    // log the copy
    printf("Copied %zu bytes from %p to %p\n", n, src, dest);

    return result;
}