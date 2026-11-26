/* the destructor and step of a holdable object, for common_object.c's
 * holdable_init and space_pickup.c's pickups. HOLDABLE_RELEASES_ANIMALS is 1
 * where a first hold releases an animal, as holdable_init says */
#ifndef SHARED_HOLDABLE_MOTION_IMPL_H
#define SHARED_HOLDABLE_MOTION_IMPL_H
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/switch.h"
#include "system/angle.h"
#include "system/task.h"

#if HOLDABLE_RELEASES_ANIMALS
#include "object/animal.h"
#define HOLDABLE_ANIMAL_LIFT 5.0F
#endif

/* the attack child's mode while a player holds its parent, which stops it
 * registering its collision */
#define ATTACK_OFF 1
/* the slot of the player holding the object, in the work's submode */
#define HOLDER(work) ((work)->submode)

static void holdable_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (!(work->mode & HOLDABLE_NO_PRESSER)) {
        switch_presser_remove(&work->pos);
    }
    body_destroy(task);
    /* the model isn't the task's, so task_destroy mustn't free it */
    task->extra.p = NULL;
}

static void holdable_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
#if HOLDABLE_RELEASES_ANIMALS
    int count;
    int kind;
#endif

    if ((work->mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return;
    }
#if HOLDABLE_RELEASES_ANIMALS
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and its two locals. the
     * image shows nothing else of the block, so its statements are
     * stand-ins */
    if (0) {
        struct player *record = player_records[0];
        struct task_work *record_work = player_work[0];

        placement_set_unk_20(task);
        record->action = record_work->mode;
        record->kind = record_work->submode;
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- MINIMAL LEV( )";
        (void)"X";
    }
#endif
    if (work->flags & WORK_HELD_BY_PLAYER) {
        player = player_holding(task);
        if (player >= 0) {
            work->ang.y = QUARTER_TURN - player_work[player]->ang.y;
            HOLDER(work) = player;
#if HOLDABLE_RELEASES_ANIMALS
            if (task->placement != NULL && !placement_unk_20(task)) {
                placement_set_unk_20(task);
                count = work->scl.x;
                if (count > 0) {
                    count--;
                    /* @hack spelling: kind, a local for the release kind, where
                     * the expression as the argument would do */
                    kind = count % ANIMAL_RELEASE_KINDS;
                    animal_release(work->pos.x,
                                   work->pos.y + HOLDABLE_ANIMAL_LIFT,
                                   work->pos.z, kind);
                }
            }
#endif
        }
    }
    body_step(task);
    if (task->child != NULL) {
        task->child->work->pos = work->pos;
        if (work->flags & WORK_HELD_BY_PLAYER) {
            task->child->work->mode = ATTACK_OFF;
        } else {
            task->child->work->mode = 0;
        }
    }
}

#endif
