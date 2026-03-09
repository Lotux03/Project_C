#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    void* data;
    size_t size;
    size_t cap;
    size_t elem_size;
} Array;

void array_init(Array* arr, size_t elem_size, int initSize) {
    arr->elem_size = elem_size;
    arr->data = malloc(elem_size * initSize);
    arr->size = 0;
    arr->cap = initSize;
}

void array_push_back(Array* arr, void* content) {
    if (arr->size == arr->cap) {
        void* temp = malloc(arr->data, (arr->elem_size * arr->cap * 2));
        if (!temp) {
            printf("realloc failed lol");
            exit(1);
        }
        arr->data = temp;
        arr->cap *= 2;
    }
    memcpy(
        (char*)arr->data + (arr->size * arr->elem_size),
        content,
        arr->elem_size
    );
    arr->size++;
}

void array_free(Array* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->cap = 0;
    arr->elem_size = 0;
}