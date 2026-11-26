/* draw_streams, for the curved conduits of Eggman's and Tails's routes in
 * Cannon's Core. stg35/core_eggman.c and stg36/core_tails.c include it, each
 * after defining draw_shape_color. that function draws a stream's shape
 * where camera 0's land_block_mask has a bit of its blocks */
#ifndef SHARED_CORE_ENERGY_STREAMS_H
#define SHARED_CORE_ENERGY_STREAMS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/color.h"
#include "shared/common_object.h"
#include "shared/core_conduit.h"
#include "shared/core_energy_art.h"
#include "shared/stage_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "shared/core_machine_lights.h"
#include "system/task.h"

/* a stream's color runs through the palette's cycle of PALETTE_CYCLE_DEGREES,
 * PALETTE_DEGREES to an entry */
#define PALETTE_CYCLE_DEGREES 360
#define PALETTE_DEGREES 4
#define ENTRIES_PER_DEGREE (1.0F / PALETTE_DEGREES)
/* a conduit is MAX_SEGMENTS segments at most, each SEGMENT_DEGREES of its
 * arc */
#define SEGMENT_DEGREES 10
#define SEGMENT_TURN ((Angle)(SEGMENT_DEGREES * ANGLE_PER_DEGREE))
#define MAX_SEGMENTS 18
/* a stream's intensity runs from STREAM_LEAST to STREAM_LEAST plus
 * STREAM_SWING and back as it crosses a segment */
#define STREAM_LEAST 0.5F
#define STREAM_SWING 0.5F
/* each channel of a segment's body lights to core_segment_base_light plus
 * SEGMENT_GAIN times its streams' light. 1 is the most it lights to */
#define SEGMENT_GAIN 2.0F
/* an 8-bit channel's top, and one step of it as a Float from 0 to 1 */
#define CHANNEL_MAX 255.0F
#define COLOR_UNIT (1.0F / CHANNEL_MAX)
/* an ARGB color's 8-bit channel at shift */
#define CHANNEL(color, shift) (((color) >> (shift)) & 0xff)
/* a palette entry's 5-bit channel at shift, as a level from 0 to 124 */
#define PALETTE_LEVEL(color, shift)                                            \
    ((((color) >> (shift)) & PALETTE_CHANNEL_MASK) << PALETTE_LEVEL_SHIFT)
#define PALETTE_CHANNEL_MASK 31
#define PALETTE_LEVEL_SHIFT 2
#define PALETTE_RED 0
#define PALETTE_GREEN 5
#define PALETTE_BLUE 10

static Uint32 stream_color(int degrees, Float intensity)
{
    Uint8 alpha = intensity * CHANNEL_MAX;
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint8 next_red;
    Uint8 next_green;
    Uint8 next_blue;
    Uint8 mixed_red;
    Uint8 mixed_green;
    Uint8 mixed_blue;
    int entry;
    int next;
    Float fraction;

    degrees = (int)njAbs((Float)degrees) % PALETTE_CYCLE_DEGREES;
    entry = degrees / PALETTE_DEGREES;
    /* fraction is degrees' place from entry to the next, from 0 up to 1 */
    /* @hack spelling: degrees / PALETTE_DEGREES at each use, where entry
     * would do */
    /* NOLINTBEGIN(bugprone-integer-division) */
    fraction =
        (degrees * ENTRIES_PER_DEGREE) - (Float)(degrees / PALETTE_DEGREES);
    /* NOLINTEND(bugprone-integer-division) */
    red = PALETTE_LEVEL(core_conduit_palette[degrees / PALETTE_DEGREES],
                        PALETTE_RED) *
          intensity;
    green = PALETTE_LEVEL(core_conduit_palette[degrees / PALETTE_DEGREES],
                          PALETTE_GREEN) *
            intensity;
    blue = PALETTE_LEVEL(core_conduit_palette[degrees / PALETTE_DEGREES],
                         PALETTE_BLUE) *
           intensity;
    if (fraction == 0.0F) {
        return (alpha * (1U << ALPHA_SHIFT)) | ((Uint32)red << RED_SHIFT) |
               ((Uint32)green << GREEN_SHIFT) | blue;
    } else {
        next = entry + 1;
        /* next wraps at PALETTE_CYCLE_DEGREES, not at CORE_CONDUIT_COLORS.
         * a blend from the last entry would read past the palette. no call
         * blends, since core_stream_color_speed is PALETTE_DEGREES and
         * fraction is then 0 */
        if (next >= PALETTE_CYCLE_DEGREES) {
            next = 0;
        }
        /* @hack redundant: a test of intensity that no path reaches */
        if (0) {
            if (intensity) {
            }
        }
        next_red =
            PALETTE_LEVEL(core_conduit_palette[next], PALETTE_RED) * intensity;
        next_green = PALETTE_LEVEL(core_conduit_palette[next], PALETTE_GREEN) *
                     intensity;
        next_blue =
            PALETTE_LEVEL(core_conduit_palette[next], PALETTE_BLUE) * intensity;
        mixed_red = (int)(red * (1.0F - fraction)) + (int)(next_red * fraction);
        mixed_green =
            (int)(green * (1.0F - fraction)) + (int)(next_green * fraction);
        mixed_blue =
            (int)(blue * (1.0F - fraction)) + (int)(next_blue * fraction);
        return ((Uint32)alpha << ALPHA_SHIFT) |
               ((Uint32)mixed_red << RED_SHIFT) |
               ((Uint32)mixed_green << GREEN_SHIFT) | mixed_blue;
    }
}

/* draw a conduit of arc degrees about y, with streams streams running through
 * it. the segments turn the way arc's sign gives. a positive arc starts one
 * segment's turn from the conduit's frame, and plays the streams' motion
 * backward. arc must span a segment and streams be 1 or more, unchecked */
static void draw_streams(struct task *task, int arc, int streams, Sint32 blocks)
{
    int hits[MAX_SEGMENTS];
    Float alpha[MAX_SEGMENTS];
    Float red[MAX_SEGMENTS];
    Float green[MAX_SEGMENTS];
    Float blue[MAX_SEGMENTS];
    int frames = core_stream_motion->nbFrame;
    int i;
    int trail;
    int segments;
    int index;
    int step;
    int spacing;
    int time;
    Float frame;
    Float fraction;
    Float intensity;
    Float lit_red;
    Float lit_green;
    Float lit_blue;
    int color;

    njPushMatrixEx();
    if (arc > 0) {
        njRotateY(NULL, SEGMENT_TURN);
    }
    segments = (int)njAbs((Float)arc) / SEGMENT_DEGREES;
    if (segments > MAX_SEGMENTS) {
        segments = MAX_SEGMENTS;
    }
    if (arc < 0) {
        step = -SEGMENT_DEGREES;
    } else {
        step = SEGMENT_DEGREES;
    }
    /* @hack redundant: a read of core_stream_trails that no path reaches */
    if (0) {
        int count = core_stream_trails;
        if (count) {
        }
    }
    spacing = frames * segments / streams;
    for (i = 0; i < segments; ++i) {
        alpha[i] = 0.0F;
        red[i] = 0.0F;
        green[i] = 0.0F;
        blue[i] = 0.0F;
        hits[i] = 0;
    }
    njPushMatrixEx();
    njSetTexture(&core_stream_texlist);
    for (i = 0; i < streams; ++i) {
        for (trail = 0; trail < core_stream_trails; ++trail) {
            time = game_state.steps + (trail * core_stream_trail_step) +
                   (i * spacing);
            frame = (time % frames) * 1.0F;
            index = (time / frames) % segments;
            fraction = (Float)(time % frames) / frames;
            njPushMatrixEx();
            njRotateY(NULL, index * step * ANGLE_PER_DEGREE);
            intensity =
                (njCos((fraction * ANGLE_PER_TURN) - (Float)HALF_TURN) + 1.0F) *
                0.5F;
            intensity = STREAM_LEAST + (intensity * STREAM_SWING);
            alpha[index] += intensity;
            ++hits[index];
            color = stream_color((time * core_stream_color_speed) %
                                     PALETTE_CYCLE_DEGREES,
                                 intensity);
            red[index] += CHANNEL(color, RED_SHIFT) * COLOR_UNIT;
            green[index] += CHANNEL(color, GREEN_SHIFT) * COLOR_UNIT;
            blue[index] += CHANNEL(color, BLUE_SHIFT) * COLOR_UNIT;
            if (step > 0) {
                frame = core_stream_motion->nbFrame - 1 - frame;
            }
            draw_shape_color(core_stream_object, core_stream_motion,
                             core_stream_shape, frame, color, blocks);
            njPopMatrixEx();
        }
    }
    /* @hack redundant: a test of the glow gain, in a block no path reaches */
    if (0) {
        Float gain = core_stream_glow_gain;
        if (gain) {
        }
    }
    /* @hack redundant: a test of a color, in a block no path reaches */
    if (0) {
        if (i * COLOR_UNIT) {
        }
    }
    /* @hack redundant: tests of njSetTexture's address, in a block no path
     * reaches */
    if (0) {
        if (&njSetTexture) {
        }
        if (&njSetTexture) {
        }
    }
    for (i = 0; i < segments; ++i) {
        intensity = alpha[i] * core_stream_glow_gain;
        lit_red = red[i];
        lit_green = green[i];
        lit_blue = blue[i];
        if (hits[i]) {
            if (intensity > 1.0F) {
                intensity = 1.0F;
            }
            lit_red = (lit_red / hits[i]) * intensity;
            lit_green = (lit_green / hits[i]) * intensity;
            lit_blue = (lit_blue / hits[i]) * intensity;
#define STREAM_ARGB(a, r, g, b)                                                \
    (((Uint32)(Uint8)(int)((a) * CHANNEL_MAX) << ALPHA_SHIFT) |                \
     ((Uint32)(Uint8)(int)((r) * CHANNEL_MAX) << RED_SHIFT) |                  \
     ((Uint32)(Uint8)(int)((g) * CHANNEL_MAX) << GREEN_SHIFT) |                \
     (Uint8)(int)((b) * CHANNEL_MAX))
            color = STREAM_ARGB(intensity, lit_red, lit_green, lit_blue);
            njPushMatrixEx();
            njRotateY(NULL, i * step * ANGLE_PER_DEGREE);
            njSetTexture(&core_segment_glow_texlist);
            model_draw_tinted_additive(&core_segment_glow, color);
            njPopMatrixEx();
        }
        lit_red = core_segment_base_light + (lit_red * SEGMENT_GAIN);
        if (lit_red > 1.0F) {
            lit_red = 1.0F;
        }
        lit_green = core_segment_base_light + (lit_green * SEGMENT_GAIN);
        if (lit_green > 1.0F) {
            lit_green = 1.0F;
        }
        lit_blue = core_segment_base_light + (lit_blue * SEGMENT_GAIN);
        if (lit_blue > 1.0F) {
            lit_blue = 1.0F;
        }
        color = STREAM_ARGB(intensity, lit_red, lit_green, lit_blue);
        njPushMatrixEx();
        njRotateY(NULL, i * step * ANGLE_PER_DEGREE);
        njSetTexture(&core_segment_texlist);
        model_draw_tinted(&core_segment_body, color);
        njPopMatrixEx();
    }
#undef STREAM_ARGB
    njPopMatrixEx();
    njPopMatrixEx();
    /* @hack redundant: a test of hits that no path reaches */
    if (0) {
        /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
        if (hits) {
        }
    }
}

#endif
