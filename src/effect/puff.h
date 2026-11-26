/* the puffs a hunter's upgraded blows give off: small sprites of the
 * hunter's effect textures, which swell and drift away */
#ifndef EFFECT_PUFF_H
#define EFFECT_PUFF_H
#include <ninja.h>

/* start a puff of the player in slot, a task named exec in level 3. nothing
 * starts where the heap can't give the task or its record. player_motion[slot]
 * must not be null, and player_tasks[slot] must hold a hunter at each of the
 * puff's draws.
 *
 * the puff starts within 0.2 of pos along each axis, at a size from 0.1 to
 * 0.14. it swells by 1.5% a step. it drifts along the player's motion at 0.4.
 * each part of the drift then gains 0.05 to 0.1, and the drift slows by 5% a
 * step. the puff ends once its count of steps, which starts from 0 to 3,
 * reaches 24. it draws added to what lies behind it, in texture count / 4 of
 * the hunter's effects_texlist */
void hunter_puff_start(int slot, NJS_POINT3 *pos);

#endif
