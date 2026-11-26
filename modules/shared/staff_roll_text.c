/* the staff roll's lines and photographs, drawn as sprites */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/staff_art.h"
#include "shared/staff_roll.h"
#include "system/sequence.h"
#include "system/util.h"

#define ALT_FONT_FIRST 57
#define LINE_CAPACITY 128
#define LINE_CENTER_X 440.0F
#define LINE_PRIORITY (-1.0F)
#define LETTER_SPACING 2.0F
#define GAP_WIDTH 12.0F
/* a line whose y lies outside its kind's bounds is off the screen, and is
 * skipped */
#define LOGO_Y_MIN (-280.0F)
#define LOGO_Y_MAX 760.0F
#define TEXT_Y_MIN (-50.0F)
#define TEXT_Y_MAX 550.0F

struct line_glyph {
    Sint32 cell; /* the glyph's cell in staff_roll_cells, or -1 for a gap */
    Float x; /* the glyph's offset from the line's left end */
};

static void staff_cell_draw(int cell, const NJS_POINT3 *pos, int attributes);

void staff_glyphs_measure(void)
{
    /* @hack spelling: a pointer stepped beside i, where staff_glyphs[i] would
     * do */
    struct staff_glyph *glyph = staff_glyphs;
    /* @hack type: i as an unsigned int, where an int would do */
    unsigned int i;

    for (i = 0; i < STAFF_GLYPHS; i++, glyph++) {
        glyph->width = staff_roll_cells[glyph->cell].sx;
    }
}

/* the index in staff_glyphs of character in kind's font, or -1 where the font
 * lacks it */
static int staff_glyph_find(enum staff_line_kind kind, char character)
{
    int i;
    int first;
    int end;

    if (kind != STAFF_LINE_ALT_TEXT) {
        first = 0;
        end = ALT_FONT_FIRST;
    } else {
        first = ALT_FONT_FIRST;
        end = STAFF_GLYPHS;
    }
    for (i = first; i < end; i++) {
        if (staff_glyphs[i].character == character) {
            return i;
        }
    }
    return -1;
}

void staff_logo_draw(enum staff_line_kind kind, Float y)
{
    NJS_POINT3 pos;

    if (y < LOGO_Y_MIN || y > LOGO_Y_MAX) {
        return;
    }
    pos.x = LINE_CENTER_X;
    pos.y = y;
    pos.z = LINE_PRIORITY;
    staff_cell_draw(STAFF_LOGO_CELL(kind), &pos, NJD_SPRITE_ALPHA);
}

void staff_text_draw(enum staff_line_kind kind, NJS_ARGB *color,
                     const char *text, Float y)
{
    struct line_glyph line[LINE_CAPACITY];
    NJS_POINT3 pos;
    int length;
    int i;
    int glyph;
    Float width;

    if (y < TEXT_Y_MIN || y > TEXT_Y_MAX) {
        return;
    }
    draw_attr_save();
    njSetConstantMaterial(color);
    width = 0.0F;
    for (length = 0; length < LINE_CAPACITY && text[length] != '\0'; length++) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((glyph = staff_glyph_find(kind, text[length])) >= 0) {
            line[length].cell = staff_glyphs[glyph].cell;
            line[length].x = width;
            width += staff_glyphs[glyph].width + LETTER_SPACING;
        } else {
            line[length].cell = -1;
            line[length].x = width;
            width += GAP_WIDTH;
        }
    }
    /* @hack spelling: width reused for the left end's offset from
     * LINE_CENTER_X, where a local of its own would do */
    width *= -0.5F;
    for (i = 0; i < length; i++) {
        if (line[i].cell >= 0) {
            pos.x = line[i].x + width + LINE_CENTER_X;
            pos.y = y;
            pos.z = LINE_PRIORITY;
            staff_cell_draw(line[i].cell, &pos,
                            NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA);
        }
    }
    draw_attr_restore();
}

/* @hack type: attributes as an int, where the Uint32 njDrawSprite2D takes
 * would do */
static void staff_cell_draw(int cell, const NJS_POINT3 *pos, int attributes)
{
    NJS_SPRITE sprite;

    sprite.tlist = &staff_roll_texlist;
    sprite.tanim = staff_roll_cells;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attributes);
}

/* @hack type: attributes as an int, where the Uint32 njDrawSprite2D takes
 * would do */
void staff_photo_draw(int texture, const NJS_POINT3 *pos, int attributes)
{
    NJS_SPRITE sprite;

    if (staff_roll_story == SEQUENCE_HERO) {
        sprite.tlist = &staff_hero_texlist;
    } else {
        sprite.tlist = &staff_dark_texlist;
    }
    sprite.tanim = &staff_photo_cell;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    staff_photo_cell.texid = texture;
    njDrawSprite2D(&sprite, 0, pos->z, attributes);
}
