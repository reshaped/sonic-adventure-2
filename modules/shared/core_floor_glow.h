/* the glow on the floor below player 0, on Cannon's Core's two mech routes.
 * each route's background samples the floor by sample_floor_glow, and draws the
 * glow by shared/core_floor_draw.inc */
#ifndef SHARED_CORE_FLOOR_GLOW_H
#define SHARED_CORE_FLOOR_GLOW_H
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "shared/core_machine_lights.h"
#include "system/task.h"

/* the background task's surface probe, or null. sample_floor_glow takes it
 * from the Shinobi heap on its first call, and the task's destructor must
 * free it. SHC's build keeps its address in unk_3c, a Sint32 */
#ifdef NONMATCHING
/* this side keeps the address in extra, a pointer the task leaves free. the
 * matching side converts it to unk_3c's Sint32 */
#define CORE_GLOW_FLOOR(task) ((struct surface_probe *)(task)->extra.p)
#define SET_GLOW_FLOOR(task, memory) ((task)->extra.p = (memory))
#else
#define CORE_GLOW_FLOOR(task) ((struct surface_probe *)(task)->unk_3c.p)
#define SET_GLOW_FLOOR(task, memory) ((task)->unk_3c.p = (memory))
#endif
#define GLOW_WALL_DISTANCE 30.0F
#define GLOW_CONTACT_HEIGHT 6.0F
#define GLOW_SLOPE_LIMIT 2.1F
#define GLOW_SAMPLE_COUNT 4
/* the probe's side that holds the glow's floor */
#define GLOW_SIDE 0
/* the glow's pulse runs from GLOW_PULSE_LOW to GLOW_PULSE_LOW plus twice
 * GLOW_PULSE_SWING, and turns GLOW_PULSE_SPEED a step */
#define GLOW_PULSE_SPEED 640
#define GLOW_PULSE_LOW 0.8F
#define GLOW_PULSE_SWING 0.1F
/* the part of the gap to its target that the glow's alpha closes each call */
#define GLOW_EASE 0.2F

/* sample the floor at four points about the center of player 0's first
 * collision shape. they lie core_floor_probe_reach along +x, -x, +z and -z,
 * in that order. where the samples give a floor, the probe's pos takes the
 * center. its sides[GLOW_SIDE] takes the floor's height and its tilt along
 * the steeper axis. a tilt along x goes in ang_z, and one along z in ang_x.
 * scl.y then takes the glow's target alpha. that is the pulse times an
 * attenuation by player 0's distance from pos. scl.x, the alpha drawn, eases
 * toward scl.y.
 *
 * the first sample without a floor takes the mean of its two neighbors in
 * that order, which wraps. it takes it only where both of those have floors.
 * its side toward the center must also find a wall within
 * GLOW_WALL_DISTANCE of the center. where it can't be filled, its height
 * stays NO_FLOOR, and the samples after it go unchecked.
 *
 * the probe and scl keep their values after a second sample without a floor.
 * they keep them after a floor with GROUND_FOOTSTEPS too. they keep them
 * where the floor rises more than GLOW_SLOPE_LIMIT times the reach between
 * opposite samples */
static void sample_floor_glow(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;
    /* @hack spelling: 2 held in found, where the constant would do */
    int found;
    int i;
    int next;
    int previous;
    int missing;
    int near_wall;
    Float rise_x;
    Float rise_z;
    Float height;
    Float distance;
    Float attenuation;
    Angle pitch;
    Angle roll;
    /* @hack spelling: the probes set up through sample, where probes[i] would
     * do */
    struct surface_probe *sample;

    if (!CORE_GLOW_FLOOR(task)) {
        SET_GLOW_FLOOR(task, syCalloc(1, sizeof(struct surface_probe)));
    }
    work->submode = 0;
    if (CORE_GLOW_FLOOR(task) && player_work[0]) {
        found = 2;
        center = player_work[0]->collision->shapes->center;
        {
            NJS_POINT3 position;
            struct surface_probe probes[GLOW_SAMPLE_COUNT];
            NJS_VECTOR slope;
            /* @hack spelling: slope reached through direction, where slope
             * would do */
            NJS_VECTOR *direction;
            /* @hack spelling: core_floor_probe_reach read through reach in the
             * samples and the first slope test, where the variable would do */
            Float *reach;

            sample = probes;
            sample->pos = center;
            reach = &core_floor_probe_reach;
            sample->pos.x += *reach;
            surface_probe_sides(sample);
            sample = &probes[1];
            sample->pos = center;
            sample->pos.x -= *reach;
            surface_probe_sides(sample);
            sample = &probes[2];
            sample->pos = center;
            sample->pos.z += *reach;
            surface_probe_sides(sample);
            sample = &probes[3];
            sample->pos = center;
            sample->pos.z -= *reach;
            surface_probe_sides(sample);

            missing = 0;
            for (i = 0; i < GLOW_SAMPLE_COUNT; ++i) {
                if (!probes[i].sides[SURFACE_NEG_Y].found) {
                    near_wall = 0;
#define CHECK_WALL(side, axis)                                                 \
    if (probes[i].sides[side].found &&                                         \
        njAbs(probes[i].sides[side].pos - center.axis) < GLOW_WALL_DISTANCE) { \
        near_wall = 1;                                                         \
    }
                    switch (i) {
                    case 0:
                        CHECK_WALL(SURFACE_NEG_X, x)
                        break;
                    case 1:
                        CHECK_WALL(SURFACE_POS_X, x)
                        break;
                    case 2:
                        CHECK_WALL(SURFACE_NEG_Z, z)
                        break;
                    case 3:
                        CHECK_WALL(SURFACE_POS_Z, z)
                        break;
                    }
#undef CHECK_WALL
                    ++missing;
                    if (missing != 1 || !near_wall) {
                        break;
                    }
                    next = (i + 1) % GLOW_SAMPLE_COUNT;
                    previous = (next + 2) % GLOW_SAMPLE_COUNT;
                    /* @hack redundant: a test of next that no path reaches */
                    if (0) {
                        if (next) {
                        }
                    }
                    if (!probes[next].sides[SURFACE_NEG_Y].found ||
                        !probes[previous].sides[SURFACE_NEG_Y].found) {
                        break;
                    }
                    probes[i].sides[SURFACE_NEG_Y].found = 1;
                    probes[i].sides[SURFACE_NEG_Y].pos =
                        (probes[next].sides[SURFACE_NEG_Y].pos +
                         probes[previous].sides[SURFACE_NEG_Y].pos) *
                        0.5F;
                } else if (probes[i].sides[SURFACE_NEG_Y].flags &
                           GROUND_FOOTSTEPS) {
                    missing = 2;
                    break;
                }
            }
            /* @hack redundant: a test of missing that no path reaches */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (missing && missing) {
                }
            }
            if (missing > 1) {
                return;
            }
            rise_x = probes[0].sides[SURFACE_NEG_Y].pos -
                     probes[1].sides[SURFACE_NEG_Y].pos;
            rise_z = probes[2].sides[SURFACE_NEG_Y].pos -
                     probes[3].sides[SURFACE_NEG_Y].pos;
            /* @hack redundant: a test of rise_x that no path reaches */
            if (0) {
                if (rise_x) {
                }
            }
            if (njAbs(rise_x) > *reach * GLOW_SLOPE_LIMIT) {
                return;
            }
            if (njAbs(rise_z) > core_floor_probe_reach * GLOW_SLOPE_LIMIT) {
                return;
            }
            if (njAbs(rise_x) > njAbs(rise_z)) {
                /* @hack redundant: a test of position, a copy of center that
                 * nothing else reads, that no path reaches */
                if (0) {
                    if (position.x) {
                    }
                }
                direction = &slope;
                /* @hack redundant: a test of direction that no path reaches */
                if (0) {
                    if (direction) {
                    }
                }
                direction->x = probes[0].pos.x - probes[1].pos.x;
                direction->y = rise_x;
                direction->z = probes[0].pos.z - probes[1].pos.z;
                position = center;
                height = (probes[0].sides[SURFACE_NEG_Y].pos +
                          probes[1].sides[SURFACE_NEG_Y].pos) *
                         0.5F;
                pitch = 0;
                roll = atan2f(direction->y, direction->x) * ANGLE_PER_RADIAN_F;
            } else {
                direction = &slope;
                direction->x = probes[2].pos.x - probes[3].pos.x;
                direction->y = rise_z;
                direction->z = probes[2].pos.z - probes[3].pos.z;
                position = center;
                roll = 0;
                height = (probes[2].sides[SURFACE_NEG_Y].pos +
                          probes[3].sides[SURFACE_NEG_Y].pos) *
                         0.5F;
                pitch =
                    atan2f(-direction->y, direction->z) * ANGLE_PER_RADIAN_F;
            }
        }
        CORE_GLOW_FLOOR(task)->sides[GLOW_SIDE].found = found;
        CORE_GLOW_FLOOR(task)->sides[GLOW_SIDE].ang_x = pitch;
        CORE_GLOW_FLOOR(task)->sides[GLOW_SIDE].ang_z = roll;
        CORE_GLOW_FLOOR(task)->sides[GLOW_SIDE].pos = height;
        CORE_GLOW_FLOOR(task)->pos = center;
        /* @bug logic: the attenuation is always 1. it measures player 0's
         * distance from the probe's pos, which the lines above set to that
         * player's center. the JSR of njDistanceP2P at 0x8c500a04 in stg35's
         * image follows that copy at 0x8c5009f2, and in stg36's, the JSR at
         * 0x8c500984 follows it at 0x8c500972 */
        distance = njDistanceP2P(&player_work[0]->collision->shapes->center,
                                 &CORE_GLOW_FLOOR(task)->pos);
        if (distance > GLOW_CONTACT_HEIGHT) {
            attenuation = core_floor_falloff /
                          (core_floor_falloff + distance - GLOW_CONTACT_HEIGHT);
        } else {
            attenuation = 1.0F;
        }
        {
            /* @hack spelling: the scales set through glow, a copy of work,
             * which a test no path reaches reads, where work would do */
            struct task_work *glow = work;
            /* @hack redundant: a test of glow, in a block no path reaches */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (glow && glow) {
                }
            }
            glow->scl.y =
                attenuation *
                (((njSin(game_state.steps * GLOW_PULSE_SPEED) + 1.0F) *
                  GLOW_PULSE_SWING) +
                 GLOW_PULSE_LOW);
            glow->scl.x =
                glow->scl.x + ((glow->scl.y - glow->scl.x) * GLOW_EASE);
        }
    }
}

#endif
