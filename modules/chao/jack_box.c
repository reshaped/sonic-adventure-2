/* the garden's jack-in-the-box. a Chao's GARDEN_TOY_ACTIVATE field winds it
 * up, and its lid opens 30 to 150 frames later. the luckier the Chao, the
 * less often the jack springs out. a jack that springs out sets a
 * GARDEN_JACK_SPRING field on the box. each Chao the field reaches cries */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/garden_field.h"
#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/life.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum jack_mode {
    JACK_CLOSED,
    JACK_WIND_UP,
    JACK_OPENING,
    JACK_OPEN,
    JACK_CLOSING
};

/* clang-format off */
static const struct collision_shape jack_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNTURNED, {0.0F, 1.6F, 0.0F}, 1.6F, 0.0F, 0.0F, 0.0F, {0, 0, 0}};
/* clang-format on */

static struct task *garden_jack = NULL;
static Float spring_rest = 5.0F;
static Float spring_pull = 0.15F;
static Float spring_damping = 0.96F;
static Float jack_sink = 0.8F;

#define CRY_RADIUS 5.0F
#define CRY_FRAMES 2
/* the jack springs out where a random draw below SPRING_DRAW falls under its
 * odds. the odds are SPRING_ODDS, less LUCK_WEIGHT for each point of the
 * Chao's luck */
#define SPRING_ODDS 50000
#define LUCK_WEIGHT 4
#define SPRING_DRAW 80000.0F
/* the wind-up's frames, from WIND_LEAST to WIND_LEAST + WIND_CHOICES - 1 */
#define WIND_LEAST 30
#define WIND_CHOICES 121.0F
#define LID_SOUND 0x102a
#define OPEN_FRAMES 60
/* the steps of the angles whose sines sway the jack */
#define PITCH_STEP 0x1000
#define ROLL_STEP 0x1300
#define SWAY_DEGREES 30.0F
/* the jack grows with its height, to GROWN_SCALE at GROWN_HEIGHT */
#define GROWN_HEIGHT 2.0F
#define GROWN_SCALE 1.5F
#define BOX_AHEAD 3.0F
#define SHADOW_WIDTH 1.5F
#define LID_FRAME(work) ((work)->scl.x)
#define JACK_HEIGHT(work) ((work)->scl.y)
#define JACK_SPEED(work) ((work)->scl.z)

int garden_jack_mode(void)
{
    if (!garden_jack) {
        return -1;
    }
    return garden_jack->work->mode;
}

static void jack_step(struct task *box)
{
    struct task_work *work = box->work;

    switch (work->mode) {
    case JACK_CLOSED: {
        struct task *activator;
        Uint16 luck;

        activator = collision_first_with_id(box, GARDEN_TOY_ACTIVATE);
        if (activator && activator->parent) {
            activator = activator->parent;
            luck = chao_skill_points(activator, CHAO_LUCK);
            if ((Float)(SPRING_ODDS - (luck * LUCK_WEIGHT)) >
                rand() * (SPRING_DRAW * RAND_SCALE)) {
                JACK_HEIGHT(work) = 0.01F;
                JACK_SPEED(work) = 0.0F;
                LID_FRAME(work) = 0.0F;
            }
            /* @bug memory: turns off shape 1 of a box that has shape 0
             * alone, writing past its shapes (0x0c53dccc). the constructor
             * gives the box one shape (0x0c53e0c4) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it turns off no shape
             * past the box's one */
#else
            collision_shape_off(box, 1);
#endif
            GARDEN_TOY_MODE(work, JACK_WIND_UP);
            work->timer =
                (int)(rand() * (WIND_CHOICES * RAND_SCALE)) + WIND_LEAST;
        }
        break;
    }
    case JACK_WIND_UP:
        /* @hack spelling: <= 0 on the unsigned timer, where == 0 would do */
        if (--work->timer <= 0) {
            sound_play_at(LID_SOUND, NULL, 0, 0, &work->pos);
            GARDEN_TOY_MODE(work, JACK_OPENING);
        }
        break;
    case JACK_OPENING:
        LID_FRAME(work) += 0.3F;
        /* @hack spelling: two nested tests, where one test of both would
         * do */
        if (work->submode == 0) {
            if (LID_FRAME(work) > 1.0F) {
                if (JACK_HEIGHT(work) > 0.0F) {
                    garden_timed_enemy_field(GARDEN_JACK_SPRING, &work->pos,
                                             CRY_RADIUS, CRY_FRAMES);
                }
                work->submode = 1;
            }
        }
        if (LID_FRAME(work) > garden_jack_lid_motion->nbFrame - 1.0F) {
            LID_FRAME(work) = garden_jack_lid_motion->nbFrame - 1.0F;
            GARDEN_TOY_MODE(work, JACK_OPEN);
        }
        break;
    case JACK_OPEN:
        if (work->timer++ > OPEN_FRAMES) {
            GARDEN_TOY_MODE(work, JACK_CLOSING);
        }
        break;
    case JACK_CLOSING:
        LID_FRAME(work) -= 0.1F;
        if (LID_FRAME(work) < 0.0F) {
            LID_FRAME(work) = 0.0F;
            JACK_HEIGHT(work) = 0.0F;
            JACK_SPEED(work) = 0.0F;
            work->ang.x = 0;
            work->ang.z = 0;
            GARDEN_TOY_MODE(work, JACK_CLOSED);
        }
        break;
    }
    switch (work->mode) {
    case JACK_OPENING:
    case JACK_OPEN:
        if (JACK_HEIGHT(work) > 0.0F) {
            JACK_SPEED(work) += spring_pull * (spring_rest - JACK_HEIGHT(work));
            JACK_SPEED(work) *= spring_damping;
            JACK_HEIGHT(work) += JACK_SPEED(work);
            work->ang.x += PITCH_STEP;
            work->ang.z += ROLL_STEP;
        }
        break;
    case JACK_CLOSING:
        if (JACK_HEIGHT(work) > 0.0F && LID_FRAME(work) < 10.0F) {
            JACK_SPEED(work) = 0.0F;
            JACK_HEIGHT(work) *= jack_sink;
        }
        work->ang.x += PITCH_STEP;
        work->ang.z += ROLL_STEP;
        break;
    }
    collision_register(box);
}

static void jack_draw(struct task *box)
{
    struct task_work *work = box->work;
    NJS_CNK_OBJECT *model;
    Angle pitch;
    Angle roll;
    Float scale;

    if (garden_visible_at_height(box, 2.5F, 1.0F)) {
        njSetTexture(&garden_toy_texlist);
        /* @hack redundant: a second njSetTexture of the same list */
        njSetTexture(&garden_toy_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, BOX_AHEAD);
        njRotateY(NULL, HALF_TURN);
        /* @hack spelling: the model's assignment inside the test, where a
         * statement of its own would do */
        if (model = garden_jack_box, LID_FRAME(work) < 0.01F) {
            njCnkEasyDrawObject(model);
        } else {
            njCnkEasyDrawMotion(model, garden_jack_lid_motion, LID_FRAME(work));
        }
        njPopMatrixEx();
        if (JACK_HEIGHT(work) > 0.01F) {
            pitch = njSin(work->ang.x) * (SWAY_DEGREES * ANGLE_PER_DEGREE);
            roll = njSin(work->ang.z) * (SWAY_DEGREES * ANGLE_PER_DEGREE);
            if (JACK_HEIGHT(work) < GROWN_HEIGHT) {
                scale = (JACK_HEIGHT(work) * 0.5F) + 0.5F;
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
        njTranslate(NULL, 0.0F, 0.5F, 0.0F);
        njScale(NULL, SHADOW_WIDTH, 1.0F, SHADOW_WIDTH);
        njCnkModDrawModel(&garden_shadow_model);
        njPopMatrixEx();
    }
}

static void jack_destroy(struct task *box)
{
    garden_unregister_object(box);
    garden_jack = NULL;
}

void garden_jack_in_box_create(NJS_POINT3 *position, Angle yaw)
{
    struct task *box = task_create(TASK_WORK, 4, jack_step, "ALO_BoxExecutor");
    struct task_work *work;

    garden_jack = box;
    work = box->work;
    collision_init(box, &jack_shape, 1, COLLISION_OBJECT);
    garden_register_object(GARDEN_OBJECT_TOY, box, GARDEN_TOY_JACK);
    work->pos = *position;
    work->ang.y = yaw;
    JACK_HEIGHT(work) = 0.0F;
    JACK_SPEED(work) = 0.0F;
    work->ang.x = 0;
    work->ang.z = 0;
    box->draw = jack_draw;
    box->destructor = jack_destroy;
}
