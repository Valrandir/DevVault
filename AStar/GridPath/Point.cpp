#include "Path.h"

bool Path::Point::operator==(const Point& pt) const
{
	return x == pt.x && y == pt.y;
}

Path::Point& Path::Point::operator+=(const Point& pt)
{
	x += pt.x;
	y += pt.y;
	return *this;
}
