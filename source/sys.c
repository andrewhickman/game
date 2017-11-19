#include "sys.h"

#include "cpnt.h"
#include "geom.h"

void sys_physics(struct cpnt_pos *pos, struct cpnt_vel const *vel)
{
	pos->x += vel->x;
	pos->y += vel->y;
}

bool sys_collide(enum cpnt_coll_shape shape, struct vec pos, int w, int h)
{
	switch (shape) {
	case CPNT_COLL_CIRCLE:
		return geom_circle_point_collide(pos, w / 2);
	case CPNT_COLL_RECT:
		pos.x += w / 2;
		pos.y += h / 2;
		return geom_rect_point_collide(pos, w, h);
	}
	ASSERT(UNREACHABLE);
	return false;
}

enum result sys_draw(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	enum sys_select_state select,
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
	switch (select) {
	case SYS_SELECT_SELECTED:
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
	case SYS_SELECT_HOVERED:
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
	case SYS_SELECT_NONE:
		break;
	}

	return RESULT_OK;
}

enum result sys_update(struct gs *gs)
{
	struct ent_store_iter iter;
	struct ent_store_iter_result res;

	iter = ent_store_iter(&gs->cpnt.ents, CPNT_POS | CPNT_VEL);
	while (ent_store_iter_next(&iter, &res)) {
		struct cpnt_pos *pos = 
			store_dense_get_mut(&gs->cpnt.pos, res.ent.id, sizeof(*pos));
		struct cpnt_vel const *vel = 
			store_dense_get(&gs->cpnt.vel, res.ent.id, sizeof(*vel));

		sys_physics(pos, vel);
	}

	iter = ent_store_iter(&gs->cpnt.ents, CPNT_HP);
	while (ent_store_iter_next(&iter, &res)) {
		struct cpnt_hp const *hp =
			store_sparse_get(&gs->cpnt.hp, res.ent.id, sizeof(*hp));

		if (hp->health <= 0) {
			gs_kill(gs, res.ent);
		}
	}

	return RESULT_OK;
}

struct ent_result sys_new_unit(struct gs *gs, struct vec pos, enum texture texture)
{
	struct ent_result ent;

	ent.value = gs_spawn(gs);

	gs_insert_pos(gs, ent.value, pos);

	{
		struct cpnt_vel vel;
		vel.x = 0.0;
		vel.y = 0.0;
		gs_insert_vel(gs, ent.value, vel);
	}

	{
		struct cpnt_draw draw;
		static int w = 10;
		draw.texture = texture_get(texture);
		if (SDL_QueryTexture(draw.texture, NULL, NULL, &draw.w, &draw.h)) {
			LOG_ERROR(SDL_GetError());
			goto fail;
		}
		draw.w = draw.h = w += 5;
		gs_insert_draw(gs, ent.value, draw);
	}

	gs_insert_coll(gs, ent.value, CPNT_COLL_CIRCLE);

	{
		struct cpnt_hp hp;
		hp.health = 100;
		gs_insert_hp(gs, ent.value, hp);
	}

	ent.result = RESULT_OK;
	return ent;

fail:
	ent_store_kill(&gs->cpnt.ents, ent.value);
	ent.result = RESULT_ERR;
	return ent;
}

struct sys_find_result sys_find(struct gs *gs, struct vec click)
{
	struct sys_find_result ret; 

	struct ent_store_iter iter;
	struct ent_store_iter_result res;

	iter = ent_store_iter(&gs->cpnt.ents, CPNT_POS | CPNT_DRAW | CPNT_COLL);
	while (ent_store_iter_next(&iter, &res)) {
		struct cpnt_pos const *pos = 
			store_dense_get(&gs->cpnt.pos, res.ent.id, sizeof(*pos));
		struct cpnt_draw const *draw = 
			store_sparse_get(&gs->cpnt.draw, res.ent.id, sizeof(*draw));
		struct cpnt_coll const *coll = 
			store_dense_get_mut(&gs->cpnt.coll, res.ent.id, sizeof(*coll));

		if (sys_collide(coll->shape, vec_sub(click, *pos), draw->w, draw->h)) {
			ret.found = true;
			ret.ent = res.ent;
			return ret;
		}
	}

	ret.found = false;
	return ret;
}

void sys_nuke(struct gs *gs, struct ent ent, int dmg)
{
	if (ent_store_test_cpnt(&gs->cpnt.ents, ent, CPNT_HP)) {
		struct cpnt_hp *hp =
			store_sparse_get_mut(&gs->cpnt.hp, ent.id, sizeof(*hp));
		hp->health -= dmg;
	}
}

struct sys_query_result sys_query(struct gs const *gs, struct ent ent)
{
	struct sys_query_result ret;

	struct cpnt_hp const *hp;
	struct cpnt_draw const *draw;

	ret.found = ent_store_test_cpnt(&gs->cpnt.ents, ent, CPNT_DRAW | CPNT_HP);
	if (ret.found) {
		hp = store_sparse_get(&gs->cpnt.hp, ent.id, sizeof(*hp));
		draw =  store_sparse_get(&gs->cpnt.draw, ent.id, sizeof(*draw));
		ret.hp = *hp;
		ret.draw = *draw;
	}

	return ret;
}