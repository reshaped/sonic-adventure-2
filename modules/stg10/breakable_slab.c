/* the breakable slab, BOAB in Metal Harbor's object list. it breaks into seven
 * pieces at a touch */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg10/breakable_slab.h"
#include "stg10/breakable_slab/text.inc"
#include "system/task.h"
#include "vmu/vibration.h"

/* the slab's modes, in its work's mode, which its placement keeps */
enum breakable_slab_mode { MODE_WHOLE, MODE_BROKEN };

/* the pieces fly apart at PIECE_SPEED, plus up to PIECE_SPREAD at random. they
 * also fall at player 0's falling speed plus PIECE_FALL, toward a floor at y
 * PIECE_FLOOR */
#define PIECE_FALL (-0.1F)
#define PIECE_SPEED 1.0F
#define PIECE_SPREAD 1.0F
#define PIECE_FLOOR (-1000.0F)
#define PIECE_FLAGS DEBRIS_TRAIL
/* debris_break_sized spins each piece at random. the range shrinks to 0 as the
 * radius of the piece's model grows to PIECE_RADIUS */
#define PIECE_RADIUS 30.0F
#define BREAK_SOUND 5
/* vibration_start's power, frequency and time for player 0's pad as the slab
 * breaks */
#define VIBRATION_POWER 6
#define VIBRATION_FREQ 25
#define VIBRATION_TIME 0

#define SLAB_PIECES 7
#define SLAB_SHAPES 1

/* Breakable slab and the two sizes of debris. */

static NJS_TEXNAME breakable_slab_textures[1] = {
#include "stg10/breakable_slab/breakable_slab_textures.inc"
};

static NJS_TEXLIST breakable_slab_texlist = {
#include "stg10/breakable_slab/breakable_slab_texlist.inc"
};

static Sint16 breakable_slab_polygons[77] = {
#include "stg10/breakable_slab/breakable_slab_polygons.inc"
};

static Sint32 breakable_slab_vertices[83] = {
#include "stg10/breakable_slab/breakable_slab_vertices.inc"
};

static NJS_CNK_MODEL breakable_slab_mesh = {
#include "stg10/breakable_slab/breakable_slab_mesh.inc"
};

static NJS_CNK_OBJECT breakable_slab_object[1] = {
#include "stg10/breakable_slab/breakable_slab_object.inc"
};

static NJS_TEXNAME breakable_slab_large_piece_textures[1] = {
#include "stg10/breakable_slab/breakable_slab_large_piece_textures.inc"
};

static NJS_TEXLIST breakable_slab_large_piece_texlist = {
#include "stg10/breakable_slab/breakable_slab_large_piece_texlist.inc"
};

static Sint16 breakable_slab_large_piece_polygons[103] = {
#include "stg10/breakable_slab/breakable_slab_large_piece_polygons.inc"
};

static Sint32 breakable_slab_large_piece_vertices[99] = {
#include "stg10/breakable_slab/breakable_slab_large_piece_vertices.inc"
};

static NJS_CNK_MODEL breakable_slab_large_piece_model = {
#include "stg10/breakable_slab/breakable_slab_large_piece_model.inc"
};

static NJS_CNK_OBJECT breakable_slab_large_piece_object = {
#include "stg10/breakable_slab/breakable_slab_large_piece_object.inc"
};

static NJS_TEXNAME breakable_slab_small_piece_textures[1] = {
#include "stg10/breakable_slab/breakable_slab_small_piece_textures.inc"
};

static NJS_TEXLIST breakable_slab_small_piece_texlist = {
#include "stg10/breakable_slab/breakable_slab_small_piece_texlist.inc"
};

static Sint16 breakable_slab_small_piece_polygons[65] = {
#include "stg10/breakable_slab/breakable_slab_small_piece_polygons.inc"
};

static Sint32 breakable_slab_small_piece_vertices[59] = {
#include "stg10/breakable_slab/breakable_slab_small_piece_vertices.inc"
};

static NJS_CNK_MODEL breakable_slab_small_piece_model = {
#include "stg10/breakable_slab/breakable_slab_small_piece_model.inc"
};

static NJS_CNK_OBJECT breakable_slab_small_piece_object = {
#include "stg10/breakable_slab/breakable_slab_small_piece_object.inc"
};

/* clang-format off */
NJS_TEXLIST *breakable_slab_texlists[6] = {
#include "stg10/breakable_slab/breakable_slab_texlists.inc"
};
/* the pieces the slab breaks into: three large ones along z, then four small
 * ones */
static struct debris_part breakable_slab_pieces[SLAB_PIECES] = {
#include "stg10/breakable_slab/breakable_slab_pieces.inc"
};
/* the whole slab's box, then the broken slab's */
static struct collision_shape breakable_slab_shapes[2] = {
#include "stg10/breakable_slab/breakable_slab_shapes.inc"
};
/* clang-format on */

static void breakable_slab_destroy(struct task *task);
static void breakable_slab_step(struct task *task);
static void breakable_slab_draw(struct task *task);

void breakable_slab_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = breakable_slab_draw;
    task->step = breakable_slab_step;
    task->destructor = breakable_slab_destroy;
    if (task->placement != NULL) {
        work->mode = placement_get_state(task);
    }
    /* @hack spelling: the mode tested as a truth value, where a test against
     * MODE_WHOLE would do */
    if (work->mode) {
        collision_init(task, &breakable_slab_shapes[MODE_BROKEN], SLAB_SHAPES,
                       COLLISION_OBJECT);
    } else {
        collision_init(task, &breakable_slab_shapes[MODE_WHOLE], SLAB_SHAPES,
                       COLLISION_OBJECT);
    }
}

static void breakable_slab_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, work->mode);
    }
}

static void breakable_slab_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR spd;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the tests nested, where one test joined by && would
     * do, and the mode tested as a truth value, where a test against
     * MODE_WHOLE would do */
    if (!work->mode) {
        if (work->collision != NULL &&
            (work->collision->flags & COLLISION_TOUCHED)) {
            pos = work->pos;
            spd.x = 0.0F;
            spd.y = player_motion[0]->spd.y + PIECE_FALL;
            spd.z = 0.0F;
            work->mode = MODE_BROKEN;
            debris_break_sized(&pos, work->ang.y, breakable_slab_pieces,
                               SLAB_PIECES, PIECE_SPEED, PIECE_SPREAD,
                               PIECE_FLOOR, &spd, PIECE_FLAGS, PIECE_RADIUS);
            sound_play(BREAK_SOUND, NULL, 0, 0);
            vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_TIME);
            task_kill(task);
            /* @hack spelling: a goto to the step's end, where a return would
             * do */
            goto end;
        }
    }
    /* @hack spelling: the mode tested as a truth value, where a test against
     * MODE_WHOLE would do */
    if (!work->mode || player_work[0]->pos.y < work->pos.y) {
        collision_register(task);
    }
end:;
}

static void breakable_slab_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    /* @hack spelling: the mode tested as a truth value, where a test against
     * MODE_WHOLE would do */
    if (work->mode) {
        return;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&breakable_slab_texlist);
    njCnkEasyDrawModel(breakable_slab_object->model);
    njPopMatrixEx();
}
