/* the platforms that ride BOSS_GOLEM.PRS's Egg Golem. A, B and C ride
 * GOLEM_SENSOR_LOW's node, and head_parts.c's three ride GOLEM_SENSOR_HEAD's */
#ifndef BOSS_GOLEM_PLATFORMS_H
#define BOSS_GOLEM_PLATFORMS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

#define GOLEM_PLATFORMS 3

/* a platform's modes, each of which goes on to the next, and the last to the
 * first. near is as golem_player_near tests it. while the golem's camera is
 * GOLEM_CAMERA_FIXED, a platform neither slides out nor slides in */
enum golem_platform_mode {
    /* slide out home, then wait. while the golem's power is 0 or less, the
     * platform holds still in this mode */
    GOLEM_PLATFORM_MODE_RETURN,
    /* wait at home until player 0 stands on the platform while near */
    GOLEM_PLATFORM_MODE_WAIT,
    /* slide in to in_z, then hold there */
    GOLEM_PLATFORM_MODE_RETRACT,
    /* hold until the mode has run 181 steps, and then until player 0 isn't
     * near */
    GOLEM_PLATFORM_MODE_IN
};
/* a platform's next_mode while no mode is requested */
#define GOLEM_PLATFORM_NO_MODE (-1)

/* a platform's record, in its task's extra. the task's motion holds its object
 * in the collision world. the platform stands at (x, 0, z) in its node's axes.
 * it lies on the line from its home to the node's origin. a step of retracting
 * moves it in by a share of its home's offset. that share is retract_base, plus
 * retract_rate for each unit of the golem's power. a step of returning moves it
 * out by return_rate of the offset */
struct golem_platform {
    enum golem_platform_mode mode;
    /* the mode the next step takes, or GOLEM_PLATFORM_NO_MODE */
    Sint32 next_mode;
    Sint32 mode_steps; /* the steps the mode has run */
    Sint32 index; /* which of the three on its node, from 0 */
    Sint32 sensor; /* the golem's sensor whose node the platform rides */
    /* a copy of its work's id, by which GOLEM_PLATFORM_MODE_WAIT finds player
     * 0 on the platform */
    Sint32 id;
    Float retract_rate;
    Float retract_base;
    Float return_rate;
    Float z;
    Float home_z;
    Float in_z;
    Float x;
    Float home_x;
    /* where a head platform's target rides, in the platform's axes */
    NJS_POINT3 target_pos;
    Bool glows; /* TRUE where the draw lights the model's child */
    /* TRUE while a head platform's target is down, which puts out the glow.
     * target_down_steps counts the steps since it went down */
    Bool target_down;
    Sint32 target_down_steps;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *texlist;
};
#define GOLEM_PLATFORM(task) ((struct golem_platform *)(task)->extra.p)

/* create platforms A, B and C as golem_task's children. each takes a copy of
 * its model in the collision world, and waits at home. where an allocation
 * fails, the call writes through null */
#ifdef NONMATCHING
/* this side returns FALSE where an allocation fails, and TRUE otherwise. the
 * tasks made by then stay golem_task's children */
Bool golem_platforms_create(struct task *golem_task);
#else
void golem_platforms_create(struct task *golem_task);
#endif

/* place the platforms' objects in the collision world, as golem_sensor_place
 * does. GOLEM_SENSOR_LOW's node's matrix must be the current one */
void golem_platforms_place(void);

/* the step, destructor and draw of a platform's task, which head_parts.c's
 * platforms take too. the destructor frees the record. it takes the object out
 * of the collision world and gives it back to its pool */
void golem_platform_step(struct task *task);
void golem_platform_destroy(struct task *task);
void golem_platform_draw(struct task *task);
/* start mode now, in place of any next_mode. its start runs, and mode_steps
 * counts from 0 */
void golem_platform_set_mode(struct task *task, int mode);
/* set *pos to where a head platform's target rides, in object's axes. that is
 * 5 back along z from the pos of object's child */
void golem_platform_target_pos(NJS_CNK_OBJECT *object, NJS_POINT3 *pos);

/* the tasks of platforms A, B and C */
extern struct task *golem_platform_tasks[GOLEM_PLATFORMS];

#endif
