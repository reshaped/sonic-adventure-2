#include "shared/deck_radar/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "object/set.h"
#include "shared/deck_scenery.h"
#include "sound/sound.h"
#include "system/task.h"

/* the set editor's label for ang.y, which no code reads */
static const char editor_rotation_y[12] = {
#include "shared/deck_radar/editor_rotation_y.inc"
};

#define DISH_TURN 144
#define RADAR_SOUND 6
#define RADAR_SOUND_STEPS 10

static NJS_TEXNAME deck_radar_textures[9] = {
#include "shared/deck_radar/deck_radar_textures.inc"
};

static NJS_TEXLIST deck_radar_texlist = {
#include "shared/deck_radar/deck_radar_texlist.inc"
};

static Sint16 deck_radar_dish_polygons[579] = {
#include "shared/deck_radar/deck_radar_dish_polygons.inc"
};

static Sint32 deck_radar_dish_vertices[531] = {
#include "shared/deck_radar/deck_radar_dish_vertices.inc"
};

static NJS_CNK_MODEL deck_radar_dish_model = {
#include "shared/deck_radar/deck_radar_dish_model.inc"
};

static NJS_CNK_OBJECT deck_radar_dish = {
#include "shared/deck_radar/deck_radar_dish.inc"
};

static Sint16 deck_radar_polygons[219] = {
#include "shared/deck_radar/deck_radar_polygons.inc"
};

static Sint32 deck_radar_vertices[183] = {
#include "shared/deck_radar/deck_radar_vertices.inc"
};

static NJS_CNK_MODEL deck_radar_model = {
#include "shared/deck_radar/deck_radar_model.inc"
};

static NJS_CNK_OBJECT deck_radar_object = {
#include "shared/deck_radar/deck_radar_object.inc"
};

NJS_TEXLIST *deck_radar_texlists[2] = {
#include "shared/deck_radar/deck_radar_texlists.inc"
};

static void deck_radar_draw(struct task *task);
static void deck_radar_step(struct task *task);
static void deck_radar_destroy(struct task *task);

void deck_radar_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = deck_radar_draw;
        task->step = deck_radar_step;
        task->destructor = deck_radar_destroy;
    }
}

static void deck_radar_destroy(struct task *task)
{
}

static void deck_radar_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->timer += DISH_TURN;
        sound_play_at_for(RADAR_SOUND, work, 1, 0, RADAR_SOUND_STEPS,
                          &work->pos);
    }
}

static void deck_radar_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&deck_radar_texlist);
    object = &deck_radar_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateY(NULL, work->timer);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
