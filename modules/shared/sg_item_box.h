/* the SG item box, SG_ITEMBOX in a stage's object list. only the players
 * whose bits are set in game_state.sg_players see it and take it */
#ifndef SHARED_SG_ITEM_BOX_H
#define SHARED_SG_ITEM_BOX_H

struct task;

/* the object list's step for SG_ITEMBOX. task needs a work. the box takes
 * task->force for its item's index, so force must hold no record.
 *
 * the work's scl.x, cut to an integer and held to 10 at most, picks the item.
 * it must be above -1, unchecked. the portable build takes 0 for a scl.x of
 * -1 or below, or a NaN. in that order the items are ITEM_SPEED_UP,
 * ITEM_5_RINGS, ITEM_1UP, ITEM_10_RINGS, ITEM_20_RINGS, ITEM_BARRIER,
 * ITEM_BOMB, ITEM_LIFE, ITEM_MAGNETIC_BARRIER, ITEM_EMPTY and ITEM_INVINCIBLE.
 * this box's ITEM_LIFE shows its icon, but gives nothing, not even points.
 *
 * a full-grown box goes to player 0 or 1 while that player's bit is set. the
 * player's collision record must hold a shape, unchecked. the center of its
 * first must lie within 15 of a point 5 above pos. where both players could
 * take the box, player 0 does. player 1 is passed over
 * where player 0 has no work or no collision record.
 *
 * the player who takes the box gets the item, even under PLAYER_MISSED. the
 * box passes that player to enemy_bounce_player, and shakes the player's
 * pad. the heap must have room for the player's icon task where none runs
 * yet, unchecked. a taken box kills itself at its next step.
 *
 * a taken box sets PLACEMENT_USED in its placement. a box whose placement
 * has the flag gives nothing, and kills itself at its second step. a box that
 * gives its 1-up sets PLACEMENT_UNK_20 in its placement, and writes through
 * null without one. a box of ITEM_1UP whose placement has PLACEMENT_UNK_20,
 * but not PLACEMENT_USED, holds ITEM_EMPTY in its place */
void sg_item_box_step(struct task *task);

#endif
