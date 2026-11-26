#include "boss_last1/weak_point.h"
#include "collision/collision.h"

struct collision_shape biolizard_upper_cylinder = {
    0,
    SHAPE_CYLINDER,
    PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_ALL | SHAPE_GUARD_LEVEL(3),
    SHAPE_IGNORE_OBJECTS | SHAPE_HURTABLE,
    {0, 0, 0},
    65,
    8,
    65,
    0,
    {0, 0, 0}};
