/* chao_debug_legacy's page edits chao_legacy_preview, a Sonic Adventure
 * memory-card record, from controller 3. up and down pick a field, A edits
 * it, and B goes back to picking. while editing, up or left lowers the field,
 * and down or right raises it. on the flags, the same keys move a cursor. A
 * turns the flag at the cursor on or off */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chao/debug.h"
#include "chao/legacy.h"
#include "chao/life.h"
#include "chao/world.h"
#include "system/pad.h"
#include "system/task.h"

enum legacy_field {
    FIELD_GROUP,
    FIELD_SWIM,
    FIELD_FLY,
    FIELD_RUN,
    FIELD_POWER,
    FIELD_HEALTH,
    FIELD_RUN_POWER,
    FIELD_FLY_SWIM,
    FIELD_GROWTH,
    FIELD_FLAGS,
    FIELD_JEWEL,
    LEGACY_FIELDS
};

enum editor_mode { EDITOR_INIT, EDITOR_SELECT, EDITOR_EDIT };
/* the finish flags' rows. the cursor moves through all but FLAG_DOWNLOAD */
enum finish_flag {
    FLAG_BLACK,
    FLAG_GOLD,
    FLAG_SILVER,
    FLAG_JEWEL,
    FLAG_DOWNLOAD
};
/* the values a new record starts with */
#define START_AFFECTION 75
#define START_HEALTH 1500
#define START_LIFE 180
#define START_ID 99
#define RECORD_PARTS 7
/* the page cycles group from 0 to LAST_GROUP, EGG, and jewel from 0 to
 * LAST_JEWEL, SAPPHIRE */
#define LAST_GROUP 7
#define LAST_JEWEL 3
#define SKILL_LIMIT 9999
#define HEALTH_LIMIT 5000
#define POINT_STEP 500
#define LABEL_COLUMN 20
#define VALUE_COLUMN 27
#define FIRST_LINE 10

static int finish_cursor = 0;
static int field_cursor = 0;

/* @hack redundant: a pad pointer for each direction of each field, where one
 * local would do */
#define DIRECTION_PADS(field)                                                  \
    const PDS_PERIPHERAL *field##_decrease;                                    \
    const PDS_PERIPHERAL *field##_increase
#define DECREASE(pad)                                                          \
    (((pad) = player_pads[CHAO_DEBUG_PAD])->press & PDD_DGT_KU ||              \
     (pad)->press & PDD_DGT_KL)
#define INCREASE(pad)                                                          \
    (((pad) = player_pads[CHAO_DEBUG_PAD])->press & PDD_DGT_KD ||              \
     (pad)->press & PDD_DGT_KR)
/* @hack redundant: two reads of the pad's press that nothing uses */
#define INCREASE_RUN(pad)                                                      \
    (((pad) = player_pads[CHAO_DEBUG_PAD])->press & PDD_DGT_KD ||              \
     ((void)(pad)->press, (void)(pad)->press, (pad)->press & PDD_DGT_KR))

/* the last multiple of POINT_STEP below limit */
#define LAST_STEP(limit) (((limit) - 1) / POINT_STEP * POINT_STEP)
/* round value down to a multiple of POINT_STEP, then step it down or up by
 * POINT_STEP. a step down stops at 0. a step up comes only from LAST_STEP or
 * below, and stops at limit */
#define EDIT_POINTS(field, value, limit, increase)                             \
    do {                                                                       \
        if (DECREASE(field##_decrease)) {                                      \
            (value) /= POINT_STEP;                                             \
            (value) *= POINT_STEP;                                             \
            if ((value) >= POINT_STEP) {                                       \
                (value) -= POINT_STEP;                                         \
            }                                                                  \
        } else if (increase) {                                                 \
            (value) /= POINT_STEP;                                             \
            (value) *= POINT_STEP;                                             \
            if ((value) <= LAST_STEP(limit)) {                                 \
                (value) += POINT_STEP;                                         \
                if ((value) > (limit)) {                                       \
                    (value) = (limit);                                         \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

#define EDIT_SCALE(field, minimum, maximum)                                    \
    do {                                                                       \
        if (DECREASE(field##_decrease)) {                                      \
            record->field -= 0.1F;                                             \
            if (record->field < (minimum)) {                                   \
                record->field = (minimum);                                     \
            }                                                                  \
        } else if (INCREASE(field##_increase)) {                               \
            record->field += 0.1F;                                             \
            if (record->field > (maximum)) {                                   \
                record->field = (maximum);                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

#define TOGGLE_FINISH(bit)                                                     \
    do {                                                                       \
        if (record->finish_flags & (bit)) {                                    \
            record->finish_flags &= ~(bit);                                    \
        } else {                                                               \
            record->finish_flags |= (bit);                                     \
        }                                                                      \
    } while (0)

#define PRINT_FINISH(flag_row, bit, label)                                     \
    do {                                                                       \
        if (flags & (bit)) {                                                   \
            njPrintC(NJM_LOCATION(VALUE_COLUMN - 1,                            \
                                  row + (FIRST_LINE + (flag_row))),            \
                     "*");                                                     \
        }                                                                      \
        njPrintC(NJM_LOCATION(VALUE_COLUMN, row + (FIRST_LINE + (flag_row))),  \
                 (label));                                                     \
    } while (0)
#define PRINT_VALUE(value)                                                     \
    njPrintD(NJM_LOCATION(VALUE_COLUMN, row + FIRST_LINE), (value), 4)

void chao_debug_legacy(struct task *page)
{
    struct task_work *work = page->work;
    struct task *resident =
        garden_active_object(GARDEN_OBJECT_CHAO, work->ang.x);
    /* @hack redundant: the selected Chao's work, which nothing reads */
    struct task_work *resident_work = resident ? resident->work : NULL;
    struct legacy_chao *record = &chao_legacy_preview;
    DIRECTION_PADS(group);
    DIRECTION_PADS(swim);
    DIRECTION_PADS(fly);
    DIRECTION_PADS(run);
    DIRECTION_PADS(power);
    DIRECTION_PADS(health);
    DIRECTION_PADS(run_power);
    DIRECTION_PADS(fly_swim);
    DIRECTION_PADS(evolution);
    DIRECTION_PADS(finish);
    DIRECTION_PADS(jewel);
    int field;
    int row;
    int flags;
    Sint8 *part;
    const char *labels[20] = {"GROUP", NULL,     "SWIM",   "FLY",  "RUN",
                              "POWER", NULL,     "HEALTH", NULL,   "HPOS",
                              "VPOS",  "GROWTH", NULL,     "FLAG", NULL,
                              NULL,    NULL,     NULL,     NULL,   "JEWEL"};

    switch (work->submode) {
    case EDITOR_INIT:
        memset(record, 0, sizeof(*record));
        record->group = 0;
        record->affection = START_AFFECTION;
        record->health = START_HEALTH;
        record->max_health = record->health;
        record->life = START_LIFE;
        record->finish_flags |= LEGACY_DOWNLOAD;
        record->id = START_ID;
        record->unk_17a = 1;
        record->name[0] = 'C';
        record->name[1] = 'H';
        record->name[2] = 'A';
        record->name[3] = 'O';
        record->mark_c = 'C';
        record->mark_h = 'H';
        record->mark_a = 'A';
        record->mark_o = 'O';
        work->submode = EDITOR_SELECT;
        part = record->parts;
        /* @hack spelling: part = part + 1, where part++ would do */
        for (field = 0; field < RECORD_PARTS; field++, part = part + 1) {
            *part = -1;
        }
        break;
    case EDITOR_SELECT:
        /* @hack redundant: a test of B with an empty body */
        if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TB) {
        }
        if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TA) {
            work->submode = EDITOR_EDIT;
        }
        if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_KU) {
            if (--field_cursor < FIELD_GROUP) {
                field_cursor = FIELD_JEWEL;
            }
        } else if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_KD) {
            if (++field_cursor > FIELD_JEWEL) {
                field_cursor = FIELD_GROUP;
            }
        }
        break;
    case EDITOR_EDIT:
        if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TB) {
            work->submode = EDITOR_SELECT;
        }
        switch (field_cursor) {
        case FIELD_GROUP:
            if (DECREASE(group_decrease)) {
                if (record->group == 0) {
                    record->group = LAST_GROUP;
                } else {
                    record->group--;
                }
            } else if (INCREASE(group_increase)) {
                if (record->group == LAST_GROUP) {
                    record->group = 0;
                } else {
                    record->group++;
                }
            }
            break;
        case FIELD_SWIM:
            EDIT_POINTS(swim, record->skill[CHAO_SWIM], SKILL_LIMIT,
                        INCREASE(swim_increase));
            record->swim_points = record->skill[CHAO_SWIM];
            break;
        case FIELD_FLY:
            EDIT_POINTS(fly, record->skill[CHAO_FLY], SKILL_LIMIT,
                        INCREASE(fly_increase));
            record->fly_points = record->skill[CHAO_FLY];
            break;
        case FIELD_RUN:
            EDIT_POINTS(run, record->skill[CHAO_RUN], SKILL_LIMIT,
                        INCREASE_RUN(run_increase));
            record->run_points = record->skill[CHAO_RUN];
            break;
        case FIELD_POWER:
            EDIT_POINTS(power, record->skill[CHAO_POWER], SKILL_LIMIT,
                        INCREASE(power_increase));
            record->power_points = record->skill[CHAO_POWER];
            break;
        case FIELD_HEALTH:
            EDIT_POINTS(health, record->health, HEALTH_LIMIT,
                        INCREASE(health_increase));
            record->max_health = record->health;
            break;
        case FIELD_RUN_POWER:
            EDIT_SCALE(run_power, -1.5F, 1.5F);
            break;
        case FIELD_FLY_SWIM:
            EDIT_SCALE(fly_swim, -1.5F, 1.5F);
            break;
        case FIELD_GROWTH:
            EDIT_SCALE(evolution, 0.0F, 1.0F);
            break;
        case FIELD_FLAGS:
            if (DECREASE(finish_decrease)) {
                if (finish_cursor == FLAG_BLACK) {
                    finish_cursor = FLAG_JEWEL;
                } else {
                    finish_cursor--;
                }
            } else if (INCREASE(finish_increase)) {
                if (finish_cursor >= FLAG_JEWEL) {
                    finish_cursor = FLAG_BLACK;
                } else {
                    finish_cursor++;
                }
            }
            if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TA) {
                switch (finish_cursor) {
                case FLAG_BLACK:
                    TOGGLE_FINISH(LEGACY_BLACK);
                    break;
                case FLAG_GOLD:
                    TOGGLE_FINISH(LEGACY_GOLD);
                    break;
                case FLAG_SILVER:
                    TOGGLE_FINISH(LEGACY_SILVER);
                    break;
                case FLAG_JEWEL:
                    TOGGLE_FINISH(LEGACY_JEWEL);
                    break;
                }
            }
            break;
        case FIELD_JEWEL:
            if (DECREASE(jewel_decrease)) {
                if (record->jewel == 0) {
                    record->jewel = LAST_JEWEL;
                } else {
                    record->jewel--;
                }
            } else if (INCREASE(jewel_increase)) {
                if (record->jewel >= LAST_JEWEL) {
                    record->jewel = 0;
                } else {
                    record->jewel++;
                }
            }
            break;
        }
        break;
    }

    row = 0;
    {
        const char *groups[] = {"CHILD",   "STANDARD", "SWIM",  "FLY",
                                "RUN",     "POWER",    "CHAOS", "EGG",
                                "NOTHING", "PARTNER",  "MAYU"};
        const char *jewels[] = {"AMETHYST", "EMERALD", "RUBY", "SAPPHIRE"};

        /* @hack spelling: row++ on both paths, where one row++ in the loop's
         * step would do */
        for (field = 0; field < LEGACY_FIELDS;) {
            if (labels[row] == NULL) {
                row++;
                continue;
            }
            if (field != field_cursor) {
                njPrintColor(0xffafafaf);
            } else {
                switch (work->submode) {
                case EDITOR_SELECT:
                    njPrintColor(0xffffffff);
                    break;
                case EDITOR_EDIT:
                    njPrintColor(0xffff7f00);
                    break;
                }
            }
            njPrintC(NJM_LOCATION(LABEL_COLUMN, row + FIRST_LINE), labels[row]);
            switch (field) {
            case FIELD_GROUP:
                njPrintC(NJM_LOCATION(VALUE_COLUMN, row + FIRST_LINE),
                         groups[record->group]);
                break;
            case FIELD_SWIM:
                PRINT_VALUE(record->skill[CHAO_SWIM]);
                break;
            case FIELD_FLY:
                PRINT_VALUE(record->skill[CHAO_FLY]);
                break;
            case FIELD_RUN:
                PRINT_VALUE(record->skill[CHAO_RUN]);
                break;
            case FIELD_POWER:
                PRINT_VALUE(record->skill[CHAO_POWER]);
                break;
            case FIELD_HEALTH:
                PRINT_VALUE(record->health);
                break;
            case FIELD_RUN_POWER:
                PRINT_VALUE(record->run_power * 1000.0F);
                break;
            case FIELD_FLY_SWIM:
                PRINT_VALUE(record->fly_swim * 1000.0F);
                break;
            case FIELD_GROWTH:
                PRINT_VALUE(record->evolution * 1000.0F);
                break;
            case FIELD_FLAGS:
                if (field == field_cursor) {
                    njPrintC(NJM_LOCATION(VALUE_COLUMN - 2,
                                          finish_cursor + row + FIRST_LINE),
                             ">");
                }
                flags = record->finish_flags;
                PRINT_FINISH(FLAG_BLACK, LEGACY_BLACK, "BLACK");
                PRINT_FINISH(FLAG_GOLD, LEGACY_GOLD, "GOLD");
                PRINT_FINISH(FLAG_SILVER, LEGACY_SILVER, "SILVER");
                PRINT_FINISH(FLAG_JEWEL, LEGACY_JEWEL, "JEWEL");
                PRINT_FINISH(FLAG_DOWNLOAD, LEGACY_DOWNLOAD, "DOWNLOAD");
                break;
            case FIELD_JEWEL:
                njPrintC(NJM_LOCATION(VALUE_COLUMN, row + FIRST_LINE),
                         jewels[record->jewel]);
                break;
            }
            field++;
            row++;
        }
    }
}
