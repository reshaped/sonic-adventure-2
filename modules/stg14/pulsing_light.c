/* the pulsing light, RLIGHT in Radical Highway's object list: a lamp with a
 * glow that brightens and dims. it is shared/pulsing_light.c's light with a
 * build of the lamp's model. its draws are cut off by depth, and its pulse
 * stops with time */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg14/pulsing_light.h"
#include "stg14/pulsing_light/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* nothing draws where the lamp lies more than BEHIND behind the camera. the
 * lamp's model draws only less than LAMP_DEPTH ahead of the camera */
#define BEHIND 25.0F
#define LAMP_DEPTH 250.0F
/* the glow turns a full turn for each GLOW_TURN_DISTANCE of the lamp's
 * distance from the camera. that distance counts the lamp's depth at
 * DEPTH_PART of itself. so the glow turns as the lamp crosses the view, and
 * hardly as it nears */
#define GLOW_TURN_DISTANCE 120.0F
#define DEPTH_PART 0.001F
#define GLOW_TEXTURE 0

#include "shared/road_furniture/pulsing_light_art.h"

NJS_TEXLIST *highway_pulsing_light_texlists[3] = {
#include "stg14/pulsing_light/highway_pulsing_light_texlists.inc"
};
/* the lamp's build, or null while highway_pulsing_light_build has made
 * none */
static NJS_DIRECT_HEAD *pulsing_light_built = NULL;
static Float glow_size = 6.0F; /* half the glow's width and height */
/* the glow's color runs from glow_bright, where the pulse's sine is -1, to
 * glow_dim, where it is 1 */
static Uint32 glow_bright = 0xffffffff;
static Uint32 glow_dim = 0x50ffffff;
static NJS_POINT3 glow_pos = {0.0F, 11.5F, 0.0F}; /* in the lamp's frame */
/* the pulse's angle is the work's timer, which each step turns by
 * pulse_turn */
static Sint32 pulse_turn = 0x180;

static void pulsing_light_destroy(struct task *task);
static void pulsing_light_draw(struct task *task);
static void pulsing_light_step(struct task *task);

Uint8 *highway_pulsing_light_build(Uint8 *buf)
{
    njSetTexture(&pulsing_light_texlist);
    pulsing_light_built = object_build(&buf, &pulsing_light_object);
    return buf;
}

void highway_pulsing_light_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
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
    Float depth;
    Angle turn;
    Angle pulse;
    NJS_POINT3 view;

    njCalcPoint(NULL, &work->pos, &view);
    depth = view.z;
    if (depth < -BEHIND) {
        return;
    }
    view.z *= DEPTH_PART;
    turn = njScalor(&view) * (FULL_TURN / GLOW_TURN_DISTANCE);
    njSetTexture(&pulsing_light_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    if (depth < LAMP_DEPTH) {
        if (pulsing_light_built != NULL) {
            njDirectDrawObject(pulsing_light_built);
        } else {
            njCnkEasyDrawModel(&pulsing_light_model);
        }
    }
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
        if (!game_state.time_stopped) {
            work->timer += pulse_turn;
        }
    }
}
