/* the font Ninja's debug print draws with, as a vector-quantized texture */
#include <nj_debug.h>

/* the glyphs use only the codebook's first 54 entries */
struct nj_debug_font _nj_debug_char_ = {
#include "ninja/debugvq/_nj_debug_char_.inc"
};
