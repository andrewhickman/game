#include "graph.h"
#include "set.h"
#include "util.h"
#include "path.h"
#include "gs.h"

struct graph graph_create(void)
{
	struct graph ret = { 0 };

	return ret;
}

struct vec graph_get(struct graph const *graph, unsigned node)
{
	return graph->buf[node].data;
}

bool graph_contains(struct graph const *graph, unsigned node)
{
	return node < graph->cap && graph->buf[node].state;
}

struct set_iter graph_neighbours(struct graph const *graph, unsigned node)
{
	ASSERT(graph_contains(graph, node));
	return set_iter(&graph->buf[node].edges);
}

/* Reserve space for additional nodes. */
void graph_reserve(struct graph *graph, size_t add)
{
	size_t cap = graph->len + add;

	if (cap <= graph->cap) return;

	cap = util_next_pow_2(cap - 1);
	graph->buf = xrealloc(graph->buf, cap * sizeof(struct graph_node));

	for (; graph->cap != cap; ++graph->cap) {
		graph->buf[graph->cap].state = GRAPH_EMPTY;
	}
}

unsigned graph_add_node(struct graph *graph, struct set edges, struct vec data)
{
	struct set_iter iter;
	unsigned i;

	ASSERT(graph->start != graph->cap);
	while (graph->buf[graph->start].state) {
		++graph->start;
		ASSERT(graph->start != graph->cap);
	}

	iter = set_iter(&edges);
	while (set_iter_next(&iter, &i)) {
		ASSERT(graph_contains(graph, i));
		set_insert(&graph->buf[i].edges, graph->start);
	}

	graph->buf[graph->start].state = GRAPH_FILLED;
	graph->buf[graph->start].edges = edges;
	graph->buf[graph->start].data = data;
	++graph->len;
	return graph->start++;
}

void graph_rm_node(struct graph *graph, unsigned node)
{
	struct set_iter iter = set_iter(&graph->buf[node].edges);
	unsigned i;

	ASSERT(graph->buf[node].state);

	while (set_iter_next(&iter, &i)) {
		set_remove(&graph->buf[i].edges, node);
	}

	if (node < graph->start) graph->start = node;
	--graph->len;

	set_destroy(graph->buf[node].edges);
	graph->buf[node].state = GRAPH_EMPTY;
}

void graph_add_blocked(
	struct graph *graph,
	struct gs_world *gs,
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll
) {
	size_t i, j, k;
	for (i = 0; i != graph->cap; ++i) {
		struct set *set;
		struct vec a;

		if (!graph->buf[i].state) continue;

		set = &graph->buf[i].edges;
		a = graph->buf[i].data;
		for (j = 0; j != set->len; ++j) {
			unsigned block = ~set->buf[j];

			while (block) {
				struct vec b;
				unsigned mask = block - 1;

				k = j * SET_BLOCK_BITS + __builtin_ctz(block);
				if (graph_contains(graph, k) && i != k) {
					b = graph->buf[k].data;
					if (path_blocks(pos, draw, coll, a, b) 
					    && path_los(gs, a, b)) 
					{
						set_insert(&graph->buf[k].edges, i);
						set->buf[j] |= ~mask;
					}
				}
				block &= mask;
			}
		}
	}
}

void graph_rm_blocked(
	struct graph *graph, 
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll
) {
	size_t i, j, k;
	for (i = 0; i != graph->cap; ++i) {
		struct set *set;
		struct vec a;

		if (!graph->buf[i].state) continue;

		set = &graph->buf[i].edges;
		a = graph->buf[i].data;
		for (j = 0; j != set->len; ++j) {
			unsigned block = set->buf[j];

			while (block) {
				struct vec b;
				unsigned mask = block - 1;

				k = j * SET_BLOCK_BITS + __builtin_ctz(block);
				ASSERT(graph_contains(graph, k));
				ASSERT(i != k);
				b = graph->buf[k].data;
				if (path_blocks(pos, draw, coll, a, b)) {
					set_remove(&graph->buf[k].edges, i);
					set->buf[j] &= mask;
				}
				block &= mask;
			}
		}
	}
}

void graph_add_path_data(struct graph *graph, unsigned node, unsigned from, unsigned cost)
{
	ASSERT(graph_contains(graph, node));
	graph->buf[node].state = GRAPH_PM_DATA;
	graph->buf[node].from = from;
	graph->buf[node].cost = cost;
}

void graph_clear_path_data(struct graph *graph)
{
	unsigned i;
	for (i = 0; i != graph->cap; ++i) {
		if (graph->buf[i].state) graph->buf[i].state = GRAPH_FILLED;
	}
}

double graph_cost(struct graph const *graph, unsigned a, unsigned b)
{
	return vec_dist(graph_get(graph, a), graph_get(graph, b));
}

struct graph_node_iter graph_node_iter(struct graph const *graph)
{
	struct graph_node_iter ret;

	ret.buf = graph->buf;
	ret.idx = 0;
	ret.cap = graph->cap;

	return ret;
}

struct graph_node_iter graph_node_iter_empty(void)
{
	struct graph_node_iter ret = { 0 };

	return ret;
}

struct graph_node_iter_result graph_node_iter_next(struct graph_node_iter *iter)
{
	struct graph_node_iter_result ret;

	while (iter->idx != iter->cap) {
		if (iter->buf[iter->idx].state) {
			ret.finished = false;
			ret.node = iter->idx;
			ret.data = &iter->buf[iter->idx];
			++iter->idx;
			return ret;
		}
		++iter->idx;
	}

	ret.finished = true;
	return ret;
}

bool graph_edge_iter_finished(struct graph_node_iter const *iter)
{
	return iter->idx == iter->cap;
}

struct graph_edge_iter graph_edge_iter(struct graph const *graph)
{
	struct graph_edge_iter ret;

	ret.buf = graph->buf;
	ret.idx = 0;
	ret.cap = graph->cap;
	ret.iter = set_iter_empty();

	return ret;
}

bool graph_edge_iter_next(struct graph_edge_iter *iter, struct graph_edge *res)
{
	while (1) {
		if (!set_iter_finished(&iter->iter)) {
			unsigned i;
			set_iter_next(&iter->iter, &i);
			if (i < iter->idx - 1) {
				res->lo = i;
				res->hi = iter->idx - 1;
				return true;
			}
		}
		if (iter->idx == iter->cap) return false;
		while (!iter->buf[iter->idx].state) {
			if (++iter->idx == iter->cap) return false;
		}
		iter->iter = set_iter(&iter->buf[iter->idx++].edges);
	}
}

void graph_destroy(struct graph graph)
{
	unsigned i;
	for (i = 0; i != graph.cap; ++i) {
		if (graph.buf[i].state) set_destroy(graph.buf[i].edges);
	}
	xfree(graph.buf);
}