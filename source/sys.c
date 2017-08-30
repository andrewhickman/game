#include "sys.h"

void sys_physics(struct cpnt_pos *pos, struct cpnt_vel const *vel)
{
	pos->x += vel->x;
	pos->y += vel->y;
}

bool sys_collide(enum cpnt_coll_shape shape, int x, int y, int w, int h)
{
	switch (shape) {
	case CPNT_COLL_ELLIPSE:
		x *= x, y *= y; 
		w = (w * w) / 4, h = (h * h) / 4;
		return x * h + y * w <= w * h;
	case CPNT_COLL_RECT:
		return 0 <= x && x <= w && 0 <= y &&  y <= h;
	}
	ASSERT(UNREACHABLE);
	return false;
}

enum result sys_draw_entity(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_select const *select, /* optional */
	SDL_Renderer *renderer,
	SDL_Rect const *cam
) {
	int err;
	SDL_Rect rect;

	rect.w = draw->w;
	rect.h = draw->h;
	rect.x = (pos->x - rect.w / 2) - cam->x;
	rect.y = (pos->y - rect.h / 2) - cam->y;

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

enum result sys_update(struct gs *gs)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	iter = ent_store_iter(&gs->ents, CPNT_POS & CPNT_VEL);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos *pos = 
			store_dense_get_mut(&gs->pos, result.ent.id, sizeof(*pos));
		struct cpnt_vel const *vel = 
			store_dense_get(&gs->vel, result.ent.id, sizeof(*vel));

		sys_physics(pos, vel);
	}

	iter = ent_store_iter(&gs->ents, CPNT_HP);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_hp const *hp =
			store_sparse_get(&gs->hp, result.ent.id, sizeof(*hp));

		if (hp->health <= 0) {
			ent_store_kill(&gs->ents, result.ent);
		}
	}

	return RESULT_OK;
}

enum result sys_draw(struct gs const *gs, SDL_Renderer *renderer, SDL_Rect const *cam)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	iter = ent_store_iter(&gs->ents, CPNT_POS & CPNT_DRAW);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos const *pos =
			store_dense_get(&gs->pos, result.ent.id, sizeof(*pos));
		struct cpnt_draw const *draw =
			store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw));
		struct cpnt_select const *select = result.cpnt & CPNT_SELECT 
			? store_dense_get(&gs->select, result.ent.id, sizeof(*select))
			: NULL;

		if (sys_draw_entity(pos, draw, select, renderer, cam) == RESULT_ERR) {
			LOG_CHAIN();
			return RESULT_ERR;
		}
	}

	return RESULT_OK;
}

struct ent_result sys_new_unit(struct gs *gs, int x, int y, enum texture texture)
{
	struct ent_result ent = ent_store_spawn(&gs->ents);
	if (ent.result == RESULT_ERR) {
		LOG_CHAIN();
		return ent;
	}

	{
		struct cpnt_pos pos;
		pos.x = x;
		pos.y = y;
		if (gs_insert_pos(gs, ent.value, pos) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_vel vel;
		vel.x = 0.0;
		vel.y = 0.0;
		if (gs_insert_vel(gs, ent.value, vel) == RESULT_ERR) {
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
		if (gs_insert_draw(gs, ent.value, draw) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_coll coll;
		coll.shape = CPNT_COLL_ELLIPSE;
		if (gs_insert_coll(gs, ent.value, coll) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_select select;
		select.state = CPNT_SELECT_NONE;
		if (gs_insert_select(gs, ent.value, select) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	{
		struct cpnt_hp hp;
		hp.health = 100;
		if (gs_insert_hp(gs, ent.value, hp) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail;
		}
	}

	return ent;

fail:
	ent_store_kill(&gs->ents, ent.value);
	ent.result = RESULT_ERR;
	return ent;
}

struct gs_find_result sys_find_hover(struct gs *gs, int x, int y)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	struct gs_find_result ret; 
	struct cpnt_select *last = NULL;

	ret.found = false;
	iter = ent_store_iter(&gs->ents, CPNT_POS | CPNT_DRAW | CPNT_COLL | CPNT_SELECT);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos const *pos = 
			store_dense_get(&gs->pos, result.ent.id, sizeof(*pos));
		struct cpnt_draw const *draw = 
			store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw));
		struct cpnt_coll const *coll = 
			store_dense_get_mut(&gs->coll, result.ent.id, sizeof(*coll));
		struct cpnt_select *select =
			store_dense_get_mut(&gs->select, result.ent.id, sizeof(*select));

		if (select->state == CPNT_SELECT_HOVERED) {
			select->state = CPNT_SELECT_NONE;
		}
		if (sys_collide(coll->shape, x - pos->x, y - pos->y, draw->w, draw->h)) {
			ret.found = true;
			ret.ent = result.ent;
			last = select;
		}
	}
	if (last && last->state != CPNT_SELECT_SELECTED) {
		last->state = CPNT_SELECT_HOVERED;
	}
	return ret;
}

struct gs_find_result sys_find_select(struct gs *gs, int x, int y)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	struct gs_find_result ret; 
	struct cpnt_select *last = NULL;

	ret.found = false;
	iter = ent_store_iter(&gs->ents, CPNT_POS | CPNT_DRAW | CPNT_COLL | CPNT_SELECT);
	while (!(result = ent_store_iter_next(&iter)).finished) {
		struct cpnt_pos const *pos = 
			store_dense_get(&gs->pos, result.ent.id, sizeof(*pos));
		struct cpnt_draw const *draw = 
			store_sparse_get(&gs->draw, result.ent.id, sizeof(*draw));
		struct cpnt_coll const *coll = 
			store_dense_get_mut(&gs->coll, result.ent.id, sizeof(*coll));
		struct cpnt_select *select =
			store_dense_get_mut(&gs->select, result.ent.id, sizeof(*select));

		select->state = CPNT_SELECT_NONE;
		if (sys_collide(coll->shape, x - pos->x, y - pos->y, draw->w, draw->h)) {
			ret.found = true;
			ret.ent = result.ent;
			last = select;
		}
	}
	if (last) last->state = CPNT_SELECT_SELECTED;
	return ret;
}

void sys_nuke(struct gs *gs, struct ent ent, int dmg)
{
	enum cpnt cpnt = ent_store_get_cpnt(&gs->ents, ent);
	struct cpnt_hp *hp = cpnt & CPNT_HP 
		? store_sparse_get_mut(&gs->hp, ent.id, sizeof(*hp))
		: NULL;
	
	if (hp) hp->health -= dmg;
}

struct sys_query_result sys_query(struct gs const *gs, struct ent ent)
{
	struct sys_query_result ret;
	enum cpnt cpnt = ent_store_get_cpnt(&gs->ents, ent);

	struct cpnt_draw const *draw = cpnt & CPNT_DRAW
		? store_sparse_get(&gs->draw, ent.id, sizeof(*draw))
		: NULL;
	struct cpnt_hp const *hp = cpnt & CPNT_HP
		? store_sparse_get(&gs->hp, ent.id, sizeof(*hp))
		: NULL;

	ret.found = draw && hp;
	if (ret.found) {
		ret.hp = *hp;
		ret.draw = *draw;
	}

	return ret;
}