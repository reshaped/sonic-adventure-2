/* the font Ninja's debug print draws, a 128 by 128 VQ texture. it holds 256
 * glyphs, each 8 by 8 */
#ifndef NJ_DEBUG_H
#define NJ_DEBUG_H
#include <sg_xpt.h>

#define NJ_DEBUG_FONT_SIDE 128

/* the font as kmLoadTexture takes a VQ texture. a codebook comes first, then an
 * index into it for each 2 by 2 block of texels. each codebook entry holds four
 * ARGB1555 texels, two columns of two. a glyph takes 16 indices. the indices
 * run in twiddled order, which interleaves the bits of row and column. so a
 * glyph's 16 indices lie together, and the glyphs follow in the same order.
 * both arrays are Uint32 since kmLoadTexture takes the texture as words */
struct nj_debug_font {
    Uint32 codebook[256][2];
    Uint32 glyph_blocks[256][4];
};
extern struct nj_debug_font _nj_debug_char_;

#endif
