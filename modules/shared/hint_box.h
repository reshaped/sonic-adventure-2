/* the hint box, HINTBOX in a stage's object list. a player near the box can
 * ask it for one of the stage's hints. the box shows the hint in a message
 * window */
#ifndef SHARED_HINT_BOX_H
#define SHARED_HINT_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* free the hints hint_box_hints_load loaded, if any */
void hint_box_hints_free(void);
/* load the stage's hints from the table file name, by
 * table_file_read_language. it returns how many hints the file holds, or -1
 * where no file reads. while hints are loaded, it loads none and returns -1 */
Sint32 hint_box_hints_load(const char *name);

/* the object list's step for HINTBOX. unless placement_try_leave(task)
 * returns TRUE, it gives task the box's step, draw and destructor. it gives
 * task a record in task->motion and a collision record too. without room for
 * the box's record, task keeps this step and tries again at its next step.
 * the heap must have room for the collision record, unchecked. task needs a
 * work, and no record in motion.
 *
 * pos places the box, and ang.y turns it. the box's hint is the one whose
 * index is the absolute value of scl.x, cut to an integer. where no loaded
 * hint has that index, the box shows a stand-in text.
 *
 * a player within 50 of pos, as player_near measures, is near. where both
 * players are near, slot 0's player counts. while the box waits for an ask,
 * it offers the near player the action PLAYER_MODE_ASK_HINT. that player asks
 * by taking it. the box takes no ask while pad_player_is_enabled returns
 * FALSE for the asker. it takes none either for 30 steps after its last hint.
 * those steps count down only while the box's collision record lacks
 * COLLISION_TOUCHED.
 *
 * the box turns the asker's pad off while its hint's message window is open.
 * it turns the pad on again once no window is open. a box destroyed while its
 * hint shows closes the window, but leaves the pad off. a near player lights
 * the box up, and the fully lit box makes that player look at it */
void hint_box_init(struct task *task);

/* the box's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *hint_box_texlists[];

#endif
