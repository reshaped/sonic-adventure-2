/* the gust, SE_TOPPU in Radical Highway's object list: a sound that plays at
 * the work's pos. the stage's sound list lies in the same source */
#ifndef STG14_GUST_SOUND_H
#define STG14_GUST_SOUND_H

#include "sound/lists.h"

struct task;

/* the stage's sound list, which radical_highway_init hands to
 * sound_load_stage */
extern const struct sound_list radical_highway_sound_list;

/* the object list's step for SE_TOPPU. unless placement_try_leave(task)
 * returns TRUE, it gives task the gust's own step, draw and destructor. task
 * needs a work, whose pos places the sound.
 *
 * each step plays sound 8 of the stage's list at pos for 121 sound updates. no
 * step plays it while game_state.time_stopped isn't 0. every gust keys its
 * sound by this function. so the gusts share one playing sound, and each gust's
 * step moves it to that gust's pos */
void highway_gust_sound_init(struct task *task);

#endif
