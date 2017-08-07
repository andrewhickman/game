#include "gs.h"
#include "sys.h"

enum result gs_insert_pos(struct gs *, unsigned id, struct cpnt_pos);
enum result gs_insert_vel(struct gs *, unsigned id, struct cpnt_vel);
enum result gs_insert_draw(struct gs *, unsigned id, struct cpnt_draw);
enum result gs_insert_select(struct gs *, unsigned id, struct cpnt_select);

bool gs_select_collide_circle(int x, int y, int w, int h);

struct gs_result gs_create(void)
{
	struct gs_result ret;

	{
		struct ent_store_result entities = ent_store_create(0);
		if (entities.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_entities;
		}
		ret.value.entities = entities.value;
	}

	{
		struct store_dense_result cpnt;
		
		cpnt = store_dense_create(0, sizeof(struct cpnt_pos));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_pos;
		}
		ret.value.pos = cpnt.value;
		
		cpnt = store_dense_create(0, sizeof(struct cpnt_vel));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_vel;
		}
		ret.value.vel = cpnt.value;
	}

	{
		struct store_sparse_result cpnt;	

		cpnt = store_sparse_create(0, sizeof(struct cpnt_draw));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_draw;
		}
		ret.value.draw = cpnt.value;

		cpnt = store_sparse_create(0, sizeof(struct cpnt_draw));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_select;
		}
		ret.value.select = cpnt.value;
	}

	{
		int i;
		struct ent_result ent;

		for (i = 0; i < 4; ++i) {
			ent = ent_store_new(&ret.value.entities);
			if (ent.result == RESULT_ERR) {
				LOG_CHAIN();
				goto fail_ents;
			}
			ret.ents[i] = ent.value;
		}
	}

	ret.result = RESULT_OK;
	return ret;

fail_ents:
	store_sparse_destroy(ret.value.select);
fail_select:
	store_sparse_destroy(ret.value.draw);
fail_draw:
	store_dense_destroy(ret.value.vel);
fail_vel:
	store_dense_destroy(ret.value.pos);
fail_pos:
	ent_store_destroy(ret.value.entities);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

enum result gs_update(struct gs *gs, SDL_Renderer *renderer)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	iter = ent_store_iter(&gs->entities);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos *pos = result.cpnt & CPNT_POS
			? store_dense_get(&gs->pos, result.ent.id, sizeof(*pos))
			: NULL;
		struct cpnt_vel *vel = result.cpnt & CPNT_VEL
			? store_dense_get(&gs->vel, result.ent.id, sizeof(*vel))
			: NULL;

		if (pos && vel) {
			sys_physics(pos, vel);
		}
	}

	iter = ent_store_iter(&gs->entities);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos *pos = result.cpnt & CPNT_POS
			? store_dense_get(&gs->pos, result.ent.id, sizeof(*pos))
			: NULL;
		struct cpnt_draw *draw = result.cpnt & CPNT_DRAW
			? store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw))
			: NULL;
		struct cpnt_select *select = result.cpnt & CPNT_SELECT 
			? store_sparse_get(&gs->select, result.ent.id, sizeof(*select))
			: NULL;

		if (pos && draw) {
			enum result err = sys_draw(pos, draw, select, renderer);
			if (err) {
				LOG_CHAIN();
				return err;
			}
		}
	}

	return RESULT_OK;
}

struct ent_result gs_new_unit(struct gs *gs, int x, int y, enum texture texture)
{
	struct ent_result ent = ent_store_new(&gs->entities);
	if (ent.result == RESULT_ERR) {
		LOG_CHAIN();
		return ent;
	}

	{
		struct cpnt_pos pos;
		pos.x = x;
		pos.y = y;
		if (gs_insert_pos(gs, ent.value.id, pos) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_vel vel;
		vel.x = 0.0;
		vel.y = 0.0;
		if (gs_insert_vel(gs, ent.value.id, vel) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_draw draw;
		draw.texture = texture_get(texture);
		if (SDL_QueryTexture(draw.texture, NULL, NULL, &draw.w, &draw.h)) {
			LOG_ERROR(SDL_GetError());
			goto fail;
		}
		if (gs_insert_draw(gs, ent.value.id, draw) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_select select;
		select.state = CPNT_SELECT_NONE;
		select.collides = &gs_select_collide_circle;
		if (gs_insert_select(gs, ent.value.id, select) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	return ent;

fail:
	ent_store_kill(&gs->entities, ent.value);
	ent.result = RESULT_ERR;
	return ent;
}

struct gs_find_result gs_hover(struct gs *gs, int x, int y)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	struct gs_find_result ret; 
	struct cpnt_select *last = NULL;

	ret.found = false;
	iter = ent_store_iter(&gs->entities);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos *pos = result.cpnt & CPNT_POS
			? store_dense_get(&gs->pos, result.ent.id, sizeof(*pos))
			: NULL;
		struct cpnt_draw *draw = result.cpnt & CPNT_DRAW
			? store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw))
			: NULL;
		struct cpnt_select *select = result.cpnt & CPNT_SELECT 
			? store_sparse_get(&gs->select, result.ent.id, sizeof(*select))
			: NULL;

		if (pos && draw && select) {
			if (select->state == CPNT_SELECT_HOVERED) {
				select->state = CPNT_SELECT_NONE;
			}
			if (select->collides(x - pos->x, y - pos->y, 
			                     draw->w, draw->w)) {
				ret.found = true;
				ret.ent = result.ent;
				last = select;
			}
		}
	}
	if (last && last->state != CPNT_SELECT_SELECTED) {
		last->state = CPNT_SELECT_HOVERED;
	}
	return ret;
}

struct gs_find_result gs_select(struct gs *gs, int x, int y)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	struct gs_find_result ret; 
	struct cpnt_select *last = NULL;

	ret.found = false;
	iter = ent_store_iter(&gs->entities);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos *pos = result.cpnt & CPNT_POS
			? store_dense_get(&gs->pos, result.ent.id, sizeof(*pos))
			: NULL;
		struct cpnt_draw *draw = result.cpnt & CPNT_DRAW
			? store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw))
			: NULL;
		struct cpnt_select *select = result.cpnt & CPNT_SELECT 
			? store_sparse_get(&gs->select, result.ent.id, sizeof(*select))
			: NULL;

		if (pos && draw && select) {
			select->state = CPNT_SELECT_NONE;
			if (select->collides(x - pos->x, y - pos->y, 
			                     draw->w, draw->w)) {
				ret.found = true;
				ret.ent = result.ent;
				last = select;
			}
		}
	}
	if (last) last->state = CPNT_SELECT_SELECTED;
	return ret;
}

enum result gs_insert_pos(struct gs *gs, unsigned id, struct cpnt_pos pos)
{
	struct cpnt_pos *space;

	gs->entities.buf[id].cpnt |= CPNT_POS;
	space = store_dense_insert(&gs->pos, id, sizeof(struct cpnt_pos));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = pos;

	return RESULT_OK;
}

enum result gs_insert_vel(struct gs *gs, unsigned id, struct cpnt_vel vel)
{
	struct cpnt_vel *space;

	gs->entities.buf[id].cpnt |= CPNT_VEL;
	space = store_dense_insert(&gs->vel, id, sizeof(struct cpnt_vel));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = vel;

	return RESULT_OK;
}

enum result gs_insert_draw(struct gs *gs, unsigned id, struct cpnt_draw draw)
{
	struct cpnt_draw *space;

	gs->entities.buf[id].cpnt |= CPNT_DRAW;
	space = store_sparse_insert(&gs->draw, id, sizeof(struct cpnt_draw));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = draw;

	return RESULT_OK;
}

enum result gs_insert_select(struct gs *gs, unsigned id, struct cpnt_select select)
{
	struct cpnt_select *space;

	gs->entities.buf[id].cpnt |= CPNT_SELECT;
	space = store_sparse_insert(&gs->select, id, sizeof(struct cpnt_select));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = select;

	return RESULT_OK;
}

struct gs_query_result gs_query(struct gs const *gs, struct ent ent)
{
	struct gs_query_result ret;

	(void)gs;

	ret.found = true;
	ret.health = 25 + ent.id * 25;

	return ret;
}

void gs_destroy(struct gs gs)
{
	store_sparse_destroy(gs.draw);
	store_dense_destroy(gs.vel);
	store_dense_destroy(gs.pos);
	ent_store_destroy(gs.entities);
}

bool gs_select_collide_circle(int x, int y, int w, int h)
{
	x *= x, y *= y; 
	w = (w * w) / 4, h = (h * h) / 4;
	return x * h + y * w < w * h;
}