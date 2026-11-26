/* the second fight of Tails's and Eggman's walkers, STAGE_VERSUS_MECH_2, the
 * program of STG29.PRS. the overlay's head names its init, its exit and its
 * task's step */
#ifndef STG29_VERSUS_MECH_2_H
#define STG29_VERSUS_MECH_2_H
#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table versus_mech_2_land;
extern const char versus_mech_2_land_filenames[311];

/* start the stage, and compile the land's models. against Eggman's walker in
 * slot 0 it puts Tails's in slot 1, and against Tails's, Eggman's. where slot
 * 1 then holds a player, the player takes its start and its work the id
 * WORK_ID_RIVAL. its pad goes off, and rival_walker_create's task steers it.
 * both players' walkers then smoke as their gauges fall. init fills
 * player_hooks' drop_rings, add_item_icon and draw_blast_ring with the stage's
 * own where they are null, and sets stage_hooks */
void versus_mech_2_init(void);
/* end the stage. it nulls drop_rings, add_item_icon and draw_blast_ring where
 * they still hold the stage's own. it leaves the rival's task, the two smokes
 * and the backdrop's task running. the kill areas, the blasts' textures, the
 * music and stage_hooks stay as init left them */
void versus_mech_2_exit(void);
/* the stage task's step. its first step gives camera 0 rival_walker.h's
 * BOSSPOINT mode, on the task's pos. each step puts that pos 18 above slot
 * 1's walker, where slot 1 has a work */
void versus_mech_2_step(struct task *task);
extern struct object_list versus_mech_2_objects;

#endif
