/* the second fight of Sonic and Shadow, stg42: the stage's init, exit and
 * step */
#ifndef STG42_VERSUS_SONIC_2_H
#define STG42_VERSUS_SONIC_2_H
#include "object/set.h"

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * versus_sonic_2_init starts the stage. a Sonic in slot 0 takes a Shadow as
 * its rival in slot 1. a Shadow takes a Sonic. the rival is a WORK_ID_RIVAL
 * player with its pad off, which the task ControlShadow steers. among the
 * rest, the init starts the set manager and the background task, BgExec. it
 * starts the kill areas too, of which there are none. it fills player_hooks'
 * drop_rings and add_item_icon with the stage's own calls, each only where it
 * is empty. it sets stage_hooks too. Ninja's far clip goes out to 20000.
 *
 * versus_sonic_2_exit ends the stage. it frees the set file and the messages,
 * and releases the textures. it stops the set manager, the fog, the particles
 * and the land, which the stage never starts. it empties each of those two
 * calls that still holds the stage's own. it leaves stage_hooks as the init
 * set them. Ninja's far clip comes back to 3000 */
void versus_sonic_2_init(void);
void versus_sonic_2_exit(void);
/* the stage task's step. player 0 falls out below y -100. it falls only while
 * both slots hold players that haven't missed, and kill_areas_task lives
 * without a child. each step calls placements_wrap too */
void versus_sonic_2_step(struct task *task);
/* the stage's object list, which versus_sonic_2_init gives to set_start */
extern struct object_list versus_sonic_2_objects;
/* the kinds of the stage's object list */
extern struct object_kind versus_sonic_2_kinds[27];

#endif
