/* the road furniture of Radical Highway's object list: its props, sign poles
 * and arrows */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg14/road_props.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"

/* editor_angle_x to editor_model are the set editor's labels. they read
 * "<- ANGLE X", "<- ANGLE Y", "<- ANGLE Z" and "<- MODEL(0-  )" */
static const char editor_angle_x[12] = {
#include "stg14/road_props/editor_angle_x.inc"
};
static const char editor_angle_y[12] = {
#include "stg14/road_props/editor_angle_y.inc"
};
static const char editor_angle_z[12] = {
#include "stg14/road_props/editor_angle_z.inc"
};
static const char editor_model[16] = {
#include "stg14/road_props/editor_model.inc"
};

#include "stg14/road_props/text.inc"

/* clang-format off */
#include "shared/road_furniture/chevron_curve_2.h"
#include "shared/road_furniture/chevron_curve_1.h"
#include "shared/road_furniture/chevron_curve_4.h"
#include "shared/road_furniture/chevron_curve_3.h"
#include "shared/road_furniture/diamond_sign.h"
#include "stg14/road/prop_5.h"
#include "stg14/road/prop_6.h"
#include "stg14/road/prop_7.h"
#include "stg14/road/prop_9.h"
#include "shared/road_furniture/single_sign_gantry.h"
#include "shared/road_furniture/double_sign_gantry.h"
#include "stg14/road/pole_2.h"
#include "shared/road_furniture/vertical_arrow_panel.h"
#include "shared/road_furniture/horizontal_arrow_panel.h"
/* clang-format on */

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL arrow_material = {
#include "stg14/road_props/arrow_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Uint16 arrow_strips[21] = {
#include "stg14/road_props/arrow_strips.inc"
};
static NJS_MESHSET arrow_mesh = {
#include "stg14/road_props/arrow_mesh.inc"
};
static NJS_POINT3 arrow_points[8] = {
#include "stg14/road_props/arrow_points.inc"
};
static NJS_VECTOR arrow_normals[8] = {
#include "stg14/road_props/arrow_normals.inc"
};
static NJS_MODEL arrow_solid_model = {
#include "stg14/road_props/arrow_solid_model.inc"
};
/* a box 60 wide, 34.3 high and 10 deep. arrow 0's solid child puts a copy of
 * it in the collision world */
static NJS_OBJECT arrow_solid = {
#include "stg14/road_props/arrow_solid.inc"
};

#define PROP_MODELS 10
#define POLE_MODELS 3
#define ARROW_MODELS 2
NJS_TEXLIST *highway_prop_texlists[PROP_MODELS + 1] = {
#include "stg14/road_props/highway_prop_texlists.inc"
};
NJS_TEXLIST *highway_pole_texlists[POLE_MODELS + 1] = {
#include "stg14/road_props/highway_pole_texlists.inc"
};
NJS_TEXLIST *highway_arrow_texlists[ARROW_MODELS + 1] = {
#include "stg14/road_props/highway_arrow_texlists.inc"
};
static NJS_CNK_OBJECT *prop_models[PROP_MODELS] = {
#include "stg14/road_props/prop_models.inc"
};
static NJS_CNK_OBJECT *pole_models[POLE_MODELS] = {
#include "stg14/road_props/pole_models.inc"
};
static NJS_CNK_OBJECT *arrow_models[ARROW_MODELS] = {
#include "stg14/road_props/arrow_models.inc"
};

static struct collision_shape tilted_box[1] = {
#include "stg14/road_props/tilted_box.inc"
};
static struct collision_shape left_curve[7] = {
#include "stg14/road_props/left_curve.inc"
};
static struct collision_shape right_curve[7] = {
#include "stg14/road_props/right_curve.inc"
};
static struct collision_shape barrier[4] = {
#include "stg14/road_props/barrier.inc"
};
static struct collision_shape post[1] = {
#include "stg14/road_props/post.inc"
};
static struct collision_shape double_post[2] = {
#include "stg14/road_props/double_post.inc"
};
static struct collision_shape horizontal_post[1] = {
#include "stg14/road_props/horizontal_post.inc"
};
static struct collision_shape *prop_shapes[PROP_MODELS] = {
#include "stg14/road_props/prop_shapes.inc"
};
static struct collision_shape single_sign[4] = {
#include "stg14/road_props/single_sign.inc"
};
static struct collision_shape wide_sign[5] = {
#include "stg14/road_props/wide_sign.inc"
};
static struct collision_shape double_sign[5] = {
#include "stg14/road_props/double_sign.inc"
};
static struct collision_shape *pole_shapes[POLE_MODELS] = {
#include "stg14/road_props/pole_shapes.inc"
};
static struct collision_shape arrow_box[1] = {
#include "stg14/road_props/arrow_box.inc"
};
static struct collision_shape *arrow_shapes[ARROW_MODELS] = {
#include "stg14/road_props/arrow_shapes.inc"
};
static int prop_shape_counts[PROP_MODELS] = {
#include "stg14/road_props/prop_shape_counts.inc"
};
static int pole_shape_counts[POLE_MODELS] = {
#include "stg14/road_props/pole_shape_counts.inc"
};
static int arrow_shape_counts[ARROW_MODELS] = {
#include "stg14/road_props/arrow_shape_counts.inc"
};

/* a kind's texture lists, models, shapes and counts of shapes, each indexed by
 * model number */
struct furniture_kind {
    NJS_TEXLIST **texlists;
    NJS_CNK_OBJECT **models;
    struct collision_shape **shapes;
    int *shape_counts;
};
/* the kinds, as prop_kinds lists them */
enum { ROAD_PROP, SIGN_POLE, DIRECTION_ARROW, KINDS };
KEPT static struct furniture_kind prop_kinds[KINDS] = {
#include "stg14/road_props/prop_kinds.inc"
};
static int prop_frame_wait = {
#include "stg14/road_props/prop_frame_wait.inc"
};
/* scroll_step is how far pole 2's panels scroll at each tick of the timer. its
 * unit is a 256th of their texture's height */
static Float scroll_step = {
#include "stg14/road_props/scroll_step.inc"
};

#define MODEL(task) ((task)->motion.p)
#define TEXLIST(task) ((task)->extra.p)
#define KIND_INDEX(work) ((work)->tick)
#define MODEL_COUNT(work) ((work)->submode)
#define KIND(work) (prop_kinds[KIND_INDEX(work)])
#define MODEL_NUMBER(work) ((Uint8)(work)->ang.x % MODEL_COUNT(work))
/* the work's mode: whether arrow_draw turns the arrow about x by ang.x */
#define NO_X_TURN 0
#define X_TURN (-1)
/* the model number of the arrow with a solid child */
#define SOLID_ARROW 0

static void furniture_destroy(struct task *task);
/* while game_state.time_stopped is 0, the work's timer ticks once every
 * prop_frame_wait + 1 steps. it wraps at 256. STEPS_WAITED counts the steps
 * since its last tick */
#define STEPS_WAITED(task) ((task)->unk_3c.l)
#define TICK_TIMER(task, work)                                                 \
    do {                                                                       \
        if (!game_state.time_stopped) {                                        \
            STEPS_WAITED(task)++;                                              \
            if (STEPS_WAITED(task) > prop_frame_wait) {                        \
                STEPS_WAITED(task) = 0;                                        \
                (work)->timer = (Uint8)((work)->timer + 1);                    \
            }                                                                  \
        }                                                                      \
    } while (0)

static void solid_arrow_step(struct task *task);
static void furniture_step(struct task *task);
static void prop_draw(struct task *task);
static void pole_draw(struct task *task);
static void arrow_draw(struct task *task);

#define ASSIGN_MODEL_AND_TEXLIST(task, work, number)                           \
    do {                                                                       \
        MODEL(task) = KIND(work).models[number];                               \
        TEXLIST(task) = KIND(work).texlists[number];                           \
    } while (0)
/* @hack spelling: a bare if as the body, where a do-while (0) block would do */
#define INIT_COLLISION(task, work)                                             \
    if (KIND(work).shapes[MODEL_NUMBER(work)] != NULL) {                       \
        collision_init(task, KIND(work).shapes[MODEL_NUMBER(work)],            \
                       KIND(work).shape_counts[MODEL_NUMBER(work)],            \
                       COLLISION_OBJECT);                                      \
    }

void highway_prop_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    MODEL_COUNT(work) = PROP_MODELS;
    KIND_INDEX(work) = ROAD_PROP;
    work->mode = NO_X_TURN;
    ASSIGN_MODEL_AND_TEXLIST(task, work, MODEL_NUMBER(work));
    task->draw = prop_draw;
    task->step = furniture_step;
    task->destructor = furniture_destroy;
    INIT_COLLISION(task, work);
}

void highway_pole_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = pole_draw;
    task->step = furniture_step;
    task->destructor = furniture_destroy;
    /* @hack redundant: a store of the mode, which the store below repeats */
    work->mode = NO_X_TURN;
    MODEL_COUNT(work) = POLE_MODELS;
    KIND_INDEX(work) = SIGN_POLE;
    work->mode = NO_X_TURN;
    ASSIGN_MODEL_AND_TEXLIST(task, work, MODEL_NUMBER(work));
    INIT_COLLISION(task, work);
}

void highway_arrow_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = arrow_draw;
    task->step = furniture_step;
    task->destructor = furniture_destroy;
    /* @hack redundant: a store of the mode, which the store below repeats */
    work->mode = NO_X_TURN;
    MODEL_COUNT(work) = ARROW_MODELS;
    KIND_INDEX(work) = DIRECTION_ARROW;
    work->mode = NO_X_TURN;
    ASSIGN_MODEL_AND_TEXLIST(task, work, MODEL_NUMBER(work));
    INIT_COLLISION(task, work);
    if (MODEL_NUMBER(work) == SOLID_ARROW) {
        /* the child takes the arrow's angles, so ang.x, which only picked the
         * model, would turn it about x */
        work->ang.x = 0;
        solid_object_create(task, NULL, NULL, NULL, 0, &arrow_solid,
                            OBJECT_TURNED);
    }
}

void highway_arrow_h3_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = arrow_draw;
    task->step = solid_arrow_step;
    task->destructor = furniture_destroy;
    MODEL_COUNT(work) = ARROW_MODELS;
    KIND_INDEX(work) = DIRECTION_ARROW;
    work->mode = X_TURN;
    ASSIGN_MODEL_AND_TEXLIST(task, work, SOLID_ARROW);
    solid_object_create(task, NULL, NULL, NULL, 0, &arrow_solid, OBJECT_TURNED);
}

/* null the model and the texture list, which aren't the task's, so that
 * task_destroy doesn't free them */
static void furniture_destroy(struct task *task)
{
    TEXLIST(task) = NULL;
    MODEL(task) = NULL;
}

static void solid_arrow_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which calls
     * matrix_load_parent three times */
    if (0) {
        matrix_load_parent();
        matrix_load_parent();
        matrix_load_parent();
    }
    TICK_TIMER(task, work);
}

static void furniture_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which makes three calls and
     * two stores */
    if (0) {
        matrix_load_parent();
        matrix_load_parent();
        matrix_load_parent();
        MODEL_COUNT(work) = KIND_INDEX(work);
        work->mode = NO_X_TURN;
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
    TICK_TIMER(task, work);
}

#define VIEW_MARGIN 10.0F
/* the blink, in steps: blink_level's period, rest and fade. the model's child
 * draws with the blink's level as its alpha. the child's sibling takes 1 minus
 * that level */
#define BLINK_PERIOD 50
#define BLINK_REST 25
#define BLINK_FADE 5
/* the tint takes the alpha, 0 to FULL_ALPHA, in its top byte over WHITE */
#define FULL_ALPHA 255
#define WHITE 0xffffff

static void prop_draw(struct task *task)
{
    NJS_CNK_OBJECT *object = MODEL(task);
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *child;
    NJS_CNK_OBJECT *sibling;
    Float level;
    int alpha;
    Uint32 color;
    Uint32 fade;

    if (object == NULL) {
        return;
    }
    if (!sphere_in_front_no_depth(&work->pos, object->model->r + VIEW_MARGIN)) {
        return;
    }
    njSetTexture(TEXLIST(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (object == &highway_prop_7) {
        njRotateX(NULL, work->ang.z);
    }
    njCnkEasyDrawModel(object->model);
    child = object->child;
    if (child != NULL) {
        sibling = child->sibling;
        /* @hack spelling: level, a local for blink_level's result, where the
         * result scaled in place would do */
        level = blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE);
        alpha = level * FULL_ALPHA;
        if (alpha > FULL_ALPHA) {
            alpha = FULL_ALPHA;
        }
        if (alpha < 0) {
            alpha = 0;
        }
        color = (alpha << ALPHA_SHIFT) + WHITE;
        fade = ((FULL_ALPHA - alpha) << ALPHA_SHIFT) + WHITE;
        njPushMatrixEx();
        njTranslateEx((NJS_VECTOR *)child->pos);
        model_draw_tinted(child->model, color);
        njPopMatrixEx();
        if (sibling != NULL) {
            njTranslateEx((NJS_VECTOR *)sibling->pos);
            model_draw_tinted(sibling->model, fade);
        }
        /* @hack redundant: a block no path reaches, which reads sibling */
        if (0) {
            if (sibling) {
            }
        }
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njPopMatrixEx();
}

/* pole 2, the one pole whose model has a child, draws two panels. each shows
 * the whole height of texture PANEL_TEXTURE of the pole's texture list. the
 * second panel's texture is shifted half that height from the first's */
#define PANEL_BOTTOM 64.40492F
#define PANEL_TOP 87.05363F
#define PANEL_LEFT_Z 1.700001F
#define PANEL_RIGHT_Z 1.700018F
#define FIRST_PANEL_LEFT 14.641949F
#define FIRST_PANEL_RIGHT 46.753178F
#define SECOND_PANEL_LEFT (-36.585518F)
#define SECOND_PANEL_RIGHT (-4.47429F)
#define SECOND_PANEL_OFFSET 0.5F
#define PANEL_TEXTURE 3
#define PANEL_COLOR 0xffffffff
#define SCROLL_UNIT (1.0F / 256.0F)
#define QUAD_VERTICES 4
#define PANEL_VERTEX(vertex, px, py, pz, pu, pv)                               \
    do {                                                                       \
        (vertex)->x = (px);                                                    \
        (vertex)->y = (py);                                                    \
        (vertex)->z = (pz);                                                    \
        (vertex)->col = PANEL_COLOR;                                           \
        (vertex)->u = (pu);                                                    \
        (vertex)->v = (pv);                                                    \
    } while (0)
#define DRAW_PANEL(vertices, left, right, offset)                              \
    do {                                                                       \
        vertex = vertices;                                                     \
        PANEL_VERTEX(vertex, left, PANEL_BOTTOM, PANEL_LEFT_Z, 0.0F,           \
                     (offset) + 1.0F);                                         \
        vertex++;                                                              \
        PANEL_VERTEX(vertex, left, PANEL_TOP, PANEL_LEFT_Z, 0.0F,              \
                     (offset) + 0.0F);                                         \
        vertex++;                                                              \
        PANEL_VERTEX(vertex, right, PANEL_BOTTOM, PANEL_RIGHT_Z, 1.0F,         \
                     (offset) + 1.0F);                                         \
        vertex++;                                                              \
        PANEL_VERTEX(vertex, right, PANEL_TOP, PANEL_RIGHT_Z, 1.0F,            \
                     (offset) + 0.0F);                                         \
        njDrawTexture3DEx(vertices, QUAD_VERTICES, TRUE);                      \
    } while (0)

static void pole_draw(struct task *task)
{
    NJS_CNK_OBJECT *object = MODEL(task);
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    NJS_TEXTURE_VTX vertices[QUAD_VERTICES];
    NJS_TEXTURE_VTX *vertex;
    Float scroll;

    if (!sphere_in_front_no_depth(&work->pos, object->model->r + VIEW_MARGIN)) {
        return;
    }
    njSetTexture(TEXLIST(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    if (object->child != NULL) {
        /* @hack spelling: reloaded, the work loaded again from task, where
         * work would do */
        struct task_work *reloaded = task->work;
        scroll = reloaded->timer * scroll_step * SCROLL_UNIT;
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTextureNum(PANEL_TEXTURE);
        njTextureClampMode(NJD_TEXTURECLAMP_NOCLAMP);
        njTextureFlipMode(NJD_TEXTUREFLIP_NOFLIP);
        DRAW_PANEL(vertices, FIRST_PANEL_LEFT, FIRST_PANEL_RIGHT, scroll);
        scroll += SECOND_PANEL_OFFSET;
        DRAW_PANEL(vertices, SECOND_PANEL_LEFT, SECOND_PANEL_RIGHT, scroll);
        njTextureClampMode(NJD_TEXTURECLAMP_CLAMP_UV);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
    njPopMatrixEx();
}

#define MATRICES_PUSHED 2
/* an arrow's lights are its model's child and the child's next two siblings.
 * each is on for LIGHT_STEPS steps in turn: the second sibling, then the first,
 * then the child. over the first LIGHT_FADE_STEPS steps a light is on, its
 * level rises from 0 to 1. over the same steps, the light that was on falls
 * from 1 to 0 */
#define LIGHTS 3
#define LIGHT_STEPS 30
#define LIGHT_FADE_STEPS 8
/* a light at LIGHT_MIN_LEVEL or below isn't drawn */
#define LIGHT_MIN_LEVEL 0.001F
/* a light's color blends from LIGHT_DARK toward LIGHT_LIT by its level */
#define LIGHT_DARK 0xff000000
#define LIGHT_LIT 0xffffffff

static void arrow_draw(struct task *task)
{
    NJS_CNK_OBJECT *object = MODEL(task);
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *light;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    int step;
    int lit;
    int i;
    Float level;

    if (!sphere_in_front_no_depth(&work->pos, object->model->r + VIEW_MARGIN)) {
        return;
    }
    njSetTexture(TEXLIST(task));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    if (work->mode == X_TURN) {
        njRotateX(NULL, work->ang.x);
    }
    njCnkEasyDrawModel(object->model);
    njPushMatrixEx();
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((light = object->child) != NULL) {
        step = game_state.steps % (LIGHTS * LIGHT_STEPS);
        lit = LIGHTS - 1 - (step / LIGHT_STEPS);
        /* @hack spelling: the remainder assigned in full, where %= would do */
        step = step % LIGHT_STEPS;
        for (i = 0; i < LIGHTS; i++, light = light->sibling) {
            if (i == lit) {
                if (step < LIGHT_FADE_STEPS) {
                    level = step * (1.0F / LIGHT_FADE_STEPS);
                } else {
                    level = 1.0F;
                }
            } else if (i == (lit + 1) % LIGHTS) {
                if (step < LIGHT_FADE_STEPS) {
                    level =
                        (LIGHT_FADE_STEPS - step) * (1.0F / LIGHT_FADE_STEPS);
                } else {
                    level = 0.0F;
                }
            } else {
                level = 0.0F;
            }
            if (level > LIGHT_MIN_LEVEL) {
                njTranslateEx((NJS_VECTOR *)light->pos);
                model_draw_tinted(light->model,
                                  color_blend(LIGHT_DARK, LIGHT_LIT, level));
                matrix_load_parent();
            }
        }
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njPopMatrix(MATRICES_PUSHED);
}
