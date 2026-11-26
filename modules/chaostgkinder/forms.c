#include <sg_xpt.h>

#include "chaostgkinder/form.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"

void kinder_forms_start(void)
{
    kinder_font_start();
    kinder_strings_load();
    kinder_keys_start();
}

void kinder_forms_stop(void)
{
    kinder_keys_stop();
    kinder_strings_release();
    kinder_font_stop();
}

void kinder_form_position(struct kinder_form *form, Float x, Float y, Float z)
{
    form->box.x = x;
    form->box.y = y;
    form->box.z = z;
}

void kinder_form_size(struct kinder_form *form, Float width, Float height)
{
    form->box.width = width;
    form->box.height = height;
}

struct kinder_form *kinder_form_create(struct kinder_form *parent,
                                       const struct form_desc *desc)
{
    struct kinder_form *form = NULL;
    switch (desc->kind) {
    case FORM_PANEL:
        form = form_panel_create(parent, desc->settings);
        break;
    case FORM_LABEL:
        form = form_label_create(parent, desc->settings);
        break;
    case FORM_FIELD:
        form = form_field_create(parent, desc->settings);
        break;
    case FORM_BUTTON:
        form = form_button_create(parent, desc->settings);
        break;
    case FORM_RECT:
        form = form_rect_create(parent, desc->settings);
        break;
    case FORM_CHOICE:
        form = form_choice_create(parent, desc->settings);
        break;
    }
    return form;
}

/* @hack redundant: form declared register, where a plain parameter would do */
void kinder_form_poll(register struct kinder_form *form)
{
    Uint16 key;
    kinder_keys_poll();
    /* @hack spelling: the comma before the test of key, where testing the
     * assignment would do */
    while ((key = kinder_keys_next()), key) {
#ifdef NONMATCHING
        /* this side keeps the tag's result, as the callback reads no third
         * argument. it calls the callback by its own type, since a call through
         * another function type is undefined C */
        form->ops->input(form, key);
#else
        /* @bug mismatched-call: the call at 0x0c9d0c88 passes a third
         * argument, 0, to an input callback that takes the form and the key */
        typedef Sint32 (*input_with_extra)(struct kinder_form *, Sint32,
                                           Sint32);
        /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
        ((input_with_extra)form->ops->input)(form, key, 0);
#endif
    }
}

void kinder_form_show(struct kinder_form *form)
{
    form->visible = TRUE;
}

void kinder_form_hide(struct kinder_form *form)
{
    form->visible = FALSE;
}
