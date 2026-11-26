/* the calls the kindergarten's classroom makes into the Chao world */
#ifndef CHAO_CLASSROOM_H
#define CHAO_CLASSROOM_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

Sint32 garden_play_time(void);
/* set the Chao's classroom slot, -1 for none, in its work and its save, and
 * return TRUE. a Chao with no save keeps its slot, and the call returns
 * FALSE */
int chao_set_classroom_slot(struct task *chao, int slot);
/* spawn the saved Chao, other than an egg, whose classroom slot is slot, and
 * return its task. where no Chao has the slot, the result is undefined */
struct task *garden_spawn_classmate(int slot, NJS_POINT3 *position, Angle yaw);

#endif
