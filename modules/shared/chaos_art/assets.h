/* the models, motions, collision shapes, paths and actions that enemy_chaos.c
 * reads. no code reads path_chaos_debug_titles */
#ifndef SHARED_CHAOS_ART_ASSETS_H
#define SHARED_CHAOS_ART_ASSETS_H
#include <ninja.h>
#include "collision/collision.h"
#include "enemy/enemy.h"
#include "stage/path.h"

extern const char path_chaos_debug_titles[271];

extern const char chaos_enemy_filenames[96];
extern NJS_TEXLIST chaos_texlist;
extern NJS_CNK_OBJECT normal_muzzle_node;
extern NJS_CNK_OBJECT normal_object[1];
extern NJS_MOTION normal_motion;
extern NJS_CNK_OBJECT normal_arm_b;
extern NJS_CNK_OBJECT normal_arm_a;
extern NJS_CNK_OBJECT normal_reach_muzzle_node;
extern NJS_CNK_OBJECT normal_reach_object;
extern NJS_CNK_OBJECT flash_a_object;
extern NJS_MOTION flash_a_motion;
extern NJS_MOTION flash_a_shape;
extern NJS_CNK_OBJECT flash_b_object;
extern NJS_MOTION flash_b_motion;
extern NJS_MOTION flash_b_shape;
extern NJS_CNK_OBJECT float_object[1];
extern NJS_MOTION float_shape;
extern NJS_MOTION float_motion;
extern NJS_CNK_OBJECT float_reach_muzzle_node;
extern NJS_CNK_OBJECT float_arm_a[1];
extern NJS_CNK_OBJECT float_arm_b[1];
extern NJS_CNK_OBJECT float_reach_object;
extern NJS_CNK_OBJECT piece_a_object;
extern NJS_CNK_OBJECT piece_b_object;
extern NJS_CNK_MODEL piece_model;
extern NJS_CNK_OBJECT shed_object;
extern NJS_MOTION shed_shape;
extern NJS_MOTION shed_end_shape;
extern NJS_CNK_OBJECT spent_object;
extern NJS_MOTION spent_motion;
extern NJS_CNK_OBJECT guard_object;
extern NJS_MOTION guard_shape;
extern NJS_MOTION guard_motion;
extern NJS_MOTION guard_shut_shape;
extern struct collision_shape chaos_shapes_round_reverse[2];
extern struct collision_shape chaos_shapes_reverse[18];
extern struct collision_shape chaos_shapes[18];
extern struct collision_shape chaos_shapes_float[18];
extern NJS_CNK_MODEL *chaos_pieces[1];
extern struct enemy_action chaos_actions[17];
extern struct collision_shape piece_shapes[1];
extern NJS_MOTION path_chaos_motion;
extern NJS_MOTION path_chaos_shape;
extern struct collision_shape path_chaos_shapes[2];
extern struct path *path_chaos_paths[12];
extern struct enemy_action path_chaos_actions[1];
extern NJS_POLYGON_VTX path_chaos_triangle[3];

#endif
