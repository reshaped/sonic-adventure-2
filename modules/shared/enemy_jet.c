/* the jets the enemies send out: puffs that fly, fade and end */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/enemy_jet.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define JET_LEVEL 3 /* the task level of a jet without a parent */
#define TEXTURE_LEVEL 0 /* the texture task's level */

/* a jet keeps its state in its work's scl and ang. ALPHA falls from 1 by
 * FADE a step, and the jet ends once ALPHA is END_ALPHA or below */
#define ALPHA(work) ((work)->scl.x)
#define FADE(work) ((work)->scl.y)
#define END_ALPHA 0.05F
/* SIZE times SPRITE_SIZE is half the sprite's width and height */
#define SIZE(work) ((work)->scl.z)
#define SPRITE_SIZE 16.0F
/* the jet's speed, three Floats in place of ang */
#define SPD(work) ((NJS_VECTOR *)&(work)->ang)

/* each kind's FADE, and its SIZE: the kind's _SIZE plus a random part below
 * its _SPREAD */
#define LARGE_FADE 0.1F
#define LARGE_SIZE 0.27F
#define LARGE_SPREAD 0.05F
#define MEDIUM_FADE 0.16F
#define MEDIUM_SIZE 0.18F
#define MEDIUM_SPREAD 0.04F
#define SMALL_FADE 0.08F
#define SMALL_SIZE 0.14F
#define SMALL_SPREAD 0.02F

/* the sprite's color: each of its CHANNEL_BITS-bit channels, alpha's too,
 * is ALPHA's share of CHANNEL_MAX. TRANSLUCENT is njPtclSpriteStart's flag
 * for the translucent list */
#define CHANNEL_MAX 255.0F
#define CHANNEL_BITS 8
#define TRANSLUCENT 1

/* a random Float from least up to, but not including, least + spread */
#define RANDOM_FROM(least, spread)                                             \
    ((rand() * ((spread) * RAND_SCALE)) + (least))

#include "shared/enemy_jet/text.inc"

static NJS_TEXNAME enemy_jet_texname[1] = {
#include "shared/enemy_jet/enemy_jet_texname.inc"
};
static NJS_TEXLIST enemy_jet_texlist = {enemy_jet_texname, 1};
/* the task that keeps enemy_jet_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *enemy_jet_texture_task = NULL;

/* draw the jet as a sprite that adds its color to what lies behind it. its
 * alpha scales the color it adds, and so a fading jet also darkens.
 *
 * jets that follow one another among a parent's children make a run, which
 * shares one setup. the run's first jet sets the texture and additive
 * blending, and turns the fog off. its last jet sets alpha blending again,
 * and turns the fog on. a jet without a parent is a run of its own */
static void enemy_jet_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float size = SIZE(work) * SPRITE_SIZE;
    Uint32 color;

    if (task->parent == NULL || task->parent->child == task ||
        task->prev->draw != enemy_jet_draw) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&enemy_jet_texlist);
        njFogDisable();
    }
    color = (Uint32)(ALPHA(work) * CHANNEL_MAX);
    color |= color << CHANNEL_BITS;
    color |= color << (2 * CHANNEL_BITS);
    njPtclSpriteStart(0, color, TRANSLUCENT);
    njPtclDrawSprite(&work->pos, 1, size, size);
    njPtclSpriteEnd();
    if (task->parent == NULL || task->parent->child == task->next ||
        task->next->draw != enemy_jet_draw) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
}

static void enemy_jet_destroy(struct task *task)
{
}

static void enemy_jet_step(struct task *task)
{
    struct task_work *work = task->work;

    if (task->parent == NULL && placement_try_leave(task)) {
        return;
    }
    if (ALPHA(work) <= END_ALPHA) {
        task_destroy(task);
        return;
    }
    if (game_state.time_stopped) {
        return;
    }
    ALPHA(work) -= FADE(work);
    if (ALPHA(work) < 0.0F) {
        ALPHA(work) = 0.0F;
    }
    njAddVector(&work->pos, SPD(work));
    work->timer++;
}

static void enemy_jet_start(struct task *task)
{
    struct task_work *work = task->work;

    ALPHA(work) = 1.0F;
    /* @hack spelling: the kinds' cases from ENEMY_JET_SMALL down, where
     * enemy_jet_kind's order would do */
    switch (work->submode) {
    case ENEMY_JET_SMALL:
        FADE(work) = SMALL_FADE;
        SIZE(work) = RANDOM_FROM(SMALL_SIZE, SMALL_SPREAD);
        break;
    case ENEMY_JET_MEDIUM:
        FADE(work) = MEDIUM_FADE;
        SIZE(work) = RANDOM_FROM(MEDIUM_SIZE, MEDIUM_SPREAD);
        break;
    /* @hack redundant: a case for ENEMY_JET_LARGE, which the default takes
     * as well */
    case ENEMY_JET_LARGE:
    default:
        FADE(work) = LARGE_FADE;
        SIZE(work) = RANDOM_FROM(LARGE_SIZE, LARGE_SPREAD);
        break;
    }
    /* @hack spelling: the destructor set before the draw, not in the
     * members' order */
    task->destructor = enemy_jet_destroy;
    task->draw = enemy_jet_draw;
}

static void enemy_jet_texture_destroy(struct task *task)
{
    njReleaseTexture(&enemy_jet_texlist);
    enemy_jet_texture_task = NULL;
}

static void enemy_jet_texture_step(struct task *task)
{
}

void enemy_jet_load_texture(void)
{
    if (enemy_jet_texture_task == NULL) {
        njLoadTexture(&enemy_jet_texlist);
        enemy_jet_texture_task =
            task_create(0, TEXTURE_LEVEL, enemy_jet_texture_step, "ManTex");
        enemy_jet_texture_task->destructor = enemy_jet_texture_destroy;
    }
}

void enemy_jet_create(struct task *parent, enum enemy_jet_kind kind,
                      NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct task_work *work;
    NJS_VECTOR *jet_spd;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, enemy_jet_step, parent);
    } else {
        task = task_create(TASK_WORK, JET_LEVEL, enemy_jet_step,
                           "EnemyJetExecutor");
    }
    if (task != NULL) {
        work = task->work;
        /* @hack spelling: jet_spd, a local for the speed's place, set before
         * the kind's store, where SPD(work) written in place would do */
        jet_spd = SPD(work);
        work->submode = kind;
        /* @hack spelling: pos stored through task->work read again, where
         * work would do */
        task->work->pos = *pos;
        *jet_spd = *spd;
        enemy_jet_start(task);
        enemy_jet_load_texture();
    }
}
