#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/body.h"
#include "boss_last2/joints.h"
#include "boss_last2/laser.h"
#include "shared/last_boss_defend.h"
#include "boss_last2/arena.h"
#include "boss_last2/panels.h"
#include "collision/collision.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/texture.h"

/* the step of rand(), the C standard's sample generator */
#define MULTIPLIER 1103515245
#define INCREMENT 12345
/* a push without a direction moves the player down at PUSH_CLIMB above
 * PUSH_HEIGHT, and else up */
#define PUSH_HEIGHT (-100.0F)
#define PUSH_CLIMB 2.0F
/* selected_test outside player 0's pause, so that the menu opens on the
 * test running */
#define MENU_CLOSED (-1)
/* where the menu prints its title, and the test it shows */
#define TITLE_LINE NJM_LOCATION(FINALHAZARD_MENU_COLUMN, 2)
#define TEST_LINE NJM_LOCATION(FINALHAZARD_MENU_COLUMN, 3)

int finalhazard_debug_attack = 0;
Sint8 finalhazard_test_mode;
static Uint32 random_state;
static Sint8 selected_test;

void finalhazard_textures_load(void)
{
    texture_load_pvm("modeltex_bosslast2", &finalhazard_model_texlist);
    texture_load_pvm("efftex_bosslast2", &finalhazard_effect_texlist);
    texture_load_pvm("efftex2_bosslast1", &defend_texlist);
    texture_load_pvm("titletex_bosslast2", &finalhazard_title_texlist);
}

void finalhazard_textures_release(void)
{
    njReleaseTexture(&finalhazard_model_texlist);
    njReleaseTexture(&finalhazard_effect_texlist);
    njReleaseTexture(&defend_texlist);
    njReleaseTexture(&finalhazard_title_texlist);
}

int finalhazard_arena_status(void)
{
    return 0;
}

#define ARENA_CENTER finalhazard_center
#define ARENA_BOSS finalhazard_task
#include "shared/last_boss_orbit.h"

void finalhazard_seed_random(void)
{
    Uint32 state = random_state;
    state *= MULTIPLIER;
    state += INCREMENT;
    srand(state);
    random_state = state;
}

#include "shared/last_boss_aim.h"

int last_boss_hit_penetrates(struct task *task, const struct collision_hit *hit)
{
    if (hit && task) {
        if (collision_get_attack(task, hit->own_shape) >
            collision_get_guard(hit->task, hit->hit_shape)) {
            return 1;
        }
    }
    return 0;
}

void finalhazard_push_player(const NJS_VECTOR *direction, Float speed)
{
    NJS_VECTOR scaled;
    Float y;
    if (!direction) {
        struct finalhazard_players *players = finalhazard_players;
        Float z;
        if (players->active == 0) {
            z = speed;
        } else {
            z = -speed;
        }
        if (player_work[players->active]->pos.y > PUSH_HEIGHT) {
            y = -PUSH_CLIMB;
        } else {
            y = PUSH_CLIMB;
        }
        player_set_speed(players->active, 0.0F, y, z);
    } else {
        scaled = *direction;
        y = scaled.y;
        last_boss_vector_length(&scaled, speed, &scaled);
        player_set_speed(finalhazard_players->active, scaled.x, y, scaled.z);
    }
}

static void test_pose(struct task *task)
{
    struct task_work *work = task->work;
    finalhazard_clear_attacks(task);
    finalhazard_debug_attack = 1;
    work->mode = FINALHAZARD_WAIT;
    work->submode = 0;
}

static void test_lasers(struct task *task)
{
    struct task_work *work = task->work;
    struct finalhazard_attack attack;
    struct task *target;
    finalhazard_clear_attacks(task);
    target = finalhazard_weak_point();
    attack.kind = FINALHAZARD_LASER_SWEEP;
    attack.count = 2;
    attack.first = &finalhazard_joint_positions[FINALHAZARD_JOINT_CANNON_A];
    attack.second = &finalhazard_joint_positions[FINALHAZARD_JOINT_CANNON_B];
    attack.target = target;
    finalhazard_fire_lasers(task, &attack);
    finalhazard_debug_attack = 1;
    work->mode = FINALHAZARD_WAIT;
    work->submode = 0;
}

Sint8 finalhazard_set_test(struct task *task, Sint8 test)
{
    /* @hack spelling: test copied to mode, where test would do */
    Sint8 mode = test;
    finalhazard_test_mode = mode;
    switch (mode) {
    /* @hack spelling: the three cases that start no test each with a body of
     * its own, where one body for the three would do */
    case FINALHAZARD_TEST_OFF:
        finalhazard_test_mode = FINALHAZARD_TEST_OFF;
        finalhazard_debug_attack = 0;
        break;
    case FINALHAZARD_TEST_POSE:
        test_pose(task);
        break;
    case FINALHAZARD_TEST_WEAK: {
        /* @hack spelling: task copied to parent, where task would do */
        struct task *parent = task;
        finalhazard_clear_attacks(parent);
        /* @bug mismatched-call: the call at 0x8c5062e4 passes the active
         * player too, and finalhazard_show_weak_points at 0x8c507198 takes
         * its parent alone */
#ifdef NONMATCHING
        /* this side keeps the tag's result, as finalhazard_show_weak_points
         * reads no player. it calls the function by its own name. only
         * symbols.txt places the second name, which this side's objects don't
         * define */
        finalhazard_show_weak_points(parent);
#else
        finalhazard_preview_for_player(parent, finalhazard_players->active);
#endif
        break;
    }
    case FINALHAZARD_TEST_LASERS:
        test_lasers(task);
        break;
    case FINALHAZARD_TEST_SPHERES:
        finalhazard_test_mode = FINALHAZARD_TEST_OFF;
        finalhazard_debug_attack = 0;
        break;
    case FINALHAZARD_TEST_CANNON:
        finalhazard_test_mode = FINALHAZARD_TEST_OFF;
        finalhazard_debug_attack = 0;
        break;
    }
    return finalhazard_test_mode;
}

static void select_test(struct task *task)
{
    if (player_pads[0]->press & PDD_DGT_ST) {
        if (selected_test != MENU_CLOSED &&
            selected_test != finalhazard_test_mode) {
            finalhazard_set_test(task, selected_test);
        }
    }
    if (selected_test == MENU_CLOSED) {
        selected_test = finalhazard_test_mode;
    }
    if (player_pads[0]->press & PDD_DGT_KL) {
        selected_test++;
    }
    if (selected_test == FINALHAZARD_TESTS) {
        selected_test = FINALHAZARD_TEST_OFF;
    }
}

void finalhazard_debug_menu(struct task *task)
{
    if (game_state.paused == PAUSED_BY_PLAYER_0) {
        select_test(task);
        if (selected_test != FINALHAZARD_TEST_OFF) {
            njPrint(TITLE_LINE, "LASTBOSS2 TEST");
        }
        switch (selected_test) {
        case FINALHAZARD_TEST_OFF:
            break;
        case FINALHAZARD_TEST_POSE:
            njPrint(TEST_LINE, "DEBUG...");
            break;
        case FINALHAZARD_TEST_WEAK:
            njPrint(TEST_LINE, "WEAK MODE");
            break;
        case FINALHAZARD_TEST_SPHERES:
            njPrint(TEST_LINE, "IKURA MODE");
            finalhazard_debug_spheres();
            break;
        case FINALHAZARD_TEST_CANNON:
            njPrint(TEST_LINE, "CANNON MODE");
            finalhazard_debug_lasers();
            break;
        case FINALHAZARD_TEST_LASERS:
            njPrint(TEST_LINE, "ALL DISP");
            break;
        }
    } else {
        selected_test = MENU_CLOSED;
    }
}
