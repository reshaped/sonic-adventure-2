#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "effect/burst.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#include "shared/dynamite_bridge.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#ifdef COLONY_BRIDGE
#include "stg27/light.h"
#endif

/* spin_x's turn a step. the drag scales only its low 16 bits, through low,
 * and leaves the high 16 as they are */
union bridge_spin {
    Angle angle;
    Sint16 low;
};

/* a bridge's record, at its task's motion. collision is its node in the world.
 * a falling bridge draws moved by offset, and turned about its model's center
 * by ang.x and ang.z. velocity, spin_x and spin_z change them each step. spin_y
 * turns ang.y, which no draw reads */
struct bridge {
    NJS_OBJECT collision;
    NJS_POINT3 offset;
    NJS_VECTOR velocity;
    struct angles ang;
    union bridge_spin spin_x;
    Angle spin_y, spin_z;
};

enum bridge_mode { BRIDGE_RESTING, BRIDGE_FALLING, BRIDGE_HIDDEN };

#define BRIDGE(task) ((struct bridge *)(task)->motion.p)
/* the id of the charges whose blast drops a BRIDGE_DYNA */
#define CHARGE_ID(work) ((Uint8)(work)->ang.x)
#define SPREAD() (((Float)rand() * RAND_SCALE) - 0.5F)
/* each step a falling bridge keeps DRAG of its speed and of spin_x */
#define DRAG 0.995F
/* a falling bridge hides once velocity.y is below HIDE_VELOCITY. velocity.y
 * only nears -gravity / (1 - DRAG), which is -10, so no bridge hides */
#define HIDE_VELOCITY (-10000.0F)
/* the burst's upward speed, and its size. a falling bridge starts at the
 * burst's speed */
#define LAUNCH_SPEED 2.5F
#define BURST_SIZE 18.0F
#define BURST_SOUND 8
#define LIGHT_HEIGHT 5.0F
/* the node is in the world's collision while a player is within
 * COLLISION_RANGE of a resting bridge's pos */
#define COLLISION_RANGE 100.0F

static Float gravity = 0.05F;
/* a falling bridge's speed starts up to half speed_spread from the burst's,
 * either way, in units a step. its spins about x and z start up to half
 * spin_spread either way, in degrees a step. its spin about y starts up to a
 * quarter */
static Float spin_spread = 12.0F;
static Float speed_spread = 2.0F;
#ifndef COLONY_BRIDGE
static NJS_VECTOR rest = {0.0F, 0.0F, 0.0F};
#endif

static void dynamite_bridge_destroy(struct task *task);
static void dynamite_bridge_step(struct task *task);
static void dynamite_bridge_draw(struct task *task);
static void fixed_bridge_destroy(struct task *task);
static void fixed_bridge_step(struct task *task);
static void fixed_bridge_draw(struct task *task);

#pragma inline(dismount_bridge)
static void dismount_bridge(struct task *task)
{
    if (BRIDGE(task)->collision.model) {
        world_remove(task, &BRIDGE(task)->collision);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#define BRIDGE_CALLBACK(name) dynamite_bridge_##name
#include "shared/bridge_mount_impl.h"
#undef BRIDGE_CALLBACK

static void dynamite_bridge_destroy(struct task *task)
{
    dismount_bridge(task);
}

static void dynamite_bridge_step(struct task *task)
{
    struct task_work *work = task->work;
#ifdef COLONY_BRIDGE
    static NJS_VECTOR rest = {0.0F, 0.0F, 0.0F};
#else
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
#endif
#ifdef COLONY_BRIDGE
    struct bridge *bridge;
#endif

    if (placement_try_leave(task)) {
        return;
    }
    if (!dynamite_count(CHARGE_ID(work))) {
        if (work->mode == BRIDGE_RESTING) {
#ifdef COLONY_BRIDGE
            /* @hack spelling: pos and velocity declared in this block, where
             * declarations at the function's top would do */
            NJS_POINT3 pos;
            NJS_VECTOR velocity;
#endif
            velocity.x = 0.0F;
            velocity.y = LAUNCH_SPEED;
            velocity.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &bridge_model->center, &pos);
            njCalcVector(NULL, &velocity, &velocity);
            njPopMatrixEx();
            burst_create_wobbling(&pos, &velocity, BURST_SIZE);
            BRIDGE(task)->velocity.x = velocity.x + (SPREAD() * speed_spread);
#ifndef COLONY_BRIDGE
            /* @hack redundant: a call of rand that no path reaches */
            if (0) {
                rand();
            }
#endif
            BRIDGE(task)->velocity.y = velocity.y + (SPREAD() * speed_spread);
            BRIDGE(task)->velocity.z = velocity.z + (SPREAD() * speed_spread);
            BRIDGE(task)->spin_x.angle =
                SPREAD() * spin_spread * ANGLE_PER_DEGREE;
            BRIDGE(task)->spin_y =
                SPREAD() * spin_spread * (ANGLE_PER_DEGREE * 0.5F);
            BRIDGE(task)->spin_z = SPREAD() * spin_spread * ANGLE_PER_DEGREE;
#ifndef COLONY_BRIDGE
            sound_play(BURST_SOUND, NULL, 0, 0);
#endif
        }
        work->mode = BRIDGE_FALLING;
    }
    if (work->mode == BRIDGE_FALLING) {
#ifdef COLONY_BRIDGE
        /* @hack spelling: ang.x's sum through bridge, a local copy of
         * BRIDGE(task), where BRIDGE(task) read in place would do */
        bridge = BRIDGE(task);
        bridge->ang.x += bridge->spin_x.angle;
        BRIDGE(task)->ang.y += BRIDGE(task)->spin_y;
#else
        BRIDGE(task)->ang.x += BRIDGE(task)->spin_x.angle;
        {
            /* @hack spelling: ang.y's sum through bridge, a local copy of
             * BRIDGE(task), where BRIDGE(task) read in place would do */
            struct bridge *bridge = BRIDGE(task);
            bridge->ang.y += bridge->spin_y;
        }
#endif
        BRIDGE(task)->ang.z += BRIDGE(task)->spin_z;
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        BRIDGE(task)->offset.x += BRIDGE(task)->velocity.x;
        BRIDGE(task)->offset.y += BRIDGE(task)->velocity.y;
        BRIDGE(task)->offset.z += BRIDGE(task)->velocity.z;
        BRIDGE(task)->spin_x.low = (Sint16)(BRIDGE(task)->spin_x.low * DRAG);
#else
        /* @hack spelling: the sum's components walked by pointers, and the
         * drag through a pointer to spin_x.low and a Float local, where named
         * components and spin_x.low would do */
        {
            Float *offset = &BRIDGE(task)->offset.x;
            Float *speed = &BRIDGE(task)->velocity.x;
#ifdef COLONY_BRIDGE
            union bridge_spin *spin;
            Float spin_speed;
            *offset = *offset + *speed;
            offset++;
            speed++;
            *offset = *offset + *speed;
            offset++;
            speed++;
            *offset = *offset + *speed;
            spin = &BRIDGE(task)->spin_x;
            spin_speed = spin->low;
            spin->low = spin_speed * DRAG;
#else
            *offset++ += *speed++;
            *offset++ += *speed++;
            *offset += *speed;
            {
                Sint16 *spin = &BRIDGE(task)->spin_x.low;
                Float damped = *spin;
                damped *= DRAG;
                *spin = damped;
            }
#endif
        }
#endif
        BRIDGE(task)->velocity.x *= DRAG;
        BRIDGE(task)->velocity.y = (BRIDGE(task)->velocity.y * DRAG) - gravity;
        BRIDGE(task)->velocity.z *= DRAG;
        if (BRIDGE(task)->velocity.y < HIDE_VELOCITY) {
            work->mode = BRIDGE_HIDDEN;
        }
        work->flags &= ~WORLD_ADDED;
    } else {
        if (work->mode == BRIDGE_RESTING &&
            player_near(&work->pos, COLLISION_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
#ifdef COLONY_BRIDGE
        BRIDGE(task)->offset = rest;
#else
        /* @hack type: rest read through a pointer to const NJS_VECTOR, where
         * rest would do */
        BRIDGE(task)->offset = *(const NJS_VECTOR *)&rest;
#endif
        BRIDGE(task)->velocity = rest;
        BRIDGE(task)->ang.x = 0;
        BRIDGE(task)->ang.y = 0;
        BRIDGE(task)->ang.z = 0;
        BRIDGE(task)->spin_x.angle = 0;
        BRIDGE(task)->spin_y = 0;
        BRIDGE(task)->spin_z = 0;
        work->mode = BRIDGE_RESTING;
    }
#ifdef COLONY_BRIDGE
    if (task->child) {
        NJS_POINT3 light;
        light = work->pos;
        light.y += LIGHT_HEIGHT;
        task->child->work->pos = light;
    }
#endif
}

static void dynamite_bridge_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_MODEL *model;

    if (work->mode != BRIDGE_HIDDEN) {
        njSetTexture(&dynamite_bridge_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        model = bridge_model;
        if (work->mode == BRIDGE_FALLING) {
            njTranslateEx(&BRIDGE(task)->offset);
            njTranslate(NULL, model->center.x, model->center.y,
                        model->center.z);
            njRotateX(NULL, BRIDGE(task)->ang.x);
            njRotateZ(NULL, BRIDGE(task)->ang.z);
            njTranslate(NULL, -model->center.x, -model->center.y,
                        -model->center.z);
        }
#ifdef COLONY_BRIDGE
        njCnkEasyDrawModel(model);
        njFogDisable();
        njCnkEasyDrawModel(&bridge_light_model);
        njFogEnable();
#else
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
#endif
        njPopMatrixEx();
    }
}

#define BRIDGE_CALLBACK(name) fixed_bridge_##name
#include "shared/bridge_mount_impl.h"
#undef BRIDGE_CALLBACK

static void fixed_bridge_destroy(struct task *task)
{
    dismount_bridge(task);
}

static void fixed_bridge_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        if (player_near(&work->pos, COLLISION_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void fixed_bridge_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&dynamite_bridge_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
#ifdef COLONY_BRIDGE
    njCnkEasyDrawModel(bridge_model);
    njFogDisable();
    njCnkEasyDrawModel(&bridge_light_model);
    njFogEnable();
#else
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(bridge_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
#endif
    njPopMatrixEx();
}
