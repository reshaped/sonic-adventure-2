/* the foothold that bobs up and down in the golem fights, UGOKIDAI in their
 * object lists. it carries the players on it */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/golem_util.h"
#include "shared/object_builds.h"
#include "shared/ugokidai.h"
#include "shared/ugokidai/slots.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/ugokidai/text.inc"

/* kinds 3 to 5 take the collision objects of kinds 0 to 2, under models of
 * their own. SHADOW indexes the shadow's model, texture list and build, after
 * the kinds' */
#define UGOKIDAI_KINDS 6
#define SHADOW UGOKIDAI_KINDS
#define UGOKIDAI_BUILDS (UGOKIDAI_KINDS + 1)
#define KIND(work) ((Uint8)(work)->ang.z % UGOKIDAI_KINDS)
/* a byte of 256ths of a turn moves HIGH_SHIFT bits up to be an Angle */
#define HIGH_SHIFT 8
#define PHASE(work) ((Uint8)(work)->ang.y << HIGH_SHIFT)
#define HIGH_BYTE 0xff00
/* the foothold's offset in y from the placed pos at game_state.steps. it is 0
 * at the top of the bob, and -2 * scl.y at its bottom */
#define RISE(work)                                                             \
    ((njCos((game_state.steps * (work)->ang.x) + PHASE(work)) - 1.0F) *        \
     (work)->scl.y)
/* the scale of the models' width and depth, which the work's scl.x holds
 * less 1 */
#define WIDTH_SCALE(work) ((work)->scl.x + 1.0F)
/* the placed pos, which the foothold bobs below */
#define ORIGIN(task) ((NJS_POINT3 *)(task)->extra.p)
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* the models and the collision object lie DROP below the work's pos */
#define DROP 10.0F
/* the collision object's scale in x and z is OBJECT_SPREAD times the
 * models' */
#define OBJECT_SPREAD 1.1F
/* the evalflags that would keep an object's pos, ang and scl from applying */
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
#define UGOKIDAI_WORLD_FLAGS (WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID)
/* world_collect_near_players takes the copy only while a player lies within
 * the kind's ugokidai_radii entry plus NEAR_MARGIN of it */
#define NEAR_MARGIN 25.0F

/* Three meter. */
static NJS_TEXNAME three_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/three_meter_names.inc"
};

static NJS_TEXLIST three_meter_texlist = {
#include "shared/ugokidai/three_meter_texlist.inc"
};

static Sint16 three_meter_polygons[141] = {
#include "shared/ugokidai/three_meter_polygons.inc"
};

static Sint32 three_meter_vertices[111] = {
#include "shared/ugokidai/three_meter_vertices.inc"
};

static NJS_CNK_MODEL three_meter_model = {
#include "shared/ugokidai/three_meter_model.inc"
};

static NJS_CNK_OBJECT three_meter_object = {
#include "shared/ugokidai/three_meter_object.inc"
};

/* Five meter. */
static NJS_TEXNAME five_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/five_meter_names.inc"
};

static NJS_TEXLIST five_meter_texlist = {
#include "shared/ugokidai/five_meter_texlist.inc"
};

static Sint16 five_meter_polygons[141] = {
#include "shared/ugokidai/five_meter_polygons.inc"
};

static Sint32 five_meter_vertices[111] = {
#include "shared/ugokidai/five_meter_vertices.inc"
};

static NJS_CNK_MODEL five_meter_model = {
#include "shared/ugokidai/five_meter_model.inc"
};

static NJS_CNK_OBJECT five_meter_object = {
#include "shared/ugokidai/five_meter_object.inc"
};

/* Seven meter. */
static NJS_TEXNAME seven_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/seven_meter_names.inc"
};

static NJS_TEXLIST seven_meter_texlist = {
#include "shared/ugokidai/seven_meter_texlist.inc"
};

static Sint16 seven_meter_polygons[141] = {
#include "shared/ugokidai/seven_meter_polygons.inc"
};

static Sint32 seven_meter_vertices[111] = {
#include "shared/ugokidai/seven_meter_vertices.inc"
};

static NJS_CNK_MODEL seven_meter_model = {
#include "shared/ugokidai/seven_meter_model.inc"
};

static NJS_CNK_OBJECT seven_meter_object = {
#include "shared/ugokidai/seven_meter_object.inc"
};

/* Dark three meter. */
static NJS_TEXNAME dark_three_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/dark_three_meter_names.inc"
};

static NJS_TEXLIST dark_three_meter_texlist = {
#include "shared/ugokidai/dark_three_meter_texlist.inc"
};

static Sint16 dark_three_meter_polygons[141] = {
#include "shared/ugokidai/dark_three_meter_polygons.inc"
};

static Sint32 dark_three_meter_vertices[103] = {
#include "shared/ugokidai/dark_three_meter_vertices.inc"
};

static NJS_CNK_MODEL dark_three_meter_model = {
#include "shared/ugokidai/dark_three_meter_model.inc"
};

static NJS_CNK_OBJECT dark_three_meter_object = {
#include "shared/ugokidai/dark_three_meter_object.inc"
};

/* Dark five meter. */
static NJS_TEXNAME dark_five_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/dark_five_meter_names.inc"
};

static NJS_TEXLIST dark_five_meter_texlist = {
#include "shared/ugokidai/dark_five_meter_texlist.inc"
};

static Sint16 dark_five_meter_polygons[141] = {
#include "shared/ugokidai/dark_five_meter_polygons.inc"
};

static Sint32 dark_five_meter_vertices[103] = {
#include "shared/ugokidai/dark_five_meter_vertices.inc"
};

static NJS_CNK_MODEL dark_five_meter_model = {
#include "shared/ugokidai/dark_five_meter_model.inc"
};

static NJS_CNK_OBJECT dark_five_meter_object = {
#include "shared/ugokidai/dark_five_meter_object.inc"
};

/* Dark seven meter. */
static NJS_TEXNAME dark_seven_meter_names[PLATFORM_TEXTURES] = {
#include "shared/ugokidai/dark_seven_meter_names.inc"
};

static NJS_TEXLIST dark_seven_meter_texlist = {
#include "shared/ugokidai/dark_seven_meter_texlist.inc"
};

static Sint16 dark_seven_meter_polygons[141] = {
#include "shared/ugokidai/dark_seven_meter_polygons.inc"
};

static Sint32 dark_seven_meter_vertices[107] = {
#include "shared/ugokidai/dark_seven_meter_vertices.inc"
};

static NJS_CNK_MODEL dark_seven_meter_model = {
#include "shared/ugokidai/dark_seven_meter_model.inc"
};

static NJS_CNK_OBJECT dark_seven_meter_object = {
#include "shared/ugokidai/dark_seven_meter_object.inc"
};

/* Shadow. */
static NJS_TEXNAME shadow_names[SHADOW_TEXTURES] = {
#include "shared/ugokidai/shadow_names.inc"
};

static NJS_TEXLIST shadow_texlist = {
#include "shared/ugokidai/shadow_texlist.inc"
};

static Sint16 shadow_polygons[51] = {
#include "shared/ugokidai/shadow_polygons.inc"
};

static Sint32 shadow_vertices[39] = {
#include "shared/ugokidai/shadow_vertices.inc"
};

static NJS_CNK_MODEL platform_shadow_model = {
#include "shared/ugokidai/platform_shadow_model.inc"
};

static NJS_CNK_OBJECT platform_shadow_object = {
#include "shared/ugokidai/platform_shadow_object.inc"
};

/* Three meter collision. */
static NJS_MATERIAL three_meter_collision_materials[1] = {
#include "shared/ugokidai/three_meter_collision_materials.inc"
};

static Sint16 three_meter_collision_faces[16] = {
#include "shared/ugokidai/three_meter_collision_faces.inc"
};

static NJS_MESHSET three_meter_collision_meshes[1] = {
#include "shared/ugokidai/three_meter_collision_meshes.inc"
};

static NJS_POINT3 three_meter_collision_points[8] = {
#include "shared/ugokidai/three_meter_collision_points.inc"
};

static NJS_VECTOR three_meter_collision_normals[8] = {
#include "shared/ugokidai/three_meter_collision_normals.inc"
};

static NJS_MODEL three_meter_collision_model = {
#include "shared/ugokidai/three_meter_collision_model.inc"
};

static NJS_OBJECT three_meter_collision = {
#include "shared/ugokidai/three_meter_collision.inc"
};

/* Five meter collision. */
static NJS_MATERIAL five_meter_collision_materials[1] = {
#include "shared/ugokidai/five_meter_collision_materials.inc"
};

static Sint16 five_meter_collision_faces[16] = {
#include "shared/ugokidai/five_meter_collision_faces.inc"
};

static NJS_MESHSET five_meter_collision_meshes[1] = {
#include "shared/ugokidai/five_meter_collision_meshes.inc"
};

static NJS_POINT3 five_meter_collision_points[8] = {
#include "shared/ugokidai/five_meter_collision_points.inc"
};

static NJS_VECTOR five_meter_collision_normals[8] = {
#include "shared/ugokidai/five_meter_collision_normals.inc"
};

static NJS_MODEL five_meter_collision_model = {
#include "shared/ugokidai/five_meter_collision_model.inc"
};

static NJS_OBJECT five_meter_collision = {
#include "shared/ugokidai/five_meter_collision.inc"
};

/* Seven meter collision. */
static NJS_MATERIAL seven_meter_collision_materials[1] = {
#include "shared/ugokidai/seven_meter_collision_materials.inc"
};

static Sint16 seven_meter_collision_faces[16] = {
#include "shared/ugokidai/seven_meter_collision_faces.inc"
};

static NJS_MESHSET seven_meter_collision_meshes[1] = {
#include "shared/ugokidai/seven_meter_collision_meshes.inc"
};

static NJS_POINT3 seven_meter_collision_points[8] = {
#include "shared/ugokidai/seven_meter_collision_points.inc"
};

static NJS_VECTOR seven_meter_collision_normals[8] = {
#include "shared/ugokidai/seven_meter_collision_normals.inc"
};

static NJS_MODEL seven_meter_collision_model = {
#include "shared/ugokidai/seven_meter_collision_model.inc"
};

static NJS_OBJECT seven_meter_collision = {
#include "shared/ugokidai/seven_meter_collision.inc"
};

/* each kind's model, and the shadow's at SHADOW */
static NJS_CNK_MODEL *ugokidai_models[UGOKIDAI_KINDS + 1] = {
#include "shared/ugokidai/ugokidai_models.inc"
};
NJS_TEXLIST *ugokidai_texlists[UGOKIDAI_BUILDS + 1] = {
#include "shared/ugokidai/ugokidai_texlists.inc"
};
static NJS_OBJECT *ugokidai_collisions[UGOKIDAI_KINDS] = {
#include "shared/ugokidai/ugokidai_collisions.inc"
};
static char *ugokidai_names[UGOKIDAI_KINDS] = {
#include "shared/ugokidai/ugokidai_names.inc"
};
/* by kind, equal to the height of the kind's collision box */
static Float dat_ugokidai_0[UGOKIDAI_KINDS] = {
    30.0F, 50.0F, 70.0F, 30.0F, 50.0F, 70.0F,
};
static Float ugokidai_radii[UGOKIDAI_KINDS] = {
    71.0F, 71.0F, 87.0F, 71.0F, 71.0F, 87.0F,
};
/* TRUE once one call of ugokidai_build has built all seven models */
static Bool ugokidai_built = FALSE;
static struct model_build ugokidai_builds[UGOKIDAI_BUILDS] = {
#include "shared/ugokidai/ugokidai_builds.inc"
};

static void ugokidai_destroy(struct task *task);
static void ugokidai_step(struct task *task);
static void ugokidai_draw(struct task *task);

Uint8 *ugokidai_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, ugokidai_builds, UGOKIDAI_BUILDS) ==
        UGOKIDAI_BUILDS) {
        ugokidai_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void ugokidai_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float rise;

    ORIGIN(task)->x = work->pos.x;
    ORIGIN(task)->y = work->pos.y;
    ORIGIN(task)->z = work->pos.z;
    /* @hack spelling: task->work read again, where work would do */
    rise = RISE(task->work);
    work->pos.y += rise;
    if (!placement_try_leave(task)) {
        task->draw = ugokidai_draw;
        task->step = ugokidai_step;
        task->destructor = ugokidai_destroy;
        object = world_pool_alloc();
        /* @bug memory: writes the copy through null where world_pool_alloc
         * has no object to lend (MOV.L at 0xce of the object's code).
         * ugokidai_destroy tests task->motion for null (TST at 0x170).
         * ugokidai_step writes through it untested (FMOV.S at 0x2ac) */
#ifdef NONMATCHING
        /* this side repairs the tag above: without an object, the init tries
         * again on its next step */
        if (object == NULL) {
            work->pos.y = ORIGIN(task)->y;
            task->step = ugokidai_init;
            task->draw = NULL;
            task->destructor = NULL;
            return;
        }
#endif
        /* @hack spelling: task->work read again here and for the scale,
         * where work would do */
        *object = *ugokidai_collisions[KIND(task->work)];
        task->motion.p = object;
        object->evalflags &= ~EVAL_UNITS;
        /* @hack spelling: the x and z scales set in one chained assignment,
         * where two would do */
        object->scl[0] = object->scl[2] =
            WIDTH_SCALE(task->work) * OBJECT_SPREAD;
        object->scl[1] = 1.0F;
        object->ang[0] = 0;
        /* @bug logic: turns the collision object by the whole of ang.y, the
         * bob's phase byte too (MOV.L at 0x12e of the object's code). the
         * draw turns the models by ang.y's high byte alone (AND at 0x35c) */
        object->ang[1] = work->ang.y;
        object->ang[2] = 0;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y - DROP;
        object->pos[2] = work->pos.z;
        world_add(UGOKIDAI_WORLD_FLAGS, task, object);
    }
}

static void ugokidai_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    /* task_destroy would free the pool's copy with syFree */
    task->motion.p = NULL;
}

static void ugokidai_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    Float rise;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->motion.p;
    /* @hack spelling: task->work read again, where work would do */
    rise = RISE(task->work);
    pos.x = ORIGIN(task)->x;
    pos.y = ORIGIN(task)->y + rise;
    pos.z = ORIGIN(task)->z;
    /* @hack spelling: each move given to both records in one chained
     * assignment, where two would do */
    FORCES(task)[0].speed.x = FORCES(task)[1].speed.x = pos.x - work->pos.x;
    FORCES(task)[0].speed.y = FORCES(task)[1].speed.y = pos.y - work->pos.y;
    FORCES(task)[0].speed.z = FORCES(task)[1].speed.z = pos.z - work->pos.z;
    work->pos = pos;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y - DROP;
    object->pos[2] = work->pos.z;
    /* @hack spelling: task->work read again for the kind, where work would
     * do */
    if (player_near((NJS_POINT3 *)object->pos,
                    ugokidai_radii[KIND(task->work)] + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void ugokidai_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.z = WIDTH_SCALE(work);
    scale.x = scale.z;
    scale.y = 1.0F;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y - DROP, work->pos.z);
    /* @hack spelling: task->work read again here and for each kind below,
     * where work would do */
    njRotateY(NULL, task->work->ang.y & HIGH_BYTE);
    njPushMatrixEx();
    njSetTexture(ugokidai_texlists[KIND(task->work)]);
    if (ugokidai_built) {
        golem_direct_draw_scaled_v(ugokidai_builds[KIND(task->work)].built,
                                   &scale);
    } else {
        golem_model_draw_scaled_v(ugokidai_models[KIND(task->work)], &scale);
    }
    njPopMatrixEx();
    njTranslate(NULL, 0.0F, ORIGIN(task)->y - work->pos.y + DROP, 0.0F);
    njSetTexture(ugokidai_texlists[SHADOW]);
    if (ugokidai_built) {
        golem_direct_draw_scaled_v(ugokidai_builds[SHADOW].built, &scale);
    } else {
        golem_model_draw_scaled_v(ugokidai_models[SHADOW], &scale);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
