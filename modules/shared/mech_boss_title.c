/* the mech bosses' title: the boss's name, spelled a letter at a time over a
 * banner. Big Foot, Hot Shot and Flying Dog show it as their fight starts */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_sounds.h"
#include "shared/mech_boss_title.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"

/* the letters of grobo_title_moji_texlist, in its order, each named for its
 * texture */
enum title_letter_code {
    LETTER_ONE,
    LETTER_THREE,
    LETTER_SIX,
    LETTER_B,
    LETTER_D,
    LETTER_F,
    LETTER_G,
    LETTER_H,
    LETTER_I,
    LETTER_L,
    LETTER_N,
    LETTER_O,
    LETTER_R,
    LETTER_S,
    LETTER_T,
    LETTER_Y,
    LETTER_XT,
    LETTER_XX,
    LETTER_A,
    LETTER_SPACE, /* draws nothing */
    LETTER_END /* ends a name, and draws nothing */
};

/* a letter of a name, and the steps before it shows */
struct title_letter {
    Sint8 code; /* an enum title_letter_code */
    Uint8 wait;
};

/* a letter's width. the next letter draws width + LETTER_GAP to its right */
struct title_letter_width {
    Sint32 unk_00; /* the entry's own index, which nothing reads */
    Float width;
};

/* a part's work, laid over the struct task_work that task_create_child
 * allocates */
struct title_work {
    Sint8 shown; /* the letters of the name shown so far */
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 wait; /* the steps until the next letter shows */
    struct title_letter *name;
    struct title_letter *next; /* the letter to show next */
    NJS_TEXLIST *texlist;
    NJS_POINT3 pos; /* the part's top left on the screen, and its depth */
    /* scroll is the line of the banner's texture at the banner's top. it
     * gains 1 a step. scroll_low runs BANNER_HEIGHT ahead of it */
    Float scroll;
    Float scroll_low;
};

/* the row keeps its alpha in scroll, and what the alpha gains a step in
 * scroll_low */
#define ROW_ALPHA(work) ((work)->scroll)
#define ROW_FADE(work) ((work)->scroll_low)

#define WORK(task) ((struct title_work *)(task)->work)

/* the banner's STRIPS strips, from the top, which its draw takes in turn */
enum banner_strip { STRIP_TOP, STRIP_MIDDLE, STRIP_LOW };
#define STRIPS 3

/* each part's top left, from the title's */
#define BANNER_X 128.0F
#define BANNER_Y 55.0F
#define ROW_X 8.0F
#define ROW_Y 45.0F
#define NAME_X 16.0F
#define NAME_Y 16.0F
/* the banner shows BANNER_HEIGHT lines of its texture. its scroll starts at
 * BANNER_START, and the banner ends once scroll passes BANNER_END */
#define BANNER_HEIGHT 78
#define BANNER_START (-(Float)BANNER_HEIGHT)
#define BANNER_END 256.0F
/* STRIP_TOP fades over bands of FADE_LINES lines. STRIP_LOW is LOW_LINES
 * lines high */
#define FADE_LINES 10
#define FADE_PER_LINE 0.2F
#define LOW_LINES 8
#define LOW_FADE_PER_LINE 0.1F
#define ROW_TEXTURES 8
#define ROW_SPACING 32.0F
#define ROW_FADE_STEP 0.008F
#define LETTER_GAP 2.0F
#define BANNER_SOUND_STEPS 2
#define BANNER_VOLUME (-60)
#define BANNER_PAN 60
#define LETTER_VOLUME (-10)
#define LETTER_PAN 30

static Float dat_mech_boss_title_0[10] = {
    3.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
};
/* clang-format off */
static struct title_letter_width title_letter_widths[] = {
    {LETTER_ONE, 19.0F}, {LETTER_THREE, 22.0F}, {LETTER_SIX, 22.0F},
    {LETTER_B, 16.0F},   {LETTER_D, 17.0F},     {LETTER_F, 13.0F},
    {LETTER_G, 17.0F},   {LETTER_H, 15.0F},     {LETTER_I, 5.0F},
    {LETTER_L, 13.0F},   {LETTER_N, 15.0F},     {LETTER_O, 17.0F},
    {LETTER_R, 14.0F},   {LETTER_S, 14.0F},     {LETTER_T, 15.0F},
    {LETTER_Y, 15.0F},   {LETTER_XT, 7.0F},     {LETTER_XX, 13.0F},
    {LETTER_A, 25.0F},   {LETTER_SPACE, 7.0F},
};
/* clang-format on */
/* the names, F-6t BIG FOOT, B-3x HOT SHOT and R-1/A FLYING DOG */
/* clang-format off */
static struct title_letter title_big_foot[] = {
    {LETTER_F, 60}, {LETTER_SIX, 25}, {LETTER_XT, 25}, {LETTER_SPACE, 55},
    {LETTER_B, 5},  {LETTER_I, 15},   {LETTER_G, 15},  {LETTER_SPACE, 40},
    {LETTER_F, 15}, {LETTER_O, 15},   {LETTER_O, 15},  {LETTER_T, 15},
    {LETTER_END, 20},
};
static struct title_letter title_hot_shot[] = {
    {LETTER_B, 60}, {LETTER_THREE, 25}, {LETTER_XX, 25}, {LETTER_SPACE, 55},
    {LETTER_H, 5},  {LETTER_O, 15},     {LETTER_T, 15},  {LETTER_SPACE, 40},
    {LETTER_S, 15}, {LETTER_H, 15},     {LETTER_O, 15},  {LETTER_T, 15},
    {LETTER_END, 20},
};
static struct title_letter title_flying_dog[] = {
    {LETTER_R, 60}, {LETTER_ONE, 25}, {LETTER_A, 25}, {LETTER_SPACE, 60},
    {LETTER_F, 5},  {LETTER_L, 15},   {LETTER_Y, 15},
    {LETTER_I, 15}, {LETTER_N, 15},   {LETTER_G, 15}, {LETTER_SPACE, 45},
    {LETTER_D, 20}, {LETTER_O, 20},   {LETTER_G, 20}, {LETTER_END, 20},
};
/* clang-format on */

static void title_banner_step(struct task *task);
static void title_banner_draw(struct task *task);
static void title_banner_destroy(struct task *task);
static void title_row_destroy(struct task *task);
static void title_row_step(struct task *task);
static void title_row_draw(struct task *task);
static void title_name_destroy(struct task *task);
static void title_name_step(struct task *task);
static void title_name_draw(struct task *task);
static void title_destroy(struct task *task);
static void title_step(struct task *task);

static void title_banner_step(struct task *task)
{
    struct title_work *work = WORK(task);

    if (game_state.time_stopped || game_state.unk_09 ||
        game_state.mini_event_running) {
        return;
    }
    work->scroll += 1.0F;
    work->scroll_low += 1.0F;
    if (work->scroll > BANNER_END) {
        task_kill(task);
    }
    sound_play_for(MECH_BOSS_SOUND_TITLE_BANNER, task, MECH_BOSS_SOUND_PRIORITY,
                   0, BANNER_SOUND_STEPS);
    sound_set_volume(task, SOUND_ANY_ID, BANNER_VOLUME);
    sound_set_pan(task, SOUND_ANY_ID, BANNER_PAN);
}

static void title_banner_draw(struct task *task)
{
    struct title_work *work = WORK(task);
    NJS_TEXANIM banner_cell = {
#include "shared/mech_boss_title/banner_cell.inc"
    };
    /* @hack type: sprite, an array of one, where one record would do */
    NJS_SPRITE sprite[1] = {{{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, NULL, NULL}};
    /* @hack redundant: a second pointer to the work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *unused = task->work;
    Float alpha;
    Float top;
    Float bottom;
    Sint16 line;
    Sint16 height;
    Sint8 band;
    Sint8 i;

    if (game_state.mini_event_running) {
        return;
    }
    njFogDisable();
    line = work->scroll;
    sprite->tanim = &banner_cell;
    sprite->p = work->pos;
    /* @hack spelling: the texture list set inside the call, where a
     * statement of its own would do */
    njSetTexture(sprite->tlist = work->texlist);
    for (i = 0; i < STRIPS; i++) {
        alpha = 1.0F;
        switch (i) {
        case STRIP_TOP:
            sprite->tanim->v1 = line;
            /* @bug uninitialized: top is unset while line + FADE_LINES is
             * below 0, as it is for the banner's first 68 steps. height
             * then takes FR12 as the draw found it (0x188 of the object's
             * code) */
#ifdef NONMATCHING
            /* this side repairs the tag above: the strip has no height until
             * line + FADE_LINES reaches 0 */
            top = 0.0F;
#endif
            if (line + FADE_LINES >= 0) {
                band = line % FADE_LINES;
                top = FADE_LINES - band;
                alpha = 1.0F - (band * FADE_PER_LINE);
                if (alpha < 0.0F) {
                    alpha = 0.0F;
                }
            }
            band = (line + 128) % FADE_LINES;
            bottom = band;
            height = top;
            sprite->p.y += height * 0.5F;
            sprite->tanim->v2 = sprite->tanim->v1 + height;
            sprite->tanim->sy = height;
            sprite->tanim->cy = height * 0.5F;
            break;
        case STRIP_MIDDLE:
            sprite->tanim->v1 = sprite->tanim->v2;
            height = BANNER_HEIGHT - top - bottom;
            sprite->p.y += sprite->tanim->cy;
            sprite->p.y += height * 0.5F;
            sprite->tanim->v2 = sprite->tanim->v1 + height;
            sprite->tanim->sy = height;
            sprite->tanim->cy = height * 0.5F;
            break;
        case STRIP_LOW:
            sprite->tanim->v1 = sprite->tanim->v2;
            height = LOW_LINES;
            sprite->p.y += sprite->tanim->cy;
            sprite->p.y += height * 0.5F;
            sprite->tanim->v2 = sprite->tanim->v1 + height;
            sprite->tanim->sy = height;
            sprite->tanim->cy = height * 0.5F;
            line = work->scroll_low;
            if (line + FADE_LINES < 256) {
                alpha = bottom * LOW_FADE_PER_LINE;
                if (alpha > 1.0F) {
                    alpha = 1.0F;
                }
            }
            break;
        }
        constant_material_set(alpha, 1.0F, 1.0F, 1.0F);
        njDrawSprite2D(sprite, 0, work->pos.z,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    }
    njFogEnable();
}

static void title_banner_destroy(struct task *task)
{
    /* @hack type: the key held as an integer, where task passed in place
     * would do */
    Sint32 key = (Sint32)task;

    sound_stop_key((void *)key);
}

static struct task *title_banner_create(struct task *parent, NJS_POINT3 *pos,
                                        NJS_TEXLIST *texlist)
{
    struct task *task;
    struct title_work *work;

    if (parent != NULL) {
        task = task_create_child(TASK_MOTION | TASK_WORK, title_banner_step,
                                 parent);
        if (task == NULL) {
            return NULL;
        }
        work = WORK(task);
        work->pos = *pos;
        work->pos.x += BANNER_X;
        work->pos.y += BANNER_Y;
        work->texlist = texlist;
        work->scroll = BANNER_START;
        work->scroll_low = 0.0F;
        task->destructor = title_banner_destroy;
        task->draw = title_banner_draw;
        return task;
    }
#ifdef NONMATCHING
    /* GCC can't see that no path gets here: mech_boss_title_create, the one
     * caller, passes its own task */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void title_row_destroy(struct task *task)
{
}

static void title_row_step(struct task *task)
{
    struct title_work *work = WORK(task);

    if (game_state.time_stopped || game_state.unk_09 ||
        game_state.mini_event_running) {
        return;
    }
    if (ROW_ALPHA(work) < 1.0F) {
        ROW_ALPHA(work) += ROW_FADE(work);
        if (ROW_ALPHA(work) > 1.0F) {
            ROW_ALPHA(work) = 1.0F;
        }
    }
}

static void title_row_draw(struct task *task)
{
    struct title_work *work = WORK(task);
    NJS_TEXANIM row_cell = {
#include "shared/mech_boss_title/row_cell.inc"
    };
    /* @hack type: sprite, an array of one, where one record would do */
    NJS_SPRITE sprite[1] = {{{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, NULL, NULL}};
    /* @hack redundant: a second pointer to the work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *unused = task->work;
    NJS_TEXLIST *texlist;
    Sint8 i;

    if (game_state.mini_event_running) {
        return;
    }
    njFogDisable();
    sprite->tanim = &row_cell;
    sprite->p = work->pos;
    texlist = work->texlist;
    sprite->tlist = work->texlist;
    for (i = 0; i < ROW_TEXTURES; i++) {
        constant_material_set(ROW_ALPHA(work), 1.0F, 1.0F, 1.0F);
        njSetTexture(texlist);
        njDrawSprite2D(sprite, 0, work->pos.z,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        sprite->tanim->texid++;
        sprite->p.x += ROW_SPACING;
    }
    njFogEnable();
}

static struct task *title_row_create(struct task *parent, NJS_POINT3 *pos,
                                     NJS_TEXLIST *texlist)
{
    struct task *task;
    struct title_work *work;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, title_row_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = WORK(task);
        work->pos = *pos;
        work->pos.y += ROW_Y;
        work->pos.x += ROW_X;
        work->texlist = texlist;
        ROW_ALPHA(work) = 0.0F;
        ROW_FADE(work) = ROW_FADE_STEP;
        task->draw = title_row_draw;
        task->destructor = title_row_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* GCC can't see that no path gets here, as in title_banner_create */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void title_name_destroy(struct task *task)
{
}

static void title_name_step(struct task *task)
{
    struct title_work *work = WORK(task);
    struct title_letter *letter;

    if (game_state.time_stopped || game_state.unk_09 ||
        game_state.mini_event_running) {
        return;
    }
    letter = work->next;
    if (letter->code == LETTER_END && work->wait == 0) {
        return;
    }
    if (--work->wait != 0) {
        return;
    }
    if (letter->code == LETTER_END) {
        return;
    }
    if (letter->code != LETTER_SPACE) {
        sound_play(MECH_BOSS_SOUND_TITLE_LETTER, NULL, 0, 0);
        sound_set_volume(NULL, MECH_BOSS_SOUND_TITLE_LETTER, LETTER_VOLUME);
        sound_set_pan(NULL, MECH_BOSS_SOUND_TITLE_LETTER, LETTER_PAN);
    }
    letter++;
    work->shown++;
    work->next = letter;
    work->wait = letter->wait;
}

static void title_name_draw(struct task *task)
{
    struct title_work *work = WORK(task);
    NJS_TEXANIM name_cell = {
#include "shared/mech_boss_title/name_cell.inc"
    };
    /* @hack type: sprite, an array of one, where one record would do */
    NJS_SPRITE sprite[1] = {{{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, NULL, NULL}};
    /* @hack spelling: letter declared after the sprite, not first */
    struct title_letter *letter = work->name;
    NJS_TEXLIST *texlist;
    Sint8 shown;
    Sint8 i;

    if (game_state.mini_event_running) {
        return;
    }
    njFogDisable();
    shown = work->shown;
    sprite->tanim = &name_cell;
    sprite->p = work->pos;
    texlist = work->texlist;
    sprite->tlist = texlist;
    for (i = 0; i < shown; i++, letter++) {
        if (letter->code != LETTER_SPACE && letter->code != LETTER_END) {
            constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
            sprite->tanim->texid = letter->code;
            njSetTexture(texlist);
            njDrawSprite2D(sprite, 0, work->pos.z,
                           NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        }
        sprite->p.x += title_letter_widths[letter->code].width + LETTER_GAP;
    }
    njFogEnable();
}

static struct task *title_name_create(struct task *parent, NJS_POINT3 *pos,
                                      struct title_letter *name,
                                      NJS_TEXLIST *texlist)
{
    struct task *task;
    struct title_work *work;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, title_name_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = WORK(task);
    work->pos = *pos;
    work->pos.y += NAME_Y;
    work->pos.x += NAME_X;
    work->name = name;
    work->texlist = texlist;
    work->next = name;
    work->shown = 0;
    work->wait = name->wait;
    task->draw = title_name_draw;
    task->destructor = title_name_destroy;
    /* @bug uninitialized: returns no value where the name's task is made.
     * mech_boss_title_create tests the result, by the TST R0,R0 at 0x8f8 of
     * the object's code. the load of the first letter's wait leaves it in R0
     * through the RTS at 0x81a. each name's first wait isn't 0, so the test
     * finds the name made */
#ifdef NONMATCHING
    /* this side keeps the test's result: the name was made */
    return task;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void title_destroy(struct task *task)
{
}

static void title_step(struct task *task)
{
}

struct task *mech_boss_title_create(struct task *parent, Sint8 boss,
                                    NJS_POINT3 *pos)
{
    struct task *task;
    struct title_work *work;
    struct title_letter *name;

    if (parent == NULL) {
#ifdef NONMATCHING
        /* for a null parent the image returns boss, which the call keeps in
         * R0. this side returns null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task = task_create_child(TASK_WORK, title_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = WORK(task);
    work->pos = *pos;
#ifdef NONMATCHING
    /* GCC can't see that every path sets name: boss is one of enum
     * mech_boss, as mech_boss_title.h requires */
    name = title_big_foot;
#endif
    switch (boss) {
    case MECH_BOSS_BIG_FOOT:
        work->texlist = &bigfoot_title_texlist;
        name = title_big_foot;
        break;
    case MECH_BOSS_HOT_SHOT:
        work->texlist = &hotshot_title_texlist;
        name = title_hot_shot;
        break;
    case MECH_BOSS_FLYING_DOG:
        work->texlist = &fdog_title_texlist;
        name = title_flying_dog;
        break;
    }
    /* @hack implicit: a bare return where a part can't be made, whose result
     * SHC's code leaves as the part's null, where return NULL would do */
    if (title_name_create(task, &work->pos, name, &grobo_title_moji_texlist) ==
        NULL) {
        task_kill(task);
#ifdef NONMATCHING
        /* keeps the value the tag names: the part's null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    if (title_row_create(task, &work->pos, work->texlist) == NULL) {
        task_kill(task);
#ifdef NONMATCHING
        /* keeps the value the tag names: the part's null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    if (title_banner_create(task, &work->pos, work->texlist) == NULL) {
        task_kill(task);
#ifdef NONMATCHING
        /* keeps the value the tag names: the part's null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task->destructor = title_destroy;
    return task;
}
