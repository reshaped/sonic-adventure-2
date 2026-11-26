/* FLAG and FLAGLOW both compile these four task functions. */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "object/set.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the frame a flag's wave starts from, which flag_init picks at random */
#define FIRST_FRAME(task) ((task)->unk_3c.l)

/* the bit of the work's ang.z that makes the flag blue. the object's own
 * label for ang.z reads "even:Red odd:Blue" */
#define FLAG_BLUE 0x1

static void flag_destroy(struct task *task);
static void flag_step(struct task *task);
static void flag_draw(struct task *task);

void flag_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = flag_draw;
        task->step = flag_step;
        task->destructor = flag_destroy;
        FIRST_FRAME(task) =
            (Sint32)((Float)rand() * (FLAG_FRAMES * RAND_SCALE));
    }
}

static void flag_destroy(struct task *task)
{
}

static void flag_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: color declared register, where a plain local would do */
    register Sint32 color;
    Float scale;

    /* @hack redundant: two blocks no path reaches, kept for the stack frame
     * they give the step. SHC still places the second's strings in this
     * source's constants */
    if (0) {
        color = work->ang.z;
        scale = work->scl.x;
    }
    if (!placement_try_leave(task)) {
        if (0) {
            (void)editor_angle_x;
            (void)editor_angle_y;
            (void)editor_flag_color;
            if (color && scale && work) {
            }
        }
    }
}

static void flag_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame = (FIRST_FRAME(task) + game_state.steps) % FLAG_FRAMES;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    if (work->ang.z & FLAG_BLUE) {
        njSetTexture(flag_blue_texlists[frame]);
        njCnkEasyDrawModel(flag_blue_models[frame]);
    } else {
        njSetTexture(flag_red_texlists[frame]);
        njCnkEasyDrawModel(flag_red_models[frame]);
    }
    njPopMatrixEx();
}
