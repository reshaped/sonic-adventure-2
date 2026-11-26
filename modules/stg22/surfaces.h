#ifndef STG22_SURFACES_H
#define STG22_SURFACES_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"

struct texture_anim;

/* a light strip whose texture scrolls. where scroll is 0, as on every strip,
 * gravity.c plays animation with texture_anim_apply. otherwise it scrolls the
 * strip with texture_anim_scroll */
struct gadget_surface_animation {
    NJS_CNK_OBJECT *object;
    struct texture_anim *animation;
    Sint32 scroll;
};

extern struct land_table gadget_scrolling_land;
extern const char gadget_scrolling_land_filenames[72];
extern struct gadget_surface_animation gadget_surface_animations[13];

extern NJS_CNK_OBJECT gadget_light_strip_191;
extern NJS_CNK_OBJECT gadget_light_strip_190;
extern NJS_CNK_OBJECT gadget_light_strip_187;
extern NJS_CNK_OBJECT gadget_light_strip_180;
extern NJS_CNK_OBJECT gadget_light_strip_179;
extern NJS_CNK_OBJECT gadget_light_strip_178;
extern NJS_CNK_OBJECT gadget_light_strip_177;
extern NJS_CNK_OBJECT gadget_light_strip_169;
extern NJS_CNK_OBJECT gadget_light_strip_160;
extern NJS_CNK_OBJECT gadget_light_strip_158;
extern NJS_CNK_OBJECT gadget_light_strip_157;
extern NJS_CNK_OBJECT gadget_light_strip_149;
extern NJS_CNK_OBJECT gadget_light_strip_031;

#endif
