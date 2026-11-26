/* two scrolling texture animations that no code reads. each scrolls its one
 * key along v by a step each frame, in the polygon list's uv units. the step
 * is -48 for negative_v_scroll and 16 for positive_v_scroll */
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static Sint16 negative_v_uvs[180] = {
#include "draw/unused_uv_scrolls/negative_v_uvs.inc"
};
static struct texture_anim negative_v_scroll = {
#include "draw/unused_uv_scrolls/negative_v_scroll.inc"
};

static Sint16 positive_v_uvs[298] = {
#include "draw/unused_uv_scrolls/positive_v_uvs.inc"
};
static struct texture_anim positive_v_scroll = {
#include "draw/unused_uv_scrolls/positive_v_scroll.inc"
};
