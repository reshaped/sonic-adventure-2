/* helpers Wild Canyon's objects share */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* port_view_aspect, the window this side's test of the screen takes */
#include "engine/dc/port.h"
#endif

/* the ranges of canyon_dust_behind's random turns about y and about x, in
 * degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* half a 640 by 480 screen's width and height */
#define SCREEN_HALF_WIDTH 320.0F
#define SCREEN_HALF_HEIGHT 240.0F
/* view_to_screen divides by a point's z, and sets a z of 0 to NEAR_Z first */
#define NEAR_Z 0.001F

/* the box in_view_box tests: VIEW_NEAR and VIEW_FAR bound z, and the rest
 * bound the screen's x and y */
#define VIEW_NEAR 15.0F
#define VIEW_FAR 450.0F
#define VIEW_HALF_WIDTH 280.0F
#define VIEW_BOTTOM (-200.0F)
#define VIEW_TOP 160.0F

/* canyon_vib_slight_create's parameters, as a VibSlightExec task's work keeps
 * them */
#define VIB_PLAYER(work) ((work)->tick)
#define VIB_POWER(work) ((work)->timer)
#define VIB_FREQ(work) ((work)->ang.x)
#define VIB_INTERVAL(work) ((work)->ang.y)
#define VIB_PULSES(work) ((work)->ang.z)
/* the steps a VibSlightExec task has taken */
#define VIB_STEPS(task) ((task)->unk_3c.l)
/* task_create's level for a VibSlightExec task */
#define VIB_SLIGHT_LEVEL 2

/* no header declares point_to_screen, on_screen or angle_turn. only on_screen
 * calls point_to_screen, and nothing calls the other two. on_screen and
 * angle_turn aren't static, since SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* view_to_screen of point through the current matrix. it works on a copy, so
 * point stays as it was */
Bool point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
/* whether point lies on a 640 by 480 screen of camera 0's view. take x, y, z
 * and d as in_view_box does, with a z of 0 taken as NEAR_Z. it returns TRUE
 * where z is above 0, x * d / z lies in (-320, 320) and -y * d / z in
 * (-240, 240) */
Bool on_screen(NJS_POINT3 *point);
/* the shorter turn from one angle to another, from -0x8000 to 0x7fff */
Angle angle_turn(Angle from, Angle to);
/* NOLINTEND(misc-use-internal-linkage) */

static Bool view_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
static Bool alone_in_half(NJS_POINT3 *pos);

Bool hunter_attacking(struct task *task)
{
    int slot = player_slot_of(task);
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    int mode;

    if (record->character != PLAYER_KNUCKLES &&
        record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    mode = work->mode;
    return mode == PLAYER_MODE_HUNTER_GLIDE ||
           mode == PLAYER_MODE_HUNTER_ATTACK_1 ||
           mode == PLAYER_MODE_HUNTER_ATTACK_2 ||
           mode == PLAYER_MODE_HUNTER_ATTACK_3 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2 ||
           mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3 ||
           mode == PLAYER_MODE_HUNTER_SPIRAL || mode == PLAYER_MODE_HUNTER_DIVE;
}

void canyon_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void canyon_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                        Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    /* @hack redundant: unused, a local that only sizes the stack frame */
    Uint8 unused[12];
    struct angles ang;

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

void canyon_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void canyon_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
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

void canyon_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                     Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void canyon_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                       NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
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

/* project point onto camera 0's screen. let d be SCREEN_HALF_WIDTH over the
 * tangent of half camera 0's angle of view. *sx takes x * d / z, and *sy takes
 * -y * d / z. it returns whether z is above 0. a z of 0 becomes NEAR_Z in point
 * itself, before the division */
static Bool view_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    Float view = camera_view_angle(0);
    Float distance = SCREEN_HALF_WIDTH / njTan((Angle)(view * 0.5F));

    if (point->z == 0.0F) {
        point->z = NEAR_Z;
    }
    /* @hack spelling: x over -z, negated, where x over z would do */
    *sx = -(point->x / -point->z * distance);
    *sy = point->y / -point->z * distance;
    return point->z > 0.0F;
}

#pragma inline(point_to_view)
static void point_to_view(NJS_POINT3 *from, NJS_POINT3 *to)
{
    njCalcPoint(NULL, from, to);
}

#pragma inline(point_to_screen)
Bool point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy)
{
    NJS_POINT3 view;

    point_to_view(point, &view);
    return view_to_screen(&view, sx, sy);
}

Bool on_screen(NJS_POINT3 *point)
{
    Float sx;
    Float sy;
    /* @hack spelling: front, a local for the call's result, where the call
     * tested in place would do */
    Bool front;

    front = point_to_screen(point, &sx, &sy);
    if (front && njAbs(sx) < SCREEN_HALF_WIDTH &&
        njAbs(sy) < SCREEN_HALF_HEIGHT) {
        return TRUE;
    }
    return FALSE;
}

Bool in_view_box(NJS_POINT3 *point)
{
    Float sx;
    Float sy;
    NJS_POINT3 view;
    /* @hack spelling: front, a local for the call's result, where the call
     * tested in place would do */
    Bool front;

    point_to_view(point, &view);
    front = view_to_screen(&view, &sx, &sy);
#ifdef NONMATCHING
    /* this side takes the port's window, which 16:9 widens outside a demo
     * (engine/dc/port.h) */
    sx *= port_view_aspect;
#endif
    if (front && VIEW_NEAR < view.z && view.z < VIEW_FAR &&
        njAbs(sx) < VIEW_HALF_WIDTH && VIEW_BOTTOM <= sy && sy < VIEW_TOP) {
        return TRUE;
    }
    return FALSE;
}

Angle angle_turn(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

static Bool alone_in_half(NJS_POINT3 *pos)
{
    if ((player_work[0]->pos.y < HALF_LINE && pos->y < HALF_LINE) ||
        (player_work[0]->pos.y >= HALF_LINE && pos->y >= HALF_LINE)) {
        return FALSE;
    }
    if (game_state.two_players != 0) {
        if ((player_work[1]->pos.y < HALF_LINE && pos->y < HALF_LINE) ||
            (player_work[1]->pos.y >= HALF_LINE && pos->y >= HALF_LINE)) {
            return FALSE;
        }
    }
    return TRUE;
}

Bool half_try_leave(struct task *task)
{
    struct task_work *work = task->work;

    if (!alone_in_half(&work->pos)) {
        return FALSE;
    }
    task->step = task_destroy;
    return TRUE;
}

static void canyon_vib_slight_step(struct task *task)
{
    if (VIB_STEPS(task) % VIB_INTERVAL(task->work) == 0) {
        vibration_start(VIB_PLAYER(task->work), VIB_POWER(task->work),
                        VIB_FREQ(task->work), VIBRATION_UNTIMED);
    }
    /* @hack spelling: the increment inside the test, where a statement of its
     * own would do */
    if (++VIB_STEPS(task) / VIB_INTERVAL(task->work) ==
        VIB_PULSES(task->work)) {
        task_kill(task);
    }
}

void canyon_vib_slight_create(int player, int power, int freq, int interval,
                              int count)
{
    struct task *task;

    task = task_create(TASK_WORK, VIB_SLIGHT_LEVEL, canyon_vib_slight_step,
                       "VibSlightExec");
    if (task != NULL) {
        VIB_PLAYER(task->work) = player;
        VIB_POWER(task->work) = power;
        VIB_FREQ(task->work) = freq;
        VIB_PULSES(task->work) = count;
        /* @hack redundant: the steps set to 0, which task_create gave them
         * already */
        VIB_STEPS(task) = 0;
        VIB_INTERVAL(task->work) = interval;
        /* @hack spelling: the work's interval read back for the test, where
         * interval tested would do */
        if (VIB_INTERVAL(task->work) == 0) {
            VIB_INTERVAL(task->work) = 1;
        }
    }
}
