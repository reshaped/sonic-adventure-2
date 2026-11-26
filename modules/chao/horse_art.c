/* CHAO.PRS's chunk models, motions and motion clips, from the disc. its D
 * lies between jack_box.c and radio.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/toy_art.h"
#include "draw/motion.h"

static Sint16 horse_polygons[457] = {
#include "chao/horse_art/horse_polygons.inc"
};
static Sint32 horse_vertices[465] = {
#include "chao/horse_art/horse_vertices.inc"
};
static NJS_CNK_MODEL horse_mesh = {
#include "chao/horse_art/horse_mesh.inc"
};
static NJS_CNK_OBJECT moving_horse = {
#include "chao/horse_art/moving_horse.inc"
};
NJS_CNK_OBJECT garden_horse_model = {
#include "chao/horse_art/garden_horse_model.inc"
};
static NJS_MKEY_F horse_motion_joint_1_position[15] = {
#include "chao/horse_art/horse_motion_joint_1_position.inc"
};
static NJS_MKEY_A horse_motion_joint_1_angles[17] = {
#include "chao/horse_art/horse_motion_joint_1_angles.inc"
};
static NJS_MDATA2 horse_motion_tracks[2] = {
#include "chao/horse_art/horse_motion_tracks.inc"
};
static NJS_MOTION horse_motion = {
#include "chao/horse_art/horse_motion.inc"
};
struct motion_table_entry garden_horse_clips[5] = {
#include "chao/horse_art/garden_horse_clips.inc"
};
static Sint16 garden_radio_model_polygons[531] = {
#include "chao/horse_art/garden_radio_model_polygons.inc"
};
static Sint32 garden_radio_model_vertices[399] = {
#include "chao/horse_art/garden_radio_model_vertices.inc"
};
static NJS_CNK_MODEL garden_radio_model_model = {
#include "chao/horse_art/garden_radio_model_model.inc"
};
NJS_CNK_OBJECT garden_radio_model = {
#include "chao/horse_art/garden_radio_model.inc"
};
NJS_CNK_OBJECT garden_radio_root = {
#include "chao/horse_art/garden_radio_root.inc"
};
