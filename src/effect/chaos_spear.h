/* the Chaos Spear: spears of light that close in on a player */
#ifndef EFFECT_CHAOS_SPEAR_H
#define EFFECT_CHAOS_SPEAR_H

/* send count spears of light in on the player in slot player, one after
 * another. they come at random pitches from headings spread evenly round the
 * player. count must not be 0. each spear is a task named
 * EffectChaosSpearExecutor in level 3. it draws with the texlist of the Sonic
 * record in player_tasks[owner]. a spear the heap can't give is left out.
 * while a spear lasts, player_records[player] must hold a record, and
 * player_tasks[owner] a task, unchecked. a spear's step does nothing while
 * game_state.time_stopped isn't 0 */
void chaos_spears_start(int owner, int player, int count);

#endif
