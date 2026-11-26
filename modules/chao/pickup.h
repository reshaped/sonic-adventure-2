/* the shapes of an item a Chao can take: fruit, seeds, hats and Chaos
 * Drives. PICKUP_VOLUME is a cylinder and PICKUP_CONTACT a sphere.
 * PICKUP_LOWER is a cylinder whose id, kind, tells a Chao what the item is.
 * each PICKUP_*_SHAPES macro sets which shapes are on in one state */
#ifndef CHAO_PICKUP_H
#define CHAO_PICKUP_H
#include "collision/collision.h"

enum garden_pickup_shape {
    PICKUP_VOLUME,
    PICKUP_CONTACT,
    PICKUP_LOWER,
    PICKUP_SHAPES
};

#ifdef NONMATCHING
/* The first frame publishes the object. Until then its physical slot is
 * reserved, and destroying the task releases the reservation. */
struct task *garden_prepare_pickup(Uint16 category, int level,
                                   void (*init)(struct task *),
                                   const char *name,
                                   const struct collision_shape *shapes);
Bool garden_take_pickup_reservation(struct task *pickup);
#endif

enum garden_pickup_mode {
    PICKUP_DROP,
    PICKUP_REST,
    PICKUP_ROLL,
    PICKUP_HELD,
    PICKUP_CARRIED
};

#define GARDEN_PICKUP_SHAPES(push, radius, kind, y, height)                    \
    {                                                                          \
        {0,                                                                    \
         SHAPE_CYLINDER,                                                       \
         PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),                                      \
         0,                                                                    \
         SHAPE_HURTABLE | SHAPE_UNK_2000,                                      \
         {0, 1.5F, 0},                                                         \
         15,                                                                   \
         3,                                                                    \
         0,                                                                    \
         0,                                                                    \
         {0, 0, 0}},                                                           \
            {0,        SHAPE_SPHERE,                                           \
             (push),   SHAPE_GUARD_LEVEL(3),                                   \
             0,        {0, 1.2F, 0},                                           \
             (radius), 0,                                                      \
             0,        0,                                                      \
             {0, 0, 0}},                                                       \
        {                                                                      \
            (kind), SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),           \
                SHAPE_GUARD_LEVEL(3), 0, {0, (y), 0}, 4.2F, (height), 0, 0,    \
                {0, 0, 0}                                                      \
        }                                                                      \
    }

#define PICKUP_FREE_SHAPES(object)                                             \
    {                                                                          \
        collision_shape_on((object), PICKUP_VOLUME);                           \
        collision_shape_on((object), PICKUP_CONTACT);                          \
        collision_shape_off((object), PICKUP_LOWER);                           \
    }
#define PICKUP_HIDE_SHAPES(object)                                             \
    {                                                                          \
        collision_shape_off((object), PICKUP_VOLUME);                          \
        collision_shape_off((object), PICKUP_CONTACT);                         \
        collision_shape_off((object), PICKUP_LOWER);                           \
    }
#define PICKUP_HELD_SHAPES(object)                                             \
    {                                                                          \
        collision_shape_off((object), PICKUP_VOLUME);                          \
        collision_shape_off((object), PICKUP_CONTACT);                         \
        collision_shape_on((object), PICKUP_LOWER);                            \
    }
#define PICKUP_CARRIED_SHAPES(object)                                          \
    {                                                                          \
        collision_shape_on((object), PICKUP_VOLUME);                           \
        collision_shape_off((object), PICKUP_CONTACT);                         \
        collision_shape_off((object), PICKUP_LOWER);                           \
    }
#endif
