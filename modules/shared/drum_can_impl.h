/* the drum can's code. shared/drum_can.c compiles it for Pumpkin Hill,
 * stg06/drum_can.c for Sky Rail and stg07/drum_can.c for Aquatic Mine. the
 * includer defines the can's art first, and may define the hooks below */
#ifndef SHARED_DRUM_CAN_IMPL_H
#define SHARED_DRUM_CAN_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/breakable.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/missile.h"
#include "system/task.h"
#include "system/util.h"

/* the bits of ang.z that hold the can's id among the missiles' targets */
#define TARGET_ID_BITS 0x1f
#define DRUM_CAN_SHAPES 1
#define DRUM_CAN_PIECES 6
/* tick counts the steps since the hit, from 1. the can breaks into its
 * pieces at BREAK_TICK, and kills itself at the first tick past LAST_TICK */
#define BREAK_TICK 2
#define LAST_TICK 4
/* the pieces fly straight away from the can's pos at PIECE_SPEED, plus up to
 * PIECE_SPREAD more at random. each also takes PIECE_RISE along y. their
 * floor lies FLOOR_OFFSET along y from the can's pos */
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_RISE 0.5F
#define FLOOR_OFFSET (-10000.0F)
#define BREAK_SOUND_KIND 0
#define BREAK_POINTS 20
/* the hit bursts DUST_PUFFS puffs of dust about the point DUST_HEIGHT above
 * the can's pos. each puff starts on a sphere about that point, of radius
 * PUFF_RADIUS plus up to PUFF_RADIUS_RANGE. its speed is PUFF_SPEED plus up
 * to PUFF_SPEED_RANGE, and its size PUFF_SCALE plus up to PUFF_SCALE_RANGE */
#define DUST_PUFFS 17
#define DUST_HEIGHT 10.0F
#define PUFF_RADIUS 6.0F
#define PUFF_RADIUS_RANGE 3.0F
#define PUFF_SPEED 1.0F
#define PUFF_SPEED_RANGE 0.7F
#define PUFF_SCALE 3.0F
#define PUFF_SCALE_RANGE 4.0F

/* the hooks, each Pumpkin Hill's unless the includer defines it. these two
 * play the sound and add the score at each step after the hit */
#ifndef DRUM_CAN_BREAK_SOUND
#define DRUM_CAN_BREAK_SOUND(work)                                             \
    object_sound_play(BREAK_SOUND_KIND, &(work)->pos)
#endif
#ifndef DRUM_CAN_BREAK_SCORE
#define DRUM_CAN_BREAK_SCORE() score_add(BREAK_POINTS)
#endif
/* the collision's registration at each step up to the hit's */
#ifndef DRUM_CAN_WHOLE_REGISTER
#define DRUM_CAN_WHOLE_REGISTER(task) collision_register(task)
#endif
/* the collision's registration at every step */
#ifndef DRUM_CAN_STEP_REGISTER
#define DRUM_CAN_STEP_REGISTER(task) ((void)0)
#endif
/* what the hit does before it bounces player 0 */
#ifndef DRUM_CAN_HIT_FIRST
#define DRUM_CAN_HIT_FIRST(work) ((void)0)
#endif
/* what the hit does after it bounces player 0 */
#ifndef DRUM_CAN_HIT
#define DRUM_CAN_HIT(work) ((void)0)
#endif

static struct collision_shape drum_can_shapes[DRUM_CAN_SHAPES] = {
#include "shared/drum_can_impl/drum_can_shapes.inc"
};

/* the pieces the can breaks into: its lid and five pieces of its side */
static struct debris_part drum_can_pieces[DRUM_CAN_PIECES] = {
#include "shared/drum_can_impl/drum_can_pieces.inc"
};

static void drum_can_destroy(struct task *task);
static void drum_can_step(struct task *task);
static void drum_can_draw(struct task *task);

void drum_can_init(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    if (!placement_try_leave(task)) {
        task->draw = drum_can_draw;
        task->step = drum_can_step;
        task->destructor = drum_can_destroy;
        work->submode = 0;
        collision_init(task, drum_can_shapes, DRUM_CAN_SHAPES,
                       COLLISION_OBJECT);
        missile_target_add(work->ang.z & TARGET_ID_BITS, &work->hit);
        work->tick = 0;
    }
}

static void drum_can_destroy(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    /* @bug memory: leaves the can's missile target in the table after its
     * work is freed. it happens where collision_init gave the work no record.
     * this test then skips the removal, by the MOV.L at 0x8c50496c in Pumpkin
     * Hill's image. drum_can_init adds the target all the same, by the JSR at
     * 0x8c504958. a missile of the can's id then writes into the freed work.
     * the variants hold the same call and test */
#ifdef NONMATCHING
    /* this side repairs the tag above: the can removes its target whether or
     * not it has a record, as drum_can_init adds the target either way */
    missile_target_remove(&work->hit);
#else
    if (work->collision != NULL) {
        missile_target_remove(&work->hit);
    }
#endif
}

static void drum_can_step(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    struct collision_work *collision;
    int i;
    /* @hack spelling: zero, a local for 0, where 0.0F in place would do */
    Float zero;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->tick) {
        pos = work->pos;
        zero = 0.0F;
        /* @hack redundant: zero added to pos.y */
        pos.y += zero;
        spd.x = zero;
        spd.y = PIECE_RISE;
        spd.z = zero;
        DRUM_CAN_BREAK_SOUND(work);
        if (work->tick == BREAK_TICK) {
            debris_break(&pos, work->ang.y, drum_can_pieces, DRUM_CAN_PIECES,
                         PIECE_SPEED, PIECE_SPREAD, work->pos.y + FLOOR_OFFSET,
                         &spd);
        }
        DRUM_CAN_BREAK_SCORE();
        if (work->tick > LAST_TICK) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
        work->tick++;
    } else {
        /* @bug memory: reads the collision record through null where
         * collision_init gave the work none. in Pumpkin Hill's image the
         * MOV.W at 0x8c504ac4 reads its flags. where they hold
         * COLLISION_TOUCHED, the MOV.L at 0x8c504ad4 reads its hit.
         * drum_can_destroy tests for that null, at 0x8c50496c. the variants
         * hold the same reads */
#ifdef NONMATCHING
        /* this side repairs the tag above: a can without a record ends, and
         * its placement makes it again while in range. drum_can_destroy
         * removes its missile target */
        if (work->collision == NULL) {
            task_kill(task);
            return;
        }
#endif
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((((collision = work->collision)->flags & COLLISION_TOUCHED) &&
             player_attacks_task(task, collision->hit->owner)) ||
            work->hit != NULL) {
            pos = work->pos;
            pos.y += DUST_HEIGHT;
            for (i = 0; i < DUST_PUFFS; i++) {
                dust_short_sphere(
                    &pos,
                    PUFF_RADIUS + (rand() * (PUFF_RADIUS_RANGE * RAND_SCALE)),
                    PUFF_SPEED + (rand() * (PUFF_SPEED_RANGE * RAND_SCALE)),
                    PUFF_SCALE + (rand() * (PUFF_SCALE_RANGE * RAND_SCALE)));
            }
            work->tick = 1;
            DRUM_CAN_HIT_FIRST(work);
            enemy_bounce_player(0);
            DRUM_CAN_HIT(work);
        }
        DRUM_CAN_WHOLE_REGISTER(task);
    }
    DRUM_CAN_STEP_REGISTER(task);
}

static void drum_can_draw(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    njSetTexture(&drum_can_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&drum_can_model);
    njPopMatrix(1);
}

#endif
