#ifndef GAMESTATE_GEN_H
#define GAMESTATE_GEN_H

#include "error.h"
#include "set.h"

#include <stddef.h>

struct gs_entities {
	struct set set;
	unsigned space;
};

struct gs_entities_result {
	enum result result;
	struct gs_entities value;
} gs_entities_create(size_t cap);

struct gs_entities_new_result {
	enum result result;	
	unsigned id;
} gs_entities_new(struct gs_entities *);

void gs_entities_kill(struct gs_entities *, unsigned id);

void gs_entities_destroy(struct gs_entities);

#endif