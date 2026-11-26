/* the wall warp, WARP and WARP_N in Wild Canyon's object list: a painted wall
 * of two kinds. a Knuckles or a Rouge who digs into it comes out somewhere
 * else */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/wall_warp.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum wall_warp_kind { KIND_BRIGHT, KIND_DARK, KINDS };
#define KIND_BITS 0xf0
#define KIND_STEP 0x10
#define KIND(work) ((((work)->ang.x & KIND_BITS) / KIND_STEP) % KINDS)
#define SIZE_BITS 0xf
#define WALL_WARP_SIZES 3
/* the bits of ang.x the init keeps: the size's, and the kind's lowest, the
 * only one KIND reads */
#define ANG_X_KEEP (SIZE_BITS | KIND_STEP)
/* the work's tick holds the wall's size, which the init picks by ang.x. the
 * wall scales its model in x and y, and its shape, by SIZE / SIZE_UNIT */
#define SIZE_UNIT 60.0F
#define SIZE(work) ((work)->tick)
/* the work's timer holds the angle about y that the digger comes out at. the
 * init moves it there from ang.z */
#define OUT_ANGLE(work) ((work)->timer)
/* the work's submode holds the digger's slot */
#define DIGGER(work) ((work)->submode)

/* a dig counts from this frame of the digging player's animation on */
#define DIG_FRAME 10.0F
/* task's unk_3c counts a dig's steps, from 1 on the step the dig starts. it
 * holds 0 while no dig runs. the screen fades out through step WARP_MOVE,
 * when the digger moves to scl, and fades in after it. the dig ends at step
 * WARP_END. bursts of dirt mark steps BURST_1 and BURST_2 at scl. a third
 * burst, at the digger, marks the move */
#define STEPS(task) ((task)->unk_3c.l)
#define WARP_MOVE 75
#define WARP_END 116
#define BURST_1 59
#define BURST_2 67
/* a burst throws a particle of dirt each BURST_TURN about y, from a random
 * angle under BURST_TURN */
#define BURST_TURN 0x800
#define BURST_START_SCALE (BURST_TURN * RAND_SCALE)
#define PARTICLE_SCALE 1.0F
/* the players who can dig: player 0, and in a two-player game player 1 */
#define PLAYERS 2

#define WALL_WARP_SHAPES 1

#include "shared/wall_warp/text.inc"

/* the placement editor's labels, which no code reads. each size rounds its
 * label up to whole words */
static const char wall_warp_edit_size[24] = "<- (      ) SIZE( m)";
static const char wall_warp_edit_yaw[20] = "<- ANGLE Y (   )";
static const char wall_warp_edit_exit_yaw[24] = "<- WARP OUT ANG Y(   )";
static const char wall_warp_edit_exit_x[20] = "<- WARP OUT POS X";
static const char wall_warp_edit_exit_y[20] = "<- WARP OUT POS Y";
static const char wall_warp_edit_exit_z[20] = "<- WARP OUT POS Z";

static NJS_TEXNAME wall_warp_bright_texlist_names[1] = {
#include "shared/wall_warp/wall_warp_bright_texlist_names.inc"
};

static NJS_TEXLIST wall_warp_bright_texlist = {
#include "shared/wall_warp/wall_warp_bright_texlist.inc"
};

#include "shared/canyon_art/geometry/wall_warp_bright.h"

static NJS_CNK_MODEL wall_warp_bright_model = {
#include "shared/wall_warp/wall_warp_bright_model.inc"
};

static NJS_CNK_OBJECT wall_warp_bright_object = {
#include "shared/wall_warp/wall_warp_bright_object.inc"
};

static NJS_TEXNAME wall_warp_dark_texlist_names[1] = {
#include "shared/wall_warp/wall_warp_dark_texlist_names.inc"
};

static NJS_TEXLIST wall_warp_dark_texlist = {
#include "shared/wall_warp/wall_warp_dark_texlist.inc"
};

#include "shared/canyon_art/geometry/wall_warp_dark.h"

static NJS_CNK_MODEL wall_warp_dark_model = {
#include "shared/wall_warp/wall_warp_dark_model.inc"
};

static NJS_CNK_OBJECT wall_warp_dark_object = {
#include "shared/wall_warp/wall_warp_dark_object.inc"
};

static void wall_warp_destroy(struct task *task);
static void wall_warp_step(struct task *task);
static void wall_warp_draw(struct task *task);

KEPT static NJS_CNK_MODEL *wall_warp_models[KINDS] = {
#include "shared/wall_warp/wall_warp_models.inc"
};
NJS_TEXLIST *wall_warp_texlists[KINDS + 1] = {
#include "shared/wall_warp/wall_warp_texlists.inc"
};
/* the box at a size of SIZE_UNIT */
/* clang-format off */
static struct collision_shape wall_warp_shapes[WALL_WARP_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, -10.0F, 0.0F}, 10.0F, 10.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float wall_warp_sizes[WALL_WARP_SIZES] = {60.0F, 50.0F, 40.0F};
static char *wall_warp_kind_names[KINDS] = {
#include "shared/wall_warp/wall_warp_kind_names.inc"
};

void wall_warp_init(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_shape shapes[WALL_WARP_SHAPES];
    Float scale;

    if (half_try_leave(task) || placement_try_leave(task)) {
        return;
    }
    task->draw = wall_warp_draw;
    task->step = wall_warp_step;
    task->destructor = wall_warp_destroy;
    OUT_ANGLE(task->work) = task->work->ang.z;
    task->work->ang.z = 0;
    /* @hack type: the size's bits taken as a Float through njAbs, where the
     * bits alone would do */
    SIZE(task->work) =
        wall_warp_sizes[(Sint32)njAbs((Float)(task->work->ang.x & SIZE_BITS)) %
                        WALL_WARP_SIZES];
    work->ang.x &= ANG_X_KEEP;
    scale = SIZE(task->work) * (1.0F / SIZE_UNIT);
    shapes[0] = wall_warp_shapes[0];
    shapes[0].radius *= scale;
    shapes[0].size_y *= scale;
    shapes[0].size_z *= scale;
    shapes[0].center.y *= scale;
    collision_init(task, shapes, WALL_WARP_SHAPES, COLLISION_OBJECT);
    STEPS(task) = 0;
}

static void wall_warp_destroy(struct task *task)
{
}

static void wall_warp_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct task *hit;
    struct task_work *work = self->work;
    int angle;
    int player;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    if (STEPS(self) == 0 && half_try_leave(self)) {
        return;
    }
    /* @hack spelling: the rest of the step in an else after the return,
     * where no else would do */
    if (STEPS(self) == 0 && placement_try_leave(self)) {
        return;
    } else {
        if (STEPS(self) >= 1) {
            ++STEPS(self);
            if (STEPS(self) <= WARP_MOVE) {
                fade_out_step();
                if (player_hooks.unk_04 &&
                    (STEPS(self) == BURST_2 || STEPS(self) == BURST_1)) {
                    pos.x = self->work->scl.x;
                    pos.y = self->work->scl.y;
                    pos.z = self->work->scl.z;
                    for (angle = (Float)rand() * BURST_START_SCALE;
                         angle < FULL_TURN; angle += BURST_TURN) {
                        spd.x = (Float)rand() * njSin(angle) * RAND_SCALE;
                        spd.y = (Float)rand() * RAND_SCALE;
                        spd.z = (Float)rand() * njCos(angle) * RAND_SCALE;
                        player_hooks.unk_04(&pos, &spd, PARTICLE_SCALE);
                    }
                }
            } else {
                fade_in_step();
            }
            if (STEPS(self) == WARP_MOVE) {
                player_set_pos(DIGGER(self->work), self->work->scl.x,
                               self->work->scl.y, self->work->scl.z);
                spd.x = self->work->scl.x;
                spd.y = self->work->scl.y;
                spd.z = self->work->scl.z;
                camera_put_at(DIGGER(self->work), &spd);
                player_request(DIGGER(self->work), PLAYER_REQUEST_RELEASE, 0);
                player_set_angles(DIGGER(self->work), 0, OUT_ANGLE(self->work),
                                  0);
                light_select(light_camera, 0);
                light_select(light_camera, 1);
                fade_set_black();
                if (player_hooks.unk_04) {
                    for (angle = (Float)rand() * BURST_START_SCALE;
                         angle < FULL_TURN; angle += BURST_TURN) {
                        pos.x = (Float)rand() * njSin(angle) * RAND_SCALE;
                        pos.y = (Float)rand() * RAND_SCALE;
                        pos.z = (Float)rand() * njCos(angle) * RAND_SCALE;
                        player_hooks.unk_04(
                            &player_work[DIGGER(self->work)]->pos, &pos,
                            PARTICLE_SCALE);
                    }
                    pos.x = pos.z = 0.0F;
                    pos.y = 1.0F;
                    turn_to_world(player_work[DIGGER(self->work)], &pos);
                    player_hooks.unk_04(&player_work[DIGGER(self->work)]->pos,
                                        &pos, PARTICLE_SCALE);
                }
            }
            /* @hack redundant: a block no path reaches, which reads the work,
             * kept for the frame it gives the step */
            if (0) {
                if (work && work->ang.x) {
                }
            }
            if (STEPS(self) == WARP_END) {
                game_state.idle_dim = 1;
                game_state.pause_disabled = 0;
                STEPS(self) = 0;
            }
        }
        /* @bug logic: warps a digging player who never touched the wall. the
         * step stores the touching player at 0x3ee of the object's code, and
         * never reads it. the loop from 0x3fc tests each slot's dig alone, so
         * in a two-player game player 0's touch warps player 1 wherever player
         * 1 digs */
        if (STEPS(self) == 0 && (work->collision->flags & COLLISION_TOUCHED) &&
            (hit = collision_next_player(self)) != NULL) {
            for (player = 0; player < PLAYERS; ++player) {
                int mode;

                if (player == 1 && !game_state.two_players) {
                    continue;
                }
                mode = player_work[player]->mode;
                if (mode == PLAYER_MODE_HUNTER_WALL_DIG ||
                    mode == PLAYER_MODE_HUNTER_WALL_DIG_OUT ||
                    mode == PLAYER_MODE_UNK_43) {
                    if (player_records[player]->anim.frame >= DIG_FRAME) {
                        game_state.idle_dim = 0;
                        game_state.pause_disabled = 1;
                        fade_reset_idle();
                        STEPS(self) = 1;
                        DIGGER(self->work) = player;
                    }
                }
            }
        }
        collision_register(self);
    }
}

static void wall_warp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.y = SIZE(task->work) * (1.0F / SIZE_UNIT);
    scale.z = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScaleEx(&scale);
    njSetTexture(wall_warp_texlists[KIND(task->work)]);
    njCnkEasyDrawModel(wall_warp_models[KIND(task->work)]);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
