#ifndef SHARED_STAGE_TEXTURE_GROUPS_H
#define SHARED_STAGE_TEXTURE_GROUPS_H

#include <ninja.h>

/* the texture lists of objects that many stages place, which the stages'
 * texture groups name. each array ends with null */
extern NJS_TEXLIST *missile_texlists[];
extern NJS_TEXLIST *messenger_texlists[];
extern NJS_TEXLIST *object_effect_texlists[];
extern NJS_TEXLIST *chao_pipe_texlists[];
extern NJS_TEXLIST *chao_box_texlists[];
extern NJS_TEXLIST *ring_effect_texlists[];

#endif
