/* the springs, SPRA and SPRB in a stage's object list: a base, a coil and a
 * top. a touch launches a player along the spring's y axis, and the spring bobs
 * after */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/spring.h"
#include "shared/spring_work.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a launch sets tick to 0. a touch launches only once tick passes
 * TOUCH_WAIT. a touch while tick is TOUCH_WAIT or less sets it back to 0 */
#define TOUCH_WAIT 2
/* a spring launches only the players in slots 0 and 1 */
#define SPRING_PLAYERS 2

/* the coil draws at COIL_WIDTH of its size along x and z */
#define COIL_WIDTH 0.98F

#define SPRING_SHAPES 1
enum spring_node {
    A_BASE,
    A_COIL,
    A_TOP,
    B_BASE,
    B_COIL,
    B_TOP,
    SPRING_BUILDS
};

/* spring a's shape is an upright cylinder 12 across and 20 high. spring b's
 * is a ball of radius 14. each is centered on its spring's pos */
/* clang-format off */
static struct collision_shape spring_a_shapes[SPRING_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL), SHAPE_HURTS_ALL, 0,
     {0.0F, 0.0F, 0.0F}, 6.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape spring_b_shapes[SPRING_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL), SHAPE_HURTS_ALL, 0,
     {0.0F, 0.0F, 0.0F}, 14.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* TRUE once one call of spring_build has built all six nodes */
static Bool spring_built = FALSE;
static struct model_build spring_builds[SPRING_BUILDS] = {
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_a_base_object, NULL},
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_a_coil_object, NULL},
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_a_top_object, NULL},
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_b_base_object, NULL},
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_b_coil_object, NULL},
    {MODEL_BUILD_ALONE, &spring_texlist, &spring_b_top_object, NULL},
};

static void spring_launch(struct task *task, Uint8 is_spring_b);
static void spring_a_destroy(struct task *task);
static void spring_a_draw(struct task *task);
static void spring_a_draw_built(struct task *task);
static void spring_b_draw(struct task *task);
static void spring_b_draw_built(struct task *task);
static void spring_b_destroy(struct task *task);

Uint8 *spring_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, spring_builds, SPRING_BUILDS) ==
        SPRING_BUILDS) {
        spring_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void spring_a_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    /* @hack spelling: player_task, a local for player_tasks[player], where
     * the element read in place would do */
    struct task *player_task;
    /* @hack type: player a Uint8, where an int would do */
    Uint8 player;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = spring_a_destroy;
        if (spring_built) {
            task->draw = spring_a_draw_built;
        } else {
            task->draw = spring_a_draw;
        }
        collision_init(task, spring_a_shapes, SPRING_SHAPES, COLLISION_OBJECT);
        work->collision->flags |= COLLISION_TARGET;
        PHASE_A(work) = PHASE_REST;
        PHASE_SPEED(work) = 0.0F;
        BOB(task) = BOB_START;
        work->mode = MODE_WAIT;
        break;
    case MODE_WAIT:
        if (work->timer) {
            work->timer--;
        }
        STRETCH(task) =
            -njAbs(BOB(task) * njSin(PHASE_A(work) * ANGLE_PER_DEGREE));
        PHASE_A(work) += PHASE_SPEED(work);
        if (PHASE_A(work) >= SLOW_PHASE &&
            PHASE_A(work) < SLOW_PHASE + PHASE_SPEED(work)) {
            PHASE_SPEED(work) = A_PHASE_SPEED;
        }
        BOB(task) = njCos((PHASE_A(work) - PHASE_REST) *
                          (ANGLE_PER_DEGREE / BOB_SLOWDOWN)) *
                    BOB_SIZE;
        if (BOB(task) < 0.0F) {
            PHASE_A(work) = PHASE_REST;
            PHASE_SPEED(work) = 0.0F;
        }
        work->tick++;
        /* @hack spelling: collision_next_player called in each arm, where one
         * call before the test would do */
        if (work->tick > TOUCH_WAIT) {
            hit = collision_next_player(task);
            if (hit != NULL) {
                for (player = 0; player < SPRING_PLAYERS; player++) {
                    player_task = player_tasks[player];
                    if (hit == player_task) {
                        STRETCH(task) = LAUNCH_STRETCH;
                        LAUNCH_SLOT(work) = player;
                        PHASE_SPEED(work) = TOUCH_PHASE_SPEED;
                        PHASE_A(work) = TOUCH_PHASE;
                        work->mode = MODE_LAUNCH;
                        work->tick = 0;
                        break;
                    }
                }
            }
        } else {
            hit = collision_next_player(task);
            if (hit != NULL) {
                work->tick = 0;
            }
        }
        if (work->mode != MODE_LAUNCH) {
            collision_register(task);
        }
        if (work->tick > TICK_LIMIT) {
            work->tick = TICK_LIMIT;
        }
        break;
    case MODE_LAUNCH:
        sound_play(SPRING_SOUND, NULL, 0, 0);
        work->tick = 0;
        spring_launch(task, FALSE);
        break;
    case MODE_KILL:
        placement_kill(task);
        break;
    }
}

/* a player whose record holds PLAYER_MISSED is neither put on the top nor
 * pulled. spring b still puts a slow one at the top's x and z */
/* @hack type: is_spring_b a Uint8, where an int would do */
static void spring_launch(struct task *task, Uint8 is_spring_b)
{
    struct task_work *work = task->work;
    struct task_work *player;
    NJS_POINT3 offset = {0.0F, LAUNCH_SPEED, 0.0F};
    /* @hack spelling: top declared ahead of up, not in the order of their
     * first use */
    NJS_POINT3 top;
    NJS_POINT3 up;
    /* @hack type: slot a Uint8, where the int of LAUNCH_SLOT(work) would do */
    Uint8 slot;
    Float dx;
    Float dy;
    Float dz;
    Float distance;
    Float speed;

    player = player_work[LAUNCH_SLOT(work)];
    /* @hack spelling: offset.y set to the whole sum, where adding
     * ADD_SPEED(work) to the initializer's y would do */
    offset.y = ADD_SPEED(work) + LAUNCH_SPEED;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &up);
    njPopMatrixEx();
    top.x = work->pos.x + up.x;
    top.y = work->pos.y + up.y;
    top.z = work->pos.z + up.z;
    /* the mode stays MODE_LAUNCH, so the next step tries the launch again */
    if (player == NULL) {
        return;
    }
    slot = LAUNCH_SLOT(work);
    dx = top.x - player->pos.x;
    /* @hack redundant: dy, which nothing reads */
    dy = top.y - player->pos.y;
    dz = top.z - player->pos.z;
    /* @hack spelling: dy's njAbs a statement of its own, where njAbs of the
     * difference would do */
    dy = njAbs(dy);
    distance = njSqrt((dx * dx) + (dz * dz));
    /* @bug uninitialized: the speed test reads speed unset where
     * player_ground_speed returns FALSE. nothing reads the call's result (the
     * call and the test's read of speed, at 0x3b0 and 0x3b4 of the object's
     * code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: speed starts at 0, so a player off the
     * ground launches as a slow one does. in the image, neither spring's step
     * writes the high three bytes of speed's stack word before the read. so
     * which arm the image takes turns on what earlier code left there */
    speed = 0.0F;
#endif
    player_ground_speed(slot, &speed);
    /* the test's STRETCH != LAUNCH_STRETCH never holds. the touch set STRETCH
     * to LAUNCH_STRETCH, and nothing has changed it since */
    /* @hack spelling: RUN_SPEED > speed, where speed < RUN_SPEED would do */
    if (RUN_SPEED > speed || STRETCH(task) != LAUNCH_STRETCH) {
        /* @hack spelling: the launch written out under each arm of the
         * test, where one copy after it would do */
        if (distance < PULL_RANGE && distance >= PULL) {
            /* @hack spelling: the record indexed by LAUNCH_SLOT(work) again,
             * where slot would do */
            if (!(player_records[LAUNCH_SLOT(work)]->flags & PLAYER_MISSED)) {
                player_request(slot, PLAYER_REQUEST_ANIM, 0);
                player->pos.x += (dx / distance) * PULL;
                player->pos.z += (dz / distance) * PULL;
                player_request(slot, PLAYER_REQUEST_RELEASE, 0);
            }
            if (!is_spring_b) {
                player_bounce(slot, up.x, up.y, up.z);
            } else {
                player->pos.x = top.x;
                player->pos.y = work->pos.y + (up.y * B_RISE);
                player->pos.z = top.z;
                if (STICK_LOCK(work) <= 0.0F) {
                    STICK_LOCK(work) = 0.0F;
                }
                player_launch(slot, ADD_SPEED(work) + LAUNCH_SPEED, &work->ang,
                              STICK_LOCK(work));
            }
            /* @hack spelling: this arm and each below store the mode
             * themselves, where one store after the test would do */
            work->mode = MODE_WAIT;
        } else {
            if (!is_spring_b) {
                player_bounce(slot, up.x, up.y, up.z);
            } else {
                player->pos.x = top.x;
                player->pos.y = work->pos.y + (up.y * B_RISE);
                player->pos.z = top.z;
                if (STICK_LOCK(work) <= 0.0F) {
                    STICK_LOCK(work) = 0.0F;
                }
                player_launch(slot, ADD_SPEED(work) + LAUNCH_SPEED, &work->ang,
                              STICK_LOCK(work));
            }
            work->mode = MODE_WAIT;
        }
    } else {
        /* @hack spelling: the record indexed by LAUNCH_SLOT(work) again, where
         * slot would do */
        if (!(player_records[LAUNCH_SLOT(work)]->flags & PLAYER_MISSED)) {
            player_request(slot, PLAYER_REQUEST_ANIM, 0);
            player->pos = top;
            player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        }
        if (!is_spring_b) {
            player_bounce(slot, up.x, up.y, up.z);
        } else {
            if (STICK_LOCK(work) <= 0.0F) {
                STICK_LOCK(work) = 0.0F;
            }
            player_launch(slot, ADD_SPEED(work) + LAUNCH_SPEED, &work->ang,
                          STICK_LOCK(work));
        }
        work->mode = MODE_WAIT;
    }
    STRETCH(task) = LAUNCH_STRETCH;
    vibration_start(slot, VIBRATION_POWER, VIBRATION_FREQ, 0);
    messenger_set_event(slot, MESSENGER_EVENT_SPRING);
}

static void spring_a_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

static void spring_a_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, where each node
     * named in place would do */
    NJS_CNK_OBJECT *object;
    /* @hack spelling: model, a local for the base's model, where the model
     * passed in place would do */
    NJS_CNK_MODEL *model;
    Float scale;

    njSetTexture(&spring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->timer) {
        scale = 1.0F / work->timer;
        njScale(NULL, scale, scale, scale);
    }
    model = &spring_a_base_model;
    njCnkEasyDrawModel(model);
    object = &spring_a_coil_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    /* @bug logic: the top squashes with the coil. the top draws in the
     * coil's scaled matrix, so its rise along y scales too. spring b's draws
     * push a matrix to keep their top out of the scale (the JSR to njScale
     * at 0x672 of the object's code, before the top's njTranslateEx at
     * 0x67e) */
    njScale(NULL, COIL_WIDTH, 1.0F + STRETCH(task), COIL_WIDTH);
    njCnkEasyDrawModel(object->model);
    object = &spring_a_top_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

/* spring_a_draw, with the nodes drawn from their builds */
static void spring_a_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, where each node
     * named in place would do */
    NJS_CNK_OBJECT *object;
    Float scale;

    njSetTexture(&spring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->timer) {
        scale = 1.0F / work->timer;
        njScale(NULL, scale, scale, scale);
    }
    njDirectDrawObject(spring_builds[A_BASE].built);
    object = &spring_a_coil_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    /* @bug logic: the top squashes with the coil, as in spring_a_draw (the
     * JSR to njScale at 0x7a2 of the object's code, before the top's
     * njTranslateEx at 0x7b0) */
    njScale(NULL, COIL_WIDTH, 1.0F + STRETCH(task), COIL_WIDTH);
    njDirectDrawObject(spring_builds[A_COIL].built);
    object = &spring_a_top_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    njDirectDrawObject(spring_builds[A_TOP].built);
    njPopMatrixEx();
}

void spring_b_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    /* @hack spelling: player_task, a local for player_tasks[player], where
     * the element read in place would do */
    struct task *player_task;
    /* @hack type: player a Uint8, where an int would do */
    Uint8 player;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = spring_b_destroy;
        if (spring_built) {
            task->draw = spring_b_draw_built;
        } else {
            task->draw = spring_b_draw;
        }
        collision_init(task, spring_b_shapes, SPRING_SHAPES, COLLISION_OBJECT);
        work->collision->flags |= COLLISION_TARGET;
        PHASE_B(task) = PHASE_REST;
        PHASE_SPEED(work) = 0.0F;
        BOB(task) = BOB_START;
        work->tick = 0;
        work->mode = MODE_WAIT;
        break;
    case MODE_WAIT:
        if (work->timer) {
            work->timer--;
        }
        STRETCH(task) =
            -njAbs(BOB(task) * njSin(PHASE_B(task) * ANGLE_PER_DEGREE));
        PHASE_B(task) += PHASE_SPEED(work);
        if (PHASE_B(task) >= SLOW_PHASE &&
            PHASE_B(task) < SLOW_PHASE + PHASE_SPEED(work)) {
            PHASE_SPEED(work) = B_PHASE_SPEED;
        }
        BOB(task) = njCos((PHASE_B(task) - PHASE_REST) *
                          (ANGLE_PER_DEGREE / BOB_SLOWDOWN)) *
                    BOB_SIZE;
        if (BOB(task) < 0.0F) {
            PHASE_B(task) = PHASE_REST;
            PHASE_SPEED(work) = 0.0F;
        }
        work->tick++;
        /* @hack spelling: collision_next_player called in each arm, where one
         * call before the test would do */
        if (work->tick > TOUCH_WAIT) {
            hit = collision_next_player(task);
            if (hit != NULL) {
                for (player = 0; player < SPRING_PLAYERS; player++) {
                    player_task = player_tasks[player];
                    if (hit == player_task) {
                        STRETCH(task) = LAUNCH_STRETCH;
                        LAUNCH_SLOT(work) = player;
                        PHASE_SPEED(work) = TOUCH_PHASE_SPEED;
                        PHASE_B(task) = TOUCH_PHASE;
                        work->mode = MODE_LAUNCH;
                        work->tick = 0;
                        break;
                    }
                }
            }
        } else {
            hit = collision_next_player(task);
            if (hit != NULL) {
                work->tick = 0;
            }
        }
        if (work->mode != MODE_LAUNCH) {
            collision_register(task);
        }
        if (work->tick > TICK_LIMIT) {
            work->tick = TICK_LIMIT;
        }
        break;
    case MODE_LAUNCH:
        work->tick = 0;
        spring_launch(task, TRUE);
        sound_play(SPRING_SOUND, NULL, 0, 0);
        break;
    case MODE_KILL:
        placement_kill(task);
        break;
    }
}

static void spring_b_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, set to the coil
     * twice, where each node named in place would do */
    NJS_CNK_OBJECT *object;
    Float scale;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    njSetTexture(&spring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->timer) {
        scale = 1.0F / work->timer;
        njScale(NULL, scale, scale, scale);
    }
    njCnkEasyDrawModel(&spring_b_base_model);
    njPushMatrixEx();
    object = &spring_b_coil_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njPushMatrixEx();
    object = &spring_b_top_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    object = &spring_b_coil_object;
    njScale(NULL, COIL_WIDTH, 1.0F + STRETCH(task), COIL_WIDTH);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(2);
}

/* spring_b_draw, with the nodes drawn from their builds */
static void spring_b_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, where each node
     * named in place would do */
    NJS_CNK_OBJECT *object;
    Float scale;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    njSetTexture(&spring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->timer) {
        scale = 1.0F / work->timer;
        njScale(NULL, scale, scale, scale);
    }
    njDirectDrawObject(spring_builds[B_BASE].built);
    njPushMatrixEx();
    object = &spring_b_coil_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njPushMatrixEx();
    object = &spring_b_top_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    njDirectDrawObject(spring_builds[B_TOP].built);
    njPopMatrixEx();
    njScale(NULL, COIL_WIDTH, 1.0F + STRETCH(task), COIL_WIDTH);
    njDirectDrawObject(spring_builds[B_COIL].built);
    njPopMatrix(2);
}

static void spring_b_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}
