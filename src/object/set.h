/* the set manager. it keeps a placement for each entry of a stage's set
 * files. its task makes a pass over the placements each step. the pass gives
 * a task to each active placement in range that has no live one. a placement
 * is in range once player 0 or player 1 comes within its kind's range of it.
 * where player 0 is missing, the camera stands in for it. with neither, the
 * pass gives no task. a new task's work, where it has one, starts in the
 * entry's pose or a saved pose */
#ifndef OBJECT_SET_H
#define OBJECT_SET_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* an object_kind's attr */
/* the kind's range is the one its range field holds, not 400 squared */
#define KIND_RANGE 0x01
/* without KIND_RANGE, the kind's placements are in range wherever the
 * players are. their tasks never leave range */
#define KIND_ALWAYS 0x02
/* while set_passed is FALSE, the kind's placements are in range wherever the
 * players are */
#define KIND_FIRST 0x04
/* the bits that hold a RINGS_ value: the rings set_tally counts for each
 * entry of the kind. 0, or a value no RINGS_ names, counts none */
#define KIND_RINGS 0x70

/* KIND_RINGS's values. RINGS_ONE counts 1, and RINGS_SCL_Z the entry's
 * scl.z */
#define RINGS_ONE 0x10
#define RINGS_SCL_Z 0x20
/* an item box's rings, by its scl.x: 5 for BOX_5_RINGS, 10 for BOX_10_RINGS
 * and 20 for BOX_20_RINGS. any other scl.x counts none */
#define RINGS_BOX 0x30

/* the items an item box's or an item balloon's scl.x picks, by their index in
 * its table of items */
#define BOX_SPEED_UP 0
#define BOX_5_RINGS 1
#define BOX_1UP 2
#define BOX_10_RINGS 3
#define BOX_20_RINGS 4
#define BOX_BARRIER 5
#define BOX_BOMB 6
#define BOX_LIFE 7
#define BOX_MAGNETIC_BARRIER 8
#define BOX_EMPTY 9
#define BOX_INVINCIBLE 10

/* the bits of a set_entry's id that index the object list's kinds */
#define SET_KIND 0x7fff

/* a kind of object that a set file's entries name. flags, level, step and
 * name are task_create's for the kind's tasks */
struct object_kind {
    Uint8 flags;
    Uint8 level;
    Sint16 attr;
    Float range; /* squared, under KIND_RANGE */
    void (*step)(struct task *task);
    const char *name;
};

/* a stage's object list: the kinds its set files' entries name */
struct object_list {
    Sint32 count; /* how many kinds the list holds */
    struct object_kind *kinds;
};
/* an object list's initializer, for kinds an array of known length */
#define OBJECT_LIST(kinds) {sizeof(kinds) / sizeof((kinds)[0]), kinds}

/* an entry of a set file: an object's kind, and how the object stands */
struct set_entry {
    Uint16 id;
    Sint16 ang_x, ang_y, ang_z;
    NJS_POINT3 pos;
    NJS_POINT3 scl;
};

#ifdef NONMATCHING
/* A held placement can outlive the object list that supplied its kind. */
struct object_kind *set_entry_kind(const struct object_list *list,
                                   const struct set_entry *entry);
#endif

/* the head of a set file, the size of an entry. count is how many entries
 * follow it */
struct set_head {
    Uint32 count;
    Uint8 unk_04[28];
};

/* how a task stood when it left range */
struct placement_pose {
    struct angles ang;
    NJS_POINT3 pos;
    NJS_POINT3 scl;
};

/* FALSE from set_start, set_reactivate or a reset until the manager's next
 * pass. that pass sets it TRUE, even where it gives no task */
extern Bool set_passed;

/* set_init and set_exit do nothing */
void set_init(void);
void set_exit(void);

/* start the manager's task, and return TRUE. list becomes the object list,
 * and head the set file. head may be null, and so may list where head holds
 * no entry. set_start calls set_tally, so snapshots_start must have run.
 * where task_create fails, set_start returns FALSE, and leaves no list, file
 * or placement in use */
Bool set_start(struct object_list *list, struct set_head *head);
/* destroy the manager's task, which leaves no list, file or placement in
 * use, and return TRUE. the task must live: a set_start must have returned
 * TRUE since the last set_end */
Bool set_end(void);

/* set_reactivate, set_reset and set_reset_but_unk_20 make the placements in
 * use active again, and set set_passed to FALSE. without a set file, each
 * does nothing. set_reactivate passes over a placement with PLACEMENT_KEEP,
 * and clears the PLACEMENT_USED and state of the others */
void set_reactivate(void);
/* make PLACEMENT_ACTIVE each placement's only flag, clear its state, and
 * free its saved pose */
void set_reset(void);
/* set_reset, but keep PLACEMENT_UNK_20 */
void set_reset_but_unk_20(void);

/* test whether task has left range, a squared distance from the pos of its
 * work, which must exist. each returns FALSE while either player, or the
 * camera outside a demo, lies within range. otherwise it returns TRUE, and
 * task destroys itself on its next step. with a range of 0, or a placement
 * with PLACEMENT_IN_RANGE, each returns FALSE without measuring.
 * placement_try_leave takes the placement's leave.range, or 410 squared for
 * a task without a placement */
Bool placement_try_leave(struct task *task);
Bool placement_try_leave_by(struct task *task, Float range);

/* calls on task->placement, which each needs but placement_set_state and
 * placement_get_state. placement_used, placement_unk_20 and placement_touched
 * return their flag where it is set, and 0 otherwise */
void placement_set_used(struct task *task);
void placement_clear_used(struct task *task);
int placement_used(struct task *task);
/* have set_reactivate leave the placement as it is */
void placement_keep(struct task *task);
void placement_set_unk_20(struct task *task);
int placement_unk_20(struct task *task);
void placement_set_touched(struct task *task);
int placement_touched(struct task *task);
/* clear PLACEMENT_ACTIVE and PLACEMENT_LIVE: the pass gives the placement no
 * task until set_reactivate or a reset. task runs on */
void placement_stop(struct task *task);
/* placement_stop, and task destroys itself on its next step */
void placement_kill(struct task *task);
/* the byte the placement keeps for its tasks, from one to the next. without a
 * placement, placement_set_state does nothing, and placement_get_state's
 * result is undefined */
void placement_set_state(struct task *task, Sint8 state);
Sint8 placement_get_state(struct task *task);

#ifdef NONMATCHING
/* Copy one checked SET file into capacity bytes outside WORK_AREA. On failure
 * the destination retains its previous contents. */
Bool set_read_into(const char *name, struct set_head *destination,
                   Uint32 capacity);
#endif

/* Read two SET files into one zeroed Shinobi heap block. Its head counts the
 * entries from s followed by the entries from u. Each file consists of a
 * 32-byte head and count whole 32-byte entries. The allocation holds at
 * least min_entries slots, including the head. WORK_AREA is scratch.
 *
 * The port returns null on a failed transfer, invalid extent or allocation
 * failure. A null name_u selects a single file. The original requires both
 * names and counts s twice when u has only a header.
 *
 * set_read_stage reads <prefix>_s.bin and <prefix>_u.bin. a null prefix
 * stands for set<stage>, the current stage in four digits. in a two-player
 * game, each file is its _2p_ form where the disc has it. otherwise, under
 * MISSION_HD, each is its _hd_ form where the disc has it */
struct set_head *set_read(const char *name_s, const char *name_u,
                          int min_entries);
struct set_head *set_read_stage(const char *prefix, int min_entries);

/* set game_state.ring_total and snapshot_current->ring_total to
 * game_state.kept_ring_total, plus what KIND_RINGS counts for the set file's
 * entries. without an object list or a set file, it sets them to
 * kept_ring_total alone. set_tally needs snapshots_start to have run */
void set_tally(void);

#endif
