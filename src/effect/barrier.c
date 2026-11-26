/* the effects a player wears: the barrier, the magnetic barrier and
 * invincibility. prominences and D sparks flicker as a magnetic barrier's
 * bolts do */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "effect/barrier.h"
#include "effect/texlist.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of each effect here that has no parent */
#define EFFECT_LEVEL 3

/* the sound a barrier plays as it starts */
#define BARRIER_SOUND 0x801a

/* follow_player puts a worn effect's size in its work's scl.y. the size is
 * the player's x scale times MECH_SIZE for a mech, and BODY_SIZE for any
 * other. a spark's model draws at MODEL_SCALE of the size */
#define MECH_SIZE 13.0F
#define BODY_SIZE 6.0F
#define MODEL_SCALE 0.2F

/* a barrier's step adds a spark where rand() * RAND_SCALE comes out above
 * SPARK_ROLL, one step in five */
#define SPARK_ROLL 0.8F

/* a flickering spark is a magnetic spark, a prominence or a D spark. while
 * its tick is LOOK_LAST or less, each step gives it a new look. each later
 * step holds its brightness to FADE_START at most, and then takes FADE_STEP
 * from it. the step that finds it dark ends it */
#define LOOK_LAST 10
#define FADE_START 0.5F
#define FADE_STEP 0.025F

/* look / 2 picks one of LOOK_TEXTURES textures, barria_t01 to t08. the
 * look's low bit picks the texture's left or right half */
#define LOOK_TEXTURES 8
#define LOOKS (2 * LOOK_TEXTURES)
#define FLASHES 3 /* barria_t09 to t11, which a magnetic spark flashes in */

/* a barrier's or an invincibility's spark counts its steps in its tick. the
 * step that finds the tick at BARRIER_SPARK_STEPS kills a barrier's spark,
 * and at INVINCIBLE_SPARK_STEPS an invincibility's */
#define BARRIER_SPARK_STEPS 30
#define INVINCIBLE_SPARK_STEPS 16

/* invincibility's timer counts its steps from 0. the step that finds it past
 * INVINCIBLE_LAST ends the effect */
#define INVINCIBLE_LAST 1260

/* a prominence's pitch is random from 0 up to PROMINENCE_PITCH. a pitch
 * past PROMINENCE_FLIP is negated */
#define PROMINENCE_PITCH 0x7000
#define PROMINENCE_FLIP 0x3800
/* a D spark's pitch is random from DSPARK_PITCH down to DSPARK_SPAN less */
#define DSPARK_PITCH 0x2400
#define DSPARK_SPAN 0x6400

/* a look's width in u, half its texture's */
#define HALF_WIDTH 0.5F
/* a color's alpha byte, at full */
#define OPAQUE 0xff000000
/* a flickering spark's channels are its brightness times BRIGHTEST. a D
 * spark's blue is its red shifted down by DSPARK_BLUE_SHIFT, which makes it
 * yellow */
#define BRIGHTEST 255.0F
#define DSPARK_BLUE_SHIFT 2
/* a barrier's spark shows each of its BARRIER_SPARK_TEXTURES textures for two
 * steps. an invincibility's spark shows each of its INVINCIBLE_SPARK_TEXTURES
 * for one */
#define BARRIER_SPARK_TEXTURES 16
#define INVINCIBLE_SPARK_TEXTURES 8

/* the slot of the player an effect or a spark belongs to, in its work's
 * submode */
#define WORN_PLAYER(work) ((work)->submode)

/* a flickering spark's brightness falls from 1 at its start to 0. its look
 * is one of LOOKS. a magnetic spark also holds its flash, one of FLASHES */
#define BRIGHTNESS(work) ((work)->scl.z)
#define LOOK(work) ((work)->timer)
#define FLASH(work) ((work)->flags)

static void magnetic_barrier_step(struct task *task);
static void magnetic_barrier_draw(struct task *task);
static void magnetic_barrier_destroy(struct task *task);
static void magnetic_spark_step(struct task *task);
static void magnetic_spark_draw(struct task *task);
static void prominence_step(struct task *task);
static void prominence_draw(struct task *task);
static void dspark_step(struct task *task);
static void dspark_draw(struct task *task);
static void barrier_step(struct task *task);
static void barrier_draw(struct task *task);
static void barrier_destroy(struct task *task);
static void barrier_spark_step(struct task *task);
static void barrier_spark_draw(struct task *task);
static void invincibility_step(struct task *task);
static void invincibility_draw(struct task *task);
static void invincibility_spark_step(struct task *task);
static void invincibility_spark_draw(struct task *task);

/* nothing calls dspark_create. it isn't static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* start a D spark, a small yellow prominence. it stands radius from unk_218
 * of the player's record, at a random place in the player's axes. the spark
 * is parent's child, or where parent is null, a task named DSpark in level 3.
 * it returns the task, or null where the heap can't give it. it ends sooner
 * once the slot's record is gone */
struct task *dspark_create(int player, struct task *parent, Float radius);
/* NOLINTEND(misc-use-internal-linkage) */

/* a magnetic spark's bolts, barria_t01 to t08, and its flashes, t09 to t11.
 * each list ends in a copy of its last texture, which no spark picks */
static NJS_TEXLIST bolt_textures[LOOK_TEXTURES + 1] = {
#include "effect/barrier/bolt_textures.inc"
};
static NJS_TEXLIST flash_textures[FLASHES + 1] = {
#include "effect/barrier/flash_textures.inc"
};
static NJS_TEXLIST barrier_spark_textures[BARRIER_SPARK_TEXTURES] = {
#include "effect/barrier/barrier_spark_textures.inc"
};
static NJS_TEXLIST invincibility_spark_textures[INVINCIBLE_SPARK_TEXTURES] = {
#include "effect/barrier/invincibility_spark_textures.inc"
};
static NJS_TEXLIST invincibility_ball_texture = {
#include "effect/barrier/invincibility_ball_texture.inc"
};

/* the quads of a magnetic spark's bolt, a prominence and a D spark. each
 * draw writes their u and colors */
static NJS_TEXTURE_VTX bolt_quad[4] = {
#include "effect/barrier/bolt_quad.inc"
};
static NJS_TEXTURE_VTX prominence_quad[4] = {
#include "effect/barrier/prominence_quad.inc"
};
static NJS_TEXTURE_VTX dspark_quad[4] = {
#include "effect/barrier/dspark_quad.inc"
};

struct task *barrier_create(int player)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, barrier_step,
                            "NBarrier")) != NULL) {
        WORN_PLAYER(task->work) = player;
        sound_play(BARRIER_SOUND, NULL, 0, 0);
        task->draw = barrier_draw;
        task->destructor = barrier_destroy;
    }
    return task;
}

struct task *magnetic_barrier_create(int player)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, magnetic_barrier_step,
                            "TBarrier")) != NULL) {
        WORN_PLAYER(task->work) = player;
        sound_play(BARRIER_SOUND, NULL, 0, 0);
        task->draw = magnetic_barrier_draw;
        task->destructor = magnetic_barrier_destroy;
    }
    return task;
}

struct task *invincibility_create(int player)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, invincibility_step,
                            "Muteki")) != NULL) {
        WORN_PLAYER(task->work) = player;
        task->destructor = barrier_destroy;
        task->draw = invincibility_draw;
    }
    return task;
}

/* put work where its player stands. give it the player's x scale in scl.x,
 * and the effect's size in scl.y. player is the player's work */
static void follow_player(struct task_work *work, struct task_work *player)
{
    struct player *record = player_records[WORN_PLAYER(work)];

    /* @bug memory: reads through null once the player's slot empties. the
     * step that finds the slot's work null kills its task (TST at 0x8c0a021c,
     * 0x8c0a0c08 and 0x8c0a0e5c). the task's draw still runs before the
     * destroy, and FMOV.S at 0x8c0a0116 reads the null work */
    work->scl.x = player->scl.x;
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        work->scl.y = work->scl.x * MECH_SIZE;
        work->pos = record->unk_218;
        break;
    /* @hack redundant: the cases that fall into default */
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    default:
        work->scl.y = work->scl.x * BODY_SIZE;
        work->pos = record->unk_230;
        break;
    }
}

static void magnetic_barrier_draw(struct task *task)
{
    struct task_work *work = task->work;

#ifdef NONMATCHING
    /* this side repairs follow_player's tag: it draws nothing while the
     * wearer's slot holds no player */
    if (player_work[WORN_PLAYER(work)] == NULL) {
        return;
    }
#endif
    follow_player(work, player_work[WORN_PLAYER(work)]);
    work->scl.x = work->scl.y * MODEL_SCALE;
}

static void magnetic_barrier_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct task_work *spark;

    if (player_work[WORN_PLAYER(work)] == NULL ||
        !(player_records[WORN_PLAYER(work)]->flags & PLAYER_MAGNETIC_BARRIER)) {
        task_kill(task);
        return;
    }
    if (rand() * RAND_SCALE > SPARK_ROLL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, magnetic_spark_step, task)) !=
            NULL) {
            spark = child->work;
            WORN_PLAYER(spark) = WORN_PLAYER(work);
            spark->ang.x = RANDOM_TURN();
            spark->ang.y = RANDOM_TURN();
            BRIGHTNESS(spark) = 1.0F;
            child->draw = magnetic_spark_draw;
        }
    }
}

static void magnetic_spark_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick++ > LOOK_LAST) {
        /* @hack redundant: the else after the return */
        if (BRIGHTNESS(work) <= 0.0F) {
            task_kill(task);
            return;
        } else {
            if (BRIGHTNESS(work) > FADE_START) {
                BRIGHTNESS(work) = FADE_START;
            }
            BRIGHTNESS(work) -= FADE_STEP;
        }
    } else {
        LOOK(work) = rand() * (LOOKS * RAND_SCALE);
        FLASH(work) = rand() * (FLASHES * RAND_SCALE);
    }
}

/* a magnetic spark draws its model in its flash's texture, and its bolt's
 * quad. both dim as the spark dims. a magnetic barrier's sparks draw as one
 * batch. the first turns the fog off, and pushes a matrix moved to the
 * barrier. each later one starts from that matrix again. the last pops it and
 * turns the fog on */
static void magnetic_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float shade;
    Float scale;
    Float left;
    Uint32 bright;
    Uint32 color;
    /* @hack spelling: texture, a local for each texture's index, where the
     * index in place would do */
    int texture;
    NJS_TEXTURE_VTX *vtx;

#ifdef NONMATCHING
    /* this side draws no spark while the slot holds no player, as the barrier's
     * own draw then draws nothing */
    if (player_work[WORN_PLAYER(task->parent->work)] == NULL) {
        return;
    }
#endif
    if (task->parent->child == task) {
        njFogDisable();
        njPushMatrixEx();
        njTranslateEx(&task->parent->work->pos);
        njPushMatrixEx();
    } else {
        matrix_load_parent();
    }
    shade = BRIGHTNESS(work) - 1.0F;
    constant_material_set(0.0F, shade, shade, shade);
    texture = FLASH(work);
    njSetTexture(&flash_textures[texture]);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = task->parent->work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(&magnetic_spark_model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    vtx = bolt_quad;
    if (LOOK(work) & 1) {
        left = HALF_WIDTH;
    } else {
        left = 0.0F;
    }
    bright = BRIGHTNESS(work) * BRIGHTEST;
    color = OPAQUE | (bright << 16) | (bright << 8) | bright;
    /* @hack redundant: the 0.0F added to left */
    /* @hack spelling: vtx stepped in the store of each col but the last,
     * where a step of its own would do */
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx++->col = color;
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx->col = color;
    /* @hack spelling: the bolt's texture as the look times 0.5F, where
     * LOOK(work) >> 1 would do */
    texture = LOOK(work) * 0.5F;
    njSetTexture(&bolt_textures[texture]);
    njSetTextureNum(0);
    njDrawTexture3DEx(bolt_quad, 4, 1);
    if (task->parent->child->prev == task) {
        njPopMatrix(2);
        njFogEnable();
    }
}

static void magnetic_barrier_destroy(struct task *task)
{
}

static void prominence_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick++ > LOOK_LAST) {
        /* @hack redundant: the else after the return */
        if (BRIGHTNESS(work) <= 0.0F) {
            task_kill(task);
            return;
        } else {
            if (BRIGHTNESS(work) > FADE_START) {
                BRIGHTNESS(work) = FADE_START;
            }
            BRIGHTNESS(work) -= FADE_STEP;
        }
    } else {
        LOOK(work) = rand() * (LOOKS * RAND_SCALE);
    }
}

/* a run of prominences among a parent's children draws as one batch. the
 * first turns the fog off, sets an additive blend and effect_texlist, and
 * pushes the matrix. each later one starts from the pushed matrix again. the
 * last pops the matrix and turns the fog on. it leaves the source blend at
 * NJD_COLOR_BLENDING_SRCALPHA and the destination's at
 * NJD_COLOR_BLENDING_INVSRCALPHA, and the chunk blend at NJD_FBS_SA and
 * NJD_FBD_ISA. a prominence without a parent is a run of its own */
static void prominence_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float left;
    Uint8 bright;
    Uint32 color;
    int texture;
    NJS_TEXTURE_VTX *vtx;

    if (task->parent == NULL || task->parent->child == task ||
        task->prev->draw != prominence_draw) {
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njPushMatrixEx();
        njSetTexture(&effect_texlist);
    }
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    vtx = prominence_quad;
    if (LOOK(work) & 1) {
        left = HALF_WIDTH;
    } else {
        left = 0.0F;
    }
    bright = BRIGHTNESS(work) * BRIGHTEST;
    color = OPAQUE | (bright << 16) | (bright << 8) | bright;
    /* @hack redundant: the 0.0F added to left */
    /* @hack spelling: vtx stepped in the store of each col but the last,
     * where a step of its own would do */
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx++->col = color;
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx->col = color;
    texture = LOOK(work) >> 1;
    if (texture > LOOK_TEXTURES - 1) {
        texture = LOOK_TEXTURES - 1;
    }
    njSetTextureNum(texture + EFFECT_BARRIER_T01);
    njDrawTexture3DEx(prominence_quad, 4, 1);
    if (task->parent == NULL || task->parent->child->prev == task ||
        task->next->draw != prominence_draw) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
        njFogEnable();
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
}

struct task *prominence_create(struct task *parent, NJS_POINT3 *pos,
                               Float radius)
{
    struct task *task;
    struct task_work *work;
    /* @hack spelling: the sines and cosines declared y's first, where x's
     * first would do */
    Float sin_y;
    Float cos_y;
    Float cos_x;
    Float sin_x;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, prominence_step, parent);
    } else {
        task = task_create(TASK_WORK, EFFECT_LEVEL, prominence_step,
                           "TProminence");
    }
    if (task != NULL) {
        work = task->work;
        work->ang.x = rand() * (PROMINENCE_PITCH * RAND_SCALE);
        if (work->ang.x > PROMINENCE_FLIP) {
            work->ang.x = -work->ang.x;
        }
        work->ang.y = RANDOM_TURN();
        sin_x = njSin(work->ang.x);
        cos_x = njCos(work->ang.x);
        sin_y = njSin(work->ang.y);
        cos_y = njCos(work->ang.y);
        work->pos.x = pos->x + (radius * sin_y * cos_x);
        work->pos.y = pos->y - (radius * sin_x);
        work->pos.z = pos->z + (radius * cos_y * cos_x);
        /* @hack spelling: task->work read again, where work would do */
        BRIGHTNESS(task->work) = 1.0F;
        task->draw = prominence_draw;
        task->destructor = magnetic_barrier_destroy;
    }
    return task;
}

static void dspark_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick++ > LOOK_LAST) {
        /* @hack redundant: the else after the return */
        if (BRIGHTNESS(work) <= 0.0F) {
            task_kill(task);
            return;
        } else {
            if (BRIGHTNESS(work) > FADE_START) {
                BRIGHTNESS(work) = FADE_START;
            }
            BRIGHTNESS(work) -= FADE_STEP;
        }
    } else {
        LOOK(work) = rand() * (LOOKS * RAND_SCALE);
    }
    if (player_records[WORN_PLAYER(work)] == NULL) {
        task_kill(task);
    }
}

/* a D spark draws as a prominence does, in yellow. its pos is in its
 * player's axes. their origin is unk_218 of the record. they turn by
 * HALF_TURN less the player's heading, then by its pitch and roll */
static void dspark_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    NJS_POINT3 *origin;
    Float left;
    Uint8 bright;
    Uint32 color;
    int texture;
    NJS_TEXTURE_VTX *vtx;

    origin = NULL;
    if (player_records[WORN_PLAYER(work)] != NULL) {
        origin = &player_records[WORN_PLAYER(work)]->unk_218;
    }
    if (task->parent == NULL || task->parent->child == task ||
        task->prev->draw != dspark_draw) {
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njPushMatrixEx();
        njSetTexture(&effect_texlist);
    }
    if (origin != NULL) {
        njTranslateEx(origin);
    }
    player = player_work[WORN_PLAYER(work)];
    if (player != NULL) {
        njRotateY(NULL, HALF_TURN - player->ang.y);
        njRotateX(NULL, player->ang.x);
        njRotateZ(NULL, player->ang.z);
    }
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    vtx = dspark_quad;
    if (LOOK(work) & 1) {
        left = HALF_WIDTH;
    } else {
        left = 0.0F;
    }
    bright = BRIGHTNESS(work) * BRIGHTEST;
    color =
        OPAQUE | (bright << 16) | (bright << 8) | (bright >> DSPARK_BLUE_SHIFT);
    /* @hack redundant: the 0.0F added to left */
    /* @hack spelling: vtx stepped in the store of each col but the last,
     * where a step of its own would do */
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx++->col = color;
    vtx->u = left + 0.0F;
    vtx++->col = color;
    vtx->u = left + HALF_WIDTH;
    vtx->col = color;
    texture = LOOK(work) >> 1;
    if (texture > LOOK_TEXTURES - 1) {
        texture = LOOK_TEXTURES - 1;
    }
    njSetTextureNum(texture + EFFECT_BARRIER_T01);
    njDrawTexture3DEx(dspark_quad, 4, 1);
    if (task->parent == NULL || task->parent->child->prev == task ||
        task->next->draw != dspark_draw) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
        njFogEnable();
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
}

struct task *dspark_create(int player, struct task *parent, Float radius)
{
    struct task *task;
    struct task_work *work;
    /* @hack spelling: the sines and cosines declared y's first, where x's
     * first would do */
    Float sin_y;
    Float cos_y;
    Float cos_x;
    Float sin_x;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, dspark_step, parent);
    } else {
        task = task_create(TASK_WORK, EFFECT_LEVEL, dspark_step, "DSpark");
    }
    if (task != NULL) {
        work = task->work;
        work->ang.x = DSPARK_PITCH - (rand() * (DSPARK_SPAN * RAND_SCALE));
        work->ang.y = RANDOM_TURN();
        sin_x = njSin(work->ang.x);
        cos_x = njCos(work->ang.x);
        sin_y = njSin(work->ang.y);
        cos_y = njCos(work->ang.y);
        work->pos.x = radius * sin_y * cos_x;
        work->pos.y = -(radius * sin_x);
        work->pos.z = radius * cos_y * cos_x;
        /* @hack spelling: task->work read again, where work would do */
        BRIGHTNESS(task->work) = 1.0F;
        task->draw = dspark_draw;
        WORN_PLAYER(work) = player;
    }
    return task;
}

static void barrier_draw(struct task *task)
{
    struct task_work *work = task->work;

#ifdef NONMATCHING
    /* this side repairs follow_player's tag: it draws nothing while the
     * wearer's slot holds no player */
    if (player_work[WORN_PLAYER(work)] == NULL) {
        return;
    }
#endif
    follow_player(work, player_work[WORN_PLAYER(work)]);
    work->scl.x = work->scl.y * MODEL_SCALE;
}

static void barrier_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct task_work *spark;

    if (player_work[WORN_PLAYER(work)] == NULL ||
        !(player_records[WORN_PLAYER(work)]->flags & PLAYER_BARRIER)) {
        task_kill(task);
        return;
    }
    if (rand() * RAND_SCALE > SPARK_ROLL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, barrier_spark_step, task)) !=
            NULL) {
            spark = child->work;
            spark->ang.x = RANDOM_TURN();
            spark->ang.y = RANDOM_TURN();
            WORN_PLAYER(spark) = WORN_PLAYER(work);
            child->draw = barrier_spark_draw;
        }
    }
}

static void barrier_spark_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick >= BARRIER_SPARK_STEPS) {
        task_kill(task);
        return;
    }
    work->tick++;
}

/* a barrier's sparks draw as one batch, as a magnetic barrier's do */
static void barrier_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    /* @hack spelling: texture, a local for the texture's index, where the
     * index in place would do */
    int texture;

#ifdef NONMATCHING
    /* this side draws no spark while the slot holds no player, as the barrier's
     * own draw then draws nothing */
    if (player_work[WORN_PLAYER(task->parent->work)] == NULL) {
        return;
    }
#endif
    if (task->parent->child == task) {
        njFogDisable();
        njPushMatrixEx();
        njTranslateEx(&task->parent->work->pos);
        njPushMatrixEx();
    } else {
        matrix_load_parent();
    }
    texture = (work->tick >> 1) & (BARRIER_SPARK_TEXTURES - 1);
    njSetTexture(&barrier_spark_textures[texture]);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = task->parent->work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(&barrier_spark_model);
    if (task->parent->child->prev == task) {
        njFogEnable();
        njPopMatrix(2);
    }
}

static void barrier_destroy(struct task *task)
{
}

static void invincibility_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;

#ifdef NONMATCHING
    /* this side repairs follow_player's tag: it draws nothing while the
     * wearer's slot holds no player */
    if (player_work[WORN_PLAYER(work)] == NULL) {
        return;
    }
#endif
    follow_player(work, player_work[WORN_PLAYER(work)]);
    njFogDisable();
    njPushMatrixEx();
    njSetTexture(&invincibility_ball_texture);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(&invincibility_ball_model);
    njPopMatrixEx();
    njFogEnable();
}

static void invincibility_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player = player_work[WORN_PLAYER(work)];
    struct player *record = player_records[WORN_PLAYER(work)];
    struct task *child;
    struct task_work *spark;
    int i;

    /* @bug memory: where the slot is empty, the effect's end below writes
     * through null. TST at 0x8c0a0e5c finds the slot's work null, and MOV.W
     * at 0x8c0a0e8a writes the flags of its record, which player_unregister
     * nulls with the work */
#ifdef NONMATCHING
    /* this side repairs the tag above: it ends the effect before the write
     * through an empty slot's null record */
    if (record == NULL) {
        task_kill(task);
        return;
    }
#endif
    if (player == NULL || work->timer++ > INVINCIBLE_LAST ||
        game_state.cleared == 1) {
        record->flags &= ~PLAYER_INVINCIBLE;
        task_kill(task);
        return;
    }
    record->flags |= PLAYER_INVINCIBLE;
    /* @hack spelling: i counted down from the sparks to make, where a count
     * up would do */
    for (i = (work->timer & 1) + 1; i > 0; i--) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, invincibility_spark_step,
                                       task)) != NULL) {
            spark = child->work;
            spark->ang.x = RANDOM_TURN();
            spark->ang.y = RANDOM_TURN();
            WORN_PLAYER(spark) = WORN_PLAYER(work);
            child->draw = invincibility_spark_draw;
        }
    }
}

static void invincibility_spark_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick >= INVINCIBLE_SPARK_STEPS) {
        task_kill(task);
        return;
    }
    work->tick++;
}

/* an invincibility's spark puts itself where the player stands, as
 * follow_player puts the effect. the first of the effect's sparks turns the
 * fog off, and the last turns it on */
static void invincibility_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    /* @hack spelling: texture, a local for the texture's index, where the
     * index in place would do */
    int texture;

#ifdef NONMATCHING
    /* this side repairs follow_player's tag: it draws nothing while the
     * wearer's slot holds no player */
    if (player_work[WORN_PLAYER(work)] == NULL) {
        return;
    }
#endif
    follow_player(work, player_work[WORN_PLAYER(work)]);
    work->scl.x = work->scl.y * MODEL_SCALE;
    if (task->parent->child == task) {
        njFogDisable();
    }
    njPushMatrixEx();
    texture = work->tick & (INVINCIBLE_SPARK_TEXTURES - 1);
    njSetTexture(&invincibility_spark_textures[texture]);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(&invincibility_spark_model);
    njPopMatrixEx();
    if (task->parent->child->prev == task) {
        njFogEnable();
    }
}
