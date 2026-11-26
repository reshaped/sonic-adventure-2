/* the race camera editor's menus, and the state its files share */
#ifndef CHAOSTGRACE_EDIT_MENU_H
#define CHAOSTGRACE_EDIT_MENU_H
#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgrace/editor.h"

struct task;
/* a menu is an array of EDIT_MENU_ENTRIES entries, the first with a NULL name
 * its end. entry 0 is the menu's title. its submenu is the menu's parent, and
 * its activate runs as the menu is left. its rows start at EDIT_FIRST_ROW */
enum { EDIT_FIRST_ROW = 1, EDIT_MENU_ENTRIES = 16 };
/* A on a row with an activate runs the activate each frame until that sets
 * the editor's action. EDIT_ENTER then enters any submenu the row has */
struct race_edit_entry {
    const char *name;
    struct race_edit_entry *submenu;
    void (*activate)(struct task *task);
};
/* what race_editor_step does next. EDIT_RUN runs the row's activate, and
 * EDIT_LEAVE leaves the menu for its parent */
enum race_edit_action { EDIT_BROWSE, EDIT_RUN, EDIT_ENTER, EDIT_LEAVE };

extern struct task *race_editor; /* the editor's task, or NULL */
extern Sint32 race_path_count, race_position_count;
extern struct race_path_cell *race_path_head;
extern struct race_position_cell *race_position_head;
/* the screens in use before the editor opened */
extern Sint32 race_saved_screen_count;
/* the row the next menu entered opens at, or EDIT_NO_ROW for
 * EDIT_FIRST_ROW */
enum { EDIT_NO_ROW = -1 };
extern Sint32 race_next_menu_row;
extern struct race_edit_entry *race_menu; /* the menu shown */
extern struct race_edit_entry race_main_menu[EDIT_MENU_ENTRIES];
/* how far up the selected path key's gate its moving line stands, from 0
 * to 1 */
extern Float race_gate_pulse;
extern const char race_progress_format[], race_editor_name[];
extern NJS_CNK_OBJECT race_eye_marker, race_chase_marker, race_fix_marker,
    race_move_marker;
/* the selected path key draws no eye marker while this is TRUE. nor does it
 * while camera 0 runs race_camera_edit_eye */
extern Bool race_hide_selected_camera;

void race_editor_idle(void);
/* sort the path list by falling progress */
void race_sort_camera_keys(void);
/* store the path list in race_camera_script from its tail, and end it with
 * an inactive key. a sorted list stores by rising progress */
void race_store_camera_keys(void);
void race_write_camera_keys(struct task *task);
void race_exit_editor(struct task *task);
void race_add_path_cell(struct task *task);
void race_add_position_cell(struct task *task);
void race_choose_path_cell(struct task *task);
void race_choose_position_cell(struct task *task);
void race_leave_cell(struct task *task);
/* open the next menu at the row of the selected key's position mode. the key
 * is the selected path key where there is one, and else the selected position
 * key */
void race_get_eye_mode(void);
/* set that key's position mode to the mode of the row chosen */
void race_apply_eye_mode(void);
void race_choose_eye_mode(struct task *task);
void race_fix_eye(struct task *task);
void race_move_eye(struct task *task);
void race_parallel_eye(struct task *task);
/* race_get_eye_mode and race_apply_eye_mode for the key's target mode */
void race_get_target_mode(void);
void race_apply_target_mode(void);
void race_choose_target_mode(struct task *task);
void race_fix_target(struct task *task);
void race_move_target(struct task *task);
void race_chase_target(struct task *task);
void race_choose_progress(struct task *task);
void race_reset_cell(struct task *task);
void race_delete_cell(struct task *task);
void race_editor_step(struct task *task);
/* draw a path key's target marker, its eye marker and the line between
 * them. the selected key also draws a gate across the track at its
 * progress */
void race_draw_path_cell(struct race_path_cell *cell);
/* show the editor's main menu on one screen, and make its task unless it
 * exists. the third pad moves camera 0 free, camera 1 runs
 * race_camera_edit_target and camera 2 race_camera_edit_progress */
void race_open_editor(void);
/* free the editor's lists, and set every camera following its entrant */
void race_resume_cameras(void);
/* open the editor on its task's first run */
void race_run_editor(struct task *task);

#endif
