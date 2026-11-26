/* the motion control: an object plays one entry of a table of motions at a
 * time, and links into another entry by blending the two */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/motion.h"

/* motion_info_print prints each number NUMBER_COLUMN columns right of its
 * label, in NUMBER_DIGITS digits. it prints the link in hundredths */
#define NUMBER_COLUMN 6
#define NUMBER_DIGITS 2
#define HUNDREDTHS 100.0F

static void motion_link_next(struct motion_control *control);
static void motion_go_on(struct motion_control *control, Sint32 index);
static void motion_link_end(struct motion_control *control, Sint32 index);

void motion_init(struct motion_control *control,
                 const struct motion_table_entry *table)
{
    /* @hack spelling: play, which points at current and then at next_play,
     * where the two plays' fields would do */
    struct motion_play *play = &control->current;

    control->flags = 0;
    control->pose = 0;
    control->index = 0;
    control->next = MOTION_NONE;
    control->speed_scale = 0.0F;
    control->link_rate = 0.0F;
    control->link = 0.0F;
    play->frame = 0.0F;
    play->start = 0.0F;
    play->end = 0.0F;
    play->motion = NULL;
    play = &control->next_play;
    play->frame = 0.0F;
    play->start = 0.0F;
    play->end = 0.0F;
    play->motion = NULL;
    control->table = table;
    motion_set(control, 0);
}

void motion_step(struct motion_control *control)
{
    /* @hack spelling: next reached by stepping play past current, where
     * &control->next_play would do. the step lands on next_play only
     * because it follows current in the control */
    struct motion_play *play = &control->current;
    struct motion_play *current = play++;
    struct motion_play *next = play;
    Bool reached = FALSE;

    control->flags &= ~(MOTION_ENDED | MOTION_HELD);
    if (control->flags & MOTION_LINKING) {
        if (control->link_rate < 0.0F) {
            /* @hack spelling: link_rate times -1, where its negation would
             * do */
            control->link_rate *= -1.0F;
        }
        control->link += control->link_rate * control->speed_scale;
        if (control->flags & MOTION_LINK_PLAYS) {
            current->frame += current->speed * control->speed_scale;
            if (current->speed >= 0.0F) {
                if (current->frame >= current->end) {
                    reached = TRUE;
                }
            } else {
                if (current->frame <= current->end) {
                    reached = TRUE;
                }
            }
            if (reached) {
                current->frame -= current->end - current->start;
            }
            next->frame += next->speed * control->speed_scale;
            reached = FALSE;
            if (next->speed >= 0.0F) {
                if (next->frame >= next->end) {
                    reached = TRUE;
                }
            } else {
                if (next->frame <= next->end) {
                    reached = TRUE;
                }
            }
            if (reached) {
                next->frame -= next->end - next->start;
            }
            if (control->link >= 1.0F) {
                control->link = 1.0F;
                motion_link_end(control, control->next);
            }
        } else if (control->link >= 1.0F) {
            if (control->flags & MOTION_LINK_TO_START) {
                if (current->speed > 0.0F) {
                    current->frame = current->start + control->link - 1.0F;
                } else {
                    current->frame = current->start - control->link + 1.0F;
                }
                control->link = 1.0F;
                control->flags &= ~(MOTION_LINKING | MOTION_LINK_TO_START);
            } else {
                control->link = 1.0F;
                motion_set(control, control->next);
            }
        }
    } else {
        current->frame += current->speed * control->speed_scale;
        if (current->speed >= 0.0F) {
            if (current->mode != MOTION_LOOP_PAST_END) {
                if (current->frame >= current->end) {
                    reached = TRUE;
                }
            } else {
                if (current->frame >= current->end + 1.0F) {
                    reached = TRUE;
                }
            }
        } else {
            if (current->mode != MOTION_LOOP_PAST_END) {
                if (current->frame <= current->end) {
                    reached = TRUE;
                }
            } else {
                if (current->frame <= current->end - 1.0F) {
                    reached = TRUE;
                }
            }
        }
        if (reached) {
            switch (current->mode) {
            case MOTION_LOOP:
                current->frame -= current->end - current->start;
                control->flags |= MOTION_ENDED;
                break;
            case MOTION_LOOP_PAST_END:
                if (current->speed >= 0.0F) {
                    current->frame -= current->end - current->start + 1.0F;
                } else {
                    current->frame -= current->end - current->start - 1.0F;
                }
                control->flags |= MOTION_ENDED;
                break;
            case MOTION_LOOP_LINKED:
                /* @hack redundant: next pointed at next_play, where it
                 * points already */
                next = &control->next_play;
                if (current->speed > 0.0F) {
                    control->link_rate = current->speed;
                    control->link = current->frame - current->end;
                } else {
                    control->link_rate = -current->speed;
                    /* @hack spelling: the frame negated and added to end,
                     * where end less the frame would do */
                    control->link = -current->frame + current->end;
                }
                /* @hack redundant: a test whose two arms do the same */
                if (control->link > 1.0F) {
                    next->mode = current->mode;
                    next->frame = current->start;
                } else {
                    next->mode = current->mode;
                    next->frame = current->start;
                }
                next->start = current->start;
                next->end = current->end;
                next->speed = current->speed;
                next->motion = current->motion;
                /* @hack spelling: the flags cleared, then the three set,
                 * where one store of the three would do */
                control->flags = 0;
                control->flags |=
                    MOTION_ENDED | MOTION_LINKING | MOTION_LINK_TO_START;
                break;
            case MOTION_HOLD:
                current->frame = current->end;
                /* @hack spelling: the two flags set one at a time, where
                 * one store of both would do */
                control->flags |= MOTION_ENDED;
                control->flags |= MOTION_HELD;
                break;
            case MOTION_LINK_NEXT:
                current->frame = current->end;
                if (control->next != MOTION_NONE) {
                    motion_link_next(control);
                }
                control->flags |= MOTION_ENDED;
                break;
            case MOTION_CHANGE_NEXT:
                if (control->next != MOTION_NONE) {
                    motion_set(control, control->next);
                }
                control->flags |= MOTION_ENDED;
                break;
            case MOTION_GO_ON_NEXT:
                if (control->next != MOTION_NONE) {
                    motion_go_on(control, control->next);
                }
                control->flags |= MOTION_ENDED;
                break;
            case MOTION_TURN_BACK: {
                Float start;

                current->frame = current->end;
                start = current->start;
                current->start = current->end;
                current->end = start;
                /* @hack spelling: speed times -1, where its negation would
                 * do */
                current->speed *= -1.0F;
                control->flags |= MOTION_ENDED;
                break;
            }
            }
        }
    }
    control->speed_scale = 1.0F;
}

void motion_link(struct motion_control *control, Sint32 index)
{
    struct motion_play *next = &control->next_play;
    const struct motion_table_entry *entry = control->table + index;

    control->next = index;
    if (entry->link_frames < 0) {
        control->link_rate = 1.0F / MOTION_LINK_FRAMES;
    } else {
        control->link_rate = 1.0F / (Float)entry->link_frames;
    }
    control->link = 0.0F;
    next->mode = entry->mode;
    next->frame = entry->start;
    next->start = entry->start;
    next->end = entry->end;
    next->speed = entry->speed;
    next->motion = entry->motion;
    /* @hack spelling: the flags cleared, then MOTION_LINKING set, where one
     * store of MOTION_LINKING would do */
    control->flags = 0;
    control->flags |= MOTION_LINKING;
}

/* MOTION_LINK_NEXT's link from the current entry into next */
static void motion_link_next(struct motion_control *control)
{
    struct motion_play *next = &control->next_play;
    /* @hack spelling: from, to and table, locals for the two indices and
     * the table, where indexing control->table in place would do */
    Sint32 from = control->index;
    /* @hack type: to volatile, where a plain local would do */
    volatile Sint32 to = control->next;
    const struct motion_table_entry *entry = control->table;
    /* @hack type: table volatile, where a plain local would do */
    const struct motion_table_entry *volatile table = entry;
    const struct motion_table_entry *target;
    Float rate;

    entry += from;
    target = table + to;
    /* @hack spelling: rate, a local set to 1 and then divided, where
     * link_rate set to the quotient would do */
    rate = 1.0F;
    if (entry->link_frames < 0) {
        rate /= -entry->link_frames;
    } else {
        rate /= target->link_frames;
    }
    control->link_rate = rate;
    control->link = 0.0F;
    next->mode = target->mode;
    next->frame = target->start;
    next->start = target->start;
    next->end = target->end;
    next->speed = target->speed;
    next->motion = target->motion;
    /* @hack spelling: the flags cleared, then MOTION_LINKING set, where one
     * store of MOTION_LINKING would do */
    control->flags = 0;
    control->flags |= MOTION_LINKING;
}

void motion_link_frames(struct motion_control *control, Sint32 index,
                        Uint16 frames)
{
    struct motion_play *next = &control->next_play;
    const struct motion_table_entry *entry = control->table + index;

    control->next = index;
    control->link_rate = 1.0F / (Float)frames;
    control->link = 0.0F;
    next->mode = entry->mode;
    next->frame = entry->start;
    next->start = entry->start;
    next->end = entry->end;
    next->speed = entry->speed;
    next->motion = entry->motion;
    /* @hack spelling: the flags cleared, then MOTION_LINKING set, where one
     * store of MOTION_LINKING would do */
    control->flags = 0;
    control->flags |= MOTION_LINKING;
}

void motion_link_synced(struct motion_control *control, Sint32 index)
{
    const struct motion_table_entry *entry = control->table + index;
    /* @hack spelling: next reached by stepping play past current, where
     * &control->next_play would do. the step lands on next_play only
     * because it follows current in the control */
    struct motion_play *play = &control->current;
    struct motion_play *current = play++;
    struct motion_play *next = play;
    /* @hack spelling: range, a local for current's frames, where the
     * difference in the division would do */
    Float range;
    Float share;

    control->next = index;
    if (entry->link_frames < 0) {
        control->link_rate = 1.0F / MOTION_LINK_FRAMES;
    } else {
        control->link_rate = 1.0F / (Float)entry->link_frames;
    }
    control->link = 0.0F;
    range = current->end - current->start;
    share = (current->frame - current->start) / range;
    next->mode = entry->mode;
    next->frame = entry->start + ((entry->end - entry->start) * share);
    next->start = entry->start;
    next->end = entry->end;
    next->speed = entry->speed;
    next->motion = entry->motion;
    /* @hack spelling: the flags cleared, then MOTION_LINKING set, where one
     * store of MOTION_LINKING would do */
    control->flags = 0;
    control->flags |= MOTION_LINKING;
    if (current->mode == MOTION_LOOP && next->mode == MOTION_LOOP) {
        control->flags |= MOTION_LINK_PLAYS;
    }
}

void motion_set(struct motion_control *control, Sint32 index)
{
    struct motion_play *current = &control->current;
    const struct motion_table_entry *entry = control->table + index;

    control->index = index;
    control->next = entry->next;
    control->pose = entry->pose;
    control->link_rate = 0.0F;
    control->link = 0.0F;
    current->mode = entry->mode;
    current->frame = entry->start;
    current->start = entry->start;
    current->end = entry->end;
    current->speed = entry->speed;
    current->motion = entry->motion;
    control->flags = 0;
}

/* MOTION_GO_ON_NEXT's change: motion_set, with the frame as far on from the
 * entry's start as current's had gone past its end */
static void motion_go_on(struct motion_control *control, Sint32 index)
{
    struct motion_play *current = &control->current;
    const struct motion_table_entry *entry = control->table + index;

    control->index = index;
    control->next = entry->next;
    control->pose = entry->pose;
    control->link_rate = 0.0F;
    control->link = 0.0F;
    current->mode = entry->mode;
    current->frame = (entry->start + current->frame) - current->end;
    current->start = entry->start;
    current->end = entry->end;
    current->speed = entry->speed;
    current->motion = entry->motion;
    control->flags = 0;
}

/* a MOTION_LINK_PLAYS link's end: the entry at index becomes current, which
 * takes next_play as it stands */
static void motion_link_end(struct motion_control *control, Sint32 index)
{
    /* @hack spelling: next reached by stepping play past current, where
     * &control->next_play would do. the step lands on next_play only
     * because it follows current in the control */
    struct motion_play *play = &control->current;
    struct motion_play *current = play++;
    const struct motion_play *next = play;
    const struct motion_table_entry *entry = control->table + index;

    control->index = index;
    control->next = entry->next;
    control->pose = entry->pose;
    control->link_rate = 0.0F;
    control->link = 0.0F;
    current->mode = next->mode;
    current->frame = next->frame;
    current->start = next->start;
    current->end = next->end;
    current->speed = next->speed;
    current->motion = next->motion;
    control->flags = 0;
}

void motion_set_speed(struct motion_control *control, Float scale)
{
    control->speed_scale = scale;
}

Bool motion_ended(struct motion_control *control)
{
    /* @hack spelling: the test's result returned through an if, where
     * returning the comparison would do */
    if (control->flags & MOTION_ENDED) {
        return TRUE;
    }
    return FALSE;
}

Bool motion_held(struct motion_control *control)
{
    /* @hack spelling: the test's result returned through an if, where
     * returning the comparison would do */
    if (control->flags & MOTION_HELD) {
        return TRUE;
    }
    return FALSE;
}

void motion_draw(NJS_CNK_OBJECT *object, struct motion_control *control)
{
    /* @hack spelling: next reached by stepping play past current, where
     * &control->next_play would do. the step lands on next_play only
     * because it follows current in the control */
    struct motion_play *play = &control->current;
    struct motion_play *current = play++;
    struct motion_play *next = play;
    NJS_MOTION_LINK link;

    if (control->flags & MOTION_LINKING) {
        link.motion[0] = current->motion;
        link.motion[1] = next->motion;
        link.frame[0] = current->frame;
        link.frame[1] = next->frame;
        njCnkEasyDrawMotionLink(object, &link, control->link);
    } else {
        njCnkEasyDrawMotion(object, current->motion, current->frame);
    }
}

void motion_draw_simple(NJS_CNK_OBJECT *object, struct motion_control *control)
{
    /* @hack spelling: next reached by stepping play past current, where
     * &control->next_play would do. the step lands on next_play only
     * because it follows current in the control */
    struct motion_play *play = &control->current;
    struct motion_play *current = play++;
    struct motion_play *next = play;
    NJS_MOTION_LINK link;

    if (control->flags & MOTION_LINKING) {
        link.motion[0] = current->motion;
        link.motion[1] = next->motion;
        link.frame[0] = current->frame;
        link.frame[1] = next->frame;
        njCnkSimpleDrawMotionLink(object, &link, control->link);
    } else {
        njCnkSimpleDrawMotion(object, current->motion, current->frame);
    }
}

void motion_set_table(struct motion_control *control,
                      const struct motion_table_entry *table)
{
    control->table = table;
}

void motion_set_frame(struct motion_control *control, Float frame)
{
    struct motion_play *current = &control->current;

    if (current->speed >= 0.0F) {
        if (frame < current->start) {
            frame = current->start;
        } else if (frame > current->end) {
            frame = current->end;
        }
    } else {
        if (frame < current->end) {
            frame = current->end;
        } else if (frame > current->start) {
            frame = current->start;
        }
    }
    current->frame = frame;
}

void motion_set_share(struct motion_control *control, Float share)
{
    struct motion_play *current = &control->current;
    /* @hack spelling: start and end, locals for current's, where current's
     * read in place would do */
    Float start = current->start;
    Float end = current->end;

    if (share < 0.0F) {
        share = 0.0F;
    } else if (share > 1.0F) {
        share = 1.0F;
    }
    current->frame = start + (share * (end - start));
}

Float motion_frame(struct motion_control *control)
{
    return control->current.frame;
}

Float motion_share(struct motion_control *control)
{
    struct motion_play *play = &control->current;
    /* @hack redundant: play stepped past current, though nothing reads it
     * again */
    struct motion_play *current = play++;
    /* @hack spelling: start, end, frame and range, locals for current's and
     * its frames, where current's read in place would do */
    Float start = current->start;
    Float end = current->end;
    Float frame = current->frame;
    Float range = end - start;

    return (frame - start) / range;
}

Sint32 motion_index(struct motion_control *control)
{
    return control->index;
}

int motion_pose(struct motion_control *control)
{
    return control->pose;
}

void motion_info_print(struct motion_control *control, int x, int y)
{
    njPrintC(NJM_LOCATION(x, y), "[MOTION INFO]");
    y++;
    njPrintC(NJM_LOCATION(x, y), "CURR:");
    njPrintD(NJM_LOCATION(x + NUMBER_COLUMN, y), control->index, NUMBER_DIGITS);
    y++;
    njPrintC(NJM_LOCATION(x, y), "NEXT:");
    njPrintD(NJM_LOCATION(x + NUMBER_COLUMN, y), control->next, NUMBER_DIGITS);
    y++;
    njPrintC(NJM_LOCATION(x, y), "FRM :");
    njPrintD(NJM_LOCATION(x + NUMBER_COLUMN, y), (Int)control->current.frame,
             NUMBER_DIGITS);
    y++;
    njPrintC(NJM_LOCATION(x, y), "LNK :");
    njPrintD(NJM_LOCATION(x + NUMBER_COLUMN, y),
             (Uint32)(control->link * HUNDREDTHS), NUMBER_DIGITS);
    y++;
    njPrintC(NJM_LOCATION(x, y), "POSE:");
    njPrintD(NJM_LOCATION(x + NUMBER_COLUMN, y), control->pose, NUMBER_DIGITS);
}
