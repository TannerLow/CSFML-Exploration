#pragma once
#include <o2/types.h>

#define MEGABYTE 1024*1024

typedef struct o2_Arena {
    uint8* base;
    size_t size;
    size_t used;
} o2_Arena;

void* o2_arena_push(o2_Arena* arena, size_t size);
void* o2_arena_pushArray(o2_Arena* arena, size_t sizePerElement, size_t count);
void* o2_arena_pushAligned(o2_Arena* arena, size_t size, size_t alignment);
void* o2_arena_pushArrayAligned(o2_Arena* arena, size_t sizePerElement, size_t count, size_t alignment);
