#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/broadcast.h"
#include "chao/garden_field.h"
#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define PULSE_STEP 0xc00

enum radio_mode {
    RADIO_OFF,
    RADIO_START,
    RADIO_STOP,
    RADIO_TRACK_1,
    RADIO_TRACK_2,
    RADIO_TRACK_3,
    RADIO_TRACK_4
};

/* clang-format off */
static const struct collision_shape radio_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNTURNED, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0.0F, {0, 0, 0}};
/* clang-format on */

struct task *garden_radio = NULL;

int garden_radio_mode(void)
{
    if (!garden_radio) {
        return -1;
    }
    return garden_radio->work->mode;
}

BROADCAST_SEAT(garden_radio_seat, garden_radio)

#define PLAY_TRACK(work, mode, track)                                          \
    {                                                                          \
        GARDEN_TOY_MODE((work), (mode));                                       \
        jingle_play(track);                                                    \
    }

static void radio_step(struct task *radio)
{
    struct task_work *work = radio->work;
    Float choice;

    switch (work->mode) {
    case RADIO_OFF:
        break;
    case RADIO_START:
        choice = rand() * RAND_SCALE;
        if (choice < 0.25F) {
            PLAY_TRACK(work, RADIO_TRACK_1, "chao_g_radicase1.adx");
        } else if (choice < 0.5F) {
            PLAY_TRACK(work, RADIO_TRACK_2, "chao_g_radicase2.adx");
        } else if (choice < 0.75F) {
            PLAY_TRACK(work, RADIO_TRACK_3, "chao_g_radicase3.adx");
        } else {
            PLAY_TRACK(work, RADIO_TRACK_4, "chao_g_radicase4.adx");
        }
        break;
    case RADIO_STOP:
        music_drop_played();
        GARDEN_TOY_MODE(work, RADIO_OFF);
        break;
    case RADIO_TRACK_1:
        if (collision_first_with_id(radio, GARDEN_TOY_NEXT)) {
            PLAY_TRACK(work, RADIO_TRACK_2, "chao_g_radicase2.adx");
        }
        break;
    case RADIO_TRACK_2:
        if (collision_first_with_id(radio, GARDEN_TOY_NEXT)) {
            PLAY_TRACK(work, RADIO_TRACK_3, "chao_g_radicase3.adx");
        }
        break;
    case RADIO_TRACK_3:
        if (collision_first_with_id(radio, GARDEN_TOY_NEXT)) {
            PLAY_TRACK(work, RADIO_TRACK_4, "chao_g_radicase4.adx");
        }
        break;
    case RADIO_TRACK_4:
        if (collision_first_with_id(radio, GARDEN_TOY_NEXT)) {
            PLAY_TRACK(work, RADIO_TRACK_1, "chao_g_radicase1.adx");
        }
        break;
    }
    switch (work->mode) {
    case RADIO_OFF:
        if (collision_first_with_id(radio, GARDEN_TOY_ACTIVATE)) {
            GARDEN_TOY_MODE(work, RADIO_START);
            garden_timed_enemy_field(GARDEN_RADIO_SIGNAL, &work->pos,
                                     BROADCAST_RADIUS, BROADCAST_FRAMES);
        }
        work->ang.x = angle_approach(work->ang.x, 0, PULSE_STEP);
        break;
    default:
        if (work->timer++ > BROADCAST_FRAMES) {
            GARDEN_TOY_MODE(work, RADIO_STOP);
        }
        work->ang.x += PULSE_STEP;
        break;
    }
    collision_register(radio);
}

static void radio_draw(struct task *radio)
{
    struct task_work *work = radio->work;
    Float pulse = (njSin(work->ang.x) * 0.05F) + 1.0F;

    if (garden_visible_at_height(radio, 3.0F, 1.0F)) {
        njSetTexture(&garden_toy_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, -1.5F, 0.0F);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, pulse, 2.0F - pulse, 1.0F);
        njCnkEasyDrawObject(&garden_radio_model);
        if (((struct garden_entry *)radio->force.p)->distance <
            chao_tuning.object_shadow_distance) {
            njTranslate(NULL, 0.0F, 0.5F, 0.0F);
            njScale(NULL, 2.0F, 1.0F, 1.5F);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrixEx();
    }
}

static void radio_destroy(struct task *radio)
{
    garden_unregister_object(radio);
    garden_radio = NULL;
}

void garden_radio_create(NJS_POINT3 *position, Angle yaw)
{
    struct task *radio =
        task_create(TASK_WORK, 4, radio_step, "ALO_RadicaseExecutor");
    struct task_work *work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused; /* NOLINT(clang-diagnostic-unused-variable) */

    garden_radio = radio;
    work = radio->work;
    collision_init(radio, &radio_shape, 1, COLLISION_OBJECT);
    garden_register_object(GARDEN_OBJECT_TOY, radio, GARDEN_TOY_RADIO);
    work->pos = *position;
    work->ang.y = yaw;
    radio->draw = radio_draw;
    radio->destructor = radio_destroy;
}
