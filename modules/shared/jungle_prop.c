/* the jungle's props, JP and JP_FAR in a stage's object list. each is one of
 * eighteen models, which nothing collides with */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/jungle_prop.h"
#include "system/task.h"
#ifdef NONMATCHING
/* texture_slot_loaded, by which this side tests a prop's texture */
#include "system/texture.h"
#endif

#include "shared/jungle_prop/text.inc"

/* The original model bank places shape 16 between shapes 9 and 10. */
static NJS_TEXNAME prop_0_names[3] = {
#include "shared/jungle_prop/prop_0_names.inc"
};

static NJS_TEXLIST prop_0_textures = {
#include "shared/jungle_prop/prop_0_textures.inc"
};

static Sint16 prop_0_polygons[289] = {
#include "shared/jungle_prop/prop_0_polygons.inc"
};

static Sint32 prop_0_vertices[271] = {
#include "shared/jungle_prop/prop_0_vertices.inc"
};

static NJS_CNK_MODEL prop_0_model = {
#include "shared/jungle_prop/prop_0_model.inc"
};

static NJS_CNK_OBJECT prop_0_object = {
#include "shared/jungle_prop/prop_0_object.inc"
};

static NJS_TEXNAME prop_1_names[2] = {
#include "shared/jungle_prop/prop_1_names.inc"
};

static NJS_TEXLIST prop_1_textures = {
#include "shared/jungle_prop/prop_1_textures.inc"
};

static Sint16 prop_1_polygons[353] = {
#include "shared/jungle_prop/prop_1_polygons.inc"
};

static Sint32 prop_1_vertices[327] = {
#include "shared/jungle_prop/prop_1_vertices.inc"
};

static NJS_CNK_MODEL prop_1_model = {
#include "shared/jungle_prop/prop_1_model.inc"
};

static NJS_CNK_OBJECT prop_1_object = {
#include "shared/jungle_prop/prop_1_object.inc"
};

static NJS_TEXNAME prop_2_names[1] = {
#include "shared/jungle_prop/prop_2_names.inc"
};

static NJS_TEXLIST prop_2_textures = {
#include "shared/jungle_prop/prop_2_textures.inc"
};

static Sint16 prop_2_polygons[301] = {
#include "shared/jungle_prop/prop_2_polygons.inc"
};

static Sint32 prop_2_vertices[255] = {
#include "shared/jungle_prop/prop_2_vertices.inc"
};

static NJS_CNK_MODEL prop_2_model = {
#include "shared/jungle_prop/prop_2_model.inc"
};

static NJS_CNK_OBJECT prop_2_object = {
#include "shared/jungle_prop/prop_2_object.inc"
};

static NJS_TEXNAME prop_3_names[1] = {
#include "shared/jungle_prop/prop_3_names.inc"
};

static NJS_TEXLIST prop_3_textures = {
#include "shared/jungle_prop/prop_3_textures.inc"
};

static Sint16 prop_3_polygons[507] = {
#include "shared/jungle_prop/prop_3_polygons.inc"
};

static Sint32 prop_3_vertices[383] = {
#include "shared/jungle_prop/prop_3_vertices.inc"
};

static NJS_CNK_MODEL prop_3_model = {
#include "shared/jungle_prop/prop_3_model.inc"
};

static NJS_CNK_OBJECT prop_3_object = {
#include "shared/jungle_prop/prop_3_object.inc"
};

static NJS_TEXNAME prop_4_names[2] = {
#include "shared/jungle_prop/prop_4_names.inc"
};

static NJS_TEXLIST prop_4_textures = {
#include "shared/jungle_prop/prop_4_textures.inc"
};

static Sint16 prop_4_polygons[327] = {
#include "shared/jungle_prop/prop_4_polygons.inc"
};

static Sint32 prop_4_vertices[227] = {
#include "shared/jungle_prop/prop_4_vertices.inc"
};

static NJS_CNK_MODEL prop_4_model = {
#include "shared/jungle_prop/prop_4_model.inc"
};

static NJS_CNK_OBJECT prop_4_object = {
#include "shared/jungle_prop/prop_4_object.inc"
};

static NJS_TEXNAME prop_5_names[2] = {
#include "shared/jungle_prop/prop_5_names.inc"
};

static NJS_TEXLIST prop_5_textures = {
#include "shared/jungle_prop/prop_5_textures.inc"
};

static Sint16 prop_5_polygons[99] = {
#include "shared/jungle_prop/prop_5_polygons.inc"
};

static Sint32 prop_5_vertices[107] = {
#include "shared/jungle_prop/prop_5_vertices.inc"
};

static NJS_CNK_MODEL prop_5_model = {
#include "shared/jungle_prop/prop_5_model.inc"
};

static NJS_CNK_OBJECT prop_5_object = {
#include "shared/jungle_prop/prop_5_object.inc"
};

static NJS_TEXNAME prop_6_names[2] = {
#include "shared/jungle_prop/prop_6_names.inc"
};

static NJS_TEXLIST prop_6_textures = {
#include "shared/jungle_prop/prop_6_textures.inc"
};

static Sint16 prop_6_polygons[265] = {
#include "shared/jungle_prop/prop_6_polygons.inc"
};

static Sint32 prop_6_vertices[243] = {
#include "shared/jungle_prop/prop_6_vertices.inc"
};

static NJS_CNK_MODEL prop_6_model = {
#include "shared/jungle_prop/prop_6_model.inc"
};

static NJS_CNK_OBJECT prop_6_object = {
#include "shared/jungle_prop/prop_6_object.inc"
};

static NJS_TEXNAME prop_7_names[2] = {
#include "shared/jungle_prop/prop_7_names.inc"
};

static NJS_TEXLIST prop_7_textures = {
#include "shared/jungle_prop/prop_7_textures.inc"
};

static Sint16 prop_7_polygons[245] = {
#include "shared/jungle_prop/prop_7_polygons.inc"
};

static Sint32 prop_7_vertices[243] = {
#include "shared/jungle_prop/prop_7_vertices.inc"
};

static NJS_CNK_MODEL prop_7_model = {
#include "shared/jungle_prop/prop_7_model.inc"
};

static NJS_CNK_OBJECT prop_7_object = {
#include "shared/jungle_prop/prop_7_object.inc"
};

static NJS_TEXNAME prop_8_names[1] = {
#include "shared/jungle_prop/prop_8_names.inc"
};

static NJS_TEXLIST prop_8_textures = {
#include "shared/jungle_prop/prop_8_textures.inc"
};

static Sint16 prop_8_polygons[317] = {
#include "shared/jungle_prop/prop_8_polygons.inc"
};

static Sint32 prop_8_vertices[227] = {
#include "shared/jungle_prop/prop_8_vertices.inc"
};

static NJS_CNK_MODEL prop_8_model = {
#include "shared/jungle_prop/prop_8_model.inc"
};

static NJS_CNK_OBJECT prop_8_object = {
#include "shared/jungle_prop/prop_8_object.inc"
};

static NJS_TEXNAME prop_9_names[3] = {
#include "shared/jungle_prop/prop_9_names.inc"
};

static NJS_TEXLIST prop_9_textures = {
#include "shared/jungle_prop/prop_9_textures.inc"
};

static Sint16 prop_9_polygons[277] = {
#include "shared/jungle_prop/prop_9_polygons.inc"
};

static Sint32 prop_9_vertices[231] = {
#include "shared/jungle_prop/prop_9_vertices.inc"
};

static NJS_CNK_MODEL prop_9_model = {
#include "shared/jungle_prop/prop_9_model.inc"
};

static NJS_CNK_OBJECT prop_9_object = {
#include "shared/jungle_prop/prop_9_object.inc"
};

static NJS_TEXNAME prop_16_names[1] = {
#include "shared/jungle_prop/prop_16_names.inc"
};

static NJS_TEXLIST prop_16_textures = {
#include "shared/jungle_prop/prop_16_textures.inc"
};

static Sint16 prop_16_polygons[31] = {
#include "shared/jungle_prop/prop_16_polygons.inc"
};

static Sint32 prop_16_vertices[27] = {
#include "shared/jungle_prop/prop_16_vertices.inc"
};

static NJS_CNK_MODEL prop_16_model = {
#include "shared/jungle_prop/prop_16_model.inc"
};

static NJS_CNK_OBJECT prop_16_object = {
#include "shared/jungle_prop/prop_16_object.inc"
};

static NJS_TEXNAME prop_10_names[4] = {
#include "shared/jungle_prop/prop_10_names.inc"
};

NJS_TEXLIST jungle_prop_texlist_10 = {
#include "shared/jungle_prop/jungle_prop_texlist_10.inc"
};

static Sint16 prop_10_polygons[441] = {
#include "shared/jungle_prop/prop_10_polygons.inc"
};

static Sint32 prop_10_vertices[343] = {
#include "shared/jungle_prop/prop_10_vertices.inc"
};

NJS_CNK_MODEL jungle_prop_model_10 = {
#include "shared/jungle_prop/jungle_prop_model_10.inc"
};

static NJS_CNK_OBJECT prop_10_object = {
#include "shared/jungle_prop/prop_10_object.inc"
};

static NJS_TEXNAME prop_11_names[3] = {
#include "shared/jungle_prop/prop_11_names.inc"
};

static NJS_TEXLIST prop_11_textures = {
#include "shared/jungle_prop/prop_11_textures.inc"
};

static Sint16 prop_11_polygons[575] = {
#include "shared/jungle_prop/prop_11_polygons.inc"
};

static Sint32 prop_11_vertices[499] = {
#include "shared/jungle_prop/prop_11_vertices.inc"
};

static NJS_CNK_MODEL prop_11_model = {
#include "shared/jungle_prop/prop_11_model.inc"
};

static NJS_CNK_OBJECT prop_11_object = {
#include "shared/jungle_prop/prop_11_object.inc"
};

static NJS_TEXNAME prop_12_names[3] = {
#include "shared/jungle_prop/prop_12_names.inc"
};

static NJS_TEXLIST prop_12_textures = {
#include "shared/jungle_prop/prop_12_textures.inc"
};

static Sint16 prop_12_polygons[417] = {
#include "shared/jungle_prop/prop_12_polygons.inc"
};

static Sint32 prop_12_vertices[359] = {
#include "shared/jungle_prop/prop_12_vertices.inc"
};

static NJS_CNK_MODEL prop_12_model = {
#include "shared/jungle_prop/prop_12_model.inc"
};

static NJS_CNK_OBJECT prop_12_object = {
#include "shared/jungle_prop/prop_12_object.inc"
};

static NJS_TEXNAME prop_13_names[4] = {
#include "shared/jungle_prop/prop_13_names.inc"
};

static NJS_TEXLIST prop_13_textures = {
#include "shared/jungle_prop/prop_13_textures.inc"
};

static Sint16 prop_13_polygons[573] = {
#include "shared/jungle_prop/prop_13_polygons.inc"
};

static Sint32 prop_13_vertices[499] = {
#include "shared/jungle_prop/prop_13_vertices.inc"
};

static NJS_CNK_MODEL prop_13_model = {
#include "shared/jungle_prop/prop_13_model.inc"
};

static NJS_CNK_OBJECT prop_13_object = {
#include "shared/jungle_prop/prop_13_object.inc"
};

static NJS_TEXNAME prop_14_names[2] = {
#include "shared/jungle_prop/prop_14_names.inc"
};

static NJS_TEXLIST prop_14_textures = {
#include "shared/jungle_prop/prop_14_textures.inc"
};

static Sint16 prop_14_polygons[347] = {
#include "shared/jungle_prop/prop_14_polygons.inc"
};

static Sint32 prop_14_vertices[299] = {
#include "shared/jungle_prop/prop_14_vertices.inc"
};

static NJS_CNK_MODEL prop_14_model = {
#include "shared/jungle_prop/prop_14_model.inc"
};

static NJS_CNK_OBJECT prop_14_object = {
#include "shared/jungle_prop/prop_14_object.inc"
};

static NJS_TEXNAME prop_15_names[2] = {
#include "shared/jungle_prop/prop_15_names.inc"
};

static NJS_TEXLIST prop_15_textures = {
#include "shared/jungle_prop/prop_15_textures.inc"
};

static Sint16 prop_15_polygons[339] = {
#include "shared/jungle_prop/prop_15_polygons.inc"
};

static Sint32 prop_15_vertices[315] = {
#include "shared/jungle_prop/prop_15_vertices.inc"
};

static NJS_CNK_MODEL prop_15_model = {
#include "shared/jungle_prop/prop_15_model.inc"
};

static NJS_CNK_OBJECT prop_15_object = {
#include "shared/jungle_prop/prop_15_object.inc"
};

static NJS_TEXNAME prop_17_names[1] = {
#include "shared/jungle_prop/prop_17_names.inc"
};

static NJS_TEXLIST prop_17_textures = {
#include "shared/jungle_prop/prop_17_textures.inc"
};

static Sint16 prop_17_polygons[1175] = {
#include "shared/jungle_prop/prop_17_polygons.inc"
};

static Sint32 prop_17_vertices[795] = {
#include "shared/jungle_prop/prop_17_vertices.inc"
};

static NJS_CNK_MODEL prop_17_model = {
#include "shared/jungle_prop/prop_17_model.inc"
};

static NJS_CNK_OBJECT prop_17_object = {
#include "shared/jungle_prop/prop_17_object.inc"
};

/* the props' models, of which the work's scl.x picks one */
#define PROPS 18
/* the range JP_FAR's prop leaves at, in place of its placement's, squared as
 * placement_try_leave_by takes it */
#define FAR_RANGE (1450.0F * 1450.0F)
/* the prop's model and texture list, in two fields of task that task_destroy
 * frees. jungle_prop_destroy empties both first */
#define PROP_MODEL(task) ((task)->motion.p)
#define PROP_TEXLIST(task) ((task)->force.p)

static void jungle_prop_destroy(struct task *task);
static void jungle_prop_step(struct task *task);
static void jungle_prop_draw(struct task *task);
static void jungle_prop_far_step(struct task *task);

NJS_TEXLIST *jungle_prop_texlists[PROPS + 1] = {
#include "shared/jungle_prop/jungle_prop_texlists.inc"
};
static NJS_CNK_MODEL *jungle_prop_models[PROPS] = {
#include "shared/jungle_prop/jungle_prop_models.inc"
};

void jungle_prop_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)PROP_EDITOR_ANGLE_X;
        (void)PROP_EDITOR_ANGLE_Y;
        (void)PROP_EDITOR_ANGLE_Z;
        (void)PROP_EDITOR_SHAPE_RANGE;
        (void)PROP_EDITOR_SHAPE;
    }
    if (!placement_try_leave(task)) {
        task->draw = jungle_prop_draw;
        task->step = jungle_prop_step;
        task->destructor = jungle_prop_destroy;
        PROP_MODEL(task) = jungle_prop_models[(Uint32)work->scl.x % PROPS];
        PROP_TEXLIST(task) = jungle_prop_texlists[(Uint32)work->scl.x % PROPS];
    }
}

static void jungle_prop_destroy(struct task *task)
{
    PROP_MODEL(task) = NULL;
    PROP_TEXLIST(task) = NULL;
}

static void jungle_prop_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which tests task once and
     * work twice */
    if (0) {
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        if (task && work && work) {
        }
    }
}

static void jungle_prop_draw(struct task *task)
{
    struct task_work *work = task->work;

#ifdef NONMATCHING
    /* The first jungle duel has no texture for this single-material prop. */
    if (PROP_TEXLIST(task) == &prop_17_textures &&
        !texture_slot_loaded(&prop_17_textures, 0)) {
        return;
    }
#endif
    njSetTexture(PROP_TEXLIST(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(PROP_MODEL(task));
    njPopMatrixEx();
}

void jungle_prop_far_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave_by(task, FAR_RANGE)) {
        task->draw = jungle_prop_draw;
        task->step = jungle_prop_far_step;
        task->destructor = jungle_prop_destroy;
        PROP_MODEL(task) = jungle_prop_models[(Uint32)work->scl.x % PROPS];
        PROP_TEXLIST(task) = jungle_prop_texlists[(Uint32)work->scl.x % PROPS];
    }
}

static void jungle_prop_far_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave_by(task, FAR_RANGE)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which tests task once and
     * work twice */
    if (0) {
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        if (task && work && work) {
        }
    }
}
