#include "interface/ui.h"
#include "texture.h"

struct ui_result ui_create(SDL_Renderer *renderer, int w, int h)
{
	struct ui_result ret;

	(void)renderer;

	ret.value.units.pos.x = 0;
	ret.value.units.pos.y = 0;
	ret.value.units.pos.w = 160;
	ret.value.units.pos.h = 320;
	ret.value.main.pos.x = w / 2 - 160;
	ret.value.main.pos.y = h - 160;
	ret.value.main.pos.w = 320;
	ret.value.main.pos.h = 160;
	ret.value.map.pos.x = w - 160;
	ret.value.map.pos.y = 0;
	ret.value.map.pos.w = 160;
	ret.value.map.pos.h = 160;

	ret.value.units.texture = texture_get(TEXTURE_UNIT_PANEL);
	ret.value.main.texture = texture_get(TEXTURE_MAIN_PANEL);
	ret.value.map.texture = texture_get(TEXTURE_MAP_PANEL);

	ret.result = RESULT_OK;
	return ret;
}

enum result ui_draw(struct ui *ui, SDL_Renderer *renderer, struct gs const *gs)
{
	(void)gs;
	{
		int i;
		SDL_Rect bound = { 0, 0, 160, 80 };
		SDL_Rect image = { 11, 11, 58, 58 };
		SDL_Rect hp = { 77, 44, 72, 25 };

		SDL_RenderSetViewport(renderer, &ui->units.pos);
		SDL_RenderCopy(renderer, ui->main.texture, NULL, &bound);
		for (i = 0; i < 4; ++i) {
			if (SDL_RenderCopy(renderer, ui->units.texture, NULL, &bound)) {
				return RESULT_ERR;
			}
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &image);
			SDL_RenderFillRect(renderer, &hp);

			bound.y += bound.h;
			image.y += bound.h;
			hp.y += bound.h;
		}
	}

	{
		SDL_Rect const bound = { 0, 0, 320, 160 };
		SDL_Rect const something = { 7, 7, 306, 146 };

		SDL_RenderSetViewport(renderer, &ui->main.pos);
		SDL_RenderCopy(renderer, ui->main.texture, NULL, &bound);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &something);
	}

	{
		SDL_Rect const bound = { 0, 0, 160, 160 };
		SDL_Rect const something = { 7, 7, 146, 146 };

		SDL_RenderSetViewport(renderer, &ui->map.pos);
		SDL_RenderCopy(renderer, ui->map.texture, NULL, &bound);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &something);
	}

	SDL_RenderSetViewport(renderer, NULL);

	return RESULT_OK;
}

enum ui_status ui_handle_event(struct ui *ui, SDL_Event *event, struct gs *gs)
{
	(void)ui;

	switch (event->type) {
	case SDL_QUIT:
		return UI_QUIT;
	case SDL_MOUSEBUTTONUP:
		if (event->button.button == SDL_BUTTON_LEFT) {
			gs_new_unit(gs, event->button.x, event->button.y, TEXTURE_UNIT);
		}
		return UI_CONTINUE;
	default:
		return UI_CONTINUE;
	}
}

void ui_resize(struct ui *ui, int w, int h)
{
	ui->main.pos.x = h < 480 ? w - 320 : w / 2 - 160;
	ui->main.pos.y = h - 160;
	ui->map.pos.x = w - 160;
}

void ui_destroy(struct ui ui)
{
	(void)ui;
}