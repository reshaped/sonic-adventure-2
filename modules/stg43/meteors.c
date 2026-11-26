/* Cosmic Wall's meteors: the flaming meteor, METEOBULLET in its object list,
 * and the rock, METEOBULLET2 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg43/meteors/text.inc"

#include "collision/collision.h"
#include "draw/light.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg43/objects.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a random Float from -1 up to 1 */
#define SIGNED_RANDOM() ((RANDOM_UNIT() - 0.5F) * 2.0F)
/* the meteor's flash, kept in task->unk_3c's word. a counted attack sets it
 * to 1, and each step keeps FLASH_FADE of it */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FLASH(task) (*(Float *)&(task)->unk_3c)
#define FLASH_FADE 0.98F
/* while the flash is above 0, a flaming meteor draws tinted from COLOR_NONE
 * toward FLASH_RED by it. a rock's light loses the flash times FLASH_SHADE of
 * its green and blue */
#define FLASH_RED 0xffff2020
#define FLASH_SHADE 0.5F
/* a rock whose scl.x is SMALL_ROCK or more is small, and draws
 * meteor_small_object. any other is large, and draws meteor_large_object */
#define SMALL_ROCK 1.0F
/* an attack counts only while the work's tick is 0. each attack sets the
 * tick to QUIET_STEPS. each step without one takes 1 from a tick above 0. so
 * the first attack counts, and a later one only after QUIET_STEPS steps in
 * a row without one. the work's submode counts the attacks that count. a
 * meteor breaks at the HIT_LIMIT-th, and a small rock at the
 * SMALL_HIT_LIMIT-th */
#define HIT_LIMIT 4
#define SMALL_HIT_LIMIT 2
#define QUIET_STEPS 3
/* a flaming meteor or a large rock breaks into PIECES pieces. the first
 * SHARDS are shards, and the rest chips. a small rock breaks into
 * SMALL_PIECES. the first SMALL_SHARDS are small shards, and the rest
 * shards */
#define PIECES 15
#define SHARDS 5
#define SMALL_PIECES 10
#define SMALL_SHARDS 3
/* a piece leaves along a random turn, at PIECE_SPEED plus up to
 * PIECE_SPEED_RANGE more. it then gains PIECE_LIFT up. it starts from half the
 * meteor's sphere's radius out to all of it. one roll sets both, so a faster
 * piece starts farther out. rand() times ANGLE_SPREAD runs from 0 up to two
 * turns. every piece falls to PIECE_FLOOR, far below the stage */
#define PIECE_SPEED 1.0F
#define PIECE_SPEED_RANGE 2.0F
#define PIECE_LIFT 0.5F
#define ANGLE_SPREAD 4.0F
#define PIECE_FLOOR (-100000.0F)
/* each step a flaming meteor sheds a spark with the chance SPARK_CHANCE. a
 * spark flies up to 1 either way along x and z. along y it flies from
 * SPARK_SINK down to 1 - SPARK_SINK up. its size is spark_size, grown by the
 * flash times SPARK_GROWTH of it, plus up to spark_variation more */
#define SPARK_CHANCE 0.2F
#define SPARK_SINK 0.2F
#define SPARK_GROWTH 0.3F
/* a meteor plays HIT_SOUND at its pos at each attack, counted or not. it
 * plays BREAK_SOUND there as it breaks, and NEAR_SOUND while a player lies
 * within HEAR_RANGE */
#define HIT_SOUND 11
#define BREAK_SOUND 12
#define SOUND_PRIORITY 0
#define NEAR_SOUND 10
#define HEAR_RANGE 300.0F
#define NEAR_SOUND_PRIORITY 1
#define NEAR_SOUND_VOLUME 0
#define NEAR_SOUND_STEPS 30
/* vibration_start's power and freq for player 0, where a player's touch
 * breaks the meteor */
#define TOUCH_POWER 6
#define TOUCH_FREQ 20
/* the points a broken meteor scores */
#define BREAK_POINTS 20

#include "stg43/meteor_models.h"

NJS_TEXLIST *cosmic_meteor_texlists[9] = {
#include "stg43/meteors/cosmic_meteor_texlists.inc"
};
static struct collision_shape flame_shape[1] = {
#include "stg43/meteors/flame_shape.inc"
};
static struct collision_shape rock_shape[1] = {
#include "stg43/meteors/rock_shape.inc"
};
static Uint32 unused_spark_color = {
#include "stg43/meteors/unused_spark_color.inc"
};
static Uint32 spark_color = {
#include "stg43/meteors/spark_color.inc"
};
static Float spark_size = {
#include "stg43/meteors/spark_size.inc"
};
static Float spark_variation = {
#include "stg43/meteors/spark_variation.inc"
};
/* a meteor's spin about x and about y, in degrees a step. its scl.y and scl.z
 * add to them */
static Float spin_x = {
#include "stg43/meteors/spin_x.inc"
};
static Float spin_y = {
#include "stg43/meteors/spin_y.inc"
};

static void flaming_meteor_destroy(struct task *task);
static void flaming_meteor_draw(struct task *task);
static void flaming_meteor_step(struct task *task);
static void rock_destroy(struct task *task);
static void rock_draw(struct task *task);
static void rock_step(struct task *task);

void cosmic_flaming_meteor_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = flaming_meteor_step;
    task->draw = flaming_meteor_draw;
    task->destructor = flaming_meteor_destroy;
    collision_init(task, flame_shape, 1, COLLISION_OBJECT);
}

static void flaming_meteor_destroy(struct task *task)
{
}

static void flaming_meteor_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: color signed, where the Uint32 that color_blend returns
     * would do */
    Sint32 color;

    njSetTexture(&meteor_flame_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (FLASH(task) > 0.0F) {
        color = color_blend(COLOR_NONE, FLASH_RED, FLASH(task));
        model_draw_tinted(meteor_flame_object->model, color);
    } else {
        njCnkEasyDrawModel(meteor_flame_object->model);
    }
    njPopMatrixEx();
}

/* @hack spelling: the step's count of attacks and fade of the flash as a
 * macro whose body is a bare block, where a function of its own would do */
#define METEOR_STEP_HITS(task, work)                                           \
    {                                                                          \
        if (collision_next_attack(task)) {                                     \
            sound_play_at(HIT_SOUND, NULL, SOUND_PRIORITY, SOUND_LOUDEST,      \
                          &(work)->pos);                                       \
            if (!(work)->tick) {                                               \
                (work)->submode++;                                             \
                FLASH(task) = 1.0F;                                            \
            }                                                                  \
            (work)->tick = QUIET_STEPS;                                        \
        } else if ((work)->tick) {                                             \
            (work)->tick--;                                                    \
        }                                                                      \
        FLASH(task) *= FLASH_FADE;                                             \
    }

static void flaming_meteor_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;
    NJS_POINT3 offset;
    NJS_POINT3 point;
    NJS_VECTOR direction;
    NJS_VECTOR velocity;
    struct angles rotation;
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *textures;
    struct particle *spark;
    Float roll;
    int i;
    /* @hack spelling: zero, a local for 0.0F, where 0.0F in place would do */
    Float zero;

    if (placement_try_leave(task)) {
        return;
    }
    zero = 0.0F;
    /* @hack redundant: a block no path reaches, which sets where SHC places
     * RAND_SCALE and rand among the function's constants */
    if (0) {
        if (roll == RAND_SCALE) {
        }
        if (rand) { /* NOLINT(clang-diagnostic-pointer-bool-conversion) */
        }
    }
    METEOR_STEP_HITS(task, work);
    player = NULL;
    if (work->submode >= HIT_LIMIT ||
        (player = collision_next_player(task)) != NULL) {
        if (player != NULL) {
            vibration_start(0, TOUCH_POWER, TOUCH_FREQ, VIBRATION_UNTIMED);
        }
        sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_LOUDEST,
                      &work->pos);
        offset.y = work->collision->shapes->radius;
        offset.x = zero;
        offset.z = zero;
        direction.x = zero;
        direction.z = zero;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        /* @hack redundant: a block no path reaches, which names
         * debris_launch_built_flags ahead of the loop that calls it */
        if (0) {
            if (debris_launch_built_flags) { /* NOLINT(clang-diagnostic-pointer-bool-conversion)
                                              */
            }
        }
        for (i = 0; i < PIECES; i++) {
            roll = RANDOM_UNIT();
            direction.y = (roll * PIECE_SPEED_RANGE) + PIECE_SPEED;
            offset.y = work->collision->shapes->radius * (roll + 1.0F) * 0.5F;
            rotation.y = 0;
            rotation.z = rand() * ANGLE_SPREAD;
            njRotateZ(NULL, rotation.z);
            rotation.x = rand() * ANGLE_SPREAD;
            njRotateX(NULL, rotation.x);
            njCalcPoint(NULL, &offset, &point);
            njCalcVector(NULL, &direction, &velocity);
            if (i < SHARDS) {
                model = meteor_flame_shard->model;
                textures = &meteor_flame_shard_texlist;
            } else {
                model = meteor_flame_chip->model;
                textures = &meteor_flame_chip_texlist;
            }
            velocity.y += PIECE_LIFT;
            debris_launch_built_flags(&point, &rotation, PIECE_FLOOR, &velocity,
                                      model, NULL, textures,
                                      DEBRIS_FADE_COLORS);
            matrix_load_parent();
        }
        /* @hack redundant: a block no path reaches, which compares direction.y
         * with ANGLE_SPREAD after the loop */
        if (0) {
            if (direction.y == ANGLE_SPREAD) {
            }
        }
        njPopMatrix(2);
        score_add(BREAK_POINTS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    collision_register(task);
    work->ang.x += (Angle)((spin_x + work->scl.y) * ANGLE_PER_DEGREE);
    work->ang.y += (Angle)((spin_y + work->scl.z) * ANGLE_PER_DEGREE);
    if (player_near(&work->pos, HEAR_RANGE)) {
        sound_play_at_for(NEAR_SOUND, task, NEAR_SOUND_PRIORITY,
                          NEAR_SOUND_VOLUME, NEAR_SOUND_STEPS, &work->pos);
    }
    if (RANDOM_UNIT() < SPARK_CHANCE) {
        point.x = SIGNED_RANDOM();
        point.y = RANDOM_UNIT() - SPARK_SINK;
        point.z = SIGNED_RANDOM();
        offset = work->pos;
        spark = burst_create_fast(
            &offset, &point,
            (spark_size * ((FLASH(task) * SPARK_GROWTH) + 1.0F)) +
                (rand() * spark_variation * RAND_SCALE),
            zero);
        if (spark != NULL) {
            spark->color = spark_color;
        }
    }
}

void cosmic_rock_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = rock_step;
    task->draw = rock_draw;
    task->destructor = rock_destroy;
    collision_init(task, rock_shape, 1, COLLISION_OBJECT);
    if (work->collision != NULL) {
        if (work->scl.x >= SMALL_ROCK) {
            work->collision->shapes->radius = meteor_small_object->model->r;
        } else {
            work->collision->shapes->radius = meteor_large_object->model->r;
        }
        collision_fit_radius(task);
    }
}

static void rock_destroy(struct task *task)
{
}

static void rock_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct light flash_light;
    /* @hack spelling: light, a pointer to flash_light, where flash_light's own
     * fields would do */
    struct light *light;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)METEORS_EDITOR_ROTSPD_X;
        (void)METEORS_EDITOR_ROTSPD_Y;
        (void)METEORS_EDITOR_MODEL;
    }
    light_apply(LIGHT_DEBRIS);
    if (FLASH(task) > 0.0F) {
        flash_light = lights[LIGHT_DEBRIS];
        flash_light.g *= 1.0F - (FLASH(task) * FLASH_SHADE);
        flash_light.b *= 1.0F - (FLASH(task) * FLASH_SHADE);
        light = &flash_light;
        njSetDirectLightIntensity(light->intensity, light->ambient);
        njCnkSetEasyLightIntensity(light->intensity, light->ambient);
        njCnkSetSimpleLightIntensity(light->intensity, light->ambient);
        njCnkSetEasyLightColor(light->r, light->g, light->b);
        njSetDirectLightColor(light->r, light->g, light->b);
        njCnkSetSimpleLightColor(light->r, light->g, light->b);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (work->scl.x >= SMALL_ROCK) {
        njSetTexture(&meteor_small_texlist);
        njCnkEasyDrawModel(meteor_small_object->model);
    } else {
        njSetTexture(&meteor_large_texlist);
        njCnkEasyDrawModel(meteor_large_object->model);
    }
    njPopMatrixEx();
    light_apply(LIGHT_LAND);
}

static void rock_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;
    NJS_POINT3 offset;
    NJS_POINT3 point;
    NJS_VECTOR direction;
    NJS_VECTOR velocity;
    struct angles rotation;
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *textures;
    Float roll;
    int count;
    int i;

    /* @hack spelling: the step's work in an else after the return, where the
     * work after the if would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        METEOR_STEP_HITS(task, work);
        player = NULL;
        if (work->submode >= HIT_LIMIT ||
            (work->scl.x >= SMALL_ROCK && work->submode >= SMALL_HIT_LIMIT) ||
            (player = collision_next_player(task)) != NULL) {
            if (player != NULL) {
                vibration_start(0, TOUCH_POWER, TOUCH_FREQ, VIBRATION_UNTIMED);
            }
            sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_LOUDEST,
                          &work->pos);
            offset.y = work->collision->shapes->radius;
            offset.x = 0.0F;
            offset.z = 0.0F;
            direction.x = 0.0F;
            direction.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njPushMatrixEx();
            if (work->scl.x >= SMALL_ROCK) {
                count = SMALL_PIECES;
            } else {
                count = PIECES;
            }
            for (i = 0; i < count; i++) {
                roll = RANDOM_UNIT();
                direction.y = (roll * PIECE_SPEED_RANGE) + PIECE_SPEED;
                offset.y =
                    work->collision->shapes->radius * (roll + 1.0F) * 0.5F;
                rotation.y = 0;
                rotation.z = rand() * ANGLE_SPREAD;
                njRotateZ(NULL, rotation.z);
                rotation.x = rand() * ANGLE_SPREAD;
                njRotateX(NULL, rotation.x);
                njCalcPoint(NULL, &offset, &point);
                njCalcVector(NULL, &direction, &velocity);
                if (work->scl.x < SMALL_ROCK) {
                    if (i < SHARDS) {
                        model = meteor_shard->model;
                        textures = &meteor_shard_texlist;
                    } else {
                        model = meteor_chip->model;
                        textures = &meteor_chip_texlist;
                    }
                } else {
                    if (i < SMALL_SHARDS) {
                        model = meteor_small_shard->model;
                        textures = &meteor_small_shard_texlist;
                    } else {
                        model = meteor_shard->model;
                        textures = &meteor_shard_texlist;
                    }
                }
                velocity.y += PIECE_LIFT;
                debris_launch_built_flags(&point, &rotation, PIECE_FLOOR,
                                          &velocity, model, NULL, textures,
                                          DEBRIS_LIGHT);
                matrix_load_parent();
            }
            njPopMatrix(2);
            score_add(BREAK_POINTS);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            collision_register(task);
            if (player_near(&work->pos, HEAR_RANGE)) {
                sound_play_at_for(NEAR_SOUND, task, NEAR_SOUND_PRIORITY,
                                  NEAR_SOUND_VOLUME, NEAR_SOUND_STEPS,
                                  &work->pos);
            }
            work->ang.x += (Angle)((spin_x + work->scl.y) * ANGLE_PER_DEGREE);
            work->ang.y += (Angle)((spin_y + work->scl.z) * ANGLE_PER_DEGREE);
        }
    }
}
