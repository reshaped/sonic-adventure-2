/* the record of a kind of car, and a wreck's record. the parked cars of
 * shared/parked_car.c and City Escape's cars of stg13/car.c share them */
#ifndef SHARED_CARS_H
#define SHARED_CARS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "system/angle.h"

struct collision_shape;

/* one term of a sum of sines: the sine of freq times a time, times amp */
struct wave {
    Angle freq;
    Float amp;
};

/* a kind of car. each source gives flags its own bits. no code reads
 * name */
struct car_kind {
    const char *name;
    /* at a depth below near_range, a City Escape car on a path draws
     * wreck_object in place of object. it does so only where its kind has an
     * object. the depth is how far the car lies ahead of the camera. no parked
     * car reads near_range */
    Float near_range;
    /* the wreck's model. a wreck's pos is the model's center, which it turns
     * about. in a City Escape kind, wreck_object's child and that child's
     * sibling are the wheels */
    NJS_TEXLIST *wreck_texlist;
    NJS_CNK_OBJECT *wreck_object;
    /* the car's model. in a parked kind with CAR_BEACONS, the object's child
     * is the car's light. the child's siblings are its beacons */
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    /* the degrees a City Escape car's wheels turn for each unit it moves */
    Sint32 wheel_turn;
    /* the car's shape_count shapes, then its wreck's. a parked car's wreck
     * takes one shape, and a City Escape car's wreck takes shape_count */
    struct collision_shape *shapes;
    int shape_count;
    /* half the shadow's size in x, and in z */
    Float shadow_x;
    Float unk_28;
    Float shadow_z;
    /* half the size of the box the car pushes the players out of */
    NJS_VECTOR box;
    /* a City Escape car's front lies front along its path from the car's place
     * on it. its back lies back along the path. the car heads from its back to
     * its front, and its pos is their midpoint */
    Float front;
    Float back;
    Uint32 flags;
    /* object as the source's build compiled it, or null */
    NJS_DIRECT_HEAD *build;
};

/* multiply the low half of ang by k in place, as a Sint16. it works through
 * two locals a caller must declare: value, a Float, and low_half, a
 * Sint16 * */
/* @hack type: the angle's low half scaled through a Sint16 view of it, where
 * masks would do */
#define SCALE_ANGLE(ang, k)                                                    \
    {                                                                          \
        low_half = (Sint16 *)&(ang);                                           \
        value = *low_half;                                                     \
        value *= (k);                                                          \
        *low_half = value;                                                     \
    }

/* a wreck's record, in its task's motion. pos and ang give the frame of the
 * floor the wreck's probe last found. the wreck's step meets the floor in
 * that frame. the frame's origin lies under the wreck, and its y axis is the
 * floor's normal */
struct wreck {
    NJS_VECTOR spd; /* in the world's axes */
    NJS_POINT3 pos;
    struct angles ang;
    struct angles spin; /* what each step adds to the work's ang */
    Sint32 steps; /* the steps it has made */
    struct surface_probe probe;
    Sint32 unseen; /* the draws in a row that found it out of view */
};

#endif
