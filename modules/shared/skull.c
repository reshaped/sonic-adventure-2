/* the skull: SKULL, PSKULL and KSKULL in a stage's object list. a hunter's
 * dig throws one up too. a player can pick up and carry a lying skull */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/skull.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/skull/text.inc"

/* a skull's modes, in its work's submode */
enum skull_mode {
    MODE_LYING, /* its body moves it, and a player can hold it */
    MODE_THROWN, /* in flight, passing through the stage */
    MODE_WAITING /* still, until a touch throws it up */
};

/* a skull keeps its alpha, a Float, in its task's extra. an alpha of 1 or
 * more draws the skull opaque. task_destroy frees a non-null extra, so the
 * destructor nulls it first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define ALPHA(task) (*(Float *)&(task)->extra)
/* ANIMAL(work) keeps the low byte of ang.z, the skull's animal byte of
 * object/animal.h. the chance that the skull's first hold releases the animal
 * is ANIMAL_CHANCE_BASE plus ANIMAL_CHANCE_STEP times the ANIMAL_CHANCE_BITS
 * field. the animal starts ANIMAL_RISE above the skull */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F
/* the placement's state once a player has held the skull. a hold releases
 * an animal only while the state is 0 */
#define STATE_HELD 1

#define SKULL_LEVEL 2 /* the task level of a thrown skull */
/* a thrown skull's spd keeps DRAG of itself each step, and spd.y then loses
 * GRAVITY */
#define DRAG 0.99F
#define GRAVITY 0.1F
#define THROW_STEPS 20
#define POP_SPEED 2.0F
#define FLOOR_BOUNCE 0.1F
/* a lying skull fades once FADE_DELAY steps pass without a hold. it loses
 * FADE_STEP of alpha a step, and ends at FADE_END. a player within NEAR_RANGE
 * keeps it opaque */
#define FADE_DELAY 2880
#define NEAR_RANGE 50.0F
#define FADE_STEP (1.0F / 60.0F)
#define FADE_END 0.01F

static NJS_TEXNAME skull_texname[1] = {
#include "shared/skull/skull_texname.inc"
};
NJS_TEXLIST skull_texlist = {
#include "shared/skull/skull_texlist.inc"
};

#define SKULL_POINTS 51
static Sint16 skull_polygons[539] = {
#include "shared/skull/skull_polygons.inc"
};
static Sint32 skull_vertices[309] = {
#include "shared/skull/skull_vertices.inc"
};
NJS_CNK_MODEL skull_model = {
#include "shared/skull/skull_model.inc"
};
static NJS_CNK_OBJECT skull_object = {
#include "shared/skull/skull_object.inc"
};

NJS_TEXLIST *skull_texlists[2] = {
#include "shared/skull/skull_texlists.inc"
};

#define SKULL_SHAPES 3
/* a lying skull's two cylinders, then a waiting or a thrown skull's sphere.
 * each mode turns the other's shapes off */
static struct collision_shape skull_shapes[SKULL_SHAPES] = {
#include "shared/skull/skull_shapes.inc"
};
static struct collision_shape skull_still_shape = {
#include "shared/skull/skull_still_shape.inc"
};

static void skull_draw(struct task *task);
static void skull_destroy(struct task *task);
static void skull_step(struct task *task);

static void skull_setup(struct task *task)
{
    /* @hack spelling: bit, one local for both SHAPE_OFF and BODY_RESTS,
     * which share the value 0x10, where each constant in place would do */
    Uint32 bit;
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    task->draw = skull_draw;
    task->step = skull_step;
    task->destructor = skull_destroy;
    /* @hack spelling: bit set after the callbacks, where an initializer at
     * its declaration would do */
    bit = 0x10;
    /* @hack redundant: the submode set to MODE_LYING, which the end of the
     * setup overwrites */
    work->submode = MODE_LYING;
    ANIMAL(work) = work->ang.z;
    collision_init(task, skull_shapes, SKULL_SHAPES, COLLISION_OBJECT);
    /* @bug memory: reads the collision record's shapes through null where
     * collision_init found no room, though skull_lying_init tests the record
     * for null: 0x36 and 0xd2 of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: a skull without a collision record
     * kills itself before it sets a shape, so it ends at its next step */
    if (work->collision == NULL) {
        task_kill(task);
        return;
    }
#endif
    work->collision->shapes[0].flags |= bit;
    work->collision->shapes[1].flags |= bit;
    work->collision->shapes[2].flags &= ~SHAPE_OFF;
    body_init(task);
    if (task->force.p != NULL) {
        BODY(task)->floor_bounce = FLOOR_BOUNCE;
        BODY(task)->flags |= bit;
    }
#ifdef NONMATCHING
    /* this side repairs the tag in skull_step: a skull without a body kills
     * itself, so it ends at its next step, before body_step */
    if (task->force.p == NULL) {
        task_kill(task);
    }
#endif
    work->submode = MODE_WAITING;
    ALPHA(task) = 1.0F;
    motion->hold_turn = 0x1300;
}

void skull_waiting_init(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (!placement_try_leave(self)) {
        skull_setup(self);
    }
}

void skull_lying_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    skull_setup(task);
    work->submode = MODE_LYING;
    if (work->collision != NULL) {
        work->collision->shapes[0].flags &= ~SHAPE_OFF;
        work->collision->shapes[1].flags &= ~SHAPE_OFF;
        work->collision->shapes[2].flags |= SHAPE_OFF;
    }
}

struct task *skull_throw_up(NJS_POINT3 *pos, Float speed)
{
    struct task *task;
    struct task_work *work;
    struct task_motion *motion;

    task = task_create(TASK_MOTION | TASK_WORK, SKULL_LEVEL, skull_waiting_init,
                       "ObjectSkullObj");
    if (task != NULL) {
        work = task->work;
        motion = task->motion.p;
        skull_setup(task);
        work->pos = *pos;
        work->ang.y = RANDOM_TURN();
        /* @hack spelling: spd set in the order y, x, z, where x, y, z would
         * do */
        motion->spd.y = speed;
        motion->spd.x = 0.0F;
        motion->spd.z = 0.0F;
        work->submode = MODE_THROWN;
    }
    return task;
}

/* make a skull as skull_throw_up does, but flying at spd. it lies after
 * THROW_STEPS steps, whatever its spd.y. nothing calls skull_throw, and no
 * header declares it. it is external only because SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct task *skull_throw(NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct task_work *work;
    struct task_motion *motion;

    task = task_create(TASK_MOTION | TASK_WORK, SKULL_LEVEL, skull_waiting_init,
                       "ObjectSkullObj");
    if (task != NULL) {
        work = task->work;
        motion = task->motion.p;
        skull_setup(task);
        work->submode = MODE_THROWN;
        work->pos = *pos;
        work->ang.y = RANDOM_TURN();
        motion->spd = *spd;
        work->timer = THROW_STEPS;
    }
    return task;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void skull_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    body_destroy(self);
    self->extra.p = NULL;
}

/* a static copy of player_holding, which common_object.h declares */
static int skull_holder(struct task *task)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (player_records[i] != NULL && player_records[i]->held == task) {
            return i;
        }
    }
    return -1;
}

static void skull_step(struct task *task)
{
    struct task_motion *motion = task->motion.p;
    struct task_work *work = task->work;
    Float chance;
    int player;
    /* @hack redundant: unused and unused_at, an array and a pointer to it
     * that nothing reads */
    int unused[1];
    int *unused_at = unused;
    /* @hack spelling: mask, a local set to ~SHAPE_OFF in the switch's
     * expression, where the constant in place would do */
    Uint32 mask;

    if (placement_try_leave(task)) {
        return;
    }
    switch (mask = ~SHAPE_OFF, work->submode) {
    case MODE_THROWN:
        if ((!work->timer && motion->spd.y < 0.0F) ||
            /* @hack spelling: the count compared as <= 0, where == 0 would
             * do */
            (work->timer && --work->timer <= 0)) {
            work->timer = 0;
            work->submode = MODE_LYING;
            work->collision->shapes[0].flags &= mask;
            work->collision->shapes[1].flags &= mask;
            work->collision->shapes[2].flags |= SHAPE_OFF;
        } else {
            /* @hack spelling: spd scaled in the order y, z, x, where x, y, z
             * would do */
            motion->spd.y *= DRAG;
            motion->spd.z *= DRAG;
            motion->spd.x *= DRAG;
            motion->spd.y -= GRAVITY;
            /* @hack spelling: pos moved in the order y, x, z, where x, y, z
             * would do */
            work->pos.y += motion->spd.y;
            work->pos.x += motion->spd.x;
            work->pos.z += motion->spd.z;
            collision_register(task);
        }
        break;
    case MODE_WAITING:
        if (work->collision->flags & COLLISION_TOUCHED) {
            motion->spd.y = POP_SPEED;
            work->submode = MODE_THROWN;
        }
        work->collision->shapes[2].flags &= mask;
        collision_register(task);
        break;
    default:
        if (work->flags & WORK_HELD_BY_PLAYER) {
            ALPHA(task) = 1.0F;
            player = skull_holder(task);
            if (player >= 0) {
                work->ang.y = QUARTER_TURN - player_work[player]->ang.y;
                if (task->placement != NULL && placement_get_state(task) == 0) {
                    placement_set_state(task, STATE_HELD);
                    if (ANIMAL(work) & ANIMAL_CHANCE_BITS) {
                        /* @hack type: the masked byte cast to Uint8 before
                         * the shift, where ANIMAL(work) >> ANIMAL_CHANCE_SHIFT
                         * would do */
                        chance = (Float)((Uint8)(ANIMAL(work) &
                                                 ANIMAL_CHANCE_BITS) >>
                                         ANIMAL_CHANCE_SHIFT);
                        /* @hack spelling: the product in a statement of its
                         * own, where one expression would do */
                        chance *= ANIMAL_CHANCE_STEP;
                        if (rand() * RAND_SCALE < chance + ANIMAL_CHANCE_BASE) {
                            animal_release(work->pos.x,
                                           work->pos.y + ANIMAL_RISE,
                                           work->pos.z,
                                           (ANIMAL(work) & ANIMAL_KIND_BITS) %
                                               ANIMAL_RELEASE_KINDS);
                        }
                    }
                }
            }
            work->timer = 0;
        } else if (work->timer < FADE_DELAY) {
            work->timer++;
            ALPHA(task) = 1.0F;
        } else if (!player_near(&work->pos, NEAR_RANGE)) {
            if (ALPHA(task) <= FADE_END) {
                if (task->placement != NULL) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
                /* @hack redundant: the else after the arm that breaks */
                break;
            } else {
                ALPHA(task) -= FADE_STEP;
                if (ALPHA(task) < 0.0F) {
                    ALPHA(task) = 0.0F;
                }
            }
        } else {
            ALPHA(task) = 1.0F;
        }
        /* @bug memory: body_step reads and writes the body through null
         * where body_init found no room, though skull_setup tests for a null
         * body: 0x60 and 0x526 of the object's code */
        body_step(task);
        break;
    }
}

static void skull_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&skull_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (ALPHA(task) >= 1.0F) {
        njCnkEasyDrawModel(&skull_model);
    } else {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        constant_material_set(ALPHA(task), 1.0F, 1.0F, 1.0F);
        njCnkEasyDrawModel(&skull_model);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
}

static void skull_still_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: an empty arm for the leave, where a return would do */
    if (placement_try_leave(task)) {
    } else if (work->scl.x < 1.0F) {
        collision_register(task);
    }
}

void skull_still_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: three strings no code reads, which the object keeps
     * among its strings */
    (void)"<- ANGLE Y";
    (void)"<- MINIMAL AP(  ) LEV( )";
    (void)"<- COL(0:ON/1:OFF)";
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = skull_draw;
    task->step = skull_still_step;
    task->destructor = skull_destroy;
    ALPHA(task) = 1.0F;
    if (work->scl.x < 1.0F) {
        collision_init(task, &skull_still_shape, 1, COLLISION_OBJECT);
    }
}
