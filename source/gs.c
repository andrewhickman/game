#include "gs.h"
#include "sys.h"
#include "path.h"

bool gs_select_collide_circle(int x, int y, int w, int h);

struct gs_result gs_create(void)
{
	struct gs_result ret;

	ret.gs.cpnt.ents = ent_store_create(4);

	ret.gs.cpnt.pos = store_dense_create(4, sizeof(struct cpnt_pos));
	ret.gs.cpnt.vel = store_dense_create(4, sizeof(struct cpnt_vel));
	ret.gs.cpnt.coll = store_dense_create(4, sizeof(struct cpnt_coll));

	ret.gs.cpnt.draw = store_sparse_create(4, sizeof(struct cpnt_draw));
	ret.gs.cpnt.hp = store_sparse_create(4, sizeof(struct cpnt_hp));

	ret.gs.path = path_map_create();

	{
		int i;
		struct ent_result ent;

		for (i = 0; i < 4; ++i) {
			struct vec pos = { (i + 2) * 100, 200 };

			ent = sys_new_unit(&ret.gs, pos, TEXTURE_UNIT);
			ASSERT(ent.result != RESULT_ERR);
			ret.ents[i] = ent.value;
		}
	}

	return ret;
}

struct ent gs_spawn(struct gs *gs)
{
	return ent_store_spawn(&gs->cpnt.ents);
}

void gs_kill(struct gs *gs, struct ent ent)
{
	enum cpnt cpnt = ent_store_kill(&gs->cpnt.ents, ent);
	if (cpnt & CPNT_COLL) gs_remove_coll(gs, ent, cpnt);
}

void gs_insert_pos(struct gs *gs, struct ent ent, struct cpnt_pos pos)
{
	struct cpnt_pos *space;

	space = store_dense_insert(&gs->cpnt.pos, ent.id, sizeof(struct cpnt_pos));
	*space = pos;
	ent_store_add_cpnt(&gs->cpnt.ents, ent, CPNT_POS);
}

void gs_insert_vel(struct gs *gs, struct ent ent, struct cpnt_vel vel)
{
	struct cpnt_vel *space;

	space = store_dense_insert(&gs->cpnt.vel, ent.id, sizeof(struct cpnt_vel));
	*space = vel;
	ent_store_add_cpnt(&gs->cpnt.ents, ent, CPNT_VEL);
}

void gs_insert_draw(struct gs *gs, struct ent ent, struct cpnt_draw draw)
{
	struct cpnt_draw *space;

	space = store_sparse_insert(&gs->cpnt.draw, ent.id, sizeof(struct cpnt_draw));
	*space = draw;
	ent_store_add_cpnt(&gs->cpnt.ents, ent, CPNT_DRAW);
}

void gs_insert_hp(struct gs *gs, struct ent ent, struct cpnt_hp hp)
{
	struct cpnt_hp *space;

	space = store_sparse_insert(&gs->cpnt.hp, ent.id, sizeof(struct cpnt_hp));
	*space = hp;
	ent_store_add_cpnt(&gs->cpnt.ents, ent, CPNT_HP);
}

void gs_insert_coll(struct gs *gs, struct ent ent, enum cpnt_coll_shape shape)
{
	struct cpnt_pos const *pos;
	struct cpnt_draw const *draw;
	struct cpnt_coll *space;

	ASSERT(ent_store_test_cpnt(&gs->cpnt.ents, ent, CPNT_POS | CPNT_DRAW));
	pos = store_dense_get(&gs->cpnt.pos, ent.id, sizeof(struct cpnt_pos));
	draw = store_sparse_get(&gs->cpnt.draw, ent.id, sizeof(struct cpnt_draw));

	space = store_dense_insert(&gs->cpnt.coll, ent.id, sizeof(struct cpnt_coll));
	*space = path_add_coll(gs, pos, draw, shape);
	ent_store_add_cpnt(&gs->cpnt.ents, ent, CPNT_COLL);
}

void gs_remove_coll(struct gs *gs, struct ent ent, enum cpnt cpnt)
{
	struct cpnt_pos const *pos;
	struct cpnt_draw const *draw;
	struct cpnt_coll *coll;

	ASSERT((cpnt & CPNT_POS) && (cpnt & CPNT_DRAW) && (cpnt & CPNT_COLL));
	pos = store_dense_get(&gs->cpnt.pos, ent.id, sizeof(struct cpnt_pos));
	draw = store_sparse_get(&gs->cpnt.draw, ent.id, sizeof(struct cpnt_draw));
	coll = store_dense_get_mut(&gs->cpnt.coll, ent.id, sizeof(*coll));

	path_rm_coll(gs, pos, draw, *coll);
}

void gs_destroy(struct gs gs)
{
	unsigned id;
	for (id = 0; id != gs.cpnt.ents.len; ++id) {
		if (gs.cpnt.ents.buf[id].gen <= 0) continue;
		if (gs.cpnt.ents.buf[id].cpnt & CPNT_COLL) {
			struct cpnt_coll *coll = 
				store_dense_get_mut(&gs.cpnt.coll, id, sizeof(*coll));
			cpnt_coll_destroy(*coll);
		}
		/*
		if (gs.ents.buf[id].cpnt & CPNT_PATH) {
			struct cpnt_path *path = 
				store_dense_get_mut(&gs.path, id, sizeof(*path));
			cpnt_path_destroy(*path);
		}
		*/
	}

	path_map_destroy(gs.path);
	store_sparse_destroy(gs.cpnt.hp);
	store_sparse_destroy(gs.cpnt.draw);
	store_dense_destroy(gs.cpnt.coll);
	store_dense_destroy(gs.cpnt.vel);
	store_dense_destroy(gs.cpnt.pos);
	ent_store_destroy(gs.cpnt.ents);
}