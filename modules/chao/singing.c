/* a garden Chao singing, and the Chao that hear it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/intention.h"
#include "chao/knowledge.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/singing.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/animal.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

enum song_field { SONG_FIELD = 167, BAD_SONG_FIELD = 168 };
#define CLASSROOM_SONGS 5
/* a song's field reaches SONG_REACH around a point SONG_AHEAD before the
 * singer */
#define SONG_REACH 20.0F
#define SONG_AHEAD 8.0F
/* a Chao whose alignment is below OFF_KEY_ALIGNMENT sings the parrot's song
 * off key */
#define OFF_KEY_ALIGNMENT (-0.5F)
#define OFF_KEY_NOTE 0x607c
/* the frames a note sounds for, and a long note */
#define NOTE_FRAMES 230
#define LONG_NOTE_FRAMES 440
/* the frames from one note to the next in each song, and after a long
 * note */
#define LESSON_GAP 240
#define PARROT_GAP 180
#define OFF_KEY_GAP 220
#define LONG_NOTE_GAP 450
static int sing_first_lesson(struct task *chao);
static int sing_two_lessons(struct task *chao);
static int sing_three_lessons(struct task *chao);
static int sing_four_lessons(struct task *chao);
static int sing_full_repertoire(struct task *chao);
static const char voice_task_name[] = "VoiceTask";
/* the song of a Chao that knows n classroom songs is song_by_lessons[n - 1].
 * the entries past CLASSROOM_SONGS are never chosen */
static chao_behavior_fn song_by_lessons[8] = {
    sing_first_lesson,    sing_two_lessons,     sing_three_lessons,
    sing_four_lessons,    sing_full_repertoire, sing_full_repertoire,
    sing_full_repertoire, sing_full_repertoire};
/* the sound ids of the in-tune songs' notes. the parrot's song draws from the
 * first four. a Chao that knows n songs, up to four, draws from the first n
 * at LESSON_NOTES. LESSON_NOTES starts at the parrot's last note. one that
 * knows five draws from all four and LONG_NOTE */
/* clang-format off */
static int song_notes[8] = {
    0x6076, 0x6077, 0x6078,
    0x607e, 0x6079, 0x607a, 0x607b,
    0x6080};
/* clang-format on */
#define LESSON_NOTES (song_notes + 3)
#define LONG_NOTE (song_notes[7])
#define NOTE_INDEX(count) ((int)(rand() * ((count) * RAND_SCALE)))
#define SONG_ENDED(chao) (!garden_hit_kind((chao), CHAO_PUSH_SHAPE, SONG_FIELD))
#define BEGIN_SONG(chao, behavior)                                             \
    chao_set_timed_eye((chao), CHAO_EYE_GLAD, CHAO_FACE_HOLD);                 \
    chao_link_motion((chao), CHAO_ANIM_UTAU_HIDARI);                           \
    (behavior)->mode++;                                                        \
    (behavior)->timer = 0;                                                     \
    (behavior)->count = 0
#define SINGING_POSE(chao)                                                     \
    if (chao_motion_held(chao)) {                                              \
        if (RANDOM_UNIT() < 0.5F) {                                            \
            chao_link_motion((chao), CHAO_ANIM_UTAU_HIDARI);                   \
        } else {                                                               \
            chao_link_motion((chao), CHAO_ANIM_UTAU_LEFT);                     \
        }                                                                      \
    }
#define SINGING_MOUTH(chao, behavior)                                          \
    if ((behavior)->count++ % 30 == 0) {                                       \
        if (RANDOM_UNIT() < 0.5F) {                                            \
            chao_set_timed_mouth((chao), CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);     \
        } else {                                                               \
            chao_set_timed_mouth((chao), 2, CHAO_FACE_HOLD);                   \
        }                                                                      \
    }
/* @hack type: the key passed as an int to sound_play_voice_for, a second
 * name of sound_play_at_for, where its pointer would do */
#ifdef NONMATCHING
/* the plain form the hack tag names: sound_play_at_for by its own name, with
 * the key's pointer */
#define VOICE_KEYED(note, key, priority, volume, duration, pos)                \
    sound_play_at_for((note), (key), (priority), (volume), (duration), (pos))
#else
Sint8 sound_play_voice_for(int note, int key, Sint8 priority, Sint16 volume,
                           Sint16 duration, const NJS_POINT3 *position);
#define VOICE_KEYED(note, key, priority, volume, duration, pos)                \
    sound_play_voice_for((note), (int)(key), (priority), (volume), (duration), \
                         (pos))
#endif
#define SING_NOTE(chao, note, duration)                                        \
    VOICE_KEYED((note), (chao), 1, GARDEN_VOICE_VOLUME, (duration),            \
                &(chao)->work->pos)
#define SING_PHRASE(chao, behavior, note, gap)                                 \
    if ((behavior)->timer++ > (gap)) {                                         \
        (behavior)->timer = 0;                                                 \
        SING_NOTE((chao), (note), NOTE_FRAMES);                                \
    }
#define CLASSROOM_SONG(action, note)                                           \
    static int action(struct task *chao)                                       \
    {                                                                          \
        struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);                  \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            BEGIN_SONG(chao, behavior);                                        \
            /* fall through */                                                 \
        case 1:                                                                \
            SINGING_POSE(chao);                                                \
            SING_PHRASE(chao, behavior, (note), LESSON_GAP);                   \
            SINGING_MOUTH(chao, behavior);                                     \
            break;                                                             \
        }                                                                      \
        return SONG_ENDED(chao);                                               \
    }

static void voice_step(struct task *voice)
{
    struct task_work *work = voice->work;
    if (work->timer-- == 0) {
        task_kill(voice);
        return;
    }
    VOICE_KEYED(work->tick, voice, 1, GARDEN_VOICE_VOLUME, 60,
                &voice->work->pos);
}

void chao_create_voice(struct task *source, int sound, Sint16 yaw,
                       Sint16 frames)
{
    struct task *voice = task_create(TASK_WORK, 3, voice_step, voice_task_name);
#ifdef NONMATCHING
    /* this side makes no voice where task_create fails. the matching side
     * writes through the null */
    if (!voice) {
        return;
    }
#endif
    voice->work->pos = source->work->pos;
    voice->work->ang.y = yaw;
    voice->work->tick = sound;
    voice->work->timer = frames;
}

CLASSROOM_SONG(sing_first_lesson, LESSON_NOTES[0])
CLASSROOM_SONG(sing_two_lessons, LESSON_NOTES[NOTE_INDEX(1.99F)])
CLASSROOM_SONG(sing_three_lessons, LESSON_NOTES[NOTE_INDEX(2.99F)])
CLASSROOM_SONG(sing_four_lessons, LESSON_NOTES[NOTE_INDEX(3.99F)])

static int sing_full_repertoire(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        BEGIN_SONG(chao, behavior);
        /* fall through */
    case 1:
        SINGING_POSE(chao);
        if (behavior->timer++ > LESSON_GAP) {
            behavior->timer = 0;
            if (RANDOM_UNIT() < 0.7F) {
                SING_NOTE(chao, LESSON_NOTES[NOTE_INDEX(3.99F)], NOTE_FRAMES);
            } else {
                SING_NOTE(chao, LONG_NOTE, LONG_NOTE_FRAMES);
                behavior->mode = 2;
            }
            if (SONG_ENDED(chao)) {
                return BEHAVIOR_FINISH;
            }
        }
        SINGING_MOUTH(chao, behavior);
        break;
    case 2:
        SINGING_POSE(chao);
        if (behavior->timer++ > LONG_NOTE_GAP) {
            behavior->timer = 0;
            if (RANDOM_UNIT() < 0.3F) {
                SING_NOTE(chao, LONG_NOTE, LONG_NOTE_FRAMES);
            } else {
                SING_NOTE(chao, LESSON_NOTES[NOTE_INDEX(3.99F)], NOTE_FRAMES);
                behavior->mode = 1;
            }
            if (SONG_ENDED(chao)) {
                return BEHAVIOR_FINISH;
            }
        }
        SINGING_MOUTH(chao, behavior);
        break;
    }
    if (SONG_ENDED(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static int sing_parrot_song(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        BEGIN_SONG(chao, behavior);
        /* fall through */
    case 1:
        SINGING_POSE(chao);
        SING_PHRASE(chao, behavior, song_notes[NOTE_INDEX(3.99F)], PARROT_GAP);
        SINGING_MOUTH(chao, behavior);
        break;
    }
    return SONG_ENDED(chao);
}

static int sing_off_key(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        BEGIN_SONG(chao, behavior);
        /* fall through */
    case 1:
        SINGING_POSE(chao);
        if (behavior->timer++ > OFF_KEY_GAP) {
            behavior->timer = 0;
            sound_play_at(OFF_KEY_NOTE, NULL, 0, GARDEN_VOICE_VOLUME,
                          &chao->work->pos);
        }
        SINGING_MOUTH(chao, behavior);
        break;
    }
    return !garden_hit_kind(chao, CHAO_PUSH_SHAPE, BAD_SONG_FIELD);
}

static int listen_to_song(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_MUSIC_SIT);
        } else {
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);
        }
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, CHAO_MOUTH_GLAD, CHAO_FACE_HOLD);
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            behavior->mode = 0;
        }
        if (SONG_ENDED(chao)) {
            chao_link_motion(chao, CHAO_ANIM_HAKUSYU_SIT);
            behavior->timer = RANDOM_COUNT(180, 121);
            behavior->mode = 2;
        }
        break;
    case 2:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int dislike_song(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    int pose;
    switch (behavior->mode) {
    case 0:
        pose = chao_motion_pose(chao);
        switch (pose) {
        case CHAO_SITTING:
        case CHAO_ON_BACK:
        case CHAO_ON_FRONT:
            chao_link_motion_frames(chao, CHAO_ANIM_ITAI_BURUBURU, 35);
            break;
        case CHAO_STANDING:
        default:
            chao_link_motion(chao, CHAO_ANIM_HIMA_SIT_GUDE);
            break;
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_eye(chao, 3, CHAO_FACE_HOLD);
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);
        } else {
            chao_set_timed_mouth(chao, 4, CHAO_FACE_HOLD);
        }
        behavior->timer = RANDOM_COUNT(300, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- > 0) {
            break;
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_emotion_add(chao, CHAO_ANGER, 150);
        }
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

void score_hearing_song(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 threshold = chao_tuning.boredom_threshold;
    Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    struct task *song = garden_hit_kind(chao, CHAO_PUSH_SHAPE, SONG_FIELD);
    if (*best < 1.0F && song) {
        if (boredom > threshold) {
            if (boredom < 7000) {
                boredom += 3000;
            }
            score = chao_need_above(boredom, threshold);
            score *= chao_tuning.boredom_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            if (song->parent) {
                mover_set_target(chao, &song->parent->work->pos);
            }
            chao_set_behavior(chao, chao_face_point_behavior);
            chao_queue_behavior(chao, chao_sit_down_behavior);
            chao_queue_behavior(chao, listen_to_song);
            *best = 0.99F;
        }
    }
}

void score_hearing_bad_song(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 threshold = chao_tuning.boredom_threshold;
    Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    struct task *song = garden_hit_kind(chao, CHAO_PUSH_SHAPE, BAD_SONG_FIELD);
    if (*best < 1.0F && song) {
        if (boredom > threshold) {
            if (RANDOM_UNIT() < 0.7F) {
                score = 0.99F;
            }
        }
        if (score > *best) {
            chao_set_behavior(chao, dislike_song);
            *best = score;
        }
    }
}

/* set position SONG_AHEAD before the singer whose work is work, through
 * heading */
#define SONG_CENTER(position, heading, work)                                   \
    (heading) = (work)->ang.y;                                                 \
    (position).x = (njSin(heading) * SONG_AHEAD) + (work)->pos.x;              \
    (position).y = (work)->pos.y;                                              \
    (position).z = (njCos(heading) * SONG_AHEAD) + (work)->pos.z

int chao_start_singing(struct task *chao)
{
    struct task_work *work = chao->work;
    NJS_POINT3 position;
    int song_level = -1;
    int i;
    Angle heading;
    if (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, SONG_FIELD)) {
        for (i = 0; i < CLASSROOM_SONGS; i++) {
            if (chao_knows_song(chao, i)) {
                song_level++;
            }
        }
        if (song_level >= 0) {
            SONG_CENTER(position, heading, work);
            chao_set_behavior(chao, song_by_lessons[song_level]);
            garden_create_timed_field(chao, SONG_FIELD, &position, SONG_REACH,
                                      RANDOM_COUNT(1500, 1801));
            return TRUE;
        } else if (chao_has_eaten_minimal(chao, ANIMAL_OUM)) {
            SONG_CENTER(position, heading, work);
            if (((struct chao_work *)chao->work)->profile.parameters.alignment <
                OFF_KEY_ALIGNMENT) {
                chao_set_behavior(chao, sing_off_key);
                garden_create_timed_field(chao, BAD_SONG_FIELD, &position,
                                          SONG_REACH, RANDOM_COUNT(1800, 1801));
            } else {
                chao_set_behavior(chao, sing_parrot_song);
                garden_create_timed_field(chao, SONG_FIELD, &position,
                                          SONG_REACH, RANDOM_COUNT(1200, 1201));
            }
            return TRUE;
        }
    }
    return FALSE;
}
