/* TOGE, Green Hill's spikes */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg31/green_hill_sounds.h"
#include "stg31/prop_art.h"
#include "stg31/spikes.h"
#include "system/task.h"

#define SPIKE_TOP 15.0F
#define SPIKE_ATTACK SHAPE_ATTACK_LEVEL(3)
/* whether the spikes attacked as the last step ended */
#define ATTACKING(work) ((work)->submode)
#define SOUND_DELAY 30

NJS_TEXLIST *green_hill_spike_texlists[] = {&green_hill_spikes_texlist, NULL};

/* clang-format off */
static struct collision_shape spike_shape = {
    0, SHAPE_BOX,
    PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS | SHAPE_HURT | SHAPE_GUARD_LEVEL(1),
    SHAPE_HURTABLE,
    {0.0F, 10.0F, 0.0F}, 20.0F, 10.0F, 10.0F, 0, {0, 0, 0}};
/* clang-format on */

static void spikes_destroy(struct task *task);
static void spikes_step(struct task *task);
static void spikes_draw(struct task *task);

void green_hill_spikes_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a statement kept for its string, the SET editor's
     * label, which SHC places in this source's constants */
    (void)"<- ANGLE Y";

    if (!placement_try_leave(task)) {
        task->draw = spikes_draw;
        task->step = spikes_step;
        task->destructor = spikes_destroy;
        work->mode = 0;
        collision_init(task, &spike_shape, 1, COLLISION_OBJECT);
    }
}

static void spikes_destroy(struct task *task)
{
}

static void spikes_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a jump to the label right below it */
    goto check_touch;
check_touch:
    if (work->collision) {
        if (ATTACKING(work) && collision_next_player(task)) {
            /* @hack type: ring_count's result taken as an int, through its
             * second name, where ring_count's own Sint16 would do */
            if (!ring_count_int(0) && !work->timer) {
                sound_play(GREEN_HILL_SPIKES, NULL, 0, 0);
            }
            work->timer = SOUND_DELAY;
        }
        if (work->timer) {
            --work->timer;
        }
        if (player_work[0]->pos.y > work->pos.y + SPIKE_TOP) {
            work->collision->shapes->levels =
                (work->collision->shapes->levels & ~SHAPE_ATTACK) |
                SPIKE_ATTACK;
            ATTACKING(work) = TRUE;
        } else {
            /* @hack spelling: the shape's levels read again to clear the
             * attack, where &= would do */
            work->collision->shapes->levels =
                work->collision->shapes->levels & ~SHAPE_ATTACK;
            ATTACKING(work) = FALSE;
        }
        collision_register(task);
    }
}

static void spikes_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&green_hill_spikes_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(green_hill_spikes_object->model);
    njPopMatrixEx();
}
