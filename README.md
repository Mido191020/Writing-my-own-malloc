# Writing My Own Malloc 🧠

A custom memory allocator implementation in C that manages a fixed heap with efficient allocation and deallocation strategies.

## 📖 Overview

This project implements a simple but functional memory allocator similar to `malloc()` and `free()` from scratch. It manages a fixed-size heap (640KB) and tracks memory chunks using two separate lists: allocated chunks and freed chunks.

## 🎯 Features

- **Custom Memory Allocation**: `heap_alloc()` - allocates memory from a managed heap
- **Memory Deallocation**: `heap_free()` - returns memory to the free pool
- **First-Fit Algorithm**: Finds the first available chunk that fits the requested size
- **Chunk Merging**: Combines adjacent free chunks to reduce fragmentation
- **Binary Search**: Fast lookup of allocated chunks using sorted lists
- **Insertion Sort**: Maintains sorted chunk lists for efficient operations

## 🏗️ Architecture

### Memory Layout

```
┌─────────────────────────────────────────────────────────┐
│                    HEAP (640KB)                          │
│                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────────────┐     │
│  │ Allocated│  │  Free    │  │    Allocated     │     │
│  │  Chunk   │  │  Chunk   │  │     Chunk        │     │
│  └──────────┘  └──────────┘  └──────────────────┘     │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### Data Structures

#### Chunk
```c
typedef struct {
    void *start;   // Starting address of the chunk
    size_t size;   // Size of the chunk in bytes
} Chunk;
```

#### Chunk List
```c
typedef struct {
    size_t count;                    // Number of chunks in the list
    Chunk chunks[chunk_list_CAP];    // Array of chunks (max 1024)
} chunk_List;
```

### Core Components

1. **Allocated Chunks List** (`alloced_Chunks`)
   - Tracks all currently allocated memory blocks
   - Sorted by start address for binary search

2. **Free Chunks List** (`freed_chunks`)
   - Tracks all available memory blocks
   - Initially contains the entire heap
   - Sorted and merged to reduce fragmentation

## 🔄 How It Works

### Allocation Process

```
┌─────────────────┐
│ heap_alloc(size)│
└────────┬────────┘
         │
         ▼
┌─────────────────────────┐
│ Search freed_chunks     │
│ for chunk >= size       │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│ Found suitable chunk?   │
└────┬──────────────┬─────┘
     │ Yes          │ No
     ▼              ▼
┌─────────────┐  ┌──────────┐
│ Remove from │  │ Return   │
│ free list   │  │ NULL     │
└──────┬──────┘  └──────────┘
       │
       ▼
┌──────────────────────┐
│ Add to allocated list│
└──────┬───────────────┘
       │
       ▼
┌──────────────────────┐
│ Split remaining space│
│ back to free list    │
└──────┬───────────────┘
       │
       ▼
┌──────────────────┐
│ Return pointer   │
└──────────────────┘
```

### Deallocation Process

```
┌─────────────────┐
│ heap_free(ptr)  │
└────────┬────────┘
         │
         ▼
┌─────────────────────────┐
│ Find chunk in           │
│ allocated list          │
│ (binary search)         │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│ Remove from             │
│ allocated list          │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│ Add to free list        │
│ (maintains sorted order)│
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│ Merge adjacent chunks   │
│ (when heap_collect()    │
│  is implemented)        │
└─────────────────────────┘
```

## 🛠️ Key Functions

### `heap_alloc(size_t size)`
Allocates a block of memory of the specified size.
- **Returns**: Pointer to allocated memory, or NULL if allocation fails
- **Algorithm**: First-fit strategy
- **Time Complexity**: O(n) where n is the number of free chunks

### `heap_free(void *ptr)`
Frees a previously allocated block of memory.
- **Parameters**: Pointer to the memory block to free
- **Algorithm**: Binary search to find chunk, then move to free list
- **Time Complexity**: O(log n) for search + O(n) for insertion

### `chunk_list_merge(chunk_List *dst, const chunk_List *src)`
Merges adjacent free chunks to reduce fragmentation.
- **Effect**: Combines consecutive memory blocks into larger chunks
- **Time Complexity**: O(n)

### `chunk_list_find(const chunk_List *list, void *ptr)`
Finds a chunk in a sorted list using binary search.
- **Returns**: Index of chunk, or -1 if not found
- **Time Complexity**: O(log n)

### `chunk_list_insert(chunk_List *list, void *start, size_t size)`
Inserts a chunk while maintaining sorted order.
- **Algorithm**: Insertion sort
- **Time Complexity**: O(n)

## 📊 Example Usage

```c
// Allocate memory for a string
char *s = (char *)heap_alloc(strlen("MIDO LOVES MALOKY") + 1);

if (s != NULL) {
    strcpy(s, "MIDO LOVES MALOKY");
    printf("Allocated string: %s\n", s);
    
    // Free the memory when done
    heap_free(s);
} else {
    printf("Allocation failed.\n");
}
```

### Output
```
Chunks (0):
Chunks (1):
    Start: 0x..., Size: 640000
Allocated string: MIDO LOVES MALOKY
Chunks (1):
    Start: 0x..., Size: 18
Chunks (1):
    Start: 0x... (offset +18), Size: 639982
Chunks (0):
Chunks (1):
    Start: 0x..., Size: 640000
```

## 🚀 Building and Running

```bash
# Clone the repository
git clone https://github.com/Mido191020/Writing-my-own-malloc.git
cd Writing-my-own-malloc

# Compile
gcc -o malloc main.cpp -lstdc++

# Run
./malloc
```

## 📝 Configuration

- **Heap Size**: 640,000 bytes (640KB)
  ```c
  #define heap_CAP 640000
  ```

- **Max Chunks**: 1,024 chunks per list
  ```c
  #define chunk_list_CAP 1024
  ```

## 🐛 Known Issues & TODOs

- [ ] `heap_collect()` is not yet implemented (automatic chunk merging)
- [ ] Bug in `chunk_list_remove()` - incorrect loop bounds (should start at `index`, not 0)
- [ ] Pointer arithmetic issue in `heap_alloc()` - should use `(char *)` instead of `(int *)`
- [ ] The allocator returns after finding first chunk without checking if allocation succeeded

## 🔧 Potential Improvements

1. **Implement heap_collect()**: Automatically merge adjacent free chunks
2. **Better Fit Algorithms**: Implement best-fit or worst-fit strategies
3. **Alignment**: Add proper memory alignment support
4. **Coalescing**: Immediate merging when freeing memory
5. **Error Handling**: More robust error checking and reporting
6. **Thread Safety**: Add mutex locks for multi-threaded environments

## 📚 Learning Resources

For a deeper understanding of garbage collection and memory management, check out these blog posts:

- [Building a Garbage Collector from Scratch - Part 1](https://midoxmax.hashnode.dev/building-a-garbage-collector-from-scratch-a-deep-dive-into-memory-management)
- [Building a Garbage Collector from Scratch - Part 2](https://midoxmax.hashnode.dev/building-a-garbage-collector-from-scratch-part2)

## 📖 Concepts Covered

- **Memory Management**: Manual memory allocation and deallocation
- **Data Structures**: Dynamic arrays, sorted lists
- **Algorithms**: Binary search, insertion sort, first-fit allocation
- **Systems Programming**: Low-level memory operations
- **Pointer Arithmetic**: Working with memory addresses

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Fix bugs
- Implement missing features
- Improve documentation
- Add tests

## 📄 License

This project is open source and available for educational purposes.

## 👨‍💻 Author

**Mido** - [GitHub](https://github.com/Mido191020)

---

⭐ If you found this project helpful, please consider giving it a star!

## 🔍 Understanding the Code

### Memory Visualization

When you allocate 18 bytes from a 640KB heap:

```
Before Allocation:
┌────────────────────────────────────────┐
│         Free: 640000 bytes             │
└────────────────────────────────────────┘

After Allocation:
┌──────────┬─────────────────────────────┐
│ Alloc:18 │    Free: 639982 bytes       │
└──────────┴─────────────────────────────┘

After Free:
┌────────────────────────────────────────┐
│         Free: 640000 bytes             │
│          (chunks merged)               │
└────────────────────────────────────────┘
```

### Why Sorted Lists?

Keeping chunks sorted by start address enables:
1. **Fast Search**: Binary search in O(log n)
2. **Easy Merging**: Adjacent chunks are next to each other in the list
3. **Fragmentation Detection**: Easy to identify memory gaps

---

*Built with ❤️ for learning systems programming*
