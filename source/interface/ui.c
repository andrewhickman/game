#include "interface/ui.h"

struct ui_result ui_create(SDL_Renderer *renderer, int w, int h)
{
	struct ui_result ret;

	{
		struct ui_units_result units = ui_units_create(renderer);
		if (units.result == RESULT_ERR) goto fail_units;
		ret.value.units = units.value;
	}
	{
		struct ui_main_result main_ = ui_main_create(renderer, w, h);
		if (main_.result == RESULT_ERR) goto fail_main;
		ret.value.main = main_.value;
	}
	{
		struct ui_map_result map = ui_map_create(renderer, w);
		if (map.result == RESULT_ERR) goto fail_map;
		ret.value.map = map.value;
	}

	ret.result = RESULT_OK;
	return ret;

fail_map:
	ui_main_destroy(ret.value.main);
fail_main:
	ui_units_destroy(ret.value.units);
fail_units:
	ret.result = RESULT_ERR;
	return ret;
}

enum result ui_draw(struct ui *ui, SDL_Renderer *renderer, struct gs const *gs)
{
	if (ui_units_draw(&ui->units, renderer, gs)) {
		return RESULT_ERR;
	}
	if (ui_main_draw(&ui->main, renderer, gs)) {
		return RESULT_ERR;
	}
	if (ui_map_draw(&ui->map, renderer, gs)) {
		return RESULT_ERR;
	}

	return RESULT_OK;
}

enum ui_status ui_handle_event(struct ui *ui, SDL_Event *event, struct gs *gs)
{
	(void)ui, (void)gs;

	switch (event->type) {
	case SDL_QUIT:
		return UI_QUIT;
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
	ui_map_destroy(ui.map);
	ui_main_destroy(ui.main);
	ui_units_destroy(ui.units);
}