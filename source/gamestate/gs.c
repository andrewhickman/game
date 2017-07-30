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
		struct gs_cpnt_result cpnt;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_pos));
		if (cpnt.result == RESULT_ERR) goto fail_pos;
		ret.value.pos = cpnt.value;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_vel));
		if (cpnt.result == RESULT_ERR) goto fail_vel;
		ret.value.vel = cpnt.value;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_draw));
		if (cpnt.result == RESULT_ERR) goto fail_draw;
		ret.value.draw = cpnt.value;
	}

	ret.result = RESULT_OK;
	return ret;

fail_draw:
	gs_cpnt_destroy(ret.value.vel);
fail_vel:
	gs_cpnt_destroy(ret.value.pos);
fail_pos:
	gs_entities_destroy(ret.value.entities);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

enum result gs_update(struct gs *gs, SDL_Renderer *renderer)
{
	struct set entities[2];
	struct set_and_iter iter;
	struct set_iter_result id;

	entities[0] = gs->pos.entities;
	entities[1] = gs->vel.entities;
	iter = set_and_iter(entities, 2);
	while (!(id = set_and_iter_next(&iter)).finished) {
		if (gs_sys_physics(id.value, 
		                   gs->pos.data, 
		                   gs->vel.data) == RESULT_ERR) {
			return RESULT_ERR;
		}
	}

	entities[0] = gs->pos.entities;
	entities[1] = gs->draw.entities;
	iter = set_and_iter(entities, 2);
	while (!(id = set_and_iter_next(&iter)).finished) {
		if (gs_sys_draw(id.value, 
		                gs->pos.data, 
		                gs->draw.data, 
		                renderer) == RESULT_ERR) {
			return RESULT_ERR;
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
	}

	gs_cpnt_insert(&gs->pos, id, sizeof(struct gs_pos));
	pos = gs->pos.data;
	pos[id].x = x;
	pos[id].y = y;

	gs_cpnt_insert(&gs->draw, id, sizeof(struct gs_draw));
	draw = gs->draw.data;
	draw[id].texture = texture_get(texture);
	SDL_QueryTexture(draw[id].texture, NULL, NULL, &draw[id].w, &draw[id].h);

	return RESULT_OK;
}

void gs_destroy(struct gs gs)
{
	gs_cpnt_destroy(gs.draw);
	gs_cpnt_destroy(gs.vel);
	gs_cpnt_destroy(gs.pos);
	gs_entities_destroy(gs.entities);
}