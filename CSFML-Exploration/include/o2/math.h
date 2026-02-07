#pragma once
#include <o2/types.h>

typedef struct o2_MovingAverage_int32 {
    int32* values;
    int32 sum;
    size_t capacity;
    size_t count;
    size_t index;
} o2_MovingAverage_int32;


void o2_math_movingAverageInit(o2_MovingAverage_int32* collection, int32* buffer, size_t capacity);
int32 o2_math_movingAverageAdd(o2_MovingAverage_int32* collection, int32 value);
