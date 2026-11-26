/* City Escape's scenery: seven kinds of object that stand still and draw a
 * model each */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "stg13/scenery.h"
#include "stg13/scenery/text.inc"

#define FENCE_SHAPES 2

static NJS_TEXNAME sblg_texnames[4] = {
#include "stg13/scenery/sblg_texnames.inc"
};
static NJS_TEXLIST sblg_texlist = {
#include "stg13/scenery/sblg_texlist.inc"
};
static Sint16 sblg_polygons[209] = {
#include "stg13/scenery/sblg_polygons.inc"
};
static Sint32 sblg_vertices[211] = {
#include "stg13/scenery/sblg_vertices.inc"
};
static NJS_CNK_MODEL sblg_model = {
#include "stg13/scenery/sblg_model.inc"
};
static NJS_CNK_OBJECT sblg_object = {
#include "stg13/scenery/sblg_object.inc"
};
static NJS_TEXNAME green_b_texnames[2] = {
#include "stg13/scenery/green_b_texnames.inc"
};
static NJS_TEXLIST green_b_texlist = {
#include "stg13/scenery/green_b_texlist.inc"
};
static Sint16 green_b_polygons[423] = {
#include "stg13/scenery/green_b_polygons.inc"
};
static Sint32 green_b_vertices[227] = {
#include "stg13/scenery/green_b_vertices.inc"
};
static NJS_CNK_MODEL green_b_model = {
#include "stg13/scenery/green_b_model.inc"
};
static NJS_CNK_OBJECT green_b_object = {
#include "stg13/scenery/green_b_object.inc"
};
/* GREEN_B's collision object, which the players stand on */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL green_b_collision_materials[1] = {
#include "stg13/scenery/green_b_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
struct green_b_collision_mesh_list {
    Uint16 mesh[4];
    Uint16 mesh_2[7];
    Uint16 mesh_3[12];
    Uint16 mesh_4[4];
    Uint16 mesh_5[6];
};
static struct green_b_collision_mesh_list green_b_collision_meshes = {
#include "stg13/scenery/green_b_collision_meshes.inc"
};
static NJS_MESHSET green_b_collision_meshsets[1] = {
#include "stg13/scenery/green_b_collision_meshsets.inc"
};
static NJS_POINT3 green_b_collision_points[12] = {
#include "stg13/scenery/green_b_collision_points.inc"
};
static NJS_VECTOR green_b_collision_normals[12] = {
#include "stg13/scenery/green_b_collision_normals.inc"
};
static NJS_MODEL green_b_collision_model = {
#include "stg13/scenery/green_b_collision_model.inc"
};
static NJS_OBJECT green_b_collision = {
#include "stg13/scenery/green_b_collision.inc"
};
static NJS_TEXNAME palm_texnames[2] = {
#include "stg13/scenery/palm_texnames.inc"
};
static NJS_TEXLIST palm_texlist = {
#include "stg13/scenery/palm_texlist.inc"
};
static Sint16 palm_polygons[447] = {
#include "stg13/scenery/palm_polygons.inc"
};
static Sint32 palm_vertices[379] = {
#include "stg13/scenery/palm_vertices.inc"
};
static NJS_CNK_MODEL palm_model = {
#include "stg13/scenery/palm_model.inc"
};
static NJS_CNK_OBJECT palm_object = {
#include "stg13/scenery/palm_object.inc"
};
static NJS_TEXNAME slider_texnames[6] = {
#include "stg13/scenery/slider_texnames.inc"
};
static NJS_TEXLIST slider_texlist = {
#include "stg13/scenery/slider_texlist.inc"
};
static Sint16 slider_polygons[213] = {
#include "stg13/scenery/slider_polygons.inc"
};
static Sint32 slider_vertices[211] = {
#include "stg13/scenery/slider_vertices.inc"
};
static NJS_CNK_MODEL slider_model = {
#include "stg13/scenery/slider_model.inc"
};
static NJS_CNK_OBJECT slider_object = {
#include "stg13/scenery/slider_object.inc"
};
/* a collision object for SLIDER, which slider_init doesn't take */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL slider_collision_materials[1] = {
#include "stg13/scenery/slider_collision_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
struct slider_collision_mesh_list {
    Uint16 mesh[5];
    Uint16 mesh_2[8];
    Uint16 mesh_3[4];
    Uint16 mesh_4[5];
};
static struct slider_collision_mesh_list slider_collision_meshes = {
#include "stg13/scenery/slider_collision_meshes.inc"
};
static NJS_MESHSET slider_collision_meshsets[1] = {
#include "stg13/scenery/slider_collision_meshsets.inc"
};
static NJS_POINT3 slider_collision_points[12] = {
#include "stg13/scenery/slider_collision_points.inc"
};
static NJS_VECTOR slider_collision_normals[12] = {
#include "stg13/scenery/slider_collision_normals.inc"
};
static NJS_MODEL slider_collision_model = {
#include "stg13/scenery/slider_collision_model.inc"
};
static NJS_OBJECT slider_collision = {
#include "stg13/scenery/slider_collision.inc"
};
static NJS_TEXNAME bar_texnames[1] = {
#include "stg13/scenery/bar_texnames.inc"
};
static NJS_TEXLIST bar_texlist = {
#include "stg13/scenery/bar_texlist.inc"
};
static Sint16 bar_polygons[57] = {
#include "stg13/scenery/bar_polygons.inc"
};
static Sint32 bar_vertices[43] = {
#include "stg13/scenery/bar_vertices.inc"
};
static NJS_CNK_MODEL bar_model = {
#include "stg13/scenery/bar_model.inc"
};
static NJS_CNK_OBJECT bar_object = {
#include "stg13/scenery/bar_object.inc"
};
static NJS_TEXNAME fence_short_texnames[1] = {
#include "stg13/scenery/fence_short_texnames.inc"
};
static NJS_TEXLIST fence_short_texlist = {
#include "stg13/scenery/fence_short_texlist.inc"
};
static Sint16 fence_short_polygons[319] = {
#include "stg13/scenery/fence_short_polygons.inc"
};
static Sint32 fence_short_vertices[279] = {
#include "stg13/scenery/fence_short_vertices.inc"
};
static NJS_CNK_MODEL fence_short_model = {
#include "stg13/scenery/fence_short_model.inc"
};
static NJS_CNK_OBJECT fence_short_object = {
#include "stg13/scenery/fence_short_object.inc"
};
static NJS_TEXNAME fence_long_texnames[1] = {
#include "stg13/scenery/fence_long_texnames.inc"
};
static NJS_TEXLIST fence_long_texlist = {
#include "stg13/scenery/fence_long_texlist.inc"
};
static Sint16 fence_long_polygons[511] = {
#include "stg13/scenery/fence_long_polygons.inc"
};
static Sint32 fence_long_vertices[447] = {
#include "stg13/scenery/fence_long_vertices.inc"
};
static NJS_CNK_MODEL fence_long_model = {
#include "stg13/scenery/fence_long_model.inc"
};
static NJS_CNK_OBJECT fence_long_object = {
#include "stg13/scenery/fence_long_object.inc"
};
NJS_TEXLIST *scenery_texlists[8] = {
#include "stg13/scenery/scenery_texlists.inc"
};
/* each fence's rail is a cylinder that its own quarter turn about z lays
 * along x */
static struct collision_shape fence_short_shapes[FENCE_SHAPES] = {
#include "stg13/scenery/fence_short_shapes.inc"
};
static struct collision_shape fence_long_shapes[FENCE_SHAPES] = {
#include "stg13/scenery/fence_long_shapes.inc"
};

void sblg_init(struct task *task)
{
    model_object_init(task, &sblg_texlist, &sblg_model, NULL, 0,
                      OBJECT_MAY_LEAVE);
}

void green_b_init(struct task *task)
{
    solid_object_init(task, &green_b_texlist, &green_b_model, NULL, 0,
                      &green_b_collision, OBJECT_MAY_LEAVE);
}

void palm_init(struct task *task)
{
    model_object_init(task, &palm_texlist, &palm_model, NULL, 0,
                      OBJECT_MAY_LEAVE);
}

void slider_init(struct task *task)
{
    model_object_init(task, &slider_texlist, &slider_model, NULL, 0,
                      OBJECT_MAY_LEAVE);
}

void bar_init(struct task *task)
{
    model_object_init(task, &bar_texlist, &bar_model, NULL, 0,
                      OBJECT_MAY_LEAVE);
}

void fence_short_init(struct task *task)
{
    model_object_init(task, &fence_short_texlist, &fence_short_model,
                      fence_short_shapes, FENCE_SHAPES, OBJECT_MAY_LEAVE);
}

void fence_long_init(struct task *task)
{
    model_object_init(task, &fence_long_texlist, &fence_long_model,
                      fence_long_shapes, FENCE_SHAPES, OBJECT_MAY_LEAVE);
}
