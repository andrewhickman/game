#include "gamestate/gs.h"

struct gs_result gs_create(void)
{
	struct gs_result ret;

	{
		struct gs_entities_result entities = gs_entities_create(32);
		if (entities.result == RESULT_ERR) goto fail_entities;
		ret.value.entities = entities.value;
	}

	{
		struct gs_cpnt_result cpnt;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_pos));
		if (cpnt.result == RESULT_ERR) goto fail_pos;
		ret.value.pos = cpnt.value;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_vel));
		if (cpnt.result == RESULT_ERR) goto fail_vel;
		ret.value.vel = cpnt.value;
		
		cpnt = gs_cpnt_create(32, sizeof(struct gs_draw));
		if (cpnt.result == RESULT_ERR) goto fail_draw;
		ret.value.draw = cpnt.value;
	}

	ret.result = RESULT_OK;
	return ret;

fail_draw:
	gs_cpnt_destroy(ret.value.vel);
fail_vel:
	gs_cpnt_destroy(ret.value.pos);
fail_pos:
	gs_entities_destroy(ret.value.entities);
fail_entities:
	ret.result = RESULT_ERR;
	return ret;
}

enum result gs_update(struct gs *gs)
{
	(void)gs;
	return RESULT_OK;
}

void gs_destroy(struct gs gs)
{
	gs_cpnt_destroy(gs.draw);
	gs_cpnt_destroy(gs.vel);
	gs_cpnt_destroy(gs.pos);
	gs_entities_destroy(gs.entities);
}