/* the Chaos Drives of a garden, as drive.h declares them. they draw with
 * the stage drive's models, glow and trail */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "chao/carry.h"
#include "chao/drive.h"
#include "chao/pickup.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/lines.h"
#include "hud/item_stock.h"
#include "object/chaos_drive.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/texture.h"

#include "chao/drive/text.inc"

enum drive_mode {
    DRIVE_REST,
    DRIVE_HOVER,
    DRIVE_HELD,
    DRIVE_CARRIED,
    DRIVE_SHRINK
};

struct garden_drive_work {
    struct task_work base;
    Uint8 kind;
    Angle glow_angle;
    Float alpha;
    Angle bob_angle;
    /* TRUE while the drive rests, which skips mover_meet_stage until it
     * moves again */
    Bool resting;
    Uint32 unk_44;
    struct trail *trails[2];
    Sint32 still_frames;
};
#define DRIVE_WORK(drive) ((struct garden_drive_work *)(drive)->work)

#define BOB_STEP 0x180 /* what each step adds to bob_angle */
/* what each step multiplies a hovering drive's speed by, across the ground
 * and along y */
#define DRAG 0.98F
#define DRAG_Y 0.97F
/* a push raises the drive's speed by PUSH_GAIN times how far it moved the
 * drive beyond that speed */
#define PUSH_GAIN 1.1F
/* a drive whose speed across the ground stays below STILL_SPEED for more
 * than REST_FRAMES steps rests */
#define STILL_SPEED 0.01F
#define REST_FRAMES 120
/* a carried drive counts the steps of PARTNER_USE, as a ball does. after
 * more than USE_STEPS it only starts the count over */
#define USE_STEPS 180
#define SHRINK 0.1F /* what each step takes from a shrinking drive's scale */
#define HELD_LIFT 0.7F /* how far a held drive draws above its pos */
#define TRAIL_POINTS 8
/* the trail draws only where the drive lies nearer the camera than this,
 * along the view's axis */
#define TRAIL_REACH 500.0F
#define GLOW_TOWARD_CAMERA 5.0F
/* the glow turns by GLOW_TURN times the angle at the camera between the
 * view's axis and the drive */
#define GLOW_TURN 4
/* the model's turn about x, a twelfth of a turn, under its turn about y */
#define MODEL_TILT 0x1555
/* the model turns by ang.y shifted right by MODEL_TURN_SHIFT, beyond the
 * drive's own turn by ang.y */
#define MODEL_TURN_SHIFT 1
/* a free drive's shadow's scale along x and y, and along z, times the
 * drive's scale */
#define SHADOW_SCALE_XY 0.65F
#define SHADOW_SCALE_Z 1.1F
/* the scale a drive draws at, until it shrinks away */
#define DRIVE_SCALE 1.1F
/* a held drive's shadow's scale, under DRIVE_SCALE */
#define HELD_SHADOW_SCALE 0.7F
/* a free drive smaller than DRAWN_SIZE draws nothing */
#define DRAWN_SIZE 0.05F
/* the drive's fall, and where a Chao holds it, as garden_set_carry_height
 * and garden_set_carry_radius take them */
#define GRAVITY (-0.03F)
#define CARRY_HEIGHT (-0.3F)
#define CARRY_RADIUS 1.3F

/* the id of PICKUP_LOWER, which tells a Chao to take the drive in, as it
 * takes in an animal */
#define TRAINING_ITEM_HIT 135

static const struct collision_shape drive_shapes[PICKUP_SHAPES] =
    GARDEN_PICKUP_SHAPES(PUSH_OBJECTS, 2.0F, (Sint8)TRAINING_ITEM_HIT, -4, 8);

/* the whole of CHAOSDRIVE_TEX's texture 6, as a cell 14 pixels square
 * about its center */
static const NJS_TEXANIM drive_glow_cell = {
#include "chao/drive/drive_glow_cell.inc"
};
/* clang-format off */
static struct drive_params drive_tuning = {
    6.0F, 0.05F, 0.85F, 0x280, 0x1400, 0x2c0, 0.16F, 1.0F, 0.3F, 0.6F,
};
/* clang-format on */
/* a copy of the stage drive's hover_height, which nothing here reads */
static Float drive_hover_height = 7.5F;
/* each step, a hovering drive's acceleration up gains drive_lift /
 * (drive_lift_height + its height above the floor) */
static Float drive_lift = 0.21F;
static Float drive_lift_height = 0.85F;
static Float drive_bob = 0.0015F;

static void drive_rest(struct task *drive)
{
    struct task_work *work = drive->work;

    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(drive);
        work->submode++;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = DRIVE_HELD;
        work->submode = 0;
        return;
    }
    if (collision_pushed(drive)) {
        work->mode = DRIVE_HOVER;
        work->submode = 0;
    }
}

static void drive_hover(struct task *drive)
{
    struct garden_drive_work *work = DRIVE_WORK(drive);
    struct mover *mover = drive->motion.p;
    NJS_VECTOR displacement;
    NJS_VECTOR direction;
    Float along;
    Float distance;

    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (work->base.submode) {
    case 0:
        PICKUP_FREE_SHAPES(drive);
        work->still_frames = 0;
        work->base.submode++;
        break;
    }
    mover_step(drive);
    mover_save_pos(drive);
    if (!work->resting) {
        mover_meet_stage(drive);
    }
    distance = work->base.pos.y - mover->probe.sides[SURFACE_NEG_Y].pos;
    mover->acc.y += drive_lift / (drive_lift_height + distance);
    mover->acc.y += drive_bob * (njSin(work->bob_angle) + 1.0F);
    mover->spd.x *= DRAG;
    mover->spd.y *= DRAG_Y;
    mover->spd.z *= DRAG;
    if (work->base.flags & WORK_HELD_BY_PLAYER) {
        work->base.mode = DRIVE_HELD;
        work->base.submode = 0;
        return;
    }
    if (collision_pushed(drive)) {
        displacement.x = work->base.pos.x - mover->last_pos.x;
        displacement.y = work->base.pos.y - mover->last_pos.y;
        displacement.z = work->base.pos.z - mover->last_pos.z;
        direction = displacement;
        distance = njScalor(&direction);
        if (distance >= 0.01F) {
            njUnitVector(&direction);
        }
        along = njInnerProduct(&direction, &mover->spd);
        distance = njInnerProduct(&direction, &displacement);
        if (distance > along) {
            distance = (distance - along) * PUSH_GAIN;
            mover->spd.x += direction.x * distance;
            mover->spd.y += direction.y * distance;
            mover->spd.z += direction.z * distance;
        }
    }
    displacement.x = mover->spd.x;
    displacement.y = 0.0F;
    displacement.z = mover->spd.z;
    if (!work->resting) {
        if (njScalor(&displacement) < STILL_SPEED) {
            if (work->still_frames++ > REST_FRAMES) {
                work->resting = TRUE;
                /* @hack redundant: a return, where control would fall to the
                 * function's end */
                return;
            }
        }
    } else if (njScalor(&displacement) > 0.0F) {
        work->resting = FALSE;
        work->still_frames = 0;
    }
}

static void drive_held(struct task *drive)
{
    struct task_work *work = drive->work;
    struct mover *mover = drive->motion.p;

    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HELD_SHAPES(drive);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        garden_separate_partners(drive);
        work->submode++;
        break;
    }
    /* @hack spelling: the player's heading negated, then aim_y added, where
     * aim_y less the heading would do */
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (garden_object_locked(drive)) {
            work->mode = DRIVE_CARRIED;
            work->submode = 0;
        } else {
            work->mode = DRIVE_HOVER;
            work->submode = 0;
        }
    }
}

static void drive_carried(struct task *drive)
{
    struct task_work *work = drive->work;
    int command = garden_take_command(drive);

    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HIDE_SHAPES(drive);
        work->submode++;
        work->timer = 0;
        break;
    }
    if (command == PARTNER_USE && work->timer++ > USE_STEPS) {
        work->timer = 0;
    }
    if (!garden_follow_chao(drive)) {
        work->mode = DRIVE_SHRINK;
        work->submode = 0;
    }
}

static void drive_shrink(struct task *drive)
{
    struct task_work *work = drive->work;

    /* @hack spelling: state, a second view of drive's work, where work would
     * do */
    struct garden_drive_work *state = DRIVE_WORK(drive);
    garden_take_command(drive);
    /* @hack spelling: a one-case switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HIDE_SHAPES(drive);
        work->submode++;
        work->timer = 0;
        break;
    }
    state->base.scl.x -= SHRINK;
    if (state->base.scl.x < 0.0F) {
        state->base.scl.x = 0.0F;
        task_kill(drive);
    }
}

static void drive_step(struct task *drive)
{
    struct garden_drive_work *work = DRIVE_WORK(drive);

    switch (work->base.mode) {
    case DRIVE_REST:
        drive_rest(drive);
        break;
    case DRIVE_HOVER:
        drive_hover(drive);
        break;
    case DRIVE_HELD:
        drive_held(drive);
        break;
    case DRIVE_CARRIED:
        drive_carried(drive);
        break;
    case DRIVE_SHRINK:
        drive_shrink(drive);
        break;
    }
    work->base.ang.y += drive_tuning.spin_y;
    work->glow_angle += drive_tuning.spin_z;
    work->bob_angle += BOB_STEP;
    collision_register(drive);
}

static void drive_draw(struct task *drive)
{
    struct garden_drive_work *work = DRIVE_WORK(drive);
    struct camera_place *view;
    NJS_POINT3 *camera_position;
    struct angles *camera_angles;
    NJS_VECTOR direction;
    /* @hack type: glow volatile, where a plain Float would do */
    volatile Float glow;
    Float alpha;
    Float lift;
    Float size;
    /* @hack spelling: turn and scale, locals for njRotateY and njScale, where
     * the calls by their names would do */
    void (*turn)(NJS_MATRIX *matrix, Angle yaw);
    void (*scale)(NJS_MATRIX *matrix, Float x, Float y, Float z);

    if (work->base.flags & WORK_HELD_BY_PLAYER) {
        lift = HELD_LIFT;
    } else {
        lift = 0.0F;
    }
    njFogDisable();
    njCalcPoint(NULL, &work->base.pos, &direction);
    if (TRAIL_REACH > _builtin_fabsf(direction.z)) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        switch (work->kind) {
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
        trail_draw(&chaos_drive_trail_style, work->trails[0], &work->base.pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    glow = drive_tuning.glow_base +
           (drive_tuning.glow_amplitude * (njCos(work->glow_angle) - 1.0F));
    alpha = drive_tuning.alpha_base +
            (drive_tuning.alpha_amplitude * (njCos(work->glow_angle) - 1.0F));
    alpha *= work->alpha;
    /* @hack redundant: the glow's address, taken and dropped */
    (void)&chaos_drive_glow;
    njSetTexture(&chaosdrive_tex_texlist);
    njPushMatrixEx();
    njTranslate(NULL, work->base.pos.x, work->base.pos.y + lift,
                work->base.pos.z);
    njPushMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    switch (work->kind) {
    case CHAOS_DRIVE_YELLOW:
        constant_material_set(alpha, 1.0F, 1.0F, 0.0F);
        break;
    case CHAOS_DRIVE_GREEN:
        constant_material_set(alpha, 0.0F, 1.0F, 0.0F);
        break;
    case CHAOS_DRIVE_RED:
        constant_material_set(alpha, 1.0F, 0.0F, 0.0F);
        break;
    case CHAOS_DRIVE_PURPLE:
        constant_material_set(alpha, 0.8F, 0.0F, 1.0F);
        break;
    }
    view = camera_views[camera_current_index];
    direction.x = view->pos.x;
    direction.y = view->pos.y;
    direction.z = view->pos.z;
    direction.x -= work->base.pos.x;
    direction.y -= work->base.pos.y;
    direction.z -= work->base.pos.z;
    njUnitVector(&direction);
    direction.x *= GLOW_TOWARD_CAMERA;
    direction.y *= GLOW_TOWARD_CAMERA;
    direction.z *= GLOW_TOWARD_CAMERA;
    njTranslateEx(&direction);
    chaos_drive_glow.sx = glow;
    chaos_drive_glow.sy = glow;
    view = camera_views[0];
    camera_position = &view->pos;
    camera_angles = &view->ang;
    direction.x = work->base.pos.x;
    direction.y = work->base.pos.y;
    direction.z = work->base.pos.z;
    direction.x -= camera_position->x;
    direction.y -= camera_position->y;
    direction.z -= camera_position->z;
    njUnitVector(&direction);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateX(NULL, -camera_angles->x);
    turn = njRotateY;
    turn(NULL, -camera_angles->y);
    njCalcVector(NULL, &direction, &direction);
    njPopMatrixEx();
    chaos_drive_glow.ang =
        (Angle)(acosf(direction.z) * ANGLE_PER_RADIAN) * GLOW_TURN;
    njDrawSprite3D(&chaos_drive_glow, 0,
                   NJD_SPRITE_ANGLE | NJD_SPRITE_COLOR | NJD_SPRITE_SCALE |
                       NJD_SPRITE_ALPHA);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    scale = njScale;
    if (!(work->base.flags & WORK_HELD_BY_PLAYER)) {
        size = work->base.scl.x;
        if (size > DRAWN_SIZE) {
            turn(NULL, work->base.ang.y);
            njPushMatrixEx();
            njRotateX(NULL, MODEL_TILT);
            turn(NULL, work->base.ang.y >> MODEL_TURN_SHIFT);
            scale(NULL, size, size, size);
            njCnkSimpleDrawObject(chaos_drive_objects[work->kind]);
            njPopMatrixEx();
            scale(NULL, size * SHADOW_SCALE_XY, size * SHADOW_SCALE_XY,
                  size * SHADOW_SCALE_Z);
            njCnkModDrawModel(shadow_model);
        }
    } else {
        turn(NULL, work->base.ang.y);
        scale(NULL, DRIVE_SCALE, DRIVE_SCALE, DRIVE_SCALE);
        njCnkSimpleDrawObject(chaos_drive_objects[work->kind]);
        scale(NULL, HELD_SHADOW_SCALE, HELD_SHADOW_SCALE, HELD_SHADOW_SCALE);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrix(1);
    njFogEnable();
}

static void drive_destroy(struct task *drive)
{
    struct garden_drive_work *work = DRIVE_WORK(drive);

    if (work->trails[0]) {
        trail_free(&chaos_drive_trail_style, work->trails[0]);
        work->trails[0] = NULL;
    }
    if (work->trails[1]) {
        trail_free(&chaos_drive_trail_style, work->trails[1]);
        work->trails[1] = NULL;
    }
    garden_unregister_object(drive);
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

#ifdef NONMATCHING
/* this side returns whether it made the drive, as drive.h says */
Bool garden_drive_create(Uint8 kind, NJS_POINT3 *position, NJS_VECTOR *velocity)
#else
void garden_drive_create(Uint8 kind, NJS_POINT3 *position, NJS_VECTOR *velocity)
#endif
{
    struct task *drive = task_create(0, 2, drive_step, GARDEN_DRIVE_TASK_NAME);
    struct garden_drive_work *work;
    struct task_work *base;
    struct mover *mover;

    (void)drive_glow_cell;
    (void)drive_hover_height;
#ifdef NONMATCHING
    /* this side returns FALSE where task_create fails. it sets the draw and the
     * destructor only once the drive is whole */
    if (drive == NULL) {
        return FALSE;
    }
#else
    drive->destructor = drive_destroy;
    drive->draw = drive_draw;
#endif
    work = syCalloc(1, sizeof(*work));
#ifdef NONMATCHING
    /* this side destroys the task and returns FALSE where the heap has no work.
     * the matching side writes through the null */
    if (work == NULL) {
        task_destroy(drive);
        return FALSE;
    }
#endif
    drive->work = &work->base;
    base = drive->work;
    mover = mover_alloc(drive);
#ifdef NONMATCHING
    /* this side destroys the drive and returns FALSE where mover_alloc fails.
     * the matching side writes through the null */
    if (mover == NULL) {
        task_destroy(drive);
        return FALSE;
    }
#endif
    mover_set_gravity(drive, GRAVITY);
    mover->probe_height = 2.5F;
    mover->floor_y = -3.5F;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 0x1800;
    } else {
        mover->hold_turn = -0x1000;
    }
    base->pos = *position;
    mover->spd = *velocity;
    mover->radius = 4.0F;
    base->scl.x = DRIVE_SCALE;
#ifdef NONMATCHING
    /* this side destroys the drive and returns FALSE where collision_init
     * fails */
    if (!collision_init(drive, drive_shapes, PICKUP_SHAPES,
                        COLLISION_OBJECT_MUTUAL)) {
        task_destroy(drive);
        return FALSE;
    }
#else
    collision_init(drive, drive_shapes, PICKUP_SHAPES, COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(base, drive->motion.p, FALL_KIND_NORMAL);
    base->mode = DRIVE_HOVER;
    base->submode = 0;
#ifdef NONMATCHING
    /* this side destroys the drive where the garden has no slot for it. only
     * then does it set the draw and the destructor, which read the whole
     * drive */
    if (!garden_register_object(GARDEN_OBJECT_MINIMAL, drive, 1)) {
        task_destroy(drive);
        return FALSE;
    }
    drive->destructor = drive_destroy;
    drive->draw = drive_draw;
#else
    garden_register_object(GARDEN_OBJECT_MINIMAL, drive, 1);
#endif
    work->base.tick = kind + ITEM_CHAOS_DRIVE;
    work->kind = kind;
    work->trails[0] = trail_alloc(&chaos_drive_trail_style, TRAIL_POINTS);
    work->trails[1] = trail_alloc(&chaos_drive_trail_style, TRAIL_POINTS);
    garden_set_carry_height(drive, CARRY_HEIGHT);
    garden_set_carry_radius(drive, CARRY_RADIUS);
#ifdef NONMATCHING
    /* this side returns that it made the drive */
    return TRUE;
#endif
}
