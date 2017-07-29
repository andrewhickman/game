#ifndef GAMESTATE_GS_H
#define GAMESTATE_GS_H

#include "gamestate/entity.h"
#include "gamestate/cpnt.h"
#include "gamestate/sys.h"
#include "error.h"
#include "set.h"

struct gs {
	struct gs_entities entities;
	struct gs_cpnt pos, vel, draw;
};

struct gs_result {
	enum result result;
	struct gs value;
} gs_create(void);

enum result gs_update(struct gs *);

void gs_destroy(struct gs);

#endif