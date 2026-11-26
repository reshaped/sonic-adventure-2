#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/font.h"
#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_leaf_impl.h"
#include "chaostgkinder/keyboard.h"

static void form_label_draw(struct kinder_form *form, Float x, Float y, Float z)
{
    const Uint16 **text = form->state;
    if (form->visible) {
        struct kinder_text run;
        x += form->box.x;
        y += form->box.y;
        z += form->box.z;
        FORM_TEXT(run, form, *text, x, y, z);
        kinder_text_draw(&run);
    }
}

static Sint32 form_label_input(struct kinder_form *form, Sint32 key)
{
    return FALSE;
}

static const struct form_ops label_ops = {form_leaf_destroy, form_leaf_tick,
                                          form_label_draw, form_label_input};

struct kinder_form *form_label_create(struct kinder_form *parent,
                                      const struct form_text_desc *desc)
{
    struct kinder_form *form;
    const Uint16 **text;

    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((form = syCalloc(1, sizeof(*form))) == NULL) {
        goto failed;
    }
    if ((text = syCalloc(1, sizeof(*text))) == NULL) {
        goto free_form;
    }
    form->ops = &label_ops;
    form->kind = FORM_LABEL;
    form->state = text;
    form->parent = parent;
    FORM_BOUNDS(form, &desc->box);
    form->visible = TRUE;
    *text = kinder_string(desc->message);
    return form;
free_form:
    syFree(form);
failed:
    return NULL;
}

void form_label_set_text(struct kinder_form *form, const Uint16 *text)
{
    const Uint16 **label = form->state;
    *label = text;
}
