/* the rings' shadow scale, collision shape and floor phase, which ring.h
 * declares */
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/ring.h"

static Float unused_ring_parameter = 50.0F;

Float ring_shadow_scale_x = 0.8F;

Float ring_shadow_scale_z = 0.4F;

/* clang-format off */
struct collision_shape ring_collision_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS), 0,
    SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_PICKUPS | SHAPE_IGNORE_UNK_9,
    {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

Sint32 ring_floor_phase = 0;
