/* the iron box of shared/iron_box.h over models of its own, whose texture
 * lists name sikake_20_128e for sikake_20_128. CONTIRON is this box in the
 * object lists of Death Chamber, Egg Quarters and Pyramid Cave. Hidden Base's
 * list names both boxes CONTIRON */
#ifndef SHARED_IRON_BOX_E_H
#define SHARED_IRON_BOX_E_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* iron_box_texlists, iron_box_build and iron_box_init, for this box. a box
 * draws and throws only its own kind's builds */
extern NJS_TEXLIST *iron_box_e_texlists[];
Uint8 *iron_box_e_build(Uint8 *buf);
void iron_box_e_init(struct task *task);

#endif
