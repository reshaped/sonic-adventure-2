/* the race camera editor: its lists of keys, its menu's actions, its task
 * and its markers */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/course.h"
#include "chao/movement.h"
#include "chao/race_record.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"
#include "chaostgrace/edit_menu.h"
#include "chaostgrace/editor.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

#ifdef NONMATCHING
/* the plain forms the hack tags name: the field and EDIT_PAD. this side's
 * objects don't define the second names, which only symbols.txt places */
#define EDIT_POSITION_KEYS race_camera_script.position
#define EDIT_PAD_ALIAS EDIT_PAD
#else
/* @hack spelling: race_position_keys, a symbol of its own at
 * race_camera_script.position, where the field would do */
extern struct race_position_key race_position_keys[RACE_CAMERA_KEYS];
/* @hack spelling: race_editor_pad, a symbol of its own at player_pads[2],
 * where EDIT_PAD would do */
extern const PDS_PERIPHERAL *race_editor_pad;
/* race_clear_camera_keys, declared at its address as the call below makes
 * it */
extern void race_clear_camera_keys_as_called(int unused);
#define EDIT_POSITION_KEYS race_position_keys
#define EDIT_PAD_ALIAS race_editor_pad
#endif

Sint32 race_edit_camera;
static Sint32 menu_action, menu_row;

void race_clear_camera_keys(void)
{
    struct race_path_cell *path;
    struct race_path_cell *next_path;
    struct race_position_cell *position;
    struct race_position_cell *next_position;
    if (race_path_head) {
        path = race_path_head;
        while (path) {
            next_path = path->next;
            syFree(path);
            path = next_path;
        }
        race_path_head = NULL;
        race_edit_path = NULL;
    }
    if (race_position_head) {
        position = race_position_head;
        while (position) {
            next_position = position->next;
            syFree(position);
            position = next_position;
        }
        race_position_head = NULL;
        race_edit_position = NULL;
    }
    race_path_count = 0;
    race_position_count = 0;
    menu_action = EDIT_ENTER;
}

void race_editor_idle(void)
{
}

#ifdef NONMATCHING
/* this side reads the script by race_read_camera_script, which returns null for
 * a script it refuses. the result and the lists are as camera.h says */
Bool race_load_camera_keys(int track)
{
    const struct race_camera_script *script = race_read_camera_script(track);
    struct race_path_cell *paths = NULL, *path;
    struct race_position_cell *positions = NULL, *position;
    int path_count = 0, position_count = 0;

    if (script == NULL) {
        return FALSE;
    }
    while (script->path.keys[path_count].camera.active) {
        path = syCalloc(1, sizeof(*path));
        if (path == NULL) {
            goto failed;
        }
        path->key = script->path.keys[path_count++];
        path->next = paths;
        if (paths != NULL) {
            paths->prev = path;
        }
        paths = path;
    }
    while (script->position[position_count].camera.active) {
        position = syCalloc(1, sizeof(*position));
        if (position == NULL) {
            goto failed;
        }
        position->key = script->position[position_count++];
        position->next = positions;
        if (positions != NULL) {
            positions->prev = position;
        }
        positions = position;
    }
    race_clear_camera_keys();
    race_camera_script = *script;
    race_path_head = paths;
    race_position_head = positions;
    race_path_count = path_count;
    race_position_count = position_count;
    return TRUE;

failed:
    while (paths != NULL) {
        path = paths;
        paths = paths->next;
        syFree(path);
    }
    while (positions != NULL) {
        position = positions;
        positions = positions->next;
        syFree(position);
    }
    return FALSE;
}
#else
void race_load_camera_keys(int track)
{
    struct race_path_key *key;
    struct race_path_cell *cell;
    struct race_path_cell *head;
    struct race_position_key *position;
    struct race_position_cell *point;
    struct race_position_cell *point_head;
    race_clear_camera_keys();
    race_path_count = 0;
    race_position_count = 0;
    race_read_camera_script(track);
    for (key = race_camera_script.path.keys; key->camera.active; ++key) {
        cell = syCalloc(1, sizeof(*cell));
        cell->key = *key;
        head = race_path_head;
        race_path_head = cell;
        cell->prev = NULL;
        cell->next = NULL;
        if (head) {
            head->prev = cell;
            cell->next = head;
        }
        race_path_count++;
    }
    for (position = EDIT_POSITION_KEYS; position->camera.active; ++position) {
        point = syCalloc(1, sizeof(*point));
        point->key = *position;
        point_head = race_position_head;
        race_position_head = point;
        if (point_head) {
            point_head->prev = point;
            point->next = point_head;
        }
        race_position_count++;
    }
    menu_action = EDIT_ENTER;
}
#endif

void race_sort_camera_keys(void)
{
    struct race_path_cell *cell;
    struct race_path_cell *next;
    struct race_path_cell *head;
    struct race_path_cell *tail;
    struct race_path_cell *scan;
    cell = race_path_head;
    if (cell) {
        next = cell->next;
        head = tail = cell;
        head->prev = NULL;
        head->next = NULL;
        while (next) {
            cell = next;
            next = next->next;
            scan = tail;
            while (scan->key.progress < cell->key.progress) {
                scan = scan->prev;
                if (!scan) {
                    cell->prev = NULL;
                    cell->next = head;
                    head->prev = cell;
                    head = cell;
                    break;
                }
            }
            if (scan) {
                cell->prev = scan;
                cell->next = scan->next;
                scan->next = cell;
                if (cell->next) {
                    cell->next->prev = cell;
                } else {
                    tail = cell;
                }
            }
        }
        race_path_head = head;
    }
}

void race_store_camera_keys(void)
{
    struct race_path_cell *cell;
    int index;
    Uint32 *word;
    struct race_path_key *destination;
    struct race_path_key *keys = race_camera_script.path.keys;

    /* @hack type: clear_words reached through keys cast to the script, where
     * race_camera_script.path.clear_words would do */
    for (index = sizeof(race_camera_script.path.clear_words) / sizeof(*word),
        word = ((struct race_camera_script *)keys)->path.clear_words;
         index != 0; index--) {
        *word++ = 0;
    }
    if (!race_path_head) {
        return;
    }
    for (cell = race_path_head; cell; cell = cell->next) {
        if (!cell->next) {
            break;
        }
    }
    index = 0;
    while (cell) {
        destination = &keys[index];
        *destination = cell->key;
        destination->camera.active = 1;
        cell = cell->prev;
        index++;
    }
    keys[index].camera.active = 0;
}

void race_write_camera_keys(struct task *task)
{
    race_sort_camera_keys();
    race_store_camera_keys();
    menu_action = EDIT_ENTER;
}

void race_exit_editor(struct task *task)
{
    task_kill(task);
    menu_action = EDIT_ENTER;
}

/* a new path key's target lies TARGET_REACH ahead of its eye */
#define TARGET_REACH 40.0F

void race_add_path_cell(struct task *task)
{
    struct race_path_cell *head = race_path_head;
    struct race_camera *camera = &race_cameras[0];
#ifdef NONMATCHING
    /* this side adds no cell past RACE_CAMERA_KEYS - 1, nor one the heap can't
     * give. the matching side writes through the null */
    struct race_path_cell *cell;
    int segment;
    if (race_path_count >= RACE_CAMERA_KEYS - 1) {
        menu_action = EDIT_ENTER;
        return;
    }
    cell = syCalloc(1, sizeof(*cell));
    if (!cell) {
        menu_action = EDIT_ENTER;
        return;
    }
#else
    struct race_path_cell *cell = syCalloc(1, sizeof(*cell));
    int segment;
#endif
    race_path_count++;
    race_path_head = cell;
    if (head) {
        head->prev = cell;
        cell->next = head;
    }
    race_edit_path = cell;
    segment = garden_nearest_track_segment(&camera->view.pos);
    if (segment < 0) {
        segment = 0;
    }
    race_edit_path->key.progress = segment;
    {
        NJS_POINT3 *target = &race_edit_path->key.camera.target;
        Angle pitch = camera->view.ang.x;
        Angle yaw = camera->view.ang.y;
        NJS_VECTOR direction = {0.0F, 0.0F, -TARGET_REACH};
        NJS_POINT3 *eye = &camera->view.pos;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, yaw);
        njRotateX(NULL, pitch);
        njCalcVector(NULL, &direction, &direction);
        njPopMatrixEx();
        target->x = eye->x + direction.x;
        target->y = eye->y + direction.y;
        target->z = eye->z + direction.z;
    }
    {
        NJS_POINT3 *position = &race_edit_path->key.camera.position;
        *position = camera->view.pos;
    }
    race_set_camera_step(0, race_camera_edit_eye);
    menu_action = EDIT_ENTER;
}

void race_add_position_cell(struct task *task)
{
    struct race_position_cell *head = race_position_head;
#ifdef NONMATCHING
    /* this side adds no cell past RACE_CAMERA_KEYS - 1, nor one the heap can't
     * give. the matching side writes through the null */
    struct race_position_cell *cell;
    if (race_position_count >= RACE_CAMERA_KEYS - 1) {
        menu_action = EDIT_ENTER;
        return;
    }
    cell = syCalloc(1, sizeof(*cell));
    if (!cell) {
        menu_action = EDIT_ENTER;
        return;
    }
#else
    struct race_position_cell *cell = syCalloc(1, sizeof(*cell));
#endif
    race_position_count++;
    race_position_head = cell;
    if (head) {
        head->prev = cell;
        cell->next = head;
    }
    race_edit_position = cell;
    race_set_camera_step(0, race_camera_edit_eye);
    menu_action = EDIT_ENTER;
}

/* the modes of a choice of key. CHOOSE_KEY steps through the keys by L and
 * R, and CHOOSE_EMPTY waits with none to choose */
enum choose_mode { CHOOSE_START, CHOOSE_KEY, CHOOSE_EMPTY };
/* with no key to choose, tick counts rounds of WAIT_DELAY frames. the editor
 * browses again once tick passes WAIT_ROUNDS */
#define WAIT_DELAY 45
#define WAIT_ROUNDS 4
#define WAIT_FOR_CELL()                                                        \
    do {                                                                       \
        if (work->timer++ > WAIT_DELAY) {                                      \
            work->timer = 0;                                                   \
            if (work->tick++ > WAIT_ROUNDS) {                                  \
                work->tick = 0;                                                \
                menu_action = EDIT_BROWSE;                                     \
            }                                                                  \
            /* @hack redundant: an empty test of tick's low bit */             \
            if (work->tick & 1) {                                              \
            }                                                                  \
        }                                                                      \
    } while (0)
#define ACCEPT_CELL()                                                          \
    do {                                                                       \
        if (EDIT_PAD->press & PDD_DGT_TA) {                                    \
            menu_action = EDIT_ENTER;                                          \
        }                                                                      \
        if (EDIT_PAD->press & PDD_DGT_TB) {                                    \
            race_set_camera_step(0, race_camera_free);                         \
            menu_action = EDIT_BROWSE;                                         \
        }                                                                      \
    } while (0)

void race_choose_path_cell(struct task *task)
{
    struct task_work *work = task->work;
    race_set_camera_step(0, race_camera_edit_eye);
    switch (work->mode) {
    case CHOOSE_START: {
        struct race_path_cell *head;
        work->tick = 0;
        head = race_path_head;
        if (race_edit_path) {
            struct race_path_cell *cell = head;
            while (race_edit_path != cell) {
                cell = cell->next;
                work->tick++;
            }
            work->mode = CHOOSE_KEY;
        } else if (head) {
            race_edit_path = head;
            work->tick = 0;
            work->mode = CHOOSE_KEY;
        } else {
            work->mode = CHOOSE_EMPTY;
        }
        break;
    }
    case CHOOSE_KEY:
        if (EDIT_PAD->press & PDD_DGT_TL) {
            if (race_edit_path->prev) {
                race_edit_path = race_edit_path->prev;
                if (work->tick > 0) {
                    work->tick--;
                }
            }
        }
        if (EDIT_PAD->press & PDD_DGT_TR) {
            if (race_edit_path->next) {
                race_edit_path = race_edit_path->next;
                work->tick++;
            }
        }
        {
            struct race_camera *camera = &race_cameras[0];
            NJS_VECTOR direction;
            {
#ifdef NONMATCHING
#else
                NJS_POINT3 fallback;
#endif
                NJS_POINT3 *target;
                NJS_POINT3 *position;
                if (race_edit_path) {
                    target = &race_edit_path->key.camera.target;
                    position = &race_edit_path->key.camera.position;
                }
                /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
                else if (race_edit_position) {
                    target = &race_edit_position->key.camera.target;
                    position = &race_edit_position->key.camera.position;
                } else {
#ifdef NONMATCHING
                    /* this side repairs the bug tagged in the matching side:
                     * with neither key selected, it turns nothing */
                    break;
#else
                    /* @bug uninitialized: with neither key selected,
                     * position is never set and target points at a local
                     * never set, at 0x0c9c8128 */
                    target = &fallback;
#endif
                }
                direction.x = target->x - position->x;
                direction.y = target->y - position->y;
                direction.z = target->z - position->z;
            }
            {
                struct angles angles;
                garden_aim_angles(&direction, &angles);
                camera->view.ang.x = -angles.x;
                camera->view.ang.y = angles.y + HALF_TURN;
            }
        }
        break;
    case CHOOSE_EMPTY:
        WAIT_FOR_CELL();
        break;
    }
    ACCEPT_CELL();
}

void race_choose_position_cell(struct task *task)
{
    struct task_work *work = task->work;
    race_set_camera_step(0, race_camera_edit_eye);
    switch (work->mode) {
    case CHOOSE_START:
        if (race_edit_position) {
            work->mode = CHOOSE_KEY;
        } else if (race_position_head) {
            race_edit_position = race_position_head;
            work->mode = CHOOSE_KEY;
        } else {
            work->mode = CHOOSE_EMPTY;
        }
        break;
    case CHOOSE_KEY:
        if (EDIT_PAD->press & PDD_DGT_TL) {
            if (race_edit_position->prev) {
                race_edit_position = race_edit_position->prev;
            }
        }
        if (EDIT_PAD->press & PDD_DGT_TR) {
            if (race_edit_position->next) {
                race_edit_position = race_edit_position->next;
            }
        }
        break;
    case CHOOSE_EMPTY:
        WAIT_FOR_CELL();
        break;
    }
    ACCEPT_CELL();
}

void race_leave_cell(struct task *task)
{
    race_set_camera_step(0, race_camera_free);
    menu_action = EDIT_ENTER;
}

/* a mode's row is its value's place among the menu's rows */
#define GET_MODE(field)                                                        \
    do {                                                                       \
        if (race_edit_path) {                                                  \
            race_next_menu_row =                                               \
                race_edit_path->key.camera.field + EDIT_FIRST_ROW;             \
        } else if (race_edit_position) {                                       \
            race_next_menu_row =                                               \
                race_edit_position->key.camera.field + EDIT_FIRST_ROW;         \
        }                                                                      \
    } while (0)
#define APPLY_MODE(field)                                                      \
    do {                                                                       \
        struct race_path_cell *cell = race_edit_path;                          \
        Uint8 mode = menu_row - EDIT_FIRST_ROW;                                \
        if (cell) {                                                            \
            cell->key.camera.field = mode;                                     \
        } else if (race_edit_position) {                                       \
            race_edit_position->key.camera.field = mode;                       \
        }                                                                      \
    } while (0)

void race_get_eye_mode(void)
{
    GET_MODE(position_mode);
}
void race_apply_eye_mode(void)
{
    APPLY_MODE(position_mode);
}
void race_choose_eye_mode(struct task *task)
{
    race_get_eye_mode();
    menu_action = EDIT_ENTER;
}

/* an edit view's activate: its first run, then each frame after */
enum activate_mode { ACTIVATE_START, ACTIVATE_RUN };
#define LEAVE_VIEW()                                                           \
    do {                                                                       \
        if (EDIT_PAD_ALIAS->press & PDD_DGT_TB) {                              \
            race_edit_camera = EDIT_NO_CAMERA;                                 \
            menu_action = EDIT_ENTER;                                          \
        }                                                                      \
    } while (0)
#define LEAVE_TARGET_VIEW()                                                    \
    do {                                                                       \
        if (EDIT_PAD_ALIAS->press & PDD_DGT_TB) {                              \
            race_edit_camera = EDIT_NO_CAMERA;                                 \
            camera_active_count = 1;                                           \
            menu_action = EDIT_ENTER;                                          \
        }                                                                      \
    } while (0)

void race_fix_eye(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_apply_eye_mode();
        race_set_camera_step(0, race_camera_edit_eye);
        race_edit_camera = EDIT_EYE_CAMERA;
        work->mode++;
    }
    LEAVE_VIEW();
}
void race_move_eye(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_apply_eye_mode();
        race_edit_camera = EDIT_EYE_CAMERA;
        work->mode++;
        break;
    default:
        break;
    }
    LEAVE_VIEW();
}
void race_parallel_eye(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_edit_camera = EDIT_EYE_CAMERA;
        race_apply_eye_mode();
        work->mode++;
    }
    LEAVE_VIEW();
}

void race_get_target_mode(void)
{
    GET_MODE(target_mode);
}
void race_apply_target_mode(void)
{
    APPLY_MODE(target_mode);
}
void race_choose_target_mode(struct task *task)
{
    race_get_target_mode();
    menu_action = EDIT_ENTER;
}
void race_fix_target(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_apply_target_mode();
        race_set_camera_step(0, race_camera_edit_target);
        race_edit_camera = EDIT_TARGET_CAMERA;
        work->mode++;
    }
    LEAVE_TARGET_VIEW();
}
void race_move_target(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_apply_target_mode();
        race_edit_camera = EDIT_TARGET_CAMERA;
        work->mode++;
        break;
    default:
        break;
    }
    LEAVE_TARGET_VIEW();
}
void race_chase_target(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case ACTIVATE_START:
        race_edit_camera = EDIT_TARGET_CAMERA;
        race_apply_target_mode();
        work->mode++;
    }
    LEAVE_TARGET_VIEW();
}

void race_choose_progress(struct task *task)
{
    struct task_work *work = task->work;
    struct race_path_cell *cell;
    Float *progress;
    switch (work->mode) {
    case ACTIVATE_START:
        work->mode++;
        break;
    }
    if ((cell = race_edit_path) != NULL) {
        progress = &cell->key.progress;
        MOVE_PROGRESS(progress);
    }
    if (EDIT_PAD->press & PDD_DGT_TB) {
        race_edit_camera = EDIT_NO_CAMERA;
        camera_active_count = 1;
        menu_action = EDIT_ENTER;
    }
}

void race_reset_cell(struct task *task)
{
    menu_action = EDIT_ENTER;
}

#define DELETE_CELL(selected, head, count, prev_cell, next_cell)               \
    do {                                                                       \
        if (selected) {                                                        \
            (prev_cell) = (selected)->prev;                                    \
            (next_cell) = (selected)->next;                                    \
            syFree(selected);                                                  \
            (count)--;                                                         \
            if (prev_cell) {                                                   \
                if (next_cell) {                                               \
                    (prev_cell)->next = (next_cell);                           \
                    (next_cell)->prev = (prev_cell);                           \
                } else {                                                       \
                    (prev_cell)->next = NULL;                                  \
                }                                                              \
                (selected) = (prev_cell);                                      \
            } else if (next_cell) {                                            \
                (next_cell)->prev = NULL;                                      \
                (head) = (next_cell);                                          \
                (selected) = (next_cell);                                      \
            } else {                                                           \
                (head) = NULL;                                                 \
                (selected) = NULL;                                             \
                menu_action = EDIT_LEAVE;                                      \
            }                                                                  \
        }                                                                      \
    } while (0)

void race_delete_cell(struct task *task)
{
    struct race_path_cell *prev_path;
    struct race_path_cell *next_path;
    struct race_position_cell *prev_position;
    struct race_position_cell *next_position;
    menu_action = EDIT_ENTER;
    DELETE_CELL(race_edit_path, race_path_head, race_path_count, prev_path,
                next_path);
    DELETE_CELL(race_edit_position, race_position_head, race_position_count,
                prev_position, next_position);
}

/* the path keys' progress, a line each from PROGRESS_ROW */
#define PROGRESS_COLUMN 18
#define PROGRESS_ROW 6
#define PULSE_STEP 0.02F
/* the phase step of the selected key's blink, which the editor's work
 * keeps in ang.x */
#define BLINK_STEP 0x400
#define ROW_COLOR 0xffffffff
#define CHOSEN_ROW_COLOR 0xffff7f00

void race_editor_step(struct task *task)
{
    struct task_work *work = task->work;
    struct race_path_cell *cell = race_path_head;
    int row;
    /* @hack redundant: frame_space, 24 bytes that only size the frame */
    Uint8 frame_space[24];

    for (row = 0; cell; row++, cell = cell->next) {
        njPrint(NJM_LOCATION(PROGRESS_COLUMN, row + PROGRESS_ROW),
                &race_progress_format[0], cell->key.progress);
    }
    race_gate_pulse += PULSE_STEP;
    if (race_gate_pulse > 1.0F) {
        race_gate_pulse = 0.0F;
    }
    work->ang.x += BLINK_STEP;
    switch (menu_action) {
    case EDIT_BROWSE:
        for (row = EDIT_FIRST_ROW; race_menu[row].name; row++) {
            if (row == menu_row) {
                njPrintColor(CHOSEN_ROW_COLOR);
            } else {
                njPrintColor(ROW_COLOR);
            }
        }
        njPrintColor(ROW_COLOR);
        if (EDIT_PAD->press & PDD_DGT_KU) {
            menu_row--;
            if (menu_row < EDIT_FIRST_ROW) {
                menu_row = EDIT_FIRST_ROW;
            }
        }
        if (EDIT_PAD->press & PDD_DGT_KD) {
            if ((race_menu + menu_row + 1)->name) {
                menu_row++;
            }
        }
        if (EDIT_PAD->press & PDD_DGT_TA) {
            race_next_menu_row = EDIT_NO_ROW;
            if (race_menu[menu_row].activate) {
                menu_action = EDIT_RUN;
            } else {
                menu_action = EDIT_ENTER;
            }
        }
        if (EDIT_PAD->press & PDD_DGT_TB) {
            menu_action = EDIT_LEAVE;
        }
        break;
    case EDIT_RUN:
        race_menu[menu_row].activate(task);
        break;
    case EDIT_ENTER:
        if (race_menu[menu_row].submenu) {
            race_menu = race_menu[menu_row].submenu;
            if (race_next_menu_row != EDIT_NO_ROW) {
                menu_row = race_next_menu_row;
                race_next_menu_row = EDIT_NO_ROW;
            } else {
                menu_row = EDIT_FIRST_ROW;
            }
        }
        work->mode = 0;
        menu_action = EDIT_BROWSE;
        break;
    case EDIT_LEAVE:
        if (race_menu->activate) {
            race_menu->activate(task);
        }
        if (race_menu->submenu) {
            race_menu = race_menu->submenu;
            menu_row = EDIT_FIRST_ROW;
        }
        menu_action = EDIT_BROWSE;
        break;
    }
}

#define DRAW_TARGET_MARKER(size, red, green, blue, model)                      \
    do {                                                                       \
        target = cell->key.camera.target;                                      \
        njPushMatrixEx();                                                      \
        njTranslateEx(&target);                                                \
        njScale(NULL, (size), (size), (size));                                 \
        constant_material_set(alpha, (red), (green), (blue));                  \
        njCnkEasyDrawObject(&(model));                                         \
        njPopMatrixEx();                                                       \
        debug_draw_mark(&target, MARK_SIZE);                                   \
    } while (0)
/* @hack spelling: the body a bare block, where do { } while (0) would do */
#define MIX_EDGE(dst, start, end, share)                                       \
    {                                                                          \
        (dst).x = ((1.0F - (share)) * (start).x) + ((share) * (end).x);        \
        (dst).y = ((1.0F - (share)) * (start).y) + ((share) * (end).y);        \
        (dst).z = ((1.0F - (share)) * (start).z) + ((share) * (end).z);        \
    }

#define EYE_MARKER_SCALE 0.55F
#define MARK_SIZE 10.0F
/* a key's markers draw MARKER_FADE less opaque, and the selected key's blink
 * by up to that */
#define MARKER_FADE 0.7F
/* the gate stands GATE_LIFT above the track, GATE_HEIGHT tall */
#define GATE_LIFT 0.1F
#define GATE_HEIGHT 40.0F

void race_draw_path_cell(struct race_path_cell *cell)
{
    NJS_VECTOR direction;
    struct angles angles;
    NJS_POINT3 target;
    struct garden_track_quad quad;
    NJS_POINT3 left;
    NJS_POINT3 right;
    NJS_POINT3 left_top;
    NJS_POINT3 right_top;
    NJS_POINT3 left_mid;
    NJS_POINT3 right_mid;
    NJS_POINT3 middle;
    Float alpha;
    Float fraction;
    Bool selected;
    Float *pulse;
    Float *progress;
    NJS_POINT3 *position;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: with no key selected, the markers
     * draw unselected and faded */
    selected = FALSE;
    alpha = -MARKER_FADE;
#endif
    /* @bug uninitialized: with no path key selected, alpha and selected are
     * never set, and the key's markers draw by them, from 0x0c9c8a68 */
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (race_edit_path) {
        if (cell == race_edit_path) {
            selected = TRUE;
            alpha = -(njSin(race_editor->work->ang.x) * MARKER_FADE);
        } else {
            selected = FALSE;
            alpha = -MARKER_FADE;
        }
    }
    switch (cell->key.camera.target_mode) {
    case RACE_TARGET_FIX:
        DRAW_TARGET_MARKER(0.22F, 1.0F, -1.0F, -1.0F, race_fix_marker);
        break;
    case RACE_TARGET_CHASE:
        DRAW_TARGET_MARKER(0.3F, -0.5F, -0.5F, 1.0F, race_chase_marker);
        break;
    case RACE_TARGET_MOVE:
        DRAW_TARGET_MARKER(0.4F, 1.0F, 1.0F, -1.0F, race_move_marker);
        break;
#ifdef NONMATCHING
    /* this side draws no marker for a move the switch doesn't name */
    default:
        return;
#endif
    }
    constant_material_set(alpha, -1.0F, -1.0F, 1.0F);
    position = &cell->key.camera.position;
    direction.x = target.x - position->x;
    direction.y = target.y - position->y;
    direction.z = target.z - position->z;
    garden_aim_angles(&direction, &angles);
    njPushMatrixEx();
    njTranslateEx(position);
    njRotateY(NULL, angles.y);
    njRotateX(NULL, angles.x);
    njRotateY(NULL, QUARTER_TURN);
    njScale(NULL, EYE_MARKER_SCALE, EYE_MARKER_SCALE, EYE_MARKER_SCALE);
    if ((!race_hide_selected_camera &&
         race_get_camera_step(0) != race_camera_edit_eye) ||
        !selected) {
        njCnkEasyDrawObject(&race_eye_marker);
    }
    njPopMatrixEx();
    if (selected) {
        progress = &cell->key.progress;
        fraction = njFraction(*progress);
        garden_track_quad((int)*progress, &quad);
        MIX_EDGE(left, quad.points[TRACK_LEFT], quad.points[TRACK_NEXT_LEFT],
                 fraction);
        MIX_EDGE(right, quad.points[TRACK_RIGHT], quad.points[TRACK_NEXT_RIGHT],
                 fraction);
        left.y += GATE_LIFT;
        right.y += GATE_LIFT;
        line_draw_points(&left, &right);
        left_top = left;
        left_top.y += GATE_HEIGHT;
        line_draw_points(&left, &left_top);
        right_top = right;
        right_top.y += GATE_HEIGHT;
        line_draw_points(&right, &right_top);
        line_draw_points(&left_top, &right_top);
        pulse = &race_gate_pulse;
        MIX_EDGE(left_mid, left, left_top, *pulse);
        MIX_EDGE(right_mid, right, right_top, *pulse);
        line_draw_points(&left_mid, &right_mid);
        if (*pulse < 0.5F) {
            middle.x = (left.x + right.x) * 0.5F;
            middle.y = (left.y + left_top.y) * 0.5F;
            middle.z = (left.z + right.z) * 0.5F;
            line_draw_points(position, &middle);
        }
    }
    line_draw_points(&target, position);
}

static void editor_draw(struct task *task)
{
    struct race_path_cell *cell;
    struct race_position_cell *position;
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR |
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    draw_attr_save();
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_set(0, NJD_FST_UA);
    draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    for (cell = race_path_head; cell; cell = cell->next) {
        race_draw_path_cell(cell);
    }
    for (position = race_position_head; position; position = position->next) {
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

static void editor_destroy(struct task *task)
{
    race_resume_cameras();
    camera_active_count = 1;
    race_editor = NULL;
}

/* the cameras race_open_editor gives the editor's steps */
#define EDITOR_CAMERAS 3

void race_open_editor(void)
{
    int index;
    struct task *task;
    camera_push_user_mode(0, race_camera_to_screen);
    race_set_camera_step(0, race_camera_free);
    race_set_camera_step(1, race_camera_edit_target);
    race_set_camera_step(2, race_camera_edit_progress);
    for (index = EDITOR_CAMERAS; index < RACE_ENTRANTS; index++) {
        race_set_camera_step(index, NULL);
    }
    race_set_viewport(0, 0);
    race_set_viewport(1, 1);
    race_set_viewport(2, 2);
    race_saved_screen_count = camera_active_count;
    camera_active_count = 1;
    race_edit_camera = EDIT_NO_CAMERA;
    menu_action = EDIT_BROWSE;
    race_menu = race_main_menu;
    if (!race_editor) {
        task = task_create(TASK_WORK, 2, race_editor_step, race_editor_name);
        task->draw = editor_draw;
        task->destructor = editor_destroy;
        menu_row = EDIT_FIRST_ROW;
        race_editor = task;
    }
}

void race_resume_cameras(void)
{
    int index;
#ifdef NONMATCHING
    /* this side keeps the tag's result, as race_clear_camera_keys reads no
     * argument. it calls the function by its own name. only symbols.txt places
     * the second name, which this side's objects don't define */
    race_clear_camera_keys();
#else
    /* @bug mismatched-call: passes race_clear_camera_keys a 0, where it
     * takes no argument, at 0x0c9c9088 */
    race_clear_camera_keys_as_called(0);
#endif
    for (index = 0; index < RACE_ENTRANTS; index++) {
        race_set_camera_step(index, race_camera_follow);
    }
}

void race_run_editor(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->submode) {
    case 0:
        race_open_editor();
        work->submode++;
        break;
    }
    /* @hack redundant: a test of the editor and the third pad's B, with
     * nothing under it */
    if (!race_editor && (pad_press_repeat[EDIT_PAD_INDEX] & PDD_DGT_TB)) {
    }
}
