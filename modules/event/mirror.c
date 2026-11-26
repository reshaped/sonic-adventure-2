#include <ninja.h>
#include <nj_motion.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "event/mirror.h"
#include "event/scene.h"
#include "system/util.h"
#include "system/vector.h"

/* the margin below the interval's low end that the one-key test reads */
#define LOW_MARGIN 0.01F

static NJS_POINT3 centers[MIRRORS];
static NJS_VECTOR normals[MIRRORS];
/* event_mirrors_init clears it, and nothing reads it */
static Sint32 dat_8c53c978;

Bool event_motion_position(NJS_MOTION *motion, int frame, NJS_POINT3 *pos)
{
    if (motion) {
        if ((Uint32)frame >= motion->nbFrame) {
            frame = motion->nbFrame - 1;
        }
        if (((NJS_MDATA1 *)motion->mdata)->p[0]) {
            njPushMatrix(NULL);
            njSetCnkCurrentMotion(motion, frame);
            njGetCnkMotionTranslate(&pos->x, 0);
            njPopMatrix(1);
            return TRUE;
        }
        pos->x = 0.0F;
        pos->y = 0.0F;
        pos->z = 0.0F;
    }
    return FALSE;
}

/* the frame a key opens with, and the frame of key index of span */
#define KEY_FRAME(key) (*(Sint32 *)(key))
#define FRAME_AT(span, index)                                                  \
    KEY_FRAME((span)->keys + ((span)->stride * (index)))

Bool event_find_keys(struct event_key_span *span)
{
    Sint32 bounds[2];
    char *key;
    Sint32 direction;
    Sint32 index;
    Sint32 first;
    Float low;
    Float high;

    if (span->count == 1) {
        if (KEY_FRAME(span->keys) < 0) {
            span->first = -1;
            span->last = -1;
            return FALSE;
        }
        span->first = 0;
        span->last = 1;
        return TRUE;
    }
    if (span->start > span->end) {
        high = span->start;
        low = span->end;
    } else {
        high = span->end;
        low = span->start;
    }
    if (KEY_FRAME(span->keys) > low || low > FRAME_AT(span, span->count - 1)) {
        span->first = -1;
        span->last = -1;
        return FALSE;
    }
    key = span->keys;
    if (low <= KEY_FRAME(key) && high > KEY_FRAME(key)) {
        index = 0;
    } else {
        if ((Sint32)high == (Sint32)(low - LOW_MARGIN)) {
            span->first = -1;
            span->last = -1;
            return FALSE;
        }
        index = 0;
        direction = 1;
        bounds[0] = 0;
        bounds[1] = span->count - 1;
        while (direction != 0) {
            if (direction == -1) {
                if (bounds[1] - bounds[0] == 1) {
                    index--;
                } else {
                    index -= (bounds[1] - bounds[0]) / 2;
                }
            } else {
                if (bounds[1] - bounds[0] == 1) {
                    index++;
                } else {
                    index += (bounds[1] - bounds[0]) / 2;
                }
            }
            key = span->keys + (span->stride * index);
            if (low <= KEY_FRAME(key)) {
                if (low > KEY_FRAME(key - span->stride)) {
                    direction = 0;
                } else {
                    direction = -1;
                    bounds[1] = index;
                }
            } else if (KEY_FRAME(key + span->stride) > low) {
                index++;
                direction = 0;
            } else {
                direction = 1;
                bounds[0] = index;
            }
        }
    }
    /* @hack spelling: a goto loop that steps index back, where a while loop
     * would do */
rewind:
    if (index != 0) {
        first = index - 1;
        if (low <= FRAME_AT(span, first)) {
            index--;
            goto rewind;
        }
    }
    first = index;
    if (high > FRAME_AT(span, index)) {
        do {
            if (++index >= span->count) {
                index = span->count - 1;
                break;
            }
        } while (high > FRAME_AT(span, index));
    }
    if (first == index) {
        span->first = -1;
        span->last = -1;
        return FALSE;
    }
    span->first = first;
    span->last = index;
    return TRUE;
}

static void draw_modifier_node(NJS_CNK_OBJECT *node)
{
    njPushMatrix(NULL);
    njCnkMotionTranslate(NULL, node, 0);
    njCnkMotionRotateZXY(NULL, node, 0);
    njCnkMotionScale(NULL, node, 0);
    njSetCnkNextMotionNode();
    if (node->model) {
        njCnkModDrawModel(node->model);
    }
    if (node->child) {
        draw_modifier_node(node->child);
    }
    njPopMatrix(1);
    if (node->sibling) {
        draw_modifier_node(node->sibling);
    }
}

void event_draw_modifier_motion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                Float frame)
{
    Float pose;

    if (frame >= motion->nbFrame - 1.0F) {
        pose = motion->nbFrame - 1.0F;
    } else {
        pose = frame;
    }
    njSetCnkCurrentMotion(motion, pose);
    draw_modifier_node(object);
}

#define QUAD(i) (event_scene_file->mirrors->quad[(i)])
#define MIDPOINT(i, axis)                                                      \
    ((QUAD(i)[0].axis + QUAD(i)[1].axis + QUAD(i)[2].axis + QUAD(i)[3].axis) * \
     (1.0F / QUAD_CORNERS))

void event_mirrors_init(void)
{
    NJS_VECTOR edges[2];
    int i;

    for (i = 0; i < event_scene_file->mirrors->count; i++) {
        centers[i].x = MIDPOINT(i, x);
        centers[i].y = MIDPOINT(i, y);
        centers[i].z = MIDPOINT(i, z);
        vector_subtract(&QUAD(i)[1], &QUAD(i)[0], &edges[0]);
        vector_subtract(&QUAD(i)[2], &QUAD(i)[0], &edges[1]);
        njOuterProduct(&edges[0], &edges[1], &normals[i]);
        njUnitVector(&normals[i]);
    }
    dat_8c53c978 = 0;
}

/* @hack spelling: the entity indexed afresh at each use, where a pointer to it
 * would do */
#define ENTITY(scene, index) (event_scenes[(scene)].entities[(index)])

void event_mirrors_draw(enum event_draw_kind kind, int scene, int index)
{
    NJS_POINT3 position;
    NJS_VECTOR to_entity;
    NJS_VECTOR to_center;
    NJS_PLANE plane;
    NJS_POINT3 projection;
    NJS_VECTOR reflection;
    NJS_VECTOR offset;
    int i;

    if (!(ENTITY(scene, index).flags & EVENT_REFLECTED)) {
        return;
    }
    if (ENTITY(scene, index).motion) {
        if (event_motion_position(ENTITY(scene, index).motion,
                                  event_scene_frame, &position) == FALSE) {
            position = ENTITY(scene, index).position;
        }
    } else {
        position = ENTITY(scene, index).position;
    }
    for (i = 0; i < event_scene_file->mirrors->count; i++) {
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        NJS_VECTOR camera;
        camera.x = _nj_motion_camera_.px;
        camera.y = _nj_motion_camera_.py;
        camera.z = _nj_motion_camera_.pz;
        vector_subtract(&position, &camera, &to_entity);
        vector_subtract(&centers[i], &camera, &to_center);
#else
        /* @hack type: the camera's px read as an NJS_VECTOR, where a copy of
         * its px, py and pz would do */
        vector_subtract(&position, (NJS_VECTOR *)&_nj_motion_camera_.px,
                        &to_entity);
        vector_subtract(&centers[i], (NJS_VECTOR *)&_nj_motion_camera_.px,
                        &to_center);
#endif
        plane.px = QUAD(i)[0].x;
        plane.py = QUAD(i)[0].y;
        plane.pz = QUAD(i)[0].z;
        plane.vx = -normals[i].x;
        plane.vy = -normals[i].y;
        plane.vz = -normals[i].z;
        njDistanceP2PL(&position, &plane, &projection);
        vector_subtract(&projection, &position, &offset);
        vector_add(&offset, &projection, &reflection);
        njPushMatrix(NULL);
        njMirror(NULL, &plane);
        event_blend_start(event_scene_file->mirrors->opacity[i]);
        draw_control_set(NJD_CONTROL_3D_MIRROR_MODEL);
        /* NOLINTNEXTLINE(clang-diagnostic-switch): no other kind reaches it */
        switch (kind) {
        case EVENT_OBJECT:
        case EVENT_MULTI_OBJECT:
            if (ENTITY(scene, index).flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleDrawObject(ENTITY(scene, index).object);
            } else {
                njCnkEasyDrawObject(ENTITY(scene, index).object);
            }
            break;
        case EVENT_MOTION:
        case EVENT_MULTI_MOTION:
            if (ENTITY(scene, index).flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleDrawMotion(ENTITY(scene, index).object,
                                      ENTITY(scene, index).motion,
                                      event_scene_frame);
            } else {
                njCnkEasyDrawMotion(ENTITY(scene, index).object,
                                    ENTITY(scene, index).motion,
                                    event_scene_frame);
            }
            break;
        case EVENT_SHAPE:
        case EVENT_MULTI_SHAPE:
            if (ENTITY(scene, index).flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleDrawShapeMotion(
                    ENTITY(scene, index).object, ENTITY(scene, index).motion,
                    ENTITY(scene, index).shape, event_scene_frame);
            } else {
                njCnkEasyDrawShapeMotion(
                    ENTITY(scene, index).object, ENTITY(scene, index).motion,
                    ENTITY(scene, index).shape, event_scene_frame);
            }
            break;
        case EVENT_MODIFIER:
            njCnkModDrawObject(ENTITY(scene, index).object);
            break;
        case EVENT_MODIFIER_MOTION:
            event_draw_modifier_motion(ENTITY(scene, index).object,
                                       ENTITY(scene, index).motion,
                                       event_scene_frame);
            break;
        }
        draw_control_clear(NJD_CONTROL_3D_MIRROR_MODEL);
        event_blend_stop();
        njPopMatrix(1);
    }
}

void event_blend_start(int opacity)
{
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_attr_save();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
    draw_attr_set(0, NJD_FST_UA);
    draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
    constant_material_set((opacity * 0.01F) - 1.0F, 0.0F, 0.0F, 0.0F);
}

void event_blend_stop(void)
{
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}
