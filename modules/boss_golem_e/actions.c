/* the Egg Golem's actions in BOSS_GOLEM_E.PRS: its choice of what to do next,
 * and each of its moves. the golem's step runs them, and no object list names
 * them. boss_golem/actions.c is BOSS_GOLEM.PRS's twin, a source of its own.
 * that golem lets three drops fall from a flash at its head before its hands
 * come down. its death sets off an explosion. this golem does neither */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_golem/footholds.h"
#include "boss_golem_e/actions.h"
#include "boss_golem_e/footholds.h"
#include "boss_golem_e/golem.h"
#include "boss_golem_e/tactics.h"
#include "draw/motion.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/golem.h"
#include "shared/golem_camera.h"
#include "shared/golem_util.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

static void action_wait(struct task *task);
static void action_rest(struct task *task);
static void action_face_player(struct task *task);
static void action_aim_hand_a(struct task *task);
static void action_aim_hand_b(struct task *task);
static void action_strike_a(struct task *task);
static void action_strike_b(struct task *task);
static void action_hands_down_a(struct task *task);
static void action_hands_down_b(struct task *task);
static void action_slam(struct task *task);
static void action_die(struct task *task);
static void die_when_spent(struct task *task);

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* request action for golem_e_actions_step's next run. only this source calls
 * it, and SHC expands each call. it is external so that SHC still writes its
 * body, which the image holds */
void golem_e_request_action(struct task *task, int action);
/* NOLINTEND(misc-use-internal-linkage) */

/* golem_motions' index of each move's motion for each of its stages, by a
 * foothold state. FOOTHOLD_GONE takes FOOTHOLD_SUNK_TWICE's motion */
static Sint32 strike_a_motions[GOLEM_STRIKE_STAGES][FOOTHOLD_STATES] = {
    {2, 7, 12, 12},  {3, 8, 13, 13},  {4, 9, 14, 14},
    {5, 10, 15, 15}, {6, 11, 16, 16},
};
static Sint32 strike_b_motions[GOLEM_STRIKE_STAGES][FOOTHOLD_STATES] = {
    {17, 22, 27, 27}, {18, 23, 28, 28}, {19, 24, 29, 29},
    {20, 25, 30, 30}, {21, 26, 31, 31},
};
static Sint32 hands_down_a_motions[GOLEM_HANDS_STAGES][FOOTHOLD_STATES] = {
    {44, 47, 50, 50},
    {45, 48, 51, 51},
    {46, 49, 52, 52},
};
static Sint32 hands_down_b_motions[GOLEM_HANDS_STAGES][FOOTHOLD_STATES] = {
    {53, 56, 59, 59},
    {54, 57, 60, 60},
    {55, 58, 61, 61},
};
static Sint32 slam_motions[GOLEM_SLAM_STAGES][FOOTHOLD_STATES] = {
    {32, 36, 40, 40},
    {33, 37, 41, 41},
    {34, 38, 42, 42},
    {35, 39, 43, 43},
};
/* the bearing, either way, within which the golem aims or strikes. the bearing
 * is to player 0, or to the aim */
static Angle aim_bearing = 0x1000;
static Angle dat_8c6095e4 = 0x2800;
/* player 0 stands behind the golem at this bearing or more, either way */
static Angle behind_bearing = QUARTER_TURN;
/* the frame of its motion at which each blow lands. its sound plays at an
 * earlier frame. a slam's wide and near blows land in one motion, and the slam
 * itself in the next */
static Float strike_frame = 17.0F;
static Float strike_sound_frame = 12.0F;
static Float slam_wide_frame = 23.0F;
static Float slam_wide_sound_frame = 20.0F;
static Float slam_near_frame = 27.0F;
static Float slam_near_sound_frame = 24.0F;
static Float slam_frame = 19.0F;
static Float slam_sound_frame = 14.0F;
static Sint32 move_volume = 25;
static Sint32 strike_volume = 125;
static Sint32 slam_near_volume = 125;
static Sint32 hold_volume = 75;
static Sint32 slam_volume = 100;
static Sint32 wave_volume = 25;
static Sint32 die_volume = 75;
static Sint32 dat_8c609628 = 60;
/* each action's step, by enum golem_action */
static void (*actions[])(struct task *task) = {
    action_wait,       action_rest,         action_face_player,
    action_aim_hand_a, action_aim_hand_b,   action_strike_a,
    action_strike_b,   action_hands_down_a, action_hands_down_b,
    action_slam,       action_die,
};
/* the steps over which the death's motion links in */
static Sint32 die_link_steps = 20;
static Float dat_8c60965c = -50.0F;
static Float dat_8c609660 = 160.0F;
static Float dat_8c609664 = 100.0F;
static Float dat_8c609668 = 0.5F;
static Float blow_shake_size = 150.0F;
static Sint32 blow_shake_steps = 3;
/* the death plays die_frames of its motion. its sound plays every
 * die_sound_steps. each step sinks the golem by sink_size times the magnitude
 * of a cosine. the cosine's angle turns sink_turn a step */
static Float die_frames = 120.0F;
static Angle sink_turn = 0x400;
static Float sink_size = 0.25F;
static Uint32 die_sound_steps = 40;

/* the opening shot's steps before the fight starts, unless player 0 presses
 * a face button first */
#define WAIT_STEPS 360
/* the stage messages a retry's messenger shows, one for each of the first
 * three misses or retries. a later one shows the third */
#define MESSAGES 3
/* the steps a strike holds still between its blows, and after them */
#define STRIKE_PAUSE 30
#define STRIKE_REST 120
/* a slam's wide blows land SLAM_WIDE slots either side of its slot. its near
 * blows then land one slot either side */
#define SLAM_WIDE 2
/* the steps between plays of HOLD_SOUND while the hands hold */
#define HOLD_SOUND_STEPS 30
/* the golem's sound ids, and the keys they play for. a key is an address a few
 * bytes into the golem's task. sound_set_volume finds a sound by its key
 * alone. the shock wave and the death share one. the hands' blows play their
 * sounds for the task itself */
#define MOVE_SOUND 6
#define MOVE_SOUND_KEY(task) ((char *)(task) + 9)
#define STRIKE_SOUND 4
#define WAVE_SOUND 5
#define WAVE_SOUND_KEY(task) ((char *)(task) + 1)
#define HOLD_SOUND 7
#define HOLD_SOUND_KEY(task) ((char *)(task) + 2)
#define SLAM_NEAR_SOUND 8
#define SLAM_SOUND 9
#define SLAM_SOUND_KEY(task) ((char *)(task) + 4)
#define DIE_SOUND 1
#define DIE_SOUND_KEY(task) ((char *)(task) + 1)
/* the death's sound plays at die_volume times a scale. the scale falls from
 * DIE_LEAST + DIE_SPAN at the motion's first frame to DIE_LEAST at
 * die_frames */
#define DIE_LEAST 0.6F
#define DIE_SPAN 0.4F
/* a hand's distance from player 0 until a blow measures it */
#define FAR_AWAY 99999.9F

/* each action's modes, in golem_e->action_mode. an action steps through its
 * modes in order */
enum strike_mode {
    MODE_STRIKE_START,
    MODE_FIRST_BLOW,
    MODE_FIRST_RISE,
    MODE_FIRST_PAUSE,
    MODE_SECOND_AIM,
    MODE_SECOND_BLOW,
    MODE_SECOND_RISE,
    MODE_LAST_PAUSE,
    MODE_STRIKE_END
};
/* the action that requests a hands-down links the lowering motion itself.
 * MODE_HANDS_START then links no motion */
enum hands_down_mode {
    MODE_HANDS_START,
    MODE_HANDS_DOWN,
    MODE_HANDS_HOLD,
    MODE_HANDS_RISE
};
/* whatever the slam's mode, the golem rests once the slam's last motion ends */
enum slam_mode {
    MODE_SLAM_START,
    MODE_SLAM_WIDE,
    MODE_SLAM_NEAR,
    MODE_SLAM,
    MODE_SLAM_DONE
};
enum die_mode { MODE_RESTORE, MODE_SINK, MODE_CLEAR, MODE_DONE };

void golem_e_action_start(struct task *task, int action)
{
    golem_e->action = action;
    golem_e->next_action = GOLEM_NO_ACTION;
    golem_e->action_mode = 0;
    golem_e->action_steps = 0;
}

void golem_e_actions_step(struct task *task)
{
    if (golem_e->next_action != GOLEM_NO_ACTION) {
        golem_e->action = golem_e->next_action;
        golem_e->next_action = GOLEM_NO_ACTION;
        golem_e->action_mode = 0;
        golem_e->action_steps = 0;
    }
    actions[golem_e->action](task);
    golem_e->action_steps++;
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
void golem_e_request_action(struct task *task, int action)
{
    golem_e->next_action = action;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void action_wait(struct task *task)
{
    struct boss_record *record = task->extra.p;

    if (golem_e->camera_steps >= WAIT_STEPS ||
        (player_pads[0]->press & PDD_DGT_TA) ||
        (player_pads[0]->press & PDD_DGT_TB) ||
        (player_pads[0]->press & PDD_DGT_TX) ||
        (player_pads[0]->press & PDD_DGT_TY)) {
        golem_camera_request(task, GOLEM_CAMERA_FAR);
        golem_e_request_action(task, GOLEM_ACTION_REST);
        motion_set(record->block, GOLEM_MOTION_IDLE);
        boss_messenger_summon_capped(0, MESSAGES, task);
    }
}

/* choose the next action once the golem's motion ends. a strike starts with
 * hand B where golem_e_player_facing_up is TRUE, and with hand A otherwise */
static void action_rest(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing;

    if (motion_ended(motion)) {
        if (golem_e_may_attack(task)) {
            bearing = golem_turn_to_player();
            if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
                if (bearing > 0) {
                    golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
                    motion_link(
                        motion,
                        hands_down_b_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
                } else {
                    golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
                    motion_link(
                        motion,
                        hands_down_a_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
                }
            } else if (!golem_player_near(task)) {
                if (_builtin_fabsf((Float)bearing) >= (Float)aim_bearing ||
                    golem_e->power <= GOLEM_POWER_LOW) {
                    golem_e_request_action(task, GOLEM_ACTION_FACE_PLAYER);
                    golem_e_set_turn(task, GOLEM_TURN_TO_PLAYER);
                    golem_e->timer = 0;
                } else {
                    if (!golem_e_player_facing_up()) {
                        golem_e_request_action(task, GOLEM_ACTION_AIM_HAND_A);
                        golem_e_set_turn(task, GOLEM_TURN_TO_HAND_A);
                    } else {
                        golem_e_request_action(task, GOLEM_ACTION_AIM_HAND_B);
                        golem_e_set_turn(task, GOLEM_TURN_TO_HAND_B);
                    }
                    golem_e->timer = 0;
                }
            }
        } else if (golem_e->rest_steps > golem_e->rest_wait / 2 &&
                   !golem_player_near(task)) {
            golem_e_request_action(task, GOLEM_ACTION_FACE_PLAYER);
            golem_e_set_turn(task, GOLEM_TURN_TO_PLAYER);
            golem_e->timer = 0;
        }
    }
    die_when_spent(task);
}

static void action_face_player(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing = golem_turn_to_player();

    golem_e_turn(task);
    if (golem_e_may_attack(task)) {
        if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
            if (bearing > 0) {
                golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
                motion_link(motion,
                            hands_down_b_motions[GOLEM_HANDS_LOWER]
                                                [golem_e->hands_down_state]);
            } else {
                golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
                motion_link(motion,
                            hands_down_a_motions[GOLEM_HANDS_LOWER]
                                                [golem_e->hands_down_state]);
            }
        } else if (golem_e->power <= GOLEM_POWER_LOW) {
            if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
                golem_e_aim_slot(task, GOLEM_HAND_A);
                golem_e->slam_state =
                    golem_e_footholds_lowest_near(golem_e->slot);
                golem_e_request_action(task, GOLEM_ACTION_SLAM);
                golem_e_set_turn(task, GOLEM_TURN_TO_SLAM);
                motion_link(
                    motion,
                    slam_motions[GOLEM_SLAM_START][golem_e->slam_state]);
            }
        } else if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            if (!golem_e_player_facing_up()) {
                golem_e_request_action(task, GOLEM_ACTION_AIM_HAND_A);
                golem_e_set_turn(task, GOLEM_TURN_TO_HAND_A);
            } else {
                golem_e_request_action(task, GOLEM_ACTION_AIM_HAND_B);
                golem_e_set_turn(task, GOLEM_TURN_TO_HAND_B);
            }
            golem_e->timer = 0;
        }
    } else if (golem_player_near(task)) {
        golem_e_request_action(task, GOLEM_ACTION_REST);
        golem_e_set_turn(task, GOLEM_TURN_NONE);
    }
    die_when_spent(task);
}

static void action_aim_hand_a(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing = golem_turn_to_player();

    if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
        if (bearing > 0) {
            golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
            motion_link(motion,
                        hands_down_b_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
        } else {
            golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
            motion_link(motion,
                        hands_down_a_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
        }
    } else {
        golem_e_aim_slot(task, GOLEM_HAND_A);
        golem_e_turn(task);
        bearing = golem_angle_turn(golem_e->heading, golem_e->aim_heading);
        if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            golem_e_request_action(task, GOLEM_ACTION_STRIKE_A);
            golem_e->hand_a_state = golem_e_foothold_state(golem_e->slot);
            motion_link(
                motion,
                strike_a_motions[GOLEM_STRIKE_START][golem_e->hand_a_state]);
        }
    }
    die_when_spent(task);
}

static void action_aim_hand_b(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing = golem_turn_to_player();

    if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
        if (bearing > 0) {
            golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
            motion_link(motion,
                        hands_down_b_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
        } else {
            golem_e_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
            motion_link(motion,
                        hands_down_a_motions[GOLEM_HANDS_LOWER]
                                            [golem_e->hands_down_state]);
        }
    } else {
        golem_e_aim_slot(task, GOLEM_HAND_B);
        golem_e_turn(task);
        bearing = golem_angle_turn(golem_e->heading, golem_e->aim_heading);
        if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            golem_e_request_action(task, GOLEM_ACTION_STRIKE_B);
            golem_e->hand_b_state = golem_e_foothold_state(golem_e->slot);
            motion_link(
                motion,
                strike_b_motions[GOLEM_STRIKE_START][golem_e->hand_b_state]);
        }
    }
    die_when_spent(task);
}

/* a blow of either strike on a gone foothold makes no sound, shake, hit or
 * shock wave */
static void action_strike_a(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    switch (golem_e->action_mode) {
    case MODE_STRIKE_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem_e->action_mode++;
        break;
    case MODE_FIRST_BLOW:
        golem_e_turn(task);
        if (motion_index(motion) ==
                strike_a_motions[GOLEM_STRIKE_BLOW][golem_e->hand_a_state] &&
            motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                strike_a_motions[GOLEM_STRIKE_BLOW][golem_e->hand_a_state] &&
            motion_frame(motion) >= strike_frame) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_e_hand_a_pos, blow_shake_size,
                             blow_shake_steps);
                golem_e_foothold_hit(golem_e->slot);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_FIRST_RISE:
        if (motion_index(motion) ==
            strike_a_motions[GOLEM_STRIKE_BLOW][golem_e->hand_a_state]) {
            if (motion_ended(motion)) {
                golem_e->action_mode++;
                golem_e->timer = 0;
                motion_link(
                    motion,
                    strike_a_motions[GOLEM_STRIKE_RISE][golem_e->hand_a_state]);
                if (golem_e->hand_a_state != FOOTHOLD_GONE) {
                    golem_e_wave_create(task, golem_e->hand_a_state);
                    sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                    sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                     wave_volume);
                }
            }
        }
        break;
    case MODE_FIRST_PAUSE:
        golem_e->hold_motion = TRUE;
        if (++golem_e->timer > STRIKE_PAUSE) {
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
            golem_e_set_slot(task, (golem_e->slot + 1) % GOLEM_SLOTS,
                             GOLEM_HAND_B);
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_AIM:
        if (motion_ended(motion)) {
            golem_e->hand_b_state = golem_e_foothold_state(golem_e->slot);
            motion_link(motion, strike_a_motions[GOLEM_STRIKE_SECOND_BLOW]
                                                [golem_e->hand_b_state]);
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_BLOW:
        golem_e_turn(task);
        if (motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_frame(motion) >= strike_frame) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_e_hand_b_pos, blow_shake_size,
                             blow_shake_steps);
                golem_e_foothold_hit(golem_e->slot);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_RISE:
        if (motion_ended(motion)) {
            golem_e->action_mode++;
            golem_e->timer = 0;
            motion_link(motion, strike_a_motions[GOLEM_STRIKE_SECOND_RISE]
                                                [golem_e->hand_b_state]);
            if (golem_e->hand_b_state != FOOTHOLD_GONE) {
                golem_e_wave_create(task, golem_e->hand_b_state);
                sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                 wave_volume);
            }
        }
        break;
    case MODE_LAST_PAUSE:
        golem_e->hold_motion = TRUE;
        if (++golem_e->timer > STRIKE_REST) {
            golem_e->action_mode++;
        }
        /* fall through */
    case MODE_STRIKE_END:
        if (motion_ended(motion)) {
            golem_e->rest_steps = 0;
            golem_e_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
    die_when_spent(task);
}

static void action_strike_b(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    switch (golem_e->action_mode) {
    case MODE_STRIKE_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem_e->action_mode++;
        break;
    case MODE_FIRST_BLOW:
        golem_e_turn(task);
        if (motion_index(motion) ==
                strike_b_motions[GOLEM_STRIKE_BLOW][golem_e->hand_b_state] &&
            motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                strike_b_motions[GOLEM_STRIKE_BLOW][golem_e->hand_b_state] &&
            motion_frame(motion) >= strike_frame) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_e_hand_b_pos, blow_shake_size,
                             blow_shake_steps);
                golem_e_foothold_hit(golem_e->slot);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_FIRST_RISE:
        if (motion_index(motion) ==
            strike_b_motions[GOLEM_STRIKE_BLOW][golem_e->hand_b_state]) {
            if (motion_ended(motion)) {
                golem_e->action_mode++;
                golem_e->timer = 0;
                motion_link(
                    motion,
                    strike_b_motions[GOLEM_STRIKE_RISE][golem_e->hand_b_state]);
                if (golem_e->hand_b_state != FOOTHOLD_GONE) {
                    golem_e_wave_create(task, golem_e->hand_b_state);
                    sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                    sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                     wave_volume);
                }
            }
        }
        break;
    case MODE_FIRST_PAUSE:
        golem_e->hold_motion = TRUE;
        if (++golem_e->timer > STRIKE_PAUSE) {
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
            golem_e_set_slot(task,
                             (golem_e->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS,
                             GOLEM_HAND_A);
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_AIM:
        if (motion_ended(motion)) {
            golem_e->hand_a_state = golem_e_foothold_state(golem_e->slot);
            motion_link(motion, strike_b_motions[GOLEM_STRIKE_SECOND_BLOW]
                                                [golem_e->hand_a_state]);
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_BLOW:
        golem_e_turn(task);
        if (motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_frame(motion) >= strike_frame) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_e_hand_a_pos, blow_shake_size,
                             blow_shake_steps);
                golem_e_foothold_hit(golem_e->slot);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_SECOND_RISE:
        if (motion_ended(motion)) {
            golem_e->action_mode++;
            golem_e->timer = 0;
            motion_link(motion, strike_b_motions[GOLEM_STRIKE_SECOND_RISE]
                                                [golem_e->hand_a_state]);
            if (golem_e->hand_a_state != FOOTHOLD_GONE) {
                golem_e_wave_create(task, golem_e->hand_a_state);
                sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                 wave_volume);
            }
        }
        break;
    case MODE_LAST_PAUSE:
        golem_e->hold_motion = TRUE;
        if (++golem_e->timer > STRIKE_REST) {
            golem_e->action_mode++;
        }
        /* fall through */
    case MODE_STRIKE_END:
        if (motion_ended(motion)) {
            golem_e->rest_steps = 0;
            golem_e_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
    die_when_spent(task);
}

/* bring the hands down on player 0 behind the golem, hold them, and raise
 * them. the golem turns toward the player except in the hold. a spent golem
 * dies only before the hold */
static void action_hands_down_a(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    switch (golem_e->action_mode) {
    case MODE_HANDS_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem_e_set_turn(task, GOLEM_TURN_TO_PLAYER);
        golem_e->action_mode++;
        die_when_spent(task);
        break;
    case MODE_HANDS_DOWN:
        golem_e_turn(task);
        if (motion_index(motion) ==
                hands_down_a_motions[GOLEM_HANDS_LOWER]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            motion_link(motion,
                        hands_down_a_motions[GOLEM_HANDS_HOLD]
                                            [golem_e->hands_down_state]);
            golem_e->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_HOLD:
        if (golem_e_holding(task) && game_state.steps % HOLD_SOUND_STEPS == 0) {
            sound_play(HOLD_SOUND, HOLD_SOUND_KEY(task), 0, 0);
            sound_set_volume(HOLD_SOUND_KEY(task), SOUND_ANY_ID, hold_volume);
        }
        if (motion_index(motion) ==
                hands_down_a_motions[GOLEM_HANDS_HOLD]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            motion_link(motion,
                        hands_down_a_motions[GOLEM_HANDS_RISE]
                                            [golem_e->hands_down_state]);
            golem_e->action_mode++;
        }
        break;
    case MODE_HANDS_RISE:
        golem_e_turn(task);
        if (motion_index(motion) ==
                hands_down_a_motions[GOLEM_HANDS_RISE]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            /* @bug logic: the golem rests without setting rest_steps to 0,
             * at 0x8c507442, where action_hands_down_b sets it at 0x8c50760a.
             * so the golem may attack again at once */
            golem_e_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
}

/* action_hands_down_a with hands_down_b_motions. unlike that action, it sets
 * rest_steps to 0 as the golem rests */
static void action_hands_down_b(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    switch (golem_e->action_mode) {
    case MODE_HANDS_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem_e_set_turn(task, GOLEM_TURN_TO_PLAYER);
        golem_e->action_mode++;
        die_when_spent(task);
        break;
    case MODE_HANDS_DOWN:
        golem_e_turn(task);
        if (motion_index(motion) ==
                hands_down_b_motions[GOLEM_HANDS_LOWER]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            motion_link(motion,
                        hands_down_b_motions[GOLEM_HANDS_HOLD]
                                            [golem_e->hands_down_state]);
            golem_e->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_HOLD:
        if (golem_e_holding(task) && game_state.steps % HOLD_SOUND_STEPS == 0) {
            sound_play(HOLD_SOUND, HOLD_SOUND_KEY(task), 0, 0);
            sound_set_volume(HOLD_SOUND_KEY(task), SOUND_ANY_ID, hold_volume);
        }
        if (motion_index(motion) ==
                hands_down_b_motions[GOLEM_HANDS_HOLD]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            motion_link(motion,
                        hands_down_b_motions[GOLEM_HANDS_RISE]
                                            [golem_e->hands_down_state]);
            golem_e->action_mode++;
        }
        break;
    case MODE_HANDS_RISE:
        golem_e_turn(task);
        if (motion_index(motion) ==
                hands_down_b_motions[GOLEM_HANDS_RISE]
                                    [golem_e->hands_down_state] &&
            motion_ended(motion)) {
            golem_e->rest_steps = 0;
            golem_e_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
}

/* slam_state picks which footholds take the slam's blows. the hands strike a
 * foothold SLAM_WIDE slots away only where it is less sunk than slam_state.
 * they strike one beside the golem's slot only at slam_state. the slam
 * strikes the golem's own slot only at slam_state too. it breaks that foothold
 * where the slot is even, and hits it where the slot is odd */
static void action_slam(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Float distance_a;
    Float distance_b;
    int after;
    int before;
    Bool landed;

    switch (golem_e->action_mode) {
    case MODE_SLAM_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem_e->action_mode++;
        break;
    case MODE_SLAM_WIDE:
        /* @hack spelling: the distances set in one chain, where a statement
         * for each would do */
        distance_a = distance_b = FAR_AWAY;
        golem_e_turn(task);
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem_e->slam_state] &&
            motion_frame(motion) >= slam_wide_sound_frame &&
            motion_frame(motion) <
                slam_wide_sound_frame + motion->speed_scale) {
            after = (golem_e->slot + SLAM_WIDE) % GOLEM_SLOTS;
            before = (golem_e->slot + (GOLEM_SLOTS - SLAM_WIDE)) % GOLEM_SLOTS;
            if ((golem_e_foothold_state(before) != FOOTHOLD_GONE &&
                 golem_e_foothold_state(before) < golem_e->slam_state) ||
                (golem_e_foothold_state(after) != FOOTHOLD_GONE &&
                 golem_e_foothold_state(after) < golem_e->slam_state)) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem_e->slam_state] &&
            motion_frame(motion) >= slam_wide_frame) {
            after = (golem_e->slot + SLAM_WIDE) % GOLEM_SLOTS;
            before = (golem_e->slot + (GOLEM_SLOTS - SLAM_WIDE)) % GOLEM_SLOTS;
            /* @bug uninitialized: landed is unset where neither slot two away
             * takes a blow, as shared/golem.h says (the test at 0x8c50784e) */
#ifdef NONMATCHING
            /* this side keeps the tag's result: the image's register for
             * landed holds before, so landed starts TRUE unless before is slot
             * 0. where neither blow lands, the golem then shakes from hand B */
            landed = before != 0;
#endif
            if (golem_e_foothold_state(before) != FOOTHOLD_GONE &&
                golem_e_foothold_state(before) < golem_e->slam_state) {
                distance_a =
                    njDistanceP2P(&golem_e_hand_a_pos, &player_work[0]->pos);
                golem_e_foothold_hit(before);
                landed = TRUE;
            }
            if (golem_e_foothold_state(after) != FOOTHOLD_GONE &&
                golem_e_foothold_state(after) < golem_e->slam_state) {
                /* @bug logic: hand B's distance goes into distance_a, as
                 * shared/golem.h says (the store at 0x8c507846) */
                distance_a =
                    njDistanceP2P(&golem_e_hand_b_pos, &player_work[0]->pos);
                golem_e_foothold_hit(after);
                landed = TRUE;
            }
            if (landed) {
                golem_rumble(distance_a < distance_b ? &golem_e_hand_a_pos
                                                     : &golem_e_hand_b_pos,
                             blow_shake_size, blow_shake_steps);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_SLAM_NEAR:
        /* @hack spelling: the distances set in one chain, where a statement
         * for each would do */
        distance_a = distance_b = FAR_AWAY;
        golem_e_turn(task);
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem_e->slam_state] &&
            motion_frame(motion) >= slam_near_sound_frame &&
            motion_frame(motion) <
                slam_near_sound_frame + motion->speed_scale) {
            after = (golem_e->slot + 1) % GOLEM_SLOTS;
            before = (golem_e->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
            if ((golem_e_foothold_state(before) != FOOTHOLD_GONE &&
                 golem_e_foothold_state(before) == golem_e->slam_state) ||
                (golem_e_foothold_state(after) != FOOTHOLD_GONE &&
                 golem_e_foothold_state(after) == golem_e->slam_state)) {
                sound_play(SLAM_NEAR_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, slam_near_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem_e->slam_state] &&
            motion_frame(motion) >= slam_near_frame) {
            after = (golem_e->slot + 1) % GOLEM_SLOTS;
            before = (golem_e->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
            landed = FALSE;
            if (golem_e_foothold_state(before) != FOOTHOLD_GONE &&
                golem_e_foothold_state(before) == golem_e->slam_state) {
                distance_a =
                    njDistanceP2P(&golem_e_hand_a_pos, &player_work[0]->pos);
                golem_e_foothold_hit(before);
                landed = TRUE;
            }
            if (golem_e_foothold_state(after) != FOOTHOLD_GONE &&
                golem_e_foothold_state(after) == golem_e->slam_state) {
                distance_b =
                    njDistanceP2P(&golem_e_hand_b_pos, &player_work[0]->pos);
                golem_e_foothold_hit(after);
                landed = TRUE;
            }
            if (landed) {
                golem_rumble(distance_a < distance_b ? &golem_e_hand_a_pos
                                                     : &golem_e_hand_b_pos,
                             blow_shake_size, blow_shake_steps);
            }
            golem_e->action_mode++;
        }
        break;
    case MODE_SLAM:
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_SLAM][golem_e->slam_state] &&
            motion_frame(motion) >= slam_sound_frame &&
            motion_frame(motion) < slam_sound_frame + motion->speed_scale) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE &&
                golem_e_foothold_state(golem_e->slot) == golem_e->slam_state) {
                sound_play(SLAM_SOUND, SLAM_SOUND_KEY(task), 0, 0);
                sound_set_volume(SLAM_SOUND_KEY(task), SOUND_ANY_ID,
                                 slam_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_SLAM][golem_e->slam_state] &&
            motion_frame(motion) >= slam_frame) {
            if (golem_e_foothold_state(golem_e->slot) != FOOTHOLD_GONE &&
                golem_e_foothold_state(golem_e->slot) == golem_e->slam_state) {
                sound_play(SLAM_SOUND, SLAM_SOUND_KEY(task), 0, 0);
                sound_set_volume(SLAM_SOUND_KEY(task), SOUND_ANY_ID,
                                 slam_volume);
                if (golem_e->slot % 2 == 0) {
                    golem_e_foothold_break(golem_e->slot);
                } else {
                    golem_e_foothold_hit(golem_e->slot);
                }
                golem_rumble(&golem_e_head_pos, blow_shake_size,
                             blow_shake_steps);
            }
            golem_e->action_mode++;
        }
        break;
    }
    if (motion_index(motion) ==
            slam_motions[GOLEM_SLAM_END][golem_e->slam_state] &&
        motion_ended(motion)) {
        golem_e->rest_steps = 0;
        golem_e_request_action(task, GOLEM_ACTION_REST);
        motion_link(motion, GOLEM_MOTION_IDLE);
    }
    die_when_spent(task);
}

/* sink the golem and bring the footholds back. once the death's motion has
 * run die_frames, clear the stage, unless player 0 has missed */
static void action_die(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    int volume_scale;

    work->pos.y -=
        _builtin_fabsf(njCos(game_state.steps * sink_turn)) * sink_size;
    switch (golem_e->action_mode) {
    case MODE_RESTORE:
        golem_e_footholds_restore();
        golem_e->action_mode++;
        break;
    case MODE_SINK:
        if (game_state.steps % die_sound_steps == 0) {
            /* @bug arithmetic: volume_scale truncates to 0 past the motion's
             * first frame, as shared/golem.h says (0x8c507d5c) */
            volume_scale = (((die_frames - motion_frame(motion)) / die_frames) *
                            DIE_SPAN) +
                           DIE_LEAST;
            sound_play(DIE_SOUND, DIE_SOUND_KEY(task), 0, 0);
            sound_set_volume(DIE_SOUND_KEY(task), SOUND_ANY_ID,
                             die_volume * volume_scale);
        }
        if (motion_frame(motion) > die_frames) {
            golem_e->action_mode++;
        }
        break;
    case MODE_CLEAR:
        if (!(player_records[0]->flags & PLAYER_MISSED)) {
            stage_clear(0);
        }
        golem_e->action_mode++;
        break;
    case MODE_DONE:
        break;
    }
}

static void die_when_spent(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    if (golem_e->power <= 0.0F) {
        golem_e->power = 0.0F;
        golem_e_request_action(task, GOLEM_ACTION_DIE);
        motion_link_frames(motion, GOLEM_MOTION_DIE, die_link_steps);
    }
}
