/* the messenger, MSGER in a stage's object list: a flying helper that shows
 * the stage's messages. a placed messenger waits until a player touches it.
 * it then rises and shows its message. a player can carry it. it says the
 * system's messages as it is picked up, carried, let go and hit. a hit stuns
 * it, and it then flies home. messenger_create summons one that shows a
 * message and then flies off for good */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/field.h"
#include "shared/messenger.h"
#include "shared/messenger_art.h"
#include "shared/object_builds.h"
#include "shared/sparkle.h"
#include "shared/table_file.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a messenger's modes, in its work's mode. a struck or bumped messenger's
 * next step stuns it. a stunned one flies home once its timer runs out, or
 * off for good where summoned */
enum messenger_mode {
    MODE_IDLE, /* waiting, or carried */
    MODE_FLY, /* flying to its goal */
    MODE_STRUCK, /* hit while it flew or talked */
    MODE_BUMPED, /* its collider met an item or an enemy while it idled */
    MODE_STUNNED, /* until its timer runs out */
    MODE_APPEAR, /* growing its propeller after a player's touch */
    MODE_RISE, /* rising to show its message */
    MODE_TALK /* showing its message */
};

/* bits of a messenger's work's submode */
#define WANDER 0x1 /* it keeps its goal near where its player is headed */
#define STEERING 0x2 /* it steers toward its goal */
#define SEEN 0x4 /* it was drawn since its last step */

/* in a messenger's work: its next goal's direction about y */
#define GOAL_ANGLE(work) ((work)->timer)
/* in a messenger's work: the steps left of its burst of speed */
#define BURST_LEFT(work) ((work)->tick)

#define SOUND_APPEAR 0x800b
#define SOUND_BURST 0x800c
#define SOUND_FLY 0x8017

#define TASK_LEVEL 2 /* the level of a root task messenger_create makes */

/* the steps a window shows a page without a W code */
#define SAY_TIME 120 /* for a system message */
#define MESSAGE_TIME 600 /* for a stage message */

/* numbers of the system's messages. each SAY_ number begins a block of
 * SAY_BLOCK. messenger_say_random says one of a block at random */
#define SAY_BLOCK 4
#define SAY_STRUCK 0 /* on a hit or a bump */
#define SAY_RECOVER 4 /* as its stun ends */
#define SAY_PICKED_UP 8
#define SAY_CHAT 12 /* the first and second chats while carried */
#define SAY_CHAT_THIRD 16
#define SAY_SET_DOWN 20 /* let go at a speed below 1 */
#define SAY_THROWN 24 /* let go at a speed of 1 or more */
/* the first of the texts messenger_knudai_texts returns, and the first of
 * messenger_upgrade_texts's */
#define KNUDAI_TEXTS 28
#define UPGRADE_TEXTS 50
#define SAY_SPRING 78 /* on MESSENGER_EVENT_SPRING */
#define SAY_SAVE_POINT 82 /* on MESSENGER_EVENT_SAVE_POINT */
/* the first goal ring message, one for each stage that has one. messenger_say
 * says the stage's message on MESSENGER_EVENT_GOAL_RING */
#define SAY_GOAL_RING 90
#define SAY_CHAT_FOURTH 110 /* after which the count of chats starts over */
/* the system messages through the fourth chat's block. the calls for the
 * stands' texts return null while the system file holds fewer */
#define SYSTEM_MESSAGES (SAY_CHAT_FOURTH + SAY_BLOCK)

/* what a talk window shows for a system message the system file lacks. it
 * reads "death" in brackets, then "(the message setting is out of range)" */
#define BAD_MESSAGE                                                            \
    "\x81\x75\x8e\x80\x81\x76\x81\x69\x83\x81\x83\x62\x83\x5a"                 \
    "\x81\x5b\x83\x57\x90\xdd\x92\xe8\x82\xaa\x94\xcd\x88\xcd"                 \
    "\x8a\x4f\x82\xc8\x82\xcc\x82\xbe\x81\x6a"
/* the stand-in for a stage message the stage's file lacks: a giggle, then
 * "(note: there is no text)" */
#define NO_TEXT                                                                \
    "\x82\xde\x82\xd3\x82\xd3\x82\xa3\n\x81\x69\x89"                           \
    "\xf0\x90\xe0\x81\x46\x08\x66\x66\x38\x30\x38"                             \
    "\x30\x83\x65\x08\x70\x30\x83\x4c\x08\x70\x31"                             \
    "\x83\x58\x08\x70\x32\x83\x67\x08\x72\x82\xaa"                             \
    "\x82\xc8\x82\xa2\x08\x38\x30\x66\x66\x38\x30"                             \
    "\x83\x66\x08\x72\x83\x41\x83\x8b\x81\x6a"

/* an entry of a message file: its text, or null where the file has none */
struct message {
    char *text;
};

/* a messenger's record, at its task's extra */
struct messenger {
    NJS_POINT3 goal;
    Float speed; /* how much a steering step adds to spd, toward goal */
    /* the center messenger_pick_goal last picked the goal around */
    NJS_POINT3 goal_center;
    Angle rotor; /* spin of the winding key about z */
    Angle twirl; /* turn of the propeller about y */
    Angle bob_phase; /* the phase of its bob up and down */
    Angle swing; /* the phase of the swing of the arms and feet */
    /* the head's sway about y while carried, which also tilts the head and
     * turns the messenger. it dies away once the messenger is let go */
    Angle sway;
    Float scale; /* the propeller's scale, from 0 to 1 */
    /* steps left of a pause. each step of it clears WORK_HURT and STEERING.
     * an idle messenger waits out the pause, and MODE_STUNNED ends with it */
    Sint32 timer;
    Sint32 talk_time; /* steps before MODE_TALK may end, unless a hit ends it */
    /* its window's task, or null. the window clears it as it closes */
    struct task *window;
    NJS_POINT3 home; /* where it began */
    Sint32 unk_4c;
    Sint32 player; /* the slot of the player it follows */
    Sint32 holder; /* the slot of the player who carries it, or last did */
    /* how red the land's light turns as the messenger draws, from 0 to 1 */
    Float flash;
    Float flash_aim; /* what flash eases toward */
    Sint32 chats; /* the chats said while carried, 0 to 3 */
    /* grows each step the messenger strays from home uncarried, or flies in
     * view. it starts over at 0 at any other step */
    Sint32 away_time;
    Float swing_speed; /* what carrying adds to the swing, in degrees a step */
    Sint32 quiet_time; /* steps since any window was last open */
    Sint32 held_time; /* steps carried */
    /* the trails from trail_left and trail_right, one for each player's
     * camera, by camera_current_index */
    struct trail *trails_left[GAME_PLAYERS];
    struct trail *trails_right[GAME_PLAYERS];
};

#define MESSENGER(task) ((struct messenger *)(task)->extra.p)
/* in a messenger's task: not 0 where messenger_create summoned it */
#define SUMMONED(task) ((task)->unk_3c.l)
/* in a window task of messenger_create: its window, or null once the window
 * closes */
#define WINDOW(task) ((task)->unk_3c.task)
/* a messenger shows the stage message whose number is its scl.x, made
 * positive and cut to an integer. a messenger whose scl.y is PLAIN_SCL_Y or
 * more draws no trails and sheds no sparkles */
#define MESSAGE_NUMBER(work) ((work)->scl.x)
#define PLAIN_SCL_Y 1.0F
/* each step carried starts timer over at HELD_PAUSE. LET_GO_DELAY steps after
 * the last, an idle messenger says SAY_SET_DOWN or SAY_THROWN, unless the
 * stage is cleared */
#define HELD_PAUSE 120
#define LET_GO_DELAY 10

/* the stage's messages, and the system's, with their counts. a count is -1
 * while its file isn't loaded */
static struct message *stage_messages = NULL;
static Sint32 stage_messages_count = -1;
static struct message *system_messages = NULL;
static Sint32 system_messages_count = -1;
/* the event messenger_set_event gave each player */
static Sint32 messenger_events[GAME_PLAYERS] = {MESSENGER_EVENT_NONE,
                                                MESSENGER_EVENT_NONE};

/* a messenger talks in a window that draws its text, and says in one that
 * never draws */
static Sint32 talk_window_kind = MESSAGE_WINDOW_FADE;
static Sint32 say_window_kind = MESSAGE_WINDOW_HIDDEN;
/* MODE_RISE steers to rise_height above where it starts */
static Float rise_height = 5.0F;
static Sint32 dat_messenger_11 = 0;
/* what spd.y gains as the messenger starts to rise */
static Float appear_hop = 0.235F;
/* the shapes in a messenger's collision record, and the points of a trail */
#define SHAPES 2
#define TRAIL_POINTS 8
/* a placed messenger's shapes and a summoned one's, and each one's
 * collider's. a collider is a child task of kind COLLISION_ATTACK */
/* clang-format off */
static struct collision_shape placed_shapes[SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_NO_OWN_HITS | SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 5.0F, 0.0F}, 10.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, 0, SHAPE_GUARD_LEVEL(1), SHAPE_HURTABLE,
     {0.0F, 4.5F, 0.0F}, 3.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape summoned_shapes[SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_NO_OWN_HITS,
     {0.0F, 5.0F, 0.0F}, 10.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, 0, 0, 0,
     {0.0F, 4.5F, 0.0F}, 3.0F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape placed_collider_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_OBJECTS | SHAPE_HURTS_ENEMIES | SHAPE_HURT |
         SHAPE_ATTACK_LEVEL(2),
     SHAPE_HURTABLE | SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_UNK_9,
     {0.0F, 4.5F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape summoned_collider_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_UNK_9,
     {0.0F, 4.5F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the most a steering step's push may turn from the messenger's heading */
static Sint32 turn_limit = 0x7000;
/* the points its two trails stream from, in the messenger's model space */
static NJS_POINT3 trail_left = {-0.7F, 0.1F, 0.0F};
static NJS_POINT3 trail_right = {0.7F, 0.1F, 0.0F};
static Sint32 hold_turn = 0x1100;

Uint8 *messenger_build_models(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, messenger_builds, MESSENGER_BUILDS) ==
        MESSENGER_BUILDS) {
        messenger_built = 1;
        messenger_white_face_build =
            model_build_colored(&buf, messenger_face_lights->model, -1);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

/* the count of the stage's messages, or -1 while none are loaded. no code
 * calls it, and no header declares it. it is external so that SHC keeps it,
 * as the image does */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 messenger_message_count(void)
{
    return stage_messages_count;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the text of the stage message whose number is number, or null where the
 * stage's file holds none */
static char *messenger_message(int number)
{
    if (number >= 0 && number < stage_messages_count) {
        return stage_messages[number].text;
    }
    return 0;
}

char **messenger_knudai_texts(void)
{
    if (system_messages_count >= SYSTEM_MESSAGES) {
        return &system_messages[KNUDAI_TEXTS].text;
    }
    return 0;
}

char **messenger_upgrade_texts(void)
{
    if (system_messages_count >= SYSTEM_MESSAGES) {
        return &system_messages[UPGRADE_TEXTS].text;
    }
    return 0;
}

void messenger_messages_free(void)
{
    if (stage_messages) {
        syFree(stage_messages);
        stage_messages = 0;
        stage_messages_count = -1;
    }
    if (system_messages) {
        syFree(system_messages);
        system_messages = 0;
        system_messages_count = -1;
    }
}

Sint32 messenger_messages_load(const char *name)
{
    if (stage_messages == 0 && system_messages == 0) {
        stage_messages =
            table_file_read_language_sparse(name, &stage_messages_count);
        system_messages =
            table_file_read_language_sparse("mhsys", &system_messages_count);
        if (stage_messages && system_messages) {
            return stage_messages_count + system_messages_count;
        }
        messenger_messages_free();
    }
    return -1;
}

void messenger_set_event(int player, enum messenger_event event)
{
    if (player < GAME_PLAYERS && event < MESSENGER_EVENTS) {
        messenger_events[player] = event;
    }
}

/* say a system message of the block that starts at message, picked at
 * random. *owner gets the window, as message_window_open says. where the
 * system file holds no such message, the call shows BAD_MESSAGE in a talk
 * window instead */
static void messenger_say_random(int message, struct task **owner)
{
    int number = message + ((int)(rand() * (100 * RAND_SCALE)) % SAY_BLOCK);

    if (number < system_messages_count) {
        message_window_open_at_voice(say_window_kind,
                                     system_messages[number].text, SAY_TIME,
                                     game_state.text_language, owner, 0);
        /* @hack spelling: a goto to the return at the end, where the if
         * alone would do */
        goto end;
    } else {
        message_window_open(talk_window_kind, BAD_MESSAGE, SAY_TIME, 0, owner);
    }
end:
    return;
}

/* say the system message whose number is message, as messenger_say_random
 * does */
static void messenger_say(int message, struct task **owner)
{
    /* @hack spelling: number, a copy of message, where message itself
     * would do */
    int number = message;

    if (number < system_messages_count) {
        message_window_open_at_voice(say_window_kind,
                                     system_messages[number].text, SAY_TIME,
                                     game_state.text_language, owner, 0);
        /* @hack spelling: a goto to the return at the end, where the if
         * alone would do */
        goto end;
    } else {
        message_window_open(talk_window_kind, BAD_MESSAGE, SAY_TIME, 0, owner);
    }
end:
    return;
}

/* each step, messenger_ease_angles moves the angles about x and y by 1 /
 * EASE_XY_PARTS of the way. it moves the angle about z by 1 / EASE_Z_PARTS.
 * each moves by at most its _MOST */
#define EASE_XY_PARTS 3
#define EASE_Z_PARTS 8
#define EASE_X_MOST DEGREES(8)
#define EASE_Y_MOST DEGREES(6)
#define EASE_Z_MOST DEGREES(2)

/* ease task's angles from ang, the angles its step began with, toward those
 * the step set */
static void messenger_ease_angles(struct task *task, struct angles *ang)
{
    struct task_work *work;
    Sint16 turn;

    work = task->work;
    turn = work->ang.x - ang->x;
    turn = turn / EASE_XY_PARTS;
    if (turn > EASE_X_MOST) {
        turn = EASE_X_MOST;
    }
    if (turn < -EASE_X_MOST) {
        turn = -EASE_X_MOST;
    }
    work->ang.x = ang->x + turn;
    turn = work->ang.y - ang->y;
    turn = turn / EASE_XY_PARTS;
    if (turn > EASE_Y_MOST) {
        turn = EASE_Y_MOST;
    }
    if (turn < -EASE_Y_MOST) {
        turn = -EASE_Y_MOST;
    }
    work->ang.y = ang->y + turn;
    turn = work->ang.z - ang->z;
    turn = turn / EASE_Z_PARTS;
    if (turn > EASE_Z_MOST) {
        turn = EASE_Z_MOST;
    }
    if (turn < -EASE_Z_MOST) {
        turn = -EASE_Z_MOST;
    }
    work->ang.z = ang->z + turn;
}

/* for vectors: VEC_SUB sets d to a - b, VEC_ADD adds a to d, VEC_SCALE
 * scales d by s, and VEC_ADD3 sets d to a + b */
/* @hack spelling: the components through walking pointers, where three
 * assignments would do */
#define VEC_SUB(d, a, b)                                                       \
    {                                                                          \
        Float *d_ = &(d)->x;                                                   \
        Float *a_ = &(a)->x;                                                   \
        Float *b_ = &(b)->x;                                                   \
        *d_++ = *a_++ - *b_++;                                                 \
        *d_++ = *a_++ - *b_++;                                                 \
        *d_ = *a_ - *b_;                                                       \
    }

/* @hack spelling: the components through walking pointers, where three
 * assignments would do */
#define VEC_ADD(d, a)                                                          \
    {                                                                          \
        Float *d_ = &(d)->x;                                                   \
        Float *a_ = &(a)->x;                                                   \
        *d_++ += *a_++;                                                        \
        *d_++ += *a_++;                                                        \
        *d_ += *a_;                                                            \
    }

/* @hack spelling: the components through a walking pointer, where three
 * assignments would do */
#define VEC_SCALE(d, s)                                                        \
    {                                                                          \
        Float *d_ = &(d)->x;                                                   \
        *d_++ *= (s);                                                          \
        *d_++ *= (s);                                                          \
        *d_ *= (s);                                                            \
    }

/* @hack spelling: the components through walking pointers, where three
 * assignments would do */
#define VEC_ADD3(d, a, b)                                                      \
    {                                                                          \
        Float *d_ = &(d)->x;                                                   \
        Float *a_ = &(a)->x;                                                   \
        Float *b_ = &(b)->x;                                                   \
        *d_++ = *a_++ + *b_++;                                                 \
        *d_++ = *a_++ + *b_++;                                                 \
        *d_ = *a_ + *b_;                                                       \
    }

/* the steps back of the two places messenger_player_ahead measures a
 * velocity between */
#define PAST_FAR 18
#define PAST_NEAR 3
#define AHEAD 18 /* the steps ahead it leads that velocity */
#define AHEAD_Y_SHARE 0.9F /* the share of that velocity's y it keeps */

/* set *spd to the player's mean velocity a step, between PAST_FAR and
 * PAST_NEAR steps back. the call scales its y by AHEAD_Y_SHARE. set *pos to
 * where that velocity takes the player AHEAD steps from now. for an empty slot,
 * the call sets *spd to 0, so spd must not be null. it then sets *pos to the
 * pos of player_work[player], or to the origin where that is null */
static void messenger_player_ahead(int player, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    NJS_POINT3 older;
    NJS_POINT3 recent;
    NJS_VECTOR vel;

    if (player_get_past(player, PAST_FAR, &older, 0) &&
        player_get_past(player, PAST_NEAR, &recent, 0)) {
        VEC_SUB(&vel, &recent, &older);
        VEC_SCALE(&vel, 1.0F / (PAST_FAR - PAST_NEAR));
        vel.y *= AHEAD_Y_SHARE;
        if (spd) {
            *spd = vel;
        }
        VEC_SCALE(&vel, (Float)(PAST_NEAR + AHEAD));
        VEC_ADD(&vel, &recent);
        *pos = vel;
    } else if (player_work[player]) {
        spd->x = 0.0F;
        spd->y = 0.0F;
        spd->z = 0.0F;
        *pos = player_work[player]->pos;
    } else {
        spd->x = 0.0F;
        spd->y = 0.0F;
        spd->z = 0.0F;
        *pos = *spd;
    }
}

/* a goal lies GOAL_DISTANCE out from its center, and up to GOAL_SPREAD
 * further. it lies GOAL_HEIGHT above the center, and up to 1 higher. the
 * MECH_ values serve a messenger whose player drives a mech */
#define GOAL_SPREAD 5
#define GOAL_DISTANCE 12.0F
#define GOAL_HEIGHT 13.0F
#define MECH_GOAL_DISTANCE 25.0F
#define MECH_GOAL_HEIGHT 30.0F
/* what each pick adds to GOAL_ANGLE */
#define PICK_TURN 0x8800

/* set the goal to a random point about center, in the direction GOAL_ANGLE
 * gives about y */
static void messenger_pick_goal(struct task *task, NJS_VECTOR *center,
                                NJS_VECTOR *player_spd)
{
    struct task_work *work;
    NJS_POINT3 point;

    work = task->work;
    if (player_character(MESSENGER(task)->player) == PLAYER_EGGMAN_MECH ||
        player_character(MESSENGER(task)->player) == PLAYER_TAILS_MECH) {
        point.x = (rand() * (GOAL_SPREAD * RAND_SCALE)) + MECH_GOAL_DISTANCE;
        point.y = (rand() * RAND_SCALE) + MECH_GOAL_HEIGHT;
        point.z = 0.0F;
    } else {
        point.x = (rand() * (GOAL_SPREAD * RAND_SCALE)) + GOAL_DISTANCE;
        point.y = (rand() * RAND_SCALE) + GOAL_HEIGHT;
        point.z = 0.0F;
    }
    njPushMatrixEx();
    njUnitMatrix(0);
    njTranslateV(0, center);
    njRotateY(0, GOAL_ANGLE(work));
    njCalcPoint(0, &point, &MESSENGER(task)->goal);
    njPopMatrix(1);
    GOAL_ANGLE(work) += PICK_TURN;
    MESSENGER(task)->goal_center = *center;
}

/* a messenger moves while its squared speed in x and z is above
 * MOVING_SQUARED. a moving messenger banks by 1 / BANK_PARTS of its push's
 * turn, up to BANK_MOST. a still one's bank keeps BANK_KEEP of itself a step.
 * a goal nearer than GOAL_NEAR adds no push */
#define MOVING_SQUARED 0.0001F
#define BANK_PARTS 5
#define BANK_MOST DEGREES(30)
#define BANK_KEEP 0.97F
#define GOAL_NEAR 0.0001F

/* add speed to spd toward the goal. a moving messenger's push turns at most
 * turn_limit from its heading */
static void messenger_steer(struct task *task)
{
    struct task_motion *motion = task->motion.p;
    struct task_work *work = task->work;
    NJS_VECTOR to_goal;
    Angle to_target;
    Angle heading;
    Sint16 turn;
    Sint16 bank;
    Float len;

    if ((motion->spd.x * motion->spd.x) + (motion->spd.z * motion->spd.z) >
        MOVING_SQUARED) {
        VEC_SUB(&to_goal, &MESSENGER(task)->goal, &work->pos);
        to_target = RADIANS(atan2f(to_goal.x, to_goal.z));
        heading = RADIANS(atan2f(motion->spd.x, motion->spd.z));
        turn = to_target - heading;
        if (turn > turn_limit || turn < -turn_limit) {
            if (turn > turn_limit) {
                turn = turn_limit;
            }
            if (turn < -turn_limit) {
                turn = -turn_limit;
            }
            njPushMatrixEx();
            njUnitMatrix(0);
            njRotateY(0, turn - to_target + heading);
            njCalcVector(0, &to_goal, &to_goal);
            njPopMatrix(1);
        }
        /* @hack spelling: the bank stored to work's ang.z and read back,
         * where bank = -turn / BANK_PARTS would do */
        work->ang.z = -turn / BANK_PARTS;
        bank = work->ang.z;
        if (bank > BANK_MOST) {
            bank = BANK_MOST;
        }
        if (bank < -BANK_MOST) {
            bank = -BANK_MOST;
        }
        work->ang.z = bank;
    } else {
        Sint16 *roll;

        VEC_SUB(&to_goal, &MESSENGER(task)->goal, &work->pos);
        /* @hack spelling: the bank scaled through roll and len, where
         * *(Sint16 *)&work->ang.z *= BANK_KEEP would do */
        roll = (Sint16 *)&work->ang.z;
        len = *roll;
        len *= BANK_KEEP;
        *roll = len;
    }
    len = njScalor(&to_goal);
    if (len > GOAL_NEAR) {
        len = MESSENGER(task)->speed / len;
        VEC_SCALE(&to_goal, len);
        VEC_ADD(&motion->spd, &to_goal);
    }
}

/* a stalled steering step, as messenger_stall says, starts a burst at a
 * chance of BURST_CHANCE. the burst lasts BURST_STEPS steps, and a random 0
 * to BURST_EXTRA - 1 more */
#define BURST_CHANCE 0.1F
#define BURST_STEPS 6
#define BURST_EXTRA 8
/* a WANDER messenger's goal lies about the point its player is headed for.
 * it picks a new goal where that point lies over WANDER_DRIFT from
 * goal_center. it also picks one once within WANDER_NEAR of its goal, and
 * every WANDER_PERIOD steps of the game. otherwise its pos moves by
 * WANDER_FOLLOW of its player's velocity */
#define WANDER_DRIFT 28.0F
#define WANDER_NEAR 8.0F
#define WANDER_PERIOD 200
#define WANDER_FOLLOW 0.87F
/* a wandering messenger's spd keeps WANDER_DRAG of itself a step */
#define WANDER_DRAG 0.99F
/* within BRAKE_NEAR of its goal, a wandering messenger faster than
 * BRAKE_SPEED slows by BRAKE, and its burst ends. beyond BURST_FAR, one not
 * bursting bursts for a step */
#define BRAKE_NEAR 18.0F
#define BRAKE_SPEED 0.8F
#define BRAKE 0.95F
#define BURST_FAR 35.0F
/* a move that starts over WANDER_LEASH from the goal ends WANDER_LEASH away */
#define WANDER_LEASH 110.0F
/* a moving messenger faster than SPARKLE_SPEED sheds a sparkle at a chance of
 * SPARKLE_CHANCE a step. the sparkle starts SPARKLE_Y above pos, give or take
 * half of SPARKLE_SPREAD_Y. it takes SPARKLE_FOLLOW of the messenger's speed in
 * x and z, and SPARKLE_FOLLOW_Y of it in y. it rises at SPARKLE_RISE more. each
 * part of that speed then changes at random by up to half of SPARKLE_JITTER
 * either way. its size is random, from SPARKLE_SIZE up to SPARKLE_SIZE +
 * SPARKLE_SIZE_SPREAD. each of its red, green and blue is full or SPARKLE_DIM,
 * at even odds */
#define SPARKLE_CHANCE 0.3F
#define SPARKLE_SPEED 0.4F
#define SPARKLE_Y 4.5F
#define SPARKLE_SPREAD_Y 6.0F
#define SPARKLE_FOLLOW 0.2F
#define SPARKLE_FOLLOW_Y 0.1F
#define SPARKLE_RISE 0.13F
#define SPARKLE_JITTER 0.2F
#define SPARKLE_SIZE 0.2F
#define SPARKLE_SIZE_SPREAD 0.2F
#define SPARKLE_DIM 0xe0

static void messenger_move(struct task *task)
{
    struct task_work *work;
    struct task_motion *motion;
    NJS_POINT3 pos;
    NJS_VECTOR player_spd;
    Float dist;

    motion = task->motion.p;
    work = task->work;
    messenger_player_ahead(MESSENGER(task)->player, &pos, &player_spd);
    /* @bug memory: reads and writes the body through null where body_init
     * found no room. messenger_init tests for that null at 0x2a68 of the
     * object's code. it is read by messenger_move at 0x82a and by
     * messenger_step at 0x1e82, in its call of body_step */
    if (work->submode & STEERING) {
        NJS_POINT3 next;
        Float approach;

        BODY(task)->flags |= BODY_NO_GRAVITY;
        VEC_ADD3(&next, &motion->spd, &work->pos);
        /* @hack spelling: the negated distance first, where the other minus
         * it would do */
        approach = -njDistanceP2P(&next, &MESSENGER(task)->goal) +
                   njDistanceP2P(&work->pos, &MESSENGER(task)->goal);
        if (BURST_LEFT(work)) {
            MESSENGER(task)->speed += messenger_accel;
            BURST_LEFT(work)--;
        }
        if (approach < messenger_stall && RANDOM_UNIT() < BURST_CHANCE &&
            !BURST_LEFT(work)) {
            BURST_LEFT(work) =
                (int)(rand() * (BURST_EXTRA * RAND_SCALE)) + BURST_STEPS;
        }
        messenger_steer(task);
        MESSENGER(task)->speed *= messenger_drag;
        if (work->submode & WANDER) {
            if (njDistanceP2P(&MESSENGER(task)->goal_center, &pos) >
                WANDER_DRIFT) {
                messenger_pick_goal(task, &pos, &player_spd);
            }
            dist = njDistanceP2P(&MESSENGER(task)->goal, &work->pos);
            if (dist < WANDER_NEAR || game_state.steps % WANDER_PERIOD == 0) {
                messenger_pick_goal(task, &pos, &player_spd);
            } else {
                NJS_VECTOR offset;

                VEC_SCALE(&motion->spd, WANDER_DRAG);
                /* @hack spelling: dist reused for the speed and then BRAKE,
                 * where njScalor and BRAKE used directly would do. both forms
                 * leave dist below BRAKE_NEAR here, so the later tests of dist
                 * fail alike */
                if (dist < BRAKE_NEAR) {
                    dist = njScalor(&motion->spd);
                    if (dist > BRAKE_SPEED) {
                        dist = BRAKE;
                        VEC_SCALE(&motion->spd, dist);
                        BURST_LEFT(work) = 0;
                    }
                }
                offset = player_spd;
                VEC_SCALE(&offset, WANDER_FOLLOW);
                if (!BURST_LEFT(work) && dist > BURST_FAR) {
                    BURST_LEFT(work) = 1;
                }
                VEC_ADD(&work->pos, &offset);
                if (dist > WANDER_LEASH) {
                    VEC_SUB(&offset, &work->pos, &MESSENGER(task)->goal);
                    dist = WANDER_LEASH / njScalor(&offset);
                    VEC_SCALE(&offset, dist);
                    VEC_ADD3(&work->pos, &offset, &MESSENGER(task)->goal);
                }
            }
        }
    } else {
        BODY(task)->flags &= ~BODY_NO_GRAVITY;
        work->submode &= ~WANDER;
    }
    if (RANDOM_UNIT() < SPARKLE_CHANCE &&
        njScalor(&motion->spd) > SPARKLE_SPEED && work->scl.y < PLAIN_SCL_Y) {
        NJS_COLOR tint;
        NJS_POINT3 at;
        NJS_VECTOR spd;
        struct particle *pt;

        at = work->pos;
        at.y += ((RANDOM_UNIT() - 0.5F) * SPARKLE_SPREAD_Y) + SPARKLE_Y;
        spd.x = ((RANDOM_UNIT() - 0.5F) * SPARKLE_JITTER) +
                (motion->spd.x * SPARKLE_FOLLOW);
        spd.y = ((RANDOM_UNIT() - 0.5F) * SPARKLE_JITTER) +
                (motion->spd.y * SPARKLE_FOLLOW_Y) + SPARKLE_RISE;
        spd.z = ((RANDOM_UNIT() - 0.5F) * SPARKLE_JITTER) +
                (motion->spd.z * SPARKLE_FOLLOW);
        pt = sparkle_create(&at, &spd,
                            (rand() * (SPARKLE_SIZE_SPREAD * RAND_SCALE)) +
                                SPARKLE_SIZE);
        if (pt) {
            tint.argb.a = 0xff;
            tint.argb.r = RANDOM_UNIT() > 0.5F ? 0xff : SPARKLE_DIM;
            tint.argb.g = RANDOM_UNIT() > 0.5F ? 0xff : SPARKLE_DIM;
            tint.argb.b = RANDOM_UNIT() > 0.5F ? 0xff : SPARKLE_DIM;
            pt->color = tint.color;
        }
    }
}

/* scale the Sint16 at p by s. a product past a Sint16's range keeps its low
 * 16 bits. the SH-4 is little-endian, so p cast from an Angle's address
 * scales the Angle's low 16 bits only */
/* @hack spelling: through a Float local, where *(p) = (Sint32)(*(p) * (s))
 * would do */
#define SCALE_SINT16(p, s)                                                     \
    {                                                                          \
        Float f_ = *(p);                                                       \
        f_ *= (s);                                                             \
        *(p) = (Sint32)f_;                                                     \
    }

/* whether the messenger or its collider was hurt, or touched by an attack */
#define WAS_HIT(task, work)                                                    \
    (((work)->flags & WORK_HURT) || collision_next_attack(task) ||             \
     ((task)->child && (((task)->child->work->flags & WORK_HURT) ||            \
                        collision_next_attack((task)->child))))

/* each step, away_time gains AWAY_UNSEEN_GAIN while the messenger is
 * unseen, uncarried and over AWAY_FAR from home. it gains AWAY_SEEN_GAIN while
 * the messenger is seen, uncarried and over AWAY_FAR_SEEN from home. it gains
 * 1 while a seen messenger flies in any other case. at any other step it
 * starts over at 0 */
#define AWAY_UNSEEN_GAIN 3
#define AWAY_SEEN_GAIN 2
#define AWAY_FAR 400.0F
#define AWAY_FAR_SEEN 100.0F
/* past AWAY_SPIN, each step turns the messenger by AWAY_SPIN_TURN and raises
 * it by AWAY_RISE. past AWAY_BURST it bursts */
#define AWAY_SPIN 3000
#define AWAY_SPIN_TURN DEGREES(6)
#define AWAY_RISE 5.0F
#define AWAY_BURST 6000
/* the size and depth bias of the burst of effect/burst.h. a messenger makes one
 * as it bursts, and as a hit or a bump stuns it */
#define BURST_EFFECT_SCALE 6.0F
#define BURST_EFFECT_DEPTH_BIAS 5.0F
/* a rise, a talk and a flight play SOUND_FLY at FLY_PRIORITY, for
 * FLY_SOUND_STEPS from each step. each step adds FLY_BOB_STEP to bob_phase
 * and FLY_SWING_STEP to swing */
#define FLY_PRIORITY 1
#define FLY_SOUND_STEPS 30
#define FLY_BOB_STEP 0x300
#define FLY_SWING_STEP 0x600
/* an idle messenger flies home once it strays over HOME_RANGE from there,
 * measured with y scaled by HOME_Y_SHARE. a placed messenger flies to
 * HOME_HEIGHT above home, and then pauses for LANDED_STEPS steps. a summoned
 * one flies off to LEAVE_HEIGHT above home, and its task ends as the flight
 * does */
#define HOME_RANGE 20.0F
#define HOME_Y_SHARE 0.2F
#define HOME_HEIGHT 15.0F
#define LANDED_STEPS 108
#define LEAVE_HEIGHT 300.0F
/* a flight ends above home, with its squared distance from the goal in x and
 * z below ARRIVED_SQUARED */
#define ARRIVED_SQUARED 15.0F
#define RISE_DONE 3.0F /* MODE_RISE ends this near its goal's height */
#define TALK_STEPS 300 /* what talk_time starts at */
/* the steps a hit and a bump stun the messenger */
#define STRUCK_STEPS 300
#define BUMPED_STEPS 200
/* a hit or a bump throws the messenger up at STUN_HOP. while stunned, it
 * pitches to STUN_PITCH. its bob_phase and swing each move STUN_EASE of the
 * way to minus a quarter turn a step */
#define STUN_HOP 0.8F
#define STUN_PITCH DEGREES(230)
#define STUN_EASE 0.6F
/* a stunned messenger sheds a puff of dust_steady_create at a chance of
 * DAZE_CHANCE a step, rising at DAZE_RISE. the step jitters a point up to half
 * of DAZE_SPREAD either way in x and z. it jitters y up to half of
 * DAZE_SPREAD_Y */
#define DAZE_CHANCE 0.08F
#define DAZE_SPREAD 3.0F
#define DAZE_SPREAD_Y 2.5F
#define DAZE_RISE 0.1F
#define DAZE_SCALE 3.3F
#define DAZE_DEPTH_BIAS 3.0F
/* an idle messenger's turn toward the nearest player is held to IDLE_TURN_MOST.
 * it then turns by IDLE_TURN_SHARE of that. uncarried, its bob_phase keeps
 * REST_KEEP of itself a step. so does its swing, but a step whose way was
 * held adds IDLE_SWING_STEP to it instead */
#define IDLE_TURN_MOST DEGREES(3.5)
#define IDLE_TURN_SHARE 0.25F
#define IDLE_SWING_STEP 0x380
#define REST_KEEP 0.8F
/* an idle or appearing messenger's ang.x and ang.z keep LEVEL_KEEP of
 * themselves a step. a carried one's keep HELD_LEVEL_KEEP. a carried
 * messenger's bob_phase swings HELD_BOB_DEGREES either way of minus a quarter
 * turn, at game_state.steps << HELD_BOB_SHIFT. its swing gains
 * HELD_SWING_STEP a step. at a chance of KICK_CHANCE a step, swing_speed takes
 * a kick. the kick is messenger_swing_kick times a random number from
 * -KICK_BIAS up to 1 - KICK_BIAS */
#define LEVEL_KEEP 0.97F
#define HELD_LEVEL_KEEP 0.3F
#define HELD_BOB_DEGREES 15
#define HELD_BOB_SHIFT 11
#define HELD_SWING_STEP 0x800
#define KICK_CHANCE 0.005F
#define KICK_BIAS 0.2F
/* an idle messenger slower than TRAIL_STILL_SPEED drops its trails' oldest
 * points */
#define TRAIL_STILL_SPEED 0.2F
/* a carried messenger faces as its holder does, turned by HELD_TURN_SHARE of
 * its sway. its sway swings SWAY_DEGREES either way, at game_state.steps *
 * SWAY_RATE. once let go, the sway keeps SWAY_KEEP of itself a step */
#define HELD_TURN_SHARE 0.18F
#define SWAY_DEGREES 46
#define SWAY_RATE 0x380
#define SWAY_KEEP 0.8F
/* flash moves FLASH_EASE of the way to flash_aim a step, and flash_aim keeps
 * FLASH_AIM_KEEP of itself */
#define FLASH_EASE 0.5F
#define FLASH_AIM_KEEP 0.99F
/* where shared/field.h's sample holds FIELD_SPD, spd moves FIELD_EASE of the
 * way to the sample's a step */
#define FIELD_EASE 0.2F
/* a steering messenger faster than FACE_SPEED faces its motion, pitched no
 * more than PITCH_MOST either way */
#define FACE_SPEED 0.01F
#define PITCH_MOST DEGREES(45)
/* a carried messenger chats at a chance of CHAT_CHANCE a step. it chats only
 * once no window has been open for CHAT_QUIET steps. quiet_time counts no
 * further than QUIET_MOST, and held_time no further than HELD_MOST */
#define CHAT_QUIET 150
#define CHAT_CHANCE 0.03F
#define QUIET_MOST 600
#define HELD_MOST 1000
/* the collider is off while the messenger is carried. it is off too while
 * the square of its speed in x and z is below COLLIDER_SPEED_SQUARED */
#define COLLIDER_SPEED_SQUARED 0.8F

static void messenger_step(struct task *task)
{
    struct task_work *work;
    struct task_motion *motion;
    struct task_work *child_work;
    struct task *other;
    struct angles ang;
    Float dist;
    /* @hack spelling: one, a local for 1.0F, where the constant would do */
    Float one;
    Float z;
    Float x;
    /* @hack spelling: scaled, a local each SCALE_SINT16 goes through, where the
     * address in the call would do */
    Sint16 *scaled;
    Sint16 turn;
    Sint16 step;
    Sint16 pitch;
    int yaw;
    int found;
    Bool turned;
    Bool hit;
    int num;
    int i;
    /* @hack spelling: state, a local for &game_state, where game_state
     * itself would do */
    struct game_state *state;

    motion = task->motion.p;
    work = task->work;
    if (!SUMMONED(task) && placement_try_leave(task)) {
        return;
    }
    one = 1.0F;
    ang = work->ang;
    state = &game_state;
    /* @hack redundant: a block no path reaches, with two stores and four
     * strings that nothing reads */
    if (0) {
        work->mode = MODE_IDLE;
        work->submode = 0;
        (void)"<- MODE(LIGHT1)";
        (void)"<- MODE(NORMAL)";
        (void)"<- ANGLE Y";
        (void)"<- MSG NUM";
    }
    if (!state->time_stopped) {
        NJS_VECTOR vec;
        NJS_POINT3 point;

        if (!message_window_current()) {
            if (MESSENGER(task)->quiet_time < QUIET_MOST) {
                MESSENGER(task)->quiet_time++;
            }
        } else {
            MESSENGER(task)->quiet_time = 0;
        }
        dist = njDistanceP2P(&MESSENGER(task)->home, &work->pos);
        if (!(work->submode & SEEN)) {
            if (!(work->flags & WORK_HELD_BY_PLAYER) && dist > AWAY_FAR) {
                MESSENGER(task)->away_time += AWAY_UNSEEN_GAIN;
            } else {
                MESSENGER(task)->away_time = 0;
            }
        } else if (!(work->flags & WORK_HELD_BY_PLAYER) &&
                   dist > AWAY_FAR_SEEN) {
            MESSENGER(task)->away_time += AWAY_SEEN_GAIN;
        } else if (work->mode == MODE_FLY) {
            MESSENGER(task)->away_time++;
        } else {
            MESSENGER(task)->away_time = 0;
        }
        if (MESSENGER(task)->away_time > AWAY_SPIN) {
            work->ang.y += AWAY_SPIN_TURN;
            work->pos.y += AWAY_RISE;
            if (MESSENGER(task)->away_time > AWAY_BURST) {
                vec.x = 0.0F;
                vec.y = 0.0F;
                vec.z = 0.0F;
                sound_play_at(SOUND_BURST, 0, 0, 0, &work->pos);
                burst_create(&work->pos, &vec, BURST_EFFECT_SCALE,
                             BURST_EFFECT_DEPTH_BIAS);
                task_kill(task);
                return;
            }
        }
        work->submode &= ~SEEN;
        switch (work->mode) {
        case MODE_IDLE:
            turned = 0;
            work->submode &= ~WANDER;
            work->submode &= ~STEERING;
            if (task->child && collision_next_item_or_enemy(task->child)) {
                work->mode = MODE_BUMPED;
                break;
            }
            if (!MESSENGER(task)->timer) {
                VEC_SUB(&vec, &work->pos, &MESSENGER(task)->home);
                vec.y *= HOME_Y_SHARE;
                if (njScalor(&vec) > HOME_RANGE) {
                    work->mode = MODE_FLY;
                    work->submode &= ~WANDER;
                    work->submode |= STEERING;
                    MESSENGER(task)->goal = MESSENGER(task)->home;
                    MESSENGER(task)->goal.y += HOME_HEIGHT;
                } else {
                    if (MESSENGER(task)->home.y > work->pos.y) {
                        work->pos.y = MESSENGER(task)->home.y;
                        motion->spd.y = 0.0F;
                    }
                    found = 0;
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if ((other = collision_next_player(task))) {
                        if (player_tasks[1] == other) {
                            found = 2;
                        } else {
                            found = 1;
                        }
                    }
                    if (found && !message_window_current() &&
                        !MESSENGER(task)->window) {
                        found--;
                        work->mode = MODE_APPEAR;
                        MESSENGER(task)->player = found;
                        sound_play(SOUND_APPEAR, 0, 0, 0);
                    } else {
                        num = player_nearest(&work->pos);
                        VEC_SUB(&point, &player_work[num]->pos, &work->pos);
                        turn = RADIANS(atan2f(point.x, point.z)) - work->ang.y;
                        if (turn > IDLE_TURN_MOST) {
                            turn = IDLE_TURN_MOST;
                            turned = 1;
                        }
                        if (turn < -IDLE_TURN_MOST) {
                            turn = -IDLE_TURN_MOST;
                            turned = 1;
                        }
                        scaled = &turn;
                        SCALE_SINT16(scaled, IDLE_TURN_SHARE);
                        work->ang.y += turn;
                    }
                }
                MESSENGER(task)->scale -= messenger_shrink_rate;
                if (MESSENGER(task)->scale < 0.0F) {
                    MESSENGER(task)->scale = 0.0F;
                }
            }
            MESSENGER(task)->rotor += messenger_rotor_idle_rate;
            MESSENGER(task)->swing_speed *= messenger_swing_damping;
            if (work->flags & WORK_HELD_BY_PLAYER) {
                scaled = (Sint16 *)&work->ang.x;
                SCALE_SINT16(scaled, HELD_LEVEL_KEEP);
                scaled = (Sint16 *)&work->ang.z;
                SCALE_SINT16(scaled, HELD_LEVEL_KEEP);
                MESSENGER(task)->twirl += messenger_twirl_held_rate;
                MESSENGER(task)->bob_phase =
                    (int)(njSin(state->steps << HELD_BOB_SHIFT) *
                          (HELD_BOB_DEGREES * ANGLE_PER_DEGREE)) -
                    QUARTER_TURN;
                MESSENGER(task)->swing +=
                    (int)(MESSENGER(task)->swing_speed * ANGLE_PER_DEGREE) +
                    HELD_SWING_STEP;
                if (RANDOM_UNIT() < KICK_CHANCE) {
                    MESSENGER(task)->swing_speed +=
                        messenger_swing_kick * (RANDOM_UNIT() - KICK_BIAS);
                }
            } else {
                scaled = (Sint16 *)&work->ang.x;
                SCALE_SINT16(scaled, LEVEL_KEEP);
                scaled = (Sint16 *)&work->ang.z;
                SCALE_SINT16(scaled, LEVEL_KEEP);
                scaled = (Sint16 *)&MESSENGER(task)->bob_phase;
                SCALE_SINT16(scaled, REST_KEEP);
                if (turned) {
                    MESSENGER(task)->swing += IDLE_SWING_STEP;
                } else {
                    scaled = (Sint16 *)&MESSENGER(task)->swing;
                    SCALE_SINT16(scaled, REST_KEEP);
                }
            }
            if (njScalor(&motion->spd) < TRAIL_STILL_SPEED &&
                work->scl.y < PLAIN_SCL_Y) {
                trail_drop(MESSENGER(task)->trails_left[0]);
                trail_drop(MESSENGER(task)->trails_right[0]);
                trail_drop(MESSENGER(task)->trails_left[1]);
                trail_drop(MESSENGER(task)->trails_right[1]);
            }
            break;
        case MODE_APPEAR:
            MESSENGER(task)->rotor += messenger_rotor_idle_rate;
            scaled = (Sint16 *)&work->ang.x;
            SCALE_SINT16(scaled, LEVEL_KEEP);
            scaled = (Sint16 *)&work->ang.z;
            SCALE_SINT16(scaled, LEVEL_KEEP);
            MESSENGER(task)->scale += messenger_grow_rate;
            if (MESSENGER(task)->scale > one) {
                MESSENGER(task)->scale = one;
                if (!message_window_current()) {
                    work->mode = MODE_RISE;
                    motion->spd.y += appear_hop;
                }
            }
            break;
        case MODE_RISE:
            MESSENGER(task)->rotor += messenger_rotor_rate;
            MESSENGER(task)->twirl += messenger_twirl_rate;
            sound_play_at_for(SOUND_FLY, work, FLY_PRIORITY, 0, FLY_SOUND_STEPS,
                              &work->pos);
            if (!(work->submode & STEERING)) {
                work->submode |= STEERING;
                MESSENGER(task)->goal = work->pos;
                MESSENGER(task)->goal.y += rise_height;
            }
            if (njAbs(MESSENGER(task)->goal.y - work->pos.y) < RISE_DONE &&
                !message_window_current()) {
                work->mode = MODE_TALK;
                work->submode |= WANDER | STEERING;
                MESSENGER(task)->talk_time = TALK_STEPS;
                work->flags &= ~WORK_HURT;
                if (task->child) {
                    task->child->work->flags &= ~WORK_HURT;
                }
                num = stage_messages_count >= 0 &&
                              (int)njAbs(MESSAGE_NUMBER(work)) <
                                  stage_messages_count
                          ? (int)njAbs(MESSAGE_NUMBER(work))
                          : -1;
                /* @bug memory: the window reads through null where the stage's
                 * file holds message num without a text. messenger_create
                 * tests such a text for null, at 0x2c4e of the object's code.
                 * the text loaded at 0x146c goes to
                 * message_window_open_at_voice unchecked. the window's code
                 * passes it to strlen, in the main executable at 0x8c0a61de */
#ifdef NONMATCHING
                /* this side repairs the tag above: a message without a text
                 * opens NO_TEXT, as messenger_create's windows do */
                {
                    char *text = messenger_message(num);

                    message_window_open_at_voice(
                        talk_window_kind, text != NULL ? text : NO_TEXT,
                        MESSAGE_TIME, state->text_language,
                        &MESSENGER(task)->window, 0);
                }
#else
                message_window_open_at_voice(
                    talk_window_kind,
                    num >= 0 ? stage_messages[num].text : NO_TEXT, MESSAGE_TIME,
                    state->text_language, &MESSENGER(task)->window, 0);
#endif
                /* @hack redundant: a break ahead of the case's own */
                break;
            }
            break;
        case MODE_TALK:
            MESSENGER(task)->rotor += messenger_rotor_rate;
            MESSENGER(task)->twirl += messenger_twirl_rate;
            MESSENGER(task)->bob_phase += FLY_BOB_STEP;
            MESSENGER(task)->swing += FLY_SWING_STEP;
            sound_play_at_for(SOUND_FLY, work, FLY_PRIORITY, 0, FLY_SOUND_STEPS,
                              &work->pos);
            if (MESSENGER(task)->talk_time > 0) {
                MESSENGER(task)->talk_time--;
            }
            /* @hack spelling: hit set by an if, where hit = WAS_HIT(task, work)
             * would do */
            if (WAS_HIT(task, work)) {
                hit = 1;
            } else {
                hit = 0;
            }
            if ((MESSENGER(task)->talk_time <= 0 && !MESSENGER(task)->window) ||
                hit) {
                if (hit) {
                    work->mode = MODE_STRUCK;
                } else {
                    work->mode = MODE_FLY;
                    work->submode &= ~WANDER;
                    if (SUMMONED(task)) {
                        MESSENGER(task)->goal = MESSENGER(task)->home;
                        MESSENGER(task)->goal.y += LEAVE_HEIGHT;
                    } else {
                        MESSENGER(task)->goal = MESSENGER(task)->home;
                        MESSENGER(task)->goal.y += HOME_HEIGHT;
                    }
                }
            }
            break;
        case MODE_FLY:
            MESSENGER(task)->rotor += messenger_rotor_rate;
            MESSENGER(task)->twirl += messenger_twirl_rate;
            MESSENGER(task)->bob_phase += FLY_BOB_STEP;
            MESSENGER(task)->swing += FLY_SWING_STEP;
            sound_play_at_for(SOUND_FLY, work, FLY_PRIORITY, 0, FLY_SOUND_STEPS,
                              &work->pos);
            /* @hack spelling: x set, then lessened by work's x, where one
             * subtraction would do */
            x = MESSENGER(task)->goal.x;
            x -= work->pos.x;
            z = MESSENGER(task)->goal.z - work->pos.z;
            if ((x * x) + (z * z) < ARRIVED_SQUARED &&
                MESSENGER(task)->home.y < work->pos.y) {
                if (SUMMONED(task)) {
                    task_kill(task);
                    return;
                }
                motion->spd.x = 0.0F;
                motion->spd.z = 0.0F;
                work->mode = MODE_IDLE;
                MESSENGER(task)->timer = LANDED_STEPS;
            }
            /* @hack spelling: hit set by an if, and then tested, where a test
             * of WAS_HIT(task, work) would do */
            if (WAS_HIT(task, work)) {
                hit = 1;
            } else {
                hit = 0;
            }
            if (hit) {
                work->mode = MODE_STRUCK;
            }
            MESSENGER(task)->scale += messenger_grow_rate;
            if (MESSENGER(task)->scale > one) {
                MESSENGER(task)->scale = one;
            }
            break;
        case MODE_BUMPED:
            work->mode = MODE_STUNNED;
            MESSENGER(task)->timer = BUMPED_STEPS;
            vec.x = 0.0F;
            vec.y = 0.0F;
            vec.z = 0.0F;
            burst_create(&work->pos, &vec, BURST_EFFECT_SCALE,
                         BURST_EFFECT_DEPTH_BIAS);
            sound_play(SOUND_BURST, 0, 0, 0);
            MESSENGER(task)->flash_aim = one;
            motion->spd.x = 0.0F;
            motion->spd.y = STUN_HOP;
            motion->spd.z = 0.0F;
            messenger_say_random(SAY_STRUCK, &MESSENGER(task)->window);
            break;
        case MODE_STRUCK:
            work->mode = MODE_STUNNED;
            MESSENGER(task)->timer = STRUCK_STEPS;
            vec.x = 0.0F;
            vec.y = 0.0F;
            vec.z = 0.0F;
            burst_create(&work->pos, &vec, BURST_EFFECT_SCALE,
                         BURST_EFFECT_DEPTH_BIAS);
            sound_play(SOUND_BURST, 0, 0, 0);
            MESSENGER(task)->flash_aim = one;
            motion->spd.x = 0.0F;
            motion->spd.y = STUN_HOP;
            motion->spd.z = 0.0F;
            messenger_say_random(SAY_STRUCK, &MESSENGER(task)->window);
            break;
        case MODE_STUNNED:
            work->ang.x = STUN_PITCH;
            work->ang.z = QUARTER_TURN;
            work->submode &= ~WANDER;
            work->submode &= ~STEERING;
            /* @hack spelling: step added from the left, where += would do */
            step = -QUARTER_TURN - MESSENGER(task)->bob_phase;
            scaled = &step;
            SCALE_SINT16(scaled, STUN_EASE);
            MESSENGER(task)->bob_phase = step + MESSENGER(task)->bob_phase;
            step = -QUARTER_TURN - MESSENGER(task)->swing;
            scaled = &step;
            SCALE_SINT16(scaled, STUN_EASE);
            MESSENGER(task)->swing = step + MESSENGER(task)->swing;
            if (!MESSENGER(task)->timer) {
                if (SUMMONED(task)) {
                    work->mode = MODE_FLY;
                    work->submode &= ~WANDER;
                    work->submode |= STEERING;
                    MESSENGER(task)->goal = MESSENGER(task)->home;
                    MESSENGER(task)->goal.y += LEAVE_HEIGHT;
                    messenger_say_random(SAY_RECOVER, &MESSENGER(task)->window);
                } else {
                    work->mode = MODE_FLY;
                    work->submode &= ~WANDER;
                    work->submode |= STEERING;
                    MESSENGER(task)->goal = MESSENGER(task)->home;
                    MESSENGER(task)->goal.y += HOME_HEIGHT;
                    messenger_say_random(SAY_RECOVER, &MESSENGER(task)->window);
                }
            } else if (RANDOM_UNIT() < DAZE_CHANCE) {
                point = work->pos;
                point.x += (RANDOM_UNIT() - 0.5F) * DAZE_SPREAD;
                point.y += (RANDOM_UNIT() - 0.5F) * DAZE_SPREAD_Y;
                point.z += (RANDOM_UNIT() - 0.5F) * DAZE_SPREAD;
                vec.x = 0.0F;
                vec.y = DAZE_RISE;
                vec.z = 0.0F;
                /* @bug logic: starts each puff at work's pos, not at point,
                 * which it has just jittered from there: 0x1904 of the
                 * object's code */
                dust_steady_create(&work->pos, &vec, DAZE_SCALE,
                                   DAZE_DEPTH_BIAS);
            }
            break;
        }
        if (work->flags & WORK_HELD_BY_PLAYER) {
            num = player_holding(task);
            if (num >= 0) {
                /* face as the holder faces, turned by 0.18 of the sway */
                yaw = MESSENGER(task)->sway;
                scaled = (Sint16 *)&yaw;
                SCALE_SINT16(scaled, HELD_TURN_SHARE);
                work->ang.y = yaw - player_work[num]->ang.y + QUARTER_TURN;
                MESSENGER(task)->holder = num;
            }
            if (!MESSENGER(task)->held_time) {
                messenger_say_random(SAY_PICKED_UP, &MESSENGER(task)->window);
                MESSENGER(task)->swing_speed += messenger_swing_kick;
                messenger_events[MESSENGER(task)->holder] =
                    MESSENGER_EVENT_NONE;
            } else {
                switch (messenger_events[MESSENGER(task)->holder]) {
                case MESSENGER_EVENT_SPRING:
                    messenger_say_random(SAY_SPRING, &MESSENGER(task)->window);
                    break;
                case MESSENGER_EVENT_SAVE_POINT:
                    messenger_say_random(SAY_SAVE_POINT,
                                         &MESSENGER(task)->window);
                    break;
                case MESSENGER_EVENT_GOAL_RING:
                    switch (stage_current_id()) {
                    case STAGE_CITY_ESCAPE:
                        messenger_say(SAY_GOAL_RING, &MESSENGER(task)->window);
                        break;
                    case STAGE_WILD_CANYON:
                        messenger_say(SAY_GOAL_RING + 1,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_METAL_HARBOR:
                        messenger_say(SAY_GOAL_RING + 2,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_GREEN_FOREST:
                        messenger_say(SAY_GOAL_RING + 3,
                                      &MESSENGER(task)->window);
                        break;
                    /* @bug logic: a goal ring's event in Pumpkin Hill or
                     * Aquatic Mine goes unanswered. the two cases below swap
                     * stage and message: they test for stages SAY_GOAL_RING
                     * + 4 and + 5, and say messages STAGE_PUMPKIN_HILL and
                     * STAGE_AQUATIC_MINE. the tests lie at 0x1a52 and 0x1a5a
                     * of the object's code */
                    case SAY_GOAL_RING + 4:
                        messenger_say(STAGE_PUMPKIN_HILL,
                                      &MESSENGER(task)->window);
                        break;
                    case SAY_GOAL_RING + 5:
                        messenger_say(STAGE_AQUATIC_MINE,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_PYRAMID_CAVE:
                        messenger_say(SAY_GOAL_RING + 6,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_DEATH_CHAMBER:
                        messenger_say(SAY_GOAL_RING + 7,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_METEOR_HERD:
                        messenger_say(SAY_GOAL_RING + 8,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_CRAZY_GADGET:
                        messenger_say(SAY_GOAL_RING + 9,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_FINAL_RUSH:
                        messenger_say(SAY_GOAL_RING + 10,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_DRY_LAGOON:
                        messenger_say(SAY_GOAL_RING + 11,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_RADICAL_HIGHWAY:
                        messenger_say(SAY_GOAL_RING + 12,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_EGG_QUARTERS:
                        messenger_say(SAY_GOAL_RING + 13,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_SECURITY_HALL:
                        messenger_say(SAY_GOAL_RING + 14,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_WHITE_JUNGLE:
                        messenger_say(SAY_GOAL_RING + 15,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_SKY_RAIL:
                        messenger_say(SAY_GOAL_RING + 16,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_MAD_SPACE:
                        messenger_say(SAY_GOAL_RING + 17,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_FINAL_CHASE:
                        messenger_say(SAY_GOAL_RING + 18,
                                      &MESSENGER(task)->window);
                        break;
                    case STAGE_RELAY_SONIC:
                    case STAGE_RELAY_EGGMAN:
                    case STAGE_RELAY_TAILS:
                    case STAGE_RELAY_ROUGE:
                    case STAGE_RELAY_KNUCKLES:
                        messenger_say(SAY_GOAL_RING + 19,
                                      &MESSENGER(task)->window);
                        break;
                    /* @hack redundant: a default that only breaks */
                    default:
                        break;
                    }
                    break;
                default:
                    if (!MESSENGER(task)->window && !message_window_current() &&
                        MESSENGER(task)->quiet_time > CHAT_QUIET &&
                        RANDOM_UNIT() < CHAT_CHANCE) {
                        if (MESSENGER(task)->chats <= 1) {
                            messenger_say_random(SAY_CHAT,
                                                 &MESSENGER(task)->window);
                            MESSENGER(task)->chats++;
                        } else if (MESSENGER(task)->chats >= 3) {
                            messenger_say_random(SAY_CHAT_FOURTH,
                                                 &MESSENGER(task)->window);
                            MESSENGER(task)->chats = 0;
                        } else {
                            messenger_say_random(SAY_CHAT_THIRD,
                                                 &MESSENGER(task)->window);
                            MESSENGER(task)->chats++;
                        }
                        MESSENGER(task)->swing_speed += messenger_swing_kick;
                    }
                    break;
                }
                messenger_events[MESSENGER(task)->holder] =
                    MESSENGER_EVENT_NONE;
            }
            MESSENGER(task)->timer = HELD_PAUSE;
            work->mode = MODE_IDLE;
            MESSENGER(task)->sway = njSin(state->steps * SWAY_RATE) *
                                    (SWAY_DEGREES * ANGLE_PER_DEGREE);
            if (MESSENGER(task)->held_time < HELD_MOST) {
                MESSENGER(task)->held_time++;
            }
        } else {
            MESSENGER(task)->held_time = 0;
            MESSENGER(task)->chats = 0;
            scaled = (Sint16 *)&MESSENGER(task)->sway;
            SCALE_SINT16(scaled, SWAY_KEEP);
            if (MESSENGER(task)->timer == HELD_PAUSE - LET_GO_DELAY &&
                work->mode == MODE_IDLE && !game_state.cleared) {
                if (njScalor(&motion->spd) < one) {
                    messenger_say_random(SAY_SET_DOWN,
                                         &MESSENGER(task)->window);
                } else {
                    messenger_say_random(SAY_THROWN, &MESSENGER(task)->window);
                }
            }
        }
        if (MESSENGER(task)->timer) {
            work->flags &= ~WORK_HURT;
            if (task->child) {
                task->child->work->flags &= ~WORK_HURT;
            }
            work->submode &= ~(WANDER | STEERING);
            MESSENGER(task)->timer--;
        }
        /* @hack spelling: flash plus its change, where += would do */
        MESSENGER(task)->flash =
            MESSENGER(task)->flash +
            ((MESSENGER(task)->flash_aim - MESSENGER(task)->flash) *
             FLASH_EASE);
        MESSENGER(task)->flash_aim *= FLASH_AIM_KEEP;
        messenger_move(task);
    }
    /* @hack spelling: a second if on time_stopped, where the block above would
     * do */
    if (!state->time_stopped) {
        struct field_sample sample;

        field_sample_at(&work->pos, &sample);
        /* @hack spelling: each speed plus its change, where += would do */
        if (sample.flags & FIELD_SPD) {
            motion->spd.x =
                motion->spd.x + ((sample.spd.x - motion->spd.x) * FIELD_EASE);
            motion->spd.y =
                motion->spd.y + ((sample.spd.y - motion->spd.y) * FIELD_EASE);
            motion->spd.z =
                motion->spd.z + ((sample.spd.z - motion->spd.z) * FIELD_EASE);
        }
        if ((sample.flags & FIELD_KILLS) && work->mode != MODE_TALK) {
            task_kill(task);
            return;
        }
    }
    body_step(task);
    if (!state->time_stopped) {
        if (work->submode & STEERING) {
            if (njScalor(&motion->spd) > FACE_SPEED) {
                x = motion->spd.x;
                z = motion->spd.z;
                /* @hack spelling: the pitch stored to work's ang.x and read
                 * back, where pitch set first would do */
                work->ang.x =
                    -RADIANS(atan2f(motion->spd.y, njSqrt((x * x) + (z * z))));
                work->ang.y = RADIANS(atan2f(motion->spd.x, motion->spd.z));
                pitch = work->ang.x;
                if (pitch > PITCH_MOST) {
                    pitch = PITCH_MOST;
                }
                if (pitch < -PITCH_MOST) {
                    pitch = -PITCH_MOST;
                }
                work->ang.x = pitch;
            }
            messenger_ease_angles(task, &ang);
        }
    }
    if (task->child) {
        child_work = task->child->work;
        child_work->pos = work->pos;
        if ((work->flags & WORK_HELD_BY_PLAYER) ||
            (motion->spd.x * motion->spd.x) + (motion->spd.z * motion->spd.z) <
                COLLIDER_SPEED_SQUARED) {
            for (i = 0; i < child_work->collision->count; i++) {
                child_work->collision->shapes[i].flags |= SHAPE_OFF;
            }
        } else {
            for (i = 0; i < child_work->collision->count; i++) {
                child_work->collision->shapes[i].flags &= ~SHAPE_OFF;
            }
            child_work->flags &= ~WORK_HURT;
        }
    }
}

/* the draw shows nothing unless a sphere of DRAW_RADIUS about pos lies in
 * front of the camera. it bobs the messenger up and down by up to BOB_HEIGHT.
 * it draws the shadow SHADOW_LIFT above the bob, scaled by SHADOW_SCALE in x
 * and z */
#define DRAW_RADIUS 100.0F
#define BOB_HEIGHT 2.3F
#define SHADOW_LIFT 0.1F
#define SHADOW_SCALE 3.0F
/* while flash is above FLASH_LEAST, the draw reddens the land's light. at a
 * flash of 1, the light loses FLASH_COLOR_LOSS of its green and blue. its
 * ambient then gains FLASH_AMBIENT_GAIN of the way to 1 */
#define FLASH_LEAST 0.01F
#define FLASH_COLOR_LOSS 0.9F
#define FLASH_AMBIENT_GAIN 0.6F
/* the face lights are off while the messenger flies or is stunned. while it
 * appears or rises, they are off for the first half of every BLINK_STEPS */
#define BLINK_STEPS 20
/* the draw's pose. the limbs take LIMB_PITCH_SHARE of the pitch and
 * LIMB_BANK_SHARE of the bank. the head turns about x by HEAD_SWAY_SHARE of the
 * sway. it turns about y by the sway plus HEAD_TWIST times the limbs' bank. the
 * arms swing ARM_SWING degrees either way, their phases ARM_PHASE before and
 * after swing. the feet swing FOOT_SWING degrees, the left's phase
 * LEFT_FOOT_PHASE before swing and the right's RIGHT_FOOT_PHASE after. the left
 * arm takes a bank below ARM_BANK_SPLIT as a Uint16, and the right one above */
#define LIMB_PITCH_SHARE 0.4F
#define LIMB_BANK_SHARE 0.6F
#define HEAD_SWAY_SHARE 0.2F
#define HEAD_TWIST 3.0F
#define ARM_SWING 30
#define ARM_PHASE 0x400
#define FOOT_SWING 20
#define LEFT_FOOT_PHASE 0x800
#define RIGHT_FOOT_PHASE 0x600
#define ARM_BANK_SPLIT 0x7000
/* the propeller draws only while its scale is above PROPELLER_LEAST */
#define PROPELLER_LEAST 0.01F
/* how far ahead of the camera the draw shows a part */
#define BODY_RANGE 300.0F
#define KEY_RANGE 150.0F /* the winding key's */
#define LIMB_RANGE 200.0F /* the propeller's and the arms' */

static void messenger_draw(struct task *task)
{
#ifdef DREAMCAST
#ifdef NONMATCHING
    /* under -fpack-struct=4 the struct below aligns only to 4, and GCC warns
     * that its NJS_MATRIX members want 8. the draw hands Ninja no member: it
     * hands a pointer into storage, rounded down to 8 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wif-not-aligned"
#endif
#endif
    /* @hack type: the matrix's pointer and the space it points into kept in
     * one struct, where a local pointer and an array of 17 Floats would do */
    struct {
        NJS_MATRIX *matrix;
        union {
            NJS_MATRIX matrix;
            struct {
                Float alignment;
                NJS_MATRIX matrix;
            } shifted;
        } storage;
    } aligned;
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the diagnostic push above the aligned union */
#pragma GCC diagnostic pop
#endif
#endif
    struct task_work *work;
    Angle az;
    Float dist;
    Angle ax;
    Angle roll;
    Angle twist;
    NJS_CNK_OBJECT *root;
    Float bob;
    Angle swing;
    /* @hack spelling: scaled, a local each SCALE_SINT16 goes through, where the
     * address in the call would do */
    Sint16 *scaled;
    int mode;

    work = task->work;
    /* aligned.matrix holds the messenger's matrix on an 8-byte boundary.
     * shifted.matrix lies 4 bytes into storage. its address rounded down to 8
     * still leaves the whole matrix inside storage */
    aligned.matrix =
        (NJS_MATRIX *)((Uint32)&aligned.storage.shifted.matrix & ~7);
    if (!sphere_in_front(&work->pos, DRAW_RADIUS, &dist)) {
        return;
    }
    work->submode |= SEEN;
    bob = njSin(MESSENGER(task)->bob_phase) * BOB_HEIGHT;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njPushMatrixEx();
    njUnitMatrix(0);
    if (work->flags & WORK_HELD_BY_PLAYER) {
        njTranslateEx(&work->pos);
        njRotateZ(0, player_work[MESSENGER(task)->holder]->ang.z);
        njRotateX(0, player_work[MESSENGER(task)->holder]->ang.x);
        njTranslate(0, 0.0F, bob, 0.0F);
    } else {
        njTranslate(0, work->pos.x, work->pos.y + bob, work->pos.z);
    }
    njRotateY(0, work->ang.y);
    njRotateX(0, work->ang.x);
    njRotateZ(0, work->ang.z);
    njGetMatrix(aligned.matrix);
    {
        NJS_POINT3 left;
        NJS_POINT3 right;

        njCalcPoint(0, &trail_left, &left);
        njCalcPoint(0, &trail_right, &right);
        njPopMatrixEx();
        if (work->scl.y < PLAIN_SCL_Y) {
            trail_draw(&messenger_trail_style,
                       MESSENGER(task)->trails_left[camera_current_index],
                       &left);
            trail_draw(&messenger_trail_style,
                       MESSENGER(task)->trails_right[camera_current_index],
                       &right);
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    if (objects_fog_off) {
        njFogDisable();
    }
    if (MESSENGER(task)->flash > FLASH_LEAST) {
        struct light light;
        /* @hack spelling: lp, a pointer to light, where light itself would
         * do */
        struct light *lp;

        light = lights[LIGHT_LAND];
        light.g *= 1.0F - (MESSENGER(task)->flash * FLASH_COLOR_LOSS);
        light.b *= 1.0F - (MESSENGER(task)->flash * FLASH_COLOR_LOSS);
        /* @hack spelling: ambient plus its change, where += would do */
        light.ambient =
            light.ambient + ((1.0F - light.ambient) * MESSENGER(task)->flash *
                             FLASH_AMBIENT_GAIN);
        lp = &light;
        njSetDirectLightIntensity(lp->intensity, lp->ambient);
        njCnkSetEasyLightIntensity(lp->intensity, lp->ambient);
        njCnkSetSimpleLightIntensity(lp->intensity, lp->ambient);
        njCnkSetEasyLightColor(lp->r, lp->g, lp->b);
        njSetDirectLightColor(lp->r, lp->g, lp->b);
        njCnkSetSimpleLightColor(lp->r, lp->g, lp->b);
    }
    njPushMatrixEx();
    njTranslate(0, work->pos.x, work->pos.y + bob + SHADOW_LIFT, work->pos.z);
    njScale(0, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
    njCnkModDrawObject(&shadow_object);
    njPopMatrixEx();
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((mode = work->mode) == MODE_FLY || mode == MODE_STUNNED ||
        ((mode == MODE_APPEAR || mode == MODE_RISE) &&
         game_state.steps % BLINK_STEPS < BLINK_STEPS / 2)) {
        messenger_face_lights->evalflags |= NJD_EVAL_HIDE;
    } else {
        messenger_face_lights->evalflags &= ~NJD_EVAL_HIDE;
    }
    messenger_propeller.evalflags &= ~(NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    messenger_propeller.ang[1] = MESSENGER(task)->twirl;
    messenger_propeller.scl[0] = MESSENGER(task)->scale;
    messenger_propeller.scl[1] = MESSENGER(task)->scale;
    messenger_propeller.scl[2] = MESSENGER(task)->scale;
    messenger_winding_key->evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_winding_key->ang[2] = MESSENGER(task)->rotor;
    /* @hack spelling: swing read ahead of ax and az, where after them would
     * do */
    swing = MESSENGER(task)->swing;
    ax = work->ang.x;
    az = work->ang.z;
    scaled = (Sint16 *)&ax;
    SCALE_SINT16(scaled, LIMB_PITCH_SHARE);
    scaled = (Sint16 *)&az;
    SCALE_SINT16(scaled, LIMB_BANK_SHARE);
    twist = az;
    scaled = (Sint16 *)&twist;
    SCALE_SINT16(scaled, HEAD_TWIST);
    roll = MESSENGER(task)->sway;
    scaled = (Sint16 *)&roll;
    SCALE_SINT16(scaled, HEAD_SWAY_SHARE);
    messenger_left_arm->evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_left_arm->ang[0] =
        ax - (Angle)(njSin(swing - ARM_PHASE) * (ARM_SWING * ANGLE_PER_DEGREE));
    messenger_left_arm->ang[2] = (Uint16)az < ARM_BANK_SPLIT ? -az : 0;
    messenger_right_arm->evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_right_arm->ang[0] = -ax + (Angle)(njSin(swing + ARM_PHASE) *
                                                (ARM_SWING * ANGLE_PER_DEGREE));
    messenger_right_arm->ang[2] = (Uint16)az > ARM_BANK_SPLIT ? -az : 0;
    messenger_left_foot->evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_left_foot->ang[0] =
        -ax + (Angle)(njSin(swing - LEFT_FOOT_PHASE) *
                      (FOOT_SWING * ANGLE_PER_DEGREE));
    messenger_right_foot->evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_right_foot->ang[0] =
        ax - (Angle)(njSin(swing + RIGHT_FOOT_PHASE) *
                     (FOOT_SWING * ANGLE_PER_DEGREE));
    messenger_head.evalflags &= ~NJD_EVAL_UNIT_ANG;
    messenger_head.ang[0] = roll;
    messenger_head.ang[1] = twist + MESSENGER(task)->sway;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&messenger_texlist);
    njPushMatrixEx();
    njMultiMatrix(0, aligned.matrix);
    /* @hack spelling: root set by a comma in the test, where a statement of
     * its own would do */
    if (root = messenger_body, messenger_built) {
        njTranslateEx((NJS_VECTOR *)root->pos);
        njRotateEx(messenger_body->ang, 0);
        if (dist < BODY_RANGE) {
            njDirectDrawObject(messenger_builds[MESSENGER_BUILD_BODY].built);
        }
        njPushMatrixEx();
        if (dist < KEY_RANGE) {
            njTranslateEx((NJS_VECTOR *)messenger_winding_key->pos);
            njRotateEx(messenger_winding_key->ang, 0);
            njDirectDrawObject(
                messenger_builds[MESSENGER_BUILD_WINDING_KEY].built);
            matrix_load_parent();
        }
        njTranslateEx((NJS_VECTOR *)messenger_head.pos);
        njRotateEx(messenger_head.ang, 0);
        njCnkEasyDrawModel(messenger_head.model);
        njPushMatrixEx();
        if (dist < LIMB_RANGE && MESSENGER(task)->scale > PROPELLER_LEAST) {
            njTranslateEx((NJS_VECTOR *)messenger_propeller.pos);
            njRotateEx(messenger_propeller.ang, 0);
            njScaleEx((NJS_VECTOR *)messenger_propeller.scl);
            njDirectDrawObject(
                messenger_builds[MESSENGER_BUILD_PROPELLER].built);
            matrix_load_parent();
        }
        if (!(messenger_face_lights->evalflags & NJD_EVAL_HIDE)) {
            njTranslateEx((NJS_VECTOR *)messenger_face_lights->pos);
            if (messenger_white_face_build) {
                njDirectDrawObject(messenger_white_face_build);
            } else {
                model_draw_colored(messenger_face_lights->model, -1);
            }
        }
        njPopMatrixEx();
        matrix_load_parent();
        if (dist < LIMB_RANGE) {
            njTranslateEx((NJS_VECTOR *)messenger_right_arm->pos);
            njRotateEx(messenger_right_arm->ang, 0);
            njDirectDrawObject(
                messenger_builds[MESSENGER_BUILD_RIGHT_ARM].built);
            matrix_load_parent();
            njTranslateEx((NJS_VECTOR *)messenger_left_arm->pos);
            njRotateEx(messenger_left_arm->ang, 0);
            njDirectDrawObject(
                messenger_builds[MESSENGER_BUILD_LEFT_ARM].built);
            matrix_load_parent();
        }
        njTranslateEx((NJS_VECTOR *)messenger_left_foot->pos);
        njRotateEx(messenger_left_foot->ang, 0);
        njDirectDrawObject(messenger_builds[MESSENGER_BUILD_LEFT_FOOT].built);
        matrix_load_parent();
        njTranslateEx((NJS_VECTOR *)messenger_right_foot->pos);
        njRotateEx(messenger_right_foot->ang, 0);
        njDirectDrawObject(messenger_builds[MESSENGER_BUILD_RIGHT_FOOT].built);
        matrix_load_parent();
    } else {
        njTranslateEx((NJS_VECTOR *)root->pos);
        njRotateEx(messenger_body->ang, 0);
        if (dist < BODY_RANGE) {
            njCnkEasyDrawModel(messenger_body->model);
        }
        njPushMatrixEx();
        if (dist < KEY_RANGE) {
            njTranslateEx((NJS_VECTOR *)messenger_winding_key->pos);
            njRotateEx(messenger_winding_key->ang, 0);
            njCnkEasyDrawModel(messenger_winding_key->model);
            matrix_load_parent();
        }
        njTranslateEx((NJS_VECTOR *)messenger_head.pos);
        njRotateEx(messenger_head.ang, 0);
        njCnkEasyDrawModel(messenger_head.model);
        njPushMatrixEx();
        if (dist < LIMB_RANGE && MESSENGER(task)->scale > PROPELLER_LEAST) {
            njTranslateEx((NJS_VECTOR *)messenger_propeller.pos);
            njRotateEx(messenger_propeller.ang, 0);
            njScaleEx((NJS_VECTOR *)messenger_propeller.scl);
            njCnkEasyDrawModel(messenger_propeller.model);
            matrix_load_parent();
        }
        if (!(messenger_face_lights->evalflags & NJD_EVAL_HIDE)) {
            njTranslateEx((NJS_VECTOR *)messenger_face_lights->pos);
            model_draw_colored(messenger_face_lights->model, -1);
        }
        njPopMatrixEx();
        matrix_load_parent();
        if (dist < LIMB_RANGE) {
            njTranslateEx((NJS_VECTOR *)messenger_right_arm->pos);
            njRotateEx(messenger_right_arm->ang, 0);
            njCnkEasyDrawModel(messenger_right_arm->model);
            matrix_load_parent();
            njTranslateEx((NJS_VECTOR *)messenger_left_arm->pos);
            njRotateEx(messenger_left_arm->ang, 0);
            njCnkEasyDrawModel(messenger_left_arm->model);
            matrix_load_parent();
        }
        njTranslateEx((NJS_VECTOR *)messenger_left_foot->pos);
        njRotateEx(messenger_left_foot->ang, 0);
        njCnkEasyDrawModel(messenger_left_foot->model);
        matrix_load_parent();
        njTranslateEx((NJS_VECTOR *)messenger_right_foot->pos);
        njRotateEx(messenger_right_foot->ang, 0);
        njCnkEasyDrawModel(messenger_right_foot->model);
        matrix_load_parent();
    }
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (MESSENGER(task)->flash > FLASH_LEAST) {
        light_apply(LIGHT_LAND);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
}

/* a collider registers its collision until its messenger is killed */
static void messenger_collider_step(struct task *task)
{
    if (!task->parent || task->parent->step != task_destroy) {
        collision_register(task);
    }
}

static void messenger_destroy(struct task *task);

void messenger_init(struct task *task)
{
    struct task_motion *motion;
    struct task_work *work;
    struct task *child;
    NJS_POINT3 pos;
    NJS_VECTOR player_spd;

    work = task->work;
    motion = task->motion.p;
    /* @hack spelling: task's motion read again for the test, where motion
     * would do */
    if (!task->motion.p) {
        return;
    }
    if (!SUMMONED(task) && placement_try_leave(task)) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct messenger));
    if (!task->extra.p) {
        return;
    }
    MESSENGER(task)->trails_left[0] =
        trail_alloc(&messenger_trail_style, TRAIL_POINTS);
    MESSENGER(task)->trails_left[1] =
        trail_alloc(&messenger_trail_style, TRAIL_POINTS);
    MESSENGER(task)->trails_right[0] =
        trail_alloc(&messenger_trail_style, TRAIL_POINTS);
    MESSENGER(task)->trails_right[1] =
        trail_alloc(&messenger_trail_style, TRAIL_POINTS);
    task->draw = messenger_draw;
    task->step = messenger_step;
    task->destructor = messenger_destroy;
    MESSENGER(task)->window = 0;
    MESSENGER(task)->home = work->pos;
    MESSENGER(task)->unk_4c = -1;
    MESSENGER(task)->player = 0;
/* @hack redundant: a label nothing jumps to */
start: /* NOLINT(clang-diagnostic-unused-label) */
    messenger_player_ahead(0, &pos, &player_spd);
    collision_init(task, SUMMONED(task) ? summoned_shapes : placed_shapes,
                   SHAPES, COLLISION_UNK_9);
    body_init(task);
    motion->hold_turn = hold_turn;
    if (task->force.p) {
        BODY(task)->flags |= BODY_RESTS;
    }
#ifdef NONMATCHING
    /* this side repairs the tag in messenger_move: a messenger without a body
     * kills itself, so it ends at its next step, before any step moves it */
    if (task->force.p == NULL) {
        task_kill(task);
    }
#endif
    messenger_pick_goal(task, &pos, &player_spd);
    child = task_create_child(TASK_WORK, messenger_collider_step, task);
    if (child) {
        child->work->pos = work->pos;
        collision_init(child,
                       SUMMONED(task) ? summoned_collider_shapes
                                      : placed_collider_shapes,
                       1, COLLISION_ATTACK);
        child->work->id = WORK_ID_ATTACK;
    }
}

static void messenger_destroy(struct task *task)
{
    if (MESSENGER(task)->window) {
        task_destroy(MESSENGER(task)->window);
        MESSENGER(task)->window = 0;
    }
    body_destroy(task);
    trail_free(&messenger_trail_style, MESSENGER(task)->trails_left[0]);
    trail_free(&messenger_trail_style, MESSENGER(task)->trails_left[1]);
    trail_free(&messenger_trail_style, MESSENGER(task)->trails_right[0]);
    trail_free(&messenger_trail_style, MESSENGER(task)->trails_right[1]);
    syFree(task->extra.p);
    /* task_destroy frees task->extra after the destructor runs, so the
     * destructor clears the pointer to the record it freed */
    task->extra.p = 0;
}

static void messenger_window_step(struct task *task)
{
    if (!WINDOW(task)) {
        task_destroy(task);
    }
}

static void messenger_window_destroy(struct task *task)
{
    if (WINDOW(task)) {
        task_destroy(WINDOW(task));
    }
}

/* the text of stage message m, or NO_TEXT where the stage's file holds none */
/* @hack spelling: messenger_message called twice, where its result kept in
 * a local would do */
#define MESSAGE_TEXT(m) (messenger_message(m) ? messenger_message(m) : NO_TEXT)

struct task *messenger_create(NJS_POINT3 *pos, Angle ang_y, int player,
                              int message, enum messenger_create_mode mode,
                              struct task *parent)
{
    struct task *task;
    struct task_work *work;

    switch (mode) {
    case MESSENGER_SAY_WINDOW:
        if (parent) {
            task = task_create_child(TASK_WORK, messenger_window_step, parent);
        } else {
            task = task_create(TASK_WORK, TASK_LEVEL, messenger_window_step,
                               "MsgerDmyTaskExec");
        }
        if (task) {
            task->destructor = messenger_window_destroy;
            message_window_open_at_voice(
                MESSAGE_WINDOW_HIDDEN, MESSAGE_TEXT(message), MESSAGE_TIME,
                game_state.text_language, &WINDOW(task), 0);
        } else {
            /* @hack redundant: an empty else */
        }
        return task;
    case MESSENGER_TALK_WINDOW:
        if (parent) {
            task = task_create_child(TASK_WORK, messenger_window_step, parent);
        } else {
            task = task_create(TASK_WORK, TASK_LEVEL, messenger_window_step,
                               "MsgerDmyTaskExec");
        }
        if (task) {
            task->destructor = messenger_window_destroy;
            message_window_open_at_voice(
                talk_window_kind, MESSAGE_TEXT(message), MESSAGE_TIME,
                game_state.text_language, &WINDOW(task), 0);
        } else {
            /* @hack redundant: an empty else */
        }
        return task;
    case MESSENGER_SUMMON:
        if (parent) {
            task = task_create_child(TASK_MOTION | TASK_WORK, messenger_init,
                                     parent);
        } else {
            task = task_create(TASK_MOTION | TASK_WORK, TASK_LEVEL,
                               messenger_init, "ObjectMessenger");
        }
        if (task) {
            work = task->work;
            work->pos = *pos;
            work->ang.y = ang_y;
            work->ang.x = 0;
            work->ang.z = 0;
            MESSAGE_NUMBER(work) = message;
            work->scl.y = PLAIN_SCL_Y;
            SUMMONED(task) = mode;
            messenger_init(task);
            /* @bug memory: writes the messenger's record through null where
             * messenger_init's syCalloc failed. messenger_init tests the
             * record for null. the write is at 0x2d70 of the object's code,
             * and the TST at 0x29be */
#ifdef NONMATCHING
            /* this side repairs the tag above: where messenger_init made no
             * record, it destroys the task and returns null */
            if (MESSENGER(task) == NULL) {
                task_destroy(task);
                return NULL;
            }
#endif
            MESSENGER(task)->player = player;
            MESSENGER(task)->scale = 1.0F;
            work->mode = MODE_RISE;
            MESSENGER(task)->goal = work->pos;
            sound_play(SOUND_APPEAR, 0, 0, 0);
        }
        return task;
    }
    return 0;
}
