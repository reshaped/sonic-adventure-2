#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#include "chao/actor.h"
#include "chao/classroom.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/work.h"
#include "chaostgkinder/lesson/text.inc"
#include "chaostgkinder/messages.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/service_menu.h"
#include "chaostgkinder/services.h"
#include "draw/lines.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* a subject is a dance from LESSON_DANCE, an instrument from
 * LESSON_INSTRUMENT, LESSON_SONG or LESSON_ART. a song or art lesson teaches
 * the first of its levels, 0 to LAST_LEVEL, that the Chao lacks. the save
 * holds its subject plus that level. LESSON_SONG and LESSON_ART are
 * multiples of SUBJECT_SPAN, so that dividing a saved code by it recovers
 * the subject */
enum lesson_subject {
    LESSON_DANCE = 0,
    LESSON_INSTRUMENT = 5,
    LESSON_LAST_INSTRUMENT = 14,
    LESSON_SONG = 20,
    LESSON_ART = 30
};
#define LAST_LEVEL 4
#define SUBJECT_SPAN 10

enum lesson_step {
    LESSON_OPEN,
    LESSON_MENU,
    LESSON_NOTICE,
    LESSON_CONFIRM,
    LESSON_PICK_STUDENT,
    LESSON_ENROLLED,
    LESSON_IDLE,
    LESSON_LEAVE,
    LESSON_DONE
};

/* the messages of KINDER_WHOLE_BANK the lesson shows: in the dialog, in a
 * prompt or on a label. a dance's or an instrument's name is
 * LESSON_SUBJECT_MESSAGE plus its subject */
#define LESSON_EXIT_DIALOG 0
#define LESSON_OPEN_DIALOG 65
#define LESSON_ENROLL_DIALOG 66
#define LESSON_ENROLL_PROMPT 68
#define LESSON_KNOWN_DANCE_PROMPT 72
#define LESSON_KNOWN_ART_PROMPT 74
#define LESSON_KNOWN_INSTRUMENT_PROMPT 76
#define LESSON_KNOWN_SONG_PROMPT 78
#define LESSON_FULL_DIALOG 80
#define LESSON_FULL_PROMPT 81
#define LESSON_PICK_UP_DIALOG 83
#define LESSON_ENROLLED_MESSAGE 85
#define LESSON_PICK_UP_PROMPT 88
#define LESSON_PICK_STUDENT_DIALOG 91
#define LESSON_UNFINISHED_PROMPT 93
#define LESSON_PICKUP_MESSAGE 95
#define LESSON_SUBJECT_MESSAGE 99
#define LESSON_SONG_MESSAGE 114
#define LESSON_ART_MESSAGE 115
#define LESSON_TITLE_LABEL 118
#define LESSON_ROSTER_LABEL 120
#define LESSON_NO_CHAO_DIALOG 121
#define LESSON_ENROLL_LABEL 126
#define LESSON_PICK_UP_LABEL 127
#define LESSON_HANDS_FULL_DIALOG 135

/* the sound of a choice the menu refuses */
#define REFUSE_SOUND 0x8009

enum lesson_choice { ENROLL, PICK_UP, EXIT_CLASSROOM };
#define CHOICES 3
enum prompt_style {
    MESSAGE_ONLY = SERVICE_PINK,
    YES_OR_NO = SERVICE_BLUE,
    ACKNOWLEDGE = SERVICE_GOLD
};
/* the ids of a prompt's buttons. classroom->answer holds the selected one */
enum prompt_answer { ANSWER_YES, ANSWER_NO };
#define CLASS_SIZE 4
/* lengths of time in frames, as play_time counts them */
enum lesson_time {
    SECOND = 60,
    LESSON_HOUR = 3600 * SECOND,
    LESSON_DURATION = LESSON_HOUR / 2
};
/* STEP_DELAY is the frames a step waits before it reads the pads.
 * BACK_DELAY is the menu's wait after its back button. a prompt opens over
 * PROMPT_DELAY frames, or over NOTICE_DELAY for a notice */
#define STEP_DELAY 20
#define BACK_DELAY 5
#define PROMPT_DELAY 20
#define NOTICE_DELAY 30
/* a closing prompt's frames, and those left when its children go */
#define CLOSE_FRAMES 15
#define CLEAR_FRAMES 5
/* the bits of a prompt's work flags, and the bit of a roster row's */
enum prompt_flags { PROMPT_CLOSING = 0x100, PROMPT_OPENING = 0x200 };
enum row_flags { ROW_SELECTED = 0x100 };

/* a seat of the class. an empty seat's subject and level are -1 */
struct student {
    int subject, level;
    Sint8 finished;
    struct task *chao;
};

/* previous is the step before the last change. a notice returns to it, or
 * to resume where previous is -1. flags stays 0 */
struct classroom {
    Sint8 state, step;
    Sint8 previous, resume;
    Sint8 cursor, choice, subject, answer;
    Sint8 selected; /* the roster's chosen row, or -1 for none */
    Uint8 unused_09;
    Sint8 flags;
    Uint16 timer;
    int play_time, notice;
    struct student students[CLASS_SIZE];
    Sint8 count;
    /* held is the Chao the player carries */
    struct task *task, *held, *prompt, *roster;
    struct task *background[CLASS_SIZE];
    struct garden_class *save;
};

/* the work of the roster, and of each of its rows */
struct roster_work {
    Sint8 mode, cursor;
    Uint8 id, count; /* a row's count is 1 where its student has finished */
    Sint16 flags;
    Uint16 timer;
    Angle phase;
    struct student *student;
    Sint8 *selection;
    NJS_POINT3 pos;
    Float scale, amplitude;
};

struct classmate_work {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    struct task *chao;
    Angle yaw;
};

#define CENTER_FIT (KINDER_MESSAGE_LABEL_CENTER | KINDER_MESSAGE_LABEL_FIT)
/* the labels' text colors, and the selected colors of a prompt's buttons */
#define WHITE_TEXT 0xffffffff
#define BLACK_TEXT 0xff000000
#define ACKNOWLEDGE_COLOR 0xff988e00
#define ANSWER_COLOR 0xff0050a7
static const struct kinder_message_label menu_label = {
#include "chaostgkinder/lesson/menu_label.inc"
};
static const char exit_text[8] = {
#include "chaostgkinder/lesson/exit_text.inc"
};
static const char line_break[4] = {
#include "chaostgkinder/lesson/line_break.inc"
};

static struct classroom *classroom = NULL;
static struct service_decoration lesson_frame = {
#include "chaostgkinder/lesson/lesson_frame.inc"
};
static struct service_box lesson_panel = {
#include "chaostgkinder/lesson/lesson_panel.inc"
};
static struct service_box subject_badge = {
#include "chaostgkinder/lesson/subject_badge.inc"
};
static struct service_decoration menu_frame = {
#include "chaostgkinder/lesson/menu_frame.inc"
};
static struct service_box menu_panel = {
#include "chaostgkinder/lesson/menu_panel.inc"
};
static struct service_box first_choice = {
#include "chaostgkinder/lesson/first_choice.inc"
};
static struct service_choice choice_colors = {
#include "chaostgkinder/lesson/choice_colors.inc"
};
static struct kinder_message_label plain_label = {
#include "chaostgkinder/lesson/plain_label.inc"
};
/* the subject of each LESSON_HOUR in turn. each is a dance, an instrument,
 * LESSON_SONG or LESSON_ART */
static int timetable[21] = {
#include "chaostgkinder/lesson/timetable.inc"
};
/* a second label of plain_label's values, which no code reads */
static struct kinder_message_label roster_label = {
#include "chaostgkinder/lesson/roster_label.inc"
};

/* the sprites of roster_cells, of which no draw uses 4 */
enum roster_cell {
    FRAME_TOP_LEFT = 0,
    FRAME_TOP = 1,
    FRAME_TOP_RIGHT = 2,
    FRAME_LEFT = 3,
    FRAME_RIGHT = 5,
    FRAME_BOTTOM_LEFT = 6,
    FRAME_BOTTOM = 7,
    FRAME_BOTTOM_RIGHT = 8,
    ROW_LEFT = 9,
    ROW_MIDDLE = 10,
    ROW_RIGHT = 11,
    FILL = 12,
    ICON_FINISHED = 13,
    ICON_UNFINISHED = 14
};
static NJS_TEXANIM roster_cells[15] = {
#include "chaostgkinder/lesson/roster_cells.inc"
};
#define CLASSMATE_MOTIONS 13
static int classmate_motions[CLASSMATE_MOTIONS] = {
#include "chaostgkinder/lesson/classmate_motions.inc"
};
static NJS_POINT3 classmate_positions[CLASS_SIZE] = {
#include "chaostgkinder/lesson/classmate_positions.inc"
};

static struct task *prompt_create(struct task *parent, Sint16 message,
                                  const char *text, Sint8 style,
                                  const Sint8 *answer, Sint16 delay);
static void prompt_close(struct task *task);
static struct task *roster_create(struct task *parent, struct student *students,
                                  int count, Sint8 *selection);
static struct task *student_row_create(struct task *parent,
                                       struct student *student,
                                       Sint8 *selection, Sint8 id,
                                       const NJS_POINT3 *pos);
static struct task *classmate_create(struct task *parent, struct task *chao,
                                     int seat);

/* 1 where chao knows subject, a song or art at every level, else 0. *level
 * gets the first level of a song or art that chao lacks, and 0 otherwise */
static int lesson_known(struct task *chao, int subject, int *level)
{
    int i;
    *level = 0;
    if (subject <= LESSON_INSTRUMENT - 1) {
        return chao_knows_dance(chao, subject);
    }
    if (subject <= LESSON_LAST_INSTRUMENT) {
        return chao_knows_instrument(chao, subject - LESSON_INSTRUMENT);
    }
    if (subject == LESSON_SONG) {
        for (i = 0; i <= LAST_LEVEL; i++) {
            if (!chao_knows_song(chao, i)) {
                break;
            }
        }
        if (i > LAST_LEVEL) {
            return 1;
        }
        *level = i;
        return 0;
    } else if (subject == LESSON_ART) {
        for (i = 0; i <= LAST_LEVEL; i++) {
            if (!chao_knows_art(chao, i)) {
                break;
            }
        }
        if (i > LAST_LEVEL) {
            return 1;
        }
        *level = i;
        return 0;
    }
    /* @bug uninitialized: lesson_known, at 0x0c9cda80, returns no value for
     * a subject none of the tests above takes. the timetable holds no such
     * subject */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other subject returns 0 */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void award_lesson(struct task *chao, int subject, int level)
{
    if (subject == LESSON_ART) {
        chao_learn_art(chao, level);
        return;
    }
    if (subject == LESSON_SONG) {
        chao_learn_song(chao, level);
        return;
    }
    if (subject >= LESSON_INSTRUMENT) {
        chao_learn_instrument(chao, subject - LESSON_INSTRUMENT);
        return;
    }
    if (subject >= LESSON_DANCE) {
        chao_learn_dance(chao, subject);
    }
}

static void lesson_change_step(int step)
{
    Sint8 previous = classroom->step;
    classroom->step = step;
    classroom->previous = previous;
    /* @hack spelling: the test of bit 0x100 of the sign-extended flags,
     * where flags < 0 would do */
    if (classroom->flags & 0x100) {
        kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
        /* @hack redundant: the read of step that nothing uses */
        (void)classroom->step;
    }
}

#define PROMPT(message, style, delay)                                          \
    prompt_create(task, (message), NULL, (style), &classroom->answer, (delay))

#define SHOW_CHOICE_DIALOG()                                                   \
    {                                                                          \
        switch (classroom->choice) {                                           \
        case ENROLL:                                                           \
            kinder_dialog_set(LESSON_ENROLL_DIALOG, KINDER_WHOLE_BANK);        \
            break;                                                             \
        case PICK_UP:                                                          \
            kinder_dialog_set(LESSON_PICK_UP_DIALOG, KINDER_WHOLE_BANK);       \
            break;                                                             \
        case EXIT_CLASSROOM:                                                   \
            kinder_dialog_set(LESSON_EXIT_DIALOG, KINDER_WHOLE_BANK);          \
            break;                                                             \
        }                                                                      \
    }

static void lesson_step(struct task *task)
{
    const PDS_PERIPHERAL *pad;
    struct service_box *title;
    struct service_box *badge;
    struct student *student;
    struct garden_class *save;
    struct student *source;
    struct task *chao;
    struct chao_work *work;
    char *lines;
    const char *subject_name;
    char *formatted;
    int level;
    int next_level;
    int message;
    int length;
    Sint8 i;
    Sint8 j;
    Sint8 slot;

    if (game_state.time_stopped) {
        return;
    }
    switch (classroom->step) {
    case LESSON_OPEN:
        kinder_dialog_create(task, 0, 0);
        service_panel_create(task, title = &lesson_panel, &lesson_frame);
        {
            struct kinder_message_label label;
            struct service_choice *colors;
            int hour;

            label = plain_label;
            label.flags = CENTER_FIT;
            label.color = WHITE_TEXT;
            label.selected_color = WHITE_TEXT;
            label.message = LESSON_TITLE_LABEL;
            label.pos.x = title->pos.x + (title->width * 0.5F);
            label.pos.y = title->pos.y + 16.0F;
            label.pos.z = title->pos.z + SERVICE_LABEL_DEPTH;
            label.width = title->width;
            kinder_message_label_create(task, &label);

            badge = &subject_badge;
            colors = &choice_colors;
            service_button_create(task, 0, badge, colors);
            hour = classroom->play_time / LESSON_HOUR %
                   (sizeof timetable / sizeof timetable[0]);
            classroom->subject = timetable[hour];
            label = plain_label;
            label.flags = CENTER_FIT;
            label.color = BLACK_TEXT;
            label.selected_color = BLACK_TEXT;
            if (classroom->subject == LESSON_ART) {
                label.message = LESSON_ART_MESSAGE;
            } else if (classroom->subject == LESSON_SONG) {
                label.message = LESSON_SONG_MESSAGE;
            } else {
                label.message = classroom->subject + LESSON_SUBJECT_MESSAGE;
            }
            label.pos.x = badge->pos.x + (badge->width * 0.5F);
            label.pos.y = badge->pos.y + (badge->height * 0.5F);
            label.pos.z = badge->pos.z + SERVICE_LABEL_DEPTH;
            label.width = badge->width;
            kinder_message_label_create(task, &label);
        }
        {
            struct service_box button;
            struct service_choice colors;
            struct kinder_message_label label;

            button = first_choice;
            colors = choice_colors;
            colors.selection = &classroom->choice;
            service_panel_create(task, &menu_panel, &menu_frame);
            for (i = 0; i < CHOICES; i++) {
                /* @hack redundant: assignments of the buttons' messages and
                 * spacing that never run */
                if (0) {
                    label.message = LESSON_ENROLL_LABEL;
                    label.message = LESSON_PICK_UP_LABEL;
                    button.pos.y = 10.0F;
                }
                service_button_create(task, i, &button, &colors);
                label = menu_label;
                switch (i) {
                case ENROLL:
                    label.message = LESSON_ENROLL_LABEL;
                    break;
                case PICK_UP:
                    label.message = LESSON_PICK_UP_LABEL;
                    break;
                case EXIT_CLASSROOM:
                    label.message = 0;
                    label.flags |= KINDER_MESSAGE_LABEL_LITERAL;
                    label.text = exit_text;
                    break;
                }
                label.id = i;
                label.selection = (const Uint8 *)&classroom->choice;
                label.pos.x = button.pos.x + (button.width * 0.5F);
                label.pos.y = button.pos.y + (button.height * 0.5F);
                label.pos.z = button.pos.z + SERVICE_LABEL_DEPTH;
                label.width = button.width - SERVICE_LABEL_INSET;
                kinder_message_label_create(task, &label);
                button.pos.y += button.height + SERVICE_ROW_GAP;
            }
        }
        kinder_dialog_set(LESSON_OPEN_DIALOG, KINDER_WHOLE_BANK);
        kinder_dialog_show_arrow();
        classroom->timer = STEP_DELAY;
        kinder_dialog_hide_arrow();
        lesson_change_step(LESSON_MENU);
        break;
    case LESSON_MENU:
        if (classroom->timer > 0) {
            classroom->timer--;
            if (classroom->timer != 0) {
                return;
            }
            if (classroom->roster) {
                return;
            }
            if (classroom->count <= 0) {
                return;
            }
            classroom->selected = -1;
            classroom->roster =
                roster_create(task, classroom->students, classroom->count,
                              &classroom->selected);
            return;
        }
        if (classroom->cursor < EXIT_CLASSROOM &&
            (pad_press_repeat[0] & PDD_DGT_KD)) {
            classroom->cursor++;
            classroom->choice = classroom->cursor;
            SHOW_CHOICE_DIALOG();
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (classroom->cursor > ENROLL && (pad_press_repeat[0] & PDD_DGT_KU)) {
            classroom->cursor--;
            classroom->choice = classroom->cursor;
            SHOW_CHOICE_DIALOG();
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (player_pads[0]->press & PDD_DGT_TB) {
            if (classroom->choice == EXIT_CLASSROOM) {
                goto leave;
            }
            sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
            kinder_dialog_set(LESSON_EXIT_DIALOG, KINDER_WHOLE_BANK);
            classroom->cursor = EXIT_CLASSROOM;
            classroom->choice = classroom->cursor;
            classroom->timer = BACK_DELAY;
        }
        if (player_pads[0]->press & PDD_DGT_TA) {
            switch (classroom->choice) {
            case EXIT_CLASSROOM:
            leave:
                /* @hack redundant: the goto to the label on the next line */
                goto leave_classroom;
            leave_classroom:
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
                classroom->timer = SECOND;
                sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
                lesson_change_step(LESSON_LEAVE);
                return;
                /* @hack redundant: the break after the return, which never
                 * runs */
                break;
            case ENROLL:
                if (!classroom->held) {
                    kinder_dialog_set(LESSON_NO_CHAO_DIALOG, KINDER_WHOLE_BANK);
                    sound_play(REFUSE_SOUND, NULL, 0, 0);
                    return;
                }
                work = CHAO_WORK(classroom->held);
                if (work->profile.parameters.type == CHAO_EGG) {
                    kinder_dialog_set(LESSON_NO_CHAO_DIALOG, KINDER_WHOLE_BANK);
                    sound_play(REFUSE_SOUND, NULL, 0, 0);
                    return;
                }
                if (classroom->count == CLASS_SIZE) {
                    classroom->prompt =
                        PROMPT(LESSON_FULL_PROMPT, ACKNOWLEDGE, PROMPT_DELAY);
                    kinder_dialog_set(LESSON_FULL_DIALOG, KINDER_WHOLE_BANK);
                    classroom->timer = STEP_DELAY;
                    sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
                    lesson_change_step(LESSON_NOTICE);
                    return;
                }
                if (lesson_known(classroom->held, classroom->subject, &level) ==
                    1) {
                    if (classroom->subject <= LESSON_INSTRUMENT - 1) {
                        message = LESSON_KNOWN_DANCE_PROMPT;
                    } else if (classroom->subject <= LESSON_LAST_INSTRUMENT) {
                        message = LESSON_KNOWN_INSTRUMENT_PROMPT;
                    } else if (classroom->subject == LESSON_ART) {
                        message = LESSON_KNOWN_ART_PROMPT;
                    } else {
                        message = LESSON_KNOWN_SONG_PROMPT;
                    }
                    classroom->prompt =
                        PROMPT(message, MESSAGE_ONLY, PROMPT_DELAY);
                    lesson_change_step(LESSON_NOTICE);
                    classroom->previous = -1;
                    classroom->resume = LESSON_MENU;
                    sound_play(REFUSE_SOUND, NULL, 0, 0);
                    return;
                } else {
                    classroom->prompt =
                        PROMPT(LESSON_ENROLL_PROMPT, YES_OR_NO, PROMPT_DELAY);
                    sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                    lesson_change_step(LESSON_CONFIRM);
                }
                break;
            case PICK_UP:
                if (classroom->count != 0) {
                    classroom->prompt =
                        PROMPT(LESSON_PICK_UP_PROMPT, YES_OR_NO, PROMPT_DELAY);
                    sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                    lesson_change_step(LESSON_CONFIRM);
                }
                return;
            }
            return;
        }
        break;
    case LESSON_NOTICE:
        if (classroom->timer > 0) {
            classroom->timer--;
            return;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (!((pad = player_pads[0])->press & PDD_DGT_TA) &&
            !(pad->press & PDD_DGT_TB)) {
            break;
        }
        classroom->timer = STEP_DELAY;
        if (classroom->prompt) {
            prompt_close(classroom->prompt);
            classroom->prompt = NULL;
            kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
        }
        if (classroom->previous == LESSON_PICK_STUDENT) {
            kinder_dialog_set(LESSON_PICK_STUDENT_DIALOG, KINDER_WHOLE_BANK);
        }
        if (classroom->previous >= 0) {
            lesson_change_step(classroom->previous);
        } else {
            lesson_change_step(classroom->resume);
        }
        sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
        return;
    case LESSON_CONFIRM:
        if (classroom->timer > 0) {
            classroom->timer--;
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_KL) {
            classroom->answer = ANSWER_YES;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_KR) {
            classroom->answer = ANSWER_NO;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (player_pads[0]->press & PDD_DGT_TA) {
            if (classroom->prompt) {
                prompt_close(classroom->prompt);
                classroom->prompt = NULL;
            }
            classroom->timer = STEP_DELAY;
            if (classroom->answer == ANSWER_NO) {
                lesson_change_step(classroom->previous);
                sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
                return;
            }
            switch (classroom->choice) {
            case ENROLL:
                slot = classroom->count;
                student = &classroom->students[slot];
                student->chao = classroom->held;
                chao = student->chao;
                CHAO_WORK(chao)->flags &= ~CHAO_MOVES;
                CHAO_WORK(chao)->flags &= ~CHAO_RUNS_BEHAVIOR;
                CHAO_WORK(chao)->flags &= ~CHAO_BODY_SPRINGS;
                CHAO_WORK(chao)->flags |= CHAO_NO_INTENTIONS;
                CHAO_WORK(chao)->flags &= ~CHAO_AGES;
                lesson_known(chao, classroom->subject, &next_level);
                student->subject = classroom->subject;
                student->level = next_level;
                student->finished = 0;
                save = classroom->save;
                save->lesson[slot] = student->subject;
                if (student->subject == LESSON_SONG) {
                    save->lesson[slot] += student->level;
                } else if (student->subject == LESSON_ART) {
                    save->lesson[slot] += student->level;
                }
                save->start_time[slot] = garden_play_time();
                save->flags[slot] = 0;
                chao_set_classroom_slot(chao, slot);
                player_let_go(0);
                classroom->held = NULL;
                classroom->count++;
                classroom->notice = LESSON_ENROLLED_MESSAGE;
                sound_play(SERVICE_RESULT_SOUND, NULL, 0, 0);
                garden_file_gather();
                if (classroom->roster) {
                    task_kill(classroom->roster);
                    classroom->roster = NULL;
                }
                classroom->timer = STEP_DELAY;
                lesson_change_step(LESSON_ENROLLED);
                classroom->previous = -1;
                classroom->resume = LESSON_MENU;
                break;
            case PICK_UP:
                kinder_dialog_set(LESSON_PICK_STUDENT_DIALOG,
                                  KINDER_WHOLE_BANK);
                sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                lesson_change_step(LESSON_PICK_STUDENT);
                break;
            }
            return;
        }
        if (player_pads[0]->press & PDD_DGT_TB) {
            if (classroom->prompt) {
                prompt_close(classroom->prompt);
                classroom->prompt = NULL;
            }
            classroom->timer = STEP_DELAY;
            sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
            lesson_change_step(classroom->previous);
            return;
        }
        break;
    case LESSON_PICK_STUDENT:
        if (classroom->timer > 0) {
            classroom->timer--;
            return;
        }
        slot = classroom->selected;
#ifdef NONMATCHING
        /* this side takes no student while no slot is selected, which the
         * matching side indexes at -1 */
        student = slot < 0 ? NULL : &classroom->students[slot];
#else
        student = &classroom->students[slot];
#endif
        if ((pad_press_repeat[0] & PDD_DGT_KU) && classroom->selected > 0) {
            classroom->selected--;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if ((pad_press_repeat[0] & PDD_DGT_KD) &&
            classroom->selected < classroom->count - 1) {
            classroom->selected++;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (player_pads[0]->press & PDD_DGT_TA) {
            if (classroom->selected < 0) {
                classroom->selected = 0;
                sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
                return;
            }
            if (classroom->prompt) {
                prompt_close(classroom->prompt);
                classroom->prompt = NULL;
            }
            if (!student->finished) {
                classroom->prompt =
                    PROMPT(LESSON_UNFINISHED_PROMPT, ACKNOWLEDGE, PROMPT_DELAY);
                sound_play(REFUSE_SOUND, NULL, 0, 0);
                lesson_change_step(LESSON_NOTICE);
                return;
            } else {
                chao = student->chao;
                length = 0;
                message = LESSON_PICKUP_MESSAGE;
                do {
                    if (message != LESSON_PICKUP_MESSAGE) {
                        /* @hack spelling: length = length + 1, where
                         * length++ would do */
                        length = length + 1;
                    }
                    length +=
                        strlen(kinder_message_text(message, KINDER_WHOLE_BANK));
                    message =
                        kinder_message_next_line(message, KINDER_WHOLE_BANK);
                } while (message >= 0);
                length++;
                lines = syCalloc(1, length);
                if (lines) {
                    message = LESSON_PICKUP_MESSAGE;
                    do {
                        if (message != LESSON_PICKUP_MESSAGE) {
                            strcat(lines, line_break);
                        }
                        strcat(lines,
                               kinder_message_text(message, KINDER_WHOLE_BANK));
                        message = kinder_message_next_line(message,
                                                           KINDER_WHOLE_BANK);
                    } while (message >= 0);
                }
                if (student->subject == LESSON_ART) {
                    subject_name = kinder_message_text(LESSON_ART_MESSAGE,
                                                       KINDER_WHOLE_BANK);
                } else if (student->subject == LESSON_SONG) {
                    subject_name = kinder_message_text(LESSON_SONG_MESSAGE,
                                                       KINDER_WHOLE_BANK);
                } else {
                    subject_name = kinder_message_text(
                        student->subject + LESSON_SUBJECT_MESSAGE,
                        KINDER_WHOLE_BANK);
                }
                /* @hack redundant: the two tests of lines inside this one,
                 * which hold wherever they run */
                if (lines) {
                    if (!kinder_message_insert(lines, subject_name,
                                               &formatted)) {
                        if (lines) {
                            syFree(lines);
                        }
                        if (formatted) {
                            syFree(formatted);
                        }
                        classroom->selected = -1;
                        classroom->timer = STEP_DELAY;
                        lesson_change_step(LESSON_MENU);
                        return;
                    } else {
                        classroom->prompt =
                            prompt_create(task, -1, formatted, MESSAGE_ONLY,
                                          &classroom->answer, NOTICE_DELAY);
                        if (formatted) {
                            syFree(formatted);
                        }
                        if (lines) {
                            syFree(lines);
                        }
                    }
                } else {
                    classroom->selected = -1;
                    classroom->timer = STEP_DELAY;
                    lesson_change_step(LESSON_MENU);
                    return;
                }
                award_lesson(chao, student->subject, student->level);
                chao_set_classroom_slot(chao, -1);
                garden_file_gather();
                sound_play(SERVICE_RESULT_SOUND, NULL, 0, 0);
                if (!classroom->held) {
                    classroom->held = chao;
                } else {
                    kinder_dialog_set(LESSON_HANDS_FULL_DIALOG,
                                      KINDER_WHOLE_BANK);
                    task_kill(chao);
                    student->chao = NULL;
                }
                classroom->timer = STEP_DELAY;
                classroom->count--;
                save = classroom->save;
                student = classroom->students;
                source = classroom->students;
                /* each student past slot moves down a seat with its save.
                 * the last seat empties */
                for (i = 0, j = 0; i < CLASS_SIZE; i++, j++) {
                    if (j == slot) {
                        j++;
                        source++;
                    }
                    if (j < CLASS_SIZE) {
                        *student = *source;
                        if (student->subject == LESSON_SONG) {
                            save->lesson[i] = student->level + LESSON_SONG;
                        } else if (student->subject == LESSON_ART) {
                            save->lesson[i] = student->level + LESSON_ART;
                        } else {
                            save->lesson[i] = student->subject;
                        }
                        save->start_time[i] = save->start_time[j];
                        save->flags[i] = save->flags[j];
                        if (student->chao) {
                            chao_set_classroom_slot(student->chao, i);
                        }
                    } else {
                        student = &classroom->students[CLASS_SIZE - 1];
                        student->subject = -1;
                        student->level = -1;
                        student->chao = NULL;
                        save->start_time[CLASS_SIZE - 1] = 0;
                        save->lesson[CLASS_SIZE - 1] = -1;
                        save->flags[CLASS_SIZE - 1] = 0;
                    }
#ifdef NONMATCHING
                    /* this side steps source no further than the class's last
                     * student, which the matching side steps past */
                    if (j < CLASS_SIZE) {
                        source++;
                    }
#else
                    source++;
#endif
                    student++;
                }
                save->count = classroom->count;
                if (classroom->roster) {
                    task_kill(classroom->roster);
                    classroom->roster = NULL;
                }
                lesson_change_step(LESSON_NOTICE);
                classroom->previous = -1;
                classroom->resume = LESSON_MENU;
            }
            return;
        }
        if (!(player_pads[0]->press & PDD_DGT_TB)) {
            break;
        }
        classroom->selected = -1;
        classroom->timer = STEP_DELAY;
        sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
        lesson_change_step(LESSON_MENU);
        return;
    case LESSON_ENROLLED:
        if (classroom->timer > 0) {
            classroom->timer--;
            break;
        }
        if (classroom->notice == LESSON_ENROLLED_MESSAGE) {
            classroom->prompt =
                PROMPT(classroom->notice, MESSAGE_ONLY, NOTICE_DELAY);
            classroom->notice = 0;
            classroom->timer = STEP_DELAY;
            lesson_change_step(LESSON_NOTICE);
            classroom->previous = -1;
            classroom->resume = LESSON_MENU;
        }
        break;
    case LESSON_IDLE:
        break;
    case LESSON_LEAVE:
        if (classroom->timer > 0) {
            classroom->timer--;
        }
        if ((player_pads[0]->press & PDD_DGT_TA) || classroom->timer == 0) {
            if (classroom->held) {
                chao = classroom->held;
                kinder_carried_chao = chao;
                classroom->held = NULL;
            } else {
                kinder_carried_chao = NULL;
            }
            lesson_change_step(LESSON_DONE);
            kinder_lesson_set_state(KINDER_LESSON_DONE);
            return;
        }
        break;
    case LESSON_DONE:
        break;
    }
    classroom->state = KINDER_LESSON_RUNNING;
}

static void lesson_destroy(struct task *task)
{
    struct task *chao;
    struct task *held;
    NJS_POINT3 *pos;
    struct garden_class *save;
    struct student *student;
    Sint8 i;

    task_destroy_children(task);
    /* @bug memory: lesson_destroy reads through a null classroom, at
     * 0x0c9cedde, after kinder_lesson_create fails to allocate it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. it returns without a classroom,
     * and frees a classroom without a save */
    if (!classroom) {
        return;
    }
    if (!classroom->save) {
        syFree(classroom);
        classroom = NULL;
        return;
    }
#endif
    if (classroom->held) {
        /* @hack spelling: held, a second local for chao, where chao alone
         * would do */
        held = chao = classroom->held;
        kinder_carried_chao = chao;
        pos = &held->work->pos;
        pos->x = player_work[0]->pos.x;
        pos->y = player_work[0]->pos.y;
        pos->z = player_work[0]->pos.z;
        classroom->held = NULL;
    }
    save = classroom->save;
    student = classroom->students;
    /* @bug memory: lesson_destroy writes through a null save, at 0x0c9cee30,
     * where garden_file_class gave kinder_lesson_create none */
    save->count = classroom->count;
    for (i = 0; i < CLASS_SIZE; i++, student++) {
        if (student->chao) {
            task_kill(student->chao);
            student->chao = NULL;
        }
    }
    classroom->held = NULL;
    classroom->prompt = NULL;
    classroom->roster = NULL;
    classroom->task = NULL;
    classroom->save = NULL;
    for (i = 0; i < CLASS_SIZE; i++) {
        if (classroom->background[i]) {
            task_kill(classroom->background[i]);
            classroom->background[i] = NULL;
        }
    }
    syFree(classroom);
    classroom = NULL;
}

struct task *kinder_lesson_create(struct task *parent)
{
    struct task *task;
    struct garden_class *save;
    struct student *student;
    struct task *chao;
    int code;
    int elapsed;
    Uint8 count;
    Sint8 i;
#ifdef NONMATCHING
    /* this side makes no lesson without the script and string banks, which the
     * lesson reads */
    if (!kinder_script_bank(KINDER_WHOLE_BANK) ||
        !kinder_string_bank(KINDER_WHOLE_BANK)) {
        return NULL;
    }
#endif
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, lesson_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = lesson_destroy;
    classroom = syCalloc(1, sizeof(*classroom));
    if (!classroom) {
        task_destroy(task);
        return NULL;
    }
    classroom->task = task;
    classroom->state = KINDER_LESSON_RUNNING;
    classroom->held = kinder_carried_chao;
    classroom->save = garden_file_class();
    if (!classroom->save) {
        task_destroy(task);
        return NULL;
    }
    classroom->play_time = garden_play_time();
    save = classroom->save;
    student = classroom->students;
    count = save->count;
    classroom->count = save->count;
#ifdef NONMATCHING
    /* this side takes an empty class from a save whose count is outside the
     * class. the matching side copies past the class */
    if (save->count < 0 || save->count > CLASS_SIZE) {
        count = 0;
        classroom->count = 0;
    }
#endif
    for (i = 0; i < CLASS_SIZE; i++, student++) {
        if (i < count) {
            /* @hack redundant: the copy of below_floor, which nothing reads */
#ifdef DREAMCAST
            NJS_POINT3 below_floor = {
#include "chaostgkinder/lesson/below_floor.inc"
            };
#endif
            student->chao = garden_spawn_classmate(i, NULL, 0);
            if (student->chao) {
                chao = student->chao;
                CHAO_WORK(chao)->flags &= ~CHAO_COLLIDES;
                CHAO_WORK(chao)->flags &= ~CHAO_MOVES;
                CHAO_WORK(chao)->flags &= ~CHAO_RUNS_BEHAVIOR;
                CHAO_WORK(chao)->flags &= ~CHAO_BODY_SPRINGS;
                CHAO_WORK(chao)->flags |= CHAO_NO_INTENTIONS;
                CHAO_WORK(chao)->flags &= ~CHAO_AGES;
                CHAO_WORK(chao)->flags &= ~CHAO_VISIBLE;
                code = save->lesson[i];
                if (code >= LESSON_SONG) {
                    student->subject = code / SUBJECT_SPAN * SUBJECT_SPAN;
                    student->level = code % SUBJECT_SPAN;
                } else {
                    student->subject = code;
                    student->level = 0;
                }
                if (save->flags[i] & GARDEN_CLASS_FINISHED) {
                    student->finished = 1;
                } else {
                    elapsed = classroom->play_time - save->start_time[i];
                    if (elapsed < 0) {
                        save->start_time[i] = classroom->play_time;
                    }
                    if (elapsed > LESSON_DURATION) {
                        student->finished = 1;
                        save->flags[i] |= GARDEN_CLASS_FINISHED;
                    } else {
                        student->finished = 0;
                    }
                }
            } else {
                student->subject = -1;
                student->level = -1;
                student->chao = NULL;
                student->finished = 0;
            }
        } else {
            student->subject = -1;
            student->level = -1;
            student->chao = NULL;
            student->finished = 0;
        }
        classroom->background[i] = classmate_create(task, NULL, i);
    }
    return task;
}

void kinder_lesson_release(void)
{
    if (!classroom) {
        return;
    }
    task_kill(classroom->task);
}

Sint8 kinder_lesson_state(void)
{
    if (!classroom) {
        return KINDER_LESSON_ABSENT;
    }
    return classroom->state;
}

void kinder_lesson_set_state(Sint8 state)
{
    if (classroom) {
        classroom->state = state;
    }
}

static void prompt_step(struct task *task)
{
    struct task_work *work = task->work;
    if (work->flags & PROMPT_CLOSING) {
        work->timer--;
        if (work->timer == CLEAR_FRAMES) {
            task_destroy_children(task);
            return;
        }
        if (work->timer == 0) {
            task_kill(task);
            return;
        }
    }
    if (work->flags & PROMPT_OPENING) {
        work->timer--;
        if (work->timer == 0) {
            work->flags &= ~PROMPT_OPENING;
        }
    }
}

static void lesson_prompt_destroy(struct task *task)
{
    task_destroy_children(task);
}

static struct task *prompt_create(struct task *parent, Sint16 message,
                                  const char *text, Sint8 style,
                                  const Sint8 *answer, Sint16 delay)
{
    struct kinder_message_label label;
    struct service_box box;
    struct service_choice choice;
    struct task *task;
    struct task *caption;
    struct task *created;
    struct task_work *work;
    const char *button_text;
    void (*add_button)(struct task *, int, const struct service_box *,
                       const struct service_choice *, const char *);
    Float height;
    Float x;
    Float y;
    Float width;
    Float button_y;
    /* @hack type: Sint16 sizes, which kinder_message_label_size fills as
     * Uint16, where Uint16 locals would do for sizes below 0x8000 */
    Sint16 text_height;
    Sint16 text_width;

    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, prompt_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = lesson_prompt_destroy;
    work = task->work;
#ifdef NONMATCHING
    /* this side zeroes box and label, whose fields the lines below don't all
     * set */
    memset(&box, 0, sizeof(box));
    memset(&label, 0, sizeof(label));
#endif
    work->tick = style;
    work->timer = delay;
    work->flags |= PROMPT_OPENING;
    label.bank = KINDER_WHOLE_BANK;
    label.id = 0;
    label.color = BLACK_TEXT;
    label.selection = NULL;
    label.pos.x = 320.0F;
    label.pos.y = 240.0F;
    label.pos.z = -1.2F;
    label.scale = 1.0F;
    label.width = 336.0F;
    if (message < 0) {
        label.flags = CENTER_FIT | KINDER_MESSAGE_LABEL_LITERAL;
        label.message = 0;
        label.text = text;
    } else {
        label.flags = CENTER_FIT;
        label.message = message;
        label.text = NULL;
    }
    /* @hack spelling: created, a second local for caption, where caption
     * alone would do */
    caption = created = kinder_message_label_create(task, &label);
    if (created) {
        kinder_message_label_size(caption, (Uint16 *)&text_width,
                                  (Uint16 *)&text_height);
    } else {
        task_destroy(task);
        return NULL;
    }
    /* @hack redundant: each case's assignment of style, which already holds
     * it */
    switch (style) {
#ifdef NONMATCHING
    /* this side destroys the prompt for a style the switch doesn't name */
    default:
        task_destroy(task);
        return NULL;
#endif
    case MESSAGE_ONLY:
        if (text_height > 88.0F) {
            if (text_height % 8) {
                height = text_height - (text_height % 8) + 8;
            } else {
                height = text_height;
            }
            height += 16.0F;
        } else {
            height = 112.0F;
        }
        style = MESSAGE_ONLY;
        break;
    case YES_OR_NO:
        if (text_height > 64.0F) {
            if (text_height % 8) {
                height = text_height - (text_height % 8) + 8;
            } else {
                height = text_height;
            }
            height += 64.0F;
        } else {
            height = 128.0F;
        }
        style = YES_OR_NO;
        break;
    case ACKNOWLEDGE:
        text_height = text_height + 112.0F;
        if (text_height % 64) {
            height = text_height - (text_height % 64) + 64;
        } else {
            height = text_height;
        }
        style = ACKNOWLEDGE;
        break;
    }
    box.pos.x = 144.0F;
    box.pos.y = 240.0F - (height * 0.5F);
    box.pos.z = -1.8F;
    box.flags = 0;
    box.width = 352.0F;
    box.height = height;
    service_frame_create(task, style, &box);
    x = box.pos.x;
    y = box.pos.y;
    width = box.width;
    switch (style) {
    case ACKNOWLEDGE:
        caption->work->pos.y = y + 80.0F;
        break;
    case MESSAGE_ONLY:
        caption->work->pos.y = (height * 0.5F) - (text_height * 0.5F) + y;
        break;
    case YES_OR_NO:
        if (height <= 128.0F) {
            caption->work->pos.y = (height * 0.5F) - text_height + y + 16.0F;
        } else {
            caption->work->pos.y = y + 16.0F;
        }
        break;
    }
    if (style == MESSAGE_ONLY) {
        return task;
    }
    box.flags = SERVICE_TRACK_SELECTION;
    /* @hack redundant: a test of x that never runs */
    if (0) {
        if (x) {
        }
    }
    box.pos.z = -1.7F;
    box.color = 0;
    box.width = 96.0F;
    box.height = 32.0F;
    /* @hack spelling: the calls through add_button with button_text, where
     * calling service_text_button with each text would do */
    add_button = service_text_button;
    choice.selection = answer;
    y += height;
    button_y = y - 48.0F;
    switch (style) {
    case ACKNOWLEDGE:
        button_text = LESSON_CONFIRM_TEXT;
        box.pos.x = 320.0F - (box.width * 0.5F);
        box.pos.y = y - 48.0F;
        choice.selected_color = ACKNOWLEDGE_COLOR;
        add_button(task, ANSWER_YES, &box, &choice, button_text);
        return task;
        /* @hack redundant: the break after the return, which never runs */
        break;
    case YES_OR_NO:
        box.pos.x = x + 24.0F;
        box.pos.y = button_y;
        choice.selected_color = ANSWER_COLOR;
        button_text = LESSON_YES_TEXT;
        add_button(task, ANSWER_YES, &box, &choice, button_text);
        box.pos.x = x + width - 120.0F;
        box.pos.y = button_y;
        choice.selected_color = ANSWER_COLOR;
        button_text = LESSON_NO_TEXT;
        add_button(task, ANSWER_NO, &box, &choice, button_text);
        break;
    }
    /* @hack redundant: a test of height that never runs */
    if (0) {
        if (height) {
        }
    }
    return task;
}

static void prompt_close(struct task *task)
{
    struct task_work *work;
    if (task && task->work) {
        work = task->work;
        work->flags |= PROMPT_CLOSING;
        work->timer = CLOSE_FRAMES;
    }
}

static void roster_step(struct task *task)
{
    if (game_state.time_stopped) {
        return;
    }
}

#define ROW_HEIGHT 56.0F
#define ROSTER_WORK(task) ((struct roster_work *)(task)->work)
#define CLASSMATE_WORK(task) ((struct classmate_work *)(task)->work)
#define DRAW_CELL(i) njDrawSprite2D(&sprite, (i), depth, sprite.tanim[(i)].attr)
#define ROSTER_BLEND()                                                         \
    {                                                                          \
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);    \
        njColorBlendingMode(NJD_DESTINATION_COLOR,                             \
                            NJD_COLOR_BLENDING_INVSRCALPHA);                   \
    }

static void roster_draw(struct task *task)
{
    struct roster_work *work;
    Float top;
    Float height;
    NJS_SPRITE sprite;
    Float depth;
    work = ROSTER_WORK(task);
    ROSTER_BLEND();
    constant_material_set(1, 1, 1, 1);
    sprite.p.x = 0;
    sprite.p.y = 0;
#ifdef NONMATCHING
    /* this side sets the sprite's z, which nothing else sets */
    sprite.p.z = 0;
#endif
    sprite.sx = 1;
    sprite.sy = 1;
    sprite.ang = 0;
    sprite.tlist = &kinder_service_textures;
    sprite.tanim = roster_cells;
    top = work->pos.y;
    if (work->count == 0) {
        height = 32.0F;
    } else {
        height = work->count * ROW_HEIGHT;
    }
    sprite.p.x = 32;
    sprite.p.y = top;
    depth = -2.4F;
    DRAW_CELL(FRAME_TOP_LEFT);
    sprite.p.x = 44;
    sprite.sx = 184;
    DRAW_CELL(FRAME_TOP);
    sprite.p.x = 224;
    sprite.sx = 1;
    DRAW_CELL(FRAME_TOP_RIGHT);
    depth = -2.42F;
    sprite.p.x = 32;
    sprite.p.y = top + 24.0F;
    sprite.sx = 1;
    sprite.sy = height + 10.0F;
    DRAW_CELL(FRAME_LEFT);
    sprite.p.x = 224;
    sprite.sx = 1;
    DRAW_CELL(FRAME_RIGHT);
    sprite.p.x = 32;
    sprite.p.y = top + height + 30.0F;
    sprite.sx = 1;
    sprite.sy = 1;
    DRAW_CELL(FRAME_BOTTOM_LEFT);
    sprite.p.x = 46;
    sprite.sx = 180;
    DRAW_CELL(FRAME_BOTTOM);
    sprite.p.x = 224;
    sprite.sx = 1;
    DRAW_CELL(FRAME_BOTTOM_RIGHT);
    depth = -2.99F;
    sprite.p.x = 40;
    sprite.p.y = top + 32.0F;
    sprite.sx = 192;
    sprite.sy = height;
    constant_material_set(1.0F, 206.0F / 255.0F, 243.0F / 255.0F,
                          212.0F / 255.0F);
    DRAW_CELL(FILL);
    constant_material_set(0, 0, 0, 0);
}

static void roster_destroy(struct task *task)
{
}

static struct task *roster_create(struct task *parent, struct student *students,
                                  int count, Sint8 *selection)
{
    struct task *task;
    struct roster_work *work;
    struct student *student;
    void *extra;
    Sint8 i;
    if (!parent) {
        return NULL;
    }
    /* @hack redundant: a test of selection that never runs */
    if (0) {
        if (selection) {
        }
    }
    task = task_create_child(TASK_WORK, roster_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = roster_draw;
    task->destructor = roster_destroy;
    work = ROSTER_WORK(task);
    extra = syCalloc(1, TASK_EXTRA_SIZE);
    if (!extra) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = extra;
    work->count = count;
    work->pos.x = 32;
    if (work->count == CLASS_SIZE) {
        work->pos.y = 96;
    } else {
        work->pos.y = 112;
    }
    if (selection) {
        work->selection = selection;
    } else {
        work->selection = &work->cursor;
    }
    {
        struct kinder_message_label label;
        label = plain_label;
        label.flags = CENTER_FIT;
        label.color = WHITE_TEXT;
        label.selected_color = WHITE_TEXT;
        label.message = LESSON_ROSTER_LABEL;
        label.pos.x = 136;
        label.pos.y = work->pos.y + 16.0F;
        label.pos.z = -2;
        label.width = 192;
        kinder_message_label_create(task, &label);
    }
    {
        NJS_POINT3 pos;
        pos.x = 32;
        if (work->count == CLASS_SIZE) {
            pos.y = 128;
        } else {
            pos.y = 144;
        }
        pos.z = -2;
        for (i = 0, student = students; i < work->count; i++, student++) {
            student_row_create(task, student, selection, i, &pos);
            pos.y += ROW_HEIGHT;
        }
    }
    return task;
}

static void student_row_step(struct task *task)
{
    struct roster_work *work = ROSTER_WORK(task);
    Sint8 *selection = work->selection;
    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & ROW_SELECTED) {
        if (work->amplitude > 0.0F) {
            work->phase += EIGHTH_TURN;
            work->scale = 1.0F + (njSin(work->phase) * work->amplitude);
            work->amplitude *= 0.9F;
            if (work->amplitude < 0.01F) {
                work->amplitude = 0;
                work->scale = 1;
                work->phase = 0;
            }
        }
        if (*selection != work->id) {
            work->scale = 1;
            work->phase = 0;
            work->flags &= ~ROW_SELECTED;
        }
    } else if (*selection == work->id) {
        work->amplitude = 0.1F;
        work->flags |= ROW_SELECTED;
    }
}

/* @hack redundant: color, which nothing reads */
static void student_draw_glyph(const NJS_POINT3 *pos, Uint8 code, Uint32 color)
{
    NJS_SPRITE sprite;
    NJS_TEXANIM glyph;
    Float tex_u;
    Float tex_v;
    /* @hack spelling: cell, a copy of code, where reading code would do */
    Uint8 cell = code;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.p.z = 0;
    sprite.ang = 0;
    sprite.sx = 1;
    sprite.sy = 1;
    sprite.tlist = &chao_common_texlist;
    sprite.tanim = &glyph;
    tex_u = cell % 16 * 16.0F;
    tex_v = (cell >> 4) * 16.0F;
    glyph.sx = glyph.sy = 20;
    glyph.cx = glyph.cy = 0;
    glyph.u1 = tex_u;
    glyph.u2 = tex_u + 16.0F;
    glyph.v1 = tex_v;
    glyph.v2 = tex_v + 16.0F;
    glyph.texid = 0;
    njDrawSprite2D(&sprite, 0, pos->z, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
}

static void student_row_draw(struct task *task)
{
    struct roster_work *work;
    Float center;
    Float half;
    Float left;
    NJS_SPRITE sprite;
    NJS_POINT3 pos;
    Float depth;
    Float width;
    Float x;
    const Uint8 *name;
    struct chao_work *chao;
    int icon;
    struct student *student;

    work = ROSTER_WORK(task);
    ROSTER_BLEND();
    constant_material_set(0, 0, 0, 0);
    sprite.p.x = 0;
    sprite.p.y = 0;
    sprite.p.z = 0;
    sprite.sx = work->scale;
    sprite.sy = work->scale;
    sprite.ang = 0;
    sprite.tlist = &kinder_service_textures;
    sprite.tanim = roster_cells;
    depth = work->pos.z + 0.01F;
    width = work->scale * 208.0F;
    center = 136.0F;
    half = width * 0.5F;
    left = center - half;
    x = center;
    sprite.p.x = left;
    sprite.p.y = work->pos.y + 4.0F;
    DRAW_CELL(ROW_LEFT);
    sprite.p.x = x + half - (work->scale * 32.0F);
    DRAW_CELL(ROW_RIGHT);
    sprite.p.x = left + (work->scale * 32.0F) - 2.0F;
    width -= work->scale * 56.0F;
    sprite.sx = width;
    DRAW_CELL(ROW_MIDDLE);
    depth += 0.02F;
    sprite.p.x = 46;
    sprite.p.y = work->pos.y + 34.0F;
    sprite.sx = 1;
    sprite.sy = 1;
    if (work->count == 1) {
        icon = ICON_FINISHED;
    } else {
        icon = ICON_UNFINISHED;
    }
    DRAW_CELL(icon);
    if (work->flags & ROW_SELECTED) {
        depth -= 0.01F;
        sprite.p.x = 48;
        sprite.p.y = work->pos.y + 32.0F;
        constant_material_set(1, 1, 1, 127.0F / 255.0F);
        sprite.sx = 176;
        sprite.sy = 24;
        DRAW_CELL(FILL);
    }
    student = work->student;
    chao = CHAO_WORK(student->chao);
    name = (const Uint8 *)chao->profile.parameters.name;
    constant_material_set(1, 0, 0, 0);
    pos.x = 66;
    pos.y = work->pos.y + 32.0F;
    pos.z = depth += 0.05F;
#ifdef NONMATCHING
    /* a name can fill the profile's eight bytes, with no terminator */
    while (name < (const Uint8 *)chao->profile.parameters.name +
                      sizeof(chao->profile.parameters.name) &&
           *name) {
#else
    while (*name) {
#endif
        student_draw_glyph(&pos, *name++, BLACK_TEXT);
        pos.x += 20.0F;
    }
    constant_material_set(0, 0, 0, 0);
}

static void student_row_destroy(struct task *task)
{
    task_destroy_children(task);
}

static struct task *student_row_create(struct task *parent,
                                       struct student *student,
                                       Sint8 *selection, Sint8 id,
                                       const NJS_POINT3 *pos)
{
    struct task *task;
    struct roster_work *work;
    struct kinder_message_label label;
    if (!student) {
        return NULL;
    }
    if (student->subject < 0) {
        return NULL;
    }
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, student_row_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = student_row_draw;
    task->destructor = student_row_destroy;
    work = ROSTER_WORK(task);
    work->pos = *pos;
    work->pos.z += 0.01F;
    work->student = student;
    if (selection) {
        work->selection = selection;
    } else {
        work->selection = &work->cursor;
    }
    work->count = student->finished;
    work->id = id;
    label = plain_label;
    label.flags = CENTER_FIT;
    label.color = WHITE_TEXT;
    label.selected_color = WHITE_TEXT;
    if (student->subject >= LESSON_ART) {
        label.message = LESSON_ART_MESSAGE;
    } else if (student->subject >= LESSON_SONG) {
        label.message = LESSON_SONG_MESSAGE;
    } else {
        label.message = student->subject + LESSON_SUBJECT_MESSAGE;
    }
    label.pos.x = 136;
    label.pos.y = work->pos.y + 14.0F;
    label.pos.z = work->pos.z + 0.03F;
    label.scale = 0.9F;
    label.width = 176;
    kinder_message_label_create(task, &label);
    work->scale = 1;
    return task;
}

/* a classmate plays a motion at random and blends into it over
 * BLEND_FRAMES. it holds the motion from MOTION_SECONDS to MOTION_SECONDS +
 * MOTION_SPREAD seconds */
#define BLEND_FRAMES 30
#define MOTION_SECONDS 60.0F
#define MOTION_SPREAD 40.0F
#define CLASSMATE_MOTION() ((int)(rand() * (CLASSMATE_MOTIONS * RAND_SCALE)))
#define CLASSMATE_DELAY()                                                      \
    (((rand() * (MOTION_SPREAD * RAND_SCALE)) + MOTION_SECONDS) * SECOND)

static void classmate_step(struct task *task)
{
    struct classmate_work *work = CLASSMATE_WORK(task);
    struct task *chao = work->chao;
    int motion;
    if (game_state.time_stopped) {
        return;
    }
    if (work->timer > 0) {
        work->timer--;
    } else {
        motion = CLASSMATE_MOTION();
        chao_link_motion_frames(chao, classmate_motions[motion], BLEND_FRAMES);
        work->timer = CLASSMATE_DELAY();
    }
}

static void classmate_destroy(struct task *task)
{
    struct classmate_work *work = CLASSMATE_WORK(task);
    struct task *chao = work->chao;
    /* @bug memory: lesson_destroy kills each classmate again, at 0x0c9ceeb0,
     * after task_destroy_children destroyed it. classmate_destroy leaves
     * its task in classroom->background */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the destroy clears the
     * classmate's slot in classroom->background */
    classroom->background[work->id] = NULL;
#endif
    if (chao) {
        task_kill(chao);
        work->chao = NULL;
    }
}

static struct task *classmate_create(struct task *parent, struct task *chao,
                                     int seat)
{
    struct task *task;
    struct classmate_work *work;
    struct task *actor;
#ifdef DREAMCAST
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[12];
#endif
    int motion;
    /* @hack type: 0 for each null this returns, where NULL would do */
    if (!parent) {
        return 0;
    }
    task = task_create_child(TASK_WORK, classmate_step, parent);
    if (!task) {
        return 0;
    }
    task->destructor = classmate_destroy;
    work = CLASSMATE_WORK(task);
    work->id = seat;
    /* within an eighth turn either side of 0 */
    work->yaw = (rand() * (QUARTER_TURN * RAND_SCALE)) - EIGHTH_TURN;
    if (!chao) {
        actor = chao_create(NULL, NULL, NULL, &classmate_positions[work->id],
                            work->yaw, NULL);
    } else {
        actor = chao;
    }
    if (!actor) {
        /* @hack type: (struct task *)0, where NULL would do */
        work->chao = (struct task *)0;
        task_destroy(task);
        return 0;
    }
    CHAO_WORK(actor)->flags &= ~CHAO_COLLIDES;
    CHAO_WORK(actor)->flags &= ~CHAO_MOVES;
    CHAO_WORK(actor)->flags &= ~CHAO_RUNS_BEHAVIOR;
    CHAO_WORK(actor)->flags &= ~CHAO_BODY_SPRINGS;
    CHAO_WORK(actor)->flags &= ~CHAO_AGES;
    motion = CLASSMATE_MOTION();
    chao_link_motion_frames(actor, classmate_motions[motion], 0);
    work->timer = CLASSMATE_DELAY();
    work->chao = actor;
    return task;
}
