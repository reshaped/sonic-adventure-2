/* the black wave: rings of dark that spread one after another from where a
 * player stood */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "effect/black_wave.h"
#include "player/player.h"
#include "system/task.h"

/* the task level of a source, and of a wave without a parent */
#define WAVE_LEVEL 3

/* a wave grows by WAVE_GROWTH a step. the step that finds its timer past
 * WAVE_LAST destroys it */
#define WAVE_GROWTH 0.3F
#define WAVE_LAST 30
/* a source starts a wave every WAVE_DELAY + 2 steps */
#define WAVE_DELAY 12

/* a wave's sprite also scales by 1 / sqrt(DEPTH_SCALE * depth), held from
 * SCALE_MIN to SCALE_MAX */
#define DEPTH_SCALE 0.04F
#define SCALE_MIN 0.2F
#define SCALE_MAX 2.0F
#define WAVE_PRIORITY (-1.0F) /* the priority a wave's sprite draws at */
/* the number of a wave's texture in the hunter's effects_texlist */
#define WAVE_TEXTURE 6

/* a wave's record, in its task's extra */
struct black_wave {
    /* the slot of player_tasks whose hunter record holds the texture list
     * the wave draws with */
    Sint8 owner;
    Sint8 player; /* a copy of the source's player, which nothing reads */
    Sint16 timer; /* the steps it has taken */
    NJS_POINT3 pos;
    Float scale; /* the sprite's scale before its depth's */
    /* 1, since only its start sets it. the draw sets the constant
     * material's alpha to alpha less 1. it gives the sprite that material's
     * color only for an alpha below 1 */
    Float alpha;
};

/* the record of a wave's source, in its task's extra */
struct black_wave_source {
    Sint8 owner; /* the owner each wave takes */
    /* the slot of the player the waves spread from. the source ends once
     * player_tasks holds no task there */
    Sint8 player;
    Sint16 count; /* the waves left to start */
    /* a count down to the next wave. the step that finds it below 0 starts a
     * wave and sets it to WAVE_DELAY */
    Sint16 delay;
    NJS_POINT3 pos; /* where each wave spreads from */
};

static void black_wave_draw(struct task *task);

/* a wave's sprite: WAVE_TEXTURE of the owner's list, 64 by 64 */
static NJS_TEXANIM black_wave_texanim = {
#include "effect/black_wave/black_wave_texanim.inc"
};
/* the sprite each wave's draw fills */
/* @hack type: an array of one, where an object would do */
static NJS_SPRITE black_wave_sprite[1] = {
#include "effect/black_wave/black_wave_sprite.inc"
};

static void black_wave_origin(int player, NJS_POINT3 *pos)
{
    struct player *record = player_records[player];

    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        *pos = record->unk_218;
        break;
    /* @hack redundant: the cases that fall into default */
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    default:
        *pos = record->unk_230;
        break;
    }
}

static void black_wave_step(struct task *task)
{
    struct black_wave *wave = task->extra.p;

    /* @hack spelling: the scale added as scale = scale + WAVE_GROWTH, where
     * scale += WAVE_GROWTH would do */
    wave->scale = wave->scale + WAVE_GROWTH;
    if (wave->timer++ > WAVE_LAST) {
        task_destroy(task);
    }
}

/* a source's waves draw as one batch. the first turns the fog off, and the
 * last turns it on */
static void black_wave_draw(struct task *task)
{
    struct black_wave *wave = task->extra.p;
    Float scale;

    /* @hack redundant: an empty test of the parent */
    if (task->parent != NULL) {
    }
    if (task->parent->child == task) {
        njFogDisable();
    }
    /* @hack spelling: view and screen in blocks of their own, where
     * declarations at the top would do */
    {
        NJS_POINT3 view;

        njCalcPoint(NULL, &wave->pos, &view);
        scale = view.z;
    }
    if (scale <= 0.0F) {
        /* @bug logic: leaves the fog off where the last wave lies behind
         * the camera. this return's BRA at 0x8c0a35ee passes over the JSR at
         * 0x8c0a36e2 that turns it on */
        return;
    }
    {
        NJS_POINT2 screen;

        scale = njInvertSqrt(scale * DEPTH_SCALE);
        if (scale < SCALE_MIN) {
            scale = SCALE_MIN;
        } else if (scale > SCALE_MAX) {
            scale = SCALE_MAX;
        }
        njProjectScreen(NULL, &wave->pos, &screen);
        black_wave_sprite->p.x = screen.x;
        black_wave_sprite->p.y = screen.y;
        black_wave_sprite->sx = black_wave_sprite->sy = wave->scale * scale;
        black_wave_sprite->tlist =
            PLAYER_RECORD(wave->owner)->by_character.hunter.effects_texlist;
        constant_material_set(wave->alpha - 1.0F, 0.0F, 0.0F, 0.0F);
        if (wave->alpha >= 1.0F) {
            njDrawSprite2D(black_wave_sprite, 0, WAVE_PRIORITY,
                           NJD_SPRITE_ALPHA);
        } else {
            njDrawSprite2D(black_wave_sprite, 0, WAVE_PRIORITY,
                           NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        if (task->parent->child->prev == task) {
            njFogEnable();
        }
    }
}

/* a wave's task with its zeroed record, which the caller fills. it is a
 * child of parent, or a task named EffectBlackWaveExecutor where parent is
 * null. it returns null where the heap can't give both */
static struct task *black_wave_task_create(struct task *parent)
{
    struct task *task;
    struct black_wave *wave;

    if (parent != NULL) {
        task = task_create_child(0, black_wave_step, parent);
    } else {
        task = task_create(0, WAVE_LEVEL, black_wave_step,
                           "EffectBlackWaveExecutor");
    }
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((wave = syCalloc(1, sizeof(struct black_wave))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = wave;
        task->draw = black_wave_draw;
    }
    return task;
}

static void black_wave_source_step(struct task *task)
{
    struct black_wave_source *source = task->extra.p;
    /* @hack spelling: player, a local for the player's task, where
     * player_tasks[source->player] in the test would do */
    struct task *player = player_tasks[source->player];
    struct task *child;
    struct black_wave *wave;

    if (player == NULL || (source->count <= 0 && task->child == NULL)) {
        task_kill(task);
        return;
    }
    if (source->delay-- < 0 && source->count > 0) {
        source->delay = WAVE_DELAY;
        source->count--;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = black_wave_task_create(task)) != NULL) {
            wave = child->extra.p;
            wave->owner = source->owner;
            wave->player = source->player;
            wave->scale = 1.0F;
            wave->alpha = 1.0F;
            wave->pos = source->pos;
        }
    }
}

/* a source's task with its zeroed record, which the caller fills. it returns
 * null where the heap can't give both */
static struct task *black_wave_source_task_create(void)
{
    struct task *task;
    struct black_wave_source *source;

    task = task_create(0, WAVE_LEVEL, black_wave_source_step,
                       "EffectBlackWaveGenerator");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((source = syCalloc(1, sizeof(struct black_wave_source))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = source;
    }
    return task;
}

void black_wave_start(int owner, int player, int count)
{
    struct task *task;
    struct black_wave_source *source;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = black_wave_source_task_create()) != NULL) {
        source = task->extra.p;
        source->owner = owner;
        source->player = player;
        source->count = count;
        black_wave_origin(player, &source->pos);
    }
}
