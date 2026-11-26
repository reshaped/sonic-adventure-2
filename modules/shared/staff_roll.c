/* the staff roll's state and task, and the loading of its textures.
 * staff_roll_text.c draws its lines and photographs */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "shared/staff_art.h"
#include "shared/staff_credits.h"
#include "shared/staff_roll.h"
#include "system/flags.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/texture.h"

#define TASK_LEVEL 2
#define LINE_HEIGHT 40.0F
#define START_Y 600.0F
/* first_y travels SCROLL_LENGTH in the roll's duration, from START_Y until
 * the last line's y reaches -LINE_HEIGHT */
#define SCROLL_LENGTH ((LINE_HEIGHT * staff_line_count) + START_Y)
#define HERO_ROLL_FRAMES 9807
#define DARK_ROLL_FRAMES 10392
/* the hold begins once the y one line below the last line passes HOLD_Y. the
 * roll ends once the hold has lasted more than HOLD_FRAMES frames */
#define HOLD_Y (-200.0F)
#define HOLD_FRAMES 60
/* a photograph fades in or out in FADE_STEPS steps */
#define FADE_STEPS 60
#define FADE_STEP (1.0F / FADE_STEPS)
#define PHOTO_PRIORITY (-2.0F)

struct staff_roll {
    Float first_y;
    Sint32 photo; /* a texture of the story's photographs, or -1 for none */
    /* the photograph's fade level, from 0 to 1, in alpha and each color of its
     * constant material */
    Float level;
    Sint32 held; /* the frames the hold has lasted */
};

int staff_roll_story;
static Float staff_scroll_speed;
/* @hack type: the count of frames since staff_roll_start as a Float, where
 * an int would do */
static Float staff_roll_frame;

Bool staff_roll_running = FALSE;
static NJS_POINT3 staff_photo_position = {50.0F, 50.0F, PHOTO_PRIORITY};
static struct staff_roll staff_roll = {START_Y, -1, 0.0F, 0};

/* move *value by step toward target, and not past it. it returns TRUE where
 * *value was not target, and FALSE, changing nothing, where it was. step must
 * not be negative */
static Bool staff_step_toward(Float *value, Float target, Float step)
{
    if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
        return TRUE;
    }
    if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
        return TRUE;
    }
    return FALSE;
}

static void staff_roll_draw(struct task *task)
{
    int line;

    for (line = 0; line < staff_line_count; line++) {
        switch (staff_lines[line].kind) {
        case STAFF_LINE_BLANK:
            break;
        case STAFF_LINE_TEXT:
        case STAFF_LINE_ALT_TEXT:
            staff_text_draw(staff_lines[line].kind, &staff_lines[line].color,
                            staff_lines[line].text,
                            staff_roll.first_y + (LINE_HEIGHT * line));
            break;
        default:
            staff_logo_draw(staff_lines[line].kind,
                            staff_roll.first_y + (LINE_HEIGHT * line));
            break;
        }
    }
    /* @hack spelling: the story and the photograph tested in nested ifs,
     * where one test of both would do */
    if (staff_roll_story != SEQUENCE_NONE) {
        if (staff_roll.photo >= 0) {
            constant_material_set(staff_roll.level, staff_roll.level,
                                  staff_roll.level, staff_roll.level);
            staff_photo_draw(staff_roll.photo, &staff_photo_position,
                             NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        }
    }
}

static void staff_roll_step(struct task *task)
{
    const struct staff_cue *cues;
    int cue;
    int photo;

    staff_roll.first_y -= staff_scroll_speed;
    staff_roll_frame += 1.0F;
    task->draw = staff_roll_draw;
    if (staff_roll_story != SEQUENCE_NONE) {
        /* each cue table starts before frame 1 and ends at frame 216000. the
         * search stays inside it through a roll of up to 216000 frames */
        cues = staff_photo_cues[staff_roll_story - SEQUENCE_HERO];
        cue = 0;
        while (staff_roll_frame > cues[cue].frame) {
            cue++;
        }
        cue--;
        photo = cues[cue].photo;
        if (photo != staff_roll.photo) {
            /* @hack spelling: staff_step_toward's result compared with FALSE,
             * where its negation would do */
            if (staff_step_toward(&staff_roll.level, 0.0F, FADE_STEP) ==
                FALSE) {
                staff_roll.photo = photo;
            }
        } else {
            staff_step_toward(&staff_roll.level, 1.0F, FADE_STEP);
        }
    }
    if (staff_roll.first_y + (LINE_HEIGHT * staff_line_count) < HOLD_Y) {
        staff_roll.held++;
    }
    if (staff_roll.held > HOLD_FRAMES) {
        task_destroy(task);
        staff_roll_running = FALSE;
    }
}

void staff_roll_set_duration(int frames)
{
    staff_scroll_speed = SCROLL_LENGTH / frames;
}

void staff_roll_start(void)
{
    /* @hack spelling: the product in each branch, where SCROLL_LENGTH times
     * the chosen reciprocal would do */
    if (game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO) {
        staff_scroll_speed = SCROLL_LENGTH * (1.0F / HERO_ROLL_FRAMES);
    } else {
        staff_scroll_speed = SCROLL_LENGTH * (1.0F / DARK_ROLL_FRAMES);
    }
    task_create(TASK_WORK, TASK_LEVEL, staff_roll_step, "starffRollExec");
    staff_roll_running = TRUE;
    staff_roll_frame = 0.0F;
}

void staff_roll_load(int story)
{
    int cell;

    /* @hack spelling: the loop over every cell, skipping the glyphs', where a
     * loop from STAFF_GLYPHS would do */
    for (cell = 0; cell < STAFF_ROLL_CELLS; cell++) {
        if (cell >= STAFF_GLYPHS) {
            staff_roll_cells[cell].cx = staff_roll_cells[cell].sx / 2;
            staff_roll_cells[cell].cy = staff_roll_cells[cell].sy / 2;
        }
    }
    staff_glyphs_measure();
    texture_load_pvm("staffRoll", &staff_roll_texlist);
    switch (story) {
    case SEQUENCE_HERO:
        njLoadTexturePvmFile("shotHero.pvm", &staff_hero_texlist);
        break;
    case SEQUENCE_DARK:
        njLoadTexturePvmFile("shotDark.pvm", &staff_dark_texlist);
        break;
    }
    staff_roll_story = story;
}

void staff_roll_release(void)
{
    njReleaseTexture(&staff_roll_texlist);
    switch (staff_roll_story) {
    case SEQUENCE_HERO:
        njReleaseTexture(&staff_hero_texlist);
        break;
    case SEQUENCE_DARK:
        njReleaseTexture(&staff_dark_texlist);
        break;
    }
}
