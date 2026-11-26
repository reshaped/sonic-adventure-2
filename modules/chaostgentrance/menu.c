/* the race entrance's menu. the player picks a race group, then the Chao
 * that race, then a course. the racers then board the elevator */
#include <ninja.h>
#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/life.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/transfer.h"
#include "chao/work.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/options.h"
#include "chaostgentrance/roster.h"
#include "save/card.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

struct entrance_menu *entrance_menu = NULL;
struct garden_transfer entrance_card_read;
Sint32 entrance_roster_ready;
static Uint8 scan_drive;

#ifdef NONMATCHING
/* this side sorts the roster by one test of each Chao, can_enter. the matching
 * side runs three loops and their hacks. this side places no saved Chao whose
 * garden is past CHAO_GARDENS or whose row is full. the matching side writes
 * past the roster for such a Chao */
static Bool can_enter(const struct chao_parameters *chao, int group,
                      Bool from_card)
{
    if (chao->type <= CHAO_EGG || chao->classroom_slot != -1) {
        return FALSE;
    }
    switch (group) {
    case RACE_DARK_COURSES:
        return !chao_type_is_dark(chao->type);
    case RACE_HERO_COURSES:
        return !chao_type_is_hero(chao->type);
    default:
        return from_card || chao->type <= CHAO_DARK_CHAOS;
    }
}

void entrance_sort_roster(void)
{
    int counts[CHAO_GARDENS + 1] = {0, 0, 0, 0};
    int group;
    int index;
    int garden;
    struct chao_save *save;

    memset(entrance_roster, 0, sizeof(entrance_roster));
    group = garden_race_group();
    for (index = 0; index < RACE_ENTRANTS; index++) {
        save = &garden_vms_racers[index];
        if (can_enter(&save->record.saved.parameters, group, TRUE)) {
            entrance_roster[ENTRANCE_CARD_ROW][index] = save;
        }
    }
    for (index = 0; index < CHAO_SAVE_SLOTS; index++) {
        save = &garden_saved_chao[index];
        garden = save->record.saved.parameters.garden;
        if (garden <= CHAO_GARDENS && counts[garden] < RACE_ENTRANTS &&
            can_enter(&save->record.saved.parameters, group, FALSE)) {
            entrance_roster[garden][counts[garden]++] = save;
        }
    }
}
#else

#define DEAD_TEST(value)                                                       \
    /* @hack redundant: a test of value that never runs */                     \
    if (0) {                                                                   \
        if (value) {                                                           \
        }                                                                      \
    }

void entrance_sort_roster(void)
{
    int counts[CHAO_GARDENS + 1] = {0, 0, 0, 0};
    struct chao_save *save;
    /* @hack type: a volatile pointer to the card's save in each group's
     * loop, which nothing reads */
    struct chao_save *volatile dark_card;
    struct chao_save *volatile hero_card;
    struct chao_save *volatile other_card;
    struct chao_parameters *chao;
    Uint8 type;
    int form;
    Sint8 classroom;
    int garden;
    int index;

    memset(entrance_roster, 0, sizeof(entrance_roster));
    switch (garden_race_group()) {
    case RACE_DARK_COURSES:
        for (index = 0; index < RACE_ENTRANTS; index++) {
            chao = (dark_card = &garden_vms_racers[index])->record.head;
            type = chao->type;
            classroom = chao->classroom_slot;
            if (type > CHAO_EGG && !chao_type_is_dark(type) &&
                classroom == -1) {
                entrance_roster[ENTRANCE_CARD_ROW][index] =
                    &garden_vms_racers[index];
            } else {
                entrance_roster[ENTRANCE_CARD_ROW][index] = NULL;
            }
        }
        for (index = 0; index < CHAO_SAVE_SLOTS; index++) {
            chao = garden_saved_chao[index].record.head;
            DEAD_TEST(chao);
            classroom = chao->classroom_slot;
            form = chao->type;
            if (form > CHAO_EGG && !chao_type_is_dark(form) &&
                classroom == -1) {
                garden = chao->garden;
                entrance_roster[garden][counts[garden]] =
                    &garden_saved_chao[index];
                counts[garden]++;
            }
        }
        break;
    case RACE_HERO_COURSES:
        for (index = 0; index < RACE_ENTRANTS; index++) {
            chao = (hero_card = &garden_vms_racers[index])->record.head;
            type = chao->type;
            DEAD_TEST(chao);
            classroom = chao->classroom_slot;
            DEAD_TEST(chao);
            if (type > CHAO_EGG && !chao_type_is_hero(type) &&
                classroom == -1) {
                entrance_roster[ENTRANCE_CARD_ROW][index] =
                    &garden_vms_racers[index];
            } else {
                entrance_roster[ENTRANCE_CARD_ROW][index] = NULL;
            }
        }
        for (index = 0; index < CHAO_SAVE_SLOTS; index++) {
            chao = garden_saved_chao[index].record.head;
            DEAD_TEST(chao);
            classroom = chao->classroom_slot;
            DEAD_TEST(chao);
            form = chao->type;
            if (form > CHAO_EGG && !chao_type_is_hero(form) &&
                classroom == -1) {
                garden = chao->garden;
                DEAD_TEST(chao);
                entrance_roster[garden][counts[garden]] =
                    &garden_saved_chao[index];
                counts[garden]++;
            }
        }
        break;
    default:
        for (index = 0; index < RACE_ENTRANTS; index++) {
            classroom =
                (chao = (other_card = &garden_vms_racers[index])->record.head)
                    ->classroom_slot;
            if (chao->type > CHAO_EGG && classroom == -1) {
                entrance_roster[ENTRANCE_CARD_ROW][index] =
                    &garden_vms_racers[index];
            } else {
                DEAD_TEST(type);
                entrance_roster[ENTRANCE_CARD_ROW][index] = NULL;
            }
        }
        for (index = 0; index < CHAO_SAVE_SLOTS; index++) {
            save = &garden_saved_chao[index];
            chao = save->record.head;
            form = chao->type;
            classroom = chao->classroom_slot;
            /* @hack spelling: < CHAO_DARK_CHAOS + 1, where <= CHAO_DARK_CHAOS
             * would do */
            if (form > CHAO_EGG && form < CHAO_DARK_CHAOS + 1 &&
                classroom == -1) {
                garden = chao->garden;
                entrance_roster[garden][counts[garden]] =
                    &garden_saved_chao[index];
                counts[garden]++;
            }
        }
        break;
    }
}
#endif

/* work->tick counts the frames the scan waits for drives to be ready. once
 * it passes DRIVE_DELAY, the drive waited on counts as empty */
#define DRIVE_DELAY 10
/* a multiplayer race needs at least FEWEST_RACERS Chao */
#define FEWEST_RACERS 2
/* the menu takes a garden as open once either of its flags is set */
#define HERO_GARDEN_FLAGS (GARDEN_HERO_OPEN | GARDEN_HERO_PLAY)
#define DARK_GARDEN_FLAGS (GARDEN_DARK_OPEN | GARDEN_DARK_PLAY)
/* A skips the ride to the race only once more than SKIP_DELAY frames of it
 * have run */
#define SKIP_DELAY 30
/* the delays the menu gives the bar, the picker's title and the captions */
#define BAR_DELAY 150
#define PICKER_TITLE_DELAY 10
#define CAPTION_DELAY 30
/* a course screen's back button stands at BACK_X, BACK_Y, or on the jewel
 * screen at BACK_X, JEWEL_BACK_Y */
#define BACK_X 192.0F
#define BACK_Y 391.0F
#define JEWEL_BACK_Y 397.0F
/* the bar's two parts as the racers board, each of BOARDING_LENGTH
 * messages */
#define FIRST_BOARDING_MESSAGE 60
#define SECOND_BOARDING_MESSAGE 65
#define BOARDING_LENGTH 2

enum {
    MENU_MOVE_SOUND = 0x8000,
    MENU_TOGGLE_SOUND = 0x800e,
    MENU_BACK_SOUND = 0x8013,
    MENU_CONFIRM_SOUND = 0x100e
};

#define PLAY(sound) sound_play((sound), NULL, 0, 0)
#define PRESSED(button) (player_pads[0]->press & (button))
#define REPEATED(button) (pad_press_repeat[0] & (button))
#define CHANGE_MODE(next)                                                      \
    {                                                                          \
        work->mode = (next);                                                   \
        work->submode = 0;                                                     \
    }
#define REQUEST(action)                                                        \
    {                                                                          \
        entrance_menu->request = (action);                                     \
        entrance_menu->wait = TRUE;                                            \
    }
#define BACK_TO(mode)                                                          \
    REQUEST(ENTRANCE_MENU_BACK);                                               \
    PLAY(MENU_BACK_SOUND);                                                     \
    CHANGE_MODE(mode)
#define ACCEPT_TO(mode)                                                        \
    REQUEST(ENTRANCE_MENU_SELECT);                                             \
    CHANGE_MODE(mode);                                                         \
    PLAY(MENU_CONFIRM_SOUND)

/* the exit leads back the way the player came. it leads to the Neutral
 * Garden from the lobby, under CHAO_RACE_MENU_NONE. from elsewhere it
 * leads out of the stage */
#define EXIT_GROUP_MENU()                                                      \
    task_kill(task);                                                           \
    if (game_state.chao_race_menu != CHAO_RACE_MENU_NONE) {                    \
        stage_end(STAGE_END_CLEAR);                                            \
    } else {                                                                   \
        chao_change_area(CHAO_AREA_NEUTRAL);                                   \
        PLAY(MENU_BACK_SOUND);                                                 \
    }

/* @hack spelling: a macro, expanded at each use, where a function would do */
#define PICKER_SCROLL(bottom)                                                  \
    if (REPEATED(PDD_DGT_KU)) {                                                \
        if (entrance_menu->cursor > 0) {                                       \
            entrance_menu->cursor--;                                           \
        }                                                                      \
        PLAY(MENU_MOVE_SOUND);                                                 \
    }                                                                          \
    if (REPEATED(PDD_DGT_KD)) {                                                \
        if (entrance_menu->cursor < (bottom)) {                                \
            entrance_menu->cursor++;                                           \
        }                                                                      \
        PLAY(MENU_MOVE_SOUND);                                                 \
    }
/* @hack spelling: a macro, expanded at each use, where a function would do */
#define PICKER_BACK(bottom)                                                    \
    if (PRESSED(PDD_DGT_TB)) {                                                 \
        if (entrance_menu->cursor == (bottom)) {                               \
            BACK_TO(MENU_CLOSE_PICKER);                                        \
        } else {                                                               \
            entrance_menu->cursor = (bottom);                                  \
            PLAY(MENU_MOVE_SOUND);                                             \
        }                                                                      \
    }

/* NOLINTNEXTLINE(readability-function-size) */
static void entrance_menu_step(struct task *task)
{
    struct task_work *work = task->work;
    int index;
    /* @hack type: number and chao in one union, where two locals would do */
    union {
        int number;
        struct chao_save *chao;
    } choice;
    int row;
    int count;
    int slot;

    entrance_menu->old_column = entrance_menu->column;
    entrance_menu->old_cursor = entrance_menu->cursor;
    switch (work->mode) {
    case MENU_INIT:
        CHANGE_MODE(MENU_BUILD_GROUPS);
        entrance_message_create(task, BAR_DELAY);
        entrance_dimmer_create(task);
        break;
    case MENU_BUILD_GROUPS:
        entrance_menu->screen = ENTRANCE_SELECT_GROUP;
        entrance_menu->column = 0;
        entrance_menu->cursor = 0;
        entrance_title_create(task, 0);
        entrance_event_buttons(task);
        work->tick = 0;
        CHANGE_MODE(MENU_WAIT_GROUPS);
        entrance_menu->wait = TRUE;
        /* fall through */
    case MENU_WAIT_GROUPS:
        if (!entrance_menu->wait) {
            CHANGE_MODE(MENU_GROUP_INPUT);
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_GROUP_INPUT:
        if (REPEATED(PDD_DGT_KU)) {
            if (!entrance_menu->cursor) {
                entrance_menu->cursor = entrance_menu->exit_row;
            } else {
                entrance_menu->cursor--;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KD)) {
            if (entrance_menu->cursor >= entrance_menu->exit_row) {
                entrance_menu->cursor = 0;
            } else {
                entrance_menu->cursor++;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (PRESSED(PDD_DGT_TA)) {
            if (entrance_menu->cursor == entrance_menu->exit_row) {
                EXIT_GROUP_MENU();
            } else {
                ACCEPT_TO(MENU_CLOSE_GROUPS);
            }
        }
        if (PRESSED(PDD_DGT_TB)) {
            if (entrance_menu->cursor == entrance_menu->exit_row) {
                EXIT_GROUP_MENU();
            } else {
                entrance_menu->cursor = entrance_menu->exit_row;
                PLAY(MENU_MOVE_SOUND);
            }
        }
        break;
    case MENU_CLOSE_GROUPS:
        if (!entrance_menu->wait) {
            CHANGE_MODE(MENU_BUILD_PICKER);
            entrance_menu->request = ENTRANCE_MENU_IDLE;
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_BUILD_PICKER:
        scan_drive = 0;
        CHANGE_MODE(MENU_SCAN_DRIVE);
        work->tick = 0;
        pause_disable();
        entrance_roster_ready = FALSE;
        entrance_menu->screen = ENTRANCE_SELECT_CHAO;
        entrance_menu->column = 0;
        entrance_menu->cursor = 0;
        if (garden_race_group() == RACE_MULTI) {
            entrance_menu->page = ENTRANCE_CARD_ROW;
        } else {
            entrance_menu->page = CHAO_AREA_NEUTRAL;
        }
        entrance_title_create(task, PICKER_TITLE_DELAY);
        entrance_caption_create(task, CAPTION_DELAY);
        entrance_chao_picker(task);
        garden_race_clear_entrants();
        entrance_menu->wait = TRUE;
        break;
    case MENU_SCAN_DRIVE:
        entrance_menu->wait = TRUE;
        if (scan_drive < CARD_DRIVES) {
            if (buIsReady(scan_drive)) {
                if (buStat(scan_drive) == 0) {
                    memset(&entrance_card_read, 0, sizeof(entrance_card_read));
                    memset(&garden_vms_racers[scan_drive], 0,
                           sizeof(struct chao_save));
                    entrance_card_read.drive = scan_drive;
                    entrance_card_read.chao =
                        syCalloc(1, sizeof(*entrance_card_read.chao));
                    entrance_card_read.egg = NULL;
                    garden_read_vms_chao(&entrance_card_read);
                    CHANGE_MODE(MENU_READ_DRIVE);
                }
            } else if (work->tick++ > DRIVE_DELAY) {
                entrance_drive_kind[scan_drive] = ENTRANCE_DRIVE_EMPTY;
                garden_vms_racers[scan_drive].record.head[0].type = CHAO_NONE;
                scan_drive++;
                work->tick = 0;
            }
        } else {
            pause_enable();
            entrance_sort_roster();
            entrance_roster_ready = TRUE;
            CHANGE_MODE(MENU_WAIT_PICKER);
        }
        break;
    case MENU_READ_DRIVE:
        /* @hack spelling: index as the read's end, where a Bool of its own
         * would do */
        index = 0;
        switch (entrance_card_read.status) {
        case GARDEN_TRANSFER_BUSY:
            break;
        case GARDEN_TRANSFER_DONE:
            njMemCopy(&garden_vms_racers[scan_drive], entrance_card_read.chao,
                      sizeof(struct chao_save));
            index = 1;
            break;
        case GARDEN_TRANSFER_CANCELLED:
        case GARDEN_TRANSFER_FAILED:
            garden_vms_racers[scan_drive].record.head[0].type = CHAO_NONE;
            index = 1;
            break;
        }
        if (index) {
            if (entrance_card_read.chao) {
                syFree(entrance_card_read.chao);
                entrance_card_read.chao = NULL;
            }
            if (buIsReady(scan_drive)) {
                entrance_drive_kind[scan_drive] = ENTRANCE_DRIVE_CARD;
                if (is_multi_bank(scan_drive)) {
                    entrance_drive_kind[scan_drive] = ENTRANCE_DRIVE_MULTI_BANK;
                }
            } else {
                entrance_drive_kind[scan_drive] = ENTRANCE_DRIVE_EMPTY;
            }
            scan_drive++;
            CHANGE_MODE(MENU_SCAN_DRIVE);
        }
        break;
    case MENU_WAIT_PICKER:
        if (!entrance_menu->wait) {
            for (index = 0; index < RACE_ENTRANTS; index++) {
                garden_race_entrants[index] = NULL;
            }
            for (choice.number = 0; choice.number < CHAO_GARDENS;
                 choice.number++) {
                for (index = 0; index < RACE_ENTRANTS; index++) {
                    entrance_menu->choices[choice.number][index].selected =
                        FALSE;
                }
            }
            CHANGE_MODE(MENU_PICKER_INPUT);
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_PICKER_INPUT:
        if (garden_race_group() == RACE_MULTI) {
            PICKER_SCROLL(MULTI_BACK_ROW);
            if (PRESSED(PDD_DGT_TA)) {
                if (entrance_menu->cursor == MULTI_BACK_ROW) {
                    BACK_TO(MENU_CLOSE_PICKER);
                } else if (entrance_menu->cursor == CONFIRM_ROW) {
                    if (garden_race_entrant_count() >= FEWEST_RACERS) {
                        /* @hack redundant: count, the choices left to place,
                         * which nothing reads */
                        count = garden_race_entrant_count();
                        /* a card's Chao races in its drive's slot, and each
                         * garden's Chao in the first slot still free */
                        for (index = 0; index < RACE_ENTRANTS; index++) {
                            if (entrance_menu->choices[ENTRANCE_CARD_ROW][index]
                                    .selected) {
                                garden_race_entrants[index] =
                                    entrance_roster[ENTRANCE_CARD_ROW][index];
                                count--;
                            }
                        }
                        slot = 0;
                        for (row = ENTRANCE_CARD_ROW + 1;
                             row < CHAO_GARDENS + 1; row++) {
                            for (index = 0; index < RACE_ENTRANTS; index++) {
                                if (entrance_menu->choices[row][index]
                                        .selected) {
                                    do {
                                        if (!garden_race_entrants[slot]) {
                                            break;
                                        }
                                        slot++;
                                    } while (slot < RACE_ENTRANTS);
                                    if (slot < RACE_ENTRANTS) {
                                        garden_race_entrants[slot] =
                                            entrance_roster[row][index];
                                    }
                                    count--;
                                }
                            }
                        }
                        (void)count;
                        REQUEST(ENTRANCE_MENU_SELECT);
                        PLAY(MENU_CONFIRM_SOUND);
                        CHANGE_MODE(MENU_CLOSE_PICKER);
                    }
                } else {
                    index = (entrance_menu->column * SLOT_ROWS) +
                            entrance_menu->cursor;
                    choice.chao = entrance_roster[entrance_menu->page][index];
                    if (choice.chao) {
                        if (entrance_menu->choices[entrance_menu->page][index]
                                .selected) {
                            entrance_menu->choices[entrance_menu->page][index]
                                .selected = FALSE;
                            garden_race_remove_entrant();
                        } else {
                            /* @hack spelling: an empty branch for a full
                             * list, where a test for room would do */
                            if (garden_race_entrant_count() >= RACE_ENTRANTS) {
                            } else {
                                entrance_menu
                                    ->choices[entrance_menu->page][index]
                                    .selected = TRUE;
                                garden_race_add_entrant();
                                if (garden_race_entrant_count() ==
                                    RACE_ENTRANTS) {
                                    entrance_menu->cursor = CONFIRM_ROW;
                                }
                            }
                        }
                    }
                    PLAY(MENU_TOGGLE_SOUND);
                }
            }
            PICKER_BACK(MULTI_BACK_ROW);
        } else {
            PICKER_SCROLL(BACK_ROW);
            if (PRESSED(PDD_DGT_TA)) {
                if (entrance_menu->cursor == BACK_ROW) {
                    BACK_TO(MENU_CLOSE_PICKER);
                } else {
                    index = (entrance_menu->column * SLOT_ROWS) +
                            entrance_menu->cursor;
                    choice.chao = entrance_roster[entrance_menu->page][index];
                    if (choice.chao) {
                        REQUEST(ENTRANCE_MENU_SELECT);
                        entrance_menu->choices[entrance_menu->page][index]
                            .selected = TRUE;
                        garden_race_entrants[0] = choice.chao;
                        CHANGE_MODE(MENU_CLOSE_PICKER);
                        PLAY(MENU_CONFIRM_SOUND);
                    }
                }
            }
            PICKER_BACK(BACK_ROW);
        }
        if (entrance_menu->cursor < SLOT_ROWS) {
            if (REPEATED(PDD_DGT_KL)) {
                if (entrance_menu->column > 0) {
                    entrance_menu->column--;
                }
                PLAY(MENU_MOVE_SOUND);
            }
            if (REPEATED(PDD_DGT_KR)) {
                if (entrance_menu->column < SLOT_COLUMNS - 1) {
                    entrance_menu->column++;
                }
                PLAY(MENU_MOVE_SOUND);
            }
        }
        entrance_menu->turn = ENTRANCE_NO_TURN;
        if (!entrance_menu->wait) {
            if (PRESSED(PDD_DGT_TL)) {
                if (entrance_menu->pages == CHAO_GARDENS + 1) {
                    if (entrance_menu->page > 0) {
                        entrance_menu->page--;
                    } else {
                        entrance_menu->page = entrance_menu->pages - 1;
                    }
                    entrance_menu->turn = ENTRANCE_TURN_PREVIOUS;
                } else if (entrance_menu->page > 0) {
                    entrance_menu->page--;
                    if (entrance_menu->page == CHAO_AREA_HERO &&
                        !entrance_menu->available[CHAO_AREA_HERO]) {
                        entrance_menu->page--;
                    }
                    entrance_menu->turn = ENTRANCE_TURN_PREVIOUS;
                }
            } else if (PRESSED(PDD_DGT_TR)) {
                if (entrance_menu->pages == CHAO_GARDENS + 1) {
                    if (entrance_menu->page < entrance_menu->pages - 1) {
                        entrance_menu->page++;
                    } else {
                        entrance_menu->page = 0;
                    }
                    entrance_menu->turn = ENTRANCE_TURN_NEXT;
                } else if (entrance_menu->page < entrance_menu->pages - 1) {
                    entrance_menu->page++;
                    if (entrance_menu->page == CHAO_AREA_HERO &&
                        !entrance_menu->available[CHAO_AREA_HERO]) {
                        entrance_menu->page++;
                    }
                    entrance_menu->turn = ENTRANCE_TURN_NEXT;
                }
            }
        }
        /* a card that came or went since the scan sends the picker out to
         * scan the drives again */
        for (index = 0; index < CARD_DRIVES; index++) {
            choice.number = 1;
            switch (entrance_drive_kind[index]) {
            case ENTRANCE_DRIVE_EMPTY:
                if (buIsReady(index)) {
                    choice.number = 0;
                }
                break;
            case ENTRANCE_DRIVE_CARD:
            case ENTRANCE_DRIVE_MULTI_BANK:
                if (!buIsReady(index)) {
                    choice.number = 0;
                }
                break;
            }
            if (!choice.number) {
                REQUEST(ENTRANCE_MENU_VMS_CHANGED);
                CHANGE_MODE(MENU_CLOSE_PICKER);
                break;
            }
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_CLOSE_PICKER:
        entrance_menu->wait = TRUE;
        CHANGE_MODE(MENU_WAIT_PICKER_CLOSE);
        /* fall through */
    case MENU_WAIT_PICKER_CLOSE:
        if (!entrance_menu->wait) {
            switch (entrance_menu->request) {
            case ENTRANCE_MENU_SELECT:
                CHANGE_MODE(MENU_BUILD_COURSES);
                break;
            case ENTRANCE_MENU_BACK:
                CHANGE_MODE(MENU_BUILD_GROUPS);
                break;
            case ENTRANCE_MENU_VMS_CHANGED:
                CHANGE_MODE(MENU_BUILD_PICKER);
                break;
            }
            entrance_menu->request = ENTRANCE_MENU_IDLE;
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_BUILD_COURSES:
        entrance_menu->screen = ENTRANCE_SELECT_COURSE;
        entrance_menu->column = 0;
        entrance_menu->cursor = 0;
        entrance_caption_create(task, CAPTION_DELAY);
        garden_race_set_level(0);
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            entrance_beginner_courses(task);
            garden_race_set_event(0);
            entrance_back_button(task, BACK_X, BACK_Y, entrance_menu->rows);
            entrance_course_preview(task);
            entrance_menu->back = entrance_menu->rows;
            break;
        case RACE_JEWEL:
            entrance_jewel_courses(task);
            garden_race_set_event(RACE_BEGINNER_COURSES);
            entrance_back_button(task, BACK_X, JEWEL_BACK_Y,
                                 entrance_menu->rows);
            entrance_course_preview(task);
            entrance_menu->back = entrance_menu->rows;
            break;
        case RACE_CHALLENGE:
            entrance_challenge_courses(task);
            garden_race_set_event(RACE_CHALLENGE_EVENT);
            if (garden_race_best_level(RACE_CHALLENGE_EVENT) <
                FIRST_CHALLENGES - 1) {
                entrance_menu->back = FIRST_CHALLENGES / GRID_COLUMNS;
                entrance_back_button(task, BACK_X, BACK_Y,
                                     FIRST_CHALLENGES / GRID_COLUMNS);
            } else {
                entrance_menu->back = RACE_CHALLENGE_LEVELS / GRID_COLUMNS;
                entrance_back_button(task, BACK_X, BACK_Y,
                                     RACE_CHALLENGE_LEVELS / GRID_COLUMNS);
            }
            entrance_course_preview(task);
            break;
        case RACE_DARK_COURSES:
            entrance_dark_courses(task);
            garden_race_set_event(RACE_DARK_EVENT);
            entrance_menu->back = RACE_ALIGNMENT_LEVELS / GRID_COLUMNS;
            entrance_back_button(task, BACK_X, BACK_Y,
                                 RACE_ALIGNMENT_LEVELS / GRID_COLUMNS);
            entrance_course_preview(task);
            break;
        case RACE_HERO_COURSES:
            garden_race_set_event(RACE_HERO_EVENT);
            entrance_hero_courses(task);
            entrance_menu->back = RACE_ALIGNMENT_LEVELS / GRID_COLUMNS;
            entrance_back_button(task, BACK_X, BACK_Y,
                                 RACE_ALIGNMENT_LEVELS / GRID_COLUMNS);
            entrance_course_preview(task);
            break;
        case RACE_MULTI:
            entrance_multiplayer_courses(task);
            if (entrance_menu->right_rows >= entrance_menu->left_rows) {
                entrance_back_button(task, BACK_X, BACK_Y,
                                     entrance_menu->right_rows);
                entrance_menu->back = entrance_menu->right_rows;
            } else {
                entrance_back_button(task, BACK_X, BACK_Y,
                                     entrance_menu->left_rows);
                entrance_menu->back = entrance_menu->left_rows;
            }
            garden_race_set_event(0);
            entrance_course_preview(task);
            break;
        }
        entrance_menu->wait = TRUE;
        CHANGE_MODE(MENU_WAIT_COURSES);
        break;
    case MENU_WAIT_COURSES:
        if (!entrance_menu->wait) {
            /* @hack spelling: a case for each group, where the groups that
             * share a mode could share a case */
            switch (garden_race_group()) {
            case RACE_BEGINNER:
                CHANGE_MODE(MENU_LEVEL_INPUT);
                break;
            case RACE_JEWEL:
                CHANGE_MODE(MENU_LEVEL_INPUT);
                break;
            case RACE_CHALLENGE:
                CHANGE_MODE(MENU_GRID_INPUT);
                break;
            case RACE_DARK_COURSES:
                CHANGE_MODE(MENU_GRID_INPUT);
                break;
            case RACE_HERO_COURSES:
                CHANGE_MODE(MENU_GRID_INPUT);
                break;
            case RACE_MULTI:
                CHANGE_MODE(MENU_MULTI_INPUT);
                break;
            }
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_MULTI_INPUT:
        if (REPEATED(PDD_DGT_KL)) {
            /* @hack spelling: a branch for each test, where one test of
             * both would do */
            if (entrance_menu->cursor < entrance_menu->left_rows) {
                entrance_menu->column = BEGINNER_COLUMN;
            } else if (entrance_menu->cursor >= entrance_menu->right_rows) {
                entrance_menu->column = BEGINNER_COLUMN;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KR)) {
            /* @hack spelling: a branch for each test, where one test of
             * both would do */
            if (entrance_menu->cursor < entrance_menu->right_rows) {
                entrance_menu->column = JEWEL_COLUMN;
            } else if (entrance_menu->cursor >= entrance_menu->left_rows) {
                entrance_menu->column = JEWEL_COLUMN;
            }
            PLAY(MENU_MOVE_SOUND);
        }
/* @hack spelling: a macro, expanded at each use, where a function would do */
#define SCROLL_COLUMN(side, other)                                             \
    if (REPEATED(PDD_DGT_KU)) {                                                \
        if (entrance_menu->cursor > 0) {                                       \
            if (entrance_menu->cursor > entrance_menu->side - 1) {             \
                entrance_menu->cursor = entrance_menu->side - 1;               \
            } else {                                                           \
                entrance_menu->cursor--;                                       \
            }                                                                  \
        }                                                                      \
        PLAY(MENU_MOVE_SOUND);                                                 \
    }                                                                          \
    if (REPEATED(PDD_DGT_KD)) {                                                \
        entrance_menu->cursor++;                                               \
        if (entrance_menu->cursor > entrance_menu->side - 1) {                 \
            if (entrance_menu->side >= entrance_menu->other) {                 \
                entrance_menu->cursor = entrance_menu->side;                   \
            } else {                                                           \
                entrance_menu->cursor = entrance_menu->other;                  \
            }                                                                  \
        }                                                                      \
        PLAY(MENU_MOVE_SOUND);                                                 \
    }
        if (entrance_menu->column == BEGINNER_COLUMN) {
            SCROLL_COLUMN(left_rows, right_rows);
        } else {
            SCROLL_COLUMN(right_rows, left_rows);
        }
#undef SCROLL_COLUMN
        switch (entrance_menu->column) {
        case BEGINNER_COLUMN:
            garden_race_set_event(entrance_menu->cursor);
            break;
        case JEWEL_COLUMN:
            garden_race_set_event(entrance_menu->cursor +
                                  RACE_BEGINNER_COURSES);
            break;
        }
        garden_race_set_level(0);
        if (PRESSED(PDD_DGT_TA)) {
            if (entrance_menu->right_rows >= entrance_menu->left_rows) {
                if (entrance_menu->cursor == entrance_menu->right_rows) {
                    BACK_TO(MENU_CLOSE_COURSES);
                } else {
                    ACCEPT_TO(MENU_CLOSE_COURSES);
                }
            } else if (entrance_menu->cursor == entrance_menu->left_rows) {
                BACK_TO(MENU_CLOSE_COURSES);
            } else {
                ACCEPT_TO(MENU_CLOSE_COURSES);
            }
        }
        if (PRESSED(PDD_DGT_TB)) {
            if (entrance_menu->right_rows >= entrance_menu->left_rows) {
                if (entrance_menu->cursor == entrance_menu->right_rows) {
                    BACK_TO(MENU_CLOSE_COURSES);
                } else {
                    entrance_menu->cursor = entrance_menu->right_rows;
                    PLAY(MENU_MOVE_SOUND);
                }
            } else if (entrance_menu->cursor == entrance_menu->left_rows) {
                BACK_TO(MENU_CLOSE_COURSES);
            } else {
                entrance_menu->cursor = entrance_menu->left_rows;
                PLAY(MENU_MOVE_SOUND);
            }
        }
        break;
    case MENU_LEVEL_INPUT:
        if (REPEATED(PDD_DGT_KL)) {
            entrance_menu->column--;
            if (entrance_menu->column < 0) {
                entrance_menu->column = 0;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KR)) {
            entrance_menu->column++;
            if (entrance_menu->column > entrance_menu->levels - 1) {
                entrance_menu->column = entrance_menu->levels - 1;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KU)) {
            entrance_menu->cursor--;
            if (entrance_menu->cursor < 0) {
                entrance_menu->cursor = entrance_menu->rows;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KD)) {
            entrance_menu->cursor++;
            if (entrance_menu->cursor > entrance_menu->rows) {
                entrance_menu->cursor = 0;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (entrance_menu->cursor != entrance_menu->back) {
            switch (garden_race_group()) {
            case RACE_BEGINNER:
                index = entrance_menu->cursor;
                break;
            case RACE_JEWEL:
                index = entrance_menu->cursor + RACE_BEGINNER_COURSES;
                break;
#ifdef NONMATCHING
            /* this side starts no race for a group the switch doesn't name. for
             * such a group, the matching side goes on with index unset */
            default:
                return;
#endif
            }
            garden_race_set_event(index);
            /* the cursor reaches a course's levels up to the first not won */
            choice.number = garden_race_best_level(index) + 1;
            if (entrance_menu->column > choice.number) {
                entrance_menu->column = choice.number;
            }
            garden_race_set_level(entrance_menu->column);
        }
/* @hack spelling: a macro, expanded at each use, where a function would do */
#define RESOLVE_COURSE(back_row)                                               \
    if (PRESSED(PDD_DGT_TA)) {                                                 \
        if (entrance_menu->cursor == (back_row)) {                             \
            BACK_TO(MENU_CLOSE_COURSES);                                       \
        } else {                                                               \
            ACCEPT_TO(MENU_CLOSE_COURSES);                                     \
            garden_race_unlock_to(garden_race_event(), garden_race_level());   \
        }                                                                      \
    }                                                                          \
    if (PRESSED(PDD_DGT_TB)) {                                                 \
        if (entrance_menu->cursor == (back_row)) {                             \
            BACK_TO(MENU_CLOSE_COURSES);                                       \
        } else {                                                               \
            entrance_menu->cursor = (back_row);                                \
            PLAY(MENU_MOVE_SOUND);                                             \
        }                                                                      \
    }
        RESOLVE_COURSE(entrance_menu->rows);
        break;
    case MENU_GRID_INPUT:
        switch (garden_race_group()) {
        case RACE_CHALLENGE:
            index = garden_race_best_level(RACE_CHALLENGE_EVENT);
            break;
        case RACE_DARK_COURSES:
            index = garden_race_best_level(RACE_DARK_EVENT);
            break;
        case RACE_HERO_COURSES:
            index = garden_race_best_level(RACE_HERO_EVENT);
            break;
#ifdef NONMATCHING
        /* this side places no cursor for a group the switch doesn't name. for
         * such a group, the matching side goes on with index unset */
        default:
            return;
#endif
        }
        /* the first level not won stands at row, in column choice.number.
         * the cursor reaches each level up to it */
        choice.number = (index + 1) % GRID_COLUMNS;
        row = (index + 1) / GRID_COLUMNS;
        if (REPEATED(PDD_DGT_KL)) {
            if (entrance_menu->cursor != entrance_menu->back) {
                entrance_menu->column--;
                if (entrance_menu->column < 0) {
                    entrance_menu->column = 0;
                }
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KR)) {
            if (entrance_menu->cursor != entrance_menu->back) {
                entrance_menu->column++;
                if (row > entrance_menu->cursor) {
                    if (entrance_menu->column > GRID_COLUMNS - 1) {
                        entrance_menu->column = GRID_COLUMNS - 1;
                    }
                } else if (entrance_menu->column > choice.number) {
                    entrance_menu->column = choice.number;
                }
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KU)) {
            if (entrance_menu->cursor == entrance_menu->back) {
                if (entrance_menu->back == row) {
                    entrance_menu->cursor = row - 1;
                } else if (entrance_menu->column <= choice.number) {
                    entrance_menu->cursor = row;
                } else {
                    entrance_menu->cursor = row - 1;
                }
            } else {
                entrance_menu->cursor--;
            }
            if (entrance_menu->cursor < 0) {
                entrance_menu->cursor = 0;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        if (REPEATED(PDD_DGT_KD)) {
            /* @hack redundant: an empty test of back against cursor */
            if (entrance_menu->back == entrance_menu->cursor) {
            }
            if (entrance_menu->back - 1 == entrance_menu->cursor) {
                entrance_menu->cursor++;
            } else if (row - 1 > entrance_menu->cursor) {
                entrance_menu->cursor++;
            } else if (row - 1 == entrance_menu->cursor) {
                if (choice.number >= entrance_menu->column) {
                    entrance_menu->cursor++;
                }
            } else if (row == entrance_menu->cursor) {
                entrance_menu->cursor = entrance_menu->back;
            }
            PLAY(MENU_MOVE_SOUND);
        }
        switch (garden_race_group()) {
        case RACE_CHALLENGE:
            garden_race_set_event(RACE_CHALLENGE_EVENT);
            break;
        case RACE_DARK_COURSES:
            garden_race_set_event(RACE_DARK_EVENT);
            break;
        case RACE_HERO_COURSES:
            garden_race_set_event(RACE_HERO_EVENT);
            break;
        }
        garden_race_set_level((entrance_menu->cursor * GRID_COLUMNS) +
                              entrance_menu->column);
        RESOLVE_COURSE(entrance_menu->back);
#undef RESOLVE_COURSE
        break;
    case MENU_CLOSE_COURSES:
        entrance_menu->wait = TRUE;
        garden_race_visit_group(garden_race_group());
        CHANGE_MODE(MENU_WAIT_COURSES_CLOSE);
        /* fall through */
    case MENU_WAIT_COURSES_CLOSE:
        if (!entrance_menu->wait) {
            switch (entrance_menu->request) {
            case ENTRANCE_MENU_SELECT:
                entrance_racers_create();
                entrance_elevator_start();
                CHANGE_MODE(MENU_BOARD_ELEVATOR);
                work->timer = 0;
                work->tick = 0;
                entrance_menu->request = ENTRANCE_MENU_LEAVE;
                entrance_message_set_group(FIRST_BOARDING_MESSAGE,
                                           BOARDING_LENGTH);
                entrance_message_append_group(SECOND_BOARDING_MESSAGE,
                                              BOARDING_LENGTH);
                break;
            case ENTRANCE_MENU_BACK:
                CHANGE_MODE(MENU_BUILD_PICKER);
                entrance_menu->request = ENTRANCE_MENU_IDLE;
                break;
            }
        }
        entrance_menu->wait = FALSE;
        break;
    case MENU_BOARD_ELEVATOR:
        if (work->timer++ > SKIP_DELAY) {
            if (PRESSED(PDD_DGT_TA)) {
                chao_change_area(CHAO_AREA_RACE);
            }
        }
        break;
    }
}

static void entrance_menu_draw(struct task *task)
{
}
static void entrance_menu_destroy(struct task *task)
{
    task_destroy_children(task);
    syFree(entrance_menu);
    entrance_menu = NULL;
}

/* set exit_row to the count of open groups up to last_group, which
 * entrance_event_buttons gives a row each. the exit button's row is the one
 * below them */
/* @hack spelling: a macro, expanded at each use, where a function would do */
#define COUNT_GROUPS(last_group)                                               \
    entrance_menu->exit_row = 0;                                               \
    for (group = 0; group <= (last_group); group++) {                          \
        /* @hack spelling: NEW before OPEN, where enum order would do */       \
        switch (garden_race_group_status(group)) {                             \
        case RACE_GROUP_HIDDEN:                                                \
            break;                                                             \
        case RACE_GROUP_NEW:                                                   \
        case RACE_GROUP_OPEN:                                                  \
        case RACE_GROUP_VISITED:                                               \
            entrance_menu->exit_row++;                                         \
            break;                                                             \
        }                                                                      \
    }
void entrance_menu_create(void)
{
    struct task *task;
    int group;
    if (entrance_menu) {
        return;
    }
    task = task_create(TASK_WORK, 2, entrance_menu_step,
                       "AL_EntranceMenuManagerExecutor");
    entrance_menu = syCalloc(1, sizeof(*entrance_menu));
    entrance_menu->task = task;
    task->draw = entrance_menu_draw;
    task->destructor = entrance_menu_destroy;
    entrance_menu->page = CHAO_AREA_NEUTRAL;
    entrance_menu->pages = 0;
    entrance_menu->available[ENTRANCE_CARD_ROW] = TRUE;
    entrance_menu->available[CHAO_AREA_NEUTRAL] = TRUE;
    if (garden_flag_test(HERO_GARDEN_FLAGS)) {
        entrance_menu->available[CHAO_AREA_HERO] = TRUE;
    }
    if (garden_flag_test(DARK_GARDEN_FLAGS)) {
        entrance_menu->available[CHAO_AREA_DARK] = TRUE;
    }
    if (garden_race_best_level(0) >= RACE_BEGINNER_LEVELS - 1 &&
        garden_race_best_level(1) >= RACE_BEGINNER_LEVELS - 1 &&
        garden_race_best_level(2) >= RACE_BEGINNER_LEVELS - 1 &&
        garden_race_best_level(3) >= RACE_BEGINNER_LEVELS - 1) {
        garden_race_unlock_group(RACE_JEWEL);
        garden_race_unlock_group(RACE_CHALLENGE);
        if (garden_flag_test(HERO_GARDEN_FLAGS) &&
            garden_flag_test(DARK_GARDEN_FLAGS)) {
            garden_race_unlock_group(RACE_DARK_COURSES);
            garden_race_unlock_group(RACE_HERO_COURSES);
        }
    }
    switch (game_state.chao_race_menu) {
    case CHAO_RACE_MENU_NONE:
        COUNT_GROUPS(RACE_MULTI);
        break;
    case CHAO_RACE_MENU_VERSUS:
        entrance_menu->exit_row = 1;
        break;
    case CHAO_RACE_MENU_EMBLEMS:
        COUNT_GROUPS(RACE_HERO_COURSES);
        break;
    }
}
