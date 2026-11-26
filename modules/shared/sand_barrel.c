/* the sand barrel, BUFFER in Mission Street's object list. a strike breaks
 * the barrel into its top and four sides. it leaves a heap of sand that
 * rises, then fades */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "effect/burst.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/sand_barrel.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define BARREL_RADIUS 10.0F
#define BARREL_HALF_HEIGHT 14.5F
#define BARREL_CENTER 12.5F
#define BARREL_SHAPES 1

/* a barrel within its model's radius plus NEAR_MARGIN of the camera draws
 * see-through, in NEAR_COLOR. one behind the camera by more than that
 * radius draws nothing */
#define NEAR_MARGIN 15.0F
#define NEAR_COLOR 0x60ffffff

/* the pieces fly at PIECE_SPEED, plus up to PIECE_SPREAD at random, and
 * rise at PIECE_RISE. they fall to the highest floor at or below the point
 * FLOOR_LOOK_HEIGHT above pos */
#define BARREL_PARTS 5
#define PIECE_SPEED 0.5F
#define PIECE_SPREAD 0.8F
#define PIECE_RISE 0.7F
#define FLOOR_LOOK_HEIGHT 1.0F

#define BREAK_POINTS 20
#define BREAK_SOUND 12
#define BREAK_SOUND_PRIORITY 0
#define BREAK_SOUND_VOLUME 0
#define SAND_LEVEL 2

/* the heap's modes, in its work's mode. the work's timer counts each mode's
 * steps */
enum sand_heap_mode { MODE_RISE, MODE_FADE };

/* the rise's progress, from 0 to RISE_FRAMES, a Float in the task's unk_3c.
 * it climbs sand_heap_frame_step a step once the timer passes RISE_WAIT */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FRAME(task) (*(Float *)&(task)->unk_3c)
#define RISE_WAIT 2
#define RISE_FRAMES 7.0F
/* the draw eases FRAME into the motion's frame. an angle starts at
 * EASE_START degrees, and gains EASE_PER_FRAME for each frame of the rise, to
 * 180. the motion's frame is RISE_FRAMES times (1 - the angle's sine) */
#define EASE_START 90.0F
#define EASE_PER_FRAME 12.857143F
/* the heap fades out over FADE_STEPS steps, then ends. its tint is white,
 * at an alpha from ALPHA_MAX down to 0 */
#define FADE_STEPS 30
#define ALPHA_MAX 255
#define WHITE 0xffffff

/* each step, the heap sheds a puff of dust at a chance of GRAIN_CHANCE. the
 * puff lies up to GRAIN_RISE above pos, in a square GRAIN_SPREAD on a side
 * about pos. its size runs from GRAIN_SIZE to GRAIN_SIZE + GRAIN_SIZE_SPREAD */
#define GRAIN_CHANCE 0.3F
#define GRAIN_RISE 9.0F
#define GRAIN_SPREAD 24.0F
#define GRAIN_SIZE 3.0F
#define GRAIN_SIZE_SPREAD 5.0F
#define GRAIN_DEPTH_BIAS 15.0F

#include "shared/mission_street/sand_barrel_geometry.h"
#include "shared/sand_barrel/text.inc"

static NJS_TEXNAME sand_barrel_textures[2] = {
#include "shared/sand_barrel/sand_barrel_textures.inc"
};

static NJS_TEXLIST sand_barrel_texlist = {
#include "shared/sand_barrel/sand_barrel_texlist.inc"
};

static Sint16 sand_barrel_polygons[415] = {
#include "shared/sand_barrel/sand_barrel_polygons.inc"
};

static Sint32 sand_barrel_vertices[235] = {
#include "shared/sand_barrel/sand_barrel_vertices.inc"
};

static NJS_CNK_MODEL sand_barrel_mesh = {
#include "shared/sand_barrel/sand_barrel_mesh.inc"
};

static NJS_CNK_OBJECT sand_barrel_object = {
#include "shared/sand_barrel/sand_barrel_object.inc"
};

static NJS_TEXNAME sand_barrel_top_textures[1] = {
#include "shared/sand_barrel/sand_barrel_top_textures.inc"
};

static NJS_TEXLIST sand_barrel_top_texlist = {
#include "shared/sand_barrel/sand_barrel_top_texlist.inc"
};

static Sint16 sand_barrel_top_polygons[181] = {
#include "shared/sand_barrel/sand_barrel_top_polygons.inc"
};

static Sint32 sand_barrel_top_vertices[103] = {
#include "shared/sand_barrel/sand_barrel_top_vertices.inc"
};

static NJS_CNK_MODEL sand_barrel_top_model = {
#include "shared/sand_barrel/sand_barrel_top_model.inc"
};

static NJS_CNK_OBJECT sand_barrel_top_object = {
#include "shared/sand_barrel/sand_barrel_top_object.inc"
};

static NJS_TEXNAME sand_barrel_side_textures[1] = {
#include "shared/sand_barrel/sand_barrel_side_textures.inc"
};

static NJS_TEXLIST sand_barrel_side_texlist = {
#include "shared/sand_barrel/sand_barrel_side_texlist.inc"
};

static Sint16 sand_barrel_side_polygons[77] = {
#include "shared/sand_barrel/sand_barrel_side_polygons.inc"
};

static Sint32 sand_barrel_side_vertices[51] = {
#include "shared/sand_barrel/sand_barrel_side_vertices.inc"
};

static NJS_CNK_MODEL sand_barrel_side_model = {
#include "shared/sand_barrel/sand_barrel_side_model.inc"
};

static NJS_CNK_OBJECT sand_barrel_side_object = {
#include "shared/sand_barrel/sand_barrel_side_object.inc"
};

static NJS_TEXNAME sand_heap_textures[1] = {
#include "shared/sand_barrel/sand_heap_textures.inc"
};

static NJS_TEXLIST sand_heap_texlist = {
#include "shared/sand_barrel/sand_heap_texlist.inc"
};

static Sint16 sand_heap_polygons[293] = {
#include "shared/sand_barrel/sand_heap_polygons.inc"
};

static Sint32 sand_heap_vertices[135] = {
#include "shared/sand_barrel/sand_heap_vertices.inc"
};

static NJS_CNK_MODEL sand_heap_mesh = {
#include "shared/sand_barrel/sand_heap_mesh.inc"
};

static NJS_CNK_OBJECT sand_heap_object[1] = {
#include "shared/sand_barrel/sand_heap_object.inc"
};

static NJS_MKEY_F heap_position[2] = {
#include "shared/sand_barrel/heap_position.inc"
};

static NJS_MKEY_A heap_angle[2] = {
#include "shared/sand_barrel/heap_angle.inc"
};

static NJS_MDATA2 heap_transform[1] = {
#include "shared/sand_barrel/heap_transform.inc"
};

static NJS_MOTION sand_heap_motion = {
#include "shared/sand_barrel/sand_heap_motion.inc"
};

static struct sand_heap_action heap_action = {
#include "shared/sand_barrel/heap_action.inc"
};

static NJS_POINT3 heap_shape_start[33] = {
#include "shared/sand_barrel/heap_shape_start.inc"
};

static NJS_POINT3 heap_shape_end[33] = {
#include "shared/sand_barrel/heap_shape_end.inc"
};

static NJS_MKEY_P heap_shapes[2] = {
#include "shared/sand_barrel/heap_shapes.inc"
};

static NJS_MDATA2 heap_shape_track[1] = {
#include "shared/sand_barrel/heap_shape_track.inc"
};

static NJS_MOTION sand_heap_shape_motion = {
#include "shared/sand_barrel/sand_heap_shape_motion.inc"
};

static void sand_heap_destroy(struct task *task);
static void sand_heap_step(struct task *task);
static void sand_heap_draw(struct task *task);
static void sand_heap_init(struct task *task);
static void sand_barrel_destroy(struct task *task);
static void sand_barrel_step(struct task *task);
static void sand_barrel_draw(struct task *task);

NJS_TEXLIST *sand_barrel_texlists[7] = {
#include "shared/sand_barrel/sand_barrel_texlists.inc"
};
/* the pieces: the top, then the four sides, each a quarter turn from the
 * last */
/* clang-format off */
static struct debris_part sand_barrel_parts[BARREL_PARTS] = {
#include "shared/sand_barrel/sand_barrel_parts.inc"
};
static struct collision_shape sand_barrel_shapes[BARREL_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(1) | SHAPE_HURT, SHAPE_HURTABLE,
     {0.0F, BARREL_CENTER, 0.0F}, BARREL_RADIUS, BARREL_HALF_HEIGHT, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */
static Float sand_heap_frame_step = 0.3F;

static void sand_heap_destroy(struct task *task)
{
}

static void sand_heap_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (rand() * RAND_SCALE < GRAIN_CHANCE) {
        pos = work->pos;
        pos.y += rand() * RAND_SCALE * GRAIN_RISE;
        pos.x += ((rand() * RAND_SCALE) - 0.5F) * GRAIN_SPREAD;
        pos.z += ((rand() * RAND_SCALE) - 0.5F) * GRAIN_SPREAD;
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        dust_steady_create(
            &pos, &spd, (rand() * RAND_SCALE * GRAIN_SIZE_SPREAD) + GRAIN_SIZE,
            GRAIN_DEPTH_BIAS);
    }
    if (work->mode == MODE_RISE) {
        work->timer++;
        if (work->timer > RISE_WAIT) {
            FRAME(task) += sand_heap_frame_step;
            if (FRAME(task) >= RISE_FRAMES) {
                FRAME(task) = RISE_FRAMES;
                work->mode = MODE_FADE;
                work->timer = 0;
            }
        }
    } else {
        work->timer++;
        if (work->timer > FADE_STEPS) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
    }
}

static void sand_heap_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float fade;
    int alpha;
    Uint32 color;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&sand_heap_texlist);
    if (work->mode != MODE_RISE && work->timer != 0) {
        fade = 1.0F - (work->timer * (1.0F / FADE_STEPS));
        alpha = fade * ALPHA_MAX;
        if (alpha < 0) {
            alpha = 0;
        }
        if (alpha > ALPHA_MAX) {
            alpha = ALPHA_MAX;
        }
        color = (alpha << ALPHA_SHIFT) + WHITE;
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
        model_draw_tinted(sand_heap_object->model, color);
        draw_attr_restore();
        draw_control_restore();
    } else {
        njCnkEasyDrawShapeMotion(
            sand_heap_object, &sand_heap_motion, &sand_heap_shape_motion,
            (1.0F -
             njSin((Angle)((EASE_START + (FRAME(task) * EASE_PER_FRAME)) *
                           ANGLE_PER_DEGREE))) *
                RISE_FRAMES);
    }
    njPopMatrixEx();
}

static void sand_heap_init(struct task *task)
{
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = sand_heap_draw;
    task->step = sand_heap_step;
    task->destructor = sand_heap_destroy;
    FRAME(task) = 0.0F;
}

void sand_barrel_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, which leaves its string in
     * the object, ahead of the sand task's name */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw set before the step, where struct task's
         * order would set the step first */
        task->draw = sand_barrel_draw;
        task->step = sand_barrel_step;
        task->destructor = sand_barrel_destroy;
        collision_init(task, sand_barrel_shapes, BARREL_SHAPES,
                       COLLISION_OBJECT);
    }
}

static void sand_barrel_destroy(struct task *task)
{
}

static void sand_barrel_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *sand;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR spd;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
    if (collision_next_attack(task) != NULL || (work->flags & WORK_HURT)) {
        pos = work->pos;
        spd.x = 0.0F;
        spd.y = PIECE_RISE;
        spd.z = 0.0F;
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        floor =
            surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
        debris_break_flags(&pos, work->ang.y, sand_barrel_parts, BARREL_PARTS,
                           PIECE_SPEED, PIECE_SPREAD, floor, &spd,
                           DEBRIS_FADE_COLORS | DEBRIS_TRAIL);
        score_add(BREAK_POINTS);
        sound_play(BREAK_SOUND, NULL, BREAK_SOUND_PRIORITY, BREAK_SOUND_VOLUME);
        sand = task_create(TASK_WORK, SAND_LEVEL, sand_heap_init, "ObjectSand");
        if (sand != NULL) {
            sand->work->pos = work->pos;
            sand->work->ang = work->ang;
        }
    } else {
        collision_register(task);
    }
}

static void sand_barrel_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 view;
    Float depth;
    Float radius;
    Bool near;

    njCalcPoint(NULL, &work->pos, &view);
    /* @hack spelling: depth, a local for view.z, where view.z read in place
     * would do */
    depth = view.z;
    radius = sand_barrel_object.model->r + NEAR_MARGIN;
    if (depth < -radius) {
        return;
    }
    /* @hack spelling: near set by an if and an else, where the comparison's
     * result would do */
    if (njScalor2(&view) < radius * radius) {
        near = TRUE;
    } else {
        near = FALSE;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&sand_barrel_texlist);
    if (near) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        model_draw_tinted(sand_barrel_object.model, NEAR_COLOR);
        draw_attr_restore();
        draw_control_restore();
    } else {
        njCnkEasyDrawModel(sand_barrel_object.model);
    }
    njPopMatrix(1);
}
