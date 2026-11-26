/* the torch cup, TORCHCUP in Pyramid Cave's object list: a fire burning in a
 * cup on a post */
#ifndef STG28_TORCH_CUP_H
#define STG28_TORCH_CUP_H
#include <ninja.h>

struct task;

/* the cup's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_torch_cup_texlists[];

/* the object list's step for TORCHCUP. task needs a work. its pos places the
 * cup, and its ang.y turns it. the call sets ang.x and ang.z to 0. scl.x plus
 * 1 scales the cup and its fire.
 *
 * the call gives task a collision record of two shapes. the fire's hurts
 * players. the cup's pushes the shapes of every group, and none of theirs
 * pushes it. the fire's sound plays while player 0 lies within 100 of a point
 * above pos. the point's height is 75 times the scale. player 0's work must not
 * be null, unchecked */
void cave_torch_cup_init(struct task *task);

#endif
