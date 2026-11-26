/* the lights of Eggman's and Tails's routes in Cannon's Core, and the palette
 * of their conduits' streams. the palette's colors make a cycle of 360 degrees,
 * 4 degrees to an entry */
#ifndef SHARED_CORE_CONDUIT_H
#define SHARED_CORE_CONDUIT_H
#include <sg_xpt.h>

#include "draw/light.h"

#define CORE_CONDUIT_COLORS 90
extern struct light core_mech_lights[2];
extern Uint16 core_conduit_palette[CORE_CONDUIT_COLORS];

#endif
