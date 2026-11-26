/* the garden editor's page of a Chao's parameters. controller three moves
 * its cursor, and with A held changes the emotion or skill under it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body_collision.h"
#include "chao/debug.h"
#include "chao/emotion.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/tuning.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/pad.h"
#include "system/task.h"

#define CELL_SIZE 12
#define TEXT_WHITE 0xffffffff
#define TEXT_LABEL 0xddff7f00
/* what one press adds or takes away: a feeling's or a trait's step, a
 * need's, and a skill's */
#define FEELING_STEP 5
#define NEED_STEP 250
#define SKILL_STEP 250

static const char *form_names[] = {
    "NONE",     "EGG",      "CHILD",   "ERROR1",  "ERROR2", "N NORMAL",
    "H NORMAL", "D NORMAL", "N SWIM",  "H SWIM",  "D SWIM", "N FLY",
    "H FLY",    "D FLY",    "N RUN",   "H RUN",   "D RUN",  "N POWER",
    "H POWER",  "D POWER",  "L CHAOS", "H CHAOS", "D CHAOS"};
static const char *garden_names[] = {"NEUTRAL", "HERO", "DARK"};
static const char *rank_names[] = {"[C]", "[B]", "[A]", "[S]"};
static int debug_column = 0;
static int debug_row = 0;

void chao_debug_gauge(int column, int row, Uint32 width, Uint32 left_color,
                      Uint32 right_color)
{
    NJS_POLYGON_VTX vertices[4];
    if (width == 0) {
        width = 1;
    }
    vertices[0].x = column * CELL_SIZE;
    vertices[0].y = (row * CELL_SIZE) + 1;
    vertices[0].z = 2.0F;
    vertices[0].col = left_color;
    vertices[1].x = column * CELL_SIZE;
    vertices[1].y = (row * CELL_SIZE) + 11;
    vertices[1].z = 2.0F;
    vertices[1].col = left_color;
    vertices[2].x = (column * CELL_SIZE) + width;
    vertices[2].y = (row * CELL_SIZE) + 1;
    vertices[2].z = 2.0F;
    vertices[2].col = right_color;
    vertices[3].x = (column * CELL_SIZE) + width;
    vertices[3].y = (row * CELL_SIZE) + 11;
    vertices[3].z = 2.0F;
    vertices[3].col = right_color;
    njDrawPolygon(vertices, 4, 0);
}

static void draw_skill(const char *name, Uint16 points, int row)
{
    njPrintColor(0xddffff55);
    njPrintC(NJM_LOCATION(35, row + 2), name);
    njPrintColor(0xddeebb00);
    row++;
    garden_draw_rect(540, (row + 2) * CELL_SIZE, 612, (row + 4) * CELL_SIZE,
                     0.5F, 0xb0222222);
    njPrintColor(0xddeebb00);
    njPrintC(NJM_LOCATION(35, row + 2), "SKL");
    njPrintColor(TEXT_WHITE);
    njPrintD(NJM_LOCATION(39, row + 2), points, 4);
    chao_debug_gauge(45, row + 2, points / 139, 0xffffff00, 0xffffbb55);
}

#define ADJUST_EMOTION(chao, step)                                             \
    {                                                                          \
        if (debug_row >= CHAO_PLEASURE && debug_row <= CHAO_TOTAL) {           \
            chao_emotion_add((chao), debug_row, (step) * FEELING_STEP);        \
        } else if (debug_row >= CHAO_SLEEPINESS && debug_row <= CHAO_THIRST) { \
            chao_emotion_add((chao), debug_row, (step) * NEED_STEP);           \
        } else if (debug_row >= CHAO_CURIOSITY &&                              \
                   debug_row <= CHAO_FICKLENESS) {                             \
            chao_emotion_add((chao), debug_row, (step) * FEELING_STEP);        \
        }                                                                      \
    }
#define SIZE_ROW(row, label, size)                                             \
    {                                                                          \
        njPrintC(NJM_LOCATION(21, (row)), (label));                            \
        njPrintD(NJM_LOCATION(27, (row)), (size), 4);                          \
    }
#define ID_ROW(row, label, value)                                              \
    {                                                                          \
        njPrintC(NJM_LOCATION(21, (row)), (label));                            \
        njPrintH(NJM_LOCATION(27, (row)), (value), 8);                         \
    }
#define PARAM_LABEL(row, label)                                                \
    {                                                                          \
        njPrintColor(TEXT_LABEL);                                              \
        njPrintC(NJM_LOCATION(21, (row)), (label));                            \
        njPrintColor(TEXT_WHITE);                                              \
    }

void chao_debug_parameters(struct task *page)
{
    struct task_work *cursor = page->work;
    struct task *chao = garden_active_object(GARDEN_OBJECT_CHAO, cursor->ang.x);
    struct chao_parameters *source = &CHAO_WORK(chao)->profile.parameters;
    /* @hack spelling: parameters copied from source, where one pointer would
     * do */
    struct chao_parameters *parameters = source;
    struct chao_gene *gene;
    int line = 0;
    int i;
    (void)rank_names;

    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (cursor->submode) {
    case 0:
        garden_enable_objects();
        cursor->submode++;
        break;
    }
    if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TB) {
        garden_disable_objects();
    }
    if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KU) {
        debug_row--;
    }
    if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KD) {
        debug_row++;
    }
    if (!(player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TA)) {
        if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KL) {
            debug_column--;
        }
        if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KR) {
            debug_column++;
        }
    }
    if (debug_column < 0) {
        debug_column = 0;
    }
    if (debug_column > 2) {
        debug_column = 2;
    }
    /* @hack redundant: a store to debug_row that never runs */
    if (0) {
        debug_row = 250;
    }
    switch (debug_column) {
    case 0:
        if (debug_row < CHAO_PLEASURE) {
            debug_row = CHAO_PLEASURE;
        }
        if (debug_row > CHAO_FICKLENESS) {
            debug_row = CHAO_FICKLENESS;
        }
        if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TA) {
            if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KL) {
                ADJUST_EMOTION(chao, -1);
            }
            if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KR) {
                ADJUST_EMOTION(chao, 1);
            }
        }
        break;
    case 1:
        if (debug_row < 0) {
            debug_row = 0;
        }
        if (debug_row > 0) {
            debug_row = 0;
        }
        break;
    case 2:
        if (debug_row < CHAO_SWIM) {
            debug_row = CHAO_SWIM;
        }
        if (debug_row > CHAO_STAMINA) {
            debug_row = CHAO_STAMINA;
        }
        if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TA) {
            if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KL) {
                if (parameters->stats[debug_row] - SKILL_STEP < 0) {
                    parameters->stats[debug_row] = 0;
                } else {
                    parameters->stats[debug_row] -= SKILL_STEP;
                }
            }
            if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KR) {
                parameters->stats[debug_row] += SKILL_STEP;
                if (parameters->stats[debug_row] > chao_tuning.skill_limit) {
                    parameters->stats[debug_row] = chao_tuning.skill_limit;
                }
            }
        }
        break;
    }

    switch (debug_column) {
    case 0:
        garden_draw_rect(12, (debug_row + 3) * CELL_SIZE, 240,
                         (debug_row + 4) * CELL_SIZE, 0.4F, 0x80ffff00);
        break;
    case 1:
        garden_draw_rect(252, (debug_row + 2) * CELL_SIZE, 480,
                         (debug_row + 3) * CELL_SIZE, 0.4F, 0x80ffff00);
        break;
    case 2:
        garden_draw_rect(420, ((debug_row * 4) + 3) * CELL_SIZE, 660,
                         ((debug_row * 4) + 4) * CELL_SIZE, 0.4F, 0x80ffff00);
        break;
    }
    garden_draw_rect(168, 36, 240, 420, 0.5F, 0xa0333333);

    /* the records' sizes in bytes. KWLG prints 648, where chao_knowledge is
     * 528 */
    SIZE_ROW(13, "PARAM", sizeof(struct chao_profile));
    SIZE_ROW(14, "EMOTION", sizeof(struct chao_emotions));
    SIZE_ROW(15, "PARTS", 12);
    SIZE_ROW(16, "KWLG", 648);
    SIZE_ROW(17, "GENE", sizeof(struct chao_gene));
    SIZE_ROW(18, "GARDEN", 824);
    njPrintH(NJM_LOCATION(27, 19), 824, 4);
    gene = &CHAO_WORK(chao)->gene;
    ID_ROW(28, "GID 1", gene->origin.console[0]);
    ID_ROW(29, "GID 2", gene->origin.console[1]);
    ID_ROW(30, "GID 3", gene->origin.timestamp);
    ID_ROW(31, "CID", gene->birth);

    PARAM_LABEL(6, "GARDEN");
    njPrintC(NJM_LOCATION(27, 6), garden_names[parameters->garden - 1]);
    PARAM_LABEL(7, "TYPE");
    njPrintC(NJM_LOCATION(27, 7), form_names[parameters->type]);
    PARAM_LABEL(8, "LIFE");
    njPrintD(NJM_LOCATION(27, 8), parameters->life, 4);
    PARAM_LABEL(9, "LMAX");
    njPrintD(NJM_LOCATION(27, 9), parameters->lifespan, 4);
    PARAM_LABEL(10, "SUCCEED");
    njPrintD(NJM_LOCATION(27, 10), parameters->reincarnations, 3);

    {
        const char *names[CHAO_FICKLENESS + 1] = {
            "PLEASURE", "ANGER",     "SORROW",      "FEAR",     "SURPRISE",
            "PAIN",     "RELAX",     "TOTAL",       "SLEEPY",   "SLPDPTH",
            "HUNGER",   "BREED",     "TEDIOUS",     "LONELY",   "TIRE",
            "STRESS",   "NOURISH",   "CONDITN",     "THIRSTY",  "CURIOSITY",
            "KINDNESS", "AGRESSIVE", "SLEEPY_HEAD", "SOLITUDE", "VITALITY",
            "GLUTTON",  "REGAIN",    "SKILLFUL",    "CHARM",    "CHATTY",
            "CALM",     "FICKLE"};
        for (i = 0; i < CHAO_FICKLENESS + 1; i++) {
            if (i >= CHAO_PLEASURE && i <= CHAO_TOTAL) {
                njPrintColor(0xdd66ff66);
                njPrintC(NJM_LOCATION(1, line + 3), names[i]);
                njPrintColor(TEXT_WHITE);
                njPrintD(NJM_LOCATION(11, line + 3), chao_emotion_get(chao, i),
                         3);
                chao_debug_gauge(14, line + 3,
                                 chao_emotion_get(chao, i) * 50 / 139,
                                 0xff55ff55, 0xff008800);
            } else if (i >= CHAO_SLEEPINESS && i <= CHAO_THIRST) {
                njPrintColor(0xddaaaaff);
                njPrintC(NJM_LOCATION(1, line + 3), names[i]);
                njPrintColor(TEXT_WHITE);
                if (chao_emotion_get(chao, i) < 10000) {
                    njPrintD(NJM_LOCATION(9, line + 3),
                             chao_emotion_get(chao, i), 4);
                } else {
                    njPrintD(NJM_LOCATION(9, line + 3),
                             chao_emotion_get(chao, i), 5);
                }
                chao_debug_gauge(14, line + 3, chao_emotion_get(chao, i) / 139,
                                 0xffaaaaff, 0xff3333ff);
            } else if (i >= CHAO_CURIOSITY && i <= CHAO_FICKLENESS) {
                njPrintColor(0xddffaa33);
                njPrintC(NJM_LOCATION(1, line + 3), names[i]);
                njPrintColor(TEXT_WHITE);
                njPrintD(NJM_LOCATION(10, line + 3), chao_emotion_get(chao, i),
                         3);
                chao_debug_gauge(17, line + 3,
                                 chao_emotion_get(chao, i) * 50 / 139,
                                 0xffffff00, 0xffff8800);
            }
            line++;
        }
    }
    {
        int row = 0;
        const char *labels[CHAO_SKILLS] = {"SWIM", "FLY",   "RUN",    "POWER",
                                           "LUCK", "INTEL", "STAMINA"};
        Uint32 skill;
        for (skill = 0; skill <= CHAO_STAMINA; skill++) {
            draw_skill(labels[skill], parameters->stats[skill], row);
            njPrintD(NJM_LOCATION(39, row + 4), parameters->best_stats[skill],
                     5);
            row += 4;
        }
    }
}
