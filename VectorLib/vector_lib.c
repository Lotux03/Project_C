#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "memtrack.h"


void init(Array* arr, size_t elem_size, int initSize) {
    arr->elem_size = elem_size;
    arr->data = my_malloc(elem_size * initSize);
    arr->size = 0;
    arr->cap = initSize;
}

void push_back(Array* arr, void* content) {
    if (arr->size == arr->cap) {
        void* temp = my_realloc(arr->data, (arr->elem_size * arr->cap * 2));
        if (!temp) {
            printf("realloc failed lol");
            exit(1);
        }
        arr->data = temp;
        arr->cap *= 2;
    }
    my_memcpy(
        (char*)arr->data + (arr->size * arr->elem_size),
        content,
        arr->elem_size
    );
    arr->size++;
}

void free_array(Array* arr) {
    my_free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->cap = 0;
    arr->elem_size = 0;
}