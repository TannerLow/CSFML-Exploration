#pragma once
#include <o2/types.h>

typedef struct MovingAverage_int32 {
    int32* values;
    int32 sum;
    size_t capacity;
    size_t count;
    size_t index;
} MovingAverage_int32;


void o2_math_movingAverageInit(MovingAverage_int32* collection, int32* buffer, size_t capacity);
int32 o2_math_movingAverageAdd(MovingAverage_int32* collection, int32 value);
