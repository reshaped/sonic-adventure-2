/* the flute stand, KNUDAI in a stage's object list */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "hud/message_window.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/knudai.h"
#include "shared/messenger.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "shared/knudai/text.inc"

#define KNUDAI_ID(work, mask) (((work)->ang.x & (mask)) % KNUDAI_IDS)
#define KNUDAI_BIT(work) (1 << KNUDAI_ID(work, KNUDAI_ID_MASK))

/* the stand's size class in the collision world is measured by its
 * collision model's radius plus WORLD_MARGIN */
#define WORLD_MARGIN 32.0F
/* the world collects the stand's copy only while a player lies near: within
 * the copy's radius plus REACH_MARGIN */
#define REACH_MARGIN 32.0F

/* a player is at the stand within NEAR_RANGE of pos, as player_near
 * measures */
#define NEAR_RANGE 38.0F

#define FLUTE_SOUND 0x101e
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define SOUND_DURATION 160 /* in sound updates */

/* the work's tick counts the steps that find a player at the stand, up to
 * TICK_MAX. a message opens only where the tick is above MESSAGE_TICKS. each
 * step that tests the tick for a message zeroes it after the test. the step for
 * PLAYER_MODE_PLAY_FLUTE zeroes it just before */
#define TICK_MAX 30
#define MESSAGE_TICKS 10

/* the texts messenger_knudai_texts returns hold CHARACTER_TEXTS for each
 * character, in the order of the PLAYER_ characters. TEXT_CANT_PLAY picks a
 * character's text for a player who can't play. TEXT_PLAY_AGAIN picks its text
 * for a play again. message_window_open shows each page of them for TEXT_STEPS
 * steps, unless the page's W code gives another count */
#define CHARACTER_TEXTS 2
#define TEXT_CANT_PLAY 0
#define TEXT_PLAY_AGAIN 1
#define TEXT_STEPS 600
/* the stand's own texts, for where messenger_knudai_texts returns null. in
 * CP932 they read "playing (not supported)", "can't play (no message either)"
 * and "only once (no message either)" */
#define STAND_IN_STEPS 200

struct knudai {
    /* the copy of knudai_collision the stand puts in the world, placed and
     * turned as its work is */
    NJS_OBJECT object;
    /* the window the stand opened last, or null once it has ended */
    struct task *message;
};

#define KNUDAI(task) ((struct knudai *)(task)->motion.p)

#define TEXTURES 4
static NJS_TEXNAME knudai_texnames[TEXTURES] = {
#include "shared/knudai/knudai_texnames.inc"
};
static NJS_TEXLIST knudai_texlist = {
#include "shared/knudai/knudai_texlist.inc"
};
#define KNUDAI_POINTS 128
static Sint16 knudai_polygons[899] = {
#include "shared/knudai/knudai_polygons.inc"
};
static Sint32 knudai_vertices[771] = {
#include "shared/knudai/knudai_vertices.inc"
};
static NJS_CNK_MODEL knudai_model = {
#include "shared/knudai/knudai_model.inc"
};
/* the model's object, which nothing reads */
/* The original member keeps this root alongside the stand model. */
static NJS_CNK_OBJECT knudai_object = {
#include "shared/knudai/knudai_object.inc"
};
#define KNUDAI_COLLISION_POINTS 56
/* SHC refuses braces around the material's two colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL knudai_collision_material = {
#include "shared/knudai/knudai_collision_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
struct knudai_collision_mesh_list {
    Uint16 mesh[12];
    Uint16 mesh_2[4];
    Uint16 mesh_3[4];
    Uint16 mesh_4[4];
    Uint16 mesh_5[6];
    Uint16 mesh_6[6];
    Uint16 mesh_7[7];
    Uint16 mesh_8[7];
    Uint16 mesh_9[11];
    Uint16 mesh_10[11];
    Uint16 mesh_11[7];
    Uint16 mesh_12[11];
    Uint16 mesh_13[7];
    Uint16 mesh_14[7];
    Uint16 mesh_15[7];
    Uint16 mesh_16[7];
    Uint16 mesh_17[7];
    Uint16 mesh_18[11];
};
static struct knudai_collision_mesh_list knudai_collision_meshes = {
#include "shared/knudai/knudai_collision_meshes.inc"
};
static NJS_MESHSET knudai_collision_meshset = {
#include "shared/knudai/knudai_collision_meshset.inc"
};
static NJS_POINT3 knudai_collision_points[KNUDAI_COLLISION_POINTS] = {
#include "shared/knudai/knudai_collision_points.inc"
};
static NJS_VECTOR knudai_collision_normals[KNUDAI_COLLISION_POINTS] = {
#include "shared/knudai/knudai_collision_normals.inc"
};
static NJS_MODEL knudai_collision_model = {
#include "shared/knudai/knudai_collision_model.inc"
};
static NJS_OBJECT knudai_collision = {
#include "shared/knudai/knudai_collision.inc"
};

static void knudai_destroy(struct task *task);
static void knudai_step(struct task *task);
static void knudai_draw(struct task *task);

/* each id's stand, as knudai.h has it, or null */
static struct task *knudai_tasks[KNUDAI_IDS] = {NULL};

NJS_TEXLIST *knudai_texlists[2] = {
#include "shared/knudai/knudai_texlists.inc"
};

int knudai_get_pos(int id, NJS_POINT3 *pos)
{
    if (id >= KNUDAI_IDS || id < 0) {
        return 0;
    }
    if (kept_object_bits & (1 << id)) {
        pos->x = 0.0F;
        pos->y = 0.0F;
        pos->z = 0.0F;
    } else {
        if (knudai_tasks[id] == NULL) {
            return 0;
        }
        *pos = knudai_tasks[id]->work->pos;
    }
    return 1;
}

int knudai_played(int id)
{
    if (id >= KNUDAI_IDS || id < 0) {
        return 0;
    }
    if (kept_object_bits & (1 << id)) {
        return 1;
    }
    if (knudai_tasks[id] == NULL) {
        return 0;
    }
    return knudai_tasks[id]->work->submode;
}

void knudai_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = knudai_draw;
    task->step = knudai_step;
    task->destructor = knudai_destroy;
    object = &KNUDAI(task)->object;
    *object = knudai_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_NO_CLIMB;
    radius = object->model->r + WORLD_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    if (knudai_tasks[KNUDAI_ID(work, KNUDAI_ID_MASK)] == NULL) {
        knudai_tasks[KNUDAI_ID(work, KNUDAI_ID_MASK)] = task;
    }
}

static void knudai_destroy(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *work = task->work;
    int i;

    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)KNUDAI_DEBUG_ID;
        (void)KNUDAI_DEBUG_YAW;
    }
    if (KNUDAI(task)->message != NULL) {
        task_destroy(KNUDAI(task)->message);
        voices_stop();
        KNUDAI(task)->message = NULL;
    }
    if (KNUDAI(task)->object.model != NULL) {
        world_remove(task, &KNUDAI(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    for (i = 0; i < KNUDAI_IDS; i++) {
        if (knudai_tasks[i] == task) {
            knudai_tasks[i] = NULL;
        }
    }
}

static void knudai_step(struct task *task)
{
    struct task_work *work = task->work;
    int near;
    int player;
    char **texts;
    int language;
    NJS_VECTOR to_stand;
    Angle turn;
    struct task_work *player_task_work;

    if (placement_try_leave(task)) {
        return;
    }
    near = player_near(&work->pos, NEAR_RANGE);
    /* @hack spelling: player set inside the test, where a statement of its
     * own would do */
    if (player = near - 1, near != 0) {
        texts = messenger_knudai_texts();
        if (texts != NULL) {
            int character = player_character(player);

            if (character >= 0) {
                texts += character * CHARACTER_TEXTS;
            }
        }
        language = game_state.text_language;
        if (!(kept_object_bits & KNUDAI_BIT(work))) {
            switch (player_work[player]->mode) {
            case PLAYER_MODE_PLAY_FLUTE:
                sound_start_at_for(FLUTE_SOUND, work, SOUND_PRIORITY,
                                   SOUND_VOLUME, SOUND_DURATION, &work->pos);
                kept_object_bits |= KNUDAI_BIT(work);
                work->tick = 0;
                /* @bug logic: the tick is zeroed just before its test, so
                 * STAND_IN_PLAYING never shows. the store is at 0x3be of the
                 * object's code, and the test at 0x3c4 */
                if (work->tick > MESSAGE_TICKS) {
                    /* @hack spelling: an empty arm for texts, where a test of
                     * texts == NULL alone would do */
                    if (texts != NULL) {
                    } else {
                        /* @hack spelling: game_state.text_language read again,
                         * where language would do */
                        message_window_open(MESSAGE_WINDOW_UNFOLD,
                                            STAND_IN_PLAYING, STAND_IN_STEPS,
                                            game_state.text_language,
                                            &KNUDAI(task)->message);
                    }
                }
                break;
            case PLAYER_MODE_PLAY_FLUTE_AGAIN:
                sound_start_at_for(FLUTE_SOUND, work, SOUND_PRIORITY,
                                   SOUND_VOLUME, SOUND_DURATION, &work->pos);
                /* fall through */
            case PLAYER_MODE_PLAYING_FLUTE_AGAIN:
                /* @bug logic: reads player 0's animation, where the mode the
                 * switch tests is that of the player at the stand. the branch
                 * for a stand played before does the same. player 0's record
                 * is read at 0x41a, 0x428, 0x522 and 0x54e of the object's
                 * code */
                if (work->tick > MESSAGE_TICKS &&
                    player_records[0]->anim.current == ANIM_FLUTE_AGAIN &&
                    player_records[0]->anim.last == ANIM_FLUTE) {
                    /* @hack spelling: game_state.text_language read again,
                     * where language would do */
                    if (texts != NULL) {
                        message_window_open(MESSAGE_WINDOW_UNFOLD,
                                            texts[TEXT_CANT_PLAY], TEXT_STEPS,
                                            game_state.text_language,
                                            &KNUDAI(task)->message);
                    } else {
                        message_window_open(MESSAGE_WINDOW_UNFOLD,
                                            STAND_IN_CANT_PLAY, STAND_IN_STEPS,
                                            game_state.text_language,
                                            &KNUDAI(task)->message);
                    }
                }
                work->tick = 0;
                break;
            case PLAYER_MODE_CANT_PLAY_FLUTE:
                if (work->tick > MESSAGE_TICKS) {
                    if (texts != NULL) {
                        message_window_open(MESSAGE_WINDOW_UNFOLD,
                                            texts[TEXT_CANT_PLAY], TEXT_STEPS,
                                            language, &KNUDAI(task)->message);
                    } else {
                        message_window_open(MESSAGE_WINDOW_UNFOLD,
                                            STAND_IN_CANT_PLAY, STAND_IN_STEPS,
                                            language, &KNUDAI(task)->message);
                    }
                }
                work->tick = 0;
                break;
            default:
                if (message_window_current() == NULL) {
                    player_records[player]->action = PLAYER_MODE_PLAY_FLUTE;
                }
                break;
            }
        } else {
            /* @hack spelling: the slot worked out again as near - 1, where
             * player would do */
            player_task_work = player_work[near - 1];
            if (player_task_work->mode != PLAYER_MODE_CANT_PLAY_FLUTE &&
                player_task_work->mode != PLAYER_MODE_PLAY_FLUTE_AGAIN &&
                player_task_work->mode != PLAYER_MODE_PLAYING_FLUTE_AGAIN) {
                if (message_window_current() == NULL) {
                    player_records[player]->action =
                        PLAYER_MODE_PLAY_FLUTE_AGAIN;
                }
            } else if (player_task_work->mode == PLAYER_MODE_PLAY_FLUTE_AGAIN ||
                       player_task_work->mode ==
                           PLAYER_MODE_PLAYING_FLUTE_AGAIN) {
                if (player_records[0]->anim.current == ANIM_FLUTE) {
                    sound_start_at_for(FLUTE_SOUND, work, SOUND_PRIORITY,
                                       SOUND_VOLUME, SOUND_DURATION,
                                       &work->pos);
                }
                if (player_records[0]->anim.current == ANIM_FLUTE_AGAIN) {
                    if (work->tick > MESSAGE_TICKS) {
                        /* @hack spelling: language read again inside the
                         * test of texts, where the language read above
                         * would do */
                        if (language = game_state.text_language,
                            texts != NULL) {
                            message_window_open(
                                MESSAGE_WINDOW_UNFOLD, texts[TEXT_PLAY_AGAIN],
                                TEXT_STEPS, language, &KNUDAI(task)->message);
                        } else {
                            message_window_open(MESSAGE_WINDOW_UNFOLD,
                                                STAND_IN_PLAY_AGAIN,
                                                STAND_IN_STEPS, language,
                                                &KNUDAI(task)->message);
                        }
                    }
                    work->tick = 0;
                }
            }
        }
        if (work->tick < TICK_MAX) {
            work->tick++;
        }
    }
    /* while the stand's window is open, the player at the stand turns toward
     * it. each step turns the player a third of the way, the shorter way
     * round */
    if (near != 0 && KNUDAI(task)->message != NULL) {
        player_task_work = player_work[player];
        /* @hack spelling: to_stand worked out through pointers that walk its
         * parts, where three assignments would do */
        {
            Float *out = (Float *)&to_stand;
            Float *stand_pos = &work->pos.x;
            Float *player_pos = &player_task_work->pos.x;

            *out++ = *stand_pos - *player_pos++;
            stand_pos++;
            *out++ = *stand_pos++ - *player_pos++;
            *out = *stand_pos - *player_pos;
        }
        /* @hack spelling: the turn worked out in two statements, without
         * -=, where one expression would do */
        turn = QUARTER_TURN -
               (Angle)(atan2f(to_stand.x, to_stand.z) * ANGLE_PER_RADIAN);
        turn = turn - player_task_work->ang.y;
        /* @hack spelling: the low half scaled through low and part, where
         * *(Sint16 *)&turn *= 1.0F / 3 would do */
        {
            Sint16 *low = (Sint16 *)&turn;
            Float part = *low;

            *low = part * (1.0F / 3);
        }
        player_task_work->ang.y += turn;
        player_motion[player]->aim_y = player_task_work->ang.y;
    }
    if (KNUDAI(task)->object.model != NULL) {
        /* @hack spelling: object, a local for the record's object, where the
         * record's object read in place would do */
        NJS_OBJECT *object = &KNUDAI(task)->object;

        if (player_near((NJS_POINT3 *)object->pos,
                        object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void knudai_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&knudai_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&knudai_model);
    njPopMatrixEx();
}
