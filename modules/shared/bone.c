/* the bone, BONE in Wild Canyon's object list: a cow's bone that a player
 * picks up and carries. the first pickup may release an animal */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/bone.h"
#include "shared/canyon_util.h"
#include "shared/common_object.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/bone/text.inc"

#define BONE_SHAPES 3
/* the placement's state once a player has held the bone. a hold releases an
 * animal only while the state is 0 */
#define STATE_HELD 1

/* the work's tick holds ang.z's low byte, the bone's animal byte of
 * object/animal.h. a count n in its ANIMAL_CHANCE_BITS makes a release's
 * chance ANIMAL_CHANCE_BASE plus n times ANIMAL_CHANCE_STEP. the animal starts
 * ANIMAL_RISE above the bone */
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F

#define FLOOR_BOUNCE 0.39F

/* the placement editor's captions for the bone's yaw and its animal */
static const char bone_edit_yaw[12] = "<- ANGLE Y";
static const char bone_edit_animal[28] = "<- MINIMAL AP(  ) LEV( )";

static NJS_TEXNAME bone_texlist_names[1] = {
#include "shared/bone/bone_texlist_names.inc"
};

static NJS_TEXLIST bone_texlist = {
#include "shared/bone/bone_texlist.inc"
};

static Sint16 bone_polygons[309] = {
#include "shared/bone/bone_polygons.inc"
};

static Sint32 bone_vertices[171] = {
#include "shared/bone/bone_vertices.inc"
};

static NJS_CNK_MODEL bone_model = {
#include "shared/bone/bone_model.inc"
};

static NJS_CNK_OBJECT bone_object = {
#include "shared/bone/bone_object.inc"
};

static void bone_destroy(struct task *task);
static void bone_step(struct task *task);
static void bone_draw(struct task *task);

NJS_TEXLIST *bone_texlists[2] = {
#include "shared/bone/bone_texlists.inc"
};
/* clang-format off */
static struct collision_shape bone_shapes[BONE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, SHAPE_UNK_2000 | SHAPE_HURTABLE,
     {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ENEMIES),
     0, SHAPE_UNK_2000,
     {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0,
     {0.0F, 0.0F, 0.0F}, 3.8F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the hold_turn the bone gives its motion record */
static Sint32 bone_hold_turn = 0xd80;

void bone_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    if (!half_try_leave(task) && !placement_try_leave(task)) {
        task->draw = bone_draw;
        task->step = bone_step;
        task->destructor = bone_destroy;
        work->tick = work->ang.z;
        collision_init(task, bone_shapes, BONE_SHAPES, COLLISION_OBJECT);
        body_init(task);
        if (task->force.p) {
            BODY(task)->floor_bounce = FLOOR_BOUNCE;
            BODY(task)->flags |= BODY_RESTS;
        }
        work->submode = 0;
        motion->hold_turn = bone_hold_turn;
    }
}

static void bone_destroy(struct task *task)
{
    body_destroy(task);
}

static void bone_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    Float chance;
    /* @hack redundant: frame and unused, locals that only size the stack
     * frame */
    Uint32 frame[1];
    Uint32 *unused = frame;

    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        task_destroy(NULL);
        task_destroy(NULL);
        task_destroy(NULL);
    }
    if (!half_try_leave(task) && !placement_try_leave(task)) {
        if (work->flags & WORK_HELD_BY_PLAYER) {
            player = player_holding(task);
            if (player >= 0) {
                work->ang.y = -player_work[player]->ang.y + QUARTER_TURN;
                if (task->placement && !placement_get_state(task)) {
                    placement_set_state(task, STATE_HELD);
                    if (work->tick & ANIMAL_CHANCE_BITS) {
                        chance = (Uint8)(work->tick & ANIMAL_CHANCE_BITS) >>
                                 ANIMAL_CHANCE_SHIFT;
                        chance *= ANIMAL_CHANCE_STEP;
                        if ((Float)rand() * RAND_SCALE <
                            chance + ANIMAL_CHANCE_BASE) {
                            animal_release(work->pos.x,
                                           work->pos.y + ANIMAL_RISE,
                                           work->pos.z,
                                           (work->tick & ANIMAL_KIND_BITS) %
                                               ANIMAL_RELEASE_KINDS);
                        }
                    }
                }
            }
        }
        /* @bug memory: reads and writes the body through null where
         * body_init found no room. bone_init tests for that null at 0x48 of
         * the object's code. bone_step reads it in its call of body_step at
         * 0x142 */
#ifdef NONMATCHING
        /* this side repairs the tag above: a bone without a body ends, and
         * its placement makes it again while in range */
        if (task->force.p == NULL) {
            task_kill(task);
            return;
        }
#endif
        body_step(task);
    }
}

static void bone_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&bone_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&bone_model);
    njPopMatrixEx();
}
