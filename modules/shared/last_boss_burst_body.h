/* shared/last_boss_sphere.h's trails and bursts, which each fight's spheres.c
 * includes once. the includer defines these:
 * - TRAIL_NAME, the name of a trail's task;
 * - TRAIL_LEVEL, the task level of a trail without a parent;
 * - TRAIL_LIFE, the steps a trail lasts;
 * - TRAIL_COLOR_COUNT, the count of trail_colors;
 * - BURST_TEXLIST, the rings' textures;
 * - BURST_MODEL, the flash's model.
 *
 * the Biolizard's fight also defines TRAIL_CROSSED and RINGS_FOLLOW_ORIGIN.
 * TRAIL_CROSSED draws a trail twice, crossed, at any distance. without it, a
 * trail draws only within TRAIL_REACH of camera 0. RINGS_FOLLOW_ORIGIN moves a
 * ring with *position. without it, a ring stays where its sphere's center was
 * as the ring began */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "shared/last_boss_sphere.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

#define TRAIL_REACH 500.0F

/* a burst's flash and rings, by enum last_boss_burst_kind. the flash's timer
 * counts down from lifetime. the flash shows while the timer's remainder by
 * blink_period is below half of it. the draw adds the flash's alpha to its
 * model's, and the alpha falls flash_fade a step. the flash ends at
 * FLASH_GONE, or after lifetime steps where lifetime isn't ENDLESS. ring_count
 * rings follow one another, each made as the one before it ends. a ring grows
 * from ring_size by ring_growth a step up to ring_limit. it then fades
 * ring_fade a step, and ends below RING_GONE */
#define ENDLESS 0xffff
#define FLASH_GONE (-1.0F)
#define RING_GONE 0.05F
struct burst_style {
    Uint16 lifetime;
    Uint16 blink_period;
    Sint16 ring_count;
    Float ring_size;
    Float ring_limit;
    Float ring_growth;
    Float ring_fade;
    Float flash_fade;
};

/* a flash's or a ring's struct task_work, as these tasks lay out its words.
 * rings counts this ring and those after it. style is the burst's kind, and
 * origin is last_boss_sphere_burst's position */
struct burst_work {
    Sint8 mode;
    Sint8 rings;
    Uint8 id;
    Uint8 style;
    Sint16 flags;
    Uint16 timer;
    const NJS_POINT3 *origin;
    Angle ang_y;
    Angle ang_z;
    NJS_POINT3 pos;
    Float size;
    Float unused_24;
    Float alpha;
};
#define BURST_WORK(task) ((struct burst_work *)(task)->work)

extern NJS_CNK_OBJECT BURST_MODEL;

/* a trail's strip has TRAIL_VERTICES vertices, and draws scaled by
 * TRAIL_WIDTH in x and y. TRANSLUCENT is njDrawPolygon3DEx's flag for a
 * translucent strip */
#define TRAIL_VERTICES 8
#define TRAIL_WIDTH 1.2F
#define TRANSLUCENT 1
static NJS_POLYGON_VTX trail_strip[TRAIL_VERTICES] = {
#include "shared/last_boss_burst_body/trail_strip.inc"
};
#ifdef NONMATCHING
/* this side keeps the datum in .data, where the image holds it. nothing writes
 * it, so GCC would fold it into the code or place it among the constants */
__attribute__((used, section(".data")))
#endif
static Uint32 trail_colors[TRAIL_COLOR_COUNT] = {
#include "shared/last_boss_burst_body/trail_colors.inc"
};
static struct burst_style styles[LAST_BOSS_BURST_KINDS] = {
    {30, 10, 1, .25F, .625F, .05F, .06F, 0},
    {60, 6, 2, .1F, .42F, .02F, .1F, .008F},
};
static NJS_TEXANIM ring_cell = {
#include "shared/last_boss_burst_body/ring_cell.inc"
};
/* @hack type: a one-element sprite array in place of the sprite itself */
static NJS_SPRITE ring[1] = {
#include "shared/last_boss_burst_body/ring.inc"
};

/* set angles to turn the z axis toward delta, or leave them where delta has
 * length 0 */
static void aim_trail(NJS_VECTOR *delta, struct angles *angles)
{
    Float length = njScalor(delta);
    if (length != 0.0F) {
        angles->y = atan2f(delta->x, delta->z) * ANGLE_PER_RADIAN;
#ifdef TRAIL_CROSSED
        angles->x = asinf(-delta->y / length) * ANGLE_PER_RADIAN;
#else
        angles->x =
            asinf(-(delta->y * njInvertSqrt(length) * njInvertSqrt(length))) *
            ANGLE_PER_RADIAN;
#endif
        angles->z = 0;
    }
}

#ifdef NONMATCHING
/* whether trail_colors holds colors age and age + 1, and either has alpha */
static Bool trail_has_color(Sint16 age)
{
    return age >= 0 && age + 1 < TRAIL_COLOR_COUNT &&
           ((trail_colors[age] | trail_colors[age + 1]) >> 24) != 0;
}
#endif

/* a trail's work holds its age in timer and its length in scl.x. its ang turns
 * the z axis along it */
static void sphere_trail_draw(struct task *task)
{
#ifdef TRAIL_CROSSED
    struct task_work *work = task->work;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. a trail whose age
     * reads past trail_colors draws nothing */
    Sint16 age = work->timer;
    if (!trail_has_color(age)) {
        return;
    }
#else
    /* @bug memory: a trail's last draw reads the color after trail_colors, at
     * 0x8c508c04. its last step brings its age to TRAIL_LIFE, the palette's
     * last index, at 0x8c508c9e */
    Sint8 age = work->timer;
#endif
    trail_strip[0].col = trail_strip[2].col = trail_strip[4].col =
        trail_strip[6].col = trail_colors[age];
    trail_strip[1].col = trail_strip[3].col = trail_strip[5].col =
        trail_strip[7].col = trail_colors[age + 1];
#else
    struct task_work *work = task->work;
#ifdef NONMATCHING
    /* the age as trail_has_color takes it */
    Sint16 age;
#else
    Sint8 age;
#endif
    if (vector_distance(&camera_views[0]->pos, &work->pos) > TRAIL_REACH) {
        return;
    }
    age = work->timer;
#ifdef NONMATCHING
    /* this side draws no trail whose age reads past trail_colors, as the
     * crossed trail's draw above */
    if (!trail_has_color(age)) {
        return;
    }
#endif
    trail_strip[0].col = trail_strip[2].col = trail_strip[4].col =
        trail_strip[6].col = trail_colors[age];
    trail_strip[1].col = trail_strip[3].col = trail_strip[5].col =
        trail_strip[7].col = trail_colors[age + 1];
#endif
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njScale(NULL, TRAIL_WIDTH, TRAIL_WIDTH, work->scl.x);
#ifdef TRAIL_CROSSED
    njDrawPolygon3DEx(trail_strip, TRAIL_VERTICES, TRANSLUCENT);
    njRotateX(NULL, QUARTER_TURN);
    njDrawPolygon3DEx(trail_strip, TRAIL_VERTICES, TRANSLUCENT);
#else
    njDrawPolygon3DEx(trail_strip, TRAIL_VERTICES, TRANSLUCENT);
#endif
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void trail_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!game_state.time_stopped && !game_state.unk_09) {
        if (++work->timer >= TRAIL_LIFE) {
            task_kill(task);
        }
    }
}

static void trail_destroy(struct task *task)
{
}

void last_boss_sphere_trail(struct task *parent, NJS_POINT3 *from,
                            const NJS_POINT3 *to)
{
    struct task *task;
    struct task_work *work;
    NJS_VECTOR direction;
    if (!parent) {
        task = task_create(TASK_WORK, TRAIL_LEVEL, trail_step, TRAIL_NAME);
    } else {
        task = task_create_child(TASK_WORK, trail_step, parent);
    }
    if (task) {
#ifndef TRAIL_CROSSED
        task->destructor = trail_destroy;
        task->draw = sphere_trail_draw;
#endif
        work = task->work;
        work->timer = 0;
        work->mode = 0;
        work->pos = *from;
        direction = *to;
        njSubVector(&direction, from);
        aim_trail(&direction, &work->ang);
        work->scl.x = njScalor(&direction);
#ifdef TRAIL_CROSSED
        task->destructor = trail_destroy;
        task->draw = sphere_trail_draw;
#endif
    }
}

static struct task *ring_create(struct task *parent, const NJS_POINT3 *origin,
                                Sint8 rings, Sint8 style);

static void burst_ring_step(struct task *task)
{
    struct burst_work *work = BURST_WORK(task);
    struct burst_style *style = &styles[work->style];
    if (!game_state.time_stopped && !game_state.unk_09) {
        if (work->size < style->ring_limit) {
            work->size += style->ring_growth;
        } else {
            work->alpha -= style->ring_fade;
        }
        if (work->alpha < RING_GONE) {
            ring_create(task->parent, work->origin, work->rings - 1,
                        work->style);
            task_kill(task);
        }
#ifdef RINGS_FOLLOW_ORIGIN
        {
            /* @hack spelling: pos, a copy of origin, where
             * work->pos = *work->origin would do */
            const NJS_POINT3 *pos = work->origin;
            work->pos = *pos;
        }
#endif
    }
}

static void ring_draw(struct task *task)
{
    struct burst_work *work = BURST_WORK(task);
    Float alpha = work->alpha;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(alpha, alpha, alpha, alpha);
    njSetTexture(&BURST_TEXLIST);
    ring->sx = ring->sy = work->size;
    ring->p = work->pos;
    njDrawSprite3D(ring, 0,
                   NJD_SPRITE_COLOR | NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA);
    constant_material_set(0, 0, 0, 0);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void ring_destroy(struct task *task)
{
}

static struct task *ring_create(struct task *parent, const NJS_POINT3 *origin,
                                Sint8 rings, Sint8 style)
{
    struct task *task;
    struct burst_work *work;
    if (rings <= 0 || !parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, burst_ring_step, parent);
    if (!task) {
        return NULL;
    }
#ifndef RINGS_FOLLOW_ORIGIN
    task->draw = ring_draw;
    task->destructor = ring_destroy;
#endif
    work = BURST_WORK(task);
#ifdef RINGS_FOLLOW_ORIGIN
    work->origin = origin;
    work->pos = *origin;
#else
    {
        /* @hack spelling: owner, pos and sphere, where one assignment from
         * task->parent->extra's center would do */
        struct task *owner = task->parent;
        NJS_POINT3 *pos = &work->pos;
        struct last_boss_sphere *sphere = owner->extra.p;
        *pos = sphere->center;
    }
#endif
    work->size = styles[style].ring_size;
    work->alpha = 1.0F;
    work->rings = rings;
    work->style = style;
#ifdef RINGS_FOLLOW_ORIGIN
    task->draw = ring_draw;
    task->destructor = ring_destroy;
#endif
    return task;
}

static void sphere_flash_step(struct task *task)
{
    struct burst_work *work = BURST_WORK(task);
    if (!game_state.time_stopped && !game_state.unk_09) {
        struct last_boss_sphere *sphere = task->parent->extra.p;
        /* @hack spelling: pos, the address of work->pos, where
         * work->pos = sphere->center would do */
        NJS_POINT3 *pos = &work->pos;
        *pos = sphere->center;
        work->alpha -= styles[work->style].flash_fade;
        if (work->alpha <= FLASH_GONE) {
            task_kill(task);
#ifndef RINGS_FOLLOW_ORIGIN
            return;
#endif
        }
        /* @hack spelling: two nested ifs, where one if of both tests would
         * do */
        if (work->timer < ENDLESS) {
            if (--work->timer == 0) {
                task_kill(task);
            }
        }
    }
}

static void sphere_flash_draw(struct task *task)
{
    struct burst_work *work = BURST_WORK(task);
    Uint16 period = styles[work->style].blink_period;
    Float alpha;
    if (work->timer % period < period * 0.5F) {
        draw_attr_save();
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR |
                         NJD_CONTROL_3D_OFFSET_MATERIAL);
        draw_attr_set(0, NJD_FST_UA);
        light_apply(LAST_BOSS_EFFECT_LIGHT);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        alpha = work->alpha;
        constant_material_set(alpha, alpha, alpha, alpha);
        njCnkEasyDrawObject(&BURST_MODEL);
        njPopMatrixEx();
        light_apply(light_after_draw);
        constant_material_set(0, 0, 0, 0);
        draw_attr_restore();
        draw_control_restore();
    }
}

static void flash_destroy(struct task *task)
{
}

struct task *last_boss_sphere_burst(struct task *parent,
                                    const NJS_POINT3 *position, Sint8 kind)
{
    struct task *task;
    struct burst_work *work;
    struct burst_style *style = &styles[kind];
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, sphere_flash_step, parent);
    if (!task) {
        return NULL;
    }
#ifndef RINGS_FOLLOW_ORIGIN
    task->draw = sphere_flash_draw;
    task->destructor = flash_destroy;
#endif
    work = BURST_WORK(task);
    work->pos = *position;
    work->timer = style->lifetime;
    work->style = kind;
    work->origin = position;
    work->alpha = 0.0F;
#ifdef RINGS_FOLLOW_ORIGIN
    task->draw = sphere_flash_draw;
    task->destructor = flash_destroy;
#endif
    ring_create(parent, position, style->ring_count, kind);
    return task;
}
