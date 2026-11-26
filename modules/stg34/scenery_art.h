/* the scenery of Sonic's route: its machinery, flames and water, their texture
 * lists and their animations' frames */
#ifndef STG34_SCENERY_ART_H
#define STG34_SCENERY_ART_H
#include <ninja.h>

extern const char cannons_core_scenery_filenames[258];
extern int core_rotor_tick;
extern NJS_TEXLIST core_sonic_background_textures;
extern NJS_TEXLIST *core_sonic_background_lists[];
extern NJS_TEXLIST cylinder_lamp_textures;
extern NJS_TEXLIST cylinder_textures;
extern NJS_TEXLIST entrance_unit_textures;
extern NJS_TEXLIST entrance_flame_textures;
extern NJS_TEXLIST rotor_flame_textures;
extern NJS_TEXLIST rotor_wall_textures;
extern NJS_TEXLIST flicker_lamp_textures;
extern NJS_TEXLIST shaft_flame_textures;
extern NJS_TEXLIST shaft_unit_textures;
extern NJS_TEXLIST shaft_motion_textures;
extern NJS_TEXLIST shutter_textures;
extern NJS_TEXLIST fan_textures;
extern NJS_TEXLIST fan_wall_textures;
extern NJS_TEXLIST water_unit_textures;
extern NJS_TEXLIST upper_channel_textures;
extern NJS_TEXLIST middle_channel_textures;
extern NJS_TEXLIST lower_channel_textures;
extern NJS_CNK_OBJECT middle_wall_rotor;
extern NJS_CNK_OBJECT middle_wall_drop;
extern NJS_CNK_OBJECT outer_wall_drop;
extern NJS_CNK_OBJECT entrance_units;
extern NJS_CNK_OBJECT entrance_flames;
extern NJS_CNK_OBJECT left_flames;
extern NJS_CNK_OBJECT right_flames;
/* @hack type: declared as an array of one, where a single object would do */
extern NJS_CNK_OBJECT cylinder_lamps[1];
extern NJS_CNK_OBJECT rotor_flames;
extern NJS_CNK_OBJECT rotor_walls;
extern NJS_CNK_OBJECT shaft_flames;
extern NJS_CNK_OBJECT shaft_units;
/* @hack type: declared as an array of one, where a single object would do */
extern NJS_CNK_OBJECT flicker_lamps[1];
extern NJS_CNK_OBJECT fan_walls;
extern NJS_CNK_OBJECT water_units;
extern NJS_CNK_OBJECT upper_channel_entry;
extern NJS_CNK_OBJECT upper_channel_exit;
extern NJS_CNK_OBJECT middle_channel_entry;
extern NJS_CNK_OBJECT middle_channel_exit;
extern NJS_CNK_OBJECT lower_channel_entry;
extern NJS_CNK_OBJECT lower_channel_exit;
extern NJS_CNK_OBJECT water_turn_entry;
extern NJS_CNK_OBJECT water_turn_exit;
extern NJS_CNK_OBJECT waterfalls;
extern NJS_CNK_OBJECT falling_water;
extern NJS_CNK_OBJECT water_upper;
extern NJS_CNK_OBJECT water_middle;
extern NJS_CNK_OBJECT water_lower;
extern NJS_CNK_OBJECT water_end;
extern NJS_CNK_OBJECT cylinder_body;
extern NJS_CNK_OBJECT shaft_assembly;
extern NJS_CNK_OBJECT shutter_odd;
extern NJS_CNK_OBJECT shutter_even;
extern NJS_CNK_OBJECT bobbing_water;
extern NJS_MOTION cylinder_motion;
extern NJS_MOTION core_shaft_motion;
extern NJS_CNK_MODEL rotor_highlight;
extern NJS_CNK_MODEL rotor_body;
/* @hack type: declared as an array of one, where a single model would do */
extern NJS_CNK_MODEL reverse_fan[1];
/* @hack type: declared as an array of one, where a single model would do */
extern NJS_CNK_MODEL bobbing_water_model[1];
extern NJS_CNK_MODEL *entrance_unit_frames[16];
extern NJS_CNK_MODEL *entrance_flame_frames[49];
extern NJS_CNK_MODEL *rotor_flame_frames[79];
extern NJS_CNK_MODEL *rotor_wall_frames[8];
extern NJS_CNK_MODEL *shaft_flame_frames[80];
extern NJS_CNK_MODEL *shaft_unit_frames[16];
extern NJS_CNK_MODEL *fan_wall_frames[8];
extern NJS_CNK_MODEL *water_unit_frames[16];
extern NJS_CNK_MODEL *upper_channel_frames[16];
extern NJS_CNK_MODEL *middle_channel_frames[16];
extern NJS_CNK_MODEL *lower_channel_frames[16];
extern NJS_CNK_MODEL *waterfall_frames[60];
extern NJS_CNK_MODEL *falling_water_frames[30];

#endif
