/* the Egg Golem's actions in BOSS_GOLEM.PRS: its choice of what to do next,
 * and each of its moves. the golem's step runs them, and no object list names
 * them. boss_golem_e/actions.c is BOSS_GOLEM_E.PRS's twin, a source of its
 * own. that golem has no flash, so no drops fall before its hands come down.
 * it starts its hands down as it chooses them, and its death sets off no
 * explosion */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_golem/actions.h"
#include "boss_golem/explosion.h"
#include "boss_golem/footholds.h"
#include "boss_golem/golem.h"
#include "boss_golem/head_parts.h"
#include "boss_golem/tactics.h"
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
/* request action for golem_actions_step's next run. only this source calls
 * it, and SHC expands each call. it is external so that SHC still writes its
 * body, which the image holds */
void golem_request_action(struct task *task, int action);
/* NOLINTEND(misc-use-internal-linkage) */

/* each move's motion for each of its stages, by the struck foothold's state. a
 * gone foothold takes FOOTHOLD_SUNK_TWICE's motion */
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
static Angle dat_8c609e38 = 0x2800;
/* player 0 stands behind the golem at this bearing or more, either way */
static Angle behind_bearing = QUARTER_TURN;
/* the frames of its motion at which each blow lands and its sound plays, the
 * sound first. a slam's hands land two slots away, then beside its
 * slot. the slam itself lands in a motion of its own */
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
static Sint32 hold_volume = 75;
static Sint32 slam_volume = 100;
static Sint32 wave_volume = 25;
static Sint32 die_volume = 75;
static Sint32 dat_8c609e78 = 60;
/* each action's step, by enum golem_action */
static void (*actions[])(struct task *task) = {
    action_wait,       action_rest,         action_face_player,
    action_aim_hand_a, action_aim_hand_b,   action_strike_a,
    action_strike_b,   action_hands_down_a, action_hands_down_b,
    action_slam,       action_die,
};
static Float dat_8c609ea8 = -50.0F;
static Float dat_8c609eac = 160.0F;
static Float dat_8c609eb0 = 100.0F;
static Float dat_8c609eb4 = 0.5F;
static Float blow_shake_size = 150.0F;
static Sint32 blow_shake_steps = 3;
/* the explosion of the golem's death: its outer shell's sizes, and its life
 * in steps */
static Float explosion_start = 5.0F;
static Float explosion_end = 40.0F;
static Sint32 explosion_steps = 120;
/* the death plays die_frames of its motion, which links in over
 * die_link_steps. its sound plays every die_sound_steps. each step sinks the
 * golem by sink_size times the magnitude of a cosine. the cosine's angle turns
 * sink_turn a step */
static Float die_frames = 120.0F;
static Angle sink_turn = 0x400;
static Float sink_size = 0.25F;
static Uint32 die_sound_steps = 40;
static Sint32 die_link_steps = 20;

/* the opening shot's steps before the fight starts, unless player 0 presses
 * a face button first */
#define WAIT_STEPS 360
/* the stage messages a retry's messenger shows, one for each of the first
 * three misses or retries. a later one shows the third */
#define MESSAGES 3
/* the steps a strike holds still between its blows */
#define STRIKE_PAUSE 30
/* the steps each move waits at its end, before the golem rests */
#define STRIKE_REST 120
#define HANDS_DOWN_REST 120
#define SLAM_REST 180
/* a slam's hands land SLAM_WIDE slots either side of its slot, then one slot
 * either side */
#define SLAM_WIDE 2
/* the flash's second drop falls once the motion the golem plays passes this
 * share */
#define SECOND_DROP_SHARE 0.333F
/* the steps between plays of HOLD_SOUND while the hands hold */
#define HOLD_SOUND_STEPS 30
/* the golem's sound ids, and the keys they play for. a key is an address a few
 * bytes into the golem's task. sound_set_volume finds a sound by its key
 * alone. the shock wave and the death share one. a strike's sound plays for
 * the task itself */
#define MOVE_SOUND 6
#define MOVE_SOUND_KEY(task) ((char *)(task) + 9)
#define STRIKE_SOUND 4
#define WAVE_SOUND 5
#define WAVE_SOUND_KEY(task) ((char *)(task) + 1)
#define HOLD_SOUND 9
#define HOLD_SOUND_KEY(task) ((char *)(task) + 2)
#define SLAM_SOUND 10
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

/* each action's modes, in golem->action_mode. an action steps through its
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
/* the hands come down after three drops of the flash, of which only the last
 * hurts */
enum hands_down_mode {
    MODE_FIRST_DROP,
    MODE_SECOND_DROP,
    MODE_HURTING_DROP,
    MODE_HANDS_START,
    MODE_HANDS_HOLD,
    MODE_HANDS_REST
};
enum slam_mode {
    MODE_SLAM_START,
    MODE_SLAM_WIDE,
    MODE_SLAM_NEAR,
    MODE_SLAM,
    MODE_SLAM_LIFT,
    MODE_SLAM_PAUSE,
    MODE_SLAM_END
};
enum die_mode { MODE_EXPLODE, MODE_SINK, MODE_CLEAR, MODE_DONE };

void golem_action_start(struct task *task, int action)
{
    golem->action = action;
    golem->next_action = GOLEM_NO_ACTION;
    golem->action_mode = 0;
    golem->action_steps = 0;
}

void golem_actions_step(struct task *task)
{
    if (golem->next_action != GOLEM_NO_ACTION) {
        golem->action = golem->next_action;
        golem->next_action = GOLEM_NO_ACTION;
        golem->action_mode = 0;
        golem->action_steps = 0;
    }
    actions[golem->action](task);
    golem->action_steps++;
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
void golem_request_action(struct task *task, int action)
{
    golem->next_action = action;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void action_wait(struct task *task)
{
    struct boss_record *record = task->extra.p;

    if (golem->camera_steps >= WAIT_STEPS ||
        (player_pads[0]->press & PDD_DGT_TA) ||
        (player_pads[0]->press & PDD_DGT_TB) ||
        (player_pads[0]->press & PDD_DGT_TX) ||
        (player_pads[0]->press & PDD_DGT_TY)) {
        golem_camera_request(task, GOLEM_CAMERA_FAR);
        golem_request_action(task, GOLEM_ACTION_REST);
        motion_set(record->block, GOLEM_MOTION_IDLE);
        boss_messenger_summon_capped(0, MESSAGES, task);
    }
}

/* choose the next action once the golem's motion ends. a strike starts with
 * hand B where golem_player_facing_up is TRUE, and with hand A otherwise */
static void action_rest(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing;

    if (motion_ended(motion)) {
        if (golem_may_attack(task)) {
            bearing = golem_turn_to_player();
            if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
                if (bearing > 0) {
                    golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
                } else {
                    golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
                }
            } else if (!golem_player_near(task)) {
                if (_builtin_fabsf((Float)bearing) >= (Float)aim_bearing ||
                    golem->power <= GOLEM_POWER_LOW) {
                    golem_request_action(task, GOLEM_ACTION_FACE_PLAYER);
                    golem_set_turn(task, GOLEM_TURN_TO_PLAYER);
                    golem->timer = 0;
                } else {
                    if (!golem_player_facing_up()) {
                        golem_request_action(task, GOLEM_ACTION_AIM_HAND_A);
                        golem_set_turn(task, GOLEM_TURN_TO_HAND_A);
                    } else {
                        golem_request_action(task, GOLEM_ACTION_AIM_HAND_B);
                        golem_set_turn(task, GOLEM_TURN_TO_HAND_B);
                    }
                    golem->timer = 0;
                }
            }
        } else if (golem->rest_steps > golem->rest_wait / 2 &&
                   !golem_player_near(task)) {
            golem_request_action(task, GOLEM_ACTION_FACE_PLAYER);
            golem_set_turn(task, GOLEM_TURN_TO_PLAYER);
            golem->timer = 0;
        }
    }
    die_when_spent(task);
}

static void action_face_player(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Angle bearing = golem_turn_to_player();

    golem_turn(task);
    if (golem_may_attack(task)) {
        if (_builtin_fabsf((Float)bearing) >= (Float)behind_bearing) {
            if (bearing > 0) {
                golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
            } else {
                golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
            }
        } else if (golem->power <= GOLEM_POWER_LOW) {
            if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
                golem_aim_slot(task, GOLEM_HAND_A);
                golem->slam_state = golem_footholds_lowest_near(golem->slot);
                golem_request_action(task, GOLEM_ACTION_SLAM);
                golem_set_turn(task, GOLEM_TURN_TO_SLAM);
                motion_link(motion,
                            slam_motions[GOLEM_SLAM_START][golem->slam_state]);
            }
        } else if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            if (!golem_player_facing_up()) {
                golem_request_action(task, GOLEM_ACTION_AIM_HAND_A);
                golem_set_turn(task, GOLEM_TURN_TO_HAND_A);
            } else {
                golem_request_action(task, GOLEM_ACTION_AIM_HAND_B);
                golem_set_turn(task, GOLEM_TURN_TO_HAND_B);
            }
            golem->timer = 0;
        }
    } else if (golem_player_near(task)) {
        golem_request_action(task, GOLEM_ACTION_REST);
        golem_set_turn(task, GOLEM_TURN_NONE);
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
            golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
        } else {
            golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
        }
    } else {
        golem_aim_slot(task, GOLEM_HAND_A);
        golem_turn(task);
        bearing = golem_angle_turn(golem->heading, golem->aim_heading);
        if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            golem_request_action(task, GOLEM_ACTION_STRIKE_A);
            golem->hand_a_state = golem_foothold_state(golem->slot);
            motion_link(
                motion,
                strike_a_motions[GOLEM_STRIKE_START][golem->hand_a_state]);
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
            golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_B);
        } else {
            golem_request_action(task, GOLEM_ACTION_HANDS_DOWN_A);
        }
    } else {
        golem_aim_slot(task, GOLEM_HAND_B);
        golem_turn(task);
        bearing = golem_angle_turn(golem->heading, golem->aim_heading);
        if (_builtin_fabsf((Float)bearing) < (Float)aim_bearing) {
            golem_request_action(task, GOLEM_ACTION_STRIKE_B);
            golem->hand_b_state = golem_foothold_state(golem->slot);
            motion_link(
                motion,
                strike_b_motions[GOLEM_STRIKE_START][golem->hand_b_state]);
        }
    }
    die_when_spent(task);
}

/* a blow of either strike on a gone foothold makes no sound, hit or shock
 * wave */
static void action_strike_a(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    switch (golem->action_mode) {
    case MODE_STRIKE_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem->action_mode++;
        break;
    case MODE_FIRST_BLOW:
        golem_turn(task);
        if (motion_index(motion) ==
                strike_a_motions[GOLEM_STRIKE_BLOW][golem->hand_a_state] &&
            motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                strike_a_motions[GOLEM_STRIKE_BLOW][golem->hand_a_state] &&
            motion_frame(motion) >= strike_frame) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_hand_a_pos, blow_shake_size,
                             blow_shake_steps);
                golem_foothold_hit(golem->slot);
            }
            golem->action_mode++;
        }
        break;
    case MODE_FIRST_RISE:
        if (motion_index(motion) ==
            strike_a_motions[GOLEM_STRIKE_BLOW][golem->hand_a_state]) {
            if (motion_ended(motion)) {
                golem->action_mode++;
                golem->timer = 0;
                motion_link(
                    motion,
                    strike_a_motions[GOLEM_STRIKE_RISE][golem->hand_a_state]);
                if (golem->hand_a_state != FOOTHOLD_GONE) {
                    golem_wave_create(task, golem->hand_a_state);
                    sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                    sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                     wave_volume);
                }
            }
        }
        break;
    case MODE_FIRST_PAUSE:
        golem->hold_motion = TRUE;
        if (++golem->timer > STRIKE_PAUSE) {
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
            golem_set_slot(task, (golem->slot + 1) % GOLEM_SLOTS, GOLEM_HAND_B);
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_AIM:
        if (motion_ended(motion)) {
            golem->hand_b_state = golem_foothold_state(golem->slot);
            motion_link(motion, strike_a_motions[GOLEM_STRIKE_SECOND_BLOW]
                                                [golem->hand_b_state]);
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_BLOW:
        golem_turn(task);
        if (motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_frame(motion) >= strike_frame) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_hand_b_pos, blow_shake_size,
                             blow_shake_steps);
                golem_foothold_hit(golem->slot);
            }
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_RISE:
        if (motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
            motion_link(motion, strike_a_motions[GOLEM_STRIKE_SECOND_RISE]
                                                [golem->hand_b_state]);
            if (golem->hand_b_state != FOOTHOLD_GONE) {
                golem_wave_create(task, golem->hand_b_state);
                sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                 wave_volume);
            }
        }
        break;
    case MODE_LAST_PAUSE:
        golem->hold_motion = TRUE;
        if (++golem->timer > STRIKE_REST) {
            golem->action_mode++;
        }
        /* fall through */
    case MODE_STRIKE_END:
        if (motion_ended(motion)) {
            golem->rest_steps = 0;
            golem_request_action(task, GOLEM_ACTION_REST);
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

    switch (golem->action_mode) {
    case MODE_STRIKE_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem->action_mode++;
        break;
    case MODE_FIRST_BLOW:
        golem_turn(task);
        if (motion_index(motion) ==
                strike_b_motions[GOLEM_STRIKE_BLOW][golem->hand_b_state] &&
            motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                strike_b_motions[GOLEM_STRIKE_BLOW][golem->hand_b_state] &&
            motion_frame(motion) >= strike_frame) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_hand_b_pos, blow_shake_size,
                             blow_shake_steps);
                golem_foothold_hit(golem->slot);
            }
            golem->action_mode++;
        }
        break;
    case MODE_FIRST_RISE:
        if (motion_index(motion) ==
            strike_b_motions[GOLEM_STRIKE_BLOW][golem->hand_b_state]) {
            if (motion_ended(motion)) {
                golem->action_mode++;
                golem->timer = 0;
                motion_link(
                    motion,
                    strike_b_motions[GOLEM_STRIKE_RISE][golem->hand_b_state]);
                if (golem->hand_b_state != FOOTHOLD_GONE) {
                    golem_wave_create(task, golem->hand_b_state);
                    sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                    sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                     wave_volume);
                }
            }
        }
        break;
    case MODE_FIRST_PAUSE:
        golem->hold_motion = TRUE;
        if (++golem->timer > STRIKE_PAUSE) {
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
            golem_set_slot(task,
                           (golem->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS,
                           GOLEM_HAND_A);
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_AIM:
        if (motion_ended(motion)) {
            golem->hand_a_state = golem_foothold_state(golem->slot);
            motion_link(motion, strike_b_motions[GOLEM_STRIKE_SECOND_BLOW]
                                                [golem->hand_a_state]);
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_BLOW:
        golem_turn(task);
        if (motion_frame(motion) >= strike_sound_frame &&
            motion_frame(motion) < strike_sound_frame + motion->speed_scale) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_frame(motion) >= strike_frame) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE) {
                golem_rumble(&golem_hand_a_pos, blow_shake_size,
                             blow_shake_steps);
                golem_foothold_hit(golem->slot);
            }
            golem->action_mode++;
        }
        break;
    case MODE_SECOND_RISE:
        if (motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
            motion_link(motion, strike_b_motions[GOLEM_STRIKE_SECOND_RISE]
                                                [golem->hand_a_state]);
            if (golem->hand_a_state != FOOTHOLD_GONE) {
                golem_wave_create(task, golem->hand_a_state);
                sound_play(WAVE_SOUND, WAVE_SOUND_KEY(task), 0, 0);
                sound_set_volume(WAVE_SOUND_KEY(task), SOUND_ANY_ID,
                                 wave_volume);
            }
        }
        break;
    case MODE_LAST_PAUSE:
        golem->hold_motion = TRUE;
        if (++golem->timer > STRIKE_REST) {
            golem->action_mode++;
        }
        /* fall through */
    case MODE_STRIKE_END:
        if (motion_ended(motion)) {
            golem->rest_steps = 0;
            golem_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
    die_when_spent(task);
}

static void action_hands_down_a(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    golem->hands_down_state = golem_footholds_lowest_state();
    switch (golem->action_mode) {
    case MODE_FIRST_DROP:
        golem_flash_drop_create(task, GOLEM_FLASH_DROP_FAINT);
        golem_set_turn(task, GOLEM_TURN_TO_PLAYER);
        golem->action_mode++;
        die_when_spent(task);
        break;
    case MODE_SECOND_DROP:
        if (!golem_player_near(task)) {
            golem_turn(task);
        }
        if (motion_share(motion) > SECOND_DROP_SHARE) {
            golem_flash_drop_create(task, GOLEM_FLASH_DROP_FAINT);
            golem->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HURTING_DROP:
        golem_turn(task);
        if (motion_ended(motion)) {
            golem_flash_drop_create(task, GOLEM_FLASH_DROP_HURTS);
            golem->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_START:
        golem_turn(task);
        if (motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
            motion_link(motion, hands_down_a_motions[GOLEM_HANDS_LOWER]
                                                    [golem->hands_down_state]);
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_HOLD:
        /* @bug logic: the golem neither turns nor checks its power while its
         * hands come down. the two tests wait on a motion of
         * hands_down_b_motions, which this action never plays. they read that
         * table at 0x8c50746e and 0x8c50748c. action_hands_down_b loads the
         * same table at 0x8c5075ea */
        /* @hack spelling: one test written twice, where one if would do */
        if (motion_index(motion) ==
            hands_down_b_motions[GOLEM_HANDS_LOWER][golem->hands_down_state]) {
            golem_turn(task);
        }
        if (motion_index(motion) ==
            hands_down_b_motions[GOLEM_HANDS_LOWER][golem->hands_down_state]) {
            die_when_spent(task);
        }
        if (motion_index(motion) ==
                hands_down_a_motions[GOLEM_HANDS_HOLD]
                                    [golem->hands_down_state] &&
            golem_holding(task) && game_state.steps % HOLD_SOUND_STEPS == 0) {
            sound_play(HOLD_SOUND, HOLD_SOUND_KEY(task), 0, 0);
            sound_set_volume(HOLD_SOUND_KEY(task), SOUND_ANY_ID, hold_volume);
        }
        if (motion_index(motion) ==
                hands_down_a_motions[GOLEM_HANDS_RISE]
                                    [golem->hands_down_state] &&
            motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
        }
        break;
    case MODE_HANDS_REST:
        if (++golem->timer > HANDS_DOWN_REST) {
            golem->rest_steps = 0;
            golem_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
}

/* action_hands_down_a, with hands_down_b_motions throughout */
static void action_hands_down_b(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    golem->hands_down_state = golem_footholds_lowest_state();
    switch (golem->action_mode) {
    case MODE_FIRST_DROP:
        golem_flash_drop_create(task, GOLEM_FLASH_DROP_FAINT);
        golem_set_turn(task, GOLEM_TURN_TO_PLAYER);
        golem->action_mode++;
        die_when_spent(task);
        break;
    case MODE_SECOND_DROP:
        if (!golem_player_near(task)) {
            golem_turn(task);
        }
        if (motion_share(motion) > SECOND_DROP_SHARE) {
            golem_flash_drop_create(task, GOLEM_FLASH_DROP_FAINT);
            golem->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HURTING_DROP:
        golem_turn(task);
        if (motion_ended(motion)) {
            golem_flash_drop_create(task, GOLEM_FLASH_DROP_HURTS);
            golem->action_mode++;
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_START:
        golem_turn(task);
        if (motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
            motion_link(motion, hands_down_b_motions[GOLEM_HANDS_LOWER]
                                                    [golem->hands_down_state]);
            sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
            sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        }
        die_when_spent(task);
        break;
    case MODE_HANDS_HOLD:
        /* @hack spelling: one test written twice, where one if would do */
        if (motion_index(motion) ==
            hands_down_b_motions[GOLEM_HANDS_LOWER][golem->hands_down_state]) {
            golem_turn(task);
        }
        if (motion_index(motion) ==
            hands_down_b_motions[GOLEM_HANDS_LOWER][golem->hands_down_state]) {
            die_when_spent(task);
        }
        if (motion_index(motion) ==
                hands_down_b_motions[GOLEM_HANDS_HOLD]
                                    [golem->hands_down_state] &&
            golem_holding(task) && game_state.steps % HOLD_SOUND_STEPS == 0) {
            sound_play(HOLD_SOUND, HOLD_SOUND_KEY(task), 0, 0);
            sound_set_volume(HOLD_SOUND_KEY(task), SOUND_ANY_ID, hold_volume);
        }
        if (motion_index(motion) ==
                hands_down_b_motions[GOLEM_HANDS_RISE]
                                    [golem->hands_down_state] &&
            motion_ended(motion)) {
            golem->action_mode++;
            golem->timer = 0;
        }
        break;
    case MODE_HANDS_REST:
        if (++golem->timer > HANDS_DOWN_REST) {
            golem->rest_steps = 0;
            golem_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
}

/* slam_state picks which footholds take the slam's blows. the hands strike a
 * foothold two slots away only where it is less sunk than slam_state. beside
 * the golem's slot, they strike one only at slam_state. the slam on the slot
 * takes the same test, and breaks the foothold where the slot is even */
static void action_slam(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Float distance_a;
    Float distance_b;
    int after;
    int before;
    Bool landed;

    switch (golem->action_mode) {
    case MODE_SLAM_START:
        sound_play(MOVE_SOUND, MOVE_SOUND_KEY(task), 0, 0);
        sound_set_volume(MOVE_SOUND_KEY(task), SOUND_ANY_ID, move_volume);
        golem->action_mode++;
        break;
    case MODE_SLAM_WIDE:
        /* @hack spelling: distance_b set ahead of distance_a, where either
         * order would do */
        distance_b = FAR_AWAY;
        distance_a = FAR_AWAY;
        golem_turn(task);
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem->slam_state] &&
            motion_frame(motion) >= slam_wide_sound_frame &&
            motion_frame(motion) <
                slam_wide_sound_frame + motion->speed_scale) {
            after = (golem->slot + SLAM_WIDE) % GOLEM_SLOTS;
            before = (golem->slot + (GOLEM_SLOTS - SLAM_WIDE)) % GOLEM_SLOTS;
            if ((golem_foothold_state(before) != FOOTHOLD_GONE &&
                 golem_foothold_state(before) < golem->slam_state) ||
                (golem_foothold_state(after) != FOOTHOLD_GONE &&
                 golem_foothold_state(after) < golem->slam_state)) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem->slam_state] &&
            motion_frame(motion) >= slam_wide_frame) {
            after = (golem->slot + SLAM_WIDE) % GOLEM_SLOTS;
            before = (golem->slot + (GOLEM_SLOTS - SLAM_WIDE)) % GOLEM_SLOTS;
            /* @bug uninitialized: landed is unset where neither slot two away
             * takes a blow, as shared/golem.h says (the test at 0x8c507a86) */
#ifdef NONMATCHING
            /* this side keeps the tag's result: the image's register for
             * landed holds before, so landed starts TRUE unless before is slot
             * 0. where neither blow lands, the golem then shakes from hand B */
            landed = before != 0;
#endif
            if (golem_foothold_state(before) != FOOTHOLD_GONE &&
                golem_foothold_state(before) < golem->slam_state) {
                distance_a =
                    njDistanceP2P(&golem_hand_a_pos, &player_work[0]->pos);
                golem_foothold_hit(before);
                landed = TRUE;
            }
            if (golem_foothold_state(after) != FOOTHOLD_GONE &&
                golem_foothold_state(after) < golem->slam_state) {
                /* @bug logic: hand B's distance goes into distance_a, as
                 * shared/golem.h says (the store at 0x8c507a7e) */
                distance_a =
                    njDistanceP2P(&golem_hand_b_pos, &player_work[0]->pos);
                golem_foothold_hit(after);
                landed = TRUE;
            }
            if (landed) {
                golem_rumble(distance_a < distance_b ? &golem_hand_a_pos
                                                     : &golem_hand_b_pos,
                             blow_shake_size, blow_shake_steps);
            }
            golem->action_mode++;
        }
        break;
    case MODE_SLAM_NEAR:
        distance_b = FAR_AWAY;
        distance_a = FAR_AWAY;
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem->slam_state] &&
            motion_frame(motion) >= slam_near_sound_frame &&
            motion_frame(motion) <
                slam_near_sound_frame + motion->speed_scale) {
            after = (golem->slot + 1) % GOLEM_SLOTS;
            before = (golem->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
            if ((golem_foothold_state(before) != FOOTHOLD_GONE &&
                 golem_foothold_state(before) == golem->slam_state) ||
                (golem_foothold_state(after) != FOOTHOLD_GONE &&
                 golem_foothold_state(after) == golem->slam_state)) {
                sound_play(STRIKE_SOUND, task, 0, 0);
                sound_set_volume(task, SOUND_ANY_ID, strike_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_BLOWS][golem->slam_state] &&
            motion_frame(motion) >= slam_near_frame) {
            after = (golem->slot + 1) % GOLEM_SLOTS;
            before = (golem->slot + (GOLEM_SLOTS - 1)) % GOLEM_SLOTS;
            landed = FALSE;
            if (golem_foothold_state(before) != FOOTHOLD_GONE &&
                golem_foothold_state(before) == golem->slam_state) {
                distance_a =
                    njDistanceP2P(&golem_hand_a_pos, &player_work[0]->pos);
                golem_foothold_hit(before);
                landed = TRUE;
            }
            if (golem_foothold_state(after) != FOOTHOLD_GONE &&
                golem_foothold_state(after) == golem->slam_state) {
                distance_b =
                    njDistanceP2P(&golem_hand_b_pos, &player_work[0]->pos);
                golem_foothold_hit(after);
                landed = TRUE;
            }
            if (landed) {
                golem_rumble(distance_a < distance_b ? &golem_hand_a_pos
                                                     : &golem_hand_b_pos,
                             blow_shake_size, blow_shake_steps);
            }
            golem->action_mode++;
        }
        break;
    case MODE_SLAM:
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_SLAM][golem->slam_state] &&
            motion_frame(motion) >= slam_sound_frame &&
            motion_frame(motion) < slam_sound_frame + motion->speed_scale) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE &&
                golem_foothold_state(golem->slot) == golem->slam_state) {
                sound_play(SLAM_SOUND, SLAM_SOUND_KEY(task), 0, 0);
                sound_set_volume(SLAM_SOUND_KEY(task), SOUND_ANY_ID,
                                 slam_volume);
            }
        }
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_SLAM][golem->slam_state] &&
            motion_frame(motion) >= slam_frame) {
            if (golem_foothold_state(golem->slot) != FOOTHOLD_GONE &&
                golem_foothold_state(golem->slot) == golem->slam_state) {
                if (golem->slot % 2 == 0) {
                    golem_foothold_break(golem->slot);
                } else {
                    golem_foothold_hit(golem->slot);
                }
                golem_rumble(&golem_flash_pos, blow_shake_size,
                             blow_shake_steps);
            }
            golem->action_mode++;
        }
        break;
    case MODE_SLAM_LIFT:
        if (motion_ended(motion)) {
            motion_link(motion,
                        slam_motions[GOLEM_SLAM_END][golem->slam_state]);
            golem->timer = 0;
            golem->action_mode++;
        }
        break;
    case MODE_SLAM_PAUSE:
        golem->hold_motion = TRUE;
        if (++golem->timer > SLAM_REST) {
            golem->action_mode++;
        }
        break;
    case MODE_SLAM_END:
        if (motion_index(motion) ==
                slam_motions[GOLEM_SLAM_END][golem->slam_state] &&
            motion_ended(motion)) {
            golem->rest_steps = 0;
            golem_request_action(task, GOLEM_ACTION_REST);
            motion_link(motion, GOLEM_MOTION_IDLE);
        }
        break;
    }
    die_when_spent(task);
}

static void action_die(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    int volume_scale;

    work->pos.y -=
        _builtin_fabsf(njCos(game_state.steps * sink_turn)) * sink_size;
    switch (golem->action_mode) {
    case MODE_EXPLODE:
        golem_explosion_create(&golem->weak_spot_pos, explosion_start,
                               explosion_end, (Float)explosion_steps);
        golem_footholds_restore();
        golem->action_mode++;
        break;
    case MODE_SINK:
        if (game_state.steps % die_sound_steps == 0) {
            /* @bug arithmetic: volume_scale truncates to 0 past the motion's
             * first frame, as shared/golem.h says (0x8c507fec) */
            volume_scale = (((die_frames - motion_frame(motion)) / die_frames) *
                            DIE_SPAN) +
                           DIE_LEAST;
            sound_play(DIE_SOUND, DIE_SOUND_KEY(task), 0, 0);
            sound_set_volume(DIE_SOUND_KEY(task), SOUND_ANY_ID,
                             die_volume * volume_scale);
        }
        if (motion_frame(motion) > die_frames) {
            golem->action_mode++;
        }
        break;
    case MODE_CLEAR:
        if (!(player_records[0]->flags & PLAYER_MISSED)) {
            stage_clear(0);
        }
        golem->action_mode++;
        break;
    case MODE_DONE:
        break;
    }
}

static void die_when_spent(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;

    if (golem->power <= 0.0F) {
        golem->power = 0.0F;
        golem_request_action(task, GOLEM_ACTION_DIE);
        motion_link_frames(motion, GOLEM_MOTION_DIE, die_link_steps);
    }
}
