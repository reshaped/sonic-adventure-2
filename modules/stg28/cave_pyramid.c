/* the pyramid, PYRAMID in Pyramid Cave's object list: a small pyramid a
 * player can pick up and carry. the first time a player holds it, it may
 * release a small animal */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg28/cave_pyramid.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define PYRAMID_SHAPES 3

/* ANIMAL(work) is the work's tick, a byte, where the init keeps ang.z's low
 * byte: the pyramid's animal byte of object/animal.h. the chance that the
 * pyramid's first hold releases the animal is ANIMAL_CHANCE_BASE plus
 * ANIMAL_CHANCE_STEP times the ANIMAL_CHANCE_BITS field. the animal starts
 * ANIMAL_RISE above the pyramid */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CHANCE_STEP 0.1F
#define ANIMAL_CHANCE_BASE 0.05F
#define ANIMAL_RISE 3.0F
/* the placement's state once a player has held the pyramid. a hold releases
 * an animal only while the state is 0 */
#define STATE_HELD 1

#define FLOOR_BOUNCE 0.39F

#include "stg28/cave_pyramid/text.inc"
#include "stg28/models/cave_pyramid.h"

static void cave_pyramid_destroy(struct task *task);
static void cave_pyramid_step(struct task *task);
static void cave_pyramid_draw(struct task *task);

NJS_TEXLIST *cave_pyramid_texlists[2] = {
#include "stg28/cave_pyramid/cave_pyramid_texlists.inc"
};
/* a cylinder an attack can hurt, which pushes no group's shapes. then a small
 * sphere and a larger one, which push */
static struct collision_shape cave_pyramid_shapes[PYRAMID_SHAPES] = {
#include "stg28/cave_pyramid/cave_pyramid_shapes.inc"
};

void cave_pyramid_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = cave_pyramid_draw;
        task->step = cave_pyramid_step;
        task->destructor = cave_pyramid_destroy;
        ANIMAL(work) = work->ang.z;
        work->ang.z = 0;
        collision_init(task, cave_pyramid_shapes, PYRAMID_SHAPES,
                       COLLISION_OBJECT);
        body_init(task);
#ifdef NONMATCHING
        /* this side repairs the tag in cave_pyramid_step: where body_init
         * found no room, the init destroys the pyramid, and set.c makes it
         * again on a later pass */
        if (task->force.p == NULL) {
            task_destroy(task);
            return;
        }
#endif
        if (task->force.p != NULL) {
            BODY(task)->floor_bounce = FLOOR_BOUNCE;
            BODY(task)->flags |= BODY_RESTS;
        }
    }
}

static void cave_pyramid_destroy(struct task *task)
{
    body_destroy(task);
}

static void cave_pyramid_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    Float chance;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which SHC
     * still places in this source's constants, and for its statements. the
     * image holds none of its code, so the statements are stand-ins, without
     * which SHC writes other code */
    if (0) {
        struct task_work *record_work = player_work[0];
        int value;

        record_work->timer = placement_get_state(task);
        record_work->mode = value;
        work->submode = record_work->submode;
        (void)&value;
        (void)CAVE_PYRAMID_EDITOR_ANGLE_Y;
        (void)CAVE_PYRAMID_EDITOR_MINIMAL_APPEARANCE_LEVEL;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        player = player_holding(task);
        if (player >= 0) {
            work->ang.x = player_work[player]->ang.x;
            work->ang.y = QUARTER_TURN - player_work[player]->ang.y;
            work->ang.z = player_work[player]->ang.z;
            if (task->placement != NULL && !placement_get_state(task)) {
                placement_set_state(task, STATE_HELD);
                if (ANIMAL(work) & ANIMAL_CHANCE_BITS) {
                    /* @hack type: the masked byte cast to a Uint8 before
                     * the shift, where the masked value alone would do */
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
     * body_init found no room. cave_pyramid_init tests for that null at
     * 0x8c50b79a. the call at 0x8c50b88c takes it untested */
    body_step(task);
}

static void cave_pyramid_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&cave_pyramid_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&cave_pyramid_model);
    njPopMatrixEx();
}
