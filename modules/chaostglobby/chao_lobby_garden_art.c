/* CHAOSTGLOBBY.PRS's gate models, from the disc, and the flame tuning and
 * collision shapes that gate_art.h declares. its D lies between dark.c and
 * neutral.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "collision/collision.h"

Uint32 lobby_unused_flame_word = 1;
Float lobby_gate_flame_speed = 0.4F;
static Sint16 lobby_hero_arch_polygons[6305] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_arch_polygons.inc"
};
static Sint32 lobby_hero_arch_vertices[4887] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_arch_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_arch_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_arch_model.inc"
};
NJS_CNK_OBJECT lobby_hero_arch = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_arch.inc"
};
static Sint16 lobby_hero_wheel_a_polygons[287] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_a_polygons.inc"
};
static Sint32 lobby_hero_wheel_a_vertices[243] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_a_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_wheel_a_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_a_model.inc"
};
NJS_CNK_OBJECT lobby_hero_wheel_a = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_a.inc"
};
static Sint16 lobby_hero_wheel_b_polygons[287] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_b_polygons.inc"
};
static Sint32 lobby_hero_wheel_b_vertices[243] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_b_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_wheel_b_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_b_model.inc"
};
NJS_CNK_OBJECT lobby_hero_wheel_b = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_wheel_b.inc"
};
static Sint16 lobby_hero_left_polygons[85] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_left_polygons.inc"
};
static Sint32 lobby_hero_left_vertices[159] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_left_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_left_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_left_model.inc"
};
NJS_CNK_OBJECT lobby_hero_left = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_left.inc"
};
static Sint16 lobby_hero_right_polygons[85] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_right_polygons.inc"
};
static Sint32 lobby_hero_right_vertices[159] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_right_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_right_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_right_model.inc"
};
NJS_CNK_OBJECT lobby_hero_right = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_right.inc"
};
static Sint16 lobby_hero_threshold_polygons[155] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_threshold_polygons.inc"
};
static Sint32 lobby_hero_threshold_vertices[103] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_threshold_vertices.inc"
};
static NJS_CNK_MODEL lobby_hero_threshold_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_threshold_model.inc"
};
NJS_CNK_OBJECT lobby_hero_threshold = {
#include "chaostglobby/chao_lobby_garden_art/lobby_hero_threshold.inc"
};
/* clang-format off */
struct collision_shape lobby_pedestal_cylinders[7] = {
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, -5.0F, 0.0F}, 19.0F, 6.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 0.1F, 0.0F}, 16.5F, 3.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 55.0F, 0.0F}, 19.0F, 30.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {18.5F, 15.0F, 0.0F}, 1.5F, 15.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {-18.5F, 15.0F, 0.0F}, 1.5F, 15.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 15.0F, 18.5F}, 1.5F, 15.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 15.0F, -18.5F}, 1.5F, 15.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint16 lobby_kinder_arch_polygons[2789] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_arch_polygons.inc"
};
static Sint32 lobby_kinder_arch_vertices[2463] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_arch_vertices.inc"
};
static NJS_CNK_MODEL lobby_kinder_arch_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_arch_model.inc"
};
NJS_CNK_OBJECT lobby_kinder_arch = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_arch.inc"
};
static Sint16 lobby_kinder_left_polygons[81] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_left_polygons.inc"
};
static Sint32 lobby_kinder_left_vertices[153] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_left_vertices.inc"
};
static NJS_CNK_MODEL lobby_kinder_left_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_left_model.inc"
};
NJS_CNK_OBJECT lobby_kinder_left = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_left.inc"
};
static Sint16 lobby_kinder_right_polygons[81] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_right_polygons.inc"
};
static Sint32 lobby_kinder_right_vertices[153] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_right_vertices.inc"
};
static NJS_CNK_MODEL lobby_kinder_right_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_right_model.inc"
};
NJS_CNK_OBJECT lobby_kinder_right = {
#include "chaostglobby/chao_lobby_garden_art/lobby_kinder_right.inc"
};
static Sint16 lobby_threshold_polygons[319] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_threshold_polygons.inc"
};
static Sint32 lobby_threshold_vertices[203] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_threshold_vertices.inc"
};
static NJS_CNK_MODEL lobby_threshold_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_threshold_model.inc"
};
NJS_CNK_OBJECT lobby_threshold = {
#include "chaostglobby/chao_lobby_garden_art/lobby_threshold.inc"
};
/* clang-format off */
struct collision_shape lobby_kinder_shapes[LOBBY_KINDER_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_ATTACK_LEVEL(3),
     SHAPE_IGNORE_ENEMIES,
     {0.0F, 15.0F, -14.0F}, 15.0F, 15.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ENEMIES,
     {0.0F, 52.0F, -4.0F}, 20.5F, 28.0F, 20.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ENEMIES,
     {16.0F, 20.0F, 12.0F}, 4.5F, 21.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ENEMIES,
     {16.0F, 20.0F, 2.0F}, 4.5F, 21.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ENEMIES,
     {-16.0F, 20.0F, 12.0F}, 4.5F, 21.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ENEMIES,
     {-16.0F, 20.0F, 2.0F}, 4.5F, 21.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint16 lobby_neutral_arch_polygons[7461] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_neutral_arch_polygons.inc"
};
static Sint32 lobby_neutral_arch_vertices[5399] = {
#include "chaostglobby/chao_lobby_garden_art/lobby_neutral_arch_vertices.inc"
};
static NJS_CNK_MODEL lobby_neutral_arch_model = {
#include "chaostglobby/chao_lobby_garden_art/lobby_neutral_arch_model.inc"
};
NJS_CNK_OBJECT lobby_neutral_arch = {
#include "chaostglobby/chao_lobby_garden_art/lobby_neutral_arch.inc"
};
