#include <o2/geometry.h>

bool o2_geometry_rectContainsPoint(o2_Rect rect, o2_Point point) {
	return
		(rect.x <= point.x and point.x <= rect.x + rect.w) and
		(rect.y <= point.y and point.y <= rect.y + rect.h);
}
