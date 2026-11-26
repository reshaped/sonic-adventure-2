/* the lights objects cast on the players */
#ifndef SHARED_OBJECT_LIGHT_H
#define SHARED_OBJECT_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct light;
struct task;

/* the most lights a player's queue holds */
#define OBJECT_LIGHT_MOST_QUEUED 16
/* lights[OBJECT_LIGHT_BLEND + player] holds player's blend */
#define OBJECT_LIGHT_BLEND 6

/* destroy the task, if there is one */
void object_light_destroy(void);
/* destroy the task there was, if any, then create the task in level 1 and
 * return it. where the task or its queues can't be allocated, it returns null
 * and leaves no task. object_light_init is object_light_create without its
 * result, and no code in the game's images calls object_light_init.
 *
 * the task keeps a queue of lights for each player. each step blends a player's
 * queue with lights[player] into lights[OBJECT_LIGHT_BLEND + player]. it
 * selects that light for the player, and empties the queue. where a player's
 * queue is empty and its blend is selected, the step selects lights[player]
 * again. the step stops at the first player without a work.
 *
 * only object_light_destroy and object_light_create may destroy the task. any
 * other destroy, as of the task's whole level, leaves object_light.c's pointer
 * to the freed task. those two calls and the add calls then use it */
struct task *object_light_create(void);
void object_light_init(void);
/* queue a copy of light for player, and return the copy. the caller may change
 * the copy until the task's next step blends it. the copy is invalid after that
 * step, or once the task ends. it returns null without a task, for a player
 * past 1, and for a full queue. player must not be negative. no code in the
 * game's images calls object_light_add */
struct light *object_light_add(const struct light *light, int player);
/* queue a copy of light as object_light_add does, for a player whose work lies
 * nearer pos than range. the copy's dir is the unit vector from pos toward the
 * work's pos. its color is light's times (range - d) / range. d is the work's
 * distance from pos. it returns null where object_light_add does. it also
 * returns null for a player without a work, and for one range or more away */
struct light *object_light_add_near(const struct light *light, int player,
                                    const NJS_POINT3 *pos, Float range);
/* turn light's dir about y and about x, each by a random angle in [-30, 30)
 * degrees. scale its color by a random factor in [0.5, 1). Ninja's matrix stack
 * must have room for one more matrix */
void object_light_jitter(struct light *light);
/* Load the stage's LIGHTS records. The playable build keeps the current
 * lights if the file is missing, incomplete, or the wrong size. The original
 * copies the reported byte count and requires the file to fit lights. */
Bool lights_load(const char *name);

#endif
