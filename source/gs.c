#include "gs.h"
#include "sys.h"

bool gs_select_collide_circle(int x, int y, int w, int h);

struct gs_result gs_create(void)
{
	struct gs_result ret;

	{
		struct ent_store_result ents = ent_store_create(0);
		if (ents.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_entities;
		}
		ret.value.ents = ents.value;
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
		
		cpnt = store_dense_create(0, sizeof(struct cpnt_select));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_select;
		}
		ret.value.select = cpnt.value;
		
		cpnt = store_dense_create(0, sizeof(struct cpnt_coll));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_coll;
		}
		ret.value.coll = cpnt.value;
	}

	{
		struct store_sparse_result cpnt;	

		cpnt = store_sparse_create(0, sizeof(struct cpnt_draw));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_draw;
		}
		ret.value.draw = cpnt.value;

		cpnt = store_sparse_create(0, sizeof(struct cpnt_hp));
		if (cpnt.result == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_hp;
		}
		ret.value.hp = cpnt.value;
	}

	{
		int i;
		struct ent_result ent;

		for (i = 0; i < 4; ++i) {
			ent = sys_new_unit(&ret.value, (i + 2) * 100, 200, TEXTURE_UNIT);
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
	store_sparse_destroy(ret.value.hp);
fail_hp:
	store_sparse_destroy(ret.value.draw);
fail_draw:
	store_dense_destroy(ret.value.select);
fail_select:
	store_dense_destroy(ret.value.coll);
fail_coll:
	store_dense_destroy(ret.value.vel);
fail_vel:
	store_dense_destroy(ret.value.pos);
fail_pos:
	ent_store_destroy(ret.value.ents);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

struct ent_result gs_spawn(struct gs *gs)
{
	return ent_store_spawn(&gs->ents);
}

void gs_kill(struct gs *gs, struct ent ent)
{
	ent_store_kill(&gs->ents, ent);
}

enum result gs_insert_pos(struct gs *gs, struct ent ent, struct cpnt_pos pos)
{
	struct cpnt_pos *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_POS);
	space = store_dense_insert(&gs->pos, ent.id, sizeof(struct cpnt_pos));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = pos;

	return RESULT_OK;
}

enum result gs_insert_vel(struct gs *gs, struct ent ent, struct cpnt_vel vel)
{
	struct cpnt_vel *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_VEL);
	space = store_dense_insert(&gs->vel, ent.id, sizeof(struct cpnt_vel));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = vel;

	return RESULT_OK;
}

enum result gs_insert_draw(struct gs *gs, struct ent ent, struct cpnt_draw draw)
{
	struct cpnt_draw *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_DRAW);
	space = store_sparse_insert(&gs->draw, ent.id, sizeof(struct cpnt_draw));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = draw;

	return RESULT_OK;
}

enum result gs_insert_hp(struct gs *gs, struct ent ent, struct cpnt_hp hp)
{
	struct cpnt_hp *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_HP);
	space = store_sparse_insert(&gs->hp, ent.id, sizeof(struct cpnt_hp));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = hp;

	return RESULT_OK;
}

enum result gs_insert_select(struct gs *gs, struct ent ent, struct cpnt_select select)
{
	struct cpnt_select *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_SELECT);
	space = store_dense_insert(&gs->select, ent.id, sizeof(struct cpnt_select));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = select;

	return RESULT_OK;
}

enum result gs_insert_coll(struct gs *gs, struct ent ent, struct cpnt_coll coll)
{
	struct cpnt_coll *space;

	ent_store_add_cpnt(&gs->ents, ent, CPNT_COLL);
	space = store_dense_insert(&gs->coll, ent.id, sizeof(struct cpnt_coll));
	if (!space) {
		LOG_CHAIN();
		return RESULT_ERR;
	}
	*space = coll;

	return RESULT_OK;
}

void gs_destroy(struct gs gs)
{
	store_sparse_destroy(gs.draw);
	store_dense_destroy(gs.vel);
	store_dense_destroy(gs.pos);
	ent_store_destroy(gs.ents);
}