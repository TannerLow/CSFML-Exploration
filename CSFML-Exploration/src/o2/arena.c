#include <o2/arena.h>
#include <assert.h>


void* o2_arena_push(o2_Arena* arena, size_t size) {
    assert(arena->used + size <= arena->size);

    void* start = (void*)(arena->base + arena->used);
    arena->used += size;

    return start;
}

void* o2_arena_pushArray(o2_Arena* arena, size_t sizePerElement, size_t count) {
    size_t size = sizePerElement * count;
    
    assert(arena->used + size <= arena->size);

    return o2_arena_push(arena, size);
}

void* o2_arena_pushAligned(o2_Arena* arena, size_t size, size_t alignment) {
    uintptr_t currentAddress = (uintptr_t)arena->base + arena->used;

    // bit manipulation to get next aligned address
    // ex.     addr=001011, alignment=8
    // mask:        000111
    //              010010, add mask to produce number at least bigger than addr + alignment
    //              111000, and with inverse mask to chop off extra bits
    // aligned addr=010000, divisible by alignment
    uintptr_t mask = alignment - 1;
    uintptr_t alignedAddress = (currentAddress + mask) & (~mask);
    size_t totalSize = alignedAddress + size - currentAddress;

    assert(arena->used + totalSize <= arena->size);

    arena->used += totalSize;

    return (void*)(alignedAddress);
}

void* o2_arena_pushArrayAligned(o2_Arena* arena, size_t sizePerElement, size_t count, size_t alignment) {
    size_t size = sizePerElement * count;

    // assert doesnt factor in alignment offset, another assert expected to be called in helper function
    assert(arena->used + size <= arena->size);

    return o2_arena_pushAligned(arena, size, alignment);
}
