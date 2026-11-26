/* Green Hill sky CHUNK streams. All four walls use one layout, and the
 * seven lower panels use another. Each placed object has its own data. */
#ifndef STG31_SKY_MESH_TYPES_H
#define STG31_SKY_MESH_TYPES_H

#include <sg_xpt.h>

#include "draw/chunk.h"

/* R13 rounds the first two horizon vertices' subnormal x coordinates to zero
 * in float initializers. Preserve their original IEEE-754 words. */
struct green_hill_axis_vertex {
    Uint32 x_bits;
    Float y, z;
    Uint32 color;
};

struct green_hill_horizon_vertex_list {
    Sint32 head[2];
    struct green_hill_axis_vertex axis_vertices[2];
    struct chunk_vertex_d8 vertices[144];
    Sint32 end;
};

#endif
