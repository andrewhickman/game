#include "cpnt.h"

void cpnt_coll_destroy(struct cpnt_coll coll)
{
	set_destroy(coll.nodes);
}

void cpnt_path_destroy(struct cpnt_path path)
{
	xfree(path.buf);	
}