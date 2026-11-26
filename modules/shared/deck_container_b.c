#include "shared/deck_container_b/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/deck_container.h"
#include "shared/deck_rail.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for ang.y, scl.x and scl.y, which no code reads */
static const char editor_angle_y[12] = {
#include "shared/deck_container_b/editor_angle_y.inc"
};
static const char editor_travel[16] = {
#include "shared/deck_container_b/editor_travel.inc"
};
static const char editor_speed[12] = {
#include "shared/deck_container_b/editor_speed.inc"
};

static NJS_TEXNAME deck_container_b_textures[6] = {
#include "shared/deck_container_b/deck_container_b_textures.inc"
};

static NJS_TEXLIST deck_container_b_texlist = {
#include "shared/deck_container_b/deck_container_b_texlist.inc"
};

static Sint16 deck_container_b_node_polygons[399] = {
#include "shared/deck_container_b/deck_container_b_node_polygons.inc"
};

static Sint32 deck_container_b_node_vertices[355] = {
#include "shared/deck_container_b/deck_container_b_node_vertices.inc"
};

static NJS_CNK_MODEL deck_container_b_model = {
#include "shared/deck_container_b/deck_container_b_model.inc"
};

static NJS_CNK_OBJECT deck_container_b_node = {
#include "shared/deck_container_b/deck_container_b_node.inc"
};

static NJS_MATERIAL deck_container_b_collision_material[1] = {
#include "shared/deck_container_b/deck_container_b_collision_material.inc"
};

static Sint16 deck_container_b_collision_faces_0[19] = {
#include "shared/deck_container_b/deck_container_b_collision_faces_0.inc"
};

static NJS_MESHSET deck_container_b_collision_meshes[1] = {
#include "shared/deck_container_b/deck_container_b_collision_meshes.inc"
};

static NJS_POINT3 deck_container_b_collision_points[8] = {
#include "shared/deck_container_b/deck_container_b_collision_points.inc"
};

static NJS_VECTOR deck_container_b_collision_normals[8] = {
#include "shared/deck_container_b/deck_container_b_collision_normals.inc"
};

static NJS_MODEL deck_container_b_collision_model = {
#include "shared/deck_container_b/deck_container_b_collision_model.inc"
};

NJS_OBJECT deck_container_empty[1] = {
#include "shared/deck_container_b/deck_container_empty.inc"
};

NJS_TEXLIST *deck_container_b_texlists[2] = {
#include "shared/deck_container_b/deck_container_b_texlists.inc"
};

static struct collision_shape box[1] = {
#include "shared/deck_container_b/box.inc"
};
static void deck_container_b_destroy(struct task *task);
static void deck_container_b_step(struct task *task);
static void deck_container_b_draw(struct task *task);

void deck_container_b_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *node;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct deck_rail));
    if (!task->motion.p) {
        return;
    }
    task->draw = deck_container_b_draw;
    task->step = deck_container_b_step;
    task->destructor = deck_container_b_destroy;
    work->mode = RAIL_START;
    node = &RAIL(task)->collision;
    *node = deck_container_empty[0];
    RAIL_REGISTER_NODE(task, work, node);
    model_object_create(task, NULL, NULL, box, 1, 0);
    if (task->child) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
    }
    SPEED(task) = 0.0F;
}

static void deck_container_b_destroy(struct task *task)
{
    if (RAIL(task)->collision.model) {
        world_remove(task, &RAIL(task)->collision);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void deck_container_b_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *collision;
    struct deck_rail *rail;
    NJS_POINT3 before;
    NJS_POINT3 center;
    NJS_VECTOR displacement;
#ifdef NONMATCHING
#else
    /* @hack spelling: the displacement's components walked by pointers,
     * where named components would do */
    Float *delta;
    Float *current;
    Float *previous;
#endif

    if (placement_try_leave(task)) {
        return;
    }
    before = RAIL(task)->offset;
    switch (work->mode) {
    default:
        SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
        if (SPEED(task) > TOP_SPEED(work)) {
            SPEED(task) = TOP_SPEED(work);
        }
        RAIL(task)->offset.z += SPEED(task);
        if (RAIL(task)->offset.z > work->scl.x) {
            RAIL(task)->offset.z = work->scl.x;
            SPEED(task) = 0.0F;
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = RAIL_BACK;
                work->timer = 0;
            }
        } else {
            sound_play_at_for(SLIDE_SOUND, work, 1, 0, SLIDE_SOUND_STEPS,
                              &work->pos);
        }
        break;
    case RAIL_BACK:
        SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
        if (SPEED(task) > TOP_SPEED(work)) {
            SPEED(task) = TOP_SPEED(work);
        }
        RAIL(task)->offset.z -= SPEED(task);
        if (RAIL(task)->offset.z < -work->scl.x) {
            RAIL(task)->offset.z = -work->scl.x;
            SPEED(task) = 0.0F;
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = RAIL_OUT;
                work->timer = 0;
            }
        } else {
            sound_play_at_for(SLIDE_SOUND, work, 1, 0, SLIDE_SOUND_STEPS,
                              &work->pos);
        }
        break;
    }
    if (task->child) {
        rail_push_frame(work);
        njCalcPoint(NULL, &RAIL(task)->offset, &task->child->work->pos);
        njPopMatrix(1);
    }
    if (RAIL(task)->collision.model) {
        rail = RAIL(task);
        collision = &rail->collision;
#ifdef NONMATCHING
        /* the displacement by named components, where the matching side walks
         * pointers for SHC's bytes */
        displacement.x = rail->offset.x - before.x;
        displacement.y = rail->offset.y - before.y;
        displacement.z = rail->offset.z - before.z;
#else
        delta = &displacement.x;
        current = &rail->offset.x;
        previous = &before.x;
        *delta++ = *current++ - *previous++;
        *delta++ = *current++ - *previous++;
        *delta = *current - *previous;
#endif
        RAIL_MOVE_NODE(task, work, collision, displacement, center,
                       REACH_MARGIN);
    }
}

static void deck_container_b_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&deck_container_b_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslateV(NULL, &RAIL(task)->offset);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&deck_container_b_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}
