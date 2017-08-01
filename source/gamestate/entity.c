#include "gamestate/entity.h"

#include <stdlib.h>

struct gs_entities_result gs_entities_create(size_t len)
{
	struct gs_entities_result ret;

	ret.value.buf = malloc(len * sizeof(enum gs_cpnt));
	if (!ret.value.buf) {
		LOG_ERROR("out of memory");
		ret.result = RESULT_ERR;
		return ret;
	}
	ret.value.len = len;
	ret.value.start = 0;
	ret.result = RESULT_OK;
	return ret;
}

struct gs_entities_new_result gs_entities_new(struct gs_entities *es)
{
	struct gs_entities_new_result ret;

	for (; es->start != es->len; ++es->start) {
		if (es->buf[es->start] == GS_CPNT_NONE) goto exit;
	}
	/* Failed to find space, resize. */
	{
		size_t i, len = es->len ? 2 * es->len : 1;
		enum gs_cpnt *buf = realloc(es->buf, len * sizeof(*buf));
		if (!buf) {
			LOG_ERROR("out of memory");
			ret.result = RESULT_ERR;
			return ret;
		}
		for (i = es->len; i != len; ++i) {
			buf[i] = GS_CPNT_NONE;
		}
		es->buf = buf;
		es->len = len;
	}
exit:
	ret.result = RESULT_OK;
	ret.id = es->start;
	return ret;
}

void gs_entities_kill(struct gs_entities *es, unsigned id)
{
	es->buf[id] = GS_CPNT_NONE;
	if (id < es->start) {
		es->start = id;
	}
}

void gs_entities_destroy(struct gs_entities es)
{
	free(es.buf);
}
