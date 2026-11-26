/* SNAKEPOT in Egg Quarters' object list: a pot with eight flames on its rim */
#define EGG_QUARTERS
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stg25/flame_prop.h"
#include "stg26/flame_props.h"
#include "stg26/prop_builds.h"
#include "stg26/quarters_util.h"
#include "stg26/snake_pot.h"
#include "system/task.h"
#include "system/util.h"

#include "stg26/snake_pot/text.inc"

static NJS_TEXNAME flame_texture[1] = {
#include "stg26/snake_pot/flame_texture.inc"
};

static NJS_TEXLIST snake_pot_flame_texlist = {
#include "stg26/snake_pot/snake_pot_flame_texlist.inc"
};

static Sint16 snake_pot_flame_polygons[25] = {
#include "stg26/snake_pot/snake_pot_flame_polygons.inc"
};

static Sint32 snake_pot_flame_vertices[19] = {
#include "stg26/snake_pot/snake_pot_flame_vertices.inc"
};

static NJS_CNK_MODEL snake_pot_flame_model[1] = {
#include "stg26/snake_pot/snake_pot_flame_model.inc"
};

static NJS_CNK_OBJECT snake_pot_flame_object = {
#include "stg26/snake_pot/snake_pot_flame_object.inc"
};

static NJS_TEXNAME pot_textures[5] = {
#include "stg26/snake_pot/pot_textures.inc"
};
NJS_TEXLIST snake_pot_texlist = {
#include "stg26/snake_pot/snake_pot_texlist.inc"
};

static Sint16 snake_pot_polygons[3533] = {
#include "stg26/snake_pot/snake_pot_polygons.inc"
};

static Sint32 snake_pot_vertices[2471] = {
#include "stg26/snake_pot/snake_pot_vertices.inc"
};

static NJS_CNK_MODEL snake_pot_model = {
#include "stg26/snake_pot/snake_pot_model.inc"
};

static NJS_CNK_OBJECT snake_pot_object = {
#include "stg26/snake_pot/snake_pot_object.inc"
};

static NJS_MATERIAL snake_pot_collision_materials[1] = {
#include "stg26/snake_pot/snake_pot_collision_materials.inc"
};

static Sint16 snake_pot_collision_faces[504] = {
#include "stg26/snake_pot/snake_pot_collision_faces.inc"
};

static NJS_MESHSET snake_pot_collision_meshsets[1] = {
#include "stg26/snake_pot/snake_pot_collision_meshsets.inc"
};

static NJS_POINT3 snake_pot_collision_points[152] = {
#include "stg26/snake_pot/snake_pot_collision_points.inc"
};

static NJS_VECTOR snake_pot_collision_normals[152] = {
#include "stg26/snake_pot/snake_pot_collision_normals.inc"
};

static NJS_MODEL snake_pot_collision_model = {
#include "stg26/snake_pot/snake_pot_collision_model.inc"
};

static NJS_OBJECT snake_pot_collision = {
#include "stg26/snake_pot/snake_pot_collision.inc"
};

static NJS_MATERIAL snake_pot_inner_collision_materials[1] = {
#include "stg26/snake_pot/snake_pot_inner_collision_materials.inc"
};

static Sint16 snake_pot_inner_collision_faces[9] = {
#include "stg26/snake_pot/snake_pot_inner_collision_faces.inc"
};

static NJS_MESHSET snake_pot_inner_collision_meshsets[1] = {
#include "stg26/snake_pot/snake_pot_inner_collision_meshsets.inc"
};

static NJS_POINT3 snake_pot_inner_collision_points[8] = {
#include "stg26/snake_pot/snake_pot_inner_collision_points.inc"
};

static NJS_VECTOR snake_pot_inner_collision_normals[8] = {
#include "stg26/snake_pot/snake_pot_inner_collision_normals.inc"
};

static NJS_MODEL snake_pot_inner_collision_model = {
#include "stg26/snake_pot/snake_pot_inner_collision_model.inc"
};

static NJS_OBJECT snake_pot_inner_collision = {
#include "stg26/snake_pot/snake_pot_inner_collision.inc"
};

#define FLAMES 8
/* the first flame shows FLAME_FRAME's frame. each flame after it shows the
 * frame FRAME_STAGGER past the previous flame's */
#define FRAME_STAGGER 3

/* @hack spelling: SCALE_ACROSS and SCALE_UP read task->work again, where
 * the caller's work would do */
#define SCALE_ACROSS(task) ((task)->work->scl.x + 1.0F)
#define SCALE_UP(task) ((task)->work->scl.y + 1.0F)

/* the outer and inner copies' radii at a scale of 1. a copy collides only
 * while a player lies near its pos. near is within REACH_MARGIN plus its
 * radius times the pot's larger scale. the inner copy's size class in the
 * collision world is that of POT_RADIUS times that scale */
#define POT_RADIUS 117.1F
#define INNER_RADIUS 105.0F
#define REACH_MARGIN 10.0F

/* the flames' sound plays at a point SOUND_HEIGHT above the pot's pos, times
 * the pot's scale up. it plays while player 0 lies within flame_sound_range
 * of that point, times the pot's scale across */
#define SOUND_HEIGHT 100.0F

static void pot_destroy(struct task *task);
static void pot_step(struct task *task);
static void pot_draw(struct task *task);
static void inner_init(struct task *task);
static void inner_destroy(struct task *task);
static void inner_step(struct task *task);

NJS_TEXLIST *quarters_snake_pot_texlists[2] = {
#include "stg26/snake_pot/quarters_snake_pot_texlists.inc"
};
/* one flame's shape at a scale of 1. the init scales a copy for each of
 * flames and moves it there */
/* clang-format off */
static struct collision_shape flame_shape = {
    0, FLAME_FORM, FLAME_PUSH, FLAME_LEVELS, FLAME_SHAPE_FLAGS,
    {0.0F, 0.0F, 0.0F}, 5.0F, 10.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the flames' places on the rim at a scale of 1. four lie on the axes, then
 * four on the diagonals */
static NJS_VECTOR flames[FLAMES] = {
#include "stg26/snake_pot/flames.inc"
};
static Sint32 flame_volume = 50;
static Float flame_sound_range = 150.0F;
static Bool built = FALSE;
/* @hack type: pot_build an array of one, where a single record would do */
static struct model_build pot_build[1] = {
#include "stg26/snake_pot/pot_build.inc"
};

Uint8 *quarters_snake_pot_build(Uint8 *buffer)
{
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, pot_build, 1) == 1) {
        built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buffer;
}

/* pose object, a collision copy, as the pot stands. it lies at the pot's pos,
 * scaled as the pot is. it turns about y as the pot turns, and by ang_xz
 * about x and z. the macro clears the evalflags that would have the copy's
 * pos, ang and scl taken as unit */
/* @hack spelling: the scales across chained, where two assignments would
 * do */
#define POSE_COPY(object, task, work, ang_xz)                                  \
    (object)->evalflags &=                                                     \
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);          \
    (object)->scl[0] = (object)->scl[2] = SCALE_ACROSS(task);                  \
    (object)->scl[1] = SCALE_UP(task);                                         \
    (object)->ang[0] = (ang_xz);                                               \
    (object)->ang[1] = (work)->ang.y;                                          \
    (object)->ang[2] = (ang_xz);                                               \
    (object)->pos[0] = (work)->pos.x;                                          \
    (object)->pos[1] = (work)->pos.y;                                          \
    (object)->pos[2] = (work)->pos.z

void quarters_snake_pot_init(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_shape shapes[FLAMES];
    NJS_OBJECT *object;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (!task_create_child(TASK_WORK, inner_init, task)) {
        return;
    }
    task->draw = pot_draw;
    task->step = pot_step;
    task->destructor = pot_destroy;
    for (i = 0; i < FLAMES; i++) {
        shapes[i] = flame_shape;
        shapes[i].radius *= SCALE_ACROSS(task);
        shapes[i].size_y *= SCALE_UP(task);
        shapes[i].center.x = flames[i].x * SCALE_ACROSS(task);
        shapes[i].center.y = flames[i].y * SCALE_UP(task);
        shapes[i].center.z = flames[i].z * SCALE_ACROSS(task);
    }
    collision_init(task, shapes, FLAMES, COLLISION_OBJECT);
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend, at 0x8c504b88. pot_destroy tests
     * task->extra for null at 0x8c504ca8, but pot_step hands player_near the
     * copy's pos untested at 0x8c504d0a */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the pot and its inner part, and set.c makes it again on a
     * later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = snake_pot_collision;
    POSE_COPY(object, task, work, 0);
    world_add(WORLD_SOLID, task, object);
    FLAME_FRAME(task) = (int)(rand() * (FLAME_FRAMES * RAND_SCALE));
}

static void pot_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void pot_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float size;
    NJS_POINT3 sound_pos;

    if (!placement_try_leave(task)) {
        object = task->extra.p;
        size = SCALE_ACROSS(task) > SCALE_UP(task) ? SCALE_ACROSS(task)
                                                   : SCALE_UP(task);
        if (player_near((NJS_POINT3 *)object->pos,
                        (size * POT_RADIUS) + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        collision_register(task);
        sound_pos = work->pos;
        sound_pos.y += SCALE_UP(task) * SOUND_HEIGHT;
        PLAY_NEAR_FLAME(task, &sound_pos,
                        flame_sound_range * SCALE_ACROSS(task), flame_volume);
        if (++FLAME_FRAME(task) >= FLAME_FRAMES) {
            FLAME_FRAME(task) = 0;
        }
    }
}

static void pot_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    int i;
    int texture;
    Uint32 blend;

    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    scale.x = scale.z = SCALE_ACROSS(task);
    scale.y = SCALE_UP(task);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&snake_pot_texlist);
    if (built) {
        quarters_direct_draw_scaled_v(pot_build[0].built, &scale);
    } else {
        quarters_model_draw_scaled_v(&snake_pot_model, &scale);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    for (i = 0; i < FLAMES; i++) {
        njPushMatrixEx();
        /* @hack spelling: texture, a local for the halfword's new value, built
         * in two statements, where one assignment to the halfword would do */
        texture = snake_pot_flame_model->plist[FLAME_TID] & ~FLAME_TID_BITS;
        texture |= (FLAME_FRAME(task) + (i * FRAME_STAGGER)) % FLAME_FRAMES;
        snake_pot_flame_model->plist[FLAME_TID] = texture;
        njTranslateEx(&flames[i]);
        njRotateY(NULL,
                  -work->ang.y + camera_views[camera_current_index]->ang.y);
        njFogDisable();
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(ADDITIVE_BLEND);
        njSetTexture(&quarters_flame_red_texlist);
        njCnkEasyDrawModel(snake_pot_flame_model);
        /* @hack spelling: blend, a local for ALPHA_BLEND, where ALPHA_BLEND
         * passed in place would do */
        blend = ALPHA_BLEND;
        njSetCnkBlendMode(blend);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        draw_control_restore();
        njFogEnable();
        njPopMatrixEx();
    }
    njPopMatrixEx();
    /* @hack redundant: a test of work->flags against ALPHA_BLEND's bits, in a
     * block no path reaches */
    if (0) {
        if (work->flags & ALPHA_BLEND) {
        }
    }
}

static void inner_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;
    NJS_OBJECT *object;
    Uint32 flags;
    Angle zero;
    Float size;

    work->pos = parent->work->pos;
    work->ang = parent->work->ang;
    work->scl = parent->work->scl;
    task->step = inner_step;
    task->destructor = inner_destroy;
    /* @hack spelling: zero, a local for the copy's turn about x and z, where 0
     * passed in place would do */
    zero = 0;
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend, at 0x8c505210. inner_destroy tests
     * task->extra for null at 0x8c505332, but inner_step hands player_near the
     * copy's pos untested at 0x8c5053b2 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the
     * inner part takes this init as its step again, to try on its next
     * step */
    if (object == NULL) {
        task->step = inner_init;
        return;
    }
#endif
    *object = snake_pot_inner_collision;
    POSE_COPY(object, task, work, zero);
    size = SCALE_ACROSS(task) > SCALE_UP(task) ? SCALE_ACROSS(task)
                                               : SCALE_UP(task);
    flags = quarters_world_size_class(size * POT_RADIUS) | WORLD_DUSTY |
            WORLD_SOLID;
    world_add(flags, task, object);
}

static void inner_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void inner_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;
    NJS_OBJECT *object;
    Float size;

    work->pos = parent->work->pos;
    work->ang = parent->work->ang;
    work->scl = parent->work->scl;
    object = task->extra.p;
    size = SCALE_ACROSS(task) > SCALE_UP(task) ? SCALE_ACROSS(task)
                                               : SCALE_UP(task);
    if (player_near((NJS_POINT3 *)object->pos,
                    (size * INNER_RADIUS) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
