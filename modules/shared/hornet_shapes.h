/* Collision shapes shared by the Hornet and Akahige. */
#ifndef SHARED_HORNET_SHAPES_H
#define SHARED_HORNET_SHAPES_H

#include "collision/collision.h"

/* the levels of the hornet's and the bombs' bodies: attack level 1, hurting
 * players */
#define BODY_LEVELS (SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS)

#define HORNET_SHAPES 2

/* clang-format off */
/* a hornet's shapes: its body, then a smaller one farther back */
static struct collision_shape hornet_shapes[HORNET_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_OBJECTS | SHAPE_HURTABLE,
     {-10.0F, 0.0F, 0.0F}, 12.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-21.0F, 0.0F, 0.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* six more shapes like the hornet's second */
static struct collision_shape dat_hornet_0[6] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-11.0F, 13.5F, 0.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-11.0F, -7.0F, 13.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-11.0F, -7.0F, -13.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-9.0F, -12.5F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-9.0F, 8.0F, 12.5F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, BODY_LEVELS,
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_HURTABLE,
     {-9.0F, 8.0F, -12.5F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#endif
