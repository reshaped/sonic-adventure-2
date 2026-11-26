/* Radical Highway, stg14: the stage's init, exit and step */
#ifndef STG14_RADICAL_HIGHWAY_H
#define STG14_RADICAL_HIGHWAY_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table radical_highway_land;
extern const char radical_highway_land_filenames[881];
extern NJS_TEXLIST radical_highway_terrain_textures;

/* the kinds radical_highway_objects lists, and the archives that hold their
 * textures. radical_highway_init loads the archives before it calls
 * set_start */
#define RADICAL_HIGHWAY_KINDS 92
extern struct object_kind radical_highway_kinds[RADICAL_HIGHWAY_KINDS];
extern struct texture_pvm radical_highway_pvms[];
extern NJS_TEXLIST **radical_highway_texture_groups[];

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * radical_highway_init starts the stage. among the rest, it starts the land,
 * the set manager, the paths, the kill areas and the background task, BgExec.
 * it fills player_hooks' drop_rings and add_item_icon with the stage's own
 * calls, each only where it is empty. it sets stage_hooks too.
 *
 * radical_highway_exit ends the stage. it frees the set file, the messages and
 * the hints, and releases the textures. it stops the set manager, the fog, the
 * particles and the land. it takes the stage's motions out of the motion
 * table. it empties each of those two calls that still holds the stage's
 * own */
void radical_highway_init(void);
void radical_highway_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void radical_highway_step(struct task *task);
/* the stage's object list, which radical_highway_init gives to set_start */
extern struct object_list radical_highway_objects;

#endif
