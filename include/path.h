#ifndef PATH_H
#define PATH_H

#include "cpnt.h"
#include "gs.h"

enum result path_add_coll(struct gs *gs, struct cpnt_coll *coll);
void path_rm_coll(struct gs *gs, struct cpnt_coll coll);

struct path_result {
	enum result result;
	struct cpnt_path value;
} path_map_find_path(struct gs *, struct vec start, struct vec end);

#endif