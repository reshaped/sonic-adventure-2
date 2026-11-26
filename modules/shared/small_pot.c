/* the small pot, MINITSUBO in Dry Lagoon's object list: a pot a player can
 * pick up and carry. the first time a player holds it, it may release a
 * small animal */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/small_pot.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define POT_SHAPES 3
/* Egg Quarters fits a smaller sphere above the pot's origin */
#ifndef POT_SPHERE_CENTER_Y
#define POT_SPHERE_CENTER_Y 0.0F
#define POT_SPHERE_RADIUS 3.8F
#endif

/* ANIMAL(work) keeps the low byte of ang.z, the pot's animal byte of
 * object/animal.h. the chance that the pot's first hold releases the animal
 * is ANIMAL_CHANCE_BASE plus ANIMAL_CHANCE_STEP times the ANIMAL_CHANCE_BITS
 * field. the animal starts ANIMAL_RISE above the pot */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F
/* the placement's state once a player has held the pot. a hold releases an
 * animal only while the state is 0 */
#define STATE_HELD 1

#define FLOOR_BOUNCE 0.39F

static void small_pot_destroy(struct task *task);
static void small_pot_step(struct task *task);
static void small_pot_draw(struct task *task);

NJS_TEXLIST *small_pot_texlists[] = {&small_pot_texlist, NULL};
/* a cylinder an attack can hurt, which pushes nothing, then a thin cylinder
 * and a sphere, which push */
/* clang-format off */
static struct collision_shape small_pot_shapes[POT_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS),
     0, SHAPE_UNK_2000 | SHAPE_HURTABLE, {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F,
     0, {0, 0, 0}},
    {0, SHAPE_CYLINDER,
     PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS | PUSH_FIRM(PUSH_ENEMIES), 0,
     SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE,
     PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS |
         PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS),
     0, 0, {0.0F, POT_SPHERE_CENTER_Y, 0.0F}, POT_SPHERE_RADIUS, 0.0F, 0.0F,
     0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 small_pot_hold_turn = 0x1000;

void small_pot_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw set before the step, where struct task's
         * order would set the step first */
        task->draw = small_pot_draw;
        task->step = small_pot_step;
        task->destructor = small_pot_destroy;
        ANIMAL(work) = work->ang.z;
        collision_init(task, small_pot_shapes, POT_SHAPES, COLLISION_OBJECT);
        body_init(task);
        if (task->force.p != NULL) {
            BODY(task)->floor_bounce = FLOOR_BOUNCE;
            BODY(task)->flags |= BODY_RESTS;
        }
#ifdef NONMATCHING
        /* this side repairs the tag in small_pot_step: a pot without a body
         * kills itself, so it ends at its next step, before body_step */
        if (task->force.p == NULL) {
            task_kill(task);
        }
#endif
        work->submode = 0;
        motion->hold_turn = small_pot_hold_turn;
    }
}

static void small_pot_destroy(struct task *task)
{
    body_destroy(task);
}

static void small_pot_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    Float chance;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches. SHC still places its strings
     * in this source's constants, saves a register for its first local and
     * keeps a word of the frame for its second. the statements are stand-ins
     * that only keep the two locals */
    if (0) {
        struct task_work *record_work = player_work[0];
        int value;

        record_work->timer = placement_get_state(task);
        record_work->mode = value;
        work->submode = record_work->submode;
        (void)&value;
        (void)"<- ANGLE Y";
        (void)"<- MINIMAL AP(  ) LEV( )";
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        player = player_holding(task);
        if (player >= 0) {
            work->ang.y = QUARTER_TURN - player_work[player]->ang.y;
            if (task->placement != NULL && !placement_get_state(task)) {
                placement_set_state(task, STATE_HELD);
                if (ANIMAL(work) & ANIMAL_CHANCE_BITS) {
                    /* @hack type: the masked byte cast to a Uint8, where the
                     * masked value alone would do */
                    chance = (Uint8)(ANIMAL(work) & ANIMAL_CHANCE_BITS) >>
                             ANIMAL_CHANCE_SHIFT;
                    chance *= ANIMAL_CHANCE_STEP;
                    if (rand() * RAND_SCALE < chance + ANIMAL_CHANCE_BASE) {
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
     * body_init found no room. small_pot_init tests for that null at 0x3e of
     * the object's code. the call at 0x12e of the object's code takes it
     * untested */
    body_step(task);
}

static void small_pot_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&small_pot_texlist);
    njCnkEasyDrawModel(&small_pot_model);
    njPopMatrixEx();
}
