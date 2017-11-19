#include "ent.h"

bool ent_eq(struct ent l, struct ent r)
{
	return l.id == r.id && l.gen == r.gen;
}

struct ent_store ent_store_create(size_t len)
{
	struct ent_store ret;

	ret.buf = xcalloc(len, sizeof(struct ent_data));
	ret.len = len;
	ret.start = 0;

	return ret;
}

struct ent ent_store_spawn(struct ent_store *es)
{
	struct ent ret;

	for (; es->start != es->len; ++es->start) {
		if (es->buf[es->start].gen <= 0) goto exit;
	}
	/* Failed to find space, resize. */
	{
		size_t len = es->len ? 2 * es->len : 1;
		es->buf = xrealloc(es->buf, len * sizeof(*es->buf));
		for (; es->len != len; ++es->len) {
			es->buf[es->len].cpnt = CPNT_NONE;
			es->buf[es->len].gen = 0;
		}
	}
exit:
	ret.gen = es->buf[es->start].gen = 1 - es->buf[es->start].gen;
	ret.id = es->start++;
	return ret;
}

enum cpnt ent_store_kill(struct ent_store *es, struct ent ent)
{
	if (ent_store_is_alive(es, ent)) {
		es->buf[ent.id].gen = -es->buf[ent.id].gen;
		if (ent.id < es->start) {
			es->start = ent.id;
		}
		return es->buf[ent.id].cpnt;
	}

	return CPNT_NONE;
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

bool ent_store_test_cpnt(struct ent_store const *es, struct ent ent, enum cpnt cpnt)
{
	ASSERT(cpnt);
	if (ent_store_is_alive(es, ent)) {
		return (es->buf[ent.id].cpnt & cpnt) == cpnt;
	} else {
		return false;
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

bool ent_store_iter_next(struct ent_store_iter *iter, struct ent_store_iter_result *res)
{
	while (iter->id != iter->len) {
		struct ent_data data = iter->buf[iter->id];
		if ((data.cpnt & iter->cpnt) == iter->cpnt && data.gen > 0) {
			res->ent.id = iter->id++;
			res->ent.gen = data.gen;
			res->cpnt = data.cpnt;
			return true;
		}
		++iter->id;
	}

	return false;
}

void ent_store_destroy(struct ent_store es)
{
	xfree(es.buf);
}
