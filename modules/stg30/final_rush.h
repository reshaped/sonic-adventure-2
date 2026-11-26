/* Final Rush, stg30: the stage's init, exit and step */
#ifndef STG30_FINAL_RUSH_H
#define STG30_FINAL_RUSH_H
#include "object/set.h"

/* the stage's object kinds, which its object list holds */
#define FINAL_RUSH_KINDS 87
extern struct object_kind stg30_kinds[FINAL_RUSH_KINDS];
/* the stage's object list, which stg30_init gives to set_start */
extern struct object_list stg30_objects;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit. stg30_init starts the stage, and stg30_exit ends it.
 *
 * stg30_init sets player_hooks' drop_rings and add_item_icon to the stage's own
 * calls, each only where it is null. stg30_exit sets each back to null where it
 * still holds the stage's call. stg30_init also sets stage_hooks and the
 * settings of shared/debris.h, which stg30_exit leaves as they are */
void stg30_init(void);
void stg30_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void stg30_step(struct task *task);

#endif
