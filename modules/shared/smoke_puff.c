/* smoke puffs: sprites of the texture jet1 that drift from a point and fade.
 * Big Foot makes them */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "shared/mech_boss.h"
#include "shared/smoke_puff.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the bytes of the record a puff keeps in task->motion, which nothing
 * reads. its first word is the pos pointer the puff was made with */
#define ORIGIN_RECORD_SIZE 20

#define START_ALPHA 1.0F
#define END_ALPHA 0.05F
/* a puff's speed is spd times a random factor within SPEED_SPREAD / 2 of 1.
 * a SMOKE_PUFF_FAST puff's factor gains FAST_SPEED_GAIN */
#define SPEED_SPREAD 0.2F
#define FAST_SPEED_GAIN 1.0F
/* a puff's size is its kind's least size plus up to SIZE_SPREAD */
#define SIZE_SPREAD 0.08F
#define LARGE_FADE 0.12F
#define LARGE_SIZE 0.33F
#define FAST_FADE 0.25F
#define FAST_SIZE 0.3F
#define SMALL_FADE 0.12F
#define SMALL_SIZE 0.24F

/* a puff's work, the struct task_work task_create_child gives it. mode,
 * timer and pos keep their places. kind lies over submode, spd over ang, and
 * alpha, fade and size over scl */
struct smoke_puff_work {
    Sint8 mode;
    Sint8 kind;
    Uint8 unk_02[4];
    Uint16 timer; /* the steps the puff has moved */
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float alpha;
    Float fade; /* what alpha loses each step */
    Float size;
};

#define WORK(task) ((struct smoke_puff_work *)(task)->work)

/* the whole of jet1, drawn 32 pixels square about its center */
static NJS_TEXANIM smoke_puff_cell = {
#include "shared/smoke_puff/smoke_puff_cell.inc"
};
static NJS_SPRITE smoke_puff_sprite = {
#include "shared/smoke_puff/smoke_puff_sprite.inc"
};

static void smoke_puff_destroy(struct task *task);
static void smoke_puff_step(struct task *task);
static void smoke_puff_draw(struct task *task);

struct task *smoke_puff_create(struct task *parent, int kind, NJS_POINT3 *pos,
                               NJS_VECTOR *spd)
{
    struct task *task;
    struct smoke_puff_work *work;
    NJS_POINT3 **origin;
    NJS_VECTOR *speed;
    Float factor;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, smoke_puff_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = WORK(task);
        origin = syCalloc(1, ORIGIN_RECORD_SIZE);
        if (origin == NULL) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = origin;
        *origin = pos;
        work->pos = *pos;
        work->alpha = START_ALPHA;
        work->kind = kind;
        /* @hack spelling: the kind read back from the work, where kind would
         * do */
        switch (work->kind) {
        case SMOKE_PUFF_SMALL:
            work->fade = SMALL_FADE;
            work->size = (rand() * (SIZE_SPREAD * RAND_SCALE)) + SMALL_SIZE;
            break;
        case SMOKE_PUFF_FAST:
            work->fade = FAST_FADE;
            work->size = (rand() * (SIZE_SPREAD * RAND_SCALE)) + FAST_SIZE;
            break;
        case SMOKE_PUFF_LARGE:
        default:
            work->fade = LARGE_FADE;
            work->size = (rand() * (SIZE_SPREAD * RAND_SCALE)) + LARGE_SIZE;
            break;
        }
        /* @hack spelling: the work reached again through task, where work
         * would do */
        speed = &WORK(task)->spd;
        factor = (((rand() * RAND_SCALE) - 0.5F) * SPEED_SPREAD) + 1.0F;
        *speed = *spd;
        if (work->kind == SMOKE_PUFF_FAST) {
            factor += FAST_SPEED_GAIN;
        }
        speed->x *= factor;
        speed->y *= factor;
        speed->z *= factor;
        task->draw = smoke_puff_draw;
        task->destructor = smoke_puff_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* for a null parent the image returns R0 as the caller left it, which C
     * can't express. this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void smoke_puff_destroy(struct task *task)
{
}

static void smoke_puff_step(struct task *task)
{
    struct smoke_puff_work *work = WORK(task);
    /* @hack redundant: locals nothing reads, one of them initialized */
    /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
    NJS_VECTOR unused = {0.0F, 0.0F, 0.0F};
    NJS_VECTOR unused_vector;
    NJS_MATRIX unused_matrix;
    /* NOLINTEND(clang-diagnostic-unused-variable) */

    if (game_state.time_stopped) {
        return;
    }
    if (!(work->alpha > END_ALPHA)) {
        task_kill(task);
        return;
    }
    work->alpha -= work->fade;
    if (work->alpha < 0.0F) {
        work->alpha = 0.0F;
    }
    njAddVector(&work->pos, &work->spd);
    work->timer++;
}

static void smoke_puff_draw(struct task *task)
{
    struct smoke_puff_work *work = WORK(task);
    NJS_SPRITE *sprite;
    /* @hack spelling: copy, a second pointer to the sprite, where sprite
     * would do */
    NJS_SPRITE *copy;
    Float alpha = work->alpha;
    Float size = work->size;

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(alpha, alpha, alpha, alpha);
    njSetTexture(&jet1_texlist);
    sprite = &smoke_puff_sprite;
    copy = sprite;
    /* @hack spelling: sy set before sx, not in the members' order */
    copy->sy = size;
    copy->sx = size;
    copy->p = work->pos;
    njDrawSprite3D(copy, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}
