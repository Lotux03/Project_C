#pragma once
// vector.h
// A dynamic array implementation in C (non generic)

#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntArray;

void init(IntArray* arr, size_t initial_capacity);
void push(IntArray* arr, int value);
size_t size(IntArray* arr);
size_t capacity(IntArray* arr);
void print_vector(IntArray* arr);
void push_back(IntArray* arr, int value);
void resize(IntArray* arr, size_t new_capacity);
void freeArray(IntArray* arr);
int get(IntArray* arr, size_t index);
void set(IntArray* arr, size_t index, int value);

#endif