/* the moving scenery of Sonic's route and its colliders. the colliders stand
 * in for its pillars, its fans and a gate. each part of the scenery draws
 * only while camera 0's land_block_mask holds one of its land blocks. each
 * collider registers its shape under the same rule */
#ifndef STG34_SCENERY_H
#define STG34_SCENERY_H
#include <ninja.h>

/* the one shape of an electric pillar's collision, the scenery's or an
 * object's. it is an upright cylinder at the task's pos, whose radius and
 * size_y are radius and half_height */
/* clang-format off */
#define PILLAR_SHAPE(radius, half_height)                                      \
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),                      \
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, radius,         \
     half_height, 0.0F, 0, {0, 0, 0}}
/* clang-format on */
#define PILLAR_SHAPES 1

struct task;
extern NJS_TEXLIST water_surface_textures, waterfall_textures;
extern NJS_CNK_OBJECT core_pillar_upper, core_pillar_lower;
/* the fans' placements */
/* @hack type: declared as an array of one, where a single object would do */
extern NJS_CNK_OBJECT core_rotor_array[1];
/* the rotor's state, ROTOR_RESTING or ROTOR_TURNING */
extern int core_rotor_turning;
enum { ROTOR_RESTING, ROTOR_TURNING };
extern Angle core_rotor_angle;
/* the frames of a pillar's arc, which shows one a step */
#define PILLAR_FRAMES 30
extern NJS_CNK_MODEL *pillar_frames[PILLAR_FRAMES];
/* a value from 0 up to 1 for each group of flickering lamps. the background
 * picks each at random every step while time runs. the lamp at index i of its
 * placements is in group i % CORE_FLICKER_GROUPS */
#define CORE_FLICKER_GROUPS 8
extern Float core_flicker[CORE_FLICKER_GROUPS];

/* the background task's draw. it also sets the texture each water model
 * shows, in the model's polygon list */
void core_scenery_draw(struct task *task);
/* create a collider at each child of placements. it registers its shape
 * while camera 0's land_block_mask holds one of blocks. a pillar's shape is
 * PILLAR_SHAPE, and a fan's a disc about x. each call stops at the first
 * task it can't create */
void core_pillars_start(NJS_CNK_OBJECT *placements, Sint32 blocks);
void core_fans_start(NJS_CNK_OBJECT *placements, Sint32 blocks);
/* create the gate's collider, a wall, which registers its shape while camera
 * 0's land_block_mask holds one of blocks */
void core_gate_start(Sint32 blocks);
/* the step of a gate's collider with both of gate_shapes: the wall, and a bar
 * along its foot. it sets the bar's height and registers both shapes. it does
 * so only while camera 0's land_block_mask holds one of the task's blocks. no
 * code names this step. it is external so that SHC keeps it */
void core_barred_gate_step(struct task *task);
/* create the entry's trigger and turn player 0's pad off. the pad stays off
 * until a Sonic touches the trigger, which then turns it on and dies */
void core_entry_trigger_start(void);

#endif
