#pragma once
// vector_lib.h

#ifndef VECTOR_LIB_H
#define VECTOR_LIB_H

#include <stddef.h>

typedef struct {
    void* data;
    size_t size;
    size_t cap;
    size_t elem_size;
} Array;

void array_init(Array* arr, size_t elem_size, int initSize);
void array_push_back(Array* arr, void* content);
void array_free(Array* arr);

#endif
