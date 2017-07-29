#include "interface/map.h"
#include "util.h"

struct ui_map_result ui_map_create(SDL_Renderer *renderer, int w)
{
	struct ui_map_result ret;

	ret.value.pos.x = 0;
	ret.value.pos.y = 0;
	ret.value.pos.w = w - 160;
	ret.value.pos.h = 160;

	ret.value.texture = util_render_image(renderer, "image/map_panel.png");
	if (!ret.value.texture) {
		ret.result = RESULT_ERR;
		return ret;
	}

	ret.result = RESULT_OK;
	return ret;
}

enum result ui_map_draw(
	struct ui_map *panel,
	SDL_Renderer *renderer,
	struct gs const *gs
) {
	SDL_Rect const bound = { 0, 0, 160, 160 };
	SDL_Rect const something = { 7, 7, 146, 146 };

	SDL_RenderSetViewport(renderer, &panel->pos);
	SDL_RenderCopy(renderer, panel->texture, NULL, &bound);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &something);
	
	(void)gs;
	return RESULT_OK;
}

void ui_map_destroy(struct ui_map panel)
{
	SDL_DestroyTexture(panel.texture);
}
