/* Metal Harbor's scenery: COVER, DAMMY, CBRI, NET and ISLAND in its object
 * list. each stands still and draws a model */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg10/scenery/text.inc"
#include "collision/collision.h"
#include "shared/common_object.h"
#include "stg10/scenery.h"

#define SCENERY_MODE OBJECT_MAY_LEAVE
#define DAMMY_SHAPES 2
#define CBRI_SHAPES 1

/* Cover geometry. */

static NJS_TEXNAME cover_names[5] = {
#include "stg10/scenery/cover_names.inc"
};

static NJS_TEXLIST cover_texlist = {
#include "stg10/scenery/cover_texlist.inc"
};

static Sint16 cover_polygons[769] = {
#include "stg10/scenery/cover_polygons.inc"
};

static Sint32 cover_vertices[739] = {
#include "stg10/scenery/cover_vertices.inc"
};

static NJS_CNK_MODEL cover_model = {
#include "stg10/scenery/cover_model.inc"
};

static NJS_CNK_OBJECT cover_object[1] = {
#include "stg10/scenery/cover_object.inc"
};

static NJS_MATERIAL cover_solid_materials = {
#include "stg10/scenery/cover_solid_materials.inc"
};

static Uint16 cover_solid_strips[36] = {
#include "stg10/scenery/cover_solid_strips.inc"
};

static NJS_MESHSET cover_solid_faces = {
#include "stg10/scenery/cover_solid_faces.inc"
};

static NJS_POINT3 cover_solid_points[32] = {
#include "stg10/scenery/cover_solid_points.inc"
};

static NJS_VECTOR cover_solid_normals[32] = {
#include "stg10/scenery/cover_solid_normals.inc"
};

static NJS_MODEL cover_solid_model = {
#include "stg10/scenery/cover_solid_model.inc"
};

static NJS_OBJECT cover_collision = {
#include "stg10/scenery/cover_collision.inc"
};
/* Dammy geometry. */

static NJS_TEXNAME dammy_names[11] = {
#include "stg10/scenery/dammy_names.inc"
};

static NJS_TEXLIST dammy_texlist = {
#include "stg10/scenery/dammy_texlist.inc"
};

static Sint16 dammy_polygons[1051] = {
#include "stg10/scenery/dammy_polygons.inc"
};

static Sint32 dammy_vertices[679] = {
#include "stg10/scenery/dammy_vertices.inc"
};

static NJS_CNK_MODEL dammy_model = {
#include "stg10/scenery/dammy_model.inc"
};

static NJS_CNK_OBJECT dammy_object[1] = {
#include "stg10/scenery/dammy_object.inc"
};
/* Cbri geometry. */

static NJS_TEXNAME cbri_names[3] = {
#include "stg10/scenery/cbri_names.inc"
};

static NJS_TEXLIST cbri_texlist = {
#include "stg10/scenery/cbri_texlist.inc"
};

static Sint16 cbri_polygons[275] = {
#include "stg10/scenery/cbri_polygons.inc"
};

static Sint32 cbri_vertices[203] = {
#include "stg10/scenery/cbri_vertices.inc"
};

static NJS_CNK_MODEL cbri_model = {
#include "stg10/scenery/cbri_model.inc"
};

static NJS_CNK_OBJECT cbri_object[1] = {
#include "stg10/scenery/cbri_object.inc"
};
/* Island geometry. */

static NJS_TEXNAME island_names[4] = {
#include "stg10/scenery/island_names.inc"
};

static NJS_TEXLIST island_texlist = {
#include "stg10/scenery/island_texlist.inc"
};

static Sint16 island_polygons[443] = {
#include "stg10/scenery/island_polygons.inc"
};

static Sint32 island_vertices[283] = {
#include "stg10/scenery/island_vertices.inc"
};

static NJS_CNK_MODEL island_model = {
#include "stg10/scenery/island_model.inc"
};

static NJS_CNK_OBJECT island_object[1] = {
#include "stg10/scenery/island_object.inc"
};
/* Net geometry. */

static NJS_TEXNAME net_names[3] = {
#include "stg10/scenery/net_names.inc"
};

static NJS_TEXLIST net_texlist = {
#include "stg10/scenery/net_texlist.inc"
};

static Sint16 net_polygons[87] = {
#include "stg10/scenery/net_polygons.inc"
};

static Sint32 net_vertices[83] = {
#include "stg10/scenery/net_vertices.inc"
};

static NJS_CNK_MODEL net_model = {
#include "stg10/scenery/net_model.inc"
};

static NJS_CNK_OBJECT net_object[1] = {
#include "stg10/scenery/net_object.inc"
};

NJS_TEXLIST *harbor_scenery_texlists[6] = {
#include "stg10/scenery/harbor_scenery_texlists.inc"
};
static struct collision_shape dammy_shapes[DAMMY_SHAPES] = {
#include "stg10/scenery/dammy_shapes.inc"
};
static struct collision_shape cbri_shapes[CBRI_SHAPES] = {
#include "stg10/scenery/cbri_shapes.inc"
};

void cover_init(struct task *task)
{
    solid_object_init(task, &cover_texlist, cover_object->model, NULL, 0,
                      &cover_collision, SCENERY_MODE);
}

void dammy_init(struct task *task)
{
    model_object_init(task, &dammy_texlist, dammy_object->model, dammy_shapes,
                      DAMMY_SHAPES, SCENERY_MODE);
}

void cbri_init(struct task *task)
{
    model_object_init(task, &cbri_texlist, cbri_object->model, cbri_shapes,
                      CBRI_SHAPES, SCENERY_MODE);
}

void net_init(struct task *task)
{
    model_object_init(task, &net_texlist, net_object->model, NULL, 0,
                      SCENERY_MODE);
}

void island_init(struct task *task)
{
    model_object_init(task, &island_texlist, island_object->model, NULL, 0,
                      SCENERY_MODE);
}
