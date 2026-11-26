/* the space pickup, PIC in Meteor Herd's and Mad Space's object lists */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/space_pickup.h"
#include "shared/switch.h"
#include "system/angle.h"
#include "system/task.h"

#define PICKUP_TEXTURES 3
/* the height from pos at which a held pickup draws */
#define HELD_HEIGHT (-2.0F)
/* how far above pos a lying pickup draws its shadow, and the shadow's scale
 * across x and z */
#define SHADOW_LIFT 1.0F
#define SHADOW_SCALE 3.0F

NJS_TEXLIST *space_pickup_textures[PICKUP_TEXTURES + 1] = {
#include "shared/space_pickup/space_pickup_textures.inc"
};
/* how far a holder's draw turns the nodes it holds the pickup with, which
 * pickup_setup takes */
static Angle hold_angle = 0x300;

#define HOLDABLE_RELEASES_ANIMALS 0
#include "shared/holdable_motion_impl.h" /* IWYU pragma: keep -- the code it compiles here */

static void holdable_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 texture = (Uint32)work->scl.x % PICKUP_TEXTURES;
    NJS_CNK_MODEL *model;

    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- TEX(0-2)";
    }
    njSetTexture(space_pickup_textures[texture]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->flags & WORK_HELD_BY_PLAYER) {
        njTranslate(NULL, 0.0F, HELD_HEIGHT, 0.0F);
        njRotateZ(NULL, player_work[work->submode]->ang.z);
        njRotateX(NULL, player_work[work->submode]->ang.x);
    } else {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, SHADOW_LIFT, 0.0F);
        njScale(NULL, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
        njCnkModDrawObject(&shadow_object);
        njPopMatrixEx();
    }
    njRotateY(NULL, work->ang.y);
    model = task->extra.p;
    njTranslate(NULL, model->center.x, model->center.y, model->center.z);
    njMultiMatrix(NULL, &((struct body *)task->force.p)->rolled);
    /* @hack redundant: model loaded from task->extra again, where the first
     * load would do */
    model = task->extra.p;
    njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
    njCnkEasyDrawModel(task->extra.p);
    njPopMatrixEx();
}

#define HOLDABLE_SETUP static void pickup_setup
#include "shared/holdable_setup.h"

void space_pickup_init(struct task *task)
{
    pickup_setup(task, &space_pickup_first_texlist, &space_pickup_model,
                 hold_angle, OBJECT_MAY_LEAVE);
}
