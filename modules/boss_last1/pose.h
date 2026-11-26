/* the Biolizard's pose: a copy of its model that its motions pose, and the
 * joints the pose places */
#ifndef BOSS_LAST1_POSE_H
#define BOSS_LAST1_POSE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last1/motion_art.h"

struct motion_control;

#define BIOLIZARD_JOINTS 32
/* the indexes of biolizard_joint_positions that the Biolizard's sources
 * name. BIOLIZARD_JOINT_HEAD, BIOLIZARD_JOINT_HEAD_MIDDLE and
 * BIOLIZARD_JOINT_MOUTH lie on the head's node, from its origin out */
enum biolizard_joint {
    BIOLIZARD_JOINT_HEAD = 0,
    BIOLIZARD_JOINT_TAIL = 1,
    BIOLIZARD_JOINT_LIFE_SUPPORT = 2,
    /* the legs' joints, whose splashes body.c tracks */
    BIOLIZARD_JOINT_LEG_A = 3,
    BIOLIZARD_JOINT_LEG_B = 4,
    BIOLIZARD_JOINT_FRONT = 5, /* body.c places it, not the pose */
    /* where the head's shape lies, and the breath and the volleys start */
    BIOLIZARD_JOINT_HEAD_MIDDLE = 6,
    BIOLIZARD_JOINT_MOUTH = 7,
    /* the joint of the last of the tail's shapes */
    BIOLIZARD_JOINT_TAIL_TIP = 23
};
/* each joint where the last biolizard_pose_draw placed it */
extern NJS_POINT3 biolizard_joint_positions[BIOLIZARD_JOINTS];

/* copy root's tree for the pose, and find its joints. the copy shares root's
 * models. returns the copy, or null for a null root or where the heap can't
 * give the copy. the calls below need the copy */
NJS_CNK_OBJECT *biolizard_pose_create(NJS_CNK_OBJECT *root);
/* free the copy. call it once, after the last draw */
void biolizard_pose_free(void);
/* save the inverse of the current matrix. the next draw places the joints in
 * the space that matrix takes points from. under the camera's view matrix
 * alone, that space is the world */
void biolizard_pose_save_matrix(void);
/* pose the copy by control's motion at its frame. while MOTION_LINKING is
 * set, pose it by both of control's motions, blended by its link. then draw
 * the copy under the current matrix, and place the joints. root must be the
 * tree biolizard_pose_create copied.
 *
 * a motion with a partner blends toward it by rate. at a rate of 0 the pose
 * is the motion's alone, and at 1 the partner's. the heap must give the draw's
 * copies of root, unchecked */
void biolizard_pose_draw(NJS_CNK_OBJECT *root, struct motion_control *control,
                         Float rate);
#endif
