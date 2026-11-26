/* the sign plate, BOARD in Dry Lagoon's object list: a plate on a post, in
 * three kinds */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg18/lagoon_util.h"
#include "stg18/sign_plate.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

#define SIGN_PLATE_KINDS 3
#define KIND(work) ((Sint32)njAbs((work)->scl.z) % SIGN_PLATE_KINDS)
#define SCALE(work) ((work)->scl.x + 1.0F)
/* the bit of (Sint32)scl.y that tips the plate back a quarter turn */
#define TIPPED 0x1

#include "stg18/sign_plate/text.inc"

static NJS_TEXNAME sign_plate_up_bright_texlist_names[3] = {
#include "stg18/sign_plate/sign_plate_up_bright_texlist_names.inc"
};

static NJS_TEXLIST sign_plate_up_bright_texlist = {
#include "stg18/sign_plate/sign_plate_up_bright_texlist.inc"
};

#include "shared/canyon_art/geometry/board_up_bright.h"

static NJS_CNK_MODEL sign_plate_up_bright_model = {
#include "stg18/sign_plate/sign_plate_up_bright_model.inc"
};

static NJS_CNK_OBJECT sign_plate_up_bright_object = {
#include "stg18/sign_plate/sign_plate_up_bright_object.inc"
};

static NJS_TEXNAME sign_plate_up_dark_texlist_names[3] = {
#include "stg18/sign_plate/sign_plate_up_dark_texlist_names.inc"
};

static NJS_TEXLIST sign_plate_up_dark_texlist = {
#include "stg18/sign_plate/sign_plate_up_dark_texlist.inc"
};

#include "shared/canyon_art/geometry/board_up_dark.h"

static NJS_CNK_MODEL sign_plate_up_dark_model = {
#include "stg18/sign_plate/sign_plate_up_dark_model.inc"
};

static NJS_CNK_OBJECT sign_plate_up_dark_object = {
#include "stg18/sign_plate/sign_plate_up_dark_object.inc"
};

static NJS_TEXNAME sign_plate_low_texlist_names[3] = {
#include "stg18/sign_plate/sign_plate_low_texlist_names.inc"
};

static NJS_TEXLIST sign_plate_low_texlist = {
#include "stg18/sign_plate/sign_plate_low_texlist.inc"
};

#include "shared/canyon_art/geometry/board_low.h"

static NJS_CNK_MODEL sign_plate_low_model = {
#include "stg18/sign_plate/sign_plate_low_model.inc"
};

static NJS_CNK_OBJECT sign_plate_low_object = {
#include "stg18/sign_plate/sign_plate_low_object.inc"
};

static void sign_plate_destroy(struct task *task);
static void sign_plate_step(struct task *task);
static void sign_plate_draw(struct task *task);

KEPT static NJS_CNK_MODEL *sign_plate_models[SIGN_PLATE_KINDS] = {
#include "stg18/sign_plate/sign_plate_models.inc"
};
NJS_TEXLIST *sign_plate_texlists[SIGN_PLATE_KINDS + 1] = {
#include "stg18/sign_plate/sign_plate_texlists.inc"
};
static char *sign_plate_kind_names[SIGN_PLATE_KINDS] = {
#include "stg18/sign_plate/sign_plate_kind_names.inc"
};

void sign_plate_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = sign_plate_draw;
    task->step = sign_plate_step;
    task->destructor = sign_plate_destroy;
}

static void sign_plate_destroy(struct task *task)
{
}

static void sign_plate_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, where the
     * call alone would do */
    if (placement_try_leave(task)) {
        return;
    }
}

static void sign_plate_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    scale.x = scale.y = SCALE(work);
    scale.z = 1.0F;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: scl.y read through task->work, where work would do */
    if ((Sint32)task->work->scl.y & TIPPED) {
        njRotateX(NULL, -QUARTER_TURN);
    }
    /* @hack spelling: scl.z read through task->work for the texture list,
     * where work would do */
    njSetTexture(sign_plate_texlists[KIND(task->work)]);
    /* @hack spelling: scl.z read through task->work for the model, where work
     * would do */
    lagoon_model_draw_scaled_v(sign_plate_models[KIND(task->work)], &scale);
    njPopMatrixEx();
}
