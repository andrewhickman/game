#ifndef GAMESTATE_CPNT_H
#define GAMESTATE_CPNT_H

#include "error.h"
#include "set.h"

#include <stddef.h>

struct gs_cpnt {
	struct set entities;	
	void *data;
};

struct gs_cpnt_result {
	enum result result;
	struct gs_cpnt value;
} gs_cpnt_create(size_t len, size_t size);
enum result gs_cpnt_insert(struct gs_cpnt *, unsigned id, size_t size);
void gs_cpnt_destroy(struct gs_cpnt);

/* Components */

struct gs_pos {
	int x, y;
};

struct gs_vel {
	int x, y;
};

struct gs_draw {
	struct SDL_Texture *texture;
	int w, h;
};

#endif