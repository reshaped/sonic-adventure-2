#ifndef STG35_MACHINERY_H
#define STG35_MACHINERY_H
#include <ninja.h>
#include <sg_xpt.h>

struct eggman_machinery_texture_names {
    char rotor_surface[8];
    char rotor_rim[8];
    char rotor_mesh[16];
    char glass[16];
};

extern const struct eggman_machinery_texture_names
    eggman_machinery_texture_names;

extern NJS_TEXLIST eggman_panel_texlist;
extern NJS_TEXLIST eggman_rotor_texlist;
extern NJS_CNK_OBJECT eggman_glass_1;
extern NJS_CNK_OBJECT eggman_glass_2;
extern NJS_CNK_OBJECT eggman_glass_3;
extern NJS_CNK_OBJECT eggman_glass_4;
/* @hack type: declared as an array of one, where a single object would do */
extern NJS_CNK_OBJECT eggman_rotor_assembly[];
extern NJS_CNK_OBJECT eggman_rotor_fast_a;
extern NJS_CNK_OBJECT eggman_rotor_fast_b;
extern NJS_CNK_OBJECT eggman_rotor_medium_a;
extern NJS_CNK_OBJECT eggman_rotor_medium_b;
extern NJS_CNK_OBJECT eggman_rotor_slow_a;
extern NJS_CNK_OBJECT eggman_rotor_slow_b;
extern NJS_CNK_OBJECT eggman_rotor_reverse_a;
extern NJS_CNK_OBJECT eggman_rotor_reverse_b;
extern NJS_CNK_OBJECT eggman_rotor_reverse_c;
extern NJS_CNK_OBJECT eggman_rotor_return_a;
extern NJS_CNK_OBJECT eggman_rotor_return_b;

extern NJS_TEXLIST eggman_pipe_texlist;
extern NJS_TEXLIST eggman_joint_texlist;
/* @hack type: the pipe's and the joint's objects and motions declared as
 * arrays of one, where single objects would do */
extern NJS_CNK_OBJECT eggman_pipe[];
extern NJS_CNK_OBJECT eggman_joint[];
extern NJS_MOTION eggman_pipe_motion[], eggman_pipe_shape[];
extern NJS_MOTION eggman_joint_motion[], eggman_joint_shape[];
extern NJS_TEXLIST eggman_glow_texlist;
extern NJS_TEXLIST eggman_strip_texlist;
extern NJS_TEXLIST eggman_inner_lamp_texlist;
extern NJS_TEXLIST eggman_outer_lamp_texlist;
extern NJS_CNK_OBJECT *eggman_inner_lamps_a[6];
extern NJS_CNK_OBJECT *eggman_inner_lamps_b[6];
extern NJS_CNK_OBJECT *eggman_outer_lamps_a[15];
extern NJS_CNK_OBJECT *eggman_outer_lamps_b[12];
extern NJS_CNK_OBJECT *eggman_outer_lamps_c[12];
extern NJS_CNK_OBJECT *eggman_outer_lamps_d[15];
extern int eggman_hum_center, eggman_hum_amplitude, eggman_hum_period;
extern int eggman_lamp_rise, eggman_lamp_fall, eggman_lamp_delay;
extern Float eggman_lamp_red, eggman_lamp_green, eggman_lamp_blue;
extern Uint32 eggman_glass_tint, eggman_motion_tint;

#endif
