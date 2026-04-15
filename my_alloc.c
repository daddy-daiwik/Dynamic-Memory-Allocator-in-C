#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

struct Block{
    int marker;
    struct Block* prev;
    struct Block* next;
    bool inUse;
    size_t size;
};

// struct block_header HEAD = {0x55, 0xDD, 0, 0};
void *heap_start = NULL;
struct Block *prev = NULL;
int MAGIC_NUMBER = 0x55;

void* my_malloc(size_t size){
    size_t total_size = sizeof(struct Block) + size;

    // 2. Ask the OS for that exact amount (Skipping 4KB page optimization for V1)
    void *request = (void *)sbrk(total_size);
    if (request == (void*)-1) {
        return NULL; // sbrk failed
    }
    struct Block *new_block = (struct Block*)request;
    new_block->prev = prev;
    new_block->next = NULL;
    new_block->inUse = true;
    new_block->size = size;
    new_block->marker = MAGIC_NUMBER;
    if (prev!=NULL){
        prev->next = new_block;
    }

    prev = new_block;
    if (heap_start == NULL) {
        heap_start = new_block;
    }
    printf("Memory successfully initialized with overall size: %zu\nAnd user requested size: %zu\n", total_size, size);
    return (void *)(new_block + 1);
}

void my_free(void* ptr){
    struct Block *old_block = (struct Block*)ptr - 1;
    if (old_block->marker != MAGIC_NUMBER){
        return;
    }
    old_block->inUse = false;
    prev = old_block->prev;
    printf("Memory freed successfully\n");
    return;
}