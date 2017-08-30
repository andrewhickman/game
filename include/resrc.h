#ifndef RESRC_H 
#define RESRC_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

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

enum font {
	FONT_VERA,
	FONT_COUNT
};

enum result font_create();
TTF_Font *font_get(enum font);
void font_destroy(void);

#endif