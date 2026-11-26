#include "shared/deck_wall/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "shared/common_object.h"
#include "shared/deck_scenery.h"

static NJS_TEXNAME deck_wall_textures[6] = {
#include "shared/deck_wall/deck_wall_textures.inc"
};

static NJS_TEXLIST deck_wall_texlist = {
#include "shared/deck_wall/deck_wall_texlist.inc"
};

static Sint16 deck_wall_polygons[533] = {
#include "shared/deck_wall/deck_wall_polygons.inc"
};

static Sint32 deck_wall_vertices[535] = {
#include "shared/deck_wall/deck_wall_vertices.inc"
};

static NJS_CNK_MODEL deck_wall_model = {
#include "shared/deck_wall/deck_wall_model.inc"
};

static NJS_CNK_OBJECT deck_wall_object[1] = {
#include "shared/deck_wall/deck_wall_object.inc"
};

static NJS_TEXNAME deck_wall_alternate_textures[4] = {
#include "shared/deck_wall/deck_wall_alternate_textures.inc"
};

static NJS_TEXLIST deck_wall_alternate_texlist = {
#include "shared/deck_wall/deck_wall_alternate_texlist.inc"
};

static Sint16 deck_wall_alternate_polygons[443] = {
#include "shared/deck_wall/deck_wall_alternate_polygons.inc"
};

static Sint32 deck_wall_alternate_vertices[283] = {
#include "shared/deck_wall/deck_wall_alternate_vertices.inc"
};

static NJS_CNK_MODEL deck_wall_alternate_model = {
#include "shared/deck_wall/deck_wall_alternate_model.inc"
};

static NJS_CNK_OBJECT deck_wall_alternate_object = {
#include "shared/deck_wall/deck_wall_alternate_object.inc"
};

NJS_TEXLIST *deck_wall_texlists[2] = {
#include "shared/deck_wall/deck_wall_texlists.inc"
};
static struct collision_shape wall_box[1] = {
#include "shared/deck_wall/wall_box.inc"
};

void deck_wall_init(struct task *task)
{
    model_object_init(task, &deck_wall_texlist, deck_wall_object[0].model,
                      wall_box, 1, OBJECT_MAY_LEAVE);
}
