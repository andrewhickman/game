#include "sys.h"

void sys_physics(
	struct cpnt_pos *pos, 
	struct cpnt_vel const *vel
) {
	pos->x += vel->x;
	pos->y += vel->y;
}

enum result sys_draw(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_select const *select, /* optional */
	SDL_Renderer *renderer
) {
	int err;
	SDL_Rect rect;

	rect.x = pos->x - (rect.w = draw->w) / 2;
	rect.y = pos->y - (rect.h = draw->h) / 2;

	err = SDL_RenderCopy(renderer, draw->texture, NULL, &rect);
	if (err) { LOG_ERROR(SDL_GetError()); return RESULT_ERR; }
	if (select) {
		switch (select->state) {
		case CPNT_SELECT_SELECTED:
			err = SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			err = SDL_RenderDrawRect(renderer, &rect);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			break;
		case CPNT_SELECT_HOVERED:
			err = SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			err = SDL_RenderDrawRect(renderer, &rect);
			if (err) { 
				LOG_ERROR(SDL_GetError()); 
				return RESULT_ERR; 
			}
			break;
		case CPNT_SELECT_NONE:
			break;
		}
	}
	return RESULT_OK;
}