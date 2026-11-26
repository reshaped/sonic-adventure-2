#include "shared/deck_signal/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/deck_signal.h"
#include "shared/dust.h"
#include "shared/vertex_colors.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's label for ang.y, which no code reads */
static const char editor_rotation_y[12] = {
#include "shared/deck_signal/editor_rotation_y.inc"
};

/* TILT runs from 0 for an upright signal to 1 for one tilted tilt_degrees.
 * TILT_SPEED is its speed a step. they are Floats kept in the pointers
 * task->force and task->extra. the destructor clears both words, which
 * task_destroy would free */
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define TILT(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define TILT_SPEED(task) (*(Float *)&(task)->extra)
/* each step a rocking signal keeps TILT_DAMPING of its speed and gains
 * tilt_acceleration. at either end it turns back at TILT_BOUNCE of its
 * speed. it rolls about z by ROLL_SHARE of its tilt */
#define TILT_DAMPING 0.99F
#define TILT_BOUNCE 0.7F
#define ROLL_SHARE 0.5F
/* the steps from the hit, in the work's mode, or 0 before it */
#define HIT_STEPS(work) ((work)->mode)
/* until HIT_STEPS reaches DUST_STEPS, dust rises at a chance of DUST_CHANCE
 * a step. it rises at DUST_RISE from DUST_HEIGHT above pos. its scale is
 * DUST_SCALE and up to DUST_SCALE_RANGE more */
#define DUST_CHANCE 0.3F
#define DUST_STEPS 60
#define DUST_HEIGHT 26.0F
#define DUST_RISE 1.0F
#define DUST_SCALE 2.0F
#define DUST_SCALE_RANGE 6.0F
#define DUST_DEPTH_BIAS 3.0F
/* the lamp's blink, in steps of game_state.steps. until the hit, the lamp is
 * white through the first half of each BLINK_STEPS. it is gray through the
 * second half, and from the hit on */
#define BLINK_STEPS 40
#define LAMP_ON 0xffffffff
#define LAMP_OFF 0xff808080
#define HIT_SOUND 3
#define HIT_POINTS 20

static NJS_TEXNAME deck_signal_textures[6] = {
#include "shared/deck_signal/deck_signal_textures.inc"
};

static NJS_TEXLIST deck_signal_texlist = {
#include "shared/deck_signal/deck_signal_texlist.inc"
};

static Sint16 deck_signal_lens_polygons[25] = {
#include "shared/deck_signal/deck_signal_lens_polygons.inc"
};

static Sint32 deck_signal_lens_vertices[19] = {
#include "shared/deck_signal/deck_signal_lens_vertices.inc"
};

static NJS_CNK_MODEL deck_signal_lens_model = {
#include "shared/deck_signal/deck_signal_lens_model.inc"
};

static NJS_CNK_OBJECT deck_signal_lens = {
#include "shared/deck_signal/deck_signal_lens.inc"
};

static Sint16 deck_signal_polygons[321] = {
#include "shared/deck_signal/deck_signal_polygons.inc"
};

static Sint32 deck_signal_vertices[347] = {
#include "shared/deck_signal/deck_signal_vertices.inc"
};

static NJS_CNK_MODEL deck_signal_model = {
#include "shared/deck_signal/deck_signal_model.inc"
};

static NJS_CNK_OBJECT deck_signal_object = {
#include "shared/deck_signal/deck_signal_object.inc"
};

NJS_TEXLIST *deck_signal_texlists[2] = {
#include "shared/deck_signal/deck_signal_texlists.inc"
};
static struct collision_shape post = {
#include "shared/deck_signal/post.inc"
};
static Float tilt_acceleration = 0.008F;
static Float tilt_degrees = 25.0F;

static void deck_signal_destroy(struct task *task);
static void deck_signal_step(struct task *task);
static void deck_signal_draw(struct task *task);

void deck_signal_init(struct task *task)
{
    struct task_work *work = task->work;
    if (!placement_try_leave(task)) {
        task->draw = deck_signal_draw;
        task->step = deck_signal_step;
        task->destructor = deck_signal_destroy;
        collision_init(task, &post, 1, COLLISION_OBJECT);
        if (task->placement) {
            HIT_STEPS(work) = placement_get_state(task);
        }
    }
}

static void deck_signal_destroy(struct task *task)
{
    struct task_work *work = task->work;
    if (task->placement) {
        placement_set_state(task, HIT_STEPS(work));
    }
    task->force.p = NULL;
    task->extra.p = NULL;
}

static void deck_signal_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR velocity;
    NJS_POINT3 position;

    if (placement_try_leave(task)) {
        return;
    }
    if (!HIT_STEPS(work) && collision_next_attack(task)) {
        HIT_STEPS(work) = 1;
        score_add(HIT_POINTS);
        sound_play_at(HIT_SOUND, NULL, 0, 0, &work->pos);
    }
    if (HIT_STEPS(work)) {
        TILT_SPEED(task) =
            (TILT_SPEED(task) * TILT_DAMPING) + tilt_acceleration;
        TILT(task) += TILT_SPEED(task);
        if (TILT(task) > 1.0F) {
            TILT(task) = 1.0F;
            TILT_SPEED(task) = -(njAbs(TILT_SPEED(task)) * TILT_BOUNCE);
        }
        if (TILT(task) < 0.0F) {
            TILT(task) = 0.0F;
            TILT_SPEED(task) = njAbs(TILT_SPEED(task)) * TILT_BOUNCE;
        }
        if (HIT_STEPS(work) < DUST_STEPS) {
            if (rand() * RAND_SCALE < DUST_CHANCE) {
                position.x = 0.0F;
                position.y = DUST_HEIGHT;
                position.z = 0.0F;
                velocity.x = 0.0F;
                velocity.y = DUST_RISE;
                velocity.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, &position, &position);
                njPopMatrixEx();
                dust_steady_create(&position, &velocity,
                                   (rand() * RAND_SCALE * DUST_SCALE_RANGE) +
                                       DUST_SCALE,
                                   DUST_DEPTH_BIAS);
            }
            ++HIT_STEPS(work);
        }
    }
    collision_register(task);
}

static void deck_signal_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float tilt;
    Uint32 color;

    njSetTexture(&deck_signal_texlist);
    object = &deck_signal_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    if (HIT_STEPS(work)) {
        tilt = TILT(task) * tilt_degrees;
        njRotateX(NULL, (Angle)(-tilt * ANGLE_PER_DEGREE));
        njRotateZ(NULL, (Angle)(-tilt * ROLL_SHARE * ANGLE_PER_DEGREE));
    }
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    if (!HIT_STEPS(work) && game_state.steps % BLINK_STEPS < BLINK_STEPS / 2) {
        color = LAMP_ON;
    } else {
        color = LAMP_OFF;
    }
    vlist_set_color(object->model->vlist, color);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
