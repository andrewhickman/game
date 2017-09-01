#ifndef GRAPH_H
#define GRAPH_H

#include "error.h"
#include "vec.h"
#include "bset.h"

#include <stddef.h>

struct graph {
	struct graph_node {
		struct bset edges;
		struct vec data;
	} *buf;
	size_t len, cap;
	unsigned start;
};

struct graph_edge {
	unsigned lo, hi;
};

struct graph graph_create(void);

struct vec graph_get_data(struct graph const *, unsigned);
bool graph_contains(struct graph const *, unsigned);
struct bset_iter graph_neighbours(struct graph const *, unsigned);

/* Reserve space for additional nodes. */
enum result graph_reserve(struct graph *, size_t);

unsigned graph_add_node(struct graph *, struct vec);
/* Remove a node and all its edges. */
void graph_rm_node(struct graph *, unsigned);

void graph_add_edge(struct graph *, unsigned a, unsigned b);
void graph_rm_edge(struct graph *, unsigned a, unsigned b);

struct graph_edge_iter {
	struct graph_node const *buf;
	size_t idx, cap;
	struct bset_iter iter;
} graph_edge_iter(struct graph const *);

struct graph_edge_iter_result {
	bool finished;
	struct graph_edge value;
} graph_edge_iter_next(struct graph_edge_iter *);

void graph_destroy(struct graph);

#endif