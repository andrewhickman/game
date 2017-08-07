#ifndef CPNT_H
#define CPNT_H

#include <stdbool.h>
#include <stddef.h>

/*
  path
   | \
   |  selectable
   |     /
  vel   draw
     \ /
     pos
*/

enum cpnt {
	CPNT_NONE = 0,
	CPNT_POS = 1 << 0,
	CPNT_VEL = 1 << 1,
	CPNT_DRAW = 1 << 2,
	CPNT_SELECT = 1 << 3
};

/* Components */

struct cpnt_pos {
	float x, y;
};

struct cpnt_vel {
	float x, y;
};

struct cpnt_draw {
	struct SDL_Texture *texture;
	int w, h;
};

struct cpnt_select {
	enum gs_select_state {
		CPNT_SELECT_NONE,	
		CPNT_SELECT_HOVERED,
		CPNT_SELECT_SELECTED
	} state;
	bool (*collides)(int x, int y, int w, int h);
};

/*
struct cpnt_path {

};
*/

#endif