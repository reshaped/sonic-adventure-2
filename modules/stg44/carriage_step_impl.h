/* the step of Mad Space's carriages, which stg44/carriage_path_impl.h
 * describes */
#ifndef STG44_CARRIAGE_STEP_IMPL_H
#define STG44_CARRIAGE_STEP_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include "player/player.h"
#include "shared/emerald.h"
#include "sound/sound.h"
#include "system/math.h"
#include "system/task.h"

/* a moving car plays RUN_SOUND while a player lies within SOUND_RANGE. the
 * train's cars also play BEAT_SOUND once each BEAT_PERIOD steps of the work's
 * timer */
#define RUN_SOUND 0
#define RUN_VOLUME 80
#define RUN_STEPS 30
#define BEAT_SOUND 1
#define BEAT_VOLUME 70
#define BEAT_STEPS 60
#define BEAT_PERIOD 60
#define SOUND_PRIORITY 1
/* each step a flat car's speed gains acceleration, and is then multiplied by
 * damping. it stays from LEAST_SPEED to MOST_SPEED */
#define LEAST_SPEED 1.0F
#define MOST_SPEED 13.0F

static void carriage_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int result;
    Angle x;
    Angle y;
    Angle z;
    /* where the car's move carries each player */
    /* @hack redundant: a third point, which only sizes the frame */
    NJS_POINT3 carried[3];
    NJS_POINT3 piece_pos;

    switch (work->mode) {
    case START_FORWARD:
        work->mode = MOVE_FORWARD;
#ifndef FLAT_CARRIAGE
        work->timer = 0;
#endif
        break;
    case START_BACKWARD:
        work->mode = MOVE_BACKWARD;
#ifndef FLAT_CARRIAGE
        work->timer = 0;
#endif
        break;
    case MOVE_FORWARD:
    case MOVE_BACKWARD:
#ifndef FLAT_CARRIAGE
        work->timer++;
#endif
        if (player_near(&work->pos, SOUND_RANGE)) {
            sound_play_at_for(RUN_SOUND, work, SOUND_PRIORITY, RUN_VOLUME,
                              RUN_STEPS, &work->pos);
#ifndef FLAT_CARRIAGE
            if (work->timer % BEAT_PERIOD == 0) {
                sound_play_at_for(BEAT_SOUND, task, SOUND_PRIORITY, BEAT_VOLUME,
                                  BEAT_STEPS, &work->pos);
            }
#endif
            if (task->child &&
                task->child->work->tick == EMERALD_TICK_PARENT_ANGLES) {
                emerald_set_shown(task->child, TRUE);
            }
        }
        /* @hack redundant: carried, passed for a direction that no code reads,
         * where null would do */
        result = carriage_advance(task,
                                  work->mode == MOVE_FORWARD
                                      ? CARRIAGE(task)->speed
                                      : -CARRIAGE(task)->speed,
                                  carried);
#ifdef FLAT_CARRIAGE
        CARRIAGE(task)->speed += acceleration;
        CARRIAGE(task)->speed *= damping;
        if (CARRIAGE(task)->speed < LEAST_SPEED) {
            CARRIAGE(task)->speed = LEAST_SPEED;
        }
        if (CARRIAGE(task)->speed > MOST_SPEED) {
            CARRIAGE(task)->speed = MOST_SPEED;
        }
#else
        CARRIAGE(task)->speed = travel_speed;
#endif
        /* carriage_advance returns 0, so no car turns back */
        if ((work->mode == MOVE_FORWARD &&
             CARRIAGE(task)->cursor.segment > 1) ||
            (work->mode == MOVE_BACKWARD &&
             CARRIAGE(task)->cursor.segment <= 0)) {
            /* @hack redundant: a block no path reaches, which tests result
             * and stores it in the work */
            if (0) {
                if (result && task->parent) {
                    task->work->mode = result;
                }
                if (result && task->child) {
#ifdef FLAT_CARRIAGE
                    task->work->submode = result;
#else
                    work->submode = result;
#endif
                }
                if (result) {
#ifndef FLAT_CARRIAGE
                    work->tick = result;
#endif
                }
            }
            if (result) {
                if (work->mode == MOVE_FORWARD) {
                    work->mode = START_BACKWARD;
                } else {
                    work->mode = START_FORWARD;
                }
            }
        }
    }
    if (!CARRIAGE(task)->object.model) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->submode == TRAILER && task->child) {
        piece_pos.x = 0.0F;
        piece_pos.y = PIECE_HEIGHT;
        piece_pos.z = PIECE_OFFSET;
        njCalcPoint(NULL, &piece_pos, &task->child->work->pos);
    }
    CARRIAGE_ANGLES(&x, &y, &z);
    /* these turns and the move undo the node's last place. the matrix then
     * carries a point from there to the car's new place. each player's force
     * moves the player as far as the matrix moves the player's pos */
    njRotateX(NULL, -CARRIAGE(task)->object.ang[0]);
    njRotateY(NULL, -CARRIAGE(task)->object.ang[1]);
    njRotateZ(NULL, -CARRIAGE(task)->object.ang[2]);
    njTranslate(NULL, -CARRIAGE(task)->object.pos[0],
                -CARRIAGE(task)->object.pos[1], -CARRIAGE(task)->object.pos[2]);
    for (i = 0; i < TASK_FORCES; i++) {
        if (player_work[i]) {
            njCalcPoint(NULL, &player_work[i]->pos, &carried[i]);
        }
    }
    njPopMatrixEx();
    /* @hack spelling: each point read through the address of the whole
     * array, where carried[i] would do */
    for (i = 0; i < TASK_FORCES; i++) {
        if (player_work[i]) {
            TASK_FORCES_OF(task)
            [i].speed.x = (*(&carried))[i].x - player_work[i]->pos.x;
            TASK_FORCES_OF(task)
            [i].speed.y = (*(&carried))[i].y - player_work[i]->pos.y;
            TASK_FORCES_OF(task)
            [i].speed.z = (*(&carried))[i].z - player_work[i]->pos.z;
            TASK_FORCES_OF(task)[i].turn.x = x - CARRIAGE(task)->object.ang[0];
            TASK_FORCES_OF(task)[i].turn.y = -y + CARRIAGE(task)->object.ang[1];
            TASK_FORCES_OF(task)[i].turn.z = z - CARRIAGE(task)->object.ang[2];
        }
    }
    CARRIAGE(task)->object.pos[0] = work->pos.x;
    CARRIAGE(task)->object.pos[1] = work->pos.y;
    CARRIAGE(task)->object.pos[2] = work->pos.z;
    CARRIAGE(task)->object.ang[0] = x;
    CARRIAGE(task)->object.ang[1] = y;
    CARRIAGE(task)->object.ang[2] = z;
    if (player_near((NJS_POINT3 *)CARRIAGE(task)->object.pos, NEAR_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
#endif
