/* the mech bosses' beam gun: it holds a charge of shared/mech_boss_beam.h at a
 * boss's muzzle, and fires it. Hot Shot and Flying Dog each carry one */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "shared/mech_boss_beam.h"
#include "shared/mech_boss_beam_gun.h"
#include "shared/mech_boss_sounds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#define SHOT_VOLUME 120

/* the gun's record, in task->extra */
struct gun {
    NJS_POINT3 *pos; /* the point it follows */
    struct angles *ang; /* the angles it takes: its parent's work's */
    struct task *charge; /* the charge it holds, or null */
};

/* a charge's offset from the gun, as mech_boss_beam_charge_create takes it */
static NJS_POINT3 charge_offset = {-8.0F, -6.0F, 0.0F};

static void gun_destroy(struct task *task);
static void gun_step(struct task *task);

struct task *mech_boss_beam_gun_create(struct task *parent, NJS_POINT3 *pos,
                                       struct angles *ang)
{
    struct task *task;
    struct task_work *work;
    struct gun *gun;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, gun_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    gun = syCalloc(1, sizeof(struct gun));
    if (gun == NULL) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = gun;
    gun->pos = pos;
    /* @hack spelling: the parent read through the task, where parent would
     * do */
    gun->ang = &task->parent->work->ang;
    work->pos = *pos;
    work->ang = *ang;
    mech_boss_beam_gun_charge(task);
    task->destructor = gun_destroy;
    return task;
}

static void gun_destroy(struct task *task)
{
}

void mech_boss_beam_gun_fire(struct task *task, NJS_POINT3 *target,
                             Sint32 *busy)
{
    struct gun *gun = task->extra.p;
    struct mech_boss_beam_aim aim;

    *busy = 1;
    aim.pos = &gun->charge->work->pos;
    aim.target = target;
    mech_boss_beam_fire(task, &aim, busy);
    sound_play(MECH_BOSS_SOUND_BEAM_SHOT, NULL, 0, 0);
    sound_set_volume(NULL, MECH_BOSS_SOUND_BEAM_SHOT, SHOT_VOLUME);
    mech_boss_beam_charge_end(gun->charge);
    gun->charge = NULL;
}

static void gun_step(struct task *task)
{
    struct task_work *work = task->work;
    struct gun *gun = task->extra.p;

    if (!game_state.time_stopped && !game_state.unk_09) {
        work->pos = *gun->pos;
        work->ang = *gun->ang;
    }
}

void mech_boss_beam_gun_charge(struct task *task)
{
    struct task_work *work = task->work;
    struct gun *gun = task->extra.p;
    NJS_POINT3 offset;

    /* @hack spelling: the offset copied into a local, where &charge_offset
     * would do */
    offset = charge_offset;
    gun->charge =
        mech_boss_beam_charge_create(task, &work->pos, &offset, &work->ang);
}
