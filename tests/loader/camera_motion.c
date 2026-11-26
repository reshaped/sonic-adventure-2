#include <assert.h>
#include "camera/mode_motion.c"

struct game_state game_state;
int camera_current_index;
static struct camera_place start;
struct camera_place *camera_views[CAMERAS] = {&start};
static struct camera_slot slot;
static int set_mode_calls, adjust_calls, expected_adjust;
static int top = CAMERA_MOTION_SLOT;

int camera_top(int camera)
{
    assert(camera == 0);
    return top;
}

/* TRUE where the motion's next step is its last, by camera_mode_motion's
 * own test */
static int last_step_next(void)
{
    struct motion_work *work = motion_works[0];
    Float next = work->frame + work->motion.speed;

    return next + work->motion.speed >
           (Float)(work->motion.motion->nbFrame - 1);
}

struct camera_slot *camera_set_mode(int camera, int index, int number)
{
    assert(camera == 0 && index == CAMERA_MOTION_SLOT &&
           number == CAMERA_MODE_MOTION);
    ++set_mode_calls;
    return &slot;
}

int camera_set_adjust(int camera, int index, int number)
{
    assert(camera == 0 && index == CAMERA_MOTION_SLOT &&
           number == expected_adjust);
    return ++adjust_calls;
}

int main(void)
{
    NJS_MOTION keys = {0};
    struct camera_motion preset = {0};
    NJS_POINT3 origin = {11, -5, 7};

    preset.motion = &keys;
    assert(camera_motion_start(0, &preset) == 0);
    assert(!set_mode_calls && !adjust_calls &&
           !game_state.camera_motion_playing);

    keys.nbFrame = 90;
    preset.flags = CAMERA_MOTION_START_BLENDS;
    preset.speed = 0.42F;
    preset.origin = &origin;
    expected_adjust = CAMERA_ADJUST_RELATIVE1;
    assert(camera_motion_start(0, &preset) == 1);
    assert(set_mode_calls == 1 && adjust_calls == 1 &&
           game_state.camera_motion_playing);
    assert(motion_works[0]->motion.motion == &keys);
    assert(motion_works[0]->motion.speed == preset.speed);
    assert(motion_works[0]->origin.y == origin.y);

    keys.nbFrame = 0;
    assert(camera_motion_start(0, &preset) == 0);
    assert(set_mode_calls == 1 && adjust_calls == 1 &&
           game_state.camera_motion_playing);

    /* the skip passes every step but the motion's last */
    keys.nbFrame = 90;
    motion_works[0]->frame = 10.0F;
    motion_works[0]->motion.speed = 1.0F;
    assert(!last_step_next());
    assert(camera_motion_skip(0) == 78 && motion_works[0]->frame == 88.0F);
    assert(last_step_next());
    assert(camera_motion_skip(0) == 0 && motion_works[0]->frame == 88.0F);
    motion_works[0]->frame = 0.0F;
    motion_works[0]->motion.speed = 0.42F;
    assert(camera_motion_skip(0) == 210 && last_step_next());
    /* it passes none under HOLD or LOOP, or below another mode */
    motion_works[0]->frame = 10.0F;
    motion_works[0]->motion.flags = CAMERA_MOTION_HOLD;
    assert(camera_motion_skip(0) == 0);
    motion_works[0]->motion.flags = CAMERA_MOTION_LOOP;
    assert(camera_motion_skip(0) == 0);
    motion_works[0]->motion.flags = 0;
    top = CAMERA_MOTION_SLOT + 1;
    assert(camera_motion_skip(0) == 0 && motion_works[0]->frame == 10.0F);
    return 0;
}
