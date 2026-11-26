/* the pulsing light, RLIGHT in a stage's object list: a lamp with a glow
 * that brightens and dims */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/pulsing_light.h"
#include "shared/pulsing_light/text.inc"
#include "system/angle.h"
#include "system/task.h"

#include "shared/road_furniture/pulsing_light_art.h"

#define GLOW_TEXTURE 0
/* the glow's quad is turned a full turn for each GLOW_TURN_DISTANCE of the
 * lamp's distance from the camera. the distance counts only DEPTH_PART of
 * the lamp's depth */
#define DEPTH_PART 0.001F
#define GLOW_TURN_DISTANCE 120.0F

static void pulsing_light_destroy(struct task *task);
static void pulsing_light_draw(struct task *task);
static void pulsing_light_step(struct task *task);

NJS_TEXLIST *pulsing_light_texlists[3] = {
#include "shared/pulsing_light/pulsing_light_texlists.inc"
};
static Float glow_size = 6.0F; /* half the glow's width and height */
/* the glow's color is glow_bright where the sine of the work's timer is -1.
 * it is glow_dim where the sine is 1 */
static Uint32 glow_bright = 0xffffffff;
static Uint32 glow_dim = 0x50ffffff;
static NJS_POINT3 glow_pos = {0.0F, 11.5F, 0.0F}; /* in the lamp's frame */
static Angle pulse_step = 0x180; /* what the work's timer gains a step */

void pulsing_light_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    if (!placement_try_leave(task)) {
        task->step = pulsing_light_step;
        task->draw = pulsing_light_draw;
        task->destructor = pulsing_light_destroy;
    }
}

static void pulsing_light_destroy(struct task *task)
{
}

static void pulsing_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 view;
    Angle turn;
    Angle pulse;

    njCalcPoint(NULL, &work->pos, &view);
    view.z *= DEPTH_PART;
    turn = njScalor(&view) * (FULL_TURN / GLOW_TURN_DISTANCE);
    njSetTexture(&pulsing_light_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCnkEasyDrawModel(&pulsing_light_model);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&pulsing_light_glow_texlist);
    /* @hack spelling: pulse, a local for the work's timer, where the timer
     * passed in place would do */
    pulse = work->timer;
    particle_draw_quad(
        &glow_pos, GLOW_TEXTURE, QUARTER_TURN, turn, glow_size, glow_size,
        color_blend(glow_bright, glow_dim, (njSin(pulse) + 1.0F) * 0.5F),
        PARTICLE_BLEND_ADD, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    njPopMatrixEx();
}

static void pulsing_light_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->timer += pulse_step;
    }
}
