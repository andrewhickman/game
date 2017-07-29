#include "gamestate/cpnt.h"

#include <stdlib.h>

struct gs_cpnt_result gs_cpnt_create(size_t len, size_t size)
{
	struct gs_cpnt_result ret;

	{
		struct set_result entities = set_create_with_capacity(len);
		if (entities.result == RESULT_ERR) goto fail_entities;
		ret.value.entities = entities.value;
	}

	ret.value.data = malloc(len * size);
	if (!ret.value.data) goto fail_data;

	return ret;

fail_data:
	set_destroy(ret.value.entities);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

enum result gs_cpnt_insert(struct gs_cpnt *cpnt, size_t id, size_t size)
{
	size_t old_len = cpnt->entities.len;
	if (set_insert(&cpnt->entities, id) == SET_ERR) return RESULT_ERR;
	if (cpnt->entities.len != old_len) {
		void *data = realloc(cpnt->data, cpnt->entities.len * size);
		if (!data) {
			LOG_ERROR("out of memory");
			return RESULT_ERR;
		}
		cpnt->data = data;
	}
	return RESULT_OK;
}

void gs_cpnt_destroy(struct gs_cpnt cpnt)
{
	set_destroy(cpnt.entities);
	free(cpnt.data);
}