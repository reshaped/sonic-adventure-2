/* the tunnel sound, SE_TUNNEL in Final Rush's object list, and the stage's
 * sound list */
#ifndef STG30_TUNNEL_SOUND_H
#define STG30_TUNNEL_SOUND_H
#include "sound/lists.h"

struct task;

/* the list the stage's setup hands to sound_load_stage. sound ids 0 to 8
 * play bank 5's numbers 0 to 8. ids 9 and 10 play bank 6's 0 and 1 */
extern const struct sound_list final_rush_sound_list;
/* the object list's step for SE_TUNNEL. task needs a work, whose pos is the
 * sound's place. while time runs, a step plays sound 1 of the stage's sound
 * list, at no place. it plays only where player_near finds a player within
 * 15 of pos whose spd.y is below 0. every tunnel sound plays under one key,
 * so they share a slot */
void tunnel_sound_init(struct task *task);

#endif
