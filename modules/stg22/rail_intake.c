#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg22/rail.h"
#include "stg22/rail_art.h"
#include "stg22/rail_intake.h"
#include "stg22/rail_sensor.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* an intake's modes, in its work's mode. an open intake's cover is broken */
enum intake_mode { INTAKE_WHOLE = 0, INTAKE_OPEN = 5 };
/* a whole intake arms its cover once its timer reaches ARM_STEPS. a player's
 * touch then breaks the cover, where player 0 moves faster than BREAK_SPEED
 * in PLAYER_MODE_SONIC_HOMING. the break scores BREAK_POINTS */
#define ARM_STEPS 100
#define BREAK_SPEED 0.5F
#define BREAK_POINTS 20
#define BREAK_SOUND 3
/* an open intake draws its cover while its timer is at most COVER_STEPS. the
 * cover's touch sphere, cover_shape, stands COVER_X along the intake's x and
 * COVER_Y above its pos */
#define COVER_STEPS 1
#define COVER_X 20.0F
#define COVER_Y 23.0F
/* the cover bursts into SHARD_COLUMNS by SHARD_ROWS shards. in the intake's
 * frame, the shard of column c and row r has its place at x SHARD_X. its y is
 * r * SHARD_HEIGHT + SHARD_Y_OFFSET, and its z c * SHARD_WIDTH -
 * SHARD_Z_OFFSET. column c's yaw is c * SHARD_YAW_STEP - SHARD_YAW_OFFSET. a
 * shard starts SHARD_OUT from its place along that yaw, and flies along it.
 * row r rises at (r - SHARD_MIDDLE_ROW) * SHARD_RISE, and up to
 * SHARD_RISE_RANGE more at random */
#define SHARD_COLUMNS 6
#define SHARD_ROWS 6
#define SHARD_X 5.0F
#define SHARD_HEIGHT 3.1F
#define SHARD_Y_OFFSET 15.0F
#define SHARD_WIDTH 5.0F
#define SHARD_Z_OFFSET 15.0F
#define SHARD_OUT 2.0F
#define SHARD_YAW_STEP 0x555
#define SHARD_YAW_OFFSET 0x800
#define SHARD_MIDDLE_ROW 2.0F
#define SHARD_RISE 0.5F
#define SHARD_RISE_RANGE 0.3F
#define SHARD_FLAGS (DEBRIS_SIMPLE | DEBRIS_FADE_COLORS)
/* past PULL_STEPS steps open, an intake pulls player 0 at PULL_SPEED toward
 * the point PULL_HEIGHT above its pos. the pull starts with PULL_SOUND, and
 * shakes player 0's pad at PULL_POWER and PULL_FREQ. player 0 boards the rail
 * within BOARD_REACH of that point. the cover mends after OPEN_STEPS. an open
 * intake's solid collides while a player lies within its radius plus
 * SOLID_MARGIN of its center */
#define PULL_STEPS 35
#define PULL_SPEED 0.8F
#define PULL_HEIGHT 15.0F
#define PULL_SOUND 4
#define PULL_POWER 5
#define PULL_FREQ 10
#define BOARD_REACH 5.0F
#define OPEN_STEPS 180
#define SOLID_MARGIN 30.0F
/* the intake draws while the view z of its pos lies from INTAKE_NEAR to
 * INTAKE_FAR */
#define INTAKE_NEAR (-60.0F)
#define INTAKE_FAR 600.0F
/* the glow blends from LAMP_DARK to LAMP_LIT by an absolute sine, whose angle
 * gains GLOW_RATE a step. the lamp blends by blink_level, of period
 * BLINK_PERIOD, rest BLINK_REST and fade BLINK_FADE */
#define GLOW_RATE 0x180
#define BLINK_PERIOD 35
#define BLINK_REST 20
#define BLINK_FADE 3
#define LAMP_DARK 0xff101010
#define LAMP_LIT 0xffffffff
/* the belt's texture scrolls SCROLL_STEP in v every SCROLL_PERIOD steps */
#define SCROLL_STEP 16
#define SCROLL_PERIOD 18

/* clang-format off */
static struct collision_shape cover_shape[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0, {-1, 0, 0}, 10, 0,
     0, 0, {0, 0, 0}},
};
/* clang-format on */

#define DRAW_RIB(node)                                                         \
    njTranslateEx((NJS_VECTOR *)(node)->pos);                                  \
    njCnkEasyDrawModel((node)->model);                                         \
    matrix_load_parent()

void gadget_rail_intake_draw(struct task *task)
{
    struct task_work *work = task->work;
    int color;
    NJS_POINT3 screen;

    njCalcPoint(NULL, &work->pos, &screen);
    if (screen.z < INTAKE_NEAR || screen.z > INTAKE_FAR) {
        return;
    }
    njSetTexture(&gadget_intake_texlist);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njCnkEasyDrawModel(gadget_intake_base->model);
    color = color_blend(LAMP_DARK, LAMP_LIT,
                        njAbs(njSin(game_state.steps * GLOW_RATE)));
    model_draw_tinted(gadget_intake_glow->model, color);
    color = color_blend(LAMP_DARK, LAMP_LIT,
                        blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
    model_draw_tinted(gadget_intake_lamp->model, color);
    if (work->mode != INTAKE_OPEN || work->timer <= COVER_STEPS) {
        njTranslateEx((NJS_VECTOR *)gadget_intake_cover->pos);
        njCnkSimpleDrawModel(gadget_intake_cover->model);
        matrix_load_parent();
    }
    DRAW_RIB(gadget_intake_rib_c);
    DRAW_RIB(gadget_intake_rib_b);
    DRAW_RIB(gadget_intake_rib_a);
    texture_anim_scroll(gadget_intake_belt->model, &gadget_intake_scroll, 0,
                        ((game_state.steps / SCROLL_PERIOD) * SCROLL_STEP) %
                            SCROLL_WRAP);
    DRAW_RIB(gadget_intake_belt);
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* a shard's speed along each axis takes its own random factor, from
 * SHARD_FACTOR up to SHARD_FACTOR + SHARD_FACTOR_RANGE */
#define SHARD_FACTOR 0.8F
#define SHARD_FACTOR_RANGE 1.5F
#define SHARD_SPEED()                                                          \
    (((Float)rand() * (SHARD_FACTOR_RANGE * RAND_SCALE)) + SHARD_FACTOR)

void gadget_rail_intake_step(struct task *task)
{
    struct task_work *work = task->work;
#ifdef NONMATCHING
#else
    Float *shard_axis;
    Float *intake_axis;
    Float *player_axis;
    Float *delta_axis;
#endif
    Float length;
    int column;
    int row;
    Angle yaw;

    switch (work->mode) {
    case INTAKE_WHOLE:
        if (work->timer < ARM_STEPS) {
            work->timer++;
            break;
        }
        if (task->child) {
            task->child->work->collision->flags |= COLLISION_TARGET;
            task->child->work->collision->shapes->flags &= ~SHAPE_OFF;
        } else {
            model_object_create(task, NULL, NULL, cover_shape, 1, 0);
#ifdef NONMATCHING
            /* this side destroys a cover that collision_init gave no record.
             * the matching side keeps it, and reads its record through null
             * the next step */
            if (task->child != NULL && task->child->work->collision == NULL) {
                task_destroy(task->child);
            }
#endif
            if (task->child) {
                NJS_POINT3 sensor;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateEx(&work->pos);
                njRotateY(NULL, work->ang.y);
                sensor.x = COVER_X;
                sensor.y = COVER_Y;
                sensor.z = 0.0F;
                njCalcPoint(NULL, &sensor, &task->child->work->pos);
                njPopMatrixEx();
            }
        }
        work->collision->shapes[0].flags &= ~SHAPE_OFF;
        work->collision->shapes[1].flags &= ~SHAPE_OFF;
        if (task->child && collision_next_player(task->child) &&
            njScalor(&player_motion[0]->spd) > BREAK_SPEED &&
            player_work[0]->mode == PLAYER_MODE_SONIC_HOMING) {
            /* @hack redundant: scale, set and never read */
            NJS_VECTOR scale;
            NJS_POINT3 position;
            NJS_VECTOR velocity;
            struct angles ang;

            score_add(BREAK_POINTS);
            sound_play(BREAK_SOUND, NULL, 0, 0);
            work->mode = INTAKE_OPEN;
            work->timer = 0;
            scale.x = 0.8F;
            scale.y = 0.8F;
            scale.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            for (column = 0; column < SHARD_COLUMNS; column++) {
                yaw = (column * SHARD_YAW_STEP) - SHARD_YAW_OFFSET;
                for (row = 0; row < SHARD_ROWS; row++) {
                    velocity.x = njCos(yaw);
                    velocity.z = njSin(yaw);
                    velocity.y =
                        ((row - SHARD_MIDDLE_ROW) * SHARD_RISE) +
                        ((Float)rand() * (SHARD_RISE_RANGE * RAND_SCALE));
                    position.z = (column * SHARD_WIDTH) +
                                 (velocity.z * SHARD_OUT) - SHARD_Z_OFFSET;
                    position.y = (row * SHARD_HEIGHT) + SHARD_Y_OFFSET;
                    position.x = (velocity.x * SHARD_OUT) + SHARD_X;
                    ang.x = 0;
                    ang.y = work->ang.y + yaw;
                    ang.z = 0;
                    njCalcPoint(NULL, &position, &position);
                    njCalcVector(NULL, &velocity, &velocity);
#ifdef NONMATCHING
                    /* the plain form the hack tag names */
                    velocity.x *= SHARD_SPEED();
                    velocity.y *= SHARD_SPEED();
                    velocity.z *= SHARD_SPEED();
#else
                    /* @hack spelling: the scaling walked through a pointer to
                     * velocity's fields, where the fields by name would do */
                    shard_axis = &velocity.x;
                    *shard_axis++ *= SHARD_SPEED();
                    *shard_axis++ *= SHARD_SPEED();
                    *shard_axis *= SHARD_SPEED();
#endif
                    if ((column + row) & 1) {
                        debris_launch_built_flags(
                            &position, &ang, work->pos.y, &velocity,
                            gadget_intake_shard_a, NULL,
                            &gadget_intake_shard_a_texlist, SHARD_FLAGS);
                    } else {
                        debris_launch_built_flags(
                            &position, &ang, work->pos.y, &velocity,
                            gadget_intake_shard_b, NULL,
                            &gadget_intake_shard_b_texlist, SHARD_FLAGS);
                    }
                }
            }
            (void)scale;
            njPopMatrixEx();
            enemy_bounce_player(0);
        }
        break;
    case INTAKE_OPEN:
        if (task->child) {
            task->child->work->collision->flags &= ~COLLISION_TARGET;
            task->child->work->collision->shapes->flags |= SHAPE_OFF;
        }
        work->collision->shapes[0].flags |= SHAPE_OFF;
        work->collision->shapes[1].flags |= SHAPE_OFF;
        if (work->timer++ > OPEN_STEPS) {
            work->mode = INTAKE_WHOLE;
            work->timer = 0;
        }
        if (player_work[0]) {
            NJS_VECTOR delta;
            /* @hack redundant: horizontal, delta without its y, set and never
             * read */
            NJS_VECTOR horizontal;

#ifdef NONMATCHING
            /* the plain form the hack tag names */
            delta.x = work->pos.x - player_work[0]->pos.x;
            delta.y = work->pos.y - player_work[0]->pos.y;
            delta.z = work->pos.z - player_work[0]->pos.z;
#else
            /* @hack spelling: the difference, and its scaling below, walked
             * through pointers to the vectors' fields, where the fields by
             * name would do */
            delta_axis = &delta.x;
            intake_axis = &work->pos.x;
            player_axis = &player_work[0]->pos.x;
            *delta_axis++ = *intake_axis++ - *player_axis++;
            *delta_axis++ = *intake_axis++ - *player_axis++;
            *delta_axis = *intake_axis - *player_axis;
#endif
            delta.y += PULL_HEIGHT;
            horizontal = delta;
            horizontal.y = 0.0F;
            length = njScalor(&delta);
            if (length < BOARD_REACH) {
                gadget_rail_board(task->parent->parent, 0);
                work->mode = INTAKE_WHOLE;
                work->timer = 0;
                break;
            }
            if (work->timer == PULL_STEPS) {
                sound_play(PULL_SOUND, NULL, 0, 0);
                vibration_start(0, PULL_POWER, PULL_FREQ, VIBRATION_UNTIMED);
            }
            if (work->timer > PULL_STEPS) {
                length = PULL_SPEED / length;
#ifdef NONMATCHING
                /* the plain form of the scaling that the hack tag at delta's
                 * difference above names */
                delta.x *= length;
                delta.y *= length;
                delta.z *= length;
#else
                delta_axis = &delta.x;
                *delta_axis++ *= length;
                *delta_axis++ *= length;
                *delta_axis *= length;
#endif
                player_updraft(0, delta.x, delta.y, delta.z);
            }
            (void)horizontal;
        }
        break;
    }
    if (work->mode == INTAKE_OPEN &&
        player_near(&RAIL_SENSOR(task)->center,
                    RAIL_SENSOR(task)->object.model->r + SOLID_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
}
