/* the toy a race awards, spinning before the current camera. it fades in
 * after more than RACE_AWARD_DELAY frames, and fades out after more than
 * SHOW_FRAMES more */
#include <ninja.h>
#include "camera/camera.h"
#include "chao/toy_art.h"
#include "chaostgrace/prize_art.h"
#include "chaostgrace/race.h"
#include "draw/lines.h"
#include "sound/adx.h"
#include "system/task.h"
#include "system/util.h"

enum prize_mode {
    PRIZE_WAIT,
    PRIZE_FADE_IN,
    PRIZE_SHOW,
    PRIZE_FADE_OUT,
    PRIZE_GONE
};
#define SHOW_FRAMES 330
#define FADE_STEP 0.08F
/* the frame of the show that starts the jingle */
#define JINGLE_FRAME 30
#define SPIN 512 /* in angle units a frame */
/* the toy draws only at an opacity above VISIBLE */
#define VISIBLE 0.001F
/* the toy stands TOY_DEPTH ahead of the camera and TOY_DROP below its line of
 * sight */
#define TOY_DEPTH 20.0F
#define TOY_DROP 3.8F
#define TOY_SCALE 0.8F
#define PRIZE_OPACITY(work) ((work)->scl.x)
/* the toy, and its enum race_toy_kind */
#define PRIZE_TOY(work) ((work)->ang.x)
#define PRIZE_KIND(work) ((work)->ang.z)

static void prize_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case PRIZE_WAIT:
        work->timer++;
        if (work->timer > RACE_AWARD_DELAY) {
            work->mode++;
            work->submode = 0;
        }
        break;
    case PRIZE_FADE_IN:
        PRIZE_OPACITY(work) += FADE_STEP;
        if (PRIZE_OPACITY(work) > 1.0F) {
            PRIZE_OPACITY(work) = 1.0F;
            work->mode++;
            work->submode = 0;
            work->timer = 0;
        }
        break;
    case PRIZE_SHOW:
        if (work->timer == JINGLE_FRAME) {
            music_play_name("chao_r_item_get.adx");
        }
        if (work->timer++ > SHOW_FRAMES) {
            work->mode++;
            work->submode = 0;
            work->timer = 0;
        }
        break;
    case PRIZE_FADE_OUT:
        PRIZE_OPACITY(work) -= FADE_STEP;
        if (PRIZE_OPACITY(work) < 0.0F) {
            PRIZE_OPACITY(work) = 0.0F;
            work->mode++;
            work->submode = 0;
        }
        break;
    }
    work->ang.y += SPIN;
}

static void prize_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct camera_place *camera = camera_views[camera_current_index];
    NJS_POINT3 *position = &camera->pos;
    /* @hack spelling: a switch on mode, where an if would do */
    switch (work->mode) {
    case PRIZE_WAIT:
        break;
    default:
        if (race_mode() < RACE_EMBLEM && PRIZE_OPACITY(work) > VISIBLE) {
            draw_control_save();
            draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR |
                               NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
            draw_attr_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
            draw_attr_set(0, NJD_FST_UA);
            draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
            constant_material_set(PRIZE_OPACITY(work) - 1.0F, 0.0F, 0.0F, 0.0F);
            njSetTexture(&garden_toy_texlist);
            njPushMatrixEx();
            njTranslateEx(position);
            njRotateY(NULL, camera->ang.y);
            njRotateX(NULL, camera->ang.x);
            njTranslate(NULL, 0.0F, -TOY_DROP, -TOY_DEPTH);
            njRotateY(NULL, work->ang.y);
            njScale(NULL, TOY_SCALE, TOY_SCALE, TOY_SCALE);
            if (PRIZE_KIND(work) == RACE_PERSONAL_TOY) {
                njCnkSimpleDrawObject(
                    race_personal_toys[PRIZE_TOY(work)].model);
            } else {
                njCnkSimpleDrawObject(race_garden_toys[PRIZE_TOY(work)]);
            }
            njPopMatrixEx();
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
    }
}

static void prize_destroy(struct task *task)
{
}

void race_present_toy(int toy, int kind)
{
    struct task *task =
        task_create(TASK_WORK, 2, prize_step, "ToyPresenterExecutor");
    task->draw = prize_draw;
    task->destructor = prize_destroy;
    PRIZE_TOY(task->work) = toy;
    PRIZE_KIND(task->work) = kind;
}
