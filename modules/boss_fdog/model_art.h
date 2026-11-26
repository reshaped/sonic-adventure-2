#ifndef BOSS_FDOG_MODEL_ART_H
#define BOSS_FDOG_MODEL_ART_H

#include <ninja.h>
#include "draw/chunk.h"

extern NJS_CNK_OBJECT fdog_object;
extern NJS_CNK_OBJECT fdog_body_node;
extern NJS_CNK_OBJECT fdog_turret_node;
extern NJS_CNK_OBJECT fdog_laser_node;
extern NJS_CNK_OBJECT fdog_gun_node;

extern Sint32 fdog_jet_marker_vertices[39];
#ifdef NONMATCHING
/* the vertices by their own spelling: this side's objects don't define
 * fdog_fast_jet_vertices, which only symbols.txt places */
#define fdog_fast_jet_vertices                                                 \
    ((struct chunk_vertex_vn *)(fdog_jet_marker_vertices + CHUNK_HEAD_WORDS))
#else
extern struct chunk_vertex_vn fdog_fast_jet_vertices[];
#endif

#endif
