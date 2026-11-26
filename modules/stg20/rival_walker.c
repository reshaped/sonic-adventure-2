/* ControlTWalker, the task that steers player 1's walker against player 0 in
 * the first walker duel */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/bossinit.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/messenger.h"
#include "shared/rival_heading.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg20/rival_walker.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

enum rival_walker_mode {
    MODE_WAIT, /* wait for STAGE_PLAY, then start the intro */
    /* say player 1's line where the record's steps are INTRO_VOICE_STEP.
     * once they pass INTRO_STEPS, end the intro and change to MODE_OPENING */
    MODE_INTRO,
    MODE_ATTACK, /* head for player 0, firing now and then */
    MODE_UNUSED,
    MODE_CIRCLE, /* run round player 0 */
    /* face player 0 while player 0's last_flags hold PLAYER_WORK_GROUNDED,
     * firing now and then */
    MODE_AIM,
    MODE_DODGE, /* run at right angles to the line to player 0 */
    /* steer nothing for OPENING_STEPS. Tails's walker changes to MODE_DODGE
     * as soon as player 0 aims at it */
    MODE_OPENING,
    /* run at player 0, and fire every 8th step while player 1's hit lasts. a
     * step without a hit changes to MODE_CIRCLE */
    MODE_HURT_FIRE,
    /* steer nothing. a step where either player's record holds PLAYER_MISSED
     * sets it, and only a start over ends it */
    MODE_DOWN,
    MODE_ATTACK_AGAIN /* runs as MODE_ATTACK does. nothing sets it */
};

/* ControlTWalker's record, the task's extra */
struct rival_walker {
    Sint8 mode; /* an enum rival_walker_mode */
    /* TRUE from the intro's end until a start over. the intro's end summons a
     * messenger only while it is FALSE, which it always is there. MODE_INTRO
     * follows only MODE_WAIT. the record starts in MODE_WAIT with the flag
     * FALSE, and a start over sets both again */
    Sint8 intro_done;
    /* the steps counted at each step's end. each change of mode zeroes the
     * count but the changes to MODE_INTRO and to MODE_DOWN, which keep it. a
     * shot in MODE_AIM or MODE_HURT_FIRE zeroes it too */
    Sint16 steps;
    Sint16 length; /* the steps a timed mode lasts */
    /* the steps before player 1 may taunt, counted down to 0 at each step's
     * end */
    Sint16 voice_wait;
    /* player 1's gauge times GAUGE_SHOWN, which the boss gauge shows */
    Float gauge;
    NJS_POINT3 focus; /* the point the intro's camera looks at */
};

/* the slots of player 0, whom the task fights, and of player 1, whose
 * walker it steers */
#define PLAYER_SLOT 0
#define RIVAL_SLOT 1
#define CONTROL_LEVEL 0

/* the steps each walker's timed modes last: the mode's least, plus at random
 * up to its spread */
#define EGGMAN_DODGE_LEAST 180.0F
#define EGGMAN_DODGE_SPREAD 180.0F
#define TAILS_DODGE_LEAST 60.0F
#define TAILS_DODGE_SPREAD 120.0F
#define EGGMAN_CIRCLE_LEAST 240.0F
#define EGGMAN_CIRCLE_SPREAD 240.0F
#define TAILS_CIRCLE_LEAST 60.0F
#define TAILS_CIRCLE_SPREAD 180.0F
#define EGGMAN_ATTACK_LEAST 180.0F
#define EGGMAN_ATTACK_SPREAD 120.0F
#define TAILS_ATTACK_LEAST 300.0F
#define TAILS_ATTACK_SPREAD 420.0F
/* MODE_OPENING's length, which it never reads */
#define OPENING_LEAST 240.0F
#define OPENING_SPREAD 120.0F
/* MODE_OPENING changes to MODE_ATTACK once its steps pass OPENING_STEPS.
 * MODE_AIM changes to MODE_CIRCLE once AIM_STEPS pass without a shot */
#define OPENING_STEPS 120
#define AIM_STEPS 64

/* the intro lasts until the record's steps pass INTRO_STEPS, counting those
 * MODE_WAIT ran. where the count is INTRO_VOICE_STEP at a step of the intro,
 * player 1 says its line. it then taunts no sooner than INTRO_TAUNT_WAIT
 * steps later */
#define INTRO_STEPS 360
#define INTRO_VOICE_STEP 120
#define INTRO_TAUNT_WAIT 180
/* the intro's BOSSINIT shot looks at the record's focus. the focus lies at
 * player 1's x and z as the intro starts, INTRO_LIFT above the arena's
 * center. the shot advances INTRO_SPEED frames a step. it closes in from
 * INTRO_START_DIST to INTRO_DIST, then turns by INTRO_TURN. its ang is player
 * 1's ang.y less INTRO_CAMERA_ANG, which comes to ang.y plus a sixteenth of a
 * turn */
#define INTRO_SPEED 1.0F
#define INTRO_CAMERA_ANG 0xf000
#define INTRO_LIFT 14.0F
#define INTRO_START_DIST 100.0F
#define INTRO_DIST 13.0F
#define INTRO_TURN 0x6000
/* after a retry, the intro's end summons a messenger MESSENGER_LIFT above the
 * arena's center. the stage message it shows is numbered by the retries, as
 * enum retry_message counts them. where player 0 drives Eggman's walker, the
 * number is RETRY_MESSAGES higher */
#define MESSENGER_LIFT 300.0F
#define NO_MESSAGE (-1)
enum retry_message {
    ONE_RETRY_MESSAGE,
    TWO_RETRIES_MESSAGE,
    MORE_RETRIES_MESSAGE,
    RETRY_MESSAGES
};
/* the lines each walker says at the intro */
#define TAILS_INTRO_VOICE 0x8d3
#define EGGMAN_INTRO_VOICE 0x813
#define NO_VOICE (-1)
/* each walker's two taunts. it says the first where its first offered action
 * is its strike, and the second otherwise. player 1 taunts at TAUNT_PRIORITY,
 * at most once every TAUNT_WAIT steps */
#define TAILS_STRIKE_TAUNT 0x8d6
#define TAILS_TAUNT 0x8d7
#define EGGMAN_STRIKE_TAUNT 0x816
#define EGGMAN_TAUNT 0x817
#define TAUNT_PRIORITY 3
#define TAUNT_WAIT 480

/* the strokes the task gives player 1's stick. Tails's walker runs at
 * STROKE_FULL, as MODE_HURT_FIRE does. Eggman's runs at STROKE_EGGMAN. it
 * dodges at STROKE_DODGE, swayed by up to STROKE_SWAY. the sway's angle
 * grows by 1 << STROKE_SWAY_SHIFT a step, by the game's count of steps.
 * MODE_AIM points the stick at player 0 at STROKE_TURN. where player 1
 * doesn't see player 0, MODE_ATTACK pushes it at STROKE_TURN too. it then
 * points at most TURN_STEP off player 1's ang.y, toward player 0 */
#define STROKE_FULL 1.0F
#define STROKE_EGGMAN 0.7F
#define STROKE_TURN 0.2F
#define STROKE_DODGE 0.6F
#define STROKE_SWAY 0.3F
#define STROKE_SWAY_SHIFT 10
#define TURN_STEP 0x180
#define SEE_ANGLE 0x3000
#define CLOSE_DIST 70.0F
#define EGGMAN_REACH 100.0F
/* MODE_CIRCLE heads for a point CIRCLE_RADIUS from player 0. the point turns
 * about player 0 by 1 << CIRCLE_TURN_SHIFT a step. it turns by the game's
 * count of steps, not the record's */
#define CIRCLE_RADIUS 90.0F
#define CIRCLE_TURN_SHIFT 8

/* in MODE_ATTACK and MODE_AIM, Tails's walker may fire on every 32nd step
 * of the record's count. Eggman's may fire on every 64th. each fires there at
 * its walker's chance. MODE_HURT_FIRE fires on every 8th step */
#define TAILS_FIRE_MASK 0x1f
#define EGGMAN_FIRE_MASK 0x3f
#define HURT_FIRE_MASK 7
#define TAILS_FIRE_CHANCE 0.8F
#define EGGMAN_FIRE_CHANCE 0.5F
#define EGGMAN_AIM_FIRE_CHANCE 0.4F
/* at MODE_ATTACK's end, a draw of RANDOM_UNIT() below CIRCLE_CHANCE changes
 * the mode to MODE_CIRCLE. failing that, Eggman's walker changes to
 * MODE_OPENING on a draw below EGGMAN_OPENING_CHANCE. Tails's changes to it
 * on a draw above TAILS_OPENING_DRAW, one in ten */
#define CIRCLE_CHANCE 0.2F
#define TAILS_OPENING_DRAW 0.9F
#define EGGMAN_OPENING_CHANCE 0.5F

/* below GAUGE_LOW, Tails's walker changes to MODE_HURT_FIRE from MODE_ATTACK
 * while hit. MODE_CIRCLE and MODE_DODGE change to it hit or not. unhit, it
 * then changes between MODE_CIRCLE and MODE_HURT_FIRE every step until player
 * 0 is close. it steers no circle meanwhile. a start over sets player 1's
 * gauge to GAUGE_FULL, a mech's full gauge without
 * UPGRADE_EWALKER_EXTRASHIELD */
#define GAUGE_LOW 5.0F
#define GAUGE_FULL 10.0F
#define GAUGE_SHOWN 0.5F

#define PRESS_ACTION()                                                         \
    do {                                                                       \
        pad_action_pressed[RIVAL_SLOT] = 1;                                    \
        pad_action_held[RIVAL_SLOT] = 1;                                       \
    } while (0)

#include "stg20/rival_walker/text.inc"

#include "shared/walker_title_textures.h"

/* the arena's center, from which arena_ends offsets the arena's box */
/* @hack type: the center as an array of one, where the point alone would
 * do */
static NJS_POINT3 arena_center[1] = {
#include "stg20/rival_walker/arena_center.inc"
};
/* the corners of the arena's box: the one high on every axis, and the one
 * low */
enum arena_corner { ARENA_HIGH, ARENA_LOW };
/* the arena's box, as its corners' offsets from arena_center */
static NJS_POINT3 arena_ends[2] = {
#include "stg20/rival_walker/arena_ends.inc"
};
static Float unused_distance = 110.0F;

#include "shared/walker_title_letters.h"

/* where player 1 lies outside the arena's box, move it to the box's nearest
 * point. rival is player 1's work. returns TRUE where it moved player 1 */
static Bool keep_rival_in_arena(struct rival_walker *control,
                                struct task_work *rival)
{
    NJS_POINT3 position;
    Bool clipped = FALSE;
#ifdef NONMATCHING
    /* a build without the disc's data has zeros in arena_ends, which would
     * pin player 1 to arena_center */
    if (arena_ends[ARENA_HIGH].x <= arena_ends[ARENA_LOW].x) {
        return FALSE;
    }
#endif
    position = rival->pos;
#define CLIP(axis)                                                             \
    if (position.axis > arena_center->axis + arena_ends[ARENA_HIGH].axis) {    \
        position.axis = arena_center->axis + arena_ends[ARENA_HIGH].axis;      \
        clipped = TRUE;                                                        \
    }                                                                          \
    if (position.axis < arena_center->axis + arena_ends[ARENA_LOW].axis) {     \
        position.axis = arena_center->axis + arena_ends[ARENA_LOW].axis;       \
        clipped = TRUE;                                                        \
    }
    CLIP(x)
    CLIP(y)
    CLIP(z)
#undef CLIP
    /* @hack spelling: TRUE returned from the test's arm and FALSE after it,
     * where clipped returned would do */
    if (clipped) {
        player_set_pos(RIVAL_SLOT, position.x, position.y, position.z);
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 0 aims at player 1. that is where another mech's lock
 * holds player 1, or player 0 holds its action button. it reads none of its
 * arguments */
static Bool rival_targeted(struct rival_walker *control,
                           struct task_work *player, struct task_work *rival)
{
    struct player *record = player_tasks[RIVAL_SLOT]->extra.p;
    /* @hack spelling: the else after the return, where the test alone would
     * do */
    if (record && record->by_character.mech.locked_by) {
        return TRUE;
    } else if (pad_action_held[PLAYER_SLOT]) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 1's ang.y lies within SEE_ANGLE of its heading toward
 * player 0. where player 0 drives Eggman's walker, it is TRUE within
 * EGGMAN_REACH of player 0 too */
static Bool rival_sees_player(struct rival_walker *control,
                              struct task_work *player, struct task_work *rival)
{
    Float distance = njDistanceP2P(&player->pos, &rival->pos);
    /* @hack spelling: TRUE returned from each test's arm, where the two
     * tests joined by || and returned would do */
    if (player_records[PLAYER_SLOT]->character == PLAYER_EGGMAN_MECH &&
        EGGMAN_REACH > distance) {
        return TRUE;
    }
    if (angle_distance(RIVAL_HEADING_TOWARD(rival, player), rival->ang.y) <
        SEE_ANGLE) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 0 lies within CLOSE_DIST of player 1. where player 0
 * drives Eggman's walker, it is TRUE within EGGMAN_REACH too */
static Bool rival_close(struct rival_walker *control, struct task_work *player,
                        struct task_work *rival)
{
    Float distance = njDistanceP2P(&player->pos, &rival->pos);
    /* @hack spelling: TRUE returned from the test's arm, where the test's
     * value returned would do */
    if ((player_records[PLAYER_SLOT]->character == PLAYER_EGGMAN_MECH &&
         EGGMAN_REACH > distance) ||
        distance <= CLOSE_DIST) {
        return TRUE;
    }
    return FALSE;
}

/* start MODE_DODGE, with the record's steps zeroed and a length drawn for
 * record's walker. record is player 1's */
static void start_dodge(struct rival_walker *control, struct player *record)
{
    control->steps = 0;
    control->mode = MODE_DODGE;
    if (record->character == PLAYER_EGGMAN_MECH) {
        control->length = RANDOM_FROM(EGGMAN_DODGE_LEAST, EGGMAN_DODGE_SPREAD);
    } else {
        control->length = RANDOM_FROM(TAILS_DODGE_LEAST, TAILS_DODGE_SPREAD);
    }
}

/* as start_dodge does, for MODE_CIRCLE */
static void start_circle(struct rival_walker *control, struct player *record)
{
    control->steps = 0;
    control->mode = MODE_CIRCLE;
    if (record->character == PLAYER_EGGMAN_MECH) {
        control->length =
            RANDOM_FROM(EGGMAN_CIRCLE_LEAST, EGGMAN_CIRCLE_SPREAD);
    } else {
        control->length = RANDOM_FROM(TAILS_CIRCLE_LEAST, TAILS_CIRCLE_SPREAD);
    }
}

/* as start_dodge does, for MODE_ATTACK */
static void start_attack(struct rival_walker *control, struct player *record)
{
    control->steps = 0;
    control->mode = MODE_ATTACK;
    if (record->character == PLAYER_EGGMAN_MECH) {
        control->length =
            RANDOM_FROM(EGGMAN_ATTACK_LEAST, EGGMAN_ATTACK_SPREAD);
    } else {
        control->length = RANDOM_FROM(TAILS_ATTACK_LEAST, TAILS_ATTACK_SPREAD);
    }
}

/* start MODE_AIM, with the record's steps zeroed. record is player 1's.
 * where voice_wait has run out, player 1 taunts, and voice_wait then holds
 * TAUNT_WAIT. the taunt first stops each stream that voice_can_start lets start
 * a voice of TAUNT_PRIORITY */
static void start_aim(struct rival_walker *control, struct player *record)
{
    int stream;
    control->steps = 0;
    control->mode = MODE_AIM;
    if (control->voice_wait <= 0) {
        for (stream = 0; stream < VOICES; ++stream) {
            /* @hack spelling: voice_can_start's result compared with TRUE,
             * where the result alone would do */
            if (voice_can_start(TAUNT_PRIORITY, stream) == TRUE) {
                voice_stop(stream);
            }
        }
        switch (record->character) {
        case PLAYER_EGGMAN_MECH:
            if (record->actions[0] == PLAYER_MODE_EGGMAN_MECH_STRIKE) {
                voice_play(EGGMAN_STRIKE_TAUNT, TAUNT_PRIORITY);
            } else {
                voice_play(EGGMAN_TAUNT, TAUNT_PRIORITY);
            }
            break;
        case PLAYER_TAILS_MECH:
            if (record->actions[0] == PLAYER_MODE_TAILS_MECH_STRIKE) {
                voice_play(TAILS_STRIKE_TAUNT, TAUNT_PRIORITY);
            } else {
                voice_play(TAILS_TAUNT, TAUNT_PRIORITY);
            }
            break;
        }
        control->voice_wait = TAUNT_WAIT;
    }
}

static void first_rival_step(struct task *task)
{
    struct rival_walker *control;
    struct task_work *player;
    struct task_work *rival;
    struct player *player_record;
    struct player *rival_record;
    struct pad_stick *stick;
    Angle angle;
    int message;
    int voice;

    control = task->extra.p;
    if (game_state.mini_event_running == 1 ||
        (game_state.time_stopped & TIME_STOPPED_BIT(RIVAL_SLOT)) ||
        game_state.cleared == 1) {
        return;
    }
    player = player_work[PLAYER_SLOT];
    rival = player_work[RIVAL_SLOT];
    player_record = player_records[PLAYER_SLOT];
    rival_record = player_records[RIVAL_SLOT];
    /* @hack spelling: player 0's record read through
     * player_records[PLAYER_SLOT], where player_record would do */
    if ((player_records[PLAYER_SLOT]->flags & PLAYER_MISSED) ||
        (rival_record->flags & PLAYER_MISSED)) {
        control->mode = MODE_DOWN;
    }
    switch (control->mode) {
    case MODE_WAIT:
        if (stage_get_mode() == STAGE_PLAY) {
            struct camera_bossinit camera;
            pad_disable_player(PAD_BOTH_PLAYERS);
            pause_disable();
            /* @bug logic: the change to MODE_INTRO keeps the steps MODE_WAIT
             * counted, and sets only the mode and the length, at 0x8c500a64
             * and 0x8c500a66. the switch's other changes of mode zero the
             * steps, as at 0x8c500bd2. so the intro ends early by the wait's
             * steps. a wait past INTRO_VOICE_STEP skips the intro's line */
            control->mode = MODE_INTRO;
            control->length = INTRO_STEPS;
            if (rival_record->kind == PLAYER_TAILS_MECH) {
                boss_title_create(NULL, &tails_title);
            } else if (rival_record->kind == PLAYER_EGGMAN_MECH) {
                boss_title_create(NULL, &eggman_title);
            }
            camera.speed = INTRO_SPEED;
            camera.ang = rival->ang.y - INTRO_CAMERA_ANG;
            control->focus = rival->pos;
            control->focus.y = arena_center->y + INTRO_LIFT;
            camera.target = &control->focus;
            camera.start_dist = INTRO_START_DIST;
            camera.dist = INTRO_DIST;
            camera.turn = INTRO_TURN;
            camera_bossinit_start(0, &camera);
        }
        break;
    case MODE_INTRO:
        if (control->steps > control->length) {
            /* @hack spelling: intro_done compared with FALSE, where its
             * negation would do */
            if (control->intro_done == FALSE) {
                NJS_POINT3 position;
                message = NO_MESSAGE;
                position = *arena_center;
                position.y += MESSENGER_LIFT;
                control->intro_done = TRUE;
                switch (stage_get_retries()) {
                case 0:
                    break;
                case 1:
                    message = ONE_RETRY_MESSAGE;
                    break;
                case 2:
                    message = TWO_RETRIES_MESSAGE;
                    break;
                /* @hack redundant: case 3, which default covers */
                case 3:
                default:
                    message = MORE_RETRIES_MESSAGE;
                    break;
                }
                if (message != NO_MESSAGE) {
                    if (player_record->character == PLAYER_EGGMAN_MECH) {
                        message += RETRY_MESSAGES;
                    }
                    messenger_create(&position, 0, PLAYER_SLOT, message,
                                     MESSENGER_SUMMON, NULL);
                }
            }
            pad_enable_player(PLAYER_SLOT);
            pause_enable();
            control->steps = 0;
            control->mode = MODE_OPENING;
            control->length = RANDOM_FROM(OPENING_LEAST, OPENING_SPREAD);
            camera_bossinit_end(0);
        } else if (control->steps == INTRO_VOICE_STEP) {
            voice = NO_VOICE;
            if (rival_record->kind == PLAYER_TAILS_MECH) {
                voice = TAILS_INTRO_VOICE;
            } else if (rival_record->kind == PLAYER_EGGMAN_MECH) {
                voice = EGGMAN_INTRO_VOICE;
            }
            if (voice != NO_VOICE) {
                voice_play_priority_2(voice);
            }
            control->voice_wait = INTRO_TAUNT_WAIT;
        }
        break;
    case MODE_ATTACK:
    case MODE_ATTACK_AGAIN:
        if (rival_sees_player(control, player, rival)) {
            angle = RIVAL_HEADING_TOWARD(rival, player);
            pad_sticks[RIVAL_SLOT].ang = angle;
            if (rival_record->character == PLAYER_TAILS_MECH) {
                pad_sticks[RIVAL_SLOT].stroke = STROKE_FULL;
            } else {
                pad_sticks[RIVAL_SLOT].stroke = STROKE_EGGMAN;
            }
        } else {
            angle = angle_approach(
                rival->ang.y, RIVAL_HEADING_TOWARD(rival, player), TURN_STEP);
            /* @hack spelling: stick, the stick's address in a local, where
             * pad_sticks[RIVAL_SLOT] would do */
            stick = &pad_sticks[RIVAL_SLOT];
            stick->ang = angle;
            stick->stroke = STROKE_TURN;
        }
        if (rival_record->character == PLAYER_TAILS_MECH && rival->timer > 0 &&
            rival_record->gauge < GAUGE_LOW) {
            control->steps = 0;
            control->mode = MODE_HURT_FIRE;
            break;
        }
        if (rival_close(control, player, rival)) {
            start_aim(control, rival_record);
            break;
        }
        if ((rival_record->character == PLAYER_TAILS_MECH &&
             !(control->steps & TAILS_FIRE_MASK) &&
             RANDOM_UNIT() < TAILS_FIRE_CHANCE) ||
            (rival_record->character == PLAYER_EGGMAN_MECH &&
             !(control->steps & EGGMAN_FIRE_MASK) &&
             RANDOM_UNIT() < EGGMAN_FIRE_CHANCE)) {
            PRESS_ACTION();
            break;
        }
        if (control->steps > control->length) {
            if (rival_targeted(control, player, rival)) {
                start_dodge(control, rival_record);
                break;
            }
            if (RANDOM_UNIT() < CIRCLE_CHANCE) {
                start_circle(control, rival_record);
                break;
            }
            if ((rival_record->character == PLAYER_TAILS_MECH &&
                 RANDOM_UNIT() > TAILS_OPENING_DRAW) ||
                (rival_record->character == PLAYER_EGGMAN_MECH &&
                 RANDOM_UNIT() < EGGMAN_OPENING_CHANCE)) {
                control->steps = 0;
                control->mode = MODE_OPENING;
                break;
            }
            start_aim(control, rival_record);
        }
        break;
    case MODE_CIRCLE:
        if (rival_close(control, player, rival)) {
            start_aim(control, rival_record);
            break;
        }
        if (rival_record->character == PLAYER_TAILS_MECH &&
            rival_record->gauge < GAUGE_LOW) {
            control->steps = 0;
            control->mode = MODE_HURT_FIRE;
            break;
        }
        if (control->steps > control->length) {
            start_attack(control, rival_record);
            break;
        }
        {
            NJS_POINT3 target;
            target.x =
                (njSin(game_state.steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS) +
                player->pos.x;
            target.y = player->pos.y;
            target.z =
                (njCos(game_state.steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS) +
                player->pos.z;
            angle =
                RIVAL_HEADING(target.x - rival->pos.x, target.z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = angle;
        }
        if (rival_record->character == PLAYER_TAILS_MECH) {
            pad_sticks[RIVAL_SLOT].stroke = STROKE_FULL;
        } else {
            pad_sticks[RIVAL_SLOT].stroke = STROKE_EGGMAN;
        }
        break;
    case MODE_AIM:
        if (player_record->last_flags & PLAYER_WORK_GROUNDED) {
            angle = RIVAL_HEADING_TOWARD(rival, player);
            pad_sticks[RIVAL_SLOT].ang = angle;
            pad_sticks[RIVAL_SLOT].stroke = STROKE_TURN;
        }
        if (!rival_close(control, player, rival) &&
            rival_targeted(control, player, rival)) {
            start_dodge(control, rival_record);
            break;
        }
        if ((rival_record->character == PLAYER_TAILS_MECH &&
             !(control->steps & TAILS_FIRE_MASK) &&
             RANDOM_UNIT() < TAILS_FIRE_CHANCE) ||
            (rival_record->character == PLAYER_EGGMAN_MECH &&
             !(control->steps & EGGMAN_FIRE_MASK) &&
             RANDOM_UNIT() < EGGMAN_AIM_FIRE_CHANCE)) {
            PRESS_ACTION();
            control->steps = 0;
            break;
        }
        if (control->steps > AIM_STEPS) {
            start_circle(control, rival_record);
        }
        break;
    case MODE_DODGE:
        if (control->steps > control->length) {
            start_attack(control, rival_record);
            break;
        }
        if (rival_record->character == PLAYER_TAILS_MECH &&
            rival_record->gauge < GAUGE_LOW) {
            control->steps = 0;
            control->mode = MODE_HURT_FIRE;
            break;
        }
        if (rival_record->character == PLAYER_TAILS_MECH &&
            rival_close(control, player, rival)) {
            start_attack(control, rival_record);
            break;
        }
        {
            /* @hack redundant: unused, a point no code reads */
            NJS_POINT3 unused;
            NJS_VECTOR across;
            across = player->pos;
            njSubVector(&across, &rival->pos);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, QUARTER_TURN);
            njCalcVector(NULL, &across, &across);
            njPopMatrixEx();
            njUnitVector(&across);
            angle = RIVAL_HEADING(across.x, across.z);
            pad_sticks[RIVAL_SLOT].ang = angle;
        }
        if (rival_record->character == PLAYER_TAILS_MECH) {
            pad_sticks[RIVAL_SLOT].stroke = STROKE_FULL;
        } else {
            pad_sticks[RIVAL_SLOT].stroke =
                (njSin(game_state.steps << STROKE_SWAY_SHIFT) * STROKE_SWAY) +
                STROKE_DODGE;
        }
        break;
    case MODE_OPENING:
        if (rival_record->character == PLAYER_TAILS_MECH &&
            rival_targeted(control, player, rival)) {
            start_dodge(control, rival_record);
            break;
        }
        if (control->steps > OPENING_STEPS) {
            start_attack(control, rival_record);
        }
        break;
    case MODE_HURT_FIRE:
        angle = RIVAL_HEADING_TOWARD(rival, player);
        pad_sticks[RIVAL_SLOT].ang = angle;
        pad_sticks[RIVAL_SLOT].stroke = STROKE_FULL;
        if (rival->timer <= 0) {
            start_circle(control, rival_record);
            break;
        }
        if (!(control->steps & HURT_FIRE_MASK)) {
            PRESS_ACTION();
            control->steps = 0;
        }
        break;
    }
    keep_rival_in_arena(control, rival);
    if ((player_record->flags & PLAYER_MISSED) ||
        stage_get_mode() == STAGE_MISS_FADE ||
        stage_get_mode() == STAGE_RESTART_FADE ||
        stage_get_mode() == STAGE_RETRY_FADE || game_state.steps == 0) {
        rival_record->gauge = GAUGE_FULL;
        if (task->child) {
            task_destroy(task->child);
        }
        control->mode = MODE_WAIT;
        control->steps = 0;
        control->intro_done = FALSE;
    } else {
        ++control->steps;
        if (control->voice_wait > 0) {
            --control->voice_wait;
        }
    }
    /* @hack spelling: player 1's record read through
     * player_records[RIVAL_SLOT], where rival_record would do */
    control->gauge = player_records[RIVAL_SLOT]->gauge * GAUGE_SHOWN;
    if (rival_record->gauge > 0.0F && !task->child) {
        boss_gauge_create(task, &control->gauge);
    }
}

static void release_tails_title(struct task *task)
{
    njReleaseTexture(&tails_title_texlist);
}

static void release_eggman_title(struct task *task)
{
    njReleaseTexture(&eggman_title_texlist);
}

void first_rival_walker_create(void)
{
    struct task *task;
    struct rival_walker *control;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(task = task_create(0, CONTROL_LEVEL, first_rival_step,
                             "ControlTWalker"))) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(control = syCalloc(1, sizeof(struct rival_walker)))) {
        task_kill(task);
        return;
    }
    task->extra.p = control;
    control->gauge = player_records[RIVAL_SLOT]->gauge * GAUGE_SHOWN;
    boss_gauge_create(task, &control->gauge);
    control->mode = MODE_WAIT;
    if (player_records[RIVAL_SLOT]->kind == PLAYER_TAILS_MECH) {
#ifdef NONMATCHING
        /* this side loads no title into an empty texture list, as a build
         * without the disc's data leaves it, and sets no destructor to release
         * one */
        if (tails_title_texlist.nbTexture == 0) {
            return;
        }
#endif
        texture_load_pvm(TAILS_TITLE_ARCHIVE, &tails_title_texlist);
        task->destructor = release_tails_title;
    } else if (player_records[RIVAL_SLOT]->kind == PLAYER_EGGMAN_MECH) {
#ifdef NONMATCHING
        /* this side loads no title into an empty texture list, as a build
         * without the disc's data leaves it, and sets no destructor to release
         * one */
        if (eggman_title_texlist.nbTexture == 0) {
            return;
        }
#endif
        texture_load_pvm(EGGMAN_TITLE_ARCHIVE, &eggman_title_texlist);
        task->destructor = release_eggman_title;
    }
}
