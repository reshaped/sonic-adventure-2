/* NOREN in Egg Quarters' object list: a hanging curtain, which only draws.
 * nothing collides with it */
#ifndef STG26_CURTAIN_H
#define STG26_CURTAIN_H
#include <ninja.h>

struct task;

/* the object list's step for NOREN. task needs a work. its pos places the
 * curtain, and its ang.y turns it. the curtain draws its first frame, and
 * nothing moves it on to another */
void quarters_curtain_init(struct task *task);
/* a texture list for each of the CURTAIN_FRAMES frames of
 * stg26/curtain_art.h, ended by null */
extern NJS_TEXLIST *quarters_curtain_texlists[];

#endif
