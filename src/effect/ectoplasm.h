/* ectoplasm: puffs that a player gives off for a time, which drift against
 * gravity and fade */
#ifndef EFFECT_ECTOPLASM_H
#define EFFECT_ECTOPLASM_H
#include <sg_xpt.h>

/* give the player in slot player a puff of ectoplasm a step, for steps
 * steps. a task named PlayerEffectEctoplasmExecutor in level 2 gives them.
 * nothing starts where the heap can't give the task. the task ends sooner at
 * the step that finds player_tasks[player] null, or PLAYER_MISSED in the
 * record's flags. while the slot holds a task, it must hold a work and a
 * record, unchecked.
 *
 * each puff is a task named EffectEctoplasmExecutor in level 3. a puff the
 * heap can't give is left out. it starts within half the player's size of
 * where the player stands, along each axis. the player stands at unk_218 of
 * the record for a mech, and unk_230 for any other. the size is the work's x
 * scale times 15 for a mech, and 6 for any other. a puff moves at a tenth of
 * minus world_gravity a step. it draws as a sprite of barria_t10, added to
 * what lies behind it. its bluish grey keeps 0.98 of itself each step. its
 * 31st step that finds game_state.time_stopped at 0 ends it */
void player_ectoplasm_start(int player, Sint16 steps);

#endif
