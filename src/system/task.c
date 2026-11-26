/* tasks: the game's objects */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/placement.h"
#ifdef NONMATCHING
/* sound_detach_task, which this side's task_destroy calls */
#include "sound/sound.h"
#endif
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/timer.h"

/* task_step_all and task_draw_all walk levels 0 to 5 */
#define TASK_WALKED_LEVELS 6
/* the first of the levels task_draw_all can leave out, 2 to 5.
 * task_destroy_levels_2_to_5 empties the same levels */
#define TASK_SKIPPED_LEVEL 2

/* a child for task_create_children to create. a table of specs ends at one
 * with a null step. flags, ang and pos set the child's work. a child without
 * a work ignores them */
struct task_spec {
    void (*step)(struct task *task);
    Uint16 create; /* task_create's flags */
    Sint16 flags; /* the work's flags */
    struct angles ang; /* added to the parent's angles */
    NJS_POINT3 pos; /* turned by the parent's angles, then added to its pos */
    Sint32 unk_20;
};

static struct task *task_alloc(int level, void (*step)(struct task *task),
                               const char *name);
static void task_draw_level(int level);
static void task_free_work(struct task *task);
#ifdef NONMATCHING
/* the step and the draw of one task, which walk_tasks calls on each task of a
 * level and of its children, in place of the walks below */
static void task_step_one(struct task *task);
static void task_draw_one(struct task *task);
#else
static void task_draw_children(struct task *task);
static void task_step_children(struct task *task);
#endif
/* no other object calls these, and nothing calls task_get_current_call or
 * task_create_children. SHC leaves out a static function that nothing calls,
 * and one whose every call it expands in place */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*task_get_current_call(void))(struct task *task);
struct task_work *task_alloc_work(void);
void *task_alloc_motion(void);
void *task_alloc_force(void);
void *task_alloc_extra(void);
void task_free_motion(struct task *task);
void task_free_force(struct task *task);
void task_free_extra(struct task *task);
struct task *task_create_children(struct task_spec *spec, struct task *parent);
/* NOLINTEND(misc-use-internal-linkage) */

/* the initializer keeps task_current_call in D, where the image holds it */
void (*task_current_call)(struct task *task) = NULL;
struct task *task_levels[TASK_LEVELS];

#ifdef NONMATCHING
/* Each active level or child ring has its own cursor. Unlinking updates
 * every cursor before the heap can reuse that task's address. */
struct task_walk {
    struct task_walk *outer;
    struct task *first;
    struct task *current;
    struct task *next;
};
static struct task_walk *active_walk;

static struct task *next_in_walk(const struct task_walk *walk,
                                 const struct task *task)
{
    return task->next == walk->first ? NULL : task->next;
}

static void remove_from_walks(struct task *task)
{
    struct task_walk *walk;

    for (walk = active_walk; walk != NULL; walk = walk->outer) {
        if (walk->current == task) {
            walk->current = NULL;
        }
        if (walk->next == task) {
            walk->next = next_in_walk(walk, task);
        }
        if (walk->first == task) {
            walk->first = task->next == task ? NULL : task->next;
        }
    }
}

static void walk_tasks(struct task *first, void (*visit)(struct task *task))
{
    struct task_walk walk;

    walk.outer = active_walk;
    active_walk = &walk;
    walk.first = first;
    for (walk.current = first; walk.current != NULL; walk.current = walk.next) {
        /* As in the original, a task that appends a sibling when it is
         * already last leaves that sibling for the next walk. */
        walk.next = next_in_walk(&walk, walk.current);
        visit(walk.current);
        if (walk.current != NULL && walk.current->child != NULL) {
            walk_tasks(walk.current->child, visit);
        }
    }
    active_walk = walk.outer;
}
#endif

void task_idle(struct task *task)
{
}

void (*task_get_current_call(void))(struct task *task)
{
    return task_current_call;
}

void task_init(void)
{
    int level;

    for (level = 0; level < TASK_LEVELS; level++) {
        task_levels[level] = NULL;
    }
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    struct task *task;
    void *record;

    task = task_alloc(level, step, name);
    if (task == NULL) {
        return NULL;
    }
    if (flags & TASK_WORK) {
        record = task_alloc_work();
        if (record == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->work = record;
    }
    if (flags & TASK_FORCE) {
        record = task_alloc_force();
        if (record == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->force.p = record;
    }
    if (flags & TASK_MOTION) {
        record = task_alloc_motion();
        if (record == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->motion.p = record;
    }
    if (flags & TASK_EXTRA) {
        record = task_alloc_extra();
        if (record == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = record;
    }
    return task;
}

/* create and link a task as task_create does, without records */
static struct task *task_alloc(int level, void (*step)(struct task *task),
                               const char *name)
{
    struct task *task;
    struct task *head;
    struct task *tail;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = syMalloc(sizeof(struct task))) == NULL) {
        return NULL;
    }
    if (step != NULL) {
        task->step = step;
    } else {
        task->step = task_idle;
    }
    task->placement = NULL;
    task->work = NULL;
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
    task->draw = NULL;
    task->destructor = NULL;
    task->child = NULL;
    task->parent = NULL;
    /* @bug uninitialized: reads task->name before anything sets it, as
     * syMalloc left the block (MOV.L @(48,R4),R3 at 0x8c05fd62) */
#ifdef NONMATCHING
    /* this side repairs the tag above: unk_34 starts null */
    task->unk_34 = NULL;
#else
    task->unk_34 = task->name;
#endif
    task->name = name;
    task->unk_3c.l = 0;
#ifdef NONMATCHING
    /* this side repairs the tag in task_destroy: an unlinked task links to
     * itself, so the destroy a failed record makes unlinks nothing */
    task->next = task;
    task->prev = task;
#endif
    if (level != TASK_UNLINKED) {
        if (task_levels[level] == NULL) {
            task->next = task;
            task->prev = task;
            task_levels[level] = task;
        } else {
            head = task_levels[level];
            tail = head->prev;
            task->prev = tail;
            task->next = head;
            head->prev = task;
            tail->next = task;
        }
    }
    return task;
}

void task_destroy(struct task *task)
{
    struct task *next;
    struct task *prev;
    struct task *parent;
    int level;

    task_current_call = task->destructor;
    if (task->destructor != NULL) {
        task->destructor(task);
    }
    task->step = NULL;
    task->draw = NULL;
    task->destructor = NULL;
    /* @bug memory: writes through next and prev where nothing has set them,
     * when task_create destroys a TASK_UNLINKED task it couldn't give a
     * record. MOV.L @R14,R12 at 0x8c05fdc8 and MOV.L @(4,R14),R11 at
     * 0x8c05fdce read the two as syMalloc left them. the stores at 0x8c05fdd4
     * and 0x8c05fdd6 write through them */
#ifdef NONMATCHING
    /* this side repairs the tags on the walks of task_step_level and
     * task_step_children, and on sound_update's read of a slot's task: it takes
     * the task out of every walk and every sound slot before it frees it. it
     * reads next and prev once the children are gone, as a child's destructor
     * may unlink a neighbor. task_alloc repairs the tag above: this side links
     * an unlinked task to itself */
    if (task->child != NULL) {
        task_destroy_children(task);
    }
    sound_detach_task(task);
    remove_from_walks(task);
    next = task->next;
    prev = task->prev;
#else
    next = task->next;
    prev = task->prev;
    if (task->child != NULL) {
        task_destroy_children(task);
    }
#endif
    next->prev = prev;
    prev->next = next;
    parent = task->parent;
    if (parent != NULL) {
        if (parent->child == task) {
            if (next == task) {
                parent->child = NULL;
            } else {
                parent->child = next;
            }
        }
    } else {
        if (task == next) {
            next = NULL;
        }
        for (level = 0; level < TASK_LEVELS; level++) {
            if (task == task_levels[level]) {
                task_levels[level] = next;
                break;
            }
        }
    }
    if (task->work != NULL) {
        task_free_work(task);
    }
    if (task->force.p != NULL) {
        task_free_force(task);
    }
    if (task->motion.p != NULL) {
        task_free_motion(task);
    }
    if (task->extra.p != NULL) {
        task_free_extra(task);
    }
    if (task->placement != NULL) {
        task->placement->flags &= ~PLACEMENT_LIVE;
        task->placement = NULL;
    }
    task->child = NULL;
    task->parent = NULL;
    task->prev = NULL;
    task->next = NULL;
    syFree(task);
    task_current_call = NULL;
}

void task_kill(struct task *task)
{
    task->step = task_destroy;
}

void task_step_level(int level)
{
#ifdef NONMATCHING
    /* this side repairs the tag below: walk_tasks walks by a cursor that
     * task_destroy moves past the task it frees */
    walk_tasks(task_levels[level], task_step_one);
#else
    struct task **head;
    struct task *task;
    struct task *first;
    struct task *next;
    void (*step)(struct task *task);

    head = &task_levels[level];
    task = *head;
    first = task;
    if (task == NULL) {
        return;
    }
    for (;;) {
        step = task->step;
        next = task->next;
        task_current_call = step;
        profile_step_start(step, task->name);
        if (step != NULL) {
            step(task);
        }
        profile_step_end(step);
        task_current_call = NULL;
        /* @bug memory: reads task's child after its step may have freed it.
         * a killed task's step is task_destroy, which JSR @R13 at 0x8c05fee8
         * calls, and MOV.L @(12,R14),R2 at 0x8c05fef4 then reads the freed
         * task */
        if (task->child != NULL) {
            task_step_children(task);
        }
        if (next == first) {
            break;
        }
        first = *head;
        if (first == NULL) {
            break;
        }
        task = next;
    }
#endif
}

static void task_draw_level(int level)
{
#ifdef NONMATCHING
    /* walk_tasks, as in task_step_level, with each task's draw */
    walk_tasks(task_levels[level], task_draw_one);
#else
    struct task *task;
    struct task *first;
    struct task *next;
    void (*draw)(struct task *task);

    task = task_levels[level];
    if (task == NULL) {
        return;
    }
    first = task;
    do {
        draw = task->draw;
        next = task->next;
        task_current_call = draw;
        profile_draw_start(draw);
        if (draw != NULL) {
            draw(task);
        }
        profile_draw_end(draw);
        task_current_call = NULL;
        if (task->child != NULL) {
            task_draw_children(task);
        }
        task = next;
    } while (task != first);
#endif
}

#ifdef NONMATCHING
/* one task's draw, for walk_tasks */
static void task_draw_one(struct task *task)
{
    void (*draw)(struct task *task) = task->draw;

    task_current_call = draw;
    profile_draw_start(draw);
    if (draw != NULL) {
        draw(task);
    }
    profile_draw_end(draw);
    task_current_call = NULL;
}
#else
static void task_draw_children(struct task *task)
{
    struct task *child;
    struct task *first;
    struct task *next;
    void (*draw)(struct task *task);

    child = task->child;
    if (child == NULL) {
        return;
    }
    first = child;
    do {
        draw = child->draw;
        next = child->next;
        task_current_call = draw;
        profile_draw_start(draw);
        if (draw != NULL) {
            draw(child);
        }
        profile_draw_end(draw);
        task_current_call = NULL;
        if (child->child != NULL) {
            task_draw_children(child);
        }
        child = next;
    } while (child != first);
}
#endif

void task_destroy_level(int level)
{
#ifdef NONMATCHING
    /* this side destroys whatever task heads the level, as a destructor can
     * free the next task the matching side holds */
    while (task_levels[level] != NULL) {
        task_destroy(task_levels[level]);
    }
#else
    struct task **head;
    struct task *task;
    struct task *next;

    head = &task_levels[level];
    task = *head;
    /* @hack spelling: a do loop under a test of its own, where while (*head
     * != NULL) would do */
    if (task != NULL) {
        do {
            next = task->next;
            task_destroy(task);
            task = next;
        } while (*head != NULL);
    }
#endif
}

void task_step_all(void)
{
    int level;

    for (level = 0; level < TASK_WALKED_LEVELS; level++) {
        task_step_level(level);
    }
}

void task_draw_all(void)
{
    int level;

    task_draw_level(0);
    task_draw_level(1);
    if (game_state.hidden_cameras == HIDDEN_CAMERA_0 &&
        camera_current_index == 0) {
        return;
    }
    if (game_state.hidden_cameras == HIDDEN_CAMERA_1 &&
        camera_current_index == 1) {
        return;
    }
    if (game_state.hidden_cameras == (HIDDEN_CAMERA_0 | HIDDEN_CAMERA_1)) {
        return;
    }
    for (level = TASK_SKIPPED_LEVEL; level < TASK_WALKED_LEVELS; level++) {
        task_draw_level(level);
    }
}

void task_destroy_levels_2_to_5(void)
{
    int level;

    for (level = TASK_SKIPPED_LEVEL; level < TASK_WALKED_LEVELS; level++) {
        task_destroy_level(level);
    }
}

void task_destroy_all(void)
{
    int level;

    for (level = 0; level < TASK_LEVELS; level++) {
        task_destroy_level(level);
    }
}

struct task_work *task_alloc_work(void)
{
    /* @hack spelling: the record through a local, not returned in place */
    struct task_work *work;

    work = syCalloc(1, sizeof(struct task_work));
    return work;
}

void *task_alloc_motion(void)
{
    /* @hack spelling: the record through a local, not returned in place */
    void *motion;

    motion = syCalloc(1, sizeof(struct task_motion));
    return motion;
}

void *task_alloc_force(void)
{
    /* @hack spelling: the records through a local, not returned in place */
    void *force;

    force = syCalloc(TASK_FORCES, sizeof(struct task_force));
    return force;
}

void *task_alloc_extra(void)
{
    /* @hack spelling: the record through a local, not returned in place */
    void *extra;

    extra = syCalloc(1, TASK_EXTRA_SIZE);
    return extra;
}

static void task_free_work(struct task *task)
{
    if (task->work->collision != NULL) {
        collision_free(task);
    }
    syFree(task->work);
    task->work = NULL;
}

void task_free_motion(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
}

void task_free_force(struct task *task)
{
    syFree(task->force.p);
    task->force.p = NULL;
}

void task_free_extra(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent)
{
    struct task *child;
    struct task *head;
    struct task *tail;
    /* @hack spelling: to and from, locals for the two works, where
     * child->work and parent->work read in place would do */
    struct task_work *to;
    struct task_work *from;

    child = task_create(flags, TASK_UNLINKED, step, NULL);
    /* @hack spelling: the null return in an else, where an early return
     * would do */
    if (child != NULL) {
        if (parent->child != NULL) {
            head = parent->child;
            tail = head->prev;
            head->prev = child;
            tail->next = child;
            child->next = head;
            child->prev = tail;
        } else {
            parent->child = child;
            child->prev = child;
            child->next = child;
        }
        child->parent = parent;
        to = child->work;
        from = parent->work;
        if (to != NULL && from != NULL) {
            to->pos = from->pos;
            to->ang = from->ang;
        }
    } else {
        return NULL;
    }
    return child;
}

/* create a child for each spec, after parent's children and in spec order,
 * until a task_create fails. parent must have a work if any child does,
 * unchecked. returns the first child created, or null if none was */
struct task *task_create_children(struct task_spec *spec, struct task *parent)
{
    struct task_work *parent_work;
    struct task_work *work;
    struct task *child;
    struct task *first;
    struct task *head;
    struct task *tail;
    NJS_POINT3 point;
    Uint16 flags;

    parent_work = parent->work;
    first = NULL;
    njPushMatrix(&_nj_unit_matrix_);
    if (parent_work != NULL) {
        njTranslateV(NULL, &parent_work->pos);
        njRotateZ(NULL, parent_work->ang.z);
        njRotateX(NULL, parent_work->ang.x);
        njRotateY(NULL, parent_work->ang.y);
    }
    for (; spec->step != NULL; spec++) {
        /* @hack spelling: the flags through a local, not passed as
         * spec->create */
        flags = spec->create;
        child = task_create(flags, TASK_UNLINKED, spec->step, NULL);
        if (child == NULL) {
            break;
        }
        if (first == NULL) {
            first = child;
        }
        if (parent->child != NULL) {
            head = parent->child;
            tail = head->prev;
            head->prev = child;
            tail->next = child;
            child->next = head;
            child->prev = tail;
        } else {
            parent->child = child;
            child->prev = child;
            child->next = child;
        }
        child->parent = parent;
        work = child->work;
        if (work != NULL) {
            work->flags = spec->flags;
            njCalcVector(NULL, &spec->pos, &point);
            njAddVector(&point, &parent_work->pos);
            njAddVector(&work->pos, &point);
            work->ang.x = parent_work->ang.x + spec->ang.x;
            work->ang.y = parent_work->ang.y + spec->ang.y;
            work->ang.z = parent_work->ang.z + spec->ang.z;
        }
    }
    njPopMatrix(1);
    return first;
}

void task_destroy_children(struct task *task)
{
    if (task->step == task_destroy_children) {
        if (task->child == NULL) {
            task->step = task_destroy;
        }
        return;
    }
    while (task->child != NULL) {
        task_destroy(task->child);
    }
}

#ifdef NONMATCHING
/* this side repairs the tag below: walk_tasks steps the children by a cursor
 * that task_destroy moves past the task it frees */

/* one task's step, for walk_tasks */
static void task_step_one(struct task *task)
{
    void (*step)(struct task *task) = task->step;

    task_current_call = step;
    profile_step_start(step, task->name);
    if (step != NULL) {
        step(task);
    }
    profile_step_end(step);
    task_current_call = NULL;
}
#else
static void task_step_children(struct task *task)
{
    struct task *child;
    struct task *first;
    struct task *next;
    void (*step)(struct task *task);

    child = task->child;
    first = child;
    if (child == NULL) {
        return;
    }
    for (;;) {
        step = child->step;
        next = child->next;
        task_current_call = step;
        profile_step_start(step, child->name);
        if (step != NULL) {
            step(child);
        }
        profile_step_end(step);
        task_current_call = NULL;
        /* @bug memory: reads child's child after its step may have freed it,
         * as task_step_level reads a task's. JSR @R13 at 0x8c0603f0 calls the
         * step, and MOV.L @(12,R14),R3 at 0x8c0603fc reads the freed task */
        if (child->child != NULL) {
            task_step_children(child);
        }
        /* @hack spelling: first == next, where next == first would do, as
         * task_step_level tests it */
        /* @hack redundant: the else and its test of first against
         * task->child, where first = task->child alone would do */
        if (first == next || task->child == NULL) {
            break;
        } else if (first != task->child) {
            first = task->child;
        }
        child = next;
    }
}
#endif
