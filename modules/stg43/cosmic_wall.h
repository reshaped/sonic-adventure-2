/* Cosmic Wall, stg43: the stage's init, exit and step, and its object list */
#ifndef STG43_COSMIC_WALL_H
#define STG43_COSMIC_WALL_H
#include "object/set.h"
struct task;

/* the overlay's init and exit, which stg43/head.c's stg43_head gives
 * overlay_run_init and overlay_run_exit. cosmic_wall_init fills
 * player_hooks' drop_rings, add_item_icon and draw_blast_ring with the stage's
 * own calls. it fills each only where it is empty. cosmic_wall_exit empties
 * each of the three that still holds the stage's own */
void cosmic_wall_init(void);
void cosmic_wall_exit(void);
/* the stage task's step. it starts the drill manager whenever there is none.
 * the manager starts both drill rides of stg43/drill.h whenever it has no
 * ride. task needs a work */
void cosmic_wall_step(struct task *task);
/* the stage's object list, which cosmic_wall_init gives to set_start */
extern struct object_list cosmic_wall_objects;

#endif
