#include <sg_xpt.h>
#include "boss_bigbogy/settings.h"
#include "boss_bigbogy/motion_art.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "system/angle.h"

const struct motion_table_entry bigbogy_motions[24] = {
#include "boss_bigbogy/settings/bigbogy_motions.inc"
};

#define FIRM PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
#define PUSH (FIRM | PUSH_PLAYERS)
#define ATTACK(rank)                                                           \
    (SHAPE_HURTS_PLAYERS | SHAPE_GUARD_LEVEL(3) | SHAPE_ATTACK_LEVEL(rank))
#define HURT SHAPE_HURTABLE
#define WORLD (HURT | SHAPE_IN_WORLD)
#define BODY (HURT | SHAPE_IGNORE_OBJECTS)
#define FLOOR (SHAPE_IGNORE_OBJECTS | SHAPE_IN_WORLD | SHAPE_NO_OWN_HITS)

const struct collision_shape bigbogy_shapes[13] = {
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(3),
     HURT,
     {0, 50, -12},
     48,
     0,
     0,
     0,
     {0, 0, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(2),
     HURT,
     {0, 70, -12},
     48,
     0,
     0,
     0,
     {0, 0, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(3),
     HURT,
     {0, 30, -12},
     50,
     0,
     0,
     0,
     {0, 0, 0}},
    {0, SHAPE_BOX, PUSH, ATTACK(2), HURT, {0, 10, 0}, 110, 40, 5, 0, {0, 0, 0}},
    {0,
     SHAPE_CAPSULE,
     PUSH,
     ATTACK(0),
     HURT,
     {0, 25, -75},
     30,
     8,
     0,
     0,
     {QUARTER_TURN, QUARTER_TURN, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(3),
     WORLD,
     {0, 0, 0},
     20,
     8,
     0,
     0,
     {QUARTER_TURN, QUARTER_TURN, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(3),
     WORLD,
     {0, 0, 0},
     20,
     8,
     0,
     0,
     {QUARTER_TURN, QUARTER_TURN, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(3),
     WORLD,
     {0, 0, 0},
     40,
     0,
     0,
     0,
     {0, 0, 0}},
    {0,
     SHAPE_SPHERE,
     FIRM,
     SHAPE_HURTS_PLAYERS,
     BODY,
     {0, 10, 0},
     20,
     0,
     0,
     0,
     {0, 0, 0}},
    {0,
     SHAPE_SPHERE,
     PUSH,
     ATTACK(0),
     BODY,
     {0, 10, 0},
     15,
     0,
     0,
     0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, FIRM, ATTACK(0), BODY, {0, 0, 0}, 20, 0, 0, 0, {0, 0, 0}},
    {0,
     SHAPE_CYLINDER,
     PUSH,
     SHAPE_GUARD_LEVEL(3),
     FLOOR,
     {200, -20, 0},
     125,
     40,
     0,
     0,
     {0, 0, 0}},
    {0,
     SHAPE_CYLINDER,
     PUSH,
     SHAPE_GUARD_LEVEL(3),
     FLOOR,
     {200, -85, 0},
     130,
     40,
     0,
     0,
     {0, 0, 0}}};

struct bigbogy_tuning bigbogy_tuning[BIGBOGY_TUNINGS] = {
    {0.05F, 0.15F, 0.4F, 0.7F, 1.3F},   {0.05F, 0.18F, 0.4F, 0.7F, 1.5F},
    {0.06F, 0.14F, 0.05F, 0.08F, 1.2F}, {0.02F, 0.14F, 0.2F, 0.4F, 1.2F},
    {0.02F, 0.14F, 0.2F, 0.4F, 0.0F},   {0.08F, 0.11F, 4.9F, 0.7F, 1.2F},
    {0.05F, 0.09F, 0.3F, 0.7F, 0.8F},   {0.05F, 0.18F, 0.4F, 0.7F, 1.7F},
    {0.05F, 0.16F, 0.3F, 0.4F, 1.7F}};

Float bigbogy_unused_8c585ed8 = 0.065F;
Float bigbogy_wall_speed = 0.7F;
struct bigbogy_sound_period shadow_sound_period = {8, {0}};
struct bigbogy_sound_period digging_sound_period = {4, {0}};
struct bigbogy_sound_period fleeing_sound_period = {12, {0}};
struct bigbogy_sound_period grounded_sound_period = {22, {0}};
