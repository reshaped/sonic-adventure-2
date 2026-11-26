/* the scenery art of Knuckles's route beyond shared/core_scenery_art.h's. it
 * also declares the points camera 0 looks from and at while a fence opens */
#ifndef STG38_WATER_SCENERY_H
#define STG38_WATER_SCENERY_H
#include <ninja.h>

extern NJS_TEXLIST fence_textures;
extern NJS_TEXLIST channel_textures;
extern NJS_TEXLIST upper_shaft_textures;
extern const char upper_rotor_texture_name[12];
extern const char knuckles_poison_frame_name[15];
extern NJS_TEXLIST upper_rotor_textures;
extern NJS_CNK_OBJECT surface_tiles;
extern NJS_CNK_OBJECT beam_fence;
extern NJS_CNK_OBJECT lower_poison;
extern NJS_CNK_OBJECT upper_poison;
extern NJS_CNK_OBJECT lower_poison_walls;
extern NJS_CNK_OBJECT upper_poison_walls;
extern NJS_CNK_OBJECT inner_wall_channel;
extern NJS_CNK_OBJECT inner_wall_exit;
extern NJS_CNK_OBJECT middle_wall_channel;
extern NJS_CNK_OBJECT middle_wall_exit;
extern NJS_CNK_OBJECT outer_wall_channel;
extern NJS_CNK_OBJECT outer_wall_exit;
extern NJS_CNK_OBJECT lower_channel;
extern NJS_CNK_OBJECT channel_turn;
extern NJS_CNK_OBJECT upper_channel;
extern NJS_CNK_OBJECT upper_shaft;
extern NJS_CNK_OBJECT upper_rotor;
extern NJS_MOTION upper_rotor_motion;
extern NJS_POINT3 first_fence_view;
extern NJS_POINT3 second_fence_view;
extern NJS_POINT3 first_fence_target;
extern NJS_POINT3 second_fence_target;

/* the y of the point camera_reach, a static of water_scenery.c, from the
 * current camera's pos toward its target. no code calls it. it is external so
 * that SHC keeps it */
Float core_camera_look_height(void);

#endif
