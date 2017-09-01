#ifndef HSET_H
#define HSET_H

#include "error.h"
#include "graph.h"

#include <stddef.h>
#include <stdbool.h>

struct hset {
	struct hset_bkt {
		size_t len;
		union hset_bkt_data {
			struct graph_edge edge, *buf;
		} data;
	} *buf;
	size_t len;
};

struct hset_result {
	enum result result;
	struct hset value;
} hset_create(struct graph_edge const *buf, size_t len);

bool hset_contains(struct hset *, struct graph_edge);

struct hset_iter {
	struct hset_bkt *ptr, *end;
	struct hset_bkt bkt;
} hset_iter(struct hset const *), hset_iter_empty(void);

struct hset_iter_result {
	bool finished;
	struct graph_edge value;
} hset_iter_next(struct hset_iter *);
bool hset_iter_finished(struct hset_iter const *);

void hset_destroy(struct hset);

#endif