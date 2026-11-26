/* the garden's editor, a task that the pad in port C drives. its menu opens
 * the pages registered with it. L and R choose the Chao a resident page
 * edits */
#ifndef CHAO_EDITOR_H
#define CHAO_EDITOR_H

#include <ninja.h>
#include <sg_xpt.h>

struct task;
/* a page's step, which the editor's task runs each frame the page is open */
typedef void chao_editor_callback(struct task *editor);
#define EDITOR_PAGE_CAPACITY 256
/* the most pages a register call adds. the menu opens page n at mode n + 2,
 * which the task keeps in a Sint8 */
#define EDITOR_PAGE_LIMIT 126
struct chao_editor_page {
    /* a global page, which opens without a Chao. a resident page edits the
     * selected Chao, and opens only while the garden holds one */
    Bool works_without_chao;
    chao_editor_callback *update;
    Sint32 value; /* the value the page was registered with */
};
/* camera_top of camera editor_camera_index when the editor's task first ran.
 * closing takes off every mode above it */
extern Sint32 editor_entry_camera_depth;
extern struct chao_editor_page editor_pages[EDITOR_PAGE_CAPACITY];
extern struct task *garden_editor; /* the open editor, or null */
extern Sint32 editor_page_count, editor_camera_index;
extern Sint32 editor_motion_index, editor_sound_id, editor_volume;
extern Float editor_motion_speed;
extern Angle editor_marker_phase;
/* open the editor, unless it is open */
void chao_editor_open(void);
void chao_editor_clear_pages(void);
/* append a resident page or a global page. at most EDITOR_PAGE_LIMIT pages
 * may be registered */
void chao_editor_register_resident_page(chao_editor_callback *update,
                                        Sint32 value);
void chao_editor_register_global_page(chao_editor_callback *update,
                                      Sint32 value);
/* take every mode above index 1 off camera editor_camera_index's stack */
void chao_editor_clear_cameras(void);
/* leave the open page for the menu. it pushes garden_fixed_camera onto
 * camera editor_camera_index's stack, and disables the garden's objects */
void chao_editor_return_to_menu(struct task *editor);
/* the menu's step. up and down choose a row, and A opens its page. the row
 * past the last page closes the editor */
void chao_editor_menu(struct task *editor);
void chao_audition_effects(struct task *editor);
void chao_audition_list_four(struct task *editor);
void chao_audition_voices(struct task *editor);
void chao_audition_music(struct task *editor);
void chao_audition_area(struct task *editor);
void fun_0c50739e(void);
void chao_edit_growth(struct task *editor);
void chao_edit_texture(struct task *editor);
void chao_edit_color(struct task *editor);
void chao_edit_animal_parts(struct task *editor);
void chao_edit_costume(struct task *editor);
void chao_edit_form(struct task *editor);
void chao_edit_small_toys(struct task *editor);
void chao_edit_art(struct task *editor);
void chao_edit_dances(struct task *editor);
void chao_edit_songs(struct task *editor);
void chao_edit_instruments(struct task *editor);
void garden_toggle_debug_flag(Bool *flag);
void garden_edit_display(struct task *editor);
void garden_edit_flags(struct task *editor);
void garden_edit_race_groups(struct task *editor);
void garden_edit_awards(struct task *editor);
void chao_edit_motion_frame(struct task *editor);
void chao_edit_motion_speed(struct task *editor);
void chao_edit_eyes(struct task *editor);
void chao_edit_eyelids(struct task *editor);
void chao_edit_mouth(struct task *editor);
void chao_edit_actor_flags(struct task *editor);
void garden_edit_free_camera(struct task *editor);

/* the phase that pulses the growth page's cursor, and the graph it edits */
extern Angle editor_growth_phase;
extern Sint32 editor_growth_graph;

#endif
