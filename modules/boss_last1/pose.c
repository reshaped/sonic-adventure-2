#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_last1/pose.h"
#include "boss_last1/model_art.h"
#include "boss_last1/motion_art.h"
#include "draw/motion.h"

#define MOTION_PAIRS 8

/* the node each joint lies on, and the joint's offset from the node's origin
 * along the node's axes. BIOLIZARD_JOINT_FRONT has no node */
static NJS_CNK_OBJECT *joint_nodes[BIOLIZARD_JOINTS] = {
#include "boss_last1/pose/joint_nodes.inc"
};

static NJS_POINT3 joint_offsets[BIOLIZARD_JOINTS] = {
#include "boss_last1/pose/joint_offsets.inc"
};

/* each motion that has a partner, and its partner */
static NJS_MOTION *motion_pairs[MOTION_PAIRS][2] = {
#include "boss_last1/pose/motion_pairs.inc"
};

NJS_POINT3 biolizard_joint_positions[BIOLIZARD_JOINTS];
static NJS_CNK_OBJECT *posed_root; /* the copy biolizard_pose_create made */
/* the walks of a tree number its nodes in node_index, from 0 at the root. a
 * node's children follow it, and its siblings follow them. joint_indices
 * holds the number of each joint's node, or 0 for a joint without a node */
static int node_index;
static int joint_indices[BIOLIZARD_JOINTS];
static NJS_MDATA2 *node_keys; /* the motion's keys for apply_motion's node */
/* the inverse of the matrix current at biolizard_pose_save_matrix, which
 * update_joints applies to each joint it finds. it lies on an eight-byte
 * boundary, as in the image */
#ifdef NONMATCHING
/* on the Dreamcast, this side's NJS_MATRIX type gives the boundary, as
 * nj_render_types.h says */
#else
#pragma aligndata8(view_to_world)
#endif
static NJS_MATRIX view_to_world;

static void index_joints(NJS_CNK_OBJECT *node)
{
    Sint8 joint;
    NJS_CNK_OBJECT **part = joint_nodes;
    int *index = joint_indices;

    for (joint = 0; joint < BIOLIZARD_JOINTS; joint++) {
        if (*part == node) {
            *index = node_index;
        }
        part++;
        index++;
    }
    node_index++;
    if (node->child) {
        index_joints(node->child);
    }
    if (node->sibling) {
        index_joints(node->sibling);
    }
}

static NJS_MOTION *paired_motion(NJS_MOTION *motion)
{
    Sint8 pair;

    for (pair = 0; pair < MOTION_PAIRS; pair++) {
        if (motion == motion_pairs[pair][0]) {
            return motion_pairs[pair][1];
        }
        if (motion == motion_pairs[pair][1]) {
            return motion_pairs[pair][0];
        }
    }
    return NULL;
}

static void free_tree(NJS_CNK_OBJECT *node);

static NJS_CNK_OBJECT *copy_tree(NJS_CNK_OBJECT *node)
{
    NJS_CNK_OBJECT *copy;
    NJS_CNK_OBJECT *branch;

    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *node;
    /* a motion can move or turn any node. no copy may skip its pos or ang as
     * a unit one */
    copy->evalflags &= ~NJD_EVAL_UNIT_ANG;
    copy->evalflags &= ~NJD_EVAL_UNIT_POS;
    if (node->child) {
        branch = copy_tree(node->child);
        if (branch) {
            copy->child = branch;
        } else {
            syFree(copy);
            return NULL;
        }
    }
    if (node->sibling) {
        branch = copy_tree(node->sibling);
        if (branch) {
            copy->sibling = branch;
        } else {
            /* @bug memory: leaks the child's copy where the sibling's copy
             * fails. the child's copy is kept at 0x8c502da6, and the failure
             * frees the node's copy alone at 0x8c502df6 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it frees the child's
             * copy */
            if (copy->child) {
                free_tree(copy->child);
            }
#endif
            syFree(copy);
            return NULL;
        }
    }
    return copy;
}

NJS_CNK_OBJECT *biolizard_pose_create(NJS_CNK_OBJECT *root)
{
    NJS_CNK_OBJECT *copy;
    if (!root) {
        return NULL;
    }
    node_index = 0;
    index_joints(root);
    /* @hack spelling: copy_tree's result passed through copy, where
     * posed_root could take it directly */
    copy = copy_tree(root);
    posed_root = copy;
    return posed_root;
}

static void free_tree(NJS_CNK_OBJECT *node)
{
    if (node) {
        if (node->child) {
            free_tree(node->child);
        }
        if (node->sibling) {
            free_tree(node->sibling);
        }
        syFree(node);
    }
}

void biolizard_pose_free(void)
{
    if (posed_root) {
        free_tree(posed_root);
#ifdef NONMATCHING
        /* clears posed_root, which the matching side leaves at the freed
         * copy */
        posed_root = NULL;
#endif
    }
}

static void apply_motion(NJS_CNK_OBJECT *node, NJS_CNK_OBJECT *pose)
{
    Sint8 axis;
    for (axis = 0; axis < 3; axis++) {
        pose->pos[axis] = node->pos[axis];
        pose->ang[axis] = node->ang[axis];
        pose->scl[axis] = node->scl[axis];
    }
    if (node_keys->p[0]) {
        njGetCnkMotionTranslate(pose->pos, 0);
    }
    if (node_keys->p[1]) {
        njGetCnkMotionRotate(pose->ang, 0);
    }
    njSetCnkNextMotionNode();
    node_keys++;
    node_index++;
    if (node->child && pose->child) {
        apply_motion(node->child, pose->child);
    }
    if (node->sibling) {
        apply_motion(node->sibling, pose->sibling);
    }
}

static void blend_poses(NJS_CNK_OBJECT *from, NJS_CNK_OBJECT *to,
                        NJS_CNK_OBJECT *out, Float rate)
{
    Float rest;
    Sint8 axis;
    if (!from || !to || !out) {
        return;
    }
    rest = 1.0F - rate;
    for (axis = 0; axis < 3; axis++) {
        out->pos[axis] = (from->pos[axis] * rest) + (to->pos[axis] * rate);
        out->ang[axis] = (from->ang[axis] * rest) + (to->ang[axis] * rate);
        out->scl[axis] = from->scl[axis];
    }
    if (from->child) {
        blend_poses(from->child, to->child, out->child, rate);
    }
    if (from->sibling) {
        blend_poses(from->sibling, to->sibling, out->sibling, rate);
    }
}

static void sample_pose(NJS_CNK_OBJECT *root, Float frame, NJS_MOTION *motion,
                        NJS_CNK_OBJECT *out)
{
    node_index = 0;
    node_keys = motion->mdata;
    njSetCnkCurrentMotion(motion, frame);
    apply_motion(root, out);
}

void biolizard_pose_save_matrix(void)
{
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
}

static void update_joints(NJS_CNK_OBJECT *node)
{
    NJS_POINT3 transformed;
    NJS_POINT3 origin = {0, 0, 0};
    NJS_POINT3 point;
    Uint16 joint;
    int *indices;
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)node->pos);
    njRotateEx(node->ang, 0);
    njScaleEx((NJS_VECTOR *)node->scl);
    /* the life support lies at its node's model's center. node 0, the root,
     * places no joint: BIOLIZARD_JOINT_FRONT's index is 0, and body.c places
     * it */
    if (node_index == joint_indices[BIOLIZARD_JOINT_LIFE_SUPPORT]) {
        point = node->model->center;
        njCalcPoint(NULL, &point, &transformed);
        njCalcPoint(&view_to_world, &transformed,
                    &biolizard_joint_positions[BIOLIZARD_JOINT_LIFE_SUPPORT]);
    } else if (node_index != 0) {
        NJS_POINT3 *out;
        NJS_POINT3 *offset;
        indices = joint_indices;
        out = biolizard_joint_positions;
        offset = joint_offsets;
        for (joint = 0; joint < BIOLIZARD_JOINTS; joint++) {
            if (node_index == *indices) {
                if (njScalor(offset) != 0.0F) {
                    njPushMatrixEx();
                    njTranslateEx(offset);
                    njCalcPoint(NULL, &origin, &transformed);
                    njCalcPoint(&view_to_world, &transformed, out);
                    njPopMatrixEx();
                } else {
                    njCalcPoint(NULL, &origin, &transformed);
                    njCalcPoint(&view_to_world, &transformed, out);
                }
            }
            out++;
            offset++;
            indices++;
        }
    }
    node_index++;
    if (node->child) {
        update_joints(node->child);
    }
    njPopMatrixEx();
    if (node->sibling) {
        update_joints(node->sibling);
    }
}

/* pose target by play's motion at its frame. where the motion has a partner,
 * the pose blends toward it by rate. the macro sets partner, and poses holds
 * the two poses it blends */
#define POSE_PLAY(root, play, partner, poses, target, rate)                    \
    do {                                                                       \
        (partner) = paired_motion((play)->motion);                             \
        if ((partner)) {                                                       \
            (poses)[0] = copy_tree((root));                                    \
            (poses)[1] = copy_tree((root));                                    \
            sample_pose((root), (play)->frame, (play)->motion, (poses)[0]);    \
            sample_pose((root), (play)->frame, (partner), (poses)[1]);         \
            blend_poses((poses)[0], (poses)[1], (target), (rate));             \
            free_tree((poses)[0]);                                             \
            free_tree((poses)[1]);                                             \
        } else {                                                               \
            sample_pose((root), (play)->frame, (play)->motion, (target));      \
        }                                                                      \
    } while (0)

void biolizard_pose_draw(NJS_CNK_OBJECT *root, struct motion_control *control,
                         Float rate)
{
    NJS_MOTION *partner;
    struct motion_play *play = &control->current;

    if (control->flags & MOTION_LINKING) {
        NJS_CNK_OBJECT *copies[2];
        NJS_CNK_OBJECT *poses[2];
        copies[0] = copy_tree(root);
        copies[1] = copy_tree(root);
        POSE_PLAY(root, play, partner, poses, copies[0], rate);
        play = &control->next_play;
        POSE_PLAY(root, play, partner, poses, copies[1], rate);
        blend_poses(copies[0], copies[1], posed_root, control->link);
        free_tree(copies[0]);
        free_tree(copies[1]);
    } else {
        NJS_CNK_OBJECT *poses[2];
        partner = paired_motion(play->motion);
        if (partner) {
            NJS_CNK_OBJECT **second;
            poses[0] = copy_tree(root);
            /* @hack spelling: second, a cursor to poses[1], where poses[1]
             * itself would do */
            second = &poses[1];
            *second = copy_tree(root);
            sample_pose(root, play->frame, play->motion, poses[0]);
            sample_pose(root, play->frame, partner, *second);
            blend_poses(poses[0], *second, posed_root, rate);
            free_tree(poses[0]);
            free_tree(*second);
        } else {
            sample_pose(root, play->frame, play->motion, posed_root);
        }
    }
    njCnkSimpleDrawObject(posed_root);
    node_index = 0;
    update_joints(posed_root);
}
