#include <ninja.h>
#include <sg_xpt.h>

#include "event/equipment.h"
#include "event/scene.h"
#include "player/player.h"
#include "system/flags.h"

/* the index in upgrades of the third list: the parts upgrades replace */
/* @hack spelling: 62, where 2 * EVENT_UPGRADES would do */
#define REPLACED_PARTS 62
/* the entries no upgrade fills */
#define NO_UPGRADE (-1)

/* clang-format off */
static Sint32 upgrade_flags[EVENT_UPGRADES] = {
    UPGRADE_SONIC_SHOES, UPGRADE_SONIC_SPARKLE, UPGRADE_SONIC_MAGICWRIST,
    UPGRADE_SONIC_FLAMERING, UPGRADE_SONIC_RUBBERUNIT, UPGRADE_SONIC_MELODY,
    UPGRADE_TWALKER_BOOSTER, UPGRADE_TWALKER_HYPERCANNON,
    UPGRADE_TWALKER_LAZERBALSTER, UPGRADE_TWALKER_MELODY,
    UPGRADE_KNUCKLES_CLAWS, UPGRADE_KNUCKLES_SUNGLASS,
    UPGRADE_KNUCKLES_GROVES, UPGRADE_KNUCKLES_AIR, UPGRADE_KNUCKLES_MELODY,
    UPGRADE_UNK_15,
    UPGRADE_SHADOW_SHOES, UPGRADE_SHADOW_SPARKLE, UPGRADE_SHADOW_FLAMERING,
    UPGRADE_SHADOW_MELODY,
    UPGRADE_EWALKER_JETENGINE, UPGRADE_EWALKER_BAZOOKA,
    UPGRADE_EWALKER_POWERGUN, UPGRADE_EWALKER_EXTRASHIELD,
    UPGRADE_EWALKER_MELODY,
    UPGRADE_ROUGE_NAILS, UPGRADE_ROUGE_SCOPE, UPGRADE_ROUGE_BOOTS,
    UPGRADE_ROUGE_MELODY,
    NO_UPGRADE, NO_UPGRADE};
/* clang-format on */

static void hide_tree(NJS_CNK_OBJECT *node)
{
    node->evalflags |= NJD_EVAL_HIDE;
    if (node->child) {
        hide_tree(node->child);
    }
    if (node->sibling) {
        hide_tree(node->sibling);
    }
}

#pragma inline(hide_part)
static void hide_part(NJS_CNK_OBJECT *node)
{
    node->evalflags |= NJD_EVAL_HIDE;
    if (node->child) {
        hide_tree(node->child);
    }
}

void event_hide_unequipped(void)
{
    int i;

    for (i = 0; i < EVENT_UPGRADES; i++) {
        /* @bug memory: the call at 0x8c50767a passes flag_get the two
         * NO_UPGRADE entries, and flag_get reads the byte before game_flags
         * for each. that byte is the high byte of mini_event_fade, which
         * holds only 0 to 127. so flag_get returns 0 for both, as for an
         * upgrade the save lacks */
#ifdef NONMATCHING
        /* this side keeps the tag's result without the read below game_flags. a
         * NO_UPGRADE entry counts as an upgrade the save lacks */
        if (upgrade_flags[i] >= 0 && flag_get(upgrade_flags[i])) {
#else
        if (flag_get(upgrade_flags[i])) {
#endif
            if (event_scene_file->upgrades[i + REPLACED_PARTS]) {
                hide_part(event_scene_file->upgrades[i + REPLACED_PARTS]);
            }
        } else {
            if (event_scene_file->upgrades[i]) {
                hide_part(event_scene_file->upgrades[i]);
            }
            if (event_scene_file->upgrades[i + EVENT_UPGRADES]) {
                hide_part(event_scene_file->upgrades[i + EVENT_UPGRADES]);
            }
        }
    }
}
