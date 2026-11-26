/* the body of CGWOOD, CGIRON and SGOLID in Crazy Gadget's object list. each is
 * a box that falls along world_gravity. it bounces between its placed pos and
 * pos + scl, axis by axis. a player's attack breaks the wooden box. the iron
 * box needs the attacking player to have its character's upgrade, as
 * player_breaks_task says. the lid never breaks.
 *
 * wood_box.c, iron_box.c and lid.c each include this file. each first defines
 * GADGET_WOOD, GADGET_IRON or GADGET_LID, and the names of its init and its
 * texture lists. it defines FALL_ACCELERATION too, its pull a step */
#ifndef STG22_FLOATING_BOX_IMPL_H
#define STG22_FLOATING_BOX_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/dust.h"
#ifdef GADGET_WOOD
#include "shared/wood_box.h"
#else
#include "shared/iron_box.h"
#endif
#include "sound/sound.h"
#include "stg22/breakables.h"
#include "stg22/crazy_gadget.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the box's record, at task->extra. home is its placed pos */
struct floating_box {
    NJS_POINT3 home;
    NJS_VECTOR speed;
#ifdef NONMATCHING
    /* this side stores the bounce's share in the record, the plain form of the
     * hack at RESTITUTION */
    Float restitution;
#endif
};

#define BOX(task) ((struct floating_box *)(task)->extra.p)
/* the share of its speed the box keeps when it bounces. its init picks it from
 * BOUNCE up to BOUNCE + BOUNCE_RANGE */
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define RESTITUTION(task) (BOX(task)->restitution)
#else
/* @hack type: the bounce's share kept in the task's unk_3c, where a field of
 * struct floating_box would do */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RESTITUTION(task) (*(Float *)&(task)->unk_3c)
#endif
#define BOUNCE 0.1F
#define BOUNCE_RANGE 0.02F
/* the box is 2 * HALF_SIZE on a side, and stands on its pos */
#define HALF_SIZE 10.0F
/* a box's modes, in its work's submode. a MODE_BROKEN box's pieces have yet
 * to fly. a MODE_GONE box's pieces have flown */
enum box_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };
/* the box starts START_DROP along world_gravity from its placed pos, held
 * within its range */
#define START_DROP 1000.0F
/* each step the box keeps SPEED_DRAG of its speed, and gains FALL_ACCELERATION
 * along world_gravity. a bounce at more than KNOCK_SPEED knocks. a knock plays
 * KNOCK_SOUND and raises dust at the box's face along gravity. it shakes the
 * pad of a player within KNOCK_RANGE of the box */
#define SPEED_DRAG 0.99F
#define KNOCK_SPEED 2.0F
#define KNOCK_SOUND 16
#define KNOCK_VOLUME 50
#define KNOCK_RANGE 150.0F
#define KNOCK_POWER 3
#define KNOCK_FREQ 25
#define DUST_SCALE 8.0F
#define DUST_DENSITY 1.2F
#define DUST_SPEED 0.5F
#define SOUND_PRIORITY 1
#define SOUND_DURATION 80
/* a broken box plays BREAK_SOUND. BREAK_PUFFS puffs of dust burst from its
 * center. a puff's radius runs from PUFF_RADIUS up to PUFF_RADIUS +
 * PUFF_RADIUS_RANGE. its speed runs from PUFF_SPEED up to PUFF_SPEED + 1. its
 * size runs from PUFF_SCALE up to PUFF_SCALE + PUFF_SCALE_RANGE. its pieces
 * fly once its timer passes PIECES_STEP */
#ifdef GADGET_WOOD
#define BREAK_SOUND 0x1010
#else
#define BREAK_SOUND 0x1011
#endif
#define FULL_VOLUME 127
#define BREAK_PUFFS 14
#define PUFF_RADIUS 7.0F
#define PUFF_RADIUS_RANGE 3.0F
#define PUFF_SPEED 0.5F
#define PUFF_SCALE 2.5F
#define PUFF_SCALE_RANGE 4.0F
#define PIECES_STEP 2
/* a piece flies at a speed from PIECE_SPEED up to PIECE_SPEED +
 * PIECE_SPEED_RANGE */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F
/* the work's tick holds the low byte of ang.z, the box's animal byte of
 * object/animal.h. its ANIMAL_CHANCE_BITS hold the chance, in percent, that a
 * broken box releases its animal. a chance of ANIMAL_CERTAIN or more is
 * certain */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CERTAIN 10
/* the box turns the chance in percent into a share of 1 by two
 * multiplications by TENTH */
#define TENTH 0.1F
/* a kind of piece a box breaks into, and how many of it */
struct box_piece {
    NJS_TEXLIST *textures;
    NJS_CNK_MODEL *model;
    int count;
};
#define BOX_PIECES 3
#include "stg22/art/floating_box.h"

#ifdef GADGET_WOOD
#define CHIP_TEXTURE(index) wood_box_piece##index##_texlist
#define CHIP_MODEL(index) wood_box_piece##index##_model
#else
#define CHIP_TEXTURE(index) iron_box_piece##index##_texlist
#define CHIP_MODEL(index) iron_box_piece##index##_model
#endif
NJS_TEXLIST *FLOATING_TEXTURES[] = {&floating_box_texlist, &CHIP_TEXTURE(1),
                                    &CHIP_TEXTURE(2), &CHIP_TEXTURE(3), NULL};
static struct box_piece pieces[BOX_PIECES] = {
    {&CHIP_TEXTURE(1), &CHIP_MODEL(1), 16},
    {&CHIP_TEXTURE(2), &CHIP_MODEL(2), 4},
    {&CHIP_TEXTURE(3), &CHIP_MODEL(3), 4}};
#ifdef GADGET_LID
#define BOX_FLAGS SHAPE_HURTABLE
#else
#define BOX_FLAGS (SHAPE_HURTABLE | SHAPE_HURT_NO_PUSH)
#endif
/* clang-format off */
static struct collision_shape box_shape[1] = {
    {0, SHAPE_AXIS_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(1),
     BOX_FLAGS, {0, HALF_SIZE, 0}, HALF_SIZE, HALF_SIZE, HALF_SIZE, 0,
     {0, 0, 0}},
};
/* clang-format on */

static void box_destroy(struct task *task);
static void box_step(struct task *task);
static void box_draw(struct task *task);

/* the box's range runs from home to home + scl, axis by axis. a component of
 * scl can be negative */
#define CLAMP_LOW(axis, bound)                                                 \
    if (work->pos.axis < (bound)) {                                            \
        work->pos.axis = (bound);                                              \
    }
#define CLAMP_HIGH(axis, bound)                                                \
    if (work->pos.axis > (bound)) {                                            \
        work->pos.axis = (bound);                                              \
    }
#define CLAMP_AXIS(axis)                                                       \
    if (work->scl.axis > 0.0F) {                                               \
        CLAMP_LOW(axis, BOX(task)->home.axis)                                  \
        CLAMP_HIGH(axis, BOX(task)->home.axis + work->scl.axis)                \
    } else {                                                                   \
        CLAMP_HIGH(axis, BOX(task)->home.axis)                                 \
        CLAMP_LOW(axis, BOX(task)->home.axis + work->scl.axis)                 \
    }

void FLOATING_INIT(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct floating_box));
    if (!task->extra.p) {
        return;
    }
    task->draw = box_draw;
    task->step = box_step;
    task->destructor = box_destroy;
    work->submode = MODE_WHOLE;
    collision_init(task, box_shape, 1, COLLISION_OBJECT);
#ifndef GADGET_LID
    ANIMAL(work) = work->ang.z;
#endif
    BOX(task)->home = work->pos;
    work->pos.x += world_gravity.x * START_DROP;
    work->pos.y += world_gravity.y * START_DROP;
    work->pos.z += world_gravity.z * START_DROP;
#ifdef GADGET_WOOD
    RESTITUTION(task) = (((Float)rand() * RAND_SCALE) * BOUNCE_RANGE) + BOUNCE;
#else
    RESTITUTION(task) = BOUNCE + ((Float)rand() * (BOUNCE_RANGE * RAND_SCALE));
#endif
    CLAMP_AXIS(y)
    CLAMP_AXIS(x)
    CLAMP_AXIS(z)
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void box_destroy(register struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void box_knock(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    NJS_VECTOR gravity;
    struct angles ang;
#ifdef GADGET_WOOD
#ifdef NONMATCHING
#else
    Float *axis;
    Float *offset;
#endif
#endif
    int player;
    point = work->pos;
    point.y += HALF_SIZE;
    gravity = world_gravity;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    gravity.x *= HALF_SIZE;
    gravity.y *= HALF_SIZE;
    gravity.z *= HALF_SIZE;
    point.x += gravity.x;
    point.y += gravity.y;
    point.z += gravity.z;
#else
    /* @hack spelling: the scaling and the sum walked through pointers to the
     * vectors' fields, where the fields by name would do. the wooden box takes
     * each walk's start from the whole vector's address */
#ifdef GADGET_WOOD
    axis = (Float *)&gravity;
    *axis++ *= HALF_SIZE;
    *axis++ *= HALF_SIZE;
    *axis *= HALF_SIZE;
    offset = (Float *)&gravity;
    axis = &(&point)->x;
    *axis++ += *offset++;
    *axis++ += *offset++;
    *axis += *offset;
#else
    {
        Float *axis = &(&gravity)->x;
        *axis++ *= HALF_SIZE;
        *axis++ *= HALF_SIZE;
        *axis *= HALF_SIZE;
    }
    {
        Float *to = &(&point)->x;
        Float *from = &(&gravity)->x;
        *to++ += *from++;
        *to++ += *from++;
        *to += *from;
    }
#endif
#endif
    ang.x = world_gravity_ang_x;
    ang.z = world_gravity_ang_z;
    ang.y = work->ang.y;
    sound_play_at_for(KNOCK_SOUND, task, SOUND_PRIORITY, KNOCK_VOLUME,
                      SOUND_DURATION, &point);
    dust_short_rect(&point, HALF_SIZE, HALF_SIZE, &ang, DUST_SCALE,
                    DUST_DENSITY, DUST_SPEED);
    player = player_near(&work->pos, KNOCK_RANGE);
    if (player) {
        vibration_start(player - 1, KNOCK_POWER, KNOCK_FREQ, VIBRATION_UNTIMED);
    }
}

#define BOUNCE_LOW(axis, bound)                                                \
    if (work->pos.axis < (bound)) {                                            \
        work->pos.axis = (bound);                                              \
        if (BOX(task)->speed.axis < -KNOCK_SPEED) {                            \
            box_knock(task);                                                   \
        }                                                                      \
        BOX(task)->speed.axis =                                                \
            njAbs(BOX(task)->speed.axis) * RESTITUTION(task);                  \
    }
#define BOUNCE_HIGH(axis, bound)                                               \
    if (work->pos.axis > (bound)) {                                            \
        work->pos.axis = (bound);                                              \
        if (BOX(task)->speed.axis > KNOCK_SPEED) {                             \
            box_knock(task);                                                   \
        }                                                                      \
        BOX(task)->speed.axis =                                                \
            -(njAbs(BOX(task)->speed.axis) * RESTITUTION(task));               \
    }

#define BOUNCE_AXIS(axis)                                                      \
    if (work->scl.axis > 0.0F) {                                               \
        BOUNCE_LOW(axis, BOX(task)->home.axis)                                 \
        BOUNCE_HIGH(axis, BOX(task)->home.axis + work->scl.axis)               \
    } else {                                                                   \
        BOUNCE_HIGH(axis, BOX(task)->home.axis)                                \
        BOUNCE_LOW(axis, BOX(task)->home.axis + work->scl.axis)                \
    }

/* whether player 0's upgrades put SHAPE_HURT_NO_PUSH in the box's shape */
#ifdef GADGET_WOOD
#define HURT_NO_PUSH()                                                         \
    (player_records[0]->upgrades &                                             \
     (UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT) |                                  \
      UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS) | UPGRADE_BIT(UPGRADE_ROUGE_NAILS)))
#else
#define HURT_NO_PUSH()                                                         \
    ((upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES) |                       \
                  UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) ||                        \
     ((upgrades & UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT)) &&                    \
      (upgrades & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |                      \
                   UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING)))))
#endif

static void box_step(struct task *task)
{
    struct task_work *work = task->work;
#ifdef GADGET_LID
    /* @hack spelling: gravity, a pointer to world_gravity, where world_gravity
     * by name would do */
    NJS_VECTOR *gravity;
#endif
    int i;
    int j;
    int chance_bits;
#ifdef GADGET_WOOD
    Bool broken;
#endif
#ifndef GADGET_LID
    int player;
#endif
#ifdef GADGET_IRON
    /* @hack spelling: upgrades, a local for player 0's upgrades, where
     * player_records[0]->upgrades read in place would do */
    Uint32 upgrades;
#endif
    /* @hack spelling: mask, a local for ANIMAL_KIND_BITS, where the constant
     * in place would do */
    int mask;
#ifdef GADGET_IRON
    /* @hack spelling: gravity, a pointer to world_gravity that the switch's
     * test sets, where world_gravity by name would do */
    NJS_VECTOR *gravity;
#endif
#ifdef GADGET_WOOD
#ifdef NONMATCHING
#else
    Float *source;
    Float *offset;
    Float *result;
#endif
#endif

    if (placement_try_leave(task)) {
        return;
    }
#ifdef GADGET_WOOD
    /* @hack redundant: a store of 0 to timer, in a block no path reaches */
    if (0) {
        work->timer = 0;
    }
    /* @hack redundant: a store of MODE_BROKEN to submode, in a block no path
     * reaches */
    if (0) {
        work->submode = MODE_BROKEN;
    }
    mask = ANIMAL_KIND_BITS;
    /* @hack redundant: a store of gravity's y to scl.y, in a loop that never
     * runs */
    while (0) {
        work->scl.y = world_gravity.y;
    }
#endif
#ifdef GADGET_IRON
    mask = ANIMAL_KIND_BITS;
    /* @hack redundant: a store of 1 to mask, in a block no path reaches */
    if (0) {
        mask = 1;
    }
    /* @hack redundant: a loop that counts timer, which never runs */
    while (0) {
        work->timer++;
    }
#endif
#ifdef GADGET_LID
    mask = ANIMAL_KIND_BITS;
    /* @hack redundant: a loop that clears mask, which never runs */
    while (0) {
        mask = 0;
    }
    /* @hack redundant: two calls of rand, in a block no path reaches */
    if (0) {
        rand();
        rand();
    }
#endif
    switch (
#ifdef GADGET_IRON
        gravity = &world_gravity,
#endif
        work->submode) {
    case MODE_WHOLE:
#ifdef GADGET_LID
        work->timer++;
        collision_register(task);
#else
#ifdef GADGET_WOOD
        chance_bits = collision_get_hit_count(task);
        broken = FALSE;
        while (chance_bits--) {
            if (player_attacks_task(
                    task, collision_get_hit(task, chance_bits)->task)) {
                broken = TRUE;
                break;
            }
        }
#define BOX_BREAKS_ON_HIT() (broken)
#else
#define BOX_BREAKS_ON_HIT()                                                    \
    ((work->collision->flags & COLLISION_TOUCHED) &&                           \
     player_breaks_task(task, work->collision->hit->owner))
#endif
        if (BOX_BREAKS_ON_HIT()) {
            NJS_POINT3 point;
            point = work->pos;
            point.y += HALF_SIZE;
#ifdef GADGET_WOOD
            /* @hack redundant: stores of the puffs' constants to point, in a
             * block no path reaches */
            if (0) {
                point.y = PUFF_SCALE;
                point.x = PUFF_RADIUS;
                point.z = PUFF_RADIUS_RANGE * RAND_SCALE;
                point.x = PUFF_RADIUS_RANGE * RAND_SCALE;
            }
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &point,
                    (((Float)rand() * RAND_SCALE) * PUFF_RADIUS_RANGE) +
                        PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + PUFF_SPEED,
                    (((Float)rand() * RAND_SCALE) * PUFF_SCALE_RANGE) +
                        PUFF_SCALE);
            }
#else
            sound_play_at_for(BREAK_SOUND, work, SOUND_PRIORITY, FULL_VOLUME,
                              SOUND_DURATION, &work->pos);
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &point,
                    PUFF_RADIUS +
                        ((Float)rand() * (PUFF_RADIUS_RANGE * RAND_SCALE)),
                    PUFF_SPEED + ((Float)rand() * RAND_SCALE),
                    PUFF_SCALE +
                        ((Float)rand() * (PUFF_SCALE_RANGE * RAND_SCALE)));
            }
            /* @hack redundant: a store of PUFF_RADIUS_RANGE * RAND_SCALE to
             * point.x, in a block no path reaches */
            if (0) {
                point.x = PUFF_RADIUS_RANGE * RAND_SCALE;
            }
#endif
            work->timer = 0;
            work->submode = MODE_BROKEN;
#ifdef GADGET_WOOD
            sound_play_at_for(BREAK_SOUND, work, SOUND_PRIORITY, FULL_VOLUME,
                              SOUND_DURATION, &work->pos);
#endif
            if (work->collision->hit->owner->work->id == WORK_ID_PLAYER) {
                player = player_slot_of(work->collision->hit->owner);
                if (player != -1 &&
                    (player_records[player]->character != PLAYER_SONIC ||
                     player_work[player]->mode != PLAYER_MODE_SONIC_BOUNCE)) {
                    player_set_speed(player, 0, 0, 0);
                }
            }
        } else {
            work->timer++;
#ifdef GADGET_IRON
            upgrades = player_records[0]->upgrades;
#endif
            if (HURT_NO_PUSH()) {
                work->collision->shapes->flags |= SHAPE_HURT_NO_PUSH;
            } else {
                work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
            }
            collision_register(task);
        }
#endif
#undef BOX_BREAKS_ON_HIT
        break;
    case MODE_BROKEN:
        if (work->timer++ > PIECES_STEP) {
#ifndef GADGET_LID
            NJS_POINT3 floor_point;
#endif
            NJS_POINT3 center;
            NJS_VECTOR scale;
#ifndef GADGET_LID
            Float floor;
#endif
            center = work->pos;
            center.y += HALF_SIZE;
#ifndef GADGET_LID
#ifdef GADGET_WOOD
#define GRAVITY world_gravity
#else
#define GRAVITY (*gravity)
#endif
            scale = GRAVITY;
            /* @hack spelling: the scaling and the sum walked through pointers
             * to the vectors' fields, where the fields by name would do */
#ifdef NONMATCHING
            /* the plain forms the hack tags above and below name */
            scale.x *= HALF_SIZE;
            scale.y *= HALF_SIZE;
            scale.z *= HALF_SIZE;
            floor_point.x = center.x + scale.x;
            floor_point.y = center.y + scale.y;
            floor_point.z = center.z + scale.z;
#else
#ifdef GADGET_WOOD
            source = (Float *)&scale;
            *source++ *= HALF_SIZE;
            *source++ *= HALF_SIZE;
            *source *= HALF_SIZE;
            result = (Float *)&floor_point;
            source = (Float *)&center;
            offset = (Float *)&scale;
            *result++ = *source++ + *offset++;
            *result++ = *source++ + *offset++;
            *result = *source + *offset;
            /* @hack redundant: tests of offset, in a block no path reaches */
            if (0) {
                if (offset) {
                }
                if (offset) {
                }
                if (offset) {
                }
                if (offset) {
                }
            }
#else
            {
                Float *axis = &(&scale)->x;
                *axis++ *= HALF_SIZE;
                *axis++ *= HALF_SIZE;
                *axis *= HALF_SIZE;
            }
            {
                Float *source;
                Float *offset = &(&scale)->x;
                Float *result;
                *(result = &(&floor_point)->x) =
                    *(source = &(&center)->x) + *offset++;
                source++;
                result++;
                *result++ = *source++ + *offset++;
                *result = *source + *offset;
            }
#endif
#endif
            floor = NO_FLOOR;
            if (njAbs(GRAVITY.x) > GRAVITY_ON_AXIS) {
                floor = floor_point.x;
            }
            if (njAbs(GRAVITY.y) > GRAVITY_ON_AXIS) {
                floor = floor_point.y;
            }
            if (njAbs(GRAVITY.z) > GRAVITY_ON_AXIS) {
                floor = floor_point.z;
            }
#endif
            scale.x = HALF_SIZE;
            scale.y = HALF_SIZE;
            scale.z = HALF_SIZE;
            for (i = 0; i < BOX_PIECES; i++) {
                for (j = 0; j < pieces[i].count; j++) {
                    debris_throw_flags(
                        &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
#ifdef GADGET_WOOD
                        ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)) +
                            PIECE_SPEED,
#else
                        PIECE_SPEED +
                            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)),
#endif
#ifdef GADGET_LID
                        NO_FLOOR,
#else
                        floor,
#endif
                        pieces[i].model, pieces[i].textures, DEBRIS_GRAVITY);
                }
            }
            /* @hack redundant: a store of PIECE_SPEED to scale.x, and for the
             * lid of NO_FLOOR to scale.y, in a block no path reaches */
            if (0) {
                scale.x = PIECE_SPEED;
#ifdef GADGET_LID
                scale.y = NO_FLOOR;
#endif
            }
            work->submode = MODE_GONE;
        }
#ifdef GADGET_LID
        collision_register(task);
#else
#ifdef GADGET_IRON
        upgrades = player_records[0]->upgrades;
#endif
        if (HURT_NO_PUSH()) {
            work->collision->shapes->flags |= SHAPE_HURT_NO_PUSH;
        } else {
            work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
            collision_register(task);
        }
#endif
        break;
    case MODE_GONE:
#ifdef GADGET_WOOD
        /* @hack redundant: a store of RAND_SCALE to scl.x, in a block no path
         * reaches */
        if (0) {
            work->scl.x = RAND_SCALE;
        }
#endif
        chance_bits =
            ((ANIMAL(work) & ANIMAL_CHANCE_BITS) >> ANIMAL_CHANCE_SHIFT) & mask;
#ifdef GADGET_LID
        /* @hack redundant: a loop that doubles chance_bits three times, which
         * never runs */
        while (0) {
            chance_bits += chance_bits;
            chance_bits += chance_bits;
            chance_bits += chance_bits;
        }
#endif
        if (chance_bits) {
            Float chance = (Float)chance_bits * TENTH;
            chance *= TENTH;
            if (chance > (Float)rand() * RAND_SCALE ||
                chance_bits >= ANIMAL_CERTAIN) {
                animal_release(work->pos.x, work->pos.y + HALF_SIZE,
                               work->pos.z,
                               (ANIMAL(work) & mask) % ANIMAL_RELEASE_KINDS);
            }
        }
        if (task->placement) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    /* @hack spelling: the sum walked through pointers to the vectors' fields,
     * where the fields by name would do */
#ifdef NONMATCHING
    /* the plain forms the hack tags above and below name */
    work->pos.x += BOX(task)->speed.x;
    work->pos.y += BOX(task)->speed.y;
    work->pos.z += BOX(task)->speed.z;
#else
#ifdef GADGET_WOOD
    source = &work->pos.x;
    offset = &BOX(task)->speed.x;
    *source++ += *offset++;
    *source++ += *offset++;
    *source += *offset;
    /* @hack redundant: a test of offset, in a block no path reaches */
    if (0) {
        if (offset) {
        }
    }
#else
    {
        Float *to = &work->pos.x;
        Float *from = &BOX(task)->speed.x;
        *to++ += *from;
        from++;
        *to++ += *from;
        from++;
        *to += *from;
    }
#endif
#endif
    BOUNCE_AXIS(y)
    BOUNCE_AXIS(x)
    BOUNCE_AXIS(z)
#ifdef GADGET_LID
    gravity = &world_gravity;
#define GRAVITY (*gravity)
#endif
    BOX(task)->speed.x =
        (GRAVITY.x * FALL_ACCELERATION) + (BOX(task)->speed.x * SPEED_DRAG);
    BOX(task)->speed.y =
        (GRAVITY.y * FALL_ACCELERATION) + (BOX(task)->speed.y * SPEED_DRAG);
    BOX(task)->speed.z =
        (GRAVITY.z * FALL_ACCELERATION) + (BOX(task)->speed.z * SPEED_DRAG);
}

static void box_draw(struct task *task)
{
    struct task_work *work = task->work;
#ifdef GADGET_LID
    NJS_CNK_MODEL *model;
#else
    NJS_POINT3 position;
    Float scale;
    Angle yaw;
#endif

    if (gadget_visible_from_camera(&work->pos)) {
        njSetTexture(&floating_box_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        draw_control_set(DRAW_CONTROL_SHADOW);
#ifdef GADGET_LID
        /* @hack spelling: the assignment to model inside the call, where a
         * statement of its own would do */
        njCnkEasyDrawModel(model = floating_box_model);
#else
        njCnkEasyDrawModel(floating_box_model);
#endif
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
#ifdef NONMATCHING
#else
#ifdef GADGET_LID
        /* @hack redundant: a test of task, in a block no path reaches */
        if (0) {
            if (task) {
            }
        }
        /* @hack redundant: a test of model, in a block no path reaches */
        if (0) {
            if (model) {
            }
        }
        /* @hack redundant: transforms and draws, in a loop that never runs */
        while (0) {
            NJS_POINT3 position;
            Float offset;
            Angle yaw;
            position = work->pos;
            offset = work->scl.y;
            yaw = work->ang.y;
            njTranslateEx(&position);
            njRotateY(NULL, yaw);
            njCnkEasyDrawModel(model);
            if (offset > 0.0F) {
                position.x += offset;
            }
            position.y += offset;
            njTranslateEx(&position);
            njRotateY(NULL, yaw);
            if (model) {
                njCnkEasyDrawModel(model);
            }
        }
#else
        /* @hack redundant: transforms and draws, in a block no path
         * reaches */
        if (0) {
            scale = work->scl.y;
            yaw = work->ang.x;
            position.x = work->pos.x;
            position.y = work->pos.y + scale;
            position.z = work->pos.z;
            njTranslateEx(&position);
            njRotateY(NULL, yaw);
            njScale(NULL, scale, scale, scale);
            njCnkEasyDrawModel(floating_box_model);
            njTranslateEx(&position);
            njRotateY(NULL, yaw);
            njScale(NULL, scale, scale, scale);
            njCnkEasyDrawModel(floating_box_model);
            njRotateY(NULL, yaw);
            njScale(NULL, scale, scale, scale);
            njCnkModDrawModel(floating_box_model);
            njCnkModDrawModel(floating_box_model);
            collision_register(task);
        }
#endif
#endif
    }
}

#endif
