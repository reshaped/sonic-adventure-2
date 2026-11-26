/* the tutorial: MENU_MODE_TUTORIAL's menu of lessons, each a page of lines
 * that a talk window shows. tutorial_panel.h's panel draws it */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/tutorial.h"
#include "advertise/tutorial_panel.h"
#include "hud/message_window.h"
#include "player/player.h"
#include "shared/messenger.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/flags.h"
#include "system/pad.h"
#include "system/task.h"

struct tutorial_work tutorial_work;

/* the list plays the next of idle_voices once its steps without a press pass
 * IDLE_STEPS. IDLE_VOICES must be a power of 2: a mask wraps the index */
#define IDLE_STEPS 900
#define IDLE_VOICES 4
/* the voices' ids in the voice partition. Cancel on the list plays
 * LEAVE_VOICE, and Choose plays a lesson's as it opens its page */
#define LEAVE_VOICE 0x923
#define LESSON_0_VOICE 0x368
#define LESSON_1_VOICE 0x361
#define LESSON_2_VOICE 0x363
#define LESSON_3_VOICE 0x365
#define LESSON_4_VOICE 0x367
/* a line's picture where the line names none */
#define NO_PICTURE (-1)
/* a line's upgrade where the line always shows, and its also_upgrade where
 * it names no second */
#define NO_UPGRADE (-1)
/* a line's upgrade where the line shows until one of a group's upgrades is
 * found. tutorial_next_line lists each group's. the line also shows while
 * game_flags[upgrade], below the array, isn't 0 */
#define UNTIL_SONIC_SHADOW_UPGRADE (-2)
#define UNTIL_KNUCKLES_ROUGE_UPGRADE (-3)
#define UNTIL_MECH_UPGRADE (-4)
#ifdef NONMATCHING
/* this side repairs the memory bug tagged at the macro's use in
 * tutorial_next_line. it reads no byte below game_flags for an UNTIL_ value. it
 * keeps the result of the mismatched call tagged there, and calls flag_get by
 * its own type */
#define TUTORIAL_HAS_UPGRADE(flag) ((flag) >= 0 && flag_get(flag) != 0)
#else
#define TUTORIAL_HAS_UPGRADE(flag) (flag_get_int(flag) != 0)
#endif
/* the talk's line before the page's first */
#define NO_LINE (-1)

#include "advertise/tutorial/text.inc"

/* menu_control.step, while the tutorial runs */
enum tutorial_step {
    STEP_SHOW,
    STEP_LIST_FADE,
    STEP_CHOOSE,
    STEP_PAGE,
    STEP_PAGE_FADE,
    STEP_PAGE_BACK, /* nothing sets it. Cancel there goes back to the list */
    STEP_LEAVE /* wait for the panel to fade out, then go to the new menu */
};

/* a line of a lesson's page. where upgrade is an upgrade's number, the line
 * shows only while that game flag or also_upgrade's is set */
struct tutorial_line {
    /* the board's picture from this line on, of the lesson's picture list.
     * NO_PICTURE keeps the picture shown, or on a page's first line shows
     * none */
    Sint32 picture;
    /* the message the talk window shows, by its number in the file mh0058 */
    Sint32 message;
    Sint32 model_mode; /* how the model moves from this line on */
    Sint32 upgrade;
    Sint32 also_upgrade;
};
struct tutorial_page {
    struct tutorial_line *lines;
    Sint32 count;
};

struct tutorial_talk {
    Sint32 idle_voice; /* the index in idle_voices of the one played last */
    /* the page's line that the window shows, from 0, or NO_LINE */
    Sint32 line;
};
static struct tutorial_talk tutorial_talk;
/* Steps since the last button press. */
static Sint32 tutorial_idle_steps;
/* Cleared when the player first presses Choose during narration. */
static Bool tutorial_voice_on;

static Sint32 idle_voices[IDLE_VOICES] = {0x35d, 0x35e, 0x35f, 0x360};
static struct tutorial_line kihon_lines[17] = {
#include "advertise/tutorial/kihon_lines.inc"
};
static struct tutorial_line sonic_lines[54] = {
#include "advertise/tutorial/sonic_lines.inc"
};
static struct tutorial_line tails_lines[45] = {
#include "advertise/tutorial/tails_lines.inc"
};
static struct tutorial_line knuckles_lines[57] = {
#include "advertise/tutorial/knuckles_lines.inc"
};
static struct tutorial_line chao_lines[49] = {
#include "advertise/tutorial/chao_lines.inc"
};
static struct tutorial_page pages[TUTORIAL_LESSONS] = {
#include "advertise/tutorial/pages.inc"
};

static Bool tutorial_next_line(void);

void tutorial_start(void)
{
    menu_textures_select(MENU_TEXTURES_TUTORIAL);
    messenger_messages_load(TUTORIAL_MESSAGES);
    menu_control.step = STEP_SHOW;
    music_change(TUTORIAL_MUSIC);
}

/* fade the panel out, and free the messages */
static void tutorial_hide(void)
{
    menu_works.work_16->shown = FALSE;
    menu_works.work_16->unk_04 = 0;
    messenger_messages_free();
}

Bool tutorial_step(void)
{
    /* @hack spelling: the buttons read through pointers, where their own
     * names would do */
    Uint32 *choose;
    Uint32 *cancel;
    /* @hack spelling: voice_play_priority_2 called through a pointer, where
     * its own name would do */
    void (*play_voice)(int id);

    /* @hack redundant: a block no path reaches */
    if (0) {
        tutorial_talk.idle_voice = 3;
    }
    choose = &pad_choose_buttons;
    cancel = &pad_cancel_buttons;
    switch (menu_control.step) {
    case STEP_SHOW:
        menu_works.work_16->shown = TRUE;
        menu_works.work_16->model.shown = TRUE;
        menu_works.work_16->list.shown = TRUE;
        menu_control.step = STEP_LIST_FADE;
        break;
    case STEP_LIST_FADE:
        /* @hack spelling: the break while the panel fades, here and in
         * STEP_PAGE_FADE and STEP_LEAVE, where an if around what follows
         * would do */
        if (menu_works.work_16->fading != 0) {
            break;
        }
        menu_control.step = STEP_CHOOSE;
        tutorial_idle_steps = 0;
        break;
    case STEP_CHOOSE:
        play_voice = voice_play_priority_2;
        if ((pad_press_repeat[0] & *cancel) != 0) {
            tutorial_hide();
            menu_control.step = STEP_LEAVE;
            play_voice(LEAVE_VOICE);
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            if (message_window_current() != NULL) {
                task_destroy(message_window_current());
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KD) != 0) {
            if (menu_works.work_16->lesson < TUTORIAL_LESSONS - 1) {
                /* @hack spelling: the lesson set to its sum with 1, where ++
                 * would do */
                menu_works.work_16->lesson = menu_works.work_16->lesson + 1;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                tutorial_idle_steps = 0;
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KU) != 0) {
            if (menu_works.work_16->lesson != 0) {
                menu_works.work_16->lesson--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                tutorial_idle_steps = 0;
            }
        } else if ((pad_press_repeat[0] & *choose) != 0) {
            menu_works.work_16->list.shown = FALSE;
            menu_works.work_16->board.shown = TRUE;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            menu_control.step = STEP_PAGE_FADE;
            tutorial_idle_steps = 0;
            menu_works.work_16->picture = NO_PICTURE;
            menu_works.work_16->camera_moved = FALSE;
            switch (menu_works.work_16->lesson) {
            case 0:
                play_voice(LESSON_0_VOICE);
                break;
            case 1:
                play_voice(LESSON_1_VOICE);
                break;
            case 2:
                play_voice(LESSON_2_VOICE);
                break;
            case 3:
                play_voice(LESSON_3_VOICE);
                break;
            case 4:
                play_voice(LESSON_4_VOICE);
                break;
            }
        } else {
            tutorial_idle_steps++;
            if (tutorial_idle_steps > IDLE_STEPS) {
                tutorial_talk.idle_voice =
                    (tutorial_talk.idle_voice + 1) & (IDLE_VOICES - 1);
                play_voice(idle_voices[tutorial_talk.idle_voice]);
                tutorial_idle_steps = 0;
            }
        }
        break;
    /* @hack spelling: STEP_PAGE_FADE's case before STEP_PAGE's, where the
     * enum's order would do */
    case STEP_PAGE_FADE:
        if (menu_works.work_16->fading != 0) {
            break;
        }
        menu_control.step = STEP_PAGE;
        tutorial_talk.line = NO_LINE;
        tutorial_voice_on = TRUE;
        break;
    case STEP_PAGE:
        if ((pad_press_repeat[0] & *cancel) != 0) {
            menu_works.work_16->list.shown = TRUE;
            menu_works.work_16->board.shown = FALSE;
            menu_works.work_16->picture = NO_PICTURE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_LIST_FADE;
            menu_works.work_16->camera_moved = FALSE;
        } else if ((pad_press_repeat[0] & *choose) != 0) {
            if (tutorial_voice_on != 0) {
                voices_stop();
                tutorial_voice_on = FALSE;
            }
            if (tutorial_next_line() != 0) {
                menu_works.work_16->list.shown = TRUE;
                menu_works.work_16->board.shown = FALSE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                menu_control.step = STEP_LIST_FADE;
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_TY) != 0) {
            menu_works.work_16->camera_moved =
                !menu_works.work_16->camera_moved;
        }
        break;
    case STEP_PAGE_BACK:
        if ((pad_press_repeat[0] & *cancel) != 0) {
            menu_works.work_16->list.shown = TRUE;
            menu_works.work_16->board.shown = FALSE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_LIST_FADE;
        }
        break;
    case STEP_LEAVE:
        if (menu_works.work_16->fading != 0) {
            break;
        }
        new_menu_screen_start();
        new_menu_screen_reopen(NEW_MENU_REOPEN_EXTRAS);
        menu_control.mode = MENU_MODE_NEW_MENU;
        break;
    }
    return FALSE;
}

/* show the page's next line that the upgrades found allow, in a talk
 * window. the line sets the board's picture and the model's mode. it
 * returns TRUE, showing nothing, where the page has no line left, and FALSE
 * otherwise. a page must end in a line of NO_UPGRADE, unchecked. the search
 * runs past a page whose last line hides */
static Bool tutorial_next_line(void)
{
    struct tutorial_line *lines;
    /* @hack type: first and second volatile, so that SHC keeps their
     * stores, which nothing reads, where lines[tutorial_talk.line] would
     * do */
    struct tutorial_line *volatile first;
    struct tutorial_line *volatile second;
    int upgrade;

    tutorial_talk.line++;
    if (tutorial_talk.line >= pages[menu_works.work_16->page].count) {
        return TRUE;
    }
    lines = pages[menu_works.work_16->lesson].lines;
    for (;;) {
        upgrade = (first = &lines[tutorial_talk.line])->upgrade;
        /* @bug memory: for a line whose upgrade is an UNTIL_ value, this
         * call reads game_flags[upgrade], 2 to 4 bytes below the array,
         * before the switch tests the line's group (0x8c5178a8). the line
         * then shows where that byte isn't 0, whatever its group's flags
         * hold. twelve lines of the pages hold an UNTIL_ value */
        /* @bug mismatched-call: the tests read a word from flag_get_int,
         * where flag_get returns a Uint8. the word still holds the flag's 0
         * or 1. flag_get loads the byte with MOV.B, which sign-extends it
         * (TST R0,R0 at 0x8c5178ac) */
        if (upgrade == NO_UPGRADE || TUTORIAL_HAS_UPGRADE(upgrade)) {
            break;
        }
        upgrade = (second = &lines[tutorial_talk.line])->also_upgrade;
        if (upgrade >= 0 && flag_get_int(upgrade) != 0) {
            break;
        }
        switch (lines[tutorial_talk.line].upgrade) {
        case UNTIL_SONIC_SHADOW_UPGRADE:
            /* @hack spelling: each group's break where a flag is set and
             * its goto where none is, where a goto under ! would do */
            if ((flag_get_int(UPGRADE_SONIC_SHOES) |
                 flag_get_int(UPGRADE_SONIC_SPARKLE) |
                 flag_get_int(UPGRADE_SONIC_MAGICWRIST) |
                 flag_get_int(UPGRADE_SONIC_FLAMERING) |
                 flag_get_int(UPGRADE_SONIC_RUBBERUNIT) |
                 flag_get_int(UPGRADE_SONIC_MELODY) |
                 flag_get_int(UPGRADE_UNK_15) |
                 flag_get_int(UPGRADE_SHADOW_SHOES) |
                 flag_get_int(UPGRADE_SHADOW_SPARKLE) |
                 flag_get_int(UPGRADE_SHADOW_FLAMERING) |
                 flag_get_int(UPGRADE_SHADOW_MELODY)) != 0) {
                break;
            } else {
                goto found;
            }
        case UNTIL_KNUCKLES_ROUGE_UPGRADE:
            if ((flag_get_int(UPGRADE_KNUCKLES_SUNGLASS) |
                 flag_get_int(UPGRADE_KNUCKLES_GROVES) |
                 flag_get_int(UPGRADE_KNUCKLES_AIR) |
                 flag_get_int(UPGRADE_KNUCKLES_MELODY) |
                 flag_get_int(UPGRADE_ROUGE_NAILS) |
                 flag_get_int(UPGRADE_ROUGE_SCOPE) |
                 flag_get_int(UPGRADE_ROUGE_BOOTS) |
                 flag_get_int(UPGRADE_ROUGE_MELODY)) != 0) {
                break;
            } else {
                goto found;
            }
        case UNTIL_MECH_UPGRADE:
            if ((flag_get_int(UPGRADE_TWALKER_BOOSTER) |
                 flag_get_int(UPGRADE_TWALKER_HYPERCANNON) |
                 flag_get_int(UPGRADE_TWALKER_LAZERBALSTER) |
                 flag_get_int(UPGRADE_TWALKER_MELODY) |
                 flag_get_int(UPGRADE_EWALKER_JETENGINE) |
                 flag_get_int(UPGRADE_EWALKER_BAZOOKA) |
                 flag_get_int(UPGRADE_EWALKER_POWERGUN) |
                 flag_get_int(UPGRADE_EWALKER_EXTRASHIELD) |
                 flag_get_int(UPGRADE_EWALKER_MELODY)) != 0) {
                break;
            } else {
                goto found;
            }
        }
        tutorial_talk.line++;
    }
found:
    if (tutorial_talk.line != 0) {
        if (lines[tutorial_talk.line].picture >= 0) {
            menu_works.work_16->picture = lines[tutorial_talk.line].picture;
        }
    } else {
        menu_works.work_16->page = menu_works.work_16->lesson;
        menu_works.work_16->picture = lines[tutorial_talk.line].picture;
    }
    messenger_create(NULL, 0, 0, lines[tutorial_talk.line].message,
                     MESSENGER_TALK_WINDOW, NULL);
    menu_works.work_16->model_mode = lines[tutorial_talk.line].model_mode;
    return FALSE;
}
