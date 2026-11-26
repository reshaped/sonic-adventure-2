/* the path enemy's four kinds: their models, pieces, shapes, paths and
 * actions */
#ifndef SHARED_PATH_ENEMY_ART_H
#define SHARED_PATH_ENEMY_ART_H
#include <ninja.h>
#include "collision/collision.h"
#include "enemy/enemy.h"
#include "stage/path.h"

extern const char patrol_texture_filenames[62];

extern NJS_TEXLIST path_kumi_texlist;
extern NJS_TEXLIST path_kyoko_texlist;
extern NJS_CNK_OBJECT path_bombkumi_object[1];
extern NJS_CNK_OBJECT path_gunkumi_object[1];
extern NJS_CNK_OBJECT path_kyoko_object[1];
extern NJS_CNK_OBJECT path_backkyoko_object[1];
extern NJS_CNK_OBJECT path_kyoko_shadow_object;
extern struct collision_shape path_enemy_shapes[6];
extern NJS_CNK_MODEL *path_gunkumi_pieces[5];
extern NJS_CNK_MODEL *path_bombkumi_pieces[4];
extern NJS_CNK_MODEL *path_kyoko_pieces[12];
extern NJS_CNK_MODEL *path_backkyoko_pieces[12];
extern struct path *path_enemy_paths[12];
extern struct enemy_action path_enemy_actions[5];

#endif
