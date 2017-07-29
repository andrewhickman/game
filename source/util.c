#include "SDL2/SDL_image.h"

#include "error.h"
#include "util.h"

SDL_Texture *util_render_text(
	SDL_Renderer *renderer, 
	TTF_Font *font, 
	char const *text, 
	SDL_Color fg
) {
	SDL_Texture *ret;
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, fg);
	if (!surf) {
		LOG_ERROR(TTF_GetError());
		return NULL;
	}

	ret = SDL_CreateTextureFromSurface(renderer, surf);
	if (!ret) {
		LOG_ERROR(SDL_GetError());
		return NULL;
	}

	return ret;
}

SDL_Texture *util_render_image(
	SDL_Renderer *renderer, 
	char const *path
) {
	SDL_Texture *ret;
	SDL_Surface *surf = IMG_Load(path);
	if (!surf) {
		LOG_ERROR(TTF_GetError());
		return NULL;
	}

	ret = SDL_CreateTextureFromSurface(renderer, surf);
	if (!ret) {
		LOG_ERROR(SDL_GetError());
		return NULL;
	}

	return ret;
}