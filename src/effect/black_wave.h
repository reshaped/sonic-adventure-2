/* the black wave: rings of dark that spread one after another from where a
 * player stood */
#ifndef EFFECT_BLACK_WAVE_H
#define EFFECT_BLACK_WAVE_H

/* start count waves of dark where the player in slot player stands at the call.
 * that is unk_218 of its record for a mech, and unk_230 for any other
 * character. the slot must hold a record, unchecked. a task named
 * EffectBlackWaveGenerator in level 3 starts the waves. the first starts at its
 * second step, and the rest 14 steps apart. it ends once player_tasks[player]
 * is null, or its waves are all started and gone. nothing starts where the heap
 * can't give the task, and a wave it can't give is left out.
 *
 * a wave is a translucent sprite of the effects_texlist in the hunter record of
 * player_tasks[owner], unchecked. it grows as it spreads, and draws smaller the
 * deeper it lies. its 32nd step ends it. the first wave's draw turns the fog
 * off, and the last's turns it on. the fog thus stays off while the last lies
 * behind the camera */
void black_wave_start(int owner, int player, int count);

#endif
