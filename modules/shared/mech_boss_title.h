/* the mech bosses' title: the boss's name, spelled a letter at a time over a
 * banner. Big Foot, Hot Shot and Flying Dog show it as their fight starts */
#ifndef SHARED_MECH_BOSS_TITLE_H
#define SHARED_MECH_BOSS_TITLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* show boss's title as parent's child, and return its task. the title's top
 * left lies at *pos on the screen, and pos->z is its depth, as
 * njDrawSprite2D's pri. the title shows until the caller ends its task. boss
 * must be one of enum mech_boss of shared/mech_boss.h, unchecked. any other
 * spells a name from an unset pointer.
 *
 * the title shows three parts, each a child task. the banner, the first texture
 * of boss's title texture list, scrolls past from the top with
 * MECH_BOSS_SOUND_TITLE_BANNER. it ends itself after 335 steps. a row of the
 * list's other eight textures fades in over 125 steps. the boss's name spells
 * out a letter at a time, with MECH_BOSS_SOUND_TITLE_LETTER at each. the
 * parts hold still while game_state.time_stopped, game_state.unk_09 or
 * game_state.mini_event_running isn't 0. they draw nothing while
 * mini_event_running isn't 0. the call loads no texture: the caller loads
 * them, as mech_boss_textures_load does.
 *
 * it returns null where task_create_child fails. it also returns null where a
 * part can't be made. the title's task then ends on its next step. parent
 * must not be null: for a null parent the result is undefined */
struct task *mech_boss_title_create(struct task *parent, Sint8 boss,
                                    NJS_POINT3 *pos);

#endif
