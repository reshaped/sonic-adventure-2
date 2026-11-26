/* the low flag, FLAGLOW in a stage's object list: shared/flag.h's flag,
 * with models and textures of its own. its frames' first texture is
 * dz_hata_long_low, where the flag's is dz_hata_long */
#ifndef SHARED_FLAG_LOW_H
#define SHARED_FLAG_LOW_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of FLAGLOW's frames, as
 * flag_blue_texlists and flag_red_texlists are FLAG's */
extern NJS_TEXLIST *flag_low_blue_texlists[];
extern NJS_TEXLIST *flag_low_red_texlists[];

/* the object list's step for FLAGLOW, as flag_init is FLAG's */
void flag_low_init(struct task *task);

#endif
