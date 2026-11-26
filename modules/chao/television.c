/* the garden's television, which a Chao turns on and sits to watch */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/broadcast.h"
#include "chao/garden_field.h"
#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "sound/adx.h"
#include "system/task.h"
#include "system/util.h"

enum tv_mode {
    TV_OFF,
    TV_START,
    TV_WARM_UP,
    TV_STOP,
    TV_FADE_OUT,
    TV_START_STATIC,
    TV_STATIC,
    TV_START_SPORTS,
    TV_SPORTS,
    TV_START_CARTOON,
    TV_CARTOON,
    TV_START_DRAMA,
    TV_DRAMA
};

/* clang-format off */
static const struct collision_shape tv_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNTURNED, {0.0F, 1.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0.0F, {0, 0, 0}};
/* clang-format on */

/* each sequence's frames, as textures of garden_toy_texlist */
static Sint16 sports_frames[] = {86, 92, 93, 94, 95};
static Sint16 cartoon_frames[] = {96, 97, 98};
static Sint16 drama_frames[] = {99, 100, 101};
static Sint16 startup_frames[] = {105, 104, 103, 102};
static Sint16 shutdown_frames[] = {102, 103, 104, 105};
static Sint16 static_frames[] = {106, 107};
struct task *garden_television = NULL;

int garden_tv_mode(void)
{
    if (!garden_television) {
        return -1;
    }
    return garden_television->work->mode;
}

BROADCAST_SEAT(garden_tv_seat, garden_television)

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side. it sets the textures
 * of the screen's model, a plain chunk model, as a scenery mesh's */
#define SET_SCREEN(texture)                                                    \
    GARDEN_ANIMATE_TEXTURE(garden_tv_screen[0].model, (texture))
#else
/* @bug memory: the screen's model is a plain chunk model, which
 * garden_set_model_texture takes for a garden_model. it reads saved_textures
 * past the model's end at 0x0c510130, and where their first slot is empty,
 * it writes them at 0x0c510142 */
#define SET_SCREEN(texture)                                                    \
    garden_set_model_texture(garden_tv_screen, (texture))
#endif

/* the screen's texture on a new television */
#define NEW_SCREEN_TEXTURE 104
/* the index of the frame the screen shows in its sequence */
#define FRAME(work) ((work)->ang.x)
#define LAST_FRAME(frames) ((int)ARRAY_COUNT(frames) - 1)

/* a sequence moves to its next frame once tick passes its interval */
#define FADE_INTERVAL 4
#define STATIC_INTERVAL 1
#define SPORTS_INTERVAL 8
#define CARTOON_INTERVAL 12
#define DRAMA_INTERVAL 150

#define START_SEQUENCE(work, frames)                                           \
    {                                                                          \
        FRAME(work) = 0;                                                       \
        SET_SCREEN((frames)[FRAME(work)]);                                     \
        (work)->mode++;                                                        \
        (work)->submode = 0;                                                   \
    }

#define LOOP_SEQUENCE(work, frames, interval)                                  \
    if ((work)->tick++ > (interval)) {                                         \
        (work)->tick = 0;                                                      \
        FRAME(work)++;                                                         \
        if (FRAME(work) > LAST_FRAME(frames)) {                                \
            FRAME(work) = 0;                                                   \
        }                                                                      \
        SET_SCREEN((frames)[FRAME(work)]);                                     \
    }

static void tv_step(struct task *television)
{
    struct task_work *work = television->work;
    Float choice;

    switch (work->mode) {
    case TV_OFF:
        break;
    case TV_START:
        START_SEQUENCE(work, startup_frames);
        /* fall through */
    case TV_WARM_UP:
        if (work->tick++ > FADE_INTERVAL) {
            work->tick = 0;
            FRAME(work)++;
            if (FRAME(work) > LAST_FRAME(startup_frames)) {
                choice = rand() * RAND_SCALE;
                FRAME(work) = LAST_FRAME(startup_frames);
                if (choice < 0.3F) {
                    GARDEN_TOY_MODE(work, TV_START_SPORTS);
                    jingle_play("chao_g_tv_sports.adx");
                } else if (choice < 0.6F) {
                    GARDEN_TOY_MODE(work, TV_START_CARTOON);
                    jingle_play("chao_g_tv_cartoon.adx");
                } else if (choice < 0.9F) {
                    GARDEN_TOY_MODE(work, TV_START_DRAMA);
                    jingle_play("chao_g_tv_drama.adx");
                } else {
                    GARDEN_TOY_MODE(work, TV_START_STATIC);
                }
            }
            SET_SCREEN(startup_frames[FRAME(work)]);
        }
        break;
    case TV_STOP:
        FRAME(work) = 0;
        SET_SCREEN(shutdown_frames[FRAME(work)]);
        music_drop_played();
        work->mode++;
        work->submode = 0;
        /* fall through */
    case TV_FADE_OUT:
        if (work->tick++ > FADE_INTERVAL) {
            work->tick = 0;
            FRAME(work)++;
            if (FRAME(work) > LAST_FRAME(shutdown_frames)) {
                FRAME(work) = LAST_FRAME(shutdown_frames);
                GARDEN_TOY_MODE(work, TV_OFF);
            }
            SET_SCREEN(shutdown_frames[FRAME(work)]);
        }
        break;
    case TV_START_STATIC:
        START_SEQUENCE(work, static_frames);
        /* fall through */
    case TV_STATIC:
        LOOP_SEQUENCE(work, static_frames, STATIC_INTERVAL);
        if (collision_first_with_id(television, GARDEN_TOY_NEXT)) {
            GARDEN_TOY_MODE(work, TV_START_CARTOON);
        }
        break;
    case TV_START_SPORTS:
        START_SEQUENCE(work, sports_frames);
        /* fall through */
    case TV_SPORTS:
        LOOP_SEQUENCE(work, sports_frames, SPORTS_INTERVAL);
        if (collision_first_with_id(television, GARDEN_TOY_NEXT)) {
            GARDEN_TOY_MODE(work, TV_START_CARTOON);
        }
        break;
    case TV_START_CARTOON:
        START_SEQUENCE(work, cartoon_frames);
        /* fall through */
    case TV_CARTOON:
        LOOP_SEQUENCE(work, cartoon_frames, CARTOON_INTERVAL);
        if (collision_first_with_id(television, GARDEN_TOY_NEXT)) {
            GARDEN_TOY_MODE(work, TV_START_DRAMA);
        }
        break;
    case TV_START_DRAMA:
        START_SEQUENCE(work, drama_frames);
        /* fall through */
    case TV_DRAMA:
        if (work->tick++ > DRAMA_INTERVAL) {
            work->tick = 0;
            /* @hack type: the cast to Uint16, where the int would do */
            FRAME(work) = (Uint16)(rand() * (2.9F * RAND_SCALE));
            SET_SCREEN(drama_frames[FRAME(work)]);
        }
        if (collision_first_with_id(television, GARDEN_TOY_NEXT)) {
            GARDEN_TOY_MODE(work, TV_START_SPORTS);
        }
        break;
    }
    switch (work->mode) {
    case TV_OFF:
        if (collision_first_with_id(television, GARDEN_TOY_ACTIVATE)) {
            GARDEN_TOY_MODE(work, TV_START);
            garden_timed_enemy_field(GARDEN_TV_SIGNAL, &work->pos,
                                     BROADCAST_RADIUS, BROADCAST_FRAMES);
        }
        break;
    default:
        if (work->timer++ > BROADCAST_FRAMES) {
            GARDEN_TOY_MODE(work, TV_STOP);
        }
        break;
    }
    collision_register(television);
}

static void tv_draw(struct task *television)
{
    struct task_work *work = television->work;

    if (garden_visible_at_height(television, 3.0F, 1.0F)) {
        njSetTexture(&garden_toy_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, -1.5F, 0.0F);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawObject(&garden_tv_model);
        if (((struct garden_entry *)television->force.p)->distance <
            chao_tuning.object_shadow_distance) {
            njTranslate(NULL, 0.0F, 0.5F, 0.0F);
            njScale(NULL, 2.0F, 1.0F, 1.5F);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrixEx();
    }
}

static void tv_destroy(struct task *television)
{
    garden_unregister_object(television);
    garden_television = NULL;
}

void garden_television_create(NJS_POINT3 *position, Angle yaw)
{
    struct task *television =
        task_create(TASK_WORK, 4, tv_step, "ALO_TVExecutor");
    struct task_work *work;
    /* @hack redundant: point, which nothing reads */
    NJS_POINT3 point; /* NOLINT(clang-diagnostic-unused-variable) */

    garden_television = television;
    work = television->work;
    collision_init(television, &tv_shape, 1, COLLISION_OBJECT);
    garden_register_object(GARDEN_OBJECT_TOY, television, GARDEN_TOY_TV);
    work->pos = *position;
    work->ang.y = yaw;
    /* @hack spelling: the texture stored in FRAME's word and read back,
     * where SET_SCREEN(NEW_SCREEN_TEXTURE) would do */
    work->ang.x = NEW_SCREEN_TEXTURE;
    SET_SCREEN(work->ang.x);
    television->draw = tv_draw;
    television->destructor = tv_destroy;
}
