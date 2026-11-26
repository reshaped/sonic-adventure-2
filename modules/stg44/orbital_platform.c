/* the orbital platform, which Mad Space's carriage manager makes. it makes a
 * second platform behind itself, and the two run round a path. they stay level
 * as they turn. stg44/carriage_path_impl.h gives them their init and
 * destructor, and stg44/carriage_step_impl.h their step */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg44/props.h"
#include "stg44/scenery_shapes.h"
#include "system/task.h"

#define FLAT_CARRIAGE
#define CARRIAGE_INIT mad_space_platform_init
#define LEAD_COLLISION mad_space_platform_collision
#define CARRIAGE_ANGLES current_matrix_angles_yxz
#define LOOK_AHEAD 20.0F
#define HEAD_START 180.0F
#define NEAR_RANGE 100.0F
#define PIECE_HEIGHT 15.0F
#define PIECE_OFFSET 0.0F

static Float acceleration = 0.03F;
static Float damping = 0.98F;
NJS_TEXLIST *mad_space_platform_textures[] = {&mad_space_platform_texlist,
                                              NULL};

#include "stg44/carriage_path_impl.h"

static void carriage_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD Y";
    }
    njSetTexture(&mad_space_platform_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkModDrawObject(&mad_space_platform_shadow);
    njTranslateEx((NJS_VECTOR *)mad_space_platform_body->pos);
    njCnkEasyDrawModel(mad_space_platform_body->model);
    njPopMatrixEx();
}

#include "stg44/carriage_step_impl.h" /* IWYU pragma: keep -- the code it compiles here */
