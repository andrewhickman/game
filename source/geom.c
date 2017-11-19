#include "geom.h"

#include <math.h>

bool geom_circle_line_collide(struct vec pos, struct vec dir, int r)
{
	long long a, b, c;
	double discr, t;

	a = vec_dot(dir, dir);
	b = 2 * vec_dot(pos, dir);
	c = vec_dot(pos, pos) - r * r;

	discr = b * b - 4 * a * c;
	if (discr <= 0.0) return false;
	discr = sqrt(discr);

	t = (-b - discr) / (2.0 * a);
	if (0.0 <= t && t <= 1.0) return true;

	t = (-b + discr) / (2.0 * a);
	if (0.0 <= t && t <= 1.0) return true;

	return false;
}

long long absll(long long n)
{
	return n < 0 ? -n : n;
}

bool proper(long long n, long long d)
{
	return (n < 0) == (d < 0) && absll(n) < absll(d);
}

bool geom_line_line_collide(struct vec dir1, struct vec pos2, struct vec dir2)
{
	long long n, d = vec_wedge(dir1, dir2);

	n = vec_wedge(pos2, dir1);
	if (!proper(n, d)) return false;

	n = vec_wedge(pos2, dir2);
	if (!proper(n, d)) return false;

	return true;
}

bool geom_rect_line_collide(struct vec pos2, struct vec dir2, int w, int h)
{
	struct vec dir1;

	dir1.x = 0;
	dir1.y = h;
	if (geom_line_line_collide(dir1, pos2, dir2)) return true;
	pos2 = vec_sub(pos2, dir1);

	dir1.x = w;
	dir1.y = 0;
	if (geom_line_line_collide(dir1, pos2, dir2)) return true;
	pos2 = vec_sub(pos2, dir1);

	dir1.x = 0;
	dir1.y = -h;
	if (geom_line_line_collide(dir1, pos2, dir2)) return true;
	pos2 = vec_sub(pos2, dir1);

	dir1.x = -w;
	dir1.y = 0;
	if (geom_line_line_collide(dir1, pos2, dir2)) return true;

	return false;
}

bool geom_circle_point_collide(struct vec pos, int r)
{
	return vec_dot(pos, pos) <= r * r;
}

bool geom_rect_point_collide(struct vec p, int w, int h)
{
	return 0 <= p.x && p.x <= w && 0 <= p.y && p.y <= h;
}