#ifndef GS_H
#define GS_H

#include "SDL2/SDL.h"

#include "cpnt.h"
#include "ent.h"
#include "error.h"
#include "resrc.h"
#include "store.h"
#include "path.h"

struct gs {
	struct gs_world {
		struct ent_store ents;
		struct store_sparse hp, draw;
		struct store_dense pos, vel, coll;
	} cpnt;
	struct path_map path;
};

struct gs_result {
	struct gs gs;
	struct ent ents[4];
} gs_create(void);

struct ent gs_spawn(struct gs *);
void gs_kill(struct gs *, struct ent);

void gs_insert_pos(struct gs *, struct ent, struct cpnt_pos);
void gs_insert_vel(struct gs *, struct ent, struct cpnt_vel);
void gs_insert_draw(struct gs *, struct ent, struct cpnt_draw);
void gs_insert_coll(struct gs *, struct ent, enum cpnt_coll_shape);
void gs_insert_hp(struct gs *gs, struct ent, struct cpnt_hp);

void gs_remove_coll(struct gs *, struct ent, enum cpnt);

void gs_destroy(struct gs);

#endif