#include <o2/math.h>


void o2_math_movingAverageInit(MovingAverage_int32* collection, int32* buffer, size_t capacity) {
    collection->values = buffer;
    collection->capacity = capacity;
    collection->sum = 0;
    collection->count = 0;
    collection->index = 0;
}

int32 o2_math_movingAverageAdd(MovingAverage_int32* collection, int32 value) {
    if (collection->count == collection->capacity) {
        collection->sum -= collection->values[collection->index];
    }
    else {
        collection->count++;
    }

    collection->values[collection->index] = value;
    collection->sum += value;

    collection->index = (collection->index + 1) % collection->capacity;

    return (int32)(collection->sum / collection->count);
}
