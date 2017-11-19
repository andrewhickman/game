#ifndef GEOM_H
#define GEOM_H

#include "vec.h"

#include <stdbool.h>

bool geom_circle_line_collide(struct vec pos, struct vec dir, int r);
bool geom_rect_line_collide(struct vec pos, struct vec dir, int w, int h);
bool geom_circle_point_collide(struct vec pos, int r);
bool geom_rect_point_collide(struct vec pos, int w, int h);

#endif