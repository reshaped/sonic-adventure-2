/* chaos drives, as chaos_drive.h declares them */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/mesh.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/trail.h"
#include "hud/item_stock.h"
#include "hud/hud.h"
#include "object/chaos_drive.h"
#include "object/set.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the drive's work. GROUND is the ground's height along the up axis of the
 * collision checks. SPEED is the drive's speed along that axis, and SCALE
 * the scale it draws at */
#define KIND(work) ((work)->tick)
#define GROUND(work) ((work)->scl.x)
#define SPEED(work) ((work)->scl.y)
#define SCALE(work) ((work)->scl.z)

/* the work's modes */
#define DRIVE_FREE 0
#define DRIVE_TAKEN 1 /* a player has taken it, and it shrinks away */

/* a trail, as the stage's program allocates it through stage_hooks. only
 * the trail calls of stage_hooks read it */
struct trail;

/* the drive's record, in its task's extra: a trail for each camera of a
 * split screen */
struct drive_trails {
    struct trail *by_camera[2];
};
#define TRAILS(task) ((struct drive_trails *)(task)->extra.p)
#define TRAIL_POINTS 8

/* how far above the drive's pos along y its ground probe starts, under every
 * up axis */
#define PROBE_LIFT 5.0F
/* the radius find_ground gathers the world's objects in, and its result
 * where it finds no floor */
#define PROBE_RADIUS 10.0F
#define NO_GROUND (-1.0e8F)
#define DRIVE_LEVEL 2
/* chaos_drive_create scales its random number to CHAOS_DRIVE_KINDS less
 * KIND_MARGIN, so the kind stays below CHAOS_DRIVE_KINDS */
#define KIND_MARGIN 0.0001F
#define SHRINK 0.06F /* what each step takes from a taken drive's scale */
#define GAUGE 0.1F /* what a drive adds to its player's gauge */
#define POINTS 10
#define GLOW_TOWARD_CAMERA 5.0F
#define MODEL_SCALE 1.5F
/* the shadow's scale along x, y and z, under the drive's own scale */
#define SHADOW_SCALE_X 1.6F
#define SHADOW_SCALE_Y 0.5F
#define SHADOW_SCALE_Z 2.6F
/* the trail draws only where the drive lies nearer the camera than this,
 * along the view's axis */
#define TRAIL_REACH 500.0F
/* the glow turns by GLOW_TURN times the angle at the camera between the
 * view's axis and the drive */
#define GLOW_TURN 4
/* the model turns by ang.y shifted right by MODEL_TURN_SHIFT, beyond the
 * drive's own turn by ang.y */
#define MODEL_TURN_SHIFT 1
#define CHAOSDRIVE_TEXTURES 8

static Float find_ground(NJS_POINT3 *pos);
static void chaos_drive_step(struct task *task);
static void chaos_drive_draw(struct task *task);
static void chaos_drive_destroy(struct task *task);
static void chaos_drive_init(struct task *task);

/* the whole of CHAOSDRIVE_TEX's texture 5, as a cell 14 pixels square about
 * its center */
static const NJS_TEXANIM glow_cell = {
#include "object/chaos_drive/glow_cell.inc"
};
/* clang-format off */
static const struct collision_shape chaos_drive_shape = {
    18, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS),
    SHAPE_GUARD_LEVEL(3), SHAPE_UNTURNED, {0.0F, 0.0F, 0.0F}, 4.0F, 0.0F, 0.0F,
    0, {0, 0, 0},
};
/* clang-format on */

/* CHAOSDRIVE_TEX's textures, unnamed: texture_load_set fills each by its
 * place */
static NJS_TEXNAME chaosdrive_tex_texname[CHAOSDRIVE_TEXTURES] = {
    {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0},
    {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0},
};
NJS_TEXLIST chaosdrive_tex_texlist = {chaosdrive_tex_texname,
                                      CHAOSDRIVE_TEXTURES};
#include "object/chaos_drive/text.inc"

static NJS_TEXNAME trail_texname[1] = {
#include "object/chaos_drive/trail_texname.inc"
};
static NJS_TEXLIST trail_texlist = {
#include "object/chaos_drive/trail_texlist.inc"
};
/* the trail's texture lists, ended by null */
static NJS_TEXLIST *chaos_drive_texlists[2] = {
#include "object/chaos_drive/chaos_drive_texlists.inc"
};
/* clang-format off */
/* the trail's colors here are the yellow drive's, and the draw sets each
 * kind's own */
struct trail_style chaos_drive_trail_style = {
#include "object/chaos_drive/chaos_drive_trail_style.inc"
};
/* the draw sets sx, sy and ang */
NJS_SPRITE chaos_drive_glow = {
    {0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, &chaosdrive_tex_texlist,
    (NJS_TEXANIM *)&glow_cell,
};
/* clang-format on */
NJS_CNK_OBJECT *chaos_drive_objects[CHAOS_DRIVE_KINDS] = {
#include "object/chaos_drive/chaos_drive_objects.inc"
};
/* clang-format off */
static struct drive_params params = {
    6.0F, 0.05F, 0.85F, 0x280, 0x1400, 0x2c0, 0.16F, 1.0F, 0.3F, 0.6F,
};
/* clang-format on */
/* below this height above the ground, a falling drive gains no more speed */
static Float hover_height = 7.5F;

/* the ground's height beneath pos, along the up axis of the collision
 * checks. under COLLISION_UP_Y it is the height of the highest floor below
 * pos among the world's objects within PROBE_RADIUS of it, or NO_GROUND.
 * under the other up axes it is the nearest surface from pos on the axis's
 * ground side */
static Float find_ground(NJS_POINT3 *pos)
{
    Float ground;
    int i;

    if (collision_get_up_axis() == COLLISION_UP_Y) {
        struct height_probe probe;

        ground = NO_GROUND;
        world_list_near(pos->x, pos->y, pos->z, PROBE_RADIUS);
        /* @hack spelling: probe.pos set a part at a time, where
         * probe.pos = *pos would do */
        probe.pos.x = pos->x;
        probe.pos.y = pos->y;
        probe.pos.z = pos->z;
        for (i = 0; i < world_count; i++) {
            if (polygon_find_heights(&probe, world_objects[i].object, 0) &&
                probe.below.found) {
                if (probe.below.pos > ground) {
                    ground = probe.below.pos;
                }
            }
        }
        return ground;
    } else {
        struct surface_probe probe;

        probe.pos = *pos;
        surface_probe_sides(&probe);
        switch (collision_get_up_axis()) {
        case COLLISION_UP_NEG_X:
            return probe.sides[SURFACE_POS_X].pos;
        case COLLISION_UP_X:
            return probe.sides[SURFACE_NEG_X].pos;
        case COLLISION_UP_NEG_Y:
            return probe.sides[SURFACE_POS_Y].pos;
        case COLLISION_UP_Y:
            return probe.sides[SURFACE_NEG_Y].pos;
        case COLLISION_UP_NEG_Z:
            return probe.sides[SURFACE_POS_Z].pos;
        case COLLISION_UP_Z:
            return probe.sides[SURFACE_NEG_Z].pos;
        }
    }
    /* @hack implicit: no return value past the switch, where a return would
     * do while the up axis is one of the six */
#ifdef NONMATCHING
    /* a result for GCC, which no path reaches */
    return 0.0F;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void chaos_drive_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;
    int slot;

    if (placement_try_leave(task)) {
        return;
    }
    switch (collision_get_up_axis()) {
    case COLLISION_UP_NEG_X:
        work->pos.x -= SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) - hover_height) > work->pos.x) {
            SPEED(work) -= params.gravity;
        }
        break;
    case COLLISION_UP_X:
        work->pos.x += SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) + hover_height) < work->pos.x) {
            SPEED(work) -= params.gravity;
        }
        break;
    case COLLISION_UP_NEG_Y:
        work->pos.y -= SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) - hover_height) > work->pos.y) {
            SPEED(work) -= params.gravity;
        }
        break;
    case COLLISION_UP_Y:
        work->pos.y += SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) + hover_height) < work->pos.y) {
            SPEED(work) -= params.gravity;
        }
        break;
    case COLLISION_UP_NEG_Z:
        work->pos.z -= SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) - hover_height) > work->pos.z) {
            SPEED(work) -= params.gravity;
        }
        break;
    case COLLISION_UP_Z:
        work->pos.z += SPEED(work);
        if (SPEED(work) > 0.0F || (GROUND(work) + hover_height) < work->pos.z) {
            SPEED(work) -= params.gravity;
        }
        break;
    }
    SPEED(work) *= params.drag;
    work->ang.y += params.spin_y;
    work->ang.z += params.spin_z;
    switch (work->mode) {
    case DRIVE_FREE:
        player = collision_next_player(task);
        if (player != NULL) {
            slot = player_slot_of(player);
            item_stock_push(slot, ITEM_CHAOS_DRIVE + KIND(work));
            player_add_gauge(slot, GAUGE);
            score_add(POINTS);
            /* @hack spelling: the mode stepped by ++, where = DRIVE_TAKEN
             * would do */
            work->mode++;
        }
        /* a rising drive can't be touched */
        if (SPEED(work) <= 0.0F) {
            collision_register(task);
        }
        break;
    case DRIVE_TAKEN:
        if (SCALE(work) > 0.0F) {
            SCALE(work) -= SHRINK;
        } else {
            task_kill(task);
        }
        break;
    }
}

#ifdef NONMATCHING
/* camera_current_index itself: this side's objects don't define
 * dat_8c25b8a8, which only symbols.txt places */
#define VIEW_INDEX camera_current_index
#else
/* @hack type: camera_current_index read through dat_8c25b8a8, a volatile
 * second name that symbols.txt places at its address, where
 * camera_current_index would do */
extern volatile int dat_8c25b8a8;
#define VIEW_INDEX dat_8c25b8a8
#endif

static void chaos_drive_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct camera_place *view;
    NJS_VECTOR dir;
    Float glow;
    Float alpha;
    /* @hack spelling: sprite, view_ang and view_pos, locals for
     * &chaos_drive_glow, &view->ang and &view->pos, where those would do */
    NJS_SPRITE *sprite;
    struct angles *view_ang;
    NJS_POINT3 *view_pos;
    /* @hack spelling: push and color, locals for njPushMatrixEx and
     * constant_material_set, where the calls by their names would do */
    Bool (*push)(void);
    void (*color)(Float alpha, Float red, Float green, Float blue);

    glow = params.glow_base +
           (params.glow_amplitude * (njCos(work->ang.z) - 1.0F));
    alpha = params.alpha_base +
            (params.alpha_amplitude * (njCos(work->ang.z) - 1.0F));
    alpha *= SCALE(work);
    njFogDisable();
    njSetTexture(&chaosdrive_tex_texlist);
    push = njPushMatrixEx;
    push();
    njTranslateEx(&work->pos);
    push();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    color = constant_material_set;
    switch (KIND(work)) {
    case CHAOS_DRIVE_YELLOW:
        color(alpha, 1.0F, 1.0F, 0.0F);
        break;
    case CHAOS_DRIVE_GREEN:
        color(alpha, 0.0F, 1.0F, 0.0F);
        break;
    case CHAOS_DRIVE_RED:
        color(alpha, 1.0F, 0.0F, 0.0F);
        break;
    case CHAOS_DRIVE_PURPLE:
        color(alpha, 0.8F, 0.0F, 1.0F);
        break;
    }
    view = camera_views[VIEW_INDEX];
    /* @hack spelling: dir copied from view->pos, then work->pos taken from
     * it, where one subtraction for each part would do */
    dir.x = view->pos.x;
    dir.y = view->pos.y;
    dir.z = view->pos.z;
    dir.x -= work->pos.x;
    dir.y -= work->pos.y;
    dir.z -= work->pos.z;
    njUnitVector(&dir);
    dir.x *= GLOW_TOWARD_CAMERA;
    dir.y *= GLOW_TOWARD_CAMERA;
    dir.z *= GLOW_TOWARD_CAMERA;
    njTranslateEx(&dir);
    sprite = &chaos_drive_glow;
    sprite->sx = glow;
    sprite->sy = glow;
    view = camera_views[VIEW_INDEX];
    view_pos = &view->pos;
    view_ang = &view->ang;
    /* @hack spelling: dir copied from work->pos, then view_pos taken from
     * it, where one subtraction for each part would do */
    dir.x = work->pos.x;
    dir.y = work->pos.y;
    dir.z = work->pos.z;
    dir.x -= view_pos->x;
    dir.y -= view_pos->y;
    dir.z -= view_pos->z;
    njUnitVector(&dir);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateX(NULL, -view_ang->x);
    njRotateY(NULL, -view_ang->y);
    njCalcVector(NULL, &dir, &dir);
    njPopMatrixEx();
    sprite->ang = (Angle)(acosf(dir.z) * ANGLE_PER_RADIAN) * GLOW_TURN;
    njDrawSprite3D(sprite, 0,
                   NJD_SPRITE_ANGLE | NJD_SPRITE_COLOR | NJD_SPRITE_SCALE |
                       NJD_SPRITE_ALPHA);
    njPopMatrixEx();
    color(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* the model stands up along the up axis */
    switch (collision_get_up_axis()) {
    case COLLISION_UP_NEG_X:
    case COLLISION_UP_X:
        njRotateZ(NULL, QUARTER_TURN);
        break;
    case COLLISION_UP_NEG_Y:
    case COLLISION_UP_Y:
        break;
    case COLLISION_UP_NEG_Z:
    case COLLISION_UP_Z:
        njRotateX(NULL, QUARTER_TURN);
        break;
    }
    njRotateY(NULL, work->ang.y);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    push();
    njRotateX(NULL, params.tilt);
    njRotateY(NULL, work->ang.y >> MODEL_TURN_SHIFT);
    njScale(NULL, MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
    njCnkSimpleDrawObject(chaos_drive_objects[KIND(work)]);
    njPopMatrixEx();
    njScale(NULL, SHADOW_SCALE_X, SHADOW_SCALE_Y, SHADOW_SCALE_Z);
    njCnkModDrawModel(shadow_model);
    njPopMatrixEx();
    njCalcPoint(NULL, &work->pos, &dir);
#ifdef NONMATCHING
    /* this side draws no trail where the drive has none, or none for this
     * camera */
    if (TRAILS(task) != NULL && TRAILS(task)->by_camera[VIEW_INDEX] != NULL &&
        TRAIL_REACH > _builtin_fabsf(dir.z)) {
#else
    if (TRAIL_REACH > _builtin_fabsf(dir.z)) {
#endif
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        switch (KIND(work)) {
        case CHAOS_DRIVE_YELLOW:
            chaos_drive_trail_style.new_color = 0x30ffff00;
            chaos_drive_trail_style.old_color = 0x02ffff00;
            break;
        case CHAOS_DRIVE_GREEN:
            chaos_drive_trail_style.new_color = 0x3033ff33;
            chaos_drive_trail_style.old_color = 0x0233ff33;
            break;
        case CHAOS_DRIVE_RED:
            chaos_drive_trail_style.new_color = 0x30ff3333;
            chaos_drive_trail_style.old_color = 0x02ff3333;
            break;
        case CHAOS_DRIVE_PURPLE:
            chaos_drive_trail_style.new_color = 0x30c02222;
            chaos_drive_trail_style.old_color = 0x02c02222;
            break;
        }
        /* @hack type: stage_hooks.trail_draw called as trail_draw_hook,
         * where the member would do */
        trail_draw_hook[0](&chaos_drive_trail_style,
                           TRAILS(task)->by_camera[VIEW_INDEX], &work->pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

static void chaos_drive_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* a drive whose trails syCalloc couldn't make has none to free */
    if (TRAILS(task) == NULL) {
        return;
    }
#endif
    /* @hack type: stage_hooks.trail_free called as trail_free_hook, here
     * and below, where the member would do */
    if (TRAILS(task)->by_camera[0] != NULL) {
        trail_free_hook[0](&chaos_drive_trail_style,
                           TRAILS(task)->by_camera[0]);
        TRAILS(task)->by_camera[0] = NULL;
    }
    if (TRAILS(task)->by_camera[1] != NULL) {
        trail_free_hook[0](&chaos_drive_trail_style,
                           TRAILS(task)->by_camera[1]);
        TRAILS(task)->by_camera[1] = NULL;
    }
}

/* the task's first step */
static void chaos_drive_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    pos = work->pos;
    pos.y += PROBE_LIFT;
    GROUND(work) = find_ground(&pos);
    SPEED(work) = params.speed;
    SCALE(work) = 1.0F;
#ifdef NONMATCHING
    /* A drive must be collectible. Retry on the next step if its collider
     * could not be allocated. */
    if (!collision_init(task, &chaos_drive_shape, 1, COLLISION_OBJECT)) {
        return;
    }
#else
    collision_init(task, &chaos_drive_shape, 1, COLLISION_OBJECT);
#endif
    task->step = chaos_drive_step;
    task->draw = chaos_drive_draw;
    task->destructor = chaos_drive_destroy;
    task->extra.p = syCalloc(1, sizeof(struct drive_trails));
#ifdef NONMATCHING
    /* this side gives the drive no trails where syCalloc fails, where the
     * matching side writes through the null */
    if (task->extra.p == NULL) {
        return;
    }
#endif
    /* @hack type: stage_hooks.trail_alloc called as trail_alloc_hook, here
     * and below, where the member would do */
    TRAILS(task)->by_camera[0] =
        trail_alloc_hook[0](&chaos_drive_trail_style, TRAIL_POINTS);
    TRAILS(task)->by_camera[1] =
        trail_alloc_hook[0](&chaos_drive_trail_style, TRAIL_POINTS);
}

void chaos_drive_create(NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, DRIVE_LEVEL, chaos_drive_init, "ChaosDrive");
#ifdef NONMATCHING
    /* this side places no drive where task_create fails */
    if (task == NULL) {
        return;
    }
#endif
    work = task->work;
    work->pos = *pos;
    KIND(work) =
        (Float)rand() * ((CHAOS_DRIVE_KINDS - KIND_MARGIN) * RAND_SCALE);
}
