/* Meteor Herd's meteors: the rocks, METEO in its object list, and the meteors
 * and fireballs that meteor_herd_step throws */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/earth_shake.h"
#include "shared/item_box.h"
#include "sound/sound.h"
#include "stg32/meteors.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the rock scl.x picks, ROCK_A or ROCK_B: the low bit of its whole part,
 * taken unsigned */
#define ROCK_KIND(work) ((Uint32)(work)->scl.x & 1)
/* meteor_rock_init and meteor_fireball_init set their sphere's radius to
 * SHAPE_SCALE times their model's radius. that radius replaces the 35.0F that
 * rock_shape and fireball_shape hold */
#define SHAPE_SCALE 0.7F
#define FIREBALL_ATTACK                                                        \
    (SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |               \
     SHAPE_HURTS_PLAYERS)
#define ROCK_SHAPES 1
#define FIREBALL_SHAPES 1
/* a rock breaks into ROCK_FRAGMENTS chips, and a fireball into
 * FIREBALL_FRAGMENTS. a fireball's first CHIP_FRAGMENTS are chips, and the
 * rest shards. a rock's first CHIP_FRAGMENTS are the plain chip,
 * rock_chips[0], and the rest its own, rock_chips[1 + ROCK_KIND] */
#define ROCK_FRAGMENTS 30
#define FIREBALL_FRAGMENTS 15
#define CHIP_FRAGMENTS 5
/* every fragment falls to FRAGMENT_FLOOR, far below the stage */
#define FRAGMENT_FLOOR (-100000.0F)
/* a fragment flies out at its kind's FRAGMENT_SPEED, plus up to
 * FRAGMENT_SPEED_RANGE at random, in a random direction. it starts from half
 * its body's sphere radius out to the whole radius. a faster fragment starts
 * farther out. a fireball's fragments then gain FRAGMENT_LIFT up */
#define ROCK_FRAGMENT_SPEED 2.0F
#define FIREBALL_FRAGMENT_SPEED 1.0F
#define FRAGMENT_SPEED_RANGE 2.0F
#define FRAGMENT_LIFT 0.5F
/* a rock's explosion grows to EXPLOSION_SCALE times its sphere's radius */
#define EXPLOSION_SCALE 5.0F
#define ROCK_BREAK_SOUND 8
/* the chance at each step that a falling meteor puffs smoke. a puff starts
 * SMOKE_LEAD of a step ahead of the meteor */
#define FALL_SMOKE_CHANCE 0.8F
#define SMOKE_LEAD 0.5F

/* a fireball flies until it comes within a step of its target, or touches
 * another record. it then lands on the target. its timer counts its steps
 * since it landed. it blasts at BLAST_STEP where a player lies within
 * BLAST_RANGE. it breaks apart at BREAK_STEP, and ends at IMPACT_STEPS. a
 * fireball whose work's submode isn't 0 does all of it as it lands */
#define BLAST_STEP 55
#define BLAST_RANGE 460.0F
#define BREAK_STEP 58
#define IMPACT_STEPS 60
/* a landing shakes the cameras within SHAKE_RANGE. it vibrates the pad of
 * each player within VIBRATION_RANGE */
#define SHAKE_RANGE 100.0F
#define SHAKE_POWER 3.0F
#define VIBRATION_RANGE 150.0F
#define VIBRATION_POWER 5
#define VIBRATION_FREQ 15
/* a landing sets bounce_speed to BOUNCE_LIFT. where bounce passes below 0, the
 * fireball rebounds. bounce becomes 0, and bounce_speed turns positive at
 * BOUNCE_REBOUND of its size. each step takes BOUNCE_GRAVITY from
 * bounce_speed, and then multiplies it by BOUNCE_DRAG */
#define BOUNCE_LIFT 3.0F
#define BOUNCE_REBOUND 0.5F
#define BOUNCE_GRAVITY 0.5F
#define BOUNCE_DRAG 0.96F
/* a fireball in flight spins FLIGHT_SPIN_X degrees a step about x, and
 * FLIGHT_SPIN_Y about y. once it lands, each step keeps SPIN_DRAG of its
 * spin */
#define FLIGHT_SPIN_X 1.2F
#define FLIGHT_SPIN_Y 1.5F
#define SPIN_DRAG 0.99F
/* the chance at each step that a fireball puffs smoke, in flight and once it
 * lands */
#define FLIGHT_SMOKE_CHANCE 0.5F
#define LANDED_SMOKE_CHANCE 0.1F
/* NEAR_SOUND plays while a player lies within HEAR_RANGE of a fireball. every
 * fireball plays it in one slot, whose key is the address of fireball_step.
 * FLIGHT_SOUND plays from each fireball in flight */
#define HEAR_RANGE 300.0F
#define NEAR_SOUND 0
#define FLIGHT_SOUND 1
#define IMPACT_SOUND 2
#define SHATTER_SOUND 3
#define SOUND_PRIORITY 0
#define FIREBALL_SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define NEAR_SOUND_STEPS 120
#define FLIGHT_SOUND_STEPS 15

/* the indexes of models[] */
enum meteor_model {
    ROCK_A,
    ROCK_B,
    ROCK_CHIP,
    ROCK_A_CHIP,
    ROCK_B_CHIP,
    FALLING,
    FIREBALL_BODY,
    FIREBALL_SHARD,
    FIREBALL_CHIP,
    METEOR_MODELS
};

/* a fireball's record, at task->motion */
struct fireball_motion {
    NJS_VECTOR velocity; /* its last step in flight */
    /* how far it lies from its target as it bounces, back along velocity */
    Float bounce;
    Float bounce_speed; /* what each step adds to bounce */
    /* its spin about x and about y, in degrees a step */
    Float spin_x;
    Float spin_y;
};
#define FIREBALL(task) ((struct fireball_motion *)(task)->motion.p)

#include "stg32/meteors/text.inc"
#include "stg32/meteor_models.h"

NJS_CNK_OBJECT meteor_fireball_shadow[1] = {
#include "stg32/meteors/meteor_fireball_shadow.inc"
};

NJS_TEXLIST *meteor_textures[10] = {
#include "stg32/meteors/meteor_textures.inc"
};
/* the rocks, by ROCK_KIND */
static NJS_CNK_OBJECT *rock_objects[2] = {
#include "stg32/meteors/rock_objects.inc"
};
static NJS_TEXLIST *rock_chip_textures[3] = {
#include "stg32/meteors/rock_chip_textures.inc"
};
static NJS_CNK_OBJECT *rock_chips[3] = {
#include "stg32/meteors/rock_chips.inc"
};
/* clang-format off */
static struct collision_shape rock_shape[ROCK_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 35.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* meteor_models_build built every model */
static Bool models_built = FALSE;
static struct model_build models[9] = {
#include "stg32/meteors/models.inc"
};

/* the spin of the rocks and the falling meteors about x and about y, in
 * degrees a step */
static Float rock_spin_x = 0.36F;
static Float rock_spin_y = 0.450000018F;
/* the scale of a falling meteor's smoke is falling_smoke_size, and up to
 * falling_smoke_range more at random. a puff's color is fresh_smoke_color
 * through the first half of the meteor's life. it shades to old_smoke_color
 * over the second */
static Float falling_smoke_size = 38.0F;
static Float falling_smoke_range = 70.0F;
Sint32 falling_meteor_count = 0;
static Uint32 fresh_smoke_color = 0xffff7000;
static Uint32 old_smoke_color = 0xffff2030;
Sint32 fireball_count = 0;
/* the scale of a fireball's smoke is fireball_smoke_size, and up to
 * fireball_smoke_range more at random. its color is fireball_smoke_color */
static Float fireball_smoke_size = 20.0F;
static Float fireball_smoke_range = 30.0F;
/* clang-format off */
static struct collision_shape fireball_shape[FIREBALL_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIREBALL_ATTACK,
     SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 35.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a landed fireball shades from impact_start_color to impact_end_color over
 * IMPACT_STEPS */
static Uint32 impact_start_color = 0xffffffff;
static Uint32 impact_end_color = 0xffff1010;
static Uint32 dat_8c5897b4 = 0xff0070ff;
static Uint32 fireball_smoke_color = 0xff3020ff;
/* how far a fireball flies a step */
static Float fireball_speed = 5.0F;

static void rock_destroy(struct task *task);
static void rock_draw(struct task *task);
static void rock_step(struct task *task);
static void fall_destroy(struct task *task);
static void fall_draw(struct task *task);
static void fall_step(struct task *task);
static void fireball_destroy(struct task *task);
static void fireball_draw(struct task *task);
static void fireball_step(struct task *task);

Uint8 *meteor_models_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, models, METEOR_MODELS) == METEOR_MODELS) {
        models_built = TRUE;
    }
    return buffer;
}

void meteor_rock_init(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 kind;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = rock_step;
    task->draw = rock_draw;
    task->destructor = rock_destroy;
    kind = ROCK_KIND(work);
    collision_init(task, rock_shape, ROCK_SHAPES, COLLISION_OBJECT);
    if (work->collision != NULL) {
        work->collision->shapes->radius =
            rock_objects[kind]->model->r * SHAPE_SCALE;
        collision_fit_radius(task);
    }
    work->ang.x = RANDOM_TURN();
    work->ang.y = RANDOM_TURN();
    work->ang.z = RANDOM_TURN();
}

static void rock_destroy(struct task *task)
{
    (void)task;
}

static void rock_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 kind;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD X";
        (void)"<- ANGSPD Y";
        (void)"<- ANGLE Z";
        (void)"<- MODEL(0-1)";
        (void)"<- ROTSPD X";
        (void)"<- ROTSPD Y";
    }
    light_apply(LIGHT_DEBRIS);
    kind = ROCK_KIND(work);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njSetTexture(meteor_textures[kind]);
    if (models_built) {
        njDirectDrawObject(models[kind].built);
    } else {
        njCnkEasyDrawModel(rock_objects[kind]->model);
    }
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void rock_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 offset;
    NJS_POINT3 pos;
    NJS_VECTOR impulse;
    NJS_VECTOR velocity;
    struct angles rotation;
    Float scale;
    int i;
    Uint32 kind;
    Uint32 chip;

    /* @hack spelling: the touch test as an else of the leave test, where an if
     * of its own after the return would do */
    if (placement_try_leave(task)) {
        return;
    } else if (work->collision != NULL && collision_next_player(task) != NULL) {
        sound_play(ROCK_BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
        kind = ROCK_KIND(work);
        offset.y = work->collision->shapes->radius;
        offset.x = 0.0F;
        offset.z = 0.0F;
        impulse.x = 0.0F;
        impulse.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        for (i = 0; i < ROCK_FRAGMENTS; i++) {
            scale = RANDOM_UNIT();
            impulse.y = (scale * FRAGMENT_SPEED_RANGE) + ROCK_FRAGMENT_SPEED;
            offset.y = work->collision->shapes->radius * (scale + 1.0F) * 0.5F;
            rotation.y = 0;
            rotation.z = RANDOM_TWO_TURNS();
            njRotateZ(NULL, rotation.z);
            rotation.x = RANDOM_TWO_TURNS();
            njRotateX(NULL, rotation.x);
            njCalcPoint(NULL, &offset, &pos);
            njCalcVector(NULL, &impulse, &velocity);
            if (i < CHIP_FRAGMENTS) {
                chip = 0;
            } else {
                chip = kind + 1;
            }
            debris_launch_built_flags(&pos, &rotation, FRAGMENT_FLOOR,
                                      &velocity, rock_chips[chip]->model,
                                      models[ROCK_CHIP + chip].built,
                                      rock_chip_textures[chip], DEBRIS_LIGHT);
            matrix_load_parent();
        }
        njPopMatrix(2);
        explosion_create_sized(&work->pos, work->collision->shapes->radius *
                                               EXPLOSION_SCALE);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
        work->ang.x += (Angle)((rock_spin_x + work->scl.y) * ANGLE_PER_DEGREE);
        work->ang.y += (Angle)((rock_spin_y + work->scl.z) * ANGLE_PER_DEGREE);
    }
}

void meteor_fall_init(struct task *task)
{
    falling_meteor_count++;
    task->step = fall_step;
    task->draw = fall_draw;
    task->destructor = fall_destroy;
}

static void fall_destroy(struct task *task)
{
    (void)task;
    falling_meteor_count--;
}

static void fall_draw(struct task *task)
{
    struct task_work *work = task->work;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njSetTexture(&herd_falling_meteor_texlist);
    if (models_built) {
        njDirectDrawObject(models[FALLING].built);
    } else {
        njCnkEasyDrawModel(herd_falling_meteor_object->model);
    }
    njPopMatrixEx();
}

/* @hack spelling: the add and the scale as walks of three Floats, where three
 * statements on the fields would do */
#define MOVE(pos, delta)                                                       \
    do {                                                                       \
        Float *dest = &(pos)->x;                                               \
        const Float *src = &(delta)->x;                                        \
        *dest++ += *src++;                                                     \
        *dest++ += *src++;                                                     \
        *dest += *src;                                                         \
    } while (0)
#define SCALE(vector, factor)                                                  \
    do {                                                                       \
        Float *coord = &(vector)->x;                                           \
        *coord++ *= (factor);                                                  \
        *coord++ *= (factor);                                                  \
        *coord *= (factor);                                                    \
    } while (0)

static void fall_step(struct task *task)
{
    struct task_work *work = task->work;
    struct particle *smoke;
    NJS_POINT3 pos;
    Float age;
    Float fade;

    MOVE(&work->pos, &work->scl);
    if (RANDOM_UNIT() < FALL_SMOKE_CHANCE) {
        pos.x = work->pos.x + (SMOKE_LEAD * work->scl.x);
        pos.y = work->pos.y + (SMOKE_LEAD * work->scl.y);
        pos.z = work->pos.z + (SMOKE_LEAD * work->scl.z);
        smoke = burst_create_fast(&pos, &work->scl,
                                  (rand() * falling_smoke_range * RAND_SCALE) +
                                      falling_smoke_size,
                                  0.0F);
        if (smoke != NULL) {
            age = (Float)work->timer / (Float)METEOR_LIFE(work);
            if (age > 0.5F) {
                fade = (age - 0.5F) * 2.0F;
            } else {
                fade = 0.0F;
            }
            smoke->color =
                color_blend(fresh_smoke_color, old_smoke_color, fade);
        }
    }
    work->ang.x += (Angle)(rock_spin_x * ANGLE_PER_DEGREE);
    work->ang.y += (Angle)(rock_spin_y * ANGLE_PER_DEGREE);
    if (work->timer++ > METEOR_LIFE(work)) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    }
}

void meteor_fireball_init(struct task *task)
{
    struct task_work *work = task->work;
    task->motion.p = syCalloc(1, sizeof(struct fireball_motion));
    fireball_count++;
    task->step = fireball_step;
    task->draw = fireball_draw;
    task->destructor = fireball_destroy;
    collision_init(task, fireball_shape, FIREBALL_SHAPES, COLLISION_OBJECT);
#ifdef NONMATCHING
    /* this side repairs the tag in fireball_step: where collision_init got no
     * record, the init destroys the fireball, so fireball_step never runs */
    if (work->collision == NULL) {
        task_destroy(task);
        return;
    }
#endif
    if (work->collision != NULL) {
        work->collision->shapes->radius =
            meteor_fireball_object->model->r * SHAPE_SCALE;
        collision_fit_radius(task);
    }
}

static void fireball_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
    fireball_count--;
}

static void fireball_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: color an int, where the Uint32 color_blend returns would
     * do */
    int color;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njCnkModDrawObject(meteor_fireball_shadow);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njSetTexture(&meteor_fireball_texlist);
    if (work->timer) {
        color = color_blend(impact_start_color, impact_end_color,
                            work->timer * (1.0F / IMPACT_STEPS));
        model_draw_tinted(meteor_fireball_object->model, color);
    } else if (models_built) {
        njDirectDrawObject(models[FIREBALL_BODY].built);
    } else {
        njCnkEasyDrawModel(meteor_fireball_object->model);
    }
    njPopMatrixEx();
}

/* @hack spelling: the difference as an inline walk of three Floats, with the
 * first out++ and each origin++ a statement of its own, where three
 * statements on the fields would do */
#pragma inline(fireball_direction)
static void fireball_direction(const Float *origin, const Float *target,
                               Float *out)
{
    *out = *target++ - *origin;
    out++;
    origin++;
    *out++ = *target++ - *origin;
    origin++;
    *out = *target - *origin;
}

static void fireball_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR direction;
    NJS_POINT3 offset;
    NJS_POINT3 pos;
    NJS_VECTOR impulse;
    NJS_VECTOR velocity;
    struct angles rotation;
    struct task_work *player;
    struct particle *smoke;
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *textures;
    Float distance;
    Float scale;
    Float smoke_chance;
    int i;
    int kind;

    if (task->motion.p == NULL) {
        task_kill(task);
        return;
    }
    if (player_near(&work->pos, HEAR_RANGE)) {
        sound_play_at_for(NEAR_SOUND, (void *)fireball_step,
                          FIREBALL_SOUND_PRIORITY, SOUND_VOLUME,
                          NEAR_SOUND_STEPS, &work->pos);
    }
    fireball_direction(&work->pos.x, &work->scl.x, &direction.x);
    distance = njScalor(&direction);
    if (distance < fireball_speed || work->timer || collision_touched(task)) {
        work->pos = work->scl;
        scale = njScalor(&FIREBALL(task)->velocity);
        scale = FIREBALL(task)->bounce / scale;
        direction = FIREBALL(task)->velocity;
        SCALE(&direction, -scale);
        MOVE(&work->pos, &direction);
        FIREBALL(task)->bounce += FIREBALL(task)->bounce_speed;
        if (FIREBALL(task)->bounce < 0.0F) {
            FIREBALL(task)->bounce_speed =
                njAbs(FIREBALL(task)->bounce_speed) * BOUNCE_REBOUND;
            FIREBALL(task)->bounce = 0.0F;
        }
        FIREBALL(task)->bounce_speed -= BOUNCE_GRAVITY;
        FIREBALL(task)->bounce_speed *= BOUNCE_DRAG;
        if (!work->timer || work->submode) {
            /* @hack spelling: each assignment to player inside its test,
             * where a statement of its own would do */
            if ((player = player_work[0]) != NULL &&
                njDistanceP2P(&work->pos, &player->pos) < VIBRATION_RANGE) {
                vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
            }
            if ((player = player_work[1]) != NULL &&
                njDistanceP2P(&work->pos, &player->pos) < VIBRATION_RANGE) {
                vibration_start(1, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
            }
            sound_play_at(IMPACT_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME,
                          &work->pos);
            shake_create(&work->pos, SHAKE_RANGE, SHAKE_POWER);
            FIREBALL(task)->bounce_speed = BOUNCE_LIFT;
        }
        if ((work->timer == BLAST_STEP || work->submode) &&
            player_near(&work->pos, BLAST_RANGE)) {
            blast_create(BLAST_HUGE, &work->pos);
        }
        if (work->timer == BREAK_STEP || work->submode) {
            /* @bug memory: reads the shape's radius through a null record where
             * collision_init gets none from the heap (MOV.L at 0x8c502b7a).
             * meteor_fireball_init tests the record for null (TST at
             * 0x8c50278e) */
            offset.y = work->collision->shapes->radius;
            offset.x = 0.0F;
            offset.z = 0.0F;
            impulse.x = 0.0F;
            impulse.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njPushMatrixEx();
            for (i = 0; i < FIREBALL_FRAGMENTS; i++) {
                scale = RANDOM_UNIT();
                impulse.y =
                    (scale * FRAGMENT_SPEED_RANGE) + FIREBALL_FRAGMENT_SPEED;
                offset.y =
                    work->collision->shapes->radius * (scale + 1.0F) * 0.5F;
                rotation.y = 0;
                rotation.z = RANDOM_TWO_TURNS();
                njRotateZ(NULL, rotation.z);
                rotation.x = RANDOM_TWO_TURNS();
                njRotateX(NULL, rotation.x);
                njCalcPoint(NULL, &offset, &pos);
                njCalcVector(NULL, &impulse, &velocity);
                if (i < CHIP_FRAGMENTS) {
                    model = meteor_fireball_chip->model;
                    textures = &meteor_fireball_chip_texlist;
                    kind = FIREBALL_CHIP;
                } else {
                    model = meteor_fireball_shard->model;
                    textures = &meteor_fireball_shard_texlist;
                    kind = FIREBALL_SHARD;
                }
                velocity.y += FRAGMENT_LIFT;
                debris_launch_built_flags(&pos, &rotation, FRAGMENT_FLOOR,
                                          &velocity, model, models[kind].built,
                                          textures, DEBRIS_FADE_COLORS);
                matrix_load_parent();
            }
            njPopMatrix(2);
            sound_play_at(SHATTER_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME,
                          &work->pos);
        }
        smoke_chance = LANDED_SMOKE_CHANCE;
        FIREBALL(task)->spin_x *= SPIN_DRAG;
        FIREBALL(task)->spin_y *= SPIN_DRAG;
        work->timer++;
        if (work->submode) {
            work->timer = IMPACT_STEPS;
        }
    } else {
        smoke_chance = FLIGHT_SMOKE_CHANCE;
        /* @hack spelling: distance reused for the flight's scale, where scale
         * would do */
        distance = fireball_speed / distance;
        SCALE(&direction, distance);
        MOVE(&work->pos, &direction);
        FIREBALL(task)->velocity = direction;
        FIREBALL(task)->spin_x = FLIGHT_SPIN_X;
        FIREBALL(task)->spin_y = FLIGHT_SPIN_Y;
        sound_play_at_for(FLIGHT_SOUND, task, FIREBALL_SOUND_PRIORITY,
                          SOUND_VOLUME, FLIGHT_SOUND_STEPS, &work->pos);
    }
    if (work->timer >= IMPACT_STEPS) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
        work->ang.x += (Angle)(FIREBALL(task)->spin_x * ANGLE_PER_DEGREE);
        work->ang.y += (Angle)(FIREBALL(task)->spin_y * ANGLE_PER_DEGREE);
        if (RANDOM_UNIT() < smoke_chance) {
            offset = work->pos;
            smoke =
                burst_create_fast(&offset, &direction,
                                  (rand() * fireball_smoke_range * RAND_SCALE) +
                                      fireball_smoke_size,
                                  0.0F);
            if (smoke != NULL) {
                smoke->color = fireball_smoke_color;
            }
        }
    }
}
