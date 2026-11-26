/* Final Rush's STROCKET rises, aims and releases its rider more quickly. */
#define FINAL_RUSH_ROCKET
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "effect/burst.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/rocket.h"
#include "shared/rocket_thrust.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "shared/rocket_work.h"
#include "stg30/stage_rocket.h"

/* how far the rocket rises above the launcher's pos */
static Float body_rise = 8.0F;
/* the point of the rocket's model that lies at the rocket's pos */
static NJS_POINT3 body_model_offset = {8.1F, -5.0F, 0.0F};
/* where a held rider stands, from the rocket's pos turned by its ang.y
 * alone */
static NJS_POINT3 rider_hold_offset = {-3.0F, -6.0F, 0.0F};
static NJS_POINT3 rider_flight_offset = {-4.0F, -6.0F, 0.0F};
/* an aiming rocket's puffs of thrust. thrust_spd is a puff's speed in the
 * frame of the rocket's model. thrust_jitter is the span of a random change
 * to each part of that speed. a puff's size runs from thrust_size_min up to
 * thrust_size_min + thrust_size_range. its floor lies thrust_rise plus its
 * size above the launcher's pos */
static NJS_VECTOR thrust_spd = {0.0F, -2.7F, 0.0F};
static Float thrust_jitter = 0.8F;
static Float thrust_rise = 1.0F;
static Float thrust_size_min = 2.0F;
static Float thrust_size_range = 1.0F;
static Float dat_rocket_1 = 20.0F;
static Float dat_rocket_2 = 20.0F;

NJS_TEXLIST *stage_rocket_texlists[5] = {
#include "stg30/stage_rocket/stage_rocket_texlists.inc"
};

/* an upright rocket's record negates the hull's center.y */
/* clang-format off */
static struct collision_shape body_shapes[BODY_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL | SHAPE_GUARD_LEVEL(1) | HULL_ATTACK,
     SHAPE_UNK_800000 | SHAPE_HURTABLE | SHAPE_TURN_YZX, {4.5F, 9.4F, 0.0F},
     2.4F, 6.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_TURN_YZX, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F,
     0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */

#include "shared/rocket_launcher.h"

static void rocket_step(struct task *task)
{
    struct task_work *work = task->work;
    struct launcher *launcher;
    int nearby_player;
    int slot;
    /* @hack spelling: vec reused for the upright rocket's shift, the way to
     * the place and the thrust's speed, where a local for each would do */
    NJS_VECTOR vec;
    struct angles aim;
    NJS_POINT3 tail;
    Sint16 turn_y;
    Sint16 turn_z;
    Float size;
    Float angle_units;
    Float toward_z;

    if (LAUNCHER(task)->riders == 0 && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, for its strings in the
     * object's constants */
    if (0) {
        /* @hack redundant: the first two strings picked by ang.x, where each
         * on its own would do */
        (void)((work->ang.x & ROCKET_ON_SWITCH) ? "<- TYPE: APPEAR ON SWITCHON"
                                                : "<- TYPE: APPEAR ALWAYS");
        (void)"<- ANGLE Y";
        (void)"<- ID";
        (void)"<- AIMPOS X";
        (void)"<- AIMPOS Y";
        (void)"<- AIMPOS Z";
    }
    nearby_player = player_near(&work->pos, 100.0F);
    /* @hack spelling: the first time-stop test addresses the byte directly. */
    if (work->mode == ROCKET_RISE || CAUGHT_IN_STOP(work) ||
        !game_state_time_stopped) {
        switch (work->mode) {
        case ROCKET_WAIT:
            if (LAUNCHER(task)->body == NULL) {
                LAUNCHER(task)->body =
                    task_create_child(TASK_WORK, rocket_body_init, task);
            }
            if (LAUNCHER(task)->body != NULL) {
                BODY_WORK(task)->submode = NO_RIDER;
                BODY_WORK(task)->pos = work->pos;
                if (work->submode == ROCKET_UPRIGHT) {
                    vec.x = WAIT_UPRIGHT_X;
                    vec.y = 0.0F;
                    vec.z = 0.0F;
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y);
                    njCalcVector(NULL, &vec, &vec);
                    ROCKET_POP();
                    BODY_WORK(task)->pos.x += vec.x;
                    BODY_WORK(task)->pos.z += vec.z;
                }
                BODY_WORK(task)->pos.y -= WAIT_DROP;
                BODY_WORK(task)->ang.z =
                    (work->submode == ROCKET_UPRIGHT) ? QUARTER_TURN : 0;
                BODY_WORK(task)->ang.y = work->ang.y;
                BODY_WORK(task)->ang.x = 0;
                LAUNCHER(task)->base_frame = 0.0F;
                LAUNCHER(task)->body_frame = 0.0F;
                if (!(work->ang.x & ROCKET_ON_SWITCH) ||
                    switch_is_on(work->ang.z & ROCKET_SWITCH_ID)) {
                    work->mode = ROCKET_RISE;
                    sound_play(SOUND_RISE, NULL, 0, 0);
                }
            }
            break;
        case ROCKET_RISE:
            if (!game_state.time_stopped) {
                LAUNCHER(task)->base_frame += BASE_FRAME_STEP;
                if (nearby_player) {
                    player_records[nearby_player - 1]->stick_lock = 5;
                }
            }
            /* @hack redundant: a discarded use of the proximity result keeps
             * the two turn limits in the registers used by this launcher. */
            if (0) {
                work->timer = nearby_player + nearby_player;
            }
            if (LAUNCHER(task)->base_frame >= BASE_FRAMES) {
                LAUNCHER(task)->base_frame = BASE_FRAMES;
                if (!game_state.time_stopped) {
                    BODY_WORK(task)->pos.y += RISE_STEP;
                }
                if (BODY_WORK(task)->pos.y >= work->pos.y + body_rise) {
                    BODY_WORK(task)->pos.y = work->pos.y + body_rise;
                    if (!game_state.time_stopped) {
                        LAUNCHER(task)->body_frame += BODY_FRAME_STEP;
                    }
                    if (LAUNCHER(task)->body_frame >= BODY_FRAMES) {
                        LAUNCHER(task)->body_frame = BODY_FRAMES;
                        if (task->child != NULL && nearby_player) {
                            slot = nearby_player - 1;
                            BODY_WORK(task)->scl = work->scl;
                            BODY_WORK(task)->submode = slot;
                            BODY_WORK(task)->mode = BODY_HELD;
                            player_request(slot, PLAYER_REQUEST_ANIM,
                                           RIDE_HOLD);
                            LAUNCHER(task)->riders++;
                            work->mode = ROCKET_AIM;
                            work->timer = 0;
                            player_work[slot]->ang.x = 0;
                            player_work[slot]->ang.y = BODY_WORK(task)->ang.y;
                            player_work[slot]->ang.z = 0;
                            CAUGHT_IN_STOP(work) = 1;
                        }
                    }
                }
            }
            break;
        case ROCKET_AIM:
#include "shared/rocket_aim.inc"
            work->timer++;
            if ((Uint16)BODY_WORK(task)->ang.y == (Uint16)aim.y &&
                (Uint16)BODY_WORK(task)->ang.z == (Uint16)aim.z &&
                work->timer > AIM_STEPS) {
                LAUNCHER(task)->base_frame -= BASE_FRAME_STEP;
                if (LAUNCHER(task)->base_frame <= 0.0F) {
                    LAUNCHER(task)->base_frame = 0.0F;
                    player_request(BODY_WORK(task)->submode,
                                   PLAYER_REQUEST_ANIM, RIDE_FLY);
                    work->timer = 0;
                    work->mode = ROCKET_WAIT;
                    BODY_WORK(task)->mode = BODY_FLYING;
                    BODY_WORK(task)->tick = 1;
                    LAUNCHER(task)->body = NULL;
                    CAUGHT_IN_STOP(work) = 0;
                }
            }
            break;
        }
    }
    /* @hack spelling: launcher, a local for the record, where LAUNCHER(task)
     * read in place would do */
    launcher = task->motion.p;
    if (player_near((NJS_POINT3 *)launcher->base.pos,
                    launcher->base.model->r + BASE_NEAR)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

#include "shared/rocket_body.h"
