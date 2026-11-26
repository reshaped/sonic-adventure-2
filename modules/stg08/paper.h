/* MONEY, Security Hall's banknotes, and the stage's build of shared/paper.c */
#ifndef STG08_PAPER_H
#define STG08_PAPER_H
#include <ninja.h>
#include "shared/paper.h"

struct task;

extern NJS_TEXLIST *security_money_textures[];
extern NJS_TEXLIST security_money_texlist;

/* paper_launch, with built, model's build. a piece draws built in model's
 * place where built isn't null, but for a fade under PAPER_FADE_COLORS */
void security_paper_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                           NJS_VECTOR *spd, struct paper_spin *spin,
                           NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *built,
                           NJS_TEXLIST *texlist, Sint16 flags);
/* build the banknote's model at buffer, and return the first free byte past
 * it */
Uint8 *security_money_build(Uint8 *buffer);
void security_money_init(struct task *task);

#endif
