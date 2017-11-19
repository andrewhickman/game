#ifndef UI_H
#define UI_H

#include "SDL2/SDL.h"

#include "error.h"
#include "gs.h"
#include "sys.h"

struct ui {
	struct ui_units {
		SDL_Rect pos;
		SDL_Texture *texture;
		struct ent ents[4];
	} units;
	struct ui_main {
		SDL_Rect pos;
		SDL_Texture *texture;
	} main;
	struct ui_map {
		SDL_Rect pos;
		SDL_Texture *texture;
	} map;
	struct sys_find_result hovered, selected;
	SDL_Rect camera;
	struct vec mouse;
	enum ui_path_state {
		UI_PATH_NONE,
		UI_PATH_START,
	} path_state;
	struct vec start;
	struct cpnt_path path;
};

struct ui_result {
	enum result result;
	struct ui ui;
	struct gs gs;
} ui_create(SDL_Renderer *, int w, int h);

enum result ui_draw(struct ui const *, SDL_Renderer *, struct gs const *);

enum ui_status {
	UI_CONTINUE, UI_QUIT, UI_ERR
} ui_handle_event(struct ui *ui, SDL_Event *event, struct gs *);

enum ui_status ui_update(struct ui *ui, struct gs *gs);

void ui_resize(struct ui *, int w, int h);

void ui_destroy(struct ui);

#endif