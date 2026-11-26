/* the lion's head, LIONHEAD in Death Chamber's object list: a head a player
 * can pick up and carry. the first time a player holds it, it may release a
 * small animal */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg25/lion_head.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "stg25/lion_head/text.inc"

#define LION_HEAD_SHAPES 3
#define LION_HEAD_BUILDS 1

/* ANIMAL(work) is the work's tick, a byte, where the init keeps ang.z's low
 * byte: the head's animal byte of object/animal.h. the chance that the head's
 * first hold releases the animal is ANIMAL_CHANCE_BASE plus
 * ANIMAL_CHANCE_STEP times the ANIMAL_CHANCE_BITS field. the animal starts
 * ANIMAL_RISE above the head */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F
/* the placement's state once a player has held the head. a hold releases an
 * animal only while the state is 0 */
#define STATE_HELD 1

#define FLOOR_BOUNCE 0.39F

static NJS_TEXNAME chamber_lion_head_texture_names[1] = {
#include "stg25/lion_head/chamber_lion_head_texture_names.inc"
};

static NJS_TEXLIST chamber_lion_head_texlist = {
#include "stg25/lion_head/chamber_lion_head_texlist.inc"
};

static Sint16 chamber_lion_head_polygons[253] = {
#include "stg25/lion_head/chamber_lion_head_polygons.inc"
};

static Sint32 chamber_lion_head_vertices[123] = {
#include "stg25/lion_head/chamber_lion_head_vertices.inc"
};

static NJS_CNK_MODEL chamber_lion_head_model = {
#include "stg25/lion_head/chamber_lion_head_model.inc"
};

static NJS_CNK_OBJECT chamber_lion_head_object = {
#include "stg25/lion_head/chamber_lion_head_object.inc"
};

static void chamber_lion_head_destroy(struct task *task);
static void chamber_lion_head_step(struct task *task);
static void chamber_lion_head_draw(struct task *task);

NJS_TEXLIST *chamber_lion_head_texlists[2] = {
#include "stg25/lion_head/chamber_lion_head_texlists.inc"
};
/* a cylinder an attack can hurt, which pushes no group's shapes. then a thin
 * cylinder and a sphere, which push */
/* clang-format off */
static struct collision_shape chamber_lion_head_shapes[LION_HEAD_SHAPES] = {
#include "stg25/lion_head/chamber_lion_head_shapes.inc"
};
/* clang-format on */
static Sint32 chamber_lion_head_hold_turn = 3072;
/* TRUE once chamber_lion_head_build has compiled the head's model */
static Bool chamber_lion_head_built = FALSE;
static struct model_build chamber_lion_head_builds[LION_HEAD_BUILDS] = {
#include "stg25/lion_head/chamber_lion_head_builds.inc"
};

Uint8 *chamber_lion_head_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_lion_head_builds,
                             LION_HEAD_BUILDS) == LION_HEAD_BUILDS) {
        chamber_lion_head_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void chamber_lion_head_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- MINIMAL AP(  ) LEV( )";
    }
    if (!placement_try_leave(task)) {
        task->draw = chamber_lion_head_draw;
        task->step = chamber_lion_head_step;
        task->destructor = chamber_lion_head_destroy;
        ANIMAL(work) = work->ang.z;
        collision_init(task, chamber_lion_head_shapes, LION_HEAD_SHAPES,
                       COLLISION_OBJECT);
        body_init(task);
        if (task->force.p != NULL) {
            BODY(task)->floor_bounce = FLOOR_BOUNCE;
            BODY(task)->flags |= BODY_RESTS;
        }
#ifdef NONMATCHING
        /* this side repairs the tag in chamber_lion_head_step: a head without
         * a body kills itself, so it ends at its next step, before
         * body_step */
        if (task->force.p == NULL) {
            task_kill(task);
        }
#endif
        work->submode = 0;
        motion->hold_turn = chamber_lion_head_hold_turn;
    }
}

static void chamber_lion_head_destroy(struct task *task)
{
    body_destroy(task);
}

static void chamber_lion_head_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    Float chance;
    /* @hack redundant: frame and unused, locals that only size the frame */
    Uint32 frame[1];
    Uint32 *unused = frame;

    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        task_destroy(NULL);
        task_destroy(NULL);
        task_destroy(NULL);
    }
    if (placement_try_leave(task)) {
        return;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        player = player_holding(task);
        if (player >= 0) {
            work->ang.y = -player_work[player]->ang.y + QUARTER_TURN;
            if (task->placement != NULL && !placement_get_state(task)) {
                placement_set_state(task, STATE_HELD);
                if (ANIMAL(work) & ANIMAL_CHANCE_BITS) {
                    /* @hack type: the masked byte cast to a Uint8 before
                     * the shift, where the masked value alone would do */
                    chance = (Uint8)(ANIMAL(work) & ANIMAL_CHANCE_BITS) >>
                             ANIMAL_CHANCE_SHIFT;
                    chance *= ANIMAL_CHANCE_STEP;
                    if ((Float)rand() * RAND_SCALE <
                        chance + ANIMAL_CHANCE_BASE) {
                        animal_release(work->pos.x, work->pos.y + ANIMAL_RISE,
                                       work->pos.z,
                                       (ANIMAL(work) & ANIMAL_KIND_BITS) %
                                           ANIMAL_RELEASE_KINDS);
                    }
                }
            }
        }
    }
    /* @bug memory: body_step reads and writes the body through null where
     * body_init found no room. chamber_lion_head_init tests for that null at
     * 0x8c5097e0. the call at 0x8c5098ce takes it untested */
    body_step(task);
}

static void chamber_lion_head_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&chamber_lion_head_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (chamber_lion_head_built) {
        njDirectDrawObject(chamber_lion_head_builds[0].built);
    } else {
        njCnkEasyDrawModel(&chamber_lion_head_model);
    }
    njPopMatrixEx();
}
