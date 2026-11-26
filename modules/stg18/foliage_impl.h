/* the build, init and callbacks of a plant that sways once as player 0 runs
 * into it. stg18/sasa.c, stg18/flower.c and stg18/leaf.c compile them for
 * SASA, FLOWER and LEAF. the includer defines the plant's art, and the sasa_
 * data that sizes and sounds it. sasa.c and flower.c define that data through
 * stg18/foliage_controller.h */
#ifndef STG18_FOLIAGE_IMPL_H
#define STG18_FOLIAGE_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stg18/lagoon_util.h"
#include "system/task.h"

enum sasa_kind { KIND_BRIGHT, KIND_DARK, KINDS };
#define KIND(work) ((int)njAbs((work)->scl.z) % KINDS)
/* the sway's frame, or 0 while the plant stands still */
#define FRAME(task) ((task)->unk_3c.l)

/* player 0 sets the plant swaying by running through it on the ground faster
 * than MIN_SPEED. the plant's inside reaches INSIDE_DEPTH below pos */
#define MIN_SPEED 0.2F
#define INSIDE_DEPTH 15.0F
#define SWAY_SOUND 8
#define SWAY_PRIORITY 0
#define SWAY_VOLUME 0

/* SASA_START(task) ends the init. leaf.c and flower.c define it to clear
 * FRAME(task) */
#ifndef SASA_START
#define SASA_START(task) ((void)0)
#endif

static void sasa_destroy(struct task *task);
static void sasa_step(struct task *task);
static void sasa_draw(struct task *task);
static Bool player_inside(struct task *task);

Uint8 *sasa_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, sasa_builds, KINDS) == KINDS) {
        sasa_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void sasa_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = sasa_draw;
    task->step = sasa_step;
    task->destructor = sasa_destroy;
    SASA_START(task);
}

static void sasa_destroy(struct task *task)
{
}

static void sasa_step(struct task *task)
{
    Float speed;

    if (placement_try_leave(task)) {
        return;
    }
    if (FRAME(task) == 0) {
        if (player_inside(task) && player_ground_speed(0, &speed) &&
            speed > MIN_SPEED) {
            sound_play(SWAY_SOUND, task, SWAY_PRIORITY, SWAY_VOLUME);
            sound_set_volume(task, SOUND_ANY_ID, sasa_volume);
            FRAME(task) = 1;
        }
    } else {
        FRAME(task)++;
        if ((Uint32)FRAME(task) >= sasa_shapes[KIND(task->work)]->nbFrame) {
            FRAME(task) = 0;
        }
    }
}

static void sasa_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for scl, where work would do */
    njSetTexture(sasa_texlists[KIND(task->work)]);
    if (FRAME(task) == 0) {
        if (sasa_built) {
            lagoon_direct_draw_scaled(sasa_builds[KIND(task->work)].built,
                                      LAGOON_SCALE(task->work));
        } else {
            lagoon_model_draw_scaled(sasa_objects[KIND(task->work)]->model,
                                     LAGOON_SCALE(task->work));
        }
    } else {
        njScale(NULL, LAGOON_SCALE(task->work), LAGOON_SCALE(task->work),
                LAGOON_SCALE(task->work));
        njCnkEasyDrawShapeMotion(
            sasa_objects[KIND(task->work)], sasa_motions[KIND(task->work)],
            sasa_shapes[KIND(task->work)], (Float)FRAME(task));
    }
    njPopMatrixEx();
}

static Bool player_inside(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *player = &player_work[0]->pos;
    Float dx;
    Float dz;
    Float radius;

    /* @hack spelling: task->work read again for scl.x, where work would do */
    if (work->pos.y - INSIDE_DEPTH <= player->y &&
        player->y <= work->pos.y + (sasa_height * LAGOON_SCALE(task->work))) {
        dx = player->x - work->pos.x;
        dz = player->z - work->pos.z;
        radius = sasa_radius * LAGOON_SCALE(task->work);
        return (dx * dx) + (dz * dz) < radius * radius;
    }
    return FALSE;
}

#endif
