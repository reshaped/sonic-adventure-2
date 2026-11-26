#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/argb_impl.h"
#include "chaostgkinder/form_field/text.inc"
#include "chaostgkinder/field_keys.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_paint.h"
#include "chaostgkinder/kana.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"
#include "draw/text_sprite.h"

#define ROW_HEIGHT (GLYPH_SIZE + KINDER_TEXT_GAP)
#define FIELD_IDLE 0
#define FIELD_EDITING 1
/* no code sets FIELD_UNK_2. a field in it draws and blinks its cursor as it
 * does while editing, but field_input uses no key in it. restore_field puts
 * it back to FIELD_EDITING */
#define FIELD_UNK_2 2
/* blink counts frames from the last key, and goes back to BLINK_RESUME at
 * BLINK_WRAP. the cursor shows while blink is below CURSOR_SHOWN */
#define BLINK_WRAP 140
#define BLINK_RESUME 80
#define CURSOR_SHOWN 120
#define KATAKANA_N 0x8393
#define KATAKANA_SMALL_TSU 0x8362
#define KANA_OFF 0
#define KANA_ON 2
/* form_field_create's colors, in ARGB 4444, and its cursor's width */
#define BLACK 0xf000
#define BLUE 0xf00f
#define RED 0xfc00
#define GREEN 0xf0f0
#define CURSOR_WIDTH 4
/* a field's face, and the edge EDGE_WIDTH wide around it */
#define FACE_COLOR 0xffffffff
#define EDGE_COLOR 0xff000000
#define EDGE_WIDTH 1.0F

/* the input mode, which every field shares. under KANA_ON, typing the
 * romaji of a syllable field_syllables lists turns them into the syllable's
 * kana */
static Sint32 kana_mode = KANA_OFF;

/* text from compose_start up to compose_end is the composition: kana typed
 * under KANA_ON, which ENTER or TAB commits. it draws in kana_color, but from
 * the cursor up to roman_end in roman_color */
struct text_field {
    Sint32 mode;
    Uint16 *initial, *text;
    Sint16 limit, capacity;
    Sint16 display_rows, edit_rows;
    Uint16 text_color, text_background;
    Uint16 kana_color, kana_background;
    Uint16 roman_color, roman_background;
    Uint16 field_background, cursor_color;
    Sint16 unk_24, unk_26, unk_28, unk_2a;
    Sint16 cursor_width;
    Sint16 compose_start, compose_end, cursor, roman_end;
    Sint16 unk_36, unk_38;
    Sint16 blink;
};

static void restore_field(struct kinder_form *form)
{
    struct text_field *field = form->state;
    memset(field->text, 0, (field->capacity + 1) * sizeof(*field->text));
    if (field->initial) {
        kinder_wide_copy_n(field->text, field->initial, field->limit);
    }
    switch (field->mode) {
    case FIELD_UNK_2:
    case FIELD_EDITING:
        form_field_begin(form);
        break;
    }
}

static int insert_character(struct kinder_form *form, Uint16 code)
{
    struct text_field *field = form->state;
    Bool inserted = FALSE;
    Sint32 length = kinder_wide_length(field->text);
    Sint32 i;
    if (length < field->limit) {
        for (i = length; i >= field->cursor; i--) {
            /* @hack spelling: the local pointer at, where assigning text[i]
             * to text[i + 1] would do */
            Uint16 *at = &field->text[i];
            at[1] = at[0];
        }
        inserted = TRUE;
        field->text[field->cursor++] = code;
        field->compose_end++;
        field->roman_end++;
    }
    return inserted;
}

/* replace the count characters at index with kana, whose characters all
 * take two bytes. the text after them moves with its terminating 0 */
static void replace_spelling(struct kinder_form *form, Sint32 index,
                             Sint32 count, const char *kana)
{
    struct text_field *field = form->state;
    Sint32 length = (Sint32)strlen(kana) / 2;
    Uint16 *src = field->text + index + count;
    Uint16 *dst = field->text + index + length;
    Uint16 *start;
    Uint16 *out;
    int high;
    if (length < count) {
        while ((*dst++ = *src++) != 0) {
        }
    } else if (length > count) {
        start = src;
        do {
            dst++;
        } while (*src++);
        do {
            *--dst = *--src;
        } while (src != start);
    }
    out = field->text + index;
    while ((high = (Uint8)*kana++) != 0) {
        *out++ = (high << 8) | (Uint8)*kana++;
    }
}

static void trim_field(struct kinder_form *form, Sint32 length)
{
    struct text_field *field = form->state;
    if (field->capacity >= length) {
        field->text[length] = 0;
    }
    if (field->compose_end > length) {
        field->compose_end = length;
    }
    if (field->roman_end > field->compose_end) {
        field->roman_end = field->compose_end;
    }
    if (field->cursor > field->roman_end) {
        field->cursor = field->roman_end;
    }
    if (field->compose_start > field->cursor) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it sets
         * compose_start */
        field->compose_start = field->cursor;
#else
        /* @bug logic: compose_start stays past the cursor. the store at
         * 0x0c9d0ef0 sets roman_end to the cursor, where compose_start was
         * meant */
        field->roman_end = field->cursor;
#endif
    }
}

static void field_destroy(struct kinder_form *form)
{
    struct text_field *field = form->state;
    if (field) {
        if (field->initial) {
            syFree(field->initial);
        }
        if (field->text) {
            syFree(field->text);
        }
        syFree(field);
    }
    syFree(form);
}

#define LAYOUT(layout, form, rows)                                             \
    (layout).width = (form)->box.width;                                        \
    (layout).height = (rows) * ROW_HEIGHT;                                     \
    (layout).spacing = KINDER_TEXT_GAP;                                        \
    (layout).leading = KINDER_TEXT_GAP;                                        \
    (layout).x = 0.0F;                                                         \
    (layout).y = 0.0F

static void field_tick(struct kinder_form *form)
{
    struct text_field *field = form->state;
    struct kinder_text_layout layout;
    Float height;
    if (form->visible) {
        layout.string = field->text;
        layout.length = KINDER_WHOLE_STRING;
        LAYOUT(layout, form, field->edit_rows);
        kinder_text_measure(&layout);
        height = layout.y + ROW_HEIGHT;
        if (form->box.height != height) {
            form->box.height = height;
            form_panel_notify(form->parent, form, WIDGET_TICK);
        }
        switch (field->mode) {
        case FIELD_EDITING:
        case FIELD_UNK_2:
            if (++field->blink >= BLINK_WRAP) {
                field->blink = BLINK_RESUME;
            }
            break;
        }
    }
}

#define TEXT_ORIGIN(run, form, rows, left_value, top_value, depth)             \
    LAYOUT((run).layout, form, rows);                                          \
    (run).left = (left_value);                                                 \
    (run).top = (top_value);                                                   \
    (run).z = (depth)

#define DISPLAY_SPAN(run, text_value, first, count, argb)                      \
    (run).layout.string = (text_value) + (first);                              \
    (run).layout.length = (count);                                             \
    (run).color = expand_argb4444(argb)

static void field_draw(struct kinder_form *form, Float x, Float y, Float z)
{
    struct text_field *field = form->state;
    Sint32 drawn;
    if (!form->visible) {
        return;
    }
    x += form->box.x;
    y += form->box.y;
    z += form->box.z;
    /* @hack redundant: the switch on mode, whose cases are every mode a
     * field takes */
    switch (field->mode) {
    case FIELD_IDLE:
    case FIELD_EDITING:
    case FIELD_UNK_2: {
        NJS_POLYGON_VTX strip[4];
        FORM_QUAD(strip, form, x, y, z, EDGE_COLOR);
        njDrawPolygon(strip, 4, FALSE);
        STRIP_LEFT(strip) = x + EDGE_WIDTH;
        STRIP_RIGHT(strip) = x + form->box.width - EDGE_WIDTH;
        STRIP_TOP(strip) = y + EDGE_WIDTH;
        STRIP_BOTTOM(strip) = y + form->box.height - EDGE_WIDTH;
        STRIP_DEPTH(strip) = z + 0.0001F;
        STRIP_COLOR(strip) = FACE_COLOR;
        njDrawPolygon(strip, 4, FALSE);
        break;
    }
    }
    switch (field->mode) {
    case FIELD_IDLE: {
        struct kinder_text run;
        TEXT_ORIGIN(run, form, field->display_rows, x, y, z + 0.001F);
        DISPLAY_SPAN(run, field->text, 0, KINDER_WHOLE_STRING,
                     field->text_color);
        drawn = kinder_text_draw(&run);
    }
        trim_field(form, drawn);
        break;
    case FIELD_EDITING:
    case FIELD_UNK_2: {
        struct kinder_text run;
        TEXT_ORIGIN(run, form, field->edit_rows, x, y, z + 0.001F);
        DISPLAY_SPAN(run, field->text, 0, field->compose_start,
                     field->text_color);
        drawn = kinder_text_draw(&run);
        DISPLAY_SPAN(run, field->text, drawn, field->cursor - drawn,
                     field->kana_color);
        drawn += kinder_text_draw(&run);
        DISPLAY_SPAN(run, field->text, drawn, field->roman_end - drawn,
                     field->roman_color);
        drawn += kinder_text_draw(&run);
        DISPLAY_SPAN(run, field->text, drawn,
                     field->compose_end - field->roman_end, field->kana_color);
        drawn += kinder_text_draw(&run);
        DISPLAY_SPAN(run, field->text, drawn, KINDER_WHOLE_STRING,
                     field->text_color);
        drawn += kinder_text_draw(&run);
    }
        trim_field(form, drawn);
        if (field->blink < CURSOR_SHOWN) {
            NJS_POLYGON_VTX strip[4];
            struct kinder_text_layout layout;
            layout.string = field->text;
            layout.length = field->cursor;
            LAYOUT(layout, form, field->edit_rows);
            kinder_text_measure(&layout);
            STRIP_LEFT(strip) = x + layout.x;
            STRIP_RIGHT(strip) = x + layout.x + field->cursor_width;
            STRIP_TOP(strip) = y + layout.y;
            STRIP_BOTTOM(strip) = y + layout.y + GLYPH_SIZE;
            STRIP_DEPTH(strip) = z + 0.002F;
            STRIP_COLOR(strip) = expand_argb4444(field->cursor_color);
            njDrawPolygon(strip, 4, FALSE);
        }
        break;
    }
}

#define READ_CHARACTER(code, key, modifiers)                                   \
    (code) = 0;                                                                \
    if ((modifiers) & KBD_SHIFT_KEYS) {                                        \
        if ((unsigned)(key) < sizeof(shifted_field_keys)) {                    \
            (code) = shifted_field_keys[key];                                  \
        }                                                                      \
    } else if ((unsigned)(key) < sizeof(field_keys)) {                         \
        (code) = field_keys[key];                                              \
    }

#define BACKSPACE(field)                                                       \
    kinder_wide_copy((field)->text + (field)->cursor - 1,                      \
                     (field)->text + (field)->cursor)
#define DELETE_CHARACTER(field)                                                \
    kinder_wide_copy((field)->text + (field)->cursor,                          \
                     (field)->text + (field)->cursor + 1)

static Sint32 field_input(struct kinder_form *form, Sint32 key)
{
    struct text_field *field = form->state;
    Sint32 used = FALSE;
    Uint8 modifiers = key >> 8;
    int code;
    const struct roman_syllable *syllable;
    const char *spelling;
    const char *kana;
    Sint32 length;
    Sint32 i;
    Sint32 added;
    Uint16 *preceding;
    Uint16 *dst;
    Uint16 *src;

    key &= KBD_USAGE_MASK;
    switch (field->mode) {
    case FIELD_IDLE:
        /* @hack redundant: the cases of the usages that report no key, which
         * only break */
        switch (key) {
        case KBD_NO_EVENT:
        case KBD_ERROR_ROLLOVER:
        case KBD_POST_FAIL:
        case KBD_ERROR_UNDEFINED:
            break;
        case KBD_ENTER:
        case KBD_SPACE:
            form_field_begin(form);
            used = TRUE;
            break;
        }
        break;
    case FIELD_EDITING:
        /* @hack redundant: case KANA_OFF, which default already takes */
        switch (kana_mode) {
        case KANA_OFF:
        default:
            /* @hack redundant: the cases of the usages that report no key,
             * which default takes alike while field_keys and
             * shifted_field_keys give them 0 */
            switch (key) {
            case KBD_NO_EVENT:
            case KBD_ERROR_ROLLOVER:
            case KBD_POST_FAIL:
            case KBD_ERROR_UNDEFINED:
                break;
            case KBD_ENTER:
            case KBD_TAB:
                form_field_set_text(form, field->text);
                form_field_end(form);
                break;
            case KBD_ESCAPE:
                restore_field(form);
                form_field_end(form);
                break;
            case KBD_BACKSPACE:
                if (field->cursor > 0) {
                    BACKSPACE(field);
                    field->cursor--;
                }
                break;
            case KBD_ZENKAKU_HANKAKU:
                kana_mode = KANA_ON;
                break;
            case KBD_DELETE:
                if (field->cursor < kinder_wide_length(field->text)) {
                    DELETE_CHARACTER(field);
                }
                break;
            case KBD_RIGHT:
                if (field->cursor < kinder_wide_length(field->text)) {
                    field->cursor++;
                }
                break;
            case KBD_LEFT:
                if (field->cursor > 0) {
                    field->cursor--;
                }
                break;
            case KBD_DOWN:
            case KBD_UP:
                break;
            default:
                READ_CHARACTER(code, key, modifiers);
                if (code) {
                    /* @hack redundant: the if around insert_character, whose
                     * body is empty */
                    if (insert_character(form, code)) {
                    }
                }
                break;
            }
            field->compose_start = field->compose_end = field->roman_end =
                field->cursor;
            used = TRUE;
            break;
        case KANA_ON:
            /* @hack redundant: the cases of the usages that report no key,
             * which default takes alike while field_keys and
             * shifted_field_keys give them 0 */
            switch (key) {
            case KBD_NO_EVENT:
            case KBD_ERROR_ROLLOVER:
            case KBD_POST_FAIL:
            case KBD_ERROR_UNDEFINED:
                break;
            case KBD_ENTER:
            case KBD_TAB:
                if (field->compose_start < field->compose_end) {
                    field->compose_start = field->compose_end;
                } else {
                    form_field_set_text(form, field->text);
                    form_field_end(form);
                }
                break;
            case KBD_ESCAPE:
                if (field->compose_start < field->compose_end) {
                    dst = &field->text[field->compose_start];
                    src = &field->text[field->compose_end];
                    while ((*dst++ = *src++) != 0) {
                    }
                    field->compose_end = field->compose_start;
                } else {
                    restore_field(form);
                    form_field_end(form);
                }
                break;
            case KBD_BACKSPACE:
                if (field->compose_start < field->compose_end) {
                    if (field->cursor > field->compose_start) {
                        BACKSPACE(field);
                        field->cursor--;
                        field->compose_end--;
                    }
                } else if (field->cursor > 0) {
                    BACKSPACE(field);
                    field->compose_start = field->compose_end = --field->cursor;
                }
                break;
            case KBD_ZENKAKU_HANKAKU:
                kana_mode = KANA_OFF;
                break;
            case KBD_DELETE:
                if (field->compose_start < field->compose_end) {
                    if (field->cursor < field->compose_end) {
                        DELETE_CHARACTER(field);
                        field->compose_end--;
                    }
                } else if (field->cursor < kinder_wide_length(field->text)) {
                    DELETE_CHARACTER(field);
                }
                break;
            case KBD_RIGHT:
                if (field->compose_start < field->compose_end) {
                    if (field->cursor < field->compose_end) {
                        field->cursor++;
                    }
                } else if (field->cursor < kinder_wide_length(field->text)) {
                    field->cursor++;
                }
                break;
            case KBD_LEFT:
                if (field->compose_start < field->compose_end) {
                    if (field->cursor > field->compose_start) {
                        field->cursor--;
                    }
                } else if (field->cursor > 0) {
                    field->cursor--;
                }
                break;
            case KBD_DOWN:
            case KBD_UP:
                break;
            default:
                READ_CHARACTER(code, key, modifiers);
                /* @hack spelling: a break for each test, where one test of
                 * both would do */
                if (!code) {
                    break;
                }
                if (!insert_character(form, code)) {
                    break;
                }
                for (syllable = field_syllables; syllable->roman; syllable++) {
                    spelling = syllable->roman;
                    kana = syllable->katakana;
                    length = strlen(spelling);
                    if (!kana) {
                        kana = syllable->hiragana;
                    }
                    if (field->cursor - length < field->compose_start) {
                        continue;
                    }
                    for (i = 0; i < length; i++) {
                        if (field->text[field->cursor - length + i] !=
                            spelling[i]) {
                            goto next_syllable;
                        }
                    }
                    if (field->cursor - length > field->compose_start) {
                        preceding = &field->text[field->cursor - length - 1];
                        if (*preceding == 'n') {
                            *preceding = KATAKANA_N;
                        } else if (*preceding == spelling[0]) {
                            *preceding = KATAKANA_SMALL_TSU;
                        }
                    }
                    replace_spelling(form, field->cursor - length, length,
                                     kana);
                    added = ((Sint32)strlen(kana) / 2) - length;
                    field->compose_end += added;
                    field->cursor += added;
                    field->roman_end = field->cursor;
                    break;
                next_syllable:;
                }
                break;
            }
            field->roman_end = field->cursor;
            used = TRUE;
            break;
        }
        break;
    case FIELD_UNK_2:
        /* @hack redundant: the switch on kana_mode, whose one case does
         * nothing */
        switch (kana_mode) {
        case KANA_OFF:
            break;
        }
        break;
    }
    field->blink = 0;
    return used;
}

static const struct form_ops field_ops = {field_destroy, field_tick, field_draw,
                                          field_input};

struct kinder_form *form_field_create(struct kinder_form *parent,
                                      const struct form_field_desc *desc)
{
    struct kinder_form *form;
    struct text_field *field;

    /* @hack spelling: each assignment inside its test, where a statement of
     * its own would do */
    if ((form = syCalloc(1, sizeof(*form))) != NULL) {
        if ((field = syCalloc(1, sizeof(*field))) != NULL) {
            FORM_BOUNDS(form, &desc->box);
            form->parent = parent;
            form->kind = FORM_FIELD;
            form->visible = TRUE;
            form->state = field;
            form->ops = &field_ops;
            field->mode = FIELD_IDLE;
            field->text_color = BLACK;
            field->text_background = 0;
            field->kana_color = BLUE;
            field->kana_background = BLACK;
            field->roman_color = RED;
            field->roman_background = BLACK;
            field->field_background = 0;
            field->cursor_color = GREEN;
            field->unk_24 = 2;
            field->unk_26 = 2;
            field->unk_28 = 2;
            field->unk_2a = 2;
            field->cursor_width = CURSOR_WIDTH;
            field->limit = desc->capacity;
            field->capacity = desc->capacity;
            field->initial = NULL;
            field->text = syCalloc(desc->capacity + 1, sizeof(*field->text));
            field->display_rows = desc->rows;
            field->edit_rows = desc->rows;
            return form;
        }
        syFree(form);
    }
    return NULL;
}

void form_field_set_text(struct kinder_form *form, const Uint16 *text)
{
    struct text_field *field = form->state;
    if (field->initial) {
        syFree(field->initial);
    }
    field->initial = syCalloc(kinder_wide_length(text) + 1, sizeof(*text));
    kinder_wide_copy(field->initial, text);
    restore_field(form);
}

void form_field_set_limit(struct kinder_form *form, Sint32 limit)
{
    struct text_field *field = form->state;
    if (limit <= field->capacity) {
        field->limit = limit;
    }
}

void form_field_resize(struct kinder_form *form, Sint32 capacity)
{
    struct text_field *field = form->state;
    field->capacity = capacity;
    syFree(field->text);
    field->text = syCalloc(capacity + 1, sizeof(*field->text));
}

void form_field_set_rows(struct kinder_form *form, Sint32 rows)
{
    struct text_field *field = form->state;
    if (rows <= field->edit_rows) {
        field->display_rows = rows;
    }
}

void form_field_set_color(struct kinder_form *form, Uint16 color, Uint32 mask)
{
    struct text_field *field = form->state;
    if (mask & FIELD_TEXT_COLOR) {
        field->text_color = color;
    }
    if (mask & FIELD_TEXT_BACKGROUND) {
        field->text_background = color;
    }
    if (mask & FIELD_KANA_COLOR) {
        field->kana_color = color;
    }
    if (mask & FIELD_KANA_BACKGROUND) {
        field->kana_background = color;
    }
    if (mask & FIELD_ROMAN_COLOR) {
        field->roman_color = color;
    }
    if (mask & FIELD_ROMAN_BACKGROUND) {
        field->roman_background = color;
    }
    if (mask & FIELD_BACKGROUND) {
        field->field_background = color;
    }
}

void form_field_end(struct kinder_form *form)
{
    struct text_field *field = form->state;
    field->mode = FIELD_IDLE;
}

void form_field_begin(struct kinder_form *form)
{
    struct text_field *field = form->state;
    field->compose_start = field->compose_end = field->cursor =
        field->roman_end = kinder_wide_length(field->text);
    field->mode = FIELD_EDITING;
}

Float form_field_height(struct kinder_form *form)
{
    return form->box.height;
}

Sint32 fun_0c9d1b90(struct kinder_form *form)
{
    struct text_field *field = form->state;
    return field->unk_36 - field->unk_28 + field->unk_26;
}

Sint32 fun_0c9d1ba8(struct kinder_form *form)
{
    struct text_field *field = form->state;
    return field->unk_38 + field->unk_26 + GLYPH_SIZE;
}

Sint32 form_field_mode(struct kinder_form *form)
{
    struct text_field *field = form->state;
    return field->mode;
}

Uint16 *form_field_text(struct kinder_form *form)
{
    struct text_field *field = form->state;
    return field->text;
}
