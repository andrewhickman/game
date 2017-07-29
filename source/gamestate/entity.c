#include "gamestate/entity.h"

#include <stdlib.h>

struct gs_entities_result gs_entities_create(size_t len)
{
	struct gs_entities_result ret;

	{
		struct set_result set = set_create_with_capacity(len);
		if (set.result == RESULT_ERR) {
			ret.result = RESULT_ERR;
			return ret;
		}
		ret.value.set = set.value;
	}

	ret.value.space = 0;
	ret.result = RESULT_OK;
	return ret;
}

struct gs_entities_new_result gs_entities_new(struct gs_entities *entities)
{
	struct gs_entities_new_result ret;

	if (set_insert(&entities->set, entities->space) == SET_ERR) {
		ret.result = RESULT_ERR;
		return ret;
	}
	ret.id = entities->space;
	entities->space = set_next_slot(&entities->set, ++entities->space);

	ret.result = RESULT_OK;
	return ret;
}

void gs_entities_kill(struct gs_entities *entities, unsigned id)
{
	set_remove(&entities->set, id);
	if (id < entities->space) {
		entities->space = id;
	}
}

void gs_entities_destroy(struct gs_entities entities)
{
	set_destroy(entities.set);
}
