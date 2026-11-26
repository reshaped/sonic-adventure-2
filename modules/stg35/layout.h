#ifndef STG35_LAYOUT_H
#define STG35_LAYOUT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "player/player.h"

#define EGG_ROUTE_BARRIERS 7
#define EGG_BARRIER_WALLS 4

extern struct kill_area core_eggman_kill_areas[];
extern struct collision_shape eggman_barrier_walls[EGG_BARRIER_WALLS];
extern NJS_POINT3 eggman_barrier_positions[EGG_ROUTE_BARRIERS];
extern Angle eggman_barrier_headings[EGG_ROUTE_BARRIERS];

#endif
