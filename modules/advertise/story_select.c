/* the story select. its step drives story_select_draw.h's panels through
 * story_select_work, which menu_works.work_03 points at */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/emblem_results_menu.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/story_select.h"
#include "advertise/story_select/text.inc"
#include "advertise/story_select_draw.h"
#include "advertise/synopsis.h"
#include "advertise/synopsis_screen.h"
#ifdef NONMATCHING
/* title_screen_start, which this side's chapter step calls */
#include "advertise/title_screen.h"
#endif
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "system/pad.h"
#include "system/sequence.h"

struct story_select_work story_select_work;

/* the story chosen */
static Sint32 story_select_chosen;

/* menu_control.step. each wait below lasts until a step of the panels'
 * task moves nothing */
enum {
    STEP_OPEN, /* show the backdrop and the stories' panels */
    STEP_OPENING, /* wait, then frame the cursor's panel */
    STEP_STORIES, /* move between the stories, and choose one */
    STEP_CHOSEN, /* wait, then show the chosen story's list */
    STEP_LIST_OPENING, /* wait, then show the list's cursor */
    STEP_CHAPTERS, /* move along the list, and choose a chapter */
    /* wait, then play the cursor's story from its saved progress. no step
     * goes to it */
    STEP_CONTINUE,
    STEP_CHAPTER_CHOSEN /* wait, then end the menus */
};

/* voice_set_play's sets as the story select opens, and as a list opens */
#define OPEN_VOICES 0
#define LIST_VOICES 1
/* the byte of save_data.awards that holds story's award, which opens its
 * chapters. the stories' awards lie in the stories' order */
#define STORY_AWARD(story) ((story) + SAVE_AWARD_HERO_STORY)

/* a story's chapter, the step of the story's sequence it starts at.
 * nothing reads unk_04 */
struct chapter {
    Sint32 step;
    Sint32 unk_04;
};

static struct chapter hero_chapters[STORY_SELECT_HERO_CHAPTERS] = {
#include "advertise/story_select/hero_chapters.inc"
};
static struct chapter dark_chapters[STORY_SELECT_DARK_CHAPTERS] = {
#include "advertise/story_select/dark_chapters.inc"
};
static struct chapter last_chapters[STORY_SELECT_LAST_CHAPTERS] = {
#include "advertise/story_select/last_chapters.inc"
};
static struct chapter *chapters[STORY_SELECT_STORIES] = {
#include "advertise/story_select/chapters.inc"
};

#ifdef NONMATCHING
/* The last chapter of each story starts after the opening step. In the
 * disc-free build the three directories retain their pointers and have
 * zero steps. */
static Bool story_select_has_chapter(int story)
{
    Uint32 count;
    const struct chapter *entries;

    switch (story) {
    case STORY_HERO:
        count = STORY_SELECT_HERO_CHAPTERS;
        break;
    case STORY_DARK:
        count = STORY_SELECT_DARK_CHAPTERS;
        break;
    case STORY_LAST:
        count = STORY_SELECT_LAST_CHAPTERS;
        break;
    default:
        return FALSE;
    }
    entries = chapters[story];
    return entries != NULL && entries[count - 1].step > 0 &&
           (Uint32)menu_works.work_03->panels[story].chapter < count;
}
#endif

static void story_select_panels_show(void);
static void story_select_play_story(int story);
static void story_select_play_chapter(int story, int chapter);

void story_select_start(void)
{
    menu_textures_select(MENU_TEXTURES_STORY_SELECT);
    menu_control.step = STEP_OPEN;
    music_play_name(STORY_SELECT_MUSIC);
}

static void story_select_hide(void)
{
    menu_works.work_03->shown = FALSE;
    menu_works.work_03->framed = FALSE;
    menu_works.work_03->panels[STORY_HERO].list.bright = 0;
    menu_works.work_03->panels[STORY_DARK].list.bright = 0;
    menu_works.work_03->panels[STORY_LAST].list.bright = 0;
    menu_works.work_03->panels[STORY_HERO].labeled = FALSE;
    menu_works.work_03->panels[STORY_DARK].labeled = FALSE;
    menu_works.work_03->panels[STORY_LAST].labeled = FALSE;
}

void story_select_reopen(int which)
{
    /* @hack spelling: a switch of one case, where an if would do */
    switch (which) {
    case STORY_SELECT_REOPEN:
        menu_control.step = STEP_OPEN;
        break;
    }
}

Bool story_select_step(void)
{
    /* @hack spelling: the masks and menu_control through local pointers,
     * where their own names would do */
    Uint32 *choose = &pad_choose_buttons;
    Uint32 *cancel = &pad_cancel_buttons;
    struct menu_control *control = &menu_control;

    switch (control->step) {
    case STEP_OPEN:
        menu_works.work_03->shown = TRUE;
        menu_works.work_03->backdrop.shown = TRUE;
        story_select_panels_show();
        control->step = STEP_OPENING;
        voice_set_play(OPEN_VOICES);
        break;
    case STEP_OPENING:
        if (!menu_works.work_03->moving) {
            control->step = STEP_STORIES;
            menu_works.work_03->framed = TRUE;
        }
        break;
    case STEP_STORIES:
        /* the panels stand hero, last, dark from the left. the last's is
         * passed over while it is hidden */
        if (pad_press_repeat[0] & PDD_DGT_KR) {
            switch (menu_works.work_03->story) {
            case STORY_HERO:
                menu_works.work_03->story++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                if (menu_works.work_03->panels[STORY_LAST].shown) {
                    menu_works.work_03->story++;
                }
                break;
            /* @hack redundant: a case that only breaks */
            case STORY_DARK:
                break;
            case STORY_LAST:
                menu_works.work_03->story--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                break;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            switch (menu_works.work_03->story) {
            /* @hack redundant: a case that only breaks */
            case STORY_HERO:
                break;
            case STORY_DARK:
                if (menu_works.work_03->panels[STORY_LAST].shown) {
                    menu_works.work_03->story++;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                } else {
                    menu_works.work_03->story--;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
                break;
            case STORY_LAST:
                /* @hack redundant: the test of story, which is STORY_LAST
                 * here */
                if (menu_works.work_03->story) {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
                menu_works.work_03->story = STORY_HERO;
                break;
            }
        } else if (pad_press_repeat[0] & *choose) {
            story_select_chosen = menu_works.work_03->story;
            if (save_data.awards[STORY_AWARD(menu_works.work_03->story)]) {
                voice_set_play(LIST_VOICES);
                control->step = STEP_CHOSEN;
                menu_works.work_03->panels[STORY_HERO].shown = FALSE;
                menu_works.work_03->panels[STORY_DARK].shown = FALSE;
                menu_works.work_03->panels[STORY_LAST].shown = FALSE;
                menu_works.work_03->framed = FALSE;
                menu_works.work_03->panels[menu_works.work_03->story].shown =
                    TRUE;
                menu_works.work_03->panels[menu_works.work_03->story].chosen =
                    TRUE;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            } else {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                story_select_hide();
                menu_works.work_03->panels[story_select_chosen].marked = FALSE;
                story_select_play_story(story_select_chosen);
                synopsis_screen_start();
                control->mode = MENU_MODE_SYNOPSIS;
            }
        } else if (pad_press_repeat[0] & *cancel) {
            story_select_hide();
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            if (menu_works.work_03->from_emblems) {
                emblem_results_menu_start();
                control->mode = MENU_MODE_EMBLEM_RESULTS;
                /* @bug logic: a story select the new menu opens later goes
                 * back to the emblem results too. the store clears the boss
                 * attack's from_emblems, work_15's (0x8c50816a), where the
                 * test read the story select's (0x8c508156). the story
                 * select's stays 1 until story_select_draw_reset */
                menu_works.work_15->from_emblems = FALSE;
            } else {
                new_menu_screen_start();
                control->mode = MENU_MODE_NEW_MENU;
                new_menu_screen_reopen(NEW_MENU_REOPEN_SINGLE);
            }
        }
        break;
    case STEP_CHOSEN:
        if (menu_works.work_03->moving) {
            break;
        }
        story_select_chosen = menu_works.work_03->story;
        menu_works.work_03->panels[story_select_chosen].list.shown = TRUE;
        menu_works.work_03->panels[story_select_chosen].list.bright = 1;
        control->step = STEP_LIST_OPENING;
        break;
    case STEP_LIST_OPENING:
        /* @hack spelling: an empty arm under the test of moving, where a
         * test of !moving would do */
        if (menu_works.work_03->moving) {
        } else {
            menu_works.work_03->panels[story_select_chosen].marked = TRUE;
            control->step = STEP_CHAPTERS;
        }
        break;
    case STEP_CHAPTERS:
#ifdef NONMATCHING
        /* this side goes back to the title for a story story_select_has_chapter
         * refuses. it refuses each story in the disc-free build, whose chapters
         * have no steps */
        if (!story_select_has_chapter(story_select_chosen)) {
            story_select_hide();
            title_screen_start();
            control->mode = MENU_MODE_TITLE;
            break;
        }
#endif
        if (!menu_works.work_03->moving) {
            if (pad_press_repeat[0] & PDD_DGT_KU) {
                menu_works.work_03->panels[story_select_chosen].chapter--;
                if (menu_works.work_03->panels[story_select_chosen].chapter <
                    0) {
                    menu_works.work_03->panels[story_select_chosen].chapter = 0;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                menu_works.work_03->panels[story_select_chosen].chapter++;
                if (menu_works.work_03->panels[story_select_chosen].chapter >=
                    menu_works.work_03->panels[story_select_chosen].chapters) {
                    menu_works.work_03->panels[story_select_chosen].chapter =
                        menu_works.work_03->panels[story_select_chosen]
                            .chapters -
                        1;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & *cancel) {
                menu_works.work_03->panels[menu_works.work_03->story]
                    .list.shown = FALSE;
                menu_works.work_03->panels[menu_works.work_03->story]
                    .list.bright = 0;
                menu_works.work_03->panels[story_select_chosen].marked = FALSE;
                story_select_panels_show();
                menu_works.work_03->panels[story_select_chosen].chosen = FALSE;
                menu_works.work_03->panels[STORY_DARK].chosen = FALSE;
                menu_works.work_03->panels[STORY_LAST].chosen = FALSE;
                control->step = STEP_OPENING;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            } else if (pad_press_repeat[0] & *choose) {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                story_select_play_chapter(
                    story_select_chosen,
                    menu_works.work_03->panels[story_select_chosen].chapter);
                story_select_hide();
                menu_works.work_03->panels[story_select_chosen].marked = FALSE;
                control->step = STEP_CHAPTER_CHOSEN;
            }
        }
        break;
    case STEP_CONTINUE:
        if (!menu_works.work_03->moving) {
            menu_start_sequence_at(
                menu_works.work_03->story + SEQUENCE_HERO,
                save_data.progress[menu_works.work_03->story]);
            menu_to_sequence();
            return TRUE;
        }
        break;
    case STEP_CHAPTER_CHOSEN:
        if (menu_works.work_03->moving) {
            break;
        }
        menu_to_sequence();
        return TRUE;
    }
    /* @hack redundant: a test of sound_play's address that never runs */
    if (0) {
        if (sound_play) { /* NOLINT(clang-diagnostic-pointer-bool-conversion) */
        }
    }
    return FALSE;
}

/* show the hero and the dark story's panels, and the last story's once
 * both their awards are given. give each list its count of chapters */
static void story_select_panels_show(void)
{
    menu_works.work_03->panels[STORY_HERO].shown = TRUE;
    menu_works.work_03->panels[STORY_DARK].shown = TRUE;
    if (save_data.awards[STORY_AWARD(STORY_HERO)] &&
        save_data.awards[STORY_AWARD(STORY_DARK)]) {
        menu_works.work_03->panels[STORY_LAST].shown = TRUE;
    }
    menu_works.work_03->panels[STORY_HERO].chapters =
        STORY_SELECT_HERO_CHAPTERS;
    menu_works.work_03->panels[STORY_DARK].chapters =
        STORY_SELECT_DARK_CHAPTERS;
    menu_works.work_03->panels[STORY_LAST].chapters =
        STORY_SELECT_LAST_CHAPTERS;
}

/* start story's sequence at the step save_data.progress keeps, and
 * select the synopsis of that step */
static void story_select_play_story(int story)
{
    synopsis_select(story, save_data.progress[story]);
    menu_start_sequence_at(story + SEQUENCE_HERO, save_data.progress[story]);
}

static void story_select_play_chapter(int story, int chapter)
{
    menu_start_sequence_at(story + SEQUENCE_HERO,
                           chapters[story][chapter].step);
}
