/* Observable outputs of the three clipping callbacks in njDirectPointLoops.src.
 * The matrix and inputs give exact finite SH-4 results in every test case. */
#include <ninja.h>
#include <nj_direct_draw.h>
#ifdef DREAMCAST
#include "draw_fixture.h"
#else
#include "fpu.h"
#endif

enum format { PLAIN, ENVIRONMENT, D8 };
enum { VERTICES = 6, WINDOWS = VERTICES - 2 };
static NJS_DIRECT_VERTEX lit[VERTICES];
static NJS_DIRECT_VERTEX_D8 packed[VERTICES];
static struct {
    Float before;
    NJS_POINT3 point;
    Float after;
} scratch;

struct event {
    enum format format;
    const void *source;
    Uint32 mask;
    Float first[4];
    Float second[3];
    NJS_POINT3 third;
};
static struct event events[WINDOWS];
static int calls;

/* Indirection retains the actual entry points and their ABI at terminal LTO. */
static void (*volatile lit_loop)(NJS_DIRECT_VERTEX *, Int, NJS_POINT3 *,
                                 Int) = njDirectCalcPointLoop;
static void (*volatile d8_loop)(NJS_DIRECT_VERTEX_D8 *, Int,
                                NJS_POINT3 *) = njDirectCalcPointLoopD8;

static void capture(enum format format, const void *source, Uint32 mask,
                    Float x, Float y, Float z, Float w, Float u, Float v,
                    Float t)
{
    int index = calls++;
    if (index < WINDOWS) {
        struct event *event = &events[index];
        event->format = format;
        event->source = source;
        event->mask = mask;
        event->first[0] = x;
        event->first[1] = y;
        event->first[2] = z;
        event->first[3] = w;
        event->second[0] = u;
        event->second[1] = v;
        event->second[2] = t;
        event->third = scratch.point;
    }
}

void njDirectNearClipSub(NJS_DIRECT_VERTEX *source, Uint32 mask, Float x,
                         Float y, Float z, Float w, Float u, Float v, Float t)
{
    capture(PLAIN, source, mask, x, y, z, w, u, v, t);
}

void njDirectEnvNearClipSub(NJS_DIRECT_VERTEX *source, Uint32 mask, Float x,
                            Float y, Float z, Float w, Float u, Float v,
                            Float t)
{
    capture(ENVIRONMENT, source, mask, x, y, z, w, u, v, t);
}

void njDirectNearClipSubD8(NJS_DIRECT_VERTEX_D8 *source, Uint32 mask, Float x,
                           Float y, Float z, Float w, Float u, Float v, Float t)
{
    capture(D8, source, mask, x, y, z, w, u, v, t);
}

static void reset(void)
{
    calls = 0;
    scratch.before = 739.0F;
    scratch.point.x = -49.0F;
    scratch.point.y = -50.0F;
    scratch.point.z = -51.0F;
    scratch.after = 947.0F;
}

static int intact(void)
{
    return scratch.before == 739.0F && scratch.after == 947.0F;
}

static void vertex(int i, Float z, Uint32 param)
{
    lit[i].px = packed[i].px = (Float)(i + 1);
    lit[i].py = packed[i].py = (Float)(10 + i);
    lit[i].pz = packed[i].pz = z;
    lit[i].param = packed[i].param = param;
    /* The lit normal never supplies w. D8 takes it from the record. */
    lit[i].nz = 55.0F;
    packed[i].one = (Float)(i + 1);
    packed[i].uv = 99;
}

static int matches(int index, enum format format, int first, Uint32 mask)
{
    const struct event *e = &events[index];
    NJS_POINT3 expected[3];
    Float first_w = 0;
    int i;

    for (i = 0; i < 3; ++i) {
        int n = first + i;
        Float w = format == D8 ? packed[n].one : 1.0F;
        expected[i].x = lit[n].px + 8.0F * w;
        expected[i].y = 2.0F * lit[n].py;
        expected[i].z = lit[n].pz;
        if (i == 0) {
            first_w = lit[n].px + w;
        }
    }
    return e->format == format && e->mask == mask &&
           e->source ==
               (format == D8 ? (void *)&packed[first] : (void *)&lit[first]) &&
           e->first[0] == expected[0].x && e->first[1] == expected[0].y &&
           e->first[2] == expected[0].z && e->first[3] == first_w &&
           e->second[0] == expected[1].x && e->second[1] == expected[1].y &&
           e->second[2] == expected[1].z && e->third.x == expected[2].x &&
           e->third.y == expected[2].y && e->third.z == expected[2].z;
}

static void draw(enum format format, int mode, int count)
{
    if (format == D8) {
        d8_loop(packed, count, &scratch.point);
    } else {
        lit_loop(lit, count, &scratch.point, mode);
    }
}

static int triangles(enum format format, int mode)
{
    Uint32 mask;
    int i;
    for (mask = 0; mask < 8; ++mask) {
        for (i = 0; i < 3; ++i) {
            /* Exactly on the plane, or behind it. The assembly uses FCMP/GT. */
            vertex(i, (mask & (4U >> i)) ? 0.5F : 1.0F, 0xe0000000U);
        }
        reset();
        draw(format, mode, 3);
        if (!intact()) {
            return 1;
        }
        if (mask == 0 || mask == 7) {
            if (calls != 0 || scratch.point.x != -49.0F ||
                scratch.point.y != -50.0F || scratch.point.z != -51.0F) {
                return 2;
            }
        } else if (calls != 1 || !matches(0, format, 0, mask)) {
            return 3;
        }
    }
    /* Windows 1 and 2 straddle the end of the first strip. The third
     * vertex's flag does not suppress windows 0 and 3. */
    for (i = 0; i < VERTICES; ++i) {
        vertex(i, (i == 0 || i == 3 || i == 5) ? 0.0F : 2.0F,
               i == 2 || i == 5 ? 0xf0000000U : 0xe0000000U);
    }
    reset();
    draw(format, mode, VERTICES);
    if (!intact() || calls != 2 || !matches(0, format, 0, 4) ||
        !matches(1, format, 3, 5)) {
        return 4;
    }
    return 0;
}

static void load_matrix(void)
{
    static const float matrix[16] __attribute__((aligned(8))) = {
        1, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 8, 0, 0, 1};
#ifdef DREAMCAST
    load_test_xmtrx(matrix);
#else
    int i;
    for (i = 0; i < 16; ++i) {
        dc_xmtrx[i] = matrix[i];
    }
#endif
}

int main(void)
{
    static const int modes[] = {-1, 0, 1, 2};
    int i, error;
    load_matrix();
    for (i = 0; i < 4; ++i) {
        error = triangles(modes[i] > 0 ? ENVIRONMENT : PLAIN, modes[i]);
        if (error) {
            return 10 * (i + 1) + error;
        }
    }
    error = triangles(D8, 0);
    if (error) {
        return 50 + error;
    }
#ifndef RETAIL_REFERENCE
    /* Only the replacement accepts short strips. Null record and output
     * pointers make any read or write observable, including at final LTO. */
    reset();
    for (i = -1; i <= 2; ++i) {
        lit_loop(0, i, 0, 0);
        lit_loop(0, i, 0, 1);
        d8_loop(0, i, 0);
    }
    if (calls) {
        return 60;
    }
#endif
    return 0;
}
