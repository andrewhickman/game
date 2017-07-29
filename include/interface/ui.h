#ifndef INTERFACE_UI_H
#define INTERFACE_UI_H

#include "SDL2/SDL.h"

#include "interface/units.h"
#include "interface/main.h"
#include "interface/map.h"
#include "error.h"
#include "gamestate/gs.h"

struct ui {
	struct ui_units units;
	struct ui_main main;
	struct ui_map map;
};

struct ui_result {
	enum result result;
	struct ui value;
} ui_create(SDL_Renderer *renderer, int w, int h);

enum result ui_draw(struct ui *, SDL_Renderer *, struct gs const *);

enum ui_status {
	UI_CONTINUE, UI_QUIT, UI_ERR
} ui_handle_event(struct ui *ui, SDL_Event *event, struct gs *);

void ui_resize(struct ui *, int w, int h);

void ui_destroy(struct ui);

#endif