#include "gamestate/sys.h"

enum result gs_sys_physics(
	struct gs_pos *pos, 
	struct gs_vel const *vel
) {
	pos->x += vel->x;
	pos->y += vel->y;
	return RESULT_OK;
}

enum result gs_sys_draw(
	struct gs_pos const *pos, 
	struct gs_draw const *draw, 
	SDL_Renderer *renderer
) {
	SDL_Rect rect;

	rect.x = pos->x - (rect.w = draw->w) / 2;
	rect.y = pos->y - (rect.h = draw->h) / 2;

	if (SDL_RenderCopy(renderer, draw->texture, NULL, &rect)) {
		LOG_ERROR(SDL_GetError());
		return RESULT_ERR;
	}
	return RESULT_OK;
}