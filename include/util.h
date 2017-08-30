#ifndef UTIL_H
#define UTIL_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <stddef.h>

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

size_t util_next_pow_2(size_t n);
size_t util_trailing_zeros(size_t n);

#endif