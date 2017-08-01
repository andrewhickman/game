#ifndef GAMESTATE_GS_H
#define GAMESTATE_GS_H

#include "SDL2/SDL.h"

#include "gamestate/cpnt.h"
#include "gamestate/entity.h"
#include "error.h"
#include "texture.h"
#include "set.h"

struct gs {
	struct gs_entities entities;
	struct gs_cpnt_sparse pos, vel, draw;
};

struct gs_result {
	enum result result;
	struct gs value;
} gs_create(void);

enum result gs_update(struct gs *, SDL_Renderer *);

enum result gs_new_unit(struct gs *, int x, int y, enum texture);

void gs_destroy(struct gs);

#endif