/* the boss title: a boss's name on the screen as its battle starts. the name
 * appears a letter at a time, above a bar */
#ifdef NONMATCHING
/* SCHAR_MAX, by which this side bounds a title's counts */
#include <limits.h>
#endif
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "shared/boss_title.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

/* the level a title without a parent runs in */
#define TITLE_LEVEL 3

/* in the title's work's flags */
#define TITLE_HIDDEN 0x200 /* the title's tasks neither step nor draw */
#define TITLE_FADING 0x400 /* boss_title_fade has started the fade */

/* a title is a task with a child task for each letter shown, and one for the
 * bar. a child's work keeps its top left corner on the screen, in pixels, in
 * pos.x and pos.y. SCALE is the scale the child draws at, and COLOR its ARGB
 * color. the bar's alpha byte is 0, and BAR_ALPHA holds its alpha */
#define SCALE(work) ((work)->scl.x)
#define COLOR(work) (*(Uint32 *)&(work)->ang.y)

/* the title's own work. its pos.x and pos.y are where the next letter's top
 * left corner goes */
#define NEXT_LETTER(work) (*(struct boss_title_letter **)&(work)->ang.x)
#define LETTERS_SHOWN(work) ((work)->submode)
/* the steps until the next letter shows. at 0, no more letters show */
#define NEXT_WAIT(work) ((work)->tick)
/* the sound each letter plays as it appears, or NO_SOUND */
#define SOUND(work) ((work)->ang.z)
/* the steps the title has left, or 0 where only its fade ends it */
#define LIFE(work) ((work)->timer)
#define FADE_ALPHA(work) ((work)->scl.y)
#define FADE_RATE(work) ((work)->scl.z)

/* a letter's work. its index is its place among the title's letters, from 0 */
#define LETTER(work) (*(struct boss_title_letter **)&(work)->ang.x)
#define LETTER_INDEX(work) ((work)->timer)
/* the shares of its texture's width and height that the letter shows */
#define LETTER_U(work) ((work)->scl.y)
#define LETTER_V(work) ((work)->scl.z)

/* the bar's work: its pieces, and their count */
#define PIECES(work) (*(struct boss_title_letter **)&(work)->ang.x)
#define PIECE_COUNT(work) ((work)->submode)
/* the steps left before the bar fades in */
#define BAR_DELAY(work) ((work)->timer)
/* the bar's alpha, where 1 is opaque */
#define BAR_ALPHA(work) ((work)->scl.z)

#define TITLE_HELD()                                                           \
    (game_state.time_stopped || game_state.unk_09 ||                           \
     game_state.mini_event_running)
#define TITLE_UNDRAWN()                                                        \
    (game_state.mini_event_running || game_state.unk_0b == 0)
/* whether task's parent, the title's task, has TITLE_HIDDEN set. a task
 * without a parent isn't hidden */
#define PARENT_HIDDEN(task)                                                    \
    ((task)->parent != NULL && ((task)->parent->work->flags & TITLE_HIDDEN))
/* the title's first child sets Ninja's state for the sprites before it draws.
 * its last child sets bilinear filtering and enables fog after it draws */
#define FIRST_CHILD(task)                                                      \
    ((task)->parent != NULL && (task)->parent->child == (task))
#define LAST_CHILD(task)                                                       \
    ((task)->parent != NULL && (task)->parent->child->prev == (task))

#define NO_SOUND (-1)
#define BOSS_LETTER_SOUND 2
#define LETTER_SOUND 0x801e
/* the steps the first letter waits beyond its own wait */
#define FIRST_WAIT 20
#define FADE_GROWTH 1.08F
/* the bar waits BAR_DELAY_STEPS steps, then gains BAR_FADE_IN of alpha a
 * step */
#define BAR_DELAY_STEPS 60
#define BAR_FADE_IN 0.003F
/* a piece of the bar is PIECE_SIZE on a side, in pixels at a scale of 1 */
#define PIECE_SIZE 16.0F
/* The child sits at this z. Its textured quads draw at 0.9. */
#define CHILD_Z 0.2F
#define CORNERS 4
#define RGB_MASK 0x00ffffff
#define OPAQUE_ALPHA 0xff000000

/* the shown title's task and texlist, each null while no title shows */
static struct task *boss_title_task = NULL;
static NJS_TEXLIST *boss_title_texlist = NULL;

/* nothing calls boss_title_hide, boss_title_show or boss_title_set_sound.
 * none is static, since SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* hide the title, so that its tasks neither step nor draw. it returns 1, or
 * 0 where the title is hidden already, or -1 where no title shows */
int boss_title_hide(void);
/* undo boss_title_hide. it returns 1, or 0 where the title isn't hidden, or
 * -1 where no title shows */
int boss_title_show(void);
/* make id the sound the letters yet to appear play, or NO_SOUND for none. it
 * returns -1 where no title shows. where one shows, it returns no defined
 * value */
int boss_title_set_sound(Sint32 id);
/* NOLINTEND(misc-use-internal-linkage) */

static void boss_title_letter_step(struct task *task);
static void boss_title_letter_draw(struct task *task);
static void boss_title_letter_destroy(struct task *task);
static void boss_title_bar_step(struct task *task);
static void boss_title_bar_draw(struct task *task);
static void boss_title_bar_destroy(struct task *task);
static void boss_title_step(struct task *task);
static void boss_title_destroy(struct task *task);

static void boss_title_set_draw_state(void)
{
    njFogDisable();
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(boss_title_texlist);
}

/* 1 while a title shows and fades, and 0 otherwise */
static int boss_title_fading(void)
{
    struct task *task = boss_title_task;
    struct task_work *work;

    if (task != NULL) {
        work = task->work;
        if (work != NULL && (work->flags & TITLE_FADING)) {
            return 1;
        }
    }
    return 0;
}

/* the shown title's copy of its struct boss_title, or null where no title
 * shows */
static struct boss_title *boss_title_current(void)
{
    struct task *task = boss_title_task;
    struct boss_title *title;

    if (task == NULL) {
        return NULL;
    }
    title = task->extra.p;
    /* @hack redundant: a test of the copy, where returning task->extra would
     * do */
    if (title == NULL) {
        return NULL;
    }
    return title;
}

/* the width of count letters at scale, each with a gap of spacing after it */
static Float boss_title_width(const struct boss_title_letter *letters,
                              Float spacing, Float scale, Sint16 count)
{
    /* @hack type: i a Sint16, where an int would do */
    Sint16 i;
    Float width = 0.0F;

    for (i = 0; i < count; i++) {
        width += letters->width * scale;
        width += spacing * scale;
        letters++;
    }
    return width;
}

/* while the title fades, the letter takes the title's alpha, and its place in
 * the title's growing row */
static void boss_title_letter_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_title *title;
    /* @hack spelling: title_work, a local for the title's work, where
     * boss_title_task->work read in place would do */
    struct task_work *title_work;
    struct boss_title_letter *letter;
    Float x;
    /* @hack spelling: y, a local for the letter's place, where pos.y set in
     * place would do */
    Float y;
    /* @hack type: i a Sint16, where an int would do */
    Sint16 i;

    if (TITLE_HELD()) {
        return;
    }
    if (PARENT_HIDDEN(task)) {
        return;
    }
    /* @hack spelling: the result compared with 1, where its truth alone
     * would do */
    if (boss_title_fading() != 1) {
        return;
    }
    title = boss_title_current();
    title_work = boss_title_task->work;
    SCALE(work) = title->scale;
    /* @hack type: the alpha cut through a Uint8, where the shift by 24 alone
     * keeps the same byte */
    COLOR(work) = ((Uint8)(FADE_ALPHA(title_work) * 255.0F) << 24) +
                  (title->color & RGB_MASK);
    letter = title->letters;
    /* @hack spelling: x set to the row's width first, where the width read
     * in place would do */
    x = boss_title_width(title->letters, title->spacing, title->scale,
                         title->letter_count);
    x = title->x - (x * 0.5F);
    y = title->y - (title->letters->height * title->scale);
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 1; i <= LETTER_INDEX(work); i++) {
        x += (letter->width * title->scale) + (title->spacing * title->scale);
        letter++;
    }
    work->pos.x = x;
    work->pos.y = y;
}

static void boss_title_letter_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_title_letter *letter = LETTER(work);
    Float scale = SCALE(work);
    NJS_TEXTURE_VTX letter_quad[CORNERS] = {
#include "shared/boss_title/letter_quad.inc"
    };
    /* @hack spelling: vertices, a local for letter_quad that the call takes
     * as *&vertices, where letter_quad itself would do */
    NJS_TEXTURE_VTX *vertices;
    NJS_TEXTURE_VTX *vertex;
    int i;

    if (TITLE_UNDRAWN()) {
        return;
    }
    if (PARENT_HIDDEN(task)) {
        return;
    }
    if (FIRST_CHILD(task)) {
        boss_title_set_draw_state();
    }
    vertex = letter_quad;
    vertices = vertex;
    for (i = 0; i < CORNERS; i++) {
        vertex->x = work->pos.x + (vertex->x * letter->width * scale);
        vertex->y = work->pos.y + (vertex->y * letter->height * scale);
        vertex->u *= LETTER_U(work);
        vertex->v *= LETTER_V(work);
        vertex->col = COLOR(work);
        vertex++;
    }
    njDrawTexture(*&vertices, CORNERS, letter->texture, TRUE);
    if (LAST_CHILD(task)) {
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        njFogEnable();
    }
}

static void boss_title_letter_destroy(struct task *task)
{
}

/* part / whole for a whole above 0, only as precise as njInvertSqrt */
/* @hack spelling: whole before part in boss_title_fraction's parameters,
 * where part before whole would do */
static Float boss_title_fraction(Sint16 whole, Sint16 part)
{
    return part * njInvertSqrt(whole) * njInvertSqrt(whole);
}

/* show letter as a child task of parent, with its top left corner at
 * (x, y). index is the letter's place among title's letters, from 0 */
static struct task *boss_title_letter_create(struct task *parent, Float x,
                                             Float y,
                                             const struct boss_title *title,
                                             struct boss_title_letter *letter,
                                             Sint16 index)
{
    struct task *task;
    struct task_work *work;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, boss_title_letter_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    work->pos.x = x;
    work->pos.y = y;
    work->pos.z = CHILD_Z;
    SCALE(work) = title->scale;
    LETTER_U(work) = boss_title_fraction(letter->texture_width, letter->width);
    LETTER_V(work) =
        boss_title_fraction(letter->texture_height, letter->height);
    LETTER_INDEX(work) = index;
    LETTER(work) = letter;
    COLOR(work) = (title->color & RGB_MASK) + OPAQUE_ALPHA;
    task->draw = boss_title_letter_draw;
    task->destructor = boss_title_letter_destroy;
    return task;
}

/* while the title fades, the bar takes the title's alpha and scale. it also
 * centers again on the title's x */
static void boss_title_bar_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_title *title;
    /* @hack spelling: x and y, locals for the bar's place, where pos.x and
     * pos.y set in place would do */
    Float x;
    Float y;

    if (TITLE_HELD()) {
        return;
    }
    if (PARENT_HIDDEN(task)) {
        return;
    }
    if (BAR_DELAY(work) == 0) {
        if (BAR_ALPHA(work) < 1.0F) {
            BAR_ALPHA(work) += BAR_FADE_IN;
            if (BAR_ALPHA(work) > 1.0F) {
                BAR_ALPHA(work) = 1.0F;
            }
        }
    } else {
        BAR_DELAY(work)--;
    }
    /* @hack spelling: the result compared with 1, where its truth alone
     * would do */
    if (boss_title_fading() != 1) {
        return;
    }
    title = boss_title_current();
    x = boss_title_width(title->bar, 0.0F, title->scale, title->bar_count);
    x = title->x - (x * 0.5F);
    y = title->y;
    work->pos.x = x;
    work->pos.y = y;
    SCALE(work) = title->scale;
    BAR_ALPHA(work) = FADE_ALPHA(boss_title_task->work);
}

static void boss_title_bar_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale = SCALE(work);
    NJS_TEXTURE_VTX bar_quad[CORNERS] = {
#include "shared/boss_title/bar_quad.inc"
    };
    struct boss_title_letter *piece = PIECES(work);
    /* @hack spelling: vertices, a local for bar_quad, where bar_quad itself
     * would do */
    NJS_TEXTURE_VTX *vertices;
    NJS_TEXTURE_VTX *vertex;
    Uint32 color;
    /* @hack type: i and j unsigned, where ints would do while the count of
     * pieces is from 0 to 127 */
    unsigned int i;
    unsigned int j;

    if (TITLE_UNDRAWN()) {
        return;
    }
    if (PARENT_HIDDEN(task)) {
        return;
    }
    /* @hack type: the alpha cut through a Uint8, where the shift by 24 alone
     * keeps the same byte */
    color = ((Uint8)(BAR_ALPHA(work) * 255.0F) << 24) + COLOR(work);
    vertices = bar_quad;
    vertex = vertices;
    for (j = 0; j < CORNERS; j++) {
        vertex->x = work->pos.x + (scale * vertex->x);
        vertex->y = work->pos.y + (scale * vertex->y);
        vertex->col = color;
        vertex++;
    }
    if (FIRST_CHILD(task)) {
        boss_title_set_draw_state();
    }
    scale *= PIECE_SIZE;
    i = 0;
    /* @hack spelling: the loop a do-while under an if, where a for would
     * do */
    if (i < (unsigned int)PIECE_COUNT(work)) {
        do {
            vertex = vertices;
            njDrawTexture(vertex, CORNERS, piece->texture, TRUE);
            piece++;
            for (j = 0; j < CORNERS; j++) {
                /* @hack spelling: vertex->x = vertex->x + scale, where +=
                 * would do */
                vertex->x = vertex->x + scale;
                vertex++;
            }
            i++;
        } while (i < (unsigned int)PIECE_COUNT(work));
    }
    if (LAST_CHILD(task)) {
        njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
        njFogEnable();
    }
}

static void boss_title_bar_destroy(struct task *task)
{
}

/* show title's bar as a child task of parent, with its top left corner
 * at (x, y) */
static struct task *boss_title_bar_create(struct task *parent, Float x, Float y,
                                          const struct boss_title *title)
{
    struct task *task;
    struct task_work *work;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, boss_title_bar_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    work->pos.x = x;
    work->pos.y = y;
    work->pos.z = CHILD_Z;
    SCALE(work) = title->scale;
    BAR_ALPHA(work) = 0.0F;
    PIECES(work) = title->bar;
    COLOR(work) = title->color & RGB_MASK;
    PIECE_COUNT(work) = title->bar_count;
    BAR_DELAY(work) = BAR_DELAY_STEPS;
    task->draw = boss_title_bar_draw;
    task->destructor = boss_title_bar_destroy;
    return task;
}

/* copy count letters, from the list at from to the list at to. each copy
 * takes the texture its BOSS_TITLE_NEXT_TEXTURE or BOSS_TITLE_SAME_TEXTURE
 * names. a wait above 0 takes the place of each letter's own. nothing is
 * copied where from or to is null */
static void boss_title_copy_letters(const struct boss_title_letter *from,
                                    struct boss_title_letter *to, Sint16 count,
                                    Sint16 wait)
{
    /* @hack type: i a Sint16, where an int would do */
    Sint16 i;
    Sint32 texture; /* the previous letter's texture, unset before the first */

#ifdef NONMATCHING
    /* GCC can't see that texture is set wherever it is read. a list's first
     * letter holds an index, as boss_title.h requires */
    texture = 0;
#endif
    for (i = 0; i < count; i++) {
        /* @hack spelling: the test of from and to inside the loop, where one
         * test before it would do */
        if (from == NULL || to == NULL) {
            return;
        }
        *to = *from;
        if (to->texture == BOSS_TITLE_NEXT_TEXTURE) {
            to->texture = ++texture;
        } else if (to->texture == BOSS_TITLE_SAME_TEXTURE) {
            to->texture = texture;
        }
        if (wait > 0) {
            to->wait = wait;
        }
        texture = to->texture;
        from++;
        to++;
    }
}

static void boss_title_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_title *title = task->extra.p;
    struct boss_title_letter *letter;

    if (TITLE_HELD()) {
        return;
    }
    if (work->flags & TITLE_HIDDEN) {
        return;
    }
    if (LETTERS_SHOWN(work) < title->letter_count && NEXT_WAIT(work) > 0 &&
        --NEXT_WAIT(work) == 0) {
        letter = NEXT_LETTER(work);
        if (letter->texture != 0) {
            boss_title_letter_create(task, work->pos.x, work->pos.y, title,
                                     letter, LETTERS_SHOWN(work));
            if (SOUND(work) != NO_SOUND) {
                sound_play(SOUND(work), NULL, 0, 0);
            }
        }
        work->pos.x +=
            (letter->width * title->scale) + (title->spacing * title->scale);
        letter++;
        NEXT_LETTER(work) = letter;
        LETTERS_SHOWN(work)++;
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side. the last
         * letter leaves the cursor one past the letters' copy, and this side
         * doesn't read its wait */
        NEXT_WAIT(work) =
            LETTERS_SHOWN(work) < title->letter_count ? letter->wait : 0;
#else
        /* @bug memory: reads the wait of the letter after the last, past the
         * end of the letters' copy, when the last letter shows (MOV.B
         * @(8,R11),R0 at 0x824 of the object's code) */
        NEXT_WAIT(work) = letter->wait;
#endif
    }
    if (LIFE(work) > 0 && --LIFE(work) == 0) {
        task_kill(task);
        return;
    }
    if (work->flags & TITLE_FADING) {
        title->scale *= FADE_GROWTH;
        FADE_ALPHA(work) -= FADE_RATE(work);
        if (FADE_ALPHA(work) <= 0.0F) {
            task_kill(task);
        }
    }
}

static void boss_title_destroy(struct task *task)
{
    struct boss_title *title = task->extra.p;

    if (title->letters != NULL) {
        syFree(title->letters);
    }
    if (title->bar != NULL) {
        syFree(title->bar);
    }
    boss_title_task = NULL;
    boss_title_texlist = NULL;
}

struct task *boss_title_create(struct task *parent,
                               const struct boss_title *title)
{
    struct task *task;
    struct task_work *work;
    struct boss_title *copy;
    struct boss_title_letter *bar;
    struct boss_title_letter *letters;
    Float x;
    Sint16 wait;

    if (boss_title_task != NULL) {
        return NULL;
    }
    if (title->texlist == NULL) {
        return NULL;
    }
#ifdef NONMATCHING
    /* Each displayed count lives in a signed byte. Positioning and timing
     * start from the first letter even before the task steps. */
    if (title->letters == NULL || title->letter_count < 1 ||
        title->letter_count > SCHAR_MAX ||
        (title->bar != NULL &&
         (title->bar_count < 1 || title->bar_count > SCHAR_MAX))) {
        return NULL;
    }
#endif
    if (parent == NULL) {
        task = task_create(TASK_WORK, TITLE_LEVEL, boss_title_step,
                           "BossTitleExec");
    } else {
        task = task_create_child(TASK_WORK, boss_title_step, parent);
    }
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    copy = syCalloc(1, sizeof(struct boss_title));
    if (copy == NULL) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = copy;
    *copy = *title;
    if (stage_current_id() >= STAGE_BOSS_BIGFOOT) {
        SOUND(work) = BOSS_LETTER_SOUND;
    } else {
        SOUND(work) = LETTER_SOUND;
    }
    if (copy->flags & BOSS_TITLE_LIFE) {
        LIFE(work) = copy->life;
    } else {
        LIFE(work) = 0;
    }
    if (!(copy->flags & BOSS_TITLE_SCALE)) {
        copy->scale = 1.0F;
    }
    if (title->bar != NULL) {
        bar = syCalloc(title->bar_count, sizeof(struct boss_title_letter));
        if (bar == NULL) {
            task_destroy(task);
            return NULL;
        }
        copy->bar = bar;
        boss_title_copy_letters(title->bar, copy->bar, title->bar_count, 0);
        /* @hack spelling: x set to the bar's width first, where the width
         * read in place would do */
        x = boss_title_width(copy->bar, 0.0F, copy->scale, copy->bar_count);
        x = copy->x - (x * 0.5F);
        boss_title_bar_create(task, x, copy->y, copy);
    }
    if (title->letters != NULL) {
        letters =
            syCalloc(title->letter_count, sizeof(struct boss_title_letter));
        if (letters == NULL) {
            /* @bug memory: leaks the bar's copy where the letters' copy
             * can't be allocated. task_destroy frees the title's copy in
             * task->extra, but the destructor that frees the bar isn't set
             * yet (JSR @R2 at 0xa88 of the object's code) */
#ifdef NONMATCHING
            /* this side repairs the tag above: it frees the bar's copy, which
             * is null without a bar */
            syFree(copy->bar);
#endif
            task_destroy(task);
            return NULL;
        }
        copy->letters = letters;
        if (copy->flags & BOSS_TITLE_LETTER_WAITS) {
            wait = 0;
        } else {
            wait = copy->wait;
        }
        boss_title_copy_letters(title->letters, copy->letters,
                                copy->letter_count, wait);
        /* @hack redundant: a test of copy->bar whose two arms place the
         * letters the same way, where one placement would do */
        if (copy->bar != NULL) {
            x = boss_title_width(copy->letters, copy->spacing, copy->scale,
                                 copy->letter_count);
            work->pos.x = copy->x - (x * 0.5F);
            work->pos.y = copy->y - (copy->letters->height * copy->scale);
        } else {
            x = boss_title_width(copy->letters, copy->spacing, copy->scale,
                                 copy->letter_count);
            work->pos.x = copy->x - (x * 0.5F);
            work->pos.y = copy->y - (copy->letters->height * copy->scale);
        }
    }
    boss_title_texlist = copy->texlist;
    /* @hack spelling: letters set to copy->letters again, where copy->letters
     * read in place would do */
    letters = copy->letters;
    NEXT_LETTER(work) = letters;
    /* @bug memory: reads the first letter's wait through null where
     * title->letters is null (MOV.B @(8,R4),R0 at 0xb2a of the object's
     * code). the test of title->letters at 0xa72 of the object's code lets
     * that null through to here */
    NEXT_WAIT(work) = letters->wait + FIRST_WAIT;
    LETTERS_SHOWN(work) = 0;
    task->destructor = boss_title_destroy;
    boss_title_task = task;
    return task;
}

int boss_title_hide(void)
{
    struct task *task = boss_title_task;
    struct task_work *work;

    /* @hack spelling: two tests that each return -1, where one test of both
     * would do */
    if (task == NULL) {
        return -1;
    }
    if (task->work == NULL) {
        return -1;
    }
    work = task->work;
    if (!(work->flags & TITLE_HIDDEN)) {
        work->flags |= TITLE_HIDDEN;
        return 1;
    }
    return 0;
}

int boss_title_show(void)
{
    struct task *task = boss_title_task;
    struct task_work *work;

    /* @hack spelling: two tests that each return -1, where one test of both
     * would do */
    if (task == NULL) {
        return -1;
    }
    if (task->work == NULL) {
        return -1;
    }
    work = task->work;
    if (work->flags & TITLE_HIDDEN) {
        work->flags &= ~TITLE_HIDDEN;
        return 1;
    }
    return 0;
}

int boss_title_set_sound(Sint32 id)
{
    struct task *task = boss_title_task;

    /* @hack spelling: two tests that each return -1, where one test of both
     * would do */
    if (task == NULL) {
        return -1;
    }
    if (task->work == NULL) {
        return -1;
    }
    SOUND(task->work) = id;
    /* @hack implicit: no return value where the sound is set, where return 0
     * would do while nothing calls the function */
#ifdef NONMATCHING
    /* the hack's plain form, for GCC */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

int boss_title_fade(Float rate)
{
    struct task *task = boss_title_task;
    struct task_work *work;

    /* @hack spelling: two tests that each return -1, where one test of both
     * would do */
    if (task == NULL) {
        return -1;
    }
    work = task->work;
    if (work == NULL) {
        return -1;
    }
    work->flags |= TITLE_FADING;
    FADE_RATE(work) = rate;
    FADE_ALPHA(work) = 1.0F;
    /* @hack implicit: no return value where the fade starts, where return 0
     * would do while no caller reads the result */
#ifdef NONMATCHING
    /* the hack's plain form, for GCC */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
