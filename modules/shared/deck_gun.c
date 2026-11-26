#include "shared/deck_gun/text.inc"
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_gun.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's label for ang.y, which no code reads */
static const char editor_rotation_y[12] = {
#include "shared/deck_gun/editor_rotation_y.inc"
};

/* LID_OPENING runs from 0 for a shut lid to 1 for one open lid_degrees.
 * LID_SPEED is its speed a step. they are Floats kept in the pointers
 * task->force and task->extra. the destructor clears both words, which
 * task_destroy would free */
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define LID_OPENING(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define LID_SPEED(task) (*(Float *)&(task)->extra)
/* the bits of the placement's state */
#define GUN_GONE 1
#define LID_HIT 2
/* the gun turns toward a player within AIM_RANGE of pos, by up to
 * MAX_GUN_TURN a step */
#define AIM_RANGE 300.0F
#define MAX_GUN_TURN 160
/* the gun's heading, in the work's timer */
#define HEADING(work) ((work)->timer)
/* the work's tick counts the steps from the gun's break, up to TICK_LIMIT.
 * the gun still draws while the count is below GUN_DRAW_STEPS */
#define TICK_LIMIT 100
#define GUN_DRAW_STEPS 3
/* a hit sets the lid moving at LID_KICK. each step it keeps LID_DAMPING of
 * its speed and gains lid_acceleration. at either end it turns back at
 * LID_BOUNCE of its speed */
#define LID_KICK 0.1F
#define LID_DAMPING 0.99F
#define LID_BOUNCE 0.7F
/* the steps from the lid's hit, in the work's mode, or 0 before it */
#define HIT_STEPS(work) ((work)->mode)
/* until HIT_STEPS reaches DUST_STEPS, dust rises at a chance of DUST_CHANCE
 * a step. it rises at DUST_RISE from DUST_X and DUST_Y in the gun's frame.
 * its scale is DUST_SCALE and up to DUST_SCALE_RANGE more */
#define DUST_CHANCE 0.5F
#define DUST_STEPS 60
#define DUST_X (-47.0F)
#define DUST_Y 45.0F
#define DUST_RISE 0.8F
#define DUST_SCALE 2.0F
#define DUST_SCALE_RANGE 6.0F
#define DUST_DEPTH_BIAS 3.0F
/* the barrel a hit breaks off leaves at BARREL_RISE */
#define BARREL_RISE 0.2F
#define HIT_SOUND 5
#define HIT_POINTS 20

static NJS_TEXNAME deck_gun_textures[7] = {
#include "shared/deck_gun/deck_gun_textures.inc"
};

static NJS_TEXLIST deck_gun_texlist = {
#include "shared/deck_gun/deck_gun_texlist.inc"
};

static Sint16 deck_gun_barrel_polygons[323] = {
#include "shared/deck_gun/deck_gun_barrel_polygons.inc"
};

static Sint32 deck_gun_barrel_vertices[351] = {
#include "shared/deck_gun/deck_gun_barrel_vertices.inc"
};

static NJS_CNK_MODEL deck_gun_barrel_model = {
#include "shared/deck_gun/deck_gun_barrel_model.inc"
};

static NJS_CNK_OBJECT deck_gun_barrel[1] = {
#include "shared/deck_gun/deck_gun_barrel.inc"
};

static Sint16 deck_gun_lid_polygons[43] = {
#include "shared/deck_gun/deck_gun_lid_polygons.inc"
};

static Sint32 deck_gun_lid_vertices[35] = {
#include "shared/deck_gun/deck_gun_lid_vertices.inc"
};

static NJS_CNK_MODEL deck_gun_lid_model = {
#include "shared/deck_gun/deck_gun_lid_model.inc"
};

static NJS_CNK_OBJECT deck_gun_lid[1] = {
#include "shared/deck_gun/deck_gun_lid.inc"
};

static Sint16 deck_gun_base_polygons[231] = {
#include "shared/deck_gun/deck_gun_base_polygons.inc"
};

static Sint32 deck_gun_base_vertices[223] = {
#include "shared/deck_gun/deck_gun_base_vertices.inc"
};

static NJS_CNK_MODEL deck_gun_base_model = {
#include "shared/deck_gun/deck_gun_base_model.inc"
};

static NJS_CNK_OBJECT deck_gun_base[1] = {
#include "shared/deck_gun/deck_gun_base.inc"
};

NJS_TEXLIST *deck_gun_texlists[3] = {
#include "shared/deck_gun/deck_gun_texlists.inc"
};
static struct collision_shape supports[2] = {
#include "shared/deck_gun/supports.inc"
};
/* @hack type: one-shape arrays, where a single collision_shape each would
 * do */
static struct collision_shape lid_box[1] = {
#include "shared/deck_gun/lid_box.inc"
};
static struct collision_shape gun_target[1] = {
#include "shared/deck_gun/gun_target.inc"
};
static Float lid_acceleration = 0.008F;
static Float lid_degrees = 20.0F;

static void deck_gun_destroy(struct task *task);
static void deck_gun_step(struct task *task);
static void deck_gun_draw(struct task *task);

/* position gets the gun's place in the world */
/* @hack spelling: a macro, where a function of work and position would do */
#define GUN_POSITION(work, position)                                           \
    do {                                                                       \
        njPushMatrixEx();                                                      \
        njUnitMatrix(NULL);                                                    \
        njTranslateV(NULL, &(work)->pos);                                      \
        njRotateY(NULL, (work)->ang.y);                                        \
        njCalcPoint(NULL, (NJS_POINT3 *)deck_gun_barrel->pos, (position));     \
        njPopMatrix(1);                                                        \
    } while (0)

void deck_gun_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;

    if (!placement_try_leave(task)) {
        task->draw = deck_gun_draw;
        task->step = deck_gun_step;
        task->destructor = deck_gun_destroy;
        if (task->placement) {
            if (placement_get_state(task) & GUN_GONE) {
                work->tick = TICK_LIMIT;
            }
            if (placement_get_state(task) & LID_HIT) {
                HIT_STEPS(work) = DUST_STEPS;
            }
        }
        GUN_POSITION(work, &position);
        collision_init(task, supports, 2, COLLISION_OBJECT);
        model_object_create(task, NULL, NULL, lid_box, 1, 0);
        if (task->child) {
            task->child->work->pos = work->pos;
            task->child->work->ang = work->ang;
        }
        if (!work->tick) {
            weak_point_create_shaped(task, 0, &position, gun_target, 1);
        }
        LID_OPENING(task) = 0.0F;
        LID_SPEED(task) = 0.0F;
    }
}

static void deck_gun_destroy(struct task *task)
{
    struct task_work *work = task->work;
    if (task->placement) {
        placement_set_state(task, (work->tick ? GUN_GONE : 0) |
                                      (HIT_STEPS(work) ? LID_HIT : 0));
    }
    task->force.p = NULL;
    task->extra.p = NULL;
}

static void deck_gun_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    int slot;

    if (placement_try_leave(task)) {
        return;
    }
    if (!work->tick) {
        slot = player_near(&work->pos, AIM_RANGE);
        if (slot) {
            NJS_VECTOR direction;
            NJS_POINT3 position;
            Sint16 turn;
            Angle target;

            --slot;
            GUN_POSITION(work, &position);
            direction.x = player_work[slot]->pos.x - position.x;
            direction.y = player_work[slot]->pos.y - position.y;
            direction.z = player_work[slot]->pos.z - position.z;
            target =
                (Angle)(atan2f(direction.x, direction.z) * ANGLE_PER_RADIAN);
            turn = target - HEADING(work);
            if (turn > 0) {
                if (turn > MAX_GUN_TURN) {
                    turn = MAX_GUN_TURN;
                }
            } else if (turn < -MAX_GUN_TURN) {
                turn = -MAX_GUN_TURN;
            }
            HEADING(work) += turn;
        }
        if (work->submode) {
            Float floor;
            /* @hack spelling: one local 0 for both angles.x and angles.z,
             * where a 0 in each would do */
            Angle level;
            NJS_POINT3 position;
            struct angles angles;
            struct angles floor_angles;
            NJS_VECTOR velocity;

            GUN_POSITION(work, &position);
            level = 0;
            angles.x = level;
            angles.y = HEADING(work);
            angles.z = level;
            velocity.x = 0.0F;
            velocity.y = BARREL_RISE;
            velocity.z = 0.0F;
            work->tick = 1;
            floor = surface_probe_floor(position.x, position.y, position.z,
                                        &floor_angles);
            debris_launch_flags(&position, &angles, floor, &velocity,
                                deck_gun_barrel->model, &deck_gun_texlist,
                                DEBRIS_TRAIL | DEBRIS_LANDING_BURST |
                                    DEBRIS_VANISH);
            score_add(HIT_POINTS);
            sound_play_at(HIT_SOUND, NULL, 0, 0, &work->pos);
        }
    } else if (work->tick < TICK_LIMIT) {
        ++work->tick;
    }

    if (!HIT_STEPS(work)) {
        if (task->child) {
            child = task->child;
            do {
                if (collision_next_attack(child)) {
                    HIT_STEPS(work) = 1;
                    LID_SPEED(task) = LID_KICK;
                }
                child = child->next;
            } while (child != task->child);
        }
        if (HIT_STEPS(work)) {
            score_add(HIT_POINTS);
            sound_play_at(HIT_SOUND, NULL, 0, 0, &work->pos);
        }
    } else {
        LID_SPEED(task) = (LID_SPEED(task) * LID_DAMPING) + lid_acceleration;
        LID_OPENING(task) += LID_SPEED(task);
        if (LID_OPENING(task) > 1.0F) {
            LID_OPENING(task) = 1.0F;
            LID_SPEED(task) = -(njAbs(LID_SPEED(task)) * LID_BOUNCE);
        }
        if (LID_OPENING(task) < 0.0F) {
            LID_OPENING(task) = 0.0F;
            LID_SPEED(task) = njAbs(LID_SPEED(task)) * LID_BOUNCE;
        }
        if (HIT_STEPS(work) < DUST_STEPS) {
            if (rand() * RAND_SCALE < DUST_CHANCE) {
                NJS_VECTOR velocity;
                NJS_POINT3 position;

                position.x = DUST_X;
                position.y = DUST_Y;
                position.z = 0.0F;
                velocity.x = 0.0F;
                velocity.y = DUST_RISE;
                velocity.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, &position, &position);
                njPopMatrixEx();
                dust_steady_create(&position, &velocity,
                                   (rand() * RAND_SCALE * DUST_SCALE_RANGE) +
                                       DUST_SCALE,
                                   DUST_DEPTH_BIAS);
            }
            ++HIT_STEPS(work);
        }
    }
    collision_register(task);
}

static void deck_gun_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float opening;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&deck_gun_texlist);
    njCnkEasyDrawModel(deck_gun_base->model);
    if (work->tick < GUN_DRAW_STEPS) {
        njPushMatrixEx();
        njTranslateV(NULL, (NJS_VECTOR *)deck_gun_barrel->pos);
        njRotateY(NULL, HEADING(work) - work->ang.y);
        njCnkEasyDrawModel(deck_gun_barrel->model);
        njPopMatrix(1);
    }
    njTranslateV(NULL, (NJS_VECTOR *)deck_gun_lid->pos);
    if (HIT_STEPS(work)) {
        opening = LID_OPENING(task) * lid_degrees;
        njRotateZ(NULL, (Angle)(opening * ANGLE_PER_DEGREE));
    }
    njCnkEasyDrawModel(deck_gun_lid->model);
    njPopMatrix(1);
}
