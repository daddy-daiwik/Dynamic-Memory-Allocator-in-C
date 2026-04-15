# Dynamic Memory Allocator in C

This is a tiny custom memory allocator written in C. In simple terms, it gives you your own `my_malloc()` and `my_free()` instead of using the standard library ones.

## What it does

The allocator asks the operating system for raw memory with `sbrk()`, adds a small header in front of each allocation, and returns the part after the header to the user. The header stores basic info like the block size, whether it is in use, and links to the previous and next block.

## How it works

- Every allocation gets a hidden header in front of it. That header is the `struct Block` in `my_alloc.c`.
- The header stores the block size, a `bool` that says whether the block is in use, a magic marker for a basic safety check, and `prev` / `next` pointers.
- All blocks are connected in a doubly linked list. `heap_start` points to the first block, and `prev` tracks the most recently allocated block.
- `my_malloc(size)` asks the OS for `sizeof(struct Block) + size` bytes with `sbrk()`. The first part becomes the header, and the part after it is returned to the caller.
- When a new block is added, the code links it to the previous block by setting `prev->next` and `new_block->prev`.
- `my_free(ptr)` jumps back from the user pointer to the hidden header, checks the magic value, and marks the block as free.

This is a learning project, so it is very small and does not do advanced things like reusing freed memory or merging blocks.

## Try it out

1. Build it:

	```bash
	gcc main.c my_alloc.c -o main
	```

2. Run the program:

	```bash
	./main
	```

3. You should see the allocated integers printed, along with messages showing when memory is created and freed.
