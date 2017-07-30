#include "texture.h"
#include "util.h"

SDL_Texture *textures[TEXTURE_COUNT];

char const *texture_paths[TEXTURE_COUNT] = {
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