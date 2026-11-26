#include <assert.h>
#include <string.h>
#include <shinobi.h>

#include "advertise/emblem_results_menu.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/story_select.h"
#include "advertise/story_select_draw.h"
#include "advertise/synopsis.h"
#include "advertise/synopsis_screen.h"
#include "advertise/title_screen.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "system/pad.h"

struct menu_control menu_control;
struct menu_works menu_works;
struct save_data save_data;
Uint32 pad_press_repeat[PAD_PORTS];
Uint32 pad_choose_buttons = PDD_DGT_TA;
Uint32 pad_cancel_buttons = PDD_DGT_TB;

static int title_starts, sequence_starts, sequence_handoffs;
static int selected_story, selected_step;

void title_screen_start(void)
{
    title_starts++;
}
void menu_start_sequence_at(int sequence, int step)
{
    selected_story = sequence;
    selected_step = step;
    sequence_starts++;
}
void menu_to_sequence(void)
{
    sequence_handoffs++;
}
void menu_textures_select(Int group)
{
    assert(group == MENU_TEXTURES_STORY_SELECT);
}
void music_play_name(const char *name)
{
    assert(name != NULL);
}
Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    (void)id;
    (void)key;
    (void)priority;
    (void)volume;
    return 0;
}
void voice_set_play(int set)
{
    (void)set;
}
void synopsis_select(int story, int step)
{
    (void)story;
    (void)step;
    assert(0);
}
void synopsis_screen_start(void)
{
    assert(0);
}
void emblem_results_menu_start(void)
{
    assert(0);
}
void new_menu_screen_start(void)
{
    assert(0);
}
void new_menu_screen_reopen(int group)
{
    (void)group;
    assert(0);
}

static Bool step(Uint32 buttons)
{
    pad_press_repeat[0] = buttons;
    return story_select_step();
}

static void open_chapters(int story)
{
    memset(&story_select_work, 0, sizeof(story_select_work));
    menu_works.work_03 = &story_select_work;
    menu_control.mode = MENU_MODE_STORY_SELECT;
    story_select_work.story = story;
    save_data.awards[SAVE_AWARD_HERO_STORY + story] = 1;
    title_starts = sequence_starts = sequence_handoffs = 0;
    story_select_start();
    assert(!step(0));
    assert(!step(0));
    assert(!step(pad_choose_buttons));
    assert(!step(0));
    assert(!step(0));
    assert(story_select_work.panels[story].list.shown);
}

int main(int argc, char **argv)
{
    int story, chapter, count;
    int populated;

    assert(argc == 2);
    populated = strcmp(argv[1], "populated") == 0;
    for (story = STORY_HERO; story <= STORY_LAST; story++) {
        open_chapters(story);
        count = story_select_work.panels[story].chapters;
        assert(count > 0);
        for (chapter = 0; chapter < count; chapter++) {
            open_chapters(story);
            story_select_work.panels[story].chapter = chapter;
            assert(!step(pad_choose_buttons));
            if (populated) {
                assert(!title_starts && sequence_starts == 1);
                assert(selected_story == SEQUENCE_HERO + story);
                assert(selected_step == chapter * (story + 2));
                assert(step(0) && sequence_handoffs == 1);
            } else {
                assert(title_starts == 1 &&
                       menu_control.mode == MENU_MODE_TITLE);
                assert(!sequence_starts && !sequence_handoffs);
            }
        }
        if (populated) {
            open_chapters(story);
            story_select_work.panels[story].chapter = count;
            assert(!step(pad_choose_buttons));
            assert(title_starts == 1 && !sequence_starts && !sequence_handoffs);
        }
    }
    return 0;
}
