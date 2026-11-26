/* FIREBALL, a burning rock that a touch or its timer breaks into pieces.
 * stg40/meteor.c compiles Final Chase's, which spins and plays BREAK_SOUND
 * once as it breaks. stg44/fireball.c defines ORBIT_FIREBALL and compiles
 * Mad Space's. that one plays LOOP_SOUND while a player is near, and
 * BREAK_SOUND for each piece.
 *
 * the includer defines FIREBALL_INIT as the init's name. it defines the
 * rock's FIREBALL_OBJECT and FIREBALL_TEXLIST, and its pieces' ones as
 * FIREBALL_SHARD_ and FIREBALL_PIECE_ names. it defines fireball_shape, and
 * fireball_spark_color, _size and _size_range for its sparks. it defines the
 * BLINK_ names and SPARK_CHANCE. it defines the break's BREAK_WAIT,
 * BREAK_NEAR, BREAK_SOUND, PIECES, SHARDS, PIECE_FLOOR and PIECE_LIFT. Final
 * Chase's adds BREAK_SOUND_VOLUME, spin_x and spin_y. Mad Space's adds
 * LOOP_SOUND and SOUND_RANGE.
 *
 * the init's collision_init must find room for the collision record, which
 * the break reads unchecked */
#ifndef SHARED_BREAKING_FIREBALL_H
#define SHARED_BREAKING_FIREBALL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

#ifdef ORBIT_FIREBALL
/* the steps LOOP_SOUND plays for at each play */
#define LOOP_SOUND_STEPS 30
#endif
/* a piece starts from half the rock's radius out to all of it. it leaves at
 * PIECE_LEAST_SPEED and up to PIECE_SPEED_RANGE more, along a random turn.
 * rand()'s range times ANGLE_SPREAD spans two turns */
#define PIECE_LEAST_SPEED 1.0F
#define PIECE_SPEED_RANGE 2.0F
#define ANGLE_SPREAD 4.0F
/* a spark flies up to 1 either way along x and z. along y it flies from
 * SPARK_SINK down to 1 - SPARK_SINK up */
#define SIGNED_RANDOM() ((RANDOM_UNIT() - 0.5F) * 2.0F)
#define SPARK_SINK 0.2F

static void fireball_destroy(struct task *task);
static void fireball_draw(struct task *task);
static void fireball_step(struct task *task);
void FIREBALL_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->step = fireball_step;
        task->draw = fireball_draw;
        task->destructor = fireball_destroy;
        collision_init(task, &fireball_shape, 1, COLLISION_OBJECT);
    }
}

static void fireball_destroy(struct task *task)
{
}

static void fireball_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: a point for the call no path reaches */
    NJS_POINT3 point;
    /* @hack spelling: the blend held in color, where the call could take the
     * blend itself */
    NJS_COLOR color;
    Float blink;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and its call */
    if (0) {
#ifdef ORBIT_FIREBALL
        (void)"<- ANGLE Y";
        (void)"<- START RANGE";
#else
        (void)"<- START RANGE";
        (void)"<- ROTSPD X";
        (void)"<- ROTSPD Y";
        (void)"<- MODEL(0-3)";
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
#endif
        njCalcPoint(NULL, &point, &point);
    }
    njSetTexture(&FIREBALL_TEXLIST);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (work->timer) {
        blink = njSin(work->timer * BLINK_RATE);
        if (blink < 0.0F) {
            blink = 0.0F;
        }
        color.color = color_blend(BLINK_COLOR, BLINK_RED, blink);
        model_draw_tinted(FIREBALL_OBJECT->model, color.color);
    } else {
        njCnkEasyDrawModel(FIREBALL_OBJECT->model);
    }
    njPopMatrixEx();
}

#ifdef ORBIT_FIREBALL
/* @hack spelling: the near sound played inside the break's test, where a
 * statement before it would do */
#define FIREBALL_BREAKS(task, work)                                            \
    ((player_near(&(work)->pos, SOUND_RANGE)                                   \
          ? sound_play_at_for(LOOP_SOUND, (work), 1, SOUND_LOUDEST,            \
                              LOOP_SOUND_STEPS, &(work)->pos)                  \
          : 0),                                                                \
     (collision_next_player(task) || (work)->timer > BREAK_WAIT))
#else
#define FIREBALL_BREAKS(task, work)                                            \
    (collision_next_player(task) || (work)->timer > BREAK_WAIT)
#endif

/* the work's timer is 0 until a player comes within scl.x + BREAK_NEAR. it
 * then counts the steps from 1 */
static void fireball_step(struct task *task)
{
    struct task_work *work = task->work;
    struct particle *spark;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (FIREBALL_BREAKS(task, work)) {
        NJS_POINT3 pos;
        NJS_POINT3 world_pos;
        NJS_VECTOR spd;
        NJS_VECTOR world_spd;
        struct angles ang;
        NJS_CNK_MODEL *model;
        NJS_TEXLIST *texlist;
        Float roll;

#ifndef ORBIT_FIREBALL
        sound_play_at(BREAK_SOUND, NULL, 0, BREAK_SOUND_VOLUME, &work->pos);
#endif
        work->submode = 1;
        /* @hack redundant: pos.y set before the loop, which sets it again
         * before any read */
        pos.y = work->collision->shapes->radius;
        pos.x = 0.0F;
        pos.z = 0.0F;
        spd.x = 0.0F;
        spd.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        for (i = 0; i < PIECES; i++) {
            roll = RANDOM_UNIT();
            spd.y = (roll * PIECE_SPEED_RANGE) + PIECE_LEAST_SPEED;
            pos.y = work->collision->shapes->radius * (roll + 1.0F) * 0.5F;
            ang.y = 0;
            ang.z = (Float)rand() * ANGLE_SPREAD;
            njRotateZ(NULL, ang.z);
            ang.x = (Float)rand() * ANGLE_SPREAD;
            njRotateX(NULL, ang.x);
            njCalcPoint(NULL, &pos, &world_pos);
            njCalcVector(NULL, &spd, &world_spd);
            if (i < SHARDS) {
                model = FIREBALL_SHARD_OBJECT->model;
                texlist = &FIREBALL_SHARD_TEXLIST;
            } else {
                model = FIREBALL_PIECE_OBJECT->model;
                texlist = &FIREBALL_PIECE_TEXLIST;
            }
            world_spd.y += PIECE_LIFT;
            debris_launch_built_flags(&world_pos, &ang, PIECE_FLOOR, &world_spd,
                                      model, NULL, texlist, DEBRIS_FADE_COLORS);
#ifdef ORBIT_FIREBALL
            sound_play_at(BREAK_SOUND, NULL, 0, 0, &work->pos);
#endif
            matrix_load_parent();
        }
        njPopMatrix(2);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
    }
    if (work->timer) {
        work->timer++;
    } else if (player_near(&work->pos, work->scl.x + BREAK_NEAR)) {
        work->timer = 1;
    }
#ifndef ORBIT_FIREBALL
    work->ang.x += DEGREES(spin_x);
    work->ang.y += DEGREES(spin_y);
#endif
    if (RANDOM_UNIT() < SPARK_CHANCE) {
        NJS_POINT3 pos;
        NJS_VECTOR spd;

        spd.x = SIGNED_RANDOM();
        spd.y = RANDOM_UNIT() - SPARK_SINK;
        spd.z = SIGNED_RANDOM();
        pos = work->pos;
        spark = burst_create_fast(
            &pos, &spd,
            fireball_spark_size +
                ((Float)rand() * fireball_spark_size_range * RAND_SCALE),
            0.0F);
        if (spark != NULL) {
            spark->color = fireball_spark_color;
        }
    }
}

#endif
