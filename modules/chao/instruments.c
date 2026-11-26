#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/body.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/instrument_models.h"
#include "chao/instruments.h"
#include "chao/intention.h"
#include "chao/knowledge.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/sound.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define BAND_FIELD 165
#define BAND_AHEAD 8.0F
#define BAND_RADIUS 20.0F
/* a band's bid scores a boredom below LIFT_BELOW as BOREDOM_LIFT higher */
#define BOREDOM_LIFT 3000
#define LIFT_BELOW 7000
/* the boredom and loneliness a Chao loses as it joins a band, and the
 * loneliness its starter loses */
#define BAND_RELIEF 4000
#define BAND_BID 0.99F
/* the sound id of each instrument's note is NOTE_SOUNDS plus its enum
 * chao_instrument */
#define NOTE_SOUNDS 0x4006
/* the last of the five glad voices a pleased Chao gives */
#define GLAD_VOICE 0x6033
/* PICK_LEARNED picks one of known's first count entries at random. its
 * draw's top is count less PICK_MARGIN, so the index stays below count */
#define PICK_MARGIN 0.00001F
#define PICK_LEARNED(known, count)                                             \
    ((known)[(int)(rand() * ((count) - PICK_MARGIN) * RAND_SCALE)])
/* BEHAVIOR_FINISH, 1, once no band's field reaches the Chao. until then
 * BEHAVIOR_CONTINUE, 0 */
#define BAND_ENDED(chao) (!garden_hit_kind((chao), CHAO_PUSH_SHAPE, BAND_FIELD))
#define PLAY_NOTE(chao, instrument, volume)                                    \
    sound_play_at(NOTE_SOUNDS + (instrument), NULL, 0, (volume),               \
                  &(chao)->work->pos)
#define SET_EXPRESSION(chao, eye, mouth)                                       \
    chao_set_timed_eye((chao), (eye), CHAO_FACE_HOLD);                         \
    chao_set_timed_mouth((chao), (mouth), CHAO_FACE_HOLD)
#define PLAY_FOR(behavior, frames)                                             \
    (behavior)->timer = (frames);                                              \
    (behavior)->mode++

/* behavior->mode in each band behavior. listen_to_band claps in
 * BAND_APPLAUD once the band has ended */
enum band_mode { BAND_START, BAND_PLAY, BAND_APPLAUD };

/* instrument_placements's entries. no code reads the trumpet's */
enum instrument_placement {
    BELL_PLACEMENT,
    FLUTE_PLACEMENT,
    CASTANETS_PLACEMENT,
    TRUMPET_PLACEMENT,
    INSTRUMENT_PLACEMENTS
};

static int play_bell(struct task *chao);
static int play_castanets(struct task *chao);
static int play_cymbals(struct task *chao);
static int play_drum(struct task *chao);
static int play_flute(struct task *chao);
static int play_maracas(struct task *chao);
static int play_trumpet(struct task *chao);
static int play_tambourine(struct task *chao);
static int listen_to_band(struct task *chao);

static struct chao_placement instrument_placements[INSTRUMENT_PLACEMENTS] = {
#include "chao/instruments/instrument_placements.inc"
};
chao_behavior_fn instrument_actions[CHAO_INSTRUMENTS] = {
    play_bell,  play_castanets, play_cymbals, play_drum,
    play_flute, play_maracas,   play_trumpet, play_tambourine};

void score_nearby_instrument(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 threshold = chao_tuning.boredom_threshold >> 1;
    Uint32 boredom;
    struct task *band;
    int known[CHAO_INSTRUMENTS];
    int count;
    int instrument;
    int i;

    boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    band = garden_hit_kind(chao, CHAO_PUSH_SHAPE, BAND_FIELD);
    count = 0;
    if (*best < 1.0F && band) {
        for (i = 0; i < CHAO_INSTRUMENTS; i++) {
            if (chao_knows_instrument(chao, i)) {
                known[count] = i;
                count++;
            }
        }
        if (boredom > threshold) {
            if (boredom < LIFT_BELOW) {
                boredom += BOREDOM_LIFT;
            }
            score = chao_need_above(boredom, threshold);
            score *= chao_tuning.boredom_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            if (count > 0) {
                instrument = PICK_LEARNED(known, count);
                mover_set_target(chao, &band->work->pos);
                chao_set_behavior(chao, chao_face_point_behavior);
                chao_queue_behavior(chao, instrument_actions[instrument]);
                chao_emotion_add(chao, CHAO_BOREDOM, -BAND_RELIEF);
                chao_emotion_add(chao, CHAO_LONELINESS, -BAND_RELIEF);
                chao_clear_intention(chao);
                if (band->parent) {
                    chao_emotion_add(band->parent, CHAO_LONELINESS,
                                     -BAND_RELIEF);
                }
            } else {
                if (band->parent) {
                    mover_set_target(chao, &band->parent->work->pos);
                }
                chao_set_behavior(chao, chao_face_point_behavior);
                chao_queue_behavior(chao, chao_sit_down_behavior);
                chao_queue_behavior(chao, listen_to_band);
            }
            *best = BAND_BID;
        }
    }
}

int chao_start_band(struct task *chao)
{
    struct task_work *work = chao->work;
    int known[CHAO_INSTRUMENTS];
    NJS_POINT3 position;
    int instrument;
    int i;
    int count;
    Angle heading;

    count = 0;
    collision_restart_scans();
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, BAND_FIELD)) {
        return FALSE;
    }
    for (i = 0; i < CHAO_INSTRUMENTS; i++) {
        if (chao_knows_instrument(chao, i)) {
            known[count] = i;
            count++;
        }
    }
    if (count > 0) {
        instrument = PICK_LEARNED(known, count);
        heading = work->ang.y;
        position.x = work->pos.x + (njSin(heading) * BAND_AHEAD);
        position.y = work->pos.y;
        position.z = work->pos.z + (njCos(heading) * BAND_AHEAD);
        chao_set_behavior(chao, instrument_actions[instrument]);
        garden_create_timed_field(chao, BAND_FIELD, &position, BAND_RADIUS,
                                  RANDOM_COUNT(1800, 2401));
        return TRUE;
    }
    return FALSE;
}

static int play_maracas(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_MARACAS_SHAKA_UP);
        SET_EXPRESSION(chao, CHAO_EYE_DISTRESSED, 1);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &chao_maraca_4,
                        &garden_toy_texlist);
        chao_equip_node(chao, CHAO_NODE_HAND_11, &chao_maraca_11,
                        &garden_toy_texlist);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- == 0 || chao_motion_held(chao)) {
            if (RANDOM_UNIT() < 0.3F) {
                chao_link_motion_frames(chao, CHAO_ANIM_MARACAS_JUMP_B, 10);
                PLAY_NOTE(chao, INSTRUMENT_MARACAS, 40);
                if (RANDOM_UNIT() < 0.4F) {
                    sound_play_at(GLAD_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                                  &chao->work->pos);
                }
            } else {
                motion = chao_motion_index(chao);
                switch (motion) {
                case CHAO_ANIM_MARACAS_SHAKA_UP:
                    chao_link_motion_synced(chao, CHAO_ANIM_MARACAS_SHAKA_DOWN);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                case CHAO_ANIM_MARACAS_SHAKA_DOWN:
                    chao_link_motion_synced(chao, CHAO_ANIM_MARACAS_SHAKA_UP);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                default:
                    chao_link_motion_frames(chao, CHAO_ANIM_MARACAS_SHAKA_UP,
                                            10);
                    behavior->timer = RANDOM_COUNT(60, 61);
                    break;
                }
            }
        }
        motion = chao_motion_index(chao);
        switch (motion) {
        case CHAO_ANIM_MARACAS_SHAKA_UP:
        case CHAO_ANIM_MARACAS_SHAKA_DOWN:
            if (behavior->count++ % 16 == 0) {
                PLAY_NOTE(chao, INSTRUMENT_MARACAS, 100);
            }
            break;
        }
    }
    return BAND_ENDED(chao);
}

static int play_bell(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case BAND_START:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_BELL_UP);
        } else {
            chao_link_motion(chao, CHAO_ANIM_BELL_DOWN);
        }
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node_at(chao, CHAO_NODE_HAND_11,
                           &instrument_placements[BELL_PLACEMENT]);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->count++ % 30 == 0) {
            PLAY_NOTE(chao, INSTRUMENT_BELL, 0);
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_drum(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_DRUM_LR);
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &chao_drumstick_4,
                        &garden_toy_texlist);
        chao_equip_node(chao, CHAO_NODE_HAND_11, &chao_drumstick_11,
                        &garden_toy_texlist);
        chao_equip_node(chao, CHAO_NODE_MEDAL, &chao_drum, &garden_toy_texlist);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            motion = chao_motion_index(chao);
            switch (motion) {
            case CHAO_ANIM_DRUM_LR:
                if (RANDOM_UNIT() < 0.6F) {
                    chao_link_motion_synced(chao, CHAO_ANIM_DRUM_LR);
                } else {
                    chao_link_motion_frames(chao, CHAO_ANIM_DRUM_DOUBLE, 10);
                }
                behavior->timer = RANDOM_COUNT(60, 181);
                break;
            case CHAO_ANIM_DRUM_DOUBLE:
                chao_link_motion_frames(chao, CHAO_ANIM_DRUM_LR, 10);
                behavior->timer = RANDOM_COUNT(60, 181);
                break;
            }
        }
        motion = chao_motion_index(chao);
        switch (motion) {
        case CHAO_ANIM_DRUM_LR:
            if (behavior->count++ % 16 == 0) {
                PLAY_NOTE(chao, INSTRUMENT_DRUM, 100);
            }
            break;
        case CHAO_ANIM_DRUM_DOUBLE:
            if (behavior->count++ % 12 == 0 && RANDOM_UNIT() < 0.4F) {
                PLAY_NOTE(chao, INSTRUMENT_DRUM, 100);
            }
            break;
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_flute(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_FUE_LR);
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node_at(chao, CHAO_NODE_HAND_11,
                           &instrument_placements[FLUTE_PLACEMENT]);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            behavior->mode = BAND_START;
        }
        if (behavior->count++ % 180 == 0 && RANDOM_UNIT() < 0.5F) {
            PLAY_NOTE(chao, INSTRUMENT_FLUTE, -40);
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_castanets(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_CASTANETS_WAIT);
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node_at(chao, CHAO_NODE_HAND_4,
                           &instrument_placements[CASTANETS_PLACEMENT]);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            motion = chao_motion_index(chao);
            switch (motion) {
            case CHAO_ANIM_CASTANETS_WAIT:
                if (RANDOM_UNIT() < 0.6F) {
                    chao_link_motion_frames(chao, CHAO_ANIM_CASTANETS_PON, 10);
                } else {
                    chao_link_motion_frames(chao, CHAO_ANIM_CASTANETS_PONPON,
                                            10);
                }
                behavior->timer = RANDOM_COUNT(60, 181);
                behavior->count = 0;
                break;
            default:
                chao_link_motion_frames(chao, CHAO_ANIM_CASTANETS_WAIT, 10);
                behavior->timer = RANDOM_COUNT(60, 181);
                behavior->count = 0;
                break;
            }
        }
        motion = chao_motion_index(chao);
        behavior->count++;
        switch (motion) {
        case CHAO_ANIM_CASTANETS_WAIT:
            break;
        case CHAO_ANIM_CASTANETS_PON:
            if (behavior->count == 30) {
                PLAY_NOTE(chao, INSTRUMENT_CASTANETS, 120);
            }
            break;
        default:
            if (behavior->count == 30) {
                PLAY_NOTE(chao, INSTRUMENT_CASTANETS, 120);
            }
            if (behavior->count == 60) {
                PLAY_NOTE(chao, INSTRUMENT_CASTANETS, 120);
            }
            break;
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_trumpet(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case BAND_START:
        /* @hack redundant: a test whose two branches link the same motion,
         * where the draw and one link would do */
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_RAPPA_UP);
        } else {
            chao_link_motion(chao, CHAO_ANIM_RAPPA_UP);
        }
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_hand_4(chao, &chao_trumpet, &garden_toy_texlist);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            behavior->mode = BAND_START;
        }
        if (behavior->count++ % 180 == 0 && RANDOM_UNIT() < 0.5F) {
            PLAY_NOTE(chao, INSTRUMENT_TRUMPET, -200);
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_cymbals(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_CYMBAL_WAIT);
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &chao_cymbal_4,
                        &garden_toy_texlist);
        chao_equip_node(chao, CHAO_NODE_HAND_11, &chao_cymbal_11,
                        &garden_toy_texlist);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            motion = chao_motion_index(chao);
            switch (motion) {
            case CHAO_ANIM_CYMBAL_WAIT:
                chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_PAN, 10);
                behavior->timer = RANDOM_COUNT(60, 181);
                behavior->count = 0;
                break;
            default:
                chao_link_motion_frames(chao, CHAO_ANIM_CYMBAL_WAIT, 10);
                behavior->timer = RANDOM_COUNT(60, 181);
                break;
            }
        }
        motion = chao_motion_index(chao);
        behavior->count++;
        switch (motion) {
        case CHAO_ANIM_CYMBAL_WAIT:
            break;
        default:
            if (behavior->count == 30) {
                PLAY_NOTE(chao, INSTRUMENT_CYMBAL, 100);
            }
            break;
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int play_tambourine(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int motion;
    switch (behavior->mode) {
    case BAND_START:
        chao_link_motion(chao, CHAO_ANIM_TAMBOURINE_SHAKA);
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &chao_tambourine,
                        &garden_toy_texlist);
        PLAY_FOR(behavior, RANDOM_COUNT(60, 61));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            motion = chao_motion_index(chao);
            switch (motion) {
            case CHAO_ANIM_TAMBOURINE_SHAKA:
                chao_link_motion_frames(chao, CHAO_ANIM_TAMBOURINE_PAN, 10);
                behavior->timer = RANDOM_COUNT(60, 61);
                behavior->count = 0;
                break;
            default:
                chao_link_motion_frames(chao, CHAO_ANIM_TAMBOURINE_SHAKA, 10);
                behavior->timer = RANDOM_COUNT(60, 61);
                behavior->count = 0;
                break;
            }
        }
        motion = chao_motion_index(chao);
        behavior->count++;
        switch (motion) {
        case CHAO_ANIM_TAMBOURINE_SHAKA:
            break;
        default:
            if (behavior->count == 30) {
                PLAY_NOTE(chao, INSTRUMENT_TAMBOURINE, 120);
            }
            break;
        }
        break;
    }
    return BAND_ENDED(chao);
}

static int listen_to_band(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case BAND_START:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_MUSIC_SIT);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);
        }
        SET_EXPRESSION(chao, CHAO_EYE_GLAD, CHAO_MOUTH_GLAD);
        PLAY_FOR(behavior, RANDOM_COUNT(300, 301));
        /* fall through */
    case BAND_PLAY:
        if (behavior->timer-- <= 0) {
            behavior->mode = BAND_START;
        }
        if (BAND_ENDED(chao)) {
            chao_link_motion(chao, CHAO_ANIM_HAKUSYU_SIT);
            behavior->timer = RANDOM_COUNT(180, 121);
            behavior->mode = BAND_APPLAUD;
        }
        break;
    case BAND_APPLAUD:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
