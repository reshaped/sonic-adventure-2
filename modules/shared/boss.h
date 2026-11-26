/* a boss's record, its power gauge, and the messengers a boss summons */
#ifndef SHARED_BOSS_H
#define SHARED_BOSS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the size in bytes of a record's block */
#define BOSS_BLOCK_SIZE 76

/* a boss's record, in its task's extra. motion, block and own are its parts,
 * each from the Shinobi heap or null */
struct boss_record {
    Uint8 unk_00;
    Sint8 unk_01;
    Sint8 unk_02;
    Uint8 unk_03[5];
    void *motion; /* the task's struct mover, which task->motion holds too */
    void *block; /* BOSS_BLOCK_SIZE bytes */
    void *own; /* boss_record_alloc_own's block */
    struct task *gauge; /* the power gauge's task */
    struct task *title; /* the task that shows the boss's title */
    NJS_POINT3 focus; /* the point the cameras look at */
    Float power; /* the hits left to beat the boss, which the gauge shows */
};

/* the gauge's texture list, which the bosses load from boss_pwguage_tex */
extern NJS_TEXLIST boss_gauge_texlist;

/* boss_record_create_parts with both parts */
struct boss_record *boss_record_create(struct task *task);
/* give task a zeroed record in task->extra, and return it. with with_motion,
 * the record and task->motion hold a new mover, as mover_alloc makes it. with
 * with_block, the record's block is BOSS_BLOCK_SIZE zeroed bytes.
 *
 * it returns null where the heap can't give the record or a part. the
 * non-matching build frees the parts it acquired and leaves task's extra and
 * motion unchanged. in the shipped game, a failed record changes nothing.
 * after a failed part, task->extra keeps the freed record. a failed mover has
 * mover_alloc write through null. with with_motion, a failed block frees the
 * mover but leaves it in task->motion. without with_motion, a failed block
 * frees task */
struct boss_record *boss_record_create_parts(struct task *task,
                                             Bool with_motion, Bool with_block);
/* give record a zeroed own block of size bytes, and return it. it returns
 * null for a null record, or where the heap has no room. an own block the
 * record held before leaks */
void *boss_record_alloc_own(struct boss_record *record, Uint32 size);
/* free the parts of task's record, and set each pointer to them to null.
 * where the record has a mover, the call frees task->motion. the
 * record itself stays in task->extra. the non-matching build also accepts
 * a null record. the shipped game requires a record */
void boss_record_free_parts(struct task *task);
/* make a task that draws a gauge of *power at the screen's top right, and
 * return it. the task is parent's child, or a task of level 3 where parent is
 * null. where the heap can't give the task, the call writes through null.
 *
 * the frame is 2 * *power + 2 cells long, for *power as it is at the call. it
 * shows alone for 41 steps. the bar then fills to show that *power. from then
 * on it drains toward *power and never grows back. once the bar is empty, the
 * frame closes and the gauge kills its task.
 *
 * the gauge holds still while game_state.time_stopped or
 * game_state.mini_event_running isn't 0. it draws nothing while
 * mini_event_running isn't 0. *power must be below 63 at the call, since the
 * frame counts its cells in a Sint8. power must stay valid while the task
 * lives */
struct task *boss_gauge_create(struct task *parent, Float *power);
/* set record's focus to pos + offset. record must not be null */
void boss_record_set_focus(struct boss_record *record, NJS_POINT3 *pos,
                           NJS_VECTOR *offset);
/* set *out to *first + *second */
void boss_vector_sum(NJS_VECTOR *out, const NJS_VECTOR *first,
                     const NJS_VECTOR *second);
/* summon a messenger for player under parent, with messenger_create's
 * MESSENGER_SUMMON. it starts 20 above player's pos, turned to an ang_y of 0.
 * let n be stage_get_retries' count. the messenger shows stage message n - 1,
 * capped at message_count - 1. an n of 0 or less summons none.
 * player_work[player] must not be null, and message_count must be 1 or more */
void boss_messenger_summon_capped(int player, int message_count,
                                  struct task *parent);

#endif
