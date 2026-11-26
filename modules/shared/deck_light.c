#include "shared/deck_light/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "effect/particle.h"
#include "object/set.h"
#include "shared/deck_scenery.h"
#include "shared/vertex_colors.h"
#include "system/task.h"

/* the set editor's labels for ang.x, ang.y and ang.z, which no code reads */
static const char editor_angle_x[12] = {
#include "shared/deck_light/editor_angle_x.inc"
};
static const char editor_angle_y[12] = {
#include "shared/deck_light/editor_angle_y.inc"
};
static const char editor_angle_z[12] = {
#include "shared/deck_light/editor_angle_z.inc"
};

static NJS_TEXNAME deck_light_textures[2] = {
#include "shared/deck_light/deck_light_textures.inc"
};

static NJS_TEXLIST deck_light_texlist = {
#include "shared/deck_light/deck_light_texlist.inc"
};

static Sint16 deck_light_lens_polygons[25] = {
#include "shared/deck_light/deck_light_lens_polygons.inc"
};

static Sint32 deck_light_lens_vertices[19] = {
#include "shared/deck_light/deck_light_lens_vertices.inc"
};

static NJS_CNK_MODEL deck_light_lens_model = {
#include "shared/deck_light/deck_light_lens_model.inc"
};

static NJS_CNK_OBJECT deck_light_lens = {
#include "shared/deck_light/deck_light_lens.inc"
};

static Sint16 deck_light_polygons[65] = {
#include "shared/deck_light/deck_light_polygons.inc"
};

static Sint32 deck_light_vertices[35] = {
#include "shared/deck_light/deck_light_vertices.inc"
};

static NJS_CNK_MODEL deck_light_model = {
#include "shared/deck_light/deck_light_model.inc"
};

static NJS_CNK_OBJECT deck_light_object = {
#include "shared/deck_light/deck_light_object.inc"
};

static NJS_TEXNAME deck_light_w_textures[2] = {
#include "shared/deck_light/deck_light_w_textures.inc"
};

static NJS_TEXLIST deck_light_w_texlist = {
#include "shared/deck_light/deck_light_w_texlist.inc"
};

static Sint16 deck_light_w_lens_polygons[65] = {
#include "shared/deck_light/deck_light_w_lens_polygons.inc"
};

static Sint32 deck_light_w_lens_vertices[67] = {
#include "shared/deck_light/deck_light_w_lens_vertices.inc"
};

static NJS_CNK_MODEL deck_light_w_lens_model = {
#include "shared/deck_light/deck_light_w_lens_model.inc"
};

static NJS_CNK_OBJECT deck_light_w_lens = {
#include "shared/deck_light/deck_light_w_lens.inc"
};

static Sint16 deck_light_w_polygons[221] = {
#include "shared/deck_light/deck_light_w_polygons.inc"
};

static Sint32 deck_light_w_vertices[131] = {
#include "shared/deck_light/deck_light_w_vertices.inc"
};

static NJS_CNK_MODEL deck_light_w_model = {
#include "shared/deck_light/deck_light_w_model.inc"
};

static NJS_CNK_OBJECT deck_light_w_object = {
#include "shared/deck_light/deck_light_w_object.inc"
};

NJS_TEXLIST *deck_light_texlists[3] = {
#include "shared/deck_light/deck_light_texlists.inc"
};
/* the lamp's color pulses between dark and bright as the work's timer gains
 * pulse_speed a step */
static Angle pulse_speed = 512;
static Uint32 dark = 0xff606060;
static Uint32 bright = 0xffffffff;

static void deck_light_step(struct task *task);
static void deck_light_draw(struct task *task);
static void deck_light_w_draw(struct task *task);
static void deck_light_destroy(struct task *task);

void deck_light_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = deck_light_draw;
        task->step = deck_light_step;
        task->destructor = deck_light_destroy;
    }
}

static void deck_light_destroy(struct task *task)
{
}

static void deck_light_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->timer += pulse_speed;
    }
    /* @hack redundant: two model draws no path reaches */
    if (0) {
        NJS_CNK_OBJECT *object = &deck_light_object;
        njCnkEasyDrawModel(object->model);
        njCnkEasyDrawModel(object->child->model);
    }
}

/* @hack spelling: a macro of a bare block, where a function of the texture
 * list and the node would do */
#define DRAW_LIGHT(texlist, node)                                              \
    {                                                                          \
        struct task_work *work = task->work;                                   \
        NJS_CNK_OBJECT *object;                                                \
        Uint32 color;                                                          \
                                                                               \
        njSetTexture(&(texlist));                                              \
        njPushMatrix(NULL);                                                    \
        njTranslateV(NULL, &work->pos);                                        \
        njRotateZ(NULL, work->ang.z);                                          \
        njRotateY(NULL, work->ang.y);                                          \
        njRotateX(NULL, work->ang.x);                                          \
        object = &(node);                                                      \
        njCnkEasyDrawModel(object->model);                                     \
        color = color_blend(dark, bright, njAbs(njSin(work->timer)));          \
        object = object->child;                                                \
        vlist_set_color(object->model->vlist, color);                          \
        njCnkEasyDrawModel(object->model);                                     \
        njPopMatrix(1);                                                        \
    }

static void deck_light_draw(struct task *task)
{
    DRAW_LIGHT(deck_light_texlist, deck_light_object);
}

void deck_light_w_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = deck_light_w_draw;
        task->step = deck_light_step;
        task->destructor = deck_light_destroy;
    }
}

static void deck_light_w_draw(struct task *task)
{
    DRAW_LIGHT(deck_light_w_texlist, deck_light_w_object);
}
