/* the boss title: a boss's name on the screen as its battle starts. the name
 * appears a letter at a time, above a bar */
#ifndef SHARED_BOSS_TITLE_H
#define SHARED_BOSS_TITLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a letter of a title, or a piece of its bar */
struct boss_title_letter {
    /* the global index of the letter's texture, as njDrawTexture takes it,
     * or one of the two codes below. a letter of texture 0 is a space. a space
     * takes its place in the row, but shows nothing and plays no sound. a piece
     * of the bar draws texture 0 like any other */
    Sint32 texture;
    /* the letter's size, in pixels at a scale of 1. the letter shows the
     * width by height pixels at the top left of its texture. the bar's pieces
     * draw 16 pixels square whatever their size, yet the bar centers by their
     * widths. it centers on the title's x only where they average 16 */
    Sint16 width;
    Sint16 height;
    /* the steps from the previous letter to this one. the first letter shows
     * wait + 20 steps after the title starts. the title counts the steps modulo
     * 256. a count of 0 shows neither this letter nor any after it */
    Sint16 wait;
    /* the size of the letter's whole texture, in pixels */
    Sint16 texture_width;
    Sint16 texture_height;
    Uint8 unk_0e[2];
};
/* the codes a letter's texture may hold in place of an index.
 * BOSS_TITLE_NEXT_TEXTURE stands for the previous letter's index plus 1.
 * BOSS_TITLE_SAME_TEXTURE stands for the previous letter's index. a list's
 * first letter must hold neither, unchecked: its index would be undefined */
#define BOSS_TITLE_NEXT_TEXTURE 1
#define BOSS_TITLE_SAME_TEXTURE 2

/* a title's flags */
#define BOSS_TITLE_LIFE 0x1 /* the title ends after life steps */
#define BOSS_TITLE_SCALE 0x2 /* the title takes scale, not 1 */
#define BOSS_TITLE_LETTER_WAITS 0x4 /* each letter keeps its own wait */

/* a title, as boss_title_create takes it */
struct boss_title {
    struct boss_title_letter *letters;
    Sint16 letter_count;
    Sint16 spacing; /* the gap after each letter, in pixels at a scale of 1 */
    /* a wait above 0 takes the place of each letter's own, unless flags has
     * BOSS_TITLE_LETTER_WAITS */
    Sint16 wait;
    Uint8 unk_0a[2];
    struct boss_title_letter *bar; /* the bar's pieces, or null */
    Sint16 bar_count;
    Uint16 flags;
    Sint16 life;
    Uint8 unk_16[2];
    NJS_TEXLIST *texlist;
    /* a point of the screen, in pixels. the row of letters, each with the
     * spacing after it, centers on x, and so does the bar. the letters' tops
     * lie letters[0].height * scale above y, and the bar's top at y */
    Float x;
    Float y;
    Float scale;
    /* an ARGB color, whose alpha byte the title ignores */
    Uint32 color;
};

/* show title as parent's child task, and return the task. a null parent makes
 * the title a task of level 3. the title keeps its own copies of title, its
 * letters and its bar. title->texlist, which it doesn't copy, must outlive it.
 *
 * it returns null and shows nothing where a title shows already, or where
 * title->texlist is null. it does the same where the heap can't give the task
 * or a copy. where the letters' copy fails, the bar's copy leaks.
 * The GCC build requires letters and a letter_count from 1 to 127. A supplied
 * bar also needs 1 to 127 pieces. The original assumes these bounds because
 * both running counts use Sint8.
 *
 * each letter but a space shows as a child task after its wait, with a sound.
 * the bar shows as a child task too, and fades in. under BOSS_TITLE_LIFE, the
 * title ends after life steps, read as a Uint16. a life of 0, or no
 * BOSS_TITLE_LIFE, leaves the title until boss_title_fade's fade ends it.
 *
 * the title holds still while game_state.time_stopped, unk_09 or
 * mini_event_running isn't 0. it draws nothing while mini_event_running isn't 0
 * or unk_0b is 0 */
struct task *boss_title_create(struct task *parent,
                               const struct boss_title *title);
/* fade the title out. its alpha starts at 1 and loses rate each step, and its
 * scale grows by 8% a step. the title ends once its alpha is 0 or less. the
 * call returns -1 where no title shows, and no defined value where one does */
int boss_title_fade(Float rate);

#endif
