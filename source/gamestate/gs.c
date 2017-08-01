#include "gamestate/gs.h"
#include "gamestate/sys.h"

struct gs_result gs_create(void)
{
	struct gs_result ret;

	{
		struct gs_entities_result entities = gs_entities_create(32);
		if (entities.result == RESULT_ERR) goto fail_entities;
		ret.value.entities = entities.value;
	}

	{
		struct gs_cpnt_sparse_result cpnt;
		
		cpnt = gs_cpnt_sparse_create(32, sizeof(struct gs_pos));
		if (cpnt.result == RESULT_ERR) goto fail_pos;
		ret.value.pos = cpnt.value;
		
		cpnt = gs_cpnt_sparse_create(32, sizeof(struct gs_vel));
		if (cpnt.result == RESULT_ERR) goto fail_vel;
		ret.value.vel = cpnt.value;
		
		cpnt = gs_cpnt_sparse_create(32, sizeof(struct gs_draw));
		if (cpnt.result == RESULT_ERR) goto fail_draw;
		ret.value.draw = cpnt.value;
	}

	ret.result = RESULT_OK;
	return ret;

fail_draw:
	gs_cpnt_sparse_destroy(ret.value.vel);
fail_vel:
	gs_cpnt_sparse_destroy(ret.value.pos);
fail_pos:
	gs_entities_destroy(ret.value.entities);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

enum result gs_update(struct gs *gs, SDL_Renderer *renderer)
{
	unsigned id;

	{
		struct gs_pos *pos;
		struct gs_vel *vel;

		for (id = 0; id < gs->entities.len; ++id, ++pos, ++vel) {
			if (gs->entities.buf[id] & GS_CPNT_VEL) {
				pos = gs_cpnt_sparse_get(&gs->pos, id, sizeof(*pos));
				vel = gs_cpnt_sparse_get(&gs->vel, id, sizeof(*vel));
				if (gs_sys_physics(pos, vel) == RESULT_ERR) {
					return RESULT_ERR;
				}
			}
		}
	}

	{
		struct gs_pos *pos;
		struct gs_draw *draw;

		for (id = 0; id < gs->entities.len; ++id, ++pos, ++draw) {
			if (gs->entities.buf[id] & GS_CPNT_DRAW) {
				pos = gs_cpnt_sparse_get(&gs->pos, id, sizeof(*pos));
				draw = gs_cpnt_sparse_get(&gs->draw, id, sizeof(*draw));
				if (gs_sys_draw(pos, draw, renderer) == RESULT_ERR) {
					return RESULT_ERR;
				}
			}
		}
	}

	return RESULT_OK;
}

enum result gs_new_unit(struct gs *gs, int x, int y, enum texture texture)
{
	unsigned id;
	struct gs_pos *pos;
	struct gs_draw *draw;

	{
		struct gs_entities_new_result r = gs_entities_new(&gs->entities);
		if (r.result == RESULT_ERR) return RESULT_ERR;
		id = r.id;
		gs->entities.buf[id] = GS_CPNT_DRAW;
	}

	pos = gs_cpnt_sparse_insert(&gs->pos, id, sizeof(struct gs_pos));
	if (!pos) return RESULT_ERR;
	pos[id].x = x;
	pos[id].y = y;

	draw = gs_cpnt_sparse_insert(&gs->draw, id, sizeof(struct gs_draw));
	if (!draw) return RESULT_ERR;
	draw[id].texture = texture_get(texture);
	if (SDL_QueryTexture(draw[id].texture, NULL, NULL, &draw[id].w, &draw[id].h)) {
		LOG_ERROR(SDL_GetError());
		return RESULT_ERR;
	}

	return RESULT_OK;
}

void gs_destroy(struct gs gs)
{
	gs_cpnt_sparse_destroy(gs.draw);
	gs_cpnt_sparse_destroy(gs.vel);
	gs_cpnt_sparse_destroy(gs.pos);
	gs_entities_destroy(gs.entities);
}