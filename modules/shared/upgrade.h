/* the upgrade stand, LEVUPDAI in a stage's object list. a player who touches
 * it takes the upgrade it holds */
#ifndef SHARED_UPGRADE_H
#define SHARED_UPGRADE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the stand's texture lists for texture_load_shared, ended by null. an
 * upgrade's model draws with its character's own textures */
extern NJS_TEXLIST *upgrade_texlists[];
/* the melody's textures, which a stage loads from levupdai_tmtex. the melody
 * shows nothing until they load */
extern NJS_TEXLIST upgrade_melody_texlist;
/* the sparkle's textures, which a stage loads from levupdai_thtex. the
 * sparkle shows nothing until they load */
extern NJS_TEXLIST upgrade_sparkle_texlist;

/* the object list's step for LEVUPDAI. task needs a work and a null motion.
 * the work's pos places the stand, and its ang turns it. scl.x's whole part
 * picks the upgrade, from 0 to 27. a whole part outside that range picks by
 * its low byte, modulo 28. a stand whose upgrade's game flag is set as it
 * starts stands empty. where the heap can't give the stand's record, the task
 * keeps this step to try again.
 *
 * a player's touch takes the upgrade. the take sets the upgrade's game flag,
 * and the upgrade's bit in that player's upgrades. it opens a window of the
 * upgrade's message, whose pages that player turns with A. the take lasts
 * until the window closes, or for one step where no window opens.
 *
 * during the take, ITEM_GET.ADX replaces the music. the stage's clock stops,
 * and the pause is disabled. player 0 stands still at the stand, whichever
 * player took the upgrade. its own pad stops driving it, and it is
 * invincible. camera 0 takes CAMERA_MODE_EASYSET onto its stack, and circles
 * player 0 from scl.y + 20 away.
 *
 * the take's end plays the music again and releases player 0. it resumes the
 * clock, and enables the pause and player 0's pad, whatever held before the
 * take. it clears player 0's invincibility only where the take gave it. the
 * mode on top of camera 0's stack comes off */
void upgrade_init(struct task *task);

#endif
