#include "hset.h"
#include "error.h"

#define NUM_EDGES 10

struct graph_edge const edges[NUM_EDGES] = {
	{ 0, 0 },
	{ 0, 1 },
	{ 1, 0 },
	{ 0, 2 },
	{ 1, 1 },
	{ 2, 0 },
	{ 0, 3 },
	{ 1, 2 },
	{ 2, 1 },
	{ 3, 0 }
};

enum result hset_insert(struct hset *, struct graph_edge);

void test_hset_1()
{
	struct hset set;
	size_t i;

	{
		struct hset_result r = hset_create(NULL, 0);
		ASSERT(r.result == RESULT_OK);
		set = r.value;
	}

	for (i = 0; i != NUM_EDGES; ++i) {
		ASSERT(!hset_contains(&set, edges[i]));
	}

	for (i = 0; i != NUM_EDGES; ++i) {
		ASSERT(!hset_contains(&set, edges[i]));
		ASSERT(hset_insert(&set, edges[i]) == RESULT_OK);
		ASSERT(hset_contains(&set, edges[i]));
	}

	for (i = 0; i != NUM_EDGES; ++i) {
		ASSERT(hset_contains(&set, edges[i]));
	}

	{
		size_t cnt = 0;
		for (i = 0; i != set.len; ++i) {
			cnt += set.buf[i].len;
		}
		ASSERT(cnt == 10);
	}	

	hset_destroy(set);
}

void test_hset_10()
{
	struct hset set;
	size_t i;

	{
		struct hset_result r = hset_create(edges, NUM_EDGES);
		ASSERT(r.result == RESULT_OK);
		set = r.value;
	}

	for (i = 0; i != NUM_EDGES; ++i) {
		ASSERT(hset_contains(&set, edges[i]));
	}

	{
		size_t cnt = 0;
		for (i = 0; i != set.len; ++i) {
			cnt += set.buf[i].len;
		}
		ASSERT(cnt == 10);
	}	

	hset_destroy(set);
}

void test_hset()
{
	test_hset_10();
	test_hset_1();
}