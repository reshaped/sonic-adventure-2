/* black smoke: dark puffs of the dust textures, the mud a player kicks up,
 * and the smoke of a broken-down player */
#ifndef SHARED_BLACK_SMOKE_H
#define SHARED_BLACK_SMOKE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

struct task;

/* make a puff of black smoke at pos, moving at spd, of size scale, and return
 * its particle. each returns null where the heap can't give one.
 *
 * a puff starts turned up to 10 degrees either way. it turns up to half a
 * degree a step either way. it shows dust_texlist's eight textures in turn,
 * starting a random part of the way into the first. each step slows the puff to
 * 0.98 of its speed, and grows it. its quad leans up to 8 degrees either side
 * of square, and back.
 *
 * black_smoke_rising_create's puff gains 0.0065 of upward speed a step. it
 * shows the eight textures over 64 to 73 steps. it then shows the last texture
 * for 70 more steps. its alpha halves every 10 steps until it reaches 0.
 *
 * black_smoke_sinking_create's puff loses 0.001 of upward speed a step, and
 * grows faster. it shows the eight textures over 117 to 134 steps, then ends
 * without a fade */
struct particle *black_smoke_rising_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale);
struct particle *black_smoke_sinking_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale);
/* make count of black_smoke_rising_create's puffs of size scale, on a level
 * ring of radius about pos. each puff moves at spd, plus 0.03 times scale
 * upward and 0.05 times scale outward from pos.
 *
 * the puffs count from 0. puff k lies k(k + 1) / 2 times a full turn / count
 * around the ring. unless count is a power of 2, some puffs lie on or beside
 * others. count must be from 1 to 255. the loop counts the puffs in a Uint8, so
 * a call with a larger count never returns */
void black_smoke_rising_burst(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                              Float scale, int count);

/* make player's mud task or smoke task, and return it. the task is parent's
 * child, or at task level 1 where parent is null. player must not be below 0.
 * each call returns null for a player above 1, and for one that has such a task
 * already. it also returns null where the heap can't give the task.
 *
 * the mud task kicks up sinking puffs as its player lands, runs or brakes. it
 * does so only where the probe ahead of the player finds WORLD_FLAT water over
 * a WORLD_DUSTY floor. that probe is ahead, in the player's record. it ends
 * once its player has no work. a player with a work must have a record,
 * unchecked.
 *
 * the smoke task puffs smoke from its player while the player's gauge is 9.375
 * or less. that gauge is a mech's, in the player's record. the lower the gauge,
 * the more often the smoke comes. it waits while game_state.time_stopped isn't
 * 0. the task ends once its player has no record */
struct task *muddy_create(int player, struct task *parent);
struct task *broken_down_smoke_create(int player, struct task *parent);

#endif
