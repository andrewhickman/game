#ifndef GS_H
#define GS_H

#include "SDL2/SDL.h"

#include "cpnt.h"
#include "ent.h"
#include "error.h"
#include "resrc.h"
#include "store.h"

struct gs {
	struct ent_store ents;
	struct store_sparse hp, draw;
	struct store_dense pos, vel, select, coll; 
};

struct gs_result {
	enum result result;
	struct gs value;
	struct ent ents[4];
} gs_create(void);

struct ent_result gs_spawn(struct gs *);
void gs_kill(struct gs *, struct ent);

enum result gs_insert_pos(struct gs *, struct ent, struct cpnt_pos);
enum result gs_insert_vel(struct gs *, struct ent, struct cpnt_vel);
enum result gs_insert_draw(struct gs *, struct ent, struct cpnt_draw);
enum result gs_insert_select(struct gs *, struct ent, struct cpnt_select);
enum result gs_insert_coll(struct gs *, struct ent, struct cpnt_coll);
enum result gs_insert_hp(struct gs *gs, struct ent, struct cpnt_hp);

void gs_destroy(struct gs);

#endif