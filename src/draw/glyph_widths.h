/* the glyph widths of the two single-byte 24-pixel message fonts */
#ifndef DRAW_GLYPH_WIDTHS_H
#define DRAW_GLYPH_WIDTHS_H
#include <sg_xpt.h>

/* the widths of efmsgfont_ascii24E.bin's characters, 0x20 to 0x7f */
static const Sint8 ascii_widths[96] = {
#include "draw/glyph_widths/ascii_widths.inc"
};
/* the widths of efmsgfont_ascii24S.bin's characters, 0x20 to 0xff */
static const Sint8 latin_widths[224] = {
#include "draw/glyph_widths/latin_widths.inc"
};

#endif
