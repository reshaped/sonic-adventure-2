#ifndef SHARED_GUN_MECH_EFFECT_ART_H
#define SHARED_GUN_MECH_EFFECT_ART_H

#include <ninja.h>
#include "draw/chunk.h"

/* the vertex lists of the jet and pod markers in attachments.c. Flying Dog's
 * fdog_find_points reads their vertices as the jets' and pods' points */

extern Sint32 mech_boss_jet_markers[21];
extern Sint32 mech_boss_pods_1_markers[27];
extern Sint32 mech_boss_pods_0_markers[27];
#ifdef NONMATCHING
/* the vertices by their own spelling: this side's objects don't define the
 * second names, which only symbols.txt places */
#define mech_boss_jet_vertices                                                 \
    ((struct chunk_vertex_vn *)(mech_boss_jet_markers + CHUNK_HEAD_WORDS))
#define mech_boss_pods_1_vertices                                              \
    ((struct chunk_vertex_vn *)(mech_boss_pods_1_markers + CHUNK_HEAD_WORDS))
#define mech_boss_pods_0_vertices                                              \
    ((struct chunk_vertex_vn *)(mech_boss_pods_0_markers + CHUNK_HEAD_WORDS))
#else
extern struct chunk_vertex_vn mech_boss_jet_vertices[];
extern struct chunk_vertex_vn mech_boss_pods_1_vertices[];
extern struct chunk_vertex_vn mech_boss_pods_0_vertices[];
#endif

extern NJS_CNK_OBJECT mech_boss_missile_object;
extern NJS_CNK_OBJECT mech_boss_body_shadow;
extern NJS_CNK_OBJECT mech_boss_foot_shadow;
extern NJS_CNK_OBJECT muzzle_object_a;
extern NJS_CNK_OBJECT muzzle_object_b;
extern NJS_CNK_OBJECT muzzle_object_c;
extern NJS_CNK_MODEL mech_boss_beam_charge_model;
extern NJS_CNK_OBJECT mech_boss_missile_shadow;
extern NJS_CNK_OBJECT mech_boss_sight_object;
extern NJS_CNK_OBJECT mech_boss_sight_ring_object;
extern NJS_CNK_OBJECT laser_pulse_object;
extern NJS_CNK_OBJECT mech_boss_laser_object;
extern NJS_CNK_MODEL mech_boss_beam_bomb_model;
extern NJS_CNK_OBJECT mech_boss_beam_pillar_object;
extern NJS_CNK_OBJECT mech_boss_beam_torus_object;

extern NJS_MOTION mech_boss_sight_ring_motion;
extern NJS_MOTION mech_boss_sight_motion;

#endif
