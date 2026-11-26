/* ectoplasm: puffs that a player gives off for a time, which drift against
 * gravity and fade */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/lines.h"
#include "effect/ectoplasm.h"
#include "effect/texlist.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the task levels of a puff, and of its source, the task that gives a
 * player its puffs */
#define PUFF_LEVEL 3
#define SOURCE_LEVEL 2

/* a puff starts within half a size of where its player stands, along each axis.
 * the size is the player's x scale times MECH_SIZE for a mech, and BODY_SIZE
 * for any other */
#define MECH_SIZE 15.0F
#define BODY_SIZE 6.0F

/* a puff moves at RISE times minus world_gravity a step. each step keeps
 * PUFF_FADE of its red, green and blue. its timer starts at PUFF_STEPS */
#define RISE 0.1F
#define PUFF_STEPS 30
#define PUFF_FADE 0.98F
/* a new puff's color: a bluish grey, and an alpha that the draw doesn't
 * read */
#define PUFF_ALPHA 2.0F
#define PUFF_RED 0.5F
#define PUFF_GREEN 0.6F
#define PUFF_BLUE 0.7F

/* a puff's record, in its task's extra */
struct ectoplasm {
    Sint8 unk_00; /* nothing reads it */
    Sint8 texlist; /* which of ectoplasm_texlists it draws with */
    /* the steps it has left, counted while game_state.time_stopped is 0. the
     * step that finds it at 0 or below ends it */
    Sint16 timer;
    /* the constant material's color, which fades. the draw takes its alpha
     * as 1 */
    NJS_ARGB color;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
};

/* a source's work. submode holds the player's slot, and timer the puffs
 * left to give */
#define ECTOPLASM_PLAYER(work) ((work)->submode)

/* the textures a puff can draw with: barria_t10, barria_n08 and mutekiball.
 * each list holds one, and every puff takes the first */
static NJS_TEXLIST ectoplasm_texlists[3] = {
#include "effect/ectoplasm/ectoplasm_texlists.inc"
};
/* a puff's sprite: the whole texture, 32 by 32 */
static NJS_TEXANIM ectoplasm_texanim = {
#include "effect/ectoplasm/ectoplasm_texanim.inc"
};
/* the sprite each puff's draw fills */
/* @hack type: an array of one, where an object would do */
static NJS_SPRITE ectoplasm_sprite[1] = {
#include "effect/ectoplasm/ectoplasm_sprite.inc"
};

/* put work where its player stands. give it the player's x scale in scl.x,
 * and the size its puffs start within in scl.y */
static void ectoplasm_follow_player(struct task_work *work)
{
    struct player *record = player_records[ECTOPLASM_PLAYER(work)];

    work->scl.x = player_work[ECTOPLASM_PLAYER(work)]->scl.x;
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

static void ectoplasm_draw(struct task *task)
{
    struct ectoplasm *puff = task->extra.p;

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(1.0F, puff->color.r, puff->color.g, puff->color.b);
    njSetTexture(&ectoplasm_texlists[puff->texlist]);
    ectoplasm_sprite->p = puff->pos;
    ectoplasm_sprite->tlist = &ectoplasm_texlists[puff->texlist];
    njDrawSprite3D(ectoplasm_sprite, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void ectoplasm_step(struct task *task)
{
    struct ectoplasm *puff = task->extra.p;

    njAddVector(&puff->pos, &puff->spd);
    puff->color.b *= PUFF_FADE;
    puff->color.g *= PUFF_FADE;
    puff->color.r *= PUFF_FADE;
    if (game_state.time_stopped == 0 && puff->timer-- <= 0) {
        task_kill(task);
    }
}

/* a puff's task with its record, which the caller fills. it returns null
 * where the heap can't give both */
static struct task *ectoplasm_task_create(void)
{
    struct task *task =
        task_create(0, PUFF_LEVEL, ectoplasm_step, "EffectEctoplasmExecutor");
    struct ectoplasm *puff;

    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((puff = syMalloc(sizeof(struct ectoplasm))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = puff;
        task->draw = ectoplasm_draw;
    }
    return task;
}

/* start a puff at pos that moves at spd, in a bluish grey. nothing starts
 * where the heap can't give both its task and its record */
static void ectoplasm_add(NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct ectoplasm *puff;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = ectoplasm_task_create()) != NULL) {
        puff = task->extra.p;
        puff->pos = *pos;
        puff->spd = *spd;
        puff->timer = PUFF_STEPS;
        puff->color.a = PUFF_ALPHA;
        puff->color.r = PUFF_RED;
        puff->color.g = PUFF_GREEN;
        puff->color.b = PUFF_BLUE;
        puff->texlist = 0;
    }
}

static void player_ectoplasm_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (player_tasks[ECTOPLASM_PLAYER(work)] == NULL ||
        (player_records[ECTOPLASM_PLAYER(work)]->flags & PLAYER_MISSED) ||
        work->timer-- <= 0) {
        task_kill(task);
        return;
    }
    ectoplasm_follow_player(work);
    pos.x = work->pos.x + ((0.5F - (rand() * RAND_SCALE)) * work->scl.y);
    pos.y = work->pos.y + ((0.5F - (rand() * RAND_SCALE)) * work->scl.y);
    pos.z = work->pos.z + ((0.5F - (rand() * RAND_SCALE)) * work->scl.y);
    spd.x = -world_gravity.x * RISE;
    spd.y = -world_gravity.y * RISE;
    spd.z = -world_gravity.z * RISE;
    ectoplasm_add(&pos, &spd);
}

void player_ectoplasm_start(int player, Sint16 steps)
{
    struct task *task;
    struct task_work *work;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, SOURCE_LEVEL, player_ectoplasm_step,
                            "PlayerEffectEctoplasmExecutor")) != NULL) {
        work = task->work;
        ECTOPLASM_PLAYER(work) = player;
        work->timer = steps;
    }
}
