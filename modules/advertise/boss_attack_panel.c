/* the boss attack's panel: the courses, and the best times and emblem of
 * the course under the cursor. its task draws the panel as boss_attack.h's
 * menu sets the work */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/boss_attack_panel.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "hud/hud.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "system/task.h"
#include "system/texture.h"

#include "advertise/boss_attack_panel/text.inc"

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define BACKDROP_PRIORITY (-10000.0F)
/* the message the panel shows once its fade passes MESSAGE_LEVEL */
#define MESSAGE 70
#define MESSAGE_LEVEL 0.98F

#define BACKDROP_TEXTURE 0
#define TITLE_X 100.0F
#define TITLE_Y 50.0F
#define WINDOW_X 450.0F
#define WINDOW_Y 260.0F
/* the sprites' priorities: the title's and the window's, the list's and the
 * times', and the emblem's */
#define PANEL_PRIORITY (-100.0F)
#define LIST_PRIORITY (-90.0F)
#define EMBLEM_PRIORITY 90.0F
/* cells. under the cursor, a course's row draws its LIT cell once its award
 * is given. it draws its DIM cell otherwise */
#define WINDOW_CELL 0
#define TITLE_FIRST_CELL 1
#define TITLE_CELLS 3
#define COURSE_0_LIT 7
#define COURSE_0_DIM 4
#define COURSE_1_LIT 8
#define COURSE_1_DIM 5
#define COURSE_2_LIT 9
#define COURSE_2_DIM 10
#define ROW_LIT_BACK 11
#define ROW_DIM_BACK 12
#define CELLS 13
/* the panel draws share_data.h's emblem EMBLEM_DRAWS times over, at one
 * place */
#define EMBLEM_DRAWS 3

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void boss_attack_panel_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* @hack type: the fade as a record of one Float, where a Float would do */
struct panel_fade {
    Float level; /* 0 while hidden, and 1 once faded in */
};
static struct panel_fade boss_attack_fade;

static NJS_POINT3 message_pos = {0.0F, 400.0F, 0.0F};
static struct panel_fade fade_start = {0.0F};
/* the task boss_attack_panel_create made, or null */
static struct task *panel_task = NULL;
static Float list_x = 150.0F;
static Float list_y = 150.0F;
static Float row_step = 35.0F;
static Float times_x = 424.0F;
static Float times_y = 252.0F;
static Float time_step = 37.0F;
static Float time_scale = 0.75F;
static Float emblem_x = 533.0F;
static Float emblem_y = 205.0F;
/* the list's row at the top of its window. the window holds every course,
 * so it stays 0 */
static Sint32 scroll_top = 0;
/* the draw sets the first two courses' lit cells by their awards */
static struct menu_row course_rows[BOSS_ATTACK_COURSES] = {
    {COURSE_0_LIT, COURSE_0_DIM},
    {COURSE_1_LIT, COURSE_1_DIM},
    {COURSE_2_LIT, COURSE_2_DIM},
};
static NJS_TEXNAME texnames[13] = {
#include "advertise/boss_attack_panel/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/boss_attack_panel/texlist.inc"
};
/* boss_attack_panel_create sets each cell's cx and cy to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/boss_attack_panel/cells.inc"
};

static void boss_attack_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                    Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void boss_attack_panel_step(struct task *task)
{
    boss_attack_work.fading = FALSE;
    if (boss_attack_work.shown) {
        if (menu_approach(&boss_attack_fade.level, 1.0F, FADE_STEP)) {
            boss_attack_work.fading = TRUE;
        }
    } else {
        if (menu_approach(&boss_attack_fade.level, 0.0F, FADE_STEP)) {
            boss_attack_work.fading = TRUE;
        }
    }
}

static void boss_attack_panel_draw(struct task *task)
{
    /* @hack spelling: the locals declared row, then pos, where another
     * order would do */
    struct menu_sprite_row row;
    NJS_POINT3 pos;
    Int i;

    pos.x = TITLE_X;
    pos.y = TITLE_Y;
    pos.z = PANEL_PRIORITY;
    if (boss_attack_fade.level != 0.0F) {
        menu_fade_begin(boss_attack_fade.level);
        row.tanim = cells;
        row.first = TITLE_FIRST_CELL;
        row.count = TITLE_CELLS;
        row.pos = &pos;
        row.draw = boss_attack_sprite_draw;
        menu_sprite_row_draw(&row);
        if (save_data.awards[BOSS_ATTACK_FIRST_AWARD + BOSS_COURSE_HERO]) {
            course_rows[BOSS_COURSE_HERO].lit = COURSE_0_LIT;
        } else {
            course_rows[BOSS_COURSE_HERO].lit = COURSE_0_DIM;
        }
        if (save_data.awards[BOSS_ATTACK_FIRST_AWARD + BOSS_COURSE_DARK]) {
            course_rows[BOSS_COURSE_DARK].lit = COURSE_1_LIT;
        } else {
            course_rows[BOSS_COURSE_DARK].lit = COURSE_1_DIM;
        }
        {
            struct menu_scroll scroll;
            struct menu_list list;

            scroll.top = &scroll_top;
            scroll.cursor = &boss_attack_work.course;
            if (save_data.awards[BOSS_ATTACK_FIRST_AWARD + BOSS_COURSE_ALL]) {
                scroll.shown = BOSS_ATTACK_COURSES;
            } else {
                scroll.shown = BOSS_ATTACK_COURSES - 1;
            }
            list.rows = course_rows;
            list.count = scroll.shown;
            list.lit_back = ROW_LIT_BACK;
            list.dim_back = ROW_DIM_BACK;
            list.language_offset = 0;
            list.pos.x = list_x;
            list.pos.y = list_y;
            list.pos.z = LIST_PRIORITY;
            list.step.x = 0.0F;
            list.step.y = row_step;
            list.step.z = 0.0F;
            list.draw = boss_attack_sprite_draw;
            menu_list_draw(&list, &scroll, 1.0F);
        }
        pos.x = WINDOW_X;
        pos.y = WINDOW_Y;
        pos.z = PANEL_PRIORITY;
        boss_attack_sprite_draw(WINDOW_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
        pos.x = times_x;
        pos.y = times_y;
        pos.z = LIST_PRIORITY;
        for (i = 0; i < SAVE_BEST; i++) {
            struct save_time time;

            /* @hack spelling: the time copied a field at a time, where a
             * copy of the record would do */
            time.minute = save_data.boss_totals[boss_attack_work.course]
                              .records[0][i]
                              .time.minute;
            time.second = save_data.boss_totals[boss_attack_work.course]
                              .records[0][i]
                              .time.second;
            time.hundredths = save_data.boss_totals[boss_attack_work.course]
                                  .records[0][i]
                                  .time.hundredths;
            share_data_time_draw(&pos, &time, time_scale);
            pos.y += time_step;
        }
        pos.x = emblem_x;
        pos.y = emblem_y;
        pos.z = EMBLEM_PRIORITY;
        for (i = 0; i < EMBLEM_DRAWS; i++) {
            if (save_data.boss_totals[boss_attack_work.course].ranks[0]) {
                share_data_sprite_draw(SHARE_DATA_EMBLEM_CELL, 1.0F, &pos,
                                       MENU_SPRITE_ATTR);
            }
        }
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                           MENU_BACKDROP_TINT);
    }
    if (boss_attack_fade.level > MESSAGE_LEVEL) {
        message_sprite_draw(MESSAGE, &message_pos, 1.0F);
    }
}

static void boss_attack_panel_destroy(struct task *task)
{
    boss_attack_panel_release();
}

void boss_attack_panel_create(void)
{
    struct task *task;
    NJS_TEXANIM *cell;
    NJS_TEXANIM *end;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, boss_attack_panel_step,
                       "bossAttackExec");
    task->draw = boss_attack_panel_draw;
    task->destructor = boss_attack_panel_destroy;
    panel_task = task;
    byte = (Uint8 *)&boss_attack_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(boss_attack_work); left != 0; left--) {
        *byte++ = 0;
    }
    boss_attack_panel_reset();
    /* @hack spelling: the cells' end set to their start and then moved on,
     * where cells + CELLS would do */
    cell = end = cells;
    end += CELLS;
    for (; cell < end; cell++) {
        cell->cx = cell->sx / 2;
        cell->cy = cell->sy / 2;
    }
}

void boss_attack_panel_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = boss_attack_work.loaded;
    byte = (Uint8 *)&boss_attack_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(boss_attack_work); left != 0; left--) {
        *byte++ = 0;
    }
    boss_attack_fade = fade_start;
    boss_attack_work.loaded = loaded;
}

void boss_attack_panel_end(void)
{
    if (panel_task) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

void boss_attack_panel_load(void)
{
    if (!boss_attack_work.loaded) {
        texture_load_pvm(BOSS_ATTACK_ARCHIVE, &texlist);
        boss_attack_work.loaded = TRUE;
    }
}

void boss_attack_panel_release(void)
{
    /* @hack spelling: the return where the work isn't loaded, where an if
     * around the release would do */
    if (!boss_attack_work.loaded) {
        return;
    }
    njReleaseTexture(&texlist);
    boss_attack_work.loaded = FALSE;
}
