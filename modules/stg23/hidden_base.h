/* Hidden Base, stg23: the stage's land and object list, and its init, exit
 * and step */
#ifndef STG23_HIDDEN_BASE_H
#define STG23_HIDDEN_BASE_H
#include <ninja.h>
#include "collision/world.h"
#include "object/set.h"

struct path;
extern struct path *hidden_base_paths[];

extern struct land_table hidden_base_land;

#define HIDDEN_BASE_KINDS 89
extern struct object_kind stg23_kinds[HIDDEN_BASE_KINDS];
/* the stage's object list, which stg23_init gives to set_start */
extern struct object_list stg23_objects;

/* the overlay's init and exit, which stg23/head.c gives overlay_run_init and
 * overlay_run_exit.
 *
 * stg23_init compiles the land's and the objects' models into
 * STAGE_MODEL_BUILD_AREA of system/areas.h. it starts the background's two
 * tasks, BgClip and BgExec. where player_hooks' drop_rings is null, it sets it
 * to ring_scatter. it sets add_item_icon to item_icon_row_add and
 * draw_blast_ring to bomb_hat_draw_ring the same way. it sets all eight of
 * stage_hooks' calls.
 *
 * stg23_exit nulls each of those three player_hooks that still holds the call
 * init gave it. it leaves stage_hooks as init left them */
void stg23_init(void);
void stg23_exit(void);
/* the stage task's step, which does nothing. task needs a work */
void stg23_step(struct task *task);

extern NJS_TEXLIST hidden_base_land_texlist;

#endif
