#include "gamestate/sys.h"

enum result gs_sys_physics(
	unsigned id,
	struct gs_pos *pos, 
	struct gs_vel const *vel
) {
	pos[id].x += vel[id].x;
	pos[id].y += vel[id].y;
	return RESULT_OK;
}

enum result gs_sys_draw(
	unsigned id,
	struct gs_pos const *pos, 
	struct gs_draw const *draw, 
	SDL_Renderer *renderer
) {
	SDL_Rect rect;

	rect.x = pos[id].x - (rect.w = draw[id].w) / 2;
	rect.y = pos[id].y - (rect.h = draw[id].h) / 2;

	if (SDL_RenderCopy(renderer, draw[id].texture, NULL, &rect)) {
		LOG_ERROR(SDL_GetError());
		return RESULT_ERR;
	}
	return RESULT_OK;
}