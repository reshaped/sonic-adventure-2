/* the texture lists of the stage HUD's life icons, result sheet and
 * two-player banners, which src/system/texture_lists.c defines */
#ifndef HUD_TEXTURE_LISTS_H
#define HUD_TEXTURE_LISTS_H
#include <ninja.h>

/* the life icons, a texture list for each look. Eggman's and Big's hold two
 * textures, and the others one */
extern NJS_TEXLIST zanki_sonic_texlist;
extern NJS_TEXLIST zanki_knuckle_texlist;
extern NJS_TEXLIST zanki_tails_texlist;
extern NJS_TEXLIST zanki_egg_texlist;
extern NJS_TEXLIST zanki_shadow_texlist;
extern NJS_TEXLIST zanki_rouge_texlist;
extern NJS_TEXLIST zanki_amy_texlist;
extern NJS_TEXLIST zanki_metal_texlist;
extern NJS_TEXLIST zanki_tical_texlist;
extern NJS_TEXLIST zanki_chaos0_texlist;
extern NJS_TEXLIST zanki_chao_texlist;
extern NJS_TEXLIST zanki_big_texlist;

/* result_texlist holds the result sheet's textures. game2p_texlist holds the
 * one texture of the two-player banners and winner screen. a two-player stage
 * loads it from GAME2PTEX */
extern NJS_TEXLIST result_texlist;
extern NJS_TEXLIST game2p_texlist;

#endif
