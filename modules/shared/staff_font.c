/* the glyphs and texture animation of ENDING.PRS and EVENT.PRS, from the
 * disc. its D lies between staff_art.c and staff_credits.c */
#include <ninja.h>

#include "shared/staff_art.h"

struct staff_glyph staff_glyphs[109] = {
#include "shared/staff_font/staff_glyphs.inc"
};
NJS_TEXANIM staff_photo_cell = {
#include "shared/staff_font/staff_photo_cell.inc"
};
