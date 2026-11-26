#include <ninja.h>

#include "stg35/layout.h"
#include "system/angle.h"

NJS_POINT3 eggman_barrier_positions[EGG_ROUTE_BARRIERS] = {
#include "stg35/barrier_positions/eggman_barrier_positions.inc"
};
Angle eggman_barrier_headings[EGG_ROUTE_BARRIERS] = {
    -3 * QUARTER_TURN, 0, HALF_TURN, 0, HALF_TURN, 0, -QUARTER_TURN};
