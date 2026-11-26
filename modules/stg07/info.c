/* MIINFO in Aquatic Mine's object list: a solid model whose face shows one of
 * three textures */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg07/info.h"
#include "system/task.h"

#include "stg07/info/text.inc"

#include "stg07/scenery/info.h"

/* the object's record, in task->motion */
struct info {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 center; /* the collision model's center, in the world */
};

#define INFO(task) ((struct info *)(task)->motion.p)

/* the copy collides while a player lies within the model's radius plus
 * REACH_MARGIN of its center. the same sum gives its size class */
#define REACH_MARGIN 15.0F
/* the face's texture index lies in the low 13 bits of FACE_TEXTURE_WORD, a
 * halfword of its polygon list. that halfword is the second of the list's
 * NJD_CT_TID chunk. the draw sets the index to one of FACE_TEXTURES, and
 * clears the index and NJD_FSS first */
#define FACE_TEXTURES 3
#define FACE_TEXTURE_WORD 7
#define TEXTURE_INDEX_BITS 0x1fff

NJS_TEXLIST *info_texlists[3] = {
#include "stg07/info/info_texlists.inc"
};

static void info_destroy(struct task *task);
static void info_step(struct task *task);
static void info_draw(struct task *task);

void info_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct info));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw and the destructor set before the step,
     * where struct task's order would set the step first */
    task->draw = info_draw;
    task->destructor = info_destroy;
    task->step = info_step;
    object = &INFO(task)->object;
    *object = info_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    radius = object->model->r + REACH_MARGIN;
    flags = WORLD_SOLID | WORLD_NO_CLIMB;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &INFO(task)->center);
    njPopMatrix(1);
}

static void info_destroy(struct task *task)
{
    if (INFO(task)->object.model != NULL) {
        world_remove(task, &INFO(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void info_step(struct task *step_task)
{
    /* @hack spelling: task, a copy of step_task, where step_task would do */
    struct task *task = step_task;
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct info *info;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else if (INFO(task)->object.model != NULL) {
        /* @hack spelling: info set inside the expression that sets object,
         * where a statement of its own would do */
        object = &(info = task->motion.p)->object;
        if (player_near(&info->center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    work->timer++;
}

static void info_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *face;
    Sint16 texture;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)INFO_EDITOR_ANGLE_X;
        (void)INFO_EDITOR_ANGLE_Y;
        (void)INFO_EDITOR_ANGLE_Z;
        (void)INFO_EDITOR_TEXNUM;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    face = info_face_object;
    texture = face->model->plist[FACE_TEXTURE_WORD];
    texture &= ~(TEXTURE_INDEX_BITS | NJD_FSS);
    texture |= (Sint16)njAbs(work->scl.x) % FACE_TEXTURES;
    face->model->plist[FACE_TEXTURE_WORD] = texture;
    njSetTexture(&info_face_texlist);
    njCnkEasyDrawModel(info_face_object->model);
    njSetTexture(&info_texlist);
    njCnkEasyDrawModel(info_object->model);
    njPopMatrix(1);
}
