#include "SDL2/SDL_image.h"

#include "error.h"
#include "util.h"

#include <limits.h>

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
	SDL_FreeSurface(surf);
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
		LOG_ERROR(IMG_GetError());
		return NULL;
	}

	ret = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (!ret) {
		LOG_ERROR(SDL_GetError());
		return NULL;
	}

	return ret;
}

size_t util_next_pow_2(size_t n)
{
	size_t bits = sizeof(n) * CHAR_BIT;
	if (!n) return 1;
	if (sizeof(size_t) == sizeof(unsigned)) {
		return 1U << (bits - __builtin_clz(n));
	} else if (sizeof(size_t) == sizeof(long unsigned)) {
		return 1UL << (bits - __builtin_clzl(n));
	}
	ASSERT(UNREACHABLE);
	return 0;
}

size_t util_trailing_zeros(size_t n)
{
	if (sizeof(size_t) == sizeof(unsigned)) {
		return __builtin_ctz(n);
	} else if (sizeof(size_t) == sizeof(long unsigned)) {
		return __builtin_ctzl(n);
	}
	ASSERT(UNREACHABLE);
	return 0;
}
