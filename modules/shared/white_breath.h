/* white breath: puffs of the snowp textures, and the task that puffs them as
 * a player's breath */
#ifndef SHARED_WHITE_BREATH_H
#define SHARED_WHITE_BREATH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

struct task;

/* snowp01 to snowp08: a puff's WHITE_BREATH_FRAMES frames, in the order it
 * shows them */
#define WHITE_BREATH_FRAMES 8
extern NJS_TEXLIST white_breath_texlist;

/* make a puff at pos, moving at spd, of size scale. each returns the puff's
 * particle, or null where the heap has no room for one.
 *
 * a puff adds its light to what lies behind it. it starts turned up to 40
 * degrees either way. it shows white_breath_texlist's textures in turn, from a
 * random point in the first. each step slows the puff, grows it and speeds it
 * upward.
 *
 * - white_breath_fade_create's puff is white. it turns up to half a degree a
 *   step either way, and grows by 0.35 a step. it shows the textures over 29
 *   to 32 steps. it then holds the last while its alpha halves to 0 over 28
 *   more.
 * - white_breath_short_create's puff is white. it turns from 0.6 degrees a
 *   step one way to 0.7 the other. it grows by 0.05 a step. it shows the
 *   textures over 36 to 41 steps, then ends without a fade.
 * - white_breath_tinted_create's puff moves, grows and lasts as
 *   white_breath_short_create's does. it is green, and turns up to a tenth of
 *   a degree a step either way. it draws depth_bias nearer the camera, as
 *   particle.h's depth_bias says.
 *
 * the first two kinds swing edge_angle up to 8 degrees either side of a
 * quarter turn, and back. their quads thus skew. a swing from one side to the
 * other takes 60 steps. the tinted kind's quads stay square */
struct particle *white_breath_fade_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                          Float scale);
struct particle *white_breath_short_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale);
struct particle *white_breath_tinted_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias);

/* make player's breath task, and return it. the task is parent's child, or at
 * task level 1 where parent is null. player must not be below 0. the call
 * returns null for a player above 1, and for one that has a breath task
 * already. it also returns null where task_create or task_create_child
 * fails.
 *
 * the task puffs a grey breath from unk_224 of player_records[player], out
 * along the record's unk_2c0. the task's timer gains 1 a step, or 2 while the
 * record's spd.x passes its consts.sprint_speed. a breath sets the timer back
 * to 0. a breath comes once the timer passes 180. it waits for a step whose
 * animation frame is no higher than the step before's. none comes while the
 * record's ahead probe has water above it. the task ends once its player has
 * no work or no record */
struct task *white_breath_create(int player, struct task *parent);

#endif
