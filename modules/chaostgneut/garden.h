/* the Neutral Garden: its task's step and its object list */
#ifndef CHAOSTGNEUT_GARDEN_H
#define CHAOSTGNEUT_GARDEN_H

#include "object/set.h"

struct task;

/* the kinds the garden's set file names by index */
extern struct object_kind chaostgneut_kinds[7];
/* chaostgneut_kinds and their count, the list chaostgneut_step gives to
 * set_start */
extern struct object_list chaostgneut_objects;

/* the step of the task CHAO.PRS creates for the Neutral Garden. the task must
 * hold a work. the step starts the garden and its land, neutral_land. it then
 * replaces itself with one that plays the garden's ambient sounds. it sets the
 * task's draw to one that draws the garden's sky. it sets GARDEN_NEUTRAL_PLAY
 * and GARDEN_KINDER_OPEN in garden_save.flags. under GCC, loading the set file
 * or starting the set manager or the land can fail. on such a failure the step
 * leaves nothing started, and runs again on the task's next run. destroying
 * the task releases the garden's set file, textures and messages, and stops
 * neutral_land */
void chaostgneut_step(struct task *task);

#endif
