/* the fluorescent lamp, FLUO in Iron Gate's object list. it is the lamp of
 * shared/fluorescent_lamp.h, compiled again with art of its own */
#ifndef STG11_FLUORESCENT_LAMP_H
#define STG11_FLUORESCENT_LAMP_H
#include <ninja.h>

struct task;

/* the lamp's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *iron_gate_fluorescent_lamp_texlists[];

/* the object list's step for FLUO. it does what fluorescent_lamp_init does,
 * but its draw tests a ball of radius 35 about the lamp. the draw draws nothing
 * while the ball lies wholly behind the camera */
void iron_gate_fluorescent_lamp_init(struct task *task);

#endif
