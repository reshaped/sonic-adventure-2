/* the Biolizard's random numbers, textures and test menu */
#ifndef BOSS_LAST1_ARENA_H
#define BOSS_LAST1_ARENA_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* the tests biolizard_set_test starts, in the order the menu steps through
 * them. BIOLIZARD_TEST_OFF returns the Biolizard to its walk */
enum biolizard_test {
    BIOLIZARD_TEST_OFF,
    BIOLIZARD_TEST_ENERGY,
    BIOLIZARD_TEST_ENERGY_DISPLAY,
    BIOLIZARD_TEST_MOTION,
    BIOLIZARD_TEST_SPHERES,
    BIOLIZARD_TEST_WIND,
    BIOLIZARD_TEST_GLIDE,
    BIOLIZARD_TEST_DEFEND,
    BIOLIZARD_TESTS
};
/* the test menu's layout. every row prints from BIOLIZARD_MENU_COLUMN, and
 * the menu shows the test it would run on BIOLIZARD_MENU_TEST_ROW. a test's
 * settings start at BIOLIZARD_MENU_SETTING_ROW. the selected setting prints
 * in BIOLIZARD_MENU_SELECTED, and the rest in BIOLIZARD_MENU_TEXT */
#define BIOLIZARD_MENU_COLUMN 20
#define BIOLIZARD_MENU_TEST_ROW 3
#define BIOLIZARD_MENU_SETTING_ROW 7
#define BIOLIZARD_MENU_SELECTED 0xffffff00
#define BIOLIZARD_MENU_TEXT 0xffffffff

extern NJS_TEXLIST biolizard_effect_texlist;
extern NJS_TEXLIST biolizard_water_texlist;
/* the test running, or BIOLIZARD_TEST_OFF */
extern Sint8 biolizard_test_mode;
extern NJS_TEXLIST biolizard_title_texlist;
extern NJS_TEXLIST biolizard_model_texlist;
/* the next number of the Biolizard's own generator, 0 to RAND_MAX. it steps
 * as rand() does, but keeps only the number it returns as its state. from a
 * state of 0, as at the start, it first takes a seed from rand() */
int biolizard_random(void);
/* step the generator's state as rand() does, and seed rand() with it */
void biolizard_seed_random(void);
/* load or release the texture lists this header names, and the guard
 * flare's defend_texlist */
void biolizard_textures_load(void);
void biolizard_textures_release(void);
/* set biolizard_test_mode to test, and start the test on task, the
 * Biolizard's. returns test */
Sint8 biolizard_set_test(struct task *task, Sint8 test);
/* step the test menu, which shows in player 0's pause while game_state.debug
 * is set. Left steps through the tests. Start runs the test shown on task,
 * the Biolizard's */
void biolizard_debug_menu(struct task *task);
#endif
