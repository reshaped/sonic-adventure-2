/* the light strips' scrolls, each one key of uvs as texture_anim.h lays them
 * out. a scroll moves v 1 a frame, and gravity.c plays 4 frames a step */
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg22/surfaces.h"

static Sint16 light_strip_191_uvs[16] = {
#include "stg22/surface_animations/light_strip_191_uvs.inc"
};

static struct texture_anim light_strip_191_scroll = {
#include "stg22/surface_animations/light_strip_191_scroll.inc"
};

static Sint16 light_strip_187_uvs[34] = {
#include "stg22/surface_animations/light_strip_187_uvs.inc"
};

static struct texture_anim light_strip_187_scroll = {
#include "stg22/surface_animations/light_strip_187_scroll.inc"
};

static Sint16 light_strip_179_uvs[16] = {
#include "stg22/surface_animations/light_strip_179_uvs.inc"
};

static struct texture_anim light_strip_179_scroll = {
#include "stg22/surface_animations/light_strip_179_scroll.inc"
};

static Sint16 light_strip_180_uvs[16] = {
#include "stg22/surface_animations/light_strip_180_uvs.inc"
};

static struct texture_anim light_strip_180_scroll = {
#include "stg22/surface_animations/light_strip_180_scroll.inc"
};

static Sint16 light_strip_177_uvs[16] = {
#include "stg22/surface_animations/light_strip_177_uvs.inc"
};

static struct texture_anim light_strip_177_scroll = {
#include "stg22/surface_animations/light_strip_177_scroll.inc"
};

static Sint16 light_strip_178_uvs[42] = {
#include "stg22/surface_animations/light_strip_178_uvs.inc"
};

static struct texture_anim light_strip_178_scroll = {
#include "stg22/surface_animations/light_strip_178_scroll.inc"
};

static Sint16 light_strip_149_uvs[12] = {
#include "stg22/surface_animations/light_strip_149_uvs.inc"
};

static struct texture_anim light_strip_149_scroll = {
#include "stg22/surface_animations/light_strip_149_scroll.inc"
};

static Sint16 light_strip_169_uvs[16] = {
#include "stg22/surface_animations/light_strip_169_uvs.inc"
};

static struct texture_anim light_strip_169_scroll = {
#include "stg22/surface_animations/light_strip_169_scroll.inc"
};

static Sint16 light_strip_157_uvs[42] = {
#include "stg22/surface_animations/light_strip_157_uvs.inc"
};

static struct texture_anim light_strip_157_scroll = {
#include "stg22/surface_animations/light_strip_157_scroll.inc"
};

static Sint16 light_strip_158_uvs[52] = {
#include "stg22/surface_animations/light_strip_158_uvs.inc"
};

static struct texture_anim light_strip_158_scroll = {
#include "stg22/surface_animations/light_strip_158_scroll.inc"
};

static Sint16 light_strip_160_uvs[46] = {
#include "stg22/surface_animations/light_strip_160_uvs.inc"
};

static struct texture_anim light_strip_160_scroll = {
#include "stg22/surface_animations/light_strip_160_scroll.inc"
};

static Sint16 light_strip_031_uvs[42] = {
#include "stg22/surface_animations/light_strip_031_uvs.inc"
};

static struct texture_anim light_strip_031_scroll = {
#include "stg22/surface_animations/light_strip_031_scroll.inc"
};

static Sint16 light_strip_190_uvs[34] = {
#include "stg22/surface_animations/light_strip_190_uvs.inc"
};

static struct texture_anim light_strip_190_scroll = {
#include "stg22/surface_animations/light_strip_190_scroll.inc"
};

struct gadget_surface_animation gadget_surface_animations[13] = {
#include "stg22/surface_animations/gadget_surface_animations.inc"
};
