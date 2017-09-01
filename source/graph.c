#include "graph.h"
#include "bset.h"
#include "util.h"

#include <stdlib.h>

struct graph graph_create(void)
{
	struct graph ret = { 0 };

	return ret;
}

struct vec graph_get_data(struct graph const *graph, unsigned node)
{
	return graph->buf[node].data;
}

bool graph_contains(struct graph const *graph, unsigned node)
{
	return node < graph->cap && !bset_is_null(&graph->buf[node].edges);
}

struct bset_iter graph_neighbours(struct graph const *graph, unsigned node)
{
	ASSERT(graph_contains(graph, node));
	return bset_iter(&graph->buf[node].edges);
}

/* Reserve space for additional nodes. */
enum result graph_reserve(struct graph *graph, size_t add)
{
	struct graph_node *buf;
	size_t i, cap = graph->len + add;
	if (cap <= graph->cap) {
		return RESULT_OK;
	}
	cap = util_next_pow_2(cap - 1);
	buf = realloc(graph->buf, cap * sizeof(*buf));
	if (!buf) {
		LOG_ERROR("out of memory");
		return RESULT_ERR;
	}
	for (i = graph->cap; i != cap; ++i) {
		buf[i].edges = bset_create_null();
	}
	graph->buf = buf;
	graph->cap = cap;
	
	return RESULT_OK;
}

unsigned graph_add_node(struct graph *graph, struct vec data)
{
	for (; graph->start != graph->cap; ++graph->start) {
		if (bset_is_null(&graph->buf[graph->start].edges)) {
			graph->buf[graph->start].edges = bset_create_empty();
			graph->buf[graph->start].data = data;
			++graph->len;
			return graph->start++;
		}
	}
	/* Not enough space reserved, error out. */
	ASSERT(UNREACHABLE);
	return 0;
}

void graph_rm_node(struct graph *graph, unsigned node)
{
	struct bset edges = bset_take(&graph->buf[node].edges);
	struct bset_iter iter = bset_iter(&edges);
	struct bset_iter_result result;

	while (!(result = bset_iter_next(&iter)).finished) {
		bset_remove(&graph->buf[result.value].edges, node);
	}

	if (node < graph->start) graph->start = node;
	--graph->len;

	bset_destroy(edges);
}

void graph_add_edge(struct graph *graph, unsigned a, unsigned b)
{
	ASSERT(graph_contains(graph, a) && graph_contains(graph, b));
	bset_insert(&graph->buf[a].edges, b);
	bset_insert(&graph->buf[b].edges, a);
}

void graph_rm_edge(struct graph *graph, unsigned a, unsigned b)
{
	ASSERT(graph_contains(graph, a) && graph_contains(graph, b));
	bset_insert(&graph->buf[a].edges, b);
	bset_insert(&graph->buf[b].edges, a);
}

struct graph_edge_iter graph_edge_iter(struct graph const *graph)
{
	struct graph_edge_iter ret;

	ret.buf = graph->buf;
	ret.idx = 0;
	ret.cap = graph->cap;
	ret.iter = bset_iter_empty();

	return ret;
}

struct graph_edge_iter_result graph_edge_iter_next(struct graph_edge_iter *iter)
{
	struct graph_edge_iter_result ret;

	while (bset_iter_finished(&iter->iter) 
	       || (ret.value.lo = bset_iter_next(&iter->iter).value) 
	       >= (ret.value.hi = iter->idx - 1)) {
		if (iter->idx == iter->cap) {
			ret.finished = true;
			return ret;
		}
		iter->iter = bset_iter(&iter->buf[iter->idx++].edges);
	}
	ret.finished = false;

	return ret;
}

void graph_destroy(struct graph graph)
{
	unsigned i;
	for (i = 0; i != graph.cap; ++i) {
		bset_destroy(graph.buf[i].edges);
	}
	free(graph.buf);
}