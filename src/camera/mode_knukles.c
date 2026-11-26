/* the camera's KNUKLES mode, and the look that stands in for it while the
 * player climbs a ledge */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/gakugaku.h"
#include "camera/knukles.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zone_collision.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/vector.h"

/* how far a step turns the camera's pitch toward KNUKLES_PITCH */
#define PITCH_TURN ((Angle)(1 * ANGLE_PER_DEGREE))
/* a pitch's turn raises the camera by lift times the sine of its offset from
 * KNUKLES_PITCH. lift is PITCH_LIFT_MECH for a mech and PITCH_LIFT for others.
 * it is PITCH_LIFT for any player where the turn brings the pitch within
 * KNUKLES_PITCH_NEAR */
#define PITCH_LIFT 1.0F
#define PITCH_LIFT_MECH 5.0F
/* past FAR, a step keeps KNUKLES_FAR_KEPT of the camera's distance beyond
 * FAR, up to FARTHEST. within NEAR, it scales the distance by BACK_OFF, to
 * NEAREST at least while work->turned is 0. neither limit holds while
 * work->unlimited isn't 0 */
#define FAR 50.0F
#define FARTHEST 70.0F
#define NEAR 35.0F
#define NEAREST 20.0F
#define BACK_OFF 1.06F
/* the camera stands at least FLAT_NEAREST from the player in x and z */
#define FLAT_NEAREST 30.0F
/* the look stands LOOK_DIST off at a pitch of LOOK_PITCH. it looks at a
 * point LOOK_RISE above the player's target */
#define LOOK_DIST 35.0F
#define LOOK_PITCH ((Angle)(-35 * ANGLE_PER_DEGREE))
#define LOOK_RISE 10.0F

static const NJS_VECTOR offset = {0.0F, 0.0F, -42.5F};

/* each step's copy of its player's target_height */
static Float target_height;
/* the number of the mode the look replaced, which it puts back */
static Sint32 look_replaced;

static void look_down(struct camera_stack *stack, struct camera_slot *slot);

static int knuckles_l_runs(void)
{
    switch (player_character(camera_current_index)) {
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        /* @hack spelling: a second return in an else, where one test joined
         * with || would do */
        if (player_work[camera_current_index]->mode ==
                PLAYER_MODE_HUNTER_CLIMB ||
            (player_work[camera_current_index]->mode >=
                 KNUKLES_WALL_DIG_FIRST &&
             player_work[camera_current_index]->mode <=
                 KNUKLES_WALL_DIG_LAST)) {
            return 1;
        } else if (player_work[1] != NULL) {
            return 1;
        }
    }
    return 0;
}

/* turn vec about y by ang->y, then about x by ang->x, then about z by ang->z.
 * then turn it by the current matrix, which the call leaves as it was. the last
 * njRotate's turn applies first */
static void knukles_turn_yxz(struct angles *ang, NJS_VECTOR *vec)
{
    njPushMatrix(NULL);
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, ang->y);
    njCalcVector(NULL, vec, vec);
    njPopMatrix(1);
}

void camera_mode_knukles(struct camera_stack *stack, struct camera_slot *slot)
{
    struct knukles_work *work = (struct knukles_work *)slot->work;
    NJS_VECTOR vec;
    Angle heading;
    Float len;
    Float lift;

    if (knuckles_l_runs()) {
        camera_mode_knuckles_l(stack, slot);
        return;
    }
    if (slot->steps == 0) {
        work->turned = 0;
        work->pos.x = KNUKLES_NOWHERE;
        work->pos.y = KNUKLES_NOWHERE;
        work->pos.z = KNUKLES_NOWHERE;
        work->still = 0;
        work->still_steps = 0;
        work->pitch_held = 0;
        work->unlimited = 0;
    }
    if (work->unlimited < 0) {
        work->unlimited = 0;
    }
    target_height = player_records[camera_current_index]->consts.target_height;
    if (work->still) {
        vector_subtract(&player_work[camera_current_index]->pos, &work->pos,
                        &vec);
        if (njScalor(&vec) > KNUKLES_STILL_REACH) {
            work->pos = player_work[camera_current_index]->pos;
            work->still_steps = 0;
            work->still = 0;
        }
    } else {
        vector_subtract(&player_work[camera_current_index]->pos, &work->pos,
                        &vec);
        if (njScalor(&vec) < KNUKLES_STILL_REACH) {
            work->still_steps++;
            if (work->still_steps > KNUKLES_STILL_STEPS) {
                work->still = 1;
            }
        } else {
            /* @bug logic: leaves still_steps as it is, as knukles_work says.
             * the arm above starts the count again as it moves pos, at
             * 0x8c0b447e, and this one moves pos alone, at 0x8c0b4512 */
            work->pos = player_work[camera_current_index]->pos;
        }
    }
    if (!work->still) {
        vec = player_work[camera_current_index]->pos;
        vec.y += target_height;
        if (camera_player_mode_before(camera_current_index,
                                      KNUKLES_LEAD_BACK) !=
            PLAYER_MODE_LEDGE_CLIMB) {
            NJS_POINT3 past;
            NJS_VECTOR lead;

            player_get_past(camera_current_index, KNUKLES_LEAD_BACK, &past,
                            NULL);
            past.y += target_height;
            vector_subtract(&vec, &past, &lead);
            lead.x *= 1.0F / KNUKLES_LEAD_BACK;
            lead.y *= 1.0F / KNUKLES_LEAD_BACK;
            lead.z *= 1.0F / KNUKLES_LEAD_BACK;
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((len = njScalor(&lead)) < KNUKLES_LEAD_SLOW) {
                if (len < KNUKLES_LEAD_NONE) {
                    lead.x = 0.0F;
                    lead.y = 0.0F;
                    lead.z = 0.0F;
                } else {
                    /* @hack spelling: the factor written out in each
                     * product, where a local would do */
                    lead.x *= (len - KNUKLES_LEAD_NONE) * KNUKLES_LEAD_RAMP;
                    lead.y *= (len - KNUKLES_LEAD_NONE) * KNUKLES_LEAD_RAMP;
                    lead.z *= (len - KNUKLES_LEAD_NONE) * KNUKLES_LEAD_RAMP;
                }
            }
            vec.x += lead.x * KNUKLES_LEAD;
            vec.y += lead.y * KNUKLES_LEAD;
            vec.z += lead.z * KNUKLES_LEAD;
        }
    } else {
        vec.x = stack->place.target.x;
        vec.y = stack->place.target.y;
        vec.z = stack->place.target.z;
    }
    if (camera_steps < KNUKLES_FIRST_STEPS) {
        camera_state.target.x = vec.x;
        camera_state.target.y = vec.y;
        camera_state.target.z = vec.z;
    } else {
        camera_state.target.x =
            stack->place.target.x +
            ((vec.x - stack->place.target.x) * KNUKLES_EASE);
        camera_state.target.y =
            stack->place.target.y +
            ((vec.y - stack->place.target.y) * KNUKLES_EASE);
        camera_state.target.z =
            stack->place.target.z +
            ((vec.z - stack->place.target.z) * KNUKLES_EASE);
    }
    camera_state.pos = stack->place.pos;
    if (camera_steps < KNUKLES_FIRST_STEPS) {
        struct angles ang;

        vec = offset;
        ang.x = KNUKLES_PITCH;
        ang.y = -player_work[camera_current_index]->ang.y;
        ang.z = 0;
        knukles_turn_yxz(&ang, &vec);
        camera_state.pos.x = player_work[camera_current_index]->pos.x + vec.x;
        camera_state.pos.y = player_work[camera_current_index]->pos.y - vec.y;
        camera_state.pos.z = player_work[camera_current_index]->pos.z + vec.z;
    }
    if (!work->pitch_held) {
        Angle *pitch = &stack->place.ang.x;
        Sint16 off;
        Angle turn;

        off = angle_difference(KNUKLES_PITCH, *pitch);
        /* @hack spelling: turn set to PITCH_TURN in each arm, where
         * PITCH_TURN in the call would do */
        switch (player_character(camera_current_index)) {
        case PLAYER_TAILS_MECH:
        case PLAYER_EGGMAN_MECH:
            turn = PITCH_TURN;
            lift = PITCH_LIFT_MECH;
            break;
        default:
            turn = PITCH_TURN;
            lift = PITCH_LIFT;
            break;
        }
        if (off < 0) {
            off = -off;
        }
        if (off > KNUKLES_PITCH_NEAR) {
            *pitch = angle_approach(*pitch, KNUKLES_PITCH, turn);
            if (angle_distance(KNUKLES_PITCH, *pitch) < KNUKLES_PITCH_NEAR) {
                lift = PITCH_LIFT;
            }
            camera_state.pos.y += lift * njSin(*pitch - KNUKLES_PITCH);
        }
    }
    if (camera_in_colli_lr == 0) {
        vector_subtract(&camera_state.pos,
                        &player_work[camera_current_index]->pos, &vec);
        len = njScalor(&vec);
        if (work->turned) {
            heading = work->ang;
        } else {
            heading =
                (Angle)(atan2f(camera_state.pos.z -
                                   player_work[camera_current_index]->pos.z,
                               camera_state.pos.x -
                                   player_work[camera_current_index]->pos.x) *
                        ANGLE_PER_RADIAN);
        }
        work->turned = 0;
        if (player_pad_records[camera_current_index].on & PDD_DGT_TL) {
            heading += KNUKLES_TRIGGER_TURN;
            work->ang = heading;
            work->turned = 1;
            camera_set_adjust(camera_current_index,
                              camera_top(camera_current_index),
                              CAMERA_ADJUST_NONE);
        }
        if (player_pad_records[camera_current_index].on & PDD_DGT_TR) {
            heading -= KNUKLES_TRIGGER_TURN;
            work->ang = heading;
            work->turned = 1;
            camera_set_adjust(camera_current_index,
                              camera_top(camera_current_index),
                              CAMERA_ADJUST_NONE);
        }
        if (work->turned) {
            camera_state.target.y = ((player_work[camera_current_index]->pos.y +
                                      target_height - stack->place.target.y) *
                                     KNUKLES_EASE) +
                                    stack->place.target.y;
            camera_state.pos.x = (njCos(heading) * len) +
                                 player_work[camera_current_index]->pos.x;
            camera_state.pos.z = (njSin(heading) * len) +
                                 player_work[camera_current_index]->pos.z;
        }
    }
    vec.x = player_work[camera_current_index]->pos.x - camera_state.pos.x;
    vec.y = player_work[camera_current_index]->pos.y - camera_state.pos.y;
    vec.z = player_work[camera_current_index]->pos.z - camera_state.pos.z;
    if (vec.x == 0.0F && vec.y == 0.0F && vec.z == 0.0F) {
        vec.x += 0.01F;
    }
    len = njScalor(&vec);
    njUnitVector(&vec);
    if (len > FAR) {
        len = ((len - FAR) * KNUKLES_FAR_KEPT) + FAR;
        if (!work->unlimited) {
            if (len > FARTHEST) {
                len = FARTHEST;
            }
        }
        work->pitch_held = 0;
    } else if (len < NEAR) {
        /* @hack spelling: the product written in each arm, where one above
         * the test would do */
        if (!work->turned) {
            len *= BACK_OFF;
            if (!work->unlimited) {
                if (len < NEAREST) {
                    len = NEAREST;
                }
            }
        } else {
            len *= BACK_OFF;
        }
    }
    camera_state.pos.x =
        player_work[camera_current_index]->pos.x - (vec.x * len);
    camera_state.pos.y =
        player_work[camera_current_index]->pos.y - (vec.y * len);
    camera_state.pos.z =
        player_work[camera_current_index]->pos.z - (vec.z * len);
    /* @hack spelling: len reused for FLAT_NEAREST, where FLAT_NEAREST in
     * place would do */
    len = FLAT_NEAREST;
    vec.x = camera_state.pos.x - player_work[camera_current_index]->pos.x;
    vec.z = camera_state.pos.z - player_work[camera_current_index]->pos.z;
    if ((vec.x * vec.x) + (vec.z * vec.z) < len * len) {
        vec.y = 0.0F;
        if (vec.x == 0.0F && vec.z == 0.0F) {
            vec.x += 0.01F;
        }
        njUnitVector(&vec);
        camera_state.pos.x =
            player_work[camera_current_index]->pos.x + (vec.x * len);
        camera_state.pos.z =
            player_work[camera_current_index]->pos.z + (vec.z * len);
    }
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    if (camera_steps < KNUKLES_FIRST_STEPS) {
        camera_state.unk_04 = 0;
    } else {
        camera_state.unk_04 = 1;
    }
    camera_state.trigger_push = 0;
    if (player_work[camera_current_index]->mode == PLAYER_MODE_LEDGE_CLIMB) {
        look_replaced = camera_mode_at(camera_current_index,
                                       camera_top(camera_current_index));
        camera_set_user_mode(camera_current_index,
                             camera_top(camera_current_index), look_down);
        camera_set_adjust(camera_current_index,
                          camera_top(camera_current_index),
                          CAMERA_ADJUST_RELATIVE1);
        camera_delay_settle(camera_current_index, CAMERA_SETTLE_DELAY);
        /* camera_run_mode counts a step in slot->steps once it returns, so
         * the new mode's first step finds 0 */
        slot->steps = -1;
    }
}

/* the look's step, which stands in for the mode on top until the player
 * leaves PLAYER_MODE_LEDGE_CLIMB */
static void look_down(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_POINT3 *point = (NJS_POINT3 *)slot->work;

    if (slot->steps == 0) {
        camera_state.target = player_work[camera_current_index]->pos;
        camera_state.target.y +=
            player_records[camera_current_index]->consts.target_height +
            LOOK_RISE;
        *point = camera_state.target;
    } else {
        camera_state.target = *point;
    }
    camera_state.ang.x = LOOK_PITCH;
    camera_state.ang.y =
        -player_work[camera_current_index]->ang.y - QUARTER_TURN;
    camera_state.ang.z = 0;
    camera_state.dist = LOOK_DIST;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
    camera_state.type = CAMERA_TARGET_ANG_DIST;
    if (player_work[camera_current_index]->mode != PLAYER_MODE_LEDGE_CLIMB) {
        camera_set_mode(camera_current_index, camera_top(camera_current_index),
                        look_replaced);
        camera_set_adjust(camera_current_index,
                          camera_top(camera_current_index),
                          CAMERA_ADJUST_RELATIVE3);
        camera_delay_settle(camera_current_index, CAMERA_SETTLE_DELAY);
        /* camera_run_mode counts a step in slot->steps once it returns, so
         * the new mode's first step finds 0 */
        slot->steps = -1;
    }
}
