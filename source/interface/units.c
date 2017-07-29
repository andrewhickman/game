#include "interface/units.h"
#include "util.h"

struct ui_units_result ui_units_create(SDL_Renderer *renderer)
{
	struct ui_units_result ret;

	ret.value.pos.x = 0;
	ret.value.pos.y = 0;
	ret.value.pos.w = 160;
	ret.value.pos.h = 320;

	ret.value.texture = util_render_image(renderer, "image/unit_panel.png");
	if (!ret.value.texture) {
		ret.result = RESULT_ERR;
		return ret;
	}

	ret.result = RESULT_OK;
	return ret;
}

enum result ui_units_draw(
	struct ui_units *panel, 
	SDL_Renderer *renderer, 
	struct gs const *gs
) {
	int i;
	SDL_Rect bound = { 0, 0, 160, 80 };
	SDL_Rect image = { 11, 11, 58, 58 };
	SDL_Rect hp = { 77, 44, 72, 25 };

	SDL_RenderSetViewport(renderer, &panel->pos);
	for (i = 0; i < 4; ++i) {
		if (SDL_RenderCopy(renderer, panel->texture, NULL, &bound)) {
			return RESULT_ERR;
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &image);
		SDL_RenderFillRect(renderer, &hp);

		bound.y += bound.h;
		image.y += bound.h;
		hp.y += bound.h;
	}

	(void)gs;
	return RESULT_OK;
}

void ui_units_destroy(struct ui_units panel)
{
	SDL_DestroyTexture(panel.texture);
}
