#include <ninja.h>
#include <sg_xpt.h>

#include "chao/carry.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/task.h"

/* the share of the way a carried object moves each step to where the Chao
 * places it */
#define FOLLOW_SHARE 0.3F

void chao_take_at_hand_4(struct task *chao, struct task *item)
{
    struct chao_work *work = CHAO_WORK(chao);
    garden_pair_objects(chao, item);
    garden_lock_entry(garden_mutual_partner(chao));
    chao_place_held_item(chao);
    work->flags |= CHAO_HOLD_AT_HAND_4;
    work->flags &= ~CHAO_HOLD_AT_HAND_11;
}

void chao_take_at_hand_11(struct task *chao, struct task *item)
{
    struct chao_work *work = CHAO_WORK(chao);
    garden_pair_objects(chao, item);
    garden_lock_entry(garden_mutual_partner(chao));
    chao_place_held_item(chao);
    work->flags &= ~CHAO_HOLD_AT_HAND_4;
    work->flags |= CHAO_HOLD_AT_HAND_11;
}

void chao_take_in_both_hands(struct task *chao, struct task *item)
{
    struct chao_work *work = CHAO_WORK(chao);
    garden_pair_objects(chao, item);
    garden_lock_entry(garden_mutual_partner(chao));
    chao_place_held_item(chao);
    /* @hack spelling: a bit set in each statement, where one mask would do */
    work->flags |= CHAO_HOLD_AT_HAND_4;
    work->flags |= CHAO_HOLD_AT_HAND_11;
}

void chao_hold_at_hand_4(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    work->flags |= CHAO_HOLD_AT_HAND_4;
    work->flags &= ~CHAO_HOLD_AT_HAND_11;
}

void chao_hold_at_hand_11(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    work->flags &= ~CHAO_HOLD_AT_HAND_4;
    work->flags |= CHAO_HOLD_AT_HAND_11;
}

void chao_hold_in_both_hands(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    /* @hack spelling: a bit set in each statement, where one mask would do */
    work->flags |= CHAO_HOLD_AT_HAND_4;
    work->flags |= CHAO_HOLD_AT_HAND_11;
}

Bool chao_place_held_item(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct garden_entry *entry;
    struct chao_shape_work *shape;
    NJS_POINT3 *hand_4;
    NJS_POINT3 *hand_11;
    Angle yaw;
    Float reach;
    Float radius;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((entry = garden_mutual_partner(chao)) != NULL) {
        /* @hack spelling: the shape read through chao->work, where work would
         * do */
        shape = &CHAO_WORK(chao)->shape;
        hand_4 = &shape->hand_4;
        hand_11 = &shape->hand_11;
        radius = entry->carry_radius;
        /* @hack spelling: the yaw read through chao->work, where work would
         * do */
        yaw = chao->work->ang.y;
        reach = njDistanceP2P(hand_4, hand_11);
        reach *= 0.5F;
        reach = njSqrt((radius * radius) + (reach * reach));
        garden_lock_entry(entry);
        if (work->flags & CHAO_HOLD_AT_HAND_4) {
            if (work->flags & CHAO_HOLD_AT_HAND_11) {
                entry->pos.x =
                    ((hand_4->x + hand_11->x) * 0.5F) + (reach * njSin(yaw));
                entry->pos.y = (hand_4->y + hand_11->y) * 0.5F;
                entry->pos.z =
                    ((hand_4->z + hand_11->z) * 0.5F) + (reach * njCos(yaw));
            } else {
                entry->pos.x = hand_4->x + (reach * njSin(yaw));
                entry->pos.y = hand_4->y;
                entry->pos.z = hand_4->z + (reach * njCos(yaw));
            }
        } else if (work->flags & CHAO_HOLD_AT_HAND_11) {
            entry->pos.x = hand_11->x + (reach * njSin(yaw));
            entry->pos.y = hand_11->y;
            entry->pos.z = hand_11->z + (reach * njCos(yaw));
        }
        entry->pos.y -= entry->carry_height;
        entry->ang.y = yaw;
        return TRUE;
    }
    return FALSE;
}

Bool garden_follow_chao(struct task *item)
{
    struct task_work *work = item->work;
    struct garden_entry *entry = item->force.p;

    if (garden_object_locked(item) &&
        garden_partner_in_category(item, GARDEN_OBJECT_CHAO)) {
        work->pos.x += (entry->pos.x - work->pos.x) * FOLLOW_SHARE;
        work->pos.y += (entry->pos.y - work->pos.y) * FOLLOW_SHARE;
        work->pos.z += (entry->pos.z - work->pos.z) * FOLLOW_SHARE;
        garden_unlock_entry(entry);
        return TRUE;
    }
    return FALSE;
}
