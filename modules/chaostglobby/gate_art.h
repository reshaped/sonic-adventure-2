/* the art of the Chao Lobby's exit and its four gates */
#ifndef CHAOSTGLOBBY_GATE_ART_H
#define CHAOSTGLOBBY_GATE_ART_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"

extern NJS_CNK_OBJECT lobby_exit_base;
extern NJS_CNK_OBJECT lobby_exit_frame;
extern NJS_CNK_OBJECT lobby_exit_disc;

extern NJS_CNK_OBJECT lobby_dark_arch;
extern NJS_CNK_OBJECT lobby_dark_threshold;

extern NJS_CNK_OBJECT lobby_hero_arch;
extern NJS_CNK_OBJECT lobby_hero_wheel_a;
extern NJS_CNK_OBJECT lobby_hero_wheel_b;
extern NJS_CNK_OBJECT lobby_hero_left;
extern NJS_CNK_OBJECT lobby_hero_right;
extern NJS_CNK_OBJECT lobby_hero_threshold;

extern NJS_CNK_OBJECT lobby_kinder_arch;
extern NJS_CNK_OBJECT lobby_kinder_left;
extern NJS_CNK_OBJECT lobby_kinder_right;

extern NJS_CNK_OBJECT lobby_neutral_arch;

/* the dark gate's flames fade into their next frame by
 * lobby_gate_flame_speed each step. lobby_unused_flame_word, before it, is a
 * word no code reads */
extern Uint32 lobby_unused_flame_word;
extern Float lobby_gate_flame_speed;

/* the pedestal's seven cylinders, which no code reads. the exit registers
 * shapes of its own */
extern struct collision_shape lobby_pedestal_cylinders[7];

/* the kindergarten gate's shapes. its collision record takes GATE_SHAPES
 * shapes from here, two past the array's end */
#define LOBBY_KINDER_SHAPES 6
extern struct collision_shape lobby_kinder_shapes[LOBBY_KINDER_SHAPES];

#endif
