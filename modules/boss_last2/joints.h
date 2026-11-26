/* the Finalhazard's joints: points of its model that its shapes, sounds and
 * lasers follow */
#ifndef BOSS_LAST2_JOINTS_H
#define BOSS_LAST2_JOINTS_H
#include <ninja.h>

#define FINALHAZARD_JOINTS 27
/* the joints the code names. the lasers start at the two cannons, and the
 * breath sounds at FINALHAZARD_JOINT_VOICE */
enum finalhazard_joint {
    FINALHAZARD_JOINT_CANNON_A = 5,
    FINALHAZARD_JOINT_CANNON_B = 6,
    FINALHAZARD_JOINT_VOICE = 18
};
/* each joint's place as the model last drew, with the matrix current at
 * finalhazard_bind_joints undone. finalhazard_draw binds under the view's
 * matrix, so the places are the world's */
extern NJS_POINT3 finalhazard_joint_positions[FINALHAZARD_JOINTS];
/* keep the inverse of the current matrix in finalhazard_joint_inverse, and
 * set the chunk motion callback. as the model then draws, the callback sets
 * finalhazard_joint_positions. it also keeps the matrices at the first
 * FINALHAZARD_PANEL_BASES joints' nodes in finalhazard_panel_bases. the
 * caller must clear the callback after the draw */
void finalhazard_bind_joints(void);
#endif
