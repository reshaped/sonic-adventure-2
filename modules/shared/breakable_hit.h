/* the attack test of the space stages' breakables */
#ifndef SHARED_BREAKABLE_HIT_H
#define SHARED_BREAKABLE_HIT_H
#include <sg_xpt.h>

#include "collision/collision.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "system/task.h"

/* the first frame at which a refused ground or wall dig counts as an attack */
#define ATTACK_FRAME 3.0F

/* TRUE where other attacks task as player_attacks_task judges it, with four
 * differences. other's touch is task's record's latest, collision->hit, where
 * player_attacks_task scans with collision_next_player. other rests on task
 * under COLLISION_UNDER alone. Knuckles's or Rouge's chains run on through
 * PLAYER_MODE_HUNTER_SPIRAL. a dive counts where ANIM_DIVE is the current or
 * the last animation, or ANIM_SPIRAL the last */
static Bool breakable_player_hit(struct task *volatile task, struct task *other)
{
    /* @hack type: task, work and slot volatile, where plain ones would do */
    struct task_work *volatile work;
    struct player *record;
    struct collision_work *collision;
    volatile int slot;
    int character;

    /* @hack spelling: the work tested again in an else-if chain, where
     * separate ifs would do */
    if (other == NULL) {
        return FALSE;
    } else if ((work = other->work) == NULL) {
        return FALSE;
    } else if (work != NULL && work->id == WORK_ID_ATTACK) {
        return TRUE;
    } else if (work->collision == NULL) {
        return FALSE;
    }
    if ((slot = player_slot_of(other)) == -1) {
        return FALSE;
    }
    record = player_records[slot];
    collision = task->work->collision;
    work = player_work[slot];
    /* @hack spelling: character set inside the first test, where a
     * statement of its own would do */
    if ((((character = record->character) == PLAYER_SONIC ||
          character == PLAYER_SHADOW) &&
         work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
         work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3 &&
         !(collision->flags & COLLISION_UNDER)) ||
        (character == PLAYER_SONIC && work->mode == PLAYER_MODE_SONIC_BOUNCE) ||
        ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
         (((collision->flags & COLLISION_UNDER) &&
           collision->hit->owner == other) ||
          record->ground_task == task) &&
         ((record->anim.current == ANIM_DIG_FAIL &&
           record->anim.frame >= ATTACK_FRAME) ||
          (record->anim.current == ANIM_DIG_WALL_FAIL &&
           record->anim.frame >= ATTACK_FRAME) ||
          record->anim.current == ANIM_DIVE || record->anim.last == ANIM_DIVE ||
          record->anim.last == ANIM_SPIRAL)) ||
        ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
         !(collision->flags & COLLISION_UNDER) &&
         collision->hit->owner == other &&
         work->mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
         work->mode <= PLAYER_MODE_HUNTER_SPIRAL)) {
        return TRUE;
    }
    return FALSE;
}

#endif
