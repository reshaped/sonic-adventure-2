/* the race's jack-in-the-box. a racer's RACE_FIELD_ENTRANT field opens it,
 * and the luckier the racer, the less often the jack springs out. a jack
 * that springs out sets a RACE_FIELD_SURPRISE field ahead of the box as the
 * lid opens */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/garden_field.h"
#include "chao/life.h"
#include "chao/toy_art.h"
#include "chaostgrace/props.h"
#include "collision/collision.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum box_mode { BOX_WAIT = 0, BOX_OPEN = 2, BOX_HOLD = 3, BOX_CLOSE = 4 };
/* the box, and STOP_SHAPE, the sphere a running racer stops at, which goes
 * off once the box opens */
enum { BOX_SHAPE, STOP_SHAPE, BOX_SHAPES };
/* clang-format off */
static const struct collision_shape box_shapes[BOX_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 1.5F, 0.0F}, 1.2F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {(Sint8)RACE_FIELD_BOX, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 1.5F, 1.0F}, 1.5F, 0.0F, 0.0F, 0, {0, 0, 0}}};
/* clang-format on */
static Float spring_rest = 5.0F;
static Float spring_pull = 0.15F;
static Float spring_damping = 0.96F;
static Float jack_sink = 0.8F;

/* the jack springs out where a random draw below SPRING_DRAW falls under its
 * odds. the odds are SPRING_ODDS, less LUCK_WEIGHT for each point of the
 * racer's luck. the jack starts at the height JACK_START, and draws once
 * past it */
#define SPRING_ODDS 50000
#define LUCK_WEIGHT 4
#define SPRING_DRAW 120000.0F
#define JACK_START 0.01F
#define LID_OPEN_STEP 0.3F
#define LID_CLOSE_STEP 0.1F
#define HOLD_DELAY 60
/* the lid frame past which the jack's surprise field appears */
#define SURPRISE_FRAME 1.0F
#define SURPRISE_AHEAD 2.0F
#define SURPRISE_Y 1.0F
#define SURPRISE_RADIUS 2.0F
#define SURPRISE_FRAMES 2
/* the lid frame below which a closing lid shrinks the jack */
#define RETRACT_FRAME 10.0F
/* the steps of the angles whose sines sway the jack */
#define PITCH_STEP 0x1000
#define ROLL_STEP 0x1300
#define JACK_SWAY (FULL_TURN / 12.0F)
/* the jack grows with its height, from SMALLEST_SCALE by SCALE_PER_HEIGHT
 * to GROWN_SCALE at GROWN_HEIGHT */
#define SMALLEST_SCALE 0.5F
#define SCALE_PER_HEIGHT 0.5F
#define GROWN_HEIGHT 2.0F
#define GROWN_SCALE 1.5F
#define BOX_AHEAD 3.0F
#define LID_SHUT 0.01F
#define SHADOW_Y 0.5F
#define SHADOW_WIDTH 1.5F
#define LID_FRAME(work) ((work)->scl.x)
#define JACK_HEIGHT(work) ((work)->scl.y)
#define JACK_SPEED(work) ((work)->scl.z)

#define BOX_PHASE(work, phase)                                                 \
    do {                                                                       \
        (work)->mode = (phase);                                                \
        (work)->submode = 0;                                                   \
        (work)->timer = 0;                                                     \
        (work)->tick = 0;                                                      \
    } while (0)

static void box_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *contact;
    Uint16 luck;
    NJS_POINT3 position;

    switch (work->mode) {
    /* @hack spelling: the default first, where last would do */
    default:
        break;
    case BOX_WAIT:
        contact = collision_first_with_id(task, RACE_FIELD_ENTRANT);
        if (contact) {
            if (contact->parent) {
                contact = contact->parent;
                luck = chao_skill_points(contact, CHAO_LUCK);
                if ((Float)(SPRING_ODDS - (luck * LUCK_WEIGHT)) >
                    rand() * (SPRING_DRAW * RAND_SCALE)) {
                    JACK_HEIGHT(work) = JACK_START;
                    JACK_SPEED(work) = 0.0F;
                    LID_FRAME(work) = 0.0F;
                }
                collision_shape_off(task, STOP_SHAPE);
                BOX_PHASE(work, BOX_OPEN);
            }
        }
        break;
    case BOX_OPEN:
        LID_FRAME(work) += LID_OPEN_STEP;
        if (!work->submode) {
            if (LID_FRAME(work) > SURPRISE_FRAME) {
                if (JACK_HEIGHT(work) > 0.0F) {
                    position.x =
                        work->pos.x + (njSin(work->ang.y) * SURPRISE_AHEAD);
                    position.y = work->pos.y + SURPRISE_Y;
                    position.z =
                        work->pos.z + (njCos(work->ang.y) * SURPRISE_AHEAD);
                    garden_timed_enemy_field(RACE_FIELD_SURPRISE, &position,
                                             SURPRISE_RADIUS, SURPRISE_FRAMES);
                }
                work->submode = 1;
            }
        }
        if (LID_FRAME(work) > garden_jack_lid_motion->nbFrame - 1.0F) {
            LID_FRAME(work) = garden_jack_lid_motion->nbFrame - 1.0F;
            BOX_PHASE(work, BOX_HOLD);
        }
        break;
    case BOX_HOLD:
        if (work->timer++ > HOLD_DELAY) {
            BOX_PHASE(work, BOX_CLOSE);
        }
        break;
    case BOX_CLOSE:
        LID_FRAME(work) -= LID_CLOSE_STEP;
        if (LID_FRAME(work) < 0.0F) {
            LID_FRAME(work) = 0.0F;
            JACK_HEIGHT(work) = 0.0F;
            JACK_SPEED(work) = 0.0F;
            work->ang.x = 0;
            work->ang.z = 0;
            BOX_PHASE(work, BOX_WAIT);
        }
        break;
    }
    switch (work->mode) {
    case BOX_OPEN:
    case BOX_HOLD:
        if (JACK_HEIGHT(work) > 0.0F) {
            JACK_SPEED(work) += spring_pull * (spring_rest - JACK_HEIGHT(work));
            JACK_SPEED(work) *= spring_damping;
            JACK_HEIGHT(work) += JACK_SPEED(work);
            work->ang.x += PITCH_STEP;
            work->ang.z += ROLL_STEP;
        }
        break;
    case BOX_CLOSE:
        if (JACK_HEIGHT(work) > 0.0F && LID_FRAME(work) < RETRACT_FRAME) {
            JACK_SPEED(work) = 0.0F;
            JACK_HEIGHT(work) *= jack_sink;
        }
        work->ang.x += PITCH_STEP;
        work->ang.z += ROLL_STEP;
        break;
    }
    collision_register(task);
}

static void box_draw(struct task *task)
{
    struct task_work *work = task->work;
    Angle pitch;
    Angle roll;
    Float scale;
    njSetTexture(&garden_toy_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, 0.0F, BOX_AHEAD);
    njRotateY(NULL, HALF_TURN);
    if (LID_FRAME(work) < LID_SHUT) {
        njCnkEasyDrawObject(garden_jack_box);
    } else {
        njCnkEasyDrawMotion(garden_jack_box, garden_jack_lid_motion,
                            LID_FRAME(work));
    }
    njPopMatrixEx();
    if (JACK_HEIGHT(work) > JACK_START) {
        pitch = njSin(work->ang.x) * JACK_SWAY;
        roll = njSin(work->ang.z) * JACK_SWAY;
        if (JACK_HEIGHT(work) < GROWN_HEIGHT) {
            scale = SMALLEST_SCALE + (JACK_HEIGHT(work) * SCALE_PER_HEIGHT);
        } else {
            scale = GROWN_SCALE;
        }
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, JACK_HEIGHT(work), 0.0F);
        njRotateX(NULL, pitch);
        njRotateZ(NULL, roll);
        njRotateY(NULL, HALF_TURN);
        njScale(NULL, scale, scale, scale);
        njCnkSimpleDrawModel(&garden_jack_model);
        njPopMatrixEx();
    }
    njTranslate(NULL, 0.0F, SHADOW_Y, 0.0F);
    njScale(NULL, SHADOW_WIDTH, 1.0F, SHADOW_WIDTH);
    njCnkModDrawModel(&garden_shadow_model);
    njPopMatrixEx();
}

static void box_destroy(struct task *task)
{
}

void race_box_init(struct task *task)
{
    struct task_work *work = task->work;
    task->step = box_step;
    task->draw = box_draw;
    task->destructor = box_destroy;
    collision_init(task, box_shapes, BOX_SHAPES, COLLISION_OBJECT);
    JACK_HEIGHT(work) = 0.0F;
    JACK_SPEED(work) = 0.0F;
    work->ang.x = 0;
    work->ang.z = 0;
}

static const char box_editor_yaw[] = "<- ANGLE Y ROTATION";
