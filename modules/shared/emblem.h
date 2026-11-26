/* the emblem screen: an emblem that turns, and the count of emblems.
 * EMBLEMGET.PRS and CHAOSTGRACE.PRS hold it */
#ifndef SHARED_EMBLEM_H
#define SHARED_EMBLEM_H
#include <sg_xpt.h>

/* 1 from emblem_start until the emblem's task ends, or until the
 * congratulations it starts end */
extern Sint32 emblem_screen_open;

/* TRUE while emblem_screen_open isn't 0 */
Bool emblem_screen_is_open(void);
/* start the emblem screen, whose task is getEmblemExec in task level 2. the
 * call sets up the screen's camera and loads its textures. it sets
 * emblem_screen_open to 1 and game_state.unlock_pending to 0.
 *
 * the emblem turns through its motion, with sparkles about it. the screen
 * counts its steps. it shows the old count, game_state.emblems, as it fades in,
 * until step 180. it then shows the new count, save_data.emblems, with a sound.
 * it fades out from step 300 to step 360. it ends at step 420. a screen whose
 * two counts are equal ends on its first step.
 *
 * the task's end starts the congratulations where game_state.unlock_id is above
 * 0. otherwise it sets emblem_screen_open to 0. the task's destructor sets
 * game_state.emblems to save_data.emblems */
void emblem_start(void);

#endif
