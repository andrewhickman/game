#ifndef WIN_H
#define WIN_H

#include "SDL2/SDL.h"

#include "error.h"
#include "ui.h"
#include "gs.h"

/* Controls the renderering context and events. */
struct win {
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct ui ui;
	struct gs gs;
};

/* Trys to create a new window. */
struct win_result {
	enum result result;
	struct win value;
} win_create(void);

/* Updates a window. */
enum win_status {
	WIN_CONTINUE, WIN_QUIT, WIN_ERR	
} win_update(struct win *);

/* Destroys a window. */
void win_destroy(struct win);

#endif