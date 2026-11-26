/* the scenery of Cannon's Core's routes. it holds texture lists, the shaft's
 * walls and rotor, and the rising and falling columns. a route that uses a name
 * defines its own */
#ifndef SHARED_CORE_SCENERY_ART_H
#define SHARED_CORE_SCENERY_ART_H
#include <ninja.h>

/* the textures in core_block_glow_texlist */
#define GLOW_TEXTURES 13
extern NJS_TEXLIST core_block_glow_texlist;
extern NJS_TEXLIST surface_textures;
extern NJS_TEXLIST column_textures;
extern NJS_TEXLIST shaft_textures;
extern const char shaft_rotor_texture_name[12];
extern NJS_TEXLIST rotor_textures;
extern NJS_TEXLIST poison_wall_textures;
extern NJS_TEXLIST inner_wall_textures;
extern NJS_TEXLIST middle_wall_textures;
extern NJS_TEXLIST outer_wall_textures;
extern NJS_CNK_OBJECT rising_columns;
extern NJS_CNK_OBJECT falling_columns;
extern NJS_CNK_OBJECT shaft_entry_walls;
extern NJS_CNK_OBJECT shaft_walls;
extern NJS_CNK_OBJECT shaft_rotor;
extern NJS_CNK_OBJECT inner_wall_entry;
extern NJS_CNK_OBJECT inner_wall_shaft;
extern NJS_CNK_OBJECT middle_wall_shaft;
extern NJS_CNK_OBJECT middle_wall_lower;
extern NJS_CNK_OBJECT outer_wall_shaft;
extern NJS_CNK_OBJECT outer_wall_lower;
extern NJS_MOTION shaft_rotor_motion;
/* the frames of each wall's model */
#define WALL_FRAMES 16
extern NJS_CNK_MODEL *inner_wall_frames[WALL_FRAMES];
extern NJS_CNK_MODEL *middle_wall_frames[WALL_FRAMES];
extern NJS_CNK_MODEL *outer_wall_frames[WALL_FRAMES];
extern NJS_CNK_OBJECT middle_wall_entry;
extern NJS_CNK_OBJECT outer_wall_entry;

#endif
