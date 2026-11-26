/* text sprites: a text drawn in one of the game's fonts into a texture of
 * its own */
#ifndef DRAW_TEXT_SPRITE_H
#define DRAW_TEXT_SPRITE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* pointer arguments must not be null unless a comment says they may be */

/* a glyph's width and height in pixels, and a line's height before pitch_y */
#define GLYPH_SIZE 24

/* the default pitches, which text_sprite_create gives and a negative pitch
 * asks for */
#define TEXT_PITCH_X 2
#define TEXT_PITCH_Y 4

/* a text is a string in its language's encoding. its language is an enum
 * text_language of system/text.h, and picks its font. the Japanese font reads
 * a byte of 0x80 or more and the byte after it as one Shift JIS code. it has
 * glyphs for the codes from 0x8140 to 0x9872, the first level's last kanji,
 * and none for the one-byte kana. the other fonts read each byte as one
 * character, 0x20 to 0xff. a character below 0x20 draws as a space, but for
 * '\n', which ends the line.
 *
 * a '\b' opens a code, which draws nothing and sets how the characters after
 * it draw. a code's letter may be a capital:
 *
 *   \bu       underline them
 *   \bi       end the underline
 *   \bpN      color them by the hexadecimal digit N: 1 red, 2 green, 3 blue,
 *             4 yellow, 5 magenta and 6 cyan, and white for 0 and from 7 up
 *   \bRRGGBB  color them 0xRRGGBB, each channel cut to its high five bits
 *   \br       give them the sprite's own color, glyph_color, without an
 *             underline
 *
 * one code at most stands before each character. a second '\b' right after
 * a code draws as a space, and the letters after it draw as text. an
 * underline is GLYPH_SIZE pixels long under each character, whatever the
 * character's width. underlined characters without a color from a code are
 * white, not the sprite's own color.
 *
 * a '\t' or '\a' as a text's first character centers the text. under '\t'
 * each line is centered in the sprite's width. under '\a' every line starts
 * at the x that centers the widest. each centered line starts in the
 * sprite's own color, without an underline. the centering measures a code's
 * characters as text, so a line that holds a code sits left of center.
 *
 * each character moves the cursor right by its width in the font and
 * pitch_x. a character that would pass the texture's right edge starts the
 * next line. rows outside the texture don't draw */

/* a text sprite. its pixels are ARGB 1555, and each draw tints them by a
 * color */
struct text_sprite {
    Uint8 hidden; /* text_sprite_draw draws nothing while it is 1 */
    /* the texture's top left corner on the screen, in pixels */
    Uint16 x;
    Uint16 y;
    /* the size of one of the texture's pixels on the screen */
    Float scale_x;
    Float scale_y;
    /* how far the shadow lies right of the text and below it, in the
     * texture's pixels */
    Float shadow_offset;
    /* the size the text is laid out in, in pixels. a centered line is
     * centered in width. text_sprite_create sizes it to the text */
    Uint16 width;
    Uint16 height;
    /* the texture's size: the least powers of two, from 8, that hold width
     * and height */
    Uint16 texture_width;
    Uint16 texture_height;
    /* where the next character's top left corner goes in the texture */
    Sint16 cursor_x;
    Sint16 cursor_y;
    /* the pixels between two characters, and between two lines */
    Sint16 pitch_x;
    Sint16 pitch_y;
    /* the sprite's own color, as a pixel: the color of each character that
     * no code colors */
    Uint16 glyph_color;
    /* the band text_sprite_draw fills behind the text. band_x and band_y
     * are its left edge and its top on the screen. it is TEXT_BAND_WIDTH
     * pixels wide and band_height tall before scale_x and scale_y. it reaches
     * TEXT_BAND_MARGIN pixels further up and down, before scale_y */
    Uint16 band_x;
    Uint16 band_y;
    Uint16 band_height;
    /* the ARGB colors text_sprite_draw draws the text, the shadow and the
     * band in. it draws none whose alpha is 0 */
    NJS_COLOR color;
    NJS_COLOR shadow_color;
    NJS_COLOR band_color;
    /* where the text is drawn for the texture to load, or null.
     * text_sprite_create leaves it pointing into WORK_AREA, which other uses
     * then write over */
    Uint16 *pixels;
    Sint32 texture; /* the texture's global index */
    /* texname is the texture, and texlist lists it alone. texlist.textures
     * is null while no texture is loaded */
    NJS_TEXLIST texlist;
    NJS_TEXNAME texname;
};
#define TEXT_BAND_WIDTH 640.0F
#define TEXT_BAND_MARGIN 4

/* read the three font files into blocks of the Shinobi heap, and point the
 * fonts at them. each kanji moves to its cell's left edge, so that it takes
 * only its own width. a second call does nothing until text_fonts_free.
 * nothing checks the reads. a font whose own file won't read draws its
 * characters from memory near address 0 */
void text_fonts_load(void);
/* free the font files. until the next text_fonts_load, each character is
 * GLYPH_SIZE wide, and its glyph is read from the freed blocks */
void text_fonts_free(void);

/* draw sprite's band, then its shadow, then its text, each only where its
 * color's alpha isn't 0. the shadow and the text need a texture loaded. a
 * hidden sprite draws nothing and changes nothing.
 *
 * it draws the texture under NJD_TEXTUREFILTER_POINT_SAMPLE and
 * NJD_CONTROL_3D_CONSTANT_MATERIAL, and without
 * NJD_CONTROL_3D_OFFSET_MATERIAL. the call then sets the filter to
 * NJD_TEXTUREFILTER_BILINEAR and puts back _nj_control_3d_flag_. it saves the
 * flag with draw_control_save, so a caller's own save is lost. a band leaves
 * njColorBlendingMode's factors at NJD_COLOR_BLENDING_SRCALPHA for the source
 * and NJD_COLOR_BLENDING_INVSRCALPHA for the destination */
void text_sprite_draw(struct text_sprite *sprite);
/* draw sprite's text alone, tinted by *color, or by sprite's color where
 * color is null. the texture's top left corner goes at x and y. each of its
 * pixels is scale_x by scale_y on the screen. it draws nothing while sprite has
 * no texture loaded, and it draws a hidden sprite too. it changes no filter or
 * control bit */
void text_sprite_draw_text(struct text_sprite *sprite, Float x, Float y,
                           Float scale_x, Float scale_y, const Uint32 *color);
/* set *width and *height to the size of text in language at sprite's
 * pitches. *width is the widest line's width, and *height is GLYPH_SIZE plus
 * pitch_y for each line. a line too wide for the texture counts as one, and
 * a code takes no room. a text that ends in '\n' counts the empty line after
 * it */
void text_sprite_measure(struct text_sprite *sprite, const char *text,
                         int language, Uint16 *width, Uint16 *height);

/* the global indexes of text_sprite_create's textures: TEXT_TEXTURES of
 * them from TEXT_TEXTURE_FIRST */
#define TEXT_TEXTURE_FIRST 102
#define TEXT_TEXTURES 32
/* the color text_sprite_create_styled takes for white, and the pitch it
 * takes for the default */
#define TEXT_WHITE (-1)
#define TEXT_PITCH_DEFAULT (-1)

/* make a sprite of text in language, from the Shinobi heap. it lies at x 0 and
 * y 0, at a scale of 1. its text is white over a black shadow, with no band.
 * its texture is the least that holds the text. the texture takes a global
 * index that no other sprite has. the text is drawn in WORK_AREA, which must
 * hold it, and the call writes over what WORK_AREA held.
 *
 * text_sprite_create_styled takes the sprite's own color as 0xRRGGBB, or
 * TEXT_WHITE. a negative pitch, as TEXT_PITCH_DEFAULT, gives the default.
 *
 * each returns the sprite, or null where the heap can't give the record or
 * every index is taken. nothing checks the texture's load */
struct text_sprite *text_sprite_create_styled(const char *text, int language,
                                              int color, Sint16 pitch_x,
                                              Sint16 pitch_y);
struct text_sprite *text_sprite_create(const char *text, int language);
/* release the texture of a sprite text_sprite_create made, give back its
 * global index, and free the sprite. the call hangs where sprite's index
 * isn't one text_sprite_create gives. it hangs too where neither the index
 * nor any index above it is taken */
void text_sprite_destroy(struct text_sprite *sprite);
/* text_sprite_draw_text at *pos, at scale both ways, in sprite's color */
void text_sprite_draw_text_at(struct text_sprite *sprite, NJS_POINT2 *pos,
                              Float scale);
/* the same as text_sprite_draw */
void text_sprite_draw_all(struct text_sprite *sprite);

#endif
