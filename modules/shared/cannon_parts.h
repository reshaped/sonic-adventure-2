/* the automatic cannon's art. a shot draws with cannon_shot_bright's textures
 * for 19 steps in every 50, and with cannon_shot_dim's for the rest. it draws
 * cannon_shot_model's root, then the root's child and its three siblings,
 * the sparks. the sparks' scale grows with the shot's homing */
#ifndef SHARED_CANNON_PARTS_H
#define SHARED_CANNON_PARTS_H
#include <ninja.h>

extern NJS_TEXLIST cannon_shot_bright;
extern NJS_TEXLIST cannon_shot_dim;
extern NJS_TEXLIST cannon_texlist;
extern NJS_CNK_OBJECT cannon_shot_model;
extern NJS_CNK_MODEL cannon_barrel;

#endif
