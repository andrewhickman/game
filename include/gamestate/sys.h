#ifndef GAMESTATE_SYS_H
#define GAMESTATE_SYS_H

#include "SDL2/SDL.h"

#include "gamestate/cpnt.h"

enum result gs_sys_physics(
	unsigned id,
	struct gs_pos *, 
	struct gs_vel const *
);

enum result gs_sys_draw(
	unsigned id,
	struct gs_pos const *, 
	struct gs_draw const *, 
	SDL_Renderer *
);

#endif