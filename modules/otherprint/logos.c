/* the logos OTHERPRINT.PRS shows under OTHER_PRINT_LOGOS */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "otherprint/logos.h"
#include "otherprint/otherprint.h"
#include "shared/screen_sprite.h"
#include "shared/staff_roll.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#ifdef NONMATCHING
/* this side shows the SH4ZAM splash after the Sonic Team logo */
#include "engine/dc/sh4zam_splash.h"
#endif

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define ALERT_HOLD_STEPS 300
#define LOGO_HOLD_STEPS 120
#define ALERT_TEXTURE 3
#define ALERT_PRIORITY (-10.0F)
#define SPRITE_ATTR (NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA)

/* the logos' cells in logo_cells. cell n takes texture n of logos_texlist.
 * the task's submode holds the cell logos_draw_logo draws */
enum logo_cell { LOGO_SEGA, LOGO_SEGA_US, LOGO_SONIC_TEAM, LOGO_CELLS };

enum {
    MODE_ALERT_IN,
    MODE_ALERT_HOLD,
    MODE_ALERT_OUT,
    MODE_SEGA_IN,
    MODE_SEGA_HOLD,
    MODE_SEGA_OUT,
    MODE_SONIC_TEAM_IN,
    MODE_SONIC_TEAM_HOLD,
    MODE_SONIC_TEAM_OUT,
    MODE_SOFDEC_IN,
    MODE_SOFDEC_HOLD,
    MODE_SOFDEC_OUT
#ifdef NONMATCHING
    /* this side's modes for the SH4ZAM splash, after the shipped ones */
    ,
    MODE_SH4ZAM_IN,
    MODE_SH4ZAM_HOLD,
    MODE_SH4ZAM_OUT
#endif
};

struct logos_fade {
    Float level; /* screen_draw_begin's level, from 0 to 1 */
    Sint32 held; /* the steps the hold has lasted */
};
static struct logos_fade logos_fade;
/* step the level by FADE_STEP toward target. where the level is at target
 * already, nothing changes and the result is TRUE */
/* @hack spelling: the step's result compared with FALSE, where its
 * negation would do */
#define FADE_TO(target)                                                        \
    (float_step_toward(&logos_fade.level, (target), FADE_STEP) == FALSE)

#include "otherprint/logos/text.inc"

static NJS_TEXNAME logos_texnames[11] = {
#include "otherprint/logos/logos_texnames.inc"
};
static NJS_TEXLIST logos_texlist = {
#include "otherprint/logos/logos_texlist.inc"
};

/* each cell takes the whole of its texture. logos_start moves each cell's
 * origin to its center */
static NJS_TEXANIM logo_cells[LOGO_CELLS] = {
#include "otherprint/logos/logo_cells.inc"
};

static struct screen_sprite logo_sprite = {
#include "otherprint/logos/logo_sprite.inc"
};
static struct screen_sprite sofdec_sprites[2] = {
#include "otherprint/logos/sofdec_sprites.inc"
};

static void logos_step(struct task *task);
static void logos_draw_logo(struct task *task);
static void logos_draw_alert(struct task *task);
static void logos_draw_sofdec(struct task *task);
#ifdef NONMATCHING
/* this side draws the SH4ZAM splash at the logos' fade level */
static void logos_draw_sh4zam(struct task *task);
#endif
static void logos_destroy(struct task *task);

void logos_start(void)
{
    int i;
    struct task *task;

    for (i = 0; i < LOGO_CELLS; i++) {
        logo_cells[i].cx = logo_cells[i].sx / 2;
        logo_cells[i].cy = logo_cells[i].sy / 2;
    }
#ifdef NONMATCHING
    /* this side ends the logos where the texture list is empty or the textures
     * don't load. the disc-free build's zeros leave the list empty */
    if (logos_texlist.nbTexture == 0 ||
        !texture_load_pvm("segalogo", &logos_texlist)) {
        otherprint_done = TRUE;
        return;
    }
#else
    texture_load_pvm("segalogo", &logos_texlist);
#endif
    task = task_create(TASK_WORK, TASK_LEVEL, logos_step, "execSegalogo");
#ifdef NONMATCHING
    /* this side discards the textures and ends the logos where task_create
     * fails. the matching side writes through the null */
    if (task == NULL) {
        texture_discard_pvm(&logos_texlist);
        otherprint_done = TRUE;
        return;
    }
#endif
    logos_fade.level = 0.0F;
    if (game_state.edition == EDITION_TENTO) {
        for (i = 0; i < STAFF_ROLL_CELLS; i++) {
            staff_roll_cells[i].cx = staff_roll_cells[i].sx / 2;
            staff_roll_cells[i].cy = staff_roll_cells[i].sy / 2;
        }
#ifdef NONMATCHING
        /* this side ends the logos where the staff roll's textures don't load.
         * it discards the logos' textures and destroys the task */
        if (!texture_load_pvm("staffRoll", &staff_roll_texlist)) {
            texture_discard_pvm(&logos_texlist);
            task_destroy(task);
            otherprint_done = TRUE;
            return;
        }
#else
        texture_load_pvm("staffRoll", &staff_roll_texlist);
#endif
        task->work->mode = MODE_ALERT_IN;
    } else {
        task->work->mode = MODE_SEGA_IN;
    }
}

static void logos_step(struct task *task)
{
    struct task_work *work = task->work;

    task->destructor = logos_destroy;
    switch (work->mode) {
    case MODE_ALERT_IN:
        task->draw = logos_draw_alert;
        if (FADE_TO(1.0F)) {
            work->mode = MODE_ALERT_HOLD;
            logos_fade.held = 0;
        }
        break;
    case MODE_ALERT_HOLD:
        logos_fade.held++;
        if (logos_fade.held >= ALERT_HOLD_STEPS) {
            work->mode = MODE_ALERT_OUT;
        }
        break;
    case MODE_ALERT_OUT:
        if (FADE_TO(0.0F)) {
            work->mode = MODE_SEGA_IN;
        }
        break;
    case MODE_SEGA_IN:
        task->draw = logos_draw_logo;
        if (game_state.us_sega_logo == 0) {
            work->submode = LOGO_SEGA;
        } else {
            work->submode = LOGO_SEGA_US;
        }
        if (pad_press_repeat[0] & PDD_DGT_ST) {
            logos_fade.level = 1.0F;
        }
        if (FADE_TO(1.0F)) {
            work->mode = MODE_SEGA_HOLD;
            logos_fade.held = 0;
        }
        break;
    case MODE_SEGA_HOLD:
        logos_fade.held++;
        if (logos_fade.held >= LOGO_HOLD_STEPS) {
            work->mode = MODE_SEGA_OUT;
        }
        break;
    case MODE_SEGA_OUT:
        if (FADE_TO(0.0F)) {
            work->mode = MODE_SONIC_TEAM_IN;
            work->submode = LOGO_SONIC_TEAM;
        }
        break;
    case MODE_SONIC_TEAM_IN:
        if (FADE_TO(1.0F)) {
            work->mode = MODE_SONIC_TEAM_HOLD;
            logos_fade.held = 0;
        }
        break;
    case MODE_SONIC_TEAM_HOLD:
        logos_fade.held++;
        if (logos_fade.held >= LOGO_HOLD_STEPS) {
            work->mode = MODE_SONIC_TEAM_OUT;
        }
        break;
    case MODE_SONIC_TEAM_OUT:
        if (FADE_TO(0.0F)) {
#ifdef NONMATCHING
            /* this side shows the SH4ZAM splash where its texture loads,
             * and goes on as shipped where it doesn't */
            if (sh4zam_splash_load()) {
                work->mode = MODE_SH4ZAM_IN;
                break;
            }
#endif
            if (game_state.edition == EDITION_TENTO) {
                work->mode = MODE_SOFDEC_IN;
            } else {
                task_destroy(task);
            }
        }
        break;
#ifdef NONMATCHING
    /* this side fades the SH4ZAM splash in, holds it and fades it out. Start
     * ends the fade in and the hold */
    case MODE_SH4ZAM_IN:
        task->draw = logos_draw_sh4zam;
        if (pad_press_repeat[0] & PDD_DGT_ST) {
            logos_fade.level = 1.0F;
        }
        if (FADE_TO(1.0F)) {
            work->mode = MODE_SH4ZAM_HOLD;
            logos_fade.held = 0;
        }
        break;
    case MODE_SH4ZAM_HOLD:
        logos_fade.held++;
        if (logos_fade.held >= LOGO_HOLD_STEPS ||
            (pad_press_repeat[0] & PDD_DGT_ST)) {
            work->mode = MODE_SH4ZAM_OUT;
        }
        break;
    case MODE_SH4ZAM_OUT:
        if (FADE_TO(0.0F)) {
            sh4zam_splash_release();
            if (game_state.edition == EDITION_TENTO) {
                work->mode = MODE_SOFDEC_IN;
            } else {
                task_destroy(task);
            }
        }
        break;
#endif
    case MODE_SOFDEC_IN:
        task->draw = logos_draw_sofdec;
        if (FADE_TO(1.0F)) {
            work->mode = MODE_SOFDEC_HOLD;
            logos_fade.held = 0;
        }
        break;
    case MODE_SOFDEC_HOLD:
        logos_fade.held++;
        if (logos_fade.held >= LOGO_HOLD_STEPS) {
            work->mode = MODE_SOFDEC_OUT;
        }
        break;
    case MODE_SOFDEC_OUT:
        if (FADE_TO(0.0F)) {
            task_destroy(task);
        }
        break;
    }
}

static void logos_draw_logo(struct task *task)
{
    struct task_work *work = task->work;

    screen_draw_begin(logos_fade.level);
    logo_sprite.cell = work->submode;
    screen_sprite_draw(&logo_sprite);
    screen_draw_end();
}

static void logos_draw_alert(struct task *task)
{
    screen_draw_begin(logos_fade.level);
    screen_picture_draw(ALERT_TEXTURE, &logos_texlist, ALERT_PRIORITY,
                        SPRITE_ATTR);
    screen_draw_end();
}

static void logos_draw_sofdec(struct task *task)
{
    screen_draw_begin(logos_fade.level);
    screen_sprite_draw(&sofdec_sprites[0]);
    screen_sprite_draw(&sofdec_sprites[1]);
    screen_draw_end();
}

#ifdef NONMATCHING
/* this side draws the SH4ZAM splash at the logos' fade level */
static void logos_draw_sh4zam(struct task *task)
{
    (void)task;
    screen_draw_begin(logos_fade.level);
    sh4zam_splash_draw();
    screen_draw_end();
}
#endif

static void logos_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side releases the splash's texture where the task ends before
     * its fade out */
    sh4zam_splash_release();
#endif
    njReleaseTexture(&logos_texlist);
    if (game_state.edition == EDITION_TENTO) {
        njReleaseTexture(&staff_roll_texlist);
    }
    otherprint_done = TRUE;
}
