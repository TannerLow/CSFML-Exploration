#pragma once
#include <o2/types.h>
#include <stdbool.h>

typedef struct o2_Rect {
	float32 x;
	float32 y;
	float32 w;
	float32 h;
} o2_Rect;

typedef struct o2_Point {
	float32 x;
	float32 y;
} o2_Point;

bool o2_geometry_rectContainsPoint(o2_Rect rect, o2_Point point);

