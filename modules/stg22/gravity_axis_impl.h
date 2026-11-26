#ifndef STG22_GRAVITY_AXIS_IMPL_H
#define STG22_GRAVITY_AXIS_IMPL_H
#include <ninja.h>

#include "collision/collision.h"
#include "stg22/crazy_gadget.h"

/* the COLLISION_UP_ code of the collision checks' up axis, for gravity, a
 * unit vector. the up axis points against the component of gravity whose size
 * passes GRAVITY_ON_AXIS. where none does, it is COLLISION_UP_NEG_X. an
 * includer names the function by defining GADGET_UP_AXIS first */
#ifndef GADGET_UP_AXIS
#define GADGET_UP_AXIS gravity_up_axis
#endif
static int GADGET_UP_AXIS(NJS_VECTOR *gravity)
{
    int up = COLLISION_UP_NEG_X;
    /* @hack redundant: a test of x's component that sets up to the
     * COLLISION_UP_NEG_X it holds already */
    if (gravity->x > GRAVITY_ON_AXIS) {
        up = COLLISION_UP_NEG_X;
    }
    if (gravity->x < -GRAVITY_ON_AXIS) {
        up = COLLISION_UP_X;
    }
    if (gravity->y > GRAVITY_ON_AXIS) {
        up = COLLISION_UP_NEG_Y;
    }
    if (gravity->y < -GRAVITY_ON_AXIS) {
        up = COLLISION_UP_Y;
    }
    if (gravity->z > GRAVITY_ON_AXIS) {
        up = COLLISION_UP_NEG_Z;
    }
    if (gravity->z < -GRAVITY_ON_AXIS) {
        up = COLLISION_UP_Z;
    }
    return up;
}
#endif
