#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/core_platform_path.h"
#include "shared/last_step.h"
#include "shared/step_models.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#ifdef ENGINE_LAST_STEP
#include "stg24/reactor.h"
#endif

/* a LASTSTEP's motion record. rider_steps counts the steps in a row player 0
 * has ridden or been held, up to MAX_RIDER_STEPS. panel_steps counts up to
 * PANEL_HOLD while the platform holds the player, and back down after.
 * reactor_steps counts up to REACTOR_LINGER while the reactor lid stands, and
 * back down after it breaks */
struct last_step_motion {
    struct core_platform_path path;
    Sint32 rider_steps;
    Sint32 panel_steps;
#ifdef ENGINE_LAST_STEP
    Sint32 reactor_steps;
#endif
};

#ifdef ENGINE_LAST_STEP
/* hold player 0, with CAPTURE_SOUND at CAPTURE_VOLUME */
/* @hack spelling: a macro written out at both holds, where one hold under
 * both tests would do */
#define CAPTURE_RIDER(work)                                                    \
    do {                                                                       \
        (work)->submode |= HOLD_RIDER;                                         \
        player_request(0, PLAYER_REQUEST_ANIM, 0);                             \
        sound_play(CAPTURE_SOUND, NULL, 0, CAPTURE_VOLUME);                    \
    } while (0)
#define CAPTURE_SOUND 23
#define CAPTURE_VOLUME SOUND_LOUDEST
/* where ang.x holds ON_RIDE, the platform takes a rider only while it moves.
 * it lets the rider go only once it stops. the bit is the lowest of ang.x's
 * second byte, which core_platform_path.h's TRIGGER_RADIUS reads too */
#define ON_RIDE 0x100
#define REACTOR_LINGER 50
/* each step plays LIFT_SOUND at the platform, or gives the sound playing there
 * LIFT_SOUND_STEPS more steps */
#define LIFT_SOUND 10
#define LIFT_SOUND_STEPS 120
/* the height of a rider let go, RIDER_HEIGHT above the platform's top. Eternal
 * Engine's adds RIDER_HEIGHT to bob_height first, and the others add each to
 * pos.y in turn. the two sums can round apart */
#define RELEASE_HEIGHT(task, work)                                             \
    ((work)->pos.y + (LAST_STEP(task)->path.bob_height + RIDER_HEIGHT))
/* how far each panel slides out once risen */
#define PANEL_SLIDE 10.75F
#define RELEASE_READY(task, work)                                              \
    (LAST_STEP(task)->rider_steps > RIDE_LIMIT &&                              \
     (((work)->ang.x & ON_RIDE) ? !((work)->submode & IN_TRANSIT)              \
                                : !LAST_STEP(task)->reactor_steps))
#define MOVE_LIFT(task, move) (move)
#else
#define RELEASE_READY(task, work) (LAST_STEP(task)->rider_steps > RIDE_LIMIT)
#define RELEASE_HEIGHT(task, work)                                             \
    ((work)->pos.y + LAST_STEP(task)->path.bob_height + RIDER_HEIGHT)
#define PANEL_SLIDE 6.25F
/* move the platform, but not while its panels rise */
#define MOVE_LIFT(task, move)                                                  \
    do {                                                                       \
        if (!(LAST_STEP(task)->panel_steps) ||                                 \
            (LAST_STEP(task)->panel_steps >= PANEL_STEPS)) {                   \
            (move);                                                            \
        }                                                                      \
    } while (0)
#endif

/* the bit of the work's submode set while the platform holds player 0. a held
 * player's place is RIDER_HEIGHT above the platform's top. each step, the
 * player keeps FOLLOW of its offset from that place. it turns FOLLOW of the
 * short way toward -ang.y less a quarter turn. RELEASE_READY lets the player
 * go once rider_steps is above RIDE_LIMIT. Eternal Engine's also waits until
 * the platform stops where ang.x holds ON_RIDE, and else until reactor_steps
 * is 0 */
#define HOLD_RIDER 0x4
#define RIDER_HEIGHT 1.0F
#define FOLLOW 0.35F
#define RIDE_LIMIT 120
/* the panels rise PANEL_RISE over PANEL_STEPS steps, then slide apart
 * PANEL_SLIDE over as many more, to PANEL_FULL_OPEN. Cannon's Core's platform
 * then plays PANEL_OPEN_SOUND. the draw's opening runs to PANEL_RISEN as the
 * panels rise, and on to PANEL_PARTED as they slide */
#define PANEL_STEPS 30
#define PANEL_RISEN 1.0F
#define PANEL_PARTED 2.0F
#define PANEL_OPEN_SOUND 16
#define PANEL_FULL_OPEN (2 * PANEL_STEPS)
#define PANEL_HOLD 65
#define PANEL_RISE 4.0F
/* the steps without a rider that core_platform_path.h's RIDER_COOLDOWN
 * reads, which task's unk_3c counts up to MAX_EMPTY_STEPS */
#define STEPS_WITHOUT_RIDER(task) ((task)->unk_3c.l)
#define MAX_RIDER_STEPS 127
/* world_collect_near_players passes over the node of a task without
 * WORLD_ADDED. the work keeps that flag only while a player is within
 * NEAR_RADIUS of the node */
#define NEAR_RADIUS 80.0F

/* the bob's height is bob_amplitude times the sine of its angle. the angle
 * turns bob_speed a step, from a start that origin's length sets */
static Angle bob_speed = 512;
static Float bob_amplitude = 3.0F;

/* @hack spelling: task->motion cast at each use, where a local of the record
 * would do */
#define LAST_STEP(task) ((struct last_step_motion *)(task)->motion.p)

/* a child, Lost Colony's light volume, rides CHILD_HEIGHT above the platform */
#define CHILD_HEIGHT 5.0F

static void last_step_destroy(struct task *task);
static void last_step_step(struct task *task);
static void last_step_draw(struct task *task);

void last_step_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    NJS_POINT3 position;
    Uint32 flags;
    Float radius;
    NJS_POINT3 *child_position;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct last_step_motion));
    if (!task->motion.p) {
        return;
    }
    task->draw = last_step_draw;
    task->step = last_step_step;
    task->destructor = last_step_destroy;
    work->mode = 0;
    CORE_PATH_START((&LAST_STEP(task)->path), work, position);
    LAST_STEP(task)->path.collision = last_step_collision;
    collision = &LAST_STEP(task)->path.collision;
    CORE_PATH_PLACE(collision, work);
    flags = WORLD_MOVING | WORLD_SOLID;
    radius = collision->model->r + CLASS_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, collision);
#ifdef COLONY_LAST_STEP
    colony_light_zone_create(task);
#endif
#ifndef ENGINE_LAST_STEP
    if (task->child) {
        /* @hack spelling: child_position, the address of position, where
         * position's own fields would do */
        child_position = &position;
        *child_position = work->pos;
        child_position->y += CHILD_HEIGHT;
        task->child->work->pos = *child_position;
    }
#endif
}

static void last_step_destroy(struct task *task)
{
    core_path_release(task);
}

static void last_step_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 previous;
    NJS_VECTOR offset;
    Angle turn;
    Float speed;
    Float fraction;
    Float scaled;
    int period;
    Bool riding;
    Uint32 frame;
    NJS_POINT3 *point;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: the set editor's readout, a matrix pop in Cannon's Core
     * and a restart of the path, in a block no path reaches */
    if (0) {
        njPrintC(NJM_LOCATION(17, 16), "<- ASIBATYPE");
#ifdef ENGINE_LAST_STEP
        (void)"ONRIDE";
        (void)"DORYOKU";
#endif
        njPrintC(NJM_LOCATION(17, 17), "<- ROT Y");
#if !defined(COLONY_LAST_STEP) && !defined(ENGINE_LAST_STEP)
        njPopMatrixEx();
#endif
        switch (PATH_KIND(work)) {
        case PATH_ARC:
            njPrintC(NJM_LOCATION(17, 18), "<- ROTY TO CENTER");
            njPrintC(NJM_LOCATION(17, 19), "<- RADIUS");
            break;
        case PATH_HORIZONTAL:
            njPrintC(NJM_LOCATION(17, 18), "<- ROTY-HORZ");
            njPrintC(NJM_LOCATION(17, 19), "<- LENGTH");
            break;
        case PATH_VERTICAL:
            njPrintC(NJM_LOCATION(17, 15), "   COLSIZE");
            njPrintC(NJM_LOCATION(17, 19), "<- LENGTH");
            break;
        case PATH_STILL:
            break;
        }
        njPrintC(NJM_LOCATION(17, 20), "<- SPD");
        njPrintC(NJM_LOCATION(17, 21), "<- STOPTIME");
        CORE_PATH_START((&LAST_STEP(task)->path), work, previous);
    } else {
        riding = player_on_task(0, task);
#ifdef ENGINE_LAST_STEP
        sound_play_at_for(LIFT_SOUND, task, 1, 0, LIFT_SOUND_STEPS, &work->pos);
        if (engine_reactor_lid != NULL) {
            if (LAST_STEP(task)->reactor_steps < REACTOR_LINGER) {
                LAST_STEP(task)->reactor_steps++;
            }
        } else if (LAST_STEP(task)->reactor_steps) {
            LAST_STEP(task)->reactor_steps--;
        }
#endif
        if (work->submode & HOLD_RIDER) {
            if (RELEASE_READY(task, work)) {
                work->submode &= ~HOLD_RIDER;
                player_set_pos(0, work->pos.x, RELEASE_HEIGHT(task, work),
                               work->pos.z);
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
            } else {
                player_request(0, PLAYER_REQUEST_ANIM, 0);
                /* @hack spelling: point, the address of offset, where
                 * offset's own fields would do */
                point = &offset;
                *point = work->pos;
                point->y += LAST_STEP(task)->path.bob_height + RIDER_HEIGHT;
                point->x =
                    ((player_work[0]->pos.x - point->x) * FOLLOW) + point->x;
                point->y =
                    ((player_work[0]->pos.y - point->y) * FOLLOW) + point->y;
                point->z =
                    ((player_work[0]->pos.z - point->z) * FOLLOW) + point->z;
                turn = -work->ang.y - player_work[0]->ang.y - QUARTER_TURN;
#ifdef NONMATCHING
                /* the plain form the hack tag names */
                scaled = (Sint16)turn;
                scaled *= FOLLOW;
                turn = (Angle)(((Uint32)turn & 0xffff0000U) |
                               (Uint16)(Sint16)scaled);
#else
                {
                    /* @hack type: turn's low half scaled through a Sint16
                     * pointer, where the masked update above would do */
                    Sint16 *low = (Sint16 *)&turn;
                    scaled = *low;
                    scaled *= FOLLOW;
                    *low = scaled;
                }
#endif
                player_set_pos(0, point->x, point->y, point->z);
                player_work[0]->ang.y += turn;
            }
#if !defined(COLONY_LAST_STEP) && !defined(ENGINE_LAST_STEP)
            if (LAST_STEP(task)->panel_steps == PANEL_FULL_OPEN) {
                sound_play(PANEL_OPEN_SOUND, NULL, 0, last_step_panel_volume);
            }
#endif
            if (LAST_STEP(task)->panel_steps < PANEL_HOLD) {
                LAST_STEP(task)->panel_steps++;
            }
        } else if (LAST_STEP(task)->panel_steps) {
            LAST_STEP(task)->panel_steps--;
        }

        previous = work->pos;
        /* @hack spelling: frame set in the switch's expression, where an
         * assignment before the switch would do */
        switch (frame = game_state.steps, PATH_KIND(work)) {
        case PATH_STILL:
            break;
        case PATH_ARC:
            speed = ARC_SPEED(work);
            if (speed < MIN_ARC_SPEED) {
                speed = MIN_ARC_SPEED;
            }
            period = (int)((ARC_DEGREES / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            LAST_STEP(task)->path.orbit_angle =
                (Angle)(fraction * QUARTER_TURN);
            offset.x = work->scl.x;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&LAST_STEP(task)->path.pivot);
            njRotateY(NULL, work->ang.z + LAST_STEP(task)->path.orbit_angle +
                                HALF_TURN);
            njCalcPoint(NULL, &offset, &work->pos);
            njPopMatrixEx();
            break;
        case PATH_VERTICAL:
            speed = PATH_SPEED(work);
            if (speed < MIN_PATH_SPEED) {
                speed = MIN_PATH_SPEED;
            }
            period = (int)((work->scl.x / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            work->pos.y =
                LAST_STEP(task)->path.origin.y + (fraction * work->scl.x);
            break;
        case PATH_HORIZONTAL:
            speed = PATH_SPEED(work);
            if (speed < MIN_PATH_SPEED) {
                speed = MIN_PATH_SPEED;
            }
            period = (int)((work->scl.x / speed) + END_WAIT(work));
            SAMPLE_TRAVEL(fraction, frame, period, END_WAIT(work));
            LAST_STEP(task)->path.travel = fraction;
            offset.x = LAST_STEP(task)->path.travel * work->scl.x;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&LAST_STEP(task)->path.origin);
            njRotateY(NULL, work->ang.z);
            njCalcPoint(NULL, &offset, &work->pos);
            njPopMatrixEx();
            break;
        case PATH_RISE:
            if (work->submode & AWAY_FROM_START) {
                if (work->submode & IN_TRANSIT) {
                    MOVE_LIFT(task, work->pos.y -= PATH_SPEED(work));
                    if (work->pos.y < LAST_STEP(task)->path.origin.y) {
                        work->pos.y = LAST_STEP(task)->path.origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                    }
                } else if (STEPS_WITHOUT_RIDER(task) > RIDER_COOLDOWN &&
                           (riding || player_near(&LAST_STEP(task)->path.origin,
                                                  TRIGGER_RADIUS(work)))) {
                    work->submode |= IN_TRANSIT;
                }
            } else if (work->submode & IN_TRANSIT) {
                MOVE_LIFT(task, work->pos.y += PATH_SPEED(work));
                if (work->pos.y >
                    LAST_STEP(task)->path.origin.y + work->scl.x) {
                    work->pos.y = LAST_STEP(task)->path.origin.y + work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                }
            } else if (riding && STEPS_WITHOUT_RIDER(task) > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
            }
            break;
        case PATH_SINK:
            if (work->submode & AWAY_FROM_START) {
                if (work->submode & IN_TRANSIT) {
                    MOVE_LIFT(task, work->pos.y += PATH_SPEED(work));
                    if (work->pos.y > LAST_STEP(task)->path.origin.y) {
                        work->pos.y = LAST_STEP(task)->path.origin.y;
                        work->submode &= ~IN_TRANSIT;
                        work->submode &= ~AWAY_FROM_START;
                    }
                } else if (STEPS_WITHOUT_RIDER(task) > RIDER_COOLDOWN &&
                           (riding || player_near(&LAST_STEP(task)->path.origin,
                                                  TRIGGER_RADIUS(work)))) {
                    work->submode |= IN_TRANSIT;
                }
            } else if (work->submode & IN_TRANSIT) {
                MOVE_LIFT(task, work->pos.y -= PATH_SPEED(work));
                if (work->pos.y <
                    LAST_STEP(task)->path.origin.y - work->scl.x) {
                    work->pos.y = LAST_STEP(task)->path.origin.y - work->scl.x;
                    work->submode &= ~IN_TRANSIT;
                    work->submode |= AWAY_FROM_START;
                }
            } else if (riding && STEPS_WITHOUT_RIDER(task) > RIDER_COOLDOWN) {
                work->submode |= IN_TRANSIT;
            }
            break;
        }
#if !defined(COLONY_LAST_STEP) && !defined(ENGINE_LAST_STEP)
        /* @hack redundant: an assignment to work that no path reaches */
        if (0) {
            work = player_work[0];
        }
#endif
        if (riding || (work->submode & HOLD_RIDER)) {
#ifdef ENGINE_LAST_STEP
            if (work->ang.x & ON_RIDE) {
                if (!(work->submode & HOLD_RIDER) &&
                    (work->submode & IN_TRANSIT)) {
                    CAPTURE_RIDER(work);
                }
            } else if (!(work->submode & HOLD_RIDER) &&
                       LAST_STEP(task)->reactor_steps) {
                CAPTURE_RIDER(work);
            }
#else
            if (!(work->submode & HOLD_RIDER) && (work->submode & IN_TRANSIT)) {
                work->submode |= HOLD_RIDER;
                player_request(0, PLAYER_REQUEST_ANIM, 0);
            }
#endif
            STEPS_WITHOUT_RIDER(task) = 0;
            if (LAST_STEP(task)->rider_steps < MAX_RIDER_STEPS) {
                LAST_STEP(task)->rider_steps++;
            }
        } else {
            LAST_STEP(task)->rider_steps = 0;
            if (STEPS_WITHOUT_RIDER(task) < MAX_EMPTY_STEPS) {
                STEPS_WITHOUT_RIDER(task)++;
            }
        }
        previous.y += LAST_STEP(task)->path.bob_height;
        LAST_STEP(task)->path.bob_phase =
            ((int)njScalor(&LAST_STEP(task)->path.origin) + game_state.steps) *
            bob_speed;
        LAST_STEP(task)->path.bob_height =
            njSin(LAST_STEP(task)->path.bob_phase) * bob_amplitude;
        if (task->force.p) {
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            offset.x = work->pos.x - previous.x;
            offset.y = work->pos.y - previous.y;
            offset.z = work->pos.z - previous.z;
#else
            /* @hack spelling: the displacement's components walked by
             * pointers, where named components would do */
            Float *delta = &offset.x;
            Float *current_axis = &work->pos.x;
            Float *previous_axis = &previous.x;
            *delta++ = *current_axis++ - *previous_axis++;
            *delta++ = *current_axis++ - *previous_axis++;
            *delta = *current_axis - *previous_axis;
#endif
            offset.y += LAST_STEP(task)->path.bob_height;
            if (player_work[0]) {
                TASK_FORCES_OF(task)[0].speed = offset;
            }
            if (player_work[1]) {
                TASK_FORCES_OF(task)[1].speed = offset;
            }
        }
        LAST_STEP(task)->path.collision.pos[0] = work->pos.x;
        LAST_STEP(task)->path.collision.pos[1] =
            work->pos.y + LAST_STEP(task)->path.bob_height;
        LAST_STEP(task)->path.collision.pos[2] = work->pos.z;
        if (player_near((NJS_POINT3 *)LAST_STEP(task)->path.collision.pos,
                        NEAR_RADIUS)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
#ifndef ENGINE_LAST_STEP
    if (task->child) {
        /* @hack spelling: point, the address of previous, where
         * previous's own fields would do */
        point = &previous;
        *point = work->pos;
        point->y += CHILD_HEIGHT;
        task->child->work->pos = *point;
    }
#endif
}

static void last_step_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR guide;
    NJS_VECTOR position;
    NJS_VECTOR size;
    Float opening;

    njSetTexture(&last_step_texlist);
    /* @hack redundant: a block no path reaches, of the set editor's guides to
     * each path's far end and trigger radius */
    if (0) {
        switch (PATH_KIND(work)) {
        case PATH_STILL:
            break;
        case PATH_ARC:
            guide.x = work->scl.x;
            guide.y = 0.0F;
            guide.z = 0.0F;
            njPushMatrixEx();
            njTranslateEx(&LAST_STEP(task)->path.pivot);
            njRotateY(NULL, work->ang.z - QUARTER_TURN);
            njTranslateEx(&guide);
            njRotateY(NULL, work->ang.y);
            njCnkEasyDrawModel(&last_step_model);
            njPopMatrixEx();
            guide.x = 5.0F;
            guide.y = 5.0F;
            guide.z = 5.0F;
            debug_draw_model(&LAST_STEP(task)->path.pivot, NULL, &guide,
                             DEBUG_MODEL_BOX);
            break;
        case PATH_RISE:
            size.x = TRIGGER_RADIUS(work);
            size.y = TRIGGER_RADIUS(work);
            size.z = TRIGGER_RADIUS(work);
            debug_draw_model(&work->pos, NULL, &size, DEBUG_MODEL_SPHERE);
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njTranslate(NULL, 0.0F, work->scl.x, 0.0F);
            njCnkEasyDrawModel(&last_step_model);
            njPopMatrixEx();
            break;
        case PATH_SINK:
            size.x = TRIGGER_RADIUS(work);
            size.y = TRIGGER_RADIUS(work);
            size.z = TRIGGER_RADIUS(work);
            debug_draw_model(&work->pos, NULL, &size, DEBUG_MODEL_SPHERE);
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njTranslate(NULL, 0.0F, -work->scl.x, 0.0F);
            njCnkEasyDrawModel(&last_step_model);
            njPopMatrixEx();
            break;
        case PATH_HORIZONTAL:
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.z);
            njTranslate(NULL, work->scl.x, 0.0F, 0.0F);
            njCnkEasyDrawModel(&last_step_model);
            njPopMatrixEx();
            break;
        case PATH_VERTICAL:
            break;
        }
    }

    njPushMatrixEx();
    position = work->pos;
    position.y += LAST_STEP(task)->path.bob_height;
    njTranslateEx(&position);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&last_step_model);
#ifndef ENGINE_LAST_STEP
    njFogDisable();
    njCnkEasyDrawModel(last_step_unfogged->model);
    njFogEnable();
#endif
    if (LAST_STEP(task)->panel_steps > 0) {
        opening = LAST_STEP(task)->panel_steps * (1.0F / PANEL_STEPS);
        if (opening > PANEL_PARTED) {
            opening = PANEL_PARTED;
        }
        if (opening > PANEL_RISEN) {
            opening -= PANEL_RISEN;
            njPushMatrixEx();
            njTranslate(NULL,
                        last_step_right_panel.pos[0] + (opening * PANEL_SLIDE),
                        last_step_right_panel.pos[1] + PANEL_RISE,
                        last_step_right_panel.pos[2]);
            njCnkEasyDrawModel(last_step_right_panel.model);
            njPopMatrixEx();
            njTranslate(NULL,
                        last_step_left_panel.pos[0] - (opening * PANEL_SLIDE),
                        last_step_left_panel.pos[1] + PANEL_RISE,
                        last_step_left_panel.pos[2]);
            njCnkEasyDrawModel(last_step_left_panel.model);
        } else {
            njPushMatrixEx();
            njTranslate(NULL, last_step_right_panel.pos[0],
                        last_step_right_panel.pos[1] + (opening * PANEL_RISE),
                        last_step_right_panel.pos[2]);
            njCnkEasyDrawModel(last_step_right_panel.model);
            njPopMatrixEx();
            njTranslate(NULL, last_step_left_panel.pos[0],
                        last_step_left_panel.pos[1] + (opening * PANEL_RISE),
                        last_step_left_panel.pos[2]);
            njCnkEasyDrawModel(last_step_left_panel.model);
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
