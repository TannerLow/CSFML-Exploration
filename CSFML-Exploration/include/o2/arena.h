#pragma once
#include <o2/types.h>

#define MEGABYTE 1024*1024

typedef struct Arena {
    uint8* base;
    size_t size;
    size_t used;
} Arena;

void* o2_arena_push(Arena* arena, size_t size);
void* o2_arena_pushArray(Arena* arena, size_t sizePerElement, size_t count);
void* o2_arena_pushAligned(Arena* arena, size_t size, size_t alignment);
void* o2_arena_pushArrayAligned(Arena* arena, size_t sizePerElement, size_t count, size_t alignment);
