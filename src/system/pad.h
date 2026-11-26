/* the pads: the scan of the four ports, and what the game reads of it */
#ifndef SYSTEM_PAD_H
#define SYSTEM_PAD_H
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/game_state.h"

/* the console's ports, A to D */
#define PAD_PORTS 4
/* the players pad_scan keeps a record and a stick for, one for each of
 * player/player.h's PLAYER_SLOTS */
#define PAD_PLAYERS 8
/* what pad_enable_player, pad_disable_player and pad_player_is_connected
 * take for both players. any number but 0 and 1 means the same */
#define PAD_BOTH_PLAYERS 3

/* the d-pad's four directions, as PDD_DGT_ bits */
#define PAD_DIRECTIONS (PDD_DGT_KU | PDD_DGT_KD | PDD_DGT_KL | PDD_DGT_KR)

/* a stick as the game reads it, from a record's x1 and y1. ang and stroke are 0
 * while both axes lie within PAD_DEAD_ZONE of center.
 *
 * otherwise each axis first moves PAD_DEAD_ZONE toward 0, past it where the
 * axis lay within PAD_DEAD_ZONE. ang is then the direction of the two axes,
 * from 0 to 0xffff. while camera_task_work isn't null, ang is taken less the
 * ang.y of the player's camera. a player at or past camera_active_count takes
 * camera 0's. stroke is the cube of the axes' length over the cube of
 * PAD_STICK_REACH, and at most 1 */
struct pad_stick {
    Angle ang;
    Float stroke;
};
/* a record's x1 or y1 at its most, and the stick's dead zone in the same units.
 * PAD_STICK_REACH is how far the stick reaches past it */
#define PAD_STICK_MAX 127.0F
#define PAD_DEAD_ZONE 12.0F
#define PAD_STICK_REACH (PAD_STICK_MAX - PAD_DEAD_ZONE)

/* a demo's frame: what players 0 and 1 hold at one step. a stage's demo file
 * holds a frame for each of game_state.steps, from 0. on, press, x1 and y1 copy
 * those of the players' records, pushed their pad_stick_pushed and stick their
 * pad_sticks */
struct pad_demo_frame {
    Uint32 on[GAME_PLAYERS];
    Uint32 press[GAME_PLAYERS];
    Sint16 x1[GAME_PLAYERS];
    Sint16 y1[GAME_PLAYERS];
    Sint8 pushed[GAME_PLAYERS];
    struct pad_stick stick[GAME_PLAYERS];
};

/* the buttons each use takes, as PDD_DGT_ masks. action takes B or X, jump A,
 * choose A or Start, cancel B, and grab B, X or Y. next action takes Y: it
 * steps a player's action window to the player's next action */
extern Uint32 pad_action_buttons;
extern Uint32 pad_jump_buttons;
extern Uint32 pad_choose_buttons;
/* pad_choose_buttons as a Sint32, for kart_race.c's reads, which the hack at
 * KART_CHOOSE_BUTTONS covers. symbols.txt places this second name at the
 * mask's address */
extern Sint32 pad_choose_buttons_signed;
extern Uint32 pad_cancel_buttons;
/* pad_cancel_buttons as a Sint32, as pad_choose_buttons_signed is */
extern Sint32 pad_cancel_buttons_signed;
extern Uint32 pad_grab_buttons;
extern Uint32 pad_next_action_buttons;

/* SHC lays out a source's bss in the order it first meets the names. the
 * declarations from player_pad_records to player_pad_ports set the order of
 * pad.c's bss, and pad.c's two statics follow them */

/* each player's input, as pad_scan leaves it. while game_state.pads_on and
 * their own game_state.player_pads_on are 1, players 0 and 1 copy
 * *player_pads[0] and *player_pads[1]. every other record is idle. an idle
 * record holds no button, and marks none pressed or released. its old keeps
 * what its on held. its axes are 0, and its l and r are half their range.
 *
 * under DEMO_ON, the scan then moves the records of players 0 and 1 on into
 * old. a player whose player_pads_on is 1 takes its buttons, x1 and y1 from the
 * demo's frame. both players take an idle record's l, r, x2 and y2 */
extern PDS_PERIPHERAL player_pad_records[PAD_PLAYERS];
/* each port's pad, A to D, as pdGetPeripheral gives its state. player 0 reads
 * port A's, and player 1 port B's. a port without a pad points at a record of
 * zeros. a pad is a device whose support holds all of pad.c's PAD_SUPPORT. that
 * is the face buttons, Start, both triggers and the first stick's axes */
extern const PDS_PERIPHERAL *player_pads[PAD_PORTS];
extern Uint8 dat_8c24f098[20];
/* each player's stick. pad_sticks_read is read from the player's record
 * before a demo's frame replaces it. pad_sticks is the same while
 * game_state.pads_on and the player's player_pads_on are 1, and zeros
 * otherwise. under DEMO_ON, players 0 and 1 take pad_sticks from the demo's
 * frame. players from GAME_PLAYERS on read no stick, so theirs are
 * zeros */
extern struct pad_stick pad_sticks[PAD_PLAYERS];
extern struct pad_stick pad_sticks_read[PAD_PLAYERS];
/* by player: 1 where the player's record holds one of the buttons, and 0
 * otherwise. a jump flag reads pad_jump_buttons, and an action flag
 * pad_action_buttons. held reads the record's on, pressed its press and
 * released its release. pad_action_pressed stays 0 where pad_action_held was 1
 * at the scan before. a press of one action button while another is held
 * doesn't count. pad_jump_off and pad_action_off are 1 after every scan */
extern Sint8 pad_jump_held[GAME_PLAYERS];
extern Sint8 pad_action_held[GAME_PLAYERS];
extern Sint8 pad_jump_off[GAME_PLAYERS];
extern Sint8 pad_action_off[GAME_PLAYERS];
extern Sint8 pad_jump_pressed[GAME_PLAYERS];
extern Sint8 pad_action_pressed[GAME_PLAYERS];
extern Sint8 pad_jump_released[GAME_PLAYERS];
extern Sint8 pad_action_released[GAME_PLAYERS];
/* each port's buttons as PDD_DGT_ masks, read from player_pads and not from
 * the players' records. pad_held_live and pad_press_live copy pad_held and
 * pad_press_repeat while game_state.pads_on isn't 0, and are 0 otherwise.
 *
 * pad_held is the pad's on, with the directions the first stick holds. an
 * axis more than PAD_STICK_PRESS from center holds its direction.
 *
 * pad_press_repeat is the pad's press, with each direction the stick holds
 * that pad_held didn't at the scan before. a direction held longer than
 * PAD_REPEAT_DELAY scans is pressed again then, and again every
 * PAD_REPEAT_RATE scans after */
extern Uint32 pad_held[PAD_PORTS];
extern Uint32 pad_press_repeat[PAD_PORTS];
extern Uint32 pad_held_live[PAD_PORTS];
extern Uint32 pad_press_live[PAD_PORTS];
#define PAD_STICK_PRESS 78
#define PAD_REPEAT_DELAY 30
#define PAD_REPEAT_RATE 6
/* by player: 1 after a scan in which the stroke of the player's pad_sticks
 * reaches PAD_STICK_PUSHED from below. it is 0 otherwise. under DEMO_ON,
 * players 0 and 1 take it from the demo's frame */
extern Sint8 pad_stick_pushed[PAD_PLAYERS];
#define PAD_STICK_PUSHED 0.5F
/* TRUE for each port that has a pad */
extern Bool pad_port_used[PAD_PORTS];
/* by port: the port's number where it has a pad, and NO_PAD where it has none.
 * ports A to D are 0 to 3. player 0 reads port A's, and player 1 port B's */
#define NO_PAD (-1)
extern Sint32 player_pad_ports[PAD_PORTS];

/* let the pads drive the game, or stop them: set game_state.pads_on to 1 or
 * to 0. pad_is_enabled returns it */
void pad_enable(void);
void pad_disable(void);
Bool pad_is_enabled(void);
/* let player 0's or 1's own pad drive the player, or stop it. each sets the
 * player's game_state.player_pads_on to 1 or to 0. PAD_BOTH_PLAYERS sets both
 * players'. pad_player_is_enabled returns the player's, and player must be 0 or
 * 1 */
void pad_enable_player(int player);
void pad_disable_player(int player);
Bool pad_player_is_enabled(int player);
/* make pad_reset_pressed return 0 until the next pad_scan */
void pad_clear_reset(void);
/* 1 after a pad_scan that finds a pad holding the reset buttons, one just
 * pressed, and 0 otherwise. the reset buttons are A, B, X, Y and Start. the
 * scan reads port A's pad, and port B's too in a two-player game. while
 * game_state.card_state is CARD_SAVING, it returns 0. unused is ignored */
Bool pad_reset_pressed(int unused);
/* for player 0 or 1, TRUE where player_pad_ports[player] isn't NO_PAD. for
 * PAD_BOTH_PLAYERS, TRUE where neither player's is */
Bool pad_player_is_connected(int player);
/* point player_pads at the ports' states, and fill the players' records and
 * sticks from them. under DEMO_ON, players 0 and 1 then take the frame of
 * DEMO_AREA for game_state.steps. under DEMO_RECORD, that frame takes what they
 * hold instead. the jump and action flags come from the records after the
 * frame. the reset and the port masks come from player_pads.
 *
 * the scan also sets SAVE_CARD_MARK in save_data.status for a device in port
 * A's second socket. the device's functions must be PAD_CARD_TYPE's alone, and
 * its max_pow above PAD_CARD_MAX_POW */
void pad_scan(void);
/* the functions of a memory card with a screen and a clock */
#define PAD_CARD_TYPE                                                          \
    (PDD_DEVTYPE_STORAGE | PDD_DEVTYPE_LCD | PDD_DEVTYPE_TIMER)
#define PAD_CARD_MAX_POW 40000
/* while player 1's player_pads_on is 1, copy *player_pads[0] into player 1's
 * record. then read the sticks and the jump and action flags again. otherwise
 * do nothing. the second read measures against pad_scan's, so player 0's
 * pad_stick_pushed and pad_action_pressed end at 0. player 1's then compare
 * port A's pad with port B's at the scan */
void pad_share_first(void);

#endif
