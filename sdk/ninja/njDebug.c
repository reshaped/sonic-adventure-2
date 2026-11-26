/* Ninja's debug print: text drawn as sprites, one to a character, from a font
 * texture */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_debug.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef NONMATCHING
/* readout_print, the port's print, which this side defines below */
#include "engine/dc/port.h"
#endif

/* the index in _nj_context_ of the print's vertex context */
#define PRINT_CONTEXT 4
/* the low FONT_COLUMN_BITS bits of a character's code give its column in the
 * font. the bits above give its row. a font has as many rows as columns */
#define FONT_COLUMN_BITS 4
#define FONT_COLUMNS (1 << FONT_COLUMN_BITS)
/* the bits of val that njPrintH writes as one digit */
#define HEX_DIGIT_BITS 4
#define HEX_DIGIT_MASK ((1 << HEX_DIGIT_BITS) - 1)

static char hex_digits[1 << HEX_DIGIT_BITS] = {'0', '1', '2', '3', '4', '5',
                                               '6', '7', '8', '9', 'A', 'B',
                                               'C', 'D', 'E', 'F'};

/* the color and size njInitPrint gives the print, as ninja.h says */
#define DEFAULT_PRINT_COLOR 0xffbfbfbf
#define DEFAULT_PRINT_SIZE 8.0F
/* the sprites' packed ARGB color */
static Uint32 print_color;
/* each character's side, in pixels */
static Float print_size;
/* the font's surface in texture memory */
static NJS_TEXSURFACE font;
/* the four words that open the sprites' parameter, which
 * kmProcessVertexRenderState packs from the print's vertex context */
static Uint32 tex_param;
static Uint32 tsp_param;
static Uint32 global_param;
static Uint32 isp_param;
/* the store queue address of the next block the open print fills */
static Uint32 *print_sq;
/* the translucent list's next address when the print opened */
static Uint32 list_next;
/* list_next's area, as QACR0 and QACR1 take it */
static Uint32 sq_area;
/* list_next's bits above SQ_OFFSET, which a closing print puts back above
 * print_sq's offset */
static Uint32 list_top;

void njInitPrint(Uint32 *texture, Uint32 size, Uint32 attr)
{
    print_color = DEFAULT_PRINT_COLOR;
    print_size = DEFAULT_PRINT_SIZE;
    if (texture == NULL) {
        kmCreateTextureSurface((PKMSURFACEDESC)&font, NJ_DEBUG_FONT_SIDE,
                               NJ_DEBUG_FONT_SIDE,
                               NJD_TEXFMT_VQ | NJD_TEXFMT_ARGB_1555);
        kmLoadTexture((PKMSURFACEDESC)&font, (KMDWORD *)&_nj_debug_char_, FALSE,
                      FALSE);
    } else {
        kmCreateTextureSurface((PKMSURFACEDESC)&font, size, size, attr);
        kmLoadTexture((PKMSURFACEDESC)&font, texture, FALSE, FALSE);
    }
    _nj_context_[PRINT_CONTEXT].RenderState =
        KM_PARAMTYPE | KM_LISTTYPE | KM_COLORTYPE | KM_UVFORMAT |
        KM_SHADINGMODE | KM_USESPECULAR | KM_USEALPHA;
    _nj_context_[PRINT_CONTEXT].ParamType = KM_SPRITE;
    _nj_context_[PRINT_CONTEXT].ListType = KM_TRANS_POLYGON;
    _nj_context_[PRINT_CONTEXT].ColorType = KM_PACKEDCOLOR;
    _nj_context_[PRINT_CONTEXT].ShadingMode = KM_TEXTUREFLAT;
    _nj_context_[PRINT_CONTEXT].pTextureSurfaceDesc = (PKMSURFACEDESC)&font;
    _nj_context_[PRINT_CONTEXT].bUseSpecular = FALSE;
    _nj_context_[PRINT_CONTEXT].bUseAlpha = TRUE;
    _nj_context_[PRINT_CONTEXT].UVFormat = KM_16BITUV;
    kmProcessVertexRenderState(&_nj_context_[PRINT_CONTEXT]);
    global_param = _nj_context_[PRINT_CONTEXT].GLOBALPARAMBUFFER;
    isp_param = _nj_context_[PRINT_CONTEXT].ISPPARAMBUFFER;
    tsp_param = _nj_context_[PRINT_CONTEXT].TSPPARAMBUFFER;
    tex_param = _nj_context_[PRINT_CONTEXT].TexturePARAMBUFFER;
}

void njExitPrint(void)
{
    kmFreeTexture((PKMSURFACEDESC)&font);
}

void njPrintSize(Uint16 size)
{
    print_size = size;
}

void njPrintColor(Uint32 c)
{
    print_color = c;
}

/* a print fills each 32-byte block downward, to its first word. a prefetch
 * of that word then sends the block. SQ_PUT steps print_sq back a word and
 * stores value there */
/* @hack spelling: a block's words stored as print_sq steps back from its end,
 * where indexed stores from its start would do */
#define SQ_PUT(type, value) (*(type *)--print_sq = (value))

/* a sprite's uv word holds u in its high half and v in its low half. each is
 * a float cut to its high halfword, which keeps 7 bits of its mantissa.
 * HIGH_HALF keeps a float's high halfword in place, and HIGH_HALF_SHIFT moves
 * it down */
#define HIGH_HALF 0xffff0000
#define HIGH_HALF_SHIFT 16
/* FONT_CELL is a cell's side in texture coordinates. a character's u and v
 * start CELL_INSET inside its cell. its right and bottom edges lie on its
 * cell's. 7 bits of mantissa hold the inset only for columns and rows 0 to 3.
 * from 4 on, a character's left and top edges lie on its cell's too */
#define FONT_CELL (1.0F / FONT_COLUMNS)
#define CELL_INSET (1.0F / 1024)
/* the depth of every character, as the 1/w a sprite's corner takes */
#define PRINT_DEPTH 9.0F

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* send c as a sprite of the print's color, print_size pixels a side. its top
 * left corner lies at (x, y). the store queues must be mapped as an open print
 * maps them. the call moves print_sq past the sprite.
 *
 * the sprite's corners run clockwise from the top left. its first block holds
 * the control word, the first two corners and the third's x. its second holds
 * the rest of the corners, then the u and v of all but the fourth */
void njCharacter(Sint16 x, Sint16 y, char c)
{
    /* unsigned, so that codes 128 to 255 take rows 8 to 15 */
    Uint8 ch;
    Sint16 col;
    Sint16 row;
    Float u;
    Float v;
    Float u2;
    Float v2;
    Uint32 left;
    Uint32 right;
    Uint32 top;
    Uint32 bottom;

    ch = c;
    col = ch & (FONT_COLUMNS - 1);
    row = ch >> FONT_COLUMN_BITS;
    u = (col * FONT_CELL) + CELL_INSET;
    v = (row * FONT_CELL) + CELL_INSET;
    u2 = u + (FONT_CELL - CELL_INSET);
    v2 = v + (FONT_CELL - CELL_INSET);
    /* @hack spelling: bottom read before top, where top first would do */
    left = *(Uint32 *)&u;
    right = *(Uint32 *)&u2;
    bottom = *(Uint32 *)&v2;
    top = *(Uint32 *)&v;
    /* @hack spelling: each word cut in a statement after its read, where one
     * statement for each would do */
    /* @hack spelling: the cuts in the order left, top, bottom and right,
     * where the reads' order would do */
    left &= HIGH_HALF;
    top >>= HIGH_HALF_SHIFT;
    bottom >>= HIGH_HALF_SHIFT;
    right &= HIGH_HALF;
    print_sq += SQ_WORDS;
    SQ_PUT(Float, x + print_size);
    SQ_PUT(Float, PRINT_DEPTH);
    SQ_PUT(Float, y);
    SQ_PUT(Float, x + print_size);
    SQ_PUT(Float, PRINT_DEPTH);
    SQ_PUT(Float, y);
    SQ_PUT(Float, x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_ENDOFSTRIP);
    prefetch(print_sq);
    print_sq += 2 * SQ_WORDS;
    SQ_PUT(Uint32, right | bottom);
    SQ_PUT(Uint32, right | top);
    SQ_PUT(Uint32, left | top);
    /* the block's word 4, which the sprite reserves, stays unwritten */
    print_sq--;
    SQ_PUT(Float, y + print_size);
    SQ_PUT(Float, x);
    SQ_PUT(Float, PRINT_DEPTH);
    SQ_PUT(Float, y + print_size);
    prefetch(print_sq);
    print_sq += SQ_WORDS;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the words of the sprites' parameter that a print sends: the four that open
 * it, then print_color */
#define PARAM_WORDS 5
/* the global index _nj_current_texture holds after a print, as ninja.h says */
#define FONT_TEXTURE (-2)
/* open a print. map the store queues to the translucent list's next
 * address, and send the sprites' parameter there */
#define PRINT_OPEN()                                                           \
    _nj_current_texture = FONT_TEXTURE;                                        \
    list_next = (Uint32)_nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON];      \
    list_top = list_next & ~SQ_OFFSET;                                         \
    sq_area = (list_next >> QACR_AREA_SHIFT) & QACR_AREA;                      \
    QACR0 = sq_area;                                                           \
    QACR1 = sq_area;                                                           \
    print_sq = SQ_ADDRESS(list_next);                                          \
    print_sq += PARAM_WORDS;                                                   \
    SQ_PUT(Uint32, print_color);                                               \
    SQ_PUT(Uint32, tex_param);                                                 \
    SQ_PUT(Uint32, tsp_param);                                                 \
    SQ_PUT(Uint32, isp_param);                                                 \
    SQ_PUT(Uint32, global_param);                                              \
    prefetch(print_sq);                                                        \
    print_sq += SQ_WORDS;

/* close the print: move the translucent list's next address past its last
 * sprite */
#define PRINT_CLOSE()                                                          \
    (_nj_vertex_buffer_.pCurrentPtr[KM_TRANS_POLYGON] =                        \
         (Uint32 *)(((Uint32)print_sq & SQ_OFFSET) | list_top))

void njPrintC(Int loc, const char *s)
{
    Sint16 x;
    Sint16 y;
    Sint16 length;
    Sint16 i;

    PRINT_OPEN();
    x = NJM_LOCATION_X(loc) * print_size;
    y = NJM_LOCATION_Y(loc) * print_size;
    length = strlen(s);
    for (i = 0; i < length; i++) {
        njCharacter(x, y, s[i]);
        x += print_size;
    }
    PRINT_CLOSE();
}

#ifdef NONMATCHING
/* the port's readout prints through this, so that the game's own prints keep
 * the size and color it set. engine/dc/port.h declares it */
void readout_print(Int loc, const char *s, Uint16 size, Uint32 c)
{
    Float kept_size = print_size;
    Uint32 kept_color = print_color;

    print_size = size;
    print_color = c;
    njPrintC(loc, s);
    print_size = kept_size;
    print_color = kept_color;
}
#endif

void njPrintD(Int loc, Int val, Int digit)
{
    Int d;
    Sint16 x;
    Sint16 y;

    PRINT_OPEN();
    x = NJM_LOCATION_X(loc) * print_size;
    y = NJM_LOCATION_Y(loc) * print_size;
    if (val < 0) {
        njCharacter(x, y, '-');
        val = -val;
        x += print_size;
    }
    for (; digit > 0; digit--) {
        switch (digit) {
        case 1:
            d = val % 10;
            break;
        case 2:
            d = val / 10;
            val -= d * 10;
            break;
        case 3:
            d = val / 100;
            val -= d * 100;
            break;
        case 4:
            d = val / 1000;
            val -= d * 1000;
            break;
        case 5:
            d = val / 10000;
            val -= d * 10000;
            break;
        case 6:
            d = val / 100000;
            val -= d * 100000;
            break;
        case 7:
            d = val / 1000000;
            val -= d * 1000000;
            break;
        case 8:
            d = val / 10000000;
            val -= d * 10000000;
            break;
        case 9:
            d = val / 100000000;
            val -= d * 100000000;
            break;
        default:
            d = val / 1000000000;
            val -= d * 1000000000;
            break;
        }
        if (d > 9) {
            d = 0;
        }
        njCharacter(x, y, '0' + d);
        x += print_size;
    }
    PRINT_CLOSE();
}

void njPrintB(Int loc, Uint32 val, Int digit)
{
    Sint16 x;
    Sint16 y;
    Int i;

    PRINT_OPEN();
    x = NJM_LOCATION_X(loc) * print_size;
    y = NJM_LOCATION_Y(loc) * print_size;
    for (i = digit - 1; i >= 0; i--) {
        njCharacter(x, y, '0' + ((val >> i) & 1));
        x += print_size;
    }
    PRINT_CLOSE();
}

void njPrintH(Int loc, Uint32 val, Int digit)
{
    Sint16 x;
    Sint16 y;
    Int d;
    Int i;

    PRINT_OPEN();
    x = NJM_LOCATION_X(loc) * print_size;
    y = NJM_LOCATION_Y(loc) * print_size;
    for (i = digit - 1; i >= 0; i--) {
        /* @hack type: the shift's count unsigned, where i * HEX_DIGIT_BITS
         * would do */
        d = (val >> (i * (Uint32)HEX_DIGIT_BITS)) & HEX_DIGIT_MASK;
        njCharacter(x, y, hex_digits[d]);
        x += print_size;
    }
    PRINT_CLOSE();
}

/* the digits njPrintF writes after the point */
#define DECIMAL_PLACES 5

void njPrintF(Int loc, Float val, Int digit)
{
    char buf[20];
    Sint16 x;
    Sint16 y;
    Int length;
    Int point;
    Int pad;
    Int padded;
    Int i;

    PRINT_OPEN();
    x = NJM_LOCATION_X(loc) * print_size;
    y = NJM_LOCATION_Y(loc) * print_size;
    if (val < 0.0F) {
        njCharacter(x, y, '-');
        /* @hack spelling: val multiplied by -1, where val = -val would do */
        val *= -1.0F;
        x += print_size;
    }
    sprintf(buf, "%0.*f", DECIMAL_PLACES, val);
    length = strlen(buf);
    for (point = 0; point < length; point++) {
        if (buf[point] == '.') {
            break;
        }
    }
    /* pad is how many '0's go before the text, so that digit digits come
     * before the point. the text and its NUL move right by pad places */
    pad = digit - point;
    padded = length + pad;
    for (i = padded; i >= pad; i--) {
        buf[i] = buf[length--];
    }
    for (i = 0; i < pad; i++) {
        buf[i] = '0';
    }
    for (i = 0; i < padded; i++) {
        njCharacter(x, y, buf[i]);
        x += print_size;
    }
    PRINT_CLOSE();
}

void njPrint(Int loc, const char *fmt, ...)
{
    char buf[128];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    njPrintC(loc, buf);
}
