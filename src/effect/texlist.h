/* the texture list the effects share */
#ifndef EFFECT_TEXLIST_H
#define EFFECT_TEXLIST_H
#include <ninja.h>

/* effect_texlist names its 39 textures in effect_texnames, in this order:
 * barria_n01 to n08, barria_t01 to t11, muteki01 to 08, mutekiball, bubble
 * and flame01 to 10. a stage's program fills them by name, through
 * texture_load_shared */
#define EFFECT_TEXTURE_COUNT 39
extern NJS_TEXNAME effect_texnames[EFFECT_TEXTURE_COUNT];
extern NJS_TEXLIST effect_texlist;

/* indices in effect_texnames. a run's textures follow its first in order, so
 * EFFECT_BARRIER_T01 + 9 is barria_t10 */
#define EFFECT_BARRIER_N01 0
#define EFFECT_BARRIER_T01 8
#define EFFECT_MUTEKI01 19
#define EFFECT_MUTEKIBALL 27
#define EFFECT_BUBBLE 28

#endif
