#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_last1/breath.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "shared/dust.h"
#include "shared/last_boss_arena.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

enum breath_kind { WHITE_BREATH, BLACK_BREATH };
/* a puff's record, in its task's extra */
struct breath {
    Sint8 kind;
    Sint8 frame; /* its texture in dust_texlist */
    Float distance; /* how far it has moved */
    Float limit; /* how far it moves before it slows and rises */
};
/* a puff's modes, in its work's submode */
enum { BREATH_OUT, BREATH_RISE };
/* the frames of dust_texlist a puff steps through, a texture each, before it
 * dies */
#define BREATH_FRAMES 8
#define CELL_SIZE 32
#define SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR)
/* how far each way a puff's heading may turn from its direction's */
#define WHITE_CONE 6144.0F
#define BLACK_CONE 12288.0F
/* a rising puff keeps RISE_DRAG of its speed across the ground each step.
 * its width takes width_step each step, but stays MIN_WIDTH or more */
#define RISE_DRAG 0.95F
#define MIN_WIDTH 0.15F

static Float width_step = -0.075F;
static Float height_step = 0.075F;
static Float white_aspect = 0.8F;
static int lifespan = 48;
static Float initial_size = 14.0F;
static Float lift = 0.008F;
/* @hack type: cell as an array of one, where a single NJS_TEXANIM would
 * do */
static NJS_TEXANIM cell[1] = {
#include "boss_last1/breath/cell.inc"
};
static NJS_SPRITE sprite = {{0, 0, 0}, 0, 0, 0, NULL, cell};
static Float white_radius = 5.0F;
static Float white_radius_range = 5.0F;
static Float white_jitter = 5.0F;
static Float unused_white = 5.0F;
static Float white_speed = 0.84F;
static Float white_height = 15.0F;
static Float white_rise = 0.01F;
static Float black_radius = 5.0F;
static Float black_radius_range = 5.0F;
static Float black_jitter = 10.0F;
static Float unused_black = 5.0F;
static Float black_speed = 1.2F;
static Float black_rise = 0.05F;
static Float black_height = 15.0F;

static void breath_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    struct breath *puff = task->extra.p;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->submode) {
    case BREATH_OUT:
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        puff->distance += njScalor(&motion->spd);
        if (puff->distance >= puff->limit) {
            ++work->submode;
        }
        break;
    case BREATH_RISE:
        motion->spd.x *= RISE_DRAG;
        motion->spd.z *= RISE_DRAG;
        motion->spd.y += lift;
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        work->scl.x += width_step;
        work->scl.y += height_step;
        if (work->scl.x < MIN_WIDTH) {
            work->scl.x = MIN_WIDTH;
        }
        break;
    }
    if (--work->timer == 0) {
        ++puff->frame;
        if (puff->frame == BREATH_FRAMES) {
            puff->frame = BREATH_FRAMES - 1;
            task_kill(task);
            return;
        }
        work->timer = work->tick;
    }
}

static void breath_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct breath *puff = task->extra.p;
    NJS_SPRITE *draw;
    Float color;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw = &sprite;
    draw->p = work->pos;
    draw->sx = work->scl.x * (1.0F / CELL_SIZE);
    draw->sy = work->scl.y * (1.0F / CELL_SIZE);
    cell[0].texid = puff->frame;
    draw->tlist = &dust_texlist;
    if (puff->kind == WHITE_BREATH) {
        color = 1.0F;
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    } else if (puff->kind == BLACK_BREATH) {
        color = 0.0F;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: a puff of any other kind draws
     * black */
    else {
        color = 0.0F;
    }
#endif
    /* @bug uninitialized: reads color unset at 0x8c50b24a for a kind other
     * than WHITE_BREATH and BLACK_BREATH */
    constant_material_set(1.0F, color, color, color);
    njSetTexture(&dust_texlist);
    njDrawSprite3D(draw, 0, SPRITE_ATTR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

static void breath_destroy(struct task *task)
{
}
static struct task *breath_create(const NJS_POINT3 *pos,
                                  const NJS_VECTOR *speed, Float limit,
                                  Sint8 kind)
{
    struct task *task;
    struct task_work *work;
    struct task_motion *motion;
    struct breath *puff;
    task =
        task_create(TASK_WORK | TASK_MOTION, 2, breath_step, "BL1BreathExec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    motion = task->motion.p;
    puff = syCalloc(1, sizeof(*puff));
    if (!puff) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = puff;
    puff->kind = kind;
    switch (puff->kind) {
    case WHITE_BREATH:
        work->scl.x = white_aspect * initial_size;
        work->scl.y = initial_size;
        break;
    case BLACK_BREATH:
        work->scl.x = initial_size;
        work->scl.y = initial_size;
        break;
    }
    work->pos = *pos;
    work->tick = lifespan / BREATH_FRAMES;
    work->timer = work->tick;
    puff->frame = 0;
    motion->spd = *speed;
    puff->limit = limit;
    task->draw = breath_draw;
    task->destructor = breath_destroy;
    return task;
}

/* @hack spelling: the scatter written into each caller by a macro, where
 * a function that takes a record of the kind's tuning would do */
#define SCATTER(origin, direction, limit, kind, color, cone)                   \
    do {                                                                       \
        NJS_VECTOR speed;                                                      \
        NJS_POINT3 pos;                                                        \
        Angle heading;                                                         \
        Float radius;                                                          \
        Float remaining;                                                       \
        pos = *(origin);                                                       \
        heading = atan2f((direction)->x, (direction)->z) * ANGLE_PER_RADIAN;   \
        heading += (cone) - (rand() * (2.0F * (cone) * RAND_SCALE));           \
        radius =                                                               \
            (rand() * (color##_radius_range) * RAND_SCALE) + (color##_radius); \
        pos.x += radius * njSin(heading);                                      \
        /* @bug logic: offsets a black puff's height by white_jitter, where    \
         * black_jitter is meant, so it starts from 15 below black_height to   \
         * 5 above, not 10 either side. biolizard_black_breath reads           \
         * white_jitter at 0x8c50b56c */                                       \
        pos.y += white_jitter -                                                \
                 ((color##_jitter) * rand() * (2 * RAND_SCALE)) +              \
                 (color##_height);                                             \
        pos.z += radius * njCos(heading);                                      \
        remaining =                                                            \
            (limit) - njSqrt(((pos.x - (origin)->x) * (pos.x - (origin)->x)) + \
                             ((pos.z - (origin)->z) * (pos.z - (origin)->z))); \
        vector_diff(&pos, (origin), &speed);                                   \
        speed.y = 0.0F;                                                        \
        last_boss_vector_length(&speed, (color##_speed), &speed);              \
        speed.y = (color##_rise);                                              \
        breath_create(&pos, &speed, remaining, (kind));                        \
    } while (0)

void biolizard_white_breath(const NJS_POINT3 *origin,
                            const NJS_VECTOR *direction, Float limit)
{
    SCATTER(origin, direction, limit, WHITE_BREATH, white, WHITE_CONE);
}
void biolizard_black_breath(const NJS_POINT3 *origin,
                            const NJS_VECTOR *direction, Float limit)
{
    SCATTER(origin, direction, limit, BLACK_BREATH, black, BLACK_CONE);
}
