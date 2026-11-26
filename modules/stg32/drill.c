/* the drill, BORING in Meteor Herd's object list. it settles once its
 * head's contacts with the base pass SETTLE_CONTACTS, and the head then rises
 * to drop_height */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/earth_shake.h"
#include "sound/sound.h"
#include "stg32/machinery.h"
#include "stg32/scenery_shapes.h"
#include "system/task.h"

/* the head's height above the base, and its speed upward */
#define HEIGHT(work) ((work)->scl.y)
#define SPEED(work) ((work)->scl.z)
/* the contacts with the base since the head last rose */
#define CONTACTS(work) ((work)->timer)
#define SETTLE_CONTACTS 50
/* the first contact of a drop shakes the cameras within SHAKE_RANGE. it plays
 * IMPACT_SOUND where a player lies within HEAR_RANGE */
#define SHAKE_RANGE 50.0F
#define SHAKE_POWER 2.0F
#define HEAR_RANGE 500.0F
#define IMPACT_SOUND 7
#define IMPACT_SOUND_PRIORITY 1
#define IMPACT_SOUND_VOLUME 80
#define IMPACT_SOUND_STEPS 80

NJS_TEXLIST *herd_drill_textures[] = {&herd_drill_texlist, NULL};
static Float drop_height = 160.0F;
static Float gravity = 0.8F;
/* the share of its speed the head keeps as it bounces */
static Float rebound = 0.38F;
static Float rise_speed = 2.0F;
static Float drag = 0.99F;

static void drill_destroy(struct task *task);
static void drill_step(struct task *task);
static void drill_draw(struct task *task);

void herd_drill_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = drill_draw;
        task->step = drill_step;
        task->destructor = drill_destroy;
        work->mode = 0;
        HEIGHT(work) = 0.0F;
        SPEED(work) = 0.0F;
    }
}

static void drill_destroy(struct task *task)
{
}

static void drill_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    HEIGHT(work) += SPEED(work);
    if (HEIGHT(work) < 0.0F) {
        HEIGHT(work) = 0.0F;
        SPEED(work) = njAbs(SPEED(work)) * rebound;
        if (CONTACTS(work) == 0) {
            shake_create(&work->pos, SHAKE_RANGE, SHAKE_POWER);
            if (player_near(&work->pos, HEAR_RANGE)) {
                sound_play_at_for(IMPACT_SOUND, work, IMPACT_SOUND_PRIORITY,
                                  IMPACT_SOUND_VOLUME, IMPACT_SOUND_STEPS,
                                  &work->pos);
            }
        }
        CONTACTS(work)++;
    }
    SPEED(work) -= gravity;
    SPEED(work) *= drag;
    if (CONTACTS(work) > SETTLE_CONTACTS) {
        SPEED(work) = 0.0F;
        HEIGHT(work) += rise_speed;
        if (HEIGHT(work) > drop_height) {
            HEIGHT(work) = drop_height;
            CONTACTS(work) = 0;
        }
    }
}

static void drill_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&herd_drill_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(herd_drill_base->model);
    njTranslate(NULL, 0.0F, HEIGHT(work) - drop_height, 0.0F);
    njCnkEasyDrawModel(herd_drill_head->model);
    njPopMatrixEx();
}
