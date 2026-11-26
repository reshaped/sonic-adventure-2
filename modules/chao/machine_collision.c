#include "chao/machine_collision.h"
#include "collision/collision.h"

#define MACHINE_SOLID PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
struct collision_shape machine_shapes[MACHINE_SHAPES] = {
#include "chao/machine_collision/machine_shapes.inc"
};
