/* DODAI and DODAIGEN in the object list of the second fight of Sonic and
 * Shadow. they are the foothold and its generator. a foothold drops a while
 * after player 0 stands on it. the generator lays footholds along x ahead of
 * the players */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "stg42/foothold/text.inc"

#include "camera/stack.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg42/foothold.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg42/foothold_art.h"

NJS_TEXLIST *foothold_texture_group[2] = {
#include "stg42/foothold/foothold_texture_group.inc"
};

/* no code sets MODE_KILL */
enum foothold_mode { MODE_MAKE, MODE_STAND, MODE_FALL, MODE_KILL };
enum generator_mode { MODE_START, MODE_LAY, MODE_DONE };

/* the copy of the collision model, in the task's extra. it takes
 * RADIUS_MARGIN more radius than the model */
#define COPY(task) ((NJS_OBJECT *)(task)->extra.p)
#define RADIUS_MARGIN 15.0F
/* world_collect_near_players takes the copy only while the work's flags hold
 * WORLD_ADDED. the foothold sets the flag only while a player lies within
 * NEAR_MARGIN of the copy's bounding sphere */
#define NEAR_MARGIN 16.0F

/* TRUE in the work's tick once player 0 has stood on the foothold. the first
 * stand sets the timer to STAND_TIMER. the foothold drops once the timer
 * passes DROP_TIMER */
#define STOOD(work) ((work)->tick)
#define STAND_TIMER 720
#define DROP_TIMER 900

#define DUST_HALF_X 150.0F
#define DUST_HALF_Z 50.0F
#define DUST_SCALE 7.0F
#define DUST_DENSITY 0.5F
#define DUST_SPEED 1.0F
#define SHAKE_STEPS 60
#define SHAKE_SIZE 2.0F
#define DROP_SOUND 4
#define DROP_VOLUME 80

/* a drop starts at DROP_SPEED a step. below FALL_DEPTH, the foothold's speed
 * grows by FALL_ACCEL a step until it passes FALL_LIMIT. the foothold tumbles
 * there, and FALL_SOUND plays */
#define DROP_SPEED (-0.15F)
#define FALL_DEPTH (-100.0F)
#define FALL_ACCEL 0.1F
#define FALL_LIMIT (-2.1F)
#define FALL_SOUND 5
#define FALL_PRIORITY 1
#define FALL_VOLUME 40
#define FALL_DURATION 120
/* the tumble's turns, in Angle units a step. rand() times TUMBLE_SCALE runs
 * from 0 up to 64. the turns about x and y take it down to a multiple of
 * TUMBLE_STEP. the turn about x is then 24, 8, -8 or -24. the turn about y is
 * 32, 16, 0 or -16. rand() times TUMBLE_Z_SCALE runs from 0 up to 16. the turn
 * about z, that less TUMBLE_Z, runs from -32 to -16 */
#define TUMBLE_SCALE (1.0F / 512)
#define TUMBLE_STEP 16
#define TUMBLE_X 24
#define TUMBLE_Y 32
#define TUMBLE_Z_SCALE (1.0F / 2048)
#define TUMBLE_Z 32.0F

/* the generator lays its footholds up to FOOTHOLD_AHEAD past each of the
 * fight's PLAYERS */
#define FOOTHOLD_AHEAD 2100.0F
#define PLAYERS 2
/* the generator's timer counts its steps while time runs, up to
 * GENERATOR_LATE_STEPS. its work's flags then hold GENERATOR_LATE. each
 * foothold laid from then on starts stood on, at DROP_TIMER, and drops at its
 * second step */
#define GENERATOR_LATE_STEPS 36000
#define GENERATOR_LATE 0x1000
/* the foothold the generator lays at the stage's clear */
#define LAST_FOOTHOLD_X (-300.0F)
#define LAST_FOOTHOLD_TIMER 0x8000
/* the generator starts a foothold's timer at TIMER_PER_BEHIND times how far
 * behind its player it lies. the timer starts at TIMER_LEAST at least. a
 * timer no higher than DROP_TIMER has no effect: player 0's first stand
 * replaces it */
#define TIMER_PER_BEHIND 0.4F
#define TIMER_LEAST (-3600.0F)
/* the foothold's TASK_FORCES records of struct task_force, which TASK_FORCE
 * allocates. a step turns the foothold by the first record's turn, and drops
 * it by that record's speed.y */
#define FORCE(task) ((struct task_force *)(task)->force.p)
/* the task level of the footholds the generator lays */
#define FOOTHOLD_LEVEL 2

static void foothold_draw(struct task *task)
{
    /* @hack redundant: work a register variable, where a plain local would
     * do */
    register struct task_work *work;
    NJS_OBJECT *object;

    object = task->extra.p;
    work = task->work;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&foothold_texlist);
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateEx(object->ang, 0);
    /* @hack redundant: a test of the work's flags no path reaches */
    if (0) {
        if (work->flags) {
        }
    }
    njCnkEasyDrawObject(&foothold_object);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void foothold_destroy(struct task *task)
{
    NJS_OBJECT *object = task->extra.p;

    if (object) {
        if (object->model) {
            world_remove(task, object);
            syFree(object->model);
            object->model = NULL;
        }
        syFree(object);
        task->extra.p = NULL;
    }
}

/* put the center of object's model, in the world, in work's scl */
static void foothold_find_center(struct task_work *work, NJS_OBJECT *object)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateEx(object->ang, 0);
    njCalcPoint(NULL, &object->model->center, &work->scl);
    njPopMatrixEx();
}

static void foothold_make(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_MODEL *model;
    Sint32 flags;
    Float radius;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((object = syMalloc(sizeof(NJS_OBJECT))) == NULL) {
        return;
    }
    if ((model = syMalloc(sizeof(NJS_MODEL))) == NULL) {
        syFree(object);
        return;
    }
    task->extra.p = object;
    *object = foothold_collision;
    *model = *object->model;
    object->model = model;
    task->destructor = foothold_destroy;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    model->r += RADIUS_MARGIN;
    radius = model->r;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    foothold_find_center(work, object);
    task->draw = foothold_draw;
    work->mode = MODE_STAND;
    /* @hack redundant: a test of flags no path reaches */
    if (0) {
        if (flags) {
        }
    }
}

void foothold_step(struct task *task)
{
    struct task_work *work = task->work;
    struct angles ang;
    NJS_OBJECT *object;

    /* @hack redundant: a switch no path reaches, which keeps a call of
     * foothold_make */
    if (1) {
        if (task->placement) {
            if (placement_try_leave(task)) {
                return;
            }
        }
    } else {
        switch (work->mode) {
        case MODE_MAKE:
            foothold_make(task);
            return;
        }
    }
    switch (work->mode) {
    case MODE_MAKE:
        foothold_make(task);
        break;
    case MODE_STAND:
        if (player_near(&work->scl, COPY(task)->model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        if ((Sint16)work->timer > DROP_TIMER) {
            work->mode = MODE_FALL;
            /* @hack spelling: ang, a copy of the work's angles, where
             * &work->ang would do */
            ang = work->ang;
            dust_short_rect(&work->pos, DUST_HALF_X, DUST_HALF_Z, &ang,
                            DUST_SCALE, DUST_DENSITY, DUST_SPEED);
            if (player_work[0]->flags & PLAYER_WORK_ON_GROUND) {
                if (player_records[0]->ground_task == task) {
                    camera_shake(0, SHAKE_STEPS, SHAKE_SIZE);
                }
            }
            FORCE(task)[0].speed.y = DROP_SPEED;
            FORCE(task)[1].speed.y = DROP_SPEED;
            sound_play_at(DROP_SOUND, NULL, 0, DROP_VOLUME, &work->pos);
        }
        break;
    case MODE_FALL:
        if (player_near(&work->scl, COPY(task)->model->r + NEAR_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        work->pos.y += FORCE(task)[0].speed.y;
        work->ang.x += FORCE(task)[0].turn.x;
        work->ang.y += FORCE(task)[0].turn.y;
        work->ang.z += FORCE(task)[0].turn.z;
        object = task->extra.p;
        object->pos[1] = work->pos.y;
        object->ang[0] = work->ang.x;
        object->ang[1] = work->ang.y;
        object->ang[2] = work->ang.z;
        if (work->pos.y < FALL_DEPTH) {
            if (placement_try_leave(task)) {
                work->flags &= ~WORLD_ADDED;
                return;
            }
            if (FORCE(task)[0].speed.y > FALL_LIMIT) {
                FORCE(task)[0].speed.y -= FALL_ACCEL;
            }
            if (!FORCE(task)[0].turn.x) {
                FORCE(task)
                [0].turn.x = TUMBLE_X - ((int)((Float)rand() * TUMBLE_SCALE) &
                                         ~(TUMBLE_STEP - 1));
                FORCE(task)
                [0].turn.y = TUMBLE_Y - ((int)((Float)rand() * TUMBLE_SCALE) &
                                         ~(TUMBLE_STEP - 1));
                FORCE(task)
                [0].turn.z = (int)(((Float)rand() * TUMBLE_Z_SCALE) - TUMBLE_Z);
                FORCE(task)[1] = FORCE(task)[0];
            }
        }
        /* @hack redundant: the test of FALL_DEPTH made again, where one test
         * would do */
        if (work->pos.y < FALL_DEPTH) {
            sound_start_at_for(FALL_SOUND, work, FALL_PRIORITY, FALL_VOLUME,
                               FALL_DURATION, &work->pos);
        }
        break;
    case MODE_KILL:
        task_kill(task);
        return;
    }
    if (!STOOD(work) && player_records[0]->ground_task == task) {
        STOOD(work) = TRUE;
        work->timer = STAND_TIMER;
    }
    if (!game_state.time_stopped && game_state.cleared != 1 && STOOD(work)) {
        work->timer++;
    }
}

void foothold_generator_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    Float timer;
    int i;

    switch (work->mode) {
    case MODE_START:
        work->pos.x = 0.0F;
        work->mode = MODE_LAY;
        break;
    case MODE_LAY:
        if (game_state.cleared) {
            /* @hack spelling: the new foothold kept in task, where a local
             * of its own would do */
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((task = task_create(TASK_WORK | TASK_FORCE, FOOTHOLD_LEVEL,
                                    foothold_step, "ObjectSS2Dodai")) != NULL) {
                task->work->pos.x = LAST_FOOTHOLD_X;
                task->work->timer = LAST_FOOTHOLD_TIMER;
            }
            work->mode = MODE_DONE;
        } else {
            for (i = 0; i < PLAYERS; i++) {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((player = player_work[i]) == NULL) {
                    continue;
                }
                /* @hack spelling: the loop's test inside a for (;;) with a
                 * break, where a while would do */
                for (;;) {
                    if (work->pos.x - player->pos.x < FOOTHOLD_AHEAD) {
                        /* @hack spelling: the new foothold kept in task,
                         * where a local of its own would do */
                        /* @hack spelling: the assignment inside the test,
                         * where a statement of its own would do */
                        if ((task = task_create(TASK_WORK | TASK_FORCE,
                                                FOOTHOLD_LEVEL, foothold_step,
                                                "ObjectSS2Dodai")) != NULL) {
                            task->work->pos.x = work->pos.x;
                            /* @hack spelling: the assignment inside the
                             * test, where a statement of its own would do */
                            if ((timer = (player->pos.x - work->pos.x) *
                                         TIMER_PER_BEHIND) < TIMER_LEAST) {
                                timer = TIMER_LEAST;
                            }
                            task->work->timer = (Sint16)timer;
                            if (work->flags & GENERATOR_LATE) {
                                task->work->timer = DROP_TIMER;
                                STOOD(task->work) = TRUE;
                            }
                        }
                        work->pos.x += FOOTHOLD_SPACING;
                    } else {
                        break;
                    }
                }
            }
        }
        break;
    }
    /* @hack type: game_state's time_stopped read as game_state_time_stopped,
     * where game_state.time_stopped would do */
    if (!game_state_time_stopped) {
        work->timer++;
        if (work->timer > GENERATOR_LATE_STEPS) {
            work->timer = GENERATOR_LATE_STEPS;
            work->flags |= GENERATOR_LATE;
        }
    }
}
