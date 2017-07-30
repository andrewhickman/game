#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDL2/SDL.h"

#include "error.h"

enum texture {
	TEXTURE_UNIT_PANEL,
	TEXTURE_MAIN_PANEL,
	TEXTURE_MAP_PANEL,
	TEXTURE_UNIT,
	TEXTURE_COUNT
};

enum result texture_create(SDL_Renderer *);

SDL_Texture *texture_get(enum texture);

void texture_destroy(void);

#endif