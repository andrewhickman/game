#ifndef GRAPH_H
#define GRAPH_H

#include "error.h"
#include "vec.h"
#include "set.h"

#include <stddef.h>

struct graph {
	struct graph_node {
		struct set set;
		struct vec data;
	} *buf;
	size_t len, cap;
	unsigned start;
};

struct graph graph_create(void);

struct vec graph_get_data(struct graph const *, unsigned);
bool graph_contains(struct graph const *, unsigned);
struct set_iter graph_neighbours(struct graph const *, unsigned);

/* Reserve space for additional nodes. */
enum result graph_reserve(struct graph *, size_t);

unsigned graph_add_node(struct graph *, struct vec);
/* Remove a node and all its edges. */
void graph_rm_node(struct graph *, unsigned);

void graph_add_edge(struct graph *, unsigned a, unsigned b);
void graph_rm_edge(struct graph *, unsigned a, unsigned b);

void graph_destroy(struct graph);

#endif