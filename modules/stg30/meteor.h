/* Final Rush's meteors: the rocks its set files place, and those that the
 * background throws */
#ifndef STG30_METEOR_H
#define STG30_METEOR_H
#include <ninja.h>

struct task;

/* the four rocks' texture lists, then the falling meteor's, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *meteor_texlists[];

/* the whole part of scl.x, taken unsigned, picks one of four rocks, mod 4. a
 * moving rock adds scl.y and scl.z to its spin about x and y. each fits a
 * sphere to its rock's model */
void meteor_move_init(struct task *task);
void meteor_set_init(struct task *task);

/* a meteor the background throws. its work's scl is its speed, and its ang.z
 * its life in steps */
void meteor_fall_init(struct task *task);

#endif
