/* white breath: puffs of the snowp textures, and the task that puffs them as
 * a player's breath */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/white_breath.h"
#include "shared/white_breath/text.inc"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a puff starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way. a steady or tinted puff turns up to STEADY_SPIN. a
 * short puff turns from SHORT_SPIN_BACK degrees a step one way up to
 * SHORT_SPIN_RANGE more */
#define TURN 40.0F
#define SPIN 0.5F
#define STEADY_SPIN 0.1F
#define SHORT_SPIN_BACK 0.6F
#define SHORT_SPIN_RANGE 1.3F

/* white_breath_fade_step and white_breath_swing_step swing a puff's
 * edge_angle up to SWING degrees either side of a quarter turn. a swing from
 * one side to the other takes SWING_STEPS steps. the swing's phase, in
 * degrees, starts at random below 2 * SWING */
#define SWING 8
#define SWING_STEPS 60
#define SWING_RATE (2.0F * SWING / SWING_STEPS)
/* a swinging puff keeps its swing's phase in depth_bias, which the draw reads
 * only under PARTICLE_DEPTH. the groups whose step swings don't set that
 * flag */
#define SWING_PHASE(particle) ((particle)->depth_bias)
/* whether the particle's frame has reached its group's frames */
/* @hack type: the frame compared through a Sint16, where
 * particle->frame >= group->frames would do for a frame from 0 below 32768 */
#define PAST_FRAMES(particle, group)                                           \
    ((Uint32)(Sint16)(particle)->frame >= (group)->frames)

#define BREATH_COLOR 0xc0606060

/* the players who can have a breath task: 0 and 1 */
#define WHITE_BREATH_PLAYERS 2
/* the task level of a breath task made without a parent */
#define BREATH_TASK_LEVEL 1
/* a breath task's work keeps its player's slot in tick. its timer counts
 * toward the next breath */
#define SLOT(work) ((work)->tick)
/* the player's animation frame of the step before */
#define LAST_FRAME(work) ((work)->scl.x)
/* a breath needs the work's timer above BREATH_WAIT */
#define BREATH_WAIT 180
/* a breath starts out along unk_2c0 from unk_224 of the player's record. its
 * reach is how far out, in lengths of unk_2c0. the reach is white_breath_reach,
 * plus BREATH_REACH_PER_SPEED for each unit of the record's spd.x */
#define BREATH_REACH_PER_SPEED 0.6F

/* make a puff as white_breath_tinted_create does, but white, and in a group
 * whose size_limit is 22500, not 75000. nothing calls it and no header
 * declares it. it isn't static, since SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct particle *white_breath_steady_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias);
/* NOLINTEND(misc-use-internal-linkage) */

static int white_breath_fade_step(struct particle_group *group,
                                  struct particle *particle);
static int white_breath_swing_step(struct particle_group *group,
                                   struct particle *particle);
static int white_breath_steady_step(struct particle_group *group,
                                    struct particle *particle);
static void white_breath_forget(struct task *task);
static void white_breath_step(struct task *task);

static NJS_TEXNAME white_breath_texnames[WHITE_BREATH_FRAMES] = {
#include "shared/white_breath/white_breath_texnames.inc"
};
NJS_TEXLIST white_breath_texlist = {
#include "shared/white_breath/white_breath_texlist.inc"
};

/* no group here has a template. their puffs take the particle system's own,
 * whose color is opaque white */
/* clang-format off */
static struct particle_group white_breath_fade_group = {
    PARTICLE_BLEND_ADD, &white_breath_texlist,
    0, WHITE_BREATH_FRAMES, 0.25F, 0.97F, 0.008F, 0.35F,
    white_breath_fade_step, 75000.0F, NULL, 0, NULL, NULL,
};
static struct particle_group white_breath_short_group = {
    PARTICLE_BLEND_ADD, &white_breath_texlist,
    0, WHITE_BREATH_FRAMES, 0.2F, 0.96F, 0.0065F, 0.05F,
    white_breath_swing_step, 22500.0F, NULL, 0, NULL, NULL,
};
static struct particle_group white_breath_steady_group = {
    PARTICLE_DEPTH | PARTICLE_BLEND_ADD, &white_breath_texlist,
    0, WHITE_BREATH_FRAMES, 0.2F, 0.96F, 0.0065F, 0.05F,
    white_breath_steady_step, 22500.0F, NULL, 0, NULL, NULL,
};
static struct particle_group white_breath_tinted_group = {
    PARTICLE_DEPTH | PARTICLE_BLEND_ADD, &white_breath_texlist,
    0, WHITE_BREATH_FRAMES, 0.2F, 0.96F, 0.0065F, 0.05F,
    white_breath_steady_step, 75000.0F, NULL, 0, NULL, NULL,
};
/* the color white_breath_tinted_create gives its puffs */
static Uint32 white_breath_tint = 0xff80ff50;
static struct particle_group white_breath_grey_group = {
    PARTICLE_BLEND_ADD, &white_breath_texlist,
    0, WHITE_BREATH_FRAMES, 0.05F, 0.995F, 0.0005F, 0.015F,
    white_breath_fade_step, 75000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */

/* each player's breath task, or null. the initializer places the array in
 * initialized data, as the image does, not in bss */
static struct task *white_breath_tasks[WHITE_BREATH_PLAYERS] = {NULL, NULL};
static Float white_breath_reach = 1.1F;
/* a breath's speed as it starts, in lengths of unk_2c0 a step */
static Float white_breath_speed = 0.1F;
static Float white_breath_scale = 0.8F; /* a breath's scale as it starts */

struct particle *white_breath_fade_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                          Float scale)
{
    struct particle *puff;

    puff = particle_create(&white_breath_fade_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

/* the distance from x to the nearest multiple of 2 * half. as x grows, the
 * distance climbs from 0 to half and falls back. half must not be 0 */
static int triangle_wave(Uint32 x, Uint32 half)
{
    Uint32 period = half * 2;

    x = x % period;
    if (x >= half) {
        x = period - x;
    }
    return x;
}

/* particle_move with the swing, but the puff stays at its last texture as it
 * fades. each time the frame reaches the group's frames, the step halves the
 * alpha. it sets the frame back to frames - 1. it ends the puff once the alpha
 * is 0 */
static int white_breath_fade_step(struct particle_group *group,
                                  struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    particle->edge_angle =
        QUARTER_TURN +
        (Angle)((triangle_wave((Uint32)SWING_PHASE(particle), 2 * SWING) -
                 SWING) *
                ANGLE_PER_DEGREE);
    SWING_PHASE(particle) += SWING_RATE;
    if (PAST_FRAMES(particle, group)) {
        particle->frame = group->frames - 1;
        particle->color = (particle->color & ~COLOR_ALPHA) |
                          ((particle->color >> 1) & COLOR_ALPHA);
        if (!(particle->color & COLOR_ALPHA)) {
            return 0;
        }
    }
    return 1;
}

struct particle *white_breath_short_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale)
{
    struct particle *puff;

    puff = particle_create(&white_breath_short_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd =
            (Angle)((((Float)rand() * (SHORT_SPIN_RANGE * RAND_SCALE)) -
                     SHORT_SPIN_BACK) *
                    ANGLE_PER_DEGREE);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

/* particle_move with the swing */
static int white_breath_swing_step(struct particle_group *group,
                                   struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    particle->edge_angle =
        QUARTER_TURN +
        (Angle)((triangle_wave((Uint32)SWING_PHASE(particle), 2 * SWING) -
                 SWING) *
                ANGLE_PER_DEGREE);
    SWING_PHASE(particle) += SWING_RATE;
    if (PAST_FRAMES(particle, group)) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

struct particle *white_breath_steady_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias)
{
    struct particle *puff;

    puff = particle_create(&white_breath_steady_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(STEADY_SPIN);
        puff->depth_bias = depth_bias;
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

/* the same step as particle_move */
static int white_breath_steady_step(struct particle_group *group,
                                    struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    if (PAST_FRAMES(particle, group)) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

struct particle *white_breath_tinted_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias)
{
    struct particle *puff;

    puff = particle_create(&white_breath_tinted_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(STEADY_SPIN);
        puff->depth_bias = depth_bias;
        puff->frame += (Float)rand() * RAND_SCALE;
        puff->color = white_breath_tint;
    }
    return puff;
}

/* make a breath, a grey puff at an alpha of 0xc0. it turns and swings as
 * white_breath_fade_create's does. it shows the textures over 141 to 160
 * steps, then fades out over 140 more */
static struct particle *white_breath_grey_create(NJS_POINT3 *pos,
                                                 NJS_VECTOR *spd, Float scale)
{
    struct particle *puff;

    puff = particle_create(&white_breath_grey_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
        puff->color = BREATH_COLOR;
    }
    return puff;
}

struct task *white_breath_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < WHITE_BREATH_PLAYERS && white_breath_tasks[player] == NULL) {
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, white_breath_step, parent);
        } else {
            task = task_create(TASK_WORK, BREATH_TASK_LEVEL, white_breath_step,
                               "WhiteBreathExec");
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = white_breath_forget;
            white_breath_tasks[player] = task;
            SLOT(work) = player;
            return task;
        }
    }
    return NULL;
}

/* empty the player's entry of white_breath_tasks where it holds task, so the
 * player can have another */
static void white_breath_forget(struct task *task)
{
    if (white_breath_tasks[SLOT(task->work)] == task) {
        white_breath_tasks[SLOT(task->work)] = NULL;
    }
}

static void white_breath_step(struct task *task)
{
    struct task_work *work = task->work;
    int player = SLOT(work);
    struct player *record;
    Float frame;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float reach;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (player_work[player] == NULL ||
        (record = player_records[player]) == NULL) {
        task_destroy(task);
        return;
    }
    frame = record->anim.frame;
    if (!(record->ahead.above_flags & WORLD_FLAT) &&
        frame <= LAST_FRAME(work) && work->timer > BREATH_WAIT) {
        work->timer = 0;
        /* @hack redundant: a switch on the record's character with only a
         * default, where its body alone would do */
        switch (record->character) {
        default:
            spd = record->unk_2c0;
            spd.x *= white_breath_speed;
            spd.y *= white_breath_speed;
            spd.z *= white_breath_speed;
            pos = record->unk_2c0;
            reach =
                white_breath_reach + (record->spd.x * BREATH_REACH_PER_SPEED);
            pos.x *= reach;
            pos.y *= reach;
            pos.z *= reach;
        }
        njAddVector(&pos, &record->unk_224);
        white_breath_grey_create(&pos, &spd, white_breath_scale);
    }
    work->timer++;
    if (record->spd.x > record->consts.sprint_speed) {
        work->timer++;
    }
    LAST_FRAME(work) = frame;
}
