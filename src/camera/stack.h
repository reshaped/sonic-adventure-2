/* the cameras' stacks. each camera keeps a stack of the modes that move it,
 * each paired with an adjust. it runs the mode on top */
#ifndef CAMERA_STACK_H
#define CAMERA_STACK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/probe.h"
#include "system/angle.h"

struct camera_zone;

/* the numbers of the modes, each named as modes.c's camera_modes spells
 * it */
#define CAMERA_MODE_NONE 0
#define CAMERA_MODE_USER 1
#define CAMERA_MODE_FOLLOW 2
#define CAMERA_MODE_KNUKLES 3
#define CAMERA_MODE_EDITOR 4
#define CAMERA_MODE_EDITOR2 5
#define CAMERA_MODE_SNAPSHOT 6
#define CAMERA_MODE_KLAMATH 7
#define CAMERA_MODE_POINT 8
#define CAMERA_MODE_ASHLAND 9
#define CAMERA_MODE_FIX 10
#define CAMERA_MODE_LEAVE 11
#define CAMERA_MODE_SPACE 12
#define CAMERA_MODE_CARMEL 13
#define CAMERA_MODE_MOTION 14
#define CAMERA_MODE_BOSSINIT 15
#define CAMERA_MODE_BOSSPOINT 16
#define CAMERA_MODE_COLLISION 17
#define CAMERA_MODE_PSTONE 18
#define CAMERA_MODE_INIT 19
#define CAMERA_MODE_EASYSET 20
#define CAMERA_MODE_BOSSKLAMATH 21
#define CAMERA_MODE_GAKUGAKU 22
#define CAMERA_MODE_KNUCKLES_L 23
#define CAMERA_MODE_FIX2 24
#define CAMERA_MODE_PSTONE2 25
#define CAMERA_MODE_SS 26
#define CAMERA_MODE_COLLI_LR 27
#define CAMERA_MODES 28

/* the numbers of the adjusts, each named as modes.c's camera_adjusts spells
 * it. each from THREE1 to RELATIVE6 has camera_blend for its step */
#define CAMERA_ADJUST_NONE 0
#define CAMERA_ADJUST_USER 1
#define CAMERA_ADJUST_HALF 2
#define CAMERA_ADJUST_THREE1 3
#define CAMERA_ADJUST_THREE2 4
#define CAMERA_ADJUST_THREE3 5
#define CAMERA_ADJUST_THREE5 7
#define CAMERA_ADJUST_RELATIVE1 8
#define CAMERA_ADJUST_RELATIVE2 9
#define CAMERA_ADJUST_RELATIVE3 10
#define CAMERA_ADJUST_RELATIVE4 11
#define CAMERA_ADJUST_RELATIVE5 12
#define CAMERA_ADJUST_RELATIVE6 13
#define CAMERA_ADJUSTS 14

struct camera_slot;
struct camera_stack;

/* the step of a mode or of an adjust, which runs with its camera's stack
 * and its own slot */
typedef void camera_step(struct camera_stack *stack, struct camera_slot *slot);

/* a slot of a camera's stack: a mode, or the adjust paired with it */
struct camera_slot {
    /* a CAMERA_MODE_ number in modes, or a CAMERA_ADJUST_ one in adjusts */
    Sint32 number;
    /* the ZONE_ shape of the zone that put the mode there, or 0 */
    Sint32 shape;
    Sint32 steps; /* the times step has run */
    camera_step *step;
    /* the zone camera_set_mode_zone keeps with a mode, or null */
    struct camera_zone *zone;
    Uint8 work[0x100]; /* for the step to keep its own state in */
};

/* the most modes a camera's stack holds. each mode pairs with the adjust at
 * its index */
#define CAMERA_STACK_DEPTH 16

/* the states of a camera's shake. camera_shake starts one at FULL. the
 * camera task moves one that runs out to FADING, then to NONE */
#define CAMERA_SHAKE_NONE 0
#define CAMERA_SHAKE_FULL 1
#define CAMERA_SHAKE_FADING 2

/* a camera's stack of modes, each with an adjust, and the state it keeps
 * between steps */
struct camera_stack {
    /* the camera's angle of view, the angle it heads for, and the most it
     * turns in a step of the camera task */
    Angle view;
    Angle view_target;
    Angle view_turn;
    /* TRUE from camera_run_mode's push of CAMERA_MODE_KNUKLES for a trigger
     * until it takes the mode then on top off. a push on a full stack sets it
     * too, though no mode went on */
    Bool trigger_pushed;
    /* 1 where camera_step_standing last found the camera's player still on
     * the ground, else 0 */
    Sint32 standing;
    Sint32 unk_014;
    struct camera_probe probe; /* camera_collide's probe */
    /* the offset added to place.pos as the camera is given to Ninja */
    NJS_POINT3 shake;
    Sint32 shake_state; /* a CAMERA_SHAKE_ state */
    /* camera_shake's steps, which the camera task counts down, and its
     * size */
    Sint32 shake_steps;
    Float shake_size;
    /* what the camera task gives Ninja for this camera: place, with shake
     * added to its pos, and view */
    NJS_CAMERA camera;
    /* the camera's place, where camera_blend's blends start.
     * camera_views[camera] points to it */
    struct camera_place place;
    struct camera_place place_before; /* place as the step before left it */
    /* this step's place as the mode and the adjust gave it, before
     * camera_collide */
    struct camera_place unclamped;
    Sint32 top; /* the index of the mode on top */
    struct camera_slot modes[CAMERA_STACK_DEPTH];
    struct camera_slot adjusts[CAMERA_STACK_DEPTH];
};
extern struct camera_stack camera_stacks[CAMERAS];

/* have camera's angle of view head for view by at most turn each step of
 * the camera task. a turn of 0 or less, such as
 * CAMERA_VIEW_TURN_USE_DEFAULT, stands for a quarter turn */
void camera_set_view_angle(int camera, Angle view, Angle turn);
#define CAMERA_VIEW_TURN_USE_DEFAULT (-1)
/* camera's angle of view */
Angle camera_view_angle(int camera);
/* the index of the mode on top of camera's stack */
int camera_top(int camera);
/* the number of camera's mode at index, and that of the adjust paired with
 * it */
int camera_mode_at(int camera, int index);
int camera_adjust_at(int camera, int index);
/* put a mode of number at index of camera's stack, and return its slot.
 * the mode has no shape and no zone, its steps are 0 and its work is zeroed.
 * a mode put above the top becomes the top, with a copy of the old top's
 * adjust. for an index of CAMERA_STACK_DEPTH or more it returns null and
 * changes nothing.
 *
 * camera_push_mode puts the mode one above the top. camera_set_mode_with_shape
 * keeps shape with the mode, for camera_mode_shape to return, as
 * camera_zones_update keeps a zone's ZONE_ shape */
struct camera_slot *camera_set_mode(int camera, int index, int number);
struct camera_slot *camera_push_mode(int camera, int number);
struct camera_slot *camera_set_mode_with_shape(int camera, int index,
                                               int number, int shape);
/* camera_set_mode for a mode of the caller's: CAMERA_MODE_USER, with step
 * for its step. camera_push_user_mode puts it one above the top */
struct camera_slot *camera_set_user_mode(int camera, int index,
                                         camera_step *step);
struct camera_slot *camera_push_user_mode(int camera, camera_step *step);
/* give camera's mode at index the adjust number, and return 1. the adjust's
 * steps are 0 and its work is zeroed. where index lies above the mode on
 * top, it returns 0 and changes nothing */
int camera_set_adjust(int camera, int index, int number);
/* take camera's mode at index off its stack, leaving CAMERA_MODE_NONE in its
 * slot. the mode at index 0 never comes off. taking off the top moves it
 * down to the next mode below that isn't CAMERA_MODE_NONE, or to 0. that
 * mode and its adjust then count their steps from 0 again */
void camera_remove_mode(int camera, int index);
/* the shape kept with camera's mode at index */
int camera_mode_shape(int camera, int index);
/* keep zone with camera's mode at index, for camera_mode_zone to return */
void camera_set_mode_zone(int camera, int index, struct camera_zone *zone);
struct camera_zone *camera_mode_zone(int camera, int index);

/* make *shake camera's shake */
void camera_set_shake(int camera, NJS_POINT3 *shake);
/* start camera shaking. each step of the camera task then sets the shake at
 * random. each of its x, y and z lies within size / 2 of 0. it shakes at size
 * for steps + 1 steps. it then shrinks to nothing over 60 steps more, and
 * stops. a negative steps keeps it at size until camera_stop_shake */
void camera_shake(int camera, int steps, Float size);
/* zero camera's shake and stop its shaking */
void camera_stop_shake(int camera);
/* put camera at pos, looking at its player's target: the player's pos
 * raised by its target_height. camera_state and camera's place take that
 * pos and target, with no roll. camera_state's unk_04 and trigger_push
 * become 0. the 8 latest places of the player's past are set to the target.
 * the mode on top becomes CAMERA_MODE_KNUKLES, with CAMERA_ADJUST_NONE.
 * camera's player must have its work and its record */
void camera_put_at(int camera, NJS_POINT3 *pos);

/* zero every camera's stack */
void camera_clear_stacks(void);
/* where the adjust paired with the mode on top of camera's stack has a step,
 * run it. count the run in the adjust's steps */
void camera_run_adjust(int camera);
/* where the mode on top of camera's stack has a step, run it. count the run in
 * the mode's steps. after a step, camera's angle of view heads for 70 degrees,
 * a quarter turn a step at most. it doesn't where the mode then on top is
 * CAMERA_MODE_ASHLAND. camera must be camera_current_index: the pad and trigger
 * tests read that in its place.
 *
 * first it may push CAMERA_MODE_GAKUGAKU with CAMERA_ADJUST_NONE, whose step
 * then runs instead. the mode on top must be CAMERA_MODE_KNUKLES, KLAMATH,
 * POINT or CARMEL, and the stage below STAGE_STG_END. the camera's pad must
 * hold neither PDD_DGT_TL nor PDD_DGT_TR, and camera_settle_step(camera)
 * must return nonzero, which it asks last. on a full stack the mode on top
 * stays, and takes CAMERA_ADJUST_NONE.
 *
 * then, whether or not a step ran, it reads the triggers. while the stack's
 * trigger_pushed is FALSE, a trigger whose reading isn't 0 pushes
 * CAMERA_MODE_KNUKLES with CAMERA_ADJUST_RELATIVE1. the stack's standing must
 * be 1, and camera_state's trigger_push not 0. the camera's pad must be
 * enabled, and camera_steps above 300. while trigger_pushed is TRUE, a standing
 * of 0 takes the mode on top off. the new top takes CAMERA_ADJUST_RELATIVE1. a
 * push on a full stack only gives the mode on top RELATIVE1. that mode comes
 * off in its turn */
void camera_run_mode(int camera);

#endif
