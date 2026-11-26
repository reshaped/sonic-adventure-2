/* the models, texture lists and collision objects of Meteor Herd's set objects,
 * and its kill area's object */
#ifndef STG32_SCENERY_SHAPES_H
#define STG32_SCENERY_SHAPES_H
#include <ninja.h>

extern NJS_OBJECT meteor_herd_kill_object;

extern NJS_CNK_OBJECT herd_beam_object[];

extern NJS_TEXLIST meteor_container_texlist;
extern NJS_CNK_OBJECT meteor_container_lamp[];
extern NJS_CNK_OBJECT meteor_container_housing[];
extern NJS_CNK_OBJECT meteor_container_shadow;
extern NJS_OBJECT meteor_container_collision;
extern NJS_TEXLIST meteor_container_side_texlist;
extern NJS_CNK_MODEL meteor_container_side;
extern NJS_TEXLIST meteor_container_corner_texlist;
extern NJS_CNK_MODEL meteor_container_corner;
extern NJS_TEXLIST meteor_container_lid_texlist;
extern NJS_CNK_MODEL meteor_container_lid;

extern NJS_TEXLIST large_meteor_texlist;
extern NJS_CNK_OBJECT large_meteor_beacon[];
extern NJS_CNK_OBJECT large_meteor_lamp[];
extern NJS_CNK_OBJECT large_meteor_body[];
extern NJS_TEXLIST large_meteor_shard_texlist;
extern NJS_CNK_OBJECT large_meteor_shard[];
extern NJS_TEXLIST large_meteor_chip_texlist;
extern NJS_CNK_OBJECT large_meteor_chip[];

/* @hack type: declared as an array of one, where a single object would do */
extern NJS_CNK_OBJECT meteo_bar_model[];
extern NJS_OBJECT meteo_bar_collision;

extern NJS_CNK_OBJECT herd_drill_head[];
extern NJS_CNK_OBJECT herd_drill_base[];

extern NJS_TEXLIST herd_iris_texlist;
extern NJS_CNK_OBJECT herd_iris_object[];
extern NJS_OBJECT herd_iris_collision[];

extern NJS_TEXLIST meteor_shutter_texlist;
extern NJS_CNK_OBJECT meteor_shutter_first_light[];
extern NJS_CNK_OBJECT meteor_shutter_second_light[];
extern NJS_CNK_OBJECT meteor_shutter_housing[];
extern NJS_OBJECT meteor_shutter_collision[];
extern NJS_TEXLIST meteor_shutter_broken_texlist;
extern NJS_CNK_OBJECT meteor_shutter_broken[];
extern NJS_OBJECT meteor_shutter_broken_collision[];

extern NJS_CNK_OBJECT herd_small_marker[];
extern NJS_CNK_OBJECT herd_large_marker[];
extern NJS_OBJECT herd_marker_collision[];

extern NJS_CNK_OBJECT herd_pole_object[];

extern NJS_TEXLIST meteor_tank_texlist;
extern NJS_CNK_OBJECT meteor_tank_object[];
extern NJS_CNK_OBJECT tank_base[];
extern NJS_TEXLIST tank_top_texlist;
extern NJS_CNK_MODEL tank_top_piece;
extern NJS_CNK_MODEL tank_side_piece;
extern NJS_TEXLIST tank_center_texlist;
extern NJS_CNK_MODEL tank_center_piece;
extern NJS_CNK_OBJECT tank_shadow[];

extern NJS_CNK_OBJECT herd_fence_object;

extern NJS_CNK_OBJECT meteor_platform_object[];
extern NJS_OBJECT meteor_platform_dusty_collision[];
extern NJS_OBJECT meteor_platform_collision[];

extern NJS_CNK_OBJECT meteor_platform_alt_object[];

#endif
