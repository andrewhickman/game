#include "vec.h"

#include <math.h>

struct vec vec_add(struct vec lhs, struct vec rhs)
{
	struct vec ret;
	
	ret.x = lhs.x + rhs.x;
	ret.y = lhs.y + rhs.y;

	return ret;
}

struct vec vec_sub(struct vec lhs, struct vec rhs)
{
	struct vec ret;
	
	ret.x = lhs.x - rhs.x;
	ret.y = lhs.y - rhs.y;

	return ret;
}

struct vec vec_perp(struct vec vec)
{
	struct vec ret;
	
	ret.x = -vec.y;
	ret.y = vec.x;

	return ret;
}

long long vec_dot(struct vec lhs, struct vec rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

long long vec_wedge(struct vec lhs, struct vec rhs)
{
	return vec_dot(vec_perp(lhs), rhs);
}

double vec_dist(struct vec lhs, struct vec rhs)
{
	struct vec d = vec_sub(lhs, rhs);
	return hypot(d.x, d.y);
}