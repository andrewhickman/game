#include "ent.h"

#include <stdlib.h>

struct ent_store_result ent_store_create(size_t len)
{
	struct ent_store_result ret;

	ret.value.buf = calloc(len, sizeof(*ret.value.buf));
	if (len && !ret.value.buf) {
		LOG_ERROR("out of memory");
		ret.result = RESULT_ERR;
		return ret;
	}
	ret.value.len = len;
	ret.value.start = 0;
	ret.result = RESULT_OK;
	return ret;
}

struct ent_result ent_store_spawn(struct ent_store *es)
{
	struct ent_result ret;

	for (; es->start != es->len; ++es->start) {
		if (es->buf[es->start].gen <= 0) goto exit;
	}
	/* Failed to find space, resize. */
	{
		size_t i, len = es->len ? 2 * es->len : 1;
		struct ent_data *buf = realloc(es->buf, len * sizeof(*buf));
		if (!buf) {
			LOG_ERROR("out of memory");
			ret.result = RESULT_ERR;
			return ret;
		}
		for (i = es->len; i != len; ++i) {
			buf[i].cpnt = CPNT_NONE;
			buf[i].gen = 0;
		}
		es->buf = buf;
		es->len = len;
	}
exit:
	ret.result = RESULT_OK;
	ret.value.gen = es->buf[es->start].gen = 1 - es->buf[es->start].gen;
	ret.value.id = es->start++;
	return ret;
}

void ent_store_kill(struct ent_store *es, struct ent ent)
{
	if (ent_store_is_alive(es, ent)) {
		es->buf[ent.id].cpnt = CPNT_NONE;
		es->buf[ent.id].gen = -es->buf[ent.id].gen;
		if (ent.id < es->start) {
			es->start = ent.id;
		}
	}
}

void ent_store_add_cpnt(struct ent_store *es, struct ent ent, enum cpnt cpnt)
{
	ASSERT(ent_store_is_alive(es, ent));
	es->buf[ent.id].cpnt |= cpnt;
}

bool ent_store_rm_cpnt(struct ent_store *es, struct ent ent, enum cpnt cpnt)
{
	if (ent_store_is_alive(es, ent)) {
		es->buf[ent.id].cpnt &= ~cpnt;
		return true;
	} else {
		return false;
	}
}

enum cpnt ent_store_get_cpnt(struct ent_store const *es, struct ent ent)
{
	if (ent_store_is_alive(es, ent)) {
		return es->buf[ent.id].cpnt;
	} else {
		return CPNT_NONE;
	}
}

bool ent_store_is_alive(struct ent_store const *es, struct ent ent)
{
	return ent.gen == es->buf[ent.id].gen;
}

struct ent_store_iter ent_store_iter(struct ent_store const *es, enum cpnt cpnt)
{
	struct ent_store_iter ret;

	ret.buf = es->buf;
	ret.len = es->len;
	ret.id = 0;
	ret.cpnt = cpnt;

	return ret;
}

struct ent_store_iter_result ent_store_iter_next(struct ent_store_iter *iter)
{
	struct ent_store_iter_result ret;

	while (iter->id != iter->len) {
		struct ent_data data = iter->buf[iter->id];
		if ((data.cpnt & iter->cpnt) == iter->cpnt && data.gen > 0) {
			ret.finished = false;
			ret.ent.id = iter->id++;
			ret.ent.gen = data.gen;
			ret.cpnt = data.cpnt;
			return ret;
		}
		++iter->id;
	}

	ret.finished = true;
	return ret;
}

void ent_store_destroy(struct ent_store es)
{
	free(es.buf);
}
