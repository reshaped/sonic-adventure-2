/* the fish, FISH in Dry Lagoon's object list: a school that swims in a box */
#ifndef STG18_FISH_H
#define STG18_FISH_H

#include <ninja.h>

struct task;

/* the three kinds' texture lists, for texture_load_shared, ended by null */
extern NJS_TEXLIST *lagoon_fish_texlists[];

/* the object list's step for FISH. unless placement_try_leave_by(task, range)
 * returns TRUE, it gives task the school's step, draw and destructor. range is
 * (radius + 400) squared, plus 10, where the school's radius is the largest of
 * scl's coordinates. task needs a work, TASK_EXTRA's record and no record in
 * motion. the low byte of ang.z is the count of fish. the call puts them in
 * task->motion, from the Shinobi heap. the heap must have room for them,
 * unchecked. the destructor frees them. player 0's work must exist, unchecked:
 * the step reads its pos while a fish swims.
 *
 * pos is the school's center, and ang.y turns the school. each fish starts at
 * random in the box of half-extents scl about pos, and bounces off its faces.
 * every fish swims and draws while the camera lies within the radius plus 105
 * of pos. fewer do as the camera recedes, and none from the radius plus 400.
 *
 * now and then a fish chooses a course. a choosing fish within a distance of
 * 30 of player 0's offset from pos swims straight from it. the swim reads the
 * offset, taken in the world's axes, as if ang.y were 0 */
void lagoon_fish_init(struct task *task);

#endif
