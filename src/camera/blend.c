/* the adjusts that blend a camera's place from where it was toward where its
 * mode puts it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/blend.h"
#include "camera/camera.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/vector.h"

/* blend_ends' indices: where a blend starts, and where it ends */
#define BLEND_FROM 0
#define BLEND_TO 1

/* member's value blend_progress of the way from blend_ends[BLEND_FROM] to
 * blend_ends[BLEND_TO]. BLENDED_ANGLE turns the short way round */
#define BLENDED(member)                                                        \
    (((blend_ends[BLEND_TO].member - blend_ends[BLEND_FROM].member) *          \
      blend_progress) +                                                        \
     blend_ends[BLEND_FROM].member)
#define BLENDED_ANGLE(member)                                                  \
    angle_approach(blend_ends[BLEND_FROM].member, blend_ends[BLEND_TO].member, \
                   angle_distance(blend_ends[BLEND_TO].member,                 \
                                  blend_ends[BLEND_FROM].member) *             \
                       blend_progress)

/* the angles blend_turns_mixed takes blend_turns toward */
#define TURN_MIX_X ((Angle)(20 * ANGLE_PER_DEGREE))
#define TURN_MIX_Y ((Angle)(30 * ANGLE_PER_DEGREE))

/* an adjust's length: the steps it takes to blend the whole way */
struct blend_length {
    Float steps;
    Sint32 unk_04;
};

/* the lengths of CAMERA_ADJUST_THREE1 to THREE5, then of RELATIVE1 to
 * RELATIVE6 */
/* clang-format off */
static struct blend_length
    blend_lengths[CAMERA_ADJUST_RELATIVE6 - CAMERA_ADJUST_THREE1 + 1] = {
    {60.0F, 0}, {180.0F, 3}, {600.0F, 10}, {1800.0F, 30}, {3600.0F, 60},
    {60.0F, 0}, {180.0F, 3}, {600.0F, 10}, {1800.0F, 30}, {3600.0F, 60},
    {30000.0F, 60},
};
/* clang-format on */

/* the adjust's work, read as angles. on the adjust's first step, x and y
 * take angle_distance from the place's x and y to blend_aim's */
static struct angles *blend_turns;
/* the blend's two ends, by BLEND_FROM and BLEND_TO: the stack's place, and
 * camera_state's members */
static struct camera_place blend_ends[2];
/* the share of the way the blend has come, 0 to 1 */
static Float blend_progress;
/* blend_turns' x and y, blend_progress of the way to TURN_MIX_X and
 * TURN_MIX_Y */
static struct angles blend_turns_mixed;
/* the x and y angles that aim from blend_ends[BLEND_TO]'s pos at the
 * current camera's player. they are as look_angles gives them */
static struct angles blend_aim;

static Bool blend_is_relative(void);
static void blend_pos(void);
static void blend_target(void);
static void blend_ang(void);
static void blend_dir(void);
static void blend_dist(void);
static void blend_aim_at_player(void);

void camera_blend(struct camera_stack *stack, struct camera_slot *adjust)
{
    NJS_POINT3 origin;

    blend_turns = (struct angles *)adjust->work;
    blend_ends[BLEND_FROM] = stack->place;
    blend_ends[BLEND_TO].pos = camera_state.pos;
    /* @hack spelling: target copied ahead of ang and dir, not in the
     * members' order */
    blend_ends[BLEND_TO].target = camera_state.target;
    blend_ends[BLEND_TO].ang = camera_state.ang;
    blend_ends[BLEND_TO].dir = camera_state.dir;
    blend_ends[BLEND_TO].dist = camera_state.dist;
    blend_aim_at_player();
    if (adjust->steps == 0) {
        blend_turns->x =
            angle_distance(blend_ends[BLEND_FROM].ang.x, blend_aim.x);
        blend_turns->y =
            angle_distance(blend_ends[BLEND_FROM].ang.y, blend_aim.y);
    }
    blend_progress =
        adjust->steps /
        blend_lengths[camera_adjust_at(camera_current_index,
                                       camera_top(camera_current_index)) -
                      CAMERA_ADJUST_THREE1]
            .steps;
    if (blend_progress > 1.0F || blend_progress < 0.0F) {
        blend_progress = 1.0F;
    }
    blend_turns_mixed.x =
        ((TURN_MIX_X - blend_turns->x) * blend_progress) + blend_turns->x;
    blend_turns_mixed.y =
        ((TURN_MIX_Y - blend_turns->y) * blend_progress) + blend_turns->y;
    if (blend_is_relative()) {
        camera_player_place(camera_current_index, &origin, NULL);
        vector_subtract(&blend_ends[BLEND_FROM].pos, &origin,
                        &blend_ends[BLEND_FROM].pos);
        vector_subtract(&blend_ends[BLEND_FROM].target, &origin,
                        &blend_ends[BLEND_FROM].target);
        vector_subtract(&blend_ends[BLEND_TO].pos,
                        &player_work[camera_current_index]->pos,
                        &blend_ends[BLEND_TO].pos);
        vector_subtract(&blend_ends[BLEND_TO].target,
                        &player_work[camera_current_index]->pos,
                        &blend_ends[BLEND_TO].target);
    }
    switch (camera_state.type) {
    case CAMERA_POS_TARGET:
        blend_pos();
        blend_target();
        break;
    case CAMERA_POS_ANG_DIST:
        blend_pos();
        blend_ang();
        blend_dist();
        break;
    case CAMERA_POS_DIR:
        blend_pos();
        blend_dir();
        break;
    case CAMERA_TARGET_ANG_DIST:
        blend_target();
        blend_ang();
        blend_dist();
        break;
    case CAMERA_TARGET_DIR:
        blend_target();
        blend_dir();
        break;
    }
    if (blend_is_relative()) {
        vector_add(&camera_state.pos, &player_work[camera_current_index]->pos,
                   &camera_state.pos);
        vector_add(&camera_state.target,
                   &player_work[camera_current_index]->pos,
                   &camera_state.target);
    }
}

/* TRUE while the current camera's top adjust is a RELATIVE one, else FALSE */
static Bool blend_is_relative(void)
{
    /* @hack spelling: the result through an if that returns TRUE or FALSE,
     * where a return of the comparison would do */
    /* @hack spelling: the test against CAMERA_ADJUST_THREE5 with >, where >=
     * CAMERA_ADJUST_RELATIVE1 would do */
    if (camera_adjust_at(camera_current_index,
                         camera_top(camera_current_index)) >
        CAMERA_ADJUST_THREE5) {
        return TRUE;
    }
    return FALSE;
}

static void blend_pos(void)
{
    camera_state.pos.x = BLENDED(pos.x);
    camera_state.pos.y = BLENDED(pos.y);
    camera_state.pos.z = BLENDED(pos.z);
}

static void blend_target(void)
{
    camera_state.target.x = BLENDED(target.x);
    camera_state.target.y = BLENDED(target.y);
    camera_state.target.z = BLENDED(target.z);
}

static void blend_ang(void)
{
    camera_state.ang.x = BLENDED_ANGLE(ang.x);
    camera_state.ang.y = BLENDED_ANGLE(ang.y);
    camera_state.ang.z = BLENDED_ANGLE(ang.z);
}

static void blend_dir(void)
{
    camera_state.dir.x = BLENDED(dir.x);
    camera_state.dir.y = BLENDED(dir.y);
    camera_state.dir.z = BLENDED(dir.z);
}

static void blend_dist(void)
{
    camera_state.dist = BLENDED(dist);
}

static void blend_aim_at_player(void)
{
    /* @hack type: from_player, a whole vector of which only x and z are
     * used, where two Floats would do */
    NJS_VECTOR from_player;

    from_player.x =
        blend_ends[BLEND_TO].pos.x - player_work[camera_current_index]->pos.x;
    from_player.z =
        blend_ends[BLEND_TO].pos.z - player_work[camera_current_index]->pos.z;
    blend_aim.y =
        (Angle)(atan2f(from_player.x, from_player.z) * ANGLE_PER_RADIAN);
    blend_aim.x = (Angle)(atan2f(player_work[camera_current_index]->pos.y -
                                     blend_ends[BLEND_TO].pos.y,
                                 njSqrt((from_player.x * from_player.x) +
                                        (from_player.z * from_player.z))) *
                          ANGLE_PER_RADIAN);
}
