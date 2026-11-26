/* the race lands' scenery. the low 16 bits of a placement's ang.x choose its
 * land's model. its scale is 1 more than its scl.x. the first neutral and
 * hero models glow at each child node */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race.h"
#include "chaostgrace/props.h"
#include "draw/lines.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/task.h"

/* a glow's scale swings GLOW_SWING about GLOW_SCALE, as ang.z turns by
 * GLOW_TURN a frame */
#define GLOW_SCALE 0.22F
#define GLOW_SWING 0.02F
#define GLOW_TURN EIGHTH_TURN
/* race_prop_glow_cells' cells, by land */
enum { NEUTRAL_GLOW, HERO_GLOW };

static void scenery_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->ang.z += GLOW_TURN;
    }
    /* @hack redundant: a draw that never runs */
    if (0) {
        NJS_CNK_OBJECT *object = race_neutral_props[0];
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(object->model);
        njCnkEasyDrawModel(object->child->model);
        njPopMatrixEx();
    }
}

static void scenery_glow(struct task_work *work, NJS_CNK_OBJECT *object,
                         NJS_TEXLIST *textures, Int cell)
{
    NJS_SPRITE sprite;
    Float scale = GLOW_SCALE + (GLOW_SWING * njSin(work->ang.z));
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    sprite.tlist = textures;
    sprite.tanim = race_prop_glow_cells;
    sprite.ang = 0;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = 0.0F;
    sprite.p.y = 0.0F;
    sprite.p.z = 0.0F;
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njDrawSprite3D(&sprite, cell, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack redundant: a test of work that never runs */
    if (0) {
        if (work) {
        }
    }
}

/* @hack redundant: index taken % count again, below which it lies */
#define DRAW_PROP(work, scale, texlist, objects, index, count)                 \
    {                                                                          \
        njSetTexture(&(texlist));                                              \
        njPushMatrixEx();                                                      \
        njTranslateEx(&(work)->pos);                                           \
        njRotateY(NULL, (work)->ang.y);                                        \
        njScale(NULL, (scale), (scale), (scale));                              \
        njCnkEasyDrawModel((objects)[(index) % (count)]->model);               \
    }

#define DRAW_GLOWS(work, index, objects, texlist, cell)                        \
    if ((index) == 0) {                                                        \
        NJS_CNK_OBJECT *object = (objects)[(index)]->child;                    \
        while (object) {                                                       \
            scenery_glow((work), object, &(texlist), (cell));                  \
            object = object->sibling;                                          \
        }                                                                      \
    }

static void scenery_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale = 1.0F + work->scl.x;

    switch (garden_race_course()->land) {
    case RACE_LAND_NEUTRAL: {
        Sint32 index = work->ang.x % race_neutral_prop_count;
        DRAW_PROP(work, scale, race_neutral_texlist, race_neutral_props, index,
                  race_neutral_prop_count);
        DRAW_GLOWS(work, index, race_neutral_props, race_neutral_texlist,
                   NEUTRAL_GLOW);
        njPopMatrixEx();
        break;
    }
    case RACE_LAND_HERO: {
        Sint32 index = work->ang.x % race_hero_prop_count;
        DRAW_PROP(work, scale, race_hero_texlist, race_hero_props, index,
                  race_hero_prop_count);
        DRAW_GLOWS(work, index, race_hero_props, race_hero_texlist, HERO_GLOW);
        njPopMatrixEx();
        break;
    }
    case RACE_LAND_DARK: {
        Sint32 index = work->ang.x % race_dark_prop_count;
        DRAW_PROP(work, scale, race_dark_texlist, race_dark_props, index,
                  race_dark_prop_count);
        njPopMatrixEx();
        break;
    }
    }
}

static void scenery_destroy(struct task *task)
{
}

void race_scenery_init(struct task *task)
{
    struct task_work *work = task->work;
    task->step = scenery_step;
    task->draw = scenery_draw;
    task->destructor = scenery_destroy;
    work->ang.x = (Uint16)work->ang.x;
}
