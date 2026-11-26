/* SIGNEND in the object lists of Final Rush and Final Chase. it is a model
 * that bobs, whose child pulses between two colors */
#ifndef STG40_SIGN_END_H
#define STG40_SIGN_END_H
#include <ninja.h>

struct task;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *sign_end_texlists[];

/* the object list's step for SIGNEND. task needs a work, whose pos places
 * the model and whose ang turns it. a scl.x above 0 draws the second model.
 * the model bobs up to 5 above and below pos. a sphere of radius 20 at pos
 * pushes players, enemies and objects. every 32 steps the object plays a
 * sound of the stage's sound list at pos. in Final Chase it is sound 10, or
 * sound 11 where scl.x is 1 or more. in Final Rush they are 2 and 3 */
void sign_end_init(struct task *task);

#endif
