// vector.c
// A dynamic array implementation in C (non generic) <-- work in progress to make it generic!

// Goal: Represent your dynamic array with size, capacity, and data storage.

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "memtrack.h"

typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntArray;

void init(IntArray* arr, size_t initial_capacity) {
    arr->data = my_malloc(sizeof(int) * initial_capacity);
    if (!arr->data) {
        perror("malloc failed");
        exit(1);
    }
    arr->size = 0;
    arr->capacity = initial_capacity;
}

// Add an element
void push_back(IntArray* arr, int value) {
    if (arr->size >= arr->capacity) {
        size_t new_capacity = arr->capacity * 2;
        int* new_data = my_realloc(arr->data, sizeof(int) * new_capacity);
        if (!new_data) {
            perror("realloc failed");
            exit(1);
        }
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    arr->data[arr->size] = value;
    arr->size++;
}

// Resize the array
void resize(IntArray* arr, size_t new_capacity) {
    if (new_capacity < arr->size) {
        arr->size = new_capacity;
    }
    int* new_data = my_realloc(arr->data, sizeof(int) * new_capacity);
    if (!new_data) {
        perror("realloc failed");
        exit(1);
    }
    arr->data = new_data;
    arr->capacity = new_capacity;
}

void freeArray(IntArray* arr) {
    my_free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

int get(IntArray* arr, size_t index) {
    if (index >= arr->size) {
        fprintf(stderr, "Index %zu out of bounds (size=%zu)\n", index, arr->size);
        exit(1);
    }
    return arr->data[index];
}

void set(IntArray* arr, size_t index, int value) {
    if (index >= arr->size) {
        fprintf(stderr, "Index %zu out of bounds (size=%zu)\n", index, arr->size);
        exit(1);
    }
    arr->data[index] = value;
}

// Alias for push_back (shell uses "push")
void push(IntArray* arr, int value) {
    push_back(arr, value);
}

// Get the current size
size_t size(IntArray* arr) {
    return arr->size;
}

// Get the current capacity
size_t capacity(IntArray* arr) {
    return arr->capacity;
}

// Print all elements in the vector
void print_vector(IntArray* arr) {
    printf("Vector [size=%zu, capacity=%zu]: ", arr->size, arr->capacity);
    if (arr->size == 0) {
        printf("(empty)\n");
        return;
    }
    printf("[ ");
    for (size_t i = 0; i < arr->size; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf(" ]\n");
}