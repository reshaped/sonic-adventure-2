#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <shinobi.h>
#include "boss_last1/body.h"
#include "boss_last1/arena.h"
#include "boss_last1/camera.h"
#include "boss_last1/energy.h"
#include "boss_last1/stage.h"
#include "boss_last1/sphere.h"
#include "collision/collision.h"
#include "player/player.h"
#include "shared/last_boss_defend.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/texture.h"

static Uint32 random_state;
static Sint8 selected_test;

#define ARENA_CENTER biolizard_arena_center
#define ARENA_BOSS biolizard_task
#define ARENA_REVERSED biolizard_reversed
#include "shared/last_boss_orbit.h"
#include "shared/last_boss_aim.h"

/* the step of rand(), the C standard's sample generator. each number is the
 * 15 bits of the state from bit RESULT_SHIFT */
#define MULTIPLIER 1103515245
#define INCREMENT 12345
#define RESULT_SHIFT 16

int biolizard_random(void)
{
    Uint32 state;
    if (!random_state) {
        /* @hack type: the seed converted through float, where rand() alone
         * gives the same value */
        random_state = rand() * 1.0F;
    }
    state = random_state;
    state *= MULTIPLIER;
    state += INCREMENT;
    /* @hack type: the narrowing to Sint16 before the mask, where the mask
     * alone keeps the same bits */
    state = (Sint16)(state >> RESULT_SHIFT) & RAND_MAX;
    random_state = state;
    return state;
}

void biolizard_seed_random(void)
{
    Uint32 state = random_state;
    state *= MULTIPLIER;
    state += INCREMENT;
    srand(state);
    random_state = state;
}

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

void biolizard_textures_load(void)
{
    texture_load_pvm("efftex_bosslast1", &biolizard_effect_texlist);
    texture_load_pvm("efftex2_bosslast1", &defend_texlist);
    texture_load_pvm("modeltex_bosslast1", &biolizard_model_texlist);
    texture_load_pvm("watertex_bosslast1", &biolizard_water_texlist);
    texture_load_pvm("titletex_bosslast1", &biolizard_title_texlist);
}

void biolizard_textures_release(void)
{
    njReleaseTexture(&biolizard_effect_texlist);
    njReleaseTexture(&defend_texlist);
    njReleaseTexture(&biolizard_model_texlist);
    njReleaseTexture(&biolizard_water_texlist);
    njReleaseTexture(&biolizard_title_texlist);
}

/* selected_test's value outside player 0's pause. the next pause sets
 * selected_test to the test running, so the menu opens on it */
#define MENU_CLOSED (-1)
/* where the menu prints its title, and the test it shows */
#define TITLE_LINE                                                             \
    NJM_LOCATION(BIOLIZARD_MENU_COLUMN, BIOLIZARD_MENU_TEST_ROW - 1)
#define TEST_LINE NJM_LOCATION(BIOLIZARD_MENU_COLUMN, BIOLIZARD_MENU_TEST_ROW)
/* the guard flare's height above player 0 */
#define DEFEND_HEIGHT 30.0F

static void test_reset(struct task *task)
{
    struct task_work *work = task->work;
    biolizard_clear_attacks();
    work->mode = BIOLIZARD_WALK;
    work->submode = 0;
    work->timer = BIOLIZARD_WALK_STEPS;
}

static void test_energy(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 attack;
    biolizard_clear_attacks();
    attack = biolizard_debug_energy_kind();
    switch (attack) {
    case BIOLIZARD_ENERGY_SLOW:
        attack = BIOLIZARD_ATTACK_SLOW_ENERGY;
        break;
    case BIOLIZARD_ENERGY_NORMAL:
        attack = BIOLIZARD_ATTACK_ENERGY;
        break;
    case BIOLIZARD_ENERGY_FAST:
        attack = BIOLIZARD_ATTACK_FAST_ENERGY;
        break;
    }
    biolizard_select_attack(attack);
    work->mode = BIOLIZARD_ENERGY_ATTACK;
    work->submode = 0;
}

static void test_energy_display(struct task *task)
{
    struct task_work *work = task->work;
    biolizard_clear_attacks();
    biolizard_energy_preview(task);
    work->mode = BIOLIZARD_RESTORE_CAMERA;
    biolizard_camera_restore();
}

static void test_motion(struct task *task)
{
    struct task_work *work = task->work;
    biolizard_clear_attacks();
    work->mode = BIOLIZARD_PREVIEW_MOTION;
    work->timer = 0;
    biolizard_camera_restore();
}

static void test_spheres(struct task *task)
{
    struct task_work *work = task->work;
    biolizard_clear_attacks();
    biolizard_select_attack(BIOLIZARD_ATTACK_WAVE);
    work->mode = BIOLIZARD_SPHERE_ATTACK;
    work->submode = 0;
}

static void test_glide(struct task *task)
{
    struct task_work *work = task->work;
    biolizard_clear_attacks();
    biolizard_select_attack(BIOLIZARD_ATTACK_LIFTED_WAVE);
    work->mode = BIOLIZARD_OFFER_RAIL;
    work->submode = 0;
}

static void test_defend(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    biolizard_clear_attacks();
    point = player_work[0]->pos;
    point.y += DEFEND_HEIGHT;
    last_boss_defend(&point, NULL);
    work->mode = BIOLIZARD_RESTORE_CAMERA;
    work->submode = 0;
}

Sint8 biolizard_set_test(struct task *task, Sint8 test)
{
    biolizard_test_mode = test;
    switch (test) {
    case BIOLIZARD_TEST_OFF:
        test_reset(task);
        break;
    case BIOLIZARD_TEST_ENERGY:
        test_energy(task);
        break;
    case BIOLIZARD_TEST_ENERGY_DISPLAY:
        test_energy_display(task);
        break;
    case BIOLIZARD_TEST_MOTION:
        test_motion(task);
        break;
    /* both start the sphere attack. the menu, as it shows
     * BIOLIZARD_TEST_WIND, has the sphere preview lift its spheres */
    case BIOLIZARD_TEST_SPHERES:
    case BIOLIZARD_TEST_WIND:
        test_spheres(task);
        break;
    case BIOLIZARD_TEST_GLIDE:
        test_glide(task);
        break;
    case BIOLIZARD_TEST_DEFEND:
        test_defend(task);
    }
    return biolizard_test_mode;
}

static void select_test(struct task *task)
{
    if (player_pads[0]->on & PDD_DGT_ST) {
        if (selected_test != MENU_CLOSED &&
            selected_test != biolizard_test_mode) {
            biolizard_set_test(task, selected_test);
            biolizard_clear_attacks();
        }
    }
    if (selected_test == MENU_CLOSED) {
        selected_test = biolizard_test_mode;
    }
    if (player_pads[0]->press & PDD_DGT_KL) {
        ++selected_test;
    }
    if (selected_test == BIOLIZARD_TESTS) {
        selected_test = BIOLIZARD_TEST_OFF;
    }
}

void biolizard_debug_menu(struct task *task)
{
    if (game_state.debug) {
        if (game_state.paused == PAUSED_BY_PLAYER_0) {
            select_test(task);
            if (selected_test != BIOLIZARD_TEST_OFF) {
                njPrint(TITLE_LINE, "LASTBOSS1 TEST");
            }
            switch (selected_test) {
            case BIOLIZARD_TEST_OFF:
                break;
            case BIOLIZARD_TEST_ENERGY:
                njPrint(TEST_LINE, "ENERGY MODE");
                biolizard_debug_energy();
                break;
            case BIOLIZARD_TEST_ENERGY_DISPLAY:
                njPrint(TEST_LINE, "ENERGY DISP");
                break;
            case BIOLIZARD_TEST_MOTION:
                njPrint(TEST_LINE, "MOTION DISP");
                break;
            case BIOLIZARD_TEST_SPHERES:
                njPrint(TEST_LINE, "IKURA MODE");
                biolizard_debug_spheres(FALSE);
                break;
            case BIOLIZARD_TEST_WIND:
                njPrint(TEST_LINE, "IKURA WIND");
                biolizard_debug_spheres(TRUE);
                break;
            case BIOLIZARD_TEST_GLIDE:
                njPrint(TEST_LINE, "GLIDING MODE");
                break;
            case BIOLIZARD_TEST_DEFEND:
                njPrint(TEST_LINE, "DEFEND DISP");
                break;
            }
        } else {
            selected_test = MENU_CLOSED;
        }
    }
}
