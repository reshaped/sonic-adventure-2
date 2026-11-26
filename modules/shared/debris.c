/* debris: the pieces a broken model flies apart into */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/field.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* task_create's level for the pieces' task */
#define LEVEL 3

/* the bits of a piece's flags that hold its debris_axis */
#define AXIS_BITS 0x7
/* in a piece's flags: the piece was made during a time stop, while
 * game_state.time_stopped isn't 0. during a time stop, only pieces with the bit
 * move. debris_step clears the bit once the time stop ends */
#define MADE_IN_TIME_STOP 0x80000000

/* RANDOM_HALF() is a random Float from -0.5 up to 0.5. RANDOM_WHOLE(n) is a
 * random whole number within n / 2 of 0. RANDOM_TURN_EITHER_WAY() is an Angle
 * of up to a turn either way, and RANDOM_TILT() one of up to 30 degrees */
#define RANDOM_HALF() ((rand() * RAND_SCALE) - 0.5F)
#define RANDOM_WHOLE(n) ((int)(RANDOM_HALF() * (n)))
#define RANDOM_TURN_EITHER_WAY() ((Angle)(RANDOM_HALF() * (2.0F * FULL_TURN)))
#define RANDOM_TILT() ((Angle)(RANDOM_HALF() * (FULL_TURN / 6.0F)))

/* a new piece's random spin about x, y and z spans these widths, in degrees a
 * step. each is written as a share of FULL_TURN, but debris_step reads it as
 * degrees. SPIN_X is about 18.2 degrees, SPIN_Y 10.9 and SPIN_Z 9.1 */
#define SPIN_X (FULL_TURN / 3600.0F)
#define SPIN_Y (FULL_TURN / 6000.0F)
#define SPIN_Z (FULL_TURN / 7200.0F)
/* SPIN(width) is a random whole number within width / 2 of 0.
 * SIZED_SPIN(small, width) reaches SIZED_REACH * small times as far */
#define SIZED_REACH 4
#define SPIN(width) ((int)(RANDOM_HALF() * (width)))
#define SIZED_SPIN(small, width)                                               \
    ((int)(RANDOM_HALF() * (small) * (SIZED_REACH * (width))))

/* a DEBRIS_THROW_SQUARE piece turns by RANDOM_WHOLE(PIECE_QUARTERS) quarter
 * turns. the point it starts from turns by RANDOM_WHOLE(POINT_QUARTERS) */
#define PIECE_QUARTERS 72
#define POINT_QUARTERS 180

#define LIFE() ((int)(rand() * debris_life_spread * RAND_SCALE) + debris_life)

/* a piece in its last SHRINK_LIFE steps of life loses SHRINK_STEP of its
 * scale each step */
#define SHRINK_LIFE 10
#define SHRINK_STEP 0.1F
/* a piece that meets its floor at a falling speed under REST_SPEED comes to
 * rest there. REST_SPEED is REST_GRAVITIES times the speed gravity adds in a
 * step */
#define REST_GRAVITIES 5.0F
#define REST_SPEED (debris_gravity * REST_GRAVITIES)
/* a piece at rest on its floor loses REST_SHRINK_STEP of its scale each step */
#define REST_SHRINK_STEP 0.05F
/* a bounce reverses a piece's falling speed. the piece keeps BOUNCE of that
 * speed and of its spin. a piece that comes to rest keeps BOUNCE of its speed
 * along its floor */
#define BOUNCE 0.6F
/* each step, a DEBRIS_FIELD piece's speed closes FIELD_PULL of its gap to the
 * field's speed */
#define FIELD_PULL 0.2F
/* a DEBRIS_TRAIL piece leaves a puff on a random TRAIL_CHANCE of its steps
 * above its floor. the puff moves at TRAIL_SPEED times the piece's speed */
#define TRAIL_CHANCE 0.1F
#define TRAIL_SPEED 0.7F
/* the scale of a piece's puffs and bursts, in radii of its model */
#define PARTICLE_SIZE 0.3F
#define BIG_PARTICLE_SIZE 0.9F
/* @hack spelling: the size reads piece->flags, where debris_step's local
 * flags would do */
#define PARTICLE_SCALE(piece)                                                  \
    ((piece)->model->r * (((piece)->flags & DEBRIS_BIG_PARTICLES)              \
                              ? BIG_PARTICLE_SIZE                              \
                              : PARTICLE_SIZE))
/* a DEBRIS_FADE_COLORS piece's tint is WHITE under an alpha from 0 to
 * ALPHA_MAX, at ALPHA_SHIFT */
#define ALPHA_MAX 255
#define WHITE 0xffffff
/* world_gravity runs along an axis where its component on that axis is over
 * AXIS_MIN or under -AXIS_MIN. a unit vector has at most one such component */
#define AXIS_MIN 0.9F
/* the axis the image's debris_gravity_axis returns where world_gravity runs
 * along no axis. it is R4 as the call finds it. syCalloc leaves R4 at 0x4c, the
 * offset of the last word it clears of the piece's 80 bytes. in a piece's
 * flags, 0x4c sets DEBRIS_AXIS_NEG_Z, DEBRIS_VANISH and bit 0x8. nothing reads
 * bit 0x8 */
#define AXIS_UNSET 0x4c
/* below a radius of MIN_RADIUS, debris_break_built spins its pieces as
 * debris_break does */
#define MIN_RADIUS 0.1F

/* one piece, from its making until it shrinks away */
struct debris {
    NJS_POINT3 pos; /* where its model's center is */
    NJS_VECTOR spd;
    struct angles ang;
    NJS_VECTOR spin; /* degrees a step */
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    NJS_DIRECT_HEAD *build; /* drawn in model's place at full scale, or null */
    Float floor;
    Sint32 life; /* steps left */
    Uint32 flags; /* DEBRIS_ flags, a debris_axis and MADE_IN_TIME_STOP */
    Float scale; /* 1 at full size. debris_step frees the piece at 0 */
    struct debris *next;
};

Float debris_friction = 0.989F;
Float debris_gravity = 0.089F;
Sint32 debris_life = 160;
Float debris_life_spread = 55.0F;
/* the task that steps and draws the pieces, or null */
static struct task *debris_task = NULL;
/* the pieces, the newest first */
static struct debris *debris_list = NULL;
/* each debris_axis's row names the components that debris_step reads as a
 * DEBRIS_GRAVITY piece's x, y and z. it reads pos and spd by the same row. an
 * entry is a component's index: 0 for x, 1 for y and 2 for z. a negative entry
 * reads its component negated. gravity pulls toward -y as debris_step reads it,
 * so the piece falls along its row's y entry */
/* @bug logic: the +x, -z and +z rows send their pieces the wrong way. the +x
 * row's y entry is 0, x's index, which can't be negated, so its pieces fall
 * toward -x. the -z and +z rows' y entries name y, so their pieces fall along
 * y. debris_step reads a row's y entry at 0x179a of the object's code */
static Sint32 debris_axes[DEBRIS_AXES][3] = {
    {1, 0, 2}, {-1, 0, 2}, {0, 1, 2}, {0, -1, 2}, {2, 1, 0}, {2, -1, 0},
};

static void debris_destroy(struct task *task);
static void debris_step(struct task *task);
static void debris_draw(struct task *task);
static void debris_launch_spin_built(NJS_POINT3 *pos, struct angles *ang,
                                     Float floor, NJS_VECTOR *spd,
                                     struct debris_spin *spin,
                                     NJS_CNK_MODEL *model,
                                     NJS_DIRECT_HEAD *build,
                                     NJS_TEXLIST *texlist, Uint32 flags);
/* nothing calls debris_launch, and no header declares it or
 * debris_launch_built. debris_launch is external only because SHC leaves out a
 * static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void debris_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                   NJS_VECTOR *spd, NJS_CNK_MODEL *model, NJS_TEXLIST *texlist);
void debris_launch_built(NJS_POINT3 *pos, struct angles *ang, Float floor,
                         NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                         NJS_DIRECT_HEAD *build, NJS_TEXLIST *texlist);
/* NOLINTEND(misc-use-internal-linkage) */

int debris_count(void)
{
    if (debris_task != NULL) {
        return debris_task->work->timer;
    }
    return 0;
}

int debris_build(Uint8 **buffer, struct debris_built_part *parts, int count)
{
    int built = 0;
    int i;

    for (i = 0; i < count; i++, parts++) {
        njSetTexture(parts->texlist);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((parts->build = model_build(buffer, parts->model)) != NULL) {
            built++;
        }
    }
    return built;
}

static enum debris_axis debris_gravity_axis(void)
{
    /* @bug uninitialized: returns axis unset where world_gravity runs along
     * no axis, as a tilted gravity can. debris_throw_built_flags then sets
     * that value's bits in a piece's flags. the return is at 0xa2 of the
     * object's code */
#ifdef NONMATCHING
    /* this side keeps the tag's result. under a tilted gravity, a thrown piece
     * falls as a DEBRIS_AXIS_NEG_Z piece and vanishes at its floor, as in the
     * image */
    enum debris_axis axis = AXIS_UNSET;
#else
    enum debris_axis axis;
#endif

    if (world_gravity.x > AXIS_MIN) {
        axis = DEBRIS_AXIS_POS_X;
    }
    if (world_gravity.x < -AXIS_MIN) {
        axis = DEBRIS_AXIS_NEG_X;
    }
    if (world_gravity.y > AXIS_MIN) {
        axis = DEBRIS_AXIS_POS_Y;
    }
    if (world_gravity.y < -AXIS_MIN) {
        axis = DEBRIS_AXIS_NEG_Y;
    }
    if (world_gravity.z > AXIS_MIN) {
        axis = DEBRIS_AXIS_POS_Z;
    }
    if (world_gravity.z < -AXIS_MIN) {
        axis = DEBRIS_AXIS_NEG_Z;
    }
    return axis;
}

void debris_throw_flags(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                        Angle angle, Float speed, Float floor,
                        NJS_CNK_MODEL *model, NJS_TEXLIST *texlist,
                        Uint32 flags)
{
    debris_throw_built_flags(pos, mode, scale, angle, speed, floor, model, NULL,
                             texlist, flags);
}

void debris_throw(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale, Angle angle,
                  Float speed, Float floor, NJS_CNK_MODEL *model,
                  NJS_TEXLIST *texlist)
{
    debris_throw_built(pos, mode, scale, angle, speed, floor, model, NULL,
                       texlist);
}

void debris_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                   NJS_VECTOR *spd, NJS_CNK_MODEL *model, NJS_TEXLIST *texlist)
{
    debris_launch_built(pos, ang, floor, spd, model, NULL, texlist);
}

void debris_launch_flags(NJS_POINT3 *pos, struct angles *ang, Float floor,
                         NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                         NJS_TEXLIST *texlist, Uint32 flags)
{
    debris_launch_built_flags(pos, ang, floor, spd, model, NULL, texlist,
                              flags);
}

void debris_launch_spin(NJS_POINT3 *pos, struct angles *ang, Float floor,
                        NJS_VECTOR *spd, struct debris_spin *spin,
                        NJS_CNK_MODEL *model, NJS_TEXLIST *texlist,
                        Uint32 flags)
{
    debris_launch_spin_built(pos, ang, floor, spd, spin, model, NULL, texlist,
                             flags);
}

void debris_throw_built(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                        Angle angle, Float speed, Float floor,
                        NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *build,
                        NJS_TEXLIST *texlist)
{
    debris_throw_built_flags(pos, mode, scale, angle, speed, floor, model,
                             build, texlist, 0);
}

void debris_throw_built_flags(NJS_POINT3 *pos, int mode, NJS_VECTOR *scale,
                              Angle angle, Float speed, Float floor,
                              NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *build,
                              NJS_TEXLIST *texlist, Uint32 flags)
{
    struct task *task;
    struct debris *piece;
    /* @hack spelling: spd holds the unit step up before it holds the speed,
     * where a local of its own would do */
    NJS_VECTOR spd;
    Float ratio;
    enum debris_axis axis;

    if (debris_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, debris_step, "EfCrash3DExec");
        if (task != NULL) {
            task->draw = debris_draw;
            task->destructor = debris_destroy;
            debris_task = task;
        }
    }
    if (debris_task == NULL) {
        return;
    }
    piece = syCalloc(1, sizeof(struct debris));
    if (piece == NULL) {
        return;
    }
    flags &= ~AXIS_BITS;
    if (flags & DEBRIS_GRAVITY) {
        axis = debris_gravity_axis();
        flags |= axis;
        /* debris_step meets floor on the y that the piece's row of
         * debris_axes reads. the +y and +z rows read y negated, which
         * negating floor matches. the +x row reads x as it is, as its bug
         * says. its piece meets its floor where x is -floor */
        switch (axis) {
        case DEBRIS_AXIS_POS_X:
        case DEBRIS_AXIS_POS_Y:
        case DEBRIS_AXIS_POS_Z:
            floor = -floor;
            break;
        default:
            break;
        }
    }
    if (game_state.time_stopped) {
        flags |= MADE_IN_TIME_STOP;
    }
    piece->flags = flags;
    piece->life = LIFE();
    piece->scale = 1.0F;
    piece->spin.x = (flags & DEBRIS_NO_SPIN_X) ? 0 : SPIN(SPIN_X);
    piece->spin.y = (flags & DEBRIS_NO_SPIN_Y) ? 0 : SPIN(SPIN_Y);
    piece->spin.z = (flags & DEBRIS_NO_SPIN_Z) ? 0 : SPIN(SPIN_Z);
    piece->model = model;
    piece->build = build;
    piece->texlist = texlist;
    piece->floor = floor;
    /* @hack spelling: each arm sets up spd and the matrix and works out the
     * piece's pos itself, where once before the switch and once after it
     * would do */
    switch (mode) {
    case DEBRIS_THROW_SQUARE:
        /* @hack type: the quarter turns multiplied as Floats, where Angle
         * arithmetic would do */
        piece->ang.x =
            (Angle)(RANDOM_WHOLE(PIECE_QUARTERS) * (Float)QUARTER_TURN);
        piece->ang.y =
            (Angle)(RANDOM_WHOLE(PIECE_QUARTERS) * (Float)QUARTER_TURN) + angle;
        piece->ang.z = 0;
        spd.x = 0.0F;
        spd.y = 1.0F;
        spd.z = 0.0F;
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        if (!(flags & DEBRIS_UNPLACED)) {
            njTranslateV(NULL, pos);
            njRotateY(NULL, angle);
        }
        njScale(NULL, scale->x, scale->y, scale->z);
        njRotateY(NULL, (RANDOM_WHOLE(POINT_QUARTERS) * QUARTER_TURN) +
                            RANDOM_TILT());
        njRotateX(NULL, (RANDOM_WHOLE(POINT_QUARTERS) * QUARTER_TURN) +
                            RANDOM_TILT());
        /* @hack redundant: the njTranslateV call in a branch that never runs */
        if (0) {
            njTranslateV(NULL, scale);
        } else {
            njTranslateV(NULL, &spd);
            njCalcPoint(NULL, &piece->model->center, &piece->pos);
        }
        break;
    case DEBRIS_THROW_ANY:
    default:
        piece->ang.x = RANDOM_TURN_EITHER_WAY();
        piece->ang.y = RANDOM_TURN_EITHER_WAY();
        piece->ang.z = 0;
        spd.x = 0.0F;
        spd.y = 1.0F;
        spd.z = 0.0F;
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        if (!(flags & DEBRIS_UNPLACED)) {
            njTranslateV(NULL, pos);
            njRotateY(NULL, angle);
        }
        njScale(NULL, scale->x, scale->y, scale->z);
        njRotateY(NULL, RANDOM_TURN_EITHER_WAY());
        njRotateX(NULL, RANDOM_TURN_EITHER_WAY());
        njTranslateV(NULL, &spd);
        njCalcPoint(NULL, &piece->model->center, &piece->pos);
        /* @hack spelling: the last arm ends without a break, where a break
         * would do */
    }
    njPopMatrix(1);
    spd.x = piece->pos.x - pos->x;
    spd.y = piece->pos.y - pos->y;
    spd.z = piece->pos.z - pos->z;
    ratio = speed / njScalor(&spd);
    spd.x *= ratio;
    spd.y *= ratio;
    spd.z *= ratio;
    piece->spd = spd;
    piece->next = debris_list;
    debris_list = piece;
}

void debris_break(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                  int count, Float speed, Float spread, Float floor,
                  NJS_VECTOR *spd)
{
    debris_break_flags(pos, angle, parts, count, speed, spread, floor, spd, 0);
}

void debris_break_flags(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                        int count, Float speed, Float spread, Float floor,
                        NJS_VECTOR *spd, Uint32 flags)
{
    struct task *task;
    struct debris *piece;
    struct debris_part *part;
    NJS_VECTOR away;
    Float ratio;
    int i;

    if (debris_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, debris_step, "EfCrash3DExec");
        if (task != NULL) {
            task->draw = debris_draw;
            task->destructor = debris_destroy;
            debris_task = task;
        }
    }
    if (debris_task == NULL) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(flags & DEBRIS_UNPLACED)) {
        njTranslateV(NULL, pos);
        njRotateY(NULL, angle);
    }
    if (game_state.time_stopped) {
        flags |= MADE_IN_TIME_STOP;
    }
    part = parts;
    for (i = 0; i < count; i++, part++) {
        piece = syCalloc(1, sizeof(struct debris));
        if (piece != NULL) {
            piece->flags = flags;
            piece->scale = 1.0F;
            piece->life = LIFE();
            piece->spin.x = (flags & DEBRIS_NO_SPIN_X) ? 0 : SPIN(SPIN_X);
            piece->spin.y = (flags & DEBRIS_NO_SPIN_Y) ? 0 : SPIN(SPIN_Y);
            piece->spin.z = (flags & DEBRIS_NO_SPIN_Z) ? 0 : SPIN(SPIN_Z);
            piece->model = part->model;
            piece->texlist = part->texlist;
            piece->floor = floor;
            piece->ang = part->ang;
            njPushMatrixEx();
            njTranslateV(NULL, &part->pos);
            njRotateZ(NULL, piece->ang.z);
            njRotateY(NULL, piece->ang.y);
            njRotateX(NULL, piece->ang.x);
            njCalcPoint(NULL, &piece->model->center, &piece->pos);
            current_matrix_angles_xyz(&piece->ang.x, &piece->ang.y,
                                      &piece->ang.z);
            njPopMatrix(1);
            /* @hack redundant: the offset from pos worked out before the
             * test, which each arm works out again */
            away.x = piece->pos.x - pos->x;
            away.y = piece->pos.y - pos->y;
            away.z = piece->pos.z - pos->z;
            /* @hack spelling: each arm works out the offset from pos and adds
             * spd itself, where once before the test and once after it would
             * do */
            if (!(flags & DEBRIS_UNPLACED)) {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio =
                    (speed + (spread * rand() * RAND_SCALE)) / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            } else {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio = speed / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += RANDOM_HALF() * spread * 2.0F;
                away.y += RANDOM_HALF() * spread * 2.0F;
                away.z += RANDOM_HALF() * spread * 2.0F;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            }
            piece->spd = away;
            piece->next = debris_list;
            debris_list = piece;
        }
    }
    njPopMatrix(1);
}

void debris_break_sized(NJS_POINT3 *pos, Angle angle, struct debris_part *parts,
                        int count, Float speed, Float spread, Float floor,
                        NJS_VECTOR *spd, Uint32 flags, Float radius)
{
    struct task *task;
    struct debris *piece;
    struct debris_part *part;
    NJS_VECTOR away;
    Float ratio;
    Float small;
    int i;

    if (debris_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, debris_step, "EfCrash3DExec");
        if (task != NULL) {
            task->draw = debris_draw;
            task->destructor = debris_destroy;
            debris_task = task;
        }
    }
    if (debris_task == NULL) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(flags & DEBRIS_UNPLACED)) {
        njTranslateV(NULL, pos);
        njRotateY(NULL, angle);
    }
    if (game_state.time_stopped) {
        flags |= MADE_IN_TIME_STOP;
    }
    part = parts;
    for (i = 0; i < count; i++, part++) {
        piece = syCalloc(1, sizeof(struct debris));
        if (piece != NULL) {
            piece->flags = flags;
            piece->scale = 1.0F;
            piece->model = part->model;
            piece->texlist = part->texlist;
            piece->floor = floor;
            piece->ang = part->ang;
            small = 1.0F - (piece->model->r / radius);
            if (small > 1.0F) {
                small = 1.0F;
            }
            if (small < 0.0F) {
                small = 0.0F;
            }
            piece->life = LIFE();
            piece->spin.x =
                (flags & DEBRIS_NO_SPIN_X) ? 0 : SIZED_SPIN(small, SPIN_X);
            piece->spin.y =
                (flags & DEBRIS_NO_SPIN_Y) ? 0 : SIZED_SPIN(small, SPIN_Y);
            piece->spin.z =
                (flags & DEBRIS_NO_SPIN_Z) ? 0 : SIZED_SPIN(small, SPIN_Z);
            njPushMatrixEx();
            njTranslateV(NULL, &part->pos);
            njRotateZ(NULL, piece->ang.z);
            njRotateY(NULL, piece->ang.y);
            njRotateX(NULL, piece->ang.x);
            njCalcPoint(NULL, &piece->model->center, &piece->pos);
            current_matrix_angles_xyz(&piece->ang.x, &piece->ang.y,
                                      &piece->ang.z);
            njPopMatrix(1);
            /* @hack spelling: each arm works out the offset from pos and adds
             * spd itself, where once before the test and once after it would
             * do */
            if (!(flags & DEBRIS_UNPLACED)) {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio =
                    (speed + (spread * rand() * RAND_SCALE)) / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            } else {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio = speed / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += RANDOM_HALF() * spread * 2.0F;
                away.y += RANDOM_HALF() * spread * 2.0F;
                away.z += RANDOM_HALF() * spread * 2.0F;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            }
            piece->spd = away;
            piece->next = debris_list;
            debris_list = piece;
        }
    }
    njPopMatrix(1);
}

void debris_break_built(NJS_POINT3 *pos, Angle angle,
                        struct debris_built_part *parts, int count, Float speed,
                        Float spread, Float floor, NJS_VECTOR *spd,
                        Uint32 flags, Float radius)
{
    struct task *task;
    struct debris *piece;
    struct debris_built_part *part;
    NJS_VECTOR away;
    Float ratio;
    Float small;
    int i;

    if (debris_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, debris_step, "EfCrash3DExec");
        if (task != NULL) {
            task->draw = debris_draw;
            task->destructor = debris_destroy;
            debris_task = task;
        }
    }
    if (debris_task == NULL) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(flags & DEBRIS_UNPLACED)) {
        njTranslateV(NULL, pos);
        njRotateY(NULL, angle);
    }
    if (game_state.time_stopped) {
        flags |= MADE_IN_TIME_STOP;
    }
    part = parts;
    for (i = 0; i < count; i++, part++) {
        piece = syCalloc(1, sizeof(struct debris));
        if (piece != NULL) {
            piece->flags = flags;
            piece->scale = 1.0F;
            piece->model = part->model;
            piece->build = part->build;
            piece->texlist = part->texlist;
            piece->floor = floor;
            piece->ang = part->ang;
            if (radius < MIN_RADIUS) {
                piece->life = LIFE();
                piece->spin.x = (flags & DEBRIS_NO_SPIN_X) ? 0 : SPIN(SPIN_X);
                piece->spin.y = (flags & DEBRIS_NO_SPIN_Y) ? 0 : SPIN(SPIN_Y);
                piece->spin.z = (flags & DEBRIS_NO_SPIN_Z) ? 0 : SPIN(SPIN_Z);
            } else {
                small = 1.0F - (piece->model->r / radius);
                if (small > 1.0F) {
                    small = 1.0F;
                }
                if (small < 0.0F) {
                    small = 0.0F;
                }
                /* @hack spelling: debris_life first in the sum, where LIFE()
                 * would do */
                piece->life = debris_life +
                              (int)(rand() * debris_life_spread * RAND_SCALE);
                piece->spin.x =
                    (flags & DEBRIS_NO_SPIN_X) ? 0 : SIZED_SPIN(small, SPIN_X);
                piece->spin.y =
                    (flags & DEBRIS_NO_SPIN_Y) ? 0 : SIZED_SPIN(small, SPIN_Y);
                piece->spin.z =
                    (flags & DEBRIS_NO_SPIN_Z) ? 0 : SIZED_SPIN(small, SPIN_Z);
            }
            njPushMatrixEx();
            njTranslateV(NULL, &part->pos);
            njRotateZ(NULL, piece->ang.z);
            njRotateY(NULL, piece->ang.y);
            njRotateX(NULL, piece->ang.x);
            njCalcPoint(NULL, &piece->model->center, &piece->pos);
            current_matrix_angles_xyz(&piece->ang.x, &piece->ang.y,
                                      &piece->ang.z);
            njPopMatrix(1);
            /* @hack spelling: each arm works out the offset from pos and adds
             * spd itself, where once before the test and once after it would
             * do */
            if (!(flags & DEBRIS_UNPLACED)) {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio =
                    (speed + (spread * rand() * RAND_SCALE)) / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            } else {
                away.x = piece->pos.x - pos->x;
                away.y = piece->pos.y - pos->y;
                away.z = piece->pos.z - pos->z;
                ratio = speed / njScalor(&away);
                away.x *= ratio;
                away.y *= ratio;
                away.z *= ratio;
                away.x += RANDOM_HALF() * spread * 2.0F;
                away.y += RANDOM_HALF() * spread * 2.0F;
                away.z += RANDOM_HALF() * spread * 2.0F;
                away.x += spd->x;
                away.y += spd->y;
                away.z += spd->z;
            }
            piece->spd = away;
            piece->next = debris_list;
            debris_list = piece;
        }
    }
    njPopMatrix(1);
}

void debris_launch_built(NJS_POINT3 *pos, struct angles *ang, Float floor,
                         NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                         NJS_DIRECT_HEAD *build, NJS_TEXLIST *texlist)
{
    debris_launch_spin_built(pos, ang, floor, spd, NULL, model, build, texlist,
                             0);
}

void debris_launch_built_flags(NJS_POINT3 *pos, struct angles *ang, Float floor,
                               NJS_VECTOR *spd, NJS_CNK_MODEL *model,
                               NJS_DIRECT_HEAD *build, NJS_TEXLIST *texlist,
                               Uint32 flags)
{
    debris_launch_spin_built(pos, ang, floor, spd, NULL, model, build, texlist,
                             flags);
}

/* debris_launch_spin, taking build as debris_launch_built_flags does */
static void debris_launch_spin_built(NJS_POINT3 *pos, struct angles *ang,
                                     Float floor, NJS_VECTOR *spd,
                                     struct debris_spin *spin,
                                     NJS_CNK_MODEL *model,
                                     NJS_DIRECT_HEAD *build,
                                     NJS_TEXLIST *texlist, Uint32 flags)
{
    struct task *task;
    struct debris *piece;
    /* @hack spelling: center, a local for the piece's pos, where njCalcPoint
     * into piece->pos would do */
    NJS_POINT3 center;

    if (debris_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, debris_step, "EfCrash3DExec");
        if (task != NULL) {
            task->draw = debris_draw;
            task->destructor = debris_destroy;
            debris_task = task;
        }
    }
    if (debris_task == NULL) {
        return;
    }
    if (game_state.time_stopped) {
        flags |= MADE_IN_TIME_STOP;
    }
    piece = syCalloc(1, sizeof(struct debris));
    if (piece == NULL) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, pos);
    njRotateZ(NULL, ang->z);
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    njCalcPoint(NULL, &model->center, &center);
    njPopMatrix(1);
    piece->flags = flags;
    piece->scale = 1.0F;
    piece->life = LIFE();
    if (spin != NULL) {
        piece->spin.x = spin->x;
        piece->spin.y = spin->y;
        piece->spin.z = spin->z;
    } else {
        piece->spin.x = (flags & DEBRIS_NO_SPIN_X) ? 0 : SPIN(SPIN_X);
        piece->spin.y = (flags & DEBRIS_NO_SPIN_Y) ? 0 : SPIN(SPIN_Y);
        piece->spin.z = (flags & DEBRIS_NO_SPIN_Z) ? 0 : SPIN(SPIN_Z);
    }
    piece->model = model;
    piece->build = build;
    piece->texlist = texlist;
    piece->floor = floor;
    piece->pos = center;
    piece->ang = *ang;
    piece->spd = *spd;
    piece->next = debris_list;
    debris_list = piece;
}

static void debris_destroy(struct task *task)
{
    struct debris *piece;

    if (task == debris_task) {
        debris_task = NULL;
    }
    while (debris_list != NULL) {
        piece = debris_list;
        /* @hack spelling: the next piece read through debris_list, where
         * piece->next would do */
        debris_list = debris_list->next;
        syFree(piece);
    }
}

/* a piece that a time stop holds still goes on shrinking in its last
 * SHRINK_LIFE steps of life. work->timer keeps the number of pieces the step
 * began with, for debris_count */
static void debris_step(struct task *task)
{
    struct debris *piece;
    struct debris *prev;
    struct debris *next;
    int count;
    int running;
    Uint32 flags;
    Uint32 mapped;
    /* @hack spelling: axes walked through the row, where axes[0], axes[1]
     * and axes[2] would do */
    Sint32 *axes;
    int ax;
    int ay;
    int az;
    /* @hack spelling: fp, a pointer walked along pos and along spd, where
     * the components named or indexed in place would do */
    Float *fp;
    Float px;
    Float py;
    Float pz;
    Float vx;
    Float vy;
    Float vz;
    /* @hack spelling: field.spd also holds each puff's speed, where a local
     * of its own would do */
    struct field_sample field;

    running = !game_state.time_stopped;
    piece = debris_list;
    prev = NULL;
    count = 0;
    while (piece != NULL) {
        count++;
        flags = piece->flags;
        if ((flags & MADE_IN_TIME_STOP) || running) {
            if (running) {
                flags &= ~MADE_IN_TIME_STOP;
                piece->flags = flags;
            }
            mapped = flags & DEBRIS_GRAVITY;
            if (mapped) {
                axes = debris_axes[flags & AXIS_BITS];
                ax = *axes++;
                ay = *axes++;
                az = *axes;
                fp = &piece->pos.x;
                px = ax >= 0 ? fp[ax] : -fp[-ax];
                py = ay >= 0 ? fp[ay] : -fp[-ay];
                pz = az >= 0 ? fp[az] : -fp[-az];
                fp = &piece->spd.x;
                vx = ax >= 0 ? fp[ax] : -fp[-ax];
                vy = ay >= 0 ? fp[ay] : -fp[-ay];
                vz = az >= 0 ? fp[az] : -fp[-az];
            } else {
                fp = &piece->pos.x;
                px = *fp++;
                py = *fp++;
                pz = *fp;
                fp = &piece->spd.x;
                vx = *fp++;
                vy = *fp++;
                vz = *fp;
                if (flags & DEBRIS_FIELD) {
                    field_sample_at(&piece->pos, &field);
                    if (field.flags & FIELD_SPD) {
                        /* @hack spelling: each sum written out, where +=
                         * would do */
                        vx = vx + ((field.spd.x - vx) * FIELD_PULL);
                        vy = vy + ((field.spd.y - vy) * FIELD_PULL);
                        vz = vz + ((field.spd.z - vz) * FIELD_PULL);
                    }
                }
            }
            px += vx;
            py += vy;
            pz += vz;
            if (piece->floor >= py) {
                vx *= debris_friction;
                vz *= debris_friction;
                if (flags & (DEBRIS_THROUGH_FLOOR | DEBRIS_WEIGHTLESS)) {
                    vy *= debris_friction;
                } else {
                    py = piece->floor;
                    if (njAbs(vy) < REST_SPEED) {
                        vy = 0.0F;
                        vx *= BOUNCE;
                        vz *= BOUNCE;
                        piece->spin.x = 0.0F;
                        piece->spin.y = 0.0F;
                        piece->spin.z = 0.0F;
                        if (piece->scale < REST_SHRINK_STEP) {
                            piece->scale = 0.0F;
                        } else {
                            piece->scale -= REST_SHRINK_STEP;
                        }
                    } else {
                        vy *= -BOUNCE;
                        /* @hack spelling: each product written out, where
                         * *= would do */
                        piece->spin.x = piece->spin.x * BOUNCE;
                        piece->spin.y = piece->spin.y * BOUNCE;
                        piece->spin.z = piece->spin.z * BOUNCE;
                    }
                }
                if (flags & DEBRIS_LANDING_BURST) {
                    field.spd.x = 0.0F;
                    field.spd.y = 0.0F;
                    field.spd.z = 0.0F;
                    piece->flags &= ~DEBRIS_LANDING_BURST;
                    burst_create(&piece->pos, &field.spd, PARTICLE_SCALE(piece),
                                 piece->model->r + 1.0F);
                }
                if (flags & DEBRIS_VANISH) {
                    piece->scale = 0.0F;
                }
            } else {
                if ((flags & DEBRIS_TRAIL) &&
                    (rand() * RAND_SCALE) < TRAIL_CHANCE) {
                    field.spd.x = piece->spd.x * TRAIL_SPEED;
                    field.spd.y = piece->spd.y * TRAIL_SPEED;
                    field.spd.z = piece->spd.z * TRAIL_SPEED;
                    if (mapped) {
                        dust_steady_gravity_create(&piece->pos, &field.spd,
                                                   PARTICLE_SCALE(piece),
                                                   piece->model->r + 1.0F);
                    } else {
                        dust_steady_create(&piece->pos, &field.spd,
                                           PARTICLE_SCALE(piece),
                                           piece->model->r + 1.0F);
                    }
                }
                vx *= debris_friction;
                if (flags & DEBRIS_WEIGHTLESS) {
                    vy *= debris_friction;
                } else {
                    vy = (vy * debris_friction) - debris_gravity;
                }
                vz *= debris_friction;
            }
            if (mapped) {
                fp = &piece->pos.x;
                if (ax >= 0) {
                    fp[ax] = px;
                } else {
                    fp[-ax] = -px;
                }
                if (ay >= 0) {
                    fp[ay] = py;
                } else {
                    fp[-ay] = -py;
                }
                if (az >= 0) {
                    fp[az] = pz;
                } else {
                    fp[-az] = -pz;
                }
                fp = &piece->spd.x;
                if (ax >= 0) {
                    fp[ax] = vx;
                } else {
                    fp[-ax] = -vx;
                }
                if (ay >= 0) {
                    fp[ay] = vy;
                } else {
                    fp[-ay] = -vy;
                }
                if (az >= 0) {
                    fp[az] = vz;
                } else {
                    fp[-az] = -vz;
                }
            } else {
                fp = &piece->pos.x;
                *fp = px;
                fp++;
                *fp = py;
                fp++;
                *fp = pz;
                fp = &piece->spd.x;
                *fp = vx;
                fp++;
                *fp = vy;
                fp++;
                *fp = vz;
            }
            piece->ang.x += (Angle)(piece->spin.x * ANGLE_PER_DEGREE);
            piece->ang.y += (Angle)(piece->spin.y * ANGLE_PER_DEGREE);
            piece->ang.z += (Angle)(piece->spin.z * ANGLE_PER_DEGREE);
            /* @hack spelling: each product written out, where *= would do */
            piece->spin.x = piece->spin.x * debris_friction;
            piece->spin.y = piece->spin.y * debris_friction;
            piece->spin.z = piece->spin.z * debris_friction;
            piece->life--;
        }
        if (piece->scale <= 0.0F) {
            next = piece->next;
            if (prev != NULL) {
                prev->next = next;
            } else {
                debris_list = next;
            }
            syFree(piece);
            piece = next;
        } else {
            if (piece->life < SHRINK_LIFE) {
                if (piece->scale < SHRINK_STEP) {
                    piece->scale = 0.0F;
                } else {
                    piece->scale -= SHRINK_STEP;
                }
            }
            prev = piece;
            piece = piece->next;
        }
    }
    task->work->timer = count;
    if (debris_list == NULL) {
        task_destroy(task);
    }
}

static void debris_draw(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct task_work *work;
    struct debris *piece;
    int alpha;
    /* @hack type: color an int, where the Uint32 that model_draw_tinted
     * takes would do */
    int color;

    work = task->work;
    piece = debris_list;
    njPushMatrixEx();
    while (piece != NULL) {
        njSetTexture(piece->texlist);
        /* @hack redundant: the test of DEBRIS_GRAVITY, whose two arms are
         * the same */
        if (piece->flags & DEBRIS_GRAVITY) {
            njTranslateV(NULL, &piece->pos);
        } else {
            njTranslateV(NULL, &piece->pos);
        }
        njRotateEx(&piece->ang.x, 0);
        /* below full scale, a piece draws in Ninja's constant material, white
         * with its scale for alpha. each strip of its model draws with alpha
         * and without specular, whatever flags the strip holds */
        if (piece->scale < 1.0F) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
            constant_material_set(piece->scale, 1.0F, 1.0F, 1.0F);
        }
        njTranslate(NULL, -piece->model->center.x, -piece->model->center.y,
                    -piece->model->center.z);
        if (piece->flags & DEBRIS_LIGHT) {
            light_apply(LIGHT_DEBRIS);
        }
        if ((piece->flags & DEBRIS_FADE_COLORS) && piece->scale < 1.0F) {
            alpha = (int)(piece->scale * ALPHA_MAX);
            if (alpha > ALPHA_MAX) {
                alpha = ALPHA_MAX;
            }
            if (alpha < 0) {
                alpha = 0;
            }
            color = (alpha << ALPHA_SHIFT) + WHITE;
            model_draw_tinted(piece->model, color);
        } else if (piece->scale >= 1.0F && piece->build != NULL) {
            njDirectDrawObject(piece->build);
        } else {
            if (piece->flags & DEBRIS_SIMPLE) {
                njCnkSimpleDrawModel(piece->model);
            } else {
                njCnkEasyDrawModel(piece->model);
            }
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        }
        if (piece->flags & DEBRIS_LIGHT) {
            light_apply(light_after_draw);
        }
        if (piece->scale < 1.0F) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        matrix_load_parent();
        piece = piece->next;
    }
    njPopMatrixEx();
}
