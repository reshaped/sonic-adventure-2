/* Each NOREN frame contains its own two PVM slots and drawable mesh. */
#ifndef STG26_CURTAIN_ART_H
#define STG26_CURTAIN_ART_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"

enum { CURTAIN_FRAMES = 31, CURTAIN_VERTICES = 130, CURTAIN_TEXTURE_COUNT = 2 };

struct curtain_frame {
    NJS_TEXNAME names[CURTAIN_TEXTURE_COUNT];
    NJS_TEXLIST texlist;
    Sint16 polygons[465];
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 points[CURTAIN_VERTICES];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

#endif
