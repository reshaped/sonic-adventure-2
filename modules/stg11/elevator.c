/* the elevator, ELV in Iron Gate's object list. its platform carries the
 * players in a straight line, between where it stands and its end point.
 * Prison Lane's, in stg09/elevator.c, has a tall gate of three boxes where
 * this one's gate is a rail */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg11/elevator.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "stg11/elevator/text.inc"

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
/* the world's copy of the collision object lies PLATFORM_OFFSET along y from
 * the drawn platform */
#define PLATFORM_OFFSET (-40.0F)
/* the rails and the gate lie at 0 when raised, and at RAILS_DOWN when
 * lowered. the gate, on the platform's +x side, drops at either end, and the
 * rails at home only */
#define RAILS_DOWN (-9.5F)
/* the first RAIL_SHAPES boxes are the rails', and the last the gate's. each
 * box has its center RAIL_CENTER above its rail's height, and rides the bob */
#define RAIL_CENTER 4.25F
#define RAIL_SHAPES 3
#define GATE_SHAPE RAIL_SHAPES
#define ELEVATOR_SHAPES (RAIL_SHAPES + 1)
/* the bob's spring, in the elevator's spring and spring_spd. each step,
 * spring_spd becomes SPRING_DAMPING of itself less SPRING_STIFFNESS of
 * spring. a slide's start sets spring_spd to START_KICK, and its stop to
 * STOP_KICK */
#define START_KICK (-0.3F)
#define STOP_KICK 0.5F
#define SPRING_DAMPING 0.94F
#define SPRING_STIFFNESS 0.5F
#define WAVES 4
/* a player is near the world's copy within its model's radius plus
 * REACH_MARGIN of its pos. the world collects the copy only while one is near.
 * the same sum gives the copy's size class */
#define REACH_MARGIN 15.0F
/* task's TASK_FORCES records, which TASK_FORCE allocates */
#define FORCES(task) ((struct task_force *)(task)->force.p)
/* the players who may board and ride, in slots 0 to RIDERS - 1 */
#define RIDERS 2

/* the platform's sounds. all but STOP_SOUND are ids of stg11/sound_point.h's
 * iron_gate_sounds. ARRIVE_SOUND and STOP_SOUND play as a slide ends. each
 * _UPDATES is sound_play_for's duration, in sound updates */
#define BOARD_SOUND 2
#define LOWERED_SOUND 3
#define SLIDE_SOUND 12
#define SLIDE_UPDATES 20
#define ARRIVE_SOUND 13
#define STOP_SOUND 0x1021
#define STOP_UPDATES 60
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0

/* vibration_start's power, frequency and time for the pad shakes, by prefix.
 * BOARD_ shakes the boarding player's pad. START_ shakes player 0's as the
 * platform starts to move. RUMBLE_ shakes each rider's every RUMBLE_STEPS
 * steps, once the work's timer is past RUMBLE_START */
#define START_POWER 6
#define START_FREQ 20
#define START_TIME 0
#define BOARD_POWER 2
#define BOARD_FREQ 15
#define BOARD_TIME 0
#define RUMBLE_POWER 1
#define RUMBLE_FREQ 12
#define RUMBLE_TIME 1
#define RUMBLE_STEPS 25
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
    Float gate; /* the gate's height, from RAILS_DOWN to 0 */
    /* how far the platform rides above its pos: the waves' sum plus spring */
    Float bob;
    Float spring_spd;
    Float spring;
};

#define ELEVATOR(task) ((struct elevator *)(task)->motion.p)

static NJS_TEXLIST elevator_texlist;
/* the platform's node, whose child is the gate and the child's sibling the
 * rails */
static NJS_CNK_OBJECT elevator_object;
static NJS_OBJECT elevator_collision;

static struct elevator_wave elevator_waves[WAVES];
static Float elevator_speed;
static Float rail_speed;
static struct collision_shape elevator_shapes[ELEVATOR_SHAPES];

static void elevator_step(struct task *task);
static void elevator_destroy(struct task *task);
static void elevator_draw(struct task *task);

/* the sum of count terms of waves at step. only step's low 16 bits count */
static Float elevator_wave_sum(struct elevator_wave *waves, int count, int step)
{
    Float sum = 0.0F;

    while (count--) {
        /* @hack type: step an int narrowed here, where a Uint16 parameter
         * would do */
        sum += njSin(waves->freq * (Uint16)step) * waves->amp;
        waves++;
    }
    return sum;
}

void iron_gate_elevator_init(struct task *task)
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
    /* @hack spelling: the angles set y, z, x, where x, y, z would do */
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y + PLATFORM_OFFSET;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
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
    ELEVATOR(task)->gate = RAILS_DOWN;
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
    for (i = 0; i < ELEVATORS; i++) {
        if (iron_gate_elevator_tasks[i] == NULL) {
            iron_gate_elevator_tasks[i] = task;
            break;
        }
    }
}

static void elevator_destroy(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    for (i = 0; i < ELEVATORS; i++) {
        if (iron_gate_elevator_tasks[i] == task) {
            iron_gate_elevator_tasks[i] = NULL;
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
    NJS_POINT3 last;
    Float last_bob;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which sets the rails and the
     * mode, with an empty else */
    if (0) {
        ELEVATOR(task)->rails = 0.0F;
        work->mode = MODE_HOME;
    } else {
    }
    last = work->pos;
    last_bob = ELEVATOR(task)->bob;
    if (work->submode == ELEVATOR_SLIDING) {
        NJS_VECTOR way;
        struct angles ang;
        Float share;

        switch (work->mode) {
        case MODE_HOME:
            ELEVATOR(task)->rails += rail_speed;
            ELEVATOR(task)->gate += rail_speed;
            if (ELEVATOR(task)->rails > 0.0F) {
                ELEVATOR(task)->rails = 0.0F;
            }
            if (ELEVATOR(task)->gate > 0.0F) {
                ELEVATOR(task)->gate = 0.0F;
            }
            if (ELEVATOR(task)->rails < 0.0F || ELEVATOR(task)->gate < 0.0F) {
                break;
            }
            if (work->timer == 0) {
                vibration_start(0, START_POWER, START_FREQ, START_TIME);
            }
            /* @hack redundant: the rails and the gate set to 0 again, which
             * the tests above have found them at */
            ELEVATOR(task)->rails = 0.0F;
            ELEVATOR(task)->gate = 0.0F;
            way.x = work->scl.x - work->pos.x;
            way.y = work->scl.y - work->pos.y;
            way.z = work->scl.z - work->pos.z;
            share = njScalor(&way);
            if (share > elevator_speed) {
                share = elevator_speed / share;
            } else {
                share = 1.0F;
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
            ELEVATOR(task)->gate += rail_speed;
            if (ELEVATOR(task)->rails > 0.0F) {
                ELEVATOR(task)->rails = 0.0F;
            }
            if (ELEVATOR(task)->gate > 0.0F) {
                ELEVATOR(task)->gate = 0.0F;
            }
            if (ELEVATOR(task)->rails < 0.0F || ELEVATOR(task)->gate < 0.0F) {
                break;
            }
            if (work->timer == 0) {
                vibration_start(0, START_POWER, START_FREQ, START_TIME);
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
        Bool done;
        Bool found;
        struct angles ang;

        done = FALSE;
        if (work->mode == MODE_HOME) {
            if (ELEVATOR(task)->rails <= RAILS_DOWN &&
                ELEVATOR(task)->gate <= RAILS_DOWN) {
                done = TRUE;
            } else {
                ELEVATOR(task)->rails -= rail_speed;
                ELEVATOR(task)->gate -= rail_speed;
                if (RAILS_DOWN > ELEVATOR(task)->rails) {
                    ELEVATOR(task)->rails = RAILS_DOWN;
                }
                if (RAILS_DOWN > ELEVATOR(task)->gate) {
                    ELEVATOR(task)->gate = RAILS_DOWN;
                }
                if (ELEVATOR(task)->rails <= RAILS_DOWN &&
                    ELEVATOR(task)->gate <= RAILS_DOWN) {
                    sound_play(LOWERED_SOUND, NULL, 0, 0);
                }
            }
        } else {
            if (ELEVATOR(task)->gate <= RAILS_DOWN) {
                done = TRUE;
            } else {
                ELEVATOR(task)->gate -= rail_speed;
                if (ELEVATOR(task)->gate <= RAILS_DOWN) {
                    ELEVATOR(task)->gate = RAILS_DOWN;
                    sound_play(LOWERED_SOUND, NULL, 0, 0);
                }
            }
        }
        if (done && work->submode == ELEVATOR_WAITING) {
            found = FALSE;
            for (i = 0; i < RIDERS && player_work[i] != NULL; i++) {
                if (player_on_task(i, task)) {
                    if (WAIT(work) >= WAIT_STEPS) {
                        work->submode = ELEVATOR_SLIDING;
                        ELEVATOR(task)->spring_spd = START_KICK;
                        ang.x = 0;
                        ang.y = work->ang.y;
                        ang.z = 0;
                        dust_short_rect(&work->pos, BOARD_DUST_HALF,
                                        BOARD_DUST_HALF, &ang, BOARD_DUST_SCALE,
                                        BOARD_DUST_DENSITY, BOARD_DUST_SPEED);
                        sound_play(BOARD_SOUND, NULL, 0, 0);
                        vibration_start(i, BOARD_POWER, BOARD_FREQ, BOARD_TIME);
                    }
                    found = TRUE;
                    break;
                }
            }
            if (found) {
                WAIT(work) = 0;
            } else if (WAIT(work) < WAIT_STEPS) {
                WAIT(work)++;
            }
        }
    }
    /* @hack spelling: the submode tested as a truth value, where a test
     * against ELEVATOR_WAITING would do */
    if (work->submode && ELEVATOR(task)->rails == 0.0F &&
        ELEVATOR(task)->gate == 0.0F) {
        work->timer++;
    }
    ELEVATOR(task)->bob = elevator_wave_sum(elevator_waves, WAVES, work->timer);
    ELEVATOR(task)->spring_spd =
        (-(ELEVATOR(task)->spring * SPRING_STIFFNESS)) +
        (ELEVATOR(task)->spring_spd * SPRING_DAMPING);
    ELEVATOR(task)->spring += ELEVATOR(task)->spring_spd;
    ELEVATOR(task)->bob += ELEVATOR(task)->spring;
    for (i = 0; i < TASK_FORCES && player_work[i] != NULL; i++) {
        FORCES(task)[i].speed.x = work->pos.x - last.x;
        FORCES(task)
        [i].speed.y = ((work->pos.y - last.y) + ELEVATOR(task)->bob) - last_bob;
        FORCES(task)[i].speed.z = work->pos.z - last.z;
    }
    ELEVATOR(task)->object.pos[0] = work->pos.x;
    ELEVATOR(task)->object.pos[1] =
        (work->pos.y + ELEVATOR(task)->bob) + PLATFORM_OFFSET;
    ELEVATOR(task)->object.pos[2] = work->pos.z;
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
            (i == GATE_SHAPE ? ELEVATOR(task)->gate : ELEVATOR(task)->rails) +
            RAIL_CENTER;
    }
    if (ELEVATOR(task)->gate == RAILS_DOWN) {
        work->collision->shapes[GATE_SHAPE].flags |= SHAPE_OFF;
    } else {
        work->collision->shapes[GATE_SHAPE].flags &= ~SHAPE_OFF;
    }
    collision_register(task);
}

static void elevator_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
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
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + ELEVATOR(task)->bob,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njPushMatrixEx();
    njTranslate(NULL, object->pos[0], object->pos[1] + ELEVATOR(task)->gate,
                object->pos[2]);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    object = object->sibling;
    njTranslate(NULL, object->pos[0], object->pos[1] + ELEVATOR(task)->rails,
                object->pos[2]);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#include "stg11/fixtures/elevator.h"
#include "shared/prison_fixtures/elevator_collision.h"

struct task *iron_gate_elevator_tasks[ELEVATORS] = {NULL};
NJS_TEXLIST *iron_gate_elevator_texlists[3] = {
#include "stg11/elevator/iron_gate_elevator_texlists.inc"
};
static struct elevator_wave elevator_waves[WAVES] = {
    {11261, 0.11F},
    {3728, 0.15F},
    {229, 0.2F},
    {41, 0.3F},
};
/* each step, the platform slides by elevator_speed. the rails and the gate
 * rise and drop by rail_speed */
static Float elevator_speed = 0.3F;
static Float rail_speed = 0.2F;
/* clang-format off */
static struct collision_shape elevator_shapes[ELEVATOR_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 4.75F, -39.0F}, 39.0F, 4.75F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 4.75F, 39.0F}, 39.0F, 4.75F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-39.0F, 4.75F, 0.0F}, 1.0F, 4.75F, 39.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {39.0F, 4.75F, 0.0F}, 1.0F, 4.75F, 37.0F, 0, {0, 0, 0}},
};
/* clang-format on */
