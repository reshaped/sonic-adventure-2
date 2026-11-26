#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/control_impl.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"

#define LETTER_SIZE 22.0F
/* the text's top below the button's, and its z past the button's */
#define TEXT_INSET 6
#define TEXT_LIFT 2
#define FOCUSED_TEXT 0xffffffff
#define UNFOCUSED_TEXT 0xff999999

static void button_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                        Sint32 z)
{
    struct kinder_button *button = widget->state;
    struct kinder_text_layout measure;
    struct kinder_text text;
    if (!widget->visible) {
        return;
    }
    x += widget->x;
    y += widget->y;
    z += widget->z;
    njSetTexture(button->texlist);
    if (button->selected) {
        kinder_rects_draw(button->focused, button->focused_count, x, y, z);
    } else {
        kinder_rects_draw(button->unfocused, button->unfocused_count, x, y, z);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
    if (button->text) {
        measure.string = button->text;
        measure.length = KINDER_WHOLE_STRING;
        measure.width = widget->width;
        njQuadTextureStart(TRUE);
        measure.height = widget->height;
        measure.spacing = KINDER_TEXT_GAP;
        measure.leading = KINDER_TEXT_GAP;
        measure.x = 0.0F;
        measure.y = 0.0F;
        kinder_text_measure_scaled(&measure, LETTER_SIZE, LETTER_SIZE);

        text.layout.string = button->text;
        text.layout.length = KINDER_WHOLE_STRING;
        text.layout.width = widget->width;
        text.layout.height = widget->height;
        text.layout.spacing = KINDER_TEXT_GAP;
        text.layout.leading = KINDER_TEXT_GAP;
        text.layout.x = 0.0F;
        text.layout.y = 0.0F;
        text.left = x + ((widget->width - measure.x) * 0.5F);
        text.top = y + TEXT_INSET;
        text.z = KINDER_WIDGET_DEPTH(z + TEXT_LIFT);
        text.color = button->selected ? FOCUSED_TEXT : UNFOCUSED_TEXT;
        kinder_text_draw_scaled(&text, LETTER_SIZE, LETTER_SIZE);
    }
}

static Sint32 button_input(struct kinder_widget *widget, Uint32 key)
{
    return kinder_widget_input(widget, key);
}

static void button_focus(struct kinder_widget *widget, Sint32 selected)
{
    struct kinder_button *button = widget->state;
    button->selected = selected;
}

static struct kinder_widget_ops button_ops = {
    control_destroy, control_update, button_draw, button_input, button_focus};

struct kinder_widget *
kinder_button_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_button_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct kinder_button *button;
    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, where a statement and a return of each would do */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((button = syCalloc(1, sizeof(*button))) == NULL) {
        goto free_widget;
    }
    widget->ops = &button_ops;
    widget->state = button;
    button->text = kinder_string(settings->message);
    button->selected = FALSE;
    button->texlist = settings->texlist;
    button->focused = settings->focused;
    button->focused_count = settings->focused_count;
    button->unfocused = settings->unfocused;
    button->unfocused_count = settings->unfocused_count;
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}

void kinder_button_set_text(struct kinder_widget *widget, const Uint16 *text)
{
    struct kinder_button *button = widget->state;
    button->text = text;
}

void kinder_button_set_texlist(struct kinder_widget *widget,
                               NJS_TEXLIST *texlist)
{
    struct kinder_button *button = widget->state;
    button->texlist = texlist;
}
