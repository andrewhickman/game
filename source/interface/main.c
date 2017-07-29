#include "interface/main.h"
#include "util.h"

struct ui_main_result ui_main_create(SDL_Renderer *renderer, int w, int h)
{
	struct ui_main_result ret;

	ret.value.pos.x = w / 2 - 160;
	ret.value.pos.y = h - 160;
	ret.value.pos.w = 320;
	ret.value.pos.h = 160;

	ret.value.texture = util_render_image(renderer, "image/main_panel.png");
	if (!ret.value.texture) {
		ret.result = RESULT_ERR;
		return ret;
	}

	ret.result = RESULT_OK;
	return ret;
}

enum result ui_main_draw(
	struct ui_main *panel,
	SDL_Renderer *renderer,
	struct gs const *gs
) {
	SDL_Rect const bound = { 0, 0, 320, 160 };
	SDL_Rect const something = { 7, 7, 306, 146 };

	SDL_RenderSetViewport(renderer, &panel->pos);
	SDL_RenderCopy(renderer, panel->texture, NULL, &bound);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &something);
	
	(void)gs;
	return RESULT_OK;
}


void ui_main_destroy(struct ui_main panel)
{
	SDL_DestroyTexture(panel.texture);
}