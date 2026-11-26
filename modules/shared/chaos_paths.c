#include <ninja.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "shared/chaos_art/assets.h"
#include "stage/path.h"

const char path_chaos_debug_titles[271] = {
#include "shared/chaos_paths/path_chaos_debug_titles.inc"
};

/* the text pointers, paths, motions, path list, enemy actions and polygon
 * vertices that 12 images place, from the disc. its D follows enemy_chaos.c */
static char *kind_titles[10] = {
#include "shared/chaos_paths/kind_titles.inc"
};
/* clang-format off */
struct collision_shape piece_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_UNTURNED | SHAPE_IGNORE_OBJECTS |
         SHAPE_UNK_800000,
     {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static struct path_point route_0_points[17] = {
#include "shared/chaos_paths/route_0_points.inc"
};
static struct path route_0 = {
#include "shared/chaos_paths/route_0.inc"
};
static struct path_point route_1_points[13] = {
#include "shared/chaos_paths/route_1_points.inc"
};
static struct path route_1 = {
#include "shared/chaos_paths/route_1.inc"
};
static struct path_point route_2_points[27] = {
#include "shared/chaos_paths/route_2_points.inc"
};
static struct path route_2 = {
#include "shared/chaos_paths/route_2.inc"
};
static struct path_point route_3_points[16] = {
#include "shared/chaos_paths/route_3_points.inc"
};
static struct path route_3 = {
#include "shared/chaos_paths/route_3.inc"
};
static struct path_point route_4_points[67] = {
#include "shared/chaos_paths/route_4_points.inc"
};
static struct path route_4 = {
#include "shared/chaos_paths/route_4.inc"
};
static struct path_point route_5_points[32] = {
#include "shared/chaos_paths/route_5_points.inc"
};
static struct path route_5 = {
#include "shared/chaos_paths/route_5.inc"
};
static struct path_point route_6_points[4] = {
#include "shared/chaos_paths/route_6_points.inc"
};
static struct path route_6 = {
#include "shared/chaos_paths/route_6.inc"
};
static struct path_point route_7_points[11] = {
#include "shared/chaos_paths/route_7_points.inc"
};
static struct path route_7 = {
#include "shared/chaos_paths/route_7.inc"
};
static struct path_point route_8_points[25] = {
#include "shared/chaos_paths/route_8_points.inc"
};
static struct path route_8 = {
#include "shared/chaos_paths/route_8.inc"
};
static struct path_point route_9_points[17] = {
#include "shared/chaos_paths/route_9_points.inc"
};
static struct path route_9 = {
#include "shared/chaos_paths/route_9.inc"
};
static struct path_point route_10_points[11] = {
#include "shared/chaos_paths/route_10_points.inc"
};
static struct path route_10 = {
#include "shared/chaos_paths/route_10.inc"
};
static struct path_point route_11_points[27] = {
#include "shared/chaos_paths/route_11_points.inc"
};
static struct path route_11 = {
#include "shared/chaos_paths/route_11.inc"
};
static NJS_MKEY_F path_chaos_motion_joint_0_position[2] = {
#include "shared/chaos_paths/path_chaos_motion_joint_0_position.inc"
};
static NJS_MKEY_A path_chaos_motion_joint_1_angles[2] = {
#include "shared/chaos_paths/path_chaos_motion_joint_1_angles.inc"
};
static NJS_MKEY_A path_chaos_motion_joint_2_angles[5] = {
#include "shared/chaos_paths/path_chaos_motion_joint_2_angles.inc"
};
static NJS_MKEY_A path_chaos_motion_joint_3_angles[4] = {
#include "shared/chaos_paths/path_chaos_motion_joint_3_angles.inc"
};
static NJS_MKEY_A path_chaos_motion_joint_4_angles[5] = {
#include "shared/chaos_paths/path_chaos_motion_joint_4_angles.inc"
};
static NJS_MDATA2 path_chaos_motion_tracks[5] = {
#include "shared/chaos_paths/path_chaos_motion_tracks.inc"
};
NJS_MOTION path_chaos_motion = {
#include "shared/chaos_paths/path_chaos_motion.inc"
};
static NJS_POINT3 path_chaos_shape_joint_0_points_frame_0[68] = {
#include "shared/chaos_paths/path_chaos_shape_joint_0_points_frame_0.inc"
};
static NJS_POINT3 path_chaos_shape_joint_0_points_frame_2[68] = {
#include "shared/chaos_paths/path_chaos_shape_joint_0_points_frame_2.inc"
};
static NJS_POINT3 path_chaos_shape_joint_0_points_frame_3[68] = {
#include "shared/chaos_paths/path_chaos_shape_joint_0_points_frame_3.inc"
};
static NJS_POINT3 path_chaos_shape_joint_0_points_frame_4[68] = {
#include "shared/chaos_paths/path_chaos_shape_joint_0_points_frame_4.inc"
};
static NJS_MKEY_P path_chaos_shape_joint_0_vertices[4] = {
#include "shared/chaos_paths/path_chaos_shape_joint_0_vertices.inc"
};
static NJS_MDATA2 path_chaos_shape_tracks[5] = {
#include "shared/chaos_paths/path_chaos_shape_tracks.inc"
};
NJS_MOTION path_chaos_shape = {
#include "shared/chaos_paths/path_chaos_shape.inc"
};
/* clang-format off */
struct collision_shape path_chaos_shapes[2] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES,
     {-2.0F, 25.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE,
     {-5.0F, 13.0F, 0.0F}, 13.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
struct path *path_chaos_paths[12] = {
#include "shared/chaos_paths/path_chaos_paths.inc"
};
struct enemy_action path_chaos_actions[1] = {
#include "shared/chaos_paths/path_chaos_actions.inc"
};
NJS_POLYGON_VTX path_chaos_triangle[3] = {
#include "shared/chaos_paths/path_chaos_triangle.inc"
};
