/* Exercise the scheduler through the level and child task interfaces. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/timer.h"

int camera_current_index;
struct game_state game_state;

static int allocations;
static int fail_record;
static char visited[64];
static unsigned int visits;
static char trigger;
static void (*change)(struct task *task);
static struct task *target;
static int profiles_started;
static int profiles_finished;

void *syMalloc(Uint32 size)
{
    void *block = malloc(size);

    assert(block != NULL);
    allocations++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;

    if (fail_record != 0 && --fail_record == 0) {
        return NULL;
    }
    block = calloc(count, size);
    assert(block != NULL);
    allocations++;
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && allocations > 0);
    allocations--;
    free(block);
}

void sound_detach_task(const struct task *task)
{
    assert(task != NULL && task->step == NULL);
}

void collision_free(struct task *task)
{
    abort(); /* No task in this test registers a collision. */
}

void profile_step_start(void (*step)(struct task *), const char *name)
{
    assert(task_current_call == step);
    profiles_started++;
}

void profile_step_end(void (*step)(struct task *))
{
    profiles_finished++;
}

void profile_draw_start(void (*draw)(struct task *))
{
    assert(task_current_call == draw);
    profiles_started++;
}

void profile_draw_end(void (*draw)(struct task *))
{
    profiles_finished++;
}

static void visit(struct task *task)
{
    assert(task_current_call == visit && visits + 1 < sizeof(visited));
    visited[visits++] = task->name[0];
    visited[visits] = '\0';
    if (change != NULL && task->name[0] == trigger) {
        void (*callback)(struct task *) = change;

        change = NULL;
        callback(task);
    }
}

static struct task *add(struct task *parent, const char *name)
{
    struct task *task;

    if (parent == NULL) {
        task = task_create(0, 2, visit, name);
    } else {
        task = task_create_child(0, visit, parent);
        assert(task != NULL);
        task->name = name;
    }
    assert(task != NULL);
    task->draw = visit;
    return task;
}

static void expect(const char *order)
{
    assert(strcmp(visited, order) == 0);
    assert(profiles_started == profiles_finished && task_current_call == NULL);
    visits = 0;
    visited[0] = '\0';
}

static void empty(void)
{
    task_destroy_all();
    assert(allocations == 0 && task_levels[2] == NULL);
}

static void remove_target(struct task *task)
{
    task_destroy(target);
}

static void append_sibling(struct task *task)
{
    add(task->parent, "x");
}

static void check_walk(void (*walk)(void))
{
    struct task *parent;
    struct task *child;

    parent = add(NULL, "a");
    child = add(parent, "b");
    add(child, "c");
    add(parent, "d");
    add(NULL, "e");
    walk();
    expect("abcde");

    /* A child removes its own branch, then the level continues. */
    target = parent;
    trigger = 'c';
    change = remove_target;
    walk();
    expect("abce");
    empty();

    parent = add(NULL, "a");
    add(parent, "b");
    target = add(NULL, "c");
    add(NULL, "d");
    trigger = 'b';
    change = remove_target;
    walk();
    expect("abd");
    empty();

    /* A new sibling participates if the walk has yet to reach the tail. */
    add(NULL, "a");
    add(NULL, "b");
    trigger = 'a';
    change = append_sibling;
    walk();
    expect("abx");
    trigger = 'x';
    change = append_sibling;
    walk();
    expect("abx");
    walk();
    expect("abxx");
    empty();
}

static void check_deferred(void)
{
    struct task *parent;
    struct task *child;

    parent = add(NULL, "a");
    child = add(parent, "b");
    add(NULL, "c");
    parent->step = task_destroy_children;
    task_kill(child);
    task_draw_all();
    expect("abc");
    task_step_all();
    expect("c");
    assert(parent->step == task_destroy_children && parent->child == NULL);
    task_step_all();
    expect("c");
    assert(parent->step == task_destroy);
    task_step_all();
    expect("c");
    assert(task_levels[2]->next == task_levels[2]);
    empty();
}

static void spawn_in_destructor(struct task *task)
{
    assert(task_current_call == spawn_in_destructor);
    add(NULL, "x");
}

static void check_teardown(void)
{
    struct task *parent;
    struct task *child;
    int record;

    parent = add(NULL, "a");
    child = add(parent, "b");
    target = add(NULL, "c");
    add(NULL, "d");
    child->destructor = remove_target;
    task_kill(parent);
    task_step_all();
    expect("d");
    task_levels[2]->destructor = spawn_in_destructor;
    task_destroy_level(2);
    assert(allocations == 0 && task_levels[2] == NULL);

    parent = add(NULL, "a");
    for (record = 1; record <= 4; record++) {
        fail_record = record;
        child = task_create_child(
            TASK_WORK | TASK_MOTION | TASK_FORCE | TASK_EXTRA, visit, parent);
        assert(child == NULL && parent->child == NULL && allocations == 1);
    }
    empty();
}

int main(void)
{
    task_init();
    check_walk(task_step_all);
    check_walk(task_draw_all);
    check_deferred();
    check_teardown();
    return 0;
}
