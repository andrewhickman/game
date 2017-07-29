#include "win.h"

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
	 
	{
		struct ui_result ui = ui_create(ret.value.renderer, 640, 480);
		if (ui.result == RESULT_ERR) {
			goto fail_ui;
		}
		ret.value.ui = ui.value;
	}

	{
		struct gs_result gs = gs_create();
		if (gs.result == RESULT_ERR) {
			goto fail_gs;
		}
		ret.value.gs = gs.value;
	}

	ret.result = RESULT_OK;
	return ret;

fail_gs:
	ui_destroy(ret.value.ui);
fail_ui:
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
				return WIN_ERR;
			case UI_QUIT:
				return WIN_QUIT;
			}
		}
	}

	gs_update(&win->gs);

	SDL_SetRenderDrawColor(win->renderer, 0, 0, 0, 255);
	SDL_RenderClear(win->renderer);
	ui_draw(&win->ui, win->renderer, &win->gs);
	SDL_RenderPresent(win->renderer);

	return WIN_CONTINUE;
}

void win_destroy(struct win win)
{
	gs_destroy(win.gs);
	ui_destroy(win.ui);
	SDL_DestroyRenderer(win.renderer);
	SDL_DestroyWindow(win.window);
}