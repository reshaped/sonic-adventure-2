/* the placements of the second fight of Sonic and Shadow, which follow
 * player 0 along x in laps */
#ifndef STG42_PLACEMENT_WRAP_H
#define STG42_PLACEMENT_WRAP_H

/* bring the placements along with player 0, a lap of 2400 along x at a time.
 * a placement more than half a lap ahead of player 0 moves back to within
 * half a lap. one more than half a lap behind moves ahead the same way. each
 * placement that moves takes PLACEMENT_ACTIVE as its only flag, and its tasks
 * and state become 0. so does one exactly half a lap behind, which stays where
 * it is.
 *
 * the call passes over the placements of a KIND_ALWAYS kind, and those with
 * PLACEMENT_LIVE or PLACEMENT_IN_RANGE. it moves nothing while player 0's
 * pos.x is below 300. slot 0 must hold a player, unchecked */
void placements_wrap(void);

#endif
