/* the race's camera manager, which steps every camera each frame. in a
 * multiplayer race it cuts screen 0 between the racers */
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/race.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

struct race_viewport race_viewports[CAMERAS];
struct race_camera race_cameras[RACE_ENTRANTS];
int race_racing_entrants[RACE_ENTRANTS];
static struct task *manager = NULL;
static Uint32 cut_frames = 0, cheer_frames = 0;
int race_camera_cheer_target = -1;

/* each time cheer_frames passes CHEER_DELAY it starts again, and screen 0
 * cuts to race_camera_cheer_target unless that is -1. once cut_frames, the
 * frames since screen 0's last cut, passes CUT_DELAY, screen 0 cuts to a
 * random racer */
#define CHEER_DELAY 180
#define CUT_DELAY 480

#define START_VIEW(x, target_y, target_z)                                      \
    {CAMERA_POS_TARGET,          0,         0,                                 \
     {x, 7.5F, 21.0F},           {0, 0, 0}, {0.0F, 0.0F, 0.0F},                \
     {0.0F, target_y, target_z}, 5.0F}
static struct camera_state starting_views[RACE_ENTRANTS] = {
    START_VIEW(0.0F, 20.0F, 0.0F),  START_VIEW(10.0F, 20.0F, 0.0F),
    START_VIEW(20.0F, 0.0F, 50.0F), START_VIEW(30.0F, 20.0F, 0.0F),
    START_VIEW(40.0F, 20.0F, 0.0F), START_VIEW(50.0F, 20.0F, 0.0F),
    START_VIEW(60.0F, 20.0F, 0.0F), START_VIEW(70.0F, 20.0F, 0.0F)};

void race_camera_to_screen(struct camera_stack *stack, struct camera_slot *slot)
{
    struct race_viewport *viewport = &race_viewports[camera_current_index];
    camera_state = race_cameras[viewport->camera].view;
}

void race_set_camera_step(int index, race_camera_step *step)
{
    struct race_camera *camera = &race_cameras[(Uint16)index];
    camera->step = step;
    camera->mode = 0;
    camera->timer = 0;
    camera->frame = 0.0F;
}

race_camera_step *race_get_camera_step(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    return camera->step;
}

void race_set_viewport(Uint16 screen, Uint16 camera)
{
    race_viewports[screen].camera = camera;
}

/* a random racer, from an index into race_racing_entrants below
 * garden_race_entrant_count() */
#define RANDOM_ENTRANT()                                                       \
    race_racing_entrants[(Uint32)RANDOM_BELOW(garden_race_entrant_count())]

void race_start_tracking(void)
{
    int index;
    struct task_work *work = manager->work;
    camera_active_count = work->tick;
    for (index = 0; index < RACE_ENTRANTS; ++index) {
        race_set_camera_step(index, race_camera_follow);
    }
    if (garden_race_group() == RACE_MULTI) {
        race_set_viewport(0, RANDOM_ENTRANT());
    }
}

void race_step_camera(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    if (camera->step) {
        camera->step(index);
    }
}

void race_select_spectator_view(int screen)
{
    if (race_mode() >= RACE_PODIUM) {
        race_set_viewport(0, 0);
        return;
    }
    if (cheer_frames++ > CHEER_DELAY) {
        if (race_camera_cheer_target >= 0) {
            race_set_viewport(0, race_camera_cheer_target);
            race_camera_cheer_target = -1;
            cut_frames = 0;
        }
        cheer_frames = 0;
    }
    if (cut_frames++ > CUT_DELAY) {
        cut_frames = 0;
        race_set_viewport(0, RANDOM_ENTRANT());
    }
}

static void manager_step(struct task *task)
{
    int index;
    for (index = 0; index < RACE_ENTRANTS; ++index) {
        race_step_camera(index);
    }
    if (race_get_camera_step(0) != race_camera_opening) {
        if (garden_race_group() == RACE_MULTI) {
            race_select_spectator_view(0);
        }
    }
}

static void manager_draw(struct task *task)
{
}

static void manager_destroy(struct task *task)
{
    int screen;
    manager = NULL;
    race_clear_camera_keys();
    for (screen = 0; screen < CAMERAS; ++screen) {
        camera_remove_mode(screen, camera_top(screen));
    }
}

#ifdef NONMATCHING
/* this side returns whether the cameras run, as camera.h says */
Bool race_camera_create(int track)
#else
void race_camera_create(int track)
#endif
{
    int index;
    struct task *task;
    if (manager) {
#ifdef NONMATCHING
        /* this side returns that the cameras already run */
        return TRUE;
#else
        return;
#endif
    }
    task = task_create(TASK_WORK, 2, manager_step, "ALR_CameraManager");
#ifdef NONMATCHING
    /* this side returns FALSE where task_create fails or the keys don't load.
     * the matching side writes through the null */
    if (task == NULL) {
        return FALSE;
    }
    if (!race_load_camera_keys(track)) {
        task_destroy(task);
        return FALSE;
    }
#endif
    task->draw = manager_draw;
    task->destructor = manager_destroy;
    manager = task;
    camera_active_count = 1;
    /* how many screens race_start_tracking puts in use */
    task->work->tick = 1;
    for (index = 0; index < CAMERAS; ++index) {
        camera_push_user_mode(index, race_camera_to_screen);
        race_set_viewport(index, 2 * index);
    }
    race_set_camera_step(0, race_camera_opening);
    /* @hack spelling: a loop that tests after its body, where
     * for (index = 1; index < RACE_ENTRANTS; ++index) would do */
    index = 1;
    for (;;) {
        race_set_camera_step(index, NULL);
        if (++index >= RACE_ENTRANTS) {
            break;
        }
    }
    for (index = 0; index < RACE_ENTRANTS; ++index) {
        struct race_camera *camera = &race_cameras[index];
        camera->key = -1;
        camera->view = starting_views[index];
        camera->entrant = index;
    }
    if (player_pads[0]->press & PDD_DGT_TY) {
        race_set_camera_step(0, race_camera_preview);
    }
#ifdef NONMATCHING
    /* this side loads the keys above, before it sets the cameras up, and
     * returns that they run */
    return TRUE;
#else
    race_load_camera_keys(track);
#endif
}
