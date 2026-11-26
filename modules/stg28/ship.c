/* the ship, SHIP in Pyramid Cave's object list: a ship whose motion loops.
 * its sound fades with player 0's distance */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/ship.h"
#include "system/task.h"

enum ship_kind { SHIP_NORMAL, SHIP_EGGMAN, SHIP_KINDS };
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % SHIP_KINDS)
#define SCALE(task) ((task)->work->scl.x + 1.0F)

/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FRAME(task) (*(Float *)&(task)->unk_3c)
/* the frames the motion advances a step, times scl.y plus 1 */
#define FRAME_STEP 0.667F

/* the ship's sound, an id in pyramid_cave_sound_list. a play of it lasts
 * SHIP_SOUND_STEPS + 1 updates, as sound_play_for says. SHIP_SOUND_VOLUME is
 * the play's volume, which the step's sound_set_volume then replaces */
#define SHIP_SOUND 10
#define SHIP_SOUND_PRIORITY 1
#define SHIP_SOUND_VOLUME 0
#define SHIP_SOUND_STEPS 15

#include "stg28/ship/text.inc"
#include "stg28/models/ship.h"

static void ship_destroy(struct task *task);
static void ship_step(struct task *task);
static void ship_draw(struct task *task);

static NJS_CNK_OBJECT *ship_objects[SHIP_KINDS] = {
#include "stg28/ship/ship_objects.inc"
};
NJS_TEXLIST *cave_ship_texlists[SHIP_KINDS + 1] = {
#include "stg28/ship/cave_ship_texlists.inc"
};
static NJS_MOTION *ship_motions[SHIP_KINDS] = {
#include "stg28/ship/ship_motions.inc"
};
static char *ship_kind_names[SHIP_KINDS] = {
#include "stg28/ship/ship_kind_names.inc"
};
/* SHIP_SOUND plays while player 0 lies within ship_far of the ship. its
 * volume is ship_near_volume within ship_near. beyond, it falls in a line to
 * ship_far_volume at ship_far */
static Sint32 ship_near_volume = 100;
static Sint32 ship_far_volume = -10;
static Float ship_near = 1500.0F;
static Float ship_far = 3500.0F;

void cave_ship_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = ship_draw;
    task->step = ship_step;
    task->destructor = ship_destroy;
    FRAME(task) = 0.0F;
}

static void ship_destroy(struct task *task)
{
}

static void ship_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Float distance;
    int volume;

    if (placement_try_leave(task)) {
        return;
    }
    pos = work->pos;
    distance = njDistanceP2P(&player_work[0]->pos, &pos);
    if (distance < ship_far) {
        /* @hack redundant: a second test against ship_far, which never
         * holds inside the first, where the line alone would do */
        if (distance <= ship_near) {
            volume = ship_near_volume;
        } else if (distance >= ship_far) {
            volume = ship_far_volume;
        } else {
            volume = ship_far_volume +
                     ((ship_far - distance) / (ship_far - ship_near) *
                      (ship_near_volume - ship_far_volume));
        }
        sound_play_for(SHIP_SOUND, task, SHIP_SOUND_PRIORITY, SHIP_SOUND_VOLUME,
                       SHIP_SOUND_STEPS);
        sound_set_volume(task, SOUND_ANY_ID, volume);
    }
    /* @hack spelling: the work read again through task, where work
     * would do */
    FRAME(task) += (task->work->scl.y + 1.0F) * FRAME_STEP;
    if (FRAME(task) > ship_motions[KIND(task)]->nbFrame - 1) {
        FRAME(task) = 0.0F;
    }
    if (FRAME(task) < 0.0F) {
        FRAME(task) = ship_motions[KIND(task)]->nbFrame - 1;
    }
}

static void ship_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float *radius;
    Float unscaled;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(cave_ship_texlists[KIND(task)]);
    /* the model clip doesn't scale r by the matrix. so the root model's r
     * scales with the ship while the motion draws */
    radius = &ship_objects[KIND(task)]->model->r;
    unscaled = *radius;
    *radius *= SCALE(task);
    njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
    njCnkEasyDrawMotion(ship_objects[KIND(task)], ship_motions[KIND(task)],
                        FRAME(task));
    *radius = unscaled;
    njPopMatrixEx();
}
