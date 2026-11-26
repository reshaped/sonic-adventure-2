/* the eye, EYE in Pyramid Cave's object list: a solid eyeball, with a light
 * that faces the camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg28/cave_util.h"
#include "stg28/eye.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/eye/text.inc"
#include "stg28/models/eye.h"

#define EYE_KINDS 1
#define KIND(work) ((Sint32)njAbs((work)->scl.z) % EYE_KINDS)

/* the eyeball's shape in the collision world collides only while a player
 * lies within REACH of its pos */
#define REACH 210.0F

KEPT static NJS_CNK_MODEL *eye_models[EYE_KINDS] = {
#include "stg28/eye/eye_models.inc"
};
NJS_TEXLIST *cave_eye_texlists[3] = {
#include "stg28/eye/cave_eye_texlists.inc"
};
static char *eye_kind_names[EYE_KINDS] = {
#include "stg28/eye/eye_kind_names.inc"
};
/* how far from the eye's pos the light draws, along the camera's z */
static Float light_offset = 20.0F;

static void eye_destroy(struct task *task);
static void eye_step(struct task *task);
static void eye_draw(struct task *task);

void cave_eye_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct angles ang;

    /* @hack redundant: SHC retains the SET editor labels below. */
    if (0) {
        (void)"<- ANGLE X (   )";
        (void)"<- ANGLE Y (   )";
        (void)"<- ANGLE Z (   )";
    }
    if (!placement_try_leave(task)) {
        task->draw = eye_draw;
        task->step = eye_step;
        task->destructor = eye_destroy;
        object = world_pool_alloc();
        /* @bug memory: copies the collision object through null where
         * world_pool_alloc has none to lend (MOV.L at 0x8c50a3ca). eye_destroy
         * tests task->extra for null (TST at 0x8c50a456). eye_step hands
         * player_near the copy's pos untested (ADD at 0x8c50a498) */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the pool has no object, the
         * init destroys the eye, and set.c makes it again on a later pass */
        if (object == NULL) {
            task_destroy(task);
            return;
        }
#endif
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
        njPopMatrixEx();
        task->extra.p = object;
        *object = cave_eye_collision;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
        object->ang[0] = ang.x;
        object->ang[1] = ang.y;
        object->ang[2] = ang.z;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SOLID | WORLD_SIZE_CLASS(2), task, object);
    }
}

static void eye_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void eye_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches */
        if (0) {
            Float scale = work->scl.x;

            task_destroy(NULL);
            task_destroy(NULL);
            task_destroy(NULL);
            if (scale) {
            }
        }
        object = task->extra.p;
        if (player_near((NJS_POINT3 *)object->pos, REACH)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void eye_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njSetTexture(cave_eye_texlists[KIND(task->work)]);
    njCnkEasyDrawModel(eye_models[KIND(task->work)]);
    /* the light draws in the camera's own axes, turned a half turn about x.
     * so it faces the camera however the eye and the camera turn */
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njTranslate(NULL, 0.0F, 0.0F, light_offset);
    njRotateX(NULL, HALF_TURN);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(&cave_eye_light_texlist);
    njCnkEasyDrawModel(&cave_eye_light_model);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
    njPopMatrixEx();
}
