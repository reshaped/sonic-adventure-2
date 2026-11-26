#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/font.h"
#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_leaf_impl.h"
#include "chaostgkinder/keyboard.h"

#define BUTTON_COLOR 0xff8080ff

static void form_button_draw(struct kinder_form *form, Float x, Float y,
                             Float z)
{
    const Uint16 **text = form->state;
    if (form->visible) {
        x += form->box.x;
        y += form->box.y;
        z += form->box.z;
        /* @hack spelling: the block that ends vertices' scope before run's
         * begins, where declaring vertices first in this one would do */
        {
            NJS_POLYGON_VTX vertices[4];
            FORM_QUAD(vertices, form, x, y, z, BUTTON_COLOR);
            njDrawPolygon(vertices, 4, FALSE);
        }
        if (*text) {
            struct kinder_text run;
            FORM_TEXT(run, form, *text, x, y, z + 0.001F);
            kinder_text_draw(&run);
        }
    }
}

static Sint32 form_button_input(struct kinder_form *form, Sint32 key)
{
    return FALSE;
}

static struct form_ops button_ops = {form_leaf_destroy, form_leaf_tick,
                                     form_button_draw, form_button_input};

struct kinder_form *form_button_create(struct kinder_form *parent,
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
    form->ops = &button_ops;
    form->kind = FORM_BUTTON;
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

void form_button_set_text(struct kinder_form *form, const Uint16 *text)
{
    const Uint16 **label = form->state;
    *label = text;
}
