/* the motion control: an object plays one entry of a table of motions at a
 * time, and links into another entry by blending the two */
#ifndef DRAW_MOTION_H
#define DRAW_MOTION_H
#include <ninja.h>
#include <sg_xpt.h>

/* what a play does as its frame reaches end, at end or past it in speed's
 * direction. a play takes its mode from its entry */
enum motion_mode {
    /* go round again: the frame goes back by end - start */
    MOTION_LOOP = 0,
    /* as MOTION_LOOP, but the frame reaches the end a frame past end, and
     * goes back a frame further */
    MOTION_LOOP_PAST_END = 1,
    /* go round again by a link from the pose past end to the pose at start.
     * the link lasts a frame at speed. it starts as far on as the frame went
     * past end */
    MOTION_LOOP_LINKED = 2,
    MOTION_HOLD = 3, /* hold the end */
    /* hold the end, and link into next where next isn't MOTION_NONE */
    MOTION_LINK_NEXT = 4,
    /* make next the current entry at once, from its start, where next isn't
     * MOTION_NONE. the frame is left past the end otherwise */
    MOTION_CHANGE_NEXT = 5,
    /* as MOTION_CHANGE_NEXT, with the frame as far on from next's start as
     * it had gone past the end */
    MOTION_GO_ON_NEXT = 6,
    /* turn back: the frame holds the end, start and end swap, and speed
     * changes sign */
    MOTION_TURN_BACK = 7
};

/* a motion_control's flags */
/* the play reached its end in this step. each step clears it first */
#define MOTION_ENDED 0x1
/* the control links from current into next_play, as link runs from 0 to 1 */
#define MOTION_LINKING 0x2
/* both plays move while the link runs, each going round as MOTION_LOOP does */
#define MOTION_LINK_PLAYS 0x4
/* MOTION_HOLD holds the end in this step. each step clears it first */
#define MOTION_HELD 0x8
/* the link is MOTION_LOOP_LINKED's, into the same entry at start */
#define MOTION_LINK_TO_START 0x10

/* the steps a link lasts, at a speed_scale of 1, where its entry gives a
 * negative link_frames */
#define MOTION_LINK_FRAMES 15
/* next's value for no entry */
#define MOTION_NONE (-1)

/* one motion of a table */
struct motion_table_entry {
    NJS_MOTION *motion;
    Uint16 mode; /* an enum motion_mode */
    Uint16 pose; /* the control's pose while the entry is current */
    /* the entry MOTION_LINK_NEXT, MOTION_CHANGE_NEXT and MOTION_GO_ON_NEXT go
     * on to, or MOTION_NONE */
    Sint32 next;
    /* the steps a link into the entry lasts at a speed_scale of 1.
     * motion_link and motion_link_synced take MOTION_LINK_FRAMES where it is
     * negative. MOTION_LINK_NEXT's link out of the entry lasts -link_frames
     * steps where the entry's own is negative. otherwise that link lasts as
     * many steps as the absolute value of next's link_frames */
    Sint32 link_frames;
    /* the frames it plays from and to, and how far it moves in a step. a
     * negative speed plays from a start above end */
    Float start;
    Float end;
    Float speed;
};

/* a motion as a control plays it, from an entry */
struct motion_play {
    Uint16 mode; /* an enum motion_mode */
    Float frame;
    Float start;
    Float end;
    Float speed;
    NJS_MOTION *motion;
};

/* how an object plays its table of motions */
struct motion_control {
    Uint16 flags;
    Uint16 pose; /* the current entry's pose */
    Sint32 index; /* the current entry's index in table */
    /* the entry a link goes into, or else the current entry's next */
    Sint32 next;
    /* what the next step's moves are scaled by. each step sets it back to
     * 1 as it ends */
    Float speed_scale;
    /* what a link adds to link in a step at a speed_scale of 1, and how far
     * the link has gone, from 0 to 1 */
    Float link_rate;
    Float link;
    /* the motion that plays, and while MOTION_LINKING is set the one the
     * link goes into. next_play must follow current: draw/motion.c reaches
     * it by stepping a pointer past current */
    struct motion_play current;
    struct motion_play next_play;
    const struct motion_table_entry *table;
};

/* clear the control, give it table and make entry 0 current, as motion_set
 * does. speed_scale is 0, so the first step moves nothing. next_play keeps
 * its mode and speed */
void motion_init(struct motion_control *control,
                 const struct motion_table_entry *table);
/* step the control. it clears MOTION_ENDED and MOTION_HELD first, and sets
 * speed_scale to 1 last.
 *
 * outside a link, current's frame moves by speed times speed_scale. where
 * the frame reaches end, current's mode acts and MOTION_ENDED is set.
 *
 * in a link, link moves by link_rate times speed_scale, with a negative
 * link_rate made positive first. the plays move only under
 * MOTION_LINK_PLAYS, each going round as MOTION_LOOP does. a link ends at 1,
 * and the entry at next becomes current with the flags clear. under
 * MOTION_LINK_PLAYS current takes next_play as it stands, and otherwise the
 * entry from its start. a MOTION_LINK_TO_START link's end keeps the entry
 * instead, and clears only MOTION_LINKING and MOTION_LINK_TO_START. current's
 * frame then lies as far past start as link passed 1 */
void motion_step(struct motion_control *control);
/* link into the entry at index, from its start. the link lasts the entry's
 * link_frames, or MOTION_LINK_FRAMES where that is negative.
 * motion_link_frames links in frames steps. each sets the flags to
 * MOTION_LINKING alone */
void motion_link(struct motion_control *control, Sint32 index);
void motion_link_frames(struct motion_control *control, Sint32 index,
                        Uint16 frames);
/* link as motion_link does, with the entry's frame as far through its
 * frames as current's frame is through its own. where current and the
 * entry are both MOTION_LOOP, it sets MOTION_LINK_PLAYS */
void motion_link_synced(struct motion_control *control, Sint32 index);
/* make the entry at index current at once, from its start, and clear the
 * flags and any link */
void motion_set(struct motion_control *control, Sint32 index);
/* scale the moves of the next step by scale */
void motion_set_speed(struct motion_control *control, Float scale);
/* TRUE where MOTION_ENDED is set, and FALSE otherwise. motion_held tests
 * MOTION_HELD alike */
Bool motion_ended(struct motion_control *control);
Bool motion_held(struct motion_control *control);
/* draw object posed by current at its frame, or while MOTION_LINKING is set
 * by the link from current to next_play. motion_draw draws with
 * njCnkEasyDrawMotion and njCnkEasyDrawMotionLink, and motion_draw_simple
 * with njCnkSimpleDrawMotion and njCnkSimpleDrawMotionLink */
void motion_draw(NJS_CNK_OBJECT *object, struct motion_control *control);
void motion_draw_simple(NJS_CNK_OBJECT *object, struct motion_control *control);
/* give the control table, and change nothing else */
void motion_set_table(struct motion_control *control,
                      const struct motion_table_entry *table);
/* set current's frame to frame, held between start and end */
void motion_set_frame(struct motion_control *control, Float frame);
/* set current's frame share of the way from start to end, share held
 * between 0 and 1 */
void motion_set_share(struct motion_control *control, Float share);
/* current's frame, and how far it lies from start toward end as a share */
Float motion_frame(struct motion_control *control);
Float motion_share(struct motion_control *control);
/* the current entry's index, and its pose */
Sint32 motion_index(struct motion_control *control);
int motion_pose(struct motion_control *control);
/* print the control's state with njPrintC and njPrintD at column x, from
 * row y down: "[MOTION INFO]", then the current entry, next, current's
 * frame, the link in hundredths and the pose */
void motion_info_print(struct motion_control *control, int x, int y);

#endif
