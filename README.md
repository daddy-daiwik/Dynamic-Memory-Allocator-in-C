# Dynamic Memory Allocator in C

This project is a tiny custom allocator written in C. It provides `my_malloc()` and `my_free()` as a learning-friendly alternative to standard `malloc()` and `free()`.

## Current behavior

- Requests memory from the OS with `sbrk()` when no suitable free block exists.
- Stores metadata in a hidden header (`struct Block`) placed right before user memory.
- Tracks all blocks in a doubly linked list using `prev` and `next` pointers.
- Reuses previously freed blocks when they are large enough for a new request.
- Uses a magic marker plus a trailing canary in `my_free()` to detect corruption.
- Aborts the process if heap corruption is detected during `my_free()`.

## Block layout

Each allocation is stored as:

1. `struct Block` header
2. User-accessible memory (returned by `my_malloc`)

The header stores:

- `marker`: validation value (`MAGIC_NUMBER`)
- `prev` / `next`: list links
- `inUse`: whether block is currently allocated
- `size`: user-requested payload size

Each allocated block also stores a trailing canary value immediately after the user payload. That extra value is used to catch writes past the end of the allocation.

## Allocation flow

`my_malloc(size)` works in two stages:

1. Scans the existing block list for a free block (`inUse == false`) with enough capacity.
2. If none is found, calls `sbrk(sizeof(struct Block) + size + sizeof(int))` to create a new block, appends it to the list, writes the trailing canary, and returns the payload address.

## Free flow

`my_free(ptr)`:

1. Moves one header-size back from `ptr` to locate the block metadata.
2. Verifies both the marker value and the trailing canary.
3. Aborts immediately if either check fails, because that indicates heap corruption.
4. Marks the block as free (`inUse = false`) so it can be reused later.

## Public API

Defined in `my_alloc.h`:

```c
void* my_malloc(size_t size);
void my_free(void* ptr);
```

## Build and run

```bash
gcc main.c my_alloc.c -o main
./main
```

## Demo program (`main.c`)

The sample program currently:

- allocates a 40-byte character buffer
- writes and prints a sample team string with "Anna" included
- frees the string buffer
- allocates an integer array of 5 elements
- fills and prints values `0, 10, 20, 30, 40`
- frees the array

You will also see allocator debug messages such as:

- `Memory successfully initialized ...` (new block from OS)
- `Using old block` (reused freed block)
- `Memory freed successfully`
- `FATAL: Heap corruption detected! Aborting.` (invalid pointer or overwritten canary)
