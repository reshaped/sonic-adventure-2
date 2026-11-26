/* the set manager, the calls on a task's placement, and the reader of set
 * files */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#ifdef NONMATCHING
/* memcpy, with which this side's set_read_into copies a file */
#include <string.h>
#endif

#include "camera/camera.h"
#include "hud/hud.h"
#include "object/placement.h"
#include "object/set.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/snapshot.h"
#include "system/task.h"

/* the length of placements, which bounds placement_count */
#define PLACEMENTS_MAX 0x5ff

/* the placement range, squared, of a kind with neither KIND_RANGE nor
 * KIND_ALWAYS */
#define PLACE_RANGE (400.0F * 400.0F)
/* the pass sets a placement's leave.range to the squared range it found the
 * placement in, plus LEAVE_MARGIN. the margin keeps a task given at the edge
 * of that range from leaving it at once. it adds to a square: at 400 it
 * moves the edge out by about 0.125 */
#define LEAVE_MARGIN 100.0F
/* a KIND_ALWAYS kind's range. plus LEAVE_MARGIN it gives leave.range 0, so
 * the task never leaves range */
#define ALWAYS_RANGE (-LEAVE_MARGIN)
/* the leave range of a task without a placement, squared */
#define LEAVE_RANGE (410.0F * 410.0F)

#define SET_LEVEL 0 /* the manager task's level */
/* the manager task's modes */
#define SET_PASSING 1
#define SET_SWAPPING 2
/* a swap's submodes, one a step */
#define SWAP_RELEASE 0
#define SWAP_WAIT 1 /* a step that does nothing */
#define SWAP_COMPACT 2
#define SWAP_ADD 3

/* a size shifted right by BLOCK_SHIFT counts njMemCopy32's 32-byte blocks */
#define BLOCK_SHIFT 5

/* where set_read reads each set file: its head, then its entries */
#define READ_HEAD ((struct set_head *)WORK_AREA)
#define READ_ENTRIES ((struct set_entry *)(READ_HEAD + 1))

/* FALSE from set_queue until its swap ends */
static Bool set_settled = TRUE;
/* TRUE from set_queue until the manager starts the swap */
static Bool set_queued = FALSE;
Bool set_passed = FALSE;
static struct task *set_task = NULL;

/* the object list, and the set file with its head and its entries, or null
 * without a file. set_head holds the same pointer as set_file. a swap takes
 * set_next_list and set_next_file in place of set_list and set_file. the
 * placements in use are the first placement_count.
 *
 * stg42's placement_wrap.c declares set_list, placement_count and placements
 * itself. SHC lays out bss in the order it first meets each name. an extern
 * in a header would thus move placement_count and placements ahead of the
 * statics */
struct object_list *set_list; /* NOLINT(misc-use-internal-linkage) */
static struct set_head *set_file;
static struct set_head *set_head;
static struct set_entry *set_entries;
static struct object_list *set_next_list;
static struct set_head *set_next_file;
Sint16 placement_count; /* NOLINT(misc-use-internal-linkage) */
struct placement
    placements[PLACEMENTS_MAX]; /* NOLINT(misc-use-internal-linkage) */

static void set_clear(void);
static void set_destroy(struct task *task);
static void set_step(struct task *task);
static void set_start_queued_swap(struct task_work *work);
static void set_take_file(struct set_head *head);
static Bool point_near_either(const Float *point, Float x, Float y, Float z,
                              Float range, const Float *other);
static Bool point_near(const Float *point, Float x, Float y, Float z,
                       Float range);
static void set_pass_two_players(void);
static void set_pass_one_player(void);
static void set_pass(void);
static void set_swap(struct task_work *work);
static void placements_clear(void);
static void placements_add(void);
static void placements_release(void);
static void placements_compact(void);
static Bool placement_far_from_camera_by(struct task *task, Float range);
static Bool placement_try_leave_saving_by(struct task *task, Float range);

/* no header declares these, and nothing calls them. as statics, SHC would
 * leave them out */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* swap to list and head over the manager's next steps. the swap drops each
 * placement without PLACEMENT_HELD, and has its live task destroy itself. it
 * then adds a placement for each of head's entries. it tallies no rings */
void set_queue(struct object_list *list, struct set_head *head);
/* TRUE where the camera lies out of task's range, which placement_try_leave
 * defines, and FALSE otherwise. task's work and camera_task_work must exist */
Bool placement_far_from_camera(struct task *task);
/* placement_try_leave, but a task that leaves first saves its pose, where
 * syMalloc has room for it. the placement's next task starts in that pose.
 * task must have a placement */
Bool placement_try_leave_saving(struct task *task);
/* calls on task->placement, which each needs. placement_held returns
 * PLACEMENT_HELD where it is set, and 0 otherwise. placement_get_kind returns
 * the entry's kind, its index in the object list's kinds */
void placement_clear_touched(struct task *task);
void placement_clear_unk_20(struct task *task);
void placement_hold(struct task *task);
int placement_held(struct task *task);
Uint16 placement_get_kind(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

#ifdef NONMATCHING
/* the kind of entry's id in list, or null where the id lies past list's kinds,
 * which the matching side's lookups index unchecked */
struct object_kind *set_entry_kind(const struct object_list *list,
                                   const struct set_entry *entry)
{
    int index;

    if (entry == NULL || list == NULL || list->kinds == NULL) {
        return NULL;
    }
    index = entry->id & SET_KIND;
    if (index >= list->count) {
        return NULL;
    }
    return &list->kinds[index];
}
#endif

void set_init(void)
{
}

void set_exit(void)
{
}

static void set_clear(void)
{
    set_settled = TRUE;
    set_queued = FALSE;
    set_passed = FALSE;
    set_task = NULL;
    set_list = NULL;
    set_file = NULL;
    set_head = NULL;
    set_entries = NULL;
    set_next_list = NULL;
    set_next_file = NULL;
    placements_clear();
}

Bool set_start(struct object_list *list, struct set_head *head)
{
    set_clear();
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((set_task = task_create(TASK_WORK, SET_LEVEL, set_step, "SetObject")) ==
        NULL) {
        return FALSE;
    }
    set_task->destructor = set_destroy;
    set_list = list;
    set_take_file(head);
    set_task->work->mode = SET_PASSING;
    return TRUE;
}

Bool set_end(void)
{
    task_destroy(set_task);
    return TRUE;
}

static void set_destroy(struct task *task)
{
    set_clear();
}

static void set_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case SET_PASSING:
        set_pass();
        set_start_queued_swap(work);
        break;
    case SET_SWAPPING:
        set_swap(work);
        break;
    /* @hack redundant: a default arm that returns */
    default:
        return;
    }
}

static void set_start_queued_swap(struct task_work *work)
{
    if (set_queued) {
        work->mode = SET_SWAPPING;
        work->submode = SWAP_RELEASE;
        set_queued = FALSE;
    }
}

/* make head the set file, which may be null, and start the placements over
 * from it */
static void set_take_file(struct set_head *head)
{
    placements_clear();
    set_file = head;
    /* @hack spelling: the file read back from set_file, where head would
     * do */
    if (set_file == NULL) {
        set_head = NULL;
        set_entries = NULL;
    } else {
        set_head = set_file;
        set_entries = (struct set_entry *)(set_file + 1);
    }
    placements_add();
    set_tally();
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* TRUE where point lies within range of (x, y, z), or of other where other
 * isn't null. FALSE otherwise. range is squared. a NaN passes each test, as
 * point_near says */
static Bool point_near_either(const Float *point, Float x, Float y, Float z,
                              Float range, const Float *other)
{
    Float px;
    Float py;
    Float pz;
    Float sum;

    /* @hack type: point and other as Float cursors that walk from a point's
     * x to its z, where const NJS_POINT3 * read by member would do */
    px = *point++;
    py = *point++;
    pz = *point;
    x -= px;
    /* @hack spelling: the first sum kept in x, where sum for both would
     * do */
    x = x * x;
    if (!(x > range)) {
        y -= py;
#ifdef NONMATCHING
        /* SHC adds each square below by FMAC, which rounds the sum once */
        x = __builtin_fmaf(y, y, x);
#else
        x += y * y;
#endif
        if (!(x > range)) {
            z -= pz;
#ifdef NONMATCHING
            /* SHC adds this square by FMAC too */
            x = __builtin_fmaf(z, z, x);
#else
            x += z * z;
#endif
            if (!(x > range)) {
                return TRUE;
            }
        }
    }
    if (other != NULL) {
        px -= *other++;
        sum = px * px;
        if (sum > range) {
            return FALSE;
        }
        py -= *other++;
#ifdef NONMATCHING
        /* SHC adds each square below by FMAC, which rounds the sum once */
        sum = __builtin_fmaf(py, py, sum);
#else
        sum += py * py;
#endif
        if (sum > range) {
            return FALSE;
        }
        /* @hack implicit: pz's read and its -= unsequenced, which SHC orders
         * with the -= first, where pz -= *other; sum += pz * pz would say
         * so */
#ifdef NONMATCHING
        /* the -= first, as SHC orders it, then the square added by FMAC, as SHC
         * adds it */
        pz -= *other;
        sum = __builtin_fmaf(pz, pz, sum);
#else
        sum += (pz -= *other) * pz; /* NOLINT(clang-diagnostic-unsequenced) */
#endif
        /* @hack spelling: the last test as an if that returns FALSE, before
         * a return of TRUE that the test of (x, y, z) shares, where return
         * !(sum > range) would do */
        if (sum > range) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/* TRUE where point lies within range of (x, y, z), and FALSE otherwise.
 * range is squared. each test is that a sum isn't above range, which a NaN
 * passes: the image tests by FCMP/GT and a branch, where a <= would fail a
 * NaN under GCC. so a task whose place holds a NaN counts as near, and
 * placement_try_leave keeps it */
static Bool point_near(const Float *point, Float x, Float y, Float z,
                       Float range)
{
    Float px;
    Float py;
    Float pz;
    Float sum;

    /* @hack type: point as a Float cursor that walks from a point's x to its
     * z, where const NJS_POINT3 * read by member would do */
    px = *point++;
    py = *point++;
    pz = *point;
    x -= px;
    sum = x * x;
    if (!(sum > range)) {
        y -= py;
#ifdef NONMATCHING
        /* SHC adds each square below by FMAC, which rounds the sum once */
        sum = __builtin_fmaf(y, y, sum);
#else
        sum += y * y;
#endif
        if (!(sum > range)) {
            z -= pz;
#ifdef NONMATCHING
            /* SHC adds this square by FMAC too */
            sum = __builtin_fmaf(z, z, sum);
#else
            sum += z * z;
#endif
            /* @hack spelling: the last test as an if that returns TRUE,
             * where return !(sum > range) would do */
            if (!(sum > range)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* @hack type: player_work seen as a struct of two slots, where
 * player_work[0] and player_work[1] would do */
struct player_pair {
    struct task_work *work[2];
};

/* the pass, measured from player 0 and player 1 */
static void set_pass_two_players(void)
{
    struct player_pair *players;
    /* @hack redundant: register on x, y, z, other, placement, entry, kind,
     * task, work, flags, in_range and i */
    register Float x;
    register Float y;
    register Float z;
    Float range;
    register Float *other;
    register struct placement *placement;
    register struct set_entry *entry;
    register struct object_kind *kind;
    register struct task *task;
    register struct task_work *work;
    register int flags;
    int saved;
    register Bool in_range;
    int attr;
    register int i;

    players = (struct player_pair *)player_work;
    if (players->work[0] == NULL) {
        if (camera_task_work == NULL) {
            return;
        }
        x = camera_task_work->pos.x;
        y = camera_task_work->pos.y;
        z = camera_task_work->pos.z;
    } else {
        x = players->work[0]->pos.x;
        y = players->work[0]->pos.y;
        z = players->work[0]->pos.z;
    }
    /* @hack spelling: a while whose body is a goto, and two labels after it,
     * where if (players->work[1] != NULL) with an else would do */
    while (players->work[1] == NULL) {
        goto no_other;
    }
    other = &players->work[1]->pos.x;
    goto have_other;
no_other:
    other = NULL;
have_other:
    placement = placements;
    for (i = placement_count - 1; i >= 0; placement++, i--) {
        flags = placement->flags;
        /* @hack spelling: flags >= 0 for !(flags & PLACEMENT_ACTIVE), the
         * same test of a Sint16's sign bit, where that would do */
        if (flags >= 0 || (flags & PLACEMENT_LIVE)) {
            continue;
        }
        entry = placement->entry;
#ifdef NONMATCHING
        /* this side skips an entry whose id lies past the list's kinds */
        kind = set_entry_kind(set_list, entry);
        if (kind == NULL) {
            continue;
        }
#else
        kind = &set_list->kinds[entry->id & SET_KIND];
#endif
        saved = flags & PLACEMENT_SAVED;
        /* @hack spelling: a do { } while (0) whose breaks skip the distance,
         * where a goto past the distance would do */
        do {
            attr = kind->attr;
            if (attr & KIND_RANGE) {
                range = kind->range;
            } else if (attr & KIND_ALWAYS) {
                range = ALWAYS_RANGE;
                in_range = TRUE;
                break;
            } else {
                range = PLACE_RANGE;
            }
            /* @hack redundant: the else after the KIND_FIRST arm's break */
            if ((attr & KIND_FIRST) && !set_passed) {
                in_range = TRUE;
                break;
            } else if (saved == 0) {
                in_range =
                    point_near_either(&entry->pos.x, x, y, z, range, other);
            } else {
                in_range = point_near_either(&placement->leave.pose->pos.x, x,
                                             y, z, range, other);
            }
        } while (0);
        if (!in_range) {
            continue;
        }
        task = task_create(kind->flags, kind->level, kind->step, kind->name);
        if (task != NULL) {
            placement->tasks++;
            placement->flags |= PLACEMENT_LIVE;
            task->placement = placement;
            work = task->work;
            if (work != NULL) {
                if (saved) {
                    work->pos = placement->leave.pose->pos;
                    work->ang = placement->leave.pose->ang;
                    work->scl = placement->leave.pose->scl;
                    syFree(placement->leave.pose);
                    placement->flags &= ~PLACEMENT_SAVED;
                } else {
                    /* @hack spelling: the points copied through a cast,
                     * where work->pos = entry->pos would do */
                    work->pos = *(NJS_POINT3 *)&entry->pos.x;
                    work->ang.x = entry->ang_x;
                    work->ang.y = entry->ang_y;
                    work->ang.z = entry->ang_z;
                    work->scl = *(NJS_POINT3 *)&entry->scl.x;
                }
            }
            placement->leave.range = range + LEAVE_MARGIN;
            placement->task = task;
        }
    }
    /* @hack redundant: set_passed set here as well as in set_pass */
    set_passed = TRUE;
}

/* the pass, measured from player 0 alone */
static void set_pass_one_player(void)
{
    /* @hack redundant: register on every local but saved */
    register Float x;
    register Float y;
    register Float z;
    register Float range;
    register struct placement *placement;
    register struct set_entry *entry;
    register struct object_kind *kind;
    register struct task *task;
    register struct task_work *work;
    int saved;
    register int attr;
    register Bool in_range;
    register int flags;
    register int i;

    if (player_work[0] == NULL) {
        if (camera_task_work == NULL) {
            return;
        }
        x = camera_task_work->pos.x;
        y = camera_task_work->pos.y;
        z = camera_task_work->pos.z;
    } else {
        x = player_work[0]->pos.x;
        y = player_work[0]->pos.y;
        z = player_work[0]->pos.z;
    }
    placement = placements;
    for (i = placement_count - 1; i >= 0; placement++, i--) {
        flags = placement->flags;
        /* @hack spelling: flags >= 0 for !(flags & PLACEMENT_ACTIVE), the
         * same test of a Sint16's sign bit, where that would do */
        if (flags >= 0 || (flags & PLACEMENT_LIVE)) {
            continue;
        }
        entry = placement->entry;
#ifdef NONMATCHING
        /* this side skips an entry whose id lies past the list's kinds */
        kind = set_entry_kind(set_list, entry);
        if (kind == NULL) {
            continue;
        }
#else
        kind = &set_list->kinds[entry->id & SET_KIND];
#endif
        saved = flags & PLACEMENT_SAVED;
        /* @hack spelling: a do { } while (0) whose breaks skip the distance,
         * where a goto past the distance would do */
        do {
            attr = kind->attr;
            if (attr & KIND_RANGE) {
                range = kind->range;
            } else if (attr & KIND_ALWAYS) {
                range = ALWAYS_RANGE;
                in_range = TRUE;
                break;
            } else {
                range = PLACE_RANGE;
            }
            /* @hack redundant: the else after the KIND_FIRST arm's break */
            if ((attr & KIND_FIRST) && !set_passed) {
                in_range = TRUE;
                break;
            } else if (saved == 0) {
                in_range = point_near(&entry->pos.x, x, y, z, range);
            } else {
                in_range =
                    point_near(&placement->leave.pose->pos.x, x, y, z, range);
            }
        } while (0);
        if (!in_range) {
            continue;
        }
        task = task_create(kind->flags, kind->level, kind->step, kind->name);
        if (task != NULL) {
            placement->tasks++;
            placement->flags |= PLACEMENT_LIVE;
            task->placement = placement;
            work = task->work;
            if (work != NULL) {
                if (saved) {
                    work->pos = placement->leave.pose->pos;
                    work->ang = placement->leave.pose->ang;
                    work->scl = placement->leave.pose->scl;
                    syFree(placement->leave.pose);
                    placement->flags &= ~PLACEMENT_SAVED;
                } else {
                    /* @hack spelling: the points copied through a cast,
                     * where work->pos = entry->pos would do */
                    work->pos = *(NJS_POINT3 *)&entry->pos.x;
                    work->ang.x = entry->ang_x;
                    work->ang.y = entry->ang_y;
                    work->ang.z = entry->ang_z;
                    work->scl = *(NJS_POINT3 *)&entry->scl.x;
                }
            }
            placement->leave.range = range + LEAVE_MARGIN;
            placement->task = task;
        }
    }
}

static void set_pass(void)
{
    if (player_work[1] == NULL) {
        set_pass_one_player();
    } else {
        set_pass_two_players();
    }
    set_passed = TRUE;
}

static void set_swap(struct task_work *work)
{
    switch (work->submode) {
    case SWAP_RELEASE:
        set_list = set_next_list;
        set_next_list = NULL;
        set_file = set_next_file;
        set_next_file = NULL;
        if (set_file == NULL) {
            set_head = NULL;
            set_entries = NULL;
        } else {
            set_head = set_file;
            set_entries = (struct set_entry *)(set_file + 1);
        }
        placements_release();
        break;
    case SWAP_COMPACT:
        placements_compact();
        break;
    case SWAP_ADD:
        set_settled = TRUE;
        work->mode = SET_PASSING;
        placements_add();
        break;
    }
    work->submode++;
}

static void placements_clear(void)
{
    /* @hack spelling: placement, a cursor that walks beside i, where
     * placements[i] would do */
    struct placement *placement;
    /* @hack type: i as Sint16, where an int would do */
    Sint16 i;

    placement_count = 0;
    placement = &placements[PLACEMENTS_MAX - 1];
    for (i = PLACEMENTS_MAX - 1; i >= 0; i--) {
        placement->tasks = 0;
        placement->state = 0;
        placement->flags = 0;
        placement->entry = NULL;
        /* @bug memory: drops each saved pose without freeing it, though the
         * resets free one. set_start and set_end clear the placements here,
         * and so leak the poses. in this source only
         * placement_try_leave_saving_by saves a pose, and no word in any
         * image holds its address or that of placement_try_leave_saving.
         * MOV.L R0,@(12,R5) at 0x8c057614 stores the null */
        placement->leave.pose = NULL;
        placement--;
    }
}

/* add an active placement for each of the file's entries, after those in
 * use, up to PLACEMENTS_MAX */
static void placements_add(void)
{
#ifdef NONMATCHING
    /* this side repairs the tag below: it adds placements only below
     * PLACEMENTS_MAX, and none past the new count */
    struct placement *placement;
    Uint32 i;

    if (set_entries == NULL) {
        return;
    }
    for (i = 0; i < set_head->count && placement_count < PLACEMENTS_MAX; i++) {
        placement = &placements[placement_count++];
        placement->flags |= PLACEMENT_ACTIVE;
        placement->flags &= ~(PLACEMENT_TOUCHED | PLACEMENT_USED);
        placement->tasks = 0;
        placement->state = 0;
        placement->entry = &set_entries[i];
    }
#else
    struct set_entry *entry;
    /* @hack spelling: placement, a cursor that walks beside i, where
     * placements[i] would do */
    struct placement *placement;
    Uint32 entries;
    int end;
    int i;

    if (set_entries == NULL) {
        return;
    }
    entry = set_entries;
    entries = set_head->count;
    if (placement_count + entries > PLACEMENTS_MAX) {
        end = PLACEMENTS_MAX;
    } else {
        end = placement_count + entries;
    }
    placement = &placements[placement_count];
    /* @bug memory: the loop runs through end, one placement past the new
     * count. where end is PLACEMENTS_MAX, it writes that placement past
     * placements, over the datum at 0x8c2574e8. where end is less, no pass
     * reaches that placement. CMP/GT R7,R6 at 0x8c057666 lets i reach end */
    for (i = placement_count; i <= end; i++) {
        placement->flags |= PLACEMENT_ACTIVE;
        placement->flags &= ~(PLACEMENT_TOUCHED | PLACEMENT_USED);
        placement->tasks = 0;
        placement->state = 0;
        placement->entry = entry;
        entry++;
        placement++;
    }
    placement_count = end;
#endif
}

/* make each placement without PLACEMENT_HELD inactive, and have its live
 * task destroy itself on its next step */
static void placements_release(void)
{
    int i;

    for (i = 0; i < placement_count; i++) {
        if (!(placements[i].flags & PLACEMENT_HELD)) {
            /* @bug memory: clears PLACEMENT_SAVED without freeing the pose,
             * and placements_compact frees a pose only under that flag, so
             * the pose leaks. no swap runs in the game: no word in any image
             * holds the address of set_queue, which alone starts one. AND
             * R14,R0 at 0x8c057712 clears the flag, and TST #2,R0 at
             * 0x8c0577a8 tests it */
            placements[i].flags &=
                ~(PLACEMENT_ACTIVE | PLACEMENT_TOUCHED | PLACEMENT_UNK_20 |
                  PLACEMENT_KEEP | PLACEMENT_USED | PLACEMENT_SAVED);
            if (placements[i].flags & PLACEMENT_LIVE) {
                placements[i].task->step = task_destroy;
            }
        }
    }
}

/* move the held placements to the front, in order, and clear the rest */
static void placements_compact(void)
{
    struct placement *to;
    int j;
    int i;

    j = 0;
    for (i = 0; i < placement_count; i++) {
        /* @bug memory: takes the task of every held placement for live.
         * where task_destroy has freed that task, the copy writes into the
         * freed task. where that task saved its pose, the clear below frees
         * the pose the copy keeps. no swap runs in the game, as
         * placements_release says. MOV.L R4,@(28,R1) at 0x8c0577a4 writes
         * into the task, and JSR @R3 at 0x8c0577bc frees the pose */
        if (placements[i].flags & PLACEMENT_HELD) {
            placements[i].flags &= ~PLACEMENT_HELD;
            to = &placements[j];
            *to = placements[i];
            to->task->placement = to;
            j++;
        }
        /* @bug logic: clears placements[i] even where j equals i, and the copy
         * was onto itself. a held placement with no unheld one ahead of it
         * thus loses its flags and its entry. no swap runs in the game, as
         * placements_release says. MOV.W R0,@(2,R11) at 0x8c0577ae clears the
         * flags */
        /* @hack spelling: the two clears in each arm, where one pair after
         * an if that frees a saved pose would do */
        if (!(placements[i].flags & PLACEMENT_SAVED)) {
            placements[i].flags = 0;
            placements[i].entry = NULL;
        } else {
            placements[i].flags = 0;
            placements[i].entry = NULL;
            syFree(placements[i].leave.pose);
            placements[i].leave.pose = NULL;
        }
    }
    placement_count = j;
}

void set_reactivate(void)
{
    /* @hack spelling: placement, a cursor that walks beside i, where
     * placements[i] would do */
    struct placement *placement;
    Uint32 entries;
    int end;
    int i;
    /* @hack spelling: flags, a local for the one read of placement->flags,
     * where the flags tested in place would do */
    Sint16 flags;

    if (set_entries != NULL) {
        entries = set_head->count;
        /* @bug logic: counts the file's entries a second time, as
         * placement_count counts them already. the loop thus makes
         * placements past the count active, where no pass reaches them. ADD
         * R4,R5 at 0x8c057804 adds the count */
        if (placement_count + entries > PLACEMENTS_MAX) {
            end = PLACEMENTS_MAX;
        } else {
            end = placement_count + entries;
        }
        placement = placements;
        for (i = 0; i < end; i++) {
            flags = placement->flags;
            if (!(flags & PLACEMENT_KEEP)) {
                placement->flags |= PLACEMENT_ACTIVE;
                placement->flags &= ~PLACEMENT_USED;
                placement->state = 0;
            }
            placement++;
        }
        set_passed = FALSE;
    }
}

void set_reset_but_unk_20(void)
{
    /* @hack spelling: placement, a cursor that walks beside i, where
     * placements[i] would do */
    struct placement *placement;
    Uint32 entries;
    int end;
    int i;
    Sint16 flags;

    if (set_entries != NULL) {
        entries = set_head->count;
        /* @bug logic: counts the file's entries a second time, as in
         * set_reactivate. ADD R4,R5 at 0x8c0578a8 adds the count */
        if (placement_count + entries > PLACEMENTS_MAX) {
            end = PLACEMENTS_MAX;
        } else {
            end = placement_count + entries;
        }
        placement = placements;
        for (i = 0; i < end; i++) {
            flags = placement->flags;
            if (flags & PLACEMENT_SAVED) {
                syFree(placement->leave.pose);
                placement->leave.pose = NULL;
            }
            if (!(flags & PLACEMENT_UNK_20)) {
                placement->flags = PLACEMENT_ACTIVE;
            } else {
                placement->flags =
                    (Sint16)(PLACEMENT_ACTIVE | PLACEMENT_UNK_20);
            }
            placement->state = 0;
            placement++;
        }
        set_passed = FALSE;
    }
}

void set_reset(void)
{
    /* @hack spelling: placement, a cursor that walks beside i, where
     * placements[i] would do */
    struct placement *placement;
    struct set_head *head;
    Uint32 entries;
    Uint32 total;
    int end;
    int i;
    /* @hack spelling: flags, a local for the one read of placement->flags,
     * where the flags tested in place would do */
    Sint16 flags;

    if (set_entries != NULL) {
        /* @hack spelling: the head through a local, where entries read from
         * set_head itself would do */
        head = set_head;
        entries = head->count;
        /* @bug logic: counts the file's entries a second time, as in
         * set_reactivate. ADD R4,R5 at 0x8c057932 adds the count */
        /* @hack spelling: total, the sum set inside the test, where
         * placement_count + entries in the test and in the else would do */
        if ((total = placement_count + entries) > PLACEMENTS_MAX) {
            end = PLACEMENTS_MAX;
        } else {
            end = total;
        }
        placement = placements;
        for (i = 0; i < end; i++) {
            flags = placement->flags;
            if (flags & PLACEMENT_SAVED) {
                syFree(placement->leave.pose);
                placement->leave.pose = NULL;
            }
            placement->flags = PLACEMENT_ACTIVE;
            placement->state = 0;
            placement++;
        }
        set_passed = FALSE;
    }
}

void set_queue(struct object_list *list, struct set_head *head)
{
    set_next_list = list;
    set_next_file = head;
    set_settled = FALSE;
    set_queued = TRUE;
}

Bool placement_far_from_camera(struct task *task)
{
    if (task->placement != NULL) {
        return placement_far_from_camera_by(task, task->placement->leave.range);
    }
    return placement_far_from_camera_by(task, LEAVE_RANGE);
}

/* placement_far_from_camera, with range in place of the placement's
 * leave.range */
static Bool placement_far_from_camera_by(struct task *task, Float range)
{
    struct placement *placement = task->placement;
    struct task_work *work;
    Float dx;
    Float dy;
    Float dz;
    Float distance;

    if (placement != NULL && (placement->flags & PLACEMENT_IN_RANGE)) {
        return FALSE;
    }
    if (range == 0.0F) {
        return FALSE;
    }
    work = task->work;
    dx = work->pos.x - camera_task_work->pos.x;
    dy = work->pos.y - camera_task_work->pos.y;
    dz = work->pos.z - camera_task_work->pos.z;
#ifdef NONMATCHING
    /* SHC's sum: dy's square by FMUL, then dx's and dz's added by FMAC */
    distance = __builtin_fmaf(dz, dz, __builtin_fmaf(dx, dx, dy * dy));
#else
    distance = (dx * dx) + (dy * dy) + (dz * dz);
#endif
    /* @hack spelling: the comparison returned through an if, where return
     * distance > range would do */
    if (distance > range) {
        return TRUE;
    }
    return FALSE;
}

Bool placement_try_leave(struct task *task)
{
    if (task->placement != NULL) {
        return placement_try_leave_by(task, task->placement->leave.range);
    }
    return placement_try_leave_by(task, LEAVE_RANGE);
}

Bool placement_try_leave_by(struct task *task, Float range)
{
    struct placement *placement = task->placement;
    /* @hack spelling: limit, a copy of range, where range itself would do */
    Float limit = range;
    struct task_work *work;
    struct task_work *viewer;
    Float x;
    Float y;
    Float z;

    if (placement != NULL && (placement->flags & PLACEMENT_IN_RANGE)) {
        return FALSE;
    }
    if (limit == 0.0F) {
        return FALSE;
    }
    work = task->work;
    x = work->pos.x;
    y = work->pos.y;
    z = work->pos.z;
    /* @hack spelling: each viewer set inside its test, where a statement ahead
     * of the test would do */
    if (game_state.demo == 0 && (viewer = camera_task_work) != NULL) {
        if (point_near(&viewer->pos.x, x, y, z, limit)) {
            return FALSE;
        }
    }
    if ((viewer = player_work[0]) != NULL) {
        if (point_near(&viewer->pos.x, x, y, z, limit)) {
            return FALSE;
        }
    }
    if ((viewer = player_work[1]) != NULL) {
        if (point_near(&viewer->pos.x, x, y, z, limit)) {
            return FALSE;
        }
    }
    task->step = task_destroy;
    return TRUE;
}

Bool placement_try_leave_saving(struct task *task)
{
    if (task->placement != NULL) {
        return placement_try_leave_saving_by(task,
                                             task->placement->leave.range);
    }
    return placement_try_leave_saving_by(task, LEAVE_RANGE);
}

/* placement_try_leave_saving, with range in place of the placement's
 * leave.range */
static Bool placement_try_leave_saving_by(struct task *task, Float range)
{
    struct placement *placement = task->placement;
    struct placement_pose *pose;
    struct task_work *work;

    /* @hack redundant: the PLACEMENT_IN_RANGE test, which
     * placement_try_leave_by makes again first */
    if (placement == NULL || !(placement->flags & PLACEMENT_IN_RANGE)) {
        if (placement_try_leave_by(task, range)) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((pose = syMalloc(sizeof(struct placement_pose))) != NULL) {
                work = task->work;
                pose->pos = work->pos;
                pose->ang = work->ang;
                pose->scl = work->scl;
                /* @bug memory: stores through task->placement, which is null
                 * for a task without one. the game never gets here: no word
                 * in any image holds the address of this function or of
                 * placement_try_leave_saving. MOV.L R4,@(12,R1) at 0x8c057baa
                 * stores through it */
                /* @hack spelling: task->placement read again for each store,
                 * where placement, which holds it, would do */
                task->placement->leave.pose = pose;
                task->placement->flags |= PLACEMENT_SAVED;
            }
            task->step = task_destroy;
            return TRUE;
        }
    }
    return FALSE;
}

void placement_set_used(struct task *task)
{
    task->placement->flags |= PLACEMENT_USED;
}

void placement_clear_used(struct task *task)
{
    task->placement->flags &= ~PLACEMENT_USED;
}

void placement_keep(struct task *task)
{
    task->placement->flags |= PLACEMENT_KEEP;
}

int placement_used(struct task *task)
{
    return task->placement->flags & PLACEMENT_USED;
}

void placement_set_touched(struct task *task)
{
    task->placement->flags |= PLACEMENT_TOUCHED;
}

void placement_clear_touched(struct task *task)
{
    task->placement->flags &= ~PLACEMENT_TOUCHED;
}

int placement_touched(struct task *task)
{
    return task->placement->flags & PLACEMENT_TOUCHED;
}

void placement_set_unk_20(struct task *task)
{
    task->placement->flags |= PLACEMENT_UNK_20;
}

void placement_clear_unk_20(struct task *task)
{
    task->placement->flags &= ~PLACEMENT_UNK_20;
}

int placement_unk_20(struct task *task)
{
    return task->placement->flags & PLACEMENT_UNK_20;
}

void placement_hold(struct task *task)
{
    task->placement->flags |= PLACEMENT_HELD;
}

int placement_held(struct task *task)
{
    return task->placement->flags & PLACEMENT_HELD;
}

void placement_stop(struct task *task)
{
    /* @hack spelling: one bit cleared at a time, where one mask would do */
    task->placement->flags &= ~PLACEMENT_ACTIVE;
    task->placement->flags &= ~PLACEMENT_LIVE;
}

void placement_kill(struct task *task)
{
    /* @hack spelling: one bit cleared at a time, where one mask would do */
    task->placement->flags &= ~PLACEMENT_ACTIVE;
    task->placement->flags &= ~PLACEMENT_LIVE;
    task->step = task_destroy;
}

Uint16 placement_get_kind(struct task *task)
{
    return task->placement->entry->id & SET_KIND;
}

void placement_set_state(struct task *task, Sint8 state)
{
    if (task->placement != NULL) {
        task->placement->state = state;
    }
}

Sint8 placement_get_state(struct task *task)
{
    if (task->placement != NULL) {
        return task->placement->state;
    }
    /* @bug uninitialized: without a placement, returns the R0 its caller
     * left, as the BT at 0x8c057cd4 skips to the RTS. a caller that doesn't
     * test for a placement first reads it, as EXTS.B R0,R0 at 0x8c06cbf6 does
     * after the call at 0x8c06cbf2 */
#ifdef NONMATCHING
    /* this side keeps the tag's outcome. each caller in 1ST_READ.BIN leaves
     * in R0 the offset it loaded task from, which isn't 0, and 1 gives their
     * tests the same outcome */
    return 1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

#ifdef NONMATCHING
/* whether size is a whole set file's: a head and whole entries, which
 * WORK_AREA holds */
static Bool set_size_valid(Sint32 size)
{
    return size >= (Sint32)sizeof(struct set_head) &&
           (Uint32)size <= (Uint32)(WORK_AREA_END - WORK_AREA) &&
           ((Uint32)size - sizeof(struct set_head)) %
                   sizeof(struct set_entry) ==
               0;
}

/* read the set file name into READ_HEAD, and return its size. it returns -1
 * where the file doesn't read whole, or where the head's count isn't the
 * entries the file holds */
static Sint32 set_read_checked(const char *name)
{
    Sint32 size =
        loader_read_file_checked(name, READ_HEAD, WORK_AREA_END - WORK_AREA);

    if (!set_size_valid(size) ||
        READ_HEAD->count != ((Uint32)size - sizeof(struct set_head)) /
                                sizeof(struct set_entry)) {
        return -1;
    }
    return size;
}

Bool set_read_into(const char *name, struct set_head *destination,
                   Uint32 capacity)
{
    Sint32 size;

    if (name == NULL || destination == NULL ||
        capacity < sizeof(struct set_head)) {
        return FALSE;
    }
    size = set_read_checked(name);
    if (size < 0 || (Uint32)size > capacity) {
        return FALSE;
    }
    memcpy(destination, READ_HEAD, (Uint32)size);
    return TRUE;
}

/* this side reads each file whole and checks its size. it repairs the tag in
 * the matching set_read: it adds the u file's own count */
struct set_head *set_read(const char *name_s, const char *name_u,
                          int min_entries)
{
    Sint32 size_s;
    Sint32 size_u = 0;
    Uint32 slots;
    struct set_head *head;
    struct set_entry *append;

    if (name_s == NULL || min_entries < 0) {
        return NULL;
    }
    if (name_u != NULL) {
        size_u = loader_file_size(name_u);
        if (!set_size_valid(size_u)) {
            return NULL;
        }
    }
    size_s = set_read_checked(name_s);
    if (size_s < 0) {
        return NULL;
    }
    slots = ((Uint32)size_s + (Uint32)size_u) / sizeof(struct set_entry);
    if (slots < (Uint32)min_entries) {
        slots = (Uint32)min_entries;
    }
    head = syCalloc(slots, sizeof(struct set_entry));
    if (head == NULL) {
        return NULL;
    }
    njMemCopy32(head, READ_HEAD, size_s >> BLOCK_SHIFT);
    if (name_u != NULL) {
        if (set_read_checked(name_u) != size_u) {
            syFree(head);
            return NULL;
        }
        if (READ_HEAD->count != 0) {
            append = (struct set_entry *)(head + 1) + head->count;
            njMemCopy32(append, READ_ENTRIES, READ_HEAD->count);
            head->count += READ_HEAD->count;
        }
    }
    return head;
}
#else
struct set_head *set_read(const char *name_s, const char *name_u,
                          int min_entries)
{
    int size_s;
    int size_u;
    int size;
    char *buffer;
    struct set_head *head;

    size_s = loader_file_size(name_s);
    if (size_s < 0) {
        return NULL;
    }
    size_u = loader_file_size(name_u);
    if (size_u < 0) {
        return NULL;
    }
    size = min_entries * sizeof(struct set_entry);
    if (size < size_s + size_u) {
        size = size_s + size_u;
    }
    buffer = syCalloc(1, size);
    if (buffer == NULL) {
        return NULL;
    }
    head = (struct set_head *)buffer;
    loader_read_file(name_s, READ_HEAD);
    njMemCopy32(head, READ_HEAD, size_s >> BLOCK_SHIFT);
    /* @hack spelling: buffer = buffer + size_s, where += would do */
    buffer = buffer + size_s;
    if (size_u > (int)sizeof(struct set_head)) {
        loader_read_file(name_u, READ_HEAD);
        njMemCopy32(buffer, READ_ENTRIES,
                    (size_u - (int)sizeof(struct set_head)) >> BLOCK_SHIFT);
    }
    /* @bug logic: where the u file holds no entry, adds the s file's count
     * again, as READ_HEAD still holds that file. MOV.L @R10,R3 at 0x8c057d8e
     * reads that count, and ADD R3,R2 at 0x8c057d90 adds it */
    head->count += READ_HEAD->count;
    return head;
}
#endif

struct set_head *set_read_stage(const char *prefix, int min_entries)
{
    char name_s[64];
    char name_u[64];
    char stage_prefix[32];
    /* @hack spelling: size, a local that holds each loader_file_size for the
     * test after it, where each call in its test would do */
    int size;

    if (prefix == NULL) {
        sprintf(stage_prefix, "set%04d", stage_current_id());
        prefix = stage_prefix;
    }
    if (game_state.two_players) {
        sprintf(name_s, "%s_2p_s.bin", prefix);
        size = loader_file_size(name_s);
        if (size < 0) {
            sprintf(name_s, "%s_s.bin", prefix);
        }
        sprintf(name_u, "%s_2p_u.bin", prefix);
        size = loader_file_size(name_u);
        if (size < 0) {
            sprintf(name_u, "%s_u.bin", prefix);
        }
    } else if (game_state.mission == MISSION_HD) {
        sprintf(name_s, "%s_hd_s.bin", prefix);
        size = loader_file_size(name_s);
        if (size < 0) {
            sprintf(name_s, "%s_s.bin", prefix);
        }
        sprintf(name_u, "%s_hd_u.bin", prefix);
        size = loader_file_size(name_u);
        if (size < 0) {
            sprintf(name_u, "%s_u.bin", prefix);
        }
    } else {
        sprintf(name_s, "%s_s.bin", prefix);
        sprintf(name_u, "%s_u.bin", prefix);
    }
    return set_read(name_s, name_u, min_entries);
}

void set_tally(void)
{
    struct set_entry *entry;
#ifdef NONMATCHING
    /* this side has no kinds local: set_entry_kind bounds each lookup */
    struct object_kind *kind;
#else
    /* @hack spelling: kinds, a local for set_list->kinds, where
     * set_list->kinds read in place would do */
    struct object_kind *kinds;
#endif
    /* @hack spelling: count, a local for the head's count, where
     * (int)set_head->count in the loop's test would do */
    int count;
    int total;
    int ring_bits;
    int i;

    ring_total_restore();
    total = game_state.ring_total;
    if (set_list != NULL && set_entries != NULL) {
#ifdef NONMATCHING
        /* set_entry_kind reads the kinds on this side */
        entry = set_entries;
#else
        kinds = set_list->kinds;
        entry = set_entries;
#endif
        count = set_head->count;
        for (i = 0; i < count; i++, entry++) {
#ifdef NONMATCHING
            /* this side skips an entry whose id lies past the list's kinds */
            kind = set_entry_kind(set_list, entry);
            if (kind == NULL) {
                continue;
            }
            ring_bits = kind->attr & KIND_RINGS;
#else
            ring_bits = kinds[entry->id & SET_KIND].attr & KIND_RINGS;
#endif
            /* @hack redundant: a test of ring_bits ahead of the switch,
             * which has no case for 0 */
            if (ring_bits != 0) {
                switch (ring_bits) {
                case RINGS_ONE:
                    total += 1;
                    break;
                case RINGS_SCL_Z:
                    total += entry->scl.z;
                    break;
                case RINGS_BOX:
                    switch ((int)entry->scl.x) {
                    case BOX_5_RINGS:
                        total += 5;
                        break;
                    case BOX_10_RINGS:
                        total += 10;
                        break;
                    case BOX_20_RINGS:
                        total += 20;
                        break;
                    }
                    break;
                }
            }
        }
        game_state.ring_total = total;
        snapshot_current->ring_total = total;
    }
}
