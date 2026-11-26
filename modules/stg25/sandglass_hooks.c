/* the sandglass's four hooks, which shared/sandglass.h declares, as Death
 * Chamber defines them */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/sandglass.h"
#include "sound/sound.h"
#include "stg25/chamber_util.h"
#include "stg25/round_shadow.h"
#include "stg25/sandglass_door.h"
#include "system/task.h"

/* ids in death_chamber_sound_list */
#define HIT_SOUND 3
#define HIT_SOUND_PRIORITY 0
#define TICK_SOUND 4
#define TICK_SOUND_PRIORITY 0

/* the shadow draws twice. the first draw lies SHADOW_LIFT above the sandglass's
 * floor, and leaves the matrix scaled by shadow_scale. the second lies
 * SHADOW_STACK above the first in that scaled matrix, so the second draw takes
 * shadow_scale twice */
#define SHADOW_LIFT 0.1F
#define SHADOW_STACK 0.01F

static Sint32 hit_volume = 100;
static Sint32 tick_volume = 100;
static Sint32 dat_8c5f13c4 = 45;
static Float shadow_scale = 1.125F;

void sandglass_hit_sound(struct task *task)
{
    sound_play(HIT_SOUND, task, HIT_SOUND_PRIORITY, 0);
    sound_set_volume(task, SOUND_ANY_ID, hit_volume);
}

void sandglass_tick_sound(struct task *task)
{
    if (chamber_sandglass_door_find_glass((Uint8)task->work->ang.z) == task) {
        sound_play(TICK_SOUND, NULL, TICK_SOUND_PRIORITY, tick_volume);
    }
}

void sandglass_land_sound(struct task *task)
{
}

void sandglass_shadow_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, SANDGLASS(task)->floor + SHADOW_LIFT,
                work->pos.z);
    njSetTexture(&chamber_round_shadow_small_texlist);
    chamber_model_draw_scaled(&chamber_round_shadow_small_model, shadow_scale);
    njTranslate(NULL, 0.0F, SHADOW_STACK, 0.0F);
    chamber_model_draw_scaled(&chamber_round_shadow_small_model, shadow_scale);
    njPopMatrixEx();
}
