/* the parts that ride the head of BOSS_GOLEM.PRS's Egg Golem, the drops of its
 * flash and its rubble */
#ifndef BOSS_GOLEM_HEAD_PARTS_H
#define BOSS_GOLEM_HEAD_PARTS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the head's parts: its three platforms, then its model */
#define GOLEM_HEAD_PLATFORMS 3
#define GOLEM_HEAD_PARTS (GOLEM_HEAD_PLATFORMS + 1)

/* create the head's parts as golem_task's children, each with a copy of its
 * model in the collision world. each platform takes a target as its child: a
 * mark for a homing attack. a hit on a target puts it and its platform's glow
 * out for 180 steps. where a part's allocation fails, the call or a later step
 * writes through null */
#ifdef NONMATCHING
/* this side returns FALSE where an allocation fails, and TRUE otherwise. the
 * parts made by then stay golem_task's children */
Bool golem_head_parts_create(struct task *golem_task);
#else
void golem_head_parts_create(struct task *golem_task);
#endif

/* place the objects of the head's parts in the collision world, as
 * golem_sensor_place does. GOLEM_SENSOR_HEAD's node's matrix must be the
 * current one. player 0's task and work must not be null, unchecked. while
 * Sonic homes in on platform A's target, that platform's object leaves the
 * world. a homing attack on platform B's or C's target leaves every object in
 * the world */
void golem_head_parts_place(void);

/* a flash drop's kinds. a GOLEM_FLASH_DROP_HURTS drop hurts players. a
 * GOLEM_FLASH_DROP_FAINT drop draws dimmer, plays quieter and hurts nothing */
enum golem_flash_drop_kind { GOLEM_FLASH_DROP_HURTS, GOLEM_FLASH_DROP_FAINT };
/* create a flash drop of kind, as parent's child, and return its task. the
 * drop starts 100 above the head sensor's place, and plays the flash's sound.
 * it falls, spreads and fades, then kills its task. where the heap can't give
 * the task or its record, the call writes through null */
struct task *golem_flash_drop_create(struct task *parent, int kind);

/* throw rubble from the place of the golem's sensor, as golem_task's child.
 * a throw needs the sensor to have moved more than 2 since its last place. a
 * chance of 0.0333 must pass too. no call throws while golem's rubble_count is
 * above 32. the count gains each piece thrown and loses each that ends.
 * golem_create leaves the count as the heap gave it, and the non-matching
 * side's golem_create starts it at 0. a piece ends once it drops below y 0 or
 * flies its life. the task kills itself once every piece has ended. where the
 * heap can't give the task or its pieces, nothing is thrown */
void golem_rubble_throw(struct task *golem_task, int sensor);

/* the tasks of the head's parts, and of the platforms' targets */
extern struct task *golem_head_tasks[GOLEM_HEAD_PARTS];
extern struct task *golem_head_targets[GOLEM_HEAD_PLATFORMS];

#endif
