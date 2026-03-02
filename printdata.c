#include <stdio.h>

void printdata(int* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("Pointer: %p, Data: %d\n",
            (void*)(data + i),
            data[i]);
    }
}