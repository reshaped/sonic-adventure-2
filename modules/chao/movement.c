#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "chao/garden_scene.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/ui.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/screen.h"
#include "object/mover.h"
#ifdef NONMATCHING
/* set_read, by which this side's garden_file_load reads a file */
#include "object/set.h"
#endif
#include "shared/water_ripple.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/util.h"

#ifdef NONMATCHING
/* port_view_x and port_view_aspect, the window this side's test takes */
#include "engine/dc/port.h"
#endif

static Float garden_screen_distance = 500.0F;
/* garden_visible_box takes an object wholly below this screen line as out of
 * view */
#define VIEW_BOTTOM 440.0F
/* the least bytes garden_file_load's block holds */
#define FILE_RESERVE 0x4000

/* @hack spelling: both loops written out at each axis, where a function
 * called for each would do */
#define WRAP_DEGREES(delta)                                                    \
    while ((delta) > 180.0F) {                                                 \
        (delta) -= 360.0F;                                                     \
    }                                                                          \
    while ((delta) < -180.0F) {                                                \
        (delta) += 360.0F;                                                     \
    }

void garden_blend_angles(const Angle *from, const Angle *to, Float share,
                         Angle *result)
{
    Float x;
    Float y;
    Float z;
    Float dx;
    Float dy;
    Float dz;

    x = *from++ * DEGREES_PER_ANGLE;
    y = *from++ * DEGREES_PER_ANGLE;
    z = *from * DEGREES_PER_ANGLE;
    dx = *to++ * DEGREES_PER_ANGLE;
    dy = *to++ * DEGREES_PER_ANGLE;
    dz = *to * DEGREES_PER_ANGLE;
    /* @hack spelling: the differences taken in y, z, x order, x's by -=, where
     * x, y, z by one spelling would do */
    dy = dy - y;
    dz = dz - z;
    dx -= x;
    WRAP_DEGREES(dx);
    WRAP_DEGREES(dy);
    WRAP_DEGREES(dz);
    result[0] = (Angle)((x + (dx * share)) * ANGLE_PER_DEGREE);
    result[1] = (Angle)((y + (dy * share)) * ANGLE_PER_DEGREE);
    result[2] = (Angle)((z + (dz * share)) * ANGLE_PER_DEGREE);
}

#define RECT_CORNER(vertex, px, py, depth, color)                              \
    do {                                                                       \
        (vertex).x = (px);                                                     \
        (vertex).y = (py);                                                     \
        (vertex).z = (depth);                                                  \
        (vertex).col = (color);                                                \
    } while (0)

void garden_draw_plain_rect(int left, int top, int right, int bottom,
                            Uint32 color)
{
    NJS_POLYGON_VTX vertices[4];

    RECT_CORNER(vertices[0], left, top, 300.5F, color);
    RECT_CORNER(vertices[1], left, bottom, 300.5F, color);
    RECT_CORNER(vertices[2], right, top, 300.5F, color);
    RECT_CORNER(vertices[3], right, bottom, 300.5F, color);
    njDrawPolygon(vertices, 4, 1);
}

void garden_draw_rect(int left, int top, int right, int bottom, Float depth,
                      Uint32 color)
{
    NJS_POLYGON_VTX vertices[4];

    RECT_CORNER(vertices[0], left, top, depth, color);
    RECT_CORNER(vertices[1], left, bottom, depth, color);
    RECT_CORNER(vertices[2], right, top, depth, color);
    RECT_CORNER(vertices[3], right, bottom, depth, color);
    njDrawPolygon(vertices, 4, 1);
}

Float chao_target_distance_squared(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    NJS_POINT3 *target = &mover->target;
    NJS_POINT3 *position = &chao->work->pos;

    /* @hack spelling: each difference written twice, where a local squared
     * would do */
    return ((target->x - position->x) * (target->x - position->x)) +
           ((target->z - position->z) * (target->z - position->z));
}

void garden_aim_angles(NJS_VECTOR *direction, struct angles *angles)
{
    NJS_VECTOR local;

    angles->y = (Angle)(atan2f(direction->x, direction->z) * ANGLE_PER_RADIAN);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, -angles->y);
    njCalcVector(NULL, direction, &local);
    njPopMatrixEx();
    angles->x = -(Angle)(atan2f(local.y, local.z) * ANGLE_PER_RADIAN);
}

Bool chao_settled(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    NJS_VECTOR horizontal;

    if (mover->flags & MOVER_ON_FLOOR) {
        horizontal.x = mover->spd.x;
        horizontal.y = 0.0F;
        horizontal.z = mover->spd.z;
        if (njScalor(&horizontal) < 0.04F) {
            return 1;
        }
    }
    return 0;
}

Bool garden_visible_box(struct task *task, Float half_width, Float half_height,
                        Float height)
{
    NJS_POINT2 screen;
    NJS_POINT3 position;
    NJS_VECTOR facing;
    NJS_VECTOR to_object;
    struct task_work *work;
    struct camera_place *camera;
    struct garden_entry *entry;
    Float distance;
    Float scale;
    Float width;
    Float height_on_screen;
    Angle yaw;

    work = task->work;
    camera = camera_views[camera_current_index];
    position = work->pos;
    position.y += height;
    distance = njDistanceP2P(&camera->pos, &position);
    entry = task->force.p;
    if (entry) {
        entry->distance = distance;
    }
    yaw = camera_views[camera_current_index]->ang.y;
    to_object.x = work->pos.x - camera->pos.x;
    to_object.y = work->pos.y - camera->pos.y;
    to_object.z = work->pos.z - camera->pos.z;
    facing.x = njSin(yaw);
    facing.y = 0.0F;
    facing.z = njCos(yaw);
    if (njInnerProduct(&to_object, &facing) > 0.0F) {
        return FALSE;
    }
    scale = (1.0F / distance) * garden_screen_distance;
    width = half_width * scale;
    height_on_screen = half_height * scale;
    njProjectScreen(NULL, &position, &screen);
#ifdef NONMATCHING
    /* this side takes the port's window, which 16:9 widens outside a demo
     * (engine/dc/port.h) */
    screen.x = port_view_x(screen.x);
    width *= port_view_aspect;
#endif
    if (screen.x + width < 0.0F || screen.x - width > SCREEN_WIDTH ||
        screen.y + height_on_screen < 0.0F ||
        screen.y - height_on_screen > VIEW_BOTTOM) {
        return FALSE;
    }
    return TRUE;
}

Bool garden_visible_at_height(struct task *task, Float radius, Float height)
{
    return garden_visible_box(task, radius, radius, height);
}

Bool garden_object_visible(struct task *task, Float radius)
{
    return garden_visible_box(task, radius, radius, 0.0F);
}

void garden_blend_models(const NJS_CNK_OBJECT *from, const NJS_CNK_OBJECT *to,
                         NJS_CNK_OBJECT *result, Float share)
{
    const NJS_CNK_MODEL *from_model;
    const NJS_CNK_MODEL *to_model;
    NJS_CNK_MODEL *result_model;
    Float keep = 1.0F - share;
    struct chunk_vertex_d8 *vertex;
    const struct chunk_vertex_d8 *from_vertex;
    const struct chunk_vertex_d8 *to_vertex;
    const NJS_POINT3 *from_point;
    const NJS_POINT3 *to_point;
    NJS_POINT3 *point;
    const Uint8 *from_color;
    const Uint8 *to_color;
    Uint8 *color;
    const Float *from_position;
    const Float *to_position;
    Float *position;
    const Angle *from_angles;
    const Angle *to_angles;
    Angle *angles;
    Uint16 count;
    int index;
    int channel;
    int value;

    result_model = result->model;
    if (result_model) {
        from_model = from->model;
        to_model = to->model;
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        count = CHUNK_VERTEX_COUNT(result_model->vlist);
#else
        /* @hack type: the count read as vlist's fourth halfword, where
         * CHUNK_VERTEX_COUNT would do */
        count = ((Uint16 *)result_model->vlist)[3];
#endif
        vertex =
            (struct chunk_vertex_d8 *)(result->model->vlist + CHUNK_HEAD_WORDS);
        from_vertex = (const struct chunk_vertex_d8 *)(from_model->vlist +
                                                       CHUNK_HEAD_WORDS);
        to_vertex = (const struct chunk_vertex_d8 *)(to_model->vlist +
                                                     CHUNK_HEAD_WORDS);
        for (index = 0; index < count; index++) {
            from_point = &from_vertex->point;
            to_point = &to_vertex->point;
            point = &vertex->point;
            point->x = (from_point->x * keep) + (to_point->x * share);
            point->y = (from_point->y * keep) + (to_point->y * share);
            point->z = (from_point->z * keep) + (to_point->z * share);
            color = (Uint8 *)&vertex->color;
            from_color = (const Uint8 *)&from_vertex->color;
            to_color = (const Uint8 *)&to_vertex->color;
            for (channel = 0; channel < 4; channel++) {
                value = (int)((*from_color * keep) + (*to_color * share));
                if (value < 0) {
                    value = 0;
                } else if (value > 255) {
                    value = 255;
                }
                *color = value;
                color++;
                from_color++;
                to_color++;
            }
            from_vertex++;
            to_vertex++;
            vertex++;
        }
    }
    to_position = to->pos;
    from_position = from->pos;
    position = result->pos;
    position[0] = (from_position[0] * keep) + (to_position[0] * share);
    position[1] = (from_position[1] * keep) + (to_position[1] * share);
    position[2] = (from_position[2] * keep) + (to_position[2] * share);
    to_angles = to->ang;
    from_angles = from->ang;
    angles = result->ang;
    angles[0] = (Angle)((from_angles[0] * keep) + (to_angles[0] * share));
    angles[1] = (Angle)((from_angles[1] * keep) + (to_angles[1] * share));
    angles[2] = (Angle)((from_angles[2] * keep) + (to_angles[2] * share));
    if (result->child) {
        garden_blend_models(from->child, to->child, result->child, share);
    }
    if (result->sibling) {
        garden_blend_models(from->sibling, to->sibling, result->sibling, share);
    }
}

Angle garden_heading_to_object(struct task *task, struct task *other)
{
    NJS_POINT3 *from = &task->work->pos;
    NJS_POINT3 *to = &other->work->pos;

    return (Angle)(atan2f(to->x - from->x, to->z - from->z) * ANGLE_PER_RADIAN);
}

int garden_turn_to_object(struct task *task, struct task *other, int step)
{
    struct task_work *work;
    struct mover *mover;

    if (!task || !other) {
        return 0;
    }
    work = task->work;
    mover = task->motion.p;
    mover->aim_y = garden_heading_to_object(task, other);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, step);
    return angle_distance(mover->aim_y, work->ang.y);
}

Float garden_object_distance(struct task *task, struct task *other)
{
    if (!task || !other) {
        return -1.0F;
    }
    return njDistanceP2P(&task->work->pos, &other->work->pos);
}

void chao_set_forward_acceleration(struct task *chao, Float acceleration)
{
    struct task_work *work = chao->work;
    struct mover *mover = chao->motion.p;

    mover->acc.x = njSin(work->ang.y) * acceleration;
    mover->acc.z = njCos(work->ang.y) * acceleration;
}

void garden_move_forward(struct task *task, Float distance)
{
    struct task_work *work = task->work;

    work->pos.x += njSin(work->ang.y) * distance;
    work->pos.z += njCos(work->ang.y) * distance;
}

void chao_slow_on_floor(struct task *chao, Float scale)
{
    struct mover *mover = chao->motion.p;

    if (mover->flags & MOVER_ON_FLOOR) {
        mover->spd.x *= scale;
        mover->spd.y *= scale;
        mover->spd.z *= scale;
    }
}

void chao_link_walk_motion(struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_RUN);

    if (points < chao_tuning.walk_slow_points) {
        chao_link_motion(chao, CHAO_ANIM_HAIHAI);
    } else if (points < chao_tuning.walk_points) {
        chao_link_motion(chao, CHAO_ANIM_ARUKU);
    } else if (points < chao_tuning.walk_fast_points) {
        chao_link_motion(chao, CHAO_ANIM_HASIRU);
    } else {
        chao_link_motion(chao, CHAO_ANIM_HASIRU_HIROGE);
    }
}

struct set_head *garden_file_load(const char *name)
{
#ifdef NONMATCHING
    /* this side reads the file by set_read, which returns null for a read that
     * fails. it returns null too for a file whose extent is bad. the matching
     * side copies whatever the work area holds */
    return set_read(name, NULL, FILE_RESERVE / sizeof(struct set_entry));
#else
    Sint32 size;
    Sint32 capacity;
    void *buffer;

    size = loader_file_size(name);
    if (size < 0) {
        return NULL;
    }
    capacity = FILE_RESERVE;
    if (capacity < size) {
        capacity = size;
    }
    buffer = syCalloc(1, capacity);
    if (!buffer) {
        return NULL;
    }
    loader_read_file(name, WORK_AREA);
    /* njMemCopy32 counts blocks of 32 bytes, so a tail shorter than one stays
     * zero */
    njMemCopy32(buffer, WORK_AREA, size >> 5);
    return buffer;
#endif
}

struct task *garden_hit_kind(struct task *object, int index, Uint8 kind)
{
    collision_restart_scans();
    return collision_next_with_id_by_shape(object, index, kind);
}

void garden_apply_buoyancy(struct task *task, Float depth)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    NJS_VECTOR displacement;
    NJS_VECTOR direction;
    Float length;
    Float speed;
    Float pushed;
    int ripple;

    if (mover->flat_y > work->pos.y) {
        if (work->pos.y > mover->flat_y - (depth * 2.0F)) {
            mover->acc.y += -(mover->gravity * (mover->flat_y - work->pos.y) *
                              (1.0F / depth));
        } else {
            mover->acc.y += -(mover->gravity * 2.0F);
        }
        if (collision_pushed(task)) {
            displacement.x = work->pos.x - mover->last_pos.x;
            displacement.y = work->pos.y - mover->last_pos.y;
            displacement.z = work->pos.z - mover->last_pos.z;
            direction = displacement;
            length = njScalor(&direction);
            if (length >= 0.01F) {
                njUnitVector(&direction);
            }
            speed = njInnerProduct(&direction, &mover->spd);
            pushed = njInnerProduct(&direction, &displacement);
            if (pushed > speed) {
                pushed = (pushed - speed) * 0.85F;
                mover->spd.x += direction.x * pushed;
                mover->spd.y += direction.y * pushed;
                mover->spd.z += direction.z * pushed;
            }
        }
        ripple = 0;
        if (njScalor(&mover->spd) > 0.2F) {
            if (rand() * RAND_SCALE < 0.05F) {
                ripple = 1;
            }
        } else {
            if (rand() * RAND_SCALE < 0.003F) {
                ripple = 1;
            }
        }
        if (ripple) {
            displacement.x = work->pos.x;
            displacement.y = mover->flat_y;
            displacement.z = work->pos.z;
            direction.x = 0.03F;
            direction.y = 0.0F;
            direction.z = 0.7F;
            ripple_create(&displacement, &direction, 0.2F);
        }
        mover->spd.x *= 0.975F;
        mover->spd.y *= 0.93F;
        mover->spd.z *= 0.975F;
    }
}
