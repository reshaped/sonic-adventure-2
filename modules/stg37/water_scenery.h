/* the water shaft of Rouge's route: its art, and water_scenery.c's external
 * calls */
#ifndef STG37_WATER_SCENERY_H
#define STG37_WATER_SCENERY_H
#include <ninja.h>
#include "draw/texture_anim.h"

/* the frames in each animation's list */
enum {
    SURFACE_FRAMES = 32,
    ROTOR_WATER_FRAMES = 20,
    BUBBLE_FRAMES = 60,
    FIRST_SPLASH_FRAMES = 36,
    SECOND_SPLASH_FRAMES = 15,
    WATER_SPLASH_FRAMES = 16
};

extern NJS_TEXLIST falling_columns_textures;
extern NJS_TEXLIST shaft_walls_textures;
extern NJS_TEXLIST moving_water_textures;
/* the textures in moving_water_textures */
#define MOVING_WATER_TEXTURES 14
extern NJS_TEXLIST entry_light_textures;
extern NJS_TEXLIST rotor_water_textures;
extern NJS_TEXLIST lower_wall_textures;
extern NJS_TEXLIST bubble_textures;
extern NJS_TEXLIST splash_textures;
extern NJS_TEXLIST splash_water_textures;
extern NJS_CNK_OBJECT left_surface;
extern NJS_CNK_OBJECT right_surface;
extern NJS_CNK_OBJECT moving_water;
extern NJS_CNK_OBJECT lower_surface;
extern NJS_CNK_OBJECT poison_surface;
extern NJS_CNK_OBJECT poison_walls;
extern NJS_CNK_OBJECT lower_walls;
extern NJS_CNK_OBJECT rising_water;
extern NJS_CNK_MODEL entry_light;
extern NJS_CNK_MODEL moving_water_model;
extern NJS_CNK_MODEL poison_wall_model;
extern NJS_CNK_MODEL lower_wall_model;
extern struct texture_anim moving_water_uv;
extern NJS_CNK_OBJECT *surface_frames[SURFACE_FRAMES];
extern NJS_CNK_MODEL *rotor_water_frames[ROTOR_WATER_FRAMES];
extern NJS_CNK_MODEL *bubble_frames[BUBBLE_FRAMES];
extern NJS_CNK_MODEL *first_splash_frames[FIRST_SPLASH_FRAMES];
extern NJS_CNK_MODEL *second_splash_frames[SECOND_SPLASH_FRAMES];
extern NJS_CNK_MODEL *water_splash_frames[WATER_SPLASH_FRAMES];

struct task;
/* the step and the destructor of the cover's collision tasks, each of which
 * keeps its object at extra. the step sets WORLD_ADDED in the task's work, so
 * that world_collect_near_players doesn't pass over the object. the destructor
 * takes a non-null extra out of the world and gives it back to
 * world_pool_alloc. it then nulls extra */
void core_cover_collision_step(struct task *task);
void core_cover_collision_destroy(struct task *task);
/* draw a model at the place of each child of root. each draws only where
 * camera 0's land_block_mask has a bit of blocks. core_draw_child_models draws
 * each child's own model, and core_draw_at_children draws the model it is
 * given */
void core_draw_child_models(NJS_CNK_OBJECT *root, Uint32 blocks);
void core_draw_at_children(NJS_CNK_MODEL *model, NJS_CNK_OBJECT *root,
                           Uint32 blocks);
/* draw the model of each child of root at the current matrix, tinted as
 * model_draw_tinted tints. no code calls it. it is external so that SHC keeps
 * it */
void core_draw_tinted_children(NJS_CNK_OBJECT *root, Uint32 color);

extern NJS_TEXLIST cover_panel_textures;
extern NJS_CNK_OBJECT cover_panel_positions;

#endif
