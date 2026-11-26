/* the camera task: it sets the cameras up, runs each one's mode and adjust
 * every step, shakes it, and gives it to Ninja */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camedit.h"
#include "camera/camera.h"
#include "camera/gakugaku.h"
#include "camera/modes.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "camera/viewport.h"
#include "camera/zones.h"
#include "draw/light.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the angle of view every camera starts at */
#define VIEW_ANGLE ((Angle)(70 * ANGLE_PER_DEGREE))
/* the steps a shake takes to fade out */
#define SHAKE_FADE_STEPS 60
/* a random number from -0.5 up to 0.5, never 0.5 itself */
#define RANDOM_HALF() ((rand() * RAND_SCALE) + -0.5F)
/* the steps of the cameras below ZONES_UPDATE_CAMERAS run
 * camera_zones_update. each run takes zones' modes off its own camera's
 * stack, and enters zones for every camera in use */
#define ZONES_UPDATE_CAMERAS 2

static void camera_task_init(struct task *task);
static void camera_task_step(struct task *task);
static void camera_task_draw(struct task *task);
static void camera_task_destroy(struct task *task);

/* initializers place camera_task, camera_task_work, draw_total_steps and
 * draw_calls in the object's data, not its bss */
static struct task *camera_task = NULL; /* null while there is none */
struct task_work *camera_task_work = NULL;
/* game_state.total_steps as camera_draw_index last found it, and the camera
 * it last returned */
static Uint32 draw_total_steps = 0;
static Sint32 draw_calls = 0;

Sint32 camera_steps;
/* the directions easy_light_set_direction and simple_light_set_direction
 * were last given */
static NJS_VECTOR easy_light_direction;
static NJS_VECTOR simple_light_direction;
/* while it isn't 0, camera_task_step does nothing */
static Sint32 camera_held;

void camera_start(int count)
{
    int i;

    if (camera_task == NULL) {
        camera_task = task_create(TASK_WORK, 0, camera_task_init, "cameraCons");
        camera_task->destructor = camera_task_destroy;
        camera_probe_alloc();
    }
    camera_clear_stacks();
    camera_trails_clear();
    camera_delay_settle_all();
    camera_active_count = count;
    camera_current_index = 0;
    camera_steps = 0;
    for (i = 0; i < CAMERAS; i++) {
        if (i < count) {
            camera_views[i] = &camera_stacks[i].place;
            camera_stacks[i].view = VIEW_ANGLE;
            camera_stacks[i].view_target = VIEW_ANGLE;
            camera_stacks[i].view_turn = QUARTER_TURN;
            camera_stop_shake(i);
            camera_set_mode(i, 0, CAMERA_MODE_CARMEL);
        } else {
            camera_views[i] = NULL;
        }
        if (stage_current_id() < STAGE_STG_END) {
            camera_set_mode(i, CAMERA_INIT_SLOT, CAMERA_MODE_INIT);
        }
        njInitCamera(&camera_stacks[i].camera);
    }
    camera_task_work = NULL;
    camedit_init();
    viewport_select_split(0);
    camera_held = 0;
}

void camera_stop(void)
{
    if (camera_task != NULL) {
        task_destroy(camera_task);
        camera_task = NULL;
    }
    camera_probe_free();
}

void camera_reset_modes(int camera)
{
    int i;

    camera_stop_shake(camera);
    for (i = CAMERA_STACK_DEPTH - 1; i > 0; i--) {
        camera_remove_mode(camera, i);
    }
    camera_set_mode(camera, 0, CAMERA_MODE_CARMEL);
    if (stage_current_id() < STAGE_STG_END) {
        camera_set_mode(camera, CAMERA_INIT_SLOT, CAMERA_MODE_INIT);
    }
}

void camera_restart(void)
{
    camera_start(camera_active_count);
}

/* the camera this draw is for. the task draws once for each camera in a
 * frame, and game_state.total_steps moves on between frames. so a draw that
 * finds total_steps moved is camera 0's. each draw after it in the frame is
 * the next camera's */
static int camera_draw_index(void)
{
    if (draw_total_steps != game_state.total_steps) {
        draw_calls = 0;
    } else {
        draw_calls++;
    }
    draw_total_steps = game_state.total_steps;
    return draw_calls;
}

#ifdef NONMATCHING
void camera_draw_restart(void)
{
    /* any count but the frame's own makes the next draw camera 0's */
    draw_total_steps = game_state.total_steps - 1;
}
#endif

/* give camera to Ninja: its area of the screen, its place with its shake
 * added, its angle of view, and its light */
static void camera_apply(int camera)
{
    struct camera_stack *stack;

    viewport_set(camera);
    stack = &camera_stacks[camera];
    stack->camera.px = stack->place.pos.x + stack->shake.x;
    stack->camera.py = stack->place.pos.y + stack->shake.y;
    stack->camera.pz = stack->place.pos.z + stack->shake.z;
    stack->camera.vx = stack->place.dir.x;
    stack->camera.vy = stack->place.dir.y;
    stack->camera.vz = stack->place.dir.z;
    stack->camera.roll = stack->place.ang.z;
    njSetCameraAngle(&stack->camera, stack->view);
    /* @hack spelling: the two calls joined by a comma, where two statements
     * would do */
    njSetCamera(&stack->camera), light_apply(light_camera);
}

/* keep work as camera_task_work, and put it where camera 0 is */
static void camera_place_work(struct task_work *work)
{
    /* @hack spelling: every store made through camera_task_work, where
     * work would do */
    camera_task_work = work;
    camera_task_work->pos = camera_stacks[0].place.pos;
    camera_task_work->ang = camera_stacks[0].place.ang;
    camera_task_work->scl.x = 1.0F;
    camera_task_work->scl.y = 1.0F;
    camera_task_work->scl.z = 1.0F;
}

void camera_state_from_place(int camera)
{
    struct camera_place *place = &camera_stacks[camera].place;

    camera_state.pos = place->pos;
    camera_state.ang = place->ang;
    camera_state.dir = place->dir;
    camera_state.target = place->target;
    camera_state.dist = place->dist;
}

/* step camera's shake. a shake at CAMERA_SHAKE_FULL lasts until its steps
 * run out, then fades over SHAKE_FADE_STEPS steps. negative steps never run
 * out */
static void camera_step_shake(int camera)
{
    NJS_POINT3 shake;
    Float size;

    switch (camera_stacks[camera].shake_state) {
    case CAMERA_SHAKE_FULL:
        shake.x = RANDOM_HALF() * camera_stacks[camera].shake_size;
        shake.y = RANDOM_HALF() * camera_stacks[camera].shake_size;
        shake.z = RANDOM_HALF() * camera_stacks[camera].shake_size;
        if (camera_stacks[camera].shake_steps > 0) {
            camera_stacks[camera].shake_steps--;
        } else if (camera_stacks[camera].shake_steps < 0) {
            camera_stacks[camera].shake_steps = -1;
        } else {
            camera_stacks[camera].shake_state = CAMERA_SHAKE_FADING;
            camera_stacks[camera].shake_steps = SHAKE_FADE_STEPS;
        }
        camera_set_shake(camera, &shake);
        break;
    case CAMERA_SHAKE_FADING:
        size = camera_stacks[camera].shake_size *
               camera_stacks[camera].shake_steps * (1.0F / SHAKE_FADE_STEPS);
        shake.x = RANDOM_HALF() * size;
        shake.y = RANDOM_HALF() * size;
        shake.z = RANDOM_HALF() * size;
        camera_set_shake(camera, &shake);
        camera_stacks[camera].shake_steps--;
        if (camera_stacks[camera].shake_steps <= 0) {
            camera_stop_shake(camera);
        }
        break;
    }
}

static void camera_task_draw(struct task *task)
{
    NJS_POINT3 pos;
    struct camera_stack *stack;
    int camera;
    int i;

    camera_apply(camera_current_index = camera_draw_index());
    camedit_draw();
    camera_probe_debug = 0;
    /* @hack redundant: a block that never runs, and the empty else after
     * it */
    if (0.0F) {
        camera = camera_active_count;
        camera_probe_debug = 1;
        camera_set_shake(camera, &pos);
        i = camera_active_count;
        stack = &camera_stacks[i];
        pos = stack->place.pos;
        camera_set_shake(i, &pos);
        stack->shake_state = CAMERA_SHAKE_NONE;
        camera_steps = 0;
        camera_set_shake(i, &pos);
        stack->shake_steps = 0;
        draw_calls = 0;
        camera_set_shake(i, &pos);
    } else {
    }
    camera_draw_end(camera_current_index);
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
void fun_8c0ab11c(void) /* NOLINT(misc-use-internal-linkage) */
{
}

/* run each camera in use: its zones, its mode, its adjust and its shake.
 * then put the task's work where camera 0 is, and give camera 0 to Ninja */
static void camera_task_step(struct task *task)
{
    if (camera_held) {
        return;
    }
    camera_pasts_step();
    for (camera_current_index = 0; camera_current_index < camera_active_count;
         camera_current_index++) {
        camera_step_standing(camera_current_index);
        camera_state_from_place(camera_current_index);
        if (camera_current_index < ZONES_UPDATE_CAMERAS) {
            camera_zones_update(camera_current_index);
        }
        camera_run_mode(camera_current_index);
        camera_state_complete();
        if (camera_steps) {
            camera_run_adjust(camera_current_index);
            camera_state_complete();
            camera_keep_unclamped(camera_current_index);
            if (game_state.unk_2a == 0 && camera_state.unk_04 != 0) {
                camera_collide(camera_current_index);
                camera_state_complete();
            }
        }
        camera_place_from_state(camera_current_index);
        camera_trail_step(camera_current_index);
        camera_step_view_angle(camera_current_index);
        camera_step_shake(camera_current_index);
    }
    camera_place_work(task->work);
    camedit_update();
    camera_steps++;
    camera_apply(camera_current_index = 0);
}

static void camera_task_destroy(struct task *task)
{
    camera_task = NULL;
    camera_task_work = NULL;
}

/* the task's first step: it gives the first two cameras their first mode
 * and no adjust, and runs a step */
static void camera_task_init(struct task *task)
{
    task->step = camera_task_step;
    task->draw = camera_task_draw;
    task->destructor = camera_task_destroy;
    camera_set_mode(0, 0, CAMERA_MODE_CARMEL);
    camera_set_adjust(0, 0, CAMERA_ADJUST_NONE);
    camera_set_mode(1, 0, CAMERA_MODE_CARMEL);
    camera_set_adjust(1, 0, CAMERA_ADJUST_NONE);
    camera_task_step(task);
}

void easy_light_set_direction(Float x, Float y, Float z)
{
    easy_light_direction.x = x;
    easy_light_direction.y = y;
    easy_light_direction.z = z;
    njCnkSetEasyLight(x, y, z);
}

void simple_light_set_direction(Float x, Float y, Float z)
{
    simple_light_direction.x = x;
    simple_light_direction.y = y;
    simple_light_direction.z = z;
    njCnkSetSimpleLight(x, y, z);
}

void camera_apply_light(void)
{
    light_apply(light_camera);
}

void camera_hold(int stage)
{
    camera_held = 1;
}

void camera_release(void)
{
    camera_held = 0;
}
