/* the menus' shared digits. the task's step and draw are empty */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "save/save.h"
#include "system/task.h"
#include "system/texture.h"

#include "advertise/share_data/text.inc"

Sint32 share_data_reset_word;
struct share_data_work share_data_work;

#define TASK_LEVEL 2
#define TEXTURES 2
#define CELLS 14
/* the pixels from one place of a number to the next, at scale 1 */
#define PLACE_WIDTH 23.0F
/* the places of a time's numbers, and of a clock's hour. the draws pass them
 * negated, so the zeros that lead draw */
#define TIME_PLACES 2
#define HOUR_PLACES 3
/* how far left of *pos a clock's hour starts, in pixels at any scale */
#define CLOCK_SHIFT 12.0F

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw as share_data_sprite_draw does, but scale cell's width alone. nothing
 * calls share_data_sprite_draw_wide, and no header declares it. it is
 * external only because SHC leaves out a static function that nothing
 * calls */
void share_data_sprite_draw_wide(Int cell, Float scale, const NJS_POINT3 *pos,
                                 Int attr);
/* destroy the task, where share_data_create made one that this hasn't
 * destroyed. nothing calls share_data_kill, and no header declares it. it is
 * external only because SHC leaves out a static function that nothing
 * calls */
void share_data_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

static Int initial_reset_word = 0;
/* the task share_data_create made, or NULL */
static struct task *share_data_task = NULL;
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/share_data/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/share_data/texlist.inc"
};
/* the cells share_data.h names. share_data_create sets each cell's center,
 * cx and cy, to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/share_data/cells.inc"
};

static void share_data_destroy(struct task *task);

static Int decimal_power(Int exponent)
{
    Int power = 1;

    while (exponent--) {
        /* @hack spelling: power = power * 10, where power *= 10 would do */
        power = power * 10;
    }
    return power;
}

void share_data_sprite_draw(Int cell, Float scale, const NJS_POINT3 *pos,
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

void share_data_sprite_draw_wide(Int cell, Float scale, const NJS_POINT3 *pos,
                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

void share_data_sprite_draw_tall(Int cell, Float scale, const NJS_POINT3 *pos,
                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = 1.0F;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

void share_data_number_draw(const NJS_POINT3 *pos, Int value, Int digits,
                            Float scale)
{
    Int count;
    Int rest;
    Bool started;
    Int digit;
    Int divisor;
    Int remaining;
    Int number;
    NJS_POINT3 point;
    Float advance;

    started = FALSE;
    rest = value;
    count = 1;
    /* @hack spelling: the quotient stored inside the test, where a
     * statement of its own would do */
    while ((rest = rest / 10)) {
        ++count;
    }
    point = *pos;
    advance = scale * PLACE_WIDTH;
    if (digits == 0) {
        divisor = decimal_power(count - 1);
        for (number = value; count--; divisor /= 10) {
            digit = number / divisor;
            /* @hack redundant: !digit beside !count or !remaining in each
             * test of share_data_number_draw that skips zeros, which the
             * test of digit after it makes needless */
            if (started || (!digit && !count) || digit) {
                share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
                point.x += advance;
                started = TRUE;
            }
            /* @hack spelling: number = number - digit * divisor throughout
             * share_data_number_draw, where number -= digit * divisor would
             * do */
            number = number - (digit * divisor);
        }
    } else if (digits > 0) {
        divisor = decimal_power(digits - 1);
        for (number = value, remaining = digits; remaining--; divisor /= 10) {
            digit = number / divisor;
            if (started || (!digit && !remaining) || digit) {
                share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
                point.x += advance;
                started = TRUE;
            }
            number = number - (digit * divisor);
        }
    } else {
        divisor = decimal_power(-digits - 1);
        for (number = value, remaining = -digits; remaining--; divisor /= 10) {
            digit = number / divisor;
            share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
            point.x += advance;
            number = number - (digit * divisor);
        }
    }
}

void share_data_time_draw(const NJS_POINT3 *pos, const struct save_time *time,
                          Float scale)
{
    NJS_POINT3 point;

    point = *pos;
    share_data_number_draw(&point, time->minute, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, time->second, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, time->hundredths, -TIME_PLACES, scale);
}

void share_data_clock_draw(const NJS_POINT3 *pos,
                           const struct share_data_clock *clock, Float scale)
{
    NJS_POINT3 point;

    point = *pos;
    point.x -= CLOCK_SHIFT;
    share_data_number_draw(&point, clock->hour, -HOUR_PLACES, scale);
    point.x += HOUR_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, clock->minute, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, clock->second, -TIME_PLACES, scale);
}

void share_data_time_draw_short(const NJS_POINT3 *pos,
                                const struct save_time *time, Float scale)
{
    NJS_POINT3 point;

    point = *pos;
    share_data_number_draw(&point, time->minute, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, time->second, -TIME_PLACES, scale);
}

void share_data_time_draw_alt(const NJS_POINT3 *pos,
                              const struct save_time *time, Float scale)
{
    NJS_POINT3 point;

    point = *pos;
    share_data_number_draw(&point, time->minute, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_ALT_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, time->second, -TIME_PLACES, scale);
    point.x += TIME_PLACES * PLACE_WIDTH * scale;
    share_data_sprite_draw(SHARE_DATA_ALT_SEPARATOR_CELL, scale, &point,
                           MENU_SPRITE_ATTR);
    point.x += PLACE_WIDTH * scale;
    share_data_number_draw(&point, time->hundredths, -TIME_PLACES, scale);
}

static void share_data_step(struct task *task)
{
}

static void share_data_draw(struct task *task)
{
}

static void share_data_destroy(struct task *task)
{
    share_data_release();
}

void share_data_create(void)
{
    struct task *task;
    /* @hack spelling: the cells through cell, where their own name would
     * do */
    NJS_TEXANIM *cell;
    Int i;
    Uint8 *byte;

    task =
        task_create(TASK_WORK, TASK_LEVEL, share_data_step, "advShareDataExec");
    task->draw = share_data_draw;
    task->destructor = share_data_destroy;
    share_data_task = task;
    byte = (Uint8 *)&share_data_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (i = sizeof(share_data_work); i != 0; --i) {
        *byte++ = 0;
    }
    share_data_reset();
    /* @hack redundant: a do loop that runs once */
    do {
        cell = cells;
    } while (0);
    for (i = 0; i < CELLS; ++i) {
        cell[i].cx = cell[i].sx / 2;
        cell[i].cy = cell[i].sy / 2;
    }
}

void share_data_reset(void)
{
    Int loaded = share_data_work.loaded;
    Int i;
    Uint8 *byte;

    byte = (Uint8 *)&share_data_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (i = sizeof(share_data_work); i != 0; --i) {
        *byte++ = 0;
    }
    share_data_reset_word = initial_reset_word;
    share_data_work.loaded = loaded;
}

void share_data_kill(void)
{
    if (share_data_task) {
        task_destroy(share_data_task);
        share_data_task = NULL;
    }
}

void share_data_load(void)
{
    if (!share_data_work.loaded) {
        texture_load_pvm(SHARE_DATA_ARCHIVE, &texlist);
        share_data_work.loaded = TRUE;
    }
}

void share_data_release(void)
{
    if (!share_data_work.loaded) {
        return;
    }
    njReleaseTexture(&texlist);
    share_data_work.loaded = FALSE;
}

void share_data_number_draw_right(const NJS_POINT3 *pos, Int value, Int digits,
                                  Float scale)
{
    Int count;
    Int rest;
    Bool started;
    Int digit;
    Int divisor;
    Int remaining;
    Int number;
    Int last;
    NJS_POINT3 point;
    Float advance;

    started = FALSE;
    rest = value;
    count = 1;
    /* @hack spelling: the quotient stored inside the test, where a
     * statement of its own would do */
    while ((rest = rest / 10)) {
        ++count;
    }
    point = *pos;
    /* @hack spelling: the two assignments in one statement, where two
     * statements would do */
    last = count - 1, advance = scale * PLACE_WIDTH;
    if (digits == 0) {
        divisor = decimal_power(last);
        number = value;
        point.x -= last * scale * PLACE_WIDTH;
        for (; count--; divisor /= 10) {
            digit = number / divisor;
            /* @hack redundant: !digit beside !count or !remaining in each
             * test of share_data_number_draw_right that skips zeros, which
             * the test of digit after it makes needless */
            if (started || (!digit && !count) || digit) {
                share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
                started = TRUE;
            }
            point.x += advance;
            /* @hack spelling: number = number - digit * divisor throughout
             * share_data_number_draw_right, where number -= digit * divisor
             * would do */
            number = number - (digit * divisor);
        }
    } else if (digits > 0) {
        divisor = decimal_power(digits - 1);
        number = value;
        remaining = digits;
        point.x -= (remaining - 1) * scale * PLACE_WIDTH;
        for (; remaining--; divisor /= 10) {
            digit = number / divisor;
            if (started || (!digit && !remaining) || digit) {
                share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
                started = TRUE;
            }
            point.x += advance;
            number = number - (digit * divisor);
        }
    } else {
        divisor = decimal_power(-digits - 1);
        number = value;
        remaining = -digits;
        point.x -= (remaining - 1) * scale * PLACE_WIDTH;
        for (; remaining--; divisor /= 10) {
            digit = number / divisor;
            share_data_sprite_draw(digit, scale, &point, MENU_SPRITE_ATTR);
            point.x += advance;
            number = number - (digit * divisor);
        }
    }
}
