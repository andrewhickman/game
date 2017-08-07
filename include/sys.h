#ifndef SYS_H
#define SYS_H

#include "SDL2/SDL.h"

#include "cpnt.h"
#include "error.h"

void sys_physics(
	struct cpnt_pos *, 
	struct cpnt_vel const *
);

enum result sys_draw(
	struct cpnt_pos const *, 
	struct cpnt_draw const *, 
	struct cpnt_select const *, /* optional */
	SDL_Renderer *
);

#endif