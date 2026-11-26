#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/key_layouts.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"
#include "system/texture.h"

/* u and v count KEY_ATLAS texels across the key atlas, and KEYCAP_ATLAS
 * across the keycap atlas. both atlases hold cells of KEY_SIZE texels */
#define KEY_ATLAS 512.0F
#define KEYCAP_ATLAS 128.0F
#define KEY_UV ((Float)KEY_SIZE / KEY_ATLAS)
#define KEYCAP_UV(cell) ((cell) * ((Float)KEY_SIZE / KEYCAP_ATLAS))
/* the border around the keys is BORDER pixels wide. it draws the keycap
 * atlas's first texel, stretched */
#define BORDER 2.0F
/* the glyphs draw GLYPH_DEPTH in front of their caps */
#define GLYPH_DEPTH 0.001F

/* texlist's two textures: the key atlas, whose palette is KEY_PALETTE_BANK,
 * and the keycap atlas */
#define KEYBOARD_TEXTURES 2
#define KEY_TEXTURE 0
#define KEYCAP_TEXTURE 1
#define KEY_PALETTE_BANK 32

/* the layouts, by their index in layouts: four tabs, then each tab shifted.
 * the first three tabs share one shifted map */
enum keyboard_layout {
    LAYOUT_HIRAGANA,
    LAYOUT_KATAKANA,
    LAYOUT_TAB_TWO,
    LAYOUT_TAB_THREE,
    LAYOUT_SHIFTED_HIRAGANA,
    LAYOUT_SHIFTED_KATAKANA,
    LAYOUT_SHIFTED_TAB_TWO,
    LAYOUT_SHIFTED_TAB_THREE,
    LAYOUTS
};
/* the codes of the keys that change the layout. KEY_TAB + n selects tab n,
 * and KEY_SHIFT shifts the tab or unshifts it. a key of KEY_BLANK does
 * nothing */
#define KEY_TAB 0xff20
#define KEY_SHIFT 0xff24
#define KEY_BLANK 0xff25
/* the codes of the command row's keys, which draw on caps of their own */
#define COMMAND_FIRST 0xff00
#define COMMAND_LAST 0xff0b
/* the cell that holds tab n's code, the last of its key's four on the top
 * row. SHIFT_CELL holds the shift key's */
#define TAB_CELL(n) (((n) * 4) + 3)
#define SHIFT_CELL 79

static NJS_TEXNAME textures[KEYBOARD_TEXTURES];
static NJS_TEXLIST texlist = {textures, KEYBOARD_TEXTURES};
static int layout = LAYOUT_HIRAGANA;
static const struct soft_key *layouts[LAYOUTS] = {
#include "chaostgkinder/soft_keyboard/layouts.inc"
};
static int cursor_x = 0;
static int cursor_y = 0;
static Float left = 100.0F;
static Float top = 60.0F;
static Float depth = 1.5F;

static void load_keyboard(void)
{
    texture_load_pvm(keyboard_atlas, &texlist);
    njSetTexture(&texlist);
    njSetPaletteBankNum(KEY_TEXTURE, KEY_PALETTE_BANK);
}

static const struct soft_key *layout_keys(void)
{
    return layouts[layout];
}

static int key_index(int x, int y)
{
    const struct soft_key *keys = layout_keys();
    int index = x + (y * KEYBOARD_COLUMNS);
    while (!keys[index].code) {
        ++index;
        /* @hack redundant: the continue that ends the loop's body */
        continue; /* NOLINT(readability-redundant-control-flow) */
    }
    return index;
}

static Bool key_highlighted(int index)
{
    Bool selected = FALSE;
    switch (layout) {
    case LAYOUT_HIRAGANA:
        selected = index == TAB_CELL(LAYOUT_HIRAGANA);
        break;
    case LAYOUT_KATAKANA:
        selected = index == TAB_CELL(LAYOUT_KATAKANA);
        break;
    case LAYOUT_TAB_TWO:
        selected = index == TAB_CELL(LAYOUT_TAB_TWO);
        break;
    case LAYOUT_TAB_THREE:
        selected = index == TAB_CELL(LAYOUT_TAB_THREE);
        break;
    case LAYOUT_SHIFTED_TAB_TWO:
        selected = index == TAB_CELL(LAYOUT_TAB_TWO) || index == SHIFT_CELL;
        break;
    }
    return selected;
}

void kinder_keyboard_start(void)
{
    load_keyboard();
}

/* @hack spelling: release_keyboard, where kinder_keyboard_stop's own call
 * to njReleaseTexture would do */
static void release_keyboard(void)
{
    njReleaseTexture(&texlist);
}

void kinder_keyboard_stop(void)
{
    release_keyboard();
}

/* set quad to a key's place. the key is width cells wide, from column x of
 * row y */
/* @hack spelling: KEY_RECT as bare statements, where a do-while block
 * would do */
#define KEY_RECT(quad, x, y, width)                                            \
    (quad).x1 = ((x) * KEY_SIZE) + left;                                       \
    (quad).x2 = (quad).x1 + ((width) * KEY_SIZE);                              \
    (quad).y1 = ((y) * KEY_SIZE) + top;                                        \
    (quad).y2 = (quad).y1 + KEY_SIZE

/* step x and y past a key width cells wide, wrapping to the next row's
 * start. then set width to 1 for the key after it */
/* @hack spelling: NEXT_KEY as bare statements, where a do-while block
 * would do */
#define NEXT_KEY(x, y, width)                                                  \
    if (((x) += (width)) >= KEYBOARD_COLUMNS) {                                \
        (x) = 0;                                                               \
        ++(y);                                                                 \
    }                                                                          \
    (width) = 1

/* texture quad with the keycap atlas's columns column to column_end and rows
 * row to row_end. each end is the cell past the last */
/* @hack spelling: CAP as bare statements, where a do-while block would do */
#define CAP(quad, column, column_end, row, row_end)                            \
    (quad).u1 = KEYCAP_UV(column);                                             \
    (quad).u2 = KEYCAP_UV(column_end);                                         \
    (quad).v1 = KEYCAP_UV(row);                                                \
    (quad).v2 = KEYCAP_UV(row_end)

void kinder_keyboard_draw(void)
{
    NJS_QUAD_TEXTURE quad;
    njSetTexture(&texlist);
    {
        int y = 0;
        int x = 0;
        int width = 1;
        const struct soft_key *key = layout_keys();
        njQuadTextureStart(TRUE);
        njSetQuadTexture(KEY_TEXTURE, 0xffffffff);
        while (y < KEYBOARD_ROWS) {
            if (key->code) {
                int index = key_index(x, y);
                KEY_RECT(quad, x, y, width);
                quad.u1 = key->column * KEY_UV;
                quad.u2 = quad.u1 + (width * KEY_UV);
                quad.v1 = key->row * KEY_UV;
                quad.v2 = quad.v1 + KEY_UV;
                if (index == key_index(cursor_x, cursor_y) ||
                    key_highlighted(index)) {
                    njSetQuadTextureColor(0xff000000);
                    njDrawQuadTexture(&quad, depth + GLYPH_DEPTH);
                    njSetQuadTextureColor(0xffffffff);
                } else {
                    njDrawQuadTexture(&quad, depth + GLYPH_DEPTH);
                }
                NEXT_KEY(x, y, width);
            } else {
                ++width;
            }
            ++key;
        }
        njQuadTextureEnd();
    }
    {
        int y = 0;
        int x = 0;
        int width = 1;
        const struct soft_key *key = layout_keys();
        njQuadTextureStart(FALSE);
        njSetQuadTexture(KEYCAP_TEXTURE, 0xffffffff);
        while (y < KEYBOARD_ROWS) {
            if (key->code) {
                int index = key_index(x, y);
                KEY_RECT(quad, x, y, width);
                if (index == key_index(cursor_x, cursor_y)) {
                    switch (width) {
                    default:
                    case 1: {
                        int code = key->code;
                        if (code >= COMMAND_FIRST && code <= COMMAND_LAST) {
                            CAP(quad, 4, 5, 1, 2);
                        } else {
                            CAP(quad, 4, 5, 3, 4);
                        }
                        break;
                    }
                    case 2:
                        CAP(quad, 0, 2, 4, 5);
                        break;
                    case 4:
                        CAP(quad, 0, 4, 2, 3);
                        break;
                    }
                } else if (key_highlighted(index)) {
                    switch (width) {
                    default:
                    case 2:
                        CAP(quad, 0, 2, 3, 4);
                        break;
                    case 4:
                        CAP(quad, 0, 4, 0, 1);
                        break;
                    }
                } else {
                    switch (width) {
                    case 1:
                    default: {
                        int code = key->code;
                        if (code >= COMMAND_FIRST && code <= COMMAND_LAST) {
                            CAP(quad, 4, 5, 0, 1);
                        } else {
                            CAP(quad, 4, 5, 2, 3);
                        }
                        break;
                    }
                    case 2:
                        CAP(quad, 2, 3, 3, 4);
                        break;
                    case 4:
                        CAP(quad, 0, 4, 1, 2);
                        break;
                    }
                }
                njDrawQuadTexture(&quad, depth);
                NEXT_KEY(x, y, width);
            } else {
                ++width;
            }
            ++key;
        }
    }
    quad.u1 = 0.0F;
    quad.u2 = 1.0F / KEYCAP_ATLAS;
    quad.v1 = 0.0F;
    quad.v2 = 1.0F / KEYCAP_ATLAS;
    quad.x1 = left - BORDER;
    quad.x2 = left + ((KEYBOARD_COLUMNS * KEY_SIZE) + BORDER);
    quad.y1 = top - BORDER;
    quad.y2 = top;
    njDrawQuadTexture(&quad, depth);
    quad.y1 = top + (KEYBOARD_ROWS * KEY_SIZE);
    quad.y2 = top + ((KEYBOARD_ROWS * KEY_SIZE) + BORDER);
    njDrawQuadTexture(&quad, depth);
    quad.x2 = left;
    quad.y1 = top;
    quad.y2 = top + (KEYBOARD_ROWS * KEY_SIZE);
    njDrawQuadTexture(&quad, depth);
    quad.x1 = left + (KEYBOARD_COLUMNS * KEY_SIZE);
    quad.x2 = left + ((KEYBOARD_COLUMNS * KEY_SIZE) + BORDER);
    njDrawQuadTexture(&quad, depth);
    njQuadTextureEnd();
}

void kinder_keyboard_open(void)
{
}

void kinder_keyboard_close(void)
{
}

/* @hack spelling: select_layout, where an assignment to layout would do */
static void select_layout(int next)
{
    layout = next;
}

Uint16 kinder_keyboard_input(Uint16 key)
{
    Uint16 result = 0;
    int y;
    int x;
    int previous;

    /* @hack spelling: the cursor and the layout read in the switch's
     * expression, where the locals' initializers would do */
    switch (y = cursor_y, x = cursor_x, previous = layout, key) {
    case WIDGET_PAD_UP:
        if (--cursor_y < 0) {
            cursor_y = KEYBOARD_ROWS - 1;
        }
        break;
    case WIDGET_PAD_DOWN:
        if (++cursor_y > KEYBOARD_ROWS - 1) {
            cursor_y = 0;
        }
        break;
    case WIDGET_PAD_LEFT: {
        int index = key_index(x, y);
        do {
            if (--cursor_x < 0) {
                cursor_x = KEYBOARD_COLUMNS - 1;
            }
        } while (key_index(cursor_x, cursor_y) == index);
        break;
    }
    case WIDGET_PAD_RIGHT: {
        int index = key_index(x, y);
        do {
            if (++cursor_x > KEYBOARD_COLUMNS - 1) {
                cursor_x = 0;
            }
        } while (key_index(cursor_x, cursor_y) == index);
        break;
    }
    case WIDGET_PAD_A: {
        /* @hack type: keys volatile, where a plain pointer would do */
        const struct soft_key *volatile keys = layouts[previous];
        const struct soft_key *selected = &keys[key_index(x, y)];
        switch (selected->code) {
        default:
            result = selected->code;
            break;
        case KEY_TAB + LAYOUT_HIRAGANA:
            select_layout(LAYOUT_HIRAGANA);
            break;
        case KEY_TAB + LAYOUT_KATAKANA:
            select_layout(LAYOUT_KATAKANA);
            break;
        case KEY_TAB + LAYOUT_TAB_TWO:
            select_layout(LAYOUT_TAB_TWO);
            break;
        case KEY_TAB + LAYOUT_TAB_THREE:
            select_layout(LAYOUT_TAB_THREE);
            break;
        case KEY_SHIFT:
            /* @hack spelling: a case for each layout, where selecting
             * layout ^ LAYOUT_SHIFTED_HIRAGANA, its twin across the shift,
             * would do */
            switch (layout) {
            case LAYOUT_HIRAGANA:
                select_layout(LAYOUT_SHIFTED_HIRAGANA);
                break;
            case LAYOUT_KATAKANA:
                select_layout(LAYOUT_SHIFTED_KATAKANA);
                break;
            case LAYOUT_TAB_TWO:
                select_layout(LAYOUT_SHIFTED_TAB_TWO);
                break;
            case LAYOUT_TAB_THREE:
                select_layout(LAYOUT_SHIFTED_TAB_THREE);
                break;
            case LAYOUT_SHIFTED_HIRAGANA:
                select_layout(LAYOUT_HIRAGANA);
                break;
            case LAYOUT_SHIFTED_KATAKANA:
                select_layout(LAYOUT_KATAKANA);
                break;
            case LAYOUT_SHIFTED_TAB_TWO:
                select_layout(LAYOUT_TAB_TWO);
                break;
            case LAYOUT_SHIFTED_TAB_THREE:
                select_layout(LAYOUT_TAB_THREE);
                break;
            /* @hack redundant: the cases of layouts 8 and 9, which layouts
             * doesn't hold */
            case 8:
                select_layout(9);
                break;
            case 9:
                select_layout(8);
                break;
            }
            break;
        case KEY_BLANK:
            break;
        }
        break;
    }
    case WIDGET_PAD_B:
        result = '\033';
        break;
    case WIDGET_PAD_X:
        result = ' ';
        break;
    case WIDGET_PAD_Y:
        result = '\n';
        break;
    case WIDGET_PAD_L:
        switch (previous) {
        case LAYOUT_HIRAGANA:
            select_layout(LAYOUT_TAB_THREE);
            break;
        case LAYOUT_KATAKANA:
            select_layout(LAYOUT_HIRAGANA);
            break;
        case LAYOUT_TAB_TWO:
            select_layout(LAYOUT_KATAKANA);
            break;
        case LAYOUT_TAB_THREE:
            select_layout(LAYOUT_TAB_TWO);
            break;
        /* @hack spelling: the three cases apart, where one case for the
         * three would do */
        case LAYOUT_SHIFTED_HIRAGANA:
            select_layout(LAYOUT_SHIFTED_TAB_THREE);
            break;
        case LAYOUT_SHIFTED_KATAKANA:
            select_layout(LAYOUT_SHIFTED_TAB_THREE);
            break;
        case LAYOUT_SHIFTED_TAB_TWO:
            select_layout(LAYOUT_SHIFTED_TAB_THREE);
            break;
        case LAYOUT_SHIFTED_TAB_THREE:
            select_layout(LAYOUT_SHIFTED_TAB_TWO);
            break;
        }
        break;
    case WIDGET_PAD_R:
        switch (previous) {
        case LAYOUT_HIRAGANA:
            select_layout(LAYOUT_KATAKANA);
            break;
        case LAYOUT_KATAKANA:
            select_layout(LAYOUT_TAB_TWO);
            break;
        case LAYOUT_TAB_TWO:
            select_layout(LAYOUT_TAB_THREE);
            break;
        case LAYOUT_TAB_THREE:
            select_layout(LAYOUT_HIRAGANA);
            break;
        case LAYOUT_SHIFTED_HIRAGANA:
            select_layout(LAYOUT_HIRAGANA);
            break;
        case LAYOUT_SHIFTED_KATAKANA:
            select_layout(LAYOUT_KATAKANA);
            break;
        case LAYOUT_SHIFTED_TAB_TWO:
            select_layout(LAYOUT_TAB_TWO);
            break;
        case LAYOUT_SHIFTED_TAB_THREE:
            select_layout(LAYOUT_TAB_THREE);
            break;
        }
        break;
    case WIDGET_PAD_START:
        break;
    }
    return result;
}

void kinder_keyboard_get_position(Float *x, Float *y, Float *z)
{
    if (x) {
        *x = left;
    }
    if (y) {
        *y = top;
    }
    if (z) {
        *z = depth;
    }
}
void kinder_keyboard_set_position(Float x, Float y, Float z)
{
    left = x;
    top = y;
    depth = z;
}
