/* a garden Chao's icon: the emotion ball over its head, and the mark that
 * springs up from it */
#ifndef CHAO_ICON_H
#define CHAO_ICON_H

#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the icon's modes, which chao_show_mark takes as its mark. a mark springs up
 * from the ball and holds for the steps chao_show_mark gives. it then shrinks
 * away, and the icon goes on to the next mark shown or to CHAO_MARK_NONE. a
 * heart or anger shrinks the ball away while it shows. CHAO_MARK_HOLD leaves
 * the icon as it stands, and no later mark replaces it */
enum chao_mark {
    CHAO_MARK_NONE,
    CHAO_MARK_EXCLAMATION,
    CHAO_MARK_HOLD,
    CHAO_MARK_QUESTION,
    CHAO_MARK_HEART,
    CHAO_MARK_ANGER
};

/* end the mark's hold at once, and show CHAO_MARK_NONE after it */
void chao_clear_emotion_mark(struct task *chao);
/* end the mark's hold at once, and show mark after it for frames steps. mark
 * replaces any mark waiting to follow. each call plays a sound at the Chao */
void chao_show_mark(struct task *chao, Sint8 mark, int frames);

/* the mark or the ball */
struct chao_icon_part {
    Uint16 phase; /* the part's stage in its mode, from 0 as the mode starts */
    Uint16 cell; /* the part's enum chao_icon_cell */
    Sint16 frame; /* the steps since its mode started */
    Uint16 unk_06;
    /* how far the mode moves the part from where it hangs. the ball hangs at
     * the icon's anchor, and the mark on the ball */
    NJS_VECTOR offset;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    NJS_POINT3 scale;
    Uint8 unk_38[12];
};

/* a Chao's icon. upper is the mark and lower the ball */
struct chao_icon {
    Sint16 mode, next_mode; /* enum chao_mark. next_mode is -1 for none */
    Sint32 timer, next_timer; /* the steps the mark holds yet, and the next's */
    /* the angles whose sines trade the parts' width for their height, and bob
     * the anchor up and down */
    Angle squash, bob;
    Uint32 color; /* the ball's, as ARGB */
    /* the entry of dark_icon_frames an animated ball shows, and the steps it
     * has shown it */
    Uint16 texture, texture_frame;
    Angle random_angle; /* a random turn, new each step */
    NJS_VECTOR direction; /* the head's up direction */
    NJS_POINT3 anchor; /* where the ball hangs */
    struct chao_icon_part upper, lower;
};

extern NJS_TEXLIST chao_icon_texlist;
/* the cells of chao_icon_texlist. a glow adds its cell to what lies behind
 * it. an inverted glow inverts what lies behind, as far as its cell is
 * bright. the mark holds CHAO_ICON_NO_MARK while it shows nothing */
enum chao_icon_cell {
    CHAO_ICON_BALL = 0,
    CHAO_ICON_EXCLAMATION = 1,
    CHAO_ICON_QUESTION = 2,
    CHAO_ICON_HEART = 3,
    CHAO_ICON_ANGER = 4,
    CHAO_ICON_DARK_BALL = 5,
    CHAO_ICON_HALO = 6,
    CHAO_ICON_GLOW = 9,
    CHAO_ICON_NO_MARK = 10,
    CHAO_ICON_INVERTED_GLOW = 11
};

/* set the ball's color from the Chao's type and parameters */
void chao_update_icon_color(struct task *chao);
/* start the icon with the ball alone, 5 above the Chao's pos, and place it */
void chao_icon_init(struct task *chao);
/* step the icon's mode. then set the ball at rest at its anchor, and the mark
 * at rest on the ball */
void chao_icon_place(struct task *chao);
/* step the icon's mode. then spring the ball toward its anchor, and the mark
 * toward the ball */
void chao_icon_step(struct task *chao);
/* draw the mark, while one shows, and the ball, each as a quad that faces the
 * camera */
void chao_draw_icon(struct task *chao);

#endif
