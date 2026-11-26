#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/control_impl.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"

struct caption_state {
    const Uint16 *text;
    Float scale, height;
    Uint32 color, flags;
};

static void caption_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                         Sint32 z)
{
    struct caption_state *caption = widget->state;
    struct kinder_text text;
    struct kinder_text_layout measure;
    Float left;
    /* @hack spelling: the locals zero and spacing, where 0.0F and
     * KINDER_TEXT_GAP in each layout's fields would do */
    Float zero;
    Float spacing;

    if (!widget->visible) {
        return;
    }
    zero = 0.0F;
    spacing = KINDER_TEXT_GAP;
    x += widget->x;
    y += widget->y;
    z += widget->z;
    left = x;
    if (caption->flags & CAPTION_CENTERED) {
        measure.string = caption->text;
        measure.length = KINDER_WHOLE_STRING;
        measure.width = widget->width;
        njQuadTextureStart(TRUE);
        measure.height = widget->height;
        /* @hack spelling: spacing and leading in one assignment, where an
         * assignment of each would do */
        measure.leading = measure.spacing = spacing;
        measure.x = zero;
        measure.y = zero;
        if (caption->flags & CAPTION_SCALED) {
            kinder_text_measure_scaled(&measure, caption->scale,
                                       caption->height);
        } else {
            kinder_text_measure(&measure);
        }
        left += (widget->width - measure.x) * 0.5F;
    }
    text.left = left;
    text.top = y;
    text.layout.width = widget->width;
    text.layout.height = widget->height;
    text.layout.length = KINDER_WHOLE_STRING;
    text.layout.string = caption->text;
    text.z = KINDER_WIDGET_DEPTH(z);
    /* @hack spelling: spacing and leading in one assignment, as above */
    text.layout.leading = text.layout.spacing = spacing;
    text.layout.x = zero;
    text.layout.y = zero;
    text.color = caption->color;
    if (caption->flags & CAPTION_SCALED) {
        kinder_text_draw_scaled(&text, caption->scale, caption->height);
    } else {
        kinder_text_draw(&text);
    }
}

static Sint32 caption_input(struct kinder_widget *widget, Uint32 key)
{
    return FALSE;
}

static const struct kinder_widget_ops caption_ops = {
    control_destroy, control_update, caption_draw, caption_input, NULL};

struct kinder_widget *
kinder_caption_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_caption_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct caption_state *caption;
    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, where a statement and a return of each would do */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((caption = syCalloc(1, sizeof(*caption))) == NULL) {
        goto free_widget;
    }
    widget->ops = &caption_ops;
    widget->state = caption;
    caption->text = kinder_string(settings->message);
    caption->scale = settings->scale;
    caption->height = settings->height;
    caption->color = settings->color;
    caption->flags = settings->flags;
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

void kinder_caption_set_text(struct kinder_widget *widget, const Uint16 *text)
{
    struct caption_state *caption = widget->state;
    caption->text = text;
}
