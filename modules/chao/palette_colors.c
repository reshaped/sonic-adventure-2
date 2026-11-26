/* each palette's CHAO_PALETTE_COLORS colors, which garden_load_palettes widens
 * from its palette file. each color is four bytes: blue, green, red and
 * alpha */
#include <sg_xpt.h>
#include "chao/palette.h"

Uint8 garden_palette_colors[CHAO_PALETTES][CHAO_PALETTE_BYTES];
