#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/argb_impl.h"
#include "chaostgkinder/composition.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"
#include "draw/text_sprite.h"

#define ROW_HEIGHT (GLYPH_SIZE + KINDER_TEXT_GAP)
#define ENTRY_IDLE 0
#define ENTRY_EDITING 1
/* blink counts frames from the last key, and goes back to BLINK_RESUME at
 * BLINK_WRAP. the cursor shows while blink is below CURSOR_SHOWN */
#define BLINK_WRAP 50
#define BLINK_RESUME 25
#define CURSOR_SHOWN 120
/* kinder_entry_create's colors, in ARGB 4444, and its cursor's width */
#define BLACK 0xf000
#define BLUE 0xf00f
#define RED 0xfc00
#define GREEN 0xf0f0
#define CURSOR_WIDTH 4

struct text_entry {
    Sint32 mode;
    Uint16 *text;
    Sint16 limit, capacity;
    Sint16 display_rows, edit_rows;
    Uint16 text_color, text_background;
    Uint16 kana_color, kana_background;
    Uint16 roman_color, roman_background;
    Uint16 field_background, cursor_color;
    Sint16 unk_20, unk_22, unk_24, unk_26;
    Sint16 cursor_width;
    Sint16 unk_2a, unk_2c;
    Sint16 blink;
};

static void entry_destroy(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    /* @hack spelling: a switch of one case, where an if would do */
    switch (entry->mode) {
    case ENTRY_EDITING:
        kinder_composition_stop();
        break;
    }
    if (entry->text) {
        syFree(entry->text);
    }
    syFree(entry);
    kinder_widget_free(widget);
}

static void entry_tick(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    if (widget->visible) {
        /* @hack spelling: a switch of one case, where an if would do */
        switch (entry->mode) {
        case ENTRY_EDITING:
            if (++entry->blink >= BLINK_WRAP) {
                entry->blink = BLINK_RESUME;
            }
            break;
        }
    }
}

#define TEXT_RUN(run, field, argb)                                             \
    (run).layout.string = (field);                                             \
    (run).color = expand_argb4444((argb));                                     \
    kinder_text_draw(&(run))

void kinder_entry_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z)
{
    struct text_entry *entry = widget->state;
    kinder_widget_draw(widget, x, y, z);
    if (!widget->visible) {
        return;
    }
    x += widget->x;
    y += widget->y;
    z += widget->z;
    switch (entry->mode) {
    case ENTRY_IDLE: {
        struct kinder_text text;
        text.layout.width = widget->width;
        text.layout.height = entry->display_rows * ROW_HEIGHT;
        text.layout.spacing = KINDER_TEXT_GAP;
        text.layout.leading = KINDER_TEXT_GAP;
        text.layout.x = 0.0F;
        text.layout.y = 0.0F;
        text.left = x;
        text.top = y;
        text.z = KINDER_WIDGET_DEPTH(z + 1);
        text.layout.string = entry->text;
        text.layout.length = KINDER_WHOLE_STRING;
        text.color = expand_argb4444(entry->text_color);
        kinder_text_draw(&text);
        break;
    }
    case ENTRY_EDITING: {
        struct kinder_composition *input = kinder_composition();
        {
            struct kinder_text text;
            text.layout.width = widget->width;
            text.layout.height = entry->edit_rows * ROW_HEIGHT;
            text.layout.spacing = KINDER_TEXT_GAP;
            text.layout.leading = KINDER_TEXT_GAP;
            text.layout.x = 0.0F;
            text.layout.y = 0.0F;
            text.left = x;
            text.top = y;
            text.z = KINDER_WIDGET_DEPTH(z + 1);
            text.layout.length = KINDER_WHOLE_STRING;
            TEXT_RUN(text, input->text, entry->text_color);
            TEXT_RUN(text, input->kana, entry->kana_color);
            TEXT_RUN(text, input->roman, entry->roman_color);
            TEXT_RUN(text, input->remaining_kana, entry->kana_color);
            TEXT_RUN(text, input->remaining_text, entry->text_color);
        }
        /* @bug logic: the cursor never blinks. entry_tick, at 0x0c9da9be,
         * sets blink back to BLINK_RESUME at BLINK_WRAP, so blink never
         * reaches the CURSOR_SHOWN this draw tests at 0x0c9da9e4 */
        if (entry->blink < CURSOR_SHOWN) {
            NJS_POLYGON_VTX cursor[4];
            struct kinder_text_layout measure;
            measure.length = KINDER_WHOLE_STRING;
            measure.width = widget->width;
            measure.height = entry->edit_rows * ROW_HEIGHT;
            measure.spacing = KINDER_TEXT_GAP;
            measure.leading = KINDER_TEXT_GAP;
            measure.x = 0.0F;
            measure.y = 0.0F;
            measure.string = input->text;
            kinder_text_measure(&measure);
            measure.string = input->kana;
            kinder_text_measure(&measure);
            measure.string = input->roman;
            kinder_text_measure(&measure);
            cursor[0].x = cursor[1].x = x + measure.x;
            cursor[2].x = cursor[3].x = x + measure.x + entry->cursor_width;
            cursor[0].y = cursor[2].y = y + measure.y;
            cursor[1].y = cursor[3].y = y + measure.y + GLYPH_SIZE;
            /* @bug logic: the cursor's depth is its layer plus 0.002F, where
             * its text's is KINDER_WIDGET_DEPTH of the layer. the sum at
             * 0x0c9dac62 adds 0.002F to z itself */
            cursor[0].z = cursor[1].z = cursor[2].z = cursor[3].z = z + 0.002F;
            cursor[0].col = cursor[1].col = cursor[2].col = cursor[3].col =
                expand_argb4444(entry->cursor_color);
            njDrawPolygon(cursor, 4, FALSE);
        }
        break;
    }
    }
}

Sint32 kinder_entry_input(struct kinder_widget *widget, Uint32 key)
{
    struct text_entry *entry = widget->state;
    Sint32 used = FALSE;
    key &= KBD_USAGE_MASK;
    switch (entry->mode) {
    case ENTRY_IDLE:
        switch (key) {
        case KBD_ENTER:
            kinder_entry_begin(widget);
            kinder_composition_start();
            kinder_composition_set_limit(entry->capacity);
            kinder_composition_set_text(entry->text);
            /* @hack spelling: the goto to the pad's tail, where setting used
             * and breaking here would do */
            goto handled;
        case WIDGET_PAD_A:
            kinder_entry_begin(widget);
            kinder_composition_start();
            kinder_composition_set_text(entry->text);
            kinder_composition_use_pad();
        handled:
            used = TRUE;
            break;
        /* @hack redundant: the default that only breaks */
        default:
            break;
        }
        break;
    case ENTRY_EDITING: {
        struct kinder_composition *input = kinder_composition();
        kinder_composition_input(key);
        /* @hack redundant: the read of result that nothing uses */
        (void)input->result;
        switch (input->result) {
        case COMPOSE_ACCEPTED:
            kinder_entry_set_text(widget, input->text);
            /* fall through */
        case COMPOSE_CANCELED:
            kinder_entry_end(widget);
            break;
        }
        used = TRUE;
        break;
    }
    }
    entry->blink = 0;
    return used;
}

static const struct kinder_widget_ops entry_ops = {
    entry_destroy, entry_tick, kinder_entry_draw, kinder_entry_input, NULL};

struct kinder_widget *kinder_entry_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_entry_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct text_entry *entry;
    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((entry = syCalloc(1, sizeof(*entry))) == NULL) {
        goto free_widget;
    }
    widget->state = entry;
    widget->ops = &entry_ops;
    entry->mode = ENTRY_IDLE;
    entry->text_color = BLACK;
    entry->text_background = 0;
    entry->kana_color = BLUE;
    entry->kana_background = BLACK;
    entry->roman_color = RED;
    entry->roman_background = BLACK;
    entry->field_background = 0;
    entry->cursor_color = GREEN;
    entry->unk_24 = 2;
    entry->unk_26 = 2;
    entry->cursor_width = CURSOR_WIDTH;
    entry->limit = settings->capacity;
    entry->capacity = settings->capacity;
    entry->text = syCalloc(settings->capacity + 1, sizeof(*entry->text));
    entry->display_rows = settings->rows;
    entry->edit_rows = settings->rows;
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

void kinder_entry_set_text(struct kinder_widget *widget, const Uint16 *text)
{
    struct text_entry *entry = widget->state;
    kinder_wide_terminate(entry->text, text, entry->limit);
}

void kinder_entry_set_limit(struct kinder_widget *widget, Sint32 limit)
{
    struct text_entry *entry = widget->state;
    if (limit <= entry->capacity) {
        entry->limit = limit;
    }
}

void kinder_entry_resize(struct kinder_widget *widget, Sint32 capacity)
{
    struct text_entry *entry = widget->state;
    /* @bug memory: kinder_entry_set_text, at 0x0c9dae3a, can write past a
     * buffer this shrinks at 0x0c9dae5a. limit keeps its old count */
    entry->capacity = capacity;
    syFree(entry->text);
    entry->text = syCalloc(capacity + 1, sizeof(*entry->text));
}

void kinder_entry_set_rows(struct kinder_widget *widget, Sint32 rows)
{
    struct text_entry *entry = widget->state;
    if (rows <= entry->edit_rows) {
        entry->display_rows = rows;
    }
}

void kinder_entry_set_color(struct kinder_widget *widget, Uint16 color,
                            Uint32 mask)
{
    struct text_entry *entry = widget->state;
    if (mask & ENTRY_TEXT_COLOR) {
        entry->text_color = color;
    }
    if (mask & ENTRY_TEXT_BACKGROUND) {
        entry->text_background = color;
    }
    if (mask & ENTRY_KANA_COLOR) {
        entry->kana_color = color;
    }
    if (mask & ENTRY_KANA_BACKGROUND) {
        entry->kana_background = color;
    }
    if (mask & ENTRY_ROMAN_COLOR) {
        entry->roman_color = color;
    }
    if (mask & ENTRY_ROMAN_BACKGROUND) {
        entry->roman_background = color;
    }
    if (mask & ENTRY_FIELD_BACKGROUND) {
        entry->field_background = color;
    }
}

void kinder_entry_end(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    entry->mode = ENTRY_IDLE;
}

void kinder_entry_begin(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    entry->mode = ENTRY_EDITING;
}

Sint32 kinder_entry_height(struct kinder_widget *widget)
{
    return widget->height;
}

Sint32 fun_0c9daf46(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    return entry->unk_2a - entry->unk_24 + entry->unk_22;
}

Sint32 fun_0c9daf5e(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    return entry->unk_2c + entry->unk_22 + GLYPH_SIZE;
}

Sint32 kinder_entry_editing(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    return entry->mode;
}

Uint16 *kinder_entry_text(struct kinder_widget *widget)
{
    struct text_entry *entry = widget->state;
    return entry->text;
}
