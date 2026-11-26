/* the camera's CARMEL mode: KNUKLES, until the land hides Knuckles or Rouge.
 * the camera then sweeps in along the player's trail, or searches for a place
 * in sight */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/avoid.h"
#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/knukles.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/task.h"
#include "system/vector.h"

#define CARMEL_HIDDEN 0 /* the land hid the player: sweep or search */
#define CARMEL_SWEEP 1 /* sweep in along the player's trail */
#define CARMEL_KNUKLES 2 /* run KNUKLES */
#define CARMEL_SEARCH 3 /* start a search */
#define CARMEL_SEARCHING 4 /* try one of the search's places a step */
#define CARMEL_MOVE 5 /* move to the place the search found */

/* a sweep lasts up to SWEEP_STEPS steps. in step s, the camera moves
 * s / SWEEP_STEPS of the way to a point along the trail. that point comes
 * forward from SWEEP_BACK points back as s grows */
#define SWEEP_STEPS 90
#define SWEEP_BACK 32.0F
/* the mode sweeps only once the trails take more than SWEEP_AGAIN points
 * after its last sweep. it searches otherwise */
#define SWEEP_AGAIN 50.0F
/* a sweep sets knukles.unlimited to SWEEP_UNLIMITED, which lifts KNUKLES's
 * FARTHEST and NEAREST */
#define SWEEP_UNLIMITED 120
/* a sweep keeps the camera at least SWEEP_NEAREST from the player in x and
 * z. it ends once the camera comes closer than SWEEP_ARRIVED to the
 * player's target */
#define SWEEP_NEAREST 1.0F
#define SWEEP_ARRIVED 35.0F
#define SEARCH_RADIUS 35.0F
/* a move lasts MOVE_STEPS steps. in step s, the camera moves s / MOVE_SPAN
 * of the way to the place the search found */
#define MOVE_STEPS 30
#define MOVE_SPAN 60

struct carmel_work {
    /* first, since KNUKLES takes the slot's work for its own */
    struct knukles_work knukles;
    Sint32 state; /* a CARMEL_ state */
    Sint32 sweep_steps; /* the steps the sweep has taken */
    /* camera_trail_points_taken as the last step of a sweep found it */
    Sint32 sweep_points;
    struct camera_avoid avoid;
    NJS_POINT3 found; /* the place the search found */
};

/* each step's copy of its player's target_height */
static Float target_height;
/* each player's mode at the last step that ran KNUKLES */
static Sint32 mode_before[PLAYER_SLOTS];

void camera_mode_carmel(struct camera_stack *stack, struct camera_slot *slot)
{
    /* @hack spelling: place_pos, a local for &stack->place.pos, where the
     * expression in its one call would do */
    NJS_POINT3 *place_pos = &stack->place.pos;
    struct carmel_work *work = (struct carmel_work *)slot->work;
    NJS_POINT3 *newer;
    NJS_POINT3 *older;
    NJS_VECTOR vec;
    NJS_POINT3 *point;
    Float part;
    Float back;
    Float nearest;
    int behind;

    if (camera_no_player(camera_current_index)) {
        return;
    }
    target_height = player_records[camera_current_index]->consts.target_height;
    if (slot->steps == 0) {
        work->state = CARMEL_KNUKLES;
    }
    switch (work->state) {
    case CARMEL_HIDDEN:
        work->knukles.pitch_held = 1;
        if (njAbs(camera_trail_points_taken() - work->sweep_points) <=
            SWEEP_AGAIN) {
            work->state = CARMEL_SEARCH;
            goto search;
        }
        work->state = CARMEL_SWEEP;
        work->knukles.unlimited = SWEEP_UNLIMITED;
        /* fall through */
    case CARMEL_SWEEP:
        work->sweep_points = camera_trail_points_taken();
        part = (Float)work->sweep_steps * (1.0F / SWEEP_STEPS);
        back = SWEEP_BACK - (part * SWEEP_BACK);
        behind = (int)(floorf(back) + 1.0F);
        newer = camera_trail_point(camera_current_index, behind);
        older = camera_trail_point(camera_current_index, behind + 1);
        /* @hack spelling: vec written and read through point, where vec's
         * own members would do */
        point = &vec;
        point->x = newer->x + ((older->x - newer->x) * (back - behind));
        point->y = newer->y + ((older->y - newer->y) * (back - behind));
        point->z = newer->z + ((older->z - newer->z) * (back - behind));
        camera_state.pos.x =
            camera_views[camera_current_index]->pos.x +
            ((point->x - camera_views[camera_current_index]->pos.x) * part);
        camera_state.pos.y =
            camera_views[camera_current_index]->pos.y +
            ((point->y - camera_views[camera_current_index]->pos.y) * part);
        camera_state.pos.z =
            camera_views[camera_current_index]->pos.z +
            ((point->z - camera_views[camera_current_index]->pos.z) * part);
        vec.x = camera_state.pos.x - player_work[camera_current_index]->pos.x;
        vec.z = camera_state.pos.z - player_work[camera_current_index]->pos.z;
        /* @hack spelling: nearest, a local for SWEEP_NEAREST, where
         * SWEEP_NEAREST in place would do */
        nearest = SWEEP_NEAREST;
        if ((vec.x * vec.x) + (vec.z * vec.z) < nearest * nearest) {
            vec.y = 0.0F;
            if (vec.x == 0.0F && vec.z == 0.0F) {
                vec.x += 0.01F;
            }
            njUnitVector(&vec);
            camera_state.pos.x =
                player_work[camera_current_index]->pos.x + (vec.x * nearest);
            camera_state.pos.z =
                player_work[camera_current_index]->pos.z + (vec.z * nearest);
        }
        camera_state.target.x = player_work[camera_current_index]->pos.x;
        camera_state.target.y =
            player_work[camera_current_index]->pos.y + target_height;
        camera_state.target.z = player_work[camera_current_index]->pos.z;
        vec.x = camera_state.pos.x - player_work[camera_current_index]->pos.x;
        vec.y = camera_state.pos.y - player_work[camera_current_index]->pos.y -
                target_height;
        vec.z = camera_state.pos.z - player_work[camera_current_index]->pos.z;
        if (njScalor(&vec) < SWEEP_ARRIVED) {
            work->state = CARMEL_KNUKLES;
        }
        work->sweep_steps++;
        if (work->sweep_steps >= SWEEP_STEPS) {
            work->state = CARMEL_KNUKLES;
        }
        camera_state.type = CAMERA_POS_TARGET;
        camera_state.unk_04 = 0;
        camera_state.trigger_push = 0;
        /* @hack spelling: the land's test nested in the state's, where one
         * test joined with && would do */
        if (work->state == CARMEL_KNUKLES) {
            if (camera_sight_blocked(camera_current_index, &camera_state.pos)) {
                work->state = CARMEL_SEARCH;
            }
        }
        break;
    case CARMEL_SEARCH:
    search:
        camera_player_target(camera_current_index, &vec);
        work->avoid.radius = SEARCH_RADIUS;
        vector_subtract(&camera_state.pos, &vec, &work->avoid.offset);
        look_angles(&work->avoid.offset, &work->avoid.ang.x,
                    &work->avoid.ang.y);
        work->avoid.ang.x *= -1;
        work->avoid.ang.z = 0;
        work->avoid.next_try = 0;
        work->state = CARMEL_SEARCHING;
        /* fall through */
    case CARMEL_SEARCHING:
        if (camera_avoid_try(camera_current_index, &work->avoid,
                             &work->found)) {
            work->state = CARMEL_MOVE;
            work->avoid.steps = 0;
        } else {
            camera_state.target.x = player_work[camera_current_index]->pos.x;
            camera_state.target.y =
                player_work[camera_current_index]->pos.y + target_height;
            camera_state.target.z = player_work[camera_current_index]->pos.z;
            break;
        }
        /* fall through */
    case CARMEL_MOVE:
        camera_state.pos.x =
            camera_stacks[camera_current_index].place.pos.x +
            ((work->found.x - camera_stacks[camera_current_index].place.pos.x) *
             (Float)work->avoid.steps * (1.0F / MOVE_SPAN));
        camera_state.pos.y =
            camera_stacks[camera_current_index].place.pos.y +
            ((work->found.y - camera_stacks[camera_current_index].place.pos.y) *
             (Float)work->avoid.steps * (1.0F / MOVE_SPAN));
        camera_state.pos.z =
            camera_stacks[camera_current_index].place.pos.z +
            ((work->found.z - camera_stacks[camera_current_index].place.pos.z) *
             (Float)work->avoid.steps * (1.0F / MOVE_SPAN));
        camera_state.target.x = player_work[camera_current_index]->pos.x;
        camera_state.target.y =
            player_work[camera_current_index]->pos.y + target_height;
        camera_state.target.z = player_work[camera_current_index]->pos.z;
        work->avoid.steps++;
        if (work->avoid.steps >= MOVE_STEPS) {
            work->state = CARMEL_KNUKLES;
        }
        break;
    case CARMEL_KNUKLES:
        work->sweep_steps = 0;
        camera_mode_knukles(stack, slot);
        switch (player_character(camera_current_index)) {
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            if (work->knukles.turned == 0 &&
                mode_before[camera_current_index] != PLAYER_MODE_HUNTER_CLIMB &&
                player_work[camera_current_index]->mode !=
                    PLAYER_MODE_HUNTER_CLIMB &&
                (player_work[camera_current_index]->mode <
                     KNUKLES_WALL_DIG_FIRST ||
                 player_work[camera_current_index]->mode >
                     KNUKLES_WALL_DIG_LAST)) {
                /* @hack spelling: the land's test nested in the others,
                 * where one more term of their && would do */
                if (camera_sight_blocked(camera_current_index, place_pos)) {
                    work->state = CARMEL_HIDDEN;
                    camera_set_adjust(camera_current_index,
                                      camera_top(camera_current_index),
                                      CAMERA_ADJUST_NONE);
                }
            }
        }
        mode_before[camera_current_index] =
            player_work[camera_current_index]->mode;
        break;
    }
}
