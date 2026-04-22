#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>

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
int TRAIL_CANARY = 3141825;
int HEAD_CANARY = 3141826;

size_t total_allocated = 0;
size_t total_freed = 0;
int active_blocks = 0;

void print_stats() {
    printf("\n----- Memory Statistics: -----\n");
    printf("Total Allocated: %zu\n", total_allocated);
    printf("Total Freed: %zu\n", total_freed);
    printf("Active blocks: %d\n", active_blocks);
    printf("-----------------------------\n\n");
}

void print_heap() {
    struct Block* temp = heap_start;
    while (temp) {
        printf("[%s %zu] -> ",
            temp->inUse ? "USED" : "FREE",
            temp->size
        );
        temp = temp->next;
    }
    printf("NULL\n");
}

void* my_malloc(size_t size){
    size_t total_size = sizeof(struct Block) + sizeof(int) + size + sizeof(int);

    //1. Traverse and find a empty block enough big to hold the requested size
    struct Block *temp = heap_start;
    while(temp!=NULL){
        
        size_t remaining = temp->size - size;
        if (!temp->inUse && temp->marker == MAGIC_NUMBER){
            if (size == temp->size || remaining < sizeof(struct Block) + 2*sizeof(int) + 8){
                int* head = (int*)(temp + 1);
                *head = HEAD_CANARY;
                int* tail = (int*)((char*)(head + 1) + size);
                *tail = TRAIL_CANARY;
                temp->size = size;
                printf("Using old block\n");
                temp->inUse = true;
                active_blocks++;
                return (void*)(head + 1);
            }
            else if (size < temp->size){
                int* head = (int*)(temp + 1);
                *head = HEAD_CANARY;
                int *tail = (int*)((char*)(head + 1) + size);
                *tail = TRAIL_CANARY;
                printf("Using old block by splitting\n");
                struct Block *new_block = (struct Block*)(tail + 1);
                new_block->inUse = false;
                new_block->marker = MAGIC_NUMBER;
                new_block->next = temp->next;
                new_block->prev = temp;
                temp->next = new_block;
                int *new_head = (int *)(new_block+1);
                new_block->size = temp->size - size - sizeof(struct Block) - 2*sizeof(int);
                *new_head = HEAD_CANARY;
                char* user_space = (char *)(new_head + 1);
                int* new_tail = (int *)(user_space + new_block->size);
                *new_tail = TRAIL_CANARY;
                size_t old_size = temp->size;
                size_t remaining = old_size - size;

                new_block->size = remaining - sizeof(struct Block) - 2*sizeof(int);
                temp->inUse = true;
                if (new_block->next)
                    new_block->next->prev = new_block;
                active_blocks++;
                return (void*)(head+1);
            }
        }
        temp = temp->next;
    }

    // 2. Ask the OS for that exact amount
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

    //[Block struct][HEAD CANARY][USER DATA][TAIL CANARY]
    int* head_address = (int *)(new_block + 1);
    *head_address = HEAD_CANARY;
    char* user_space = (char *)(head_address + 1) + size;
    int* trail_address = (int *)(user_space);
    *trail_address = TRAIL_CANARY;
    total_allocated += size;
    active_blocks++;
    printf("Memory successfully initialized with overall size: %zu\nAnd user requested size: %zu\n", total_size, size);
    return (void *)(head_address + 1);
}

void* my_calloc(size_t n, size_t size) {
    if (size != 0 && n > SIZE_MAX / size) {
        return NULL; // overflow
    }
    void* ptr = my_malloc(n * size);
    if (ptr) memset(ptr, 0, n * size);
    return ptr;
}

void my_free(void* ptr){
    if (ptr == NULL) return;
    printf("Freeing memory at address: %p\n", ptr);
    char* p = (char*)ptr;
    int* head_address = (int*)(p - sizeof(int));
    if (*head_address != HEAD_CANARY) {
        fprintf(stderr, "UNDERFLOW detected!\n");
        abort();
    }
    struct Block* old_block = (struct Block*)(p - sizeof(int) - sizeof(struct Block));
    char* user_space = (char *)(ptr) + old_block->size;
    int* trail_address = (int *)(user_space);
    if (!old_block->inUse) {
        fprintf(stderr, "Double free detected\n");
        abort();
    }
    if (old_block->marker != MAGIC_NUMBER || *trail_address != TRAIL_CANARY || *head_address != HEAD_CANARY){
        fprintf(stderr, "Heap corruption: overflow detected at %p\n", ptr);
        abort();
        return;
    }
    old_block->inUse = false;
    total_freed += old_block->size;
    active_blocks--;
    printf("Memory freed successfully\n");
    return;
}