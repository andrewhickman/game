#include "hset.h"
#include "util.h"

#include <stdlib.h>

size_t hset_rot5(size_t n)
{
	return (n << 5) | (n >> (sizeof(n) * CHAR_BIT - 5));
}

#define KEY 0x27220A95u
size_t hset_hash(struct graph_edge edge)
{
	return (hset_rot5(edge.lo * KEY) ^ edge.hi) * KEY;
}

size_t hset_index(struct graph_edge edge, size_t len)
{
	return hset_hash(edge) & (len - 1);
}

#define EDGE_EQ(l, r) ((l).lo == (r).lo && (l).hi == (r).hi)

enum result hset_insert(struct hset *, struct graph_edge);

struct hset_result hset_create(struct graph_edge const *buf, size_t len)
{
	struct hset_result ret;
	size_t i;

	ret.value.len = util_next_pow_2(len);
	ret.value.buf = calloc(ret.value.len, sizeof(struct hset_bkt));
	if (!ret.value.buf) {
		LOG_ERROR("out of memory");
		goto fail_buf;
	}

	for (i = 0; i != len; ++i) {
		if (hset_insert(&ret.value, buf[i]) == RESULT_ERR) {
			LOG_CHAIN();
			goto fail_insert;
		}
	}

	ret.result = RESULT_OK;

	return ret;

fail_insert:
	hset_destroy(ret.value);
fail_buf:
	ret.result = RESULT_ERR;
	return ret;
}

enum result hset_insert(struct hset *hset, struct graph_edge edge)
{
	struct hset_bkt *bkt = hset->buf + hset_index(edge, hset->len);
	struct graph_edge *buf;

	switch (bkt->len++) {
	case 0:
		bkt->data.edge = edge;
		return RESULT_OK;
	case 1:
		buf = malloc(bkt->len * sizeof(*buf));
		if (!buf) goto fail;
		buf[0] = bkt->data.edge;
		buf[1] = edge;
		break;
	default:
		buf = realloc(bkt->data.buf, bkt->len * sizeof(*buf));
		if (!buf) goto fail;
		buf[bkt->len - 1] = edge;
	}
	bkt->data.buf = buf;
	return RESULT_OK;

fail:
	LOG_ERROR("out of memory");
	return RESULT_ERR;
}

bool hset_contains(struct hset *hset, struct graph_edge edge)
{
	struct hset_bkt *bkt = hset->buf + hset_index(edge, hset->len);
	size_t i;

	if (bkt->len == 1) return EDGE_EQ(edge, bkt->data.edge);

	for (i = 0; i != bkt->len; ++i) {
		if (EDGE_EQ(edge, bkt->data.buf[i])) return true;
	}

	return false;
}

struct hset_iter hset_iter(struct hset const *hset)
{
	struct hset_iter ret;

	ret.ptr = hset->buf;
	ret.end = hset->buf + hset->len;
	ret.bkt.len = 0;

	return ret;
}

struct hset_iter hset_iter_empty(void)
{
	struct hset_iter ret = { 0 };

	return ret;
}

struct hset_iter_result hset_iter_next(struct hset_iter *iter)
{
	struct hset_iter_result ret;

	while (!iter->bkt.len) {
		if (iter->ptr == iter->end) {
			ret.finished = true;
			return ret;
		}
		iter->bkt = *iter->ptr++;
	}
	ret.finished = false;
	if (iter->bkt.len-- == 1) {
		ret.value = iter->bkt.data.edge;
	} else {
		ret.value = iter->bkt.data.buf[iter->bkt.len];
	}

	return ret;
}

bool hset_iter_finished(struct hset_iter const *iter)
{
	return iter->ptr == iter->end && !iter->bkt.len;
}

void hset_destroy(struct hset hset)
{
	size_t i;
	for (i = 0; i != hset.len; ++i) {
		if (hset.buf[i].len > 1) free(hset.buf[i].data.buf);
	}
	free(hset.buf);
}