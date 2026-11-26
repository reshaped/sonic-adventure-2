/* A container frees each block it allocates, whether its create fails or it
 * is destroyed. An entry frame's destroy frees its entry with it. */
#include <assert.h>
#include <stdlib.h>

#include "chaostgkinder/widget.h"

/* syCalloc fails its call numbered fail_at, counting from 1, or none at 0.
 * live counts the blocks allocated and not yet freed */
static int calls, fail_at, live;
static int entries_destroyed;

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;
    if (++calls == fail_at) {
        return NULL;
    }
    block = calloc(count, size);
    assert(block);
    live++;
    return block;
}

void syFree(void *block)
{
    if (block) {
        live--;
        free(block);
    }
}

struct kinder_widget *kinder_widget_alloc(const struct kinder_widget_desc *desc)
{
    struct kinder_widget *widget = syCalloc(1, sizeof(*widget));
    if (widget) {
        widget->kind = desc->kind;
        widget->visible = TRUE;
    }
    return widget;
}

void kinder_widget_free(struct kinder_widget *widget)
{
    syFree(widget);
}

static void entry_destroy(struct kinder_widget *widget)
{
    entries_destroyed++;
    kinder_widget_free(widget);
}

static const struct kinder_widget_ops entry_ops = {entry_destroy};

/* the frame's entry: a widget with no state of its own */
struct kinder_widget *
kinder_widget_create(const struct kinder_widget_desc *desc)
{
    struct kinder_widget *widget = kinder_widget_alloc(desc);
    if (widget) {
        widget->ops = &entry_ops;
    }
    return widget;
}

/* what the ops other than destroy call. the tests call no other op */
void kinder_widget_update(struct kinder_widget *widget)
{
    assert(0);
}

void kinder_widget_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                        Sint32 z)
{
    assert(0);
}

Sint32 kinder_widget_input(struct kinder_widget *widget, Uint32 key)
{
    assert(0);
    return 0;
}

void kinder_rects_draw(const struct kinder_rect *entries, Int count, Int x,
                       Int y, Int z)
{
    assert(0);
}

void kinder_entry_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z)
{
    assert(0);
}

Sint32 kinder_entry_input(struct kinder_widget *widget, Uint32 key)
{
    assert(0);
    return 0;
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    assert(0);
    return 0;
}

void njTextureFilterMode(Int mode)
{
    assert(0);
}

static void check_container(void)
{
    static const struct kinder_container_desc settings = {0};
    struct kinder_widget_desc desc = {WIDGET_CONTAINER};
    struct kinder_widget *container;

    desc.settings = &settings;
    /* the widget, the container and its array of children, in that order */
    for (fail_at = 1; fail_at <= 3; fail_at++) {
        calls = 0;
        assert(!kinder_container_create(&desc));
        assert(live == 0);
    }
    fail_at = 0;
    container = kinder_container_create(&desc);
    assert(container);
    container->ops->destroy(container);
    assert(live == 0);
}

static void check_frame(void)
{
    static const struct kinder_frame_desc settings = {0};
    struct kinder_widget_desc desc = {WIDGET_FRAME};
    struct kinder_widget *frame;

    desc.width = 100;
    desc.settings = &settings;
    fail_at = 0;
    frame = kinder_frame_create(&desc);
    assert(frame);
    entries_destroyed = 0;
    frame->ops->destroy(frame);
    assert(entries_destroyed == 1);
    assert(live == 0);
}

int main(void)
{
    check_container();
    check_frame();
    return 0;
}
