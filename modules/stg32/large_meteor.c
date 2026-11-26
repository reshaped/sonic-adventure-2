/* the large meteor, METEOBIG in Meteor Herd's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/item_box.h"
#include "sound/sound.h"
#include "stg32/meteor_gate.h"
#include "stg32/meteors.h"
#include "stg32/scenery_shapes.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define METEOR_ATTACK                                                          \
    (SHAPE_ATTACK_LEVEL(2) | SHAPE_HURT | SHAPE_HURTS_ENEMIES |                \
     SHAPE_HURTS_OBJECTS)
#define METEOR_SHAPES 1
/* in the work's submode: the meteor breaks apart at its next step */
#define BREAKING(work) ((work)->submode)
/* the meteor starts START_LIFT above its placement, unless a player lies within
 * START_CLEARANCE at its init */
#define START_CLEARANCE 180.0F
#define START_LIFT 5.0F
#define BEACON_PERIOD 14
#define BEACON_REST 10
#define BEACON_FADE 3
#define LAMP_PULSE_SHIFT 9
/* the meteor breaks into FRAGMENTS pieces, which fall to FRAGMENT_FLOOR, far
 * below the stage. the first CHIP_FRAGMENTS are chips, and the rest shards.
 * piece i flies out at FRAGMENT_SPEED, plus FRAGMENT_SPEED_RANGE times i /
 * FRAGMENTS, in a random direction. it starts from half the shape's radius out
 * to the whole radius. a faster piece starts farther out. the explosion grows
 * to EXPLOSION_SCALE times the shape's radius */
#define FRAGMENTS 20
#define CHIP_FRAGMENTS 5
#define FRAGMENT_FLOOR (-100000.0F)
#define FRAGMENT_SPEED 1.0F
#define FRAGMENT_SPEED_RANGE 1.5F
#define EXPLOSION_SCALE 8.0F
/* an attack pushes where the attacker's spd is longer than MIN_PUSH_SPEED.
 * its spd.y must be no lower than -MAX_PUSH_FALL. the push adds push_speed
 * along that spd, and PUSH_LIFT up */
#define MIN_PUSH_SPEED 0.3F
#define MAX_PUSH_FALL 0.5F
#define PUSH_LIFT 1.3F
/* ROLL_SOUND plays while the meteor's spd is longer than ROLL_SPEED */
#define ROLL_SPEED 0.5F
#define PUSH_SOUND 4
#define ROLL_SOUND 5
#define BREAK_SOUND 6
#define SOUND_PRIORITY 0
#define ROLL_SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define ROLL_SOUND_STEPS 15

NJS_TEXLIST *large_meteor_textures[] = {&large_meteor_shard_texlist,
                                        &large_meteor_chip_texlist,
                                        &large_meteor_texlist, NULL};
/* clang-format off */
static struct collision_shape meteor_shape[METEOR_SHAPES] = {
    {SHAPE_ID_BREAKER, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     METEOR_ATTACK, SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 35.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;
static Float push_speed = 3.0F;

static void large_meteor_destroy(struct task *task);
static void large_meteor_draw(struct task *task);
static void large_meteor_step(struct task *task);

void large_meteor_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (player_near(&work->pos, START_CLEARANCE)) {
        return;
    }
    task->step = large_meteor_step;
    task->draw = large_meteor_draw;
    task->destructor = large_meteor_destroy;
    work->pos.y += START_LIFT;
    collision_init(task, meteor_shape, METEOR_SHAPES, COLLISION_UNK_9);
    body_init(task);
    work->flags &= ~FALL_OFFERABLE;
    if (task->force.p) {
        BODY(task)->floor_bounce = 0.5F;
        BODY(task)->flags |= BODY_RESTS;
    }
    /* @bug memory: writes the body's sizes through null where body_init gets
     * no record from the heap (FMOV.S at 0x8c50365e). the test of task->force
     * above expects that null (TST at 0x8c503638) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where body_init got no body, the init
     * destroys the meteor, and set.c makes it again on a later pass */
    if (task->force.p == NULL) {
        task_destroy(task);
        return;
    }
#endif
    BODY(task)->radius = 50.0F;
    BODY(task)->probe_offset.y = 0.0F;
    BODY(task)->bottom = -25.0F;
    BODY(task)->top = 75.0F;
    BODY(task)->step_down = 150.0F;
}

static void large_meteor_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->collision) {
        body_destroy(task);
    }
}

static void large_meteor_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Uint32 beacon;
    Uint32 pulse;
    Uint32 color;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD X";
        (void)"<- ANGSPD Y";
        (void)"<- ANGLE Z";
        (void)"<- MODEL(0-1)";
    }
    njSetTexture(&large_meteor_texlist);
    light_apply(LIGHT_DEBRIS);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njCnkModDrawObject(meteor_fireball_shadow);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(large_meteor_body->model);
    pulse = color_blend(bright_color, dim_color,
                        (njSin(game_state.steps << LAMP_PULSE_SHIFT) + 1.0F) *
                            0.5F);
    color = color_blend(bright_color, dim_color,
                        blink_level(BEACON_PERIOD, BEACON_REST, BEACON_FADE));
    /* @hack redundant: beacon, a copy of color, where color itself would do */
    beacon = color;
    model_draw_tinted(large_meteor_beacon->model, beacon);
    object = large_meteor_lamp;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateEx(object->ang, 0);
    model_draw_tinted(object->model, pulse);
    light_apply(light_after_draw);
    njPopMatrixEx();
}

static void large_meteor_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    NJS_POINT3 pos;
    NJS_VECTOR direction;
    NJS_VECTOR velocity;
    struct angles turn;
    struct task *other;
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *textures;
    Float fraction;
    Float scale;
    int attacked;
    int hits;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if ((BODY(task)->flags & BODY_HIT_WALL) || BREAKING(work)) {
        point.y = work->collision->shapes->radius;
        point.x = 0.0F;
        point.z = 0.0F;
        direction.x = 0.0F;
        direction.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        for (i = 0; i < FRAGMENTS; i++) {
            fraction = i * (1.0F / FRAGMENTS);
            direction.y = FRAGMENT_SPEED + (FRAGMENT_SPEED_RANGE * fraction);
            point.y =
                work->collision->shapes->radius * (fraction + 1.0F) * 0.5F;
            turn.y = 0;
            turn.z = RANDOM_TWO_TURNS();
            njRotateZ(NULL, turn.z);
            turn.x = RANDOM_TWO_TURNS();
            njRotateX(NULL, turn.x);
            njCalcPoint(NULL, &point, &pos);
            njCalcVector(NULL, &direction, &velocity);
            if (i < CHIP_FRAGMENTS) {
                model = large_meteor_chip->model;
                textures = &large_meteor_chip_texlist;
            } else {
                model = large_meteor_shard->model;
                textures = &large_meteor_shard_texlist;
            }
            debris_launch_built_flags(&pos, &turn, FRAGMENT_FLOOR, &velocity,
                                      model, NULL, textures,
                                      DEBRIS_LIGHT | DEBRIS_FADE_COLORS);
            matrix_load_parent();
        }
        njPopMatrix(2);
        explosion_create_sized(&work->pos, work->collision->shapes->radius *
                                               EXPLOSION_SCALE);
        sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME,
                      &work->pos);
        task_kill(task);
        /* @hack redundant: a block no path reaches, which sets direction.y to
         * 1 */
        if (0) {
            direction.y = 1.0F;
        }
    } else {
        /* with no hits, the test that sets BREAKING reads other, which no hit
         * has set. the hit count of 0 fails that test whatever other holds */
#ifdef NONMATCHING
        /* this side sets other to the 0 that SHC's code holds there */
        other = NULL;
#endif
        hits = collision_get_hit_count(task);
        attacked = FALSE;
        while (hits--) {
            /* @hack spelling: the assignment to other inside the call, where a
             * statement of its own would do */
            if (player_attacks_task(
                    task, other = collision_get_hit(task, hits)->task)) {
                attacked = TRUE;
                break;
            }
        }
        if (attacked && other->motion.p) {
            point = TASK_MOTION_OF(other)->spd;
            fraction = njScalor(&point);
            if (fraction > MIN_PUSH_SPEED && point.y >= -MAX_PUSH_FALL) {
                scale = push_speed / fraction;
                /* @hack spelling: the scale as a walk of three Floats, where
                 * three statements on point's fields would do */
                {
                    Float *coord = &point.x;
                    *coord++ *= scale;
                    *coord++ *= scale;
                    *coord *= scale;
                }
                point.y += PUSH_LIFT;
                /* @hack spelling: the add as a walk of three Floats, where
                 * three statements on spd's fields would do */
                {
                    Float *coord = &TASK_MOTION_OF(task)->spd.x;
                    Float *push = &point.x;
                    *coord++ += *push++;
                    *coord++ += *push++;
                    *coord += *push;
                }
                sound_play(PUSH_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            }
        } else if (other != player_tasks[0] && other != player_tasks[1] &&
                   collision_get_hit_count(task)) {
            BREAKING(work) = TRUE;
        }
        if (njScalor(&TASK_MOTION_OF(task)->spd) > ROLL_SPEED) {
            sound_play_at_for(ROLL_SOUND, work, ROLL_SOUND_PRIORITY,
                              SOUND_VOLUME, ROLL_SOUND_STEPS, &work->pos);
        }
        body_step(task);
    }
}
