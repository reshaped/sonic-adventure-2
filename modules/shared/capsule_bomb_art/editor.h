#ifndef SHARED_CAPSULE_BOMB_ART_EDITOR_H
#define SHARED_CAPSULE_BOMB_ART_EDITOR_H

static const char damage_task_name[12] = {
#include "shared/capsule_bomb_art/editor/damage_task_name.inc"
};

/* the set editor's labels. this source's constants hold them ahead of
 * the capsule's texture names */
static const char editor_wait_enabled[16] = {
#include "shared/capsule_bomb_art/editor/editor_wait_enabled.inc"
};
static const char editor_wait_disabled[16] = {
#include "shared/capsule_bomb_art/editor/editor_wait_disabled.inc"
};
static const char editor_angle_y[12] = {
#include "shared/capsule_bomb_art/editor/editor_angle_y.inc"
};
static const char editor_speed[8] = {
#include "shared/capsule_bomb_art/editor/editor_speed.inc"
};
static const char editor_height[12] = {
#include "shared/capsule_bomb_art/editor/editor_height.inc"
};

#endif
