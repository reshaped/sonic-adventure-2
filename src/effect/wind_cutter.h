/* the wind cutter: blades of wind that spin about a player, and rings that
 * fade in and out */
#ifndef EFFECT_WIND_CUTTER_H
#define EFFECT_WIND_CUTTER_H

/* send count blades of wind about the player in slot player. their headings
 * lie evenly round a turn, from a random one. four rings of wind follow, a
 * quarter turn apart. each blade and ring is a task in level 3. one the heap
 * can't give is left out. count must not be 0.
 *
 * each stands where the player stood at the call. that is the record's
 * unk_218 for a mech, and its unk_230 for any other character. each draws with
 * the texlist of the Sonic record in player_tasks[owner].
 * player_records[player] must hold a record at the call, unchecked.
 * player_tasks[owner] must hold a task while any blade or ring lasts, also
 * unchecked. neither steps while game_state.time_stopped isn't 0 */
void wind_cutters_start(int owner, int player, int count);

#endif
