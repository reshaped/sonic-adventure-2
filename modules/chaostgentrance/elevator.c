#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "chao/race_record.h"
#include "chaostgentrance/fixture_art.h"
#include "chaostgentrance/entrance.h"
#include "collision/world.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"

enum elevator_phase {
    ELEVATOR_PARKED,
    ELEVATOR_RISING,
    ELEVATOR_SETTLING,
    ELEVATOR_WAITING,
    ELEVATOR_AT_TOP
};

/* the work holds each door's distance from the shaft's middle, from SHUT_X
 * to OPEN_X. it holds the floor's height in the shaft too, from LOW_Y to 0 */
#define DOOR_X(work) ((work)->scl.x)
#define FLOOR_Y(work) ((work)->scl.y)
#define SHUT_X 12.0F
#define OPEN_X 24.0F
#define LOW_Y (-20.0F)
#define OPEN_SPEED 0.12F
#define RISE_SPEED 0.06F
/* once the floor stops, it shakes until work->timer passes SHAKE_FRAMES. a
 * frame's shake is a random height from -SHAKE_SIZE / 2 to SHAKE_SIZE / 2.
 * work->timer then counts again, and the gate opens once it passes
 * GATE_DELAY */
#define SHAKE_FRAMES 6
#define SHAKE_SIZE 0.1F
#define GATE_DELAY 600
/* from ELEVATOR_WAITING on, the step lifts each racer below LOWEST_RACER_Y
 * to it, and sets it on the floor */
#define LOWEST_RACER_Y (-0.1F)
/* the floor takes its size class from its model's radius plus
 * CLASS_MARGIN */
#define CLASS_MARGIN 5.0F
#define ELEVATOR_Z (-75.5F)

#define START_SOUND 2
#define RISE_SOUND 0
#define STOP_SOUND 1
#define RISE_PRIORITY 1
#define RISE_VOLUME 30
/* sound_play_at_for's duration, in sound updates */
#define RISE_UPDATES 10

static struct task *elevator;

static void elevator_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *floor;
    NJS_POINT3 *pos;
    int slot;
    switch (work->mode) {
    case ELEVATOR_PARKED:
        break;
    case ELEVATOR_RISING:
        FLOOR_Y(work) += RISE_SPEED;
        if (FLOOR_Y(work) > 0.0F) {
            FLOOR_Y(work) = 0.0F;
            work->timer = 0;
            work->mode = ELEVATOR_SETTLING;
            sound_play(STOP_SOUND, NULL, 0, 0);
        }
        DOOR_X(work) += OPEN_SPEED;
        if (DOOR_X(work) > OPEN_X) {
            DOOR_X(work) = OPEN_X;
        }
        sound_play_at_for(RISE_SOUND, task, RISE_PRIORITY, RISE_VOLUME,
                          RISE_UPDATES, &work->pos);
        break;
    case ELEVATOR_SETTLING:
        FLOOR_Y(work) =
            ((rand() * (1.0F / (RAND_MAX + 1))) - 0.5F) * SHAKE_SIZE;
        if (work->timer++ > SHAKE_FRAMES) {
            work->timer = 0;
            FLOOR_Y(work) = 0.0F;
            entrance_release_racers();
            work->mode = ELEVATOR_WAITING;
        }
        break;
    case ELEVATOR_WAITING:
        if (work->timer++ > GATE_DELAY) {
            entrance_gate_open();
            work->mode = ELEVATOR_AT_TOP;
        }
        break;
    }
    floor = task->motion.p;
    floor->pos[1] = work->pos.y + FLOOR_Y(work);
    if (work->mode >= ELEVATOR_WAITING) {
        for (slot = 0; slot < RACE_ENTRANTS; slot++) {
            if (entrance_racers[slot]) {
                pos = &entrance_racers[slot]->work->pos;
                if (pos->y < LOWEST_RACER_Y) {
                    pos->y = LOWEST_RACER_Y;
                    entrance_racers[slot]->work->flags |= MOVER_WORK_ON_FLOOR;
                    ((struct mover *)entrance_racers[slot]->motion.p)->flags |=
                        MOVER_ON_FLOOR;
                }
            }
        }
    }
}

static void elevator_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&entrance_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njCnkEasyDrawModel(&entrance_elevator_shaft);
    njPushMatrixEx();
    njTranslate(NULL, -DOOR_X(work), 0, 0);
    njCnkEasyDrawModel(&entrance_elevator_left_door);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, DOOR_X(work), 0, 0);
    njCnkEasyDrawModel(&entrance_elevator_right_door);
    njPopMatrixEx();
    njTranslate(NULL, 0, FLOOR_Y(work), 0);
    njCnkEasyDrawModel(&entrance_elevator_floor);
    njPopMatrixEx();
}

static void elevator_destroy(struct task *task)
{
    elevator = NULL;
}

void entrance_elevator_start(void)
{
    if (elevator) {
        elevator->work->mode = ELEVATOR_RISING;
        sound_play(START_SOUND, NULL, 0, 0);
    }
}

void entrance_elevator_create(void)
{
    struct task *task = task_create(TASK_WORK, 2, elevator_step,
                                    "ALO_EntranceElevatorExecutor");
    struct task_work *work;
    NJS_OBJECT *floor;
    Uint32 flags;
    Float radius;
    if (task) {
        elevator = task;
        work = task->work;
        task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
        if (!task->motion.p) {
            /* @bug memory: after a failed allocation of the floor,
             * entrance_elevator_start at 0x0c9cb57e writes the mode of a
             * freed task. the failure at 0x0c9cb616 kills the task, which
             * elevator, set at 0x0c9cb5fe, still names. it returns before the
             * destructor that would clear elevator is set */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it clears elevator
             * before the failure kills the task */
            elevator = NULL;
#endif
            task_kill(task);
            return;
        }
        work->pos.x = 0;
        work->pos.y = 0;
        work->pos.z = ELEVATOR_Z;
        DOOR_X(work) = SHUT_X;
        FLOOR_Y(work) = LOW_Y;
        floor = task->motion.p;
        *floor = entrance_elevator_collision;
        floor->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        floor->pos[0] = work->pos.x;
        floor->pos[1] = work->pos.y;
        floor->pos[2] = work->pos.z;
        flags = WORLD_MOVING | WORLD_SOLID;
        radius = floor->model->r + CLASS_MARGIN;
        if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
            if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(1);
            } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(2);
            } else {
                flags = WORLD_MOVING | WORLD_SOLID | WORLD_SIZE_CLASS(3);
            }
        }
        world_add(flags, task, floor);
        task->draw = elevator_draw;
        task->destructor = elevator_destroy;
    }
}
