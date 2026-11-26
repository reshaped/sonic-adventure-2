/* the weed, WEED in Pyramid Cave's object list: a plant that sways once as
 * player 0 runs past */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "stg28/cave_util.h"
#include "stg28/weed.h"
#include "system/port.h"
#include "system/task.h"

enum weed_kind { WEED_SMALL, WEED_LARGE, WEED_KINDS };
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % WEED_KINDS)
#define SCALE(task) ((task)->work->scl.x + 1.0F)
/* the frames the sway advances a step */
#define SWAY_STEP(task) ((task)->work->scl.y + 1.0F)

/* the sway's frame, in task's unk_3c, or 0 while the weed rests. unk_3c is an
 * int, so it keeps only the whole part of each sum */
#define FRAME(task) ((task)->unk_3c.l)
/* the speed along the ground above which player 0 starts a sway */
#define STIR_SPEED 0.2F
/* player 0 starts a sway within weed_reach times the scale of pos across x
 * and z. the player must stand from REACH_DOWN below pos to weed_height times
 * the scale above it. a LARGE weed's reach and height are LARGE_NEAR_SCALE
 * times a SMALL one's */
#define REACH_DOWN 25.0F
#define LARGE_NEAR_SCALE 2.0F

#include "stg28/weed/text.inc"
#include "stg28/models/weed.h"

static void weed_destroy(struct task *task);
static void weed_step(struct task *task);
static void weed_draw(struct task *task);
static Bool weed_player_near(struct task *task);

KEPT static NJS_CNK_OBJECT *weed_objects[WEED_KINDS] = {
#include "stg28/weed/weed_objects.inc"
};
NJS_TEXLIST *cave_weed_texlists[WEED_KINDS + 1] = {
#include "stg28/weed/cave_weed_texlists.inc"
};
KEPT static NJS_MOTION *weed_motions[WEED_KINDS] = {
#include "stg28/weed/weed_motions.inc"
};
KEPT static NJS_MOTION *weed_shapes[WEED_KINDS] = {
#include "stg28/weed/weed_shapes.inc"
};
static char *weed_kind_names[WEED_KINDS] = {
#include "stg28/weed/weed_kind_names.inc"
};
static Float weed_height = 2.0F;
static Float weed_reach = 4.0F;

void cave_weed_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE X (   )";
        (void)"<- ANGLE Y (   )";
        (void)"<- ANGLE Z (   )";
        (void)"<- SCALE(1.0+x)";
        (void)"<- SHAPE SPEED(1.0+x)";
        (void)"<- WEED(      )";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = weed_draw;
    task->step = weed_step;
    task->destructor = weed_destroy;
    FRAME(task) = 0;
}

static void weed_destroy(struct task *task)
{
}

static void weed_step(struct task *task)
{
    struct task_work *work = task->work;
    Float speed;

    /* @hack redundant: a block no path reaches */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        njTranslateEx(&work->scl);
        njPushMatrixEx();
        njTranslateEx(&task->work->pos);
        njPushMatrixEx();
    }
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack type: the frame compared with 0.0F as a Float, where with 0 as
     * an int would do */
    if (FRAME(task) == 0.0F) {
        /* @hack spelling: three tests nested, where one && test would do */
        if (weed_player_near(task)) {
            if (player_ground_speed(0, &speed)) {
                if (speed > STIR_SPEED) {
                    FRAME(task) = SWAY_STEP(task);
                }
            }
        }
    } else {
        FRAME(task) += SWAY_STEP(task);
        if ((Uint32)FRAME(task) >= weed_shapes[KIND(task)]->nbFrame) {
            FRAME(task) = 0;
        }
    }
}

static void weed_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_weed_texlists[KIND(task)]);
    /* @hack type: the frame compared with 0.0F as a Float, where with 0 as
     * an int would do */
    if (FRAME(task) == 0.0F) {
        cave_model_draw_scaled(weed_objects[KIND(task)]->model, SCALE(task));
    } else {
        njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
        njCnkEasyDrawShapeMotion(weed_objects[KIND(task)],
                                 weed_motions[KIND(task)],
                                 weed_shapes[KIND(task)], FRAME(task));
    }
    njPopMatrixEx();
}

static Bool weed_player_near(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *pos = &player_work[0]->pos;
    Float reach = SCALE(task);
    Float height = weed_height * reach;
    Float x;
    Float z;

    reach *= weed_reach;
    if (KIND(task) == WEED_LARGE) {
        height *= LARGE_NEAR_SCALE;
        reach *= LARGE_NEAR_SCALE;
    }
    /* @hack spelling: the test of the distance nested in a test of the
     * height, where a return of FALSE past the height would do */
    if (!(work->pos.y - REACH_DOWN > pos->y || work->pos.y + height < pos->y)) {
        x = pos->x - work->pos.x;
        /* @hack spelling: z set to the weed's z and then to the difference,
         * where the difference alone would do */
        z = work->pos.z;
        z = pos->z - z;
        return (x * x) + (z * z) < reach * reach;
    }
    return FALSE;
}
