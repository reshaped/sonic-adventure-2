/* the cloth of Sand Ocean's and Hidden Base's flags, as FLAG_FRAMES meshes, one
 * a frame. each is a strip of FLAG_CLOTH_VERTICES colored vertices */
#ifndef SHARED_OCEAN_FLAG_FRAMES_H
#define SHARED_OCEAN_FLAG_FRAMES_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"
#include "stg21/flag.h"

#define FLAG_CLOTH_VERTICES 13

struct flag_polygons {
    struct {
        Uint16 head, words;
        struct part_color diffuse, ambient;
    } material;
    struct part_texture texture;
    struct part_strip_header strips;
    Sint16 length;
    struct part_uv strip[FLAG_CLOTH_VERTICES];
    Uint16 strip_pad;
    Uint16 end;
};

struct flag_vertices {
    Sint32 head[2];
    struct chunk_vertex_d8 points[FLAG_CLOTH_VERTICES];
    Sint32 end;
};

struct flag_cloth_mesh {
    struct flag_polygons polygons;
    struct flag_vertices vertices;
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

/* a frame of Sand Ocean's cloth, with a texture list of its own. Hidden
 * Base's frames share hidden_base_flag_cloth */
struct ocean_flag_frame {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    struct flag_cloth_mesh cloth;
};

extern struct ocean_flag_frame flag_frames[FLAG_FRAMES];
extern struct flag_cloth_mesh hidden_base_flag_frames[FLAG_FRAMES];
extern NJS_CNK_OBJECT hidden_base_flag_root;
#endif
