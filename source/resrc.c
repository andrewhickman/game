#include "resrc.h"
#include "util.h"

SDL_Texture *textures[TEXTURE_COUNT];

char const *const texture_paths[TEXTURE_COUNT] = {
	"image/unit_panel.png",
	"image/main_panel.png",
	"image/map_panel.png",
	"image/unit.png"
};

void texture_destroy_n(unsigned n);

enum result texture_create(SDL_Renderer *renderer)
{
	unsigned i;
	for (i = 0; i < TEXTURE_COUNT; ++i) {
		textures[i] = util_render_image(renderer, texture_paths[i]);
		if (!textures[i]) {
			LOG_CHAIN();
			texture_destroy_n(i);
			return RESULT_ERR;
		}
	}
	return RESULT_OK;
}

SDL_Texture *texture_get(enum texture n)
{
	return textures[n];
}

void texture_destroy(void)
{
	texture_destroy_n(TEXTURE_COUNT);
}

void texture_destroy_n(unsigned n) {
	while (n--) {
		SDL_DestroyTexture(textures[n]);
	}
}

TTF_Font *fonts[FONT_COUNT];

struct {
	char const *path;
	int size;
} const font_data[TEXTURE_COUNT] = {
	{ "font/Vera.ttf", 25 },
};

void font_destroy_n(unsigned n);

enum result font_create()
{
	unsigned i;
	for (i = 0; i < FONT_COUNT; ++i) {
		fonts[i] = TTF_OpenFont(font_data[i].path, font_data[i].size);
		if (!fonts[i]) {
			LOG_CHAIN();
			texture_destroy_n(i);
			return RESULT_ERR;
		}
	}
	return RESULT_OK;
}

TTF_Font *font_get(enum font n)
{
	return fonts[n];
}

void font_destroy(void)
{
	font_destroy_n(FONT_COUNT);
}

void font_destroy_n(unsigned n) {
	while (n--) {
		TTF_CloseFont(fonts[n]);
	}
}