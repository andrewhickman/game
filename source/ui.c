#include "ui.h"
#include "texture.h"

struct ui_result ui_create(SDL_Renderer *renderer, int w, int h) {
	struct ui_result ret;
	int i;

	(void)renderer;

	ret.ui.units.pos.x = 0;
	ret.ui.units.pos.y = 0;
	ret.ui.units.pos.w = 160;
	ret.ui.units.pos.h = 320;
	ret.ui.main.pos.x = w / 2 - 160;
	ret.ui.main.pos.y = h - 160;
	ret.ui.main.pos.w = 320;
	ret.ui.main.pos.h = 160;
	ret.ui.map.pos.x = w - 160;
	ret.ui.map.pos.y = 0;
	ret.ui.map.pos.w = 160;
	ret.ui.map.pos.h = 160;

	ret.ui.units.texture = texture_get(TEXTURE_UNIT_PANEL);
	ret.ui.main.texture = texture_get(TEXTURE_MAIN_PANEL);
	ret.ui.map.texture = texture_get(TEXTURE_MAP_PANEL);

	{
		struct gs_result gs = gs_create();
		if (gs.result == RESULT_ERR) {
			LOG_CHAIN();
			ret.result = RESULT_ERR;
			return ret;
		}
		for (i = 0; i < 4; ++i) ret.ui.units.ents[i] = gs.ents[i];
		ret.gs = gs.value;
	}

	ret.result = RESULT_OK;
	return ret;
}

enum result ui_draw(struct ui *ui, SDL_Renderer *renderer, struct gs const *gs)
{
	int err;

	{
		int i;
		SDL_Rect bound = { 0, 0, 160, 80 };
		SDL_Rect image = { 11, 11, 58, 58 };
		SDL_Rect hp = { 77, 44, 72, 25 };

		err = SDL_RenderSetViewport(renderer, &ui->units.pos);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		for (i = 0; i < 4; ++i) {
			struct gs_query_result query = gs_query(gs, ui->units.ents[i]);
			if (!query.found) {
				LOG_ERROR("bad entity id");
				return RESULT_ERR;
			}

			err = SDL_RenderCopy(renderer, ui->units.texture, 
			                     NULL, &bound);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			err = SDL_SetRenderDrawColor(renderer, 255, 255, 255, 
			                             255);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			err = SDL_RenderFillRect(renderer, &image);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}

			hp.w = 72 * query.health / 100;

			err = SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			err = SDL_RenderFillRect(renderer, &hp);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}

			bound.y += bound.h;
			image.y += bound.h;
			hp.y += bound.h;
		}
	}

	{
		SDL_Rect const bound = { 0, 0, 320, 160 };
		SDL_Rect const something = { 7, 7, 306, 146 };

		err = SDL_RenderSetViewport(renderer, &ui->main.pos);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_RenderCopy(renderer, ui->main.texture, NULL, &bound);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_RenderFillRect(renderer, &something);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
	}

	{
		SDL_Rect const bound = { 0, 0, 160, 160 };
		SDL_Rect const something = { 7, 7, 146, 146 };

		err = SDL_RenderSetViewport(renderer, &ui->map.pos);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_RenderCopy(renderer, ui->map.texture, NULL, &bound);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
		err = SDL_RenderFillRect(renderer, &something);
		if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
	}

	err = SDL_RenderSetViewport(renderer, NULL);
	if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }

	return RESULT_OK;
}

enum ui_status ui_handle_event(struct ui *ui, SDL_Event *event, struct gs *gs)
{
	switch (event->type) {
	case SDL_QUIT:
		return UI_QUIT;
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT) {
			ui->selected = gs_select(gs, event->motion.x, event->motion.y);
			return UI_CONTINUE;
		} else if (event->button.button == SDL_BUTTON_RIGHT) {
			if (gs_new_unit(gs, event->button.x, event->button.y, TEXTURE_UNIT).result == RESULT_ERR) {
				LOG_CHAIN();
				return UI_ERR;
			}
		}
	case SDL_MOUSEMOTION:
		gs_hover(gs, event->motion.x, event->motion.y);
		return UI_CONTINUE;
	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_DELETE && ui->selected.found) {
			ent_store_kill(&gs->entities, ui->selected.ent);
			ui->selected.found = false;
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