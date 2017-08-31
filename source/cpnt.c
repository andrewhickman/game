#include "cpnt.h"

#include <stdlib.h>

void cpnt_coll_destroy(struct cpnt_coll coll)
{
	bset_destroy(coll.nodes);
	hset_destroy(coll.blocked);
}

void cpnt_path_destroy(struct cpnt_path path)
{
	free(path.buf);	
}