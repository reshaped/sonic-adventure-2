/* the path list and the fog setting of Cannon's Core's two walker routes,
 * Eggman's and Tails's. each route reads the fog setting from its own fog file
 */
#ifndef SHARED_CORE_MECH_ROUTE_H
#define SHARED_CORE_MECH_ROUTE_H

#include "shared/fog.h"
#include "stage/path.h"

extern struct path *core_mech_paths[];
extern struct fog_setting core_mech_fog;

#endif
