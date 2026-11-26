/* Final Chase, stg40: the stage's init, exit and step */
#ifndef STG40_FINAL_CHASE_H
#define STG40_FINAL_CHASE_H
#include "object/set.h"

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit. final_chase_init starts the stage, and final_chase_exit
 * ends it.
 *
 * final_chase_init sets player_hooks' drop_rings and add_item_icon to the
 * stage's own calls, each only where it is null. final_chase_exit sets each
 * back to null where it still holds the stage's call. final_chase_init also
 * sets stage_hooks and the settings of shared/debris.h, which final_chase_exit
 * leaves as they are */
void final_chase_init(void);
void final_chase_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void final_chase_step(struct task *task);
/* the stage's object list, which final_chase_init gives to set_start */
extern struct object_list final_chase_objects;

#endif
