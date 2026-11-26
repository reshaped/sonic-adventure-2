/* the floor and wall hatches. this file includes hatch_field.h,
 * hatch_task_impl.h and hatch_draw_impl.h for each hatch. PART(name) gives
 * each name the hatch's prefix, floor_hatch_ or wall_hatch_. WALL_HATCH is set
 * for the wall's */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#include "shared/field.h"
#include "sound/sound.h"
#include "stg24/hatch.h"
#include "stg24/hatch_art.h"
#include "stg24/hatch_air.h"
#include "stg24/hatch_textures.h"
#include "system/task.h"

/* a hatch's record. depth is how far the lid has sunk into the hatch, and
 * sink_speed how fast it sinks. bounce is how far the spring stands out */
struct hatch {
    NJS_OBJECT collision; /* a copy of PART(collision), in the world */
    Angle phase[2]; /* angles in proportion to depth, which nothing reads */
    Float sink_speed;
    Float depth;
    Float bounce;
    Float bounce_speed;
};

/* the record lies in task->unk_3c, since task's pointer fields are taken. a
 * field takes motion and extra, and force holds the object list's force
 * records */
#define HATCH(task) ((struct hatch *)(task)->unk_3c.p)
#define SET_HATCH(task, record) ((task)->unk_3c.p = (record))
/* a hatch's state, in its work's mode */
enum hatch_mode { SHUT, OPEN };
/* the id of the dynamite charges that hold a hatch shut, in ang.x's low byte */
#define CHARGE_ID(work) ((Uint8)(work)->ang.x)
/* the collision model collides only while a player lies within LID_RANGE of
 * the hatch. the hatch must also be shut, or its depth below CLEARANCE */
#define CLEARANCE 25.0F
#define LID_RANGE 100.0F
/* in a hatch's axes, the floor hatch's mouth lies at y = FLOOR_MOUTH_Y. the
 * wall hatch's lies at x = WALL_MOUTH_X. a point above the first, or past the
 * second along +x, is outside */
#define FLOOR_MOUTH_Y (-5.0F)
#define WALL_MOUTH_X 8.0F
/* inside, a field pulls INNER_PULL straight in, scaled by its pull. it pulls
 * nothing past FLOOR_INNER_REACH from the floor hatch, or past the wall
 * hatch's reach */
#define INNER_PULL 1.5F
#define FLOOR_INNER_REACH 250.0F
/* the floor's field kills inside, below FLOOR_KILL_Y and within
 * FLOOR_KILL_RANGE. the wall's kills within its reach, below WALL_KILL_EDGE in
 * y or past it along -x */
#define FLOOR_KILL_Y (-20.0F)
#define FLOOR_KILL_RANGE 100.0F
#define WALL_KILL_EDGE (-5.0F)
/* a pull this long or shorter is none */
#define NO_PULL 0.00001F
/* the hatch's size class takes the collision model's radius plus
 * CLASS_MARGIN */
#define CLASS_MARGIN 15.0F
/* a hatch that opens makes a burst at its lid's center, moving BURST_SPEED
 * out of the mouth */
#define BURST_SPEED 2.0F
#define BURST_SCALE 18.0F
/* each step, an open hatch plays OPEN_SOUND at its pos for OPEN_SOUND_UPDATES
 * + 1 sound updates. the task is the sound's key, so each call renews it */
#define OPEN_SOUND 9
#define OPEN_SOUND_PRIORITY 1
#define OPEN_SOUND_VOLUME 0
#define OPEN_SOUND_UPDATES 10
/* as the hatch opens, sink_speed starts at SINK_START_SPEED, so the depth goes
 * below 0 first. the lid draws in place until the depth is past 0 again. each
 * step, sink_speed keeps SINK_DRAG of itself and gains the hatch's
 * acceleration. the depth stops at DEPTH_MAX */
#define SINK_START_SPEED (-1.5F)
#define SINK_DRAG 0.99F
#define DEPTH_MAX 5000.0F
/* the spring stretches toward SPRING_TOP, and rebounds from it. each step its
 * speed gains SPRING_PULL of the way left, and keeps SPRING_DRAG of itself. a
 * rebound keeps SPRING_REBOUND of the speed */
#define SPRING_TOP 10.0F
#define SPRING_PULL 0.5F
#define SPRING_DRAG 0.992F
#define SPRING_REBOUND 0.6F
/* phase[0]'s and phase[1]'s angles for each unit of depth */
#define FIRST_PHASE_RATE 23.665777F
#define SECOND_PHASE_RATE 20.02489F
/* the point an open hatch's air spirals in toward, in the hatch's axes */
#define FLOOR_AIR_Y (-20.0F)
#define WALL_AIR_X (-30.0F)
#define WALL_AIR_Y 12.0F
/* below CLEARANCE, the lid draws as deep as SHALLOW_SHARE of its depth. past
 * it, the lid draws DEEP_LAG shallower than its depth. the two meet at
 * CLEARANCE. a wall hatch's lid draws that deep along -x. it also draws
 * WALL_DROP_Y times as far down, and WALL_DROP_Z times as far along +z */
#define SHALLOW_SHARE 0.2F
#define DEEP_LAG 20.0F
#define WALL_DROP_Y 0.6F
#define WALL_DROP_Z 0.25F
/* the lamps blink between no tint and LAMP_DARK, as blink_level(BLINK_PERIOD,
 * BLINK_REST, BLINK_FADE) says */
#define LAMP_DARK 0xff101010
#define BLINK_PERIOD 38
#define BLINK_REST 20
#define BLINK_FADE 6

/* the air an open hatch makes: hatch_air_create's size, length and count */
static Float air_size = 0.5F;
static Float air_length = 80.0F;
static Sint32 air_specks = 35;
static Bool models_ready = FALSE;
enum hatch_model {
    FLOOR_LID,
    FLOOR_LAMP,
    FLOOR_OPEN_LAMP,
    FLOOR_SPRING,
    WALL_LID,
    WALL_LAMP,
    WALL_OPEN_LAMP,
    WALL_SPRING,
    WALL_CLOSED_LAMP,
    HATCH_MODELS
};
static struct model_build models[HATCH_MODELS] = {
    {MODEL_BUILD_ALONE_AS_LAND, &floor_hatch_texlist, &floor_hatch_lid, NULL},
    {MODEL_BUILD_ALONE, &floor_hatch_texlist, &floor_hatch_lamp, NULL},
    {MODEL_BUILD_ALONE, &floor_hatch_texlist, &floor_hatch_open_lamp, NULL},
    {MODEL_BUILD_ALONE, &floor_hatch_texlist, &floor_hatch_spring, NULL},
    {MODEL_BUILD_ALONE_AS_LAND, &wall_hatch_texlist, &wall_hatch_lid, NULL},
    {MODEL_BUILD_ALONE, &wall_hatch_texlist, wall_hatch_lamp, NULL},
    {MODEL_BUILD_ALONE, &wall_hatch_texlist, wall_hatch_open_lamp, NULL},
    {MODEL_BUILD_ALONE, &wall_hatch_texlist, wall_hatch_spring, NULL},
    {MODEL_BUILD_ALONE, &wall_hatch_texlist, wall_hatch_closed_lamp, NULL}};
/* pull scales each hatch's field's speed. reach is how far from the hatch's
 * origin the field pulls, outside the mouth for the floor's. acceleration is
 * what sink_speed gains each step */
static Float floor_hatch_pull = 1.5F;
static Float floor_hatch_reach = 130.0F;
static Float wall_hatch_pull = 2.0F;
static Float wall_hatch_reach = 130.0F;
static Float wall_hatch_acceleration = 0.1F;
static Float floor_hatch_acceleration = 0.1F;

static void wall_hatch_destroy(struct task *task);
static void wall_hatch_step(struct task *task);
static void wall_hatch_draw(struct task *task);
static void wall_hatch_draw_direct(struct task *task);
static void floor_hatch_destroy(struct task *task);
static void floor_hatch_step(struct task *task);
static void floor_hatch_draw(struct task *task);
static void floor_hatch_draw_direct(struct task *task);

Uint8 *hatches_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, models, HATCH_MODELS) == HATCH_MODELS) {
        models_ready = TRUE;
    }
    return buffer;
}

#define WALL_HATCH
#define PART(name) wall_hatch_##name
#include "stg24/hatch_field.h"
#undef PART
#undef WALL_HATCH
#define PART(name) floor_hatch_##name
#include "stg24/hatch_field.h"
#undef PART

#define WALL_HATCH
#define PART(name) wall_hatch_##name
#include "stg24/hatch_task_impl.h"
#define DRAW_SPRING() njCnkEasyDrawModel(&wall_hatch_spring_model)
#define DRAW_LID() njCnkEasyDrawModel(wall_hatch_lid_model)
#define DRAW_NAME wall_hatch_draw
#include "stg24/hatch_draw_impl.h"
#undef DRAW_NAME
#undef DRAW_LID
#undef DRAW_SPRING
#define DRAW_SPRING() njDirectDrawObject(models[WALL_SPRING].built)
#define DRAW_LID() njDirectDrawObject(models[WALL_LID].built)
#define DRAW_NAME wall_hatch_draw_direct
#include "stg24/hatch_draw_impl.h"
#undef DRAW_NAME
#undef DRAW_LID
#undef DRAW_SPRING
#undef PART
#undef WALL_HATCH

#define PART(name) floor_hatch_##name
#include "stg24/hatch_task_impl.h"
#define DRAW_SPRING() njDirectDrawObject(models[FLOOR_SPRING].built)
#define DRAW_LID() njDirectDrawObject(models[FLOOR_LID].built)
#define DRAW_NAME floor_hatch_draw_direct
#include "stg24/hatch_draw_impl.h"
#undef DRAW_NAME
#undef DRAW_LID
#undef DRAW_SPRING
#define DRAW_SPRING() njCnkEasyDrawModel(&floor_hatch_spring_model)
#define DRAW_LID() njCnkEasyDrawModel(floor_hatch_lid_model)
#define DRAW_NAME floor_hatch_draw
#include "stg24/hatch_draw_impl.h"
