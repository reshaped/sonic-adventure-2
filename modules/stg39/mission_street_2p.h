/* Mission Street's two-player stage, stg39: the stage's init, exit and step */
#ifndef STG39_MISSION_STREET_2P_H
#define STG39_MISSION_STREET_2P_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table mission_street_2p_land;
extern const char mission_street_2p_land_filenames[337];
extern NJS_TEXLIST mission_street_2p_land_texlist;

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * mission_street_2p_init starts the stage. among the rest, it starts the
 * land, the set manager, the kill area and the background task, BgExec. it
 * starts broken_down_smoke_create's task for player 0, and for player 1 where
 * player 1 has a task. it fills player_hooks' drop_rings, add_item_icon and
 * draw_blast_ring with the stage's own calls, each only where it is empty. it
 * sets stage_hooks too.
 *
 * mission_street_2p_exit ends the stage. it frees the set file, the messages
 * and the hints, and releases the textures. it stops the set manager, the
 * fog, the particles and the land. it empties each of those three calls that
 * still holds the stage's own */
void mission_street_2p_init(void);
void mission_street_2p_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void mission_street_2p_step(struct task *task);
/* the stage's object list, which mission_street_2p_init gives to set_start */
extern struct object_list mission_street_2p_objects;

#endif
