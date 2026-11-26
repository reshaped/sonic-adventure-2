/* the board, BOARD in Wild Canyon's object list: a sign on a post, in one
 * of three kinds */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "shared/board.h"
#include "shared/canyon_util.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

#define BOARD_KINDS 3
/* @hack spelling: the work read through task, where the caller's work would
 * do */
#define KIND(task) ((Sint32)njAbs((task)->work->scl.z) % BOARD_KINDS)

/* the bit of (Sint32)scl.y that tips the board back a quarter turn */
#define BOARD_TIPPED 0x1

static const char board_shadow_texture[16] = {
#include "shared/board/board_shadow_texture.inc"
};

static const char board_plate_texture[20] = {
#include "shared/board/board_plate_texture.inc"
};

static const char board_post_texture[20] = {
#include "shared/board/board_post_texture.inc"
};

static const char board_upper_bright_label[12] = {
#include "shared/board/board_upper_bright_label.inc"
};

static const char board_upper_dark_label[12] = {
#include "shared/board/board_upper_dark_label.inc"
};

static const char board_lower_label[12] = {
#include "shared/board/board_lower_label.inc"
};

/* the placement editor's captions for the board's angles, scale and kind */
static const char board_edit_pitch[20] = "<- ANGLE X (   )";

static const char board_edit_yaw[20] = "<- ANGLE Y (   )";

static const char board_edit_roll[20] = "<- ANGLE Z (   )";

static const char board_edit_scale[16] = "<- SCALE(1.0+x)";

static const char board_edit_kind[20] = "<- BOARD(         )";

static NJS_TEXNAME board_up_bright_texlist_names[3] = {
#include "shared/board/board_up_bright_texlist_names.inc"
};

static NJS_TEXLIST board_up_bright_texlist = {
#include "shared/board/board_up_bright_texlist.inc"
};

#include "shared/canyon_art/geometry/board_up_bright.h"

static NJS_CNK_MODEL board_up_bright_model = {
#include "shared/board/board_up_bright_model.inc"
};

/* The renderer uses the model directly. Each mesh also stores an object. */
static NJS_CNK_OBJECT board_up_bright_object = {
#include "shared/board/board_up_bright_object.inc"
};

static NJS_TEXNAME board_up_dark_texlist_names[3] = {
#include "shared/board/board_up_dark_texlist_names.inc"
};

static NJS_TEXLIST board_up_dark_texlist = {
#include "shared/board/board_up_dark_texlist.inc"
};

#include "shared/canyon_art/geometry/board_up_dark.h"

static NJS_CNK_MODEL board_up_dark_model = {
#include "shared/board/board_up_dark_model.inc"
};

static NJS_CNK_OBJECT board_up_dark_object = {
#include "shared/board/board_up_dark_object.inc"
};

static NJS_TEXNAME board_low_texlist_names[3] = {
#include "shared/board/board_low_texlist_names.inc"
};

static NJS_TEXLIST board_low_texlist = {
#include "shared/board/board_low_texlist.inc"
};

#include "shared/canyon_art/geometry/board_low.h"

static NJS_CNK_MODEL board_low_model = {
#include "shared/board/board_low_model.inc"
};

static NJS_CNK_OBJECT board_low_object = {
#include "shared/board/board_low_object.inc"
};

static void board_destroy(struct task *task);
static void board_step(struct task *task);
static void board_draw(struct task *task);

KEPT static NJS_CNK_MODEL *board_models[BOARD_KINDS] = {
#include "shared/board/board_models.inc"
};
NJS_TEXLIST *board_texlists[BOARD_KINDS + 1] = {
#include "shared/board/board_texlists.inc"
};

void board_init(struct task *task)
{
    if (half_try_leave(task)) {
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = board_draw;
    task->step = board_step;
    task->destructor = board_destroy;
}

static void board_destroy(struct task *task)
{
}

static void board_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (half_try_leave(self)) {
        return;
    }
    /* @hack redundant: a return that ends the function anyway */
    if (placement_try_leave(self)) {
        return;
    }
}

static void board_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.y = work->scl.x + 1.0F;
    scale.z = 1.0F;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: the work read again through task, where work would do */
    if ((Sint32)task->work->scl.y & BOARD_TIPPED) {
        njRotateX(NULL, -QUARTER_TURN);
    }
    njSetTexture(board_texlists[KIND(task)]);
    canyon_model_draw_scaled_v(board_models[KIND(task)], &scale);
    njPopMatrixEx();
}

static const char *board_names[BOARD_KINDS] = {
#include "shared/board/board_names.inc"
};
