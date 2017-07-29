#ifndef INTERFACE_UNITS_H
#define INTERFACE_UNITS_H

#include "SDL2/SDL.h"

#include "error.h"
#include "gamestate/gs.h"

struct ui_units {
	SDL_Rect pos;
	SDL_Texture *texture;
};

struct ui_units_result {
	enum result result;
	struct ui_units value;
} ui_units_create(SDL_Renderer *renderer);

enum result ui_units_draw(
	struct ui_units *panel, 
	SDL_Renderer *renderer, 
	struct gs const *gs
);

void ui_units_destroy(struct ui_units);

#endif