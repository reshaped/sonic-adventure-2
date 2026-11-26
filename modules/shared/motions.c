/* the motions a module adds to the players' */
#include <ninja.h>

#include "player/action.h"
#include "player/player.h"
#include "shared/motions.h"

void module_motions_register(void)
{
    /* @hack spelling: list, a local for module_motions, where module_motions
     * indexed in place would do */
    struct motion_entry *list = module_motions;
    /* @hack spelling: motion, a local for the entry's motion, where
     * list[i].motion stored in place would do */
    NJS_MOTION *motion;
    int i;

    /* @hack spelling: the list's end tested inside an endless loop, where
     * for (i = 0; list[i].index != -1; i++) would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MOTION_SLOTS &&
            motion_table[list[i].index].motion == NULL) {
            motion = list[i].motion;
            motion_table[list[i].index].nodes = list[i].nodes;
            motion_table[list[i].index].motion = motion;
        }
    }
}

void module_motions_unregister(void)
{
    /* @hack spelling: list, a local for module_motions, where module_motions
     * indexed in place would do */
    struct motion_entry *list = module_motions;
    struct player *player;
    int i;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((player = player_records[0]) != NULL) {
        player_anim_end_blend(&player->anim);
        /* @hack spelling: the record reached again through
         * player_records[player->slot], where player would do while slot is
         * its own index */
        player_records[player->slot]->anim.flags |= PLAYER_ANIM_NO_BLEND;
        player_stand(player_work[0], player_motion[0], player);
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((player = player_records[1]) != NULL) {
        player_anim_end_blend(&player->anim);
        /* @hack spelling: the record reached again through
         * player_records[player->slot], where player would do while slot is
         * its own index */
        player_records[player->slot]->anim.flags |= PLAYER_ANIM_NO_BLEND;
        player_stand(player_work[1], player_motion[1], player);
    }
    /* @hack spelling: the list's end tested inside an endless loop, where
     * for (i = 0; list[i].index != -1; i++) would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MOTION_SLOTS &&
            motion_table[list[i].index].motion == list[i].motion) {
            /* @hack spelling: motion cleared before nodes, not in the
             * members' order */
            motion_table[list[i].index].motion = NULL;
            motion_table[list[i].index].nodes = 0;
        }
    }
}
