/* the round shadow, SHADOW in Death Chamber's object list: a model textured as
 * a round shadow. it comes in two sizes */
#ifndef STG25_ROUND_SHADOW_H
#define STG25_ROUND_SHADOW_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the shadow's texture lists for texture_load_shared, the large size's and the
 * small size's, ended by null */
extern NJS_TEXLIST *chamber_round_shadow_texlists[];

/* Other placed objects draw the small shadow beneath their own models. */
extern NJS_TEXLIST chamber_round_shadow_small_texlist;
extern NJS_CNK_MODEL chamber_round_shadow_small_model;

/* the object list's step for SHADOW. task needs a work. its pos places the
 * shadow, and its ang.y turns it. scl.x plus 1 scales it along x, and scl.y
 * plus 1 along z. the whole part of |scl.z| modulo 2 picks its size: 0 is
 * large, and 1 small */
void chamber_round_shadow_init(struct task *task);

#endif
