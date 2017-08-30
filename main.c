#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include "error.h"
#include "win.h"

#include <stdlib.h>

int main(int argc, char *argv[]) 
{
	int ret = EXIT_SUCCESS;
	struct win win;

	(void)argc, (void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		LOG_ERROR(SDL_GetError());
		ret = EXIT_FAILURE;
		goto fail_sdl;
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		LOG_ERROR(IMG_GetError());
		ret = EXIT_FAILURE;
		goto fail_img;
	}

	if (TTF_Init()) {
		LOG_ERROR(TTF_GetError());
		ret = EXIT_FAILURE;
		goto fail_ttf;
	}

	{
		struct win_result win_result = win_create();
		if (win_result.result == RESULT_ERR) {
			LOG_CHAIN();
			ret = EXIT_FAILURE;
			goto fail_win;
		}
		win = win_result.value;
	}

	while (1) {
		switch (win_update(&win)) {
		case WIN_CONTINUE:
			continue;
		case WIN_ERR:
			LOG_CHAIN();
			ret = EXIT_FAILURE;
		case WIN_QUIT:
			goto exit;
		}
	}

exit:
	win_destroy(win);
fail_win:
	TTF_Quit();
fail_ttf:
	IMG_Quit();
fail_img:
	SDL_Quit();
fail_sdl:
	if (ret) {
		SDL_Log("exiting unsuccessfully (exit code %d).", ret);
	} else {
		SDL_Log("exiting successfully.");
	}
	return ret;
}
