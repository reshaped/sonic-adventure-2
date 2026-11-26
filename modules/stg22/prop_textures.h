#ifndef STG22_PROP_TEXTURES_H
#define STG22_PROP_TEXTURES_H
#include <ninja.h>

extern NJS_TEXLIST gadget_arrow_texlist;
extern NJS_TEXLIST gadget_grind_rail_texlist;
extern NJS_TEXLIST gadget_marker_texlist;
extern NJS_TEXLIST gadget_capsule_texlist;
extern NJS_TEXLIST gadget_machine_texlist;

extern NJS_TEXLIST gadget_poison_body_texlist;
extern NJS_TEXLIST gadget_poison_glass_texlist;

#define GADGET_LIGHT_FRAMES 3
/* a lamp's or a sign's texture list for one frame, after the names it
 * points at */
struct gadget_lamp_frame {
    NJS_TEXNAME name;
    NJS_TEXLIST list;
};
struct gadget_sign_frame {
    NJS_TEXNAME names[2];
    NJS_TEXLIST list;
};
extern struct gadget_lamp_frame gadget_lamp_frames[GADGET_LIGHT_FRAMES];
extern struct gadget_sign_frame gadget_sign_frames[GADGET_LIGHT_FRAMES];
extern NJS_TEXLIST gadget_sign_panel_texlist;

#endif
