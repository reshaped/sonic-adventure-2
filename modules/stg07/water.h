/* Aquatic Mine's water, the SuimenManager task. the water stands at one of
 * three levels, and switches pick the level */
#ifndef STG07_WATER_H
#define STG07_WATER_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST water_texlist;

/* the height water_height returns where no piece of the current level holds
 * the point's x and z */
#define WATER_NONE (-100000.0F)

/* start the water's manager at level 0, and return its task. it returns null
 * where a manager runs already, or where task_create gives no task */
struct task *water_start(void);
/* destroy the manager's task, where one runs. the manager starts on the first
 * of its steps, from the fourth on, that finds player 0. until then it has no
 * destructor, so a manager destroyed before it starts stays recorded.
 * water_start then starts no other, and a later water_stop destroys the freed
 * task again */
void water_stop(void);
/* the height of the water at pos, from the pieces of the current level in
 * turn. the first whose bounds hold pos's x and z gives its center's height.
 * it returns WATER_NONE where none does */
Float water_height(NJS_POINT3 *pos);

#endif
