/* the bridge, BRIDGE in Prison Lane's object list. its span slides aside out
 * of an elevator's way, and back */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "stg09/bridge.h"
#include "stg09/elevator.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg09/bridge/text.inc"

/* the bridge's place, in the work's mode. while it slides, the mode is the
 * place it left */
enum bridge_mode { MODE_HOME = 1, MODE_ASIDE };

/* in the work's submode: the bridge rests, slides to its other place, or
 * never slides */
#define BRIDGE_RESTING 0
#define BRIDGE_SLIDING 1
#define BRIDGE_HELD 2
/* in the placement's state: the bridge is aside, and it never slides */
#define STATE_ASIDE 0x1
#define STATE_HELD 0x2

/* the resting steps since the bridge last wanted to slide, in the work's tick,
 * up to WAIT_STEPS. a slide starts only once they have reached it */
#define WAIT(work) ((work)->tick)
#define WAIT_STEPS 120
/* the aside place lies ASIDE_X along the bridge's x. an elevator waiting
 * within WAIT_RANGE of it draws the bridge there. the search for the nearest
 * starts at NEAREST_START, which the first elevator found replaces unread */
#define ASIDE_X (-140.0F)
#define WAIT_RANGE 450.0F
#define NEAREST_START 400.0F
/* the kicks to the spring's speed: down as the bridge starts, and up as it
 * stops */
#define START_KICK (-0.15F)
#define STOP_KICK 0.3F
#define SPRING_DAMPING 0.94F
#define SPRING_STIFFNESS 0.5F
#define WAVES 4

/* the dust along the bridge's edge, EDGE_X along its x, as it starts and
 * stops */
#define EDGE_X (-67.5F)
#define START_DUST_HALF_WIDTH 135.0F
#define STOP_DUST_HALF_WIDTH 128.0F
#define DUST_HALF_DEPTH 33.0F
#define DUST_SCALE 7.0F
#define DUST_DENSITY 0.7F
#define DUST_SPEED 0.1F

/* the world's copy collides while a player lies within its model's radius
 * plus REACH_MARGIN of the model's center. the same sum gives its size
 * class */
#define REACH_MARGIN 15.0F
/* task's TASK_FORCES force records, one for each of players 0 and 1 */
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* a term of the bridge's bob: amp times the sine of freq times the step */
struct bridge_wave {
    Sint32 freq;
    Float amp;
};

/* the bridge's record, in task->motion */
struct bridge {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 aside;
    NJS_POINT3 home;
    /* how far the bridge rides above its pos: the waves' sum plus the
     * spring */
    Float bob;
    Float spring_spd;
    Float spring;
};

#define BRIDGE(task) ((struct bridge *)(task)->motion.p)

static NJS_TEXLIST bridge_texlist;
static NJS_CNK_OBJECT bridge_object;
static NJS_OBJECT bridge_collision;

static struct bridge_wave bridge_waves[WAVES];
static Float bridge_speed;
static Float dat_8c574e90;

static void bridge_destroy(struct task *task);
static void bridge_step(struct task *task);
static void bridge_draw(struct task *task);

/* the sum of count terms of waves at step. only step's low 16 bits count */
/* @hack type: step an int narrowed in the body, where a Uint16 parameter
 * would do */
static Float bridge_wave_sum(struct bridge_wave *waves, int count, int step)
{
    Float sum = 0.0F;
    int at = (Uint16)step;

    while (count--) {
        sum += njSin(waves->freq * at) * waves->amp;
        waves++;
    }
    return sum;
}

void bridge_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 offset;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct bridge));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = bridge_draw;
    task->step = bridge_step;
    task->destructor = bridge_destroy;
    work->mode = MODE_HOME;
    BRIDGE(task)->home = work->pos;
    WAIT(work) = WAIT_STEPS;
    offset.x = ASIDE_X;
    offset.y = 0.0F;
    offset.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &BRIDGE(task)->home);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &BRIDGE(task)->aside);
    njPopMatrix(1);
    if (task->placement != NULL) {
        Sint8 state = placement_get_state(task);

        if (state & STATE_ASIDE) {
            work->mode = MODE_ASIDE;
        } else {
            work->mode = MODE_HOME;
        }
        if (state & STATE_HELD) {
            work->submode = BRIDGE_HELD;
        }
    } else {
        work->mode = MODE_HOME;
    }
    if (work->mode == MODE_ASIDE) {
        work->pos = BRIDGE(task)->aside;
    } else {
        work->pos = BRIDGE(task)->home;
    }
    object = &BRIDGE(task)->object;
    *object = bridge_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
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
}

static void bridge_destroy(struct task *task)
{
    struct task_work *work = task->work;

    /* @bug logic: the destructor sets STATE_ASIDE where the bridge is home
     * (0x8c501598), and bridge_init reads it as aside (0x8c501448). so the
     * placement's next task starts the bridge at its other place.
     * elevator_destroy's bit 0 marks the end its platform is at
     * (0x8c50065e) */
    if (task->placement != NULL) {
        placement_set_state(task,
                            (work->submode == BRIDGE_HELD ? STATE_HELD : 0) |
                                (work->mode == MODE_HOME ? STATE_ASIDE : 0));
    }
    if (BRIDGE(task)->object.model != NULL) {
        world_remove(task, &BRIDGE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void bridge_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 last;
    Float last_bob;
    /* @hack spelling: calc, a local for njCalcPoint, where its calls by name
     * would do */
    void (*calc)(NJS_MATRIX *matrix, NJS_POINT3 *ps, NJS_POINT3 *pd);
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    calc = njCalcPoint;
    /* @hack redundant: a block no path reaches, which zeroes the first
     * wave's height */
    if (0) {
        bridge_waves[0].amp = 0.0F;
    }
    last = work->pos;
    last_bob = BRIDGE(task)->bob;
    if (work->submode == BRIDGE_SLIDING) {
        NJS_POINT3 way;
        Float share;
        struct angles ang;
        NJS_POINT3 edge;

        switch (work->mode) {
        case MODE_HOME:
            way.x = BRIDGE(task)->aside.x - work->pos.x;
            way.y = BRIDGE(task)->aside.y - work->pos.y;
            way.z = BRIDGE(task)->aside.z - work->pos.z;
            share = njScalor(&way);
            if (share > bridge_speed) {
                share = bridge_speed / share;
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
                edge.x = EDGE_X;
                edge.y = 0.0F;
                edge.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                calc(NULL, &edge, &edge);
                njPopMatrix(1);
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                dust_short_rect(&edge, STOP_DUST_HALF_WIDTH, DUST_HALF_DEPTH,
                                &ang, DUST_SCALE, DUST_DENSITY, DUST_SPEED);
                work->submode = BRIDGE_RESTING;
                work->mode = MODE_ASIDE;
                BRIDGE(task)->spring_spd = STOP_KICK;
            }
            break;
        case MODE_ASIDE:
            way.x = BRIDGE(task)->home.x - work->pos.x;
            way.y = BRIDGE(task)->home.y - work->pos.y;
            way.z = BRIDGE(task)->home.z - work->pos.z;
            share = njScalor(&way);
            if (share > bridge_speed) {
                share = bridge_speed / share;
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
                edge.x = EDGE_X;
                edge.y = 0.0F;
                edge.z = 0.0F;
                /* @hack redundant: a block no path reaches, which clears the
                 * edge's y again */
                if (0) {
                    edge.y = 0.0F;
                }
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                calc(NULL, &edge, &edge);
                njPopMatrix(1);
                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                dust_short_rect(&edge, STOP_DUST_HALF_WIDTH, DUST_HALF_DEPTH,
                                &ang, DUST_SCALE, DUST_DENSITY, DUST_SPEED);
                work->submode = BRIDGE_RESTING;
                work->mode = MODE_HOME;
                BRIDGE(task)->spring_spd = STOP_KICK;
            }
            break;
        default:
            break;
        }
    } else if (work->submode == BRIDGE_RESTING) {
        struct task *nearest;
        Float nearest_distance;
        Bool restart;
        Bool wanted;
        struct task **elevator;
        struct task **end;

        wanted = FALSE;
        nearest = NULL;
        restart = FALSE;
        nearest_distance = NEAREST_START;
        elevator = &elevator_tasks[0];
        end = elevator_tasks + ELEVATORS;
        while (elevator < end) {
            struct task *other = *elevator++;

            if (other != NULL && other->work->submode != ELEVATOR_SLIDING) {
                Float distance;

                if ((distance = njDistanceP2P(&BRIDGE(task)->aside,
                                              &other->work->pos)) <
                    WAIT_RANGE) {
                    if (nearest != NULL) {
                        if (nearest_distance > distance) {
                            nearest = other;
                            nearest_distance = distance;
                        }
                    } else {
                        nearest_distance = distance;
                        nearest = other;
                    }
                }
            }
        }
        if (nearest != NULL) {
            if (work->mode == MODE_HOME) {
                wanted = TRUE;
            }
        } else if (work->mode == MODE_ASIDE) {
            wanted = TRUE;
        }
        if (wanted) {
            if (WAIT(work) >= WAIT_STEPS) {
                struct angles ang;
                NJS_POINT3 edge;
                NJS_POINT3 player;

                edge.x = EDGE_X;
                edge.y = 0.0F;
                edge.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                calc(NULL, &edge, &edge);
                njInvertMatrix(NULL);
                calc(NULL, &player_work[0]->pos, &player);
                njPopMatrix(1);
                if (player.x > 0.0F) {
                    ang.x = 0;
                    ang.y = work->ang.y;
                    ang.z = 0;
                    dust_short_rect(&edge, START_DUST_HALF_WIDTH,
                                    DUST_HALF_DEPTH, &ang, DUST_SCALE,
                                    DUST_DENSITY, DUST_SPEED);
                    work->submode = BRIDGE_SLIDING;
                    BRIDGE(task)->spring_spd = START_KICK;
                }
            }
            restart = TRUE;
        }
        if (restart) {
            WAIT(work) = 0;
        } else if (WAIT(work) < WAIT_STEPS) {
            WAIT(work)++;
        }
    }
    if (work->submode != BRIDGE_RESTING) {
        work->timer++;
    }
    BRIDGE(task)->bob = bridge_wave_sum(bridge_waves, WAVES, work->timer);
    BRIDGE(task)->spring_spd = -(BRIDGE(task)->spring * SPRING_STIFFNESS) +
                               (BRIDGE(task)->spring_spd * SPRING_DAMPING);
    BRIDGE(task)->spring += BRIDGE(task)->spring_spd;
    BRIDGE(task)->bob += BRIDGE(task)->spring;
    for (i = 0; i < TASK_FORCES && player_work[i] != NULL; ++i) {
        FORCES(task)[i].speed.x = work->pos.x - last.x;
        FORCES(task)
        [i].speed.y = ((work->pos.y - last.y) + BRIDGE(task)->bob) - last_bob;
        FORCES(task)[i].speed.z = work->pos.z - last.z;
    }
    BRIDGE(task)->object.pos[0] = work->pos.x;
    BRIDGE(task)->object.pos[1] = work->pos.y + BRIDGE(task)->bob;
    BRIDGE(task)->object.pos[2] = work->pos.z;
    if (BRIDGE(task)->object.model != NULL) {
        NJS_POINT3 center;
        Float radius;

        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, (NJS_VECTOR *)BRIDGE(task)->object.pos);
        njRotateY(NULL, BRIDGE(task)->object.ang[1]);
        calc(NULL, &BRIDGE(task)->object.model->center, &center);
        njPopMatrix(1);
        radius = BRIDGE(task)->object.model->r + REACH_MARGIN;
        if (player_near(&center, radius)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void bridge_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&bridge_texlist);
    object = &bridge_object;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + BRIDGE(task)->bob,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

#include "stg09/fixtures/bridge.h"

NJS_TEXLIST *bridge_texlists[3] = {
#include "stg09/bridge/bridge_texlists.inc"
};
static struct bridge_wave bridge_waves[WAVES] = {
    {12432, 0.12F},
    {3728, 0.15F},
    {240, 0.19F},
    {19, 0.5F},
};
/* how far the bridge slides in a step */
static Float bridge_speed = 1.5F;
static Float dat_8c574e90 = 0.2F;
