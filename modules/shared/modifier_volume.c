/* the modifier volumes, MODMOD in a stage's object list. each is a box or a
 * cylinder, drawn as a modifier volume */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/modifier_volume.h"
#include "system/task.h"

/* the work's timer holds the shape's index in modifier_volume_shapes: the
 * low 4 bits of ang.z, modulo MODIFIER_VOLUME_SHAPES */
#define SHAPE_BITS 0xf
#define MODIFIER_VOLUME_SHAPES 2
#define SHAPE(work) ((work)->timer)

/* the factor that scales a shape along an axis, from that axis's scl. a scl
 * of 0 leaves the shape at its models' size */
#define SCALE(scl) ((scl) + 1.0F)

/* a shape's two models */
struct modifier_volume_shape {
    NJS_CNK_MODEL *solid; /* a strip chunk of light gray faces */
    NJS_CNK_MODEL *volume; /* the volume chunk the draw draws */
};

static void modifier_volume_destroy(struct task *task);
static void modifier_volume_step(struct task *task);
static void modifier_volume_draw(struct task *task);

static Sint16 modifier_volume_box_polygons[41] = {
#include "shared/modifier_volume/modifier_volume_box_polygons.inc"
};
struct modifier_volume_box_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[8];
    Sint32 end;
};
static struct modifier_volume_box_vertex_list modifier_volume_box_vertices = {
#include "shared/modifier_volume/modifier_volume_box_vertices.inc"
};
NJS_CNK_MODEL modifier_volume_box_model = {
#include "shared/modifier_volume/modifier_volume_box_model.inc"
};
static NJS_CNK_OBJECT modifier_volume_box_object = {
#include "shared/modifier_volume/modifier_volume_box_object.inc"
};

static Sint16 modifier_volume_cylinder_polygons[77] = {
#include "shared/modifier_volume/modifier_volume_cylinder_polygons.inc"
};
struct modifier_volume_cylinder_vertex_list {
    Sint32 head[2];
    NJS_POINT3 vertices[14];
    Sint32 end;
};
static struct modifier_volume_cylinder_vertex_list
    modifier_volume_cylinder_vertices = {
#include "shared/modifier_volume/modifier_volume_cylinder_vertices.inc"
};
NJS_CNK_MODEL modifier_volume_cylinder_model = {
#include "shared/modifier_volume/modifier_volume_cylinder_model.inc"
};
static NJS_CNK_OBJECT modifier_volume_cylinder_object = {
#include "shared/modifier_volume/modifier_volume_cylinder_object.inc"
};

static Sint16 modifier_volume_box_solid_polygons[33] = {
#include "shared/modifier_volume/modifier_volume_box_solid_polygons.inc"
};
static Sint32 modifier_volume_box_solid_vertices[51] = {
#include "shared/modifier_volume/modifier_volume_box_solid_vertices.inc"
};
static NJS_CNK_MODEL modifier_volume_box_solid_model = {
#include "shared/modifier_volume/modifier_volume_box_solid_model.inc"
};
static NJS_CNK_OBJECT modifier_volume_box_solid_object = {
#include "shared/modifier_volume/modifier_volume_box_solid_object.inc"
};

static Sint16 modifier_volume_cylinder_solid_polygons[51] = {
#include "shared/modifier_volume/modifier_volume_cylinder_solid_polygons.inc"
};
static Sint32 modifier_volume_cylinder_solid_vertices[87] = {
#include "shared/modifier_volume/modifier_volume_cylinder_solid_vertices.inc"
};
static NJS_CNK_MODEL modifier_volume_cylinder_solid_model = {
#include "shared/modifier_volume/modifier_volume_cylinder_solid_model.inc"
};
static NJS_CNK_OBJECT modifier_volume_cylinder_solid_object = {
#include "shared/modifier_volume/modifier_volume_cylinder_solid_object.inc"
};

static struct modifier_volume_shape
    modifier_volume_shapes[MODIFIER_VOLUME_SHAPES] = {
#include "shared/modifier_volume/modifier_volume_shapes.inc"
};

void modifier_volume_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = modifier_volume_draw;
    task->step = modifier_volume_step;
    task->destructor = modifier_volume_destroy;
    SHAPE(work) = (work->ang.z & SHAPE_BITS) % MODIFIER_VOLUME_SHAPES;
}

static void modifier_volume_destroy(struct task *task)
{
}

static void modifier_volume_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a readout no path reaches. SHC writes none of its
     * code, but keeps its strings, and still sets up the registers its calls
     * would use. the njScale call stands in for those calls */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- SHAPE";
        (void)"<- SCL X";
        (void)"<- SCL Y";
        (void)"<- SCL Z";
        njScale(NULL, SCALE(work->scl.x), SCALE(work->scl.y),
                SCALE(work->scl.z));
    }
}

static void modifier_volume_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, SCALE(work->scl.x), SCALE(work->scl.y), SCALE(work->scl.z));
    njCnkModDrawModel(modifier_volume_shapes[SHAPE(work)].volume);
    njPopMatrix(1);
}
