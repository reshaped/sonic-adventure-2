/* helpers Dry Lagoon's objects share */
#ifndef STG18_LAGOON_UTIL_H
#define STG18_LAGOON_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the land block masks lagoon_camera_blocks gives, named for where the
 * current camera stands. south lies toward -z, and west toward -x */
#define SOUTH_WEST_BLOCKS 0x2
#define SOUTH_EAST_BLOCKS 0x100000
#define WEST_BLOCKS 0x800
#define EAST_BLOCKS 0x1000
#define LOW_BLOCKS 0x200000
#define HALL_BLOCKS 0x400000
#define MIDDLE_BLOCKS 0x400

/* whether task's player is a Rouge that glides, punches, spirals or dives.
 * the player is the one in the slot player_slot_of gives for task. that slot
 * must be 0 or more, and its work and record must not be null, unchecked */
Bool rouge_attacking(struct task *task);

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void lagoon_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* the scale a Dry Lagoon prop draws at: its work's scl.x plus 1. a scl.x of 0
 * draws the art as built */
#define LAGOON_SCALE(work) ((work)->scl.x + 1.0F)

/* draw model scaled. lagoon_model_draw_scaled and lagoon_direct_draw_scaled
 * scale by scale along each axis. the _v calls scale by scale's x, y and z.
 * the model_ calls draw with njCnkEasyDrawModel, and the direct_ calls draw a
 * build with njDirectDrawObject.
 *
 * each call scales the current matrix, as njScale does, and leaves it so.
 * for the draw, model->r is scaled by the largest factor. any clip test in the
 * draw takes the scaled r. the call then puts r back */
void lagoon_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void lagoon_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void lagoon_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale);
void lagoon_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale);

/* the land block mask for where the current camera stands, one of the
 * _BLOCKS above */
Sint32 lagoon_camera_blocks(void);
/* set camera 0's land_block_mask to lagoon_camera_blocks() */
void lagoon_land_clip(void);
/* the water's height where the current camera stands. it is 0 in the south's
 * land blocks, and 245 in the east's and the west's. it is 180 in the rest */
Float lagoon_water_level(void);

/* the short turn from one angle to another, from -0x8000 to 0x7fff */
Angle lagoon_angle_turn(Angle from, Angle to);
/* return the WORLD_SIZE_CLASS flags of the class that size falls in.
 * collision/world.h gives each class's bounds */
Sint32 lagoon_size_class_flags(Float size);
/* turn (x, 0, z) by ang about y, as njRotateY turns a point. point->x and
 * point->z take the result's x and z. point->y keeps its value */
void lagoon_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point);
/* the angle about y that turns the z axis toward the point (x, 0, z) */
Angle lagoon_heading(Float x, Float z);
/* whether the current camera's view dips below height. the point it tests is
 * the middle of the view's bottom edge, at a depth of 3. the view is 70
 * degrees across a 640 by 480 screen. it turns by the camera's ang.x alone.
 * the call returns FALSE where the camera lies below height.
 *
 * the call leaves the current matrix as it found it. Ninja's matrix stack must
 * have room for one more matrix, unchecked */
Bool lagoon_view_dips_below(Float height);

/* pulse player's pad count times, interval steps apart. each pulse is
 * vibration_start(player, power, freq, 0). a task named VibSlightExec, at task
 * level 2, makes the pulses. it pulses on its first step, and ends interval - 1
 * steps after its last pulse. an interval of 0 counts as 1. where the heap
 * can't give the task, nothing pulses.
 *
 * count must be 1 or more, interval 0 or more, and power 0 or more. with a
 * count of 0 and an interval above 1, the task pulses once. with a count of 0
 * and an interval of 0 or 1, it pulses every step forever. the task keeps
 * power in a Uint16, so a negative power reaches vibration_start as 65536
 * plus power */
void lagoon_vib_slight_create(int player, int power, int freq, int interval,
                              int count);

#endif
