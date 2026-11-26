/* PIC in Final Chase's object list, and SIGNSHIP in Final Rush's. its model
 * has three parts: one plain, one whose tint pulses, and one whose texture
 * scrolls */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/pic.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#define PIC_SHAPES 1

NJS_TEXLIST *pic_texlists[] = {&pic_texlist, NULL};
/* a capsule along the model's z, through pos */
/* clang-format off */
static struct collision_shape pic_shapes[PIC_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, 20.0F, 40.0F, 0.0F, 0, {QUARTER_TURN, 0, 0}},
};
/* clang-format on */
/* the pulsing part's tint is bright_color where the sine of game_state.steps
 * times pulse_step is -1. it is dim_color where the sine is 1 */
static Uint32 bright_color = 0xffffffff;
static Uint32 dim_color = 0xff101010;
static Angle pulse_step = 0x1000;
/* the scroll's speeds in u and v, a step. the draw reads each as a Sint16 */
static Sint32 scroll_u_speed = 0;
static Sint32 scroll_v_speed = 5;

static void pic_destroy(struct task *task);
static void pic_step(struct task *task);
static void pic_draw(struct task *task);

void pic_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = pic_draw;
        task->step = pic_step;
        task->destructor = pic_destroy;
        collision_init(task, pic_shapes, PIC_SHAPES, COLLISION_OBJECT);
    }
}

static void pic_destroy(struct task *task)
{
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void pic_step(register struct task *task)
{
    /* @hack spelling: collision_register in an else of its own, where a call
     * after the test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        collision_register(task);
    }
}

static void pic_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&pic_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(pic_object->model);
    model_draw_tinted(
        pic_pulse_object->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * pulse_step) + 1.0F) * 0.5F));
    texture_anim_scroll(pic_scroll_object->model, &pic_anim,
                        ((Sint16)game_state.steps * (Sint16)scroll_u_speed) &
                            (SCROLL_WRAP - 1),
                        ((Sint16)game_state.steps * (Sint16)scroll_v_speed) &
                            (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(pic_scroll_object->model);
    njPopMatrixEx();
}
