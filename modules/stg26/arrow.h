/* ARROW in Egg Quarters' object list: an arrow on the floor, whose light
 * pulses */
#ifndef STG26_ARROW_H
#define STG26_ARROW_H
#include <ninja.h>

struct task;

/* the object list's step for ARROW. task needs a work. its pos places the
 * arrow, and its ang.y turns it. the whole part of |scl.z| modulo 3 picks the
 * red, green or blue arrow. scl.x + 1 scales its length. its light pulses
 * in step with the tint of shared/tint_pulse.h, from black to white and
 * back */
void quarters_arrow_init(struct task *task);
/* the red, green and blue arrows' texture lists, ended by null */
extern NJS_TEXLIST *quarters_arrow_texlists[];

/* each color's arrow, and the light drawn over it */
extern NJS_CNK_MODEL arrow_red_base;
extern NJS_CNK_MODEL arrow_green_base;
extern NJS_CNK_MODEL arrow_blue_base;
extern NJS_CNK_MODEL arrow_red_light;
extern NJS_CNK_MODEL arrow_green_light;
extern NJS_CNK_MODEL arrow_blue_light;

#endif
