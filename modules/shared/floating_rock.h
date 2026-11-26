/* the floating rock, ROCK in a stage's object list: a rock that turns and
 * bobs in the air. a hunter's strike breaks it into eight pieces */
#ifndef SHARED_FLOATING_ROCK_H
#define SHARED_FLOATING_ROCK_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the wind stones, and of the
 * rock and its pieces. each is ended by null */
extern NJS_TEXLIST *floating_rock_stone_texlists[];
extern NJS_TEXLIST *floating_rock_texlists[];

/* the wind stones' models, texture lists, masses and areas, by kind. the
 * masses and areas set their lift. shared/wind_stone.c reads them, and the
 * rock's object holds them among its own data */
extern NJS_CNK_MODEL *wind_stone_models[];
extern NJS_TEXLIST *wind_stone_texlists[];
extern Float wind_stone_masses[];
extern Float wind_stone_areas[];

/* the object list's step for ROCK. for a task that has an extra already, it
 * does nothing where placement_try_leave(task) returns TRUE. it gives task
 * the rock's record in extra, and the rock's step, draw and destructor. where
 * the heap can't give the record, it destroys task.
 *
 * task needs a work. pos places the rock, and ang turns it. the rock is
 * 1 + scl.x / 10 times its model's size. the heap must have room for the
 * rock's collision record, of one sphere, unchecked.
 *
 * the low byte of ang.y names the rock's enemy group, or is 0xff for none. a
 * group can hide an emerald piece. a rock in a group takes an enemy_work in
 * motion from enemy_work_alloc_grouped, which takes and clears that byte.
 * that call kills task where the heap can't give the enemy_work.
 *
 * the rock turns as it bobs. a bob rises (scl.y + 1) * (scl.z + 1) / 10
 * above pos and falls back. its length is 4 * (scl.z + 1) steps, cut to an
 * integer. its step is game_state.steps plus a random phase, modulo the
 * length. a length of 1 holds the rock at pos. so does a length of 0, since
 * the runtime gives a remainder by 0 as 0. scl.z must be above -1.25,
 * unchecked. at -1.25 or below, the length is negative. the remainder takes
 * it as unsigned, so the rock drifts ever farther from pos.
 *
 * a player that touches the rock breaks it where hunter_attacking is TRUE
 * for the player. the first break of its placement gives up the group's
 * piece by enemy_defeat. the pieces fly for 91 steps. the placement then
 * stops, and the task ends */
void floating_rock_init(struct task *task);

#endif
