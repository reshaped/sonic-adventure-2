#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_paint.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"

#define CHOICE_ROW_HEIGHT 32
#define CHOICE_TOP 16.0F
#define CHOICE_BOTTOM 464.0F
#define CHOICE_CHANGED 9
/* the closed choice's face, the open list's panel, and its selected row */
#define FACE_COLOR 0xffc0c000
#define LIST_COLOR 0xffc0c0c0
#define SELECTED_COLOR 0xfff00000

struct choice_item {
    Sint32 value;
    const Uint16 *text;
};

struct form_choice {
    Sint32 count;
    struct choice_item *items;
    Sint32 selected, saved;
    Bool open;
};

static Sint32 choice_index(struct kinder_form *form, Sint32 value)
{
    struct form_choice *choice = form->state;
    Sint32 i;
    for (i = 0; i < choice->count; i++) {
        if (choice->items[i].value == value) {
            return i;
        }
    }
    return -1;
}

static void choice_destroy(struct kinder_form *form)
{
    struct form_choice *choice = form->state;
    syFree(choice->items);
    syFree(choice);
    syFree(form);
}

static void choice_tick(struct kinder_form *form)
{
}

static void choice_draw(struct kinder_form *form, Float x, Float y, Float z)
{
    struct form_choice *choice = form->state;
    Float height;
    Float bottom;
    Float text_depth;
    Sint32 i;

    if (!form->visible) {
        return;
    }
    x += form->box.x;
    y += form->box.y;
    z += form->box.z;
    /* @hack spelling: the blocks that scope each strip and run, where one
     * strip and one run declared first in the function would do */
    {
        NJS_POLYGON_VTX strip[4];
        FORM_QUAD(strip, form, x, y, z, FACE_COLOR);
        njDrawPolygon(strip, 4, FALSE);
    }
    {
        struct kinder_text run;
        FORM_TEXT(run, form, choice->items[choice->selected].text, x, y,
                  z + 0.001F);
        kinder_text_draw(&run);
    }
    if (!choice->open) {
        return;
    }
    height = choice->count * CHOICE_ROW_HEIGHT;
    z += 0.01F;
    y += CHOICE_ROW_HEIGHT;
    if (y > CHOICE_BOTTOM - height) {
        y = CHOICE_BOTTOM - height;
        if (y < CHOICE_TOP) {
            y = CHOICE_TOP;
            if (y > (CHOICE_BOTTOM - CHOICE_ROW_HEIGHT) -
                        (choice->selected * CHOICE_ROW_HEIGHT)) {
                y = (CHOICE_BOTTOM - CHOICE_ROW_HEIGHT) -
                    (choice->selected * CHOICE_ROW_HEIGHT);
            }
        }
    }
    {
        NJS_POLYGON_VTX strip[4];
        STRIP_LEFT(strip) = x;
        STRIP_RIGHT(strip) = x + form->box.width;
        STRIP_TOP(strip) = y;
        STRIP_BOTTOM(strip) = bottom = y + height;
        STRIP_DEPTH(strip) = z;
        STRIP_COLOR(strip) = LIST_COLOR;
        njDrawPolygon(strip, 4, FALSE);
        FRAME_EDGES(strip, x, y, x + form->box.width, bottom, z);
    }
    text_depth = z + 0.002F;
    for (i = 0; i < choice->count; i++) {
        struct kinder_text run;
        FORM_TEXT(run, form, choice->items[i].text, x,
                  (i * CHOICE_ROW_HEIGHT) + y, text_depth);
        kinder_text_draw(&run);
    }
    {
        NJS_POLYGON_VTX strip[4];
        Float top = y + (choice->selected * CHOICE_ROW_HEIGHT);
        STRIP_LEFT(strip) = x;
        STRIP_RIGHT(strip) = x + form->box.width;
        STRIP_TOP(strip) = top;
        STRIP_BOTTOM(strip) = top + CHOICE_ROW_HEIGHT;
        STRIP_DEPTH(strip) = z + 0.001F;
        STRIP_COLOR(strip) = SELECTED_COLOR;
        njDrawPolygon(strip, 4, FALSE);
    }
}

static Sint32 choice_input(struct kinder_form *form, Sint32 key)
{
    struct form_choice *choice = form->state;
    Sint32 used = FALSE;

    if (choice->open) {
        switch ((Uint8)key) {
        case KBD_ENTER:
        case KBD_SPACE:
            choice->saved = choice->selected;
            form_panel_notify(form->parent, form, CHOICE_CHANGED);
            /* @hack redundant: the break after the goto, which never runs */
            goto close;
            break;
        case KBD_ESCAPE:
            /* @bug logic: an escape restores the selection of the last
             * commit, or item 0 before any, not the one the list opened on.
             * nothing saves the selection as the list opens, at 0x0c9d23cc,
             * and the escape at 0x0c9d238a restores saved */
            choice->selected = choice->saved;
        close:
            choice->open = FALSE;
            break;
        case KBD_TAB:
            if (key & KBD_MODIFIER_MASK) {
                goto previous_item;
            }
            /* fall through */
        case KBD_DOWN:
            if (++choice->selected >= choice->count) {
                choice->selected = 0;
            }
            break;
        case KBD_UP:
        previous_item:
            if (--choice->selected < 0) {
                choice->selected = choice->count - 1;
            }
            break;
        }
        used = TRUE;
    } else {
        switch (key) {
        case KBD_ENTER:
        case KBD_SPACE:
            choice->open = TRUE;
            used = TRUE;
            break;
        }
    }
    return used;
}

static const struct form_ops choice_ops = {choice_destroy, choice_tick,
                                           choice_draw, choice_input};

struct kinder_form *form_choice_create(struct kinder_form *parent,
                                       const struct form_choice_desc *desc)
{
    struct kinder_form *form;
    struct form_choice *choice;
    Sint32 i;

    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((form = syCalloc(1, sizeof(*form))) == NULL) {
        goto failed;
    }
    if ((choice = syCalloc(1, sizeof(*choice))) == NULL) {
        goto free_form;
    }
    if ((choice->items = syCalloc(desc->count, sizeof(*choice->items))) ==
        NULL) {
        goto free_choice;
    }
    form->ops = &choice_ops;
    form->kind = FORM_CHOICE;
    form->state = choice;
    form->parent = parent;
    FORM_BOUNDS(form, &desc->box);
    form->visible = TRUE;
    choice->open = FALSE;
    choice->count = desc->count;
    choice->selected = 0;
    if (desc->items) {
        for (i = 0; i < choice->count; i++) {
            choice->items[i].value = desc->items[i].value;
            choice->items[i].text = kinder_string(desc->items[i].message);
            if (desc->value == desc->items[i].value) {
                choice->selected = i;
            }
        }
    } else {
        for (i = 0; i < choice->count; i++) {
            choice->items[i].value = -1;
            choice->items[i].text = kinder_string(0);
        }
    }
    return form;

free_choice:
    syFree(choice);
free_form:
    syFree(form);
failed:
    return NULL;
}

Sint32 form_choice_value(struct kinder_form *form)
{
    struct form_choice *choice = form->state;
    return choice->items[choice->selected].value;
}

void form_choice_select(struct kinder_form *form, Sint32 value)
{
    struct form_choice *choice = form->state;
    Sint32 index = choice_index(form, value);
    if (index >= 0) {
        choice->selected = index;
    }
}

void form_choice_set_item(struct kinder_form *form, Sint32 index, Sint32 value,
                          const Uint16 *text)
{
    struct form_choice *choice = form->state;
    struct choice_item *item = &choice->items[index];
    item->value = value;
    item->text = text;
}
