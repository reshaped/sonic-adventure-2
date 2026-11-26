/* the sea of Metal Harbor, Weapons Bed, Weapons Bed's two-player deck and the
 * first walker duel. stg10/sea.c compiles it again for Metal Harbor, with
 * sea_waves_clear */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "effect/particle.h"
#include "shared/sea.h"
#include "system/areas.h"
#include "system/game_state.h"

/* a wave goes once every vertex within SEA_REACH of its center is past its
 * life */
#define SEA_REACH 6500.0F
/* the sea's texture runs through SEA_FRAMES frames, SEA_FRAME_STEPS steps
 * each */
#define SEA_FRAMES 20
#define SEA_FRAME_STEPS 2
/* the color the waves' swing blends the sea toward. the blend is CREST_SCALE
 * times the swings' sum at the vertex, from 0 up to 1 */
#define CREST_COLOR 0xd0ffffff
#define CREST_SCALE 0.5F
/* the words of a vertex's point, ahead of its color */
#define POINT_WORDS 3
/* @hack type: a vertex's word, copied to the buffer, read back as a Float,
 * where the vertex's own field read as one would do */
#define WORD_FLOAT(word) (*(Float *)&(word))

/* the waves in use are the first sea_wave_count */
static struct sea_wave sea_waves[SEA_WAVES];

static Float dat_sea_0 = 400.0F;
/* the steps a wave's ring takes to spread by one unit */
static Float sea_wave_lag = 0.07F;
static Sint32 dat_sea_1 = 768;
/* a wave's swing is divided by 1 plus sea_wave_falloff times the vertex's
 * distance from the center */
static Float sea_wave_falloff = 0.005F;
/* the steps the sea's colors take to scroll one row */
static Sint32 sea_color_row_steps = 50;
Sint32 sea_wave_count = 0;
Float sea_wave_height_max = 800.0F;

void sea_waves_step(Float dx, Float dy)
{
    struct sea_wave *src = sea_waves;
    struct sea_wave *dst = sea_waves;
    int limit = sea_wave_lag * SEA_REACH;
    int count = sea_wave_count;
    int i;

#ifdef NONMATCHING
    /* this loop steps dst only past each wave it keeps. SHC's below steps dst
     * back before sea_waves where a wave goes before any is kept. C leaves that
     * pointer undefined */
    for (i = 0; i < count; i++, src++) {
        if (src != dst) {
            *dst = *src;
        }
        dst->age++;
        if (limit < dst->age - dst->life) {
            sea_wave_count--;
        } else {
            dst->pos.x += dx;
            dst->pos.y += dy;
            dst++;
        }
    }
#else
    /* @hack spelling: dst stepped back where a wave goes and on by the loop,
     * where a dst stepped only past each kept wave would do */
    for (i = 0; i < count; i++, src++, dst++) {
        if (src != dst) {
            *dst = *src;
        }
        dst->age++;
        if (limit < dst->age - dst->life) {
            dst--;
            sea_wave_count--;
        } else {
            dst->pos.x += dx;
            dst->pos.y += dy;
        }
    }
#endif
}

struct sea_wave *sea_wave_add(void)
{
    if (sea_wave_count < SEA_WAVES) {
        sea_wave_count++;
        sea_waves[sea_wave_count - 1].age = 0;
        return &sea_waves[sea_wave_count - 1];
    }
    return NULL;
}

#ifdef METAL_HARBOR_SEA
void sea_waves_clear(void)
{
    sea_wave_count = 0;
}
#endif

void sea_draw(void)
{
    Sint32 word;
    int count = sea_wave_count;
    /* @hack spelling: waves, a local for sea_waves, where sea_waves read in
     * place would do */
    struct sea_wave *waves = sea_waves;
    Sint32 *buffer = (Sint32 *)WORK_AREA;
    Sint32 *vlist = sea_object->model->vlist;
    Sint32 *src = vlist;
    Sint32 *dst = buffer;
    struct sea_wave *wave;
    NJS_CNK_MODEL *model;
    Sint16 *plist;
    int nverts;
    int side;
    int row;
    int row0;
    int row1;
    int i;
    int r0;
    int r1;
    int j;
    int k;
    int age;
    Angle angle;
    int tile;
    Uint32 color;
    Float frac;
    Float x;
    Float y;
    Float z;
    Float dx;
    Float dz;
    Float dist;
    Float fade;
    Float swing;
    Float sum;

    /* @hack redundant: the chunk's first word kept in word, which nothing
     * reads before the next store */
    word = *dst++ = *src++;
    word = *dst++ = *src++;
    nverts = (Uint16)(word >> CHUNK_HIGH_SHIFT);
    side = njSqrt(nverts);
    row = game_state.steps % (side * sea_color_row_steps);
    frac = (Float)(row % sea_color_row_steps) / sea_color_row_steps;
    row /= sea_color_row_steps;
    vlist += CHUNK_HEAD_WORDS + POINT_WORDS;
    /* @hack spelling: row0 taken from row1 as row1 steps, where row0 = row
     * and row1 = row + 1 would do */
    row1 = row;
    row0 = row1++;
    for (i = 0; i < side; i++) {
        r0 = row0 + i;
        if (r0 >= side) {
            r0 -= side;
        }
        r0 *= side;
        r1 = row1 + i;
        if (r1 >= side) {
            r1 -= side;
        }
        r1 *= side;
        /* @hack spelling: j = j + 1, where j++ would do */
        for (j = 0; j < side; j = j + 1) {
            word = *dst++ = *src++;
            x = WORD_FLOAT(word);
            word = *dst++ = *src++;
            y = WORD_FLOAT(word);
            word = *dst++ = *src++;
            z = WORD_FLOAT(word);
            sum = 0.0F;
            wave = waves;
            for (k = 0; k < count; k++, wave++) {
                dx = x - wave->pos.x;
                dz = z - wave->pos.y;
                dist = njSqrt((dx * dx) + (dz * dz));
                /* @hack spelling: the ring's lag kept in age before the
                 * subtraction, where one expression would do */
                age = (int)(dist * sea_wave_lag);
                age = wave->age - age;
                /* @hack spelling: the rate kept in angle before the product,
                 * where one expression would do */
                angle = wave->rate;
                angle *= age;
                if (age < 0) {
                    fade = 0.0F;
                } else if (age < wave->life) {
                    fade = 1.0F - ((Float)age / wave->life);
                } else {
                    fade = 0.0F;
                }
                if (fade != 0.0F) {
                    swing = njSin(angle) * fade /
                            (1.0F + (dist * sea_wave_falloff));
                    sum += swing;
                    swing *= wave->pos.z;
                    if (swing > sea_wave_height_max) {
                        swing = sea_wave_height_max;
                    }
                    y = swing;
                }
            }
            /* dst stands past the vertex's z, and its y lies POINT_WORDS - 1
             * words back */
            ((Float *)dst)[1 - POINT_WORDS] = y;
            sum *= CREST_SCALE;
            if (sum > 1.0F) {
                sum = 1.0F;
            }
            if (sum < 0.0F) {
                sum = 0.0F;
            }
            src++; /* past the model's color, which the blend replaces */
            color = color_blend(vlist[(r0 + j) * CHUNK_D8_WORDS],
                                vlist[(r1 + j) * CHUNK_D8_WORDS], frac);
            color = color_blend(color, CREST_COLOR, sum);
            *dst++ = color;
        }
    }
    /* @hack redundant: a test of count no path reaches */
    if (0) {
        if (count < 0 || count > 0) {
        }
    }
    *dst = *src;
    model = sea_object->model;
    vlist = model->vlist;
    model->vlist = buffer;
    njSetTexture(&sea_texlist);
    /* @hack spelling: plist set, then stepped to the texture's halfword,
     * where &model->plist[1] would do */
    plist = model->plist;
    plist++;
    /* @hack spelling: tile loaded in a statement of its own, where one
     * expression would do */
    tile = *plist;
    tile &= NJD_FFM_MASK;
    tile |= (game_state.steps / SEA_FRAME_STEPS) % SEA_FRAMES;
    *plist = tile;
    njCnkSimpleDrawModel(model);
    model->vlist = vlist;
    /* @hack redundant: a test of model no path reaches */
    if (0) {
        if (model) {
        }
    }
}
