/* a garden Chao's body posed by its motion, node by node, and moved by its
 * springs */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/body.h"
#ifdef NONMATCHING
/* chao_body_has_matrix_room, by which this side's pose tests the matrix
 * stack */
#include "chao/body_matrix.h"
#endif
#include "chao/body_pose.h"
#include "chao/icon.h"
#include "chao/movement.h"
#include "chao/work.h"
#include "draw/chunk.h"
#include "draw/motion.h"
#include "system/angle.h"
#include "system/task.h"

/* how far above the Chao's position an anchor lies under
 * CHAO_SHAPE_GROUND_SHADOW */
#define GROUND_LIFT 0.01F
/* the icon's bob: how high it rises, and the angle its bob steps a frame */
#define ICON_BOB 0.045F
#define ICON_BOB_STEP 0x470
/* how much higher a foot's anchor lies off the ground */
#define FOOT_RISE 0.4F
/* how far a mesh may move in a step toward its spring's pull, on each axis */
#define SPRING_REACH 0.6F

static Sint16 posing_node = 0;

/* motion_control's current and next_play, which lie side by side */
struct pose_play_pair {
    struct motion_play plays[2];
};

void chao_select_body_motion(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct motion_control *control = &work->motion;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    struct motion_play *current = &control->current;
    struct motion_play *next = &control->next_play;
#else
    /* @hack spelling: current and next_play as an array's two entries, where
     * &control->current and &control->next_play would do */
    struct motion_play *current =
        &((struct pose_play_pair *)&control->current)->plays[0];
    struct motion_play *next =
        &((struct pose_play_pair *)&control->current)->plays[1];
#endif
    NJS_MOTION *clip = current->motion;
    chao_pose.current.node = clip->mdata;
    chao_pose.current.frames = clip->nbFrame;
    chao_pose.current.frame = current->frame;
    if (control->flags & MOTION_LINKING) {
        clip = next->motion;
        chao_pose.next.node = clip->mdata;
        chao_pose.next.frames = clip->nbFrame;
        chao_pose.next.frame = next->frame;
        chao_pose.share = control->link;
        /* @hack spelling: a return at the arm's end, where falling to the
         * function's end would do */
        return;
    } else {
        chao_pose.next.node = NULL;
    }
}

/* @bug memory: the key search reads past the end of a channel of one key,
 * once the frame passes that key (0x0c501558) */
#ifdef NONMATCHING
/* this side repairs the bug tagged above. the search stops at the channel's
 * last key. that key loops to the first by its share of the frames left */
#define HAS_NEXT_KEY(list, axis) (next < key_begin + (list)->nb[axis])
#define IS_FINAL(list, axis, track)                                            \
    (next == key_begin + (list)->nb[axis] ||                                   \
     key->keyframe == (track).frames - 1)
#define KEY_FIRST(list, axis) (key_begin = (list)->p[axis])
#define LOOP_SHARE(track)                                                      \
    (((track).frame - (Float)key->keyframe) /                                  \
     (Float)((track).frames - key->keyframe))
#else
#define HAS_NEXT_KEY(list, axis) 1
#define IS_FINAL(list, axis, track) (key->keyframe == (track).frames - 1)
#define KEY_FIRST(list, axis) ((list)->p[axis])
#define LOOP_SHARE(track) ((track).frame - (Float)key->keyframe)
#endif

/* set key and next to the keys of list's channel axis around track's frame.
 * share becomes the frame's share of the way from key to next. where key lies
 * at the motion's last frame, next is the channel's first key */
#define FIND_KEYS(track, list, axis)                                           \
    {                                                                          \
        frame = (Uint32)(track).frame;                                         \
        key = KEY_FIRST(list, axis);                                           \
        next = key + 1;                                                        \
        while (HAS_NEXT_KEY(list, axis) && frame > key->keyframe &&            \
               frame >= next->keyframe) {                                      \
            key++;                                                             \
            next++;                                                            \
        }                                                                      \
        if (IS_FINAL(list, axis, track)) {                                     \
            next = (list)->p[axis];                                            \
            share = LOOP_SHARE(track);                                         \
        } else {                                                               \
            share = ((track).frame - (Float)key->keyframe) /                   \
                    (Float)(next->keyframe - key->keyframe);                   \
        }                                                                      \
    }

#define SAMPLE_POSITION(track, list, out)                                      \
    if (!(list)->p[0]) {                                                       \
        (out).x = node->object.pos[0];                                         \
        (out).y = node->object.pos[1];                                         \
        (out).z = node->object.pos[2];                                         \
    } else {                                                                   \
        FIND_KEYS(track, list, 0);                                             \
        from = key->key;                                                       \
        to = next->key;                                                        \
        x = *from++;                                                           \
        y = *from++;                                                           \
        z = *from;                                                             \
        x = ((*to++ - x) * share) + x;                                         \
        y = ((*to++ - y) * share) + y;                                         \
        z = ((*to - z) * share) + z;                                           \
        (out).x = x;                                                           \
        (out).y = y;                                                           \
        (out).z = z;                                                           \
        njAddVector(&(out), &node->offset);                                    \
    }

static void translate_pose(struct chao_node *node)
{
    NJS_VECTOR current = {0.0F, 0.0F, 0.0F};
    NJS_VECTOR next_position = {0.0F, 0.0F, 0.0F};
    NJS_VECTOR blended;
    NJS_MDATA2 *current_keys = chao_pose.current.node;
    NJS_MDATA2 *next_keys = chao_pose.next.node;
    NJS_MKEY_F *key;
    NJS_MKEY_F *next;
#ifdef NONMATCHING
    /* the channel's first key, by which this side's search finds its last */
    NJS_MKEY_F *key_begin;
#endif
    Uint32 frame;
    Float share;
    Float *from;
    Float *to;
    Float x;
    Float y;
    Float z;
    SAMPLE_POSITION(chao_pose.current, current_keys, current);
    if (next_keys) {
        SAMPLE_POSITION(chao_pose.next, next_keys, next_position);
        share = chao_pose.share;
        blended.x = (current.x * (1.0F - share)) + (next_position.x * share);
        blended.y = (current.y * (1.0F - share)) + (next_position.y * share);
        blended.z = (current.z * (1.0F - share)) + (next_position.z * share);
        njTranslateV(NULL, &blended);
    } else {
        njTranslateV(NULL, &current);
    }
}

#ifdef NONMATCHING
/* the plain form the hack tag names */
typedef Angle pose_angles[3];
#define ANGLES(angles) (angles)
#define X(angles) (angles)[0]
#define Y(angles) (angles)[1]
#define Z(angles) (angles)[2]
#else
/* @hack type: the angles as a struct angles, passed as an Angle pointer,
 * where an Angle[3] would do */
typedef struct angles pose_angles;
#define ANGLES(angles) ((Angle *)&(angles))
#define X(angles) (angles).x
#define Y(angles) (angles).y
#define Z(angles) (angles).z
#endif

#define SAMPLE_ROTATION(track, list, out)                                      \
    if (!(list)->p[1]) {                                                       \
        X(out) = node->object.ang[0];                                          \
        Y(out) = node->object.ang[1];                                          \
        Z(out) = node->object.ang[2];                                          \
    } else {                                                                   \
        FIND_KEYS(track, list, 1);                                             \
        garden_blend_angles(key->key, next->key, share, ANGLES(out));          \
    }

static void rotate_pose(struct chao_node *node)
{
    pose_angles current = {0, 0, 0};
    pose_angles next_rotation = {0, 0, 0};
    pose_angles blended;
    NJS_MDATA2 *current_keys = chao_pose.current.node;
    NJS_MDATA2 *next_keys = chao_pose.next.node;
    NJS_MKEY_A *key;
    NJS_MKEY_A *next;
#ifdef NONMATCHING
    /* the channel's first key, by which this side's search finds its last */
    NJS_MKEY_A *key_begin;
#endif
    Uint32 frame;
    Float share;
    SAMPLE_ROTATION(chao_pose.current, current_keys, current);
    if (next_keys) {
        SAMPLE_ROTATION(chao_pose.next, next_keys, next_rotation);
        garden_blend_angles(ANGLES(current), ANGLES(next_rotation),
                            chao_pose.share, ANGLES(blended));
        njRotateXYZ(NULL, X(blended), Y(blended), Z(blended));
    } else {
        njRotateXYZ(NULL, X(current), Y(current), Z(current));
    }
}

void chao_transform_node(struct chao_node *node)
{
    NJS_MDATA2 *next;
    chao_pose_cursor = chao_pose.current.node;
    next = chao_pose.next.node;
    translate_pose(node);
    rotate_pose(node);
    if (!(node->object.evalflags & NJD_EVAL_UNIT_SCL)) {
        njScale(NULL, node->object.scl[0], node->object.scl[1],
                node->object.scl[2]);
    }
    chao_pose_cursor++;
    chao_pose.current.node = chao_pose_cursor;
    if (next) {
        next++;
        chao_pose.next.node = next;
    }
}

#ifdef NONMATCHING
/* whether node and its siblings, with all below them, fit in remaining
 * frames of the matrix stack. each level of the tree takes one frame */
static Bool body_fits(const NJS_CNK_OBJECT *node, Sint32 remaining)
{
    if (remaining == 0) {
        return FALSE;
    }
    do {
        if (node->child && !body_fits(node->child, remaining - 1)) {
            return FALSE;
        }
        node = node->sibling;
    } while (node);
    return TRUE;
}

Bool chao_body_has_matrix_room(const NJS_CNK_OBJECT *root,
                               enum chao_matrix_frames extra)
{
    Sint32 remaining;

    if (!root || !chao_matrix_slot_available()) {
        return FALSE;
    }
    remaining = _nj_max_stack_size_ - _nj_stack_size_ - (Sint32)extra;
    if (remaining < 1) {
        return FALSE;
    }
    return remaining >= CHAO_BODY_NODES || body_fits(root, remaining);
}
#endif

static void build_joint_matrix(struct chao_node *node)
{
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)node->object.pos);
    njRotateXYZ(NULL, node->object.ang[0], node->object.ang[1],
                node->object.ang[2]);
    njGetMatrix(node->matrix);
    if (node->object.child) {
        build_joint_matrix(CHAO_NODE_CHILD(node));
    }
    njPopMatrixEx();
    if (node->object.sibling) {
        build_joint_matrix(CHAO_NODE_SIBLING(node));
    }
}

void chao_build_node_matrices(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
#ifdef NONMATCHING
    /* this side builds no matrices where Ninja's matrix stack has no room for
     * the tree */
    if (!chao_body_has_matrix_room((NJS_CNK_OBJECT *)work->shape.root,
                                   CHAO_BUILD_FRAMES)) {
        return;
    }
#endif
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->base.pos);
    njRotateY(NULL, work->base.ang.y);
    build_joint_matrix(work->shape.root);
    njPopMatrixEx();
}

#define READ_DIRECTION(axis, result)                                           \
    {                                                                          \
        njPushMatrixEx();                                                      \
        garden_clear_translation(NULL);                                        \
        njCalcVector(NULL, &(axis), &(result));                                \
        njPopMatrixEx();                                                       \
    }
#define FOOT_ANCHOR(foot, side)                                                \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, 0.1F, -0.95F, (side));                               \
        njGetTranslation(NULL, &(foot));                                       \
        njPopMatrixEx();                                                       \
        if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {                         \
            (foot).y = base->pos.y + GROUND_LIFT;                              \
        } else {                                                               \
            (foot).y += FOOT_RISE;                                             \
        }                                                                      \
    }

/* read the anchor at posing_node, if it is one, from the current matrix. the
 * icon, held objects, tears and shadows are placed by the anchors. every form
 * numbers its nodes alike */
static void read_body_anchor(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_icon *icon;
    switch (posing_node) {
    case CHAO_NODE_ICON_ANCHOR:
        /* @hack redundant: a read of shape.root, which nothing uses */
        (void)CHAO_WORK(chao)->shape.root;
        icon = &CHAO_WORK(chao)->icon;
        njGetTranslation(NULL, &icon->anchor);
        icon->anchor.y += njSin(icon->bob) * ICON_BOB;
        icon->bob += ICON_BOB_STEP;
        icon->bob &= ANGLE_MASK;
        break;
    case CHAO_NODE_HEAD_ANCHOR: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        struct chao_icon *head_icon = &CHAO_WORK(chao)->icon;
        NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
        READ_DIRECTION(up, head_icon->direction);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 2.0F, 0.0F);
        njGetTranslation(NULL, &shape->head_position);
        njPopMatrixEx();
        if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {
            shape->head_position.y = base->pos.y + GROUND_LIFT;
        }
    } break;
    case CHAO_NODE_MOUTH_ANCHOR: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        struct chao_icon *head_icon = &CHAO_WORK(chao)->icon;
        NJS_VECTOR forward = {0.0F, 0.0F, 1.0F};
        /* @hack redundant: head_icon, which nothing uses */
        (void)head_icon;
        READ_DIRECTION(forward, shape->mouth_direction);
        njGetTranslation(NULL, &shape->mouth_position);
    } break;
    case CHAO_NODE_EYE_ANCHOR_0: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        NJS_VECTOR outward = {1.0F, 0.0F, 0.0F};
        READ_DIRECTION(outward, shape->eyes[0].direction);
        njGetTranslation(NULL, &shape->eyes[0].position);
    } break;
    case CHAO_NODE_EYE_ANCHOR_1: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        NJS_VECTOR outward = {-1.0F, 0.0F, 0.0F};
        READ_DIRECTION(outward, shape->eyes[1].direction);
        njGetTranslation(NULL, &shape->eyes[1].position);
    } break;
    case CHAO_NODE_TRUNK: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        njGetTranslation(NULL, &shape->body_position);
        if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {
            shape->body_position.y = base->pos.y + GROUND_LIFT;
        }
    } break;
    case CHAO_NODE_HAND_4: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        njGetTranslation(NULL, &shape->hand_4);
    } break;
    case CHAO_NODE_HAND_11: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        njGetTranslation(NULL, &shape->hand_11);
    } break;
    case CHAO_NODE_FOOT_6: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        FOOT_ANCHOR(shape->foot_6, 0.5F);
    } break;
    case CHAO_NODE_FOOT_13: {
        struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
        FOOT_ANCHOR(shape->foot_13, -0.5F);
        break;
    }
    }
}

/* push a matrix that takes a point of the node to its move since node->matrix
 * was stored. the move is in the node's axes as they were then.
 * STORE_JOINT_MATRIX pops the matrix, and stores the node's transform now in
 * node->matrix */
#define PUSH_JOINT_DELTA()                                                     \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njGetMatrix(&chao_spring_delta);                                       \
        garden_subtract_matrix(&chao_spring_delta, node->matrix);              \
        garden_clear_translation(node->matrix);                                \
        njInvertMatrix(node->matrix);                                          \
        njPushMatrix(&_nj_unit_matrix_);                                       \
        njMultiMatrix(NULL, node->matrix);                                     \
        njMultiMatrix(NULL, &chao_spring_delta);                               \
    }
#define STORE_JOINT_MATRIX()                                                   \
    {                                                                          \
        njPopMatrixEx();                                                       \
        njPopMatrixEx();                                                       \
        njGetMatrix(node->matrix);                                             \
    }
#define LIMIT_DISPLACEMENT(value)                                              \
    if ((value) > upper) {                                                     \
        (value) = upper;                                                       \
    } else if ((value) < lower) {                                              \
        (value) = lower;                                                       \
    }

#define DEAD_TEST(value)                                                       \
    /* @hack redundant: a test of value that never runs */                     \
    if (0) {                                                                   \
        if (value) {                                                           \
        }                                                                      \
    }

static void move_pose_node(struct task *chao, struct chao_node *node)
{
    struct chao_mesh *mesh = (struct chao_mesh *)node->object.model;
    /* @hack redundant: a push that never runs */
    if (0) {
        njPushMatrixEx();
    }
    njPushMatrixEx();
    chao_transform_node(node);
    read_body_anchor(chao);
    if (node->animal_part || node->prop.object) {
        if (node->prop.object) {
            if (node->prop.fade_in) {
                node->prop.scale += CHAO_PROP_FADE;
                if (node->prop.scale > 1.0F) {
                    node->prop.scale = 1.0F;
                }
            } else {
                node->prop.scale -= CHAO_PROP_FADE;
                if (node->prop.scale < 0.0F) {
                    node->prop.scale = 0.0F;
                    node->prop.object = NULL;
                }
            }
        }
    } else {
        struct chao_work *work;
        /* @hack spelling: work assigned inside flags' initializer, where a
         * statement of its own would do */
        Uint32 flags = (work = CHAO_WORK(chao))->flags;
        if ((flags & CHAO_BODY_SPRINGS) && mesh &&
            (flags & CHAO_BODY_IN_VIEW)) {
            Float upper = SPRING_REACH;
            Float lower = -SPRING_REACH;
            if (node->swing) {
                NJS_POINT3 previous;
                PUSH_JOINT_DELTA();
                njCalcPoint(NULL, &node->rest_position, &previous);
                STORE_JOINT_MATRIX();
                {
                    NJS_VECTOR step;
                    Float factor;
                    step.x = node->velocity.x - previous.x;
                    step.y = node->velocity.y - previous.y;
                    step.z = node->velocity.z - previous.z;
                    LIMIT_DISPLACEMENT(step.x);
                    LIMIT_DISPLACEMENT(step.y);
                    LIMIT_DISPLACEMENT(step.z);
                    factor = node->weight;
                    /* @hack redundant: a test of factor that never runs */
                    DEAD_TEST(factor && factor);
                    node->object.pos[0] += node->velocity.x + (step.x * factor);
                    node->object.pos[1] += node->velocity.y + (step.y * factor);
                    node->object.pos[2] += node->velocity.z + (step.z * factor);
                    factor = node->gain;
                    step.x = node->object.pos[0] - node->rest_position.x;
                    step.y = node->object.pos[1] - node->rest_position.y;
                    step.z = node->object.pos[2] - node->rest_position.z;
                    node->velocity.x -= factor * step.x;
                    node->velocity.y -= factor * step.y;
                    node->velocity.z -= factor * step.z;
                    factor = node->damping;
                    node->velocity.x *= factor;
                    node->velocity.y *= factor;
                    node->velocity.z *= factor;
                }
            }
            if (!node->rigid) {
                /* @hack redundant: register on vertex, spring, rest, normal and
                 * the loop's factor, where plain locals would do */
                register struct chunk_vertex_vn *vertex;
                struct chunk_vertex_vn *end;
                register struct chao_vertex_spring *spring;
                register NJS_POINT3 *rest;
                register NJS_VECTOR *normal;
                NJS_POINT3 *previous;
                PUSH_JOINT_DELTA();
                njCalcPoints(NULL, mesh->rest_points, chao_spring_points,
                             mesh->vertex_count);
                STORE_JOINT_MATRIX();
                spring = mesh->springs;
                vertex = mesh->vertices;
                rest = mesh->rest_points;
                normal = mesh->rest_normals;
                previous = chao_spring_points;
                end = vertex + mesh->vertex_count;
                for (; vertex < end;
                     vertex++, spring++, previous++, rest++, normal++) {
                    Float restore_x;
                    Float vx;
                    Float vy;
                    Float vz;
                    Float x;
                    Float y;
                    Float z;
                    Float dx;
                    Float dy;
                    Float dz;
                    register Float factor;
                    x = vertex->point.x;
                    y = vertex->point.y;
                    z = vertex->point.z;
                    vx = spring->velocity.x;
                    vy = spring->velocity.y;
                    vz = spring->velocity.z;
                    /* @hack redundant: tests of y, z and previous that never
                     * run */
                    DEAD_TEST(y && y && y && z && z && previous);
                    factor = spring->weight;
                    dx = vx - previous->x;
                    dy = vy - previous->y;
                    dz = vz - previous->z;
                    /* @hack redundant: a test of dz that never runs */
                    DEAD_TEST(dz);
                    LIMIT_DISPLACEMENT(dx);
                    LIMIT_DISPLACEMENT(dy);
                    LIMIT_DISPLACEMENT(dz);
                    /* @hack spelling: dx carried in restore_x, where dx would
                     * do */
                    restore_x = dx;
                    x += vx + (restore_x * factor);
                    y += vy + (dy * factor);
                    z += vz + (dz * factor);
                    factor = spring->gain;
                    restore_x = x - rest->x;
                    dy = y - rest->y;
                    dz = z - rest->z;
                    vx -= restore_x * factor;
                    vy -= dy * factor;
                    vz -= dz * factor;
                    /* @hack redundant: tests of dy, restore_x and rest that
                     * never run */
                    DEAD_TEST(dy && restore_x && rest);
                    factor = spring->damping;
                    vx *= factor;
                    vy *= factor;
                    vz *= factor;
                    spring->velocity.x = vx;
                    spring->velocity.y = vy;
                    spring->velocity.z = vz;
                    switch (posing_node) {
                    case CHAO_NODE_EYE_0:
                    case CHAO_NODE_EYE_1:
                    case CHAO_NODE_LID_0:
                    case CHAO_NODE_LID_1:
                        break;
                    default:
                        vertex->point.x = x;
                        vertex->point.y = y;
                        vertex->point.z = z;
                    }
                    vertex->normal.x = normal->x + restore_x;
                    vertex->normal.y = normal->y + dy;
                    vertex->normal.z = normal->z + dz;
                }
            }
            /* @hack redundant: flags set to the vertex count, which nothing
             * reads */
            flags = mesh->vertex_count;
        }
        /* @hack redundant: flags read again, which nothing reads */
        flags = work->flags;
        (void)flags;
    }
    posing_node++;
    if (node->object.child) {
        move_pose_node(chao, CHAO_NODE_CHILD(node));
    }
    njPopMatrixEx();
    if (node->object.sibling) {
        move_pose_node(chao, CHAO_NODE_SIBLING(node));
    }
    /* @hack redundant: a pop that never runs */
    if (0) {
        njPopMatrixEx();
    }
}

/* @hack redundant: register on chao, where a plain parameter would do */
void chao_move_body_tree(register struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
#ifdef NONMATCHING
    /* this side poses no tree where Ninja's matrix stack has no room for it and
     * the pose's frames */
    if (!chao_body_has_matrix_room((NJS_CNK_OBJECT *)work->shape.root,
                                   CHAO_POSE_FRAMES)) {
        return;
    }
#endif
    posing_node = 0;
    chao_select_body_motion(chao);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->base.pos);
    njRotateY(NULL, work->base.ang.y);
    move_pose_node(chao, work->shape.root);
    njPopMatrixEx();
}
