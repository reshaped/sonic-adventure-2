/* the Hero Garden: its task's step and its object list */
#ifndef CHAOSTGHERO_GARDEN_H
#define CHAOSTGHERO_GARDEN_H

#include "object/set.h"

struct task;

/* the kinds the garden's set file names by index. chaostghero_step gives the
 * list of them to set_start */
extern struct object_kind chaostghero_kinds[14];
extern struct object_list chaostghero_objects;

/* the step of the task CHAO.PRS creates for the Hero Garden. the task must
 * hold a work. the step starts the garden and its land, hero_land. it then
 * replaces itself with one that plays the garden's ambient sounds. under GCC,
 * loading the set file or starting the set manager or the land can fail. on
 * such a failure the step leaves nothing started, and runs again on the
 * task's next run. destroying the task releases the garden's set file,
 * textures and messages, and stops hero_land */
void chaostghero_step(struct task *task);

#endif
