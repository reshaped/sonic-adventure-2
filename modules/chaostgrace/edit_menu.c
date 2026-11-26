/* the race camera editor's menus, and the state its files share */
#include <sg_xpt.h>
#ifdef NONMATCHING
/* garden_race_course, by which this side's menu finds the course's camera
 * track */
#include "chao/race.h"
#endif
#include "chaostgrace/camera.h"
#include "chaostgrace/edit_menu.h"
#include "chaostgrace/editor.h"

struct task *race_editor = NULL;
Sint32 race_path_count = 0;
Sint32 race_position_count = 0;
struct race_path_cell *race_path_head = NULL;
struct race_position_cell *race_position_head = NULL;
struct race_path_cell *race_edit_path = NULL;
struct race_position_cell *race_edit_position = NULL;
Sint32 race_saved_screen_count = 0;
Sint32 race_next_menu_row = EDIT_NO_ROW;
struct race_edit_entry *race_menu = NULL;

const char race_progress_format[8] = "%3.2f";
const char race_editor_name[] = "ALR_CameraEditorExecutor";
static struct race_edit_entry cell_menu[EDIT_MENU_ENTRIES];
static struct race_edit_entry data_menu[EDIT_MENU_ENTRIES];
static struct race_edit_entry eye_menu[] = {
    {"SELECT CAMPOS MODE", cell_menu, NULL},
    {"MODE FIX", NULL, race_fix_eye},
    {"MODE MOVE", NULL, race_move_eye},
    {"MODE PARALLEL", NULL, race_parallel_eye},
    {NULL, NULL, NULL}};
static struct race_edit_entry target_menu[] = {
    {"SELECT TARGET MODE", cell_menu, NULL},
    {"MODE FIX", NULL, race_fix_target},
    {"MODE MOVE", NULL, race_move_target},
    {"MODE CHASE", NULL, race_chase_target},
    {NULL, NULL, NULL}};
static struct race_edit_entry cell_menu[EDIT_MENU_ENTRIES] = {
    {"EDIT CELL", data_menu, race_leave_cell},
    {"CAMPOS", eye_menu, race_choose_eye_mode},
    {"TARGET", target_menu, race_choose_target_mode},
    {"COLLI", NULL, race_choose_progress},
    {"SORT&SAVE", NULL, race_write_camera_keys},
    {"RESET CELL", NULL, race_reset_cell},
    {"DELETE CELL", NULL, race_delete_cell}};
static struct race_edit_entry data_menu[EDIT_MENU_ENTRIES] = {
    {"EDIT DATA", race_main_menu, NULL},
    {"CREATE CELL (PATH)", cell_menu, race_add_path_cell},
    {"CREATE CELL (POS)", cell_menu, race_add_position_cell},
    {"CHOOSE CELL (PATH)", cell_menu, race_choose_path_cell},
    {"CHOOSE CELL (POS)", cell_menu, race_choose_position_cell}};

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side. the menu's entries
 * call the camera keys' functions as declared. the load takes the course's
 * camera track */
static void clear_keys(struct task *task)
{
    race_clear_camera_keys();
}
static void load_course_keys(struct task *task)
{
    race_load_camera_keys(garden_race_course()->camera_track);
}
#define EDIT_CLEAR clear_keys
#define EDIT_LOAD load_course_keys
#else
#define EDIT_CLEAR ((void (*)(struct task *))race_clear_camera_keys)
/* @bug memory: the menu passes race_load_camera_keys the editor's task as its
 * track, and race_read_camera_script reads track_names at that index,
 * outside the table, at 0x0c9c7b94 */
#define EDIT_LOAD ((void (*)(struct task *))race_load_camera_keys)
#endif
struct race_edit_entry race_main_menu[EDIT_MENU_ENTRIES] = {
    {"FREE MOVE", NULL, NULL},
    {"CLEAR", NULL, EDIT_CLEAR},
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    {"LOAD", NULL, EDIT_LOAD},
    {"SORT&SAVE", NULL, race_write_camera_keys},
    {"EDIT", data_menu, NULL},
    {"EXIT", NULL, race_exit_editor}};
Float race_gate_pulse = 0.0F;
