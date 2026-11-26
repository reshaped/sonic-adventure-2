#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#include "chao/diagnosis.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/residents.h"
#include "chao/work.h"
#include "chaostgkinder/medicine_models.h"
#include "chaostgkinder/checkup_text.h"
#include "chaostgkinder/messages.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/service_menu.h"
#include "chaostgkinder/services.h"
#include "draw/lines.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"

struct checkup {
    Sint8 state, step, cursor, selected;
    Uint16 timer, egg;
    Sint32 message;
    struct task *task, *medicine, *patient;
};

enum checkup_step {
    CHECKUP_OPEN = 0,
    CHECKUP_GREET = 2,
    CHECKUP_CHOOSE = 3,
    CHECKUP_EXAMINE = 5,
    CHECKUP_SHOW_RESULT = 6,
    CHECKUP_NEXT_PAGE = 7,
    CHECKUP_COUNT_LIVES = 9,
    CHECKUP_WAIT_MEDICINE = 10,
    CHECKUP_CLOSING = 11,
    CHECKUP_DONE = 12
};

/* entries of checkup_messages. ILLNESS_GROUP's replies start at
 * ILLNESS_RESULT, ILLNESS_ENTRIES entries each, and reply n shows the
 * medicine medicine_models[n]. PERSONALITY_PAGE and STAMINA_PAGE each open a
 * reply of two pages */
enum checkup_message {
    ILLNESS_RESULT = 6,
    EGG_RESULT = 18,
    PERSONALITY_PAGE = 90,
    STAMINA_PAGE = 151,
    LIVES_RESULT = 170
};
#define ILLNESS_ENTRIES 2
#define LIVES_DIGITS 5

/* the messages of KINDER_WHOLE_BANK the checkup shows */
enum checkup_prompt {
    EXIT_MESSAGE = 0,
    GREETING_MESSAGE = 22,
    EXAMINE_MESSAGE = 23,
    FAREWELL_MESSAGE = 25,
    NO_PATIENT_MESSAGE = 28
};

enum checkup_choice { CHOICE_EXAMINE, CHOICE_EXIT };

/* the frames each wait lasts. EXAMINE_FRAMES waits for the patient, and
 * PAUSE_FRAMES before the checkup closes */
#define GREETING_FRAMES 120
#define PAUSE_FRAMES 60
#define STEP_FRAMES 5
#define EXAMINE_FRAMES 40
#define RESULT_FRAMES 30
#define PAGE_FRAMES 40
#define MEDICINE_FRAMES 180
/* where the patient stands on the table, and its yaw */
#define PATIENT_X 0.343882F
#define PATIENT_Y 3.284631F
#define PATIENT_Z (-2.164542F)
#define PATIENT_YAW (-4551)
/* the chance that the checkup tries a comment on the patient before a
 * rating. RANDOM_CHOICE picks a comment's kind from COMMENT_KINDS by
 * COMMENT_SCALE, and a rating's from RATING_KINDS by RATING_SCALE */
#define COMMENT_CHANCE 0.8F
#define COMMENT_SCALE 100.0F
#define COMMENT_KINDS 10
#define RATING_SCALE 64.0F
#define RATING_KINDS 8
/* a skill's rating, and a character's dislike, run from 0 to TOP_RATING */
#define TOP_RATING 4
/* @hack spelling: rating > TOP_RATING - 1, where rating >= TOP_RATING would
 * do */
#define IS_TOP(rating) ((rating) > TOP_RATING - 1)

enum medicine_mode {
    MEDICINE_OPEN = 0,
    MEDICINE_UNFOLD = 1,
    MEDICINE_SHOW = 2,
    MEDICINE_DONE = 4
};
enum medicine_flags { MEDICINE_PANEL = 0x100, MEDICINE_MODEL = 0x200 };
/* the medicine's panel unfolds from UNFOLD_WIDTH, in steps of UNFOLD_STEP_X
 * and UNFOLD_STEP_Y. its model turns by SPIN_X and SPIN_Y a step, and
 * stands MODEL_SCALE as big at MODEL_Y and MODEL_Z. models 0 to
 * LAST_UPRIGHT_MODEL stand tilted by MODEL_TILT, and the rest lie at
 * MODEL_LIE */
#define UNFOLD_WIDTH 0.3F
#define UNFOLD_STEP_X 0.08F
#define UNFOLD_STEP_Y 0.04F
#define SPIN_X 0x100
#define SPIN_Y 0x200
#define LAST_UPRIGHT_MODEL 1
#define MODEL_TILT (-0x1000)
#define MODEL_LIE (-0x6000)
#define MODEL_Y 6.65F
#define MODEL_Z 7.5F
#define MODEL_SCALE 0.8F
#define PANEL_COLOR 0xff000000
#define PANEL_DEPTH (-9.0F)
/* an English panel and its label are WIDE_TEXT times the width of
 * Japanese ones */
#define WIDE_TEXT 1.5F

static const struct kinder_message_label examine_label = {
#include "chaostgkinder/checkup/examine_label.inc"
};
static const struct kinder_message_label leave_label = {
#include "chaostgkinder/checkup/leave_label.inc"
};
static const char exit_text[8] = {
#include "chaostgkinder/checkup/exit_text.inc"
};
static const char medicine_archive[24] = {
#include "chaostgkinder/checkup/medicine_archive.inc"
};
static const struct service_box medicine_frame = {
#include "chaostgkinder/checkup/medicine_frame.inc"
};
static const struct kinder_message_label medicine_label = {
#include "chaostgkinder/checkup/medicine_label.inc"
};
static const struct service_box medicine_backdrop = {
#include "chaostgkinder/checkup/medicine_backdrop.inc"
};
static const char first_texture[12] = {
#include "chaostgkinder/checkup/first_texture.inc"
};
static const char second_texture[12] = {
#include "chaostgkinder/checkup/second_texture.inc"
};
static const char third_texture[12] = {
#include "chaostgkinder/checkup/third_texture.inc"
};

static NJS_TEXNAME texture_names[3] = {
#include "chaostgkinder/checkup/texture_names.inc"
};
static NJS_TEXLIST medicine_textures = {
#include "chaostgkinder/checkup/medicine_textures.inc"
};
static struct checkup *clinic = NULL;
static struct service_decoration menu_frame = {
#include "chaostgkinder/checkup/menu_frame.inc"
};
static struct service_box menu_box = {
#include "chaostgkinder/checkup/menu_box.inc"
};
static struct service_box first_button = {
#include "chaostgkinder/checkup/first_button.inc"
};
static struct service_choice button_colors = {
#include "chaostgkinder/checkup/button_colors.inc"
};
static NJS_CNK_OBJECT *medicine_models[6] = {
#include "chaostgkinder/checkup/medicine_models.inc"
};
static Sint32 illness_messages[6] = {
#include "chaostgkinder/checkup/illness_messages.inc"
};

static struct task *medicine_create(struct task *parent, Uint8 model);
static struct chao_parameters *patient_parameters(struct task *chao);

static struct chao_parameters *patient_parameters(struct task *chao)
{
    return &CHAO_WORK(chao)->profile.parameters;
}

#define FACE_BUTTONS (PDD_DGT_TA | PDD_DGT_TB | PDD_DGT_TX | PDD_DGT_TY)
/* rand() * RANDOM_SCALE lies in [0, 1) */
#define RANDOM_SCALE (1.0F / (RAND_MAX + 1.0F))
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define RANDOM_CHOICE(scale, count)                                            \
    ((unsigned)(rand() * ((scale) * RANDOM_SCALE)) % (count))
#else
/* @hack type: the int conversion, where an unsigned one would do: the value
 * is never negative */
#define RANDOM_CHOICE(scale, count)                                            \
    ((int)(rand() * ((scale) * RANDOM_SCALE)) % (count))
#endif

static void checkup_step(struct task *task)
{
    char *message;
    struct task *carried;
    struct task *patient;
    struct task *display;
    const char *text;
    char *number;
    char *digit;
    int variant;
    /* @hack spelling: group, which also holds each random pick, a rebirth
     * count, a character and an entry, where a local of each would do */
    int group;
    /* checkup_messages' first group holds the openings, the lines shown
     * before a result. opening is one of them */
    int opening;
    int value;
    int digits;
    int quotient;
    int place;
    int i;
    int remaining;
    Float chance;

    if (game_state.time_stopped) {
        return;
    }
    /* @hack spelling: carried, set from kinder_carried_chao in the switch's
     * operand, where a statement before the switch would do */
    switch (carried = kinder_carried_chao, clinic->step) {
    case CHECKUP_OPEN: {
        struct service_box box;
        struct service_choice choice;
        struct kinder_message_label label;
        service_panel_create(task, &menu_box, &menu_frame);
        box = first_button;
        choice = button_colors;
        clinic->cursor = clinic->selected = -1;
        choice.selection = &clinic->selected;
        service_button_create(task, CHOICE_EXAMINE, &box, &choice);
        label = examine_label;
        label.selection = (const Uint8 *)&clinic->selected;
        label.pos.x = box.pos.x + (box.width * 0.5F);
        label.pos.y = box.pos.y + (box.height * 0.5F);
        label.pos.z = box.pos.z + SERVICE_LABEL_DEPTH;
        label.width = box.width - SERVICE_LABEL_INSET;
        kinder_message_label_create(task, &label);
        box.pos.y += box.height + SERVICE_ROW_GAP;
        service_button_create(task, CHOICE_EXIT, &box, &choice);
        label = leave_label;
        label.selection = (const Uint8 *)&clinic->selected;
        label.pos.x = box.pos.x + (box.width * 0.5F);
        label.pos.y = box.pos.y + (box.height * 0.5F);
        label.pos.z = box.pos.z + SERVICE_LABEL_DEPTH;
        label.text = exit_text;
        kinder_message_label_create(task, &label);
        /* @bug logic: the greeting never times out. CHECKUP_GREET tests the
         * timer for 0 at 0x0c9cb846, but no step counts down the
         * GREETING_FRAMES set here. advice_step counts its own down */
        clinic->timer = GREETING_FRAMES;
        kinder_dialog_create(task, 0, 0);
        kinder_dialog_set(GREETING_MESSAGE, KINDER_WHOLE_BANK);
        kinder_dialog_show_arrow();
    }
        clinic->step = CHECKUP_GREET;
        break;
    case CHECKUP_GREET:
        if ((pad_press_repeat[0] & FACE_BUTTONS) || clinic->timer == 0) {
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            clinic->cursor = clinic->selected = CHOICE_EXAMINE;
            kinder_dialog_set(EXAMINE_MESSAGE, KINDER_WHOLE_BANK);
            kinder_dialog_hide_arrow();
            clinic->timer = PAUSE_FRAMES;
            clinic->step = CHECKUP_CHOOSE;
        }
        break;
    case CHECKUP_CHOOSE:
        if (clinic->cursor == CHOICE_EXAMINE &&
            (pad_press_repeat[0] & PDD_DGT_KD)) {
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            clinic->cursor++;
            clinic->selected = clinic->cursor;
            /* @hack redundant: a goto to the next statement */
            goto moved;
        moved:
            kinder_dialog_set(EXIT_MESSAGE, KINDER_WHOLE_BANK);
        } else if (clinic->cursor == CHOICE_EXIT &&
                   (pad_press_repeat[0] & PDD_DGT_KU)) {
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            clinic->cursor--;
            clinic->selected = clinic->cursor;
            kinder_dialog_set(EXAMINE_MESSAGE, KINDER_WHOLE_BANK);
        } else if (pad_press_repeat[0] & PDD_DGT_TB) {
            if (clinic->selected == CHOICE_EXAMINE) {
                clinic->cursor++;
                clinic->selected = clinic->cursor;
                clinic->timer = STEP_FRAMES;
                kinder_dialog_set(EXIT_MESSAGE, KINDER_WHOLE_BANK);
            } else {
                sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
                clinic->timer = PAUSE_FRAMES;
                clinic->step = CHECKUP_CLOSING;
                /* @hack redundant: a goto to the end of its arm */
                goto cancelled;
            cancelled:;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_TA) {
            if (clinic->selected == CHOICE_EXAMINE) {
#ifdef NONMATCHING
                /* the patient's work keeps a pointer to save past this
                 * step, so this side keeps save static. one checkup
                 * runs at a time */
                static struct chao_save save;
#else
                struct chao_save save;
#endif
                NJS_POINT3 position;
                if (!carried) {
                    kinder_dialog_set(NO_PATIENT_MESSAGE, KINDER_WHOLE_BANK);
                    clinic->timer = PAUSE_FRAMES;
                    sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
                    clinic->step = CHECKUP_CLOSING;
                    return;
                }
                if (patient_parameters(carried)->type == CHAO_EGG) {
                    garden_pack_egg(carried, &save);
                    clinic->egg = 1;
                } else {
                    garden_pack_chao(carried, &save);
                    clinic->egg = 0;
                }
                position.x = PATIENT_X;
                position.y = PATIENT_Y;
                position.z = PATIENT_Z;
                display = garden_spawn_chao(&save, &position, PATIENT_YAW);
                if (!display) {
                    kinder_dialog_set(NO_PATIENT_MESSAGE, KINDER_WHOLE_BANK);
                    clinic->timer = PAUSE_FRAMES;
                    clinic->step = CHECKUP_CLOSING;
                    return;
                }
                CHAO_WORK(display)->flags &= ~CHAO_COLLIDES;
                CHAO_WORK(display)->flags &= ~CHAO_MOVES;
                CHAO_WORK(display)->flags &= ~CHAO_RUNS_BEHAVIOR;
                CHAO_WORK(display)->flags &= ~CHAO_BODY_SPRINGS;
                CHAO_WORK(display)->flags |= CHAO_NO_INTENTIONS;
                CHAO_WORK(display)->flags &= ~CHAO_AGES;
                if (clinic->egg == 0) {
                    chao_link_motion_frames(display, 47, 1);
                }
                clinic->patient = display;
                sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                clinic->timer = EXAMINE_FRAMES;
                clinic->step = CHECKUP_EXAMINE;
                return;
            } else {
                kinder_dialog_set(FAREWELL_MESSAGE, KINDER_WHOLE_BANK);
                clinic->timer = PAUSE_FRAMES;
                clinic->step = CHECKUP_CLOSING;
            }
        } else {
            /* @hack redundant: an empty else, for a frame with no press */
        }
        break;
    case CHECKUP_EXAMINE:
        if (clinic->timer > 0) {
            clinic->timer--;
            break;
        }
        if (clinic->egg == 1) {
            clinic->message = EGG_RESULT;
            kinder_dialog_set(5, KINDER_SERVICE_BANK);
            clinic->step = CHECKUP_SHOW_RESULT;
            clinic->timer = RESULT_FRAMES;
            return;
        }
        patient = clinic->patient;
        variant = chao_treat_illness(carried);
        clinic->timer = RESULT_FRAMES;
        if (variant >= 0) {
            variant = illness_messages[variant];
            clinic->message = checkup_message_variant(ILLNESS_GROUP, variant);
            kinder_dialog_set(0, KINDER_SERVICE_BANK);
            clinic->step = CHECKUP_SHOW_RESULT;
            return;
        }
        chance = rand() * RANDOM_SCALE;
        if (chance < COMMENT_CHANCE) {
            /* @hack spelling: the block's own comment_opening, where the
             * function's opening would do */
            int comment_opening;
            group = RANDOM_CHOICE(COMMENT_SCALE, COMMENT_KINDS);
            switch (group) {
            case 0:
                variant = chao_headgear_comment(patient);
                group = HEADGEAR_GROUP;
                comment_opening = 1;
                break;
            case 1:
            case 7:
                variant = chao_form_comment(patient);
                group = FORM_GROUP;
                comment_opening = 4;
                break;
            case 2:
                variant = chao_affection_comment(patient);
                group = AFFECTION_GROUP;
                comment_opening = 2;
                break;
            case 3:
            case 8:
                variant = chao_personality_comment(patient);
                group = PERSONALITY_GROUP;
                comment_opening = 2;
                break;
            case 4:
            case 9:
                variant = chao_condition_comment(patient);
                group = CONDITION_GROUP;
                if (variant == 8 || variant == 6) {
                    comment_opening = 4;
                } else {
                    comment_opening = 0;
                }
                break;
            case 5:
                chao_taste_comment(patient);
                comment_opening = 4;
                group = TASTE_GROUP;
                variant = -1;
                break;
            case 6:
                group = chao_reincarnation_count(patient);
                if (!group) {
                    variant = -1;
                    break;
                }
                chao_reincarnation_comment(patient);
                clinic->message = LIVES_RESULT;
                kinder_dialog_set(5, KINDER_SERVICE_BANK);
                clinic->step = CHECKUP_SHOW_RESULT;
                return;
            }
            if (variant >= 0) {
                clinic->message = checkup_message_variant(group, variant);
                kinder_dialog_set(comment_opening, KINDER_SERVICE_BANK);
                clinic->step = CHECKUP_SHOW_RESULT;
                return;
            }
        }
        group = RANDOM_CHOICE(RATING_SCALE, RATING_KINDS);
        switch (group) {
        case 0:
            variant = chao_swim_rating(patient);
            group = SWIM_GROUP;
            if (IS_TOP(variant)) {
                opening = 1;
            } else {
                opening = 5;
            }
            break;
        case 1:
            variant = chao_fly_rating(patient);
            group = FLY_GROUP;
            if (IS_TOP(variant)) {
                opening = 1;
            } else {
                opening = 5;
            }
            break;
        case 2:
            variant = chao_run_rating(patient);
            group = RUN_GROUP;
            if (IS_TOP(variant)) {
                opening = 1;
            } else {
                opening = 5;
            }
            break;
        case 4:
            variant = chao_power_rating(patient);
            group = POWER_GROUP;
            if (IS_TOP(variant)) {
                opening = 1;
            } else {
                opening = 5;
            }
            break;
        case 5:
            variant = chao_stamina_rating(patient);
            group = STAMINA_GROUP;
            if (IS_TOP(variant)) {
                opening = 1;
            } else {
                opening = 5;
            }
            break;
        case 6:
            variant = chao_age_comment(patient);
            group = AGE_GROUP;
            opening = 5;
            break;
        case 3:
        case 7:
            group = stage_character(0);
            switch (group) {
            case PLAYER_SONIC:
                variant = chao_sonic_opinion(patient);
                group = SONIC_GROUP;
                break;
            case PLAYER_SHADOW:
                variant = chao_shadow_opinion(patient);
                group = SHADOW_GROUP;
                break;
            case PLAYER_TAILS:
                variant = chao_tails_opinion(patient);
                group = TAILS_GROUP;
                break;
            case PLAYER_EGGMAN:
                variant = chao_eggman_opinion(patient);
                group = EGGMAN_GROUP;
                break;
            case PLAYER_KNUCKLES:
                variant = chao_knuckles_opinion(patient);
                group = KNUCKLES_GROUP;
                break;
            case PLAYER_ROUGE:
                variant = chao_rouge_opinion(patient);
                group = ROUGE_GROUP;
                break;
            default:
                variant = chao_sonic_opinion(patient);
                group = SONIC_GROUP;
                break;
            }
            if (IS_TOP(variant)) {
                opening = 3;
            } else {
                opening = 2;
            }
            break;
        }
        clinic->message = checkup_message_variant(group, variant);
        kinder_dialog_set(opening, KINDER_SERVICE_BANK);
        clinic->timer = RESULT_FRAMES;
        clinic->step = CHECKUP_SHOW_RESULT;
        return;
    case CHECKUP_SHOW_RESULT:
        if (clinic->timer > 0) {
            clinic->timer--;
            break;
        }
        if (!(pad_press_repeat[0] & PDD_DGT_TA)) {
            break;
        }
        sound_play(SERVICE_RESULT_SOUND, NULL, 0, 0);
        if (clinic->message != LIVES_RESULT) {
            kinder_dialog_set(clinic->message, KINDER_SERVICE_BANK);
            if (clinic->message == STAMINA_PAGE ||
                clinic->message == PERSONALITY_PAGE) {
                kinder_dialog_show_arrow();
            }
        } else {
            carried = clinic->patient;
            text = kinder_message_text(LIVES_RESULT, KINDER_SERVICE_BANK);
            value = chao_reincarnation_count(carried);
            digits = LIVES_DIGITS;
            do {
                place = 1;
                for (i = digits - 1; i > 0; i--) {
                    place *= 10;
                }
                quotient = value / place;
                digits--;
            } while (quotient == 0 && digits != 0);
            if (quotient == 0) {
                number = syCalloc(1, 2);
#ifdef NONMATCHING
                /* this side shows no number where the heap has no room for it.
                 * the matching side writes through the null */
                if (!number) {
                    break;
                }
#endif
                number[0] = '0';
                number[1] = '\0';
            } else {
                digits++;
                number = syCalloc(1, digits + 1);
#ifdef NONMATCHING
                /* this side shows no number where the heap has no room for it.
                 * the matching side writes through the null */
                if (!number) {
                    break;
                }
#endif
                digit = number;
                do {
                    place = 1;
                    /* @hack spelling: remaining, stepped by remaining =
                     * remaining - 1, where i and i-- would do as above */
                    for (remaining = digits - 1; remaining > 0;
                         remaining = remaining - 1) {
                        place *= 10;
                    }
                    quotient = value / place;
#ifdef NONMATCHING
                    /* this side repairs the bug tagged in the matching side:
                     * the remainder divides by place */
                    value %= place;
#else
                    /* @bug arithmetic: the remainder at 0x0c9cbf90 divides by
                     * zero at a zero digit, as in 105. its divisor is
                     * quotient * place, where place alone would keep the
                     * digits after it */
                    value %= quotient * place;
#endif
                    *digit = quotient + '0';
                    digit++;
                } while (--digits > 0);
                *digit = '\0';
            }
            if (!kinder_message_insert(text, number, &message)) {
                clinic->message = KINDER_NO_MESSAGE;
                syFree(number);
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_SERVICE_BANK);
            } else {
                kinder_dialog_set_text(message, strlen(message));
                syFree(number);
                syFree(message);
                clinic->message = LIVES_RESULT;
                clinic->step = CHECKUP_COUNT_LIVES;
                return;
            }
        }
        clinic->timer = PAGE_FRAMES;
        clinic->step = CHECKUP_NEXT_PAGE;
        return;
    case CHECKUP_NEXT_PAGE:
        if (clinic->timer > 0) {
            clinic->timer--;
            break;
        }
        if (!(pad_press_repeat[0] & PDD_DGT_TA)) {
            break;
        }
        if (checkup_message_group(clinic->message) == ILLNESS_GROUP) {
            Uint8 model = (clinic->message - ILLNESS_RESULT) / ILLNESS_ENTRIES;
            clinic->medicine = medicine_create(task, model);
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            clinic->step = CHECKUP_WAIT_MEDICINE;
        } else if (clinic->message == STAMINA_PAGE) {
            clinic->message = STAMINA_PAGE + 1;
            kinder_dialog_set(clinic->message, KINDER_SERVICE_BANK);
            kinder_dialog_hide_arrow();
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
        } else if (clinic->message == PERSONALITY_PAGE) {
            clinic->message = PERSONALITY_PAGE + 1;
            kinder_dialog_set(clinic->message, KINDER_SERVICE_BANK);
            kinder_dialog_hide_arrow();
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
        } else {
            kinder_dialog_set(FAREWELL_MESSAGE, KINDER_WHOLE_BANK);
            sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
            clinic->step = CHECKUP_CLOSING;
        }
        clinic->timer = PAGE_FRAMES;
        return;
    case CHECKUP_COUNT_LIVES:
        if (clinic->timer > 0) {
            clinic->timer--;
            break;
        }
        if (!(pad_press_repeat[0] & PDD_DGT_TA)) {
            break;
        }
        patient = clinic->patient;
        clinic->timer = PAGE_FRAMES;
        if (clinic->message == LIVES_RESULT) {
            variant = chao_reincarnation_comment(patient);
            group = checkup_message_variant(LIVES_GROUP, variant);
            kinder_dialog_set(group, KINDER_SERVICE_BANK);
            clinic->message = group;
            if (kinder_message_next_page(clinic->message,
                                         KINDER_SERVICE_BANK) >= 0) {
                kinder_dialog_show_arrow();
            }
        } else {
            group =
                kinder_message_next_page(clinic->message, KINDER_SERVICE_BANK);
            if (group >= 0) {
                kinder_dialog_set(group, KINDER_SERVICE_BANK);
                clinic->message = group;
                if (kinder_message_next_page(clinic->message,
                                             KINDER_SERVICE_BANK) >= 0) {
                    kinder_dialog_show_arrow();
                }
            } else {
                kinder_dialog_set(FAREWELL_MESSAGE, KINDER_WHOLE_BANK);
                clinic->timer = PAUSE_FRAMES;
                clinic->step = CHECKUP_CLOSING;
            }
        }
        sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
        break;
    case CHECKUP_WAIT_MEDICINE:
        if (kinder_checkup_state() != KINDER_CHECKUP_MEDICINE) {
            kinder_dialog_set(FAREWELL_MESSAGE, KINDER_WHOLE_BANK);
            clinic->timer = PAUSE_FRAMES;
            sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
            clinic->step = CHECKUP_CLOSING;
        }
        break;
    case CHECKUP_CLOSING:
        if (clinic->timer > 0) {
            clinic->timer--;
            break;
        }
        kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
        clinic->state = KINDER_CHECKUP_DONE;
        clinic->step = CHECKUP_DONE;
        break;
    case CHECKUP_DONE:
        break;
    }
}

static void checkup_destroy(struct task *task)
{
    if (clinic->patient) {
        task_kill(clinic->patient);
        clinic->patient = NULL;
    }
    task_destroy_children(task);
    clinic->medicine = NULL;
    clinic->task = NULL;
    syFree(clinic);
    clinic = NULL;
    kinder_message_table(NULL, KINDER_SERVICE_BANK);
    njReleaseTexture(&medicine_textures);
    /* @hack redundant: a return at the function's end */
    /* NOLINTNEXTLINE(readability-redundant-control-flow) */
    return;
}

struct task *kinder_checkup_create(struct task *parent)
{
    /* @hack redundant: register on task, where a plain local would do */
    register struct task *task;
    if (clinic) {
        return NULL;
    }
    if (!parent) {
        return NULL;
    }
#ifdef NONMATCHING
    /* the checkup shows KINDER_WHOLE_BANK's prompts, and KINDER_SERVICE_BANK's
     * replies from checkup_messages. this side opens no checkup without them */
    if (!checkup_messages_ready() || !kinder_script_bank(KINDER_WHOLE_BANK) ||
        !kinder_string_bank(KINDER_WHOLE_BANK) ||
        !kinder_string_bank(KINDER_SERVICE_BANK)) {
        return NULL;
    }
#endif
    task = task_create_child(TASK_WORK, checkup_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = checkup_destroy;
    texture_load_pvm(medicine_archive, &medicine_textures);
    clinic = syCalloc(1, sizeof(*clinic));
    if (!clinic) {
        /* @bug memory: checkup_destroy reads through clinic, still NULL,
         * when task_destroy at 0x0c9cc2c6 runs it */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. the destroy runs no
         * destructor, and the textures loaded above are released */
        task->destructor = NULL;
        njReleaseTexture(&medicine_textures);
#endif
        task_destroy(task);
        return NULL;
    }
    clinic->task = task;
    clinic->state = KINDER_CHECKUP_OPEN;
    kinder_message_table(checkup_messages, KINDER_SERVICE_BANK);
    return task;
}

void kinder_checkup_release(void)
{
    if (clinic) {
        clinic->task = NULL;
        syFree(clinic);
        clinic = NULL;
        kinder_message_table(NULL, KINDER_SERVICE_BANK);
    }
}

Sint8 kinder_checkup_state(void)
{
    if (!clinic) {
        return KINDER_CHECKUP_ABSENT;
    }
    return clinic->state;
}

void kinder_checkup_set_state(Sint8 state)
{
    if (clinic) {
        clinic->state = state;
    }
}

/* the medicine's task work: tick is the model, an index of medicine_models.
 * scl scales the panel, and ang turns the model */
static void medicine_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case MEDICINE_OPEN: {
        struct service_box box;
        Float width;
        box = medicine_frame;
        if (game_state.text_language != TEXT_JAPANESE) {
            width = box.width;
            box.width = width * WIDE_TEXT;
            width = (box.width - width) * 0.5F;
            box.pos.x -= width;
        }
        service_frame_create(task, SERVICE_PINK, &box);
        work->mode = MEDICINE_UNFOLD;
        work->submode = 0;
        break;
    }
    case MEDICINE_UNFOLD:
        if (!(work->flags & MEDICINE_PANEL)) {
            /* @hack redundant: a first store to scl.x, which the next
             * replaces */
            work->scl.x = 0.2F;
            work->scl.x = UNFOLD_WIDTH;
            work->flags |= MEDICINE_PANEL;
        } else {
            if (work->scl.x < 1.0F) {
                work->scl.x += UNFOLD_STEP_X;
                if (work->scl.x > 1.0F) {
                    work->scl.x = 1.0F;
                }
            }
            if (work->scl.y < 1.0F) {
                work->scl.y += UNFOLD_STEP_Y;
                if (work->scl.y > 1.0F) {
                    work->scl.y = 1.0F;
                }
            }
        }
        if (work->scl.x == 1.0F && work->scl.y == 1.0F) {
            struct kinder_message_label label;
            work->flags |= MEDICINE_MODEL;
            label = medicine_label;
            if (game_state.text_language != TEXT_JAPANESE) {
                label.width *= WIDE_TEXT;
            }
            kinder_message_label_create(task, &label);
            work->timer = MEDICINE_FRAMES;
            work->mode = MEDICINE_SHOW;
            work->submode = 0;
        }
        break;
    case MEDICINE_SHOW:
        if (work->timer > 0) {
            work->timer--;
        }
        if ((pad_press_repeat[0] & PDD_DGT_TA) || work->timer == 0) {
            work->mode = MEDICINE_DONE;
            work->submode = 0;
            kinder_checkup_set_state(KINDER_CHECKUP_OPEN);
            return;
        }
        break;
    case MEDICINE_DONE:
        break;
    }
    work->ang.x += SPIN_X;
    work->ang.y += SPIN_Y;
    if (work->mode < MEDICINE_DONE) {
        kinder_checkup_set_state(KINDER_CHECKUP_MEDICINE);
    }
}

struct medicine_pose {
    NJS_POINT3 pos;
    Float width, height;
    struct angles rotation;
};

static void medicine_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: the model's rotation in a union with the sprite, past 20
     * bytes of medicine_pose that nothing reads, where two locals would do */
    union {
        NJS_SPRITE sprite;
        struct medicine_pose model;
    } pose;
    struct service_box box;
    struct angles *rotation;

    if (work->flags & MEDICINE_PANEL) {
#ifdef NONMATCHING
        /* this side zeroes the sprite, whose fields the lines below don't all
         * set */
        memset(&pose.sprite, 0, sizeof(pose.sprite));
#endif
        box = medicine_backdrop;
        pose.sprite.sx = work->scl.x;
        pose.sprite.sy = work->scl.y;
        pose.sprite.tlist = NULL;
        pose.sprite.tanim = NULL;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        service_set_color(PANEL_COLOR);
        service_box_draw(&pose.sprite, &box, 0, PANEL_DEPTH);
        constant_material_set(0, 0, 0, 0);
    }
    if (work->flags & MEDICINE_MODEL) {
        /* @hack type: the rotation through a cast of &pose, where
         * &pose.model.rotation would do */
        rotation = &((struct medicine_pose *)&pose)->rotation;
        if (work->tick <= LAST_UPRIGHT_MODEL) {
            rotation->x = work->ang.x;
            rotation->y = work->ang.y;
            rotation->z = MODEL_TILT;
        } else {
            rotation->x = MODEL_LIE;
            rotation->y = work->ang.y;
            rotation->z = work->ang.x;
        }
        draw_control_save();
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
        njSetTexture(&medicine_textures);
        njPushMatrixEx();
        njTranslate(NULL, 0, MODEL_Y, MODEL_Z);
        njRotateEx(&rotation->x, 0);
        njScale(NULL, MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
        njCnkSimpleDrawObject(medicine_models[work->tick]);
        njPopMatrixEx();
        draw_control_restore();
    }
}

static void medicine_destroy(struct task *task)
{
    clinic->medicine = NULL;
}

static struct task *medicine_create(struct task *parent, Uint8 model)
{
    struct task *task;
    struct task_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, medicine_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = medicine_draw;
    task->destructor = medicine_destroy;
    work = task->work;
    work->tick = model;
    return task;
}
