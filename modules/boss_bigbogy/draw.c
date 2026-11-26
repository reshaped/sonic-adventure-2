#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "boss_bigbogy/draw.h"
#include "camera/stack.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/sandglass.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

/* the dust's random turns, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F
/* half the screen's size, in pixels */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
/* the depth view_to_screen gives a point at depth 0, so that its division
 * stays finite */
#define NEAR_Z 0.001F

Bool sandglass_blow_strikes(struct task *player)
{
    int slot = player_slot_of(player);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (record->character != PLAYER_KNUCKLES) {
        return FALSE;
    }
    return work->mode == PLAYER_MODE_HUNTER_GLIDE ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_SPIRAL ||
           work->mode == PLAYER_MODE_HUNTER_DIVE;
}

Bool sandglass_blow_stops(struct task *player)
{
    int slot = player_slot_of(player);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (record->character != PLAYER_KNUCKLES &&
        record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    return work->mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3;
}

void sandglass_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void bigbogy_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                         Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    /* @hack redundant: the array unused, which only sizes the frame */
    Uint8 unused[12];
    struct angles ang;

    (void)unused;
    offset.x = 0.0F;
    offset.y = 0.0F;
    offset.z = radius;
    njPushMatrixEx();
    njRotateY(NULL, (Angle)((((Float)rand() * (TURN_Y_SPREAD * RAND_SCALE)) +
                             TURN_Y_MIN) *
                            ANGLE_PER_DEGREE));
    njRotateX(NULL, (Angle)((Float)rand() *
                            (TILT_MAX * ANGLE_PER_DEGREE * RAND_SCALE)));
    current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateXYZ(NULL, ang.x, ang.y, ang.z);
    njCalcPoint(NULL, &offset, &point);
    dust_create(&point, spd, scale);
    njPopMatrixEx();
}

void bigbogy_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r itself
     * would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void bigbogy_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r itself
     * would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void bigbogy_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                      Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r itself
     * would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void bigbogy_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                        NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r itself
     * would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

static void view_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    Float view = camera_view_angle(0);
    Float distance = SCREEN_HALF_WIDTH / njTan((Angle)(view * 0.5F));

    if (point->z == 0.0F) {
        point->z = NEAR_Z;
    }
    /* @hack spelling: x over -z, negated, where x over z would do */
    *sx = -(point->x / -point->z * distance);
    *sy = point->y / -point->z * distance;
}

#pragma inline(point_to_view)
static void point_to_view(NJS_POINT3 *from, NJS_POINT3 *to)
{
    njCalcPoint(NULL, from, to);
}

#pragma inline(bigbogy_point_to_screen)
void bigbogy_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    view_to_screen(&view, sx, sy);
}

Bool bigbogy_on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;

    bigbogy_point_to_screen(point, &sx, &sy);
    if (njAbs(sx) < SCREEN_HALF_WIDTH && njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Angle sandglass_angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

void sandglass_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                                   Float green, Float blue)
{
    draw_control_save();
    draw_attr_save();
    if (alpha != 1.0F) {
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    } else {
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    }
    constant_material_set(alpha, red, green, blue);
    njCnkSimpleDrawModel(model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}
