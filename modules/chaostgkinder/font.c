#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/font.h"
#include "chaostgkinder/font/text.inc"
#include "draw/glyph_widths.h"
#include "draw/text_sprite.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"

/* a glyph row's bytes. a byte holds BYTE_PIXELS pixels, the leftmost in its
 * low bit */
#define GLYPH_ROW_BYTES 3
#define BYTE_PIXELS 8
#define GLYPH_BYTES (GLYPH_SIZE * GLYPH_ROW_BYTES)
#define KANJI_GLYPHS 3423
#define BLANK_WIDTH 18
#define PAGE_SIZE 128
#define PAGE_BYTES (PAGE_SIZE * PAGE_SIZE / 2)
#define PAGE_SIDE (PAGE_SIZE / GLYPH_SIZE)
#define PAGE_CELLS (PAGE_SIDE * PAGE_SIDE)
#define FONT_PAGES 32
#define FONT_CELLS (FONT_PAGES * PAGE_CELLS)
#define PAGE_LOADED 1
#define PAGE_CHANGED 2
#define FONT_BANK 32U
#define BANK_COLORS 16
#define FIRST_TEXTURE 0x30a84
#define GLYPH_UV ((Float)GLYPH_SIZE / PAGE_SIZE)
#define FONT_LEVEL 2
/* a page's texel of a set pixel, the bank's last color. a byte holds two
 * texels, the upper row's in its low nibble */
#define INK 0x0f
/* the bank's colors are white, each ALPHA_STEP more opaque than the last */
#define ALPHA_STEP 0x11000000U
#define WHITE 0x00ffffffU
/* a line break needs room below y for the line it ends and the next */
#define BREAK_LINES 2.0F

struct font_file {
    const char *name;
    Uint8 *glyphs;
    const Sint8 *widths;
};

struct glyph_cell {
    Uint16 code;
    Sint8 width;
    Uint8 unused[5];
    Uint32 last_used;
};

static Sint8 kanji_widths[KANJI_GLYPHS];
static NJS_TEXLIST pages[FONT_PAGES];
static NJS_TEXNAME names[FONT_PAGES];
static NJS_TEXINFO textures[FONT_PAGES];
static struct glyph_cell cells[FONT_CELLS];
static int used_cells;
static Uint8 page_flags[FONT_PAGES];
static struct task *font_task;

/* the Japanese, Latin and ASCII fonts. lookup_glyph reads only the first
 * two */
static struct font_file fonts[3] = {
#include "chaostgkinder/font/fonts.inc"
};

/* the offset of the byte that holds pixel x of row pair row_pair, in a
 * twiddled 4-bit texture. a pair is rows 2 * row_pair and 2 * row_pair + 1.
 * the offset's even bits are x's, and its odd bits row_pair's */
static Uint32 pixel_pair_offset(int row_pair, int x)
{
    /* @hack implicit: offset left unset, where starting it at 0 would do.
     * the loop shifts out all 32 of its first bits */
    Uint32 offset;
    int bit;
    int next;
#ifdef NONMATCHING
    /* this side starts offset at 0, which the loop below can read before it
     * sets it */
    offset = 0;
#endif
    for (bit = 31; bit >= 0; bit--) {
        /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
        offset >>= 1;
        offset |= (Uint32)x << 31;
        next = x >> 1;
        x = row_pair;
        row_pair = next;
    }
    return offset;
}

static int column_width(const Uint8 *column)
{
    int width = 0;
    int row;
    for (row = 0; row < GLYPH_SIZE; row++) {
        int bits = *column;
        int used = 0;
        column += GLYPH_ROW_BYTES;
        while (bits) {
            bits >>= 1;
            used++;
        }
        if (width < used) {
            width = used;
        }
    }
    return width;
}

static int glyph_width(const Uint8 *glyph)
{
    int width;
    if ((width = column_width(glyph + 2)) > 0) {
        width += 2 * BYTE_PIXELS;
    } else if ((width = column_width(glyph + 1)) > 0) {
        width += BYTE_PIXELS;
    } else if ((width = column_width(glyph)) <= 0) {
        width = BLANK_WIDTH;
    }
    return width;
}

/* the Japanese font's glyphs are the Shift JIS codes in order from
 * FIRST_LEAD. each lead byte has LEAD_CODES of them: the trail bytes from
 * FIRST_TRAIL to 0xfc but SKIPPED_TRAIL. a code's place in that order is its
 * index. the font leaves out HIRAGANA_GAP codes below the first hiragana,
 * 0x829f, whose index is HIRAGANA_INDEX. it leaves out KANJI_GAP below the
 * first kanji, 0x889f */
#define FIRST_LEAD 0x81
#define FIRST_TRAIL 0x40
#define SKIPPED_TRAIL 0x7f
#define LEAD_CODES 188
#define HIRAGANA_INDEX 282
#define HIRAGANA_GAP 4
#define KANJI_INDEX 1410
#define KANJI_GAP 952

static void lookup_glyph(int code, const Uint8 **glyph, int *width)
{
    struct font_file *font = fonts;
    int row;
    int column;
    if (code <= 0xff) {
        font++;
        code -= ' ';
        if (code < 0) {
            code = 0;
        }
    } else {
        row = code >> 8;
        column = code & 0xff;
        row -= FIRST_LEAD;
        if (column <= SKIPPED_TRAIL - 1) {
            column -= FIRST_TRAIL;
        } else {
            column -= FIRST_TRAIL + 1;
        }
        code = column + (row * LEAD_CODES);
        if (code >= KANJI_INDEX) {
            code -= KANJI_GAP;
        } else if (code >= HIRAGANA_INDEX) {
            code -= HIRAGANA_GAP;
        }
    }
    if (glyph) {
        *glyph = font->glyphs + (code * GLYPH_BYTES);
    }
    if (width) {
        *width = font->widths[code];
    }
}

static void load_fonts(void)
{
    int i;
    struct font_file *font;
    const Uint8 *glyph;
    for (i = 0; (unsigned)i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        font = &fonts[i];
        font->glyphs = loader_read_alloc(font->name);
    }
    glyph = fonts[0].glyphs;
    for (i = 0; i < KANJI_GLYPHS; i++) {
        kanji_widths[i] = glyph_width(glyph);
        glyph += GLYPH_BYTES;
    }
}

static void free_fonts(void)
{
    int i;
    for (i = 0; (unsigned)i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        struct font_file *font = &fonts[i];
        syFree(font->glyphs);
    }
}

static int cell_width(int cell)
{
    return cells[cell].width;
}

static int cell_page(int cell)
{
    return cell / PAGE_CELLS;
}

static int cell_column(int cell)
{
    return cell % PAGE_SIDE;
}

static int cell_row(int cell)
{
    return cell / PAGE_SIDE % PAGE_SIDE;
}

static void replace_cell(int index, int code)
{
    int width;
    struct glyph_cell *cell = &cells[index];
    cell->code = code;
    lookup_glyph(code, NULL, &width);
    cell->width = width;
    cell->last_used = game_state.total_steps;
    page_flags[cell_page(index)] |= PAGE_CHANGED;
}

static int find_cell(int code)
{
    int index;
    int other;
    Uint32 oldest;
    for (index = 0; index < used_cells; index++) {
        if (cells[index].code == code) {
            cells[index].last_used = game_state.total_steps;
            return index;
        }
    }
    if (used_cells == FONT_CELLS) {
        oldest = cells[0].last_used;
        index = 0;
        for (other = 1; other < used_cells; other++) {
            if (oldest >= cells[other].last_used) {
                oldest = cells[other].last_used;
                index = other;
            }
        }
    } else {
        used_cells++;
    }
    replace_cell(index, code);
    return index;
}

static int cell_index(int row, int column, int page)
{
    return (((page * PAGE_SIDE) + row) * PAGE_SIDE) + column;
}

static void upload_page(int page)
{
    Uint8 *pixels;
    const Uint8 *glyph;
    Uint8 *pair;
    NJS_TEXLIST *texlist;
    NJS_TEXINFO *info;
    NJS_TEXNAME *name;
    int row;
    int column;
    int y;
    int x;
    int bits;
#ifdef NONMATCHING
    /* each row reads bits at its first pixel, so this 0 is never read */
    bits = 0;
#endif

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(pixels = syCalloc(PAGE_BYTES, 1))) {
        return;
    }
    for (row = 0; row < PAGE_SIDE; row++) {
        for (column = 0; column < PAGE_SIDE; column++) {
            lookup_glyph(cells[cell_index(row, column, page)].code, &glyph,
                         NULL);
            for (y = 0; y < GLYPH_SIZE; y++) {
                for (x = 0; x < GLYPH_SIZE; x++) {
                    pair = pixels +
                           pixel_pair_offset(((row * GLYPH_SIZE) + y) >> 1,
                                             (column * GLYPH_SIZE) + x);
                    if ((x & (BYTE_PIXELS - 1)) == 0) {
                        bits = *glyph++;
                    }
                    if (!(y & 1)) {
                        *pair = (bits & 1) * INK;
                    } else {
                        *pair |= (bits & 1) * (INK << 4);
                    }
                    bits >>= 1;
                }
            }
        }
    }
    if (page_flags[page] & PAGE_LOADED) {
        njSetTexture(&pages[page]);
        njReLoadTextureNum(0, pixels, NJD_TEXATTR_TYPE_MEMORY, 0);
    } else {
        texlist = &pages[page];
        info = &textures[page];
        name = &names[page];
        njSetTextureInfo(info, (Uint16 *)pixels, NJD_TEXFMT_PALETTIZE4,
                         PAGE_SIZE, PAGE_SIZE);
        njSetTextureName(name, info,
                         ((page + FIRST_TEXTURE) & NJD_TEXPALGLOBAL_MASK) |
                             (FONT_BANK << NJD_TEXBANK_SHIFT),
                         NJD_TEXATTR_TYPE_MEMORY | NJD_TEXATTR_PALGLOBALINDEX);
        texlist->textures = name;
        texlist->nbTexture = 1;
        njLoadTexture(texlist);
    }
    page_flags[page] = PAGE_LOADED;
    syFree(pixels);
}

static void font_destroy(struct task *task)
{
}

static void font_draw(struct task *task)
{
}

static void font_step(struct task *task)
{
    int page;
    if (!task->destructor) {
        task->destructor = font_destroy;
        task->draw = font_draw;
    }
    for (page = 0; page < FONT_PAGES; page++) {
        if (page_flags[page] & PAGE_CHANGED) {
            upload_page(page);
        }
    }
}

/* the quad of the glyph in the cell at column and row of its page. the quad
 * is width by height, at x, y in text's box. it shows the cell's leftmost
 * dots texels */
#define TEXT_QUAD(quad, text, x, y, width, height, dots, column, row)          \
    (quad).x1 = (text)->left + (x);                                            \
    (quad).x2 = (quad).x1 + (width);                                           \
    (quad).y1 = (text)->top + (y);                                             \
    (quad).y2 = (quad).y1 + (height);                                          \
    (quad).u1 = (column) * GLYPH_UV;                                           \
    (quad).u2 = (quad).u1 + ((dots) * (1.0F / PAGE_SIZE));                     \
    (quad).v1 = (row) * GLYPH_UV;                                              \
    (quad).v2 = (quad).v1 + GLYPH_UV

int kinder_text_draw(struct kinder_text *text)
{
    NJS_QUAD_TEXTURE quad;
    const Uint16 *next = text->layout.string;
    int left = text->layout.length;
    int code;
    int cell;
    int page;
    int column;
    int row;
    Bool started;
    Float x = text->layout.x;
    Float y = text->layout.y;
    Float width;

    started = FALSE;
    for (; left && (code = *next) != 0; next++, left--) {
        if (code == '\n') {
            if (y + ((text->layout.leading + GLYPH_SIZE) * BREAK_LINES) >
                text->layout.height) {
                break;
            }
            y += text->layout.leading + GLYPH_SIZE;
            x = 0.0F;
            continue;
        }
        cell = find_cell(code);
        page = cell_page(cell);
        width = cell_width(cell);
        if (x + width + text->layout.spacing > text->layout.width) {
            if (y + ((text->layout.leading + GLYPH_SIZE) * BREAK_LINES) >
                text->layout.height) {
                break;
            }
            x = 0.0F;
            y += text->layout.leading + GLYPH_SIZE;
        }
        if (page_flags[page] & PAGE_LOADED) {
            column = cell_column(cell);
            row = cell_row(cell);
            TEXT_QUAD(quad, text, x, y, width, GLYPH_SIZE, width, column, row);
            if (!started) {
                njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
                njQuadTextureStart(TRUE);
                started = TRUE;
            }
            njSetTexture(&pages[page]);
            njSetQuadTexture(0, text->color);
            njDrawQuadTexture(&quad, text->z);
        }
        width += text->layout.spacing;
        x += width;
        /* @hack redundant: the continue that ends the loop's body */
        continue; /* NOLINT(readability-redundant-control-flow) */
    }
    if (started) {
        njQuadTextureEnd();
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    }
    text->layout.x = x;
    text->layout.y = y;
    return next - text->layout.string;
}

int kinder_text_draw_scaled(struct kinder_text *text, Float scale, Float height)
{
    NJS_QUAD_TEXTURE quad;
    const Uint16 *next = text->layout.string;
    int left = text->layout.length;
    int code;
    int cell;
    int page;
    int column;
    int row;
    Bool started;
    Float x = text->layout.x;
    Float y = text->layout.y;
    Float width;
    Float dots;

    started = FALSE;
    for (; left && (code = *next) != 0; next++, left--) {
        if (code == '\n') {
            if (y + ((height + text->layout.leading) * BREAK_LINES) >
                text->layout.height) {
                break;
            }
            y += height + text->layout.leading;
            x = 0.0F;
            continue;
        }
        cell = find_cell(code);
        page = cell_page(cell);
        dots = cell_width(cell);
        width = scale * dots * (1.0F / GLYPH_SIZE);
        if (x + width + text->layout.spacing > text->layout.width) {
            if (y + ((height + text->layout.leading) * BREAK_LINES) >
                text->layout.height) {
                break;
            }
            x = 0.0F;
            y += height + text->layout.leading;
        }
        if (page_flags[page] & PAGE_LOADED) {
            column = cell_column(cell);
            row = cell_row(cell);
            TEXT_QUAD(quad, text, x, y, width, height, dots, column, row);
            if (!started) {
                njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
                njQuadTextureStart(TRUE);
                started = TRUE;
            }
            njSetTexture(&pages[page]);
            njSetQuadTexture(0, text->color);
            njDrawQuadTexture(&quad, text->z);
        }
        width += text->layout.spacing;
        x += width;
        /* @hack redundant: the continue that ends the loop's body */
        continue; /* NOLINT(readability-redundant-control-flow) */
    }
    if (started) {
        njQuadTextureEnd();
    }
    text->layout.x = x;
    text->layout.y = y;
    return next - text->layout.string;
}

int kinder_text_measure(struct kinder_text_layout *layout)
{
    const Uint16 *next = layout->string;
    int left = layout->length;
    int code;
    int cell;
    Float x = layout->x;
    Float y = layout->y;
    Float width;

    while (left-- && (code = *next) != 0) {
        cell = find_cell(code);
        width = cell_width(cell);
        if (x + width + layout->spacing > layout->width) {
            if (y + ((layout->leading + GLYPH_SIZE) * BREAK_LINES) >
                layout->height) {
                break;
            }
            y += layout->leading + GLYPH_SIZE;
            x = 0.0F;
        }
        width += layout->spacing;
        x += width;
        next++;
    }
    layout->x = x;
    layout->y = y;
    return layout->string - next;
}

int kinder_text_measure_scaled(struct kinder_text_layout *layout, Float scale,
                               Float height)
{
    const Uint16 *next = layout->string;
    int left = layout->length;
    int code;
    int cell;
    Float x = layout->x;
    Float y = layout->y;
    Float width;
    Float dots;

    while (left-- && (code = *next) != 0) {
        cell = find_cell(code);
        dots = cell_width(cell);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (x + (width = scale * dots * (1.0F / GLYPH_SIZE)) + layout->spacing >
            layout->width) {
            if (y + ((height + layout->leading) * BREAK_LINES) >
                layout->height) {
                break;
            }
            x = 0.0F;
            y += height + layout->leading;
        }
        width += layout->spacing;
        x += width;
        next++;
    }
    layout->x = x;
    layout->y = y;
    return layout->string - next;
}

void kinder_font_start(void)
{
    Uint32 palette[BANK_COLORS];
    int i;

    load_fonts();
    for (i = 0; i < BANK_COLORS; i++) {
        palette[i] = (i * ALPHA_STEP) | WHITE;
    }
    njSetPaletteMode(NJD_TEXFMT_ARGB_8888);
    njSetPaletteData(FONT_BANK * BANK_COLORS, BANK_COLORS, palette);
    used_cells = 0;
    memset(page_flags, 0, sizeof(page_flags));
    font_task = task_create(0, FONT_LEVEL, font_step, "KbdFontSentinelExec");
}

void kinder_font_stop(void)
{
    int page;
    task_kill(font_task);
    for (page = 0; page < FONT_PAGES; page++) {
        if (page_flags[page] & PAGE_LOADED) {
            njReleaseTexture(&pages[page]);
        }
    }
    free_fonts();
}
