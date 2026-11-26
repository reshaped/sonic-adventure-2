/* the voice sets, from which the menus' theme picks a voice */
#ifndef SOUND_VOICE_SET_H
#define SOUND_VOICE_SET_H

#define VOICE_SETS 28
/* a set holds a voice, or none, for each value of game_state.theme */
#define VOICE_SET_VOICES 13

/* play set's voice for game_state.theme with voice_play_priority_2. where
 * the set has no voice for the theme, nothing plays. set must be below
 * VOICE_SETS, and game_state.theme below VOICE_SET_VOICES, unchecked */
void voice_set_play(int set);

#endif
