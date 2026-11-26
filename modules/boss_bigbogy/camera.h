/* King Boom Boo's views on camera 0: the opening shots, a move and a look */
#ifndef BOSS_BIGBOGY_CAMERA_H
#define BOSS_BIGBOGY_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/bossinit.h"

struct task;
/* the view moves from pos_from to pos_to. the point it looks at moves from
 * target_from to target_to */
struct bigbogy_camera_path {
    NJS_POINT3 target_from;
    NJS_POINT3 pos_from;
    NJS_POINT3 target_to;
    NJS_POINT3 pos_to;
};
/* the opening shots. camera 0 runs opening's BOSSINIT mode. it cuts away count
 * times to a fixed view of target, each time coming back. the opening holds for
 * BIGBOGY_OPENING_STEPS + 1 steps before the first cutaway. each cutaway lasts
 * shot_frames + 1 steps, and the hold after it hold_frames + 1 */
struct bigbogy_camera_shots {
    Uint8 count;
    Uint16 hold_frames;
    Uint16 shot_frames;
    /* the fixed view looks at target as it lies at the call. it stands
     * BIGBOGY_SHOT_DISTANCE from target in x and z, along heading, and
     * BIGBOGY_SHOT_RISE above the floor */
    NJS_POINT3 *target;
    Angle heading;
    struct camera_bossinit opening;
};
#define BIGBOGY_OPENING_STEPS 240
#define BIGBOGY_SHOT_DISTANCE 150.0F
#define BIGBOGY_SHOT_RISE 30.0F

/* push CAMERA_MODE_EASYSET onto camera 0's stack, with its view at spec's
 * start. the old top and the new mode take CAMERA_ADJUST_NONE. the call returns
 * a child task of parent that keeps a copy of spec. the view waits at the start
 * until bigbogy_camera_move. as the task ends, camera 0's top mode takes
 * CAMERA_ADJUST_THREE3 and the mode below it CAMERA_ADJUST_THREE2. the top
 * mode then comes off.
 *
 * parent must not be null, or the call returns an unset pointer. the call
 * allocates the task's motion record, then the copy of spec. where the first
 * allocation fails, the call kills the task and returns null. where the second
 * fails, it kills the task, frees the motion record twice and returns an unset
 * pointer */
struct task *bigbogy_camera_path_create(struct task *parent,
                                        const struct bigbogy_camera_path *spec);
/* move the view of task, from bigbogy_camera_path_create, evenly from its
 * path's start to its end over frames steps. it then holds at the end */
void bigbogy_camera_move(struct task *task, Uint16 frames);
/* push spec's opening onto camera 0's stack, and run spec's cutaways. the
 * call returns a child task of parent that keeps a copy of spec. each
 * cutaway pushes CAMERA_MODE_EASYSET, and takes it off as it ends. the top
 * mode takes CAMERA_ADJUST_NONE before each push and each removal. as the
 * task ends, it ends camera 0's top mode where that is a BOSSINIT mode.
 * spec->target and spec->opening.target must outlive the task.
 *
 * parent must not be null, or the call returns an unset pointer. the call
 * allocates the task's motion record, then the copy of spec. where either
 * allocation fails, the call kills the task and returns an unset pointer.
 * where the second fails, it frees the motion record twice too */
struct task *bigbogy_camera_shots_create(struct task *parent,
                                         struct bigbogy_camera_shots *spec);
#define BIGBOGY_LOOK_RISE 5.0F
/* set the adjust of camera 0's top mode to adjust, a CAMERA_ADJUST_ number.
 * then push a mode above it, which takes the same adjust. camera 0's stack
 * must have room for it. the mode's view stands at pos, and looks
 * BIGBOGY_LOOK_RISE above target, which it reads at each step. target must
 * outlive the mode. bigbogy_camera_pop sets the top mode's adjust to adjust,
 * then takes the top mode off */
void bigbogy_camera_look(NJS_POINT3 *target, const NJS_POINT3 *pos, int adjust);
void bigbogy_camera_pop(int adjust);
#endif
