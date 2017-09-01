#include "path.h"

#include <stdlib.h>

struct hset_result path_find_blocked(struct graph *graph, struct cpnt_coll *coll)
{
	struct hset_result ret;

	struct graph_edge_iter iter = graph_edge_iter(graph);
	struct graph_edge_iter_result result;
	struct graph_edge *buf = NULL;
	size_t len = 0, cap = 0;

	while (!(result = graph_edge_iter_next(&iter)).finished) {
		(void)coll;
		/* TODO check if blocked */
		if (len == cap) {
			struct graph_edge *new;

			cap = cap ? cap * 2 : 1;
			new = realloc(buf, cap * sizeof(*buf));
			if (!new) goto fail_buf;
			buf = new;
		}
		buf[len++] = result.value;
	}

	ret = hset_create(buf, len);
	free(buf);
	return ret;

fail_buf:
	LOG_ERROR("out of memory");
	free(buf);
	ret.result = RESULT_ERR;
	return ret;
}

enum result path_add_coll(struct gs *gs, struct cpnt_coll *coll)
{
	{
		struct hset_result blocked = path_find_blocked(&gs->path, coll);
		if (blocked.result == RESULT_ERR) return RESULT_ERR;
		coll->blocked = blocked.value;
	}



	coll->nodes = bset_create_null();

	return RESULT_OK;
}

void path_rm_coll(struct gs *gs, struct cpnt_coll coll)
{
	(void)gs;

	cpnt_coll_destroy(coll);
}