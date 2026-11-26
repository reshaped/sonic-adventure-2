#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/form.h"
#include "chaostgkinder/form_layout.h"
#include "chaostgkinder/form_leaf_impl.h"

static void form_rect_draw(struct kinder_form *form, Float x, Float y, Float z)
{
    NJS_POLYGON_VTX vertices[4];
    const Uint32 *color = form->state;
    x += form->box.x;
    y += form->box.y;
    z += form->box.z;
    FORM_QUAD(vertices, form, x, y, z, *color);
    njDrawPolygon(vertices, 4, FALSE);
}

static Sint32 form_rect_input(struct kinder_form *form, Sint32 key)
{
    return FALSE;
}

static struct form_ops rectangle_ops = {form_leaf_destroy, form_leaf_tick,
                                        form_rect_draw, form_rect_input};

struct kinder_form *form_rect_create(struct kinder_form *parent,
                                     const struct form_box *box)
{
    struct kinder_form *form;
    Uint32 *color;
    /* @hack spelling: each assignment inside its test, and the gotos to the
     * frees at the end, where each allocation could free what came before
     * and return on its own */
    if ((form = syCalloc(1, sizeof(*form))) == NULL) {
        goto failed;
    }
    if ((color = syCalloc(1, sizeof(*color))) == NULL) {
        goto free_form;
    }
    form->ops = &rectangle_ops;
    form->kind = FORM_RECT;
    /* nothing sets the color, so it stays calloc's 0. the rect draws opaque,
     * so that 0 shows black */
    form->state = color;
    form->parent = parent;
    FORM_BOUNDS(form, box);
    return form;
free_form:
    syFree(form);
failed:
    return NULL;
}
