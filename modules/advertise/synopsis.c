/* the story synopsis: lines that tell a story so far. they scroll up over a
 * backdrop of one of its characters, each voiced as it passes */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/synopsis.h"
#include "draw/screen.h"
#include "draw/text_sprite.h"
#include "player/player.h"
#include "shared/message_sprite.h"
#include "sound/adx.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void synopsis_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define BACKDROP_PRIORITY (-8000.0F)
#define BACKDROP_TEXTURE 0
/* a backdrop's textures, as menu_backdrop_draw takes them */
#define BACKDROP_TEXTURES 8
/* a set must have at most SPRITES lines, unchecked. none has more than 7 */
#define SPRITES 10
/* a line's places, one for each game_state.voice_language and three more.
 * each line's last four are alike */
#define PLACES 5
/* the lines scroll up SCROLL_STEP pixels a step. a line's voice plays
 * while the line lies less than SCROLL_STEP above VOICE_Y, so it plays
 * once. the set is done once its last line lies above GONE_Y */
#define SCROLL_STEP 1.0F
#define VOICE_Y 360.0F
#define GONE_Y (-120.0F)
/* the lines' x, and a z that text_sprite_draw_text_at doesn't read */
#define LINES_X 20.0F
#define LINES_Z (-100.0F)
/* the index in sets of each story's first set */
#define HERO_FIRST_SET 0
#define DARK_FIRST_SET 24
#define LAST_FIRST_SET 46

/* a line of a set: its message, and the voice that reads it */
struct synopsis_line {
    Sint32 message;
    Sint32 voice;
    /* the line's place below the set's first line, in pixels, by
     * game_state.voice_language. the lines scroll a pixel a step, so it
     * also times the voice */
    Sint32 y[PLACES];
};
/* the synopsis of a story from a step of its progress on */
struct synopsis_set {
    Sint32 story; /* a STORY_ value, or -1 at the end */
    Sint32 step;
    Sint32 character; /* whose backdrop shows, a PLAYER_ value */
    struct synopsis_line *lines;
    Sint32 count;
};
struct synopsis_state {
    Float fade; /* the backdrop's, from 0 to 1 */
    Float offset; /* the lines' y from their start, in pixels: 0 or less */
    Bool from_start; /* synopsis_from_start's result */
};

/* the state, and each line's text sprite or null */
static struct synopsis_state synopsis_state;
static struct text_sprite *synopsis_sprites[SPRITES];

#include "advertise/synopsis/text.inc"

static struct synopsis_state state_start = {0.0F, 0.0F, FALSE};
static struct task *synopsis_task = NULL;
static NJS_TEXNAME amy_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/amy_texnames.inc"
};
static NJS_TEXLIST amy_texlist = {
#include "advertise/synopsis/amy_texlist.inc"
};
static NJS_TEXNAME egg_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/egg_texnames.inc"
};
static NJS_TEXLIST egg_texlist = {
#include "advertise/synopsis/egg_texlist.inc"
};
static NJS_TEXNAME knuck_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/knuck_texnames.inc"
};
static NJS_TEXLIST knuck_texlist = {
#include "advertise/synopsis/knuck_texlist.inc"
};
static NJS_TEXNAME rouge_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/rouge_texnames.inc"
};
static NJS_TEXLIST rouge_texlist = {
#include "advertise/synopsis/rouge_texlist.inc"
};
static NJS_TEXNAME shadow_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/shadow_texnames.inc"
};
static NJS_TEXLIST shadow_texlist = {
#include "advertise/synopsis/shadow_texlist.inc"
};
static NJS_TEXNAME son_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/son_texnames.inc"
};
static NJS_TEXLIST son_texlist = {
#include "advertise/synopsis/son_texlist.inc"
};
static NJS_TEXNAME tails_texnames[BACKDROP_TEXTURES] = {
#include "advertise/synopsis/tails_texnames.inc"
};
static NJS_TEXLIST tails_texlist = {
#include "advertise/synopsis/tails_texlist.inc"
};
/* the backdrops, by the work's character, from PLAYER_SONIC to
 * PLAYER_KIND_AMY. 9 and 10, which no set names, take Sonic's and
 * Shadow's */
KEPT static struct texture_pvm backdrops[CHARACTER_KINDS] = {
#include "advertise/synopsis/backdrops.inc"
};
static struct synopsis_line lines_hero_1[5] = {
#include "advertise/synopsis/lines_hero_1.inc"
};
static struct synopsis_line lines_hero_2[5] = {
#include "advertise/synopsis/lines_hero_2.inc"
};
static struct synopsis_line lines_hero_4[6] = {
#include "advertise/synopsis/lines_hero_4.inc"
};
static struct synopsis_line lines_hero_6[7] = {
#include "advertise/synopsis/lines_hero_6.inc"
};
static struct synopsis_line lines_hero_8[4] = {
#include "advertise/synopsis/lines_hero_8.inc"
};
static struct synopsis_line lines_hero_10[5] = {
#include "advertise/synopsis/lines_hero_10.inc"
};
static struct synopsis_line lines_hero_11[3] = {
#include "advertise/synopsis/lines_hero_11.inc"
};
static struct synopsis_line lines_hero_12[3] = {
#include "advertise/synopsis/lines_hero_12.inc"
};
static struct synopsis_line lines_hero_14[4] = {
#include "advertise/synopsis/lines_hero_14.inc"
};
static struct synopsis_line lines_hero_16[6] = {
#include "advertise/synopsis/lines_hero_16.inc"
};
static struct synopsis_line lines_hero_18[5] = {
#include "advertise/synopsis/lines_hero_18.inc"
};
static struct synopsis_line lines_hero_20[6] = {
#include "advertise/synopsis/lines_hero_20.inc"
};
static struct synopsis_line lines_hero_22[5] = {
#include "advertise/synopsis/lines_hero_22.inc"
};
static struct synopsis_line lines_hero_23[3] = {
#include "advertise/synopsis/lines_hero_23.inc"
};
static struct synopsis_line lines_hero_25[5] = {
#include "advertise/synopsis/lines_hero_25.inc"
};
static struct synopsis_line lines_hero_26[4] = {
#include "advertise/synopsis/lines_hero_26.inc"
};
static struct synopsis_line lines_hero_28[5] = {
#include "advertise/synopsis/lines_hero_28.inc"
};
static struct synopsis_line lines_hero_30[5] = {
#include "advertise/synopsis/lines_hero_30.inc"
};
static struct synopsis_line lines_hero_32[4] = {
#include "advertise/synopsis/lines_hero_32.inc"
};
static struct synopsis_line lines_hero_33[4] = {
#include "advertise/synopsis/lines_hero_33.inc"
};
static struct synopsis_line lines_hero_35[5] = {
#include "advertise/synopsis/lines_hero_35.inc"
};
static struct synopsis_line lines_hero_37[4] = {
#include "advertise/synopsis/lines_hero_37.inc"
};
static struct synopsis_line lines_hero_38[4] = {
#include "advertise/synopsis/lines_hero_38.inc"
};
static struct synopsis_line lines_hero_41[3] = {
#include "advertise/synopsis/lines_hero_41.inc"
};
static struct synopsis_line lines_dark_1[6] = {
#include "advertise/synopsis/lines_dark_1.inc"
};
static struct synopsis_line lines_dark_3[5] = {
#include "advertise/synopsis/lines_dark_3.inc"
};
static struct synopsis_line lines_dark_5[6] = {
#include "advertise/synopsis/lines_dark_5.inc"
};
static struct synopsis_line lines_dark_7[4] = {
#include "advertise/synopsis/lines_dark_7.inc"
};
static struct synopsis_line lines_dark_9[4] = {
#include "advertise/synopsis/lines_dark_9.inc"
};
static struct synopsis_line lines_dark_10[5] = {
#include "advertise/synopsis/lines_dark_10.inc"
};
static struct synopsis_line lines_dark_12[3] = {
#include "advertise/synopsis/lines_dark_12.inc"
};
static struct synopsis_line lines_dark_14[4] = {
#include "advertise/synopsis/lines_dark_14.inc"
};
static struct synopsis_line lines_dark_16[4] = {
#include "advertise/synopsis/lines_dark_16.inc"
};
static struct synopsis_line lines_dark_18[3] = {
#include "advertise/synopsis/lines_dark_18.inc"
};
static struct synopsis_line lines_dark_19[6] = {
#include "advertise/synopsis/lines_dark_19.inc"
};
static struct synopsis_line lines_dark_21[4] = {
#include "advertise/synopsis/lines_dark_21.inc"
};
static struct synopsis_line lines_dark_22[3] = {
#include "advertise/synopsis/lines_dark_22.inc"
};
static struct synopsis_line lines_dark_25[4] = {
#include "advertise/synopsis/lines_dark_25.inc"
};
static struct synopsis_line lines_dark_26[3] = {
#include "advertise/synopsis/lines_dark_26.inc"
};
static struct synopsis_line lines_dark_28[4] = {
#include "advertise/synopsis/lines_dark_28.inc"
};
static struct synopsis_line lines_dark_30[5] = {
#include "advertise/synopsis/lines_dark_30.inc"
};
static struct synopsis_line lines_dark_31[3] = {
#include "advertise/synopsis/lines_dark_31.inc"
};
static struct synopsis_line lines_dark_33[5] = {
#include "advertise/synopsis/lines_dark_33.inc"
};
static struct synopsis_line lines_dark_35[3] = {
#include "advertise/synopsis/lines_dark_35.inc"
};
static struct synopsis_line lines_dark_37[4] = {
#include "advertise/synopsis/lines_dark_37.inc"
};
static struct synopsis_line lines_dark_39[5] = {
#include "advertise/synopsis/lines_dark_39.inc"
};
static struct synopsis_line lines_last_1[7] = {
#include "advertise/synopsis/lines_last_1.inc"
};
static struct synopsis_line lines_last_3[4] = {
#include "advertise/synopsis/lines_last_3.inc"
};
static struct synopsis_line lines_last_5[5] = {
#include "advertise/synopsis/lines_last_5.inc"
};
/* each story's sets, in the order of their steps, as synopsis_select
 * requires */
KEPT static struct synopsis_set sets[50] = {
#include "advertise/synopsis/sets.inc"
};

static void synopsis_lines_make(int set)
{
    Sint32 i;

    for (i = 0; i < sets[set].count; i++) {
        if (synopsis_sprites[i] == NULL) {
            synopsis_sprites[i] =
                message_sprite_create(sets[set].lines[i].message);
        }
    }
}

static void synopsis_lines_free(void)
{
    Sint32 i;

    for (i = 0; i < SPRITES; i++) {
        if (synopsis_sprites[i] != NULL) {
            text_sprite_destroy(synopsis_sprites[i]);
            synopsis_sprites[i] = NULL;
        }
    }
}

static void synopsis_step(struct task *task)
{
    synopsis_work.fading = FALSE;
    if (synopsis_work.shown) {
        if (menu_approach(&synopsis_state.fade, 1.0F, FADE_STEP)) {
            synopsis_work.fading = TRUE;
        }
    } else if (menu_approach(&synopsis_state.fade, 0.0F, FADE_STEP)) {
        synopsis_work.fading = TRUE;
    }
    if (synopsis_work.scrolling) {
        synopsis_state.offset -= SCROLL_STEP;
        synopsis_lines_make(synopsis_work.set);
    } else {
        synopsis_state.offset = 0.0F;
        synopsis_lines_free();
        synopsis_work.done = FALSE;
    }
}

static void synopsis_draw(struct task *task)
{
    Float top;
    Sint32 i;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (synopsis_state.fade != 0.0F) {
        menu_fade_begin(synopsis_state.fade);
        menu_backdrop_draw(BACKDROP_TEXTURE,
                           backdrops[synopsis_work.character].texlist,
                           BACKDROP_PRIORITY, 0);
        menu_tint_end();
    }
    if (synopsis_work.scrolling) {
        NJS_POINT3 pos = {LINES_X, SCREEN_HEIGHT, LINES_Z};

        top = pos.y + synopsis_state.offset;
        for (i = 0; i < sets[synopsis_work.set].count; i++) {
            pos.y = top + (Float)sets[synopsis_work.set]
                              .lines[i]
                              .y[game_state.voice_language];
            /* @hack type: the place an NJS_POINT3 read as an NJS_POINT2,
             * where an NJS_POINT2 would do */
            text_sprite_draw_text_at(synopsis_sprites[i], (NJS_POINT2 *)&pos,
                                     1.0F);
            if (pos.y < VOICE_Y && pos.y >= VOICE_Y - SCROLL_STEP) {
                voice_play_priority_2(sets[synopsis_work.set].lines[i].voice);
            }
        }
        if (pos.y < GONE_Y) {
            synopsis_work.done = TRUE;
        }
    }
}

static void synopsis_destroy(struct task *task)
{
    synopsis_release();
    synopsis_lines_free();
}

void synopsis_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, synopsis_step, "arasujiExec");
    task->draw = synopsis_draw;
    task->destructor = synopsis_destroy;
    synopsis_task = task;
    byte = (Uint8 *)&synopsis_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(synopsis_work); left != 0; left--) {
        *byte++ = 0;
    }
    synopsis_reset();
    byte = (Uint8 *)synopsis_sprites;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(synopsis_sprites); left != 0; left--) {
        *byte++ = 0;
    }
}

void synopsis_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = synopsis_work.loaded;
    byte = (Uint8 *)&synopsis_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(synopsis_work); left != 0; left--) {
        *byte++ = 0;
    }
    synopsis_state = state_start;
    synopsis_work.loaded = loaded;
}

void synopsis_end(void)
{
    if (synopsis_task != NULL) {
        task_destroy(synopsis_task);
        synopsis_task = NULL;
    }
}

void synopsis_load(void)
{
    if (!synopsis_work.loaded) {
        texture_load_pvm(backdrops[synopsis_work.character].name,
                         backdrops[synopsis_work.character].texlist);
        synopsis_work.loaded = TRUE;
    }
}

void synopsis_release(void)
{
    if (synopsis_work.loaded) {
        njReleaseTexture(backdrops[synopsis_work.character].texlist);
        synopsis_work.loaded = FALSE;
    }
}

void synopsis_select(int story, int step)
{
    Sint32 found;
    Sint32 i;

    /* @hack spelling: the flag picked by an if, where the comparison stored
     * would do */
    if (step != 0) {
        synopsis_state.from_start = FALSE;
    } else {
        synopsis_state.from_start = TRUE;
    }
#ifdef NONMATCHING
    /* GCC can't see that every path sets found. story is a STORY_ value, as
     * synopsis.h requires */
    found = HERO_FIRST_SET;
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (story) {
    case STORY_HERO:
        found = HERO_FIRST_SET;
        break;
    case STORY_DARK:
        found = DARK_FIRST_SET;
        break;
    case STORY_LAST:
        found = LAST_FIRST_SET;
        break;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    for (i = found; sets[i].story >= 0; i++) {
#ifdef NONMATCHING
        /* The disc-free registry has no lines. Continue into the story as
         * the synopsis screen does at its first step. */
        if (sets[i].count == 0) {
            synopsis_state.from_start = TRUE;
            break;
        }
#endif
        /* @hack spelling: the set's story and step tested in ifs of their
         * own, not joined by && */
        if (story == sets[i].story) {
            if (sets[i].step <= step) {
                found = i;
            }
        }
    }
    synopsis_work.set = found;
    synopsis_work.character = sets[found].character;
    if (found == LAST_FIRST_SET) {
        synopsis_work.character = PLAYER_KIND_AMY;
    }
}

Bool synopsis_from_start(void)
{
    return synopsis_state.from_start;
}
