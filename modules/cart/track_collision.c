/* the course's solids: a task for each section. each puts its copy of its
 * section's solid in the world while a racer is near */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "cart/course.h"
#include "cart/race.h"
#include "cart/track.h"
#include "cart/vehicle.h"
#include "collision/world.h"
#include "system/task.h"

/* each section's task keeps its section's index, and whether its solid is in
 * the world */
#define SECTION(work) ((work)->timer)
#define IN_WORLD(work) ((work)->tick)
/* a racer stands in the section of its gate. a section's solid is in the world
 * from BEHIND sections behind a racer's to AHEAD ahead. the solids of the first
 * EDGE_SECTIONS sections and of the last EDGE_SECTIONS - 1 never leave the
 * world. on a story course, neither do the last STORY_END_SECTIONS sections'
 * solids */
#define BEHIND 10
#define AHEAD 4
#define EDGE_SECTIONS 5
#define STORY_END_SECTIONS 30
/* the three pieces from SOLIDLESS_PIECE, whose sections get no solid task */
#define SOLIDLESS_PIECE 25

static Bool stopping = 1;
static void collision_step(struct task *task);
static void copy_solid(struct task *task);
static void release_solid(struct task *task);
static void enable_solid(struct task *task, Bool enabled);

static Bool skip_solid(int piece)
{
    switch (piece) {
    case SOLIDLESS_PIECE:
    case SOLIDLESS_PIECE + 1:
    case SOLIDLESS_PIECE + 2:
        return TRUE;
    }
    return FALSE;
}

void cart_start_course_collision(void)
{
    int i;
    struct task *task;
    for (i = 0; i < cart_section_count; i++) {
        if (skip_solid(cart_sections[i].piece)) {
            continue;
        }
        task =
            task_create(TASK_WORK, 2, collision_step, "coursePartsEntryExec");
        task->draw = NULL;
        task->destructor = release_solid;
        SECTION(task->work) = i;
        IN_WORLD(task->work) = FALSE;
        copy_solid(task);
    }
    stopping = 0;
}
void cart_stop_course_collision(void)
{
    stopping = 1;
}

static void collision_step(struct task *task)
{
    struct task_work *work;
    int i;
    int section;
    struct cart_vehicle *kart;
    if (stopping) {
        task_destroy(task);
        return;
    }
    work = task->work;
    for (i = 0; i < CART_RACERS; i++) {
        if (!cart_racers[i]) {
            enable_solid(task, FALSE);
            return;
        }
        kart = cart_racers[i]->motion.p;
        section = cart_gate_sections[kart->gate];
        switch (cart_setup.course_mode) {
        case CART_TAILS:
        case CART_ROUGE:
            if (SECTION(work) >= cart_section_count - STORY_END_SECTIONS) {
                enable_solid(task, TRUE);
                return;
            }
            break;
        }
        if (SECTION(work) < EDGE_SECTIONS) {
            enable_solid(task, TRUE);
            return;
        }
        if (SECTION(work) > cart_section_count - EDGE_SECTIONS) {
            enable_solid(task, TRUE);
            return;
        }
        if (SECTION(work) >= section - BEHIND &&
            SECTION(work) <= section + AHEAD) {
            enable_solid(task, TRUE);
            return;
        }
    }
    enable_solid(task, FALSE);
}
static void copy_solid(struct task *task)
{
    NJS_OBJECT *solid;
    int index;
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    solid = task->motion.p;
    index = SECTION(task->work);
#ifdef NONMATCHING
    /* this side ends the task where the heap can't give a copy of the solid.
     * the matching side copies through the null */
    if (!solid) {
        task_kill(task);
        return;
    }
#endif
    *solid = *cart_sections[index].solid;
    solid->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    solid->ang[0] = 0;
    solid->ang[1] = cart_sections[index].yaw;
    solid->ang[2] = 0;
    solid->pos[0] = cart_sections[index].pos.x;
    solid->pos[1] = cart_sections[index].pos.y;
    solid->pos[2] = cart_sections[index].pos.z;
}
static void release_solid(struct task *task)
{
    enable_solid(task, FALSE);
    syFree(task->motion.p);
    task->motion.p = NULL;
}
static void enable_solid(struct task *task, Bool enabled)
{
    struct task_work *work;
    NJS_OBJECT *solid;
    work = task->work;
    solid = task->motion.p;
    if (enabled) {
        /* @hack spelling: IN_WORLD tested against FALSE here and TRUE below,
         * where truth tests would do */
        if (IN_WORLD(work) == FALSE) {
            world_add(WORLD_SIZE_CLASS(1) | WORLD_SOLID, task, solid);
            IN_WORLD(work) = TRUE;
        }
    } else if (IN_WORLD(work) == TRUE) {
        world_remove(task, solid);
        IN_WORLD(work) = FALSE;
    }
}
