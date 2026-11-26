/* the item box a flute stand calls up, KDITEMBOX in a stage's object list */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/item_box_air.h"
#include "shared/knudai.h"
#include "shared/knudai_item_box.h"
#include "shared/ring_sparkle.h"
#include "system/task.h"

#include "shared/knudai_item_box/text.inc"

NJS_TEXLIST *knudai_item_box_texlists[2] = {
#include "shared/knudai_item_box/knudai_item_box_texlists.inc"
};

#define KNUDAI_ID(work, mask) (((work)->ang.x & (mask)) % KNUDAI_IDS)

/* the object's modes, in its work's submode */
enum knudai_item_box_mode { MODE_WAIT, MODE_APPEAR, MODE_SHOWN };
/* the placement's state once the object's stand is played */
#define STATE_PLAYED 1
/* the air item box's submode for a child box */
#define BOX_CHILD 1
/* the work's mode, which the object sets FALSE once it makes its box.
 * nothing of the object reads it */
#define DONE(work) ((work)->mode)

static void knudai_item_box_destroy(struct task *task);
static void knudai_item_box_step(struct task *task);

void knudai_item_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = knudai_item_box_step;
    task->destructor = knudai_item_box_destroy;
    work->submode = MODE_WAIT;
    if (task->placement != NULL && placement_get_state(task)) {
        work->submode = MODE_APPEAR;
    }
}

static void knudai_item_box_destroy(struct task *task)
{
}

static void knudai_item_box_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *box;
    NJS_POINT3 knudai_pos;
    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    int mask;

    if (placement_try_leave(task)) {
        return;
    }
    mask = KNUDAI_ID_MASK;
    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants. without the block's
     * stores, tests and call, SHC's code for the switch below differs */
    if (0) {
        (void)KNUDAI_ITEM_BOX_DEBUG_ID;
        work->submode = MODE_APPEAR;
        DONE(work) = TRUE;
        /* NOLINTBEGIN(clang-diagnostic-pointer-bool-conversion) */
        if (task_create_child) {
        }
        if (item_box_air_step) {
        }
        /* NOLINTEND(clang-diagnostic-pointer-bool-conversion) */
        task_create_child(TASK_WORK, item_box_air_step, task);
    }
    switch (work->submode) {
    case MODE_WAIT:
        /* @hack redundant: a test of knudai_get_pos, which returns 1 for
         * every id knudai_played returns 1 for. nothing reads the pos it
         * writes */
        if (knudai_played(KNUDAI_ID(work, mask)) == 1 &&
            knudai_get_pos(KNUDAI_ID(work, mask), &knudai_pos)) {
            work->submode = MODE_APPEAR;
            if (task->placement != NULL) {
                placement_set_state(task, STATE_PLAYED);
            }
        }
        /* @hack spelling: a return ending the case, where break would do */
        return;
    case MODE_APPEAR:
        if (task->child == NULL) {
            task_create_child(TASK_WORK, item_box_air_step, task);
        }
        if (task->child != NULL) {
            box = task->child;
            box->work->submode = BOX_CHILD;
            box->work->pos = work->pos;
            box->work->ang = work->ang;
            box->work->scl = work->scl;
            work->submode = MODE_SHOWN;
            DONE(work) = FALSE;
        }
        break;
    /* @hack redundant: the case of MODE_SHOWN, which does nothing */
    case MODE_SHOWN:
        break;
    }
}
