/* a garden Chao's task work. after the task work come the profile and the
 * gene that its save keeps */
#ifndef CHAO_WORK_H
#define CHAO_WORK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/perception.h"
#include "chao/race_record.h"
#include "draw/motion.h"
#include "system/task.h"

struct chao_save;

#define CHAO_MOVES 0x2
#define CHAO_ANIMATES 0x4
#define CHAO_COLLIDES 0x8
#define CHAO_RUNS_BEHAVIOR 0x10
/* the Chao's behaviors run without intentions, cooldowns or timed ends */
#define CHAO_NO_INTENTIONS 0x2000
#define CHAO_BODY_SPRINGS 0x20
#define CHAO_BODY_IN_VIEW 0x1000
#define CHAO_SHAPE_GROUND_SHADOW 8
/* a bit of chao_shape_work.flags, under which chao_step_body morphs the
 * body again */
#define CHAO_SHAPE_RESHAPE 2
/* a bit of chao_shape_work.flags, under which the body's meshes draw with
 * chao_env_texlist */
#define CHAO_SHAPE_JEWEL 16
/* a bit of chao_shape_work.flags, under which the body wears the shiny
 * material */
#define CHAO_SHAPE_SHINY 0x40
#define CHAO_COURSE_CONSTRAIN 0x40
/* the last chao_follow_course_floor moved the Chao into another segment */
#define CHAO_COURSE_NEW_SEGMENT 0x1
/* the hand the Chao holds its object at. with both set, it holds the object
 * between them */
#define CHAO_HOLD_AT_HAND_4 0x400
#define CHAO_HOLD_AT_HAND_11 0x800
#define CHAO_COURSE_AIM 0x8000
#define CHAO_COURSE_AIM_NEAR 0x10000
#define CHAO_AGES 0x20000 /* chao_age runs only while it is set */
#define CHAO_UNK_40000 0x40000 /* chao_create sets it, and nothing reads it */
#define CHAO_RACE_STAGE_COLLISION 0x80
/* while it is set, a racer keeps its heading rather than turn to its target */
#define CHAO_RACE_FIXED_HEADING 0x4000
#define CHAO_VISIBLE 0x200 /* the Chao's body is drawn only while it is set */

enum chao_body {
    CHAO_BODY_STANDARD,
    CHAO_BODY_MODEL_1,
    CHAO_BODY_MODEL_2,
    CHAO_BODY_MINIMAL,
    CHAO_BODY_ALTERNATE
};

/* a Chao's animal part slots. each holds an animal kind or CHAO_NO_PART */
#define CHAO_PART_SLOTS 8
#define CHAO_NO_PART (-1)

struct chao_parameters {
    Uint8 type;
    Uint8 garden;
    Sint16 affection;
    Sint8 classroom_slot; /* its place in the classroom, or -1 for none */
    Uint8 reborn_egg;
    Uint8 unk_06[2];
    char name[8];
    Uint8 unk_10[8];
    Uint16 stats[CHAO_SKILLS];
    Uint16 age;
    Uint16 reincarnations;
    Uint16 age_timer;
    Uint8 unk_2c[8];
    Uint8 unk_34[8];
    Sint32 race_stamina;
    Sint32 race_target_stamina;
    Uint16 best_stats[CHAO_SKILLS];
    Uint16 adult_age, life, lifespan;
    Float run_power, fly_swim, alignment;
    Uint8 unk_64[12];
    Float evolution;
    Uint8 unk_74;
    struct garden_race_time personal_records[RACE_RECORD_TRACKS];
    Uint8 unk_93[10];
    Sint8 eye, mouth, emotion_ball;
    Uint8 unk_a0[4];
    Uint8 hat, costume, medal, color, texture, shiny;
    Uint8 body, minimal;
    Uint8 unk_ac[4];
    Uint32 eaten_minimals;
    Sint8 animal_parts[CHAO_PART_SLOTS];
};

/* the illnesses a Chao can catch, each with its count in illness */
#define CHAO_ILLNESSES 6

struct chao_emotions {
    Uint8 unk_00[2];
    Uint16 feeling_timer, illness_timer;
    Uint16 upkeep_timer;
    Uint8 feelings[CHAO_TOTAL - CHAO_PLEASURE + 1];
    Uint16 needs[CHAO_THIRST - CHAO_SLEEPINESS + 1];
    union {
        struct {
            Sint8 traits[CHAO_FICKLENESS - CHAO_CURIOSITY + 1];
            Uint8 taste, tv, music;
        } preferences;
        struct {
            Uint8 unk_26[10];
            Sint16 first, second;
        } initial;
    } personality;
    Sint8 illness[CHAO_ILLNESSES];
};

/* what a Chao knows of a player and of another Chao. chao_knowledge keeps
 * one for each of six players and twenty other Chao */
struct chao_player_relationship {
    Sint8 affinity;
    Uint8 fear;
    Uint16 time_apart, meetings;
};
struct chao_memory {
    Uint8 unk_00[0x12];
    Uint16 time_apart;
    Uint8 unk_14[4];
};
struct chao_knowledge {
    Uint8 art, dance, song, instruments;
    Uint16 small_toys, unk_06;
    Sint32 timer;
    struct chao_player_relationship players[6];
    struct chao_memory peers[20];
};

struct chao_profile {
    struct chao_parameters parameters;
    struct chao_emotions emotions;
    struct chao_knowledge knowledge;
    Uint8 unk_308[0x380 - 0x308];
};

struct chao_work {
    struct task_work base;
    struct chao_profile profile;
    struct chao_gene gene;
    Uint32 frames;
    struct task *cocoon;
    struct task *book;
    Sint32 calling_mate;
    struct task *field; /* the field task its behavior made, or null */
    struct task *race_target;
    Sint32 race_slot;
    Sint32 race_choice;
    Angle body_angle;
    Float climb_start_y;
    struct chao_save *save;
    Uint8 unk_428[0x438 - 0x428];
    Uint32 flags;
    Uint16 ground_mode;
    Uint8 unk_43e[2];
    Float race_progress;
    struct motion_control motion;
    struct motion_control minimal_motion;
    struct motion_table_entry minimal_clips[4];
    struct chao_behavior behavior;
    Uint8 unk_854[0x8e4 - 0x854];
    struct chao_shape_work {
        struct chao_node *root;
        NJS_CNK_OBJECT *nodes[CHAO_BODY_NODES];
        struct chao_morph_targets *form, *hero, *dark;
        NJS_POINT3 body_position, head_position;
        NJS_POINT3 hand_4, hand_11;
        NJS_POINT3 foot_6, foot_13;
        NJS_POINT3 mouth_position;
        NJS_VECTOR mouth_direction;
        struct chao_eye_anchor {
            NJS_POINT3 position;
            NJS_VECTOR direction;
        } eyes[2];
        struct chao_hand_prop prop_4, prop_11;
        Uint16 flags;
        Uint8 unk_162[6];
        Uint32 color;
    } shape;
    Uint8 unk_a50[0xa98 - 0xa50];
    struct chao_face face;
    struct chao_icon icon;
    struct chao_perception perception;
    Uint8 unk_16dc[4];
    Float lane; /* 0 at the left course edge, 1 at the right */
    NJS_PLANE race_floor;
    Uint8 unk_16fc[4];
};
/* the work of chao, a Chao's task, and its behavior queue */
#define CHAO_WORK(chao) ((struct chao_work *)(chao)->work)
#define CHAO_BEHAVIOR(chao) (&CHAO_WORK(chao)->behavior)

#endif
