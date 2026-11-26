/* the camera's BOSSKLAMATH mode: KLAMATH about a point between two that the
 * caller keeps */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bossklamath.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/task.h"

/* param is camera_bossklamath_start's copy of its param */
struct bossklamath_work {
    Sint32 unk_00;
    /* @hack redundant: drop, which nothing sets but to 0, cut by a tenth and
     * taken off pos.y each step */
    Float drop;
    struct camera_bossklamath param;
};

/* each camera's BOSSKLAMATH work, as camera_bossklamath_start last set it */
static struct bossklamath_work *bossklamath_works[CAMERAS];

int camera_bossklamath_start(int camera, struct camera_bossklamath *param)
{
    struct camera_slot *slot;
    struct bossklamath_work *work;

    if (camera_mode_at(camera, camera_top(camera)) == CAMERA_MODE_INIT) {
        camera_remove_mode(camera, camera_top(camera));
    }
    slot = camera_push_mode(camera, CAMERA_MODE_BOSSKLAMATH);
    if (slot == NULL) {
        return 0;
    }
    camera_set_adjust(camera, camera_top(camera), param->adjust);
    work = (struct bossklamath_work *)slot->work;
    work->param = *param;
    bossklamath_works[camera] = work;
    return 1;
}

void camera_mode_bossklamath(struct camera_stack *stack,
                             struct camera_slot *slot)
{
    struct bossklamath_work *work = (struct bossklamath_work *)slot->work;
    NJS_VECTOR dir;
    NJS_POINT3 center;
    Float length;

    if (slot->steps == 0) {
        work->drop = 0.0F;
    }
    center.x = (work->param.to->x * work->param.part) +
               (work->param.from->x * (1.0F - work->param.part));
    /* @hack redundant: center's y, which nothing reads */
    center.y = (player_work[0]->pos.y * work->param.part) +
               (work->param.from->y * (1.0F - work->param.part));
    center.z = (work->param.to->z * work->param.part) +
               (work->param.from->z * (1.0F - work->param.part));
    /* @hack spelling: target set a member at a time, where one copy of the
     * player's pos would do */
    camera_state.target.x = player_work[camera_current_index]->pos.x;
    camera_state.target.y = player_work[camera_current_index]->pos.y;
    camera_state.target.z = player_work[camera_current_index]->pos.z;
    dir.x = player_work[camera_current_index]->pos.x - center.x;
    dir.y = 0.0F;
    dir.z = player_work[camera_current_index]->pos.z - center.z;
    njUnitVector(&dir);
    camera_state.pos.x = camera_state.target.x + (work->param.dist * dir.x);
    camera_state.pos.y = work->param.height + camera_state.target.y;
    camera_state.pos.z = camera_state.target.z + (work->param.dist * dir.z);
    work->drop *= 0.9F;
    camera_state.pos.y -= work->drop;
    dir.x = camera_state.pos.x - center.x;
    dir.y = 0.0F;
    dir.z = camera_state.pos.z - center.z;
    length = njUnitVector(&dir);
    camera_state.target.y +=
        work->param.height * (length - work->param.dist) / length;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = work->param.collides;
    camera_state.trigger_push = work->param.trigger_push;
}
