#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/control_impl.h"
#include "chaostgkinder/widget.h"

/* the entry inside a frame: its inset from the frame's edges, its height,
 * and its layer above the frame */
#define ENTRY_INSET 4
#define ENTRY_HEIGHT 32
#define ENTRY_LAYER 1

struct entry_frame {
    Bool selected;
    NJS_TEXLIST *texlist;
    const struct kinder_rect *focused;
    Sint32 focused_count;
    const struct kinder_rect *unfocused;
    Sint32 unfocused_count;
    struct kinder_widget *entry;
};

static void frame_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z)
{
    struct entry_frame *frame = widget->state;
    if (widget->visible) {
        x += widget->x;
        y += widget->y;
        z += widget->z;
        njSetTexture(frame->texlist);
        if (frame->selected) {
            kinder_rects_draw(frame->focused, frame->focused_count, x, y, z);
        } else {
            kinder_rects_draw(frame->unfocused, frame->unfocused_count, x, y,
                              z);
        }
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        kinder_entry_draw(frame->entry, x, y, z);
    }
}

static Sint32 frame_input(struct kinder_widget *widget, Uint32 key)
{
    struct entry_frame *frame = widget->state;
    /* @hack implicit: the key the call leaves out, which SHC's code supplies
     * from frame_input's own, where passing key would do */
#ifdef NONMATCHING
    /* the plain form the hack tag names: the call passes key */
    if (kinder_entry_input(frame->entry, key) == 0 &&
#else
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    if (((Sint32 (*)(struct kinder_widget *))kinder_entry_input)(
            frame->entry) == 0 &&
#endif
        kinder_widget_input(widget, key) == 0) {
        return FALSE;
    }
    return TRUE;
}

static void frame_focus(struct kinder_widget *widget, Sint32 selected)
{
    struct entry_frame *frame = widget->state;
    frame->selected = selected;
}

#ifdef NONMATCHING
/* this side repairs the bug tagged in kinder_frame_create: a frame destroys
 * its entry before itself */
static void frame_destroy(struct kinder_widget *widget)
{
    struct entry_frame *frame = widget->state;
    if (frame->entry) {
        frame->entry->ops->destroy(frame->entry);
    }
    control_destroy(widget);
}

static struct kinder_widget_ops frame_ops = {
    frame_destroy, control_update, frame_draw, frame_input, frame_focus};
#else
static struct kinder_widget_ops frame_ops = {
    control_destroy, control_update, frame_draw, frame_input, frame_focus};
#endif

struct kinder_widget *kinder_frame_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_frame_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct entry_frame *frame;
    struct kinder_widget_desc child;
    struct kinder_entry_desc entry;
    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((frame = syCalloc(1, sizeof(*frame))) == NULL) {
        goto free_widget;
    }
    widget->ops = &frame_ops;
    widget->state = frame;
    frame->selected = FALSE;
    frame->texlist = settings->texlist;
    frame->focused = settings->focused;
    frame->focused_count = settings->focused_count;
    frame->unfocused = settings->unfocused;
    frame->unfocused_count = settings->unfocused_count;
    child.kind = WIDGET_TEXT_ENTRY;
    child.id = 0;
    child.x = ENTRY_INSET;
    child.y = ENTRY_INSET;
    child.z = ENTRY_LAYER;
    child.width = widget->width - (ENTRY_INSET * 2.0F);
    child.height = ENTRY_HEIGHT;
    child.notify = NULL;
    child.settings = &entry;
    entry.rows = 1;
    entry.capacity = settings->capacity;
    /* @bug memory: no path frees the entry kinder_frame_create makes here, at
     * 0x0c9db300. the frame's destructor, at 0x0c9db1e0, frees only the
     * frame's state and widget */
    frame->entry = kinder_widget_create(&child);
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

Uint16 *kinder_frame_text(struct kinder_widget *widget)
{
    struct entry_frame *frame = widget->state;
    return kinder_entry_text(frame->entry);
}

void kinder_frame_set_text(struct kinder_widget *widget, const Uint16 *text)
{
    struct entry_frame *frame = widget->state;
    kinder_entry_set_text(frame->entry, text);
}

void kinder_frame_set_texlist(struct kinder_widget *widget,
                              NJS_TEXLIST *texlist)
{
    struct entry_frame *frame = widget->state;
    frame->texlist = texlist;
}
