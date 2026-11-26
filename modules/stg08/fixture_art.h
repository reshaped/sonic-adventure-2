/* the models, texture lists and solids of Security Hall's own objects */
#ifndef STG08_FIXTURE_ART_H
#define STG08_FIXTURE_ART_H

#include <ninja.h>

extern NJS_TEXLIST security_laser_texlist;
/* @hack type: each object declared [1] below, an array of one, where a single
 * object would do */
extern NJS_CNK_OBJECT security_laser_left[1];
extern NJS_CNK_OBJECT security_laser_right[1];
extern NJS_CNK_OBJECT security_laser_body[1];

extern NJS_TEXLIST security_tank_texlist;
extern NJS_CNK_OBJECT security_tank_object[1];
extern NJS_TEXLIST security_tank_rim_texlist;
extern NJS_CNK_MODEL security_tank_rim_model;
extern NJS_TEXLIST security_tank_base_texlist;
extern NJS_CNK_MODEL security_tank_base_model;
extern NJS_TEXLIST security_tank_side_texlist;
extern NJS_CNK_MODEL security_tank_side_model;

extern NJS_CNK_OBJECT security_cylinder_object[1];
extern NJS_CNK_MODEL security_cylinder_cap_model;
extern NJS_CNK_MODEL security_cylinder_wall_model;
extern NJS_CNK_MODEL security_cylinder_base_model;

extern NJS_CNK_OBJECT security_fan_object[1];
extern NJS_CNK_OBJECT security_panel_a_object;
extern NJS_CNK_OBJECT security_panel_b_object;
extern NJS_CNK_OBJECT security_panel_c_object;

extern NJS_CNK_OBJECT security_lamp_object[1];

extern NJS_CNK_MODEL security_container_model;
extern NJS_OBJECT security_container_collision;

extern NJS_CNK_OBJECT security_money_object[1];

extern NJS_CNK_MODEL security_platform_model;
extern NJS_OBJECT security_platform_collision;

#endif
