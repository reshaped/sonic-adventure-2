/* the monkey's model: monkey_object, its body, and the tail, limbs and head
 * below it */
#ifndef SHARED_MONKEY_ART_BODY_H
#define SHARED_MONKEY_ART_BODY_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 monkey_head_polygons[419] = {
#include "shared/monkey_art/body/monkey_head_polygons.inc"
};

static Sint32 monkey_head_vertices[237] = {
#include "shared/monkey_art/body/monkey_head_vertices.inc"
};

static NJS_CNK_MODEL monkey_head_model = {
#include "shared/monkey_art/body/monkey_head_model.inc"
};

static NJS_CNK_OBJECT monkey_head = {
#include "shared/monkey_art/body/monkey_head.inc"
};

static Sint16 monkey_left_hand_polygons[159] = {
#include "shared/monkey_art/body/monkey_left_hand_polygons.inc"
};

static Sint32 monkey_left_hand_vertices[87] = {
#include "shared/monkey_art/body/monkey_left_hand_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_hand_model = {
#include "shared/monkey_art/body/monkey_left_hand_model.inc"
};

static NJS_CNK_OBJECT monkey_left_hand = {
#include "shared/monkey_art/body/monkey_left_hand.inc"
};

static Sint16 monkey_left_forearm_polygons[59] = {
#include "shared/monkey_art/body/monkey_left_forearm_polygons.inc"
};

static Sint32 monkey_left_forearm_vertices[39] = {
#include "shared/monkey_art/body/monkey_left_forearm_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_forearm_model = {
#include "shared/monkey_art/body/monkey_left_forearm_model.inc"
};

static NJS_CNK_OBJECT monkey_left_forearm = {
#include "shared/monkey_art/body/monkey_left_forearm.inc"
};

static Sint16 monkey_left_upper_arm_polygons[53] = {
#include "shared/monkey_art/body/monkey_left_upper_arm_polygons.inc"
};

static Sint32 monkey_left_upper_arm_vertices[39] = {
#include "shared/monkey_art/body/monkey_left_upper_arm_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_upper_arm_model = {
#include "shared/monkey_art/body/monkey_left_upper_arm_model.inc"
};

static NJS_CNK_OBJECT monkey_left_upper_arm = {
#include "shared/monkey_art/body/monkey_left_upper_arm.inc"
};

static Sint16 monkey_right_hand_polygons[159] = {
#include "shared/monkey_art/body/monkey_right_hand_polygons.inc"
};

static Sint32 monkey_right_hand_vertices[87] = {
#include "shared/monkey_art/body/monkey_right_hand_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_hand_model = {
#include "shared/monkey_art/body/monkey_right_hand_model.inc"
};

static NJS_CNK_OBJECT monkey_right_hand = {
#include "shared/monkey_art/body/monkey_right_hand.inc"
};

static Sint16 monkey_right_forearm_polygons[59] = {
#include "shared/monkey_art/body/monkey_right_forearm_polygons.inc"
};

static Sint32 monkey_right_forearm_vertices[39] = {
#include "shared/monkey_art/body/monkey_right_forearm_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_forearm_model = {
#include "shared/monkey_art/body/monkey_right_forearm_model.inc"
};

static NJS_CNK_OBJECT monkey_right_forearm = {
#include "shared/monkey_art/body/monkey_right_forearm.inc"
};

static Sint16 monkey_right_upper_arm_polygons[53] = {
#include "shared/monkey_art/body/monkey_right_upper_arm_polygons.inc"
};

static Sint32 monkey_right_upper_arm_vertices[39] = {
#include "shared/monkey_art/body/monkey_right_upper_arm_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_upper_arm_model = {
#include "shared/monkey_art/body/monkey_right_upper_arm_model.inc"
};

static NJS_CNK_OBJECT monkey_right_upper_arm = {
#include "shared/monkey_art/body/monkey_right_upper_arm.inc"
};

static Sint16 monkey_right_foot_polygons[145] = {
#include "shared/monkey_art/body/monkey_right_foot_polygons.inc"
};

static Sint32 monkey_right_foot_vertices[69] = {
#include "shared/monkey_art/body/monkey_right_foot_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_foot_model = {
#include "shared/monkey_art/body/monkey_right_foot_model.inc"
};

static NJS_CNK_OBJECT monkey_right_foot = {
#include "shared/monkey_art/body/monkey_right_foot.inc"
};

static Sint16 monkey_right_shin_polygons[63] = {
#include "shared/monkey_art/body/monkey_right_shin_polygons.inc"
};

static Sint32 monkey_right_shin_vertices[39] = {
#include "shared/monkey_art/body/monkey_right_shin_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_shin_model = {
#include "shared/monkey_art/body/monkey_right_shin_model.inc"
};

static NJS_CNK_OBJECT monkey_right_shin = {
#include "shared/monkey_art/body/monkey_right_shin.inc"
};

static Sint16 monkey_right_thigh_polygons[39] = {
#include "shared/monkey_art/body/monkey_right_thigh_polygons.inc"
};

static Sint32 monkey_right_thigh_vertices[39] = {
#include "shared/monkey_art/body/monkey_right_thigh_vertices.inc"
};

static NJS_CNK_MODEL monkey_right_thigh_model = {
#include "shared/monkey_art/body/monkey_right_thigh_model.inc"
};

static NJS_CNK_OBJECT monkey_right_thigh = {
#include "shared/monkey_art/body/monkey_right_thigh.inc"
};

static Sint16 monkey_left_foot_polygons[145] = {
#include "shared/monkey_art/body/monkey_left_foot_polygons.inc"
};

static Sint32 monkey_left_foot_vertices[69] = {
#include "shared/monkey_art/body/monkey_left_foot_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_foot_model = {
#include "shared/monkey_art/body/monkey_left_foot_model.inc"
};

static NJS_CNK_OBJECT monkey_left_foot = {
#include "shared/monkey_art/body/monkey_left_foot.inc"
};

static Sint16 monkey_left_shin_polygons[63] = {
#include "shared/monkey_art/body/monkey_left_shin_polygons.inc"
};

static Sint32 monkey_left_shin_vertices[39] = {
#include "shared/monkey_art/body/monkey_left_shin_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_shin_model = {
#include "shared/monkey_art/body/monkey_left_shin_model.inc"
};

static NJS_CNK_OBJECT monkey_left_shin = {
#include "shared/monkey_art/body/monkey_left_shin.inc"
};

static Sint16 monkey_left_thigh_polygons[39] = {
#include "shared/monkey_art/body/monkey_left_thigh_polygons.inc"
};

static Sint32 monkey_left_thigh_vertices[39] = {
#include "shared/monkey_art/body/monkey_left_thigh_vertices.inc"
};

static NJS_CNK_MODEL monkey_left_thigh_model = {
#include "shared/monkey_art/body/monkey_left_thigh_model.inc"
};

static NJS_CNK_OBJECT monkey_left_thigh = {
#include "shared/monkey_art/body/monkey_left_thigh.inc"
};

static Sint16 monkey_tail_polygons[169] = {
#include "shared/monkey_art/body/monkey_tail_polygons.inc"
};

static Sint32 monkey_tail_vertices[135] = {
#include "shared/monkey_art/body/monkey_tail_vertices.inc"
};

static NJS_CNK_MODEL monkey_tail_model = {
#include "shared/monkey_art/body/monkey_tail_model.inc"
};

static NJS_CNK_OBJECT monkey_tail = {
#include "shared/monkey_art/body/monkey_tail.inc"
};

static Sint16 monkey_body_polygons[125] = {
#include "shared/monkey_art/body/monkey_body_polygons.inc"
};

static Sint32 monkey_body_vertices[75] = {
#include "shared/monkey_art/body/monkey_body_vertices.inc"
};

static NJS_CNK_MODEL monkey_body_model = {
#include "shared/monkey_art/body/monkey_body_model.inc"
};

static NJS_CNK_OBJECT monkey_object[1] = {
#include "shared/monkey_art/body/monkey_object.inc"
};

#endif
