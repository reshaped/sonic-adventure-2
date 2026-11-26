/* the elevator, ELV in Prison Lane's object list. its platform carries the
 * players in a straight line, between where it stands and its end point */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg09/elevator.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "stg09/elevator/text.inc"

/* the platform's place, in the work's mode. while it slides, the mode is the
 * place it left */
enum elevator_mode { MODE_HOME = 1, MODE_END };

/* in the work's submode, beside ELEVATOR_SLIDING: the platform waits for a
 * rider, or never slides again */
#define ELEVATOR_WAITING 0
#define ELEVATOR_HELD 2
/* in ang.x: the platform slides once */
#define ONE_WAY_BIT 0x1
/* in the placement's state: the platform is at its end, and it never slides
 * again */
#define STATE_END 0x1
#define STATE_HELD 0x2

/* the steps since a player last stood on the waiting platform, in the work's
 * tick, up to WAIT_STEPS. a slide starts only once they have reached it */
#define WAIT(work) ((work)->tick)
#define WAIT_STEPS 120
/* the world's copy of the collision object lies PLATFORM_DROP below the
 * drawn platform */
#define PLATFORM_DROP 40.0F
/* the rails and the gate lie at 0 when raised. lowered, the rails lie at
 * RAILS_DOWN and the gate at GATE_DOWN */
#define RAILS_DOWN (-9.5F)
#define GATE_DOWN (-36.3F)
/* the first RAIL_SHAPES boxes are the rails', and the rest the gate's. a
 * rail's box has its center RAIL_CENTER - BOX_SINK above the rails' height. a
 * gate's box has its center GATE_CENTER - BOX_SINK above the gate's. both
 * ride the bob */
#define RAIL_CENTER 4.75F
#define GATE_CENTER 18.15F
#define BOX_SINK 0.5F
#define RAIL_SHAPES 3
#define GATE_SHAPES 3
#define ELEVATOR_SHAPES (RAIL_SHAPES + GATE_SHAPES)
/* the gate's first box */
#define GATE_SHAPE RAIL_SHAPES
/* the kicks to the spring's speed: down as a slide starts, and up as it
 * stops */
#define START_KICK (-0.3F)
#define STOP_KICK 0.5F
#define SPRING_DAMPING 0.94F
#define SPRING_STIFFNESS 0.5F
#define WAVES 4
/* the world's copy takes its size class from its model's radius plus
 * CLASS_MARGIN. it collides while a player lies within the radius plus
 * REACH_MARGIN of the copy's pos */
#define CLASS_MARGIN 15.0F
#define REACH_MARGIN 30.0F
/* the platform carries the players of slots 0 and 1, each by one of task's
 * TASK_FORCES force records */
#define RIDERS 2
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* the ids of stg09/sound_point.h's prison_lane_sounds that the platform plays,
 * and the clang of a stop. each _UPDATES is sound_play_for's duration, in
 * sound updates */
#define BOARD_SOUND 6
#define SLIDE_SOUND 10
#define SLIDE_UPDATES 20
#define ARRIVE_SOUND 11
#define STOP_SOUND 0x1021
#define STOP_UPDATES 60
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0

/* vibration_start's power, frequency and time for the pad shakes. the
 * boarding player's pad shakes by BOARD_POWER and BOARD_FREQ. player 0's
 * shakes by START_POWER and START_FREQ as the platform starts to move. each
 * rider's shakes by RUMBLE_POWER, RUMBLE_FREQ and RUMBLE_TIME every
 * RUMBLE_STEPS steps, once the work's timer is past RUMBLE_START */
#define START_POWER 6
#define START_FREQ 20
#define BOARD_POWER 2
#define BOARD_FREQ 15
#define RUMBLE_POWER 1
#define RUMBLE_FREQ 12
#define RUMBLE_TIME 1
#define RUMBLE_STEPS 18
#define RUMBLE_START 15

#define BOARD_DUST_HALF 40.0F
#define BOARD_DUST_SCALE 5.0F
#define BOARD_DUST_DENSITY 0.5F
#define BOARD_DUST_SPEED 0.1F
#define ARRIVE_DUST_HALF_X 42.0F
#define ARRIVE_DUST_HALF_Z 40.0F
#define ARRIVE_DUST_SCALE 7.0F
#define ARRIVE_DUST_DENSITY 0.7F
#define ARRIVE_DUST_SPEED 0.15F

/* a term of the platform's bob: amp times the sine of freq times the step */
struct elevator_wave {
    Sint32 freq;
    Float amp;
};

/* the elevator's record, in task->motion */
struct elevator {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 home;
    Float rails; /* the rails' height, from RAILS_DOWN to 0 */
    Float gate; /* the gate's height, from GATE_DOWN to 0 */
    /* how far the platform rides above its pos: the waves' sum plus the
     * spring */
    Float bob;
    Float spring_spd;
    Float spring;
};

#define ELEVATOR(task) ((struct elevator *)(task)->motion.p)

/* the platform's node, whose child is the rails and the child's sibling the
 * gate */
static NJS_TEXLIST elevator_texlist;
static NJS_CNK_OBJECT elevator_object;
static NJS_OBJECT elevator_collision;

static struct elevator_wave elevator_waves[WAVES];
static Float elevator_speed;
static Float rail_speed;
static Float gate_speed;
static Float dat_8c572d18;
static struct collision_shape elevator_shapes[ELEVATOR_SHAPES];

static void elevator_step(struct task *task);
static void elevator_destroy(struct task *task);
static void elevator_draw(struct task *task);

/* the sum of count terms of waves at step. only step's low 16 bits count */
/* @hack type: step an int narrowed in the body, where a Uint16 parameter
 * would do */
static Float elevator_wave_sum(struct elevator_wave *waves, int count, int step)
{
    Float sum = 0.0F;

    while (count--) {
        sum += njSin(waves->freq * (Uint16)step) * waves->amp;
        waves++;
    }
    return sum;
}

void elevator_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;
    Sint8 state;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct elevator));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = elevator_draw;
    task->step = elevator_step;
    task->destructor = elevator_destroy;
    work->mode = MODE_HOME;
    ELEVATOR(task)->home = work->pos;
    WAIT(work) = WAIT_STEPS;
    if (task->placement != NULL) {
        state = placement_get_state(task);
        if (state & STATE_END) {
            work->mode = MODE_END;
        } else {
            work->mode = MODE_HOME;
        }
        if (state & STATE_HELD) {
            work->submode = ELEVATOR_HELD;
        }
    } else {
        work->mode = MODE_HOME;
    }
    if (work->mode == MODE_END) {
        work->pos = work->scl;
    } else {
        work->pos = ELEVATOR(task)->home;
    }
    object = &ELEVATOR(task)->object;
    *object = elevator_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y - PLATFORM_DROP;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    radius = object->model->r + CLASS_MARGIN;
    if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    ELEVATOR(task)->rails = RAILS_DOWN;
    ELEVATOR(task)->gate = 0.0F;
#ifdef NONMATCHING
    /* this side kills the elevator where collision_init fails, whose record the
     * step reads */
    if (!collision_init(task, elevator_shapes, ELEVATOR_SHAPES,
                        COLLISION_OBJECT)) {
        task_kill(task);
        return;
    }
#else
    collision_init(task, elevator_shapes, ELEVATOR_SHAPES, COLLISION_OBJECT);
#endif
    /* @bug logic: an elevator takes every null entry of elevator_tasks, where
     * one was meant. no break follows the store (0x8c50060a). so while one
     * elevator is listed, no other is. the wall gate and the bridge each look
     * for the nearest of all ELEVATORS entries */
    for (i = 0; i < ELEVATORS; i++) {
        if (elevator_tasks[i] == NULL) {
            elevator_tasks[i] = task;
        }
    }
}

static void elevator_destroy(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    for (i = 0; i < ELEVATORS; i++) {
        if (elevator_tasks[i] == task) {
            elevator_tasks[i] = NULL;
        }
    }
    if (task->placement != NULL) {
        placement_set_state(task,
                            (work->submode == ELEVATOR_HELD ? STATE_HELD : 0) |
                                (work->mode == MODE_END ? STATE_END : 0));
    }
    if (ELEVATOR(task)->object.model != NULL) {
        world_remove(task, &ELEVATOR(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void elevator_rumble(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (work->timer > RUMBLE_START && work->timer % RUMBLE_STEPS == 0) {
        for (i = 0; i < RIDERS && player_work[i] != NULL; i++) {
            if (player_on_task(i, task)) {
                vibration_start(i, RUMBLE_POWER, RUMBLE_FREQ, RUMBLE_TIME);
            }
        }
    }
}

static void elevator_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 start; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float y;
    Float x;
    Float z;
    Float zero;
    Bool done;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: zero, a local for 0.0F set at the step's head, where
     * 0.0F in place would do */
    zero = 0.0F;
    /* @hack redundant: a block no path reaches, which numbers the first
     * shape */
    if (0) {
        elevator_shapes[0].id = 1;
    }
    /* @hack redundant: a copy of the work's pos that nothing reads */
    start = work->pos;
    if (work->submode == ELEVATOR_SLIDING) {
        NJS_VECTOR way;
        struct angles ang;
        Float share;

        switch (work->mode) {
        case MODE_HOME:
            ELEVATOR(task)->rails += rail_speed;
            ELEVATOR(task)->gate += gate_speed;
            if (ELEVATOR(task)->rails > 0.0F) {
                ELEVATOR(task)->rails = zero;
            }
            if (ELEVATOR(task)->gate > 0.0F) {
                ELEVATOR(task)->gate = zero;
            }
            if (ELEVATOR(task)->rails < 0.0F || ELEVATOR(task)->gate < 0.0F) {
                break;
            }
            /* @hack spelling: the shake in the else of an empty arm, where
             * a test of the timer for 0 would do */
            if (work->timer) {
            } else {
                vibration_start(0, START_POWER, START_FREQ, 0);
            }
            ELEVATOR(task)->rails = zero;
            ELEVATOR(task)->gate = zero;
            way.x = work->scl.x - work->pos.x;
            way.y = work->scl.y - work->pos.y;
            way.z = work->scl.z - work->pos.z;
            share = njScalor(&way);
            if (share > elevator_speed) {
                share = elevator_speed / share;
            } else {
                share = 1.0F;
            }
            /* @hack redundant: a block no path reaches, which reads the
             * share four times */
            if (0) {
                if (share + share + share + share) {
                }
            }
            way.x *= share;
            way.y *= share;
            way.z *= share;
            work->pos.x += way.x;
            work->pos.y += way.y;
            work->pos.z += way.z;
            if (share == 1.0F) {
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                work->submode = (work->ang.x & ONE_WAY_BIT) ? ELEVATOR_HELD
                                                            : ELEVATOR_WAITING;
                work->mode = MODE_END;
                work->timer = 0;
                ELEVATOR(task)->spring_spd = STOP_KICK;
                dust_rect(&work->pos, ARRIVE_DUST_HALF_X, ARRIVE_DUST_HALF_Z,
                          &ang, ARRIVE_DUST_SCALE, ARRIVE_DUST_DENSITY,
                          ARRIVE_DUST_SPEED);
                sound_play_for(ARRIVE_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               STOP_UPDATES);
                sound_play_for(STOP_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               STOP_UPDATES);
            } else {
                sound_play_for(SLIDE_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               SLIDE_UPDATES);
                elevator_rumble(task);
            }
            break;
        case MODE_END:
            ELEVATOR(task)->rails += rail_speed;
            ELEVATOR(task)->gate += gate_speed;
            if (ELEVATOR(task)->rails > 0.0F) {
                ELEVATOR(task)->rails = zero;
            }
            if (ELEVATOR(task)->gate > 0.0F) {
                ELEVATOR(task)->gate = zero;
            }
            if (ELEVATOR(task)->rails < 0.0F || ELEVATOR(task)->gate < 0.0F) {
                break;
            }
            /* @hack spelling: the shake in the else of an empty arm, where
             * a test of the timer for 0 would do */
            if (work->timer) {
            } else {
                vibration_start(0, START_POWER, START_FREQ, 0);
            }
            way.x = ELEVATOR(task)->home.x - work->pos.x;
            way.y = ELEVATOR(task)->home.y - work->pos.y;
            way.z = ELEVATOR(task)->home.z - work->pos.z;
            share = njScalor(&way);
            if (share > elevator_speed) {
                share = elevator_speed / share;
            } else {
                share = 1.0F;
            }
            /* @hack redundant: a block no path reaches, which reads the
             * share four times */
            if (0) {
                if (share + share + share + share) {
                }
            }
            way.x *= share;
            way.y *= share;
            way.z *= share;
            work->pos.x += way.x;
            work->pos.y += way.y;
            work->pos.z += way.z;
            if (share == 1.0F) {
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                work->submode = (work->ang.x & ONE_WAY_BIT) ? ELEVATOR_HELD
                                                            : ELEVATOR_WAITING;
                work->mode = MODE_HOME;
                work->timer = 0;
                ELEVATOR(task)->spring_spd = STOP_KICK;
                dust_rect(&work->pos, ARRIVE_DUST_HALF_X, ARRIVE_DUST_HALF_Z,
                          &ang, ARRIVE_DUST_SCALE, ARRIVE_DUST_DENSITY,
                          ARRIVE_DUST_SPEED);
                sound_play_for(ARRIVE_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               STOP_UPDATES);
                sound_play_for(STOP_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               STOP_UPDATES);
            } else {
                sound_play_for(SLIDE_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                               SLIDE_UPDATES);
                elevator_rumble(task);
            }
            break;
        }
    } else {
        done = FALSE;
        if (work->mode == MODE_HOME) {
            if (!(ELEVATOR(task)->rails > RAILS_DOWN) &&
                !(ELEVATOR(task)->gate > GATE_DOWN)) {
                done = TRUE;
            } else {
                ELEVATOR(task)->rails -= rail_speed;
                ELEVATOR(task)->gate -= rail_speed;
                if (ELEVATOR(task)->rails < RAILS_DOWN) {
                    ELEVATOR(task)->rails = RAILS_DOWN;
                }
                if (ELEVATOR(task)->gate < GATE_DOWN) {
                    ELEVATOR(task)->gate = GATE_DOWN;
                }
                if (!(ELEVATOR(task)->rails > RAILS_DOWN) &&
                    !(ELEVATOR(task)->gate > GATE_DOWN)) {
                    sound_play_for(STOP_SOUND, work, SOUND_PRIORITY,
                                   SOUND_VOLUME, STOP_UPDATES);
                }
            }
        } else {
            if (!(ELEVATOR(task)->gate > GATE_DOWN)) {
                done = TRUE;
            } else {
                ELEVATOR(task)->gate -= rail_speed;
                if (!(ELEVATOR(task)->gate > GATE_DOWN)) {
                    ELEVATOR(task)->gate = GATE_DOWN;
                    sound_play_for(STOP_SOUND, work, SOUND_PRIORITY,
                                   SOUND_VOLUME, STOP_UPDATES);
                }
            }
        }
        if (done && work->submode == ELEVATOR_WAITING) {
            done = FALSE;
            for (i = 0; i < RIDERS && player_work[i] != NULL; i++) {
                if (player_on_task(i, task)) {
                    if (WAIT(work) >= WAIT_STEPS) {
                        struct angles ang;

                        work->submode = ELEVATOR_SLIDING;
                        ELEVATOR(task)->spring_spd = START_KICK;
                        ang.x = 0;
                        ang.y = work->ang.y;
                        ang.z = 0;
                        dust_short_rect(&work->pos, BOARD_DUST_HALF,
                                        BOARD_DUST_HALF, &ang, BOARD_DUST_SCALE,
                                        BOARD_DUST_DENSITY, BOARD_DUST_SPEED);
                        sound_play(BOARD_SOUND, NULL, 0, 0);
                        vibration_start(i, BOARD_POWER, BOARD_FREQ, 0);
                    }
                    done = TRUE;
                    break;
                }
            }
            if (done) {
                WAIT(work) = 0;
            } else if (WAIT(work) < WAIT_STEPS) {
                WAIT(work)++;
            }
        }
    }
    if (work->submode && ELEVATOR(task)->rails == 0.0F &&
        ELEVATOR(task)->gate == 0.0F) {
        work->timer++;
    }
    ELEVATOR(task)->bob = elevator_wave_sum(elevator_waves, WAVES, work->timer);
    ELEVATOR(task)->spring_spd =
        ((-ELEVATOR(task)->spring) * SPRING_STIFFNESS) +
        (ELEVATOR(task)->spring_spd * SPRING_DAMPING);
    ELEVATOR(task)->spring += ELEVATOR(task)->spring_spd;
    ELEVATOR(task)->bob += ELEVATOR(task)->spring;
    x = work->pos.x;
    y = work->pos.y + ELEVATOR(task)->bob - PLATFORM_DROP;
    z = work->pos.z;
    for (i = 0; i < TASK_FORCES && player_work[i] != NULL; i++) {
        FORCES(task)[i].speed.x = x - ELEVATOR(task)->object.pos[0];
        FORCES(task)[i].speed.y = y - ELEVATOR(task)->object.pos[1];
        FORCES(task)[i].speed.z = z - ELEVATOR(task)->object.pos[2];
    }
    /* @hack redundant: a block no path reaches, which reads y */
    if (0) {
        if (y) {
        }
    }
    ELEVATOR(task)->object.pos[0] = x;
    ELEVATOR(task)->object.pos[1] = y;
    ELEVATOR(task)->object.pos[2] = z;
    if (ELEVATOR(task)->object.model != NULL) {
        if (player_near((NJS_POINT3 *)ELEVATOR(task)->object.pos,
                        ELEVATOR(task)->object.model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    for (i = 0; i < ELEVATOR_SHAPES; i++) {
        work->collision->shapes[i].center.y =
            ELEVATOR(task)->bob +
            (i >= GATE_SHAPE ? ELEVATOR(task)->gate + GATE_CENTER
                             : ELEVATOR(task)->rails + RAIL_CENTER) -
            BOX_SINK;
    }
    if (ELEVATOR(task)->gate == GATE_DOWN) {
        work->collision->shapes[GATE_SHAPE].flags |= SHAPE_OFF;
        work->collision->shapes[GATE_SHAPE + 1].flags |= SHAPE_OFF;
        work->collision->shapes[GATE_SHAPE + 2].flags |= SHAPE_OFF;
    } else {
        work->collision->shapes[GATE_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[GATE_SHAPE + 1].flags &= ~SHAPE_OFF;
        work->collision->shapes[GATE_SHAPE + 2].flags &= ~SHAPE_OFF;
    }
    collision_register(task);
}

static void elevator_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODE[ONEWAY]";
        (void)"<- MODE[TWOWAY]";
        (void)"<- ANGLE Y";
        (void)"<- END POS X";
        (void)"<- END POS Y";
        (void)"<- END POS Z";
    }
    njSetTexture(&elevator_texlist);
    object = &elevator_object;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + ELEVATOR(task)->bob,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njPushMatrixEx();
    njTranslate(NULL, object->pos[0], object->pos[1] + ELEVATOR(task)->rails,
                object->pos[2]);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    object = object->sibling;
    njPushMatrixEx();
    njTranslate(NULL, object->pos[0], object->pos[1] + ELEVATOR(task)->gate,
                object->pos[2]);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#include "stg09/fixtures/elevator.h"
#include "shared/prison_fixtures/elevator_collision.h"

struct task *elevator_tasks[ELEVATORS] = {NULL};
NJS_TEXLIST *elevator_texlists[3] = {
#include "stg09/elevator/elevator_texlists.inc"
};
static struct elevator_wave elevator_waves[WAVES] = {
    {11261, 0.11F},
    {3728, 0.15F},
    {229, 0.2F},
    {41, 0.3F},
};
/* each step, the platform slides by elevator_speed. the rails rise by
 * rail_speed, and the gate by gate_speed. both drop by rail_speed */
static Float elevator_speed = 0.6F;
static Float rail_speed = 0.2F;
static Float gate_speed = 0.32F;
static Float dat_8c572d18 = 256.0F;
/* clang-format off */
static struct collision_shape elevator_shapes[ELEVATOR_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {10.0F, RAIL_CENTER, -39.0F}, 34.0F, 4.75F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {10.0F, RAIL_CENTER, 39.0F}, 34.0F, 4.75F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {39.0F, RAIL_CENTER, 0.0F}, 1.0F, 4.75F, 37.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {-39.0F, GATE_CENTER, 0.0F}, 1.0F, 18.15F, 39.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {-29.0F, GATE_CENTER, 39.0F}, 10.0F, 18.15F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_2000,
     {-29.0F, GATE_CENTER, -39.0F}, 10.0F, 18.15F, 1.0F, 0, {0, 0, 0}},
};
/* clang-format on */
