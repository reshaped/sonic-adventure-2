/* the chimney, CHIMNEY in Pyramid Cave's object list. at random it bursts: a
 * blast flares above it while its model swells and settles */
#ifndef STG28_CHIMNEY_H
#define STG28_CHIMNEY_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the chimney's six models and
 * of a burst's ten blasts. a model's or a frame's lies at its index, and each
 * array ends in null */
extern NJS_TEXLIST *cave_chimney_texlists[];
extern NJS_TEXLIST *cave_chimney_blast_texlists[];

/* the object list's step for CHIMNEY. task needs a work. its pos places the
 * chimney, and its ang.y turns it. scl.x plus 1 scales the chimney as it
 * draws. the call gives task a collision record of one cylinder, 50 across
 * and 600 high. its center lies 190 below pos. the cylinder pushes
 * players, enemies and objects, and resists their push.
 *
 * each step without a burst starts one by a chance of 1 in 300. a burst runs
 * through 10 frames, and takes the next on each step that finds
 * game_state.steps even. sound 12 of the stage's list marks its start. sound 13
 * plays on each later step of it. each plays 120 above pos, and only
 * while player 0 lies within 250 of there */
void cave_chimney_init(struct task *task);

#endif
