/* the background's task of Sonic's route */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

/* the rotor's angle as its turn began, and how far the turn takes it. both
 * come before the includes, so that SHC places them ahead of core_flicker */
static Angle turn_from;
static Angle turn_travel;

#include "camera/camera.h"
#include "sound/sound.h"
#include "stg34/ambient_levels.h"
#include "stg34/background.h"
#include "stg34/scenery.h"
#include "stg34/scenery_art.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of the background's task */
#define BACKGROUND_LEVEL 1
/* the rotor starts turning every TURN_PERIOD steps, and turns for TURN_STEPS.
 * each time it turns by a random whole number of EIGHTH_TURN, from -8 to 7 */
#define TURN_PERIOD 60
#define TURN_STEPS 40
#define TURN_CHOICES 16

/* MACHINERY_SOUND plays under two keys, and so in two slots. in the room and
 * by the shaft its key is the task. in the drop and on the lower route its key
 * is SECOND_KEY(task). a step that plays it gives it a time of SOUND_STEPS + 1
 * again */
#define MACHINERY_SOUND 5
#define SOUND_PRIORITY 1
#define SOUND_STEPS 10
#define SECOND_KEY(task) ((char *)(task) + 1)
/* the camera hears the room's machinery while it stands inside the room. the
 * room's floor lies ROOM_DEPTH below y 0, and its top core_room_height above
 * the floor */
#define ROOM_DEPTH 530.0F
#define ROOM_MIN_X (-610.0F)
#define ROOM_MAX_X (-410.0F)
#define ROOM_MIN_Z (-2650.0F)
#define ROOM_MAX_Z (-2150.0F)
/* the shaft past the drop. its axis runs SHAFT_LENGTH down from SHAFT_TOP,
 * and its side lies core_shaft_radius from the axis. the camera hears its
 * machinery within core_shaft_falloff of the side */
#define SHAFT_X (-510.0F)
#define SHAFT_TOP (-1150.0F)
#define SHAFT_Z (-4450.0F)
#define SHAFT_LENGTH 500.0F
/* the drop lies past DROP_Z toward -z, or below DROP_Y. the lower route lies
 * past LOWER_Z */
#define DROP_Z (-3500.0F)
#define DROP_Y (-850.0F)
#define LOWER_Z (-5200.0F)

Float core_flicker[CORE_FLICKER_GROUPS];

static void background_step(struct task *task);

void core_sonic_background_init(void)
{
    struct task *background =
        task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (background) {
        background->draw = core_scenery_draw;
    }
}

void core_sonic_background_start(void)
{
    core_rotor_turning = ROTOR_RESTING;
    core_rotor_tick = 0;
    turn_from = turn_travel = 0;
    core_rotor_angle = 0;
    core_entry_trigger_start();
    core_pillars_start(&core_pillar_upper, LAND_BLOCKS(8, 9));
    core_pillars_start(&core_pillar_lower, LAND_BLOCKS(9, 10));
    core_fans_start(core_rotor_array, LAND_BLOCKS(4, 6));
    core_gate_start(LAND_BLOCKS(5, 6));
}

static void background_step(struct task *task)
{
    /* @hack redundant: a read of task's work that nothing uses */
    struct task_work *work = task->work;
    int group;
    Float rate;
    Float distance;
    int volume;

    if (!game_state.time_stopped) {
        for (group = 0; group < CORE_FLICKER_GROUPS; ++group) {
            core_flicker[group] = rand() * RAND_SCALE;
        }
        switch (core_rotor_turning) {
        case ROTOR_RESTING:
            if (core_rotor_tick % TURN_PERIOD == 0) {
                turn_travel = ((int)(rand() * (TURN_CHOICES * RAND_SCALE)) -
                               (TURN_CHOICES / 2)) *
                              EIGHTH_TURN;
                turn_from = core_rotor_angle;
                core_rotor_turning = ROTOR_TURNING;
            }
            break;
        case ROTOR_TURNING:
            core_rotor_angle += turn_travel * (1.0F / TURN_STEPS);
            if (core_rotor_tick % TURN_PERIOD == TURN_STEPS) {
                core_rotor_angle = turn_from + turn_travel;
                core_rotor_turning = ROTOR_RESTING;
            }
        }
        ++core_rotor_tick;
    }
    if (!game_state.time_stopped) {
        NJS_POINT3 pos;
        pos = camera_views[camera_current_index]->pos;
        if (pos.y >= -ROOM_DEPTH && pos.y < core_room_height - ROOM_DEPTH &&
            ROOM_MIN_X < pos.x && pos.x < ROOM_MAX_X && ROOM_MIN_Z < pos.z &&
            pos.z < ROOM_MAX_Z) {
            volume = ((core_room_top_volume - core_room_floor_volume) /
                      core_room_height * (pos.y + ROOM_DEPTH)) +
                     core_room_floor_volume;
            sound_play_for(MACHINERY_SOUND, task, SOUND_PRIORITY, 0,
                           SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, volume);
        }
    }
    if (!game_state.time_stopped) {
        NJS_LINE axis;
        axis.px = SHAFT_X;
        axis.py = SHAFT_TOP;
        axis.pz = SHAFT_Z;
        axis.vx = 0.0F;
        axis.vy = -SHAFT_LENGTH;
        axis.vz = 0.0F;
        distance = njSqrt(point_segment_distance2(
                       &camera_views[camera_current_index]->pos, &axis, NULL)) -
                   core_shaft_radius;
        if (distance <= core_shaft_falloff) {
            rate = (core_shaft_near_volume - core_shaft_far_volume) /
                   core_shaft_falloff;
            rate *= core_shaft_falloff - distance;
            volume = rate + core_shaft_far_volume;
            sound_play_for(MACHINERY_SOUND, task, SOUND_PRIORITY, 0,
                           SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, volume);
        }
    }
    if (!game_state.time_stopped) {
        if (camera_views[camera_current_index]->pos.z < LOWER_Z) {
            sound_play_for(MACHINERY_SOUND, SECOND_KEY(task), SOUND_PRIORITY, 0,
                           SOUND_STEPS);
            sound_set_volume(SECOND_KEY(task), SOUND_ANY_ID, core_lower_volume);
        } else if (camera_views[camera_current_index]->pos.z < DROP_Z ||
                   camera_views[camera_current_index]->pos.y < DROP_Y) {
            sound_play_for(MACHINERY_SOUND, SECOND_KEY(task), SOUND_PRIORITY, 0,
                           SOUND_STEPS);
            sound_set_volume(SECOND_KEY(task), SOUND_ANY_ID, core_drop_volume);
        }
    }
}

/* scenery.c's code starts at the next 32-byte boundary. asmsh pads up to it
 * with NOPs, two after the last pool */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
