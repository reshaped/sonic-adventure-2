#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"

#define VISIBLE_ROWS 5
#define ROW_HEIGHT 25
#define BORDER 4
/* an open list's top is at most OPEN_BOTTOM less OPEN_ROW_SPAN for each row
 * it shows. the rows themselves draw ROW_HEIGHT apart */
#define OPEN_BOTTOM 440
#define OPEN_ROW_SPAN 26
/* the height of the open list's bottom edge, a pixel less than BORDER */
#define BOTTOM_EDGE 3
/* the dotted line between an open list's rows */
#define DOT_SIZE 3
#define DOT_PITCH 6
/* a row's highlight reaches HIGHLIGHT_OUTSET past the list's sides and
 * top. its caps are HIGHLIGHT_CAP wide, and it is HIGHLIGHT_HEIGHT tall */
#define HIGHLIGHT_OUTSET 1
#define HIGHLIGHT_CAP 4
#define HIGHLIGHT_HEIGHT 30
/* a closed list's face is FACE_HEIGHT tall and a row's text TEXT_HEIGHT.
 * the text lies TEXT_DROP below the row's top */
#define FACE_HEIGHT 28
#define TEXT_HEIGHT 28.0F
#define TEXT_DROP 5
#define FONT_SIZE 18.0F

/* the list's cells in its atlas, in KINDER_ATLAS_UNITS. each is a left cap,
 * a middle and a right cap across. the open list's panel is a top edge and a
 * middle down. the dot between its rows is the panel's first texel. a
 * highlight's middle is the open list's, or a focused closed list's */
#define PANEL_LEFT_U 96
#define PANEL_MIDDLE_U 104
#define PANEL_RIGHT_U 136
#define PANEL_END_U 144
#define PANEL_TOP_V 80
#define PANEL_MIDDLE_V 88
#define PANEL_END_V 128
#define HIGHLIGHT_LEFT_U 192
#define HIGHLIGHT_MIDDLE_U 200
#define HIGHLIGHT_RIGHT_U 232
#define HIGHLIGHT_END_U 240
#define FOCUSED_MIDDLE_U 208
#define FOCUSED_MIDDLE_END_U 224
#define HIGHLIGHT_TOP_V 80
#define HIGHLIGHT_END_V 140
#define FACE_LEFT_U 208
#define FACE_LEFT_END_U 216
#define FACE_MIDDLE_U 224
#define FACE_MIDDLE_END_U 240
#define FACE_RIGHT_U 248
#define FACE_END_U 256
#define FACE_TOP_V 0
#define FACE_END_V 56
#define SELECTED_TEXT 0xffffffff
#define TEXT_COLOR 0xff000000

struct list_item {
    Sint32 key;
    const Uint16 *text;
};

struct kinder_list {
    struct list_item *items;
    Sint32 count;
    Sint32 cursor, saved, top;
    Bool focused, open;
    NJS_TEXLIST *texlist;
};

static Sint32 item_index(struct kinder_widget *widget, Sint32 key)
{
    struct kinder_list *list = widget->state;
    Sint32 i;
    for (i = 0; i < list->count; i++) {
        if (list->items[i].key == key) {
            return i;
        }
    }
    return -1;
}

static void list_destroy(struct kinder_widget *widget)
{
    struct kinder_list *list = widget->state;
    syFree(list->items);
    syFree(list);
    syFree(widget);
}

static void list_tick(struct kinder_widget *widget)
{
}

/* LEFT_CAP sets the whole quad. STRIP_PIECE sets only its x and u. a piece
 * drawn after a left cap keeps the cap's y and v */
#define LEFT_CAP(quad, depth, left, right, top, bottom, u_left, u_right,       \
                 v_top, v_bottom)                                              \
    (quad).x1 = (left);                                                        \
    (quad).x2 = (right);                                                       \
    (quad).y1 = (top);                                                         \
    (quad).y2 = (bottom);                                                      \
    (quad).u1 = KINDER_ATLAS_UV(u_left);                                       \
    (quad).u2 = KINDER_ATLAS_UV(u_right);                                      \
    (quad).v1 = KINDER_ATLAS_UV(v_top);                                        \
    (quad).v2 = KINDER_ATLAS_UV(v_bottom);                                     \
    njDrawQuadTexture(&(quad), depth)

#define STRIP_PIECE(quad, depth, left, right, u_left, u_right)                 \
    (quad).x1 = (left);                                                        \
    (quad).x2 = (right);                                                       \
    (quad).u1 = KINDER_ATLAS_UV(u_left);                                       \
    (quad).u2 = KINDER_ATLAS_UV(u_right);                                      \
    njDrawQuadTexture(&(quad), depth)

/* a row of the open list's panel, across the list x to x + width */
#define PANEL_ROW(quad, depth, x, width, top, bottom, v_top, v_bottom)         \
    LEFT_CAP(quad, depth, x, (x) + BORDER, top, bottom, PANEL_LEFT_U,          \
             PANEL_MIDDLE_U, v_top, v_bottom);                                 \
    STRIP_PIECE(quad, depth, (x) + (width) - BORDER, (x) + (width),            \
                PANEL_RIGHT_U, PANEL_END_U);                                   \
    STRIP_PIECE(quad, depth, (x) + BORDER, (x) + (width) - BORDER,             \
                PANEL_MIDDLE_U, PANEL_RIGHT_U)

/* a highlight from top to bottom, across the list from x to x + width. its
 * middle's u runs from u_left to u_right */
#define HIGHLIGHT_ROW(quad, depth, x, width, top, bottom, u_left, u_right)     \
    LEFT_CAP(quad, depth, (x) - HIGHLIGHT_OUTSET,                              \
             (x) + (HIGHLIGHT_CAP - HIGHLIGHT_OUTSET), top, bottom,            \
             HIGHLIGHT_LEFT_U, HIGHLIGHT_MIDDLE_U, HIGHLIGHT_TOP_V,            \
             HIGHLIGHT_END_V);                                                 \
    STRIP_PIECE(                                                               \
        quad, depth, (x) + (width) - (HIGHLIGHT_CAP - HIGHLIGHT_OUTSET),       \
        (x) + (width) + HIGHLIGHT_OUTSET, HIGHLIGHT_RIGHT_U, HIGHLIGHT_END_U); \
    STRIP_PIECE(quad, depth, (x) + (HIGHLIGHT_CAP - HIGHLIGHT_OUTSET),         \
                (x) + (width) - (HIGHLIGHT_CAP - HIGHLIGHT_OUTSET), u_left,    \
                u_right)

static void list_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                      Sint32 z)
{
    struct kinder_list *list = widget->state;
    Float depth;
    Float dots_depth;
    Sint32 rows;
    Sint32 i;
    Sint32 dot;

    if (!widget->visible) {
        return;
    }
    x += widget->x;
    y += widget->y;
    z += widget->z;
    if (list->open) {
        rows = list->count;
        if (rows > VISIBLE_ROWS) {
            rows = VISIBLE_ROWS;
        }
        if (y > OPEN_BOTTOM - (rows * OPEN_ROW_SPAN)) {
            y = OPEN_BOTTOM - (rows * OPEN_ROW_SPAN);
        }
        depth = KINDER_WIDGET_DEPTH(z + 10);
        njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
        njSetTexture(list->texlist);
        njQuadTextureStart(FALSE);
        njSetQuadTexture(1, 0xffffffff);
        /* @hack spelling: the blocks that scope each quad and text, where
         * one quad and one text declared first in the function would do */
        {
            NJS_QUAD_TEXTURE quad;
            PANEL_ROW(quad, depth, x, widget->width, y, y + BORDER, PANEL_TOP_V,
                      PANEL_MIDDLE_V);
            PANEL_ROW(quad, depth, x, widget->width, y + (rows * ROW_HEIGHT),
                      y + (rows * ROW_HEIGHT) + BOTTOM_EDGE, PANEL_TOP_V,
                      PANEL_MIDDLE_V);
            PANEL_ROW(quad, depth, x, widget->width, y + BORDER,
                      y + (rows * ROW_HEIGHT), PANEL_MIDDLE_V, PANEL_END_V);
            dots_depth = KINDER_WIDGET_DEPTH(z + 11);
            quad.u1 = KINDER_ATLAS_UV(PANEL_LEFT_U);
            quad.u2 = KINDER_ATLAS_UV(PANEL_LEFT_U + 1);
            quad.v1 = KINDER_ATLAS_UV(PANEL_TOP_V);
            quad.v2 = KINDER_ATLAS_UV(PANEL_TOP_V + 1);
            for (i = 1; i < rows; i++) {
                quad.y1 = y + (i * ROW_HEIGHT);
                quad.y2 = y + (i * ROW_HEIGHT) + DOT_SIZE;
                for (dot = BORDER; dot < widget->width - BORDER;
                     dot += DOT_PITCH) {
                    quad.x1 = x + dot;
                    quad.x2 = x + dot + DOT_SIZE;
                    njDrawQuadTexture(&quad, dots_depth);
                }
            }
            depth = KINDER_WIDGET_DEPTH(z + 12);
            HIGHLIGHT_ROW(quad, depth, x, widget->width,
                          ((list->cursor - list->top) * ROW_HEIGHT) + y -
                              HIGHLIGHT_OUTSET,
                          ((list->cursor - list->top) * ROW_HEIGHT) + y +
                              (HIGHLIGHT_HEIGHT - HIGHLIGHT_OUTSET),
                          HIGHLIGHT_MIDDLE_U, HIGHLIGHT_RIGHT_U);
        }
        njQuadTextureEnd();
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);

        for (i = 0; i < rows; i++) {
            struct kinder_text text;
            text.layout.string = list->items[list->top + i].text;
            text.layout.length = KINDER_WHOLE_STRING;
            text.layout.width = widget->width;
            text.layout.height = TEXT_HEIGHT;
            text.layout.spacing = KINDER_TEXT_GAP;
            text.layout.leading = KINDER_TEXT_GAP;
            text.layout.x = 0.0F;
            text.layout.y = 0.0F;
            text.left = x + BORDER;
            text.top = (i * ROW_HEIGHT) + y + TEXT_DROP;
            text.z = KINDER_WIDGET_DEPTH(z + 13);
            if (i == list->cursor - list->top) {
                text.color = SELECTED_TEXT;
            } else {
                text.color = TEXT_COLOR;
            }
            kinder_text_draw_scaled(&text, FONT_SIZE, FONT_SIZE);
        }
    } else {
        NJS_QUAD_TEXTURE quad;
        depth = KINDER_WIDGET_DEPTH(z);
        njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
        njSetTexture(list->texlist);
        njQuadTextureStart(FALSE);
        if (list->focused) {
            njSetQuadTexture(1, 0xffffffff);
            HIGHLIGHT_ROW(quad, depth, x, widget->width, y - HIGHLIGHT_OUTSET,
                          y + (HIGHLIGHT_HEIGHT - HIGHLIGHT_OUTSET),
                          FOCUSED_MIDDLE_U, FOCUSED_MIDDLE_END_U);
        } else {
            njSetQuadTexture(0, 0xffffffff);
            LEFT_CAP(quad, depth, x, x + BORDER, y, y + FACE_HEIGHT,
                     FACE_LEFT_U, FACE_LEFT_END_U, FACE_TOP_V, FACE_END_V);
            STRIP_PIECE(quad, depth, x + widget->width - BORDER,
                        x + widget->width, FACE_RIGHT_U, FACE_END_U);
            STRIP_PIECE(quad, depth, x + BORDER, x + widget->width - BORDER,
                        FACE_MIDDLE_U, FACE_MIDDLE_END_U);
        }
        njQuadTextureEnd();
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    }
    {
        struct kinder_text text;
        text.layout.string = list->items[list->cursor].text;
        text.layout.length = KINDER_WHOLE_STRING;
        text.layout.width = widget->width - (2 * BORDER);
        text.layout.height = TEXT_HEIGHT;
        text.layout.spacing = KINDER_TEXT_GAP;
        text.layout.leading = KINDER_TEXT_GAP;
        text.layout.x = 0.0F;
        text.layout.y = 0.0F;
        text.left = x + BORDER;
        text.top = y + TEXT_DROP;
        text.z = KINDER_WIDGET_DEPTH(z + 1);
        if (!list->open && list->focused) {
            text.color = SELECTED_TEXT;
        } else {
            text.color = TEXT_COLOR;
        }
        kinder_text_draw_scaled(&text, FONT_SIZE, FONT_SIZE);
    }
}

static void reveal_cursor(struct kinder_widget *widget)
{
    struct kinder_list *list = widget->state;
    Sint32 count = list->count;
    Sint32 top = list->top;
    Sint32 cursor = list->cursor;
    if (count <= VISIBLE_ROWS || cursor == 0) {
        top = 0;
    } else if (cursor == count - 1) {
        top = count - VISIBLE_ROWS;
        if (top < 0) {
            top = 0;
        }
    } else {
        if (top < cursor - (VISIBLE_ROWS - 1)) {
            top = cursor - (VISIBLE_ROWS - 1);
        }
        if (top > cursor) {
            top = cursor;
        }
        if (top > count - VISIBLE_ROWS) {
            top = count - VISIBLE_ROWS;
        }
    }
    list->top = top;
}

static Sint32 list_input(struct kinder_widget *widget, Uint32 key)
{
    struct kinder_list *list = widget->state;
    Sint32 used = FALSE;
    if (list->open) {
        switch ((Uint8)key) {
        case KBD_ENTER:
        case KBD_SPACE:
        case WIDGET_PAD_A:
            widget->notify(widget->owner, widget, WIDGET_SELECTED);
            list->open = FALSE;
            break;
        /* @hack redundant: the default that only breaks */
        default:
            break;
        case KBD_ESCAPE:
        case WIDGET_PAD_B:
            list->cursor = list->saved;
            list->open = FALSE;
            break;
        case KBD_TAB:
            if (key & KBD_MODIFIER_MASK) {
                goto previous;
            }
            /* fall through */
        case KBD_DOWN:
        case WIDGET_PAD_DOWN:
            if (++list->cursor >= list->count) {
                list->cursor = 0;
            }
            reveal_cursor(widget);
            break;
        case KBD_UP:
        case WIDGET_PAD_UP:
        previous:
            if (--list->cursor < 0) {
                list->cursor = list->count - 1;
            }
            reveal_cursor(widget);
            break;
        }
        used = TRUE;
    } else {
        switch (key) {
        case KBD_ENTER:
        case KBD_SPACE:
        case WIDGET_PAD_A:
            list->saved = list->cursor;
            list->top = list->cursor;
            list->open = TRUE;
            reveal_cursor(widget);
            used = TRUE;
            break;
        }
    }
    return used || kinder_widget_input(widget, key);
}

static void list_focus(struct kinder_widget *widget, Sint32 selected)
{
    struct kinder_list *list = widget->state;
    list->focused = selected;
}

static const struct kinder_widget_ops list_ops = {
    list_destroy, list_tick, list_draw, list_input, list_focus};

struct kinder_widget *kinder_list_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_list_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct kinder_list *list;
    Sint32 i;

    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((list = syCalloc(1, sizeof(*list))) == NULL) {
        goto free_widget;
    }
    if ((list->items = syCalloc(settings->count, sizeof(*list->items))) ==
        NULL) {
        goto free_list;
    }
    widget->ops = &list_ops;
    widget->state = list;
    list->focused = FALSE;
    list->open = FALSE;
    list->count = settings->count;
    list->cursor = 0;
    list->texlist = settings->texlist;
    if (settings->entries) {
        for (i = 0; i < list->count; i++) {
            list->items[i].key = settings->entries[i].key;
            list->items[i].text = kinder_string(settings->entries[i].message);
            if (settings->selected == settings->entries[i].key) {
                list->cursor = i;
            }
        }
    } else {
        for (i = 0; i < list->count; i++) {
            list->items[i].key = 0;
            list->items[i].text = kinder_string(0);
        }
    }
    return widget;
free_list:
    syFree(list);
free_widget:
    syFree(widget);
failed:
    return NULL;
}

Sint32 kinder_list_key(struct kinder_widget *widget)
{
    struct kinder_list *list = widget->state;
    return list->items[list->cursor].key;
}

void kinder_list_select(struct kinder_widget *widget, Sint32 key)
{
    struct kinder_list *list = widget->state;
    Sint32 index = item_index(widget, key);
    if (index >= 0) {
        list->cursor = index;
    }
}

void kinder_list_set_item(struct kinder_widget *widget, Sint32 index,
                          Sint32 key, const Uint16 *text)
{
    struct kinder_list *list = widget->state;
    struct list_item *item = &list->items[index];
    item->key = key;
    item->text = text;
}

void kinder_list_set_texlist(struct kinder_widget *widget, NJS_TEXLIST *texlist)
{
    struct kinder_list *list = widget->state;
    list->texlist = texlist;
}
