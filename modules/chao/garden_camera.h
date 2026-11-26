/* the garden's camera: the body that garden_camera_create makes, and the
 * camera modes a garden runs */
#ifndef CHAO_GARDEN_CAMERA_H
#define CHAO_GARDEN_CAMERA_H

#include <ninja.h>
#include <sg_xpt.h>
#include "camera/stack.h"
#include "system/pad.h"
#include "system/task.h"

/* the body's work. the camera views from base.pos toward target */
struct garden_camera {
    struct task_work base;
    Uint16 type; /* camera_state's type: CAMERA_POS_TARGET */
    Uint8 unk_32[14];
    NJS_POINT3 eye; /* the point the body chases */
    Uint8 unk_4c[12];
    NJS_POINT3 target;
    Uint8 unk_64[12];
    Float distance; /* camera_state's dist */
    Uint8 unk_74[4];
    /* the target's height above the player and above what the player holds,
     * then the eye's above the player */
    Float target_height, carry_height, eye_height;
    Float follow; /* the share of its gap to the eye the body closes a step */
    Bool still; /* the body has stopped, and skips garden_meet_floor */
};

/* garden_camera_start and garden_camera_reset clear it */
extern Sint32 garden_camera_reset_flag;
/* all 0: where the camera views from while no body exists */
extern struct garden_camera garden_camera_fallback;

/* the editor's camera modes. the pad in port C flies the free camera. the
 * fixed camera holds where it stands */
void garden_free_camera(struct camera_stack *stack, struct camera_slot *slot);
void garden_fixed_camera(struct camera_stack *stack, struct camera_slot *slot);
/* a camera mode that orbits the player as the body does, without a body */
void garden_orbit_camera(struct camera_stack *stack, struct camera_slot *slot);

/* while player 0 holds button, turn vector about y by turn. the body and
 * garden_orbit_camera orbit the player so */
#define GARDEN_ORBIT_TRIGGER(button, turn, vector)                             \
    {                                                                          \
        if (player_pad_records[0].on & (button)) {                             \
            njPushMatrix(&_nj_unit_matrix_);                                   \
            njRotateY(NULL, (turn));                                           \
            njCalcVector(NULL, &(vector), &(vector));                          \
            njPopMatrixEx();                                                   \
        }                                                                      \
    }
#endif
