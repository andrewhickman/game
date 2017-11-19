#ifndef VEC_H
#define VEC_H

#include "SDL2/SDL.h"

#define vec SDL_Point

#define PI 3.14159265358979323846

struct vec vec_add(struct vec, struct vec);
struct vec vec_sub(struct vec, struct vec);
struct vec vec_perp(struct vec);
long long vec_dot(struct vec, struct vec);
long long vec_wedge(struct vec, struct vec);
double vec_dist(struct vec, struct vec);

#endif