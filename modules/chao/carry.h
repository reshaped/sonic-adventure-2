/* a Chao carries the garden object it pairs with at hand 4, at hand 11 or
 * between them. the hands are its body's nodes 4 and 11. the Chao writes where
 * the object goes into the object's entry, and locks the entry. the object's
 * step then moves it there, and unlocks the entry */
#ifndef CHAO_CARRY_H
#define CHAO_CARRY_H
#include <sg_xpt.h>

struct task;

/* pair the Chao with item, place it, and hold it at a hand or both. the
 * placing comes first, so it follows the hands the Chao held at before */
void chao_take_at_hand_4(struct task *chao, struct task *item);
void chao_take_at_hand_11(struct task *chao, struct task *item);
void chao_take_in_both_hands(struct task *chao, struct task *item);
/* hold the object at a hand or both, from the next placing on */
void chao_hold_at_hand_4(struct task *chao);
void chao_hold_at_hand_11(struct task *chao);
void chao_hold_in_both_hands(struct task *chao);
/* place the object the Chao pairs with: write its entry's pos and ang.y, and
 * lock the entry. the object goes ahead of the holding hand, or of the
 * hands' midpoint, along the Chao's yaw. it sits carry_height lower. FALSE
 * where no object pairs with the Chao both ways */
Bool chao_place_held_item(struct task *chao);
/* move item part of the way to its entry's pos, and unlock the entry. FALSE,
 * moving nothing, unless the entry is locked and pairs both ways with a
 * Chao */
Bool garden_follow_chao(struct task *item);

/* crouch to the Chao's target. once the motion ends, take it in both hands
 * unless it pairs with another, and return BEHAVIOR_FINISH. BEHAVIOR_BREAK
 * where the motion runs 1800 frames without ending */
int chao_pick_up_target_behavior(struct task *chao);

#endif
