// vector_lib.h

#ifndef VECTOR_LIB_H
#define VECTOR_LIB_H

#include <stddef.h>

void init(Array* arr, size_t elem_size, int initSize);
void push_back(Array* arr, void* content);
void free_array(Array* arr);

#endif
