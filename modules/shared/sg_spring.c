/* the SG springs, SG_SPRA and SG_SPRB in a stage's object list, or SGSPRING
 * and SGSPRINGB. they are the springs of shared/spring.h. only the players
 * whose bits are set in game_state.sg_players see them and touch them */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/sg_spring.h"
#include "shared/spring.h"
#include "shared/spring_work.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a spring takes a touch only once tick passes TOUCH_WAIT. a launch of spring
 * a sets tick to 0, and one of spring b to B_LAUNCH_TICK. spring b starts with
 * tick at TOUCH_WAIT, so its first waiting step can take a touch */
#define TOUCH_WAIT 10
#define B_LAUNCH_TICK 5
#define TOUCH_REACH 14.0F

static void sg_spring_launch(struct task *task, Uint8 is_spring_b);
static void sg_spring_a_destroy(struct task *task);
static void sg_spring_a_draw(struct task *task);
static void sg_spring_b_draw(struct task *task);
static void sg_spring_b_destroy(struct task *task);

void sg_spring_a_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = sg_spring_a_destroy;
        task->draw = sg_spring_a_draw;
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
        if (work->tick > TOUCH_WAIT) {
            for (player = 0;
                 player < GAME_PLAYERS && player_work[player] != NULL &&
                 player_work[player]->collision != NULL;
                 player++) {
                if ((game_state.sg_players & (1 << player)) &&
                    njDistanceP2P(
                        &player_work[player]->collision->shapes->center,
                        &work->pos) < TOUCH_REACH) {
                    STRETCH(task) = LAUNCH_STRETCH;
                    LAUNCH_SLOT(work) = player;
                    PHASE_SPEED(work) = TOUCH_PHASE_SPEED;
                    PHASE_A(work) = TOUCH_PHASE;
                    work->mode = MODE_LAUNCH;
                    break;
                }
            }
        }
        if (work->tick > TICK_LIMIT) {
            work->tick = TICK_LIMIT;
        }
        break;
    case MODE_LAUNCH:
        sound_play(SPRING_SOUND, NULL, 0, 0);
        work->tick = 0;
        sg_spring_launch(task, FALSE);
        break;
    case MODE_KILL:
        placement_kill(task);
        break;
    }
}

/* @hack type: is_spring_b a Uint8, where an int would do */
static void sg_spring_launch(struct task *task, Uint8 is_spring_b)
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
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &up);
    njPopMatrix(1);
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
    /* @bug uninitialized: reads speed unset where player_ground_speed
     * returns FALSE, as spring.c's spring_launch does (the call and the read,
     * at 0x34e and 0x352 of the object's code) */
#ifdef NONMATCHING
    /* this side keeps the image's launch from spring a and repairs it from
     * spring b. sg_spring_a_step holds PHASE_REST in FR15 as it calls
     * sound_play. where sound_may_play lets the sound play, slot_take saves
     * FR15 in the stack word that speed takes next. PHASE_REST is past
     * RUN_SPEED, so the image puts a player off the ground on spring a's top.
     * spring b's step writes nothing there before the read. this side starts
     * spring b's speed at 0, so the player launches as a slow one does */
    speed = is_spring_b ? 0.0F : RUN_SPEED;
#endif
    player_ground_speed(slot, &speed);
    /* the test's STRETCH != LAUNCH_STRETCH never holds. the touch set STRETCH
     * to LAUNCH_STRETCH, and nothing has changed it since */
    /* @hack spelling: RUN_SPEED > speed, where speed < RUN_SPEED would do */
    if (RUN_SPEED > speed || STRETCH(task) != LAUNCH_STRETCH) {
        /* @hack spelling: the launch written out under each arm of the
         * test, where one copy after it would do */
        if (distance < PULL_RANGE && distance >= PULL) {
            player_request(slot, PLAYER_REQUEST_ANIM, 0);
            player->pos.x += (dx / distance) * PULL;
            player->pos.z += (dz / distance) * PULL;
            player_request(slot, PLAYER_REQUEST_RELEASE, 0);
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
        player_request(slot, PLAYER_REQUEST_ANIM, 0);
        player->pos = top;
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
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
}

static void sg_spring_a_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

static void sg_spring_a_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, where each node
     * named in place would do */
    NJS_CNK_OBJECT *object;
    /* @hack spelling: model, a local for the base's model, where the model
     * passed in place would do */
    NJS_CNK_MODEL *model;
    Float scale;

    /* @hack redundant: the test's && 1 */
    if (!(game_state.sg_players & (1 << camera_current_index)) && 1) {
        return;
    }
    njSetTexture(&spring_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
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
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
    /* @bug logic: the top squashes with the coil, as in spring.c's
     * spring_a_draw (the JSR to njScale at 0x66a of the object's code, before
     * the top's njTranslate at 0x682) */
    njScale(NULL, 1.0F, 1.0F + STRETCH(task), 1.0F);
    njCnkEasyDrawModel(object->model);
    object = &spring_a_top_object;
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

void sg_spring_b_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = sg_spring_b_destroy;
        task->draw = sg_spring_b_draw;
        PHASE_B(task) = PHASE_REST;
        PHASE_SPEED(work) = 0.0F;
        BOB(task) = BOB_START;
        work->tick = TOUCH_WAIT;
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
        if (work->tick > TOUCH_WAIT) {
            for (player = 0;
                 player < GAME_PLAYERS && player_work[player] != NULL &&
                 player_work[player]->collision != NULL;
                 player++) {
                if ((game_state.sg_players & (1 << player)) &&
                    njDistanceP2P(
                        &player_work[player]->collision->shapes->center,
                        &work->pos) < TOUCH_REACH) {
                    STRETCH(task) = LAUNCH_STRETCH;
                    LAUNCH_SLOT(work) = player;
                    PHASE_SPEED(work) = TOUCH_PHASE_SPEED;
                    PHASE_B(task) = TOUCH_PHASE;
                    work->mode = MODE_LAUNCH;
                    break;
                }
            }
        }
        if (work->tick > TICK_LIMIT) {
            work->tick = TICK_LIMIT;
        }
        break;
    case MODE_LAUNCH:
        work->tick = B_LAUNCH_TICK;
        sg_spring_launch(task, TRUE);
        sound_play(SPRING_SOUND, NULL, 0, 0);
        break;
    case MODE_KILL:
        placement_kill(task);
        break;
    }
}

static void sg_spring_b_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for each node in turn, set to the coil
     * twice, where each node named in place would do */
    NJS_CNK_OBJECT *object;
    /* @hack spelling: model, a local for each node's model, where
     * object->model passed in place would do */
    NJS_CNK_MODEL *model;
    Float scale;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    /* @hack redundant: the test's && 1 */
    if (!(game_state.sg_players & (1 << camera_current_index)) && 1) {
        return;
    }
    njSetTexture(&spring_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->timer) {
        scale = 1.0F / work->timer;
        njScale(NULL, scale, scale, scale);
    }
    njCnkEasyDrawModel(&spring_b_base_model);
    njPushMatrix(NULL);
    object = &spring_b_coil_object;
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
    njPushMatrix(NULL);
    object = &spring_b_top_object;
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
    njTranslate(NULL, 0.0F, STRETCH(task) * TOP_TRAVEL, 0.0F);
    model = object->model;
    njCnkEasyDrawModel(model);
    njPopMatrix(1);
    object = &spring_b_coil_object;
    njScale(NULL, 1.0F, 1.0F + STRETCH(task), 1.0F);
    model = object->model;
    njCnkEasyDrawModel(model);
    njPopMatrix(1);
    njPopMatrix(1);
}

static void sg_spring_b_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}
