#include "path.h"

enum result path_add_coll(struct gs *gs, unsigned id, struct cpnt_coll *coll)
{
	(void)gs, (void)coll;
	SDL_Log("inserting entity %d into pathfinding", id);
	return RESULT_OK;
}

void path_rm_coll(struct gs *gs, unsigned id, struct cpnt_coll coll)
{
	(void)gs;

	cpnt_coll_destroy(coll);
	SDL_Log("removing entity %d from pathfinding", id);
}