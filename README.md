# Dynamic Memory Allocator in C

This project is a tiny custom allocator written in C. It provides `my_malloc()`, `my_calloc()`, and `my_free()` as learning-friendly alternatives to standard allocation routines.

## Latest features

- **Custom allocation (`my_malloc`)**
  - Reuses a previously freed block if it is large enough.
  - Otherwise requests more heap memory from the OS using `sbrk()`.
- **Zero-initialized allocation (`my_calloc`)**
  - Allocates `n * size` bytes using `my_malloc()`.
  - Clears the allocated memory with `memset`.
  - Includes integer-overflow protection for `n * size`.
- **Safe deallocation (`my_free`)**
  - Detects and aborts on:
    - buffer underflow (head canary mismatch),
    - buffer overflow / heap corruption (tail canary mismatch),
    - double free,
    - invalid/corrupted block marker.
- **Heap bookkeeping**
  - Maintains a doubly linked block list (`prev`/`next`).
  - Tracks allocation statistics:
    - `total_allocated`
    - `total_freed`
    - `active_blocks`
- **Debug utilities**
  - `print_stats()` shows allocator statistics.
  - `print_heap()` prints block usage layout like `[USED size] -> [FREE size] -> NULL`.

## Block layout

Each allocation is stored as:

1. `struct Block` header
2. Head canary (`HEAD_Canary`)
3. User-accessible memory (returned by `my_malloc`)
4. Tail canary (`TRAIL_CANARY`)

The header stores:

- `marker`: validation value (`MAGIC_NUMBER`)
- `prev` / `next`: list links
- `inUse`: whether block is currently allocated
- `size`: user-requested payload size

The extra head/tail canary values are used to detect out-of-bounds writes.

## Allocation flow

`my_malloc(size)` works in two stages:

1. Scans the existing block list for a free block (`inUse == false`) with enough capacity.
2. If none is found, calls `sbrk(sizeof(struct Block) + size + sizeof(int))` to create a new block, appends it to the list, writes the trailing canary, and returns the payload address.

## Free flow

`my_free(ptr)`:

1. Returns immediately for `NULL`.
2. Validates the head canary (underflow check).
3. Locates metadata and validates marker + tail canary.
4. Rejects double free.
5. Marks the block free and updates stats.

## Public API

Defined in `my_alloc.h`:

```c
void* my_malloc(size_t size);
void* my_calloc(size_t n, size_t size);
void my_free(void* ptr);
void print_stats(void);
void print_heap(void);
```

## Build and run

```bash
gcc main.c my_alloc.c -o main
./main
```

## Demo program (`main.c`)

The sample program currently:

- allocates a 40-byte character buffer
- writes and prints a sample team string
- frees the string buffer
- allocates a zero-initialized integer array of 5 elements with `my_calloc`
- fills and prints values `0, 10, 20, 30, 40`
- frees the array
- prints final heap layout using `print_heap()`

You will also see allocator debug messages such as:

- `Memory successfully initialized ...` (new block from OS)
- `Using old block` (reused freed block)
- `Memory freed successfully`
- `UNDERFLOW detected!`
- `Double free detected`
- `Heap corruption: overflow detected ...`
