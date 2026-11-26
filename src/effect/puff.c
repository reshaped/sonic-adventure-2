/* the puffs a hunter's upgraded blows give off: small sprites of the
 * hunter's effect textures, which swell and drift away */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "effect/puff.h"
#include "player/player.h"
#include "system/task.h"
#include "system/util.h"

#define PUFF_LEVEL 3 /* a puff's task level */

/* a puff starts within PUFF_REACH of pos along each axis. its size starts at
 * PUFF_SIZE_MIN up to PUFF_SIZE_SPAN more */
#define PUFF_REACH 0.2F
#define PUFF_SIZE_MIN 0.1F
#define PUFF_SIZE_SPAN 0.04F
/* the drift is PUFF_DRIFT of the unit vector of the player's motion. each of
 * its parts then gains PUFF_DRIFT_GAIN_MAX, less up to PUFF_DRIFT_GAIN_SPAN */
#define PUFF_DRIFT 0.4F
#define PUFF_DRIFT_GAIN_MAX 0.1F
#define PUFF_DRIFT_GAIN_SPAN 0.05F
/* what a step multiplies the size and the drift by */
#define PUFF_GROWTH 1.015F
#define PUFF_FRICTION 0.95F
/* a puff's count of steps starts at random below PUFF_START_STEPS. the step
 * that takes it to PUFF_STEPS ends the puff. the puff shows a texture for
 * 1 << PUFF_TEXTURE_SHIFT steps */
#define PUFF_START_STEPS 4
#define PUFF_STEPS 24
#define PUFF_TEXTURE_SHIFT 2

/* a puff, the extra of its task */
struct puff {
    Sint8 mode; /* 0, which nothing reads */
    Sint8 slot; /* the player whose textures it draws with */
    Sint16 steps; /* the count of its steps, to PUFF_STEPS */
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float size;
};

/* @hack type: an array of one, where an object would do */
static NJS_TEXANIM puff_cells[1] = {
#include "effect/puff/puff_cells.inc"
};
/* the sprite each draw sets up before it draws */
static NJS_SPRITE puff_sprite = {
#include "effect/puff/puff_sprite.inc"
};

/* a task named exec that runs step, with a puff for its extra, which the
 * caller fills. it returns null where the heap can't give both */
static struct task *hunter_puff_task_create(void (*step)(struct task *task))
{
    struct task *task;
    struct puff *puff;

    task = task_create(0, PUFF_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((puff = syMalloc(sizeof(struct puff))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = puff;
    }
    return task;
}

static void hunter_puff_draw(struct task *task)
{
    struct puff *puff = task->extra.p;
    struct player *record;

    record = player_tasks[puff->slot]->extra.p;
    puff_sprite.tlist = record->by_character.hunter.effects_texlist;
    puff_cells[0].texid = puff->steps >> PUFF_TEXTURE_SHIFT;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(puff_sprite.tlist);
    puff_sprite.sx = puff_sprite.sy = puff->size;
    puff_sprite.p = puff->pos;
    njDrawSprite3D(&puff_sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void hunter_puff_destroy(struct task *task)
{
}

/* task_kill destroys the task only at its next step, so the last step also
 * takes the draw away */
static void hunter_puff_step(struct task *task)
{
    struct puff *puff = task->extra.p;

    /* @hack spelling: the increment inside the test, where a statement of
     * its own would do */
    if (++puff->steps >= PUFF_STEPS) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    puff->size *= PUFF_GROWTH;
    njAddVector(&puff->pos, &puff->spd);
    puff->spd.x *= PUFF_FRICTION;
    puff->spd.y *= PUFF_FRICTION;
    puff->spd.z *= PUFF_FRICTION;
}

void hunter_puff_start(int slot, NJS_POINT3 *pos)
{
    struct task *task;
    struct puff *puff;

    task = hunter_puff_task_create(hunter_puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->slot = slot;
        puff->steps = rand() * (PUFF_START_STEPS * RAND_SCALE);
        puff->mode = 0;
        puff->pos = *pos;
        puff->pos.x += PUFF_REACH - (rand() * (2 * PUFF_REACH * RAND_SCALE));
        puff->pos.y += PUFF_REACH - (rand() * (2 * PUFF_REACH * RAND_SCALE));
        puff->pos.z += PUFF_REACH - (rand() * (2 * PUFF_REACH * RAND_SCALE));
        puff->size = (rand() * (PUFF_SIZE_SPAN * RAND_SCALE)) + PUFF_SIZE_MIN;
        puff->spd = player_motion[slot]->spd;
        njUnitVector(&puff->spd);
        puff->spd.x *= PUFF_DRIFT;
        puff->spd.y *= PUFF_DRIFT;
        puff->spd.z *= PUFF_DRIFT;
        puff->spd.x += PUFF_DRIFT_GAIN_MAX -
                       (rand() * (PUFF_DRIFT_GAIN_SPAN * RAND_SCALE));
        puff->spd.y += PUFF_DRIFT_GAIN_MAX -
                       (rand() * (PUFF_DRIFT_GAIN_SPAN * RAND_SCALE));
        puff->spd.z += PUFF_DRIFT_GAIN_MAX -
                       (rand() * (PUFF_DRIFT_GAIN_SPAN * RAND_SCALE));
        task->destructor = hunter_puff_destroy;
        task->draw = hunter_puff_draw;
    }
}
