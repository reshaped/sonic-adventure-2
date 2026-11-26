/* the restart marker, RESTART_OBJ in the cart's object list. the first marker
 * to step starts the race. a marker shows only as a debug cone, while
 * game_state.unk_09 is set */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* each draw turns every marker's cone by these about x, y and z */
#define SPIN_X ((Angle)(10 * ANGLE_PER_DEGREE))
#define SPIN_Y ((Angle)(20 * ANGLE_PER_DEGREE))
#define SPIN_Z ((Angle)(30 * ANGLE_PER_DEGREE))

/* 1 until a marker steps, and again once that marker is destroyed */
static Bool first = 1;
static NJS_VECTOR marker_scale = {20, 20, 20};
static void marker_draw(struct task *task);
static void restart_destroy(struct task *task);

void cart_restart_step(struct task *task)
{
    if (!first) {
        /* @hack redundant: a statement of the warning "2koijou okuna!", don't
         * place two or more, whose string the object's constants hold */
        (void)"2koijou okuna!";
        task->draw = marker_draw;
    } else {
        cart_start_race();
        cart_camera_start(0);
        first = 0;
        task->destructor = restart_destroy;
        task->draw = marker_draw;
    }
}
static void marker_draw(struct task *task)
{
    static struct angles rotation = {0};
    struct task_work *work = task->work;
    if (game_state.unk_09) {
        rotation.x += SPIN_X;
        rotation.y += SPIN_Y;
        rotation.z += SPIN_Z;
        debug_draw_model(&work->pos, &rotation, &marker_scale,
                         DEBUG_MODEL_CONE);
    }
}
static void restart_destroy(struct task *task)
{
    first = 1;
}
