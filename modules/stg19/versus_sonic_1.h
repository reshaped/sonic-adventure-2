/* the first fight of Sonic and Shadow, STAGE_VERSUS_SONIC_1, the program of
 * STG19.PRS. the overlay's head names its init, its exit and its task's
 * step */
#ifndef STG19_VERSUS_SONIC_1_H
#define STG19_VERSUS_SONIC_1_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

/* the names of the land's PVM, in archive, and of its textures, which
 * versus_sonic_1_land points at. each name starts at a multiple of 4 */
struct duel_land_texture_names {
    char archive[12];
    char jungle128_11[16];
    char jungle128_13[16];
    char jungle128_5[12];
    char jungle256_2[12];
    char jungle256_6[12];
    char jungle256_9[12];
    char jungle64_3[11];
};
extern const struct duel_land_texture_names versus_sonic_1_land_names;

extern NJS_TEXLIST versus_sonic_1_water_texlist;

struct task;

extern struct land_table versus_sonic_1_land;

/* start the stage, and compile the land's models. against a Sonic in slot 0
 * the call puts Shadow in slot 1, and against a Shadow, Sonic. the rival's
 * work takes the id WORK_ID_RIVAL, and its pad goes off. ControlShadow steers
 * it. the call gives player_hooks.drop_rings ring_scatter, add_item_icon
 * item_icon_row_add and splash big_splash_create, each where it is null. it
 * gives stage_hooks message_window_open_unowned and the trail calls, and
 * makes its other four calls null */
void versus_sonic_1_init(void);
/* end the stage. it releases the textures, the messages and the set file. it
 * stops the fog, the particles, the set manager and the land. it clears each
 * of player_hooks.drop_rings, add_item_icon and splash that still holds what
 * init gave it */
void versus_sonic_1_exit(void);
/* the stage task's step. its first step gives camera 0 a BOSSPOINT mode.
 * the mode stands the camera behind player 0, on the far side from the
 * task's pos. each step puts that pos 10 above player 1, where slot 1 holds a
 * player */
void versus_sonic_1_step(struct task *task);
/* the stage's object list, which versus_sonic_1_init gives to set_start */
extern struct object_list versus_sonic_1_objects;

#endif
