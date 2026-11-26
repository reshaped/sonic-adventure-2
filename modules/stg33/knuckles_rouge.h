/* the fight of Knuckles and Rouge, STAGE_VERSUS_KNUCKLES, the program of
 * STG33.PRS. the overlay's head names its init, its exit and its task's
 * step */
#ifndef STG33_KNUCKLES_ROUGE_H
#define STG33_KNUCKLES_ROUGE_H
#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table knuckles_rouge_land;
extern const char knuckles_rouge_land_filenames[230];

/* start the stage, and compile the land's models. against a Rouge in slot 0
 * the call puts Knuckles in slot 1, and against a Knuckles, Rouge. where slot
 * 1 then holds a player, the call puts it at its restart point or its start.
 * its work takes the id WORK_ID_RIVAL, and its pad goes off. ControlRouge
 * steers it. the call gives player_hooks.drop_rings ring_scatter,
 * add_item_icon item_icon_row_add and flash flash_manager_create_sparks_depth,
 * each where it is null. it gives stage_hooks message_window_open_unowned and
 * the trail calls, and makes its other four calls null. it hands BOSS_03.ADX to
 * music_play_or_defer */
void knuckles_rouge_init(void);
/* end the stage. it releases the textures, the messages and the set file. it
 * stops the fog, the particles, the set manager and the land. it clears each
 * of player_hooks.drop_rings, add_item_icon and flash that still holds what
 * init gave it. the tasks, the music and stage_hooks stay as init left
 * them */
void knuckles_rouge_exit(void);
/* the stage task's step, which does nothing */
void knuckles_rouge_step(struct task *task);
/* the stage's object list, which knuckles_rouge_init gives to set_start */
extern struct object_list knuckles_rouge_objects;

#endif
