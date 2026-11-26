/* the init and callbacks of Death Chamber's warps, whose settings
 * stg25/warp_task.h gives. stg25/wall_warp.c compiles them for the wall warp,
 * WARP. stg25/ground_warp.c compiles them for the ground warp, WARP2, with the
 * switches warp_task.h names. it renames chamber_wall_warp_init to
 * chamber_ground_warp_init. the includer defines the warp's art, shapes and
 * sizes first */
#ifndef STG25_WARP_TASK_IMPL_H
#define STG25_WARP_TASK_IMPL_H
#include "stg25/warp_task.h"

void chamber_wall_warp_init(struct task *task)
{
    struct collision_shape shapes[WARP_SHAPES];
    Float scale;

    /* @hack redundant: SHC retains the six SET editor captions. */
    if (0) {
        (void)"<- SIZE ( m)";
        (void)"<- ANGLE Y (   )";
        (void)"<- WARP OUT ANG Y(   )";
        (void)"<- WARP OUT POS X";
        (void)"<- WARP OUT POS Y";
        (void)"<- WARP OUT POS Z";
    }
    if (!placement_try_leave(task)) {
        task->draw = chamber_warp_draw;
        task->step = chamber_warp_step;
        task->destructor = chamber_warp_destroy;
        OUT_ANGLE(task->work) = task->work->ang.z;
        task->work->ang.z = 0;
        SIZE(task->work) =
            chamber_warp_sizes[(Sint32)njAbs((Float)task->work->ang.x) %
                               WARP_SIZES];
        task->work->ang.x = 0;
        scale = SIZE(task->work) * (1.0F / SIZE_UNIT);
        shapes[0] = chamber_warp_shapes[0];
        shapes[0].radius *= scale;
        shapes[0].size_y *= scale;
        shapes[0].size_z *= scale;
        shapes[0].center.y *= scale;
        collision_init(task, shapes, WARP_SHAPES, COLLISION_OBJECT);
        STEPS(task) = 0;
    }
}

static void chamber_warp_destroy(struct task *task)
{
}

static void chamber_warp_step(struct task *task)
{
    /* @hack redundant: hit, which keeps the touching player's task, though
     * nothing reads it */
    struct task *hit;
    struct task_work *work = task->work;
    Angle angle;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    /* @hack spelling: the rest of the step in an else after the return,
     * where no else would do */
    if (STEPS(task) == 0 && placement_try_leave(task)) {
        return;
    } else {
        if (STEPS(task) >= 1) {
            ++STEPS(task);
            if (STEPS(task) <= WARP_MOVE) {
                fade_out_step();
                if (player_hooks.unk_04 &&
                    (STEPS(task) == BURST_2 || STEPS(task) == BURST_1)) {
                    pos.x = task->work->scl.x;
                    pos.y = task->work->scl.y;
                    pos.z = task->work->scl.z;
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
            if (STEPS(task) == WARP_MOVE) {
                /* @hack spelling: out, a copy of task->work, where
                 * task->work would do */
                struct task_work *out = task->work;

                player_set_pos(0, out->scl.x, out->scl.y, out->scl.z);
                /* @hack spelling: the point camera 0 moves to in spd, and the
                 * burst's speeds in pos, where each would keep its role from
                 * the burst above */
                spd.x = task->work->scl.x;
                spd.y = task->work->scl.y;
                spd.z = task->work->scl.z;
                camera_put_at(0, &spd);
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                player_set_angles(0, 0, OUT_ANGLE(task->work), 0);
                MOVE_LIGHTS;
                fade_set_black();
                if (player_hooks.unk_04) {
                    for (angle = (Float)rand() * BURST_START_SCALE;
                         angle < FULL_TURN; angle += BURST_TURN) {
                        pos.x = (Float)rand() * njSin(angle) * RAND_SCALE;
                        pos.y = (Float)rand() * RAND_SCALE;
                        pos.z = (Float)rand() * njCos(angle) * RAND_SCALE;
                        player_hooks.unk_04(&player_work[0]->pos, &pos,
                                            PARTICLE_SCALE);
                    }
                    pos.x = pos.z = 0.0F;
                    pos.y = 1.0F;
                    turn_to_world(player_work[0], &pos);
                    player_hooks.unk_04(&player_work[0]->pos, &pos,
                                        PARTICLE_SCALE);
                }
            }
            /* @hack redundant: a block no path reaches, which calls
             * DEAD_CALL */
            if (0) {
                if (work) {
                    DEAD_CALL(work->ang.x);
                }
            }
            if (STEPS(task) == WARP_END) {
                game_state.idle_dim = 1;
                game_state.pause_disabled = 0;
                STEPS(task) = 0;
            }
        }
        /* @hack spelling: the dig's test in an if of its own, and its frame's
         * in another, where operands of the && would do */
        if (STEPS(task) == 0 && (work->collision->flags & COLLISION_TOUCHED) &&
            (hit = collision_next_player(task)) != NULL) {
            if (DIGGING(player_work[0]->mode)) {
                if (player_records[0]->anim.frame >= DIG_FRAME) {
                    game_state.idle_dim = 0;
                    game_state.pause_disabled = 1;
                    fade_reset_idle();
                    STEPS(task) = 1;
                }
            }
        }
        collision_register(task);
    }
}

static void chamber_warp_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.y = SIZE(work) * (1.0F / SIZE_UNIT);
    scale.z = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    DRAW_TURN;
    njScaleEx(&scale);
    njSetTexture(&chamber_warp_texlist);
    njCnkEasyDrawModel(&chamber_warp_model);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#endif
