#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define and &&
#define or ||
#define not !

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef float    float32;
typedef double   float64;

_Static_assert(sizeof(float32) == 4, "Expected float to be 32 bits");
_Static_assert(sizeof(float64) == 8, "Expected double to be 64 bits");

#define alignof(x) _Alignof(x)
