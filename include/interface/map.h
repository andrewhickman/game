#ifndef INTERFACE_MAP_H
#define INTERFACE_MAP_H

#include "SDL2/SDL.h"

#include "error.h"
#include "gamestate/gs.h"

struct ui_map {
	SDL_Rect pos;
	SDL_Texture *texture;
};

struct ui_map_result {
	enum result result;
	struct ui_map value;
} ui_map_create(SDL_Renderer *renderer, int w);

enum result ui_map_draw(
	struct ui_map *ui,
	SDL_Renderer *renderer,
	struct gs const *gs
);

void ui_map_destroy(struct ui_map);

#endif