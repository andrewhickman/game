#include "graph.h"
#include "set.h"
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
	return node < graph->cap && !set_is_null(&graph->buf[node].set);
}

struct set_iter graph_neighbours(struct graph const *graph, unsigned node)
{
	ASSERT(graph_contains(graph, node));
	return set_iter(&graph->buf[node].set);
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
		buf[i].set = set_create_null();
	}
	graph->buf = buf;
	graph->cap = cap;
	
	return RESULT_OK;
}

unsigned graph_add_node(struct graph *graph, struct vec data)
{
	for (; graph->start != graph->cap; ++graph->start) {
		if (set_is_null(&graph->buf[graph->start].set)) {
			graph->buf[graph->start].set = set_create_empty();
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
	struct set edges = set_take(&graph->buf[node].set);
	struct set_iter iter = set_iter(&edges);
	struct set_iter_result result;

	while (!(result = set_iter_next(&iter)).finished) {
		set_remove(&graph->buf[result.value].set, node);
	}

	if (node < graph->start) graph->start = node;
	--graph->len;

	set_destroy(edges);
}

void graph_add_edge(struct graph *graph, unsigned a, unsigned b)
{
	ASSERT(graph_contains(graph, a) && graph_contains(graph, b));
	set_insert(&graph->buf[a].set, b);
	set_insert(&graph->buf[b].set, a);
}

void graph_rm_edge(struct graph *graph, unsigned a, unsigned b)
{
	ASSERT(graph_contains(graph, a) && graph_contains(graph, b));
	set_insert(&graph->buf[a].set, b);
	set_insert(&graph->buf[b].set, a);
}

void graph_destroy(struct graph graph)
{
	unsigned i;
	for (i = 0; i != graph.cap; ++i) {
		set_destroy(graph.buf[i].set);
	}
	free(graph.buf);
}