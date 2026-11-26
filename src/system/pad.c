/* the pads: the scan of the four ports, and what the game reads of it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "save/save.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/pad.h"

/* what a port's support must hold for its device to be a pad */
#define PAD_SUPPORT                                                            \
    (PDD_DEV_SUPPORT_AY1 | PDD_DEV_SUPPORT_AX1 | PDD_DEV_SUPPORT_AL |          \
     PDD_DEV_SUPPORT_AR | PDD_DEV_SUPPORT_TX | PDD_DEV_SUPPORT_TY |            \
     PDD_DEV_SUPPORT_ST | PDD_DEV_SUPPORT_TA | PDD_DEV_SUPPORT_TB)
/* an idle record's l and r, half a trigger's range, and its off, every
 * button up */
#define IDLE_TRIGGER 0x80
#define IDLE_OFF 0xffffffff
/* the buttons a pad holds together to reset the game, and how many scans in
 * a row must find them held */
#define RESET_BUTTONS                                                          \
    (PDD_DGT_TA | PDD_DGT_TB | PDD_DGT_TX | PDD_DGT_TY | PDD_DGT_ST)
#define RESET_SCANS 1

static void pad_count_reset(void);
static void pad_read_buttons(int port);
static void pad_read_sticks(void);
static void pad_run_demo(void);
static void pad_read_actions(void);

Uint32 pad_action_buttons = PDD_DGT_TB | PDD_DGT_TX;
Uint32 pad_jump_buttons = PDD_DGT_TA;
/* a mask of no button */
Uint32 dat_8c0db110 = 0; /* NOLINT(misc-use-internal-linkage) */
Uint32 pad_choose_buttons = PDD_DGT_TA | PDD_DGT_ST;
Uint32 pad_cancel_buttons = PDD_DGT_TB;
Uint32 pad_grab_buttons = PDD_DGT_TB | PDD_DGT_TY | PDD_DGT_TX;
Uint32 pad_next_action_buttons = PDD_DGT_TY;
/* a mask of X */
Uint32 dat_8c0db124 = PDD_DGT_TX; /* NOLINT(misc-use-internal-linkage) */
/* what player_pads points to for a port without a pad */
/* @hack type: an array of one record, where a record taken by & would do */
static PDS_PERIPHERAL pad_none[1] = {{0}};
static Sint8 reset_pressed = 0;
static Sint16 reset_held_scans = 0;
/* the ports pad_scan reads, in player_pads' order */
static Uint32 scan_ports[PAD_PORTS] = {PDD_PORT_A0, PDD_PORT_B0, PDD_PORT_C0,
                                       PDD_PORT_D0};

PDS_PERIPHERAL player_pad_records[PAD_PLAYERS];
const PDS_PERIPHERAL *player_pads[PAD_PORTS];
Uint8 dat_8c24f098[20];
struct pad_stick pad_sticks[PAD_PLAYERS];
struct pad_stick pad_sticks_read[PAD_PLAYERS];
Sint8 pad_jump_held[GAME_PLAYERS];
Sint8 pad_action_held[GAME_PLAYERS];
Sint8 pad_jump_off[GAME_PLAYERS];
Sint8 pad_action_off[GAME_PLAYERS];
Sint8 pad_jump_pressed[GAME_PLAYERS];
Sint8 pad_action_pressed[GAME_PLAYERS];
Sint8 pad_jump_released[GAME_PLAYERS];
Sint8 pad_action_released[GAME_PLAYERS];
Uint32 pad_held[PAD_PORTS];
Uint32 pad_press_repeat[PAD_PORTS];
Uint32 pad_held_live[PAD_PORTS];
Uint32 pad_press_live[PAD_PORTS];
Sint8 pad_stick_pushed[PAD_PLAYERS];
Bool pad_port_used[PAD_PORTS];
Sint32 player_pad_ports[PAD_PORTS];
/* by port: the scans a direction has been held since its press, less
 * PAD_REPEAT_RATE for each repeat */
static Sint32 pad_repeat_scans[PAD_PORTS];
/* the device in port A's second socket, as pad_scan last found it */
static const PDS_PERIPHERALINFO *socket_info;

void pad_enable(void)
{
    game_state.pads_on = 1;
}

void pad_disable(void)
{
    game_state.pads_on = 0;
}

Bool pad_is_enabled(void)
{
    return game_state.pads_on;
}

void pad_enable_player(int player)
{
    /* @hack redundant: the else after this arm's return */
    if (player != 0 && player != 1) {
        game_state.player_pads_on[0] = 1;
        game_state.player_pads_on[1] = 1;
        return;
    } else {
        game_state.player_pads_on[player] = 1;
    }
}

void pad_disable_player(int player)
{
    /* @hack redundant: the else after this arm's return */
    if (player != 0 && player != 1) {
        game_state.player_pads_on[0] = 0;
        game_state.player_pads_on[1] = 0;
        return;
    } else {
        game_state.player_pads_on[player] = 0;
    }
}

Bool pad_player_is_enabled(int player)
{
    return game_state.player_pads_on[player];
}

void pad_clear_reset(void)
{
    reset_pressed = 0;
    reset_held_scans = 0;
}

Bool pad_reset_pressed(int unused)
{
    if (game_state.card_state == CARD_SAVING) {
        return 0;
    }
    return reset_pressed;
}

/* set reset_pressed, for one scan, once RESET_SCANS scans in a row have
 * found a pad holding all of RESET_BUTTONS, one of them just pressed */
static void pad_count_reset(void)
{
    Sint8 held = 0;

    if ((player_pads[0]->on & RESET_BUTTONS) == RESET_BUTTONS &&
        (player_pads[0]->press & RESET_BUTTONS)) {
        held = 1;
    }
    if (game_state.two_players != 0 &&
        (player_pads[1]->on & RESET_BUTTONS) == RESET_BUTTONS &&
        (player_pads[1]->press & RESET_BUTTONS)) {
        held = 1;
    }
    if (held == 1) {
        reset_held_scans++;
    } else {
        reset_held_scans = 0;
    }
    if (reset_held_scans >= RESET_SCANS) {
        reset_pressed = 1;
        reset_held_scans = 0;
    } else {
        reset_pressed = 0;
    }
}

static void pad_read_buttons(int port)
{
    Uint32 sticks = 0;

    if (player_pads[port] == NULL) {
        return;
    }
    if (player_pads[port]->y1 > PAD_STICK_PRESS) {
        sticks |= PDD_DGT_KD;
    }
    if (player_pads[port]->y1 < -PAD_STICK_PRESS) {
        sticks |= PDD_DGT_KU;
    }
    if (player_pads[port]->x1 > PAD_STICK_PRESS) {
        sticks |= PDD_DGT_KR;
    }
    if (player_pads[port]->x1 < -PAD_STICK_PRESS) {
        sticks |= PDD_DGT_KL;
    }
    pad_press_repeat[port] = ~pad_held[port] & sticks;
    pad_held[port] = sticks;
    pad_held[port] |= player_pads[port]->on;
    pad_press_repeat[port] |= player_pads[port]->press;
    if (pad_press_repeat[port] & PAD_DIRECTIONS) {
        pad_repeat_scans[port] = 0;
    } else if (pad_held[port] & PAD_DIRECTIONS) {
        if (++pad_repeat_scans[port] > PAD_REPEAT_DELAY) {
            pad_repeat_scans[port] -= PAD_REPEAT_RATE;
            pad_press_repeat[port] |= pad_held[port] & PAD_DIRECTIONS;
        }
    } else {
        pad_repeat_scans[port] = 0;
    }
    if (game_state.pads_on == 0) {
        pad_held_live[port] = 0;
        pad_press_live[port] = 0;
    } else {
        pad_held_live[port] = pad_held[port];
        pad_press_live[port] = pad_press_repeat[port];
    }
}

Bool pad_player_is_connected(int player)
{
    if (player == 0) {
        return player_pad_ports[0] != NO_PAD;
    }
    if (player == 1) {
        return player_pad_ports[1] != NO_PAD;
    }
    if (player_pad_ports[0] != NO_PAD && player_pad_ports[1] != NO_PAD) {
        return TRUE;
    }
    return FALSE;
}

void pad_scan(void)
{
    int pads = 0;
    int i;

    for (i = 0; i < PAD_PORTS; i++) {
        player_pads[i] = pdGetPeripheral(scan_ports[i]);
        if ((player_pads[i]->support & PAD_SUPPORT) == PAD_SUPPORT) {
            pad_port_used[i] = TRUE;
            player_pad_ports[i] = i;
            pads++;
        } else {
            pad_port_used[i] = FALSE;
            player_pads[i] = pad_none;
            player_pad_ports[i] = NO_PAD;
        }
    }
    if (pads == 0) {
        player_pads[0] = pad_none;
    }
    if (game_state.pads_on == 1) {
        for (i = 0; i < PAD_PLAYERS; i++) {
            if (i == 0 && game_state.player_pads_on[0] == 1) {
                player_pad_records[0] = *player_pads[0];
            } else if (i == 1 && game_state.player_pads_on[1] == 1) {
                player_pad_records[1] = *player_pads[1];
            } else {
                player_pad_records[i].old = player_pad_records[i].on;
                player_pad_records[i].on = 0;
                player_pad_records[i].off = IDLE_OFF;
                player_pad_records[i].press = 0;
                player_pad_records[i].release = 0;
                player_pad_records[i].x1 = 0;
                player_pad_records[i].y1 = 0;
                player_pad_records[i].l = IDLE_TRIGGER;
                player_pad_records[i].r = IDLE_TRIGGER;
                player_pad_records[i].x2 = 0;
                player_pad_records[i].y2 = 0;
            }
        }
    } else {
        for (i = 0; i < PAD_PLAYERS; i++) {
            player_pad_records[i].old = player_pad_records[i].on;
            player_pad_records[i].on = 0;
            player_pad_records[i].off = IDLE_OFF;
            player_pad_records[i].press = 0;
            player_pad_records[i].release = 0;
            player_pad_records[i].x1 = 0;
            player_pad_records[i].y1 = 0;
            player_pad_records[i].l = IDLE_TRIGGER;
            player_pad_records[i].r = IDLE_TRIGGER;
            player_pad_records[i].x2 = 0;
            player_pad_records[i].y2 = 0;
        }
    }
    pad_count_reset();
    pad_read_sticks();
    pad_run_demo();
    pad_read_actions();
    for (i = 0; i < PAD_PORTS; i++) {
        pad_read_buttons(i);
    }
    socket_info = pdGetPeripheralInfo(PDD_PORT_A2);
    if (socket_info->type == PAD_CARD_TYPE &&
        socket_info->max_pow > PAD_CARD_MAX_POW) {
        save_data.status |= SAVE_CARD_MARK;
    }
}

/* fill pad_sticks_read, pad_sticks and pad_stick_pushed from the players'
 * records */
static void pad_read_sticks(void)
{
    int i;
    Float last_stroke;
    Float x;
    Float y;
    Float squared;
    Float stroke;
    Angle ang;
    PDS_PERIPHERAL *record;

    for (i = 0; i < PAD_PLAYERS; i++) {
        last_stroke = pad_sticks[i].stroke;
        /* @hack redundant: a test of i that never runs */
        if (0) {
            if (i) {
            }
        }
        if (i < GAME_PLAYERS) {
            record = &player_pad_records[i];
            /* @hack type: x1 and y1 cast to int, where their conversion to
             * Float alone would do */
            x = (int)record->x1;
            y = (int)record->y1;
            /* @hack redundant: a test of record that never runs */
            if (0) {
                if (record) {
                }
            }
        } else {
            x = y = 0.0F;
        }
        if (x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
            y < -PAD_DEAD_ZONE) {
            if (x >= 0.0F) {
                x -= PAD_DEAD_ZONE;
            } else {
                x += PAD_DEAD_ZONE;
            }
            if (y >= 0.0F) {
                y -= PAD_DEAD_ZONE;
            } else {
                y += PAD_DEAD_ZONE;
            }
            if (camera_task_work == NULL) {
                ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN);
            } else if (i < camera_active_count) {
                ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) -
                      camera_views[i]->ang.y;
            } else {
                ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) -
                      camera_views[0]->ang.y;
            }
            squared = (x * x) + (y * y);
            stroke =
                squared * njSqrt(squared) *
                (1.0F / (PAD_STICK_REACH * PAD_STICK_REACH * PAD_STICK_REACH));
            /* @hack redundant: a store to squared that never runs */
            if (0) {
                squared = stroke;
            }
            /* @hack redundant: a test of squared that never runs */
            if (0) {
                if ((int)squared) {
                }
            }
            if (stroke > 1.0F) {
                stroke = 1.0F;
            }
        } else {
            ang = 0;
            stroke = 0.0F;
        }
        ang = (Uint16)ang;
        pad_sticks_read[i].ang = ang;
        pad_sticks_read[i].stroke = stroke;
#ifdef NONMATCHING
        /* this side repairs the tag below: game_state.player_pads_on holds a
         * flag for players 0 and 1 alone, and it reads no other */
        if (i < GAME_PLAYERS && game_state.pads_on == 1 &&
            game_state.player_pads_on[i] == 1) {
#else
        /* @bug memory: for players 2 to 7, the test reads
         * game_state.player_pads_on[i] past its two bytes, by the MOV.B
         * @(R0,R14),R0 at 0x8c029ac8, 0x8c029c82, 0x8c029e6e and 0x8c02a034.
         * the bytes it reads are game_state's rank to trial_screen. their
         * sticks are zeros either way */
        if (game_state.pads_on == 1 && game_state.player_pads_on[i] == 1) {
#endif
            pad_sticks[i].ang = ang;
            pad_sticks[i].stroke = stroke;
        } else {
            pad_sticks[i].ang = 0;
            pad_sticks[i].stroke = 0.0F;
        }
        if (pad_sticks[i].stroke >= PAD_STICK_PUSHED &&
            last_stroke < PAD_STICK_PUSHED) {
            pad_stick_pushed[i] = 1;
        } else {
            pad_stick_pushed[i] = 0;
        }
        /* @hack redundant: an if and an else that hold nothing */
        if (0) {
        } else {
        }
    }
    /* @hack redundant: a test of the camera task's work and a call of njSqrt
     * that never run */
    if (0) {
        if (camera_task_work) {
            njSqrt(PAD_DEAD_ZONE);
        }
    }
}

/* under DEMO_ON, give players 0 and 1 the demo's frame for this step. under
 * DEMO_RECORD, write the frame from what they hold */
static void pad_run_demo(void)
{
    struct pad_demo_frame *frame;
    int i;

    if (game_state.demo == 0) {
        return;
    }
    /* @hack spelling: frame stepped to the step's frame in a statement of its
     * own, where the one that sets it would do */
    frame = (struct pad_demo_frame *)DEMO_AREA;
    frame += game_state.steps;
    if (game_state.demo == DEMO_ON) {
        for (i = 0; i < GAME_PLAYERS; i++) {
            player_pad_records[i].old = player_pad_records[i].on;
            if (i == 0 && game_state.player_pads_on[0] == 1) {
                player_pad_records[0].on = frame->on[0];
                player_pad_records[0].off = ~frame->on[0];
                player_pad_records[0].press = frame->press[0];
                /* @bug logic: release marks every button whose on differs
                 * from old, so a button just pressed counts as released too.
                 * XOR R2,R3 at 0x8c02a108, and at 0x8c02a182 for player 1,
                 * leaves out the AND with off that keeps the peripheral
                 * server's release to the buttons that went up */
                player_pad_records[0].release =
                    ~player_pad_records[0].old ^ player_pad_records[0].off;
                player_pad_records[0].x1 = frame->x1[0];
                player_pad_records[0].y1 = frame->y1[0];
            } else if (i == 1 && game_state.player_pads_on[1] == 1) {
                player_pad_records[1].on = frame->on[1];
                player_pad_records[1].off = ~frame->on[1];
                player_pad_records[1].press = frame->press[1];
                player_pad_records[1].release =
                    ~player_pad_records[1].old ^ player_pad_records[1].off;
                player_pad_records[1].x1 = frame->x1[1];
                player_pad_records[1].y1 = frame->y1[1];
            }
            player_pad_records[i].l = IDLE_TRIGGER;
            player_pad_records[i].r = IDLE_TRIGGER;
            player_pad_records[i].x2 = 0;
            player_pad_records[i].y2 = 0;
        }
        pad_sticks[0].stroke = frame->stick[0].stroke;
        pad_sticks[1].stroke = frame->stick[1].stroke;
        pad_sticks[0].ang = frame->stick[0].ang;
        pad_sticks[1].ang = frame->stick[1].ang;
        pad_stick_pushed[0] = frame->pushed[0];
        pad_stick_pushed[1] = frame->pushed[1];
    } else if (game_state.demo == DEMO_RECORD) {
        frame->on[0] = player_pad_records[0].on;
        frame->on[1] = player_pad_records[1].on;
        frame->press[0] = player_pad_records[0].press;
        frame->press[1] = player_pad_records[1].press;
        frame->x1[0] = player_pad_records[0].x1;
        frame->x1[1] = player_pad_records[1].x1;
        frame->y1[0] = player_pad_records[0].y1;
        frame->y1[1] = player_pad_records[1].y1;
        frame->pushed[0] = pad_stick_pushed[0];
        frame->pushed[1] = pad_stick_pushed[1];
        frame->stick[0].stroke = pad_sticks[0].stroke;
        frame->stick[1].stroke = pad_sticks[1].stroke;
        frame->stick[0].ang = pad_sticks[0].ang;
        frame->stick[1].ang = pad_sticks[1].ang;
    }
}

/* set the jump and action flags of players 0 and 1 from their records */
static void pad_read_actions(void)
{
    int i;
    int was_held;

    for (i = 0; i < GAME_PLAYERS; i++) {
        pad_jump_held[i] = 0;
        pad_jump_pressed[i] = 0;
        /* @bug logic: pad_jump_off is 1 whether or not the record's off
         * holds a jump button. MOV.B R4,@(R0,R5) at 0x8c02a278 starts it at
         * the 1 that the test of off stores at 0x8c02a2aa. pad_action_off
         * starts at 1 the same way, at 0x8c02a2d0, for its test's store at
         * 0x8c02a302 */
        pad_jump_off[i] = 1;
        pad_jump_released[i] = 0;
        if (player_pad_records[i].on & pad_jump_buttons) {
            pad_jump_held[i] = 1;
        }
        if (player_pad_records[i].press & pad_jump_buttons) {
            pad_jump_pressed[i] = 1;
        }
        if (player_pad_records[i].off & pad_jump_buttons) {
            pad_jump_off[i] = 1;
        }
        if (player_pad_records[i].release & pad_jump_buttons) {
            pad_jump_released[i] = 1;
        }
        was_held = pad_action_held[i];
        pad_action_held[i] = 0;
        pad_action_pressed[i] = 0;
        pad_action_off[i] = 1;
        pad_action_released[i] = 0;
        if (player_pad_records[i].on & pad_action_buttons) {
            pad_action_held[i] = 1;
        }
        if ((player_pad_records[i].press & pad_action_buttons) &&
            was_held != 1) {
            pad_action_pressed[i] = 1;
        }
        if (player_pad_records[i].off & pad_action_buttons) {
            pad_action_off[i] = 1;
        }
        if (player_pad_records[i].release & pad_action_buttons) {
            pad_action_released[i] = 1;
        }
    }
}

void pad_share_first(void)
{
    if (game_state.player_pads_on[1] == 1) {
        player_pad_records[1] = *player_pads[0];
        pad_read_sticks();
        pad_read_actions();
    }
}
