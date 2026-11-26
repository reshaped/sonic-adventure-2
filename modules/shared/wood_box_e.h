/* the wooden box of shared/wood_box.h over models of its own, whose texture
 * lists name sikake_18_128e for sikake_18_128. CONTWOOD is this box in the
 * object lists of Death Chamber, Egg Quarters and Pyramid Cave. Hidden Base's
 * list names both boxes CONTWOOD */
#ifndef SHARED_WOOD_BOX_E_H
#define SHARED_WOOD_BOX_E_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* wood_box_texlists, wood_box_build and wood_box_init, for this box. a box
 * draws and throws only its own kind's builds */
extern NJS_TEXLIST *wood_box_e_texlists[];
Uint8 *wood_box_e_build(Uint8 *buf);
void wood_box_e_init(struct task *task);

#endif
