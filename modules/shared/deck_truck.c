#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "effect/burst.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_rail.h"
#include "shared/deck_truck.h"
#include "shared/dust.h"
#include "shared/vertex_colors.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for ang.y, scl.x and scl.y, which no code reads */
static const char editor_angle_y[12] = {
#include "shared/deck_truck/editor_angle_y.inc"
};

static const char editor_travel[16] = {
#include "shared/deck_truck/editor_travel.inc"
};

static const char editor_speed[12] = {
#include "shared/deck_truck/editor_speed.inc"
};

struct deck_truck {
    struct deck_rail rail;
    Angle wheel_angle;
    Float vertical_speed;
    Angle tilt;
};
#define TRUCK(task) ((struct deck_truck *)(task)->motion.p)
/* the truck's place on the rail. offset.y is its height above the rail */
#define OFFSET(task) (TRUCK(task)->rail.offset)
#define TO_ANGLE(degrees) ((Angle)((degrees) * ANGLE_PER_DEGREE))
#define QUARTER_DEGREES 90.0F
/* DAMAGED is the placement's state for a damaged truck. it is also the
 * work's submode bit that a hit on the weak point sets */
#define DAMAGED 1
/* the work's tick counts the steps from the hit, up to TICK_LIMIT */
#define TICK_LIMIT 100
#define BREAK_POINTS 20
/* each step the speed in y keeps AIR_DRAG of itself, and then loses
 * gravity. a landing faster than LANDING_SOUND_SPEED plays LAND_SOUND */
#define AIR_DRAG 0.993F
#define LANDING_SOUND_SPEED 1.5F
/* a damaged truck above DUST_HEIGHT raises dust at a chance of DUST_CHANCE a
 * step. the fragments' floor lies FLOOR_DROP below pos */
#define DUST_HEIGHT 0.1F
#define DUST_CHANCE 0.5F
#define FLOOR_DROP 1.0F
#define ENGINE_SOUND 0
#define ENGINE_SOUND_STEPS 10
#define BREAK_SOUND 1
#define LAND_SOUND 2
/* the draw skips the truck while a ball of VIEW_RADIUS about it lies wholly
 * behind the camera */
#define VIEW_RADIUS 38.0F
/* the damaged body turns by its tilt about this point of its model */
#define TILT_PIVOT_X 0.075001F
#define TILT_PIVOT_Y 10.22224F
#define TILT_PIVOT_Z (-1.508598F)
/* the lamp's blink, in steps of game_state.steps. it is gray through step
 * LAMP_GRAY_LAST of each BLINK_STEPS, and white after. a damaged truck's
 * lamp stays gray */
#define BLINK_STEPS 48
#define LAMP_GRAY_LAST 20
#define LAMP_ON 0xffffffff
#define LAMP_OFF 0xff808080
/* the body rolls about z by half its tilt */
#define ROLL(task) (TRUCK(task)->tilt / 2)
/* the hit's burst lies BURST_HEIGHT above the truck and rises at BURST_RISE.
 * the fragments fly at FRAGMENT_SPEED and up to FRAGMENT_SPREAD more, and rise
 * at FRAGMENT_RISE. FRAGMENT_RADIUS sets their spin, as debris_break_sized
 * says */
#define BURST_HEIGHT 2.0F
#define BURST_RISE 0.7F
#define BURST_SCALE 14.0F
#define BURST_DEPTH_BIAS 30.0F
#define FRAGMENT_SPEED 0.4F
#define FRAGMENT_SPREAD 0.8F
#define FRAGMENT_RISE 2.5F
#define FRAGMENT_RADIUS 50.0F
/* a damaged truck's dust rises at DUST_RISE. it starts DUST_LIFT above the
 * truck, give or take 1, within a square of side DUST_SPAN about it */
#define DUST_RISE 0.1F
#define DUST_SPAN 20.0F
#define DUST_LIFT 2.0F
#define DUST_SCALE 10.0F
#define DUST_DEPTH_BIAS 30.0F

#include "shared/deck_truck/text.inc"
/* the truck's models. the intact truck is a tree of five nodes. under its
 * base lie the wheels, the body and the lamp. the draw takes each node's
 * model alone, to lift the body, turn the wheels and color the lamp.
 * damaged.h holds the body a hit leaves, and piece.h the fragments' model */
#include "shared/deck_truck/intact.h"
#include "shared/deck_truck/damaged.h"
#include "shared/deck_truck/collision.h"
#include "shared/deck_truck/piece.h"

NJS_TEXLIST *deck_truck_texlists[6] = {
#include "shared/deck_truck/deck_truck_texlists.inc"
};
#define TRUCK_FRAGMENTS 4
static struct debris_part fragments[TRUCK_FRAGMENTS] = {
#include "shared/deck_truck/fragments.inc"
};
/* the wheels' turn a step, in degrees, for each unit of speed */
static Float wheel_degrees = {
#include "shared/deck_truck/wheel_degrees.inc"
};
/* the box of the model object under an intact truck's weak point */
static struct collision_shape truck_box = {
#include "shared/deck_truck/truck_box.inc"
};
/* the upward speed a hit throws the truck at */
static Float launch_speed = {
#include "shared/deck_truck/launch_speed.inc"
};
/* the upward speed the truck loses each step */
static Float gravity = {
#include "shared/deck_truck/gravity.inc"
};
/* the share of its fall that a landing sends the truck up at again */
static Float rebound = {
#include "shared/deck_truck/rebound.inc"
};
/* a damaged body tilts as it rises, to tilt_degrees at the top */
static Float tilt_degrees = {
#include "shared/deck_truck/tilt_degrees.inc"
};
/* the share of its speed along the rail that the truck keeps at a landing */
static Float rolling_friction = {
#include "shared/deck_truck/rolling_friction.inc"
};

static void deck_truck_destroy(struct task *task);
static void deck_truck_step(struct task *task);
static void deck_truck_draw(struct task *task);

void deck_truck_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *node;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct deck_truck));
    if (!task->motion.p) {
        return;
    }
    task->draw = deck_truck_draw;
    task->step = deck_truck_step;
    task->destructor = deck_truck_destroy;
    work->mode = RAIL_START;
    SPEED(task) = 0.0F;
    TRUCK(task)->vertical_speed = 0.0F;
    node = &TRUCK(task)->rail.collision;
    *node = deck_truck_collision;
    RAIL_REGISTER_NODE(task, work, node);
    if (task->placement && placement_get_state(task)) {
        TRUCK(task)->tilt = TO_ANGLE(tilt_degrees);
        TRUCK(task)->vertical_speed = -gravity;
        work->submode |= DAMAGED;
        work->tick = TICK_LIMIT;
    } else {
        weak_point_create(task, 0, &work->pos);
        if (task->child) {
            task->child->work->pos = work->pos;
            task->child->work->ang = work->ang;
            model_object_create(task->child, NULL, NULL, &truck_box, 1, 0);
            if (task->child->child) {
                task->child->child->work->pos = work->pos;
                task->child->child->work->ang = work->ang;
            }
        }
    }
}

static void deck_truck_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->tick) {
        placement_set_state(task, DAMAGED);
    }
    if (TRUCK(task)->rail.collision.model) {
        world_remove(task, &TRUCK(task)->rail.collision);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void deck_truck_step(struct task *task)
{
    struct task_work *work;
    NJS_OBJECT *collision;
    struct deck_truck *truck;
    NJS_POINT3 before;
    NJS_POINT3 center;
    NJS_VECTOR local;
    NJS_POINT3 delta;
    /* @hack redundant: a vector nothing uses, which sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR velocity;

    work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    before = OFFSET(task);
    if (work->mode != RAIL_BACK) {
        if (work->timer || work->tick) {
            OFFSET(task).z += SPEED(task);
            if (!work->tick) {
                if (OFFSET(task).z > work->scl.x) {
                    OFFSET(task).z = work->scl.x;
                    SPEED(task) = -(SPEED(task) * REBOUND);
                } else {
                    SPEED(task) = ((work->scl.x - OFFSET(task).z) * SPRING) +
                                  (SPEED(task) * DAMPING);
                }
                /* @hack redundant: tests of the timer in a block no path
                 * reaches */
                if (0) {
                    if (work->timer) {
                    }
                    if (work->timer > SETTLE_STEPS) {
                    }
                    work->timer = 0;
                }
                if (work->timer++ > SETTLE_STEPS) {
                    work->mode = RAIL_BACK;
                    work->timer = 0;
                    SPEED(task) = 0.0F;
                }
            }
        } else {
            sound_play_at_for(ENGINE_SOUND, work, 1, 0, ENGINE_SOUND_STEPS,
                              &work->pos);
            SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
            if (SPEED(task) > TOP_SPEED(work)) {
                SPEED(task) = TOP_SPEED(work);
            }
            OFFSET(task).z += SPEED(task);
            if (OFFSET(task).z > work->scl.x) {
                OFFSET(task).z = work->scl.x;
                work->timer++;
            }
            TRUCK(task)->wheel_angle += TO_ANGLE(SPEED(task) * wheel_degrees);
        }
    } else {
        if (work->timer || work->tick) {
            OFFSET(task).z -= SPEED(task);
            if (!work->tick) {
                if (OFFSET(task).z < -work->scl.x) {
                    OFFSET(task).z = -work->scl.x;
                    SPEED(task) = -(SPEED(task) * REBOUND);
                } else {
                    SPEED(task) = ((work->scl.x + OFFSET(task).z) * SPRING) +
                                  (SPEED(task) * DAMPING);
                }
                if (work->timer++ > SETTLE_STEPS) {
                    work->mode = RAIL_OUT;
                    work->timer = 0;
                    SPEED(task) = 0.0F;
                }
            }
        } else {
            sound_play_at_for(ENGINE_SOUND, work, 1, 0, ENGINE_SOUND_STEPS,
                              &work->pos);
            SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
            if (SPEED(task) > TOP_SPEED(work)) {
                SPEED(task) = TOP_SPEED(work);
            }
            TRUCK(task)->wheel_angle -= TO_ANGLE(SPEED(task) * wheel_degrees);
            OFFSET(task).z -= SPEED(task);
            if (OFFSET(task).z < -work->scl.x) {
                OFFSET(task).z = -work->scl.x;
                work->timer++;
            }
        }
    }
    OFFSET(task).y += TRUCK(task)->vertical_speed;
    if (OFFSET(task).y < 0.0F) {
        if (TRUCK(task)->vertical_speed < -LANDING_SOUND_SPEED) {
            sound_play_at(LAND_SOUND, NULL, 0, 0, &work->pos);
        }
        OFFSET(task).y = 0.0F;
        TRUCK(task)->vertical_speed =
            njAbs(TRUCK(task)->vertical_speed) * rebound;
        SPEED(task) *= rolling_friction;
    }
    TRUCK(task)->vertical_speed *= AIR_DRAG;
    TRUCK(task)->vertical_speed -= gravity;
    if (work->tick) {
        if (TRUCK(task)->vertical_speed > 0.0F) {
            TRUCK(task)->tilt = TO_ANGLE(
                njSin(TO_ANGLE(((TRUCK(task)->vertical_speed / launch_speed) *
                                QUARTER_DEGREES) +
                               QUARTER_DEGREES)) *
                tilt_degrees);
        }
    } else {
        /* @hack redundant: an empty else */
    }

    if (task->child) {
#ifdef NONMATCHING
        /* the plain form the hack tag names, in rail_push_frame */
        rail_push_frame(work);
#else
        /* @hack spelling: a conditional that picks njPushMatrixEx either
         * way, where a call of it would do */
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        (1.0F ? njPushMatrixEx : njPushMatrixEx)();
        njUnitMatrix(NULL);
        /* @hack redundant: a second njUnitMatrix no path reaches */
        if (0) {
            njUnitMatrix(NULL);
        }
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
#endif
        njCalcPoint(NULL, &OFFSET(task), &center);
        njPopMatrix(1);
        task->child->work->pos = center;
        if (task->child->child) {
            task->child->child->work->pos = center;
        }
    }
    if (!work->tick && work->submode) {
        TRUCK(task)->vertical_speed = launch_speed;
        work->tick = 1;
        score_add(BREAK_POINTS);
    }
    if (work->tick) {
        local.x = 0.0F;
        local.y = DUST_RISE;
        local.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        if (work->tick == 1) {
            /* @hack redundant: a copy of pos into delta, which center
             * replaces before any read */
            delta = work->pos;
            velocity.x = 0.0F;
            velocity.y = BURST_RISE;
            velocity.z = work->mode == RAIL_BACK ? -SPEED(task) : SPEED(task);
            njCalcPoint(NULL, &OFFSET(task), &center);
            njCalcVector(NULL, &velocity, &velocity);
            delta = center;
            center.y += BURST_HEIGHT;
            burst_create(&center, &velocity, BURST_SCALE, BURST_DEPTH_BIAS);
            velocity.y = FRAGMENT_RISE;
            debris_break_sized(
                &delta, work->ang.y, fragments, TRUCK_FRAGMENTS, FRAGMENT_SPEED,
                FRAGMENT_SPREAD, work->pos.y - FLOOR_DROP, &velocity,
                DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH,
                FRAGMENT_RADIUS);
            sound_play_at(BREAK_SOUND, NULL, 0, 0, &work->pos);
        } else if (RANDOM_UNIT() < DUST_CHANCE &&
                   OFFSET(task).y > DUST_HEIGHT) {
            njTranslate(NULL, (RANDOM_UNIT() - 0.5F) * DUST_SPAN,
                        ((RANDOM_UNIT() - 0.5F) * 2.0F) + DUST_LIFT,
                        (RANDOM_UNIT() - 0.5F) * DUST_SPAN);
            njCalcPoint(NULL, &OFFSET(task), &center);
            dust_steady_create(&center, &local, DUST_SCALE, DUST_DEPTH_BIAS);
        }
        njPopMatrix(1);
        if (work->tick < TICK_LIMIT) {
            work->tick++;
        }
    }
    if (TRUCK(task)->rail.collision.model) {
        truck = TRUCK(task);
        local = truck->rail.offset;
        collision = &truck->rail.collision;
#ifdef NONMATCHING
        /* the plain form of the hacks tagged in the matching side */
        local.y = 0.0F;
        delta.x = OFFSET(task).x - before.x;
        delta.y = OFFSET(task).y - before.y;
        delta.z = OFFSET(task).z - before.z;
#else
        {
            Float *out;
            Float *current;
            Float *previous;

            /* @hack spelling: the displacement's components walked by
             * pointers, where named components would do */
            out = &delta.x;
            local.y = 0.0F;
            current = &OFFSET(task).x;
            previous = &before.x;
            /* @hack redundant: tests of previous in a block no path
             * reaches */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (previous && previous && previous) {
                }
            }
            *out++ = *current++ - *previous++;
            *out++ = *current++ - *previous++;
            *out = *current - *previous;
        }
#endif
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &local, (NJS_POINT3 *)collision->pos);
        njCalcVector(NULL, &delta, &TASK_FORCES_OF(task)[0].speed);
        njTranslateV(NULL, &collision->model->center);
        njCalcPoint(NULL, &local, &center);
        njPopMatrix(1);
        TASK_FORCES_OF(task)[1].speed = TASK_FORCES_OF(task)[0].speed;
        collision->ang[2] = ROLL(task);
        collision->ang[0] = -TRUCK(task)->tilt;
        if (player_near(&center, collision->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void deck_truck_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, OFFSET(task).x, 0.0F, OFFSET(task).z);
    if (!sphere_in_front_no_depth(&OFFSET(task), VIEW_RADIUS)) {
        njPopMatrix(1);
        return;
    }
    njSetTexture(&deck_truck_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(deck_truck_base->model);
    njTranslate(NULL, 0.0F, OFFSET(task).y, 0.0F);
    if (work->tick) {
        njTranslate(NULL, TILT_PIVOT_X, TILT_PIVOT_Y, TILT_PIVOT_Z);
        njRotateZ(NULL, ROLL(task));
        njRotateX(NULL, -TRUCK(task)->tilt);
        njTranslate(NULL, -TILT_PIVOT_X, -TILT_PIVOT_Y, -TILT_PIVOT_Z);
        njTranslate(NULL, 0.0F, OFFSET(task).y, 0.0F);
        njSetTexture(&deck_truck_damaged_texlist);
        njCnkEasyDrawModel(deck_truck_damaged->model);
        njSetTexture(&deck_truck_texlist);
    } else {
        njTranslate(NULL, 0.0F, OFFSET(task).y, 0.0F);
        njCnkEasyDrawModel(deck_truck_body->model);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)deck_truck_lamp->pos);
    if (game_state.steps % BLINK_STEPS > LAMP_GRAY_LAST && !work->tick) {
        color = LAMP_ON;
    } else {
        color = LAMP_OFF;
    }
    vlist_set_color(deck_truck_lamp->model->vlist, color);
    njCnkEasyDrawModel(deck_truck_lamp->model);
    njPopMatrix(1);
    if (!work->tick) {
        njPushMatrixEx();
        njTranslateV(NULL, (NJS_VECTOR *)deck_truck_front_wheel->pos);
        njRotateX(NULL, TRUCK(task)->wheel_angle);
        njCnkEasyDrawModel(deck_truck_front_wheel->model);
        njPopMatrix(1);
        njPushMatrixEx();
        njTranslateV(NULL, (NJS_VECTOR *)deck_truck_back_wheel->pos);
        njRotateX(NULL, TRUCK(task)->wheel_angle);
        njCnkEasyDrawModel(deck_truck_back_wheel->model);
        njPopMatrix(1);
    }
    njPopMatrix(1);
}
