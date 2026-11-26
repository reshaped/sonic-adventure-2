/* the camera's GAKUGAKU mode, and camera_settle_step, the test that pushes
 * it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/gakugaku.h"
#include "camera/modes.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

/* the mode's first step moves a probe of PROBE_RADIUS from PROBE_RISE above
 * the player toward the camera */
#define PROBE_RISE 10.0F
#define PROBE_RADIUS 1.0F

/* each camera's last CAMERA_SETTLE_PLACES places, the count of places it
 * has kept, and its delay */
static NJS_POINT3 settle_places[CAMERAS][CAMERA_SETTLE_PLACES];
static Sint32 settle_counts[CAMERAS];
static Sint32 settle_delays[CAMERAS];

void camera_mode_gakugaku(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_probe probe;
    NJS_VECTOR line;
    NJS_POINT3 *pos = &stack->place.pos;
    int top;

    if (slot->steps == 0) {
        probe.pos = player_work[camera_current_index]->pos;
        probe.pos.y += PROBE_RISE;
        probe.next = *pos;
        probe.skip = 0;
        probe.radius = PROBE_RADIUS;
        if (camera_probe_move_two_sided(&probe)) {
            vector_diff(&probe.next, &probe.pos, &line);
            if (vector_angle(&line, &probe.slides[0].normal, NULL) <
                QUARTER_TURN) {
                *pos = probe.hit;
            }
        }
    }
    /* @hack spelling: top, a local for the top in the first and the last
     * test, where the call in the argument would do, as the second test
     * has it */
    if (!camera_settle_step(camera_current_index)) {
        top = camera_top(camera_current_index);
        camera_remove_mode(camera_current_index, top);
    }
    /* @bug logic: takes off the mode under GAKUGAKU too, where an earlier
     * test has taken GAKUGAKU off: each remove takes off whatever is on top.
     * while a motion plays, the first test and the last both hold, since
     * camera_settle_step returns 0 then. the removes are at 0x8c0b7c72,
     * 0x8c0b7c8e and 0x8c0b7ca0 */
    if (player_pad_records[camera_current_index].on &
        (PDD_DGT_TR | PDD_DGT_TL)) {
        camera_remove_mode(camera_current_index,
                           camera_top(camera_current_index));
    }
    if (game_state.camera_motion_playing) {
        top = camera_top(camera_current_index);
        camera_remove_mode(camera_current_index, top);
    }
}

int camera_settle_step(int camera)
{
    int latest = settle_counts[camera] % CAMERA_SETTLE_PLACES;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused; /* NOLINT(clang-diagnostic-unused-variable) */
    NJS_POINT3 max;
    NJS_POINT3 min;
    int i;

    if (game_state.camera_motion_playing) {
        return 0;
    }
    if (world_gravity.y > CAMERA_SETTLE_GRAVITY) {
        return 0;
    }
    if (settle_delays[camera] > 0) {
        settle_delays[camera]--;
        return 0;
    }
    if (player_work[camera_current_index]->mode == PLAYER_MODE_HUNTER_CLIMB) {
        camera_delay_settle(camera_current_index, CAMERA_SETTLE_DELAY);
        return 0;
    }
    settle_places[camera][latest] = camera_views[camera]->pos;
    settle_counts[camera]++;
    if (camera_stacks[camera].standing == 0) {
        return 0;
    }
    if (latest == 0) {
        max = settle_places[camera][1];
        min = settle_places[camera][1];
    } else {
        max = settle_places[camera][0];
        min = settle_places[camera][0];
    }
    for (i = 0; i < CAMERA_SETTLE_PLACES; i++) {
        if (latest == i) {
            continue;
        }
        if (max.x < settle_places[camera][i].x) {
            max.x = settle_places[camera][i].x;
        }
        if (max.y < settle_places[camera][i].y) {
            max.y = settle_places[camera][i].y;
        }
        if (max.z < settle_places[camera][i].z) {
            max.z = settle_places[camera][i].z;
        }
        if (min.x > settle_places[camera][i].x) {
            min.x = settle_places[camera][i].x;
        }
        if (min.y > settle_places[camera][i].y) {
            min.y = settle_places[camera][i].y;
        }
        if (min.z > settle_places[camera][i].z) {
            min.z = settle_places[camera][i].z;
        }
    }
    if (camera_views[camera]->pos.x <= max.x &&
        camera_views[camera]->pos.y <= max.y &&
        camera_views[camera]->pos.z <= max.z &&
        camera_views[camera]->pos.x >= min.x &&
        camera_views[camera]->pos.y >= min.y &&
        camera_views[camera]->pos.z >= min.z) {
        return 1;
    }
    return 0;
}

void camera_delay_settle(int camera, int calls)
{
    settle_delays[camera] = calls;
}

void camera_delay_settle_all(void)
{
    int i;

    for (i = 0; i < CAMERAS; i++) {
        settle_delays[i] = CAMERA_SETTLE_DELAY;
    }
}
