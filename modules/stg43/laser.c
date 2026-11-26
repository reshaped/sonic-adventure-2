/* the laser gun, LAZER in Cosmic Wall's object list. it raises its barrel,
 * fires a beam and lowers the barrel again. its placement keeps it broken */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg43/laser/text.inc"

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/enemy_laser.h"
#include "stg43/objects.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum laser_mode {
    LASER_WAIT,
    LASER_RAISE,
    LASER_AIM,
    LASER_RETRACT,
    LASER_WRECK
};

/* the barrel's rise above its rest, kept in task->unk_3c's word. it rises
 * RAISE_SPEED a step to BARREL_TRAVEL, and falls RETRACT_SPEED a step back to
 * 0. the barrel's rest lies BARREL_REST above the housing's origin */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define EXTENSION(task) (*(Float *)&(task)->unk_3c)
#define BARREL_TRAVEL 10.0F
#define RAISE_SPEED 0.9F
#define RETRACT_SPEED 1.5F
#define BARREL_REST 50.0F
/* the raised barrel fires once a wait passes AIM_STEPS steps. it starts to
 * fall once a second wait passes AIM_STEPS */
#define AIM_STEPS 10
/* the bits of ang.x that pitch the gun. the others, shifted up by AIM_SHIFT,
 * turn the shot about the gun's y */
#define PITCH_BITS 0xff00
#define AIM_BITS 0xff
#define AIM_SHIFT 8
/* the beam leaves MUZZLE_Y above the gun's pos, and scl.x plus MUZZLE_Z
 * ahead. its length and its speed are scl.x and scl.z plus BEAM_LEAST */
#define MUZZLE_Y 45.0F
#define MUZZLE_Z 15.0F
#define BEAM_LEAST 1.0F
/* the weak point's index, the bit of the work's submode that its loss sets */
#define BARREL_POINT 0
/* the broken barrel's pieces fly at BARREL_SPEED plus up to BARREL_SPREAD.
 * each takes BARREL_PUSH more along x and z, and BARREL_PUSH_Y along y. they
 * fall to BARREL_DROP below the gun's pos. a radius below 0.1 has
 * debris_break_built spin them as debris_break does */
#define BARREL_SPEED 0.7F
#define BARREL_SPREAD 1.0F
#define BARREL_PUSH 1.0F
#define BARREL_PUSH_Y 1.5F
#define BARREL_DROP 1.0F
#define BARREL_SPIN_RADIUS 0.0F
/* the gun breaks into SHARDS shards, which start SHARD_HEIGHT above its pos.
 * a shard's speed is up to half SHARD_SPREAD either way along x and z. along
 * y it is SHARD_RISE, give or take half SHARD_RISE_SPREAD. across x and z, a
 * shard starts SHARD_LEAD steps along its flight */
#define SHARDS 12
#define SHARD_HEIGHT 40.0F
#define SHARD_SPREAD 2.5F
#define SHARD_RISE 1.5F
#define SHARD_RISE_SPREAD 1.5F
#define SHARD_LEAD 10.0F
/* the spans of a shard's spin about x, y and z, in degrees a step. a spin is
 * up to half its span either way, converted to angle units by
 * ANGLE_PER_DEGREE */
#define SHARD_SPIN_X 0.5F
#define SHARD_SPIN_Y 0.3F
#define SHARD_SPIN_Z 0.1F
/* the points a broken gun scores */
#define BREAK_POINTS 20

static NJS_CNK_OBJECT turret_object;
static NJS_CNK_OBJECT turret_wreck;
static NJS_CNK_MODEL turret_shard[1];
static NJS_CNK_MODEL turret_barrel;
static NJS_TEXLIST turret_texlist;
static NJS_TEXLIST turret_shard_texlist;
static NJS_TEXLIST turret_wreck_texlist;
static NJS_TEXLIST turret_barrel_texlist;

static struct debris_built_part barrel_part[1];
static struct collision_shape barrel_target[1];

static void laser_destroy(struct task *task);
static void laser_step(struct task *task);
static void laser_draw(struct task *task);

void cosmic_laser_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = laser_draw;
    task->step = laser_step;
    task->destructor = laser_destroy;
    work->mode = LASER_WAIT;
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
    if (!work->submode) {
        weak_point_create_shaped(task, BARREL_POINT, &work->pos, barrel_target,
                                 1);
        if (task->child != NULL) {
            task->child->work->pos = work->pos;
            task->child->work->ang = work->ang;
        }
    } else {
        work->mode = LASER_WRECK;
    }
}

static void laser_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

static void laser_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *target;
    NJS_VECTOR velocity;
    int i;
    /* @hack redundant: radius, which nothing reads */
    Float radius; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (placement_try_leave(task)) {
        return;
    }
    if (work->submode) {
        if (work->mode != LASER_WRECK) {
            struct debris_spin spin;
            NJS_POINT3 pos;

            velocity.x = BARREL_PUSH;
            velocity.y = BARREL_PUSH_Y;
            velocity.z = BARREL_PUSH;
            debris_break_built(
                &work->pos, work->ang.y, barrel_part, 1, BARREL_SPEED,
                BARREL_SPREAD, work->pos.y - BARREL_DROP, &velocity,
                DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH,
                BARREL_SPIN_RADIUS);
            for (i = 0; i < SHARDS; i++) {
                pos = work->pos;
                pos.y += SHARD_HEIGHT;
                /* @bug mismatched-call: the spin is in angle units, where
                 * debris_launch_spin takes whole degrees a step. each shard
                 * spins up to about 45, 27 and 9 degrees a step about x, y
                 * and z. SHC unrolls the loop four times, and the first copy
                 * stores the spin at 0x8c5049e8, 0x8c504a00 and 0x8c504a18 */
                spin.x =
                    (RANDOM_UNIT() - 0.5F) * (SHARD_SPIN_X * ANGLE_PER_DEGREE);
                spin.y =
                    (RANDOM_UNIT() - 0.5F) * (SHARD_SPIN_Y * ANGLE_PER_DEGREE);
                spin.z =
                    (RANDOM_UNIT() - 0.5F) * (SHARD_SPIN_Z * ANGLE_PER_DEGREE);
                velocity.x = (RANDOM_UNIT() - 0.5F) * SHARD_SPREAD;
                velocity.y =
                    ((RANDOM_UNIT() - 0.5F) * SHARD_RISE_SPREAD) + SHARD_RISE;
                velocity.z = (RANDOM_UNIT() - 0.5F) * SHARD_SPREAD;
                pos.x += velocity.x * SHARD_LEAD;
                pos.z += velocity.z * SHARD_LEAD;
                debris_launch_spin(&pos, &work->ang, work->pos.y, &velocity,
                                   &spin, turret_shard, &turret_shard_texlist,
                                   DEBRIS_FADE_COLORS | DEBRIS_VANISH |
                                       DEBRIS_TRAIL);
            }
            radius = turret_shard->r;
        }
        /* @bug logic: the wreck scores BREAK_POINTS at each step in range,
         * not once as the gun breaks. the call at 0x8c504dba runs at each
         * step whose work's submode is set */
        score_add(BREAK_POINTS);
        work->mode = LASER_WRECK;
    }
    switch (work->mode) {
    case LASER_WAIT:
        work->timer++;
        if (work->timer > (Uint32)work->scl.y) {
            work->mode = LASER_RAISE;
            work->timer = 0;
        }
        break;
    case LASER_RAISE:
        EXTENSION(task) += RAISE_SPEED;
        if (EXTENSION(task) >= BARREL_TRAVEL) {
            EXTENSION(task) = BARREL_TRAVEL;
            work->mode = LASER_AIM;
            work->timer = 0;
        }
        break;
    case LASER_AIM:
        work->timer++;
        if (work->timer > AIM_STEPS) {
            NJS_POINT3 muzzle;

            work->timer = 0;
            work->mode = LASER_RETRACT;
            velocity.x = 0.0F;
            velocity.y = 0.0F;
            velocity.z = work->scl.z + BEAM_LEAST;
            muzzle.x = 0.0F;
            muzzle.y = MUZZLE_Y;
            muzzle.z = work->scl.x + MUZZLE_Z;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x & PITCH_BITS);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &muzzle, &muzzle);
            njRotateY(NULL, (work->ang.x & AIM_BITS) << AIM_SHIFT);
            njCalcVector(NULL, &velocity, &velocity);
            njPopMatrixEx();
            enemy_laser_fire_charged(task, work->scl.x + BEAM_LEAST, &velocity,
                                     &muzzle);
        }
        break;
    case LASER_RETRACT:
        if (work->timer > AIM_STEPS) {
            EXTENSION(task) -= RETRACT_SPEED;
            if (EXTENSION(task) <= 0.0F) {
                EXTENSION(task) = 0.0F;
                work->mode = LASER_WAIT;
                work->timer = 0;
            }
        } else {
            work->timer++;
        }
    }
    if (task->child != NULL) {
        /* @hack spelling: target set inside the assignment, where a
         * statement of its own would do */
        (target = task->child->work)->pos = work->pos;
        target->ang = work->ang;
    }
}

static void laser_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;
    /* @hack spelling: set_texture, draw_model and zero, locals for
     * njSetTexture, njCnkEasyDrawModel and 0.0F, where those in place would
     * do */
    Sint32 (*set_texture)(NJS_TEXLIST *);
    void (*draw_model)(NJS_CNK_MODEL *);
    Float zero;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)LASER_EDITOR_ANGLE_X_LASER_ROTATION;
        (void)LASER_EDITOR_ANGLE_Y;
        (void)LASER_EDITOR_ANGLE_Z;
        (void)LASER_EDITOR_LASER_LENGTH;
        (void)LASER_EDITOR_WAIT_TIME;
        (void)LASER_EDITOR_LASER_SPEED;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x & PITCH_BITS);
    njRotateY(NULL, work->ang.y);
    set_texture = njSetTexture;
    draw_model = njCnkEasyDrawModel;
    zero = 0.0F;
    if (work->mode != LASER_WRECK) {
        /* @hack redundant: a translation no path reaches */
        if (0) {
            njTranslate(NULL, 1.0F, 1.0F, 1.0F);
        }
        part = &turret_object;
        set_texture(&turret_texlist);
        draw_model(part->model);
        part = part->child;
        njTranslate(NULL, zero, EXTENSION(task) + BARREL_REST, zero);
        draw_model(part->model);
    } else {
        part = &turret_wreck;
        set_texture(&turret_wreck_texlist);
        draw_model(part->model);
    }
    njPopMatrixEx();
}

#include "stg43/foreground/laser.h"

NJS_TEXLIST *cosmic_laser_texlists[5] = {
#include "stg43/laser/cosmic_laser_texlists.inc"
};
static struct debris_built_part barrel_part[1] = {
#include "stg43/laser/barrel_part.inc"
};
static struct collision_shape barrel_target[1] = {
#include "stg43/laser/barrel_target.inc"
};
