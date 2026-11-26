/* engine/dc's kernels, their C models on console.c's FIPR, FTRV and FSCA,
 * against the shipped instructions they stand for. the reference here runs
 * each shipped member's FSCAs and FTRVs as its listing in sdk/ninja/ gives
 * them. the turns must give the reference's bits for any vector, an
 * infinite one too. the places must give them for a finite pos. for an
 * infinite pos or offset, place's check holds what turn.h says. each window
 * that calls a turn must give its function's turns' bits. the batch runs
 * the reference's own FTRV. so its check holds the stride logic alone.
 * test_kernels.py holds each asm to its model. the test selects round
 * toward zero and uses inputs whose arithmetic produces no denormal. it
 * fails at the first failed check */
#include <fenv.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fpu.h"
#include "turn.h"
#include "xmtrx.h"

/* the cases each check runs on random inputs, which take the edge cases now
 * and then */
#define CASES 20000
/* the NaN FIPR gives of 0 times an infinity */
#define FIPR_NAN 0x7fbfffffU

#define CHECK(test)                                                            \
    do {                                                                       \
        if (!(test)) {                                                         \
            fprintf(stderr, "%s:%d: %s, case %d: %s\n", __FILE__, __LINE__,    \
                    running, at, #test);                                       \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

/* the check under way and its case, for a failure's line */
static const char *running;
static int at;

static uint64_t seed = 0x9e3779b97f4a7c15ULL;

static uint64_t next(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

/* a float of either sign whose magnitude lies from low to high, or now and
 * then 0 */
static float any(float low, float high)
{
    float magnitude = low + ((high - low) * (float)(next() >> 40) * 0x1p-24F);

    if (next() % 16 == 0) {
        magnitude = 0.0F;
    }
    return next() & 1 ? -magnitude : magnitude;
}

/* an angle: one of the edge cases, which FSCA gives exact floats or which
 * Ninja's tests of 0 tell apart, or any 32 bits */
static int32_t any_angle(void)
{
    static const uint32_t edges[] = {
        0, 0x4000, 0x8000, 0xc000, 0x10000, 0x2000, 1, 0xffff, 0xffffffff,
    };

    if (next() % 4 == 0) {
        return (int32_t)edges[next() % (sizeof edges / sizeof edges[0])];
    }
    return (int32_t)next();
}

static int same(const float *got, const float *want, size_t count)
{
    return memcmp(got, want, count * sizeof *got) == 0;
}

static uint32_t bits_of(float value)
{
    uint32_t bits;

    memcpy(&bits, &value, sizeof bits);
    return bits;
}

static const float unit[16] = {
    1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F,
};

/* njTranslateEx, or njTranslateV's path for a null matrix: FTRV of
 * (by, 1), whose product is row 3 */
static void nj_translate(float matrix[16], const float by[3])
{
    float point[4] = {by[0], by[1], by[2], 1.0F};

    dc_ftrv(matrix, point);
    memcpy(&matrix[12], point, sizeof point);
}

enum axis { X, Y, Z };

/* one turn of njRotateX, njRotateY or njRotateZ with a null matrix, or of
 * njRotateEx's block for that axis. FSCA's sine and cosine make two
 * vectors, and FTRV's products of them replace two rows. njRotateEx skips
 * an angle whose 32 bits are 0. the single turns run it */
static void nj_turn(float matrix[16], enum axis axis, int32_t angle)
{
    static const size_t rows[3][2] = {{1, 2}, {0, 2}, {0, 1}};
    dc_sincos_t turn = dc_fsca((uint32_t)angle);
    float first[4] = {0.0F, 0.0F, 0.0F, 0.0F};
    float second[4] = {0.0F, 0.0F, 0.0F, 0.0F};
    size_t one = rows[axis][0];
    size_t two = rows[axis][1];

    /* the turn about y takes z toward x, so its first row turns away from
     * its second, where the others turn toward it */
    first[one] = turn.cos;
    first[two] = axis == Y ? -turn.sin : turn.sin;
    second[one] = axis == Y ? turn.sin : -turn.sin;
    second[two] = turn.cos;
    dc_ftrv(matrix, first);
    dc_ftrv(matrix, second);
    memcpy(&matrix[4 * one], first, sizeof first);
    memcpy(&matrix[4 * two], second, sizeof second);
}

/* njCalcVector's path for a null matrix, with njInitMatrix's flag 0, or
 * njCalcPoint's: FTRV of (in, fourth) */
static void nj_calc(const float matrix[16], const float in[3], float fourth,
                    float out[3])
{
    float point[4] = {in[0], in[1], in[2], fourth};

    dc_ftrv(matrix, point);
    memcpy(out, point, 3 * sizeof *out);
}

/* a place kernel, or the window it stands for: pos plus offset turned by
 * ang. zam_vec3_place_y's turn takes ang's y alone */
typedef void place_t(const float pos[3], const int32_t ang[3],
                     const float offset[3], float out[3]);

/* njTranslateEx(pos) and njRotateY(NULL, ang[1]) on the unit matrix, then
 * njCalcPoint of offset */
static void nj_place_y(const float pos[3], const int32_t ang[3],
                       const float offset[3], float out[3])
{
    float matrix[16];

    memcpy(matrix, unit, sizeof matrix);
    nj_translate(matrix, pos);
    nj_turn(matrix, Y, ang[1]);
    nj_calc(matrix, offset, 1.0F, out);
}

/* zam_vec3_place_xyz's window: njTranslateV(NULL, pos) on the unit matrix,
 * its three turns, and njCalcPoint of offset */
static void nj_place_xyz(const float pos[3], const int32_t ang[3],
                         const float offset[3], float out[3])
{
    float matrix[16];

    memcpy(matrix, unit, sizeof matrix);
    nj_translate(matrix, pos);
    nj_turn(matrix, Z, ang[2]);
    nj_turn(matrix, Y, ang[1]);
    nj_turn(matrix, X, ang[0]);
    nj_calc(matrix, offset, 1.0F, out);
}

static void place_y(const float pos[3], const int32_t ang[3],
                    const float offset[3], float out[3])
{
    zam_vec3_place_y(pos, ang[1], offset, out);
}

static void place_xyz(const float pos[3], const int32_t ang[3],
                      const float offset[3], float out[3])
{
    zam_vec3_place_xyz(pos, ang[0], ang[1], ang[2], offset, out);
}

static void any_matrix(float matrix[16])
{
    int i;

    for (i = 0; i < 16; i++) {
        matrix[i] = any(0.01F, 4.0F);
    }
}

/* a vector whose components lie within most, or now and then are
 * infinite */
static void any_vector(float vector[3], float most)
{
    int i;

    for (i = 0; i < 3; i++) {
        vector[i] = next() % 32 == 0 ? copysignf(INFINITY, any(1.0F, 2.0F))
                                     : any(0.001F, most);
    }
}

static int any_infinite(const float vector[3])
{
    return isinf(vector[0]) || isinf(vector[1]) || isinf(vector[2]);
}

/* a place kernel against its window, in place. an infinite component of
 * pos must make each of the window's floats FIPR's NaN. call pos with its
 * infinite components 0 its finite part. where an axis of pos is finite,
 * the kernel's float must be the window's for the finite part. for a
 * finite offset, a float whose component is infinite must be that
 * infinity. where flips, out[2] may be the other infinity than the
 * window's */
static void check_place(place_t *kernel, place_t *shipped, int flips,
                        const float pos[3], const int32_t ang[3],
                        const float offset[3])
{
    float pushed[3];
    float out[3];
    float finite[3];
    int i;

    memcpy(out, offset, sizeof out);
    kernel(pos, ang, out, out);
    shipped(pos, ang, offset, pushed);
    if (!any_infinite(pos)) {
        CHECK(same(out, pushed, 2));
        CHECK(same(&out[2], &pushed[2], 1) ||
              (flips && isinf(out[2]) && isinf(pushed[2])));
        return;
    }
    for (i = 0; i < 3; i++) {
        CHECK(bits_of(pushed[i]) == FIPR_NAN);
        finite[i] = isinf(pos[i]) ? 0.0F : pos[i];
    }
    shipped(finite, ang, offset, pushed);
    for (i = 0; i < 3; i++) {
        CHECK(isinf(pos[i]) ? any_infinite(offset) || out[i] == pos[i]
                            : same(&out[i], &pushed[i], 1));
    }
}

/* each turn against its window. the window turns the unit matrix in the
 * kernel's order, and reads by njCalcVector. each must be the window's to the
 * bit, for an infinite component too, and in place */
static void check_turns(void)
{
    for (at = 0; at < CASES; at++) {
        float in[3];
        float pos[3];
        float matrix[16];
        float pushed[3];
        float out[3];
        int32_t x = any_angle();
        int32_t y = any_angle();
        int32_t z = any_angle();

        any_vector(in, 1000.0F);
        any_vector(pos, 1000.0F);

        running = "zam_vec3_turn_x";
        memcpy(matrix, unit, sizeof matrix);
        nj_turn(matrix, X, x);
        nj_calc(matrix, in, 0.0F, pushed);
        zam_vec3_turn_x(x, in, out);
        CHECK(same(out, pushed, 3));

        running = "zam_vec3_turn_y";
        memcpy(matrix, unit, sizeof matrix);
        nj_turn(matrix, Y, y);
        nj_calc(matrix, in, 0.0F, pushed);
        zam_vec3_turn_y(y, in, out);
        CHECK(same(out, pushed, 3));

        running = "zam_vec3_turn_z";
        memcpy(matrix, unit, sizeof matrix);
        nj_turn(matrix, Z, z);
        nj_calc(matrix, in, 0.0F, pushed);
        zam_vec3_turn_z(z, in, out);
        CHECK(same(out, pushed, 3));

        running = "zam_vec3_turn_yxz";
        memcpy(matrix, unit, sizeof matrix);
        nj_turn(matrix, Z, z);
        nj_turn(matrix, X, x);
        nj_turn(matrix, Y, y);
        nj_calc(matrix, in, 0.0F, pushed);
        memcpy(out, in, sizeof out);
        zam_vec3_turn_yxz(x, y, z, out, out);
        CHECK(same(out, pushed, 3));

        running = "zam_vec3_turn_zxy";
        memcpy(matrix, unit, sizeof matrix);
        nj_turn(matrix, Y, y);
        nj_turn(matrix, X, x);
        nj_turn(matrix, Z, z);
        nj_calc(matrix, in, 0.0F, pushed);
        zam_vec3_turn_zxy(x, y, z, in, out);
        CHECK(same(out, pushed, 3));

        {
            const int32_t ang[3] = {x, y, z};

            running = "zam_vec3_place_y";
            check_place(place_y, nj_place_y, 0, pos, ang, in);
            running = "zam_vec3_place_xyz";
            check_place(place_xyz, nj_place_xyz, 1, pos, ang, in);
            memcpy(out, pos, sizeof out);
            place_xyz(out, ang, in, out);
            nj_place_xyz(pos, ang, in, pushed);
            CHECK(any_infinite(pos) || any_infinite(in) ||
                  same(out, pushed, 3));
        }
    }
}

/* the kernels a window's non-matching side calls */
enum kernel { TURN_X, TURN_Y, TURN_Z, TURN_YXZ, TURN_ZXY };

/* an angle a window turns by: the input it reads, and whether its code
 * negates it */
struct angle {
    int input;
    int negated;
};

/* a window of the game's C whose non-matching side calls a kernel. turns
 * are the shipped code's njRotateX, njRotateY and njRotateZ calls, in
 * order. its listing loads their angles from the address at onward. given
 * are the kernel's angles: its x, y and z, or its one angle */
struct window {
    const char *function;
    uint32_t at;
    int count;
    enum axis axes[3];
    struct angle turns[3];
    enum kernel kernel;
    struct angle given[3];
};

/* turn_to_world's turns: z, then x, then y negated, as a yxz turn */
#define TO_WORLD 3, {Z, X, Y}, {{2, 0}, {0, 0}, {1, 1}}, TURN_YXZ
#define TO_WORLD_GIVEN {{0, 0}, {1, 1}, {2, 0}}
/* turn_from_world's: y, then x negated, then z negated, as a zxy turn */
#define FROM_WORLD 3, {Y, X, Z}, {{1, 0}, {0, 1}, {2, 1}}, TURN_ZXY
#define FROM_WORLD_GIVEN {{0, 1}, {1, 0}, {2, 1}}

/* the three inputs are the x, y and z of the angles a window reads. a
 * window of one turn reads the one its axis names. player_accel_ground's y
 * is its local angle. collide_sphere_box's and collide_cylinder_box's y is
 * the difference their listings take by NEG and SUB */
#define TURN_ABOUT(axis, input, negated, kernel)                               \
    1, {axis}, {{input, negated}}, kernel,                                     \
    {                                                                          \
        {                                                                      \
            input, negated                                                     \
        }                                                                      \
    }
static const struct window windows[] = {
    {"turn_from_world", 0x8c045daa, FROM_WORLD, FROM_WORLD_GIVEN},
    {"turn_to_world", 0x8c045e42, TO_WORLD, TO_WORLD_GIVEN},
    {"ground_turn_from_world", 0x8c045ec4, FROM_WORLD, FROM_WORLD_GIVEN},
    {"ground_turn_to_world", 0x8c045f4a, TO_WORLD, TO_WORLD_GIVEN},
    {"player_upright", 0x8c039c2a, TO_WORLD, TO_WORLD_GIVEN},
    {"player_read_stick", 0x8c039ed0, TURN_ABOUT(Y, 1, 0, TURN_Y)},
    {"player_accel_ground", 0x8c03ad40, TO_WORLD, TO_WORLD_GIVEN},
    {"player_step_up", 0x8c041b38, TO_WORLD, TO_WORLD_GIVEN},
    {"tails_step", 0x8c088714, TO_WORLD, TO_WORLD_GIVEN},
    {"turn_x_axis", 0x8c0ac27e, TO_WORLD, TO_WORLD_GIVEN},
    {"look_vector",
     0x8c0ac2f4,
     3,
     {Y, X, Z},
     {{1, 0}, {0, 0}, {2, 0}},
     TURN_ZXY,
     {{0, 0}, {1, 0}, {2, 0}}},
    {"point_to_local", 0x8c01ac74, TURN_ABOUT(Y, 1, 0, TURN_Y)},
    {"point_from_local", 0x8c01acc0, TURN_ABOUT(Y, 1, 1, TURN_Y)},
    {"collide_standing_box2", 0x8c01b3c6, TURN_ABOUT(Z, 2, 1, TURN_Z)},
    {"collide_sphere_box", 0x8c01d1cc, TURN_ABOUT(Y, 1, 0, TURN_Y)},
    {"collide_cylinder_box", 0x8c01df76, TURN_ABOUT(Y, 1, 0, TURN_Y)},
};

/* input's value as angle reads it: NEG's two's complement, where negated */
static int32_t read_angle(const int32_t input[3], struct angle angle)
{
    uint32_t value = (uint32_t)input[angle.input];

    return (int32_t)(angle.negated ? 0U - value : value);
}

/* each window's kernel call against its shipped turns and njCalcVector, on
 * the unit matrix */
static void check_windows(void)
{
    size_t w;

    for (w = 0; w < sizeof windows / sizeof windows[0]; w++) {
        const struct window *window = &windows[w];

        running = window->function;
        for (at = 0; at < CASES / 10; at++) {
            int32_t input[3];
            int32_t given[3];
            float matrix[16];
            float in[3];
            float pushed[3];
            float out[3];
            int i;

            for (i = 0; i < 3; i++) {
                input[i] = any_angle();
                in[i] = any(0.001F, 1000.0F);
            }
            for (i = 0; i < 3; i++) {
                given[i] = read_angle(input, window->given[i]);
            }
            memcpy(matrix, unit, sizeof matrix);
            for (i = 0; i < window->count; i++) {
                nj_turn(matrix, window->axes[i],
                        read_angle(input, window->turns[i]));
            }
            nj_calc(matrix, in, 0.0F, pushed);
            switch (window->kernel) {
            case TURN_X:
                zam_vec3_turn_x(given[0], in, out);
                break;
            case TURN_Y:
                zam_vec3_turn_y(given[0], in, out);
                break;
            case TURN_Z:
                zam_vec3_turn_z(given[0], in, out);
                break;
            case TURN_YXZ:
                zam_vec3_turn_yxz(given[0], given[1], given[2], in, out);
                break;
            case TURN_ZXY:
                zam_vec3_turn_zxy(given[0], given[1], given[2], in, out);
                break;
            }
            CHECK(same(out, pushed, 3));
        }
    }
}

/* the batch against njCalcPoint on each point, for counts of 0 to 9 and
 * strides of 12 to 24 bytes, in place where the strides allow. no float
 * outside the points' changes. the model's FTRV is the reference's, so this
 * holds the stride logic alone */
static void check_points(void)
{
    enum { MOST = 9, ROOM = (MOST * 6) + 6 };

    running = "zam_xmtrx_transform_points";
    for (at = 0; at < CASES / 10; at++) {
        float matrix[16];
        float in[ROOM];
        float out[ROOM];
        float expect[ROOM];
        uint32_t count = (uint32_t)at % (MOST + 1);
        uint32_t in_stride = 12 + (4 * (uint32_t)(next() % 4));
        uint32_t out_stride = 12 + (4 * (uint32_t)(next() % 4));
        int in_place = out_stride <= in_stride && next() % 2 == 0;
        uint32_t i;

        any_matrix(matrix);
        for (i = 0; i < ROOM; i++) {
            in[i] = any(0.001F, 1000.0F);
            out[i] = in_place ? in[i] : any(0.001F, 1000.0F);
        }
        memcpy(expect, out, sizeof expect);
        for (i = 0; i < count; i++) {
            nj_calc(matrix, &in[(i * in_stride) / 4], 1.0F,
                    &expect[(i * out_stride) / 4]);
        }
        memcpy(dc_xmtrx, matrix, sizeof dc_xmtrx);
        zam_xmtrx_transform_points(in_place ? out : in, in_stride, out,
                                   out_stride, count);
        CHECK(same(out, expect, ROOM));
        CHECK(same(dc_xmtrx, matrix, 16));
    }
}

int main(void)
{
    if (fesetround(FE_TOWARDZERO) != 0) {
        fprintf(stderr, "%s: the host can't round toward zero\n", __FILE__);
        return 1;
    }
    check_turns();
    check_windows();
    check_points();
    return 0;
}
