/* the mech bosses' camera: two views for camera 0's BOSSPOINT mode, and a
 * shake. Big Foot, Hot Shot and Flying Dog use them */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bosspoint.h"
#include "camera/camera.h"
#include "camera/stack.h"
#include "player/player.h"
#include "shared/mech_boss_camera.h"
#include "system/task.h"
#include "system/vector.h"

#define CAMERA 0

/* mech_boss_camera_step's camera stands beyond the point, away from the
 * world's origin. RISE raises the point's offset from the origin. the camera's
 * offset from the point is BEYOND times that */
#define RISE 40.0F
#define BEYOND 0.5F

#define FAR_HEIGHT 100.0F
#define FAR_DIST 100.0F
#define FAR_PART 0.2F
#define FAR_LEVEL 0

/* a shake's steps, as camera_shake takes them, and player 0's distance at
 * which its size is 1. camera_shake shakes for one step more */
#define HEAVY_STEPS 5
#define HEAVY_RANGE 200.0F
#define LIGHT_STEPS 2
#define LIGHT_RANGE 400.0F

static const NJS_POINT3 mech_boss_camera_origin = {0.0F, 0.0F, 0.0F};

void mech_boss_camera_step(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_POINT3 **point = (NJS_POINT3 **)slot->work;
    NJS_POINT3 pos;
    /* @hack spelling: origin reused for the look, where a local of its own
     * would do */
    NJS_POINT3 origin;
    NJS_VECTOR offset;
    /* @hack spelling: reference, a pointer to origin, where origin read in
     * place would do */
    NJS_POINT3 *reference;

    camera_state.unk_04 = 1;
    camera_state.trigger_push = 1;
    pos = **point;
    /* @hack redundant: the target set before the place, though it is set
     * again after */
    camera_state.target = player_work[0]->pos;
    reference = &origin;
    *reference = mech_boss_camera_origin;
    offset.x = pos.x - reference->x;
    offset.y = pos.y - reference->y + RISE;
    offset.z = pos.z - reference->z;
    camera_state.pos.x = pos.x + (offset.x * BEYOND);
    camera_state.pos.y = pos.y + (offset.y * BEYOND);
    camera_state.pos.z = pos.z + (offset.z * BEYOND);
    camera_state.target = player_work[0]->pos;
    origin = camera_state.target;
    njSubVector(&origin, &camera_state.pos);
    camera_state.dist = njScalor(&origin);
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
}

void mech_boss_camera_far(struct task *task, int unused)
{
    camera_bosspoint_ease_to(CAMERA, FAR_HEIGHT, FAR_DIST, FAR_PART);
    camera_bosspoint_set_level(CAMERA, FAR_LEVEL);
}

void mech_boss_camera_near(int unused)
{
    camera_bosspoint_ease_to(CAMERA, MECH_BOSS_CAMERA_HEIGHT,
                             MECH_BOSS_CAMERA_DIST, MECH_BOSS_CAMERA_PART);
}

void mech_boss_camera_shake(NJS_POINT3 *pos, Sint8 kind)
{
    Float distance = vector_distance(pos, &player_work[0]->pos);
    Float size;
    int steps;

#ifdef NONMATCHING
    /* GCC can't see that every path sets size and steps. kind is one of enum
     * mech_boss_shake, as mech_boss_camera.h requires */
    size = 0.0F;
    steps = 0;
#endif
    switch (kind) {
    case MECH_BOSS_SHAKE_HEAVY:
        size = distance * njInvertSqrt(HEAVY_RANGE) * njInvertSqrt(HEAVY_RANGE);
        steps = HEAVY_STEPS;
        break;
    case MECH_BOSS_SHAKE_LIGHT:
        size = distance * njInvertSqrt(LIGHT_RANGE) * njInvertSqrt(LIGHT_RANGE);
        steps = LIGHT_STEPS;
    }
    camera_shake(CAMERA, steps, size);
}
