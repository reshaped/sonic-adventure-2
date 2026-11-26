/* the E-1000's motions in flight: flying, alert and shooting. each has a
 * channel for each of e1000_object's 14 nodes */
#ifndef SHARED_E1000_ART_FLIGHT_MOTIONS_H
#define SHARED_E1000_ART_FLIGHT_MOTIONS_H

#include <ninja.h>

static NJS_MKEY_F fly_body_position[2] = {
#include "shared/e1000_art/flight_motions/fly_body_position.inc"
};

static NJS_MKEY_A fly_body_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_body_rotation.inc"
};

static NJS_MKEY_F fly_housing_position[2] = {
#include "shared/e1000_art/flight_motions/fly_housing_position.inc"
};

static NJS_MKEY_A fly_housing_link_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_housing_link_rotation.inc"
};

static NJS_MKEY_A fly_high_thigh_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_high_thigh_rotation.inc"
};

static NJS_MKEY_A fly_high_shin_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_high_shin_rotation.inc"
};

static NJS_MKEY_A fly_high_foot_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_high_foot_rotation.inc"
};

static NJS_MKEY_A fly_gun_high_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_gun_high_rotation.inc"
};

static NJS_MKEY_A fly_barrel_high_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_barrel_high_rotation.inc"
};

static NJS_MKEY_A fly_low_thigh_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_low_thigh_rotation.inc"
};

static NJS_MKEY_A fly_low_shin_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_low_shin_rotation.inc"
};

static NJS_MKEY_A fly_low_foot_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_low_foot_rotation.inc"
};

static NJS_MKEY_A fly_gun_low_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_gun_low_rotation.inc"
};

static NJS_MKEY_A fly_barrel_low_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_fly_tracks[14] = {
#include "shared/e1000_art/flight_motions/e1000_fly_tracks.inc"
};

static NJS_MOTION e1000_fly_motion = {
#include "shared/e1000_art/flight_motions/e1000_fly_motion.inc"
};

static NJS_MKEY_F fly_alert_body_position[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_body_position.inc"
};

static NJS_MKEY_A fly_alert_body_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_body_rotation.inc"
};

static NJS_MKEY_F fly_alert_housing_position[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_housing_position.inc"
};

static NJS_MKEY_A fly_alert_housing_link_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_housing_link_rotation.inc"
};

static NJS_MKEY_A fly_alert_high_thigh_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_high_thigh_rotation.inc"
};

static NJS_MKEY_A fly_alert_high_shin_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_high_shin_rotation.inc"
};

static NJS_MKEY_A fly_alert_high_foot_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_high_foot_rotation.inc"
};

static NJS_MKEY_A fly_alert_gun_high_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_gun_high_rotation.inc"
};

static NJS_MKEY_A fly_alert_barrel_high_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_barrel_high_rotation.inc"
};

static NJS_MKEY_A fly_alert_low_thigh_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_low_thigh_rotation.inc"
};

static NJS_MKEY_A fly_alert_low_shin_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_low_shin_rotation.inc"
};

static NJS_MKEY_A fly_alert_low_foot_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_low_foot_rotation.inc"
};

static NJS_MKEY_A fly_alert_gun_low_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_gun_low_rotation.inc"
};

static NJS_MKEY_A fly_alert_barrel_low_rotation[2] = {
#include "shared/e1000_art/flight_motions/fly_alert_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_fly_alert_tracks[14] = {
#include "shared/e1000_art/flight_motions/e1000_fly_alert_tracks.inc"
};

static NJS_MOTION e1000_fly_alert_motion = {
#include "shared/e1000_art/flight_motions/e1000_fly_alert_motion.inc"
};

static NJS_MKEY_F fly_shoot_body_position[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_body_position.inc"
};

static NJS_MKEY_A fly_shoot_body_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_body_rotation.inc"
};

static NJS_MKEY_F fly_shoot_housing_position[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_housing_position.inc"
};

static NJS_MKEY_A fly_shoot_housing_link_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_housing_link_rotation.inc"
};

static NJS_MKEY_A fly_shoot_high_thigh_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_high_thigh_rotation.inc"
};

static NJS_MKEY_A fly_shoot_high_shin_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_high_shin_rotation.inc"
};

static NJS_MKEY_A fly_shoot_high_foot_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_high_foot_rotation.inc"
};

static NJS_MKEY_A fly_shoot_gun_high_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_gun_high_rotation.inc"
};

static NJS_MKEY_A fly_shoot_barrel_high_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_barrel_high_rotation.inc"
};

static NJS_MKEY_A fly_shoot_low_thigh_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_low_thigh_rotation.inc"
};

static NJS_MKEY_A fly_shoot_low_shin_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_low_shin_rotation.inc"
};

static NJS_MKEY_A fly_shoot_low_foot_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_low_foot_rotation.inc"
};

static NJS_MKEY_A fly_shoot_gun_low_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_gun_low_rotation.inc"
};

static NJS_MKEY_A fly_shoot_barrel_low_rotation[3] = {
#include "shared/e1000_art/flight_motions/fly_shoot_barrel_low_rotation.inc"
};

static NJS_MDATA2 e1000_fly_shoot_tracks[14] = {
#include "shared/e1000_art/flight_motions/e1000_fly_shoot_tracks.inc"
};

static NJS_MOTION e1000_fly_shoot_motion = {
#include "shared/e1000_art/flight_motions/e1000_fly_shoot_motion.inc"
};

#endif
