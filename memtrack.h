// memtrack.h

#ifndef MEMTRACK_H
#define MEMTRACK_H

#include <stddef.h>

void* my_malloc(size_t size);
void* my_realloc(void* data, size_t size);
void my_free(void* data);

#endif