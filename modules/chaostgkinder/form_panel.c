#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_paint.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"

/* the focus frame lies from FOCUS_INNER to FOCUS_OUTER pixels outside the
 * focused child. the tick scrolls the whole frame into view */
#define FOCUS_INNER 2.0F
#define FOCUS_OUTER 4.0F
#define FACE_COLOR 0xffc0c0c0
#define FOCUS_COLOR 0xffff0000
/* the fewest children a panel has room for */
#define MIN_CAPACITY 8

static void form_panel_destroy(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    Sint32 i;
    for (i = 0; i < panel->count; i++) {
        struct kinder_form *child = panel->children[i];
        child->ops->destroy(child);
    }
    syFree(panel->children);
    syFree(panel);
    syFree(form);
}

static void form_panel_tick(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    Sint32 i;
    struct kinder_form *child;

    for (i = 0; i < panel->count; i++) {
        child = panel->children[i];
        child->ops->tick(child);
    }
    child = panel->children[panel->focus];
    if (panel->scroll_x > child->box.x - FOCUS_OUTER) {
        panel->scroll_x = child->box.x - FOCUS_OUTER;
    }
    if (panel->scroll_x <
        child->box.x + child->box.width - form->box.width + FOCUS_OUTER) {
        panel->scroll_x =
            child->box.x + child->box.width - form->box.width + FOCUS_OUTER;
    }
    if (panel->scroll_y > child->box.y - FOCUS_OUTER) {
        panel->scroll_y = child->box.y - FOCUS_OUTER;
    }
    if (panel->scroll_y <
        child->box.y + child->box.height - form->box.height + FOCUS_OUTER) {
        panel->scroll_y =
            child->box.y + child->box.height - form->box.height + FOCUS_OUTER;
    }
}

static void form_panel_draw(struct kinder_form *form, Float x, Float y, Float z)
{
    struct form_panel *panel = form->state;
    Sint32 i;
    struct kinder_form *child;

    x += form->box.x;
    y += form->box.y;
    z += form->box.z;
    {
        NJS_POLYGON_VTX strip[4];
        STRIP_DEPTH(strip) = z;
        STRIP_COLOR(strip) = FACE_COLOR;
        STRIP_LEFT(strip) = x;
        STRIP_RIGHT(strip) = x + form->box.width;
        STRIP_TOP(strip) = y;
        STRIP_BOTTOM(strip) = y + form->box.height;
        njDrawPolygon(strip, 4, FALSE);
        FRAME_EDGES(strip, x, y, x + form->box.width, y + form->box.height, z);
    }
    x -= panel->scroll_x;
    y -= panel->scroll_y;
    for (i = 0; i < panel->count; i++) {
        child = panel->children[i];
        child->ops->draw(child, x, y, z);
    }
    if (panel->show_focus) {
        NJS_POLYGON_VTX strip[4];
        Float left;
        Float right;
        Float top;
        Float bottom;
        Float outer_right;
        Float inner_top;
        Float inner_bottom;
        child = panel->children[panel->focus];
        left = x + child->box.x;
        right = left + child->box.width;
        top = y + child->box.y;
        bottom = top + child->box.height;
        STRIP_COLOR(strip) = FOCUS_COLOR;
        STRIP_DEPTH(strip) = z + 0.01F;
        STRIP_LEFT(strip) = left - FOCUS_OUTER;
        STRIP_RIGHT(strip) = outer_right = right + FOCUS_OUTER;
        STRIP_TOP(strip) = top - FOCUS_OUTER;
        STRIP_BOTTOM(strip) = inner_top = top - FOCUS_INNER;
        njDrawPolygon(strip, 4, FALSE);
        STRIP_TOP(strip) = inner_bottom = bottom + FOCUS_INNER;
        STRIP_BOTTOM(strip) = bottom + FOCUS_OUTER;
        njDrawPolygon(strip, 4, FALSE);
        STRIP_RIGHT(strip) = left - FOCUS_INNER;
        STRIP_TOP(strip) = inner_top;
        STRIP_BOTTOM(strip) = inner_bottom;
        njDrawPolygon(strip, 4, FALSE);
        STRIP_LEFT(strip) = right + FOCUS_INNER;
        STRIP_RIGHT(strip) = outer_right;
        njDrawPolygon(strip, 4, FALSE);
    }
}

static Sint32 panel_input(struct kinder_form *form, Sint32 key)
{
    struct form_panel *panel = form->state;
    struct kinder_form *child = panel->children[panel->focus];
    Sint32 event;
    if (!child->ops->input(child, key)) {
        /* @hack spelling: the default first, where last would do */
        switch (key) {
        default:
            return TRUE;
        case KBD_ENTER:
        case KBD_SPACE:
            event = WIDGET_ACCEPT;
            break;
        case KBD_ESCAPE:
            event = WIDGET_CANCEL;
            break;
        case KBD_TAB:
            event = WIDGET_TAB;
            break;
        case KBD_RIGHT:
            event = WIDGET_RIGHT;
            break;
        case KBD_LEFT:
            event = WIDGET_LEFT;
            break;
        case KBD_DOWN:
            event = WIDGET_DOWN;
            break;
        case KBD_UP:
            event = WIDGET_UP;
        }
        panel->notify(panel->context, form, event, child->box.id);
    }
    return TRUE;
}

static const struct form_ops panel_ops = {form_panel_destroy, form_panel_tick,
                                          form_panel_draw, panel_input};

static Sint32 panel_index(struct kinder_form *form, Uint32 id)
{
    struct form_panel *panel = form->state;
    Sint32 i;
    for (i = 0; i < panel->count; i++) {
        if (panel->children[i]->box.id == id) {
            return i;
        }
    }
    return -1;
}

static int grow_panel(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    Sint32 old_capacity = panel->capacity;
    Sint32 capacity = old_capacity * 2;
    Bool grew = FALSE;
    struct kinder_form **children;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((children = syCalloc(capacity, sizeof(*children))) != NULL) {
        memcpy(children, panel->children, old_capacity * sizeof(*children));
        syFree(panel->children);
        panel->children = children;
        panel->capacity = capacity;
        grew = TRUE;
    }
    return grew;
}

struct kinder_form *form_panel_create(struct kinder_form *parent,
                                      const struct form_panel_desc *desc)
{
    struct kinder_form *form;
    struct form_panel *panel;
    Sint32 i;

    /* @hack spelling: each assignment inside its test, where a statement of
     * its own would do */
    if ((form = syCalloc(1, sizeof(*form))) != NULL) {
        if ((panel = syCalloc(1, sizeof(*panel))) != NULL) {
            form->ops = &panel_ops;
            form->kind = FORM_PANEL;
            form->state = panel;
            form->parent = parent;
            FORM_BOUNDS(form, &desc->box);
            panel->scroll_x = 0.0F;
            panel->scroll_y = 0.0F;
            panel->show_focus = TRUE;
            panel->notify = desc->notify;
            panel->capacity =
                desc->count < MIN_CAPACITY ? MIN_CAPACITY : desc->count;
            panel->count = desc->count;
            if (!(panel->children =
                      syCalloc(panel->capacity, sizeof(*panel->children)))) {
                goto free_panel;
            }
            for (i = 0; i < panel->count; i++) {
                const struct form_desc *item = &desc->children[i];
                /* @hack redundant: the test of item, an element's address,
                 * which is never null */
                if (item) {
                    struct kinder_form *child = kinder_form_create(form, item);
                    panel->children[i] = child;
                    if (child->box.id == desc->focus_id) {
                        panel->focus = i;
                    }
                }
            }
            return form;
        free_panel:
            syFree(panel);
        }
        syFree(form);
    }
    return NULL;
}

struct kinder_form *form_panel_add(struct kinder_form *form,
                                   const struct form_desc *desc)
{
    struct form_panel *panel = form->state;
    struct kinder_form *child = NULL;
    if (panel->count < panel->capacity || grow_panel(form)) {
        child = panel->children[panel->count++] =
            kinder_form_create(form, desc);
    }
    return child;
}

Sint32 form_panel_index_of(struct kinder_form *form, struct kinder_form *child)
{
    struct form_panel *panel = form->state;
    Sint32 count = panel->count;
    Sint32 i;
    for (i = 0; i < count; i++) {
        if (panel->children[i] == child) {
            return i;
        }
    }
    return -1;
}

void form_panel_notify(struct kinder_form *form, struct kinder_form *child,
                       Sint32 event)
{
    struct form_panel *panel = form->state;
    Sint32 index = form_panel_index_of(form, child);
    if (index >= 0) {
        panel->notify(panel->context, form, event, child->box.id);
    }
}

void form_panel_set_callback(struct kinder_form *form, form_notify *notify,
                             void *context)
{
    struct form_panel *panel = form->state;
    panel->notify = notify;
    panel->context = context;
}

void form_panel_focus_index(struct kinder_form *form, Sint32 index)
{
    struct form_panel *panel = form->state;
    panel->focus = index;
}

void form_panel_focus_id(struct kinder_form *form, Uint32 id)
{
    struct form_panel *panel = form->state;
    Sint32 index;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((index = panel_index(form, id)) >= 0) {
        panel->focus = index;
    }
}

struct kinder_form *form_panel_at(struct kinder_form *form, Sint32 index)
{
    struct form_panel *panel = form->state;
    return panel->children[index];
}

struct kinder_form *form_panel_find(struct kinder_form *form, Uint32 id)
{
    struct form_panel *panel = form->state;
    Sint32 index;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((index = panel_index(form, id)) >= 0) {
        return panel->children[index];
    }
    return NULL;
}

Sint32 form_panel_count(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    return panel->count;
}

struct kinder_form **form_panel_children(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    return panel->children;
}

void form_panel_hide_focus(struct kinder_form *form)
{
    struct form_panel *panel = form->state;
    panel->show_focus = FALSE;
}
