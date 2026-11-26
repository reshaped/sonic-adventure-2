/* the grind rail, GLINDRB in a stage's object list. the players grind on it,
 * and it comes in three sizes. a light on it pulses, and a texture along it
 * scrolls */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/grind_rail.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"

#include "shared/grind_rail/text.inc"

#define SIZES 3
/* the angle the work's timer, the light's pulse, moves in a step */
#define PULSE_STEP 640

#define SIZE(work) ((Uint32)(work)->scl.x % SIZES)

/* each size's path in the model's space, a line along x centered on the
 * origin */
static struct path_point grind_rail_0_points[2] = {
#include "shared/grind_rail/grind_rail_0_points.inc"
};
static struct path grind_rail_0_path = {
#include "shared/grind_rail/grind_rail_0_path.inc"
};
static struct path_point grind_rail_1_points[2] = {
#include "shared/grind_rail/grind_rail_1_points.inc"
};
static struct path grind_rail_1_path = {
#include "shared/grind_rail/grind_rail_1_path.inc"
};
static struct path_point grind_rail_2_points[2] = {
#include "shared/grind_rail/grind_rail_2_points.inc"
};
static struct path grind_rail_2_path = {
#include "shared/grind_rail/grind_rail_2_path.inc"
};
static Sint16 grind_rail_2_uvs[38] = {
#include "shared/grind_rail/grind_rail_2_uvs.inc"
};
static struct texture_anim grind_rail_2_anim = {
#include "shared/grind_rail/grind_rail_2_anim.inc"
};
static Sint16 grind_rail_1_uvs[38] = {
#include "shared/grind_rail/grind_rail_1_uvs.inc"
};
static struct texture_anim grind_rail_1_anim = {
#include "shared/grind_rail/grind_rail_1_anim.inc"
};
static Sint16 grind_rail_0_uvs[38] = {
#include "shared/grind_rail/grind_rail_0_uvs.inc"
};
static struct texture_anim grind_rail_0_anim = {
#include "shared/grind_rail/grind_rail_0_anim.inc"
};
NJS_TEXLIST *grind_rail_texlists[SIZES + 1] = {
#include "shared/grind_rail/grind_rail_texlists.inc"
};
KEPT static NJS_CNK_OBJECT *objects[SIZES] = {
#include "shared/grind_rail/objects.inc"
};
KEPT static struct texture_anim *anims[SIZES] = {
#include "shared/grind_rail/anims.inc"
};
KEPT static struct path *paths[SIZES] = {
#include "shared/grind_rail/paths.inc"
};
/* a box that no step here gives a record */
/* clang-format off */
static struct collision_shape dat_grind_rail_0 = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {0.0F, 9.0F, 0.0F}, 10.0F, 9.0F, 46.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff303030;

static void grind_rail_destroy(struct task *task);
static void grind_rail_draw(struct task *task);
static void grind_rail_step(struct task *task);

#include "shared/grind_path.h"

void grind_rail_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct path_task *record;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = grind_rail_step;
    task->draw = grind_rail_draw;
    task->destructor = grind_rail_destroy;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task->extra.p = grind_rail_path_place(&work->pos, &work->ang,
                                               paths[SIZE(work)]))) {
        child = task_create_child(0, path_rail_init, task);
        if (child != NULL) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((record = syCalloc(1, sizeof(struct path_task))) == NULL) {
                task_destroy(child);
            } else {
                child->extra.p = record;
                record->path = task->extra.p;
            }
        }
    }
}

static void grind_rail_destroy(struct task *task)
{
    struct path *path;

    if (task->extra.p != NULL) {
        path = task->extra.p;
        syFree(path->points);
        syFree(path);
    }
    task->extra.p = NULL;
}

static void grind_rail_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Uint32 size = SIZE(work);

    object = objects[size];
    njSetTexture(grind_rail_texlists[size]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    /* @hack spelling: SIZE(work) taken again, where size would do */
    texture_anim_scroll(object->model, anims[SIZE(work)],
                        ((Sint16)game_state.steps * (Sint16)work->scl.y) &
                            (SCROLL_WRAP - 1),
                        0);
    njCnkEasyDrawModel(object->model);
    object = object->sibling;
    model_draw_tinted(object->model,
                      color_blend(bright_color, dim_color,
                                  (njSin(work->timer) + 1.0F) * 0.5F));
    njPopMatrixEx();
}

static void grind_rail_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->timer += PULSE_STEP;
    }
    /* @hack redundant: a block no path reaches. SHC still places its strings
     * in this source's constants, and the image holds the code of its draws
     * of size 0's model and first child */
    if (0) {
        NJS_CNK_OBJECT *object = objects[0];

        (void)RAIL_EDITOR_PITCH;
        (void)RAIL_EDITOR_YAW;
        (void)RAIL_EDITOR_ROLL;
        (void)RAIL_EDITOR_SIZE;
        (void)RAIL_EDITOR_UV_SPEED;
        if (object != NULL) {
            njCnkEasyDrawModel(object->model);
            njCnkEasyDrawModel(object->child->model);
        }
    }
}
