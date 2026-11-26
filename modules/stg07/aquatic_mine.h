/* Aquatic Mine, the stage of STG07. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG07_AQUATIC_MINE_H
#define STG07_AQUATIC_MINE_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table aquatic_mine_land;
extern const char aquatic_mine_land_filenames[229];
extern NJS_TEXLIST aquatic_mine_land_texlist;

/* start the stage. among the rest, it starts the land, the object list, the
 * water and the background task, BgExec. it fills each of player_hooks'
 * drop_rings, unk_04, add_item_icon, create_piece, show_piece_sign, ripple and
 * splash that is null with the stage's own, and sets stage_hooks */
void aquatic_mine_init(void);
/* end the stage. it frees the set file, the block grid and the hint file. it
 * releases the textures, the messages and the hint box's hints. it stops the
 * set manager, the land, the water and the fog, among the rest. it clears each
 * of those calls of player_hooks that still holds the stage's own. it leaves
 * stage_hooks and the hunt's table of hints as aquatic_mine_init set them */
void aquatic_mine_exit(void);
/* the stage task's step, which starts the fog unless it runs already. task
 * needs a work */
void aquatic_mine_step(struct task *task);
/* the stage's object list, which aquatic_mine_init gives to set_start */
extern struct object_list aquatic_mine_objects;

#endif
