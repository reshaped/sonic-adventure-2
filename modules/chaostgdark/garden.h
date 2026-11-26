/* the Dark Garden: its task's step and its object list */
#ifndef CHAOSTGDARK_GARDEN_H
#define CHAOSTGDARK_GARDEN_H

#include "object/set.h"

struct task;

/* the kinds the garden's set file names by index. chaostgdark_step gives the
 * list of them to set_start */
extern struct object_kind chaostgdark_kinds[15];
extern struct object_list chaostgdark_objects;

/* the step of the task CHAO.PRS creates for the Dark Garden. the task must
 * hold a work. the step starts the garden and its land, dark_land. it then
 * replaces itself with one that plays the garden's ambient sounds. under GCC,
 * loading the set file or starting the set manager or the land can fail. on
 * such a failure the step leaves nothing started, and runs again on the
 * task's next run. destroying the task releases the garden's set file,
 * textures and messages, and stops dark_land */
void chaostgdark_step(struct task *task);

#endif
