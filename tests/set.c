#include "error.h"
#include "set.h"

void test_set_basic()
{
	struct set set = set_create_empty();
	ASSERT(set_insert(&set, 3) == SET_INSERTED);
	ASSERT(set_insert(&set, 3) == SET_WAS_FILLED);
	ASSERT(set_contains(&set, 3));
	ASSERT(set_insert(&set, 4) == SET_INSERTED);
	ASSERT(set_insert(&set, 4) == SET_WAS_FILLED);
	ASSERT(set_contains(&set, 4));
	ASSERT(set_insert(&set, 400) == SET_INSERTED);
	ASSERT(set_insert(&set, 400) == SET_WAS_FILLED);
	ASSERT(set_contains(&set, 400));
	set_destroy(set);
}

void test_set_null()
{
	struct set null = set_create_null();
	ASSERT(set_is_null(&null));
}

void test_set_remove()
{
	struct set set = set_create_empty();

	ASSERT(set_insert(&set, 1) == SET_INSERTED);
	ASSERT(set_remove(&set, 1));

	ASSERT(set_insert(&set, 100) == SET_INSERTED);
	ASSERT(set_remove(&set, 100));

	ASSERT(set_insert(&set, 1000) == SET_INSERTED);
	ASSERT(set_remove(&set, 1000));

	set_destroy(set);
}

void test_set_iter()
{
	struct set set = set_create_empty();
	struct set_iter iter;
	struct set_iter_result result;

	ASSERT(set_insert(&set, 2) == SET_INSERTED);
	ASSERT(set_insert(&set, 3) == SET_INSERTED);
	ASSERT(set_insert(&set, 5) == SET_INSERTED);
	ASSERT(set_insert(&set, 7) == SET_INSERTED);
	ASSERT(set_insert(&set, 11) == SET_INSERTED);
	ASSERT(set_insert(&set, 130) == SET_INSERTED);
	ASSERT(set_insert(&set, 1700) == SET_INSERTED);

	iter = set_iter(&set);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 2);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 3);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 5);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 7);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 11);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 130);
	result = set_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 1700);
	result = set_iter_next(&iter);
	ASSERT(result.finished);
}

void test_set(void)
{
	test_set_basic();
	test_set_null();
	test_set_remove();
	test_set_iter();
}