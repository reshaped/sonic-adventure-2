#ifndef STG44_SCENERY_CARRIAGE_CHUNKS_H
#define STG44_SCENERY_CARRIAGE_CHUNKS_H

/* Chunk lists for the engine, trailer, headlights and their shadows. */
#include <ninja.h>
#include <sg_xpt.h>

struct mad_space_engine_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[20];
    Sint32 end;
};

struct mad_space_trailer_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[16];
    Sint32 end;
};

#endif
