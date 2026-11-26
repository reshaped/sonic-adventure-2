/* Wild Canyon's two-player stage, stg41: the stage's init, exit and step */
#ifndef STG41_WILD_CANYON_2P_H
#define STG41_WILD_CANYON_2P_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table wild_canyon_2p_land;
extern const char wild_canyon_2p_land_filenames[548];
extern NJS_TEXLIST wild_canyon_2p_land_texlist;

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit. the stage needs player 1's work: the ambience task,
 * BgExec, reads it unchecked at each step.
 *
 * wild_canyon_2p_init starts the stage. among the rest, it starts the land,
 * the set manager, the kill area and the stage's three tasks. it starts
 * smoke_task_create's task for player 0, and for player 1 in a two-player
 * game. it gives the hunt its hints, and EMERALD_KIND_A_PATHMOVE's pieces
 * their paths. it fills eight of player_hooks' calls with the stage's own,
 * each only where it is empty. it sets stage_hooks too.
 *
 * wild_canyon_2p_exit ends the stage. it frees the set file, the hint file,
 * the messages and the hint box's hints. it releases the textures. it stops
 * the set manager, the object lights, the fog, the particles and the land. it
 * takes away the hunt's hints and the pieces' paths. it empties each of the
 * eight calls that still holds the stage's own */
void wild_canyon_2p_init(void);
void wild_canyon_2p_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void wild_canyon_2p_step(struct task *task);
/* the stage's object list, which wild_canyon_2p_init gives to set_start */
extern struct object_list wild_canyon_2p_objects;

#endif
