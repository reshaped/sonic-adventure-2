/* ControlTWalker, the task that steers player 1 against player 0 in the
 * walkers' second fight. player 1 is the rival, slot 1's walker. the first
 * fight's ControlTWalker is in stg20/rival_walker.c */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/bossinit.h"
#include "camera/bosspoint.h"
#include "camera/stack.h"
#include "player/player.h"
#include "player/walker.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/messenger.h"
#include "shared/rival_heading.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg29/rival_walker.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

enum rival_walker_mode {
    MODE_WAIT, /* wait for STAGE_PLAY, then start the intro */
    /* the title, and camera 0's BOSSINIT turn about player 1. its end takes
     * MODE_OPENING, and after a retry summons a messenger */
    MODE_INTRO,
    MODE_ATTACK, /* head for player 0, firing now and then */
    MODE_ATTACK_LOW, /* MODE_ATTACK while player 1's gauge is below GAUGE_LOW */
    MODE_UNUSED,
    MODE_CIRCLE, /* run round player 0 */
    MODE_AIM, /* face player 0, firing every 16 steps */
    MODE_DODGE, /* run at right angles to the line to player 0 */
    /* steer nothing, after the intro and after MODE_DOWN. player 0's aim takes
       MODE_DODGE, and the mode's steps past OPENING_STEPS take MODE_ATTACK */
    MODE_OPENING,
    MODE_HURT_FIRE, /* fire every 8 steps while player 1's hit lasts */
    /* MODE_DODGE's run while player 1 has the second ring level. press action
     * as the level's steps run out */
    MODE_SPECIAL_2,
    /* face player 0 while player 1 has the third ring level. press action as
     * the level's steps run out */
    MODE_LASER,
    MODE_RETREAT, /* run to the arena's edge across the center from player 0 */
    MODE_DOWN, /* player 1's gauge is empty: steer nothing until it fills */
    MODE_ATTACK_AGAIN /* runs as MODE_ATTACK does. nothing sets it */
};

/* ControlTWalker's record, the task's extra */
struct rival_walker {
    Sint8 mode; /* an enum rival_walker_mode */
    /* 1 once the intro has ended, and 0 until then. the intro's end summons a
     * messenger only while it is 0, which it always is there. MODE_INTRO
     * follows only MODE_WAIT, which the record starts in and a start over
     * takes. a start over also clears the flag */
    Sint8 intro_done;
    /* the steps the mode has run, counted at each step's end. the changes to
     * MODE_INTRO, to MODE_DOWN and from MODE_DOWN keep the last mode's
     * count */
    Sint16 steps;
    /* the steps a timed mode lasts. MODE_OPENING and MODE_HURT_FIRE take one
     * they never read */
    Sint16 length;
    Uint8 unk_06[2];
    /* player 1's gauge times GAUGE_SHOWN, which the boss gauge shows */
    Float gauge;
    /* the point camera 0 keeps. through the intro it lies INTRO_LIFT above
     * the arena's floor. its x and z are player 1's as the intro starts.
     * after the intro it lies RIVAL_LIFT above player 1 */
    NJS_POINT3 focus;
};

/* a timed mode's length: its least steps, plus at random up to its spread */
#define LENGTH_LEAST 120.0F
#define SHORT_LEAST 60.0F
#define LONG_LEAST 180.0F
#define LENGTH_SPREAD 180.0F
/* MODE_OPENING's length, which it never reads */
#define OPENING_LEAST 240.0F
#define OPENING_SPREAD 120.0F
/* MODE_DODGE's spread as player 0 aims in MODE_ATTACK or MODE_OPENING, and
 * MODE_CIRCLE's */
#define DODGE_SPREAD 360.0F
#define CIRCLE_SPREAD 300.0F
/* MODE_AIM's spreads from MODE_ATTACK and from MODE_CIRCLE */
#define AIM_SPREAD 120.0F
#define CIRCLE_AIM_SPREAD 240.0F
/* the spread of MODE_ATTACK_LOW's MODE_AIM, and of the MODE_DODGE it takes
 * in that choice as player 0 aims */
#define LOW_SPREAD 60.0F

#define CONTROL_LEVEL 0

/* the intro lasts until the record's steps pass INTRO_STEPS, counting those
 * MODE_WAIT ran. player 1 says its line at step INTRO_VOICE_STEP */
#define INTRO_STEPS 480
#define INTRO_VOICE_STEP 120
/* the intro's BOSSINIT camera looks at the record's focus. it closes in from
 * INTRO_START_DIST to INTRO_DIST, then turns by INTRO_TURN */
#define INTRO_SPEED 1.0F
#define INTRO_LIFT 14.0F
#define INTRO_START_DIST 160.0F
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
/* the lines each walker says at the intro and as it takes a ring level */
#define TAILS_INTRO_VOICE 0x8de
#define EGGMAN_INTRO_VOICE 0x81f
#define TAILS_LEVEL_VOICE 0x8e2
#define EGGMAN_LEVEL_VOICE 0x823
#define LEVEL_VOICE_PRIORITY 3
#define NO_VOICE (-1)
/* the steps a ring level lasts, as game_state.ring_level_timer counts them */
#define LEVEL_STEPS 60

/* the strokes the task gives player 1's stick: all the way, a turn's and
 * MODE_HURT_FIRE's */
#define STROKE_FULL 1.0F
#define STROKE_TURN 0.2F
#define STROKE_FIRE 0.4F
/* MODE_ATTACK and MODE_ATTACK_LOW point player 1's stick at most TURN_STEP
 * off its ang.y toward player 0. they do so where player 1 doesn't see player
 * 0. MODE_AIM does so by LASER_TURN_STEP while player 1 fires its laser */
#define TURN_STEP 0x180
#define LASER_TURN_STEP 0x800
/* player 1 fights in MODE_ATTACK_LOW while its gauge is below GAUGE_LOW */
#define GAUGE_LOW 6.0F
/* the gauges at which MODE_ATTACK's choice of mode changes. from GAUGE_HIGH up
 * it never gives player 1 the second ring level. at GAUGE_MIDDLE or below it
 * always does, unless a ring level runs. player 1's second special and its
 * laser hold it back too */
#define GAUGE_HIGH 9.0F
#define GAUGE_MIDDLE 8.0F
/* MODE_ATTACK's choices. it draws one at random of ATTACK_CHOICES, or of
 * HIGH_CHOICES from GAUGE_HIGH up. the draws past CHOICE_AIM give the
 * second ring level */
enum attack_choice { CHOICE_CIRCLE, CHOICE_AIM, CHOICE_SPECIAL_2 };
#define ATTACK_CHOICES 4
#define HIGH_CHOICES 2
/* MODE_ATTACK_LOW's choices */
enum low_choice { LOW_CHOICE_AIM, LOW_CHOICE_SPECIAL_2, LOW_CHOICE_RETREAT };
/* MODE_ATTACK_LOW takes MODE_RETREAT or the laser where player 1's gauge is
 * above GAUGE_SHAKY and player 0's below PLAYER_GAUGE_LOW */
#define GAUGE_SHAKY 3.0F
#define PLAYER_GAUGE_LOW 5.0F
/* MODE_CIRCLE heads for a point CIRCLE_RADIUS from player 0. the point turns
 * about player 0 by 1 << CIRCLE_TURN_SHIFT a step. it turns by the game's
 * count of steps, not the record's */
#define CIRCLE_RADIUS 70.0F
#define CIRCLE_TURN_SHIFT 8
#define FACE_ANGLE 0xc00
#define SEE_ANGLE QUARTER_TURN
#define CLOSE_DIST 60.0F
#define EGGMAN_REACH 110.0F
/* MODE_RETREAT heads across the arena's center from player 0, to a point
 * RETREAT_MARGIN past near_radius from the center */
#define RETREAT_MARGIN 10.0F
/* a mode presses player 1's action button where the record's steps under its
 * mask are 0. MODE_ATTACK and MODE_ATTACK_LOW press every 32nd step, MODE_AIM
 * every 16th and MODE_HURT_FIRE every 8th. MODE_ATTACK presses there only at
 * a chance of FIRE_CHANCE */
#define ATTACK_FIRE_MASK 0x1f
#define AIM_FIRE_MASK 0xf
#define HURT_FIRE_MASK 7
#define FIRE_CHANCE 0.8F
/* MODE_LASER pushes the stick all the way on every 4th step of the game's
 * count, and by STROKE_TURN on the others */
#define LASER_PUSH_MASK 3
/* MODE_ATTACK and MODE_ATTACK_LOW pick another mode once they have run these
 * steps, or once player 1 is hit. MODE_OPENING takes MODE_ATTACK after
 * OPENING_STEPS */
#define ATTACK_STEPS 120
#define ATTACK_LOW_STEPS 60
#define OPENING_STEPS 120
/* the lengths of MODE_HURT_FIRE, of MODE_RETREAT and of MODE_AIM after the
 * laser */
#define HURT_FIRE_STEPS 120
#define RETREAT_STEPS 240
#define AIM_STEPS 240
/* the timer of player 1's work counts a hit down from PLAYER_HIT_STEPS to 0.
 * above HIT_FRESH the hit is fresh, and below HIT_STALE near its end */
#define HIT_FRESH 65
#define HIT_STALE 60
#define GAUGE_SHOWN 0.5F

static void rival_walker_draw(struct task *task);
static void rival_walker_step(struct task *task);
static void release_tails_title(struct task *task);
static void release_eggman_title(struct task *task);

#include "stg29/rival_walker/text.inc"

#include "shared/walker_title_textures.h"

/* the arena's center, at the height of its floor */
/* @hack type: the center as an array of one, where the point alone would
 * do */
static NJS_POINT3 arena_center[1] = {{15.0F, -50.0F, -20.0F}};
static Float arena_radius = 130.0F;
static Float dat_8c54ba7c = 95.0F;
/* player 1 is near the arena's center within near_radius of it */
static Float near_radius = 120.0F;

#include "shared/walker_title_letters.h"

static void rival_walker_draw(struct task *task)
{
}

/* lift player 0, whose work is player, to the arena's floor where it lies
 * below. control isn't read. returns TRUE where it lifted the player */
static Bool lift_player_0(struct rival_walker *control,
                          struct task_work *player)
{
    NJS_POINT3 pos;

    pos = player->pos;
    if (pos.y < arena_center->y) {
        pos.y = arena_center->y;
        player_set_pos(0, pos.x, pos.y, pos.z);
        return TRUE;
    }
    return FALSE;
}

/* hold player 1, whose work is rival, within arena_radius of the arena's
 * center. a player below the floor is measured as at the floor. one at the
 * radius or past it moves in along x and z to the edge. its y there is no
 * lower than the floor. pad_sticks[1]'s heading then turns a quarter turn.
 * control isn't read. returns TRUE where it moved the player */
static Bool keep_rival_in_arena(struct rival_walker *control,
                                struct task_work *rival)
{
    NJS_POINT3 pos;
    Float dx;
    Float dz;
    Float scale;
    Float x;
    Float z;

    pos = rival->pos;
    if (pos.y < arena_center->y) {
        pos.y = arena_center->y;
    }
    if (njDistanceP2P(&pos, arena_center) >= arena_radius) {
        dx = pos.x - arena_center->x;
        dz = pos.z - arena_center->z;
        scale = arena_radius * njInvertSqrt((dx * dx) + (dz * dz));
        /* @hack spelling: the edge's x and z held in locals before the call,
         * where the arguments would do */
        x = arena_center->x + (dx * scale);
        z = arena_center->z + (dz * scale);
        player_set_pos(1, x, pos.y, z);
        pad_sticks[1].ang += QUARTER_TURN;
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
    struct player *record = player_tasks[1]->extra.p;

    /* @hack spelling: the else after the return, where the test alone would
     * do */
    if (record && record->by_character.mech.locked_by) {
        return TRUE;
    } else if (pad_action_held[0]) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 1's ang.y lies within FACE_ANGLE of its heading toward
 * player 0 */
static Bool rival_faces_player(struct rival_walker *control,
                               struct task_work *player,
                               struct task_work *rival)
{
    /* @hack spelling: TRUE returned from the test's arm, where the test's
     * value returned would do */
    if (angle_distance(RIVAL_HEADING_TOWARD(rival, player), rival->ang.y) <
        FACE_ANGLE) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 1 lies within near_radius of the arena's center */
static Bool rival_near_center(struct rival_walker *control,
                              struct task_work *player, struct task_work *rival)
{
    /* @hack spelling: TRUE returned from the test's arm, where the test's
     * value returned would do */
    if (near_radius > njDistanceP2P(arena_center, &rival->pos)) {
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
    Float distance;

    distance = njDistanceP2P(&player->pos, &rival->pos);
    /* @hack spelling: TRUE returned from each test's arm, where the two
     * tests joined by || and returned would do */
    if (player_records[0]->character == PLAYER_EGGMAN_MECH &&
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
    Float distance;

    distance = njDistanceP2P(&player->pos, &rival->pos);
    /* @hack spelling: TRUE returned from the test's arm, where the test's
     * value returned would do */
    if ((player_records[0]->character == PLAYER_EGGMAN_MECH &&
         EGGMAN_REACH > distance) ||
        distance <= CLOSE_DIST) {
        return TRUE;
    }
    return FALSE;
}

/* give player 1 the third ring level alone for LEVEL_STEPS, and start
 * MODE_LASER. record is player 1's. every voice that gives way to
 * LEVEL_VOICE_PRIORITY stops, and record's walker says its level's line */
static void give_laser(struct rival_walker *control, struct player *record)
{
    int i;

    control->steps = 0;
    control->mode = MODE_LASER;
    game_state.ring_levels[1][RING_LEVEL_SPECIAL_1] = 0;
    game_state.ring_levels[1][RING_LEVEL_SPECIAL_2] = 0;
    game_state.ring_levels[1][RING_LEVEL_LASER] = 1;
    for (i = 0; i < VOICES; i++) {
        /* @hack spelling: voice_can_start's result compared with TRUE, where
         * the result alone would do */
        if (voice_can_start(LEVEL_VOICE_PRIORITY, i) == TRUE) {
            voice_stop(i);
        }
    }
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
        voice_play(EGGMAN_LEVEL_VOICE, LEVEL_VOICE_PRIORITY);
        break;
    case PLAYER_TAILS_MECH:
        voice_play(TAILS_LEVEL_VOICE, LEVEL_VOICE_PRIORITY);
        break;
    }
    game_state.ring_level_timer[1] = LEVEL_STEPS;
}

/* as give_laser does, with the second ring level and MODE_SPECIAL_2 */
static void give_special_2(struct rival_walker *control, struct player *record)
{
    int i;

    control->steps = 0;
    control->mode = MODE_SPECIAL_2;
    game_state.ring_levels[1][RING_LEVEL_SPECIAL_1] = 0;
    game_state.ring_levels[1][RING_LEVEL_SPECIAL_2] = 1;
    game_state.ring_levels[1][RING_LEVEL_LASER] = 0;
    for (i = 0; i < VOICES; i++) {
        /* @hack spelling: voice_can_start's result compared with TRUE, where
         * the result alone would do */
        if (voice_can_start(LEVEL_VOICE_PRIORITY, i) == TRUE) {
            voice_stop(i);
        }
    }
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
        voice_play(EGGMAN_LEVEL_VOICE, LEVEL_VOICE_PRIORITY);
        break;
    case PLAYER_TAILS_MECH:
        voice_play(TAILS_LEVEL_VOICE, LEVEL_VOICE_PRIORITY);
        break;
    }
    game_state.ring_level_timer[1] = LEVEL_STEPS;
}

static void rival_walker_step(struct task *task)
{
    struct rival_walker *control;
    struct task_work *player;
    struct task_work *rival;
    struct player *player_record;
    struct player *rival_record;
    /* @hack spelling: state, a pointer to game_state, where game_state
     * named at each use would do */
    struct game_state *state = &game_state;
    Angle ang;
    struct pad_stick *stick;
    int message;
    int voice;
    /* @hack spelling: low_gauge, step_span and mode_base, locals for
     * GAUGE_LOW, LENGTH_SPREAD * RAND_SCALE and LENGTH_LEAST, where the
     * constants in place would do */
    Float low_gauge;
    Float step_span;
    Float mode_base;

    control = task->extra.p;
    if (state->mini_event_running == 1 ||
        (state->time_stopped & TIME_STOPPED_BIT(1)) || state->cleared == 1) {
        return;
    }
    player = player_work[0];
    rival = player_work[1];
    player_record = player_records[0];
    rival_record = player_records[1];
#ifdef NONMATCHING
    /* this side repairs the tag in MODE_WAIT: while slot 1 holds no player,
     * the step returns before it reads the rival's work or record */
    if (rival == NULL) {
        return;
    }
#endif
    if (rival_record->gauge <= 0.0F) {
        control->mode = MODE_DOWN;
    } else if (control->mode == MODE_DOWN) {
        /* @bug logic: the change from MODE_DOWN to MODE_OPENING keeps the
         * steps MODE_DOWN counted, and stores only the mode, at 0x8c500aa8.
         * so after a down of more than OPENING_STEPS, the opening ends at its
         * first step */
        control->mode = MODE_OPENING;
    }
    low_gauge = GAUGE_LOW;
    step_span = LENGTH_SPREAD * RAND_SCALE;
    mode_base = LENGTH_LEAST;
    switch (control->mode) {
    case MODE_WAIT:
        if (stage_get_mode() == STAGE_PLAY) {
            struct camera_bossinit camera;

            pad_disable_player(PAD_BOTH_PLAYERS);
            pause_disable();
            /* @bug logic: the change to MODE_INTRO keeps the steps MODE_WAIT
             * counted, and sets only the length, at 0x8c500b2e. the switch's
             * other changes of mode zero the steps too, as at 0x8c500c80. so
             * the intro ends early by the wait's steps, and the pads come
             * back before the title's INTRO_STEPS are up. a wait past
             * INTRO_VOICE_STEP skips the intro's line */
            control->mode = MODE_INTRO;
            control->length = INTRO_STEPS;
            if (rival_record->kind == PLAYER_TAILS_MECH) {
                boss_title_create(NULL, &tails_title);
            } else if (rival_record->kind == PLAYER_EGGMAN_MECH) {
                boss_title_create(NULL, &eggman_title);
            }
            camera.speed = INTRO_SPEED;
            /* @bug memory: reads slot 1's work through rival untested, here
             * at 0x8c500b58, though the step's end tests rival for null at
             * 0x8c501796. the other modes read it untested too, and
             * keep_rival_in_arena does on every path, at 0x8c50069c. an empty
             * slot 1 also leaves rival_record null, which the step reads at
             * 0x8c500a6e */
            camera.ang = rival->ang.y - HALF_TURN;
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
            if (control->intro_done == 0) {
                NJS_POINT3 pos;

                message = NO_MESSAGE;
                pos = *arena_center;
                pos.y += MESSENGER_LIFT;
                control->intro_done = 1;
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
                    messenger_create(&pos, 0, 0, message, MESSENGER_SUMMON,
                                     NULL);
                }
            }
            pad_enable_player(0);
            pause_enable();
            control->steps = 0;
            control->mode = MODE_OPENING;
            control->length = RANDOM_FROM(OPENING_LEAST, OPENING_SPREAD);
            camera_bossinit_end(0);
            /* @hack spelling: camera's own block, where the test's block
             * would do */
            {
                struct camera_bosspoint camera;

                camera.target = &control->focus;
                camera.dist = RIVAL_CAMERA_DIST;
                camera.height = RIVAL_CAMERA_HEIGHT;
                camera.part = RIVAL_CAMERA_PART;
                camera.adjust = CAMERA_ADJUST_RELATIVE3;
                camera.collides = 1;
                camera.trigger_push = 0;
                camera.lift = RIVAL_CAMERA_LIFT;
                camera.level = 0;
                camera_bosspoint_start(0, &camera);
            }
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
        }
        break;
    case MODE_ATTACK:
    case MODE_ATTACK_AGAIN:
        if (rival_record->gauge < low_gauge) {
            control->steps = 0;
            control->mode = MODE_ATTACK_LOW;
        }
        if (rival_sees_player(control, player, rival)) {
            ang = RIVAL_HEADING_TOWARD(rival, player);
            /* @hack spelling: stick, the stick's address in a local, where
             * pad_sticks[1] would do */
            stick = &pad_sticks[1];
            stick->ang = ang;
            stick->stroke = STROKE_FULL;
        } else {
            ang = angle_approach(
                rival->ang.y, RIVAL_HEADING_TOWARD(rival, player), TURN_STEP);
            /* @hack redundant: a block no path reaches, which reads ang
             * twice */
            if (0) {
                if (ang + ang) {
                }
            }
            /* @hack spelling: stick, the stick's address in a local, where
             * pad_sticks[1] would do */
            stick = &pad_sticks[1];
            stick->ang = ang;
            stick->stroke = STROKE_TURN;
        }
        if (rival_close(control, player, rival)) {
            control->steps = 0;
            control->length = (rand() * step_span) + SHORT_LEAST;
            control->mode = MODE_AIM;
            break;
        }
        if (!(control->steps & ATTACK_FIRE_MASK) &&
            rand() * RAND_SCALE < FIRE_CHANCE) {
            pad_action_pressed[1] = 1;
            pad_action_held[1] = 1;
            break;
        }
        if (control->steps > ATTACK_STEPS || rival->timer > 0) {
            int choices;
            int choice;

            if (rival_targeted(control, player, rival) && rival->timer <= 0) {
                control->steps = 0;
                control->mode = MODE_DODGE;
                control->length = RANDOM_FROM(mode_base, DODGE_SPREAD);
                break;
            }
            if (rival_record->gauge >= GAUGE_HIGH) {
                choices = HIGH_CHOICES;
            } else {
                choices = ATTACK_CHOICES;
            }
            choice = (int)(rand() * (Float)choices * RAND_SCALE);
            if (rival->mode == PLAYER_MODE_MECH_SPECIAL_2) {
                choice = CHOICE_CIRCLE;
            } else if (state->ring_level_timer[1] > 0 ||
                       rival->mode == PLAYER_MODE_MECH_LASER) {
                choice = CHOICE_AIM;
            } else if (rival_record->gauge <= GAUGE_MIDDLE) {
                choice = CHOICE_SPECIAL_2;
            }
            switch (choice) {
            case CHOICE_CIRCLE:
                control->steps = 0;
                control->mode = MODE_CIRCLE;
                control->length = RANDOM_FROM(mode_base, CIRCLE_SPREAD);
                break;
            case CHOICE_AIM:
                if (rival_record->character == PLAYER_TAILS_MECH &&
                    rival->timer > 0) {
                    control->steps = 0;
                    control->mode = MODE_HURT_FIRE;
                    control->length = HURT_FIRE_STEPS;
                } else {
                    control->steps = 0;
                    control->length = RANDOM_FROM(mode_base, AIM_SPREAD);
                    control->mode = MODE_AIM;
                }
                break;
            default:
                give_special_2(control, rival_record);
                /* @hack redundant: the level's steps set again, as
                 * give_special_2 set them */
                state->ring_level_timer[1] = LEVEL_STEPS;
                break;
            }
        }
        break;
    case MODE_ATTACK_LOW:
        if (rival_record->gauge >= low_gauge) {
            control->steps = 0;
            control->mode = MODE_ATTACK;
        }
        if (rival_sees_player(control, player, rival)) {
            ang = RIVAL_HEADING_TOWARD(rival, player);
            /* @hack redundant: a block no path reaches, which reads ang */
            if (0) {
                if (ang) {
                }
            }
            pad_sticks[1].ang = ang;
            pad_sticks[1].stroke = STROKE_FULL;
        } else {
            ang = angle_approach(
                rival->ang.y, RIVAL_HEADING_TOWARD(rival, player), TURN_STEP);
            pad_sticks[1].ang = ang;
            pad_sticks[1].stroke = STROKE_TURN;
        }
        if (!(control->steps & ATTACK_FIRE_MASK)) {
            pad_action_pressed[1] = 1;
            pad_action_held[1] = 1;
            break;
        }
        if (control->steps > ATTACK_LOW_STEPS || rival->timer > 0) {
            int choice;

            if (rival_targeted(control, player, rival) && rival->timer <= 0) {
                control->steps = 0;
                control->mode = MODE_DODGE;
                control->length = (rand() * step_span) + SHORT_LEAST;
                break;
            }
            if (state->ring_level_timer[1] > 0 ||
                rival->mode == PLAYER_MODE_MECH_SPECIAL_2 ||
                rival->mode == PLAYER_MODE_MECH_LASER) {
                choice = LOW_CHOICE_AIM;
            } else if ((rival_record->gauge > GAUGE_SHAKY &&
                        player_record->gauge < PLAYER_GAUGE_LOW) ||
                       rival->timer > HIT_FRESH) {
                choice = LOW_CHOICE_RETREAT;
            } else {
                choice = LOW_CHOICE_SPECIAL_2;
            }
            switch (choice) {
            case LOW_CHOICE_AIM:
                control->steps = 0;
                if (rival_targeted(control, player, rival) &&
                    rival->timer <= 0) {
                    control->mode = MODE_DODGE;
                    control->length = RANDOM_FROM(LONG_LEAST, LOW_SPREAD);
                } else {
                    /* @hack redundant: the steps zeroed again, as the case's
                     * first statement zeroed them */
                    control->steps = 0;
                    control->length = RANDOM_FROM(SHORT_LEAST, LOW_SPREAD);
                    control->mode = MODE_AIM;
                }
                break;
            case LOW_CHOICE_SPECIAL_2:
                give_special_2(control, rival_record);
                /* @hack redundant: the level's steps set again, as
                 * give_special_2 set them */
                state->ring_level_timer[1] = LEVEL_STEPS;
                break;
            default:
                if (rival_near_center(control, player, rival)) {
                    give_special_2(control, rival_record);
                    control->length = RETREAT_STEPS;
                    control->mode = MODE_RETREAT;
                } else {
                    give_laser(control, rival_record);
                }
                /* @hack redundant: the level's steps set again, as each give
                 * call set them */
                state->ring_level_timer[1] = LEVEL_STEPS;
                break;
            }
        }
        break;
        /* @hack spelling: pos's scope shared by the modes from MODE_CIRCLE to
         * MODE_LASER, where MODE_CIRCLE's own would do */
        {
            NJS_POINT3 pos;

        case MODE_CIRCLE:
            if (rival_close(control, player, rival)) {
                control->steps = 0;
                control->length = RANDOM_FROM(mode_base, CIRCLE_AIM_SPREAD);
                control->mode = MODE_AIM;
            } else if (control->steps > control->length) {
                control->mode = MODE_ATTACK;
                control->steps = 0;
            } else {
                pos.x =
                    (njSin(state->steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS) +
                    player->pos.x;
                pos.y = player->pos.y;
                pos.z =
                    (njCos(state->steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS) +
                    player->pos.z;
                ang = RIVAL_HEADING(pos.x - rival->pos.x, pos.z - rival->pos.z);
                pad_sticks[1].ang = ang;
                pad_sticks[1].stroke = STROKE_FULL;
            }
            break;
        case MODE_AIM:
            ang = RIVAL_HEADING_TOWARD(rival, player);
            if (rival->mode == PLAYER_MODE_MECH_LASER) {
                pad_sticks[1].stroke = STROKE_FULL;
                ang = angle_approach(rival->ang.y, ang, LASER_TURN_STEP);
            } else {
                pad_sticks[1].stroke = STROKE_TURN;
            }
            pad_sticks[1].ang = ang;
            if (rival->timer <= 0 && !(rival->flags & PLAYER_WORK_CHARGED) &&
                rival->mode != PLAYER_MODE_MECH_LASER &&
                (rival_targeted(control, player, rival) ||
                 rival_faces_player(control, player, rival))) {
                control->steps = 0;
                control->mode = MODE_DODGE;
                control->length = (rand() * step_span) + SHORT_LEAST;
                break;
            }
            if (!(control->steps & AIM_FIRE_MASK)) {
                pad_action_pressed[1] = 1;
                pad_action_held[1] = 1;
                break;
            }
            if (control->steps > control->length) {
                control->steps = 0;
                control->mode = MODE_ATTACK;
            }
            break;
        case MODE_DODGE:
            if (control->steps > control->length || rival->timer > 0) {
                if (rival_record->gauge > low_gauge ||
                    rival->timer < HIT_STALE) {
                    control->mode = MODE_ATTACK;
                    control->steps = 0;
                } else {
                    give_laser(control, rival_record);
                    /* @hack redundant: the level's steps set again, as
                     * give_laser set them */
                    state->ring_level_timer[1] = LEVEL_STEPS;
                }
                break;
            }
            {
                NJS_VECTOR way;

                way = player->pos;
                njSubVector(&way, &rival->pos);
                way.y = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateY(NULL, QUARTER_TURN);
                njCalcVector(NULL, &way, &way);
                njPopMatrixEx();
                njUnitVector(&way);
                ang = RIVAL_HEADING(way.x, way.z);
                pad_sticks[1].ang = ang;
            }
            pad_sticks[1].stroke = STROKE_FULL;
            break;
        case MODE_OPENING:
            if (rival_targeted(control, player, rival)) {
                control->steps = 0;
                control->mode = MODE_DODGE;
                control->length = RANDOM_FROM(mode_base, DODGE_SPREAD);
                break;
            }
            if (control->steps > OPENING_STEPS) {
                control->mode = MODE_ATTACK;
                control->steps = 0;
            }
            break;
        case MODE_HURT_FIRE:
            ang = RIVAL_HEADING_TOWARD(rival, player);
            pad_sticks[1].ang = ang;
            pad_sticks[1].stroke = STROKE_FIRE;
            if (rival->timer <= 0) {
                control->steps = 0;
                control->mode = MODE_ATTACK_LOW;
                break;
            }
            if (!(control->steps & HURT_FIRE_MASK)) {
                pad_action_pressed[1] = 1;
                pad_action_held[1] = 1;
            }
            break;
        case MODE_SPECIAL_2: {
            NJS_VECTOR way;

            way = player->pos;
            njSubVector(&way, &rival->pos);
            way.y = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, QUARTER_TURN);
            njCalcVector(NULL, &way, &way);
            njPopMatrixEx();
            njUnitVector(&way);
            ang = RIVAL_HEADING(way.x, way.z);
            /* @hack spelling: stick, the stick's address in a local, where
             * pad_sticks[1] would do */
            stick = &pad_sticks[1];
            stick->ang = ang;
        }
            stick->stroke = STROKE_FULL;
            if (rival->timer > 0 &&
                game_state.ring_levels[1][RING_LEVEL_SPECIAL_2] == 0) {
                control->mode = MODE_ATTACK;
                control->steps = 0;
                break;
            }
            if (state->ring_level_timer[1] <= 0) {
                control->steps = 0;
                control->mode = MODE_DODGE;
                control->length = (rand() * step_span) + LONG_LEAST;
                pad_action_pressed[1] = 1;
                pad_action_held[1] = 1;
            }
            break;
        case MODE_LASER:
            ang = RIVAL_HEADING_TOWARD(rival, player);
            pad_sticks[1].ang = ang;
            if (state->steps & LASER_PUSH_MASK) {
                pad_sticks[1].stroke = STROKE_TURN;
            } else {
                pad_sticks[1].stroke = STROKE_FULL;
            }
            if (rival->timer > 0 &&
                game_state.ring_levels[1][RING_LEVEL_LASER] == 0) {
                control->mode = MODE_ATTACK;
                control->steps = 0;
                break;
            }
            if (state->ring_level_timer[1] <= 0) {
                control->steps = 0;
                control->length = AIM_STEPS;
                control->mode = MODE_AIM;
                pad_action_pressed[1] = 1;
                pad_action_held[1] = 1;
            }
            break;
        }
    case MODE_RETREAT:
        if (control->steps > control->length) {
            if (rival_near_center(control, player, rival)) {
                control->mode = MODE_ATTACK_LOW;
                control->steps = 0;
            } else {
                give_laser(control, rival_record);
                /* @hack redundant: the level's steps set again, as give_laser
                 * set them */
                state->ring_level_timer[1] = LEVEL_STEPS;
            }
            break;
        }
        {
            NJS_VECTOR way;
            Float *radius;

            way = *arena_center;
            njSubVector(&way, &player->pos);
            way.y = 0.0F;
            njUnitVector(&way);
            /* @hack spelling: radius, near_radius's address in a local, where
             * near_radius would do */
            radius = &near_radius;
            way.x *= *radius + RETREAT_MARGIN;
            way.z *= *radius + RETREAT_MARGIN;
            njAddVector(&way, arena_center);
            njSubVector(&way, &rival->pos);
            way.y = 0.0F;
            njUnitVector(&way);
            ang = RIVAL_HEADING(way.x, way.z);
            pad_sticks[1].ang = ang;
        }
        pad_sticks[1].stroke = STROKE_FULL;
        break;
    }
    lift_player_0(control, player);
    keep_rival_in_arena(control, rival);
    if (state->steps == 0 ||
        (stage_get_mode() == STAGE_FADE_IN && control->mode != MODE_WAIT)) {
        walker_fill_gauge(rival_record);
        if (task->child) {
            task_destroy(task->child);
        }
        control->mode = MODE_WAIT;
        control->intro_done = 0;
        control->steps = 0;
    } else {
        control->steps++;
    }
    control->gauge = rival_record->gauge * GAUGE_SHOWN;
    if (rival_record->gauge > 0.0F && !task->child) {
        boss_gauge_create(task, &control->gauge);
    }
    if (rival && control->mode != MODE_INTRO && control->mode != MODE_WAIT) {
        control->focus = rival->pos;
        control->focus.y += RIVAL_LIFT;
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

void rival_walker_create(void)
{
    struct task *task;
    struct rival_walker *control;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(task = task_create(0, CONTROL_LEVEL, rival_walker_step,
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
    control->gauge = player_records[1]->gauge * GAUGE_SHOWN;
    boss_gauge_create(task, &control->gauge);
    control->mode = MODE_WAIT;
    task->draw = rival_walker_draw;
    if (player_records[1]->kind == PLAYER_TAILS_MECH) {
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
    } else if (player_records[1]->kind == PLAYER_EGGMAN_MECH) {
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
