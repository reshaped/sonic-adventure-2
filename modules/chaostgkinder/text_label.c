#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/control_impl.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"

/* scaled_label.c builds this source again with SCALED_LABEL defined, as
 * kinder_scaled_text_create */
#ifdef SCALED_LABEL
#define LABEL_SETTINGS kinder_scaled_label_desc
#define LABEL_CREATE kinder_scaled_text_create
#define LABEL_SET_TEXT kinder_scaled_text_set
#else
#define LABEL_SETTINGS kinder_label_desc
#define LABEL_CREATE kinder_text_create
#define LABEL_SET_TEXT kinder_text_set
#endif

struct label_state {
    const Uint16 *text;
#ifdef SCALED_LABEL
    Float scale, height;
#endif
};

static void label_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z)
{
    struct label_state *label = widget->state;
    struct kinder_text text;
    if (widget->visible) {
        x += widget->x;
        y += widget->y;
        z += widget->z;
        text.layout.string = label->text;
        text.layout.length = KINDER_WHOLE_STRING;
        text.layout.width = widget->width;
        text.layout.height = widget->height;
        text.layout.spacing = KINDER_TEXT_GAP;
        text.layout.leading = KINDER_TEXT_GAP;
        text.layout.x = 0.0F;
        text.layout.y = 0.0F;
        text.left = x;
        text.top = y;
        text.z = KINDER_WIDGET_DEPTH(z);
        text.color = 0xff000000;
#ifdef SCALED_LABEL
        kinder_text_draw_scaled(&text, label->scale, label->height);
#else
        kinder_text_draw(&text);
#endif
    }
}

static Sint32 label_input(struct kinder_widget *widget, Uint32 key)
{
    return FALSE;
}

static const struct kinder_widget_ops label_ops = {
    control_destroy, control_update, label_draw, label_input, NULL};

struct kinder_widget *LABEL_CREATE(const struct kinder_widget_desc *desc)
{
    const struct LABEL_SETTINGS *settings = desc->settings;
    struct kinder_widget *widget;
    struct label_state *label;

    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, where a statement and a return of each would do */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((label = syCalloc(1, sizeof(*label))) == NULL) {
        goto free_widget;
    }
    widget->ops = &label_ops;
    widget->state = label;
    label->text = kinder_string(settings->message);
#ifdef SCALED_LABEL
    label->scale = settings->scale;
    label->height = settings->height;
#endif
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

void LABEL_SET_TEXT(struct kinder_widget *widget, const Uint16 *text)
{
    struct label_state *label = widget->state;
    label->text = text;
}
