/* the two scrap models, each one strip under one untextured material. no
 * code reads their root objects */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk_polygon.h"
#include "enemy/scrap_models.h"

struct gray_scrap_faces {
    Sint16 material[8];
    struct part_strip_header strip;
    Sint16 length;
    Uint16 indices[6];
    Uint16 end;
};
static struct gray_scrap_faces gray_faces = {
#include "enemy/scrap_models/gray_faces.inc"
};

static Sint32 gray_vertices[27] = {
#include "enemy/scrap_models/gray_vertices.inc"
};

NJS_CNK_MODEL enemy_scrap_gray_model = {
#include "enemy/scrap_models/enemy_scrap_gray_model.inc"
};

static NJS_CNK_OBJECT gray_fragment = {
#include "enemy/scrap_models/gray_fragment.inc"
};

struct red_scrap_faces {
    Sint16 material[8];
    struct part_strip_header strip;
    Sint16 length;
    Uint16 indices[10];
    Uint16 end;
};
static struct red_scrap_faces red_faces = {
#include "enemy/scrap_models/red_faces.inc"
};

static Sint32 red_vertices[39] = {
#include "enemy/scrap_models/red_vertices.inc"
};

NJS_CNK_MODEL enemy_scrap_red_model = {
#include "enemy/scrap_models/enemy_scrap_red_model.inc"
};

static NJS_CNK_OBJECT red_fragment = {
#include "enemy/scrap_models/red_fragment.inc"
};
