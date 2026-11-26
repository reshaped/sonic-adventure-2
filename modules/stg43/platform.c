/* the platform, STAGE in Cosmic Wall's object list: a solid floor with a
 * pulsing light. ang.x may add a fence */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg43/platform/text.inc"

#include "effect/particle.h"
#include "shared/common_object.h"
#include "shared/frstage.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* platform_collision.h's placement with one fence, which stands where ang.x
 * has FENCE_ENABLED set */
#define FENCES 1
#define FENCE_ENABLED 1
/* the platform collides only while a player lies within NEAR_RANGE of pos */
#define NEAR_RANGE 80.0F
/* the angle the light's pulse advances a step. a pulse takes 0x10000 /
 * PULSE_RATE steps, about 85 */
#define PULSE_RATE 0x300

static Sint16 frstage_light_polygons[117] = {
#include "stg43/platform/frstage_light_polygons.inc"
};

static Sint32 frstage_light_vertices[131] = {
#include "stg43/platform/frstage_light_vertices.inc"
};

static NJS_CNK_MODEL frstage_light_model = {
#include "stg43/platform/frstage_light_model.inc"
};

NJS_CNK_OBJECT frstage_light[1] = {
#include "stg43/platform/frstage_light.inc"
};

static Sint16 frstage_fence_0_polygons[395] = {
#include "stg43/platform/frstage_fence_0_polygons.inc"
};

static Sint32 frstage_fence_0_vertices[355] = {
#include "stg43/platform/frstage_fence_0_vertices.inc"
};

static NJS_CNK_MODEL frstage_fence_0_model = {
#include "stg43/platform/frstage_fence_0_model.inc"
};

NJS_CNK_OBJECT frstage_fence_0[1] = {
#include "stg43/platform/frstage_fence_0.inc"
};

static Sint16 frstage_floor_polygons[395] = {
#include "stg43/platform/frstage_floor_polygons.inc"
};

static Sint32 frstage_floor_vertices[211] = {
#include "stg43/platform/frstage_floor_vertices.inc"
};

static NJS_CNK_MODEL frstage_floor_model = {
#include "stg43/platform/frstage_floor_model.inc"
};

NJS_CNK_OBJECT frstage_floor[1] = {
#include "stg43/platform/frstage_floor.inc"
};

static NJS_MATERIAL frstage_fence_collision_0_material[1] = {
#include "stg43/platform/frstage_fence_collision_0_material.inc"
};

static Sint16 frstage_fence_collision_0_faces_0[35] = {
#include "stg43/platform/frstage_fence_collision_0_faces_0.inc"
};

static NJS_MESHSET frstage_fence_collision_0_meshes[1] = {
#include "stg43/platform/frstage_fence_collision_0_meshes.inc"
};

static NJS_POINT3 frstage_fence_collision_0_points[16] = {
#include "stg43/platform/frstage_fence_collision_0_points.inc"
};

static NJS_VECTOR frstage_fence_collision_0_normals[16] = {
#include "stg43/platform/frstage_fence_collision_0_normals.inc"
};

static NJS_MODEL frstage_fence_collision_0_model = {
#include "stg43/platform/frstage_fence_collision_0_model.inc"
};

NJS_OBJECT frstage_fence_collision_0[1] = {
#include "stg43/platform/frstage_fence_collision_0.inc"
};

static NJS_MATERIAL frstage_collision_material[1] = {
#include "stg43/platform/frstage_collision_material.inc"
};

static Sint16 frstage_collision_faces_0[21] = {
#include "stg43/platform/frstage_collision_faces_0.inc"
};

static NJS_MESHSET frstage_collision_meshes[1] = {
#include "stg43/platform/frstage_collision_meshes.inc"
};

static NJS_POINT3 frstage_collision_points[8] = {
#include "stg43/platform/frstage_collision_points.inc"
};

static NJS_VECTOR frstage_collision_normals[8] = {
#include "stg43/platform/frstage_collision_normals.inc"
};

static NJS_MODEL frstage_collision_model = {
#include "stg43/platform/frstage_collision_model.inc"
};

static NJS_OBJECT frstage_collision = {
#include "stg43/platform/frstage_collision.inc"
};

NJS_TEXLIST *frstage_texlists[2] = {
#include "stg43/platform/frstage_texlists.inc"
};
static Uint32 bright_color = {
#include "stg43/platform/bright_color.inc"
};
static Uint32 dim_color = {
#include "stg43/platform/dim_color.inc"
};

#include "shared/platform_collision.h"

static void frstage_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)PLATFORM_EDITOR_SAKU_ON_OFF;
        (void)PLATFORM_EDITOR_ANGLE_Y;
    }
    njSetTexture(&frstage_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(frstage_floor->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    model_draw_tinted(
        frstage_light->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    if (work->ang.x & FENCE_ENABLED) {
        njRotateY(NULL, frstage_fence_0->ang[1]);
        njCnkEasyDrawModel(frstage_fence_0->model);
    }
    njPopMatrix(2);
}
