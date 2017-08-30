#include "error.h"
#include "ent.h"

struct ent spawn(struct ent_store *es) {
	struct ent_result er = ent_store_spawn(es);
	ASSERT(er.result == RESULT_OK);
	return er.value;
}

void test_es_basic(void) 
{
	unsigned i;
	struct ent_store es;
	struct ent ent;

	{
		struct ent_store_result esr = ent_store_create(16);
		ASSERT(esr.result == RESULT_OK);
		es = esr.value;
	}
	ASSERT(es.len >= 16);

	for (i = 0; i < 32; ++i) {
		ent = spawn(&es);
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

	ent = spawn(&es);
	ASSERT(ent.id == 1);
	ASSERT(ent.gen == 2);
	ent = spawn(&es);
	ASSERT(ent.id == 2);
	ASSERT(ent.gen == 2);

	ent_store_destroy(es);
}

void test_es_iter(void)
{
	struct ent_store es;
	struct ent ent0, ent1, ent2, ent3;
	struct ent_store_iter iter;
	struct ent_store_iter_result result;

	{
		struct ent_store_result esr = ent_store_create(4);
		ASSERT(esr.result == RESULT_OK);
		es = esr.value;
	}

	ent0 = spawn(&es);
	ent_store_add_cpnt(&es, ent0, CPNT_POS | CPNT_VEL);
	ent1 = spawn(&es);
	ent_store_add_cpnt(&es, ent1, CPNT_POS | CPNT_VEL | CPNT_DRAW);
	ent2 = spawn(&es);
	ent_store_add_cpnt(&es, ent2, CPNT_HP);
	ent3 = spawn(&es);
	ent_store_add_cpnt(&es, ent3, CPNT_POS | CPNT_DRAW);

	iter = ent_store_iter(&es, CPNT_POS | CPNT_VEL);

	result = ent_store_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.ent.id == ent0.id && result.ent.gen == ent0.gen);

	result = ent_store_iter_next(&iter);
	ASSERT(!result.finished);
	ASSERT(result.ent.id == ent1.id && result.ent.gen == ent1.gen);

	result = ent_store_iter_next(&iter);
	ASSERT(result.finished);

	ent_store_destroy(es);
}

void test_es(void)
{
	test_es_basic();
	test_es_iter();
}
