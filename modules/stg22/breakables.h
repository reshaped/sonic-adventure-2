/* Crazy Gadget's wooden and iron boxes, its lid and its missile cage */
#ifndef STG22_BREAKABLES_H
#define STG22_BREAKABLES_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the boxes and the lid fall along world_gravity. each stays between its
 * placed pos and pos + scl, axis by axis */
void gadget_wood_box_init(struct task *task);
void gadget_iron_box_init(struct task *task);
void gadget_lid_init(struct task *task);
void gadget_cage_init(struct task *task);
Uint8 *gadget_cage_build(Uint8 *buf);
extern NJS_TEXLIST *gadget_wood_box_texlists[], *gadget_iron_box_texlists[];
extern NJS_TEXLIST *gadget_lid_texlists[], *gadget_cage_texlists[];
#endif
