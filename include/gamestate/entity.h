#ifndef GAMESTATE_ENTITY_H
#define GAMESTATE_ENTITY_H

#include "error.h"
#include "set.h"
#include "gamestate/cpnt.h"

#include <stddef.h>

struct gs_entities {
	enum gs_cpnt *buf;
	size_t len;
	unsigned start;
};

struct gs_entities_result {
	enum result result;
	struct gs_entities value;
} gs_entities_create(size_t len);

struct gs_entities_new_result {
	enum result result;	
	unsigned id;
} gs_entities_new(struct gs_entities *);

void gs_entities_kill(struct gs_entities *, unsigned id);

void gs_entities_destroy(struct gs_entities);

#endif