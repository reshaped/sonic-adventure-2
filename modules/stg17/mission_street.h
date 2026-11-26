/* Mission Street, stg17: the stage's init, exit and step */
#ifndef STG17_MISSION_STREET_H
#define STG17_MISSION_STREET_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table mission_street_land;
extern const char mission_street_land_filenames[1197];
extern NJS_TEXLIST mission_street_land_texlist;

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * mission_street_init starts the stage. among the rest, it starts the land,
 * the set manager and the kill areas. it starts the background task, BgExec,
 * and player 0's smoke task. it fills player_hooks' drop_rings and
 * add_item_icon with the stage's own calls, each only where it is empty. it
 * sets stage_hooks too.
 *
 * mission_street_exit ends the stage. it frees the set file, the messages and
 * the hints, and releases the textures. it stops the set manager, the fog,
 * the particles and the land. it takes the stage's motions out of the motion
 * table. it empties each of those two calls that still holds the stage's
 * own. it empties player_hooks.draw_blast_ring too where it holds
 * bomb_hat_draw_ring, which the init never puts there */
void mission_street_init(void);
void mission_street_exit(void);
/* the stage task's step. a player in slot 0 or 1 falls out of the stage
 * below a height of -260. one that has missed doesn't. players fall only while
 * kill_areas_task lives and has no child. player_fall_out makes the miss's
 * task that child, so no player falls while a miss runs. while the work's mode
 * is 0, a press of Y on player 1's pad sends player 0 PLAYER_REQUEST_BOARD.
 * task needs a work */
void mission_street_step(struct task *task);
/* the stage's object list, which mission_street_init gives to set_start */
extern struct object_list mission_street_objects;

#endif
