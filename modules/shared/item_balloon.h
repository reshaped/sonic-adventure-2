/* the item balloon, ITEMBOXBALLOON in a stage's object list. the player who
 * breaks a balloon gets the item it holds. its generator, ITEMBOXBAL GEN in
 * an object list, makes balloons of random items, one at a time */
#ifndef SHARED_ITEM_BALLOON_H
#define SHARED_ITEM_BALLOON_H
#include <ninja.h>

struct task;

/* the balloon's texture list and node. a stage's table of texture lists names
 * the list, and the kart race's balloon draws the node */
extern NJS_TEXLIST item_balloon_texlist;
extern NJS_CNK_OBJECT item_balloon_object;

/* the object list's step for ITEMBOXBALLOON, and the step of each balloon a
 * generator makes. task needs a work, and no record in force, where the balloon
 * keeps its item's index. the heap must have room for the balloon's collision
 * record, unchecked. it must also have room for the breaker's icon row, where
 * none runs yet, unchecked. each step returns at once where
 * placement_try_leave(task) returns TRUE.
 *
 * the work's scl.x, cut to a whole number and held from 0 to 10, picks the
 * item. in that order the items are ITEM_SPEED_UP, ITEM_5_RINGS, ITEM_1UP,
 * ITEM_10_RINGS, ITEM_20_RINGS, ITEM_BARRIER, ITEM_BOMB, ITEM_LIFE,
 * ITEM_MAGNETIC_BARRIER, ITEM_EMPTY and ITEM_INVINCIBLE. ang.y turns the
 * balloon.
 *
 * once grown, the balloon breaks at a touch by a player's or an attack's
 * collision record. the item goes to that record's player. a touch by a record
 * of no player leaves the balloon whole. a player whose record has
 * PLAYER_MISSED breaks it, but gets nothing. a player in the air that gets the
 * item takes a speed of 2, straight up. the break shakes the player's pad. the
 * broken balloon grows and fades out, then ends.
 *
 * a balloon that breaks sets PLACEMENT_USED in its placement. one that gives
 * its 1-up sets PLACEMENT_UNK_20 there, and writes through null where it has no
 * placement. a balloon whose placement has PLACEMENT_USED breaks at its first
 * step, and gives nothing. a balloon of ITEM_1UP whose placement has
 * PLACEMENT_UNK_20, but not PLACEMENT_USED, holds ITEM_EMPTY in its place */
void item_balloon_step(struct task *task);
/* the object list's step for ITEMBOXBAL GEN. it gives task the generator's
 * step and draw, which draws nothing. it adds ang.z to the work's timer as a
 * head start. task needs a work.
 *
 * the generator counts its timer up while it has no child, and holds it at 0
 * while it has one. once the timer passes ang.x, the generator makes a
 * balloon as its child, at its own pos and angles. it then starts the timer
 * again from 0, even where the heap had no room for the balloon.
 *
 * the balloon's item is random. its odds are 1 in 12 for ITEM_SPEED_UP, 2 in
 * 12 for ITEM_5_RINGS, 4 in 12 for ITEM_10_RINGS, 3 in 12 for ITEM_20_RINGS
 * and 2 in 12 for ITEM_BARRIER */
void item_balloon_generator_init(struct task *task);

#endif
