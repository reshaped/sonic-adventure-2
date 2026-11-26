/* text sprites, their fonts, the message window and timed texts. a timed text
 * is a task that shows a sprite for a time */
#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "draw/glyph_widths.h"
#include "draw/text_sprite.h"
#include "draw/text_sprite/text.inc"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"
#include "system/util.h"

/* a glyph is GLYPH_SIZE rows of GLYPH_ROW_BYTES bytes, a bit for each
 * pixel. a row's leftmost pixel is bit 0 of its first byte */
#define GLYPH_ROW_BYTES 3
#define GLYPH_BYTES (GLYPH_SIZE * GLYPH_ROW_BYTES)
/* the width glyph_trim gives a glyph with no pixel set */
#define BLANK_WIDTH 18
#define FIRST_CHARACTER 32 /* the first character with a glyph, the space */

/* the kanji file's glyphs follow the Shift JIS codes from 0x8140 to 0x9872.
 * each lead byte has KANJI_ROW trail bytes, 0x40 to 0x7e and then 0x80 to
 * 0xfc. KANJI_PLACE is a code's place in that order */
#define KANJI_FIRST_LEAD 0x81
#define KANJI_FIRST_TRAIL 0x40
#define KANJI_LOW_TRAIL_LAST 0x7e
#define KANJI_ROW 188
#define KANJI_PLACE(lead, trail)                                               \
    ((((lead) - KANJI_FIRST_LEAD) * KANJI_ROW) + (trail) - KANJI_FIRST_TRAIL - \
     ((trail) > KANJI_LOW_TRAIL_LAST ? 1 : 0))
/* the file leaves gaps out. from 0x829f, the first hiragana, a glyph lies
 * HIRAGANA_GAP places before its code's place. from 0x889f, the first kanji of
 * the first level, it lies LEVEL_1_GAP before */
#define HIRAGANA KANJI_PLACE(0x82, 0x9f)
#define HIRAGANA_GAP 4
#define LEVEL_1 KANJI_PLACE(0x88, 0x9f)
#define LEVEL_1_GAP 952
#define KANJI_GLYPHS (KANJI_PLACE(0x98, 0x72) + 1 - LEVEL_1_GAP)

/* an attr is what a text's codes set for the characters after them. its low 16
 * bits are a pixel, and ATTR_UNDERLINE underlines. ATTR_OWN, which no code's
 * color equals, draws in the sprite's own color. ATTR_OWN's low 16 bits are
 * PIXEL_WHITE, so ATTR_OWN with ATTR_UNDERLINE added is no longer ATTR_OWN, and
 * draws white */
#define ATTR_UNDERLINE 0x10000
#define ATTR_OWN 0x8000ffff
/* the alpha bit of an ARGB 1555 pixel */
#define PIXEL_OPAQUE 0x8000
#define PIXEL_WHITE 0xffff
/* a color's channel runs to CHANNEL_MAX in CHANNEL_BITS bits, and a pixel's to
 * PIXEL_CHANNEL_MAX in PIXEL_CHANNEL_BITS. CHANNELS counts red, green and
 * blue */
#define CHANNEL_MAX 255
#define CHANNEL_BITS 8
#define PIXEL_CHANNEL_MAX 31
#define PIXEL_CHANNEL_BITS 5
#define CHANNELS 3

/* the glyph_color for color, 0xRRGGBB, with its alpha bit set. a color of
 * -1 gives 0, which text_sprite_start makes white */
#define OWN_PIXEL(color)                                                       \
    ((color) != -1 ? color_to_pixel(color) | PIXEL_OPAQUE : 0)

#define TEXTURE_ATTR (NJD_TEXATTR_TYPE_MEMORY | NJD_TEXATTR_GLOBALINDEX)
/* a texture's least width and height */
#define TEXTURE_MIN 8
/* the bytes a sprite's pixels align to */
#define PIXELS_ALIGN 32
/* the shadow_offset text_sprite_start gives */
#define SHADOW_OFFSET 2.5F

/* text_sprite_draw's priorities, the band farthest */
#define BAND_PRIORITY (-1.3F)
#define SHADOW_PRIORITY (-1.2F)
#define TEXT_PRIORITY (-1.1F)

/* the bytes of text_sprite_printf's text and of number_read's digits, each
 * with its NUL */
#define PRINTF_SIZE 128
#define DIGITS_SIZE 128
/* the hexadecimal digits of \bpN and of \bRRGGBB */
#define PALETTE_DIGITS 1
#define COLOR_DIGITS 6

/* the task level of the message window and of a timed text */
#define TEXT_LEVEL 1

/* the alpha a message window gains with each step of its fade in, and
 * loses with each step of its fade out */
#define FADE_IN_STEP 40
#define FADE_OUT_STEP 24
#define ALPHA_MAX 255
/* a window's alpha is the submode of its task's work, read as a Uint8.
 * WINDOW_OPAQUE is ALPHA_MAX as the Sint8 submode holds it */
#define WINDOW_ALPHA(work) ((Uint8)(work)->submode)
#define WINDOW_OPAQUE (-1)
/* a page's count where the window's time is negative. the page shows that many
 * steps at full alpha before a press of A can turn it */
#define PRESS_COUNT 15
/* a MESSAGE_WINDOW_UNFOLD window's height while it widens, as a share of
 * its full height */
#define FOLDED 0.1F

/* a fading timed text's tick, the unit of alpha its fades count in */
#define TIMED_FADE_STEP 8

/* the modes of a timed text's work. a text that doesn't fade stays in
 * TIMED_TEXT_FADING_IN for its whole life */
enum timed_text_mode {
    TIMED_TEXT_FADING_IN,
    TIMED_TEXT_SHOWN,
    TIMED_TEXT_FADING_OUT,
    TIMED_TEXT_DONE
};

/* the font files, as font_files numbers them */
enum font_file { FILE_KANJI, FILE_LATIN, FILE_ASCII, FONT_FILES };

/* a font. a byte from lead_min to lead_max leads a code of two bytes, whose
 * glyph is the kanji file's. any other byte is a character of the font's
 * own file */
struct font {
    /* where it isn't null, the character that stands for each character
     * from FIRST_CHARACTER. no font has one */
    Uint8 *remap;
    /* each character's width from FIRST_CHARACTER, a pixel less than
     * font_width gives */
    const Sint8 *widths;
    /* the font's own file, once text_fonts_load has read it. glyph_file is its
     * place in font_files, or negative for none */
    Uint8 *glyphs;
    Sint32 glyph_file;
    Sint8 *kanji_widths; /* each kanji's width, as widths holds them */
    /* the same for the kanji file */
    Uint8 *kanji;
    Sint32 kanji_file;
    Uint8 lead_min;
    Uint8 lead_max;
};

/* a font file: its name on the disc, and its bytes once text_fonts_load has
 * read them */
struct font_file_entry {
    const char *name;
    void *data;
};

/* the record of a message window's task, and the sprite and the owner of a
 * timed text's task */
#define WINDOW(task) ((struct message_window *)(task)->motion.p)
#define SPRITE(task) ((struct text_sprite *)(task)->force.p)
#define OWNER(task) ((struct task **)(task)->motion.p)

static int font_width(int code, int language);
static void text_sprite_put(struct text_sprite *sprite, int code, int language,
                            Uint32 attr);
static void print_line(struct text_sprite *sprite, const char *text,
                       int language);
static void print_centered(struct text_sprite *sprite, const char *text,
                           int language);
static void print_block(struct text_sprite *sprite, const char *text,
                        int language);
static struct task *message_window_create_unowned(int kind, const char *text,
                                                  int time, int language,
                                                  int color, Sint16 pitch_x,
                                                  Sint16 pitch_y);
static struct task *
message_window_create_at_voice(int kind, const char *text, int time,
                               int language, struct task **owner, int color,
                               Sint16 pitch_x, Sint16 pitch_y, int voice_base);
static void message_window_destroy(struct task *task);
static void message_window_step(struct task *task);
static void message_window_draw(struct task *task);
static void timed_text_destroy(struct task *task);
static void timed_text_draw(struct task *task);
static void timed_text_step(struct task *task);

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* how many sprites have a texture loaded */
int text_sprites_loaded(void);
/* clear sprite's pixels, which must not be null, and move its cursor to the
 * texture's top left corner */
void text_sprite_clear(struct text_sprite *sprite);
/* text_sprite_print what string_vformat makes of format and its arguments,
 * which must come to fewer than PRINTF_SIZE characters */
void text_sprite_printf(struct text_sprite *sprite, int language,
                        const char *format, ...);
/* make sprite show text in language at x and y, laid out in width by
 * height. its texture takes the global index texture. it takes the default
 * pitches and white text. the text is drawn in WORK_AREA. it returns sprite,
 * with its pixels null */
struct text_sprite *text_sprite_show(struct text_sprite *sprite,
                                     const char *text, int x, int y, int width,
                                     int height, Sint32 texture, int language);
/* create a task that draws a sprite of text in language at *pos. the task
 * destroys itself after time steps. a time of 0 shows the text until something
 * else destroys the task. where owner isn't null, *owner gets the task. the
 * task sets *owner back to null as it ends, if *owner still holds it. each call
 * returns the task, or null where task_create or text_sprite_create fails.
 *
 * timed_text_create_fading's text fades in from an alpha of 1, shows for time
 * steps, and fades out. each step of the fade in adds 2 * TIMED_FADE_STEP to
 * the text's alpha and the shadow's. neither passes ALPHA_MAX. each step of the
 * fade out takes TIMED_FADE_STEP from every alpha. the task destroys itself
 * once every alpha is 0 */
struct task *timed_text_create(const char *text, int language, NJS_POINT2 *pos,
                               Sint16 time, struct task **owner);
struct task *timed_text_create_fading(const char *text, int language,
                                      NJS_POINT2 *pos, Sint16 time,
                                      struct task **owner);
/* NOLINTEND(misc-use-internal-linkage) */

/* no other object calls these four. none is static, since SHC leaves out a
 * static function whose every call it expands in place */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void text_sprite_new_line(struct text_sprite *sprite);
void text_sprite_start_plain(struct text_sprite *sprite, int x, int y,
                             int width, int height, Sint32 texture,
                             Uint16 *pixels);
void text_sprite_print_load(struct text_sprite *sprite, const char *text,
                            int language);
struct task *message_window_create(int kind, const char *text, int time,
                                   int language, struct task **owner, int color,
                                   Sint16 pitch_x, Sint16 pitch_y);
/* NOLINTEND(misc-use-internal-linkage) */

/* @hack spelling: five functions SHC writes into their callers under
 * #pragma inline, where calls to them would do */
#pragma inline(text_sprite_new_line, text_sprite_start_plain,                  \
               text_sprite_print_load, message_window_create,                  \
               text_sprite_create)

/* while it isn't 0, text_sprite_draw draws a sprite whose hidden is 1.
 * nothing writes it */
static Sint8 show_hidden;
/* each kanji's width, as text_fonts_load measures it */
static Sint8 kanji_widths[KANJI_GLYPHS];

/* initializers place sprites_loaded and fonts_loaded in the object's data,
 * not its bss, as they do message_window_task and textures_taken */
static Sint32 sprites_loaded = 0;
/* TRUE from text_fonts_load to text_fonts_free */
static Bool fonts_loaded = FALSE;

static struct font_file_entry font_files[FONT_FILES] = {
#include "draw/text_sprite/font_files.inc"
};

/* each language's font, in enum text_language's order. only the Japanese
 * font has bytes that lead a kanji's code */
static struct font fonts[TEXT_LANGUAGES] = {
#include "draw/text_sprite/fonts.inc"
};

/* the pixels of \bp0 to \bpf: white, red, green, blue, yellow, magenta and
 * cyan, then white for the rest */
static Uint16 palette[16] = {
    PIXEL_WHITE, 0xfc00,      0x83e0,      0x801f,
    0xffe0,      0xfc1f,      0x83ff,      PIXEL_WHITE,
    PIXEL_WHITE, PIXEL_WHITE, PIXEL_WHITE, PIXEL_WHITE,
    PIXEL_WHITE, PIXEL_WHITE, PIXEL_WHITE, PIXEL_WHITE,
};

/* the texture coordinates of a sprite's whole texture, for
 * text_sprite_draw's shadow. each draw of the text makes a copy of its own */
static NJS_TEX shadow_corners[4] = {
#include "draw/text_sprite/shadow_corners.inc"
};

/* the pitch_x a sprite takes for a negative one */
static Sint32 default_pitch_x = TEXT_PITCH_X;

/* the colors each page of a message window starts with: its text's, its
 * band's and its shadow's */
static Uint32 window_color = 0xffffffff;
static Uint32 window_band_color = 0x60003090;
static Uint32 window_shadow_color = 0xff000000;
/* nothing refers to it */
static Uint32 dat_8c0f63f4 = 0xffff;

/* the open message window's task, or null */
static struct task *message_window_task = NULL;

/* a bit for each of the TEXT_TEXTURES global indexes from
 * TEXT_TEXTURE_FIRST, set while a sprite has it */
static Uint32 textures_taken = 0;

/* the value of a hexadecimal digit, or -1 for any other character */
static int hex_digit(char digit)
{
    if (digit >= '0' && digit <= '9') {
        return digit - '0';
    }
    if (digit >= 'A' && digit <= 'F') {
        return digit - 'A' + 10;
    }
    if (digit >= 'a' && digit <= 'f') {
        return digit - 'a' + 10;
    }
    return -1;
}

/* read up to count hexadecimal digits at text into *value, which gets 0
 * where there are none. it returns where the digits end */
static char *hex_read(char *text, int count, Uint32 *value)
{
    Uint32 number = 0;
    int digit;

    while (count-- != 0 && (digit = hex_digit(*text)) >= 0) {
        number = (number << 4) + (digit & 15);
        text++;
    }
    *value = number;
    return text;
}

/* a channel of 0 to 255, as the five bits a pixel gives it */
static int channel_to_bits(int channel)
{
    Float scaled = channel;
    int bits;

    scaled *= (PIXEL_CHANNEL_MAX + 1.0F) / CHANNEL_MAX;
    /* @hack type: scaled converted as unsigned, where int would do */
    bits = (unsigned int)scaled;

    if (bits < 0) {
        bits = 0;
    }
    if (bits > PIXEL_CHANNEL_MAX) {
        bits = PIXEL_CHANNEL_MAX;
    }
    return bits;
}

/* the ARGB 1555 pixel of a 0xRRGGBB color, without its alpha bit */
static Uint16 color_to_pixel(int color)
{
    int i;
    int shift;
    int pixel;

    /* @hack spelling: a while whose counters are set and stepped in
     * statements of their own, where a for would do */
    pixel = 0;
    i = 0;
    shift = 0;
    while (i < CHANNELS) {
        pixel |= channel_to_bits((Uint8)color) << shift;
        color >>= CHANNEL_BITS;
        shift += PIXEL_CHANNEL_BITS;
        i++;
    }
    return pixel;
}

/* apply the code at text to *attr. it returns the code's length, or 0 where
 * text doesn't start with '\b' */
static int text_read_code(Uint8 *text, Uint32 *attr)
{
    int code = *text;
    /* @hack redundant: value's first 0xffffff, which hex_read always
     * overwrites */
    Uint32 value = 0xffffff;
    Uint8 *end = text;
    Uint16 pixel;

    if (code == '\b') {
        end++;
        code = *end;
        switch (code) {
        case 'r':
        case 'R':
            end++;
            *attr = ATTR_OWN;
            break;
        case 'u':
        case 'U':
            end++;
            *attr |= ATTR_UNDERLINE;
            break;
        case 'I':
        case 'i':
            end++;
            *attr &= ~ATTR_UNDERLINE;
            break;
        case 'p':
        case 'P':
            end++;
            end = (Uint8 *)hex_read((char *)end, PALETTE_DIGITS, &value);
            *attr = (*attr & ATTR_UNDERLINE) | palette[value];
            break;
        default:
            end = (Uint8 *)hex_read((char *)end, COLOR_DIGITS, &value);
            pixel = color_to_pixel(value);
            *attr = (*attr & ATTR_UNDERLINE) | pixel | PIXEL_OPAQUE;
            break;
        }
    }
    return end - text;
}

/* 1 where pixel x of the glyph's row at row is set, else 0 */
static int glyph_pixel(const Uint8 *row, int x)
{
    int byte = x / 8;
    int bit = x % 8;

    return (row[byte] >> bit) & 1;
}

/* move glyph's pixels left past its blank columns. it returns the glyph's width
 * less its blank columns, or BLANK_WIDTH where no pixel is set */
static int glyph_trim(Uint8 *glyph)
{
    int left = GLYPH_SIZE + 1;
    int right = GLYPH_SIZE + 1;
    Uint8 *row;
    Uint8 *scan;
    Uint8 *dst;
    Uint8 bits;
    Uint8 rbits;
    int count;
    int i;
    int y;
    int k;
    int x;
    int bit;
    int byte;
    int width;

    /* @hack spelling: each step of this function written out, as row = row
     * + GLYPH_ROW_BYTES, bits = bits >> 1, rbits = rbits << 1, dst = dst + 1
     * and x = x + 8, where +=, >>=, <<= and ++ would do. and in both scans
     * the if that counts a blank byte ahead of an else that breaks, where a
     * test of the byte that breaks would do */
    for (row = glyph; row < glyph + GLYPH_BYTES; row = row + GLYPH_ROW_BYTES) {
        for (scan = row, count = 0, i = 0; i < GLYPH_ROW_BYTES; i++) {
            bits = *scan;
            if (!bits) {
                count += 8;
                scan++;
            } else {
                break;
            }
        }
        if (bits) {
            while (!(bits & 1)) {
                count++;
                bits = bits >> 1;
            }
        }
        if (count < left) {
            left = count;
        }
    }
    for (row = glyph + (GLYPH_ROW_BYTES - 1);
         row < glyph + GLYPH_BYTES + (GLYPH_ROW_BYTES - 1);
         row = row + GLYPH_ROW_BYTES) {
        for (scan = row, count = 0, i = 0; i < GLYPH_ROW_BYTES; i++) {
            rbits = *scan;
            if (!rbits) {
                count += 8;
                scan--;
            } else {
                break;
            }
        }
        if (rbits) {
            while (!(rbits & 0x80)) {
                count++;
                rbits = rbits << 1;
            }
        }
        if (count < right) {
            right = count;
        }
    }
    for (y = 0; y < GLYPH_SIZE; y++) {
        for (k = 0, dst = glyph + (y * GLYPH_ROW_BYTES), x = left;
             k < GLYPH_ROW_BYTES; k++, dst = dst + 1, x = x + 8) {
            byte = 0;
            for (bit = 0, i = x; bit < 8; bit++, i++) {
                byte |= i < GLYPH_SIZE
                            ? glyph_pixel(glyph + (y * GLYPH_ROW_BYTES), i)
                                  << bit
                            : 0;
            }
            *dst = byte;
        }
    }
    width = GLYPH_SIZE - left - right;
    if (width <= 0) {
        width = BLANK_WIDTH;
    }
    return width;
}

/* load sprite's pixels as its texture, or into the texture it has loaded
 * already */
static void text_sprite_load(struct text_sprite *sprite)
{
    NJS_TEXINFO info;

    if (sprite->texlist.textures) {
        njReLoadTextureNumG(sprite->texture, sprite->pixels, TEXTURE_ATTR, 0);
    } else {
        njSetTextureInfo(&info, sprite->pixels,
                         NJD_TEXFMT_RECTANGLE | NJD_TEXFMT_ARGB_1555,
                         sprite->texture_width, sprite->texture_height);
        njSetTextureName(&sprite->texname, &info, sprite->texture,
                         TEXTURE_ATTR);
        sprite->texlist.textures = &sprite->texname;
        sprite->texlist.nbTexture = 1;
        njLoadTexture(&sprite->texlist);
        sprites_loaded++;
    }
}

/* release sprite's texture, where it has one loaded, and set its pixels
 * to null */
static void text_sprite_release(struct text_sprite *sprite)
{
    if (sprite->texlist.textures) {
        njReleaseTexture(&sprite->texlist);
        sprite->texlist.textures = NULL;
        sprites_loaded--;
    }
    sprite->pixels = NULL;
}

int text_sprites_loaded(void)
{
    return sprites_loaded;
}

void text_sprite_draw(struct text_sprite *sprite)
{
    Float left;
    Float top;
    Float right;
    Float bottom;

    if (sprite->hidden == 1 && !show_hidden) {
        return;
    }
    left = sprite->x;
    top = sprite->y;
    right = left + (sprite->texture_width * sprite->scale_x);
    bottom = top + (sprite->texture_height * sprite->scale_y);
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    if (sprite->band_color.argb.a) {
        NJS_COLOR colors[4];
        NJS_POINT2 points[4];
        NJS_POINT2COL polygon;

        /* @hack redundant: the 0.0F added to band_x, where its conversion
         * alone would do */
        points[0].x = points[1].x = sprite->band_x + 0.0F;
        points[2].x = points[3].x =
            sprite->band_x + (TEXT_BAND_WIDTH * sprite->scale_x);
        points[0].y = points[3].y =
            sprite->band_y - (TEXT_BAND_MARGIN * sprite->scale_y);
        points[1].y = points[2].y =
            sprite->band_y +
            ((sprite->band_height + TEXT_BAND_MARGIN) * sprite->scale_y);
        colors[0].color = colors[1].color = colors[2].color = colors[3].color =
            sprite->band_color.color;
        polygon.p = points;
        polygon.col = colors;
        polygon.tex = NULL;
        polygon.num = 4;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njDrawPolygon2D(&polygon, 4, BAND_PRIORITY, NJD_FILL | NJD_TRANSPARENT);
    }
    if (sprite->shadow_color.argb.a && sprite->texlist.textures) {
        NJS_COLOR colors[4];
        NJS_POINT2 points[4];
        NJS_POINT2COL polygon;
        Float dx = sprite->shadow_offset * sprite->scale_x;
        Float dy = sprite->shadow_offset * sprite->scale_y;

        points[0].x = points[1].x = left + dx;
        points[2].x = points[3].x = right + dx;
        points[0].y = points[3].y = top + dy;
        points[1].y = points[2].y = bottom + dy;
        colors[0].color = colors[1].color = colors[2].color = colors[3].color =
            sprite->shadow_color.color;
        polygon.p = points;
        polygon.col = colors;
        polygon.tex = (NJS_COLOR *)shadow_corners;
        polygon.num = 4;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        njDrawPolygon2D(&polygon, 4, SHADOW_PRIORITY,
                        NJD_USE_TEXTURE | NJD_TRANSPARENT | NJD_FILL);
    }
    if (sprite->color.argb.a && sprite->texlist.textures) {
        NJS_COLOR colors[4];
        NJS_TEX sprite_corners[4] = {
#include "draw/text_sprite/sprite_corners.inc"
        };
        NJS_POINT2 points[4];
        NJS_POINT2COL polygon;

        points[0].x = points[1].x = left;
        points[2].x = points[3].x = right;
        points[0].y = points[3].y = top;
        points[1].y = points[2].y = bottom;
        colors[0].color = colors[1].color = colors[2].color = colors[3].color =
            sprite->color.color;
        polygon.p = points;
        polygon.col = colors;
        polygon.tex = (NJS_COLOR *)sprite_corners;
        polygon.num = 4;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        njDrawPolygon2D(&polygon, 4, TEXT_PRIORITY,
                        NJD_USE_TEXTURE | NJD_TRANSPARENT | NJD_FILL);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    draw_control_restore();
}

void text_sprite_draw_text(struct text_sprite *sprite, Float x, Float y,
                           Float scale_x, Float scale_y, const Uint32 *color)
{
    /* @hack redundant: left and top, copies of x and y, where x and y would
     * do */
    Float left = x;
    Float top = y;
    Float right = left + (sprite->texture_width * scale_x);
    Float bottom = top + (sprite->texture_height * scale_y);

    if (sprite->texlist.textures) {
        NJS_COLOR colors[4];
        NJS_TEX text_corners[4] = {
#include "draw/text_sprite/text_corners.inc"
        };
        NJS_POINT2 points[4];
        NJS_POINT2COL polygon;

        points[0].x = points[1].x = left;
        points[2].x = points[3].x = right;
        points[0].y = points[3].y = top;
        points[1].y = points[2].y = bottom;
        colors[0].color = colors[1].color = colors[2].color = colors[3].color =
            color ? *color : sprite->color.color;
        polygon.p = points;
        polygon.col = colors;
        polygon.tex = (NJS_COLOR *)text_corners;
        polygon.num = 4;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        njDrawPolygon2D(&polygon, 4, TEXT_PRIORITY,
                        NJD_USE_TEXTURE | NJD_TRANSPARENT | NJD_FILL);
    }
}

void text_sprite_clear(struct text_sprite *sprite)
{
    Uint32 *word = (Uint32 *)sprite->pixels;
    int left = sprite->texture_width * sprite->texture_height / 2;

    for (; left != 0; left--) {
        *word++ = 0;
    }
    sprite->cursor_x = sprite->cursor_y = 0;
}

/* the glyph of code in language's font. a font without the kanji file gives
 * a code of two bytes its first glyph. a code past the font's last glyph is
 * unchecked */
static Uint8 *font_glyph(int code, int language)
{
    int lead_min = fonts[language].lead_min;
    int lead_max = fonts[language].lead_max;
    int row;

    if (code < 0x100) {
        if (code > lead_max) {
            code -= lead_max - lead_min;
        }
        code -= FIRST_CHARACTER;
        if (code < 0) {
            code = 0;
        }
        if (fonts[language].remap) {
            code = fonts[language].remap[code] - FIRST_CHARACTER;
        }
        return fonts[language].glyphs + (code * GLYPH_BYTES);
        /* @hack redundant: the else after the return */
    } else if (fonts[language].kanji) {
        /* @hack spelling: KANJI_FIRST_LEAD taken off the row in a statement
         * of its own, as in font_width */
        row = (Uint8)(code >> 8);
        code = (Uint8)code;
        row -= KANJI_FIRST_LEAD;
        if (code <= KANJI_LOW_TRAIL_LAST) {
            code -= KANJI_FIRST_TRAIL;
        } else {
            code -= KANJI_FIRST_TRAIL + 1;
        }
        code += row * KANJI_ROW;
        if (code >= LEVEL_1) {
            code -= LEVEL_1_GAP;
        } else if (code >= HIRAGANA) {
            code -= HIRAGANA_GAP;
        }
        return fonts[language].kanji + (code * GLYPH_BYTES);
    }
    return fonts[language].glyphs;
}

void text_fonts_load(void)
{
    int i;
    int j;
    Uint8 *glyph;

    if (fonts_loaded) {
        return;
    }
    /* @hack type: i compared as unsigned, here and below, where int would
     * do */
    for (i = 0; (Uint32)i < FONT_FILES; i++) {
        if (!font_files[i].data) {
            font_files[i].data = loader_read_alloc(font_files[i].name);
        }
    }
    if (font_files[FILE_KANJI].data) {
        glyph = font_files[FILE_KANJI].data;
        for (j = 0; j < KANJI_GLYPHS; j++, glyph += GLYPH_BYTES) {
            kanji_widths[j] = glyph_trim(glyph);
        }
    }
    for (i = 0; (Uint32)i < TEXT_LANGUAGES; i++) {
        if (fonts[i].glyph_file >= 0) {
            fonts[i].glyphs = font_files[fonts[i].glyph_file].data;
        }
        if (fonts[i].kanji_file >= 0) {
            fonts[i].kanji = font_files[fonts[i].kanji_file].data;
        }
    }
    fonts_loaded = TRUE;
}

void text_fonts_free(void)
{
    int i;

    /* @hack type: i compared as unsigned, where int would do */
    for (i = 0; (Uint32)i < FONT_FILES; i++) {
        if (font_files[i].data) {
            syFree(font_files[i].data);
        }
        font_files[i].data = NULL;
    }
    fonts_loaded = FALSE;
}

/* the pixels code takes of a line in language's font, before pitch_x: its
 * width and one more. it is GLYPH_SIZE while the fonts aren't loaded. so is a
 * code of two bytes in a font without kanji widths */
static int font_width(int code, int language)
{
    int lead_min = fonts[language].lead_min;
    int lead_max = fonts[language].lead_max;
    int row;

    if (!fonts_loaded) {
        return GLYPH_SIZE;
    }
    if (code < 0x100) {
        if (code > lead_max) {
            code -= lead_max - lead_min;
        }
        if (code < FIRST_CHARACTER) {
            code = FIRST_CHARACTER;
        }
        if (fonts[language].remap) {
            code = fonts[language].remap[code - FIRST_CHARACTER];
        }
        return fonts[language].widths[code - FIRST_CHARACTER] + 1;
    } else if (fonts[language].kanji_widths) {
        /* @hack spelling: KANJI_FIRST_LEAD taken off the row in a statement
         * of its own, after code is narrowed, where taking it off as the
         * row is set would do */
        row = (Uint8)(code >> 8);
        code = (Uint8)code;
        row -= KANJI_FIRST_LEAD;
        if (code <= KANJI_LOW_TRAIL_LAST) {
            code -= KANJI_FIRST_TRAIL;
        } else {
            code -= KANJI_FIRST_TRAIL + 1;
        }
        code += row * KANJI_ROW;
        if (code >= LEVEL_1) {
            code -= LEVEL_1_GAP;
        } else if (code >= HIRAGANA) {
            code -= HIRAGANA_GAP;
        }
        return fonts[language].kanji_widths[code] + 1;
    }
    return GLYPH_SIZE;
}

void text_sprite_new_line(struct text_sprite *sprite)
{
    sprite->cursor_x = 0;
    sprite->cursor_y += sprite->pitch_y + GLYPH_SIZE;
}

/* write color at dst where mask's bit of bits is set, and step dst to the
 * next pixel */
#define PLOT(mask)                                                             \
    if (bits & (mask)) {                                                       \
        *dst = color;                                                          \
    }                                                                          \
    dst++

/* draw code's glyph at sprite's cursor, and move the cursor past it. a
 * glyph that would pass the texture's right edge goes to the next line
 * first. '\n' only ends the line, and any other code below FIRST_CHARACTER
 * draws as a space. the glyph's pixels take attr's pixel, or glyph_color
 * for ATTR_OWN. ATTR_UNDERLINE sets the glyph's last row whole. rows outside
 * the texture don't draw */
static void text_sprite_put(struct text_sprite *sprite, int code, int language,
                            Uint32 attr)
{
    Sint8 *glyph;
    int width;
    int tw;
    Uint16 *pixels;
    /* @hack redundant: dst declared register, where a plain local would do */
    register Uint16 *dst;
    int y;
    int rows;
    /* @hack redundant: color declared register, where a plain local would do */
    register int color;
    int i;
    /* @hack redundant: bits declared register, where a plain local would do */
    register int bits;

    if (code < FIRST_CHARACTER) {
        if (code == '\n') {
            text_sprite_new_line(sprite);
            return;
        }
        code = FIRST_CHARACTER;
    }
    glyph = (Sint8 *)font_glyph(code, language);
    width = font_width(code, language);
    tw = sprite->texture_width;
    if (sprite->cursor_x > tw - width) {
        text_sprite_new_line(sprite);
    }
    if (sprite->cursor_y < sprite->texture_height &&
        sprite->cursor_y + (GLYPH_SIZE - 1) >= 0) {
        pixels = sprite->pixels;
        dst = pixels + sprite->cursor_x;
        dst += sprite->cursor_y * tw;
        y = sprite->cursor_y;
        rows = GLYPH_SIZE;
        color = attr != ATTR_OWN ? (Uint16)attr : sprite->glyph_color;
        if (y < 0) {
            rows += y;
            glyph += -y * GLYPH_ROW_BYTES;
            dst += -y * tw;
        }
        tw -= GLYPH_SIZE;
        if (attr & ATTR_UNDERLINE) {
            while (rows-- != 0) {
                if (dst < pixels) {
                    dst += GLYPH_SIZE;
                    glyph += GLYPH_ROW_BYTES;
                } else if (rows == 0) {
                    for (i = GLYPH_SIZE; i != 0; i--) {
                        *dst = color;
                        dst++;
                    }
                } else {
                    for (i = GLYPH_ROW_BYTES; i != 0; i--) {
                        bits = *glyph++;
                        PLOT(0x01);
                        PLOT(0x02);
                        PLOT(0x04);
                        PLOT(0x08);
                        PLOT(0x10);
                        PLOT(0x20);
                        PLOT(0x40);
                        PLOT(0x80);
                    }
                }
                dst += tw;
                if (++y >= sprite->texture_height) {
                    break;
                }
            }
        } else {
            while (rows-- != 0) {
                if (dst < pixels) {
                    dst += GLYPH_SIZE;
                    glyph += GLYPH_ROW_BYTES;
                } else {
                    for (i = GLYPH_ROW_BYTES; i != 0; i--) {
                        bits = *glyph++;
                        PLOT(0x01);
                        PLOT(0x02);
                        PLOT(0x04);
                        PLOT(0x08);
                        PLOT(0x10);
                        PLOT(0x20);
                        PLOT(0x40);
                        PLOT(0x80);
                    }
                }
                dst += tw;
                if (++y >= sprite->texture_height) {
                    break;
                }
            }
        }
    }
    sprite->cursor_x += width + sprite->pitch_x;
}
#undef PLOT

/* draw text into sprite's pixels from its cursor, in language's font */
static void text_sprite_print(struct text_sprite *sprite, const char *text,
                              int language)
{
    struct font *font;
    int lead_min;
    int lead_max;
    int code;
    Uint32 attr;

    if (*text == '\t') {
        print_centered(sprite, text + 1, language);
    } else if (*text == '\a') {
        print_block(sprite, text + 1, language);
    } else {
        font = &fonts[language];
        lead_min = font->lead_min;
        lead_max = font->lead_max;
        attr = ATTR_OWN;
        while (text += text_read_code((Uint8 *)text, &attr),
               (code = (Uint8)*text++) != 0) {
            if (code >= lead_min && code <= lead_max) {
                code = (code << 8) | (Uint8)*text++;
            }
            text_sprite_put(sprite, code, language, attr);
        }
    }
}

/* draw text's first line from sprite's cursor, with the '\n' that ends
 * it */
static void print_line(struct text_sprite *sprite, const char *text,
                       int language)
{
    struct font *font = &fonts[language];
    int lead_min = font->lead_min;
    int lead_max = font->lead_max;
    int code;
    Uint32 attr = ATTR_OWN;

    while (text += text_read_code((Uint8 *)text, &attr),
           (code = (Uint8)*text++) != 0) {
        if (code >= lead_min && code <= lead_max) {
            code = (code << 8) | (Uint8)*text++;
        }
        text_sprite_put(sprite, code, language, attr);
        if (code == '\n') {
            break;
        }
    }
}

/* draw each line of text centered in sprite's width */
static void print_centered(struct text_sprite *sprite, const char *text,
                           int language)
{
    struct font *font = &fonts[language];
    int lead_min = font->lead_min;
    int lead_max = font->lead_max;
    const char *scan;
    int width;
    int code;

    while (*text) {
        scan = text;
        width = 0;
        while (*scan) {
            code = (Uint8)*scan++;
            if (code == '\n') {
                break;
            }
            if (code >= lead_min && code <= lead_max) {
                code = (code << 8) | (Uint8)*scan++;
            }
            width += font_width(code, language) + sprite->pitch_x;
        }
        width -= sprite->pitch_x;
        if (width < 0) {
            width = 0;
        }
        sprite->cursor_x = (sprite->width - width) / 2;
        print_line(sprite, text, language);
        text = scan;
    }
}

/* draw the lines of text from one x, which centers the widest in sprite's
 * width */
static void print_block(struct text_sprite *sprite, const char *text,
                        int language)
{
    struct font *font = &fonts[language];
    int lead_min = font->lead_min;
    int lead_max = font->lead_max;
    const Uint8 *scan = (const Uint8 *)text;
    const char *line;
    int max = 0;
    int width;
    int x;
    int code;

    do {
        width = 0;
        /* @hack spelling: the read joined to the test by a comma, here and
         * below, where an assignment inside the test would do */
        while (code = *scan++, code != 0 && code != '\n') {
            if (code >= lead_min && code <= lead_max) {
                code = (code << 8) | *scan++;
            }
            width += font_width(code, language) + sprite->pitch_x;
        }
        width -= sprite->pitch_x;
        if (max < width) {
            max = width;
        }
    } while (code);
    x = (sprite->width - max) / 2;
    line = text;
    do {
        sprite->cursor_x = x;
        print_line(sprite, line, language);
        while (code = *line++, code != 0 && code != '\n') {
            if (code >= lead_min && code <= lead_max) {
                line++;
            }
        }
    } while (code);
}

void text_sprite_printf(struct text_sprite *sprite, int language,
                        const char *format, ...)
{
    char text[PRINTF_SIZE];
    va_list args;

    va_start(args, format);
    string_vformat(text, format, args);
    text_sprite_print(sprite, text, language);
}

void text_sprite_measure(struct text_sprite *sprite, const char *text,
                         int language, Uint16 *width, Uint16 *height)
{
    struct font *font = &fonts[language];
    int lead_min = font->lead_min;
    int lead_max = font->lead_max;
    int max = 0;
    int lines_height = 0;
    int line_width;
    int code;
    Uint32 attr;
    Uint8 *scan;

    if (*text == '\t' || *text == '\a') {
        text++;
    }
    attr = ATTR_OWN;
    scan = (Uint8 *)text;
    do {
        line_width = 0;
        while (scan += text_read_code(scan, &attr),
               (code = *scan++) != 0 && code != '\n') {
            if (code >= lead_min && code <= lead_max) {
                code = (code << 8) | *scan++;
            }
            line_width += font_width(code, language) + sprite->pitch_x;
        }
        line_width -= sprite->pitch_x;
        if (max < line_width) {
            max = line_width;
        }
        lines_height += sprite->pitch_y + GLYPH_SIZE;
    } while (code);
    *width = max;
    *height = lines_height;
}

/* set sprite up for a text of width by height at x and y on the screen. its
 * texture takes the global index texture. the text is drawn at pixels, rounded
 * up to a multiple of PIXELS_ALIGN bytes. the call clears a texture's worth of
 * pixels there. a sprite set up with pixels null serves only
 * text_sprite_measure.
 *
 * a negative pitch gives the default, and a glyph_color of 0 gives white.
 * color is white, shadow_color black and the scale 1. the band starts at 0 and
 * y, height tall, and band_color is 0. the call forgets a texture sprite has
 * loaded, without releasing it */
static void text_sprite_start(struct text_sprite *sprite, int x, int y,
                              int width, int height, Sint32 texture,
                              Uint16 *pixels, int pitch_x, int pitch_y,
                              Uint16 glyph_color)
{
    int texture_width;
    int texture_height;
    unsigned int size;
    unsigned int i;
    Uint32 *word;
    Uint16 *buf;

    for (texture_width = TEXTURE_MIN; texture_width < width;
         texture_width <<= 1) {
    }
    for (texture_height = TEXTURE_MIN; texture_height < height;
         texture_height <<= 1) {
    }
    size = texture_width * texture_height;
    buf = pixels;
    if ((Uint32)buf & (PIXELS_ALIGN - 1)) {
        buf = (Uint16 *)(((Uint32)buf + PIXELS_ALIGN) & ~(PIXELS_ALIGN - 1));
    }
    if (buf) {
        word = (Uint32 *)buf;
        for (i = size / 2; i != 0; i--) {
            *word++ = 0;
        }
    }
    sprite->hidden = 0;
    sprite->x = x;
    sprite->y = y;
    sprite->band_x = 0;
    sprite->band_y = y;
    sprite->width = width;
    sprite->height = height;
    sprite->band_height = height;
    sprite->texture_width = texture_width;
    sprite->texture_height = texture_height;
    sprite->cursor_x = 0;
    sprite->cursor_y = 0;
    sprite->pitch_x = pitch_x >= 0 ? pitch_x : default_pitch_x;
    sprite->pitch_y = pitch_y >= 0 ? pitch_y : TEXT_PITCH_Y;
    sprite->scale_x = 1.0F;
    sprite->scale_y = 1.0F;
    sprite->glyph_color = glyph_color ? glyph_color : PIXEL_WHITE;
    sprite->color.color = 0xffffffff;
    sprite->shadow_color.color = 0xff000000;
    sprite->band_color.color = 0;
    sprite->pixels = buf;
    sprite->texture = texture;
    sprite->shadow_offset = SHADOW_OFFSET;
    sprite->texlist.textures = NULL;
}

/* text_sprite_start with the default pitches and a white text */
void text_sprite_start_plain(struct text_sprite *sprite, int x, int y,
                             int width, int height, Sint32 texture,
                             Uint16 *pixels)
{
    text_sprite_start(sprite, x, y, width, height, texture, pixels, -1, -1, 0);
}

/* draw text into sprite's pixels, and give them to its texture */
void text_sprite_print_load(struct text_sprite *sprite, const char *text,
                            int language)
{
    text_sprite_print(sprite, text, language);
    text_sprite_load(sprite);
}

struct text_sprite *text_sprite_show(struct text_sprite *sprite,
                                     const char *text, int x, int y, int width,
                                     int height, Sint32 texture, int language)
{
    text_sprite_start_plain(sprite, x, y, width, height, texture,
                            (Uint16 *)WORK_AREA);
    text_sprite_print(sprite, text, language);
    text_sprite_load(sprite);
    sprite->pixels = NULL;
    return sprite;
}

struct task *message_window_current(void)
{
    return message_window_task;
}

struct message_window *message_window_record(struct task *task)
{
    /* @hack redundant: the test of the record, and the null it returns
     * for a null record, where returning the record would do */
    if (task->motion.p) {
        return task->motion.p;
    }
    return NULL;
}

struct task *message_window_open_unowned(int kind, const char *text, int time,
                                         int language)
{
    return message_window_create_unowned(kind, text, time, language, -1, -1,
                                         -1);
}

struct task *message_window_open(int kind, const char *text, int time,
                                 int language, struct task **owner)
{
    return message_window_create(kind, text, time, language, owner, -1, -1, -1);
}

struct task *message_window_open_at_voice(int kind, const char *text, int time,
                                          int language, struct task **owner,
                                          int voice_base)
{
    return message_window_create_at_voice(kind, text, time, language, owner, -1,
                                          -1, -1, voice_base);
}

/* read the decimal number at text into *value, and return where its digits
 * end. *value keeps its value where text starts with no digit. there must
 * be fewer than DIGITS_SIZE digits, unchecked */
static char *number_read(char *text, int *value)
{
    char digits[DIGITS_SIZE];
    char *end = digits;

    while (*text >= '0' && *text <= '9') {
        *end++ = *text++;
    }
    *end = 0;
    if (end != digits) {
        *value = atoi(digits);
    }
    return text;
}

/* read the codes that open page, a '\f' and the letters after it. a space or a
 * ':' ends them. a text's first page may lack the '\f', and then has no codes.
 * W<n> sets *count, the steps the page shows, to n. S<n> sets *voice, the voice
 * it plays, to n, and A<n> to n + voice_base. a letter without a number leaves
 * its value as it was, but A still adds voice_base. any other character is
 * skipped. *start gets the page's text after its codes. *next gets the next
 * page's '\f', or null */
static void page_read_codes(char *page, char **start, int *count, int *voice,
                            char **next, int voice_base)
{
    if (*page == '\f') {
        page++;
        while (*page) {
            if (*page == ' ' || *page == ':') {
                page++;
                break;
            }
            switch (*page) {
            case 'A':
            case 'a':
                page = number_read(page + 1, voice);
                *voice += voice_base;
                break;
            case 'S':
            case 's':
                page = number_read(page + 1, voice);
                break;
            case 'w':
            case 'W':
                page = number_read(page + 1, count);
                break;
            default:
                page++;
                break;
            }
        }
    }
    *start = page;
    page = strchr(page, '\f');
    *next = page;
}

/* message_window_open_unowned's work. color is the text's own as 0xRRGGBB,
 * or -1 for white. a negative pitch gives the default */
static struct task *message_window_create_unowned(int kind, const char *text,
                                                  int time, int language,
                                                  int color, Sint16 pitch_x,
                                                  Sint16 pitch_y)
{
    struct task *task;
    char *start;
    char *copy;
    int count;
    int voice;
    int length;
    Bool replaced;
    Uint16 width;
    Uint16 height;

    replaced = FALSE;
    if (message_window_task) {
        task_destroy(message_window_task);
        replaced = TRUE;
    }
    task = message_window_task = task_create(
        TASK_WORK, TEXT_LEVEL, message_window_step, "EfMsgWnd0Exec");
    if (task) {
        task->motion.p = syCalloc(1, sizeof(struct message_window));
        length = strlen(text);
        if (task->motion.p && (copy = syMalloc(length + 2)) != 0) {
            /* @hack spelling: _builtin_strcpy, where strcpy would do */
            _builtin_strcpy(copy, text);
            WINDOW(task)->text = copy;
            count = -1;
            voice = -1;
            page_read_codes(copy, &start, &count, &voice, &WINDOW(task)->next,
                            0);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = 0;
            }
            text_sprite_start(&WINDOW(task)->sprite, MESSAGE_WINDOW_X,
                              MESSAGE_WINDOW_Y, MESSAGE_WINDOW_WIDTH,
                              MESSAGE_WINDOW_HEIGHT, MESSAGE_WINDOW_TEXTURE,
                              NULL, pitch_x, pitch_y, OWN_PIXEL(color));
            text_sprite_measure(&WINDOW(task)->sprite, start, language, &width,
                                &height);
            if (width < MESSAGE_WINDOW_WIDTH) {
                width = MESSAGE_WINDOW_WIDTH;
            }
            text_sprite_start(&WINDOW(task)->sprite, MESSAGE_WINDOW_X,
                              MESSAGE_WINDOW_Y, width, height,
                              MESSAGE_WINDOW_TEXTURE, (Uint16 *)WORK_AREA,
                              pitch_x, pitch_y, OWN_PIXEL(color));
            text_sprite_print_load(&WINDOW(task)->sprite, start, language);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = '\f';
            }
            task->draw = message_window_draw;
            task->step = message_window_step;
            task->destructor = message_window_destroy;
            WINDOW(task)->unk_58 = 0;
            WINDOW(task)->color = color;
            WINDOW(task)->count = count >= 0 ? count : time;
            WINDOW(task)->language = language;
            WINDOW(task)->owner = NULL;
            WINDOW(task)->pitch_x = pitch_x;
            WINDOW(task)->pitch_y = pitch_y;
            WINDOW(task)->time = time;
            WINDOW(task)->voice = -1;
            if (WINDOW(task)->time < 0) {
                WINDOW(task)->count = PRESS_COUNT;
            }
            WINDOW(task)->kind = kind;
            WINDOW(task)->sprite.color.color = window_color;
            WINDOW(task)->sprite.band_color.color = window_band_color;
            WINDOW(task)->sprite.shadow_color.color = window_shadow_color;
            if (replaced) {
                task->work->submode = WINDOW_OPAQUE;
            }
            switch (kind) {
            case MESSAGE_WINDOW_LOW:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                    WINDOW(task)->sprite.y += WINDOW(task)->sprite.band_height -
                                              WINDOW(task)->sprite.height;
                }
                break;
            case MESSAGE_WINDOW_FITTED:
            case MESSAGE_WINDOW_UNFOLD:
                break;
            case MESSAGE_WINDOW_INSTANT:
                task->work->submode = WINDOW_OPAQUE;
                /* fall through */
            default:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                }
                break;
            }
            if (voice >= 0) {
                WINDOW(task)->voice = voice_play(voice, 0);
            }
        } else {
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            message_window_task = NULL;
        }
    }
    return message_window_task;
}

/* message_window_open's work: message_window_create_at_voice with a
 * voice_base of 0 */
struct task *message_window_create(int kind, const char *text, int time,
                                   int language, struct task **owner, int color,
                                   Sint16 pitch_x, Sint16 pitch_y)
{
    return message_window_create_at_voice(kind, text, time, language, owner,
                                          color, pitch_x, pitch_y, 0);
}

/* message_window_open_at_voice's work: message_window_create_unowned with
 * an owner. the task's unk_3c keeps voice_base for the later pages */
static struct task *
message_window_create_at_voice(int kind, const char *text, int time,
                               int language, struct task **owner, int color,
                               Sint16 pitch_x, Sint16 pitch_y, int voice_base)
{
    struct task *task;
    char *start;
    char *copy;
    int count;
    int voice;
    int length;
    Bool replaced;
    Uint16 width;
    Uint16 height;

    replaced = FALSE;
    if (message_window_task) {
        task_destroy(message_window_task);
        replaced = TRUE;
    }
    task = message_window_task = task_create(
        TASK_WORK, TEXT_LEVEL, message_window_step, "EfMsgWnd0Exec");
    if (task) {
        task->motion.p = syCalloc(1, sizeof(struct message_window));
        length = strlen(text);
        if (task->motion.p && (copy = syMalloc(length + 2)) != 0) {
            /* @hack spelling: _builtin_strcpy, where strcpy would do */
            _builtin_strcpy(copy, text);
            WINDOW(task)->text = copy;
            count = -1;
            voice = -1;
            page_read_codes(copy, &start, &count, &voice, &WINDOW(task)->next,
                            voice_base);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = 0;
            }
            text_sprite_start(&WINDOW(task)->sprite, MESSAGE_WINDOW_X,
                              MESSAGE_WINDOW_Y, MESSAGE_WINDOW_WIDTH,
                              MESSAGE_WINDOW_HEIGHT, MESSAGE_WINDOW_TEXTURE,
                              NULL, pitch_x, pitch_y, OWN_PIXEL(color));
            text_sprite_measure(&WINDOW(task)->sprite, start, language, &width,
                                &height);
            if (width < MESSAGE_WINDOW_WIDTH) {
                width = MESSAGE_WINDOW_WIDTH;
            }
            text_sprite_start(&WINDOW(task)->sprite, MESSAGE_WINDOW_X,
                              MESSAGE_WINDOW_Y, width, height,
                              MESSAGE_WINDOW_TEXTURE, (Uint16 *)WORK_AREA,
                              pitch_x, pitch_y, OWN_PIXEL(color));
            text_sprite_print_load(&WINDOW(task)->sprite, start, language);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = '\f';
            }
            task->draw = message_window_draw;
            task->step = message_window_step;
            task->destructor = message_window_destroy;
            WINDOW(task)->unk_58 = 0;
            WINDOW(task)->color = color;
            WINDOW(task)->count = count >= 0 ? count : time;
            WINDOW(task)->language = language;
            WINDOW(task)->owner = owner;
            WINDOW(task)->pitch_x = pitch_x;
            WINDOW(task)->pitch_y = pitch_y;
            WINDOW(task)->time = time;
            WINDOW(task)->voice = -1;
            if (WINDOW(task)->time < 0) {
                WINDOW(task)->count = PRESS_COUNT;
            }
            WINDOW(task)->kind = kind;
            WINDOW(task)->sprite.color.color = window_color;
            WINDOW(task)->sprite.band_color.color = window_band_color;
            WINDOW(task)->sprite.shadow_color.color = window_shadow_color;
            if (replaced) {
                task->work->submode = WINDOW_OPAQUE;
            }
            switch (kind) {
            case MESSAGE_WINDOW_LOW:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                    WINDOW(task)->sprite.y += WINDOW(task)->sprite.band_height -
                                              WINDOW(task)->sprite.height;
                }
                break;
            case MESSAGE_WINDOW_FITTED:
            case MESSAGE_WINDOW_UNFOLD:
                break;
            case MESSAGE_WINDOW_INSTANT:
                task->work->submode = WINDOW_OPAQUE;
                /* fall through */
            default:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                }
                break;
            }
            task->unk_3c.l = voice_base;
            if (owner) {
                *owner = task;
            }
            if (voice >= 0) {
                WINDOW(task)->voice = voice_play(voice, 0);
            }
        } else {
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            message_window_task = NULL;
        }
    }
    return message_window_task;
}

/* the window's destructor. it stops the page's voice only while the
 * window's alpha isn't 0 */
static void message_window_destroy(struct task *task)
{
    struct task_work *work = task->work;

    text_sprite_release(&WINDOW(task)->sprite);
    if (WINDOW(task) && WINDOW(task)->voice != -1 && work->submode) {
        voice_stop(WINDOW(task)->voice);
    }
    if (WINDOW(task) && WINDOW(task)->owner && *WINDOW(task)->owner == task) {
        *WINDOW(task)->owner = NULL;
    }
    if (WINDOW(task) && WINDOW(task)->text) {
        syFree(WINDOW(task)->text);
    }
    if (message_window_task == task) {
        message_window_task = NULL;
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* the window's step. a page shows for its count of steps at full alpha. at a
 * count of 0 the window turns to its next page. after the last page it sets the
 * count to -1. under a negative time the window waits at a count of 0 until
 * player_pads[-time - 1] presses A. while the count is above 0 the window fades
 * in, and then counts down at full alpha. once the count is below 0 it fades
 * out, and then destroys its task. a MESSAGE_WINDOW_INSTANT window shows at
 * full alpha without counting down. it skips the fade out, and destroys its
 * task as soon as the count is below 0 */
static void message_window_step(struct task *task)
{
    struct task_work *work = task->work;
    char *start;
    int count;
    int voice;
    int alpha;
    Uint16 width;
    Uint16 height;

    if (WINDOW(task)->count == 0) {
        if (WINDOW(task)->time < 0 &&
            !(player_pads[-WINDOW(task)->time - 1]->press & PDD_DGT_TA)) {
            return;
        }
        if (WINDOW(task)->next) {
            text_sprite_release(&WINDOW(task)->sprite);
            count = -1;
            voice = -1;
            page_read_codes(WINDOW(task)->next, &start, &count, &voice,
                            &WINDOW(task)->next, task->unk_3c.l);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = 0;
            }
            text_sprite_start(
                &WINDOW(task)->sprite, MESSAGE_WINDOW_X, MESSAGE_WINDOW_Y,
                MESSAGE_WINDOW_WIDTH, MESSAGE_WINDOW_HEIGHT,
                MESSAGE_WINDOW_TEXTURE, NULL, WINDOW(task)->pitch_x,
                WINDOW(task)->pitch_y, OWN_PIXEL(WINDOW(task)->color));
            text_sprite_measure(&WINDOW(task)->sprite, start,
                                WINDOW(task)->language, &width, &height);
            if (width < MESSAGE_WINDOW_WIDTH) {
                width = MESSAGE_WINDOW_WIDTH;
            }
            text_sprite_start(&WINDOW(task)->sprite, MESSAGE_WINDOW_X,
                              MESSAGE_WINDOW_Y, width, height,
                              MESSAGE_WINDOW_TEXTURE, (Uint16 *)WORK_AREA,
                              WINDOW(task)->pitch_x, WINDOW(task)->pitch_y,
                              OWN_PIXEL(WINDOW(task)->color));
            text_sprite_print_load(&WINDOW(task)->sprite, start,
                                   WINDOW(task)->language);
            if (WINDOW(task)->next) {
                *WINDOW(task)->next = '\f';
            }
            WINDOW(task)->sprite.color.color = window_color;
            WINDOW(task)->sprite.band_color.color = window_band_color;
            WINDOW(task)->sprite.shadow_color.color = window_shadow_color;
            switch (WINDOW(task)->kind) {
            case MESSAGE_WINDOW_LOW:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                    WINDOW(task)->sprite.y += WINDOW(task)->sprite.band_height -
                                              WINDOW(task)->sprite.height;
                }
                break;
            case MESSAGE_WINDOW_FITTED:
            case MESSAGE_WINDOW_UNFOLD:
                break;
            default:
                if (WINDOW(task)->sprite.band_height < MESSAGE_WINDOW_HEIGHT) {
                    WINDOW(task)->sprite.band_height = MESSAGE_WINDOW_HEIGHT;
                }
                break;
            }
            WINDOW(task)->unk_58 = 0;
            WINDOW(task)->count = count >= 0 ? count : WINDOW(task)->time;
            if (WINDOW(task)->time < 0) {
                WINDOW(task)->count = PRESS_COUNT;
            }
            if (voice >= 0) {
                if (WINDOW(task)->voice != -1) {
                    voice_stop(WINDOW(task)->voice);
                    WINDOW(task)->voice = -1;
                }
                WINDOW(task)->voice = voice_play(voice, 0);
            }
        } else {
            WINDOW(task)->count = -1;
        }
    } else if (WINDOW(task)->kind == MESSAGE_WINDOW_INSTANT) {
        if (WINDOW(task)->count > 0) {
            work->submode = WINDOW_OPAQUE;
        } else {
            work->submode = 0;
            task_destroy(task);
        }
    } else {
        alpha = WINDOW_ALPHA(work);
        if (WINDOW(task)->count > 0) {
            if (alpha == ALPHA_MAX) {
                WINDOW(task)->count--;
            } else {
                alpha += FADE_IN_STEP;
                if (alpha > ALPHA_MAX) {
                    alpha = ALPHA_MAX;
                }
                work->submode = alpha;
            }
        } else {
            alpha -= FADE_OUT_STEP;
            if (alpha < 0) {
                work->submode = 0;
                task_destroy(task);
            } else {
                work->submode = alpha;
            }
        }
    }
}

/* the window's draw. a MESSAGE_WINDOW_HIDDEN window draws nothing, and so does
 * any window while game_state.message_window_on isn't 1. neither changes the
 * fog. any other draw turns the fog off, and turns it on after, whatever it
 * was. below full alpha the draw shows a copy of the page's sprite. the copy's
 * alphas are scaled by the window's. a MESSAGE_WINDOW_UNFOLD window's copy
 * widens over the fade's first half. it grows to its full height over the
 * second */
static void message_window_draw(struct task *task)
{
    /* @hack type: work declared volatile, where a plain pointer would do */
    struct task_work *volatile work = task->work;
    struct text_sprite sprite;
    int alpha;
    Uint32 mask;
    Float progress;
    Float unfold_x;
    Float unfold_y;

    if (WINDOW(task)->kind == MESSAGE_WINDOW_HIDDEN) {
        return;
    }
    if (game_state.message_window_on != 1) {
        return;
    }
    njFogDisable();
    alpha = WINDOW_ALPHA(work);
    if (alpha != ALPHA_MAX) {
        sprite = WINDOW(task)->sprite;
        if (WINDOW(task)->kind == MESSAGE_WINDOW_UNFOLD) {
            progress = alpha;
            progress *= 1.0F / ALPHA_MAX;
            if (progress > 0.5F) {
                unfold_x = 1.0F;
                unfold_y = FOLDED + (njSin((progress - 0.5F) * HALF_TURN) *
                                     (1.0F - FOLDED));
            } else {
                unfold_y = FOLDED;
                unfold_x = njSin(progress * HALF_TURN);
            }
            sprite.scale_x *= unfold_x;
            sprite.scale_y *= unfold_y;
        }
        mask = (alpha << 24) | ~COLOR_ALPHA;
        sprite.color.color = color_multiply(mask, sprite.color.color);
        sprite.band_color.color = color_multiply(mask, sprite.band_color.color);
        sprite.shadow_color.color =
            color_multiply(mask, sprite.shadow_color.color);
        text_sprite_draw(&sprite);
        /* @hack spelling: a goto to the function's last lines, where the
         * else's end would reach them */
        goto fog;
    } else {
        text_sprite_draw(&WINDOW(task)->sprite);
    }
fog:
    njFogEnable();
}

/* take the first of the TEXT_TEXTURES global indexes that no sprite has. it
 * returns the index, or 0 where every one is taken */
static int text_texture_take(void)
{
    int i;

    for (i = 0; i < TEXT_TEXTURES; i++) {
        if (!((textures_taken >> i) & 1)) {
            textures_taken |= 1 << i;
            return i + TEXT_TEXTURE_FIRST;
        }
    }
    return 0;
}

/* give back a global index that text_texture_take returned. it hangs for
 * an index outside the TEXT_TEXTURES */
static void text_texture_give(Uint32 texture)
{
    if (texture < TEXT_TEXTURE_FIRST ||
        texture >= TEXT_TEXTURE_FIRST + TEXT_TEXTURES) {
        for (;;) {
        }
    }
    texture -= TEXT_TEXTURE_FIRST;
    /* @bug logic: the check that texture is taken tests every bit from its
     * own up, where its own alone was meant. so an index given back twice
     * passes while a higher index is taken, and hangs only where none is.
     * TST R2,R2 at 0x8c0a6a4a tests the whole shifted word */
    while (!(textures_taken >> texture)) {
    }
    textures_taken &= ~(1 << texture);
}

struct text_sprite *text_sprite_create_styled(const char *text, int language,
                                              int color, Sint16 pitch_x,
                                              Sint16 pitch_y)
{
    struct text_sprite *sprite;
    int texture;
    Uint16 width;
    Uint16 height;

    sprite = syCalloc(1, sizeof(struct text_sprite));
    if (!sprite) {
        return NULL;
    }
    texture = text_texture_take();
    if (!texture) {
        syFree(sprite);
        return NULL;
    }
    text_sprite_start(sprite, 0, 0, 0, 0, texture, NULL, pitch_x, pitch_y,
                      OWN_PIXEL(color));
    text_sprite_measure(sprite, text, language, &width, &height);
    text_sprite_start(sprite, 0, 0, width, height, texture, (Uint16 *)WORK_AREA,
                      pitch_x, pitch_y, OWN_PIXEL(color));
    text_sprite_print(sprite, text, language);
    text_sprite_load(sprite);
    return sprite;
}

struct text_sprite *text_sprite_create(const char *text, int language)
{
    return text_sprite_create_styled(text, language, -1, -1, -1);
}

void text_sprite_destroy(struct text_sprite *sprite)
{
    text_texture_give(sprite->texture);
    text_sprite_release(sprite);
    syFree(sprite);
}

void text_sprite_draw_text_at(struct text_sprite *sprite, NJS_POINT2 *pos,
                              Float scale)
{
    text_sprite_draw_text(sprite, pos->x, pos->y, scale, scale, NULL);
}

void text_sprite_draw_all(struct text_sprite *sprite)
{
    text_sprite_draw(sprite);
}

struct task *timed_text_create(const char *text, int language, NJS_POINT2 *pos,
                               Sint16 time, struct task **owner)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, TEXT_LEVEL, timed_text_step, "ftsDMTexec");
    if (!task) {
        /* @hack implicit: no return value where task_create fails, where
         * return NULL would do: task_create's null stays the result */
#ifdef NONMATCHING
        /* this side returns the null the matching side leaves in R0 */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task->force.p = text_sprite_create(text, language);
    if (!task->force.p) {
        task_destroy(task);
        return NULL;
        /* @hack redundant: the else after the return */
    } else {
        work = task->work;
        task->motion.p = owner;
        if (owner) {
            *owner = task;
        }
        SPRITE(task)->x = pos->x;
        SPRITE(task)->y = pos->y;
        work->timer = time;
        task->draw = timed_text_draw;
        task->step = timed_text_step;
        task->destructor = timed_text_destroy;
        work->mode = TIMED_TEXT_FADING_IN;
        work->tick = 0;
    }
    return task;
}

struct task *timed_text_create_fading(const char *text, int language,
                                      NJS_POINT2 *pos, Sint16 time,
                                      struct task **owner)
{
    struct task *task = timed_text_create(text, language, pos, time, owner);

    if (task) {
        task->work->tick = TIMED_FADE_STEP;
        if (SPRITE(task)->color.argb.a) {
            SPRITE(task)->color.argb.a = 1;
        }
        if (SPRITE(task)->shadow_color.argb.a) {
            SPRITE(task)->shadow_color.argb.a = 1;
        }
        if (SPRITE(task)->band_color.argb.a) {
            SPRITE(task)->band_color.argb.a = 1;
        }
    }
    return task;
}

/* the timed text's destructor. it frees the sprite itself. it nulls motion,
 * which holds owner, and force, so that task_destroy frees neither */
static void timed_text_destroy(struct task *task)
{
    struct text_sprite *sprite = task->force.p;

    text_texture_give(sprite->texture);
    text_sprite_release(sprite);
    syFree(sprite);
    if (task->motion.p && *OWNER(task) == task) {
        *OWNER(task) = NULL;
    }
    task->motion.p = NULL;
    task->force.p = NULL;
}

static void timed_text_draw(struct task *task)
{
    text_sprite_draw(task->force.p);
}

/* the timed text's step. the work's tick is the alpha the fade adds or takes at
 * a time. it is 0 for a text that doesn't fade. its timer is the steps left to
 * show the text once any fade in is done */
static void timed_text_step(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 alpha;
    Int fade;

    /* @bug logic: the fade in runs twice a step: in this if, and again in the
     * if below while the mode is still TIMED_TEXT_FADING_IN. this if leaves
     * the mode at TIMED_TEXT_SHOWN only where every alpha is full. MOV.B at
     * 0x8c0a6d7c writes the mode here, and the test at 0x8c0a6dfc reads it */
    if (!work->mode && work->tick) {
        work->mode = TIMED_TEXT_SHOWN;
        if (SPRITE(task)->color.color & COLOR_ALPHA) {
            if ((alpha = SPRITE(task)->color.argb.a + work->tick) > ALPHA_MAX) {
                alpha = ALPHA_MAX;
            } else {
                work->mode = TIMED_TEXT_FADING_IN;
            }
            SPRITE(task)->color.argb.a = alpha;
        }
        if (SPRITE(task)->shadow_color.color & COLOR_ALPHA) {
            if ((alpha = SPRITE(task)->shadow_color.argb.a + work->tick) >
                ALPHA_MAX) {
                alpha = ALPHA_MAX;
            } else {
                work->mode = TIMED_TEXT_FADING_IN;
            }
            SPRITE(task)->shadow_color.argb.a = alpha;
        }
        /* @bug logic: the band's arm adds to the shadow's alpha, so the band
         * never fades in, and the shadow takes a tick more while the band has
         * alpha. MOV.L @(52,R3) at 0x8c0a6dce reads the band's color, and the
         * shadow's alpha byte, at 51, is read and written after it. the if
         * below does the same at 0x8c0a6e54 */
        if (SPRITE(task)->band_color.color & COLOR_ALPHA) {
            if ((alpha = SPRITE(task)->shadow_color.argb.a + work->tick) >
                ALPHA_MAX) {
                alpha = ALPHA_MAX;
            } else {
                work->mode = TIMED_TEXT_FADING_IN;
            }
            SPRITE(task)->shadow_color.argb.a = alpha;
        }
    }
    if (work->tick) {
        if (work->mode == TIMED_TEXT_FADING_IN) {
            work->mode = TIMED_TEXT_SHOWN;
            if (SPRITE(task)->color.color & COLOR_ALPHA) {
                if ((alpha = SPRITE(task)->color.argb.a + work->tick) >
                    ALPHA_MAX) {
                    alpha = ALPHA_MAX;
                } else {
                    work->mode = TIMED_TEXT_FADING_IN;
                }
                SPRITE(task)->color.argb.a = alpha;
            }
            if (SPRITE(task)->shadow_color.color & COLOR_ALPHA) {
                if ((alpha = SPRITE(task)->shadow_color.argb.a + work->tick) >
                    ALPHA_MAX) {
                    alpha = ALPHA_MAX;
                } else {
                    work->mode = TIMED_TEXT_FADING_IN;
                }
                SPRITE(task)->shadow_color.argb.a = alpha;
            }
            if (SPRITE(task)->band_color.color & COLOR_ALPHA) {
                if ((alpha = SPRITE(task)->shadow_color.argb.a + work->tick) >
                    ALPHA_MAX) {
                    alpha = ALPHA_MAX;
                } else {
                    work->mode = TIMED_TEXT_FADING_IN;
                }
                SPRITE(task)->shadow_color.argb.a = alpha;
            }
        }
        if (work->mode == TIMED_TEXT_SHOWN && work->timer) {
            work->timer--;
            if (!work->timer) {
                work->mode = TIMED_TEXT_FADING_OUT;
            }
        }
        if (work->mode == TIMED_TEXT_FADING_OUT) {
            work->mode = TIMED_TEXT_DONE;
            if (SPRITE(task)->color.color & COLOR_ALPHA) {
                fade = SPRITE(task)->color.argb.a - work->tick;
                if (fade < 0) {
                    fade = 0;
                } else {
                    work->mode = TIMED_TEXT_FADING_OUT;
                }
                SPRITE(task)->color.argb.a = fade;
            }
            if (SPRITE(task)->shadow_color.color & COLOR_ALPHA) {
                fade = SPRITE(task)->shadow_color.argb.a - work->tick;
                if (fade < 0) {
                    fade = 0;
                } else {
                    work->mode = TIMED_TEXT_FADING_OUT;
                }
                SPRITE(task)->shadow_color.argb.a = fade;
            }
            if (SPRITE(task)->band_color.color & COLOR_ALPHA) {
                fade = SPRITE(task)->band_color.argb.a - work->tick;
                if (fade < 0) {
                    fade = 0;
                } else {
                    work->mode = TIMED_TEXT_FADING_OUT;
                }
                SPRITE(task)->band_color.argb.a = fade;
            }
        }
        if (work->mode == TIMED_TEXT_DONE) {
            task_destroy(task);
        }
    } else if (work->timer) {
        work->timer--;
        if (!work->timer) {
            task_destroy(task);
        }
    }
}
