#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg27/red_lamps.h"
#include "system/task.h"

#define LAMP_RADIUS 80.0F
#define BLACK 0xff000000
#define WHITE 0xffffffff
/* the lens's blink_level arguments, in steps */
#define BLINK_PERIOD 80
#define BLINK_REST 40
#define BLINK_FADE 8

/* the set editor's label for the lamps' ang.y */
static const char editor_rotation[12] = "<- ROT Y";

const char red_lamp_texture_name[16] = {
#include "stg27/red_lamps/red_lamp_texture_name.inc"
};
static NJS_TEXNAME textures[1] = {
#include "stg27/red_lamps/textures.inc"
};
NJS_TEXLIST red_lamp_a_texlist = {
#include "stg27/red_lamps/red_lamp_a_texlist.inc"
};

/* red_lamp_a_object is REDLA's housing. the lens is its child */

static Sint16 red_lamp_a_lens_polygons[175] = {
#include "stg27/red_lamps/red_lamp_a_lens_polygons.inc"
};

static Sint32 red_lamp_a_lens_vertices[103] = {
#include "stg27/red_lamps/red_lamp_a_lens_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_a_lens_model = {
#include "stg27/red_lamps/red_lamp_a_lens_model.inc"
};

static NJS_CNK_OBJECT red_lamp_a_lens_object = {
#include "stg27/red_lamps/red_lamp_a_lens_object.inc"
};

static Sint16 red_lamp_a_polygons[193] = {
#include "stg27/red_lamps/red_lamp_a_polygons.inc"
};

static Sint32 red_lamp_a_vertices[131] = {
#include "stg27/red_lamps/red_lamp_a_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_a_model = {
#include "stg27/red_lamps/red_lamp_a_model.inc"
};

static NJS_CNK_OBJECT red_lamp_a_object[1] = {
#include "stg27/red_lamps/red_lamp_a_object.inc"
};

static void red_lamp_destroy(struct task *task);
static void red_lamp_step(struct task *task);
static void red_lamp_a_draw(struct task *task);
static void red_lamp_b_draw(struct task *task);

/* draw object's child, the lens, tinted from black to white by blink_level of
 * the BLINK_ constants. it leaves object and color as the lens and its tint */
#define DRAW_LENS(object, color)                                               \
    do {                                                                       \
        (object) = (object)->child;                                            \
        njTranslateV(NULL, (NJS_VECTOR *)(object)->pos);                       \
        (color) = color_blend(                                                 \
            BLACK, WHITE, blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));  \
        model_draw_tinted((object)->model, color);                             \
    } while (0)

void red_lamp_a_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = red_lamp_a_draw;
    task->step = red_lamp_step;
    task->destructor = red_lamp_destroy;
}

static void red_lamp_destroy(struct task *task)
{
}

static void red_lamp_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
}

static void red_lamp_a_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    /* @hack type: the packed color an int, where Uint32 would do */
    int color;

    if (!sphere_in_front_no_depth(&work->pos, LAMP_RADIUS)) {
        return;
    }
    njSetTexture(&red_lamp_a_texlist);
    object = red_lamp_a_object;
    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    DRAW_LENS(object, color);
    njPopMatrixEx();
    njFogEnable();
}

void red_lamp_b_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = red_lamp_b_draw;
    task->step = red_lamp_step;
    task->destructor = red_lamp_destroy;
}

static void red_lamp_b_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    /* @hack type: the packed color an int, where Uint32 would do */
    int color;

    if (!sphere_in_front_no_depth(&work->pos, LAMP_RADIUS)) {
        return;
    }
    njSetTexture(&red_lamp_b_texlist);
    object = red_lamp_b_object;
    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(object->model);
    DRAW_LENS(object, color);
    njPopMatrixEx();
    njFogEnable();
}
