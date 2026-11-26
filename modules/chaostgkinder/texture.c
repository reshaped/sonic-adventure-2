#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/control_impl.h"
#include "chaostgkinder/widget.h"

/* the atlas counts UNITS_PER_PIXEL units to a screen pixel */
#define UNITS_PER_PIXEL 2
/* the low two bits of kinder_texture_desc.mode hold its drawing mode.
 * TEXTURE_TILE and TEXTURE_STRETCH_X draw, and 2 and 3 draw nothing */
#define MODE_MASK 3

static void texture_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                         Sint32 z)
{
    struct kinder_texture_desc *desc = widget->state;
    NJS_QUAD_TEXTURE quad;
    Float depth;
    Sint32 tile_height;
    Sint32 dx;
    Sint32 dy;
    Sint32 tile_width;
    njSetTexture(desc->texlist);
    x += desc->x;
    y += desc->y;
    z += desc->z;
    depth = KINDER_WIDGET_DEPTH(z);
    if (widget->visible) {
        njQuadTextureStart(desc->translucent);
        njSetQuadTexture(desc->texture, desc->color);
        switch (desc->mode & MODE_MASK) {
        case TEXTURE_TILE:
            tile_height = (desc->v2 - desc->v1) / UNITS_PER_PIXEL;
            quad.v1 = KINDER_ATLAS_UV(desc->v1);
            quad.v2 = KINDER_ATLAS_UV(desc->v2);
            for (dy = 0; dy < desc->height;) {
                tile_width = (desc->u2 - desc->u1) / UNITS_PER_PIXEL;
                quad.y1 = y + dy;
                dx = 0;
                if (dy + tile_height <= desc->height) {
                    dy += tile_height;
                } else {
                    quad.v2 = KINDER_ATLAS_UV(
                        desc->v1 + ((desc->height - dy) * UNITS_PER_PIXEL));
                    dy = desc->height;
                }
                quad.y2 = y + dy;
                quad.u1 = KINDER_ATLAS_UV(desc->u1);
                quad.u2 = KINDER_ATLAS_UV(desc->u2);
                while (dx < desc->width) {
                    quad.x1 = x + dx;
                    if (dx + tile_width < desc->width) {
                        dx += tile_width;
                    } else {
                        quad.u2 = KINDER_ATLAS_UV(
                            desc->u1 + ((desc->width - dx) * UNITS_PER_PIXEL));
                        dx = desc->width;
                    }
                    quad.x2 = x + dx;
                    njDrawQuadTexture(&quad, depth);
                }
            }
            break;
        case TEXTURE_STRETCH_X:
            quad.x1 = x;
            quad.x2 = x + desc->width;
            quad.u1 = KINDER_ATLAS_UV(desc->u1);
            quad.u2 = KINDER_ATLAS_UV(desc->u2);
            quad.v1 = KINDER_ATLAS_UV(desc->v1);
            quad.v2 = KINDER_ATLAS_UV(desc->v2);
            tile_height = (desc->v2 - desc->v1) / UNITS_PER_PIXEL;
            for (dy = 0; tile_height + dy <= desc->height;) {
                quad.y1 = y + dy;
                dy += tile_height;
                quad.y2 = y + dy;
                njDrawQuadTexture(&quad, depth);
            }
            if (dy < desc->height) {
                quad.y1 = y + dy;
                quad.y2 = y + desc->height;
                quad.v2 = KINDER_ATLAS_UV(
                    desc->v1 + ((desc->height - dy) * UNITS_PER_PIXEL));
                njDrawQuadTexture(&quad, depth);
            }
            break;
        /* @hack redundant: the empty cases of modes 2 and 3 */
        case 2:
            break;
        case 3:
            break;
        }
        njQuadTextureEnd();
    }
}

static Sint32 texture_input(struct kinder_widget *widget, Uint32 key)
{
    return FALSE;
}

static void texture_focus(struct kinder_widget *widget, Sint32 selected)
{
}

static const struct kinder_widget_ops texture_ops = {
    control_destroy, control_update, texture_draw, texture_input,
    texture_focus};

struct kinder_widget *
kinder_texture_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_texture_desc *settings = desc->settings;
    struct kinder_widget *widget;
    struct kinder_texture_desc *copy;
    /* @hack spelling: the gotos to the failure returns, where a return at
     * each test would do */
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((widget = kinder_widget_alloc(desc)) == NULL) {
        goto failed;
    }
    if ((copy = syCalloc(1, sizeof(*copy))) == NULL) {
        goto free_widget;
    }
    widget->ops = &texture_ops;
    widget->state = copy;
    /* @hack spelling: the fields copied one by one, where an assignment of
     * the whole descriptor would do */
    copy->texlist = settings->texlist;
    copy->texture = settings->texture;
    copy->translucent = settings->translucent;
    copy->x = settings->x;
    copy->y = settings->y;
    copy->z = settings->z;
    copy->width = settings->width;
    copy->height = settings->height;
    copy->u1 = settings->u1;
    copy->u2 = settings->u2;
    copy->v1 = settings->v1;
    copy->v2 = settings->v2;
    copy->color = settings->color;
    copy->mode = settings->mode;
    return widget;
free_widget:
    kinder_widget_free(widget);
failed:
    return NULL;
}
