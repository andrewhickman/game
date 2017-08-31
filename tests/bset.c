#include "error.h"
#include "bset.h"

void test_bset_basic()
{
	struct bset bset = bset_create_empty();
	ASSERT(bset_insert(&bset, 3) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 3) == SET_WAS_FILLED);
	ASSERT(bset_contains(&bset, 3));
	ASSERT(bset_insert(&bset, 4) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 4) == SET_WAS_FILLED);
	ASSERT(bset_contains(&bset, 4));
	ASSERT(bset_insert(&bset, 400) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 400) == SET_WAS_FILLED);
	ASSERT(bset_contains(&bset, 400));
	bset_destroy(bset);
}

void test_bset_null()
{
	struct bset null = bset_create_null();
	ASSERT(bset_is_null(&null));
}

void test_bset_remove()
{
	struct bset bset = bset_create_empty();

	ASSERT(bset_insert(&bset, 1) == SET_INSERTED);
	ASSERT(bset_remove(&bset, 1));

	ASSERT(bset_insert(&bset, 100) == SET_INSERTED);
	ASSERT(bset_remove(&bset, 100));

	ASSERT(bset_insert(&bset, 1000) == SET_INSERTED);
	ASSERT(bset_remove(&bset, 1000));

	bset_destroy(bset);
}

void test_bset_iter()
{
	struct bset bset = bset_create_empty();
	struct bset_iter iter;
	struct bset_iter_result result;

	ASSERT(bset_insert(&bset, 2) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 3) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 5) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 7) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 11) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 130) == SET_INSERTED);
	ASSERT(bset_insert(&bset, 1700) == SET_INSERTED);

	iter = bset_iter(&bset);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 2);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 3);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 5);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 7);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 11);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 130);
	result = bset_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.value == 1700);
	result = bset_iter_next(&iter);
	ASSERT(result.finished);

	bset_destroy(bset);
}

void test_bset(void)
{
	test_bset_basic();
	test_bset_null();
	test_bset_remove();
	test_bset_iter();
}