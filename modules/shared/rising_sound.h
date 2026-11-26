/* the rising sound: the mech bosses' sound 5, held while its pitch climbs to
 * the sound's own. Big Foot and Hot Shot play it */
#ifndef SHARED_RISING_SOUND_H
#define SHARED_RISING_SOUND_H

struct task;

/* start the sound in a child task of parent, and return the task. it
 * returns null for a null parent, or where task_create_child fails. where a
 * sound runs already, it starts none, and its result is undefined.
 *
 * the sound starts 1536 below its own pitch and climbs 32 a step to it. its
 * volume, on sound/sound.h's scale, climbs with the pitch from -3 to 0. the
 * task ends on its 240th step. while game_state.time_stopped or
 * game_state.unk_09 isn't 0, the steps do nothing, and the sound stops until
 * they resume */
struct task *rising_sound_start(struct task *parent);
/* where a sound runs, set it 256 above its own pitch and to a volume of 30.
 * its volume then falls 0.4 a step until the task's 240th step ends it */
void rising_sound_fade(void);
/* end the sound's task on its next step, where a sound runs */
void rising_sound_stop(void);

#endif
