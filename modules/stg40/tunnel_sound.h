/* the tunnel sound, SE_TUNNEL in Final Chase's object list, and the stage's
 * sound list */
#ifndef STG40_TUNNEL_SOUND_H
#define STG40_TUNNEL_SOUND_H
#include "sound/lists.h"

struct task;

/* the list the stage's setup hands to sound_load_stage. sound ids 0 to 12
 * index it, and play bank 5's numbers 0 to 12 */
extern const struct sound_list final_chase_sound_list;

/* the object list's step for SE_TUNNEL. task needs a work, whose pos is the
 * sound's place. while time runs, a step plays sound 9 of the stage's sound
 * list, at no place. it plays only where the player that player_near finds
 * within 15 of pos has a spd.y below 0. every tunnel sound plays under one
 * key, so they share a slot */
void tunnel_sound_init(struct task *task);

#endif
