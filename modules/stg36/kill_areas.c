#include "player/player.h"
#include "stg36/route.h"

#include "stg36/kill_planes.h"

struct kill_area core_tails_kill_areas[4] = {
#include "stg36/kill_areas/core_tails_kill_areas.inc"
};
