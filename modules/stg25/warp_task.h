/* The two Death Chamber warps share a task, its settings and its callbacks. */
#ifndef STG25_WARP_TASK_H
#define STG25_WARP_TASK_H
/* IWYU pragma: begin_exports */
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
#include "stg25/wall_warp.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
/* IWYU pragma: end_exports */

#define WARP_SIZES 3
#define WARP_SHAPES 1

/* the work's tick holds the warp's size, which the init picks by ang.x. the
 * warp scales its model in x and y, and its shape, by SIZE / SIZE_UNIT */
#define SIZE_UNIT 60.0F
#define SIZE(work) ((work)->tick)
/* the work's timer holds the angle about y that player 0 comes out at. the init
 * moves it there from ang.z */
#define OUT_ANGLE(work) ((work)->timer)

/* task's unk_3c counts a dig's steps, from 1 on the step the dig starts. it
 * holds 0 while no dig runs. the screen fades out through step WARP_MOVE, when
 * player 0 moves to scl, and fades in after it. the dig ends at step WARP_END.
 * bursts of particles mark steps BURST_1 and BURST_2 at scl. a third burst, at
 * player 0, marks the move */
#define STEPS(task) ((task)->unk_3c.l)
#define WARP_MOVE 75
#define WARP_END 116
#define BURST_1 59
#define BURST_2 67
/* a burst throws its particles BURST_TURN apart about y, from a random angle
 * under BURST_TURN */
#define BURST_TURN 0x800
#define BURST_START_SCALE (BURST_TURN * RAND_SCALE)
#define PARTICLE_SCALE 1.0F

/* the switches stg25/ground_warp.c sets. DIGGING(mode) holds for the modes
 * of a player who digs into the warp */
#ifndef DIGGING
#define DIGGING(mode)                                                          \
    ((mode) == PLAYER_MODE_HUNTER_WALL_DIG ||                                  \
     (mode) == PLAYER_MODE_HUNTER_WALL_DIG_OUT ||                              \
     (mode) == PLAYER_MODE_UNK_43)
#endif
/* a dig counts from this frame of the digging player's animation on */
#ifndef DIG_FRAME
#define DIG_FRAME 10.0F
#endif
/* the lights the move gives the players */
#ifndef MOVE_LIGHTS
#define MOVE_LIGHTS                                                            \
    light_select(light_camera, 0);                                             \
    light_select(light_camera, 1)
#endif
/* the draw's turn of the model, after ang.y's */
#ifndef DRAW_TURN
#define DRAW_TURN (void)0
#endif
/* the function a block of the step calls, though no path reaches the block */
#ifndef DEAD_CALL
#define DEAD_CALL njSin
#endif

static void chamber_warp_destroy(struct task *task);
static void chamber_warp_step(struct task *task);
static void chamber_warp_draw(struct task *task);

#endif
