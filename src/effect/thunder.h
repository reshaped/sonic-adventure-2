/* thunder: bolts that walk a jagged line toward a point, and leave lines of
 * light that fade */
#ifndef EFFECT_THUNDER_H
#define EFFECT_THUNDER_H

/* strike the player in slot player with count bolts of thunder, each a task
 * in level 3. a bolt starts above the player, against world_gravity. it walks
 * a jagged line to the record's unk_218, and leaves lines of light that
 * fade. a bolt can branch into more, which walk toward random points near
 * it. a bolt the heap can't give is left out. the slot must hold a work and
 * a record, unchecked.
 *
 * player_hooks.flash, where it is set, makes a flash where a bolt ends. a
 * bolt that passes its aim makes a flash there and leaves a matrix pushed on
 * Ninja's stack. it does both again at each step it has left. no bolt or
 * light steps while game_state.time_stopped isn't 0 */
void thunder_strike(int player, int count);

#endif
