// my_alloc.h
#ifndef MY_ALLOC_H
#define MY_ALLOC_H

#include <stddef.h> // For size_t

void* my_malloc(size_t size);
void my_free(void* ptr);

#endif