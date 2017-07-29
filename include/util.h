#ifndef UTIL_H
#define UTIL_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

SDL_Texture *util_render_text(
	SDL_Renderer *renderer, 
	TTF_Font *font, 
	char const *text, 
	SDL_Color fg
);

SDL_Texture *util_render_image(
	SDL_Renderer *renderer, 
	char const *path
);

#endif