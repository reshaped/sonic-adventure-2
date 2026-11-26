/* the models, texture lists and collision objects of Mad Space's set objects,
 * and its kill area's object */
#ifndef STG44_SCENERY_SHAPES_H
#define STG44_SCENERY_SHAPES_H
#include <ninja.h>

extern NJS_OBJECT mad_space_kill_object;

extern NJS_TEXLIST mad_space_engine_texlist;
extern NJS_CNK_OBJECT mad_space_engine_body[];
extern NJS_TEXLIST mad_space_trailer_texlist;
extern NJS_CNK_OBJECT mad_space_trailer_body[];
extern NJS_OBJECT mad_space_engine_collision;
extern NJS_OBJECT mad_space_trailer_collision;
extern NJS_CNK_OBJECT mad_space_engine_shadow;
extern NJS_CNK_OBJECT mad_space_trailer_shadow;

extern NJS_TEXLIST mad_space_platform_texlist;
extern NJS_CNK_OBJECT mad_space_platform_body[];
extern NJS_OBJECT mad_space_platform_collision;
extern NJS_CNK_OBJECT mad_space_platform_shadow;

extern NJS_TEXLIST mad_space_shutter_texlist;
extern NJS_CNK_OBJECT mad_space_shutter_first_light[];
extern NJS_CNK_OBJECT mad_space_shutter_second_light[];
extern NJS_CNK_OBJECT mad_space_shutter_housing[];
extern NJS_OBJECT mad_space_shutter_collision[];

extern NJS_TEXLIST mad_space_marker_a_texlist;
extern NJS_CNK_OBJECT mad_space_marker_a_object;
extern NJS_TEXLIST mad_space_marker_b_texlist;
extern NJS_CNK_OBJECT mad_space_marker_b_object;
extern NJS_TEXLIST mad_space_marker_c_texlist;
extern NJS_CNK_OBJECT mad_space_marker_c_object;
extern NJS_TEXLIST mad_space_marker_d_texlist;
extern NJS_CNK_OBJECT mad_space_marker_d_object;
extern NJS_OBJECT mad_space_marker_collision;

extern NJS_TEXLIST mad_space_ore_large_texlist;
extern NJS_CNK_OBJECT mad_space_ore_large_object[];
extern NJS_TEXLIST mad_space_ore_large_cracked_texlist;
extern NJS_CNK_OBJECT mad_space_ore_large_cracked_object[];
extern NJS_TEXLIST mad_space_ore_small_texlist;
extern NJS_CNK_OBJECT mad_space_ore_small_object[];
extern NJS_TEXLIST mad_space_ore_small_cracked_texlist;
extern NJS_CNK_OBJECT mad_space_ore_small_cracked_object[];
extern NJS_TEXLIST mad_space_ore_first_chip_texlist;
extern NJS_CNK_OBJECT mad_space_ore_first_chip[];
extern NJS_TEXLIST mad_space_ore_second_chip_texlist;
extern NJS_CNK_OBJECT mad_space_ore_second_chip[];
extern NJS_TEXLIST mad_space_ore_third_chip_texlist;
extern NJS_CNK_OBJECT mad_space_ore_third_chip[];

extern NJS_CNK_OBJECT mad_space_sl_first_object;
extern NJS_CNK_OBJECT mad_space_sl_second_object;
extern NJS_CNK_OBJECT mad_space_sl_third_object;

extern NJS_TEXLIST mad_space_foothold_texlist;
extern NJS_CNK_OBJECT mad_space_foothold_object;
extern NJS_TEXLIST mad_space_foothold_alt_texlist;
extern NJS_CNK_OBJECT mad_space_foothold_alt_object;
extern NJS_OBJECT mad_space_foothold_collision;

extern NJS_CNK_OBJECT landing_model[];
extern NJS_OBJECT landing_dust_collision[];
extern NJS_OBJECT landing_collision[];

extern NJS_CNK_OBJECT mad_space_pole_object;

extern NJS_CNK_OBJECT planter_object;

extern NJS_CNK_OBJECT mad_space_line_object;

extern NJS_CNK_OBJECT mad_space_fence_object;

#endif
