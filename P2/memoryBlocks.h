#ifndef MEMORY_BLOCKS_H
#define MEMORY_BLOCKS_H

#include "includes.h"

typedef enum
{MALLOC = 1, SHARED = 2, MMAP = 3} MemoryBlockType;

typedef struct malloc_block {
    MemoryBlockType type;
    void *address;
    size_t size;
    time_t timestamp;
} malloc_block;

typedef struct shared_block {
    MemoryBlockType type;
    void *address;
    size_t size;
    time_t timestamp;
    key_t key;
    int shmid;
} shared_block;

typedef struct mmap_block {
    MemoryBlockType type;
    void *address;
    size_t size;
    time_t timestamp;
    char *filename;
    int df;
} mmap_block;

#endif // MEMORY_BLOCKS_H

