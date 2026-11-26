/* the dock marker, DMK in Mad Space's object list: a solid marker whose lamp
 * blinks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg44/dock_marker.h"
#include "stg44/scenery_shapes.h"
#include "system/port.h"
#include "system/task.h"

#define MARKERS 4
/* the marker's node in the collision world, a copy of
 * mad_space_marker_collision. the node takes the size class that its model's
 * radius plus CLASS_MARGIN fits */
#define NODE(task) ((NJS_OBJECT *)(task)->motion.p)
#define CLASS_MARGIN 15.0F
/* the work keeps WORLD_ADDED only while a player lies within NEAR_RANGE of
 * the node */
#define NEAR_RANGE 22.0F
/* the lamp's color is blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE) of
 * the way from bright_color to dim_color */
#define BLINK_PERIOD 16
#define BLINK_REST 12
#define BLINK_FADE 3

NJS_TEXLIST *mad_space_marker_textures[MARKERS + 1] = {
#include "stg44/dock_marker/mad_space_marker_textures.inc"
};
KEPT static NJS_CNK_OBJECT *models[MARKERS] = {
#include "stg44/dock_marker/models.inc"
};
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;

static void marker_destroy(struct task *task);
static void marker_draw(struct task *task);
static void marker_step(struct task *task);

void mad_space_marker_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;
    /* @hack type: flags a Uint32, where the Sint32 world_add takes would do */
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->step = marker_step;
    task->draw = marker_draw;
    task->destructor = marker_destroy;
    *NODE(task) = mad_space_marker_collision;
    object = NODE(task);
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    reach = object->model->r + CLASS_MARGIN;
    flags = WORLD_SOLID;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
}

static void marker_destroy(struct task *task)
{
    if (NODE(task)->model) {
        world_remove(task, NODE(task));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void marker_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 kind = (Uint32)work->scl.x % MARKERS;
    NJS_CNK_OBJECT *object;
    /* @hack type: color a Sint32, where the Uint32 color_blend returns would
     * do */
    Sint32 color;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD Y";
        (void)"<- MODEL(0-3)";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njSetTexture(mad_space_marker_textures[kind]);
    object = models[kind];
    njCnkEasyDrawModel(object->model);
    color = color_blend(bright_color, dim_color,
                        blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
    object = object->child;
    model_draw_tinted(object->model, color);
    njPopMatrixEx();
}

static void marker_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: the else after the return, where an if of its own would
     * do */
    if (placement_try_leave(task)) {
        return;
    } else if (NODE(task)->model) {
        if (player_near((NJS_POINT3 *)NODE(task)->pos, NEAR_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}
