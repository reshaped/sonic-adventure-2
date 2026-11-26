#ifndef SHARED_CHAOS_ART_TEXTURE_WORDS_H
#define SHARED_CHAOS_ART_TEXTURE_WORDS_H
#include <sg_xpt.h>
#include "shared/chaos_art/materials.h"

/* @hack type: each texture word named by a Sint16 of its own, which symbols.txt
 * places at the word, where the polygon list's field would do */
#ifdef NONMATCHING
/* the plain form the hack tag names. this side's objects don't define the
 * second names, which only symbols.txt places */
#define NORMAL_TEXID (&normal_object_polygons.texture1.texture)
#define NORMAL_REACH_TEXID (normal_reach_object_polygons.texture1.texture)
#define FLOAT_TEXID (&float_object_polygons.texture1.texture)
#define FLOAT_REACH_TEXID (float_reach_object_polygons.texture1.texture)
#define PIECE_TEXID (piece_polygons.texture3.texture)
#define SHED_TEXID (&shed_object_polygons.texture1.texture)
#define GUARD_TEXID (&guard_object_polygons.texture1.texture)
#define PIECE_A_WATER_TEXTURE (&piece_a_strips[0].texture3.texture)
#define PIECE_B_WATER_TEXTURE (&piece_b_strips[0].texture3.texture)
#else
extern Sint16 normal_texid[];
#define NORMAL_TEXID normal_texid
extern Sint16 normal_reach_texid;
#define NORMAL_REACH_TEXID normal_reach_texid
extern Sint16 float_texid[];
#define FLOAT_TEXID float_texid
extern Sint16 float_reach_texid;
#define FLOAT_REACH_TEXID float_reach_texid
extern Sint16 piece_texid;
#define PIECE_TEXID piece_texid
extern Sint16 shed_texid[];
#define SHED_TEXID shed_texid
extern Sint16 guard_texid[];
#define GUARD_TEXID guard_texid
/* @hack spelling: each strip list's water texture word reached by a byte offset
 * from the list, where its texture3.texture would do */
#define PIECE_WATER_OFFSET (217 * sizeof(Sint16))
#define PIECE_A_WATER_TEXTURE                                                  \
    ((Sint16 *)((Uint8 *)piece_a_strips + PIECE_WATER_OFFSET))
#define PIECE_B_WATER_TEXTURE                                                  \
    ((Sint16 *)((Uint8 *)piece_b_strips + PIECE_WATER_OFFSET))
#endif

#endif
