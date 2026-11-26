#include "collision/collision.h"
#include "player/player.h"
#include "stg35/layout.h"
#include "system/angle.h"

#include "stg35/kill_planes.h"

struct kill_area core_eggman_kill_areas[4] = {
#include "stg35/route_barriers/core_eggman_kill_areas.inc"
};

/* a barrier's four boxes, about the barrier's pos and turned by its ang.y. two
 * make a front wall with a gap in its middle. one closes each end */
#define WALL(x, z, width, depth)                                               \
    {0,                                                                        \
     SHAPE_BOX,                                                                \
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),                   \
     SHAPE_GUARD_LEVEL(3),                                                     \
     SHAPE_UNK_2000,                                                           \
     {(x), 8.5F, (z)},                                                         \
     (width),                                                                  \
     8.5F,                                                                     \
     (depth),                                                                  \
     0,                                                                        \
     {0, 0, 0}}
struct collision_shape eggman_barrier_walls[EGG_BARRIER_WALLS] = {
    WALL(48.0F, -23.0F, 27.5F, 2.0F), WALL(-48.0F, -23.0F, 27.5F, 2.0F),
    WALL(73.5F, 0.0F, 2.0F, 25.0F), WALL(-73.5F, 0.0F, 2.0F, 25.0F)};

/* cylinders along the tops of the four walls, which no code reads */
#define RAIL(x, z, length, pitch, roll)                                        \
    {                                                                          \
        0, SHAPE_CYLINDER2,                                                    \
            PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),            \
            SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {(x), 17.0F, (z)}, 2.0F,     \
            (length), 0.0F, 0.0F,                                              \
        {                                                                      \
            (pitch), 0, (roll)                                                 \
        }                                                                      \
    }
static struct collision_shape upper_rails[] = {
    RAIL(48.0F, -23.0F, 27.5F, 0, QUARTER_TURN),
    RAIL(-48.0F, -23.0F, 27.5F, 0, QUARTER_TURN),
    RAIL(73.5F, 0.0F, 25.0F, QUARTER_TURN, 0),
    RAIL(-73.5F, 0.0F, 25.0F, QUARTER_TURN, 0),
};
