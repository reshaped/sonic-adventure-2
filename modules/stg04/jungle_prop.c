/* White Jungle's props, JP and JP_FAR in the stage's object list. each is
 * one of sixteen models, and JP's model 0 stands in a collision sphere */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "object/set.h"
#include "stg04/jungle_prop.h"
#include "system/task.h"

#include "stg04/jungle_prop/text.inc"

/* Keep the sixteen independent meshes in their exported order. */
static NJS_TEXNAME prop_0_names[2] = {
#include "stg04/jungle_prop/prop_0_names.inc"
};

static NJS_TEXLIST prop_0_textures = {
#include "stg04/jungle_prop/prop_0_textures.inc"
};

static Sint16 prop_0_polygons[353] = {
#include "stg04/jungle_prop/prop_0_polygons.inc"
};

static Sint32 prop_0_vertices[327] = {
#include "stg04/jungle_prop/prop_0_vertices.inc"
};

static NJS_CNK_MODEL prop_0_model = {
#include "stg04/jungle_prop/prop_0_model.inc"
};

static NJS_CNK_OBJECT prop_0_object = {
#include "stg04/jungle_prop/prop_0_object.inc"
};

static NJS_TEXNAME prop_1_names[1] = {
#include "stg04/jungle_prop/prop_1_names.inc"
};

static NJS_TEXLIST prop_1_textures = {
#include "stg04/jungle_prop/prop_1_textures.inc"
};

static Sint16 prop_1_polygons[269] = {
#include "stg04/jungle_prop/prop_1_polygons.inc"
};

static Sint32 prop_1_vertices[227] = {
#include "stg04/jungle_prop/prop_1_vertices.inc"
};

static NJS_CNK_MODEL prop_1_model = {
#include "stg04/jungle_prop/prop_1_model.inc"
};

static NJS_CNK_OBJECT prop_1_object = {
#include "stg04/jungle_prop/prop_1_object.inc"
};

static NJS_TEXNAME prop_2_names[1] = {
#include "stg04/jungle_prop/prop_2_names.inc"
};

static NJS_TEXLIST prop_2_textures = {
#include "stg04/jungle_prop/prop_2_textures.inc"
};

static Sint16 prop_2_polygons[539] = {
#include "stg04/jungle_prop/prop_2_polygons.inc"
};

static Sint32 prop_2_vertices[411] = {
#include "stg04/jungle_prop/prop_2_vertices.inc"
};

static NJS_CNK_MODEL prop_2_model = {
#include "stg04/jungle_prop/prop_2_model.inc"
};

static NJS_CNK_OBJECT prop_2_object = {
#include "stg04/jungle_prop/prop_2_object.inc"
};

static NJS_TEXNAME prop_3_names[2] = {
#include "stg04/jungle_prop/prop_3_names.inc"
};

static NJS_TEXLIST prop_3_textures = {
#include "stg04/jungle_prop/prop_3_textures.inc"
};

static Sint16 prop_3_polygons[351] = {
#include "stg04/jungle_prop/prop_3_polygons.inc"
};

static Sint32 prop_3_vertices[175] = {
#include "stg04/jungle_prop/prop_3_vertices.inc"
};

static NJS_CNK_MODEL prop_3_model = {
#include "stg04/jungle_prop/prop_3_model.inc"
};

static NJS_CNK_OBJECT prop_3_object = {
#include "stg04/jungle_prop/prop_3_object.inc"
};

static NJS_TEXNAME prop_4_names[2] = {
#include "stg04/jungle_prop/prop_4_names.inc"
};

static NJS_TEXLIST prop_4_textures = {
#include "stg04/jungle_prop/prop_4_textures.inc"
};

static Sint16 prop_4_polygons[347] = {
#include "stg04/jungle_prop/prop_4_polygons.inc"
};

static Sint32 prop_4_vertices[315] = {
#include "stg04/jungle_prop/prop_4_vertices.inc"
};

static NJS_CNK_MODEL prop_4_model = {
#include "stg04/jungle_prop/prop_4_model.inc"
};

static NJS_CNK_OBJECT prop_4_object = {
#include "stg04/jungle_prop/prop_4_object.inc"
};

static NJS_TEXNAME prop_5_names[1] = {
#include "stg04/jungle_prop/prop_5_names.inc"
};

static NJS_TEXLIST prop_5_textures = {
#include "stg04/jungle_prop/prop_5_textures.inc"
};

static Sint16 prop_5_polygons[195] = {
#include "stg04/jungle_prop/prop_5_polygons.inc"
};

static Sint32 prop_5_vertices[171] = {
#include "stg04/jungle_prop/prop_5_vertices.inc"
};

static NJS_CNK_MODEL prop_5_model = {
#include "stg04/jungle_prop/prop_5_model.inc"
};

static NJS_CNK_OBJECT prop_5_object = {
#include "stg04/jungle_prop/prop_5_object.inc"
};

static NJS_TEXNAME prop_6_names[2] = {
#include "stg04/jungle_prop/prop_6_names.inc"
};

static NJS_TEXLIST prop_6_textures = {
#include "stg04/jungle_prop/prop_6_textures.inc"
};

static Sint16 prop_6_polygons[245] = {
#include "stg04/jungle_prop/prop_6_polygons.inc"
};

static Sint32 prop_6_vertices[243] = {
#include "stg04/jungle_prop/prop_6_vertices.inc"
};

static NJS_CNK_MODEL prop_6_model = {
#include "stg04/jungle_prop/prop_6_model.inc"
};

static NJS_CNK_OBJECT prop_6_object = {
#include "stg04/jungle_prop/prop_6_object.inc"
};

static NJS_TEXNAME prop_7_names[1] = {
#include "stg04/jungle_prop/prop_7_names.inc"
};

static NJS_TEXLIST prop_7_textures = {
#include "stg04/jungle_prop/prop_7_textures.inc"
};

static Sint16 prop_7_polygons[283] = {
#include "stg04/jungle_prop/prop_7_polygons.inc"
};

static Sint32 prop_7_vertices[227] = {
#include "stg04/jungle_prop/prop_7_vertices.inc"
};

static NJS_CNK_MODEL prop_7_model = {
#include "stg04/jungle_prop/prop_7_model.inc"
};

static NJS_CNK_OBJECT prop_7_object = {
#include "stg04/jungle_prop/prop_7_object.inc"
};

static NJS_TEXNAME prop_8_names[1] = {
#include "stg04/jungle_prop/prop_8_names.inc"
};

static NJS_TEXLIST prop_8_textures = {
#include "stg04/jungle_prop/prop_8_textures.inc"
};

static Sint16 prop_8_polygons[221] = {
#include "stg04/jungle_prop/prop_8_polygons.inc"
};

static Sint32 prop_8_vertices[163] = {
#include "stg04/jungle_prop/prop_8_vertices.inc"
};

static NJS_CNK_MODEL prop_8_model = {
#include "stg04/jungle_prop/prop_8_model.inc"
};

static NJS_CNK_OBJECT prop_8_object = {
#include "stg04/jungle_prop/prop_8_object.inc"
};

static NJS_TEXNAME prop_15_names[1] = {
#include "stg04/jungle_prop/prop_15_names.inc"
};

static NJS_TEXLIST prop_15_textures = {
#include "stg04/jungle_prop/prop_15_textures.inc"
};

static Sint16 prop_15_polygons[31] = {
#include "stg04/jungle_prop/prop_15_polygons.inc"
};

static Sint32 prop_15_vertices[27] = {
#include "stg04/jungle_prop/prop_15_vertices.inc"
};

static NJS_CNK_MODEL prop_15_model = {
#include "stg04/jungle_prop/prop_15_model.inc"
};

static NJS_CNK_OBJECT prop_15_object = {
#include "stg04/jungle_prop/prop_15_object.inc"
};

static NJS_TEXNAME prop_9_names[3] = {
#include "stg04/jungle_prop/prop_9_names.inc"
};

NJS_TEXLIST white_jungle_prop_texlist_9 = {
#include "stg04/jungle_prop/white_jungle_prop_texlist_9.inc"
};

static Sint16 prop_9_polygons[423] = {
#include "stg04/jungle_prop/prop_9_polygons.inc"
};

static Sint32 prop_9_vertices[327] = {
#include "stg04/jungle_prop/prop_9_vertices.inc"
};

NJS_CNK_MODEL white_jungle_prop_model_9 = {
#include "stg04/jungle_prop/white_jungle_prop_model_9.inc"
};

static NJS_CNK_OBJECT prop_9_object = {
#include "stg04/jungle_prop/prop_9_object.inc"
};

static NJS_TEXNAME prop_10_names[3] = {
#include "stg04/jungle_prop/prop_10_names.inc"
};

static NJS_TEXLIST prop_10_textures = {
#include "stg04/jungle_prop/prop_10_textures.inc"
};

static Sint16 prop_10_polygons[575] = {
#include "stg04/jungle_prop/prop_10_polygons.inc"
};

static Sint32 prop_10_vertices[499] = {
#include "stg04/jungle_prop/prop_10_vertices.inc"
};

static NJS_CNK_MODEL prop_10_model = {
#include "stg04/jungle_prop/prop_10_model.inc"
};

static NJS_CNK_OBJECT prop_10_object = {
#include "stg04/jungle_prop/prop_10_object.inc"
};

static NJS_TEXNAME prop_11_names[3] = {
#include "stg04/jungle_prop/prop_11_names.inc"
};

static NJS_TEXLIST prop_11_textures = {
#include "stg04/jungle_prop/prop_11_textures.inc"
};

static Sint16 prop_11_polygons[417] = {
#include "stg04/jungle_prop/prop_11_polygons.inc"
};

static Sint32 prop_11_vertices[359] = {
#include "stg04/jungle_prop/prop_11_vertices.inc"
};

static NJS_CNK_MODEL prop_11_model = {
#include "stg04/jungle_prop/prop_11_model.inc"
};

static NJS_CNK_OBJECT prop_11_object = {
#include "stg04/jungle_prop/prop_11_object.inc"
};

static NJS_TEXNAME prop_12_names[4] = {
#include "stg04/jungle_prop/prop_12_names.inc"
};

static NJS_TEXLIST prop_12_textures = {
#include "stg04/jungle_prop/prop_12_textures.inc"
};

static Sint16 prop_12_polygons[573] = {
#include "stg04/jungle_prop/prop_12_polygons.inc"
};

static Sint32 prop_12_vertices[495] = {
#include "stg04/jungle_prop/prop_12_vertices.inc"
};

static NJS_CNK_MODEL prop_12_model = {
#include "stg04/jungle_prop/prop_12_model.inc"
};

static NJS_CNK_OBJECT prop_12_object = {
#include "stg04/jungle_prop/prop_12_object.inc"
};

static NJS_TEXNAME prop_13_names[2] = {
#include "stg04/jungle_prop/prop_13_names.inc"
};

static NJS_TEXLIST prop_13_textures = {
#include "stg04/jungle_prop/prop_13_textures.inc"
};

static Sint16 prop_13_polygons[319] = {
#include "stg04/jungle_prop/prop_13_polygons.inc"
};

static Sint32 prop_13_vertices[299] = {
#include "stg04/jungle_prop/prop_13_vertices.inc"
};

static NJS_CNK_MODEL prop_13_model = {
#include "stg04/jungle_prop/prop_13_model.inc"
};

static NJS_CNK_OBJECT prop_13_object = {
#include "stg04/jungle_prop/prop_13_object.inc"
};

static NJS_TEXNAME prop_14_names[2] = {
#include "stg04/jungle_prop/prop_14_names.inc"
};

static NJS_TEXLIST prop_14_textures = {
#include "stg04/jungle_prop/prop_14_textures.inc"
};

static Sint16 prop_14_polygons[339] = {
#include "stg04/jungle_prop/prop_14_polygons.inc"
};

static Sint32 prop_14_vertices[319] = {
#include "stg04/jungle_prop/prop_14_vertices.inc"
};

static NJS_CNK_MODEL prop_14_model = {
#include "stg04/jungle_prop/prop_14_model.inc"
};

static NJS_CNK_OBJECT prop_14_object = {
#include "stg04/jungle_prop/prop_14_object.inc"
};

/* the props' models, of which the work's scl.x picks one */
#define PROPS 16
/* the shapes of a prop's collision record */
#define PROP_SHAPES 1
/* the range JP_FAR's prop leaves at, in place of its placement's, squared as
 * placement_try_leave_by takes it */
#define FAR_RANGE (1450.0F * 1450.0F)
/* the prop's model and texture list, in two fields of task that task_destroy
 * frees. prop_destroy empties both first */
#define PROP_MODEL(task) ((task)->motion.p)
#define PROP_TEXLIST(task) ((task)->force.p)

static void prop_destroy(struct task *task);
static void prop_step(struct task *task);
static void prop_draw(struct task *task);
static void prop_far_step(struct task *task);

NJS_TEXLIST *white_jungle_prop_texlists[PROPS + 1] = {
#include "stg04/jungle_prop/white_jungle_prop_texlists.inc"
};
static NJS_CNK_MODEL *prop_models[PROPS] = {
#include "stg04/jungle_prop/prop_models.inc"
};
/* clang-format off */
static struct collision_shape prop_sphere = {
#include "stg04/jungle_prop/prop_sphere.inc"
};
/* @hack type: the shapes in a record of their own, where the array alone
 * would do */
struct prop_shapes {
    struct collision_shape *shapes[PROPS]; /* by prop, null for none */
};

static struct prop_shapes prop_shapes = {
#include "stg04/jungle_prop/prop_shapes.inc"
};
/* clang-format on */

void white_jungle_prop_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)PROP_EDITOR_ANGLE_X;
        (void)PROP_EDITOR_ANGLE_Y;
        (void)PROP_EDITOR_ANGLE_Z;
        (void)PROP_EDITOR_SHAPE;
    }
    if (!placement_try_leave(task)) {
        task->draw = prop_draw;
        task->step = prop_step;
        task->destructor = prop_destroy;
        PROP_MODEL(task) = prop_models[(Uint32)work->scl.x % PROPS];
        PROP_TEXLIST(task) =
            white_jungle_prop_texlists[(Uint32)work->scl.x % PROPS];
        if (prop_shapes.shapes[(Uint32)work->scl.x]) {
            collision_init(task, prop_shapes.shapes[(Uint32)work->scl.x],
                           PROP_SHAPES, COLLISION_OBJECT);
        }
    }
}

static void prop_destroy(struct task *task)
{
    PROP_MODEL(task) = NULL;
    PROP_TEXLIST(task) = NULL;
}

/* @hack redundant: task and work declared register, where ordinary ones would
 * do */
static void prop_step(register struct task *task)
{
    register struct task_work *work = task->work;

    /* @hack redundant: the else after the return */
    if (placement_try_leave(task)) {
        return;
    } else if (work->collision) {
        collision_register(task);
    }
}

static void prop_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(PROP_TEXLIST(task));
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(PROP_MODEL(task));
    njPopMatrix(1);
}

void white_jungle_prop_far_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave_by(task, FAR_RANGE)) {
        task->draw = prop_draw;
        task->step = prop_far_step;
        task->destructor = prop_destroy;
        PROP_MODEL(task) = prop_models[(Uint32)work->scl.x % PROPS];
        PROP_TEXLIST(task) =
            white_jungle_prop_texlists[(Uint32)work->scl.x % PROPS];
        /* NOLINTBEGIN(clang-diagnostic-array-bounds) */
        /* @bug memory: reads prop_shapes' entry PROPS, one past its end, at
         * 0x8c500fec and 0x8c500ff4. the word there points at the name of the
         * bungee vine's first texture, so collision_init copies a shape from
         * that name's bytes */
#ifdef NONMATCHING
        /* this side repairs the tag above: the far prop takes no collision
         * record. the image's record collides with nothing, since its
         * shape's flags hold SHAPE_OFF. at each step it still takes an entry
         * in COLLISION_OBJECT's list, which this side leaves free */
#else
        if (prop_shapes.shapes[PROPS]) {
            collision_init(task, prop_shapes.shapes[PROPS], PROP_SHAPES,
                           COLLISION_OBJECT);
        }
#endif
        /* NOLINTEND(clang-diagnostic-array-bounds) */
    }
}

/* @hack redundant: task and work declared register, where ordinary ones would
 * do */
static void prop_far_step(register struct task *task)
{
    register struct task_work *work = task->work;

    /* @hack redundant: the else after the return */
    if (placement_try_leave_by(task, FAR_RANGE)) {
        return;
    } else if (work->collision) {
        collision_register(task);
    }
}
