/* the E AI's art, for enemy_ai.c: three models, the pieces a beaten AI breaks
 * into and ai_actions' motions */
#ifndef SHARED_ENEMY_AI_ART_H
#define SHARED_ENEMY_AI_ART_H
#include <ninja.h>

/* the textures of every kind, which ai_setup loads from E_AITEX */
extern NJS_TEXLIST ai_texlist;
/* the model of the gun and capturing kinds and of AI_ONLY_STAND */
/* @hack type: ai_gun_object declared as an array, where a single object would
 * do */
extern NJS_CNK_OBJECT ai_gun_object[];
extern NJS_CNK_OBJECT ai_laser_object; /* the laser kinds' model */
extern NJS_CNK_OBJECT ai_shielder_object; /* the shielder's model */
/* the part of each model that holds the gun */
extern NJS_CNK_OBJECT ai_gun_weapon_object;
extern NJS_CNK_OBJECT ai_laser_weapon_object;
extern NJS_CNK_OBJECT ai_shielder_weapon_object;
/* the pieces a beaten AI of any kind breaks into. all are parts of the gun
 * kinds' model */
extern NJS_CNK_MODEL ai_gun_right_foot_model;
extern NJS_CNK_MODEL ai_gun_right_shin_model;
extern NJS_CNK_MODEL ai_gun_right_thigh_model;
extern NJS_CNK_MODEL ai_gun_right_hand_model;
extern NJS_CNK_MODEL ai_gun_right_forearm_model;
extern NJS_CNK_MODEL ai_gun_right_upper_arm_model;
extern NJS_CNK_MODEL ai_gun_left_foot_model;
extern NJS_CNK_MODEL ai_gun_left_shin_model;
extern NJS_CNK_MODEL ai_gun_left_thigh_model;
extern NJS_CNK_MODEL ai_gun_left_forearm_model;
extern NJS_CNK_MODEL ai_gun_left_upper_arm_model;
extern NJS_CNK_MODEL ai_gun_weapon_model;
extern NJS_CNK_MODEL ai_gun_weapon_mount_model;
extern NJS_CNK_MODEL ai_gun_body_model;
/* the motions of ai_actions */
extern NJS_MOTION ai_land_motion;
extern NJS_MOTION ai_rise_motion;
extern NJS_MOTION ai_stand_motion;
extern NJS_MOTION ai_shoot_motion;
extern NJS_MOTION ai_turn_motion;
extern NJS_MOTION ai_unused_turn_motion;
extern NJS_MOTION ai_fly_motion;
extern NJS_MOTION ai_fly_shoot_motion;
extern NJS_MOTION ai_shielder_open_motion;
extern NJS_MOTION ai_shielder_close_motion;
extern NJS_MOTION ai_shielder_deflect_motion;
extern NJS_MOTION ai_shielder_stand_motion;
extern NJS_MOTION ai_shielder_turn_motion;

#endif
