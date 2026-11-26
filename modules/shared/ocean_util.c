#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "draw/chunk.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/ocean_util.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the ranges of ocean_dust_behind's random turns about y and about x, in
 * degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* the words ocean_model_draw_tinted_stack's copy has room for */
#define STACK_COPY_WORDS 0x300
/* the words vlist_copy_tinted asks for past a chunk's size. the copy takes two
 * of them, for the chunk's head and the NJD_CE after the chunk. the size leaves
 * out the head */
#define COPY_SPARE_WORDS 4

#ifndef HIDDEN_BASE_UTIL
/* the camera ocean_camera_shake_at shakes, the player it measures from and
 * whose pad it vibrates */
#define SHAKE_CAMERA 0
#define SHAKE_PLAYER 0
/* past SHAKE_NEAR, the shake's size is divided by the distance. past
 * VIBRATION_NEAR, the vibration's power is divided by VIBRATION_FALLOFF times
 * the distance */
#define SHAKE_NEAR 1.0F
#define VIBRATION_NEAR 10.0F
#define VIBRATION_FALLOFF 0.1F

/* the vibration ocean_camera_shake_at starts, at its full power */
static Sint32 vibration_power = 18;
static Sint32 vibration_freq = 20;
static Sint32 vibration_time = 1;
#endif

void ocean_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void ocean_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                       Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
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

#ifdef HIDDEN_BASE_UTIL
#include "shared/ocean_simple_scaled_impl.h"
#endif

void ocean_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
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

void ocean_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
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

#ifndef HIDDEN_BASE_UTIL
#include "shared/ocean_simple_scaled_impl.h" /* IWYU pragma: keep -- the code it compiles here */
#endif

#ifdef HIDDEN_BASE_UTIL
void base_model_draw_float_tint(NJS_CNK_MODEL *model, Float color, Float scale)
#else
void ocean_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                    Float scale)
#endif
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

void ocean_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
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

void ocean_turn_xz(Float x, Float z, Angle ang, Float *turned_x,
                   Float *turned_z)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    /* @hack spelling: z negated in place, and each product's sign turned to
     * suit, where (x * cosine) + (z * sine) and (z * cosine) - (x * sine)
     * would do */
    z = -z;
    *turned_x = (x * cosine) - (z * sine);
    *turned_z = -(x * sine) - (z * cosine);
}

#ifdef HIDDEN_BASE_UTIL
#include "stg23/base_util_impl.h"
#endif

/* copy src's first chunk and the word after it to dst, which has room for limit
 * words. the copy's vertex colors are tinted by color. the call never returns
 * where the chunk's size passes limit - COPY_SPARE_WORDS. nor does it where
 * NJD_CE doesn't follow the chunk */
static void vlist_copy_tinted(Sint32 *dst, Sint32 *src, Uint32 color, int limit)
{
    Sint32 word;
    int size;
    int count;

    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    size = (Uint16)(word >> CHUNK_HIGH_SHIFT);
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    count = (Uint16)(word >> CHUNK_HIGH_SHIFT);
    /* NOLINTNEXTLINE(bugprone-infinite-loop) */
    while (limit < size + COPY_SPARE_WORDS) {
    }
    /* @hack type: dst and src stepped word by word, where
     * struct chunk_vertex_d8 pointers stepped from vertex to vertex would do */
    while (count--) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = color_multiply(*src++, color);
    }
    word = *src;
    *dst = word;
    /* NOLINTNEXTLINE(bugprone-infinite-loop) */
    while (word != NJD_CE) {
    }
}

void ocean_model_draw_tinted_stack(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 copy[STACK_COPY_WORDS];
    Sint32 *vlist;

    vlist = model->vlist;
    vlist_copy_tinted(copy, vlist, color, STACK_COPY_WORDS);
    model->vlist = copy;
    njCnkEasyDrawModel(model);
    model->vlist = vlist;
}

void ocean_model_tint(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 *word;
    /* @hack spelling: value, a local for the chunk's second word and for the
     * word after the chunk, where each word read in place would do */
    Sint32 value;
    int count;

    word = model->vlist + 1;
    value = *word++;
    count = (Uint16)(value >> CHUNK_HIGH_SHIFT);
    /* @hack type: word stepped to each vertex's color, where a
     * struct chunk_vertex_d8 pointer stepped from vertex to vertex would do */
    while (count--) {
        word += CHUNK_V_WORDS;
        *word = color_multiply(*word, color);
        word++;
    }
    value = *word;
    if (value != NJD_CE) {
        trapa(0);
    }
}

#ifndef HIDDEN_BASE_UTIL
void ocean_camera_shake_at(NJS_POINT3 *pos, Float size, int steps)
{
    Float distance;
    int power;

    distance = njDistanceP2P(pos, &player_work[SHAKE_PLAYER]->pos);
    power = vibration_power;
    if (distance > SHAKE_NEAR) {
        size /= distance;
    }
    if (distance > VIBRATION_NEAR) {
        power = power / (distance * VIBRATION_FALLOFF);
    }
    /* @hack spelling: the shake in each branch, where one test with || would
     * do */
    if (camera_stacks[SHAKE_CAMERA].shake_steps > 0) {
        if (size >= camera_stacks[SHAKE_CAMERA].shake_size) {
            camera_shake(SHAKE_CAMERA, steps, size);
            vibration_start(SHAKE_PLAYER, power, vibration_freq,
                            vibration_time);
        }
    } else {
        camera_shake(SHAKE_CAMERA, steps, size);
        vibration_start(SHAKE_PLAYER, power, vibration_freq, vibration_time);
    }
}
#endif
