/* the Finalhazard's fight: its textures, random numbers, knockback, test
 * menu and players */
#ifndef BOSS_LAST2_ARENA_H
#define BOSS_LAST2_ARENA_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* a pointer may be null only where a comment says so */

/* the arena's center, where the Finalhazard stands and the camera's motions
 * play */
extern const NJS_POINT3 finalhazard_center;
extern NJS_TEXLIST finalhazard_effect_texlist;
extern NJS_TEXLIST finalhazard_model_texlist;
extern NJS_TEXLIST finalhazard_title_texlist;

/* the players who take turns at the fight. player 0 is Super Sonic, and
 * player 1 Super Shadow. super.c's manager allocates the record */
#define FINALHAZARD_PLAYERS 2
struct finalhazard_players {
    Sint8 active; /* the player in play */
    Sint8 transition; /* the switch's phase, a finalhazard_transition */
    Sint8 turns[FINALHAZARD_PLAYERS]; /* the turns each has ended, up to 127 */
    /* 1 where a hit on the Finalhazard calls finalhazard_switch_players,
     * which clears it */
    Sint8 hit_switch;
    Sint8 hits; /* the switches that followed a hit */
    Sint8 radio_message; /* the radio's timed messages played so far */
    Sint8 unused_07;
    Sint16 switch_timer; /* the steps since transition last changed */
    /* the steps of play since the active player last lost a ring */
    Sint16 ring_timer;
    /* the rings the two players hold between them. each ring the active
     * player loses goes to the other */
    Sint16 ring_budget;
};
/* a switch's phases. a switch fades to black and hands the fight to the
 * other player. it then fades in and waits before play goes on.
 * SWITCH_LOST follows the active player's last ring */
enum finalhazard_transition {
    SWITCH_PLAY,
    SWITCH_FADE_OUT,
    SWITCH_FADE_IN,
    SWITCH_WAIT,
    SWITCH_LOST
};

/* the manager's record, or null without the manager */
extern struct finalhazard_players *finalhazard_players;
/* show the spheres' settings under the test menu. the active player's Y
 * picks one, and L and R lower or raise it */
void finalhazard_debug_spheres(void);
/* begin to hand the fight from the active player to the other.
 * finalhazard_players must not be null. transition becomes SWITCH_FADE_OUT,
 * and the pause and both pads stop. the leaving player's voice plays: its
 * first turn's, a hit's, or else its usual one. it plays only where no voice
 * of another priority plays, before the clear and the winner's pose.
 *
 * where hit_switch is 1, the call counts the hit. it sets ring_budget to 50,
 * or to 40 from the fourth hit on. it tops the other player's rings up to
 * ring_budget. the leaving player loses every ring and takes its hit pose.
 * last the call counts the leaving player's turn, and clears hit_switch */
void finalhazard_switch_players(void);

/* the lasers finalhazard_fire_lasers fires: count beams, 1 or 2, of kind.
 * of two, one starts at first, and the other at second where second isn't
 * null. a lone beam starts at first where second is null. else target must be
 * the weak point's task, and the beam starts at the cannon
 * finalhazard_panel_cannon gives. each beam aims at a point on the way from
 * the active player to target's pos */
struct finalhazard_attack {
    Sint8 kind;
    Sint8 count;
    NJS_POINT3 *first;
    NJS_POINT3 *second;
    struct task *target;
};
/* finalhazard_attack's kinds. a pulse's beams reach out, hold and draw back,
 * again and again. a sweep's reach out once and circle their aim until
 * finalhazard_lasers_retract */
enum finalhazard_laser_kind {
    FINALHAZARD_LASER_PULSE,
    FINALHAZARD_LASER_SWEEP
};

/* load or release the texture lists this header names, and the guard
 * flare's defend_texlist */
void finalhazard_textures_load(void);
void finalhazard_textures_release(void);
/* returns 0. no code calls it */
int finalhazard_arena_status(void);

/* step the generator's state as rand() does, and seed rand() with it */
void finalhazard_seed_random(void);
/* a random value from -radius up to radius */
#define FINALHAZARD_SCATTER(radius)                                            \
    ((radius) - ((Float)rand() * (radius) * (2.0F * RAND_SCALE)))

/* give the active player a speed. finalhazard_players must not be null.
 * direction may be null. with a direction, the speed is direction scaled to a
 * length of speed, with direction's own y. without one, x is 0 and z is speed
 * for player 0, or -speed for player 1. y is then -2 above a height of -100,
 * else 2 */
void finalhazard_push_player(const NJS_VECTOR *direction, Float speed);
/* the speeds the fight pushes the active player at. a beam, the weak point or
 * a harmless touch of the body pushes it at FINALHAZARD_PUSH_SPEED. the body
 * or a sphere that hurts it pushes it at FINALHAZARD_KNOCKBACK */
#define FINALHAZARD_PUSH_SPEED 10.0F
#define FINALHAZARD_KNOCKBACK 5.0F

/* the tests finalhazard_set_test starts, in the order the menu steps through
 * them. FINALHAZARD_TEST_SPHERES and FINALHAZARD_TEST_CANNON start none: their
 * pages show the spheres' and the lasers' settings */
enum finalhazard_test {
    FINALHAZARD_TEST_OFF,
    FINALHAZARD_TEST_POSE, /* the Finalhazard waits, its attacks ended */
    FINALHAZARD_TEST_SPHERES,
    FINALHAZARD_TEST_CANNON,
    /* the attacks end, and the weak point shows in each of its slots */
    FINALHAZARD_TEST_WEAK,
    /* the Finalhazard waits, with FINALHAZARD_DOUBLE_SWEEP's lasers firing */
    FINALHAZARD_TEST_LASERS,
    FINALHAZARD_TESTS
};

/* set finalhazard_test_mode to test, and start the test on task, the
 * Finalhazard's. a test that starts none sets it back to FINALHAZARD_TEST_OFF.
 * returns finalhazard_test_mode */
Sint8 finalhazard_set_test(struct task *task, Sint8 test);
/* step the test menu, which shows in player 0's pause. Left steps through
 * the tests, and Start runs the test shown on task, the Finalhazard's */
void finalhazard_debug_menu(struct task *task);
/* the test running, or FINALHAZARD_TEST_OFF */
extern Sint8 finalhazard_test_mode;
/* 1 once FINALHAZARD_TEST_POSE or FINALHAZARD_TEST_LASERS starts, and 0
 * once a test that starts none does. while it is 1 the active player loses
 * no rings */
extern int finalhazard_debug_attack;

/* the test menu's layout. its title and tests print in column
 * FINALHAZARD_MENU_COLUMN. a page prints setting n in that column, in row
 * FINALHAZARD_MENU_FIRST_ROW + n. the selected setting prints in
 * FINALHAZARD_MENU_SELECTED, and the rest in FINALHAZARD_MENU_TEXT */
#define FINALHAZARD_MENU_COLUMN 20
#define FINALHAZARD_MENU_FIRST_ROW 7
#define FINALHAZARD_SETTING_LINE(setting)                                      \
    NJM_LOCATION(FINALHAZARD_MENU_COLUMN,                                      \
                 (setting) + FINALHAZARD_MENU_FIRST_ROW)
#define FINALHAZARD_MENU_SELECTED 0xffffff00
#define FINALHAZARD_MENU_TEXT 0xffffffff
/* a page's L or R held changes the setting once every
 * FINALHAZARD_MENU_REPEAT + 1 steps. each moves a speed by
 * FINALHAZARD_MENU_SPEED_STEP. L lowers it only while it is above 0. R
 * raises it only while it is below FINALHAZARD_MENU_TOP_SPEED */
#define FINALHAZARD_MENU_REPEAT 5
#define FINALHAZARD_MENU_SPEED_STEP 0.01F
#define FINALHAZARD_MENU_TOP_SPEED 5.0F
#endif
