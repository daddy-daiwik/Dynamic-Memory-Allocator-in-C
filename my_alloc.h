// my_alloc.h
#ifndef MY_ALLOC_H
#define MY_ALLOC_H

#include <stddef.h> // For size_t

void* my_malloc(size_t size);
void* my_calloc(size_t n, size_t size);
void my_free(void* ptr);
void print_stats();
void print_heap();
#endif