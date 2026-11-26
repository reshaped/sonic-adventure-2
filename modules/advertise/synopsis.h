/* the story synopsis: lines that tell a story so far. they scroll up over a
 * backdrop of one of its characters, each voiced as it passes */
#ifndef ADVERTISE_SYNOPSIS_H
#define ADVERTISE_SYNOPSIS_H
#include <sg_xpt.h>

#include "advertise/menus.h"
#include "system/sequence.h"

/* the stories, each its sequence less one, as the story select and
 * save_data.progress index them */
#define STORY_HERO (SEQUENCE_HERO - 1)
#define STORY_DARK (SEQUENCE_DARK - 1)
#define STORY_LAST (SEQUENCE_LAST - 1)

/* the synopsis's work, menu_works.work_06 */
extern struct synopsis_work synopsis_work;

/* create the task, arasujiExec, with the work and the lines' sprites
 * cleared. while scrolling is 1, the lines scroll up a pixel a step. while
 * it is 0, the lines stay at their start and done stays 0. the task's
 * destructor releases the textures and frees the sprites */
void synopsis_create(void);
/* clear the work but loaded, and hide the backdrop at once with the scroll
 * at its start. synopsis_from_start is then FALSE */
void synopsis_reset(void);
/* load the backdrop's textures, or release them, and set loaded to match.
 * the backdrop is the work's character's. neither does anything where
 * loaded matches already */
void synopsis_load(void);
void synopsis_release(void);
/* set the work's set to the synopsis of story at step, its progress. that
 * is the story's last set whose step is at most step, or else its first. the
 * work's character becomes the set's, but PLAYER_KIND_AMY for STORY_LAST's
 * first set. story must be a STORY_ value, unchecked */
void synopsis_select(int story, int step);
/* TRUE at the story's first step, or when the disc-free registry has no
 * synopsis. synopsis_reset makes it FALSE. */
Bool synopsis_from_start(void);

#endif
