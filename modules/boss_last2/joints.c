#include <ninja.h>
#include <sg_xpt.h>
#include "boss_last2/joints.h"
#include "boss_last2/body_art.h"

NJS_POINT3 finalhazard_joint_positions[FINALHAZARD_JOINTS];
/* @hack redundant: a word that nothing reads, which places
 * finalhazard_panel_bases after it as the image does */
static Uint32 unused_before_bases;

/* panels.h declares finalhazard_panel_bases and finalhazard_joint_inverse.
 * it comes after finalhazard_joint_positions and unused_before_bases, so
 * that SHC places the two after them */
#include "boss_last2/panels.h"

/* a joint, at offset from the origin of object, a node of the model */
struct attachment {
    /* the joint's entry of finalhazard_joint_positions, which nothing
     * reads */
    NJS_POINT3 *position;
    NJS_VECTOR offset;
    NJS_CNK_OBJECT *object;
};

NJS_MATRIX finalhazard_panel_bases[FINALHAZARD_PANEL_BASES];
NJS_MATRIX finalhazard_joint_inverse;

/* the joints, by index. the first FINALHAZARD_PANEL_BASES lie at the nodes
 * whose matrices finalhazard_panel_bases keeps, in its order */
/* clang-format off */
static struct attachment attachments[FINALHAZARD_JOINTS] = {
    {&finalhazard_joint_positions[0], {0, 0, 0}, &finalhazard_node_7},
    {&finalhazard_joint_positions[1], {0, 0, 0}, &finalhazard_node_5},
    {&finalhazard_joint_positions[2], {0, 0, 0}, &finalhazard_node_4},
    {&finalhazard_joint_positions[3], {0, 0, 0}, &finalhazard_node_3},
    {&finalhazard_joint_positions[4], {0, 0, 0}, &finalhazard_node_14},
    {&finalhazard_joint_positions[5], {0, 0, 0}, &finalhazard_node_42},
    {&finalhazard_joint_positions[6], {0, 0, 0}, &finalhazard_node_44},
    {&finalhazard_joint_positions[7], {5, 0, 0}, &finalhazard_node_33},
    {&finalhazard_joint_positions[8], {25, 0, 0}, &finalhazard_node_33},
    {&finalhazard_joint_positions[9], {0, 0, 0}, &finalhazard_node_28},
    {&finalhazard_joint_positions[10], {0, 0, 0}, &finalhazard_node_29},
    {&finalhazard_joint_positions[11], {0, 0, 0}, &finalhazard_node_30},
    {&finalhazard_joint_positions[12], {55, 0, 0}, &finalhazard_node_4},
    {&finalhazard_joint_positions[13], {0, 0, 0}, &finalhazard_node_5},
    {&finalhazard_joint_positions[14], {0, 0, 0}, &finalhazard_node_6},
    {&finalhazard_joint_positions[15], {0, 0, 0}, &finalhazard_node_7},
    {&finalhazard_joint_positions[16], {0, 0, 0}, &finalhazard_node_8},
    {&finalhazard_joint_positions[17], {20, 0, 0}, &finalhazard_node_8},
    {&finalhazard_joint_positions[18], {0, 0, 0}, &finalhazard_node_10},
    {&finalhazard_joint_positions[19], {0, 0, 0}, &finalhazard_node_14},
    {&finalhazard_joint_positions[20], {0, 0, 0}, &finalhazard_node_15},
    {&finalhazard_joint_positions[21], {45, 0, 0}, &finalhazard_node_15},
    {&finalhazard_joint_positions[22], {30, 0, 0}, &finalhazard_node_18},
    {&finalhazard_joint_positions[23], {20, 0, 0}, &finalhazard_node_25},
    {&finalhazard_joint_positions[24], {0, 0, 0}, &finalhazard_node_21},
    {&finalhazard_joint_positions[25], {0, 0, 0}, &finalhazard_node_22},
    {&finalhazard_joint_positions[26], {0, 162.13898F, 0}, &finalhazard_node_41},
};
/* clang-format on */

static void track_joints(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 transformed;
    NJS_POINT3 origin = {0, 0, 0};
    struct attachment *attachment;
    NJS_VECTOR *offset;
    NJS_POINT3 *joint;
    Uint16 i;
    /* @hack redundant: the root skipped, which no attachment names, where
     * the loop alone would do */
    if (object != &finalhazard_root) {
        /* @hack spelling: the address of attachments[0], where attachments
         * would do */
        attachment = &attachments[0];
        /* @hack spelling: joint stepped through finalhazard_joint_positions
         * beside attachment, where attachment->position would do */
        joint = finalhazard_joint_positions;
        for (i = 0; i < FINALHAZARD_JOINTS; i++) {
            if (object == attachment->object) {
                offset = &attachment->offset;
                if (njScalor(offset) != 0.0F) {
                    njPushMatrixEx();
                    njTranslateEx(offset);
                    njCalcPoint(NULL, &origin, &transformed);
                    njCalcPoint(&finalhazard_joint_inverse, &transformed,
                                joint);
                    njPopMatrixEx();
                } else {
                    njCalcPoint(NULL, &origin, &transformed);
                    njCalcPoint(&finalhazard_joint_inverse, &transformed,
                                joint);
                }
                /* @hack spelling: the first three tests joined by else, where
                 * five tests of their own would do: the first five
                 * attachments' nodes differ */
                if (object == attachments[0].object) {
                    njGetMatrix(&finalhazard_panel_bases[0]);
                } else if (object == attachments[1].object) {
                    njGetMatrix(&finalhazard_panel_bases[1]);
                } else if (object == attachments[2].object) {
                    njGetMatrix(&finalhazard_panel_bases[2]);
                }
                if (object == attachments[3].object) {
                    njGetMatrix(&finalhazard_panel_bases[3]);
                }
                if (object == attachments[4].object) {
                    njGetMatrix(&finalhazard_panel_bases[4]);
                }
            }
            joint++;
            attachment++;
        }
    }
}

void finalhazard_bind_joints(void)
{
    njGetMatrix(&finalhazard_joint_inverse);
    njInvertMatrix(&finalhazard_joint_inverse);
    njCnkSetMotionCallback(track_joints);
}
