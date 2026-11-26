/* Final Rush's rocks, BURST in its object list. each is a boulder or a shard
 * that rolls down the floor below it and bounces off walls */
#ifndef STG30_ROCKFALL_H
#define STG30_ROCKFALL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "system/angle.h"

struct task;

/* an angle of a rock's spin, and the angle's low 16 bits */
union rock_spin_angle {
    Angle angle;
    Sint16 low;
};

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC aligns NJS_MATRIX to 8, and a record keeps that alignment only under
 * pack(8). the matrix comes first, so no member moves. the record grows from
 * 348 bytes to 352, and only its allocation reads its size */
#pragma pack(push, 8)
#endif
#endif
/* a rock's record, which rockfall_attach puts in its task's motion */
struct rockfall {
    NJS_MATRIX rotation; /* the rock's turn */
    /* the turn that each step adds to rotation, as njRotateEx takes it. each
     * step also damps each angle's low 16 bits, read as a Sint16 */
    union {
        struct angles angles;
        union rock_spin_angle axis[3];
    } spin;
    NJS_VECTOR velocity; /* what each step adds to the work's pos */
    /* the floor's frame. its origin, floor, is a point of the floor.
     * floor_angles.x and .y stand the frame's y axis on the floor's normal, and
     * floor_angles.z is 0 */
    NJS_POINT3 floor;
    struct angles floor_angles;
    Sint32 unk_70;
    Sint32 sparks; /* the bursts of sparks left since the rock last landed */
    /* the size of the rock's kind. its x and z set how high above the floor the
     * rock rides */
    NJS_VECTOR size;
    /* the surfaces around probe.pos at the last search. sides[SURFACE_NEG_Y] is
     * the floor, and the rock bounces off the walls on the sides of x and z */
    struct surface_probe probe;
    /* the steps left in which each step brakes velocity.z */
    Sint32 brake_steps;
    Sint32 delay; /* the steps the rock has waited to move */
    Float drag; /* the share of velocity that each step keeps */
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct rockfall */
#pragma pack(pop)
#endif
#endif

#define ROCKFALL(task) ((struct rockfall *)(task)->motion.p)

/* a rock's kinds. a rock is the BOULDER or one of SHARD_KINDS shards from
 * FIRST_SHARD on */
#define ROCK_KINDS 4
#define BOULDER 0
#define FIRST_SHARD 1
#define SHARD_KINDS 3
/* a rock's modes, in its work's mode */
#define ROCK_WAITING 0
#define ROCK_MOVING 1
/* the level of a rock's task */
#define ROCK_LEVEL 2
/* the steps of a shard's timer as a boulder or the background starts it. it
 * takes SHARD_LIFE, and fewer than SHARD_LIFE_RANGE more at random */
#define SHARD_LIFE 300
#define SHARD_LIFE_RANGE 150.0F

/* compile the four kinds' models at buffer, with Ninja's fog off, and return
 * buffer past them. once a call has compiled all four, the rocks draw the
 * compiled models. until then they draw their chunk models, fog off */
Uint8 *rockfall_build(Uint8 *buffer);
/* set task up as a rock, and return TRUE. where the heap can't give the rock's
 * record, it returns FALSE and changes nothing.
 *
 * task needs a work, and player 0's record must exist, unchecked. pos places
 * the rock, and ang turns it. its kind is the whole part of scl.x, taken
 * unsigned, modulo ROCK_KINDS.
 *
 * a rock in ROCK_WAITING waits until player 0 or 1 is within scl.y plus 10 of
 * pos. it then takes ROCK_MOVING. a caller that sets ROCK_MOVING starts it at
 * once. in ROCK_MOVING, the rock first waits the whole part of scl.z steps,
 * and then moves.
 *
 * once the rock moves, a timer above 0 counts the steps it has left. a shard
 * with a timer goes sooner once more than 10 draws in a row leave it out. a
 * boulder whose timer is 0 sheds shards toward player 0. every rock goes once
 * player 0's z is below 7500 */
Bool rockfall_attach(struct task *task);
/* rockfall_attach(task) as a step. it is BURST's step in the object list. it
 * is also the first step of each rock the background or a boulder starts.
 * where the call returns FALSE, the step runs again at task's next step */
void rockfall_init(struct task *task);
/* the four kinds' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *rockfall_texlists[];
/* the rocks that rockfall_attach has set up and no destructor has freed */
extern int rockfall_count;

#endif
