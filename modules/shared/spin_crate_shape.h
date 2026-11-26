/* the spinning crates' collision sphere and their turn rates */
#ifndef SHARED_SPIN_CRATE_SHAPE_H
#define SHARED_SPIN_CRATE_SHAPE_H

#include "collision/collision.h"
#include "system/angle.h"

#define CRATE_PUSH (PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS)
#define CRATE_SHAPE                                                            \
    {0,                                                                        \
     SHAPE_SPHERE,                                                             \
     CRATE_PUSH | PUSH_FIRM(CRATE_PUSH),                                       \
     SHAPE_GUARD_LEVEL(1),                                                     \
     SHAPE_HURT_NO_PUSH | SHAPE_HURTABLE,                                      \
     {0.0F, 0.0F, 0.0F},                                                       \
     13.0F,                                                                    \
     0.0F,                                                                     \
     10.0F,                                                                    \
     0.0F,                                                                     \
     {0, 0, 0}}

/* what a crate turns about x, y and z a step, for each unit of scl.x */
#define CRATE_TURN_X (0.1F * ANGLE_PER_DEGREE)
#define CRATE_TURN_Y (0.8F * ANGLE_PER_DEGREE)
#define CRATE_TURN_Z ANGLE_PER_DEGREE
#endif
