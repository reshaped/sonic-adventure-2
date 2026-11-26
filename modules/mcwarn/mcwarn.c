/* MCWARN.PRS's init, exit and step, which its head names. the program shows
 * the memory card warning that game_state.card_warning names */
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/fade.h"
#include "main.h"
#include "mcwarn/mcwarn.h"
#include "save/card.h"
#include "save/save.h"
#include "shared/card_warning.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

#ifdef NONMATCHING
/* crash_install and crash_remove, by which this side's overlay links its own
 * crash screen. the overlay then gives the resident's vectors back */
#include "engine/dc/crash.h"
#endif

#define FRAME_BLANKS 1
#define FRAME_STEPS 1
#define TASK_LEVEL 0

enum {
    PHASE_NO_CARD_START,
    PHASE_NO_CARD,
    PHASE_NO_CARD_DONE,
    PHASE_NO_ROOM_START,
    PHASE_NO_ROOM,
    PHASE_NO_ROOM_DONE,
    PHASE_SAVE_START,
    PHASE_SAVE,
    PHASE_SAVE_DONE
};

/* the modes of the screen task, whose step is screen_step or
 * save_screen_step */
enum { MODE_FADE_IN, MODE_WAIT, MODE_FADE_OUT, MODE_DONE };

/* the steps of MODE_WAIT before the one that plays the voice */
#define VOICE_DELAY 120
/* the voice the screen task plays, kept in its work's ang.x */
#define VOICE(work) ((work)->ang.x)

/* what ports_case finds in the ports, which picks a warning's voice.
 * ports_case never returns PORTS_UNK_7 */
enum {
    PORTS_OTHER,
    PORTS_KEYBOARD_D,
    PORTS_TWO_PADS_VIBRATION,
    PORTS_MICROPHONE,
    PORTS_VIBRATION,
    PORTS_TWO_PADS,
    PORTS_MICROPHONE_KEYBOARD,
    PORTS_UNK_7,
    PORTS_KEYBOARD_A,
    PORTS_ALL_CONNECTORS
};
/* the first of each warning's voices, and each voice's place among them by
 * what ports_case finds. a place is the same for both warnings */
#define NO_CARD_VOICES 1051
#define NO_ROOM_VOICES 1063
enum {
    MICROPHONE_VOICE = 0,
    ALL_CONNECTORS_VOICE = 1,
    VIBRATION_VOICE = 3,
    TWO_PADS_VIBRATION_VOICE = 4,
    UNK_7_VOICE = 5,
    MICROPHONE_KEYBOARD_VOICE = 6,
    TWO_PADS_VOICE = 7,
    KEYBOARD_A_VOICE = 8,
    OTHER_VOICE = 10,
    KEYBOARD_D_VOICE = 11
};

/* each index of ports, named for the Shinobi port it holds */
enum {
    PORT_A0,
    PORT_A1,
    PORT_A2,
    PORT_B0,
    PORT_B1,
    PORT_B2,
    PORT_C0,
    PORT_C1,
    PORT_C2,
    PORT_D0,
    PORT_D1,
    PORT_D2,
    PORT_COUNT
};

static Sint8 pal_frames = 0;
static enum overlay_result mcwarn_result = OVERLAY_RUNNING;
/* the warning's task, which card_warning.h's calls create */
static struct task *warning_window = NULL;
static int mcwarn_phase = 0;
static Uint32 ports[PORT_COUNT] = {
    PDD_PORT_A0, PDD_PORT_A1, PDD_PORT_A2, PDD_PORT_B0,
    PDD_PORT_B1, PDD_PORT_B2, PDD_PORT_C0, PDD_PORT_C1,
    PDD_PORT_C2, PDD_PORT_D0, PDD_PORT_D1, PDD_PORT_D2,
};

static int ports_case(void);
static void no_card_start(void);
static void no_room_start(void);
static void save_start(void);
static void warning_window_forget(void);

void mcwarn_init(void)
{
#ifdef NONMATCHING
    /* This overlay links its own screen. Restore the resident's vectors
     * in mcwarn_exit before the loader reuses the slot. */
    crash_install();
#endif
    mcwarn_result = OVERLAY_RUNNING;
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_load_player_sounds();
    sound_task_create(NULL);
    game_state.four_cameras = 0;
    game_state.single_view = 0;
    switch (game_state.card_warning) {
    case CARD_WARNING_NO_CARD:
        mcwarn_phase = PHASE_NO_CARD_START;
        break;
    case CARD_WARNING_NO_ROOM:
        mcwarn_phase = PHASE_NO_ROOM_START;
        break;
    case SAVE_NOT_READY:
    case SAVE_NO_ROOM:
    case SAVE_NOT_OURS:
        mcwarn_phase = PHASE_SAVE_START;
        break;
    default:
        mcwarn_result = OVERLAY_DONE;
        return;
    }
    warning_window = NULL;
    card_init();
}

void mcwarn_exit(void)
{
    task_destroy_all();
    sound_task_kill();
    card_exit();
#ifdef NONMATCHING
    /* this side undoes mcwarn_init's crash_install before the loader reuses the
     * overlay's slot */
    crash_remove();
#endif
}

int mcwarn_step(void)
{
    switch (mcwarn_phase) {
    case PHASE_NO_CARD_START:
        no_card_start();
        mcwarn_phase++;
        pal_frames = 0;
        break;
    case PHASE_NO_CARD:
        RUN_TASKS();
        if (pad_reset_pressed(0)) {
            mcwarn_result = OVERLAY_RESET;
            warning_window = NULL;
        } else if (mcwarn_result != OVERLAY_RUNNING) {
            warning_window = NULL;
            mcwarn_phase++;
        }
        break;
    case PHASE_NO_CARD_DONE:
        mcwarn_result = OVERLAY_DONE;
        break;
    case PHASE_NO_ROOM_START:
        no_room_start();
        mcwarn_phase++;
        pal_frames = 0;
        break;
    case PHASE_NO_ROOM:
        RUN_TASKS();
        if (pad_reset_pressed(0)) {
            mcwarn_result = OVERLAY_RESET;
            warning_window_forget();
        } else if (mcwarn_result != OVERLAY_RUNNING) {
            warning_window_forget();
            mcwarn_phase++;
        }
        break;
    case PHASE_NO_ROOM_DONE:
        mcwarn_result = OVERLAY_DONE;
        break;
    case PHASE_SAVE_START:
        save_start();
        mcwarn_phase++;
        pal_frames = 0;
        break;
    case PHASE_SAVE:
        RUN_TASKS();
        if (pad_reset_pressed(0)) {
            mcwarn_result = OVERLAY_RESET;
            warning_window = NULL;
        } else if (mcwarn_result != OVERLAY_RUNNING) {
            warning_window = NULL;
            mcwarn_phase++;
        }
        break;
    case PHASE_SAVE_DONE:
        mcwarn_result = OVERLAY_DONE;
        break;
    }
    return mcwarn_result;
}

/* the result of the first test that holds */
static int ports_case(void)
{
    Uint32 types[PORT_COUNT];
    Uint32 i;

    for (i = 0; i < PORT_COUNT; i++) {
        types[i] = pdGetPeripheralInfo(ports[i])->type;
    }
    if ((types[PORT_A0] & PDD_DEVTYPE_CONTROLLER) &&
        (types[PORT_A2] & PDD_DEVTYPE_VIBRATION) &&
        (types[PORT_B0] & PDD_DEVTYPE_CONTROLLER) &&
        (types[PORT_B2] & PDD_DEVTYPE_VIBRATION) &&
        (types[PORT_C0] & PDD_DEVTYPE_KEYBOARD) &&
        (types[PORT_D0] & PDD_DEVTYPE_LIGHTGUN)) {
        return PORTS_ALL_CONNECTORS;
    }
    if ((types[PORT_A0] & types[PORT_B0] & types[PORT_C0] & types[PORT_D0]) &
        PDD_DEVTYPE_CONTROLLER) {
        return PORTS_ALL_CONNECTORS;
    }
    if ((types[PORT_A0] & PDD_DEVTYPE_CONTROLLER) &&
        (types[PORT_A2] & PDD_DEVTYPE_SOUNDINPUT) &&
        (types[PORT_B0] & PDD_DEVTYPE_KEYBOARD)) {
        return PORTS_MICROPHONE_KEYBOARD;
    }
    if (types[PORT_A0] & PDD_DEVTYPE_KEYBOARD) {
        return PORTS_KEYBOARD_A;
    }
    if ((types[PORT_A1] | types[PORT_B1] | types[PORT_C1] | types[PORT_D1]) &
        PDD_DEVTYPE_SOUNDINPUT) {
        return PORTS_MICROPHONE;
    }
    if ((types[PORT_A1] | types[PORT_B1] | types[PORT_C1] | types[PORT_D1]) &
        PDD_DEVTYPE_VIBRATION) {
        return PORTS_VIBRATION;
    }
    if (types[PORT_D0] & PDD_DEVTYPE_KEYBOARD) {
        return PORTS_KEYBOARD_D;
    }
    if ((types[PORT_A0] & PDD_DEVTYPE_CONTROLLER) &&
        (types[PORT_A2] & PDD_DEVTYPE_VIBRATION) &&
        (types[PORT_B0] & PDD_DEVTYPE_CONTROLLER)) {
        return PORTS_TWO_PADS_VIBRATION;
    }
    if ((types[PORT_A0] & types[PORT_B0]) & PDD_DEVTYPE_CONTROLLER) {
        return PORTS_TWO_PADS;
    }
    return PORTS_OTHER;
}

static void screen_step(struct task *task);
static void screen_draw(struct task *task);

static void no_card_start(void)
{
    struct task *task;
    int voice;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, TASK_LEVEL, screen_step,
                            "MCWarnNoCardExecutor")) != NULL) {
        task->draw = screen_draw;
    }
    fade_set_black();
    pad_enable();
    warning_window = no_card_warning_create();
    if (task != NULL) {
        switch (ports_case()) {
        case PORTS_OTHER:
            voice = NO_CARD_VOICES + OTHER_VOICE;
            break;
        case PORTS_KEYBOARD_D:
            voice = NO_CARD_VOICES + KEYBOARD_D_VOICE;
            break;
        case PORTS_TWO_PADS_VIBRATION:
            voice = NO_CARD_VOICES + TWO_PADS_VIBRATION_VOICE;
            break;
        case PORTS_MICROPHONE:
            voice = NO_CARD_VOICES + MICROPHONE_VOICE;
            break;
        case PORTS_VIBRATION:
            voice = NO_CARD_VOICES + VIBRATION_VOICE;
            break;
        case PORTS_TWO_PADS:
            voice = NO_CARD_VOICES + TWO_PADS_VOICE;
            break;
        case PORTS_MICROPHONE_KEYBOARD:
            voice = NO_CARD_VOICES + MICROPHONE_KEYBOARD_VOICE;
            break;
        case PORTS_UNK_7:
            voice = NO_CARD_VOICES + UNK_7_VOICE;
            break;
        case PORTS_KEYBOARD_A:
            voice = NO_CARD_VOICES + KEYBOARD_A_VOICE;
            break;
        default:
            voice = NO_CARD_VOICES + ALL_CONNECTORS_VOICE;
            break;
        }
        VOICE(task->work) = voice;
    }
}

static void warning_window_forget(void)
{
    /* @hack redundant: two calls of the function itself and a return, behind
     * a test no path passes */
    if (0) {
        warning_window_forget();
        warning_window_forget();
        return;
    }
    warning_window = NULL;
}

static void screen_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_FADE_IN:
        /* @hack spelling: the step's result compared with TRUE, where the
         * result alone would do */
        if (fade_in_step() == TRUE) {
            work->mode = MODE_WAIT;
        }
        break;
    case MODE_WAIT:
        if (work->timer++ == VOICE_DELAY) {
            voice_play_priority_2(VOICE(work));
        }
        /* the warning's task has destroyed itself */
        if (warning_window->step == task_destroy) {
            work->mode = MODE_FADE_OUT;
            fade_reset_idle();
            pad_disable();
        }
        break;
    case MODE_FADE_OUT:
        /* @hack spelling: the step's result compared with TRUE, where the
         * result alone would do */
        if (fade_out_step() == TRUE) {
            work->mode = MODE_DONE;
            mcwarn_result = OVERLAY_DONE;
        }
        break;
    }
}

static void screen_draw(struct task *task)
{
}

static void no_room_start(void)
{
    struct task *task;
    int voice;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, TASK_LEVEL, screen_step,
                            "MCWarnNoCardExecutor")) != NULL) {
        task->draw = screen_draw;
    }
    fade_set_black();
    pad_enable();
    warning_window = no_room_warning_create();
    if (task != NULL) {
        switch (ports_case()) {
        case PORTS_OTHER:
            voice = NO_ROOM_VOICES + OTHER_VOICE;
            break;
        case PORTS_KEYBOARD_D:
            voice = NO_ROOM_VOICES + KEYBOARD_D_VOICE;
            break;
        case PORTS_TWO_PADS_VIBRATION:
            voice = NO_ROOM_VOICES + TWO_PADS_VIBRATION_VOICE;
            break;
        case PORTS_MICROPHONE:
            voice = NO_ROOM_VOICES + MICROPHONE_VOICE;
            break;
        case PORTS_VIBRATION:
            voice = NO_ROOM_VOICES + VIBRATION_VOICE;
            break;
        case PORTS_TWO_PADS:
            voice = NO_ROOM_VOICES + TWO_PADS_VOICE;
            break;
        case PORTS_MICROPHONE_KEYBOARD:
            voice = NO_ROOM_VOICES + MICROPHONE_KEYBOARD_VOICE;
            break;
        case PORTS_UNK_7:
            voice = NO_ROOM_VOICES + UNK_7_VOICE;
            break;
        case PORTS_KEYBOARD_A:
            voice = NO_ROOM_VOICES + KEYBOARD_A_VOICE;
            break;
        default:
            voice = NO_ROOM_VOICES + ALL_CONNECTORS_VOICE;
            break;
        }
        VOICE(task->work) = voice;
    }
}

static void save_screen_step(struct task *task);
static void save_screen_draw(struct task *task);

static void save_start(void)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, TASK_LEVEL, save_screen_step,
                            "MCWarnMultiExecutor")) != NULL) {
        task->draw = save_screen_draw;
    }
    fade_set_black();
    pad_enable();
    warning_window = save_warning_create();
}

/* screen_step, without the voice */
static void save_screen_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_FADE_IN:
        /* @hack spelling: the step's result compared with TRUE, where the
         * result alone would do */
        if (fade_in_step() == TRUE) {
            work->mode = MODE_WAIT;
        }
        break;
    case MODE_WAIT:
        if (warning_window->step == task_destroy) {
            work->mode = MODE_FADE_OUT;
            fade_reset_idle();
            pad_disable();
        }
        break;
    case MODE_FADE_OUT:
        /* @hack spelling: the step's result compared with TRUE, where the
         * result alone would do */
        if (fade_out_step() == TRUE) {
            work->mode = MODE_DONE;
            mcwarn_result = OVERLAY_DONE;
        }
        break;
    }
}

static void save_screen_draw(struct task *task)
{
}
