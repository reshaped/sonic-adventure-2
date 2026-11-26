/* the big splash: a column of water that leaps up and falls outward, with its
 * drops and ripples */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "effect/particle.h"
#include "player/action.h"
#include "shared/big_splash.h"
#include "shared/water_ripple.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "shared/big_splash/text.inc"

/* big_splash_texlist holds the column's COLUMN_TEXTURES textures, pasha01
 * and pasha03. the DROP_FRAMES frames a drop ages through follow them,
 * water01 to water08 */
#define COLUMN_TEXTURES 2
#define DROP_FRAMES 8

/* a drop starts turned up to DROP_TURN degrees either way. it turns up to
 * DROP_SPIN degrees a step either way */
#define DROP_TURN 40.0F
#define DROP_SPIN 0.25F

#define COLUMN_TASK_LEVEL 3
/* the column's model draws scaled by WIDTH across and HEIGHT up, at frame FRAME
 * of column_shape. HEIGHT is a Float kept in the task's word unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define HEIGHT(task) (*(Float *)&(task)->unk_3c)
#define WIDTH(work) ((work)->scl.x)
#define OPACITY(work) ((work)->scl.y)
#define FRAME(work) ((work)->scl.z)
/* the column draws pasha01 until TEXTURE_STEPS steps past frame TIMER_FRAME,
 * and pasha03 from then on. the work's timer counts those steps, and
 * column_step keeps it below 2 * TEXTURE_STEPS */
#define TEXTURE_SHIFT 2
#define TEXTURE_STEPS (1 << TEXTURE_SHIFT)
#define TEXTURE(work) ((work)->timer >> TEXTURE_SHIFT)
#define TIMER_FRAME 1.0F
/* the column fades past frame FADE_FRAME, and faster past FAST_FADE_FRAME.
 * the column ends at an opacity of OPACITY_END or less */
#define FADE_FRAME 4.0F
#define FAST_FADE_FRAME 6.0F
#define OPACITY_END 0.01F
/* the column draws its model again, as a base around its foot. the base is
 * BASE_WIDTH times as wide as the column, and BASE_HEIGHT times as high */
#define BASE_WIDTH 1.3F
#define BASE_HEIGHT 0.3F

/* a splash throws DROPS_PER_SIZE drops for each unit of its size. a drop leaves
 * at DROP_SPEED times the square root of the splash's speed. it leaves at
 * DROP_PITCH from straight up, plus a random part below DROP_PITCH_RANGE, both
 * Angles. its size is DROP_SCALE plus a random part below DROP_SCALE_RANGE */
#define DROPS_PER_SIZE 16.0F
#define DROP_SPEED 1.7F
#define DROP_PITCH 0x2800
#define DROP_PITCH_RANGE 0xc00
#define DROP_SCALE 0.9F
#define DROP_SCALE_RANGE 0.3F

/* the first ripple starts at RIPPLE_1_SIZE times the splash's size. it grows a
 * step by RIPPLE_GROWTH plus RIPPLE_1_GROWTH_PER_SPEED times the splash's
 * speed. a random part below RIPPLE_1_JITTER, drawn as it starts, adds to that.
 * the second and third ripples take the RIPPLE_2_ and RIPPLE_3_ values alike. a
 * ripple fades once its size passes RIPPLE_LIMIT */
#define RIPPLE_GROWTH 0.03F
#define RIPPLE_LIMIT 3.0F
#define RIPPLE_1_SIZE 1.1F
#define RIPPLE_1_GROWTH_PER_SPEED 0.03F
#define RIPPLE_1_JITTER 0.02F
#define RIPPLE_2_SIZE 1.2F
#define RIPPLE_2_GROWTH_PER_SPEED 0.035F
#define RIPPLE_2_JITTER 0.03F
#define RIPPLE_3_SIZE 1.3F
#define RIPPLE_3_GROWTH_PER_SPEED 0.04F
#define RIPPLE_3_JITTER 0.02F

static int drop_step(struct particle_group *group, struct particle *particle);
static void column_step(struct task *task);
static void column_draw(struct task *task);
static void column_destroy(struct task *task);

static NJS_TEXNAME big_splash_texnames[COLUMN_TEXTURES + DROP_FRAMES] = {
#include "shared/big_splash/big_splash_texnames.inc"
};
static NJS_TEXLIST big_splash_texlist = {
#include "shared/big_splash/big_splash_texlist.inc"
};
/* drop_texlist shares big_splash_texlist's entries, so one load fills both */
static NJS_TEXLIST drop_texlist = {
#include "shared/big_splash/drop_texlist.inc"
};
/* a texture list of pasha01 alone */
static NJS_TEXNAME dat_big_splash_0_texname[1] = {
#include "shared/big_splash/dat_big_splash_0_texname.inc"
};
static NJS_TEXLIST dat_big_splash_0_texlist = {
#include "shared/big_splash/dat_big_splash_0_texlist.inc"
};

/* the column's model: COLUMN_POINTS points in a ring about the y axis */
#define COLUMN_POINTS 24
struct column_polygon_list {
    Sint16 material[6];
    Sint16 texture[2];
    Sint16 strip_head[3];
    Sint16 strip[79];
    Sint16 end;
};
static struct column_polygon_list column_polygons = {
#include "shared/big_splash/column_polygons.inc"
};
static Sint32 column_vertices[147] = {
#include "shared/big_splash/column_vertices.inc"
};
static NJS_CNK_MODEL column_model = {
#include "shared/big_splash/column_model.inc"
};
static NJS_CNK_OBJECT column_object = {
#include "shared/big_splash/column_object.inc"
};
/* column_shape moves the model's points through COLUMN_SHAPES keyed shapes,
 * over 11 frames. the keys lie at frames 0 to 6 and at frame 10. the
 * ring's outer points leap up into a column by frame 1. they then fall outward
 * into a wide flat ring. njCnkSimpleDrawShapeMotion takes a motion beside the
 * shapes, and column_motion, as long as column_shape, moves nothing */
#define COLUMN_SHAPES 8
static NJS_POINT3 column_shape_points[COLUMN_SHAPES][COLUMN_POINTS] = {
#include "shared/big_splash/column_shape_points.inc"
};
static NJS_MKEY_P column_shape_keys[COLUMN_SHAPES] = {
#include "shared/big_splash/column_shape_keys.inc"
};
static NJS_MDATA2 column_shape_data[1] = {
#include "shared/big_splash/column_shape_data.inc"
};
static NJS_MOTION column_shape = {
#include "shared/big_splash/column_shape.inc"
};
static NJS_MDATA2 column_motion_data[1] = {
#include "shared/big_splash/column_motion_data.inc"
};
static NJS_MOTION column_motion = {
#include "shared/big_splash/column_motion.inc"
};

/* a drop's scale starts from DROP_SCALE up to DROP_SCALE + DROP_SCALE_RANGE.
 * the group's scale_step takes it past 0 after 23 to 30 of the drop's 51
 * steps. the scale then grows negative, which draws the quad turned half a
 * turn */
static struct particle_group drop_group = {
#include "shared/big_splash/drop_group.inc"
};

static Float column_height_per_speed = 0.8F;
static Float column_frame_step = 0.14F;
static Float column_first_frame = 0.0F;
static Float column_first_opacity = 1.0F;
static Float column_fast_fade = 0.9F;
static Float column_fade_step = -0.04F;
static Float column_first_texture_opacity = 0.7F;

/* particle_move, but a frame that reaches the group's frames stays there.
 * particle_move sets it back to frames - 1 */
static int drop_step(struct particle_group *group, struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        return 0;
    }
    return 1;
}

/* make a drop at pos, moving at spd, of size scale, and return its particle. it
 * returns null where the heap can't give one */
static struct particle *drop_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                    Float scale)
{
    struct particle *drop;

    drop = particle_create(&drop_group);
    if (drop != NULL) {
        drop->pos = *pos;
        drop->spd = *spd;
        drop->scale = scale;
        drop->ang = RANDOM_ANGLE(DROP_TURN);
        drop->ang_spd = RANDOM_ANGLE(DROP_SPIN);
        /* @hack redundant: the frame set to 0, which particle_create gave it
         * already from the group's first_frame */
        drop->frame = 0.0F;
    }
    return drop;
}

void big_splash_load_texture(void)
{
    switch (stage_current_id()) {
    case STAGE_BOSS_LAST1:
    case STAGE_RELAY_SONIC:
    case STAGE_RELAY_KNUCKLES:
    case STAGE_RELAY_ROUGE:
        texture_load_pvm(SPLASH_ARCHIVE_FINAL, &big_splash_texlist);
        break;
    case STAGE_DRY_LAGOON:
        texture_load_pvm(SPLASH_ARCHIVE_LAGOON, &big_splash_texlist);
        break;
    default:
        texture_load_pvm(SPLASH_ARCHIVE, &big_splash_texlist);
        break;
    }
}

void big_splash_release_texture(void)
{
    njReleaseTexture(&big_splash_texlist);
}

void big_splash_create(NJS_POINT3 *pos, struct angles *ang, Float size,
                       Float speed)
{
    struct task *task;
    struct task_work *work;
    /* @hack spelling: height, a pointer to the column's height, where
     * HEIGHT(task) assigned in place would do */
    Float *height;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (size > 0.0F &&
        (task = task_create(TASK_WORK, COLUMN_TASK_LEVEL, column_step,
                            SPLASH_TASK_NAME)) != NULL) {
        work = task->work;
        work->pos = *pos;
        if (ang != NULL) {
            work->ang = *ang;
        }
        WIDTH(work) = size;
        height = &HEIGHT(task);
        *height = column_height_per_speed * speed;
        OPACITY(work) = column_first_opacity;
        FRAME(work) = column_first_frame;
        task->draw = column_draw;
        task->destructor = column_destroy;
    }
    if (speed > 0.0F) {
        NJS_VECTOR up = {0.0F, DROP_SPEED, 0.0F};
        int drops;
        int gap;
        int heading;

        drops = (int)(size * DROPS_PER_SIZE);
        up.y *= njSqrt(speed);
        heading = (int)RANDOM_TURN();
        /* @bug arithmetic: divides by 0 where size * DROPS_PER_SIZE cuts to
         * 0, in the call to _divls at 0x280 of the object's code. the main
         * executable passes a size of 0 from 0x8c0504da. the division then
         * sets errno and returns 0. the loop below throws no drop, so no code
         * reads the gap */
        gap = FULL_TURN / drops;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njPushMatrixEx();
        for (; drops > 0; drops--) {
            /* @hack type: spd an array of one, where a single NJS_VECTOR
             * would do */
            NJS_VECTOR spd[1];

            njRotateY(NULL,
                      (Angle)(heading +
                              (gap * (0.5F - ((Float)rand() * RAND_SCALE)))));
            njRotateX(NULL, (Angle)(((Float)rand() *
                                     (DROP_PITCH_RANGE * RAND_SCALE)) +
                                    DROP_PITCH));
            njCalcVector(NULL, &up, spd);
            drop_create(pos, spd,
                        ((Float)rand() * (DROP_SCALE_RANGE * RAND_SCALE)) +
                            DROP_SCALE);
            heading += gap;
            matrix_load_parent();
        }
        njPopMatrix(2);
        if (player_hooks.ripple != NULL) {
            /* @hack spelling: growth, a local for each ripple's growth, where
             * the sum stored in ripple.x would do */
            Float growth;
            NJS_VECTOR ripple;

            growth = speed * RIPPLE_1_GROWTH_PER_SPEED;
            ripple.x = growth +
                       ((Float)rand() * (RIPPLE_1_JITTER * RAND_SCALE)) +
                       RIPPLE_GROWTH;
            ripple.y = 0.0F;
            ripple.z = RIPPLE_LIMIT;
            ripple_create(pos, &ripple, size * RIPPLE_1_SIZE);
            growth = (speed * RIPPLE_2_GROWTH_PER_SPEED) +
                     ((Float)rand() * (RIPPLE_2_JITTER * RAND_SCALE));
            /* @hack spelling: RIPPLE_GROWTH added in a statement of its own,
             * where the sum assigned to growth would take it */
            growth += RIPPLE_GROWTH;
            ripple.x = growth;
            ripple_create(pos, &ripple, size * RIPPLE_2_SIZE);
            growth = (speed * RIPPLE_3_GROWTH_PER_SPEED) +
                     ((Float)rand() * (RIPPLE_3_JITTER * RAND_SCALE)) +
                     RIPPLE_GROWTH;
            ripple.x = growth;
            ripple_create(pos, &ripple, size * RIPPLE_3_SIZE);
        }
    }
}

static void column_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: motion, a pointer to column_motion, where
     * column_motion.nbFrame read in place would do */
    NJS_MOTION *motion;

    FRAME(work) += column_frame_step;
    /* @hack spelling: motion set after the frame advances, where the
     * initializer of its declaration would do */
    motion = &column_motion;
    if (FRAME(work) >= (Float)motion->nbFrame - 1.0F) {
        FRAME(work) = (Float)motion->nbFrame - 1.0F;
        /* @hack spelling: the goto to the destroy below, where an else about
         * the rest of the step would do */
        goto destroy;
    }
    if (FRAME(work) >= TIMER_FRAME) {
        work->timer++;
        if (work->timer >= 2 * TEXTURE_STEPS) {
            work->timer = TEXTURE_STEPS;
        }
    }
    if (FRAME(work) > FAST_FADE_FRAME) {
        OPACITY(work) *= OPACITY(work) * column_fast_fade;
    } else if (FRAME(work) > FADE_FRAME) {
        OPACITY(work) += column_fade_step;
    }
    if (OPACITY(work) > OPACITY_END) {
        return;
    }
destroy:
    task_destroy(task);
}

/* the column fades by the constant material's alpha, which the draw sets to
 * the opacity less 1. under NJD_CONTROL_3D_OFFSET_MATERIAL, that alpha adds to
 * the column material's alpha of 1. the fade shows only while that bit is set
 * and NJD_CONTROL_3D_CONSTANT_MATERIAL is clear. the draw leaves both bits as
 * it finds them */
static void column_draw(struct task *task)
{
    /* @hack spelling: column, a copy of task that the reads go through,
     * where task itself would do */
    struct task *column = task;
    struct task_work *work = column->work;
    /* @hack spelling: height, a local for the column's height, where
     * HEIGHT(column) passed to njScale would do */
    Float height = HEIGHT(column);
    /* @hack spelling: texture, a pointer to the column's texture index,
     * where column_polygons.texture[1] written in place would do */
    Sint16 *texture;
    /* @hack spelling: id, the texture chunk's halfword read before the
     * store, where *texture read in place would do */
    Sint16 id;

    njSetTexture(&big_splash_texlist);
    if (TEXTURE(work) == 0) {
        constant_material_set(column_first_texture_opacity - 1.0F, 0.0F, 0.0F,
                              0.0F);
    } else {
        constant_material_set(OPACITY(work) - 1.0F, 0.0F, 0.0F, 0.0F);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, WIDTH(work), height, WIDTH(work));
    texture = &column_polygons.texture[1];
    id = *texture;
    *texture = (id & NJD_FFM_MASK) | TEXTURE(work);
    njCnkSimpleDrawShapeMotion(&column_object, &column_motion, &column_shape,
                               FRAME(work));
    njScale(NULL, BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH);
    njCnkSimpleDrawShapeMotion(&column_object, &column_motion, &column_shape,
                               FRAME(work));
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

static void column_destroy(struct task *task)
{
}
