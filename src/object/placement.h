/* placements: what the set manager keeps for each entry of a stage's set
 * files. set.h defines the manager's pass, which gives the placements their
 * tasks */
#ifndef OBJECT_PLACEMENT_H
#define OBJECT_PLACEMENT_H
#include <sg_xpt.h>

struct placement_pose;
struct set_entry;
struct task;

/* a placement's flags */
/* the manager counts the placement's task as live. the pass sets the flag as
 * it gives the placement a task, and task_destroy clears it. placement_stop
 * and placement_kill clear it too, even while the task runs. so do the calls
 * that reset, start or end the manager */
#define PLACEMENT_LIVE 0x0001
/* leave.pose holds how the placement's last task stood as it left range. the
 * pass measures the placement's range from there, and starts its next task in
 * that pose */
#define PLACEMENT_SAVED 0x0002
/* placement_set_used sets it for the placement's next task to read, and a
 * task's end leaves it. placement_clear_used and set_reactivate clear it, and
 * so do the calls that reset, start or end the manager */
#define PLACEMENT_USED 0x0004
/* the range tests find the placement's task in range wherever it is. they are
 * placement_try_leave, placement_try_leave_saving, placement_far_from_camera
 * and their _by forms */
#define PLACEMENT_IN_RANGE 0x0008
/* set_reactivate leaves the placement as it is */
#define PLACEMENT_KEEP 0x0010
/* a bit the placement's tasks set and test for their own use */
#define PLACEMENT_UNK_20 0x0020
/* a player has touched the placement's save point: a save point whose
 * placement has it starts passed */
#define PLACEMENT_TOUCHED 0x0040
/* a swap to another set file keeps the placement and its task, and then
 * clears the flag. only set_queue starts a swap, and nothing calls it */
#define PLACEMENT_HELD 0x4000
/* while PLACEMENT_LIVE is clear, the pass gives the placement a task once it
 * comes in range */
#define PLACEMENT_ACTIVE 0x8000

/* the set manager's record of one entry */
struct placement {
    Sint8 tasks; /* how many tasks the pass has given it */
    /* a byte the placement keeps for its tasks, from one to the next */
    Sint8 state;
    Sint16 flags;
    /* the task it was given last, which lives while PLACEMENT_LIVE is set */
    struct task *task;
    struct set_entry *entry;
    union {
        /* while the task lives, the squared distance past which it leaves
         * range */
        Float range;
        /* while PLACEMENT_SAVED is set, the pose its next task starts in */
        struct placement_pose *pose;
    } leave;
};

#endif
