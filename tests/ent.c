#include "error.h"
#include "ent.h"

void test_es_basic(void) 
{
	unsigned i;
	struct ent_store es = ent_store_create(16);
	struct ent ent;

	ASSERT(es.len >= 16);

	for (i = 0; i < 32; ++i) {
		ent = ent_store_spawn(&es);
		ASSERT(ent.id == i);
		ASSERT(ent.gen == 1);
		ASSERT(i < es.len);
	}

	ent.id = 1;
	ent_store_kill(&es, ent);
	ASSERT(!ent_store_is_alive(&es, ent));
	ent.id = 2;
	ent_store_kill(&es, ent);
	ASSERT(!ent_store_is_alive(&es, ent));

	ent = ent_store_spawn(&es);
	ASSERT(ent.id == 1);
	ASSERT(ent.gen == 2);
	ent = ent_store_spawn(&es);
	ASSERT(ent.id == 2);
	ASSERT(ent.gen == 2);

	ent_store_destroy(es);
}

void test_es_iter(void)
{
	struct ent_store es = ent_store_create(4);
	struct ent ent0, ent1, ent2, ent3;
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	ent0 = ent_store_spawn(&es);
	ent_store_add_cpnt(&es, ent0, CPNT_POS | CPNT_VEL);
	ent1 = ent_store_spawn(&es);
	ent_store_add_cpnt(&es, ent1, CPNT_POS | CPNT_VEL | CPNT_DRAW);
	ent2 = ent_store_spawn(&es);
	ent_store_add_cpnt(&es, ent2, CPNT_HP);
	ent3 = ent_store_spawn(&es);
	ent_store_add_cpnt(&es, ent3, CPNT_POS | CPNT_DRAW);

	iter = ent_store_iter(&es, CPNT_POS | CPNT_VEL);

	ASSERT(ent_store_iter_next(&iter, &result));
	ASSERT(result.ent.id == ent0.id && result.ent.gen == ent0.gen);

	ASSERT(ent_store_iter_next(&iter, &result));
	ASSERT(result.ent.id == ent1.id && result.ent.gen == ent1.gen);

	ASSERT(!ent_store_iter_next(&iter, &result));
	ASSERT(!ent_store_iter_next(&iter, &result));
	ASSERT(!ent_store_iter_next(&iter, &result));

	ent_store_destroy(es);
}

void test_es(void)
{
	test_es_basic();
	test_es_iter();
}
