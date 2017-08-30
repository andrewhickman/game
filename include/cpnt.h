#ifndef CPNT_H
#define CPNT_H

#include "SDL2/SDL.h"

#include "vec.h"

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
	CPNT_SELECT = 1 << 4,
	CPNT_HP = 1 << 5
};

/* Component data. */

#define cpnt_pos vec

#define cpnt_vel vec

struct cpnt_draw {
	SDL_Texture *texture;
	int w, h;
};

struct cpnt_coll {
	enum cpnt_coll_shape {
		CPNT_COLL_ELLIPSE,
		CPNT_COLL_RECT
	} shape;
};

struct cpnt_select {
	enum cpnt_select_state {
		CPNT_SELECT_NONE,	
		CPNT_SELECT_HOVERED,
		CPNT_SELECT_SELECTED
	} state;
};

struct cpnt_hp {
	int health;
};

/*
struct cpnt_path {

};
*/

#endif