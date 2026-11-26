/* ORI in Crazy Gadget's object list: a cage that a missile of its id breaks,
 * as shared/cage.h's does. its pieces fall along world_gravity */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/cage.h"
#include "shared/cage_models.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/missile.h"
#include "stg22/breakables.h"
#include "stg22/crazy_gadget.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

NJS_TEXLIST *gadget_cage_texlists[5] = {
#include "stg22/cage/gadget_cage_texlists.inc"
};
KEPT static struct cage_piece cage_pieces[CAGE_PIECES] = {
#include "stg22/cage/cage_pieces.inc"
};
static Bool cage_built = {
#include "stg22/cage/cage_built.inc"
};
/* the cage's model, then its pieces' in cage_pieces' order */
static struct model_build cage_builds[CAGE_BUILDS] = {
#include "stg22/cage/cage_builds.inc"
};

/* the set editor's labels */
struct cage_editor_labels {
    char crash_through[24];
    char crash_reflect[24];
    char angle_y[11];
};
static const struct cage_editor_labels cage_editor = {
#include "stg22/cage/cage_editor.inc"
};

static struct collision_shape cage_shape[1] = {
#include "stg22/cage/cage_shape.inc"
};
/* the cage is 2 * HALF_SIZE on a side, and stands on its pos */
#define HALF_SIZE 10.0F

/* a cage's modes, in its work's submode. a MODE_BROKEN cage's pieces have
 * yet to fly */
enum cage_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };

/* the cage's record, at task->motion. hit is where a missile of the cage's id
 * landed, or null. missile_target_add takes its address */
struct cage {
    NJS_POINT3 *hit;
#ifdef NONMATCHING
    /* this side stores the floor's height in the record, the plain form of the
     * hack at FLOOR */
    Float floor;
#endif
};

#define CAGE(task) ((struct cage *)(task)->motion.p)

/* the height of the cage's floor, or NO_FLOOR until it finds one. a whole
 * cage looks for one every FLOOR_STEPS steps, a power of 2. it looks down from
 * FLOOR_LOOK_HEIGHT above its pos */
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define FLOOR(task) (CAGE(task)->floor)
#else
/* @hack type: the floor's height kept in the task's extra word, where a field
 * of struct cage would do */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)
#endif
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* the steps a broken cage waits, in task->unk_3c, before its work's timer
 * counts. a missile sets the wait to the cage's distance from its hit times
 * DELAY_PER_DISTANCE, in whole steps */
#define BREAK_DELAY(task) ((task)->unk_3c.l)
#define DELAY_PER_DISTANCE 0.6F
/* once the wait is over, the boxes on the cage fall on the step its timer is
 * DROP_STEP. its pieces fly once the timer passes PIECES_STEP */
#define DROP_STEP 1
#define PIECES_STEP 2

/* a falling cage keeps the height it has left to fall in scl.y, which
 * stack_drop raises. it keeps its speed in y in scl.z. each step scales the
 * speed by FALL_DRAG and takes FALL_GRAVITY off it. a cage that lands within
 * DUST_HEIGHT of its floor raises dust */
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F
#define DUST_HEIGHT 5.0F
#define LANDING_DUST_SCALE 8.0F
#define LANDING_DUST_DENSITY 1.2F
#define LANDING_DUST_SPEED 0.5F

/* a missile's hit scores BREAK_POINTS. BREAK_PUFFS puffs of dust burst from
 * SMOKE_HEIGHT above the cage's pos. a puff's radius runs from PUFF_RADIUS up
 * to PUFF_RADIUS + PUFF_RADIUS_RANGE. its speed runs from PUFF_SPEED up to
 * PUFF_SPEED + 1. its size runs from PUFF_SCALE up to PUFF_SCALE +
 * PUFF_SCALE_RANGE */
#define BREAK_POINTS 20
#define BREAK_PUFFS 14
#define SMOKE_HEIGHT 8.0F
#define PUFF_RADIUS 7.0F
#define PUFF_RADIUS_RANGE 3.0F
#define PUFF_SPEED 0.5F
#define PUFF_SCALE 2.5F
#define PUFF_SCALE_RANGE 4.0F

/* a bit of ang.x: the cage's pieces fall through its floor */
#define PIECES_FALL_THROUGH 0x1
/* the bits of ang.z that hold the cage's missile target id */
#define TARGET_ID_BITS 0x1f
/* a cage's pieces fly from its center at speeds from PIECE_SPEED up to
 * PIECE_SPEED + PIECE_SPEED_RANGE. PIECES_FLOOR is the floor they bounce on,
 * NO_FLOOR under PIECES_FALL_THROUGH. otherwise it is the cage's floor, or its
 * pos.y where it has found none. it is a height in y */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F
#define RANDOM_PIECE_SPEED()                                                   \
    (PIECE_SPEED + ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)))
#define PIECES_FLOOR(task, work)                                               \
    (((work)->ang.x & PIECES_FALL_THROUGH)                                     \
         ? NO_FLOOR                                                            \
         : (FLOOR(task) != NO_FLOOR ? FLOOR(task) : (work)->pos.y))

static void cage_destroy(struct task *task);
static void cage_step(struct task *task);
static void cage_draw(struct task *task);
static void cage_draw_built(struct task *task);

Uint8 *gadget_cage_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, cage_builds, CAGE_BUILDS) ==
        CAGE_BUILDS) {
        cage_built = TRUE;
    }
    return buffer;
}

void gadget_cage_init(struct task *task)
{
    struct task_work *work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct cage));
    if (!task->motion.p) {
        return;
    }
    if (cage_built) {
        task->draw = cage_draw_built;
    } else {
        task->draw = cage_draw;
    }
    task->step = cage_step;
    task->destructor = cage_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    /* @hack redundant: a store of null to hit, which syCalloc has zeroed and
     * missile_target_add nulls */
    CAGE(task)->hit = NULL;
    work->ang.x &= PIECES_FALL_THROUGH;
    work->ang.z &= TARGET_ID_BITS;
    collision_init(task, cage_shape, 1, COLLISION_OBJECT);
    /* @hack redundant: a second mask of ang.z with TARGET_ID_BITS, after the
     * statement that masked it */
    missile_target_add(work->ang.z & TARGET_ID_BITS, &CAGE(task)->hit);
    stack_add(task);
    work->scl.y = 0.0F;
    work->scl.z = 0.0F;
}
static void cage_destroy(struct task *task)
{
    missile_target_remove(&CAGE(task)->hit);
    stack_remove(task);
    /* extra holds the floor's height in place of a pointer, and task_destroy
     * frees a non-null extra */
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void cage_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int j;
    /* @hack redundant: the test of the work's mode, which the cage never sets:
     * it keeps its modes in submode */
    if (work->mode == 0 && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: stores of MODE_BROKEN and MODE_WHOLE to submode, in a
     * block no path reaches */
    if (0) {
        work->submode = MODE_BROKEN;
        work->submode = MODE_WHOLE;
    }
    if (work->scl.y > 0.0F) {
        /* @hack spelling: height, a local for scl.y, where scl.y read in place
         * would do */
        Float height = work->scl.y;
        Float step = work->scl.z;
        if (-step > height) {
            step = -work->scl.y;
        }
        work->pos.y += step;
        work->scl.y += step;
        work->scl.z = (work->scl.z * FALL_DRAG) - FALL_GRAVITY;
        if (!task->child) {
            model_object_create(task, NULL, NULL, cage_shape, 1, 0);
        }
        if (task->child) {
            struct task_work *child = task->child->work;
            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - work->scl.y;
        }
    } else if (task->child) {
        task_destroy_children(task);
    }
    if (work->scl.y <= 0.0F && work->scl.z != 0.0F) {
        work->scl.y = 0.0F;
        work->scl.z = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            LANDING_DUST_SCALE, LANDING_DUST_DENSITY,
                            LANDING_DUST_SPEED);
        }
    }
    switch (work->submode) {
    case MODE_WHOLE:
        if (CAGE(task)->hit) {
            NJS_POINT3 smoke;
            Float delay;
            delay =
                njDistanceP2P(&work->pos, CAGE(task)->hit) * DELAY_PER_DISTANCE;
            BREAK_DELAY(task) = (int)delay;
            smoke = work->pos;
            smoke.y += SMOKE_HEIGHT;
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &smoke,
                    (((Float)rand() * RAND_SCALE) * PUFF_RADIUS_RANGE) +
                        PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + PUFF_SPEED,
                    (((Float)rand() * RAND_SCALE) * PUFF_SCALE_RANGE) +
                        PUFF_SCALE);
            }
            /* @hack redundant: stores of RAND_SCALE and its products with the
             * puffs' ranges to smoke, in a block no path reaches */
            if (0) {
                smoke.x = RAND_SCALE;
                smoke.y = RAND_SCALE;
                smoke.z = PUFF_RADIUS_RANGE * RAND_SCALE;
                smoke.x = PUFF_SCALE_RANGE * RAND_SCALE;
            }
            work->timer = 0;
            work->submode = MODE_BROKEN;
            score_add(BREAK_POINTS);
            collision_register(task);
        } else {
            work->timer++;
            if (FLOOR(task) == NO_FLOOR && !(work->timer & (FLOOR_STEPS - 1))) {
                struct angles ang;
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                FLOOR(task) = surface_probe_floor(
                    work->pos.x, work->pos.y + FLOOR_LOOK_HEIGHT, work->pos.z,
                    &ang);
            }
            collision_register(task);
        }
        break;
    case MODE_BROKEN:
        if (work->timer == 0 && BREAK_DELAY(task) != 0) {
            BREAK_DELAY(task)--;
            collision_register(task);
            break;
        }
        if (work->timer == DROP_STEP) {
            /* @hack spelling: position, a copy of the work's pos, where
             * &work->pos would do: stack_drop doesn't write through pos */
            NJS_POINT3 position;
            position = work->pos;
            stack_drop(&position);
        }
        if (work->timer++ > PIECES_STEP) {
            NJS_POINT3 below;
            NJS_POINT3 center;
            NJS_VECTOR scale;
            Float floor;
            center = work->pos;
            center.y += HALF_SIZE;
            /* @hack redundant: below, the point HALF_SIZE along world_gravity
             * from center, and floor, its coordinate on gravity's axis, which
             * no code reads */
            scale = world_gravity;
#ifdef NONMATCHING
            /* the plain forms the hack tags below name */
            scale.x *= HALF_SIZE;
            scale.y *= HALF_SIZE;
            scale.z *= HALF_SIZE;
            below.x = center.x + scale.x;
            below.y = center.y + scale.y;
            below.z = center.z + scale.z;
#else
            /* @hack spelling: the scaling and the sum walked through pointers
             * to the vectors' fields, where the fields by name would do */
            {
                Float *axis = &(&scale)->x;
                *axis++ *= HALF_SIZE;
                *axis++ *= HALF_SIZE;
                *axis *= HALF_SIZE;
            }
            {
                Float *from = &(&center)->x;
                Float *offset = &(&scale)->x;
                Float *to = &(&below)->x;
                *to++ = *from++ + *offset++;
                *to++ = *from++ + *offset++;
                *to = *from + *offset;
                /* @hack redundant: tests of from and to, in a block no path
                 * reaches */
                if (0) {
                    if (from) {
                    }
                    if (from) {
                    }
                    if (to) {
                    }
                }
            }
#endif
            floor = NO_FLOOR;
            if (njAbs(world_gravity.x) > GRAVITY_ON_AXIS) {
                floor = below.x;
            }
            if (njAbs(world_gravity.y) > GRAVITY_ON_AXIS) {
                floor = below.y;
            }
            if (njAbs(world_gravity.z) > GRAVITY_ON_AXIS) {
                floor = below.z;
            }
            (void)floor;
            scale.x = HALF_SIZE;
            scale.y = HALF_SIZE;
            scale.z = HALF_SIZE;
            /* @bug logic: under gravity along x, the pieces bounce on the
             * wrong floor. PIECES_FLOOR is a height in y, and debris_step
             * meets it on x. the throws at 0x8c5059f4 and 0x8c505ad6 pass
             * it */
            if (cage_built) {
                for (i = 0; i < CAGE_PIECES; i++) {
                    for (j = 0; j < cage_pieces[i].count; j++) {
                        debris_throw_built_flags(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            RANDOM_PIECE_SPEED(), PIECES_FLOOR(task, work),
                            cage_pieces[i].model, cage_builds[i + 1].built,
                            cage_pieces[i].texlist, DEBRIS_GRAVITY);
                    }
                }
            } else {
                for (i = 0; i < CAGE_PIECES; i++) {
                    for (j = 0; j < cage_pieces[i].count; j++) {
                        debris_throw_flags(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            RANDOM_PIECE_SPEED(), PIECES_FLOOR(task, work),
                            cage_pieces[i].model, cage_pieces[i].texlist,
                            DEBRIS_GRAVITY);
                    }
                }
            }
            /* @hack redundant: a store of PIECE_SPEED to scale.x, in a block no
             * path reaches */
            if (0) {
                scale.x = PIECE_SPEED;
            }
            work->submode = MODE_GONE;
        }
        collision_register(task);
        break;
    case MODE_GONE:
        if (task->placement) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

static void cage_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&cage_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&cage_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}

static void cage_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    if (gadget_visible_from_camera(&work->pos)) {
        njSetTexture(&cage_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njDirectDrawObject(cage_builds[0].built);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrix(1);
    }
}
