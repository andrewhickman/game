#ifndef CPNT_H
#define CPNT_H

#include "SDL2/SDL.h"

#include "vec.h"
#include "set.h"

#include <stdbool.h>
#include <stddef.h>

/*
       path    selectable
        |  \      /
        |   collidable
        |       /  
       vel   draw
         \ /      
         pos       hp
*/

/* A bit-mask of the different components an entity can have. */
enum cpnt {
	CPNT_NONE = 0,
	CPNT_POS = 1 << 0,
	CPNT_VEL = 1 << 1,
	CPNT_DRAW = 1 << 2,
	CPNT_COLL = 1 << 3,
	CPNT_HP = 1 << 4,
	CPNT_PATH = 1 << 5
};

/* Component data. */

#define cpnt_pos vec

#define cpnt_vel vec

struct cpnt_draw {
	SDL_Texture *texture;
	int w, h;
};

struct cpnt_coll {
	struct set nodes;
	unsigned offset;
	enum cpnt_coll_shape {
		CPNT_COLL_CIRCLE,
		CPNT_COLL_RECT
	} shape;
};

void cpnt_coll_destroy(struct cpnt_coll);

struct cpnt_hp {
	int health;
};

struct cpnt_path {
	struct vec *buf;
	size_t idx;
};

void cpnt_path_destroy(struct cpnt_path);

#endif