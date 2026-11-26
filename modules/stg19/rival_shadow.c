/* ControlShadow, the task that steers player 1 against player 0 in the first
 * fight of Sonic and Shadow. player 1 is the rival. the second fight's
 * ControlShadow is in stg42/versus_sonic_2.c, and shared/shadow_fight.h holds
 * what the two share */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/bossinit.h"
#include "camera/stack.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/messenger.h"
#include "shared/rival_heading.h"
#include "shared/shadow_fight.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg19/rival_shadow.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* ControlShadow's modes. MODE_CHASE, MODE_CHASE_AGAIN, MODE_CIRCLE and
 * MODE_LANDED watch player 0's attacks. once player 1 faces player 0 in one,
 * they press jump and take MODE_JUMP */
enum rival_shadow_mode {
    MODE_WAIT, /* wait for STAGE_PLAY, then start the intro */
    /* the intro, for SHADOW_FIGHT_INTRO_STEPS steps. its end starts
     * MODE_CIRCLE. after a retry, the end also calls messenger_create with
     * MESSENGER_SUMMON, and a line for the retries so far */
    MODE_INTRO,
    /* steer at player 0 while player 0 is in the air, as its record's
     * last_flags say. after CHASE_STEPS, press jump, and take MODE_JUMP at a
     * chance of JUMP_CHANCE or else MODE_CIRCLE */
    MODE_CHASE,
    MODE_JUMP, /* hold jump until player 1 tops out or lands */
    MODE_JUMP_TOP, /* press jump again at the top */
    MODE_CIRCLE, /* steer about player 0, for the record's wait steps */
    MODE_ATTACK, /* wait for player 1 to land */
    MODE_LANDED, /* wait CHASE_STEPS, then take MODE_CHASE */
    /* MODE_CHASE, after a jump that lands before its second press */
    MODE_CHASE_AGAIN,
    MODE_STOPPED /* a player has missed: no mode steers player 1 */
};

#define CONTROL_LEVEL 0
/* a work's flags while its player stands on the ground or on a shape */
#define ON_FOOT (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)
/* the stroke of a stick pushed all the way */
#define RUN_STROKE 1.0F
/* no line to say, for messenger_create or for the intro's voice */
#define NO_LINE (-1)

/* player 1's gauge as the fight starts, and as it starts over */
#define START_GAUGE 3.0F

/* player 1 says its line at the intro's step INTRO_VOICE_STEP */
#define INTRO_VOICE_STEP 120
#define SONIC_INTRO_VOICE 2236
#define SHADOW_INTRO_VOICE 2212
/* the intro's BOSSINIT camera, by camera_bossinit's fields. it looks at the
 * record's look. the look takes player 1's x and z, and a y INTRO_LOOK_HEIGHT
 * above the arena's center. the camera stands off the look at player 1's
 * heading less a sixteenth turn */
#define INTRO_SPEED 1.0F
#define INTRO_ANG (FULL_TURN - (EIGHTH_TURN / 2))
#define INTRO_START_DIST 140.0F
#define INTRO_DIST 10.0F
#define INTRO_TURN QUARTER_TURN
#define INTRO_LOOK_HEIGHT 7.0F
/* the index on camera 0's stack where the intro's end expects the BOSSINIT
 * mode. the end takes the top mode off only at that index */
#define INTRO_CAMERA_INDEX 13

/* a mode's length drawn at random: least steps and up to spread more */
#define RANDOM_STEPS(least, spread)                                            \
    ((rand() * ((spread) * RAND_SCALE)) + (least))
#define CIRCLE_LEAST 120.0F
#define CIRCLE_SPREAD 360.0F
/* the first MODE_CIRCLE, after the intro, draws its length by these */
#define FIRST_CIRCLE_LEAST 240.0F
#define FIRST_CIRCLE_SPREAD 120.0F
/* player 1 circles toward a point CIRCLE_RADIUS from player 0. the point
 * turns about player 0 by CIRCLE_TURN each step of the game */
#define CIRCLE_RADIUS 35.0F
#define CIRCLE_TURN 256
#define CHASE_STEPS 60
#define JUMP_CHANCE 0.2F
/* a jump tops out where player 1's record has a spd.y of 0. player 1 must
 * then lie more than JUMP_TOP_HEIGHT above the ground below player 0 */
#define JUMP_TOP_HEIGHT 30.0F
/* MODE_JUMP, MODE_JUMP_TOP and MODE_ATTACK count no landing in their first
 * TAKE_OFF_STEPS steps */
#define TAKE_OFF_STEPS 2
#define ATTACK_REACH 20.0F
/* once player 1's gauge is empty, each step turns player 0's look to player
 * 1 for LOOK_STEPS steps */
#define LOOK_STEPS 3
/* the pull back to the arena's edge aims EDGE_INSET inside arena_radius */
#define EDGE_INSET 1.0F

#include "stg19/rival_shadow/text.inc"
#include "shared/rival_title_textures.h"

/* @hack type: arena_center an array of one, where the point would do */
static NJS_POINT3 arena_center[1] = {{0.0F, -42.0F, 0.0F}};
static Float dat_8c531568 = 90.0F;
static Float arena_radius = 130.0F;
#include "shared/rival_titles.h"

/* hold player 1 within the arena, and no lower than its center. it returns
 * TRUE where player 1 lay arena_radius or more from arena_center */
static Bool rival_keep_in_arena(struct shadow_control *control,
                                struct task_work *rival)
{
    Float x;
    Float y;
    Float z;
    Float scale;
    Float edge_x;
    Float edge_z;

    if (njDistanceP2P(&rival->pos, arena_center) >= arena_radius) {
        x = rival->pos.x;
        y = rival->pos.y;
        z = rival->pos.z;
        if (y < arena_center->y) {
            y = arena_center->y;
        }
        /* @bug logic: the pull back to the edge scales by player 1's
         * distance from the origin, at 0x8c500594. the test at 0x8c500558
         * measures from arena_center, which lies 42 below the origin. so a
         * player 1 at the center's height and 130 from it lies about 137 from
         * the origin. it comes back to about 123 from the center, not 129 */
        scale = (arena_radius - EDGE_INSET) *
                njInvertSqrt((x * x) + (y * y) + (z * z));
        edge_x = arena_center->x + (scale * (x - arena_center->x));
        edge_z = arena_center->z + (scale * (z - arena_center->z));
        player_set_pos(1, edge_x, y, edge_z);
        return TRUE;
    } else if (rival->pos.y < arena_center->y) {
        x = rival->pos.x;
        y = arena_center->y;
        z = rival->pos.z;
        player_set_pos(1, x, y, z);
        rival->pos.y = y;
    }
    return FALSE;
}

/* whether player 1 faces player 0 while player 0 attacks. a homing attack
 * counts from any distance, and a jump, a spin or a roll within
 * ATTACK_REACH */
static Bool rival_sees_attack(struct shadow_control *control,
                              struct task_work *player, struct task_work *rival)
{
    Angle heading;

    if (player->mode == PLAYER_MODE_SONIC_HOMING) {
        if (angle_distance(SHADOW_FIGHT_HEADING(rival, player), rival->ang.y) <
            SHADOW_FIGHT_FACING) {
            return TRUE;
        }
    }
    if (player->mode == PLAYER_MODE_JUMP ||
        player->mode == PLAYER_MODE_SONIC_SPIN ||
        player->mode == PLAYER_MODE_ROLL) {
        /* @hack spelling: heading, a local for the heading, where the heading
         * in place would do */
        heading = SHADOW_FIGHT_HEADING(rival, player);
        if (angle_distance(heading, rival->ang.y) < SHADOW_FIGHT_FACING &&
            njDistanceP2P(&player->pos, &rival->pos) < ATTACK_REACH) {
            return TRUE;
        }
    }
    return FALSE;
}

/* whether player 1 faces player 0 while player 0 is in a move of its
 * combo */
static Bool rival_sees_combo(struct shadow_control *control,
                             struct task_work *player, struct task_work *rival)
{
    /* @hack spelling: TRUE returned from the inner test's arm, where the two
     * tests joined by && and returned would do */
    if (player->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
        player->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
        if (angle_distance(SHADOW_FIGHT_HEADING(rival, player), rival->ang.y) <
            SHADOW_FIGHT_FACING) {
            return TRUE;
        }
    }
    return FALSE;
}

static void rival_shadow_step(struct task *task)
{
    struct shadow_control *control;
    struct task_work *player;
    struct task_work *rival;
    struct player *player_record;
    struct player *rival_record;
    Angle heading;
    /* @hack spelling: stick, a local for player 1's stick, where
     * pad_sticks[1] would do */
    struct pad_stick *stick;
    int line;

    control = task->extra.p;
    if (game_state.mini_event_running == 1 ||
        (game_state.time_stopped & TIME_STOPPED_BIT(1)) ||
        game_state.cleared == 1) {
        return;
    }
    player = player_work[0];
    rival = player_work[1];
    player_record = player_records[0];
    rival_record = player_records[1];
    if ((player_record->flags & PLAYER_MISSED) ||
        (rival_record->flags & PLAYER_MISSED)) {
        control->mode = MODE_STOPPED;
    }
    switch (control->mode) {
    case MODE_WAIT:
        if (stage_get_mode() == STAGE_PLAY) {
            struct camera_bossinit intro;

            pad_disable_player(PAD_BOTH_PLAYERS);
            pause_disable();
            control->mode = MODE_INTRO;
            control->wait = SHADOW_FIGHT_INTRO_STEPS;
            if (rival_record->kind == PLAYER_SONIC) {
                boss_title_create(NULL, &title_sonic);
            } else if (rival_record->kind == PLAYER_SHADOW) {
                boss_title_create(NULL, &title_shadow);
            }
            intro.speed = INTRO_SPEED;
            intro.ang = rival->ang.y + INTRO_ANG;
            control->look = rival->pos;
            control->look.y = arena_center->y + INTRO_LOOK_HEIGHT;
            intro.target = &control->look;
            intro.start_dist = INTRO_START_DIST;
            intro.dist = INTRO_DIST;
            intro.turn = INTRO_TURN;
            camera_bossinit_start(0, &intro);
        }
        break;
    case MODE_INTRO:
        if (control->timer > control->wait) {
            if (control->intro_done == 0) {
                NJS_POINT3 pos;

                line = NO_LINE;
                pos = *arena_center;
                pos.y += SHADOW_FIGHT_MESSENGER_RISE;
                control->intro_done = 1;
                switch (stage_get_retries()) {
                case 0:
                    break;
                case 1:
                    line = 0;
                    break;
                case 2:
                    line = 1;
                    break;
                /* @hack redundant: case 3 beside the default it joins */
                case 3:
                default:
                    line = 2;
                    break;
                }
                if (line != NO_LINE) {
                    if (player_record->character == PLAYER_SHADOW) {
                        line += SHADOW_FIGHT_LINES;
                    }
                    messenger_create(&pos, 0, 0, line, MESSENGER_SUMMON, NULL);
                }
            }
            pad_enable_player(0);
            pause_enable();
            control->timer = 0;
            control->mode = MODE_CIRCLE;
            control->wait =
                RANDOM_STEPS(FIRST_CIRCLE_LEAST, FIRST_CIRCLE_SPREAD);
            if (camera_top(0) == INTRO_CAMERA_INDEX) {
                camera_bossinit_end(0);
            }
        } else if (control->timer == INTRO_VOICE_STEP) {
            line = NO_LINE;
            if (rival_record->kind == PLAYER_SONIC) {
                line = SONIC_INTRO_VOICE;
            } else if (rival_record->kind == PLAYER_SHADOW) {
                line = SHADOW_INTRO_VOICE;
            }
            if (line != NO_LINE) {
                voice_play_priority_2(line);
            }
        }
        break;
    case MODE_CHASE:
    case MODE_CHASE_AGAIN:
        if (!(player_record->last_flags & ON_FOOT)) {
            heading = SHADOW_FIGHT_HEADING(rival, player);
            stick = &pad_sticks[1];
            stick->ang = heading;
            stick->stroke = RUN_STROKE;
        }
        if (rival_sees_attack(control, player, rival) ||
            rival_sees_combo(control, player, rival)) {
            control->timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            control->mode = MODE_JUMP;
        } else if (control->timer > CHASE_STEPS) {
            if (rand() * RAND_SCALE < JUMP_CHANCE) {
                control->timer = 0;
                pad_jump_pressed[1] = 1;
                pad_jump_held[1] = 1;
                control->mode = MODE_JUMP;
            } else {
                control->timer = 0;
                pad_jump_pressed[1] = 1;
                pad_jump_held[1] = 1;
                control->mode = MODE_CIRCLE;
                control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
            }
        }
        break;
    case MODE_JUMP:
        if (control->timer < TAKE_OFF_STEPS || !(rival->flags & ON_FOOT)) {
            pad_jump_held[1] = 1;
            if (rival_record->spd.y == 0.0F &&
                rival->pos.y - player_record->ground.below_y >
                    JUMP_TOP_HEIGHT) {
                control->mode = MODE_JUMP_TOP;
                control->timer = 0;
            }
        } else {
            control->mode = MODE_CHASE_AGAIN;
        }
        break;
    case MODE_JUMP_TOP:
        if (control->timer < TAKE_OFF_STEPS || !(rival->flags & ON_FOOT)) {
            if (rival_record->spd.y == 0.0F &&
                rival->pos.y - player_record->ground.below_y >
                    JUMP_TOP_HEIGHT) {
                control->mode = MODE_ATTACK;
                control->timer = 0;
                pad_jump_pressed[1] = 1;
                pad_jump_held[1] = 1;
            }
        } else {
            control->mode = MODE_CHASE_AGAIN;
        }
        break;
    case MODE_CIRCLE:
        if (rival_sees_attack(control, player, rival) ||
            rival_sees_combo(control, player, rival)) {
            control->timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            control->mode = MODE_JUMP;
        } else if (control->timer > control->wait) {
            control->mode = MODE_CHASE;
            control->timer = 0;
        } else {
            NJS_POINT3 pos;

            pos.x = player->pos.x +
                    (CIRCLE_RADIUS * njSin(game_state.steps * CIRCLE_TURN));
            pos.y = player->pos.y;
            pos.z = player->pos.z +
                    (CIRCLE_RADIUS * njCos(game_state.steps * CIRCLE_TURN));
            heading = RIVAL_HEADING(pos.x - rival->pos.x, pos.z - rival->pos.z);
            stick = &pad_sticks[1];
            stick->ang = heading;
            stick->stroke = RUN_STROKE;
        }
        break;
    case MODE_ATTACK:
        if (control->timer < TAKE_OFF_STEPS) {
            break;
        }
        if (rival->flags & ON_FOOT) {
            control->mode = MODE_LANDED;
            control->timer = 0;
        }
        break;
    case MODE_LANDED:
        if (rival_sees_attack(control, player, rival) ||
            rival_sees_combo(control, player, rival)) {
            control->timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            control->mode = MODE_JUMP;
        } else if (control->timer > CHASE_STEPS) {
            control->mode = MODE_CHASE;
            control->timer = 0;
        }
        break;
    }
    rival_keep_in_arena(control, rival);
    if (game_state.steps == 0 ||
        (stage_get_mode() == STAGE_FADE_IN && control->mode != MODE_WAIT &&
         control->mode != MODE_INTRO)) {
        rival_record->gauge = START_GAUGE;
        if (task->child) {
            task_destroy(task->child);
        }
        control->intro_done = 0;
        control->timer = 0;
        control->mode = MODE_WAIT;
    } else {
        control->timer++;
    }
    if (rival_record->gauge > 0.0F && !task->child) {
        boss_gauge_create(task, &player_records[1]->gauge);
    }
    if (!(rival_record->gauge > 0.0F)) {
        player_look_at_task(0, player_tasks[1], NULL, LOOK_STEPS);
    }
}

static void rival_shadow_destroy_sonic(struct task *task)
{
    njReleaseTexture(&title_sonic_texlist);
}

static void rival_shadow_destroy_shadow(struct task *task)
{
    njReleaseTexture(&title_shadow_texlist);
}

void rival_shadow_create(void)
{
    struct task *task;
    struct shadow_control *control;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if (!(task = task_create(0, CONTROL_LEVEL, rival_shadow_step,
                             "ControlShadow"))) {
        return;
    }
    if (!(control = syCalloc(1, sizeof(struct shadow_control)))) {
        task_kill(task);
        return;
    }
    task->extra.p = control;
    player_records[1]->gauge = START_GAUGE;
    boss_gauge_create(task, &player_records[1]->gauge);
    control->mode = MODE_WAIT;
    if (player_records[1]->kind == PLAYER_SONIC) {
        texture_load_pvm(RIVAL_SONIC_ARCHIVE, &title_sonic_texlist);
        task->destructor = rival_shadow_destroy_sonic;
    } else if (player_records[1]->kind == PLAYER_SHADOW) {
        texture_load_pvm(RIVAL_SHADOW_ARCHIVE, &title_shadow_texlist);
        task->destructor = rival_shadow_destroy_shadow;
    }
}
