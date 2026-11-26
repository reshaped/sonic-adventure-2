/* the road objects and signs. each draws a kind of shared/model_kind.h from
 * its list, ROADOBJ's road_object_kinds or SIGNS's sign_kinds.
 * road_objects_lookup reads a third list, lookup_kinds */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/model_kind.h"
#include "stg13/road_objects.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"
#include "stg13/road_objects/text.inc"

/* a task's list of kinds, in task->motion. task_destroy would free motion as
 * a record, so the destructor nulls it */
#define LIST(task) ((struct model_kind_list *)(task)->motion.p)
/* the low byte of the work's ang.x picks the task's kind */
#define KIND_INDEX 0xff
#define KIND(task, work) (&LIST(task)->kinds[(work)->ang.x])
/* unk_3c holds a KIND_MATRIX task's matrix of its pos and ang.y, from the
 * Shinobi heap, or null. no kind here has KIND_MATRIX */
#define MATRIX(task) ((task)->unk_3c.p)
#define ROAD_KINDS 16
#define SIGN_KINDS 8
#define LOOKUP_KINDS 9
enum { LIST_ROAD, LIST_SIGN, LIST_LOOKUP, LISTS };

static void road_object_start(struct task *task);
static void road_object_destroy(struct task *task);
static void road_object_step(struct task *task);
static void road_object_draw(struct task *task);
static void road_object_draw_scaled(struct task *task);

/* each kind's texture list and model, named for its index in
 * road_object_kinds or sign_kinds. they lie in the image's order, not by
 * index */
static NJS_TEXNAME road_object_0_texnames[4] = {
#include "stg13/road_objects/road_object_0_texnames.inc"
};
static NJS_TEXLIST road_object_0_texlist = {
#include "stg13/road_objects/road_object_0_texlist.inc"
};
static Sint16 road_object_0_polygons[359] = {
#include "stg13/road_objects/road_object_0_polygons.inc"
};
static Sint32 road_object_0_vertices[303] = {
#include "stg13/road_objects/road_object_0_vertices.inc"
};
static NJS_CNK_MODEL road_object_0_model = {
#include "stg13/road_objects/road_object_0_model.inc"
};
static NJS_CNK_OBJECT road_object_0_object = {
#include "stg13/road_objects/road_object_0_object.inc"
};
static NJS_TEXNAME road_object_1_texnames[3] = {
#include "stg13/road_objects/road_object_1_texnames.inc"
};
static NJS_TEXLIST road_object_1_texlist = {
#include "stg13/road_objects/road_object_1_texlist.inc"
};
static Sint16 road_object_1_polygons[445] = {
#include "stg13/road_objects/road_object_1_polygons.inc"
};
static Sint32 road_object_1_vertices[267] = {
#include "stg13/road_objects/road_object_1_vertices.inc"
};
static NJS_CNK_MODEL road_object_1_model = {
#include "stg13/road_objects/road_object_1_model.inc"
};
static NJS_CNK_OBJECT road_object_1_object = {
#include "stg13/road_objects/road_object_1_object.inc"
};
static NJS_TEXNAME road_object_2_3_texnames[4] = {
#include "stg13/road_objects/road_object_2_3_texnames.inc"
};
static NJS_TEXLIST road_object_2_3_texlist = {
#include "stg13/road_objects/road_object_2_3_texlist.inc"
};
static Sint16 road_object_2_polygons[281] = {
#include "stg13/road_objects/road_object_2_polygons.inc"
};
static Sint32 road_object_2_3_vertices[315] = {
#include "stg13/road_objects/road_object_2_3_vertices.inc"
};
static Sint16 road_object_3_polygons[281] = {
#include "stg13/road_objects/road_object_3_polygons.inc"
};
static NJS_CNK_MODEL road_object_2_model = {
#include "stg13/road_objects/road_object_2_model.inc"
};
static NJS_CNK_MODEL road_object_3_model = {
#include "stg13/road_objects/road_object_3_model.inc"
};
static NJS_CNK_OBJECT road_object_2_object = {
#include "stg13/road_objects/road_object_2_object.inc"
};
static NJS_CNK_OBJECT road_object_3_object = {
#include "stg13/road_objects/road_object_3_object.inc"
};
static NJS_TEXNAME road_object_4_texnames[1] = {
#include "stg13/road_objects/road_object_4_texnames.inc"
};
static NJS_TEXLIST road_object_4_texlist = {
#include "stg13/road_objects/road_object_4_texlist.inc"
};
static Sint16 road_object_4_polygons[349] = {
#include "stg13/road_objects/road_object_4_polygons.inc"
};
static Sint32 road_object_4_vertices[171] = {
#include "stg13/road_objects/road_object_4_vertices.inc"
};
static NJS_CNK_MODEL road_object_4_model = {
#include "stg13/road_objects/road_object_4_model.inc"
};
static NJS_CNK_OBJECT road_object_4_object = {
#include "stg13/road_objects/road_object_4_object.inc"
};
static NJS_TEXNAME road_object_5_texnames[1] = {
#include "stg13/road_objects/road_object_5_texnames.inc"
};
static NJS_TEXLIST road_object_5_texlist = {
#include "stg13/road_objects/road_object_5_texlist.inc"
};
static Sint16 road_object_5_polygons[307] = {
#include "stg13/road_objects/road_object_5_polygons.inc"
};
static Sint32 road_object_5_vertices[331] = {
#include "stg13/road_objects/road_object_5_vertices.inc"
};
static NJS_CNK_MODEL road_object_5_model = {
#include "stg13/road_objects/road_object_5_model.inc"
};
static NJS_CNK_OBJECT road_object_5_object = {
#include "stg13/road_objects/road_object_5_object.inc"
};
static NJS_TEXNAME road_object_6_texnames[2] = {
#include "stg13/road_objects/road_object_6_texnames.inc"
};
static NJS_TEXLIST road_object_6_texlist = {
#include "stg13/road_objects/road_object_6_texlist.inc"
};
static Sint16 road_object_6_polygons[369] = {
#include "stg13/road_objects/road_object_6_polygons.inc"
};
static Sint32 road_object_6_vertices[403] = {
#include "stg13/road_objects/road_object_6_vertices.inc"
};
static NJS_CNK_MODEL road_object_6_model = {
#include "stg13/road_objects/road_object_6_model.inc"
};
static NJS_CNK_OBJECT road_object_6_object = {
#include "stg13/road_objects/road_object_6_object.inc"
};
static NJS_TEXNAME road_object_7_texnames[2] = {
#include "stg13/road_objects/road_object_7_texnames.inc"
};
static NJS_TEXLIST road_object_7_texlist = {
#include "stg13/road_objects/road_object_7_texlist.inc"
};
static Sint16 road_object_7_polygons[189] = {
#include "stg13/road_objects/road_object_7_polygons.inc"
};
static Sint32 road_object_7_vertices[163] = {
#include "stg13/road_objects/road_object_7_vertices.inc"
};
static NJS_CNK_MODEL road_object_7_model = {
#include "stg13/road_objects/road_object_7_model.inc"
};
static NJS_CNK_OBJECT road_object_7_object = {
#include "stg13/road_objects/road_object_7_object.inc"
};
static NJS_TEXNAME road_object_8_texnames[2] = {
#include "stg13/road_objects/road_object_8_texnames.inc"
};
static NJS_TEXLIST road_object_8_texlist = {
#include "stg13/road_objects/road_object_8_texlist.inc"
};
static Sint16 road_object_8_polygons[247] = {
#include "stg13/road_objects/road_object_8_polygons.inc"
};
static Sint32 road_object_8_vertices[187] = {
#include "stg13/road_objects/road_object_8_vertices.inc"
};
static NJS_CNK_MODEL road_object_8_model = {
#include "stg13/road_objects/road_object_8_model.inc"
};
static NJS_CNK_OBJECT road_object_8_object = {
#include "stg13/road_objects/road_object_8_object.inc"
};
static NJS_TEXNAME road_object_9_texnames[2] = {
#include "stg13/road_objects/road_object_9_texnames.inc"
};
static NJS_TEXLIST road_object_9_texlist = {
#include "stg13/road_objects/road_object_9_texlist.inc"
};
static Sint16 road_object_9_polygons[203] = {
#include "stg13/road_objects/road_object_9_polygons.inc"
};
static Sint32 road_object_9_vertices[163] = {
#include "stg13/road_objects/road_object_9_vertices.inc"
};
static NJS_CNK_MODEL road_object_9_model = {
#include "stg13/road_objects/road_object_9_model.inc"
};
static NJS_CNK_OBJECT road_object_9_object = {
#include "stg13/road_objects/road_object_9_object.inc"
};
static NJS_TEXNAME road_object_10_texnames[1] = {
#include "stg13/road_objects/road_object_10_texnames.inc"
};
static NJS_TEXLIST road_object_10_texlist = {
#include "stg13/road_objects/road_object_10_texlist.inc"
};
static Sint16 road_object_10_polygons[25] = {
#include "stg13/road_objects/road_object_10_polygons.inc"
};
static Sint32 road_object_10_vertices[19] = {
#include "stg13/road_objects/road_object_10_vertices.inc"
};
static NJS_CNK_MODEL road_object_10_model = {
#include "stg13/road_objects/road_object_10_model.inc"
};
static NJS_CNK_OBJECT road_object_10_object = {
#include "stg13/road_objects/road_object_10_object.inc"
};
static NJS_TEXNAME road_object_11_texnames[1] = {
#include "stg13/road_objects/road_object_11_texnames.inc"
};
static NJS_TEXLIST road_object_11_texlist = {
#include "stg13/road_objects/road_object_11_texlist.inc"
};
static Sint16 road_object_11_polygons[91] = {
#include "stg13/road_objects/road_object_11_polygons.inc"
};
static Sint32 road_object_11_vertices[99] = {
#include "stg13/road_objects/road_object_11_vertices.inc"
};
static NJS_CNK_MODEL road_object_11_model = {
#include "stg13/road_objects/road_object_11_model.inc"
};
static NJS_CNK_OBJECT road_object_11_object = {
#include "stg13/road_objects/road_object_11_object.inc"
};
static NJS_TEXNAME road_object_14_texnames[1] = {
#include "stg13/road_objects/road_object_14_texnames.inc"
};
static NJS_TEXLIST road_object_14_texlist = {
#include "stg13/road_objects/road_object_14_texlist.inc"
};
static Sint16 road_object_14_polygons[25] = {
#include "stg13/road_objects/road_object_14_polygons.inc"
};
static Sint32 road_object_14_vertices[19] = {
#include "stg13/road_objects/road_object_14_vertices.inc"
};
static NJS_CNK_MODEL road_object_14_model = {
#include "stg13/road_objects/road_object_14_model.inc"
};
static NJS_CNK_OBJECT road_object_14_object = {
#include "stg13/road_objects/road_object_14_object.inc"
};
static NJS_TEXNAME road_object_15_texnames[1] = {
#include "stg13/road_objects/road_object_15_texnames.inc"
};
static NJS_TEXLIST road_object_15_texlist = {
#include "stg13/road_objects/road_object_15_texlist.inc"
};
static Sint16 road_object_15_polygons[91] = {
#include "stg13/road_objects/road_object_15_polygons.inc"
};
static Sint32 road_object_15_vertices[99] = {
#include "stg13/road_objects/road_object_15_vertices.inc"
};
static NJS_CNK_MODEL road_object_15_model = {
#include "stg13/road_objects/road_object_15_model.inc"
};
static NJS_CNK_OBJECT road_object_15_object = {
#include "stg13/road_objects/road_object_15_object.inc"
};
static NJS_TEXNAME road_object_12_texnames[1] = {
#include "stg13/road_objects/road_object_12_texnames.inc"
};
static NJS_TEXLIST road_object_12_texlist = {
#include "stg13/road_objects/road_object_12_texlist.inc"
};
static Sint16 road_object_12_polygons[25] = {
#include "stg13/road_objects/road_object_12_polygons.inc"
};
static Sint32 road_object_12_vertices[19] = {
#include "stg13/road_objects/road_object_12_vertices.inc"
};
static NJS_CNK_MODEL road_object_12_model = {
#include "stg13/road_objects/road_object_12_model.inc"
};
static NJS_CNK_OBJECT road_object_12_object = {
#include "stg13/road_objects/road_object_12_object.inc"
};
static NJS_TEXNAME road_object_13_texnames[1] = {
#include "stg13/road_objects/road_object_13_texnames.inc"
};
static NJS_TEXLIST road_object_13_texlist = {
#include "stg13/road_objects/road_object_13_texlist.inc"
};
static Sint16 road_object_13_polygons[91] = {
#include "stg13/road_objects/road_object_13_polygons.inc"
};
static Sint32 road_object_13_vertices[99] = {
#include "stg13/road_objects/road_object_13_vertices.inc"
};
static NJS_CNK_MODEL road_object_13_model = {
#include "stg13/road_objects/road_object_13_model.inc"
};
static NJS_CNK_OBJECT road_object_13_object = {
#include "stg13/road_objects/road_object_13_object.inc"
};
static NJS_TEXNAME sign_0_texnames[1] = {
#include "stg13/road_objects/sign_0_texnames.inc"
};
static NJS_TEXLIST sign_0_texlist = {
#include "stg13/road_objects/sign_0_texlist.inc"
};
static Sint16 sign_0_polygons[25] = {
#include "stg13/road_objects/sign_0_polygons.inc"
};
static Sint32 sign_0_vertices[19] = {
#include "stg13/road_objects/sign_0_vertices.inc"
};
static NJS_CNK_MODEL sign_0_model = {
#include "stg13/road_objects/sign_0_model.inc"
};
static NJS_CNK_OBJECT sign_0_object = {
#include "stg13/road_objects/sign_0_object.inc"
};
static NJS_TEXNAME sign_1_texnames[1] = {
#include "stg13/road_objects/sign_1_texnames.inc"
};
static NJS_TEXLIST sign_1_texlist = {
#include "stg13/road_objects/sign_1_texlist.inc"
};
static Sint16 sign_1_polygons[37] = {
#include "stg13/road_objects/sign_1_polygons.inc"
};
static Sint32 sign_1_vertices[35] = {
#include "stg13/road_objects/sign_1_vertices.inc"
};
static NJS_CNK_MODEL sign_1_model = {
#include "stg13/road_objects/sign_1_model.inc"
};
static NJS_CNK_OBJECT sign_1_object = {
#include "stg13/road_objects/sign_1_object.inc"
};
static NJS_TEXNAME sign_2_texnames[1] = {
#include "stg13/road_objects/sign_2_texnames.inc"
};
static NJS_TEXLIST sign_2_texlist = {
#include "stg13/road_objects/sign_2_texlist.inc"
};
static Sint16 sign_2_polygons[37] = {
#include "stg13/road_objects/sign_2_polygons.inc"
};
static Sint32 sign_2_vertices[35] = {
#include "stg13/road_objects/sign_2_vertices.inc"
};
static NJS_CNK_MODEL sign_2_model = {
#include "stg13/road_objects/sign_2_model.inc"
};
static NJS_CNK_OBJECT sign_2_object = {
#include "stg13/road_objects/sign_2_object.inc"
};
static NJS_TEXNAME sign_3_texnames[3] = {
#include "stg13/road_objects/sign_3_texnames.inc"
};
static NJS_TEXLIST sign_3_texlist = {
#include "stg13/road_objects/sign_3_texlist.inc"
};
static Sint16 sign_3_polygons[173] = {
#include "stg13/road_objects/sign_3_polygons.inc"
};
static Sint32 sign_3_vertices[131] = {
#include "stg13/road_objects/sign_3_vertices.inc"
};
static NJS_CNK_MODEL sign_3_model = {
#include "stg13/road_objects/sign_3_model.inc"
};
static NJS_CNK_OBJECT sign_3_object = {
#include "stg13/road_objects/sign_3_object.inc"
};
static NJS_TEXNAME sign_4_texnames[4] = {
#include "stg13/road_objects/sign_4_texnames.inc"
};
static NJS_TEXLIST sign_4_texlist = {
#include "stg13/road_objects/sign_4_texlist.inc"
};
static Sint16 sign_4_polygons[237] = {
#include "stg13/road_objects/sign_4_polygons.inc"
};
static Sint32 sign_4_vertices[203] = {
#include "stg13/road_objects/sign_4_vertices.inc"
};
static NJS_CNK_MODEL sign_4_model = {
#include "stg13/road_objects/sign_4_model.inc"
};
static NJS_CNK_OBJECT sign_4_object = {
#include "stg13/road_objects/sign_4_object.inc"
};
static NJS_TEXNAME sign_5_texnames[4] = {
#include "stg13/road_objects/sign_5_texnames.inc"
};
static NJS_TEXLIST sign_5_texlist = {
#include "stg13/road_objects/sign_5_texlist.inc"
};
static Sint16 sign_5_polygons[145] = {
#include "stg13/road_objects/sign_5_polygons.inc"
};
static Sint32 sign_5_vertices[123] = {
#include "stg13/road_objects/sign_5_vertices.inc"
};
static NJS_CNK_MODEL sign_5_model = {
#include "stg13/road_objects/sign_5_model.inc"
};
static NJS_CNK_OBJECT sign_5_object = {
#include "stg13/road_objects/sign_5_object.inc"
};
static NJS_TEXNAME sign_6_texnames[4] = {
#include "stg13/road_objects/sign_6_texnames.inc"
};
static NJS_TEXLIST sign_6_texlist = {
#include "stg13/road_objects/sign_6_texlist.inc"
};
static Sint16 sign_6_polygons[109] = {
#include "stg13/road_objects/sign_6_polygons.inc"
};
static Sint32 sign_6_vertices[91] = {
#include "stg13/road_objects/sign_6_vertices.inc"
};
static NJS_CNK_MODEL sign_6_model = {
#include "stg13/road_objects/sign_6_model.inc"
};
static NJS_CNK_OBJECT sign_6_object = {
#include "stg13/road_objects/sign_6_object.inc"
};
static NJS_TEXNAME sign_7_texnames[6] = {
#include "stg13/road_objects/sign_7_texnames.inc"
};
static NJS_TEXLIST sign_7_texlist = {
#include "stg13/road_objects/sign_7_texlist.inc"
};
static Sint16 sign_7_polygons[175] = {
#include "stg13/road_objects/sign_7_polygons.inc"
};
static Sint32 sign_7_vertices[155] = {
#include "stg13/road_objects/sign_7_vertices.inc"
};
static NJS_CNK_MODEL sign_7_model = {
#include "stg13/road_objects/sign_7_model.inc"
};
static NJS_CNK_OBJECT sign_7_object = {
#include "stg13/road_objects/sign_7_object.inc"
};
/* two square models, textured flower and flower2. no kind draws them */
static NJS_TEXNAME flower_texnames[1] = {
#include "stg13/road_objects/flower_texnames.inc"
};
static NJS_TEXLIST flower_texlist = {
#include "stg13/road_objects/flower_texlist.inc"
};
static Sint16 flower_polygons[25] = {
#include "stg13/road_objects/flower_polygons.inc"
};
static Sint32 flower_vertices[19] = {
#include "stg13/road_objects/flower_vertices.inc"
};
static NJS_CNK_MODEL flower_model = {
#include "stg13/road_objects/flower_model.inc"
};
static NJS_CNK_OBJECT flower_object = {
#include "stg13/road_objects/flower_object.inc"
};
static NJS_TEXNAME flower_2_texnames[1] = {
#include "stg13/road_objects/flower_2_texnames.inc"
};
static NJS_TEXLIST flower_2_texlist = {
#include "stg13/road_objects/flower_2_texlist.inc"
};
static Sint16 flower_2_polygons[25] = {
#include "stg13/road_objects/flower_2_polygons.inc"
};
static Sint32 flower_2_vertices[19] = {
#include "stg13/road_objects/flower_2_vertices.inc"
};
static NJS_CNK_MODEL flower_2_model = {
#include "stg13/road_objects/flower_2_model.inc"
};
static NJS_CNK_OBJECT flower_2_object = {
#include "stg13/road_objects/flower_2_object.inc"
};
NJS_TEXLIST *road_objects_texlists[25] = {
#include "stg13/road_objects/road_objects_texlists.inc"
};
/* each kind's shapes, named for the first road object that takes them */
/* clang-format off */
static struct collision_shape road_object_0_shapes[1] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 10.0F, 0.0F},
     7.0F, 10.0F, 7.0F, 0, {0, 0, 0}},
};
static struct collision_shape road_object_1_shapes[1] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 10.0F, 9.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape road_object_4_shapes[1] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 40.0F, 0.0F}, 6.0F, 43.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape road_object_2_shapes[2] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 3.5F, 0.0F},
     19.0F, 3.5F, 5.0F, 0, {0, 0, QUARTER_TURN}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 8.0F, -6.5F},
     19.0F, 8.0F, 0.8F, 0, {0, 0, QUARTER_TURN}},
};
static struct collision_shape road_object_5_shapes[1] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 9.0F, 0.0F},
     5.0F, 9.0F, 12.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* The original member also stores two spheres that no object kind takes. */
static struct collision_shape unused_small_sphere[1] = {
#include "stg13/road_objects/unused_small_sphere.inc"
};
static struct collision_shape unused_large_sphere[1] = {
#include "stg13/road_objects/unused_large_sphere.inc"
};

static struct model_kind road_object_kinds[ROAD_KINDS] = {
    {KIND_BUILD | KIND_SHADOWED, &road_object_0_texlist, &road_object_0_object,
     road_object_0_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD | KIND_SHADOWED, &road_object_1_texlist, &road_object_1_object,
     road_object_1_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD | KIND_SHADOWED, &road_object_2_3_texlist,
     &road_object_2_object, road_object_2_shapes, 2, road_object_draw, NULL,
     NULL, 0, NULL},
    {KIND_BUILD | KIND_SHADOWED, &road_object_2_3_texlist,
     &road_object_3_object, road_object_2_shapes, 2, road_object_draw, NULL,
     NULL, 0, NULL},
    {KIND_BUILD, &road_object_4_texlist, &road_object_4_object,
     road_object_4_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD | KIND_SHADOWED, &road_object_5_texlist, &road_object_5_object,
     road_object_5_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD | KIND_SHADOWED, &road_object_6_texlist, &road_object_6_object,
     road_object_5_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_7_texlist, &road_object_7_object, NULL, 1,
     road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_8_texlist, &road_object_8_object, NULL, 1,
     road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_9_texlist, &road_object_9_object, NULL, 1,
     road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_10_texlist, &road_object_10_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_11_texlist, &road_object_11_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_12_texlist, &road_object_12_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_13_texlist, &road_object_13_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_14_texlist, &road_object_14_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_15_texlist, &road_object_15_object, NULL, 1,
     road_object_draw_scaled, NULL, NULL, 0, NULL},
};
static struct model_kind sign_kinds[SIGN_KINDS] = {
    {KIND_BUILD, &sign_0_texlist, &sign_0_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_1_texlist, &sign_1_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_2_texlist, &sign_2_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_3_texlist, &sign_3_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_4_texlist, &sign_4_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_5_texlist, &sign_5_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_6_texlist, &sign_6_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_7_texlist, &sign_7_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
};
static struct model_kind lookup_kinds[LOOKUP_KINDS] = {
    {KIND_BUILD, &road_object_0_texlist, &road_object_0_object,
     road_object_0_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_4_texlist, &road_object_4_object,
     road_object_4_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_5_texlist, &road_object_5_object,
     road_object_5_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &road_object_6_texlist, &road_object_6_object,
     road_object_5_shapes, 1, road_object_draw, NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_3_texlist, &sign_3_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_4_texlist, &sign_4_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_5_texlist, &sign_5_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_6_texlist, &sign_6_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
    {KIND_BUILD, &sign_7_texlist, &sign_7_object, NULL, 1, road_object_draw,
     NULL, NULL, 0, NULL},
};
static struct model_kind_list kind_lists[LISTS] = {
    {road_object_kinds, ROAD_KINDS},
    {sign_kinds, SIGN_KINDS},
    {lookup_kinds, LOOKUP_KINDS},
};

int road_objects_lookup_count(void)
{
    return LOOKUP_KINDS;
}

int road_objects_lookup(int index, NJS_TEXLIST **texlist,
                        NJS_CNK_OBJECT **object, NJS_DIRECT_HEAD **built)
{
    int i;
    struct model_kind *kind;
    int count;

    /* @hack type: i converted to a Uint32 for the test, where i would do */
    for (i = LIST_LOOKUP; (Uint32)i < LISTS; i++) {
        count = kind_lists[i].count;
        /* @hack spelling: an else after the return, where the subtraction
         * after the if would do */
        if (index < count) {
            kind = &kind_lists[i].kinds[index];
            *texlist = kind->texlist;
            *object = kind->object;
            *built = kind->built;
            return 1;
        } else {
            index -= count;
        }
    }
    return 0;
}

void road_object_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &kind_lists[LIST_ROAD];
        road_object_start(task);
    }
}

void sign_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &kind_lists[LIST_SIGN];
        road_object_start(task);
    }
}

Uint8 *road_objects_build(Uint8 *buf)
{
    int i;
    int j;
    struct model_kind_list *list;
    struct model_kind *kind;
    NJS_CNK_OBJECT object;
    Uint8 *end;

    /* @hack type: i converted to a Uint32 for the test, as in
     * road_objects_lookup's loop */
    for (i = 0; (Uint32)i < LISTS; i++) {
        list = &kind_lists[i];
        for (j = 0; j < list->count; j++) {
            kind = &list->kinds[j];
            if ((kind->flags & KIND_BUILD) && !(kind->flags & KIND_COMPILED)) {
                object = *kind->object;
                if (!(kind->flags & KIND_TREE)) {
                    object.sibling = NULL;
                    object.child = NULL;
                    object.evalflags |= NJD_EVAL_BREAK;
                }
                if (kind->flags & KIND_SHADOWED) {
                    draw_control_set(DRAW_CONTROL_SHADOW);
                }
                if (kind->texlist != NULL) {
                    njSetTexture(kind->texlist);
                }
                end = buf + njCnkDirectObjectCompileSize(&object);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(&object, buf);
                    kind->built = (NJS_DIRECT_HEAD *)buf;
                    buf = end;
                    kind->flags |= KIND_COMPILED;
                }
                if (kind->flags & KIND_SHADOWED) {
                    draw_control_clear(DRAW_CONTROL_SHADOW);
                }
            }
        }
    }
    return buf;
}

static void road_object_start(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;

    task->step = road_object_step;
    task->destructor = road_object_destroy;
    work->ang.x &= KIND_INDEX;
    work->ang.x %= LIST(task)->count;
    kind = KIND(task, work);
    if (kind->shapes != NULL) {
        collision_init(task, kind->shapes, kind->shape_count, COLLISION_OBJECT);
    }
    task->draw = kind->draw;
    MATRIX(task) = NULL;
    if (kind->flags & KIND_MATRIX) {
        MATRIX(task) = syMalloc(sizeof(NJS_MATRIX));
        if (MATRIX(task) != NULL) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njGetMatrix(MATRIX(task));
            njPopMatrixEx();
        }
    }
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
}

static void road_object_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (MATRIX(task) != NULL) {
        syFree(MATRIX(task));
        MATRIX(task) = NULL;
    }
    task->motion.p = NULL;
    task->extra.p = NULL;
    task->force.p = NULL;
    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

static void road_object_step(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODEL(0-  )";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE X";
        (void)"<- ANGLE Z";
    }
    if (!placement_try_leave(task)) {
        kind = KIND(task, work);
        if (kind->step != NULL) {
            kind->step(task, work, kind);
        } else if (work->collision != NULL) {
            collision_register(task);
        }
    }
}

/* road_object_step, with the kind's index in the work's scl.x, which nothing
 * calls. the image holds it. SHC leaves out a static function its source never
 * names, so it is external */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void road_object_step_by_scl_x(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind;

    if (!placement_try_leave(task)) {
        kind = &LIST(task)->kinds[(int)work->scl.x];
        if (kind->step != NULL) {
            kind->step(task, work, kind);
        } else if (work->collision != NULL) {
            collision_register(task);
        }
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

static void road_object_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);
    Uint32 flags = kind->flags;

    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (flags & KIND_SHADOWED) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    if (flags & KIND_COMPILED) {
        njDirectDrawObject(kind->built);
    } else {
        njCnkEasyDrawModel(kind->object->model);
    }
    if (flags & KIND_SHADOWED) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    njPopMatrixEx();
}

static void road_object_draw_scaled(struct task *task)
{
    struct task_work *work = task->work;
    struct model_kind *kind = KIND(task, work);
    Uint32 flags = kind->flags;
    Float stretch;

    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    stretch = work->scl.y + 1.0F;
    njScale(NULL, 1.0F, stretch, stretch);
    if (flags & KIND_SHADOWED) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    if (flags & KIND_COMPILED) {
        njDirectDrawObject(kind->built);
    } else {
        njCnkEasyDrawModel(kind->object->model);
    }
    if (flags & KIND_SHADOWED) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    njPopMatrixEx();
}
