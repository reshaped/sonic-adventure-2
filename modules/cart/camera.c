/* the cart's camera mode, which chases each player's kart. the mode plays the
 * road's or the tunnel's sound under cart_racers[0]'s kart. this source also
 * tests how far a point lies from the cameras */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "camera/viewport.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/vehicle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"

/* the camera's phases, in its slot's work. no phase leads to FOLLOW_DIRECT */
enum camera_phase {
    SET_UP,
    APPROACH,
    COUNTDOWN,
    FOLLOW,
    FOLLOW_DIRECT,
    HOLD,
    CHASE_END
};
/* the mode's state in its slot's work */
struct cart_camera {
    enum camera_phase phase, after_approach;
    NJS_POINT3 previous; /* the kart's position at the last step */
};

/* the approach starts APPROACH_DIST farther out than CHASE_DIST and at
 * START_PITCH. the distance past CHASE_DIST falls as the square of the steps
 * left, to 0 at APPROACH_STEPS. the pitch turns toward CHASE_PITCH by
 * PITCH_STEP a step */
#define APPROACH_DIST 400.0F
#define APPROACH_STEPS 300
#define START_PITCH ((Angle)(-50 * ANGLE_PER_DEGREE))
#define PITCH_STEP 24
/* after the approach, the lights count down from COUNT_FROM to 0, a count
 * each COUNT_STEPS steps */
#define COUNT_FROM 3
#define COUNT_STEPS 60
/* the chase looks at TARGET_RISE above the kart, from CHASE_DIST away, or
 * SPLIT_CHASE_DIST with two cameras. its yaw is the kart's YAW_LAG steps
 * back */
#define TARGET_RISE 10
#define CHASE_DIST 40
#define SPLIT_CHASE_DIST 80
#define CHASE_PITCH ((Angle)(-10 * ANGLE_PER_DEGREE))
#define YAW_LAG 5
/* a story race's end moves the camera's target toward a point near the kart.
 * that point is END_SIDE from the kart along x. in y and z it lies halfway to
 * the chase target. the camera moves toward END_OFFSET from that point along x,
 * y and -z. the target and the camera close END_BLEND of the way a step */
#define END_SIDE 50
#define END_OFFSET 300
#define END_BLEND 0.002F
/* the sounds a one-player race plays while cart_racers[0]'s kart is out of a
 * tunnel, and in one. both play under AMBIENCE_KEY, so each stops the other */
#define ROAD_SOUND 28
#define TUNNEL_SOUND 29
#define AMBIENCE_KEY ((void *)ROAD_SOUND)
#define AMBIENCE_PRIORITY 1
#define AMBIENCE_VOLUME (-50)
/* the layout viewport_set gives the two cameras */
#define SPLIT_LAYOUT 7
/* how far a point may be from a camera before cart_out_of_view or
 * cart_out_of_range takes it as out */
#define VIEW_RANGE 2000.0F

struct task *cart_racers[CART_RACERS];
Sint32 cart_start_count;
/* whether cart_racers[0]'s kart was in a tunnel at the last check. the camera's
 * set-up makes it -1 until the first */
static int last_tunnel;
static void camera_follow(struct camera_stack *stack, struct camera_slot *slot);
static void interpolate(Float amount, NJS_POINT3 *out, NJS_POINT3 *from,
                        NJS_POINT3 *to);

void cart_camera_start(int screen)
{
    while (camera_top(screen)) {
        camera_remove_mode(screen, camera_top(screen));
    }
    camera_push_user_mode(screen, camera_follow);
    camera_release();
    cart_finished = 0;
    viewport_select_split(SPLIT_LAYOUT);
}
#define CHASE_CAMERA(work, distance)                                           \
    do {                                                                       \
        camera_state.target = (work)->pos;                                     \
        camera_state.target.y += TARGET_RISE;                                  \
        camera_state.ang.x = CHASE_PITCH;                                      \
        camera_state.ang.z = 0;                                                \
        camera_state.dist = (distance);                                        \
        camera_state.type = CAMERA_TARGET_ANG_DIST;                            \
        camera_state.unk_04 = 1;                                               \
        camera_state.trigger_push = 0;                                         \
    } while (0)

/* the cart's camera mode. a race's camera first approaches the kart. a story
 * race then goes straight to the chase. any other race holds the camera
 * through the lights' count, which ends in cart_release_start */
static void camera_follow(struct camera_stack *stack, struct camera_slot *slot)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    struct cart_camera *camera;
    struct task_work *target;
    if (!cart_racers[camera_current_index]) {
        return;
    }
    camera = (struct cart_camera *)slot->work;
    work = cart_racers[camera_current_index]->work;
    kart = cart_racers[camera_current_index]->motion.p;
    if (slot->steps == 0) {
        camera->phase = SET_UP;
    }
    if (cart_finished) {
        camera->phase = HOLD;
    }
    switch (camera->phase) {
    case SET_UP:
        switch (cart_setup.course_mode) {
        case CART_TAILS:
        case CART_ROUGE:
            camera->phase = APPROACH;
            camera->after_approach = FOLLOW;
            camera_state.dist = APPROACH_DIST;
            camera_state.ang.x = START_PITCH;
            break;
        default:
            camera->phase = APPROACH;
            camera->after_approach = COUNTDOWN;
            camera_state.dist = APPROACH_DIST;
            camera_state.ang.x = START_PITCH;
            cart_start_count = COUNT_FROM;
            break;
        }
        last_tunnel = -1;
        /* fall through */
    case APPROACH: {
        Sint32 remaining;
        camera_state.target = work->pos;
        camera_state.target.y += TARGET_RISE;
        camera_state.ang.x =
            angle_approach(camera_state.ang.x, CHASE_PITCH, PITCH_STEP);
        camera_state.ang.y = work->ang.y;
        camera_state.ang.z = 0;
        remaining = APPROACH_STEPS - slot->steps;
        camera_state.dist =
            (remaining * remaining *
             (APPROACH_DIST / (APPROACH_STEPS * APPROACH_STEPS))) +
            CHASE_DIST;
        camera_state.type = CAMERA_TARGET_ANG_DIST;
        camera_state.unk_04 = 0;
        camera_state.trigger_push = 0;
        if (slot->steps == APPROACH_STEPS) {
            camera->phase = camera->after_approach;
        }
        break;
    }
    case COUNTDOWN:
        camera_state.pos = camera_views[camera_current_index]->pos;
        camera_state.target = camera_views[camera_current_index]->target;
        camera_state.ang = camera_views[camera_current_index]->ang;
        camera_state.type = CAMERA_POS_TARGET;
        switch (slot->steps) {
        case APPROACH_STEPS + COUNT_STEPS:
            cart_start_count = COUNT_FROM - 1;
            break;
        case APPROACH_STEPS + (2 * COUNT_STEPS):
            cart_start_count = COUNT_FROM - 2;
            break;
        case APPROACH_STEPS + (COUNT_FROM * COUNT_STEPS):
            cart_start_count = 0;
            camera->phase = FOLLOW;
            cart_release_start();
            break;
        default:
            break;
        }
        break;
    case FOLLOW: {
        struct angles ang;
        if (camera_active_count >= 2) {
            CHASE_CAMERA(work, SPLIT_CHASE_DIST);
        } else {
            CHASE_CAMERA(work, CHASE_DIST);
        }
        cart_previous_angles(camera_current_index, YAW_LAG, &ang);
        camera_state.ang.y = ang.y;
        if (cart_story_clear) {
            camera->phase = CHASE_END;
            camera->previous = work->pos;
        }
        break;
    }
    case FOLLOW_DIRECT:
        camera_state.target = work->pos;
        camera_state.target.y += TARGET_RISE;
        camera_state.ang.x = CHASE_PITCH;
        camera_state.ang.y = work->ang.y;
        camera_state.ang.z = 0;
        camera_state.dist = CHASE_DIST;
        camera_state.type = CAMERA_TARGET_ANG_DIST;
        camera_state.unk_04 = 0;
        camera_state.trigger_push = 0;
        if (cart_story_clear) {
            camera->phase = CHASE_END;
            camera->previous = work->pos;
        }
        break;
    case CHASE_END: {
        NJS_POINT3 position;
        NJS_POINT3 prior_position;
        NJS_POINT3 midpoint;
        NJS_POINT3 prior_target;
        target = kart->chase_target->work;
        midpoint.x = END_SIDE;
        midpoint.y = ((work->pos.y + target->pos.y) * 0.5F) - work->pos.y;
        midpoint.z = ((work->pos.z + target->pos.z) * 0.5F) - work->pos.z;
        position.x = midpoint.x + END_OFFSET;
        position.y = midpoint.y + END_OFFSET;
        position.z = midpoint.z - END_OFFSET;
        vector_diff(&camera_views[camera_current_index]->pos, &camera->previous,
                    &prior_position);
        vector_diff(&camera_views[camera_current_index]->target,
                    &camera->previous, &prior_target);
        interpolate(END_BLEND, &camera_state.pos, &prior_position, &position);
        interpolate(END_BLEND, &camera_state.target, &prior_target, &midpoint);
        vector_sum(&camera_state.pos, &work->pos, &camera_state.pos);
        vector_sum(&camera_state.target, &work->pos, &camera_state.target);
        camera_state.type = CAMERA_POS_TARGET;
        camera_state.unk_04 = 0;
        camera_state.trigger_push = 0;
        camera->previous = work->pos;
        break;
    }
    case HOLD:;
    }
    if (game_state.two_players == 0) {
        if (last_tunnel != cart_leader_tunnel_sound()) {
            sound_stop_key(AMBIENCE_KEY);
            if (last_tunnel) {
                sound_play_held(ROAD_SOUND, AMBIENCE_KEY, AMBIENCE_PRIORITY,
                                AMBIENCE_VOLUME);
            } else {
                sound_play_held(TUNNEL_SOUND, AMBIENCE_KEY, AMBIENCE_PRIORITY,
                                AMBIENCE_VOLUME);
            }
            last_tunnel = !last_tunnel;
        }
    }
}
static void interpolate(Float amount, NJS_POINT3 *out, NJS_POINT3 *from,
                        NJS_POINT3 *to)
{
    out->x = from->x + ((to->x - from->x) * amount);
    out->y = from->y + ((to->y - from->y) * amount);
    out->z = from->z + ((to->z - from->z) * amount);
}
Bool cart_is_racer(struct task *task)
{
    struct task **slot;
    struct task *racer;
    /* @hack spelling: slot stepped, and its task kept in racer, inside the
     * test, where a for that steps slot and reads *slot would do */
    for (slot = cart_racers; slot < cart_racers + CART_RACERS;) {
        if ((racer = *slot++) && racer == task) {
            return TRUE;
        }
    }
    return FALSE;
}
Bool cart_out_of_view(NJS_POINT3 *pos)
{
    NJS_VECTOR delta;
    vector_diff(pos, &camera_views[camera_current_index]->pos, &delta);
    if (njScalor2(&delta) < VIEW_RANGE * VIEW_RANGE) {
        return FALSE;
    }
    return TRUE;
}
Bool cart_out_of_range(NJS_POINT3 *pos)
{
    NJS_VECTOR delta;
    vector_diff(pos, &camera_views[0]->pos, &delta);
    if (njScalor2(&delta) < VIEW_RANGE * VIEW_RANGE) {
        return FALSE;
    }
    if (camera_active_count == 2) {
        vector_diff(pos, &camera_views[1]->pos, &delta);
        if (njScalor2(&delta) < VIEW_RANGE * VIEW_RANGE) {
            return FALSE;
        }
    }
    return TRUE;
}
