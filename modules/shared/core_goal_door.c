/* GOAL DOOR, which opens once CLEAR SW is down. it takes player 0's pad and
 * camera, and holds player 0 on the switch until the stage clears */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "shared/core_clear_switch.h"
#include "shared/core_goal.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

enum door_mode { DOOR_WAIT, DOOR_ANIMATE, DOOR_FINISHED };
enum door_sound {
    SOUND_MOVING = 22,
    SOUND_START = 4,
    SOUND_OPEN = 3,
    SOUND_END = 9
};
#define DOOR_NEXT_NONE (-1)
/* player 0 stands RIDER_HEIGHT above the switch's pos */
#define RIDER_HEIGHT 15.0F
/* SOUND_MOVING's key lies one byte past task. the volume calls keyed by task
 * thus leave SOUND_MOVING alone */
#define SOUND_KEY(task) ((char *)(task) + 1)
#define MOVING_PRIORITY 1
#define MOVING_DURATION 10
#define DOOR_VOICE 1759
/* the place and target of the camera that watches the door open, in the
 * stage's frame */
#define VIEW_X 45.0F
#define VIEW_Y 680.0F
#define VIEW_Z (-160.0F)
#define VIEW_TARGET_X (-65.0F)
#define VIEW_TARGET_Y 680.0F
#define VIEW_TARGET_Z (-345.0F)

/* the door's state, at its task's extra */
struct core_door {
    enum door_mode mode;
    int next_mode; /* the mode the next step enters, or DOOR_NEXT_NONE */
    int unk_08;
    int frame; /* core_door_motion's frame */
    /* the steps waited, before the motion starts and again after it ends */
    int delay;
    int unk_14;
};
#define DOOR(task) ((struct core_door *)(task)->extra.p)

/* the current camera's view as the switch went down. nothing reads it */
static NJS_POINT3 door_camera_pos, door_camera_target;

static void door_destroy(struct task *task);
static void door_step(struct task *task);
static void door_draw(struct task *task);
static struct core_door *door_allocate(struct task *task);
static void door_free(struct task *task);
static void door_apply_mode(struct task *task);
/* @hack type: external linkage, where static would do. SHC leaves out a
 * static function whose every call it inlines */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void core_door_request_mode(struct task *task, int next_mode);
static void door_wait(struct task *task);
static void door_animate(struct task *task);
static void door_finish(struct task *task);

static int start_volume = 50;
static int open_volume = 50;
static int end_volume = 125;
static int moving_volume = 25;
static int dat_core_goal_door_0 = 150;
static int open_sound_frame = 135;
static int end_sound_frame = 290;
static int opening_delay = 120;
static void (*door_steps[])(struct task *) = {door_wait, door_animate,
                                              door_finish};
static int clear_delay = 120;

/* @hack spelling: next_mode set by inlined calls, where an assignment would
 * do */
#pragma inline(core_door_request_mode)
void core_goal_door_init(struct task *task)
{
    if (door_allocate(task)) {
        task->draw = door_draw;
        task->step = door_step;
        task->destructor = door_destroy;
        DOOR(task)->mode = DOOR_WAIT;
        DOOR(task)->next_mode = DOOR_NEXT_NONE;
    }
}

static void door_destroy(struct task *task)
{
    door_free(task);
}

static void door_step(struct task *task)
{
    door_apply_mode(task);
    door_steps[DOOR(task)->mode](task);
    if (DOOR(task)->mode == DOOR_ANIMATE || DOOR(task)->mode == DOOR_FINISHED) {
        /* @hack spelling: the two calls joined by a comma, where two statements
         * would do */
        sound_play_for(SOUND_MOVING, SOUND_KEY(task), MOVING_PRIORITY, 0,
                       MOVING_DURATION),
            sound_set_volume(SOUND_KEY(task), SOUND_ANY_ID, moving_volume);
    }
}

static void door_draw(struct task *task)
{
    struct task_work *work = task->work;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njSetTexture(core_door_texlists[0]);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    switch (DOOR(task)->mode) {
    case DOOR_WAIT:
        njCnkEasyDrawObject(core_door_models[0]);
        break;
    case DOOR_ANIMATE:
    case DOOR_FINISHED:
        njCnkEasyDrawMotion(core_door_models[0], core_door_motion,
                            DOOR(task)->frame);
        break;
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static struct core_door *door_allocate(struct task *task)
{
    task->extra.p = syMalloc(sizeof(struct core_door));
    if (!task->extra.p) {
        return NULL;
    }
    DOOR(task)->frame = 0;
    DOOR(task)->delay = 0;
    return task->extra.p;
}

static void door_free(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void door_apply_mode(struct task *task)
{
    if (DOOR(task)->next_mode != DOOR_NEXT_NONE) {
        DOOR(task)->mode = DOOR(task)->next_mode;
        DOOR(task)->next_mode = DOOR_NEXT_NONE;
    }
}

void core_door_request_mode(struct task *task, int next_mode)
{
    DOOR(task)->next_mode = next_mode;
}

static void door_wait(struct task *task)
{
    if (core_clear_switch && core_clear_switch->extra.p &&
        CLEAR_SWITCH(core_clear_switch)->mode == SWITCH_DOWN) {
        core_door_request_mode(task, DOOR_ANIMATE);
        pad_disable_player(0);
        door_camera_pos = camera_views[camera_current_index]->pos;
        door_camera_target = camera_views[camera_current_index]->target;
        camera_push_mode(0, CAMERA_MODE_EASYSET);
        camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
        camera_easyset_states[0].pos.x = VIEW_X;
        camera_easyset_states[0].pos.y = VIEW_Y;
        camera_easyset_states[0].pos.z = VIEW_Z;
        camera_easyset_states[0].target.x = VIEW_TARGET_X;
        camera_easyset_states[0].target.y = VIEW_TARGET_Y;
        camera_easyset_states[0].target.z = VIEW_TARGET_Z;
        camera_easyset_states[0].type = CAMERA_POS_TARGET;
        voice_play(DOOR_VOICE, 0);
    }
}

static void door_animate(struct task *task)
{
    NJS_POINT3 pos;
    pos = core_clear_switch->work->pos;
    player_set_pos(0, pos.x, pos.y + RIDER_HEIGHT, pos.z);
    if (DOOR(task)->delay < opening_delay) {
        ++DOOR(task)->delay;
    } else {
        if (DOOR(task)->frame == 0) {
            sound_play(SOUND_START, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, start_volume);
        }
        if (DOOR(task)->frame == open_sound_frame) {
            sound_play(SOUND_OPEN, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, open_volume);
        }
        if (DOOR(task)->frame == end_sound_frame) {
            sound_play(SOUND_END, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, end_volume);
        }
        if ((Uint32)++DOOR(task)->frame >= core_door_motion[0].nbFrame - 1) {
            DOOR(task)->delay = 0;
            core_door_request_mode(task, DOOR_FINISHED);
        }
    }
}

static void door_finish(struct task *task)
{
    NJS_POINT3 pos;
    pos = core_clear_switch->work->pos;
    player_set_pos(0, pos.x, pos.y + RIDER_HEIGHT, pos.z);
    if (++DOOR(task)->delay >= clear_delay) {
        stage_clear(0);
    }
}

/* @hack redundant: a step that does nothing and that nothing names, with
 * external linkage, since SHC leaves out a static function nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void core_door_idle(struct task *task)
{
}
