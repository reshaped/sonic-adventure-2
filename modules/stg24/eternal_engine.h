/* Eternal Engine, stg24: the stage's init, exit and step */
#ifndef STG24_ETERNAL_ENGINE_H
#define STG24_ETERNAL_ENGINE_H
#include <ninja.h>
#include "collision/world.h"
#include "object/set.h"

extern struct land_table eternal_engine_land;
extern const char eternal_engine_land_filenames[541];

struct path;
extern struct path *eternal_engine_paths[];

struct task;
/* the stage's object list, which stg24_init gives to set_start */
extern struct object_list stg24_objects;
/* the overlay's init and exit, which stg24/head.c gives overlay_run_init and
 * overlay_run_exit.
 *
 * stg24_init starts the stage. among the rest, it starts the land, the set
 * manager and the music. it also starts the paths, the kill areas and the
 * background task, BgExec. it fills each of player_hooks' drop_rings,
 * add_item_icon and draw_blast_ring that is null. it sets stage_hooks too.
 *
 * stg24_exit ends the stage. it frees the set file, the hints and the
 * messages, and releases the textures. it stops the set manager, the fog, the
 * particles and the land. it takes the stage's motions out of the motion
 * table. it clears each of player_hooks' drop_rings, add_item_icon and
 * draw_blast_ring that still holds the stage's call */
void stg24_init(void);
void stg24_exit(void);
/* the stage task's step. it makes the reactor's manager where none lives, and
 * sets the fog. it moves players 0 and 1 as the fields at their places say. a
 * player at a point a field kills falls out of the stage. task needs a work */
void stg24_step(struct task *task);

extern NJS_TEXLIST eternal_engine_land_texlist;

#endif
