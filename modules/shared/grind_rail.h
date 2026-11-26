/* the grind rail, GLINDRB in a stage's object list. the players grind on it,
 * and it comes in three sizes. a light on it pulses, and a texture along it
 * scrolls */
#ifndef SHARED_GRIND_RAIL_H
#define SHARED_GRIND_RAIL_H
#include <ninja.h>

struct task;

/* each size's texture list and model. a model's first child has the
 * scrolling texture, and the child's sibling is the light */
extern NJS_TEXLIST grind_rail_0_texlist;
extern NJS_TEXLIST grind_rail_1_texlist;
extern NJS_TEXLIST grind_rail_2_texlist;
extern NJS_CNK_OBJECT grind_rail_0_object;
extern NJS_CNK_OBJECT grind_rail_1_object;
extern NJS_CNK_OBJECT grind_rail_2_object;

/* the rails' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *grind_rail_texlists[];

/* the object list's step for GLINDRB. unless placement_try_leave(task)
 * returns TRUE, it sets the rail up.
 *
 * task needs a work. scl.x picks the size: its whole part, taken unsigned,
 * modulo 3. the sizes' rails are 100, 150 and 200 long, along the model's x.
 * pos places the model and its rail, and ang turns them.
 *
 * a child task, set up by path_rail_init of stage/path_task.h, runs the path
 * the players grind on. the path is a copy from the heap, which task->extra
 * holds and the destructor frees. the rail has no path where the heap can't
 * give the copy, the child or the child's record.
 *
 * the texture's scroll along u is game_state.steps times the whole part of
 * scl.y. the light pulses with the sine of the work's timer, which
 * each later step raises by 640 */
void grind_rail_init(struct task *task);

#endif
