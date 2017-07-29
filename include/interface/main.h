#ifndef INTERFACE_MAIN_H
#define INTERFACE_MAIN_H

#include "SDL2/SDL.h"

#include "error.h"
#include "gamestate/gs.h"

struct ui_main {
	SDL_Rect pos;
	SDL_Texture *texture;
};

struct ui_main_result {
	enum result result;
	struct ui_main value;
} ui_main_create(SDL_Renderer *renderer, int w, int h);

enum result ui_main_draw(
	struct ui_main *panel,
	SDL_Renderer *renderer,
	struct gs const *gs
);

void ui_main_destroy(struct ui_main);

#endif