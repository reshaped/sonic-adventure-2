/* the tuning, models and shapes of drill.c's ride. a player rides the platform,
 * and the drill chases it with DRILL_TEETH teeth */
#ifndef STG43_DRILL_ASSETS_H
#define STG43_DRILL_ASSETS_H
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
struct task;

#define DRILL_TEETH 6
extern Float platform_slope_gain;
extern Float platform_drag;
extern Float platform_drive;
extern Float drill_drag;
extern Float drill_drive;
extern Float drill_near_drag;
extern NJS_POINT3 rider_seat;
extern NJS_TEXLIST platform_texlist;
extern NJS_CNK_OBJECT platform_body;
extern NJS_CNK_OBJECT platform_light[];
extern NJS_CNK_OBJECT platform_right_clamp;
extern NJS_CNK_OBJECT platform_left_clamp;
extern NJS_OBJECT platform_collision;
extern NJS_TEXLIST drill_body_texlist;
extern NJS_CNK_OBJECT drill_body[];
extern NJS_TEXLIST drill_head_texlist;
extern NJS_CNK_OBJECT drill_head[];
extern NJS_CNK_OBJECT *drill_teeth[DRILL_TEETH];
extern struct collision_shape tooth_shape[];
extern struct collision_shape drill_shape;
extern struct task *drill_triggers;
extern Uint32 drill_light_on;
extern Uint32 drill_light_off;
extern NJS_VECTOR launch_velocity;
extern NJS_POINT3 launch_target;
extern NJS_TEXLIST *cosmic_drill_texlists[];

#endif
