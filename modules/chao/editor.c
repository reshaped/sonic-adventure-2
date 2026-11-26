/* the garden's editor: its task, its menu and the pages the menu opens */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body.h"
#include "chao/editor.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/pad.h"
#include "system/task.h"

#include "camera/stack.h"
#include "chao/body_collision.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/garden_camera.h"
#include "chao/garden_save.h"
#include "chao/knowledge.h"
#include "chao/model.h"
#include "chao/motion.h"
#include "chao/parts.h"
#include "chao/race.h"
#include "chao/race_state.h"
#include "chao/small_toys.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "object/animal.h"
#include "player/player.h"
#include "sound/sound.h"
#include <stdlib.h>

#define EDITOR_PAD 2 /* port C */
/* PRESSED and HELD read pad_press_repeat and pad_held, which take the
 * stick's directions too. PRESSED repeats a held direction. PAD_PRESSED and
 * PAD_HELD read the pad as pdGetPeripheral gives it */
#define PRESSED(button) (pad_press_repeat[EDITOR_PAD] & (button))
#define HELD(button) (pad_held[EDITOR_PAD] & (button))
#define PAD_PRESSED(button) (player_pads[EDITOR_PAD]->press & (button))
#define PAD_HELD(button) (player_pads[EDITOR_PAD]->on & (button))

/* the editor's task work. ang.x indexes the selected Chao among the
 * garden's live Chao. ang.y is the menu's row. mode is EDITOR_START or
 * EDITOR_MENU, or on a page EDITOR_PAGE plus the page's index. a page keeps
 * its own state in submode, which is 0 when the page opens */
enum editor_mode { EDITOR_START, EDITOR_MENU, EDITOR_PAGE };
#define CHAO_INDEX(selection) ((selection)->ang.x)
#define MENU_ROW(selection) ((selection)->ang.y)
#define SELECTED_CHAO(selection)                                               \
    garden_active_object(GARDEN_OBJECT_CHAO, CHAO_INDEX(selection))
/* select_chao sets this flag each frame. it draws the selected Chao's marker
 * only on a frame that finds the flag set. the free camera page clears it each
 * frame, which hides the marker */
#define SHOW_MARKER 0x100

/* phase runs on without a reset, past Sint32's top. njSin reads an angle's
 * low 16 bits alone, so the wrap changes nothing */
#ifdef NONMATCHING
/* this side adds in Uint32, whose wrap C defines */
#define ADVANCE_PHASE(phase, delta)                                            \
    ((phase) = (Angle)((Uint32)(phase) + (delta)))
#else
#define ADVANCE_PHASE(phase, delta) ((phase) += (delta))
#endif

#ifdef NONMATCHING
/* this side's macros expand to nothing. the empty tests and unread reads that
 * the hack tags below name serve only SHC's bytes */
#define EMPTY_TEST(condition)
#define EMPTY_IF_ELSE(condition)
#define UNREAD(expression)
#define UNREAD_PROFILE()
#define UNREAD_CHAO()
#else
/* @hack redundant: a test with nothing under it */
#define EMPTY_TEST(condition)                                                  \
    if (condition) {                                                           \
    }
/* @hack redundant: a test with nothing under either arm */
#define EMPTY_IF_ELSE(condition)                                               \
    if (condition) {                                                           \
    } else {                                                                   \
    }
/* @hack redundant: a read whose value nothing uses */
#define UNREAD(expression) ((void)(expression))
/* @hack redundant: the Chao's work and parameters, which nothing reads */
#define UNREAD_PROFILE()                                                       \
    struct chao_work *work = CHAO_WORK(chao);                                  \
    struct chao_parameters *parameters = &work->profile.parameters;
/* @hack redundant: the selected Chao and its profile, which nothing reads */
#define UNREAD_CHAO()                                                          \
    struct task *chao = SELECTED_CHAO(selection);                              \
    UNREAD_PROFILE()
#endif
#define IGNORE_B() EMPTY_TEST(PRESSED(PDD_DGT_TB))

enum growth_graph { EVOLUTION_GRAPH, SKILL_GRAPH, ALIGNMENT_GRAPH };
/* the growth page's submode. in CHOOSING_GRAPH, left and right choose a
 * graph, and A starts editing it. in EDITING_GRAPH, the D-pad changes the
 * values the graph shows, and B stops */
enum growth_submode { CHOOSING_GRAPH, EDITING_GRAPH };

#define IDLE_FRAME 0x44ffff00
#define ACTIVE_FRAME 0xaaffff00
#define GRAPH_BACKGROUND 0x7f000000
#define GROWTH_CURSOR 0xffff0000
#define CURSOR_RADIUS 6
/* the pixels a cursor moves from its graph's origin for a value of 1. a skill
 * is a run_power or a fly_swim */
#define EVOLUTION_SCALE 96.0F
#define SKILL_SCALE 32.0F
#define ALIGNMENT_SCALE 48.0F
#define EVOLUTION_X 144
#define EVOLUTION_Y 156
#define SKILL_X 348
#define SKILL_Y 108
#define ALIGNMENT_X 528
#define ALIGNMENT_Y 108
#define EVOLUTION_AXIS 0xffff7f00
#define SKILL_SHADE 0x2f000000
#define SKILL_AXIS 0xff00ff00
#define ALIGNMENT_AXIS 0xff3333ff
/* the pulse's phase turns PULSE_STEP a frame */
#define PULSE_STEP 0x600
/* a radius of 3 to 9 pixels, with the phase */
#define PULSE_RADIUS() ((int)((njSin(editor_growth_phase) + 1.0F) * 3.25F) + 3)
#define DRAW_CURSOR(x, y, radius)                                              \
    garden_draw_rect((x) - (radius), (y) - (radius), (x) + (radius),           \
                     (y) + (radius), 0.5F, GROWTH_CURSOR)

void chao_edit_growth(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    int power_x = parameters->run_power * SKILL_SCALE;
    int flight_y = -(parameters->fly_swim * SKILL_SCALE);
    int alignment_y = -(parameters->alignment * ALIGNMENT_SCALE);
    int evolution_y = -(parameters->evolution * EVOLUTION_SCALE);
    int evolution_radius;
    int skill_radius;
    int alignment_radius;

    ADVANCE_PHASE(editor_growth_phase, PULSE_STEP);
    switch (selection->submode) {
    case CHOOSING_GRAPH:
        if (PRESSED(PDD_DGT_TA)) {
            selection->submode++;
        }
        IGNORE_B();
        if (PRESSED(PDD_DGT_KR)) {
            editor_growth_graph++;
            if (editor_growth_graph > ALIGNMENT_GRAPH) {
                editor_growth_graph = ALIGNMENT_GRAPH;
            }
        }
        if (PRESSED(PDD_DGT_KL)) {
            editor_growth_graph--;
            if (editor_growth_graph < EVOLUTION_GRAPH) {
                editor_growth_graph = EVOLUTION_GRAPH;
            }
        }
        switch (editor_growth_graph) {
        case EVOLUTION_GRAPH:
            garden_draw_rect(126, 55, 162, 161, 0.4F, IDLE_FRAME);
            break;
        case SKILL_GRAPH:
            garden_draw_rect(295, 55, 401, 161, 0.4F, IDLE_FRAME);
            break;
        case ALIGNMENT_GRAPH:
            garden_draw_rect(510, 55, 546, 161, 0.4F, IDLE_FRAME);
            break;
        }
        break;
    case EDITING_GRAPH:
        if (PRESSED(PDD_DGT_TB)) {
            selection->submode--;
        }
        switch (editor_growth_graph) {
        case EVOLUTION_GRAPH:
            if (HELD(PDD_DGT_KU)) {
                chao_add_evolution(chao, 0.01F);
            }
            if (HELD(PDD_DGT_KD)) {
                chao_add_evolution(chao, -0.01F);
            }
            garden_draw_rect(126, 55, 162, 161, 0.4F, ACTIVE_FRAME);
            break;
        case SKILL_GRAPH:
            if (HELD(PDD_DGT_KL)) {
                chao_add_run_power(chao, -0.02F);
            }
            if (HELD(PDD_DGT_KR)) {
                chao_add_run_power(chao, 0.02F);
            }
            if (HELD(PDD_DGT_KU)) {
                chao_add_fly_swim(chao, 0.02F);
            }
            if (HELD(PDD_DGT_KD)) {
                chao_add_fly_swim(chao, -0.02F);
            }
            garden_draw_rect(295, 55, 401, 161, 0.4F, ACTIVE_FRAME);
            break;
        case ALIGNMENT_GRAPH:
            if (HELD(PDD_DGT_KU)) {
                chao_add_alignment(chao, 0.02F);
            }
            if (HELD(PDD_DGT_KD)) {
                chao_add_alignment(chao, -0.02F);
            }
            garden_draw_rect(510, 55, 546, 161, 0.4F, ACTIVE_FRAME);
            break;
        }
        break;
    }
    njPrintSize(24);
    njPrintC(NJM_LOCATION(4, 1), "GROW");
    njPrintC(NJM_LOCATION(4, 7), "ZERO");
    njPrintF(NJM_LOCATION(2, 8), parameters->evolution, 5);
    njPrintC(NJM_LOCATION(14, 1), "F");
    njPrintC(NJM_LOCATION(14, 7), "S");
    njPrintC(NJM_LOCATION(11, 4), "R");
    njPrintC(NJM_LOCATION(17, 4), "P");
    njPrintF(NJM_LOCATION(4, 8), parameters->run_power, 5);
    njPrintF(NJM_LOCATION(4, 9), parameters->fly_swim, 5);
    njPrintC(NJM_LOCATION(20, 1), "HERO");
    njPrintC(NJM_LOCATION(20, 7), "DARK");
    njPrintF(NJM_LOCATION(4, 8), parameters->alignment, 5);
    njPrintSize(12);
    garden_draw_rect(131, 60, 157, 156, 0.5F, GRAPH_BACKGROUND);
    garden_draw_rect(143, 60, 145, 156, 0.5F, EVOLUTION_AXIS);
    garden_draw_rect(316, 76, 380, 140, 0.5F, SKILL_SHADE);
    garden_draw_rect(300, 60, 396, 156, 0.5F, GRAPH_BACKGROUND);
    garden_draw_rect(300, 107, 396, 109, 0.5F, SKILL_AXIS);
    garden_draw_rect(347, 60, 349, 156, 0.5F, SKILL_AXIS);
    garden_draw_rect(515, 60, 541, 156, 0.5F, GRAPH_BACKGROUND);
    garden_draw_rect(527, 60, 529, 156, 0.5F, ALIGNMENT_AXIS);
    garden_draw_rect(515, 107, 541, 109, 0.5F, ALIGNMENT_AXIS);

#ifdef NONMATCHING
    /* this side starts each radius at CURSOR_RADIUS. the switch below sets
     * them only for the submodes and graphs it names */
    evolution_radius = skill_radius = alignment_radius = CURSOR_RADIUS;
#endif
    switch (selection->submode) {
    case CHOOSING_GRAPH:
        evolution_radius = CURSOR_RADIUS;
        skill_radius = CURSOR_RADIUS;
        alignment_radius = CURSOR_RADIUS;
        break;
    case EDITING_GRAPH:
        switch (editor_growth_graph) {
        case EVOLUTION_GRAPH:
            evolution_radius = PULSE_RADIUS();
            skill_radius = CURSOR_RADIUS;
            alignment_radius = CURSOR_RADIUS;
            break;
        case SKILL_GRAPH:
            evolution_radius = CURSOR_RADIUS;
            skill_radius = PULSE_RADIUS();
            alignment_radius = CURSOR_RADIUS;
            break;
        case ALIGNMENT_GRAPH:
            evolution_radius = CURSOR_RADIUS;
            skill_radius = CURSOR_RADIUS;
            alignment_radius = PULSE_RADIUS();
            break;
        }
        break;
    }
    DRAW_CURSOR(EVOLUTION_X, evolution_y + EVOLUTION_Y, evolution_radius);
    DRAW_CURSOR(power_x + SKILL_X, flight_y + SKILL_Y, skill_radius);
    DRAW_CURSOR(ALIGNMENT_X, alignment_y + ALIGNMENT_Y, alignment_radius);
    if (!HELD(PDD_DGT_TA)) {
        chao_step_body(chao);
        chao_icon_step(chao);
    }
}

#define EDITOR_WHITE 0xffffffff
#define PART_SELECTION 0xffff9f00
#define FLAG_SELECTION 0xffffa000

#define PREVIOUS_CHOICE(value, count)                                          \
    if ((value) > 0) {                                                         \
        (value)--;                                                             \
    } else {                                                                   \
        (value) = (count) - 1;                                                 \
    }
#define NEXT_CHOICE(value, count)                                              \
    if ((value) < (count) - 1) {                                               \
        (value)++;                                                             \
    } else {                                                                   \
        (value) = 0;                                                           \
    }
#define SELECT_ROW(rows)                                                       \
    if (PRESSED(PDD_DGT_KU)) {                                                 \
        PREVIOUS_CHOICE(selection->submode, rows)                              \
    }                                                                          \
    if (PRESSED(PDD_DGT_KD)) {                                                 \
        NEXT_CHOICE(selection->submode, rows)                                  \
    }

/* the hats, costumes, textures and colors the pages offer, each from 0 for
 * none */
#define HATS 4
#define COSTUMES 2
#define TEXTURES 15
#define COLORS 13

#ifdef NONMATCHING
/* this side shifts an unsigned 1, the plain form the hack tag names */
#define GARDEN_PROGRESS_BIT(row) (1U << (row))
/* this side prints ---- for a value past the end of names. the matching side
 * reads past it */
#define GEAR_LABEL(names, value)                                               \
    ((value) < sizeof(names) / sizeof((names)[0]) ? (names)[value] : "----")
/* this side keeps the result of the arithmetic bug tagged in the matching side.
 * it records nothing for an emptied slot, and makes no shift by -1 */
#define RECORD_MINIMAL(parameters, slot)                                       \
    if ((parameters)->animal_parts[slot] >= 0) {                               \
        chao_record_eaten_minimal(chao, (parameters)->animal_parts[slot]);     \
    }
/* this side repairs the memory bug tagged in the matching side. L on 0 wraps to
 * count - 1 */
#define STEP_BACK_MATERIAL(value, count) PREVIOUS_CHOICE(value, count)
#else
/* @hack type: the bit shifts a signed 1, where 1U would suit the Uint32 mask
 * garden_flag_set takes */
#define GARDEN_PROGRESS_BIT(row) (1 << (row))
#define GEAR_LABEL(names, value) ((names)[value])
/* @bug arithmetic: a change that empties a slot shifts 1 by its kind, -1, in
 * chao_record_eaten_minimal. the call is at 0x0c50486a, and the shift at
 * 0x0c514efc gives 0, so it records nothing */
#define RECORD_MINIMAL(parameters, slot)                                       \
    chao_record_eaten_minimal(chao, (parameters)->animal_parts[slot]);
/* @bug memory: L on 0 leaves the texture or the color at 255, and
 * chao_apply_body_material reads past its tables. the test of the Uint8 for
 * a negative value never holds, at 0x0c504644 for the texture and at
 * 0x0c5046fe for the color */
#define STEP_BACK_MATERIAL(value, count)                                       \
    (value)--;                                                                 \
    if ((value) < 0) {                                                         \
        (value) = (count) - 1;                                                 \
    }
#endif

void chao_edit_texture(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_parameters *parameters = &work->profile.parameters;
#ifdef NONMATCHING
#else
    struct chao_shape_work *shape = &work->shape;
#endif

    IGNORE_B();
    if (PRESSED(PDD_DGT_KL)) {
        STEP_BACK_MATERIAL(parameters->texture, TEXTURES)
    }
    if (PRESSED(PDD_DGT_KR)) {
        parameters->texture++;
        if (parameters->texture >= TEXTURES) {
            parameters->texture = 0;
        }
    }
    njPrintD(NJM_LOCATION(26, 10), parameters->texture, 2);
    EMPTY_IF_ELSE(shape->flags & CHAO_SHAPE_JEWEL);
    if (parameters->texture == 0) {
        chao_clear_jewel_texture(chao);
    } else {
        chao_apply_body_material(chao);
    }
}

void chao_edit_color(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_parameters *parameters = &work->profile.parameters;

    IGNORE_B();
    if (PRESSED(PDD_DGT_KR)) {
        parameters->color++;
        if (parameters->color >= COLORS) {
            parameters->color = 0;
        }
    }
    if (PRESSED(PDD_DGT_KL)) {
        STEP_BACK_MATERIAL(parameters->color, COLORS)
    }
    chao_apply_body_material(chao);
}

void chao_edit_animal_parts(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Bool changed = FALSE;
    int kind;
    int i;

    UNREAD(chao->work);
    SELECT_ROW(CHAO_PART_SLOTS)
    if (PRESSED(PDD_DGT_KL)) {
        changed = TRUE;
        if (parameters->animal_parts[selection->submode] > CHAO_NO_PART) {
            parameters->animal_parts[selection->submode]--;
        } else {
            parameters->animal_parts[selection->submode] = ANIMAL_KINDS - 1;
        }
    }
    if (PRESSED(PDD_DGT_KR)) {
        changed = TRUE;
        if (parameters->animal_parts[selection->submode] < ANIMAL_KINDS - 1) {
            parameters->animal_parts[selection->submode]++;
        } else {
            parameters->animal_parts[selection->submode] = CHAO_NO_PART;
        }
    }
    if (PAD_PRESSED(PDD_DGT_TA)) {
        changed = TRUE;
        parameters->animal_parts[selection->submode] = CHAO_NO_PART;
    }
    if (PAD_HELD(PDD_DGT_TX)) {
        kind = parameters->animal_parts[selection->submode];
        for (i = 0; i < CHAO_PART_SLOTS; i++) {
            parameters->animal_parts[i] = kind;
            chao_set_animal_part(chao, i, kind);
        }
    }
    if (changed) {
        RECORD_MINIMAL(parameters, selection->submode)
        chao_set_animal_part(chao, selection->submode,
                             parameters->animal_parts[selection->submode]);
    }
    {
        const char *parts[CHAO_PART_SLOTS] = {"ARM", "EAR",  "FRHD", "HORN",
                                              "REG", "TAIL", "WING", "TONG"};
        const char *animals[ANIMAL_KINDS] = {
            "PEN",  "GOMA", "RAKO", "USA", "CHIT", "INO", "KUMA",
            "TORA", "GORI", "KUJA", "OUM", "KON",  "SKA", "SHEP",
            "ARA",  "HAN",  "DOG",  "BAT", "DRA",  "UNI", "PHEN"};
        for (i = 0; i < CHAO_PART_SLOTS; i++) {
            if (i == selection->submode) {
                njPrintColor(PART_SELECTION);
            } else {
                njPrintColor(EDITOR_WHITE);
            }
            njPrintC(NJM_LOCATION(20, i + 10), parts[i]);
            if (parameters->animal_parts[i] < 0) {
                njPrintC(NJM_LOCATION(25, i + 10), "----");
            } else {
                njPrintC(NJM_LOCATION(25, i + 10),
                         animals[parameters->animal_parts[i]]);
            }
        }
        njPrintColor(EDITOR_WHITE);
        IGNORE_B();
    }
}

enum costume_row { HEAD_ROW, BODY_ROW, COSTUME_ROWS };

void chao_edit_costume(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_parameters *parameters = &work->profile.parameters;
    int row;

    SELECT_ROW(COSTUME_ROWS)
    switch (selection->submode) {
    case HEAD_ROW:
        if (PRESSED(PDD_DGT_KL)) {
            PREVIOUS_CHOICE(parameters->hat, HATS)
        }
        if (PRESSED(PDD_DGT_KR)) {
            NEXT_CHOICE(parameters->hat, HATS)
        }
        break;
    case BODY_ROW:
        if (PRESSED(PDD_DGT_KL)) {
            PREVIOUS_CHOICE(parameters->costume, COSTUMES)
        }
        if (PRESSED(PDD_DGT_KR)) {
            NEXT_CHOICE(parameters->costume, COSTUMES)
        }
        break;
    }
    {
        const char *rows[COSTUME_ROWS] = {"HEAD", "BODY"};
        const char *hats[HATS] = {"NONE", "PUMPKIN", "SKULL", "EGG"};
        const char *costumes[COSTUMES] = {"NONE", "SPECTOR"};
        for (row = 0; row < COSTUME_ROWS; row++) {
            if (row == selection->submode) {
                njPrintColor(PART_SELECTION);
            } else {
                njPrintColor(EDITOR_WHITE);
            }
            njPrintC(NJM_LOCATION(20, row + 10), rows[row]);
            switch (row) {
            case HEAD_ROW:
                njPrintC(NJM_LOCATION(25, row + 10),
                         GEAR_LABEL(hats, parameters->hat));
                break;
            case BODY_ROW:
                njPrintC(NJM_LOCATION(25, row + 10),
                         GEAR_LABEL(costumes, parameters->costume));
                break;
            }
        }
        njPrintColor(EDITOR_WHITE);
        IGNORE_B();
    }
}

/* the form page's rows: the child, then each adult form at its number less
 * FORM_ROW_OFFSET */
#define FORM_ROW_OFFSET (CHAO_NORMAL_ADULT - 1)
#define FORM_ROWS 19

void chao_edit_form(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_work *work = CHAO_WORK(chao);
    int form;
    int row;

    UNREAD(chao->work);
    IGNORE_B();
    SELECT_ROW(FORM_ROWS)
    if (PAD_PRESSED(PDD_DGT_TA)) {
        switch (selection->submode) {
        case 0:
            chao_change_form(chao, CHAO_CHILD);
            break;
        default:
            chao_change_form(chao, selection->submode + FORM_ROW_OFFSET);
            break;
        }
    }
    {
        const char *forms[FORM_ROWS] = {
            "CHILD",   "N NORMAL", "H NORMAL", "D NORMAL", "N SWIM",
            "H SWIM",  "D SWIM",   "N FLY",    "H FLY",    "D FLY",
            "N RUN",   "H RUN",    "D RUN",    "N POWER",  "H POWER",
            "D POWER", "N CHAOS",  "H CHAOS",  "D CHAOS"};
        for (row = 0; row < FORM_ROWS; row++) {
            if (row == selection->submode) {
                njPrintColor(PART_SELECTION);
            } else {
                njPrintColor(EDITOR_WHITE);
            }
            njPrintC(NJM_LOCATION(20, row + 10), forms[row]);
        }
        njPrintColor(EDITOR_WHITE);
        /* @hack spelling: the type read through a pointer to the parameters,
         * where work->profile.parameters.type would do */
        {
            struct chao_parameters *parameters = &work->profile.parameters;
            form = parameters->type;
        }
        njPrintC(NJM_LOCATION(15, 8), "CURR TYPE");
        if (form >= CHAO_NORMAL_ADULT) {
            njPrintC(NJM_LOCATION(26, 8), forms[form - FORM_ROW_OFFSET]);
        } else {
            njPrintC(NJM_LOCATION(26, 8), "CHILD");
        }
    }
}

#define FLAG_COLOR()                                                           \
    if (selection->submode == row) {                                           \
        njPrintColor(FLAG_SELECTION);                                          \
    } else {                                                                   \
        njPrintColor(EDITOR_WHITE);                                            \
    }
/* print count labels from line first_line down. each reads ON where enabled
 * holds for its row, and OFF where it doesn't */
#define SHOW_FLAGS(count, first_line, enabled)                                 \
    for (row = 0; row < (count); row++) {                                      \
        FLAG_COLOR()                                                           \
        njPrintC(NJM_LOCATION(20, row + (first_line)), labels[row]);           \
        if (enabled) {                                                         \
            njPrintC(NJM_LOCATION(35, row + (first_line)), "ON");              \
        } else {                                                               \
            njPrintC(NJM_LOCATION(35, row + (first_line)), "OFF");             \
        }                                                                      \
    }                                                                          \
    njPrintColor(EDITOR_WHITE)

#define ARTS 5
#define DANCES 5
#define SONGS 5
#define INSTRUMENTS 8
#define EDIT_LESSON(learn, count)                                              \
    IGNORE_B();                                                                \
    SELECT_ROW(count)                                                          \
    if (PAD_PRESSED(PDD_DGT_TA)) {                                             \
        learn(chao, selection->submode);                                       \
    }

void chao_edit_small_toys(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    UNREAD_PROFILE()
    int row;

    EDIT_LESSON(chao_add_small_toy, CHAO_SMALL_TOYS)
    {
        const char *labels[CHAO_SMALL_TOYS] = {
            "GARAGARA", "CAR",     "BOOK",   "-----",  "DOLL", "HOUKI",
            "-----",    "HOPPING", "CRAYON", "SYABON", "SCOP", "JYOURO"};
        SHOW_FLAGS(CHAO_SMALL_TOYS, 20, chao_has_small_toy(chao, row));
    }
}
void chao_edit_art(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    UNREAD_PROFILE()
    int row;

    EDIT_LESSON(chao_learn_art, ARTS)
    {
        const char *labels[ARTS] = {"ART 1", "ART 2", "ART 3", "ART 4",
                                    "ART 5"};
        SHOW_FLAGS(ARTS, 20, chao_knows_art(chao, row));
    }
}
void chao_edit_dances(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    UNREAD_PROFILE()
    int row;

    EDIT_LESSON(chao_learn_dance, DANCES)
    {
        const char *labels[DANCES] = {"FURIRFURI", "KURUKURU", "BOX", "GOGO",
                                      "RADIO"};
        SHOW_FLAGS(DANCES, 20, chao_knows_dance(chao, row));
    }
}
void chao_edit_songs(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    UNREAD_PROFILE()
    int row;

    EDIT_LESSON(chao_learn_song, SONGS)
    {
        const char *labels[SONGS] = {"SONG 1", "SONG 2", "SONG 3", "SONG 4",
                                     "SONG 5"};
        SHOW_FLAGS(SONGS, 20, chao_knows_song(chao, row));
    }
}
void chao_edit_instruments(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    int row;

    EDIT_LESSON(chao_learn_instrument, INSTRUMENTS)
    {
        const char *labels[INSTRUMENTS] = {"BELL",  "CASTANETS", "CYMBAL",
                                           "DRUM",  "FUE",       "MARACAS",
                                           "RAPPA", "TAMBOURINE"};
        SHOW_FLAGS(INSTRUMENTS, 20, chao_knows_instrument(chao, row));
    }
}

#pragma inline(garden_toggle_debug_flag)
void garden_toggle_debug_flag(Bool *flag)
{
    if (*flag) {
        *flag = FALSE;
    } else {
        *flag = TRUE;
    }
}
/* garden_toggle_debug_flag's toggle, inlined into the display page */
#pragma inline(toggle_display)
static void toggle_display(Bool *flag)
{
    if (*flag) {
        *flag = FALSE;
    } else {
        /* @hack spelling: TRUE set through a local, where a call of
         * garden_toggle_debug_flag in place of toggle_display would do */
        Bool enabled = TRUE;
        *flag = enabled;
    }
}

enum display_row { ENTRIES_ROW, BOUNDARIES_ROW, FIELDS_ROW, DISPLAY_ROWS };

void garden_edit_display(struct task *editor)
{
    struct task_work *selection = editor->work;
    const char *state;
    int line;
    int row;

    IGNORE_B();
    SELECT_ROW(DISPLAY_ROWS)
    if (PAD_PRESSED(PDD_DGT_TA)) {
        switch (selection->submode) {
        case ENTRIES_ROW:
            toggle_display(&chao_tuning.show_entries);
            break;
        case BOUNDARIES_ROW:
            toggle_display(&chao_tuning.show_boundaries);
            break;
        case FIELDS_ROW:
            toggle_display(&chao_tuning.show_fields);
            break;
        }
    }
    {
        const char *labels[DISPLAY_ROWS + 1] = {
            "DISP ENTRY", "DISP BOUNDARY BOX", "DISP FIELD", NULL};
        /* @hack spelling: row = row + 1, where row++ would do */
        for (row = 0; row < DISPLAY_ROWS; row = row + 1) {
            njPrintColor(selection->submode == row ? FLAG_SELECTION
                                                   : EDITOR_WHITE);
            njPrintC(NJM_LOCATION(20, row + 20), labels[row]);
            line = row + 20;
            switch (row) {
            case ENTRIES_ROW:
                if (chao_tuning.show_entries) {
                    njPrintC(NJM_LOCATION(35, line), "ON");
                } else {
                    njPrintC(NJM_LOCATION(35, line), "OFF");
                }
                break;
            case BOUNDARIES_ROW:
                if (chao_tuning.show_boundaries) {
                    njPrintC(NJM_LOCATION(35, line), "ON");
                } else {
                    njPrintC(NJM_LOCATION(35, line), "OFF");
                }
                break;
            case FIELDS_ROW:
                if (chao_tuning.show_fields) {
                    state = "ON";
                } else {
                    state = "OFF";
                }
                njPrintC(NJM_LOCATION(35, line), state);
                break;
            }
        }
        njPrintColor(EDITOR_WHITE);
    }
}

#define GARDEN_FLAGS 32 /* the bits of garden_flag_set's mask */

void garden_edit_flags(struct task *editor)
{
    struct task_work *selection = editor->work;
    int row;

    IGNORE_B();
    SELECT_ROW(GARDEN_FLAGS)
    if (PAD_PRESSED(PDD_DGT_TA)) {
        garden_flag_set(GARDEN_PROGRESS_BIT(selection->submode));
    }
    {
        const char *labels[GARDEN_FLAGS + 1] = {
            "PLAY",      "NEUT_PLAY", "KINDER_OPEN", "KINDER_PLAY", "HERO_OPEN",
            "HERO_PLAY", "DARK_OPEN", "DARK_PLAY",   "EMPTY08",     "EMPTY09",
            "EMPTY10",   "EMPTY11",   "EMPTY12",     "EMPTY13",     "EMPTY14",
            "EMPTY15",   "EMPTY16",   "EMPTY17",     "EMPTY18",     "EMPTY19",
            "EMPTY20",   "EMPTY21",   "EMPTY22",     "EMPTY23",     "EMPTY24",
            "EMPTY25",   "EMPTY26",   "EMPTY27",     "EMPTY28",     "EMPTY29",
            "EMPTY30",   "EMPTY31",   NULL};
        SHOW_FLAGS(GARDEN_FLAGS, 4, garden_flag_test(GARDEN_PROGRESS_BIT(row)));
    }
}

void garden_edit_race_groups(struct task *editor)
{
    struct task_work *selection = editor->work;
    int row;

    /* @bug logic: garden_race_restore never runs. editor_step runs a page
     * only where mode is neither EDITOR_START nor EDITOR_MENU, so the test at
     * 0x0c505e82 never holds */
    switch (selection->mode) {
    case EDITOR_START:
        garden_race_restore();
        selection->mode++;
        break;
    }
    IGNORE_B();
    SELECT_ROW(RACE_GROUPS)
    if (PAD_PRESSED(PDD_DGT_TA)) {
        garden_race_unlock_group(selection->submode);
        garden_gather_race();
    }
    {
        const char *labels[RACE_GROUPS + 1] = {
            "BEGINNER", "JEWEL", "CHALLENGE", "HERO", "DARK", "MULTI", NULL};
        SHOW_FLAGS(RACE_GROUPS, 4,
                   garden_race_group_status(row) > RACE_GROUP_HIDDEN);
    }
}

#define AWARDS 12

void garden_edit_awards(struct task *editor)
{
    struct task_work *selection = editor->work;
    int row;

    IGNORE_B();
    SELECT_ROW(AWARDS)
    if (PAD_PRESSED(PDD_DGT_TA)) {
        garden_add_award(selection->submode);
    }
    {
        const char *labels[AWARDS + 1] = {
            "TV",   "RADICASE", "BOX",  "BALL_N", "BALL_H", "BALL_D", "HORSE",
            "----", "----",     "----", "PIANO",  "ORGAN",  NULL};
        SHOW_FLAGS(AWARDS, 20, garden_has_award(row));
    }
}

/* the motion pages color the chosen motion's line and MOTION_NEIGHBORS lines
 * on either side, wrapping through CHAO_MOTIONS. they print none of them */
#define MOTION_NEIGHBORS 5
#define MOTION_LINES ((2 * MOTION_NEIGHBORS) + 1)
#define OTHER_MOTION 0xffafafaf

void chao_edit_motion_frame(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    Float frame;
    int motion;
    int line;
    int row;

    IGNORE_B();
    frame = chao_motion_frame(chao);
    motion = chao_motion_index(chao);
    if (HELD(PDD_DGT_KL)) {
        frame -= 0.05F;
        chao_set_motion_frame(chao, frame);
    } else if (HELD(PDD_DGT_KR)) {
        frame += 0.05F;
        chao_set_motion_frame(chao, frame);
    }
    if (PRESSED(PDD_DGT_KU)) {
        motion--;
        if (motion < 0) {
            motion = CHAO_MOTIONS - 1;
        }
        chao_set_motion(chao, motion);
    } else if (PRESSED(PDD_DGT_KD)) {
        motion++;
        if (motion > CHAO_MOTIONS - 1) {
            motion = 0;
        }
        chao_set_motion(chao, motion);
    }
    motion -= MOTION_NEIGHBORS;
    if (motion < 0) {
        motion += CHAO_MOTIONS;
    }
    for (row = 0; row < MOTION_LINES; row++) {
        /* @hack redundant: line, which nothing prints */
        line = motion + row;
        if (line > CHAO_MOTIONS - 1) {
            line -= CHAO_MOTIONS;
        }
        if (row != MOTION_NEIGHBORS) {
            njPrintColor(OTHER_MOTION);
        } else {
            njPrintColor(EDITOR_WHITE);
        }
    }
    chao_step_body(chao);
    chao_icon_step(chao);
}

void chao_edit_motion_speed(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    int line;
    int row;

    IGNORE_B();
    /* @hack redundant: the motion's frame and index, which nothing reads */
    chao_motion_frame(chao);
    chao_motion_index(chao);
    if (PRESSED(PDD_DGT_KU)) {
        editor_motion_index--;
        if (editor_motion_index < 0) {
            editor_motion_index = CHAO_MOTIONS - 1;
        }
    } else if (PRESSED(PDD_DGT_KD)) {
        editor_motion_index++;
        if (editor_motion_index > CHAO_MOTIONS - 1) {
            editor_motion_index = 0;
        }
    }
    if (PRESSED(PDD_DGT_KL)) {
        editor_motion_speed -= 0.05F;
        if (editor_motion_speed < 0.0F) {
            editor_motion_speed = 0.0F;
        }
    } else if (PRESSED(PDD_DGT_KR)) {
        editor_motion_speed += 0.05F;
        if (editor_motion_speed > 5.0F) {
            editor_motion_speed = 5.0F;
        }
    }
    chao_set_motion_speed(chao, editor_motion_speed);
    if (PAD_PRESSED(PDD_DGT_TA)) {
        chao_link_motion(chao, editor_motion_index);
    }
    /* @hack redundant: line, which nothing prints */
    line = editor_motion_index - MOTION_NEIGHBORS;
    if (line < 0) {
        line = line + CHAO_MOTIONS;
    }
    for (row = 0; row < MOTION_LINES; row++) {
        if (line > CHAO_MOTIONS - 1) {
            line -= CHAO_MOTIONS;
        }
        if (row != MOTION_NEIGHBORS) {
            njPrintColor(OTHER_MOTION);
        } else {
            njPrintColor(EDITOR_WHITE);
        }
        line++;
    }
    chao_step_body(chao);
    chao_icon_step(chao);
    chao_motion_step(chao);
}

#define EYES 14
#define MOUTHS 13

#define SELECT_FACE()                                                          \
    struct task_work *selection = editor->work;                                \
    struct task *chao = SELECTED_CHAO(selection);                              \
    struct chao_work *work = CHAO_WORK(chao);                                  \
    struct chao_face *face = &work->face

#define EDIT_EXPRESSION(member, count, apply)                                  \
    IGNORE_B();                                                                \
    if (PRESSED(PDD_DGT_KL)) {                                                 \
        face->member--;                                                        \
        if (face->member < 0) {                                                \
            face->member = (count) - 1;                                        \
        }                                                                      \
        apply(chao, face->member);                                             \
    }                                                                          \
    if (PRESSED(PDD_DGT_KR)) {                                                 \
        face->member++;                                                        \
        if (face->member >= (count)) {                                         \
            face->member = 0;                                                  \
        }                                                                      \
        apply(chao, face->member);                                             \
    }                                                                          \
    njPrintD(NJM_LOCATION(20, 10), face->member, 2)

void chao_edit_eyes(struct task *editor)
{
    SELECT_FACE();
    EDIT_EXPRESSION(eye, EYES, chao_set_eye);
}

void chao_edit_eyelids(struct task *editor)
{
    SELECT_FACE();

    IGNORE_B();
    if (HELD(PDD_DGT_KU)) {
        face->default_closure += 0x100;
        face->closure = face->target_closure = face->default_closure;
    }
    if (HELD(PDD_DGT_KD)) {
        face->default_closure -= 0x100;
        face->closure = face->target_closure = face->default_closure;
    }
    if (HELD(PDD_DGT_KL)) {
        face->default_slope += 0x100;
        face->slope = face->target_slope = face->default_slope;
    }
    if (HELD(PDD_DGT_KR)) {
        face->default_slope -= 0x100;
        face->slope = face->target_slope = face->default_slope;
    }
}

void chao_edit_mouth(struct task *editor)
{
    SELECT_FACE();
    EDIT_EXPRESSION(mouth, MOUTHS, chao_set_mouth);
}

/* @hack spelling: the flag written through chao->work, where work holds the
 * same */
#define TOGGLE_ACTOR_FLAG(mask)                                                \
    if (PRESSED(PDD_DGT_TA)) {                                                 \
        if (work->flags & (mask)) {                                            \
            CHAO_WORK(chao)->flags &= ~(mask);                                 \
        } else {                                                               \
            CHAO_WORK(chao)->flags |= (mask);                                  \
        }                                                                      \
    }
#define ACTOR_SELECTION 0xffff8888
#define ACTOR_OTHER 0xffcccccc
#define ACTOR_FLAG_COLOR(row)                                                  \
    if (selection->submode == (row)) {                                         \
        njPrintColor(ACTOR_SELECTION);                                         \
    } else {                                                                   \
        njPrintColor(ACTOR_OTHER);                                             \
    }

#define LAST_FLAG_ROW 2
void chao_edit_actor_flags(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao = SELECTED_CHAO(selection);
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape;

    IGNORE_B();
    switch (selection->submode) {
    case 0:
        TOGGLE_ACTOR_FLAG(CHAO_ANIMATES)
        break;
    case 1:
        TOGGLE_ACTOR_FLAG(CHAO_MOVES)
        break;
    case 2:
        TOGGLE_ACTOR_FLAG(CHAO_RUNS_BEHAVIOR)
        break;
    case 3:
        /* no frame reaches row 3: down stops at LAST_FLAG_ROW */
        shape = &work->shape;
        if (!(shape->flags & 0x80)) {
            shape->flags |= 0x80;
        } else {
            shape->flags &= ~0x80;
        }
        break;
    }
    if (PRESSED(PDD_DGT_KU)) {
        if (selection->submode > 0) {
            selection->submode--;
        }
    }
    if (PRESSED(PDD_DGT_KD)) {
        if (selection->submode < LAST_FLAG_ROW) {
            selection->submode++;
        }
    }
    ACTOR_FLAG_COLOR(0)
    EMPTY_TEST(work->flags & CHAO_ANIMATES);
    ACTOR_FLAG_COLOR(1)
    EMPTY_TEST(work->flags & CHAO_MOVES);
    ACTOR_FLAG_COLOR(2)
    EMPTY_TEST(work->flags & CHAO_RUNS_BEHAVIOR);
    ACTOR_FLAG_COLOR(3)
#ifdef NONMATCHING
#else
    shape = &work->shape;
    EMPTY_TEST(shape->flags & 0x80);
#endif
}

void garden_edit_free_camera(struct task *editor)
{
    struct task_work *selection = editor->work;
    UNREAD_CHAO()

    selection->flags &= ~SHOW_MARKER;
    switch (selection->submode) {
    case 0:
        camera_push_user_mode(editor_camera_index, garden_free_camera);
        selection->submode++;
        break;
    case 1:
        IGNORE_B();
        if (HELD(PDD_DGT_TA)) {
            garden_disable_objects();
        } else {
            garden_enable_objects();
        }
        break;
    }
}

#define MENU_SELECTION 0xffff6f00

void chao_editor_menu(struct task *editor)
{
    struct task_work *selection = editor->work;
    int row;

    if (PRESSED(PDD_DGT_KU)) {
        MENU_ROW(selection)--;
        if (MENU_ROW(selection) < 0) {
            MENU_ROW(selection) += editor_page_count + 1;
        }
    }
    if (PRESSED(PDD_DGT_KD)) {
        MENU_ROW(selection)++;
        if (MENU_ROW(selection) >= editor_page_count + 1) {
            MENU_ROW(selection) = 0;
        }
    }
    if (PRESSED(PDD_DGT_TA)) {
        if (MENU_ROW(selection) == editor_page_count) {
            task_kill(editor);
        } else if (garden_object_count(GARDEN_OBJECT_CHAO) > 0 ||
                   editor_pages[MENU_ROW(selection)].works_without_chao) {
            selection->mode = MENU_ROW(selection) + EDITOR_PAGE;
            selection->submode = 0;
        }
    }
    IGNORE_B();
    njPrintColor(0xff00ff00);
    for (row = 0; row < editor_page_count + 1; row++) {
        if (row == MENU_ROW(selection)) {
            njPrintColor(MENU_SELECTION);
        } else {
            njPrintColor(EDITOR_WHITE);
        }
        EMPTY_TEST(row == editor_page_count);
    }
    garden_draw_rect(246, 54, 390, ((editor_page_count + 6) * 12) + 6, 0.5F,
                     GRAPH_BACKGROUND);
    garden_draw_rect(213, 33, 417, 51, 0.5F, GRAPH_BACKGROUND);
    njPrintColor(EDITOR_WHITE);
}

/* L and R step editor_sound_id between first and last. L at first wraps to
 * left_wrap, and R at last wraps to first */
#define SELECT_SOUND(first, left_wrap, last)                                   \
    if (selection->submode == 0) {                                             \
        editor_sound_id = (first);                                             \
        selection->submode++;                                                  \
    }                                                                          \
    if (PRESSED(PDD_DGT_KL)) {                                                 \
        if (editor_sound_id > (first)) {                                       \
            editor_sound_id--;                                                 \
        } else {                                                               \
            editor_sound_id = (left_wrap);                                     \
        }                                                                      \
    }                                                                          \
    if (PRESSED(PDD_DGT_KR)) {                                                 \
        if (editor_sound_id < (last)) {                                        \
            editor_sound_id++;                                                 \
        } else {                                                               \
            editor_sound_id = (first);                                         \
        }                                                                      \
    }

#define ADJUST_VOLUME()                                                        \
    if (PRESSED(PDD_DGT_KU)) {                                                 \
        editor_volume++;                                                       \
        if (editor_volume > SOUND_LOUDEST) {                                   \
            editor_volume = SOUND_LOUDEST;                                     \
        }                                                                      \
    }                                                                          \
    if (PRESSED(PDD_DGT_KD)) {                                                 \
        editor_volume--;                                                       \
        if (editor_volume < -SOUND_LOUDEST) {                                  \
            editor_volume = -SOUND_LOUDEST;                                    \
        }                                                                      \
    }

#define AUDITION_LABELS(column, text, id_column)                               \
    njPrintC(NJM_LOCATION(column, 20), text);                                  \
    njPrintH(NJM_LOCATION(id_column, 20), editor_sound_id, 4);                 \
    njPrintC(NJM_LOCATION(10, 21), "VOL");                                     \
    njPrintD(NJM_LOCATION(16, 21), editor_volume, 4)

void chao_audition_effects(struct task *editor)
{
    struct task_work *selection = editor->work;
    UNREAD_CHAO()
    IGNORE_B();
    SELECT_SOUND(0x1000, 0x1032, 0x1033);
    if (PAD_PRESSED(PDD_DGT_TA)) {
        sound_play(editor_sound_id, NULL, 0, editor_volume);
    }
    ADJUST_VOLUME();
    AUDITION_LABELS(10, "SE No.", 16);
}

void chao_audition_list_four(struct task *editor)
{
    struct task_work *selection = editor->work;
    UNREAD_CHAO()
    IGNORE_B();
    SELECT_SOUND(0x4006, 0x400d, 0x400d);
    if (PAD_PRESSED(PDD_DGT_TA)) {
        sound_play(editor_sound_id, NULL, 0, editor_volume);
    }
    ADJUST_VOLUME();
    AUDITION_LABELS(10, "SE No.", 16);
}

/* a voice plays at the page's volume plus GARDEN_VOICE_VOLUME, for
 * VOICE_STEPS */
#define VOICE_STEPS 300
void chao_audition_voices(struct task *editor)
{
    struct task_work *selection = editor->work;
    UNREAD_CHAO()
    IGNORE_B();
    SELECT_SOUND(0x6000, 0x60a0, 0x60a1);
    if (PAD_PRESSED(PDD_DGT_TA)) {
#ifdef NONMATCHING
        /* this side plays nothing without player 0, whose position SHC's
         * code reads through null */
        if (player_work[0] == NULL) {
            return;
        }
#endif
        sound_play_at_for(editor_sound_id, editor, 1,
                          editor_volume + GARDEN_VOICE_VOLUME, VOICE_STEPS,
                          &player_work[0]->pos);
    }
    ADJUST_VOLUME();
    AUDITION_LABELS(10, "VOICE No.", 20);
}

void chao_audition_music(struct task *editor)
{
    struct task_work *selection = editor->work;
    UNREAD_CHAO()
    IGNORE_B();
    SELECT_SOUND(0x4000, 0x400f, 0x4010);
    if (PAD_PRESSED(PDD_DGT_TA)) {
        sound_play_sequence(editor_sound_id, NULL, 2, editor_volume);
    }
    ADJUST_VOLUME();
    AUDITION_LABELS(10, "MUSIC No.", 20);
}

void chao_audition_area(struct task *editor)
{
    struct task_work *selection = editor->work;
    Uint32 count;
    UNREAD_CHAO()

    IGNORE_B();
    if (selection->submode == 0) {
        editor_sound_id = 0;
        selection->submode++;
    }
    switch (chao_current_area()) {
    case CHAO_AREA_NEUTRAL:
        count = neutral_garden_sound_count;
        break;
    case CHAO_AREA_HERO:
        count = hero_garden_sound_count;
        break;
    case CHAO_AREA_DARK:
        count = dark_garden_sound_count;
        break;
    case CHAO_AREA_RACE:
        count = race_circuit_sound_count;
        break;
    case CHAO_AREA_ENTRANCE:
        count = race_entrance_sound_count;
        break;
    case CHAO_AREA_KINDERGARTEN:
        count = kindergarten_sound_count;
        break;
    case CHAO_AREA_LOBBY:
        count = race_circuit_sound_count;
        break;
#ifdef NONMATCHING
    /* this side leaves an area the switch doesn't name, where SHC's
     * code reads count unset */
    default:
        return;
#endif
    }
#ifdef NONMATCHING
    /* this side plays nothing in an area without sounds, where count - 1
     * would wrap. the disc-free build gives every area none */
    if (count == 0) {
        return;
    }
#endif
    if (PRESSED(PDD_DGT_KL)) {
        if (editor_sound_id > 0) {
            editor_sound_id--;
        } else {
            editor_sound_id = count - 1;
        }
    }
    if (PRESSED(PDD_DGT_KR)) {
        if ((Uint32)editor_sound_id < count - 1) {
            editor_sound_id++;
        } else {
            editor_sound_id = 0;
        }
    }
    if (PAD_PRESSED(PDD_DGT_TA)) {
        sound_play(editor_sound_id, NULL, 0, editor_volume);
    }
    ADJUST_VOLUME();
    AUDITION_LABELS(8, "STG SE No.", 17);
}

void chao_editor_clear_cameras(void)
{
    int depth;

    /* @hack spelling: a for loop that calls camera_top in its first and
     * third clauses, where a while would call it in its test */
    for (depth = camera_top(editor_camera_index); depth > 1;
         depth = camera_top(editor_camera_index)) {
        camera_remove_mode(editor_camera_index, depth);
    }
}

void chao_editor_return_to_menu(struct task *editor)
{
    struct task_work *selection = editor->work;
    selection->mode = EDITOR_MENU;
    selection->submode = 0;
    camera_push_user_mode(editor_camera_index, garden_fixed_camera);
    garden_disable_objects();
}

void chao_editor_register_resident_page(chao_editor_callback *update,
                                        Sint32 value)
{
#ifdef NONMATCHING
    /* this side refuses a page past EDITOR_PAGE_LIMIT, whose mode would
     * overflow task_work's Sint8 */
    if (editor_page_count >= EDITOR_PAGE_LIMIT) {
        return;
    }
#endif
    editor_pages[editor_page_count].works_without_chao = FALSE;
    editor_pages[editor_page_count].update = update;
    editor_pages[editor_page_count].value = value;
    editor_page_count++;
}

void chao_editor_register_global_page(chao_editor_callback *update,
                                      Sint32 value)
{
#ifdef NONMATCHING
    /* this side registers no page past EDITOR_PAGE_LIMIT, which the matching
     * side writes past */
    if (editor_page_count >= EDITOR_PAGE_LIMIT) {
        return;
    }
#endif
    editor_pages[editor_page_count].works_without_chao = TRUE;
    editor_pages[editor_page_count].update = update;
    editor_pages[editor_page_count].value = value;
    editor_page_count++;
}

void chao_editor_clear_pages(void)
{
    int page;
    for (page = 0; page < EDITOR_PAGE_CAPACITY; page++) {
        editor_pages[page].update = NULL;
        editor_pages[page].value = 0;
    }
    editor_page_count = 0;
}

void fun_0c50739e(void)
{
}

static void editor_draw(struct task *editor)
{
}

static void editor_close(struct task *editor)
{
    int depth;
    garden_enable_objects();
    /* @hack spelling: for (;;) with a break, where a while would do */
    for (;;) {
        depth = camera_top(editor_camera_index);
        if (depth <= editor_entry_camera_depth) {
            break;
        }
        camera_remove_mode(editor_camera_index, depth);
    }
    garden_editor = NULL;
}

/* the selected Chao's marker hangs MARKER_HEIGHT above it. its size swings
 * MARKER_PULSE either side of MARKER_SCALE. its phase turns MARKER_PHASE_STEP
 * a frame. its fill is MARKER_INSET of its edge's size */
#define MARKER_HEIGHT 9.0F
#define MARKER_PHASE_STEP 0x700
#define MARKER_SCALE 1.7F
#define MARKER_PULSE 0.3F
#define MARKER_INSET 0.8F
#define MARKER_EDGE 0xff000000
#define MARKER_FILL 0xffff0000
static void select_chao(struct task *editor)
{
    struct task_work *selection = editor->work;
    struct task *chao;
    Float scale;
    NJS_POINT3 *position;

    if (PRESSED(PDD_DGT_TL)) {
        CHAO_INDEX(selection)--;
        if (CHAO_INDEX(selection) < 0) {
            CHAO_INDEX(selection) = garden_object_count(GARDEN_OBJECT_CHAO) - 1;
        }
    }
    if (PRESSED(PDD_DGT_TR)) {
        CHAO_INDEX(selection)++;
        if (CHAO_INDEX(selection) >= garden_object_count(GARDEN_OBJECT_CHAO)) {
            CHAO_INDEX(selection) = 0;
        }
    }
    chao = SELECTED_CHAO(selection);
    /* @hack spelling: an else after the return, where the code could follow
     * the if alone */
    if (chao == NULL) {
        return;
    } else {
        UNREAD_PROFILE()
        njPrintColor(0xffff8000);
        njPrintColor(EDITOR_WHITE);
        if (selection->flags & SHOW_MARKER) {
            NJS_POLYGON_VTX triangle[3] = {
#include "chao/editor/triangle.inc"
            };
            NJS_POINT3 above;
            NJS_POINT3 screen;

            position = &chao->work->pos;
            above = *position;
            above.y += MARKER_HEIGHT;
            ADVANCE_PHASE(editor_marker_phase, MARKER_PHASE_STEP);
            scale = (njSin(editor_marker_phase) * MARKER_PULSE) + MARKER_SCALE;
            njCalcPoint(NULL, &above, &screen);
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateV(NULL, &screen);
            triangle[0].col = MARKER_EDGE;
            triangle[1].col = MARKER_EDGE;
            triangle[2].col = MARKER_EDGE;
            njScale(NULL, scale, -scale, 1.0F);
            njDrawPolygon3DEx(triangle, 3, 0);
            triangle[0].col = MARKER_FILL;
            triangle[1].col = MARKER_FILL;
            triangle[2].col = MARKER_FILL;
            njScale(NULL, MARKER_INSET, MARKER_INSET, 0.5F);
            njDrawPolygon3DEx(triangle, 3, 0);
            njPopMatrixEx();
        }
        selection->flags |= SHOW_MARKER;
    }
}

static void editor_step(struct task *editor)
{
    struct task_work *selection = editor->work;
    chao_editor_callback *update;

    switch (selection->mode) {
    case EDITOR_START:
        selection->mode = EDITOR_MENU;
        CHAO_INDEX(selection) = 0;
        garden_disable_objects();
        editor_entry_camera_depth = camera_top(editor_camera_index);
        camera_push_user_mode(editor_camera_index, garden_fixed_camera);
        break;
    case EDITOR_MENU:
        garden_disable_objects();
        chao_editor_menu(editor);
        break;
    default:
        select_chao(editor);
#ifdef NONMATCHING
        /* this side runs no page for a mode that no page has. for a resident
         * page, it repairs the bug tagged below. where the page's Chao has left
         * the garden, it returns to the menu */
        if (selection->mode < EDITOR_PAGE ||
            selection->mode - EDITOR_PAGE >= editor_page_count) {
            break;
        }
        if (!editor_pages[selection->mode - EDITOR_PAGE].works_without_chao &&
            SELECTED_CHAO(selection) == NULL) {
            chao_editor_return_to_menu(editor);
            break;
        }
#endif
        /* @bug memory: a page that reads the selected Chao reads through null
         * where no Chao is selected, as chao_edit_texture does at 0x0c50461c.
         * select_chao finds no Chao at 0x0c507478 and returns, and the page
         * runs anyway */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. a page runs without a Chao
         * only where it works without one */
        if (CHAO_INDEX(editor->work) >= 0 ||
            editor_pages[selection->mode - EDITOR_PAGE].works_without_chao)
#else
        /* @hack spelling: the index read through editor->work, where
         * selection holds the same */
        if (CHAO_INDEX(editor->work) >= 0)
#endif
        {
            update = editor_pages[selection->mode - EDITOR_PAGE].update;
            if (update != NULL) {
                update(editor);
            } else {
                IGNORE_B();
            }
        }
        break;
    }
    if (++selection->timer > 5) {
        selection->timer = 0;
        if (++selection->tick > 9) {
            selection->tick = 0;
        }
    }
}

void chao_editor_open(void)
{
    struct task *editor;
    if (garden_editor == NULL) {
        editor = task_create(TASK_WORK, 2, editor_step, "AL_EditorExecutor");
#ifdef NONMATCHING
        /* this side opens no editor where task_create fails. the matching side
         * writes through the null */
        if (editor == NULL) {
            return;
        }
#endif
        editor->draw = editor_draw;
        editor->destructor = editor_close;
        garden_editor = editor;
    }
}
