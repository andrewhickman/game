#include "error.h"
#include "set.h"

void test_set_basic()
{
	struct set set = set_create_empty();
	ASSERT(set_insert(&set, 3));
	ASSERT(!set_insert(&set, 3));
	ASSERT(set_contains(&set, 3));
	ASSERT(set_insert(&set, 4));
	ASSERT(!set_insert(&set, 4));
	ASSERT(set_contains(&set, 4));
	ASSERT(set_insert(&set, 400));
	ASSERT(!set_insert(&set, 400));
	ASSERT(set_contains(&set, 400));
	set_destroy(set);
}

void test_set_remove()
{
	struct set set = set_create_empty();

	ASSERT(set_insert(&set, 1));
	ASSERT(set_remove(&set, 1));

	ASSERT(set_insert(&set, 100));
	ASSERT(set_remove(&set, 100));

	ASSERT(set_insert(&set, 1000));
	ASSERT(set_remove(&set, 1000));

	set_destroy(set);
}

void test_set_iter()
{
	struct set set = set_create_empty();
	struct set_iter iter;
	size_t result;

	ASSERT(set_insert(&set, 2));
	ASSERT(set_insert(&set, 3));
	ASSERT(set_insert(&set, 5));
	ASSERT(set_insert(&set, 7));
	ASSERT(set_insert(&set, 11));
	ASSERT(set_insert(&set, 130));
	ASSERT(set_insert(&set, 1700));

	iter = set_iter(&set);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 2);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 3);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 5);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 7);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 11);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 130);
	ASSERT(set_iter_next(&iter, &result));
	ASSERT(result == 1700);
	ASSERT(!set_iter_next(&iter, &result));
	ASSERT(!set_iter_next(&iter, &result));
	ASSERT(set_iter_finished(&iter));

	iter = set_iter_empty();
	ASSERT(!set_iter_next(&iter, &result));
	ASSERT(!set_iter_next(&iter, &result));
	ASSERT(set_iter_finished(&iter));

	set_destroy(set);
}

void test_set(void)
{
	test_set_basic();
	test_set_remove();
	test_set_iter();
}