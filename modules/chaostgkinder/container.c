#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/widget.h"

struct kinder_container {
    struct kinder_widget **children;
    Sint32 count, capacity;
    struct kinder_widget *focused;
    Sint16 scroll_x, scroll_y;
    NJS_TEXLIST *texlist;
    const struct kinder_rect *background;
    Sint32 background_count;
};

#define FIRST_CAPACITY 4
/* a container scrolls to keep its focused child this far inside its edges */
#define SCROLL_MARGIN 4.0F

static void container_destroy(struct kinder_widget *widget);
static void container_update(struct kinder_widget *widget);
static void container_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                           Sint32 z);
static Sint32 container_input(struct kinder_widget *widget, Uint32 key);
static void container_focus(struct kinder_widget *widget, Sint32 selected);
static const struct kinder_widget_ops container_ops = {
    container_destroy, container_update, container_draw, container_input,
    container_focus};

static Sint32 child_index(struct kinder_widget *widget, Sint32 id)
{
    struct kinder_container *container = widget->state;
    Sint32 i;
    for (i = 0; i < container->count; i++) {
        if (container->children[i]->id == id) {
            return i;
        }
    }
    return -1;
}

static void container_destroy(struct kinder_widget *widget)
{
    struct kinder_container *container = widget->state;
    Sint32 i;
    for (i = 0; i < container->count; i++) {
        struct kinder_widget *child = container->children[i];
        child->ops->destroy(child);
    }
    syFree(container->children);
    syFree(container);
    kinder_widget_free(widget);
}

static void container_update(struct kinder_widget *widget)
{
    struct kinder_container *container = widget->state;
    Sint32 i;
    struct kinder_widget *child;

    kinder_widget_update(widget);
    for (i = 0; i < container->count; i++) {
        container->children[i]->ops->update(container->children[i]);
    }
    if (container->focused) {
        child = container->focused;
        if (container->scroll_x > child->x - SCROLL_MARGIN) {
            container->scroll_x = child->x - SCROLL_MARGIN;
        }
        if (container->scroll_x <
            child->x + child->width - widget->width + SCROLL_MARGIN) {
            container->scroll_x =
                child->x + child->width - widget->width + SCROLL_MARGIN;
        }
        if (container->scroll_y > child->y - SCROLL_MARGIN) {
            container->scroll_y = child->y - SCROLL_MARGIN;
        }
        if (container->scroll_y <
            child->y + child->height - widget->height + SCROLL_MARGIN) {
            container->scroll_y =
                child->y + child->height - widget->height + SCROLL_MARGIN;
        }
    }
}

static void container_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                           Sint32 z)
{
    struct kinder_container *container = widget->state;
    Sint32 i;
    x += widget->x;
    y += widget->y;
    z += widget->z;
    kinder_widget_draw(widget, x, y, z);
    njSetTexture(container->texlist);
    kinder_rects_draw(container->background, container->background_count, x, y,
                      z);
    x -= container->scroll_x;
    y -= container->scroll_y;
    for (i = 0; i < container->count; i++) {
        struct kinder_widget *child = container->children[i];
        child->ops->draw(child, x, y, z);
    }
}

static Sint32 container_input(struct kinder_widget *widget, Uint32 key)
{
    struct kinder_container *container = widget->state;
    return (container->focused &&
            container->focused->ops->input(container->focused, key)) ||
           kinder_widget_input(widget, key);
}

static void container_focus(struct kinder_widget *widget, Sint32 selected)
{
}

static Sint32 grow_children(struct kinder_widget *widget)
{
    struct kinder_container *container = widget->state;
    Sint32 capacity = container->capacity;
    Sint32 new_capacity = capacity * 2;
    struct kinder_widget **children;
    Sint32 grown = FALSE;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((children = syCalloc(new_capacity, sizeof(*children))) != NULL) {
        memcpy(children, container->children, capacity * sizeof(*children));
        syFree(container->children);
        container->children = children;
        container->capacity = new_capacity;
        grown = TRUE;
    }
    return grown;
}

struct kinder_widget *
kinder_container_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_container_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct kinder_container *container;
    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, where a statement and a return of each would do */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((container = syCalloc(1, sizeof(*container))) == NULL) {
        goto free_widget;
    }
    widget->state = container;
    widget->ops = &container_ops;
    container->capacity = FIRST_CAPACITY;
    container->count = 0;
    container->children =
        syCalloc(container->capacity, sizeof(*container->children));
    /* @bug memory: kinder_widget_add stores through NULL at 0x0c9d389a when
     * the syCalloc above, at 0x0c9d37ea, fails. capacity still says
     * FIRST_CAPACITY children fit */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: where the array's allocation
     * fails, the create frees what it made and returns null */
    if (!container->children) {
        syFree(container);
        goto free_widget;
    }
#endif
    container->focused = NULL;
    container->scroll_x = 0;
    container->scroll_y = 0;
    container->texlist = settings->texlist;
    container->background = settings->background;
    container->background_count = settings->background_count;
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

struct kinder_widget *kinder_widget_add(struct kinder_widget *parent,
                                        const struct kinder_widget_desc *desc)
{
    struct kinder_container *container = parent->state;
    struct kinder_widget *child = NULL;
    if (container->count < container->capacity || grow_children(parent)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = kinder_widget_create(desc)) != NULL) {
            container->children[container->count++] = child;
            child->parent = parent;
        }
    }
    return child;
}

void kinder_widget_focus(struct kinder_widget *parent,
                         struct kinder_widget *child)
{
    struct kinder_container *container = parent->state;
    if (container->focused) {
        container->focused->ops->focus(container->focused, FALSE);
    }
    container->focused = child;
    if (container->focused) {
        container->focused->ops->focus(container->focused, TRUE);
    }
}

void kinder_widget_focus_at(struct kinder_widget *parent, Sint32 index)
{
    struct kinder_container *container = parent->state;
    kinder_widget_focus(parent, container->children[index]);
}

struct kinder_widget *kinder_widget_select(struct kinder_widget *parent,
                                           Sint32 id)
{
    struct kinder_container *container = parent->state;
    Sint32 index;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((index = child_index(parent, id)) >= 0) {
        kinder_widget_focus(parent, container->children[index]);
        return container->children[index];
    }
    return NULL;
}

struct kinder_widget *kinder_widget_find(struct kinder_widget *parent,
                                         Sint32 id)
{
    struct kinder_container *container = parent->state;
    Sint32 index;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((index = child_index(parent, id)) >= 0) {
        return container->children[index];
    }
    return NULL;
}

void kinder_widget_drop_at(struct kinder_widget *parent, Sint32 index)
{
    struct kinder_container *container = parent->state;
    /* @hack type: i an Int, where the Sint32 of the file's other loops would
     * do */
    Int i;
    container->children[index]->ops->destroy(container->children[index]);
    if (container->focused == container->children[index]) {
        container->focused = NULL;
    }
    container->count--;
    for (i = index; i < container->count; i++) {
        container->children[i] = container->children[i + 1];
    }
}

void kinder_widget_drop(struct kinder_widget *parent, Sint32 id)
{
    /* @hack redundant: widget, a copy of parent, where parent would do */
    struct kinder_widget *widget = parent;
    Sint32 index;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((index = child_index(widget, id)) >= 0) {
        kinder_widget_drop_at(widget, index);
    }
}
