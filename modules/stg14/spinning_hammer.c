/* the spinning hammer, HAMMER2 in Radical Highway's object list: a drum above
 * a bar. the drum spins about its own x axis. both are solid. it glows as
 * stg14/hammer.c's hammer does */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg14/hammer.h"
#include "stg14/spinning_hammer.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

#include "stg14/spinning_hammer/text.inc"

/* the settings the set file gives a hammer in its work, as
 * spinning_hammer.h lists them */
#define TURN_RATE(work) ((Uint16)(work)->ang.x)
#define LENGTH_SCALE(work) ((work)->scl.x + 1.0F)
#define THICKNESS_SCALE(work) ((work)->scl.y + 1.0F)
/* the drum's turn about x, which the step works out from game_state.steps */
#define DRUM_TURN(work) ((work)->timer)

/* the drum's collision model reaches HALF_LENGTH each way along x from its
 * middle, and DRUM_RADIUS from its axis. the bar's reaches BAR_HALF_HEIGHT
 * above and below its center. THICKNESS_SCALE scales the drum's radius and the
 * bar's height. so the drum's bottom stays DRUM_CLEARANCE above pos, and the
 * bar's BAR_CLEARANCE above pos */
#define HALF_LENGTH 50.0F
#define DRUM_RADIUS 25.0F
#define DRUM_CLEARANCE 5.0F
#define BAR_HALF_HEIGHT 5.0F
#define BAR_CLEARANCE 2.5F
#define DRUM_HEIGHT(work)                                                      \
    ((DRUM_RADIUS * THICKNESS_SCALE(work)) + DRUM_CLEARANCE)
#define BAR_HEIGHT(work)                                                       \
    ((BAR_HALF_HEIGHT * THICKNESS_SCALE(work)) + BAR_CLEARANCE)
/* the bar's scale along z is BAR_DEPTH_SCALE times THICKNESS_SCALE */
#define BAR_DEPTH_SCALE 2.0F
/* the world collects a hammer's copies only while a player lies near: within
 * its radius plus REACH_MARGIN */
#define REACH_MARGIN 10.0F

/* the drum's sound, which each step plays at pos for SPIN_SOUND_DURATION + 1
 * sound updates */
#define SPIN_SOUND 4
#define SPIN_SOUND_PRIORITY 1
#define SPIN_SOUND_VOLUME 0
#define SPIN_SOUND_DURATION 5

/* the glow's color blends glow_color_low toward glow_color_high by
 * |sin(game_state.steps << GLOW_RATE_SHIFT)| * 0.5 + 0.5. so it pulses every
 * 64 steps, between the color halfway to glow_color_high and glow_color_high
 * itself */
#define GLOW_RATE_SHIFT 9
/* the shadow is a translucent black, flat on the plane SHADOW_LIFT above
 * pos */
#define SHADOW_COLOR 0x20000000
#define SHADOW_LIFT 0.1F

/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RADIUS(task) (*(Float *)&(task)->unk_3c)
/* TASK_FORCE's records, whose turn.x turns a riding player with the drum */
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* Rotating hammer and the drum and bar collision models used for its two
 * surfaces. */

static NJS_TEXNAME spinning_hammer_textures[4] = {
#include "stg14/spinning_hammer/spinning_hammer_textures.inc"
};

static NJS_TEXLIST spinning_hammer_texlist = {
#include "stg14/spinning_hammer/spinning_hammer_texlist.inc"
};

static Sint16 spinning_hammer_glow_polygons[91] = {
#include "stg14/spinning_hammer/spinning_hammer_glow_polygons.inc"
};

static Sint32 spinning_hammer_glow_vertices[99] = {
#include "stg14/spinning_hammer/spinning_hammer_glow_vertices.inc"
};

static NJS_CNK_MODEL spinning_hammer_glow_model = {
#include "stg14/spinning_hammer/spinning_hammer_glow_model.inc"
};

static NJS_CNK_OBJECT spinning_hammer_glow_object = {
#include "stg14/spinning_hammer/spinning_hammer_glow_object.inc"
};

static Sint16 spinning_hammer_polygons[465] = {
#include "stg14/spinning_hammer/spinning_hammer_polygons.inc"
};

static Sint32 spinning_hammer_vertices[347] = {
#include "stg14/spinning_hammer/spinning_hammer_vertices.inc"
};

static NJS_CNK_MODEL spinning_hammer_model = {
#include "stg14/spinning_hammer/spinning_hammer_model.inc"
};

static NJS_CNK_OBJECT spinning_hammer_object = {
#include "stg14/spinning_hammer/spinning_hammer_object.inc"
};

static NJS_MATERIAL spinning_hammer_drum_materials[1] = {
#include "stg14/spinning_hammer/spinning_hammer_drum_materials.inc"
};

static Sint16 spinning_hammer_drum_indices_0[35] = {
#include "stg14/spinning_hammer/spinning_hammer_drum_indices_0.inc"
};

static NJS_MESHSET spinning_hammer_drum_meshsets[1] = {
#include "stg14/spinning_hammer/spinning_hammer_drum_meshsets.inc"
};

static NJS_POINT3 spinning_hammer_drum_points[12] = {
#include "stg14/spinning_hammer/spinning_hammer_drum_points.inc"
};

static NJS_VECTOR spinning_hammer_drum_normals[12] = {
#include "stg14/spinning_hammer/spinning_hammer_drum_normals.inc"
};

static NJS_MODEL spinning_hammer_drum_model = {
#include "stg14/spinning_hammer/spinning_hammer_drum_model.inc"
};

static NJS_OBJECT spinning_hammer_drum = {
#include "stg14/spinning_hammer/spinning_hammer_drum.inc"
};

static NJS_MATERIAL spinning_hammer_bar_materials[1] = {
#include "stg14/spinning_hammer/spinning_hammer_bar_materials.inc"
};

static Sint16 spinning_hammer_bar_indices_0[21] = {
#include "stg14/spinning_hammer/spinning_hammer_bar_indices_0.inc"
};

static NJS_MESHSET spinning_hammer_bar_meshsets[1] = {
#include "stg14/spinning_hammer/spinning_hammer_bar_meshsets.inc"
};

static NJS_POINT3 spinning_hammer_bar_points[8] = {
#include "stg14/spinning_hammer/spinning_hammer_bar_points.inc"
};

static NJS_VECTOR spinning_hammer_bar_normals[8] = {
#include "stg14/spinning_hammer/spinning_hammer_bar_normals.inc"
};

static NJS_MODEL spinning_hammer_bar_model = {
#include "stg14/spinning_hammer/spinning_hammer_bar_model.inc"
};

static NJS_OBJECT spinning_hammer_bar = {
#include "stg14/spinning_hammer/spinning_hammer_bar.inc"
};

NJS_TEXLIST *spinning_hammer_texlists[2] = {
#include "stg14/spinning_hammer/spinning_hammer_texlists.inc"
};
static Uint32 glow_color_low = 0x00000000;
static Uint32 glow_color_high = 0xffffffff;
/* a matrix that flattens y to 0, which the shadow's draw starts from */
static NJS_MATRIX shadow_flatten = {
    1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F,
};

static void spinning_hammer_destroy(struct task *task);
static void spinning_hammer_step(struct task *task);
static void spinning_hammer_draw(struct task *task);
static void spinning_hammer_draw_shadow(struct task *task);

void spinning_hammer_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *drum;
    NJS_OBJECT *bar;
    Uint32 flags;
    Float along;
    Float across;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = spinning_hammer_draw;
    task->step = spinning_hammer_step;
    task->destructor = spinning_hammer_destroy;
    FORCES(task)[0].turn.x = FORCES(task)[1].turn.x = work->ang.x;
    DRUM_TURN(work) = 0;
    drum = world_pool_alloc();
    bar = world_pool_alloc();
    task->motion.p = drum;
    task->extra.p = bar;
    /* @bug memory: copies spinning_hammer_drum through null where
     * world_pool_alloc has none to lend, though spinning_hammer_destroy tests
     * task->motion for null. spinning_hammer_step writes through it untested:
     * the copy at 0x8c5007da, the test at 0x8c50098e and the write at
     * 0x8c500a6a */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no drum to lend,
     * the init destroys the hammer, and set.c makes it again on a later
     * pass */
    if (drum == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *drum = spinning_hammer_drum;
    drum->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    drum->scl[0] = LENGTH_SCALE(work);
    drum->scl[1] = THICKNESS_SCALE(work);
    drum->scl[2] = THICKNESS_SCALE(work);
    drum->ang[0] = 0;
    drum->ang[1] = work->ang.y;
    drum->ang[2] = 0;
    drum->pos[0] = work->pos.x;
    drum->pos[1] = work->pos.y + DRUM_HEIGHT(work);
    drum->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    /* @hack spelling: along and across, locals for the radius's two squares,
     * where the sum written in the call would do */
    along =
        LENGTH_SCALE(work) * LENGTH_SCALE(work) * (HALF_LENGTH * HALF_LENGTH);
    across = THICKNESS_SCALE(work) * THICKNESS_SCALE(work) *
             (DRUM_RADIUS * DRUM_RADIUS);
    RADIUS(task) = njSqrt(along + across);
    flags |= highway_hammer_size_class(RADIUS(task));
    world_add(flags, task, drum);
    /* @bug memory: copies spinning_hammer_bar through null where
     * world_pool_alloc has none left to lend, though spinning_hammer_destroy
     * tests task->extra for null: the copy at 0x8c5008b0 and the test at
     * 0x8c5009a6 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no bar to lend, the
     * init destroys the hammer, whose destructor takes the drum out of the
     * world. set.c makes it again on a later pass */
    if (bar == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *bar = spinning_hammer_bar;
    bar->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    bar->scl[0] = LENGTH_SCALE(work);
    bar->scl[1] = THICKNESS_SCALE(work);
    bar->scl[2] = THICKNESS_SCALE(work) * BAR_DEPTH_SCALE;
    bar->ang[0] = 0;
    bar->ang[1] = work->ang.y;
    bar->ang[2] = 0;
    bar->pos[0] = work->pos.x;
    bar->pos[1] = work->pos.y + BAR_HEIGHT(work);
    bar->pos[2] = work->pos.z;
    flags = highway_hammer_size_class(RADIUS(task)) | WORLD_SOLID;
    world_add(flags, task, bar);
}

static void spinning_hammer_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void spinning_hammer_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *drum;

    if (placement_try_leave(task)) {
        return;
    }
    DRUM_TURN(work) = TURN_RATE(work) * (Uint16)game_state.steps;
    drum = task->motion.p;
    drum->ang[0] = DRUM_TURN(work);
    if (player_near((NJS_POINT3 *)drum->pos, RADIUS(task) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    sound_play_at_for(SPIN_SOUND, work, SPIN_SOUND_PRIORITY, SPIN_SOUND_VOLUME,
                      SPIN_SOUND_DURATION, &work->pos);
}

static void spinning_hammer_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + DRUM_HEIGHT(work),
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, DRUM_TURN(work));
    njScale(NULL, LENGTH_SCALE(work), THICKNESS_SCALE(work),
            THICKNESS_SCALE(work));
    njSetTexture(&spinning_hammer_texlist);
    njCnkEasyDrawModel(&spinning_hammer_model);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
    model_draw_tinted(&spinning_hammer_glow_model,
                      color_blend(glow_color_low, glow_color_high,
                                  (_builtin_fabsf(njSin(game_state.steps
                                                        << GLOW_RATE_SHIFT)) *
                                   0.5F) +
                                      0.5F));
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
    /* @hack spelling: the test of scl.z written with !, where == 0.0F would
     * do */
    /* @hack spelling: task->work read again for scl.z, where work would do */
    if (!task->work->scl.z) {
        spinning_hammer_draw_shadow(task);
    }
}

/* draw the drum's model flattened onto the plane SHADOW_LIFT above pos */
static void spinning_hammer_draw_shadow(struct task *task)
{
    /* @hack spelling: flat, a pointer to the matrix, where the matrix by
     * its name would do */
    Float *flat;
    struct task_work *work = task->work;
    NJS_MATRIX matrix;

    flat = matrix;
    njPushMatrixEx();
    njSetMatrix(NULL, &shadow_flatten);
    njTranslate(NULL, work->pos.x, work->pos.y + DRUM_HEIGHT(work),
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, DRUM_TURN(work));
    njScale(NULL, LENGTH_SCALE(work), THICKNESS_SCALE(work),
            THICKNESS_SCALE(work));
    njGetMatrix(&matrix);
    njPopMatrixEx();
    flat[MATRIX_Y] = work->pos.y + SHADOW_LIFT;
    njPushMatrixEx();
    njMultiMatrix(NULL, &matrix);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    model_draw_tinted(&spinning_hammer_model, SHADOW_COLOR);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}
