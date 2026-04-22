# Dynamic Memory Allocator in C

This project is a small custom allocator written in C for learning purposes. It implements `my_malloc`, `my_calloc`, and `my_free`, plus helper functions to inspect allocator state.

## Features

- Requests memory from the OS with `sbrk()` when no suitable free block exists.
- Stores per-allocation metadata in an internal `struct Block` header.
- Maintains a doubly linked block list (`prev` / `next`).
- Reuses free blocks and can split a larger free block when possible.
- Uses a head canary and a trailing canary to detect underflow/overflow.
- Detects double free and aborts on corruption.
- Tracks simple runtime stats (`total_allocated`, `total_freed`, `active_blocks`).

## Public API

Defined in `my_alloc.h`:

```c
void* my_malloc(size_t size);
void* my_calloc(size_t n, size_t size);
void my_free(void* ptr);
void print_stats();
void print_heap();
```

## Block Layout

Each allocated region is laid out as:

```text
[struct Block][HEAD_CANARY][user bytes][TRAIL_CANARY]
```

Where `struct Block` currently contains:

- `marker` (must match `MAGIC_NUMBER`)
- `prev` / `next`
- `inUse`
- `size`

## Allocation Behavior

`my_malloc(size)`:

1. Scans the block list for a free block with enough capacity.
2. If an exact or near-exact fit is found, reuses the block.
3. If a free block is larger, it may split into:
	 - one used block for the request
	 - one new free block for the remainder
4. If no reusable block exists, requests fresh memory via `sbrk()`.

`my_calloc(n, size)`:

1. Checks multiplication overflow (`n > SIZE_MAX / size`).
2. Allocates with `my_malloc(n * size)`.
3. Zero-initializes the returned memory with `memset`.

## Free and Safety Checks

`my_free(ptr)` performs:

1. Null check (`NULL` is ignored).
2. Head canary verification (detects underflow).
3. Marker + trailing canary verification (detects corruption/overflow).
4. Double-free check (`inUse` must be true).
5. Marks block free and updates accounting.

On any corruption check failure, the allocator prints an error and calls `abort()`.

## Introspection Helpers

- `print_stats()` prints:
	- total allocated bytes
	- total freed bytes
	- active block count
- `print_heap()` prints block list state like:
	- `[USED 40] -> [FREE 8] -> NULL`

## Build and Run

```bash
gcc main.c my_alloc.c -o main
./main
```

## Current Demo (`main.c`)

The demo currently:

- Allocates a 40-byte string buffer with `my_malloc` and prints it.
- Prints allocator stats twice.
- Allocates space for two integers with `my_calloc`.
- Writes and prints `0` and `10`.
- Frees only the integer array (the string free call is commented out).
- Prints stats and full heap layout.

Typical output includes:

- `Memory successfully initialized ...`
- `Freeing memory at address: ...`
- `Memory freed successfully`
- Heap/state summaries from `print_stats()` and `print_heap()`
