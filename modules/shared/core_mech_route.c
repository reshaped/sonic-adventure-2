#include "shared/core_mech_route.h"
#include "shared/fog.h"
#include "stage/path.h"

/* the walkers' routes have no paths, so path_create_manager starts no path's
 * task from this list */
struct path *core_mech_paths[] = {NULL};
struct fog_setting core_mech_fog;
