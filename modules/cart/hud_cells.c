/* the cart's stage_hooks.action_icon and number_cell, which give the HUD the
 * UVs of their sprites of cart_hud_sprites */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/objects.h"
#include "cart/stage.h"
#include "hud/action_window.h"
#include "hud/hud.h"

/* the hooks' sprites in cart_hud_sprites */
#define ACTION_SPRITE 28
#define NUMBER_SPRITE 30
/* a sprite's UVs run to UV_SPAN across its texture, and a rect's to 1 */
#define UV_SPAN 256.0F

/* whether cart_action_icon and cart_number_cell have yet to convert their
 * sprites' UVs, which each does once */
static Bool need_icon = 1;
static Bool need_number = 1;
static struct icon_rect icon;
static struct hud_number_cell number;

#define CACHE_RECT(rect, sprite)                                               \
    do {                                                                       \
        (rect).u0 = (sprite).u1 * (1.0F / UV_SPAN);                            \
        (rect).v0 = (sprite).v1 * (1.0F / UV_SPAN);                            \
        (rect).u1 = (sprite).u2 * (1.0F / UV_SPAN);                            \
        (rect).v1 = (sprite).v2 * (1.0F / UV_SPAN);                            \
    } while (0)
#define GIVE_RECT(dest, src)                                                   \
    do {                                                                       \
        (dest)->texlist = &cart_object_textures;                               \
        (dest)->u0 = (src).u0;                                                 \
        (dest)->v0 = (src).v0;                                                 \
        (dest)->u1 = (src).u1;                                                 \
        (dest)->v1 = (src).v1;                                                 \
    } while (0)

void cart_action_icon(struct icon_rect *rect)
{
    if (need_icon) {
        CACHE_RECT(icon, cart_hud_sprites[ACTION_SPRITE]);
        need_icon = 0;
    }
    GIVE_RECT(rect, icon);
}
void cart_number_cell(struct hud_number_cell *cell)
{
    /* @bug logic: tests need_icon at 0x8c5004a6, where need_number is meant.
     * the cell converts its UVs at each call until the action icon's first,
     * and a cell first asked for after the action icon keeps UVs of 0 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it tests need_number */
    if (need_number) {
#else
    if (need_icon) {
#endif
        CACHE_RECT(number, cart_hud_sprites[NUMBER_SPRITE]);
        need_number = 0;
    }
    GIVE_RECT(cell, number);
}
