#include "shared/deck_sign/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/deck_scenery.h"
#include "system/task.h"

/* the set editor's label for ang.y, which no code reads */
static const char editor_rotation_y[12] = {
#include "shared/deck_sign/editor_rotation_y.inc"
};

#define VIEW_MARGIN 10.0F
#define PULSE_PERIOD 80
#define PULSE_REST 40
#define PULSE_FADE 8

static NJS_TEXNAME deck_sign_textures[2] = {
#include "shared/deck_sign/deck_sign_textures.inc"
};

static NJS_TEXLIST deck_sign_texlist = {
#include "shared/deck_sign/deck_sign_texlist.inc"
};

static Sint16 deck_sign_panel_polygons[33] = {
#include "shared/deck_sign/deck_sign_panel_polygons.inc"
};

static Sint32 deck_sign_panel_vertices[27] = {
#include "shared/deck_sign/deck_sign_panel_vertices.inc"
};

static NJS_CNK_MODEL deck_sign_panel_model = {
#include "shared/deck_sign/deck_sign_panel_model.inc"
};

static NJS_CNK_OBJECT deck_sign_panel = {
#include "shared/deck_sign/deck_sign_panel.inc"
};

static Sint16 deck_sign_polygons[91] = {
#include "shared/deck_sign/deck_sign_polygons.inc"
};

static Sint32 deck_sign_vertices[51] = {
#include "shared/deck_sign/deck_sign_vertices.inc"
};

static NJS_CNK_MODEL deck_sign_model = {
#include "shared/deck_sign/deck_sign_model.inc"
};

static NJS_CNK_OBJECT deck_sign_object = {
#include "shared/deck_sign/deck_sign_object.inc"
};

NJS_TEXLIST *deck_sign_texlists[2] = {
#include "shared/deck_sign/deck_sign_texlists.inc"
};

static void deck_sign_step(struct task *task);
static void deck_sign_draw(struct task *task);
static void deck_sign_destroy(struct task *task);

void deck_sign_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = deck_sign_draw;
        task->step = deck_sign_step;
        task->destructor = deck_sign_destroy;
    }
}

static void deck_sign_destroy(struct task *task)
{
}

static void deck_sign_step(struct task *task)
{
    /* @hack redundant: the work read into a local nothing uses */
    struct task_work *work = task->work;

    (void)work;
    if (!placement_try_leave(task)) {
        collision_register(task);
    }
}

static void deck_sign_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &deck_sign_object;
    NJS_CNK_OBJECT *panel;

    if (sphere_in_front_no_depth(&work->pos, object->model->r + VIEW_MARGIN)) {
        njSetTexture(&deck_sign_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(object->model);
        panel = object->child;
        njTranslateV(NULL, (NJS_VECTOR *)panel->pos);
        model_draw_tinted(
            panel->model,
            blink_light_gray(PULSE_PERIOD, PULSE_REST, PULSE_FADE));
        njPopMatrixEx();
    }
}
