#include "win.h"
#include "resrc.h"
#include "sys.h"

struct win_result win_create(void)
{
	struct win_result ret;

	ret.value.window = SDL_CreateWindow(
		"game", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE 
	);
	if (!ret.value.window) {
		LOG_ERROR(SDL_GetError());
		goto fail_window;
	}

	SDL_SetWindowMinimumSize(ret.value.window, 480, 320);

	ret.value.renderer = SDL_CreateRenderer(
		ret.value.window,
		-1,
		SDL_RENDERER_ACCELERATED | 
		SDL_RENDERER_TARGETTEXTURE | 
		SDL_RENDERER_PRESENTVSYNC
	);
	if (!ret.value.renderer) {
		LOG_ERROR(SDL_GetError());
		goto fail_renderer;
	}

	if (font_create() == RESULT_ERR) {
		LOG_CHAIN();
		goto fail_font;
	}

	if (texture_create(ret.value.renderer) == RESULT_ERR) {
		LOG_CHAIN();
		goto fail_texture;
	}
	 
	{
		struct ui_result ui = ui_create(ret.value.renderer, 640, 480);
		if (ui.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_ui;
		}
		ret.value.ui = ui.ui;
		ret.value.gs = ui.gs;
	}

	ret.result = RESULT_OK;
	return ret;

fail_ui:
	texture_destroy();
fail_texture:
	font_destroy();
fail_font:
	SDL_DestroyRenderer(ret.value.renderer);
fail_renderer:
	SDL_DestroyWindow(ret.value.window);
fail_window:
	ret.result = RESULT_ERR;
	return ret;
}

enum win_status win_update(struct win *win)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT) {
		   	if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				ui_resize(
					&win->ui, 
					event.window.data1, 
					event.window.data2
				);
			}
		} else {
			switch (ui_handle_event(&win->ui, &event, &win->gs)) {
			case UI_CONTINUE:
				continue;
			case UI_ERR:
				LOG_CHAIN();
				return WIN_ERR;
			case UI_QUIT:
				return WIN_QUIT;
			}
		}
	}

	ui_update(&win->ui, &win->gs);

	if (SDL_SetRenderDrawColor(win->renderer, 0, 0, 0, 255)) {
		LOG_ERROR(SDL_GetError());
		return WIN_ERR;
	}
	if (SDL_RenderClear(win->renderer)) {
		LOG_ERROR(SDL_GetError());
		return WIN_ERR;
	}
	if (sys_update(&win->gs) == RESULT_ERR) {
		LOG_CHAIN();
		return WIN_ERR;
	}
	if (ui_draw(&win->ui, win->renderer, &win->gs) == RESULT_ERR) {
		LOG_CHAIN();
		return WIN_ERR;
	}
	SDL_RenderPresent(win->renderer);

	return WIN_CONTINUE;
}

void win_destroy(struct win win)
{
	gs_destroy(win.gs);
	ui_destroy(win.ui);
	texture_destroy();
	font_destroy();
	SDL_DestroyRenderer(win.renderer);
	SDL_DestroyWindow(win.window);
}