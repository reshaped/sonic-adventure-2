/* the tables of the camera's modes and adjusts, and the calls on a camera's
 * stack */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/blend.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"

/* camera_put_at sets its player's past pos, from back 0 to PAST_PLACES - 1,
 * to the camera's new target */
#define PAST_PLACES 8

/* copy camera's adjust at from over its adjust at to, and return 1. where
 * either lies above the mode on top, it returns 0 and copies nothing */
static int camera_copy_adjust(int camera, int from, int to);

/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
int camera_mode_below_top(int camera);
/* the step of mode number, and of adjust number, or null for one without a
 * step. neither checks number against its table */
camera_step *camera_mode_step(int number);
camera_step *camera_adjust_step(int number);
/* NOLINTEND(misc-use-internal-linkage) */

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* camera_set_adjust for an adjust of the caller's: CAMERA_ADJUST_USER, with
 * step for its step */
int camera_set_user_adjust(int camera, int index, camera_step *step);
/* the name of camera's mode at index, and of the adjust paired with it */
const char *camera_mode_name(int camera, int index);
const char *camera_adjust_name(int camera, int index);
/* keep shape with camera's mode at index, for camera_mode_shape to return */
void camera_set_mode_shape(int camera, int index, int shape);
/* NOLINTEND(misc-use-internal-linkage) */

struct camera_mode {
    const char *name;
    camera_step *step;
    Sint32 unk_08; /* no code reads it */
};

struct camera_adjust {
    const char *name;
    camera_step *step;
};

/* the modes, by their CAMERA_MODE_ numbers. camera_set_user_mode gives USER
 * its step. COLLISION and COLLI_LR name zones that camera_zones_collide acts
 * on */
static const struct camera_mode camera_modes[CAMERA_MODES] = {
    {"NONE", NULL, 0},
    {"USER", NULL, 0},
    {"FOLLOW", camera_mode_follow, 1},
    {"KNUKLES", camera_mode_knukles, 1},
    {"EDITOR", camera_mode_editor, 0},
    {"EDITOR2", camera_mode_editor2, 0},
    {"SNAPSHOT", camera_mode_snapshot, 1},
    {"KLAMATH", camera_mode_klamath, 0},
    {"POINT", camera_mode_point, 0},
    {"ASHLAND", camera_mode_ashland, 0},
    {"FIX", camera_mode_fix, 0},
    {"LEAVE", camera_mode_leave, 0},
    {"SPACE", camera_mode_space, 1},
    {"CARMEL", camera_mode_carmel, 1},
    {"MOTION", camera_mode_motion, 0},
    {"BOSSINIT", camera_mode_bossinit, 0},
    {"BOSSPOINT", camera_mode_bosspoint, 0},
    {"COLLISION", NULL, 0},
    {"PSTONE", camera_mode_pstone, 0},
    {"INIT", camera_mode_init, 0},
    {"EASYSET", camera_mode_easyset, 0},
    {"BOSSKLAMATH", camera_mode_bossklamath, 0},
    {"GAKUGAKU", camera_mode_gakugaku, 0},
    {"KNUCKLES_L", camera_mode_knuckles_l, 1},
    {"FIX2", camera_mode_fix2, 0},
    {"PSTONE2", camera_mode_pstone2, 0},
    {"SS", camera_mode_ss, 0},
    {"COLLI_LR", NULL, 0},
};

/* the adjusts, by their CAMERA_ADJUST_ numbers. camera_set_user_adjust gives
 * USER its step */
static const struct camera_adjust camera_adjusts[CAMERA_ADJUSTS] = {
    {"NONE", NULL},
    {"USER", NULL},
    {"HALF", camera_adjust_half},
    {"THREE1", camera_blend},
    {"THREE2", camera_blend},
    {"THREE3", camera_blend},
    {"THREE4", camera_blend},
    {"THREE5", camera_blend},
    {"RELATIVE1", camera_blend},
    {"RELATIVE2", camera_blend},
    {"RELATIVE3", camera_blend},
    {"RELATIVE4", camera_blend},
    {"RELATIVE5", camera_blend},
    {"RELATIVE6", camera_blend},
};

void camera_set_view_angle(int camera, Angle view, Angle turn)
{
    if (turn <= 0) {
        turn = QUARTER_TURN;
    }
    camera_stacks[camera].view_target = view;
    camera_stacks[camera].view_turn = turn;
}

Angle camera_view_angle(int camera)
{
    return camera_stacks[camera].view;
}

int camera_top(int camera)
{
    return camera_stacks[camera].top;
}

/* the highest index below index whose mode isn't CAMERA_MODE_NONE, or 0
 * where there is none. an index of 0 or less gives -1 */
static int camera_mode_below(int camera, int index)
{
    struct camera_stack *stack;

    if (--index < 0) {
        return -1;
    }
    stack = &camera_stacks[camera];
    while (index != 0) {
        if (stack->modes[index].number) {
            break;
        }
        index--;
    }
    return index;
}

int camera_mode_below_top(int camera)
{
    return camera_mode_below(camera, camera_top(camera));
}

int camera_mode_at(int camera, int index)
{
    return camera_stacks[camera].modes[index].number;
}

int camera_adjust_at(int camera, int index)
{
    return camera_stacks[camera].adjusts[index].number;
}

struct camera_slot *camera_set_mode(int camera, int index, int number)
{
    int top;
    int i;
    Uint8 *byte;

    if (index >= CAMERA_STACK_DEPTH) {
        return NULL;
    }
    top = camera_top(camera);
    camera_stacks[camera].modes[index].shape = 0;
    camera_stacks[camera].modes[index].number = number;
    camera_stacks[camera].modes[index].steps = 0;
    /* @hack spelling: the work cleared through a pointer under a count that
     * runs down to 0, where a loop over an index would do */
    byte = camera_stacks[camera].modes[index].work;
    for (i = sizeof(camera_stacks[camera].modes[index].work); i != 0; i--) {
        *byte++ = 0;
    }
    camera_stacks[camera].modes[index].step = camera_mode_step(number);
    camera_stacks[camera].modes[index].zone = NULL;
    if (top < index) {
        camera_stacks[camera].top = index;
        camera_copy_adjust(camera, top, index);
    }
    return &camera_stacks[camera].modes[index];
}

struct camera_slot *camera_push_mode(int camera, int number)
{
    /* @hack spelling: index, a local for the slot above the top, where the
     * sum in the call would do */
    int index = camera_top(camera) + 1;

    return camera_set_mode(camera, index, number);
}

struct camera_slot *camera_set_mode_with_shape(int camera, int index,
                                               int number, int shape)
{
    struct camera_slot *slot;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((slot = camera_set_mode(camera, index, number)) != NULL) {
        camera_stacks[camera].modes[index].shape = shape;
        return slot;
    }
    return NULL;
}

struct camera_slot *camera_set_user_mode(int camera, int index,
                                         camera_step *step)
{
    struct camera_slot *slot;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((slot = camera_set_mode(camera, index, CAMERA_MODE_USER)) != NULL) {
        /* @hack redundant: shape and steps cleared, which camera_set_mode
         * has done already */
        camera_stacks[camera].modes[index].shape = 0;
        camera_stacks[camera].modes[index].step = step;
        camera_stacks[camera].modes[index].steps = 0;
        /* @hack redundant: the top raised to index, which camera_set_mode
         * has left no lower than index */
        if (camera_top(camera) < index) {
            camera_stacks[camera].top = index;
        }
        return slot;
    }
    return NULL;
}

struct camera_slot *camera_push_user_mode(int camera, camera_step *step)
{
    /* @hack spelling: index, a local for the slot above the top, where the
     * sum in the call would do */
    int index = camera_top(camera) + 1;

    return camera_set_user_mode(camera, index, step);
}

int camera_set_adjust(int camera, int index, int number)
{
    int i;
    Uint8 *byte;

    if (index > camera_top(camera)) {
        return 0;
    }
    camera_stacks[camera].adjusts[index].number = number;
    camera_stacks[camera].adjusts[index].shape = 0;
    camera_stacks[camera].adjusts[index].steps = 0;
    /* @hack spelling: the work cleared through a pointer under a count that
     * runs down to 0, where a loop over an index would do */
    byte = camera_stacks[camera].adjusts[index].work;
    for (i = sizeof(camera_stacks[camera].adjusts[index].work); i != 0; i--) {
        *byte++ = 0;
    }
    camera_stacks[camera].adjusts[index].step = camera_adjust_step(number);
    return 1;
}

int camera_set_user_adjust(int camera, int index, camera_step *step)
{
    if (camera_set_adjust(camera, index, CAMERA_ADJUST_USER)) {
        /* @hack redundant: shape and steps cleared, which camera_set_adjust
         * has done already */
        camera_stacks[camera].adjusts[index].shape = 0;
        camera_stacks[camera].adjusts[index].step = step;
        camera_stacks[camera].adjusts[index].steps = 0;
        return 1;
    }
    return 0;
}

static int camera_copy_adjust(int camera, int from, int to)
{
    int top = camera_top(camera);

    if (top < from || top < to) {
        return 0;
    }
    /* @hack type: camera_stacks through a cast to its own type, on both
     * sides, where camera_stacks[camera] would do */
    ((struct camera_stack *)camera_stacks)[camera].adjusts[to] =
        ((struct camera_stack *)camera_stacks)[camera].adjusts[from];
    return 1;
}

void camera_remove_mode(int camera, int index)
{
    int below;

    if (index == 0) {
        return;
    }
    camera_stacks[camera].modes[index].number = CAMERA_MODE_NONE;
    if (camera_top(camera) != index) {
        return;
    }
    below = camera_mode_below_top(camera);
    /* @hack redundant: the return for a below under 0, which
     * camera_mode_below_top gives only for a top of 0 or less */
    if (below < 0) {
        return;
    }
    camera_stacks[camera].top = below;
    camera_stacks[camera].modes[below].steps = 0;
    camera_stacks[camera].adjusts[below].steps = 0;
}

camera_step *camera_mode_step(int number)
{
    return camera_modes[number].step;
}

camera_step *camera_adjust_step(int number)
{
    return camera_adjusts[number].step;
}

const char *camera_mode_name(int camera, int index)
{
    return camera_modes[camera_mode_at(camera, index)].name;
}

const char *camera_adjust_name(int camera, int index)
{
    return camera_adjusts[camera_adjust_at(camera, index)].name;
}

int camera_mode_shape(int camera, int index)
{
    return camera_stacks[camera].modes[index].shape;
}

void camera_set_mode_shape(int camera, int index, int shape)
{
    camera_stacks[camera].modes[index].shape = shape;
}

void camera_set_mode_zone(int camera, int index, struct camera_zone *zone)
{
    camera_stacks[camera].modes[index].zone = zone;
}

struct camera_zone *camera_mode_zone(int camera, int index)
{
    return camera_stacks[camera].modes[index].zone;
}

void camera_set_shake(int camera, NJS_POINT3 *shake)
{
    camera_stacks[camera].shake = *shake;
}

void camera_shake(int camera, int steps, Float size)
{
    camera_stacks[camera].shake_state = CAMERA_SHAKE_FULL;
    camera_stacks[camera].shake_size = size;
    camera_stacks[camera].shake_steps = steps;
}

void camera_stop_shake(int camera)
{
    camera_stacks[camera].shake.x = 0.0F;
    camera_stacks[camera].shake.y = 0.0F;
    camera_stacks[camera].shake.z = 0.0F;
    camera_stacks[camera].shake_state = CAMERA_SHAKE_NONE;
}

void camera_put_at(int camera, NJS_POINT3 *pos)
{
    NJS_POINT3 target;
    int i;

    target = player_work[camera]->pos;
    target.y += player_records[camera]->consts.target_height;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    camera_state.pos = *pos;
    camera_state.target = target;
    camera_state.ang.z = 0;
    camera_state_complete();
    camera_place_from_state(camera);
    for (i = 0; i < PAST_PLACES; i++) {
        player_set_past(camera, i, &target, NULL);
    }
    camera_set_mode(camera, camera_top(camera), CAMERA_MODE_KNUKLES);
    camera_set_adjust(camera, camera_top(camera), CAMERA_ADJUST_NONE);
}
