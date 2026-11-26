/* a Chao's palette, blended from the palette files by its evolution and by
 * its run_power and fly_swim. a child's blends by its alignment too */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chao/life.h"
#include "chao/palette.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "system/task.h"

Sint32 child_palette_textures[8] = {7, 8, 9, -1, -1, -1, -1, -1};

/* the palettes in garden_palette_colors, in palette_files's order. the
 * neutral child has two, young and grown. the hero child has a set of six.
 * the dark child has one. each hero adult form has a set of six. a set's first
 * palette is the one at evolution 0. the rest are grown: with no lean, then
 * leaning to swim, fly, run or power */
enum {
    SET_YOUNG,
    SET_NORMAL,
    SET_SWIM,
    SET_FLY,
    SET_RUN,
    SET_POWER,
    SET_PALETTES
};
enum {
    NEUTRAL_YOUNG = 0,
    NEUTRAL_GROWN = 1,
    HERO_CHILD = 2,
    DARK_CHILD = HERO_CHILD + SET_PALETTES,
    HERO_ADULTS = DARK_CHILD + 1
};
/* the hero adult forms, in the order of their palette sets and textures */
enum hero_form {
    HERO_NORMAL,
    HERO_SWIM,
    HERO_FLY,
    HERO_RUN,
    HERO_POWER,
    HERO_FORMS
};
/* palette_textures's sets: the child's, then each hero adult form's */
#define CHILD_TEXTURES 0
#define HERO_TEXTURES 1
#define TEXTURE_SETS (HERO_TEXTURES + HERO_FORMS)

#define LOAD_BYTES 10240

/* the debug grid: a cell for each of the hardware's palette entries */
#define PALETTE_ENTRIES 1024
#define GRID_COLUMNS 128
#define GRID_LEFT 50
#define GRID_TOP 350
#define CELL_WIDTH 4
#define CELL_HEIGHT 15
#define ROW_GAP 2

/* the paletted textures of a child's body, then of each hero adult form's */
/* clang-format off */
static int palette_textures[TEXTURE_SETS][CHAO_PALETTE_BANKS] = {
    {7, 8, 9},
    {41, 42, 43},
    {51, 50, 52},
    {39, 38, 40},
    {48, 47, 49},
    {45, 44, 46}};
/* clang-format on */
static const char *palette_files[CHAO_PALETTES];

static void halt(const char *message)
{
    njPrintColor(0xffff0000);
    do {
        njPrintC(NJM_LOCATION(3, 3), message);
        njWaitVSync();
    } while (1);
}

/* a 5-, 6- or 4-bit channel in value's low bits, widened to 8 bits */
#pragma inline(channel5, channel6, channel4, alpha1, alpha4)
static Uint32 channel5(Uint32 value)
{
    return ((value & 31) << 3) | ((value & 31) >> 2);
}
static Uint32 channel6(Uint32 value)
{
    return ((value & 63) << 2) | ((value & 63) >> 4);
}
static Uint32 channel4(Uint32 value)
{
    return ((value & 15) << 4) | (value & 15);
}
/* 0 for a set bit and 255 for a clear one, the reverse of ARGB1555's sense */
static Uint32 alpha1(Uint32 value)
{
    return (value & 1) ? 0 : 255;
}
/* @hack spelling: alpha4, where channel4 of value shifted right by 12 would
 * do */
static Uint32 alpha4(Uint32 value)
{
    value >>= 12;
    return ((value & 15) << 4) | (value & 15);
}

#ifdef NONMATCHING
/* the plain form the hack tag names: no test */
#define UNREACHED_TEST(condition)
#else
#define UNREACHED_TEST(condition)                                              \
    /* @hack redundant: a test of condition that never runs */                 \
    if (0) {                                                                   \
        if (condition) {                                                       \
        }                                                                      \
    }
#endif

void garden_load_palettes(void)
{
    NJS_TEXPALETTE palette;
    Sint32 index;
    Sint32 i;
    Uint16 *source;
    Uint8 *destination;
    Uint8 *colors;
    Uint32 color;
    palette.palette = syMalloc(LOAD_BYTES);
#ifdef NONMATCHING
    /* this side loads no palettes where the heap has no buffer for them */
    if (!palette.palette) {
        return;
    }
#endif
    for (index = 0; index < CHAO_PALETTES; index++) {
#ifdef NONMATCHING
        /* this side skips a palette file that didn't load or holds too few
         * colors */
        if (njLoadPaletteFile(palette_files[index], &palette) < 0 ||
            palette.count < CHAO_PALETTE_COLORS) {
            continue;
        }
#else
        njLoadPaletteFile(palette_files[index], &palette);
#endif
        /* @hack spelling: the assignment to colors inside the switch's
         * expression, where a statement of its own would do */
        switch (colors = garden_palette_colors[index], palette.mode) {
        case NJD_TEXFMT_ARGB_1555:
            destination = colors;
            source = palette.palette;
            i = CHAO_PALETTE_COLORS - 1;
            while (i >= 0) {
                color = *source++;
                *destination++ = channel5(color);
                *destination++ = channel5(color >> 5);
                UNREACHED_TEST(destination && destination);
                *destination++ = channel5(color >> 10);
                *destination++ = alpha1(color >> 15);
                i--;
            }
            break;
        case NJD_TEXFMT_RGB_565:
            destination = colors;
            source = palette.palette;
            i = CHAO_PALETTE_COLORS - 1;
            while (i >= 0) {
                color = *source++;
                *destination++ = channel5(color);
                *destination++ = channel6(color >> 5);
                *destination++ = channel5(color >> 11);
                *destination++ = 255;
                i--;
            }
            break;
        case NJD_TEXFMT_ARGB_4444:
            destination = colors;
            source = palette.palette;
            i = CHAO_PALETTE_COLORS - 1;
            while (i >= 0) {
                color = *source++;
                *destination++ = channel4(color);
                *destination++ = channel4(color >> 4);
                *destination++ = channel4(color >> 8);
                *destination++ = alpha4(color);
                i--;
            }
            break;
        case NJD_TEXFMT_ARGB_8888:
            destination = colors;
            /* @bug logic: copies a quarter of an ARGB8888 palette,
             * CHAO_PALETTE_COLORS bytes of its CHAO_PALETTE_BYTES, at
             * 0x0c522d92 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it copies the whole
             * palette */
            memcpy(destination, palette.palette, CHAO_PALETTE_BYTES);
#else
            memcpy(destination, palette.palette, CHAO_PALETTE_COLORS);
#endif
            break;
        default:
            halt("ILLEGAL COLOR MODE");
            break;
        }
    }
    UNREACHED_TEST((i = palette.mode, i));
    syFree(palette.palette);
    njSetPaletteMode(NJD_TEXFMT_ARGB_8888);
}

/* each axis's share divides by the sum of the axes' sizes, which
 * KEEP_FROM_ZERO keeps from 0 */
#define KEEP_FROM_ZERO(value)                                                  \
    if ((value) == 0.0F) {                                                     \
        (value) = 0.000001F;                                                   \
    }
#define LIMIT_AXIS(value)                                                      \
    if ((value) > 1.0F) {                                                      \
        (value) = 1.0F;                                                        \
    } else if ((value) < -1.0F) {                                              \
        (value) = -1.0F;                                                       \
    }
#define LIMIT_GROWTH()                                                         \
    if (growth > 1.0F) {                                                       \
        growth = 1.0F;                                                         \
    }
/* limit run_power and fly_swim to -1 to 1, then fold each to its size. a
 * negative run_power leans to run, and a negative fly_swim to swim */
#define FOLD_AXES(run, swim)                                                   \
    LIMIT_AXIS(run_power);                                                     \
    if (run_power < 0.0F) {                                                    \
        run_power_lean = (run);                                                \
        run_power = -run_power;                                                \
    }                                                                          \
    LIMIT_AXIS(fly_swim);                                                      \
    if (fly_swim < 0.0F) {                                                     \
        fly_swim_lean = (swim);                                                \
        fly_swim = -fly_swim;                                                  \
    }
#define AXIS_COLOR()                                                           \
    ((((*normal * rest_run_power) + (*run_power_lean * run_power)) *           \
      run_power_share) +                                                       \
     (((*normal * rest_fly_swim) + (*fly_swim_lean * fly_swim)) *              \
      fly_swim_share))
#ifdef NONMATCHING
/* this side returns where the heap has no scratch buffer, which the matching
 * side writes through */
#define REQUIRE_SCRATCH(pointer)                                               \
    if (!(pointer)) {                                                          \
        return;                                                                \
    }
#else
#define REQUIRE_SCRATCH(pointer)
#endif

void chao_update_palette(struct chao_parameters *parameters, int slot)
{
    Uint8 *buffer;
    Uint8 *destination;
    Uint8 *young;
    Uint8 *grown;
    Uint8 *hero;
    Uint8 *normal;
    Uint8 *fly_swim_lean;
    Uint8 *run_power_lean;
    Uint8 *dark;
    Float run_power;
    Float fly_swim;
    Float growth;
    Float alignment;
    Float rest_run_power;
    Float rest_fly_swim;
    Float rest_growth;
    Float rest_alignment;
    Float run_power_share;
    Float fly_swim_share;
    Float aligned;
    Float neutral;
    int first;
    int i;
    switch (parameters->type) {
    case CHAO_CHILD:
        first = NEUTRAL_YOUNG;
        break;
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
        first = HERO_ADULTS + (HERO_NORMAL * SET_PALETTES);
        break;
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
        first = HERO_ADULTS + (HERO_SWIM * SET_PALETTES);
        break;
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
        first = HERO_ADULTS + (HERO_FLY * SET_PALETTES);
        break;
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
        first = HERO_ADULTS + (HERO_RUN * SET_PALETTES);
        break;
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
        first = HERO_ADULTS + (HERO_POWER * SET_PALETTES);
        break;
    default:
        return;
    }
    if (parameters->type == CHAO_CHILD) {
        buffer = syMalloc(CHAO_PALETTE_BYTES);
        REQUIRE_SCRATCH(buffer);
        young = garden_palette_colors[NEUTRAL_YOUNG];
        grown = garden_palette_colors[NEUTRAL_GROWN];
        hero = garden_palette_colors[HERO_CHILD + SET_YOUNG];
        normal = garden_palette_colors[HERO_CHILD + SET_NORMAL];
        run_power_lean = garden_palette_colors[HERO_CHILD + SET_POWER];
        fly_swim_lean = garden_palette_colors[HERO_CHILD + SET_FLY];
        dark = garden_palette_colors[DARK_CHILD];
        destination = buffer;
        run_power = parameters->run_power;
        fly_swim = parameters->fly_swim;
        growth = parameters->evolution;
        alignment = parameters->alignment;
        LIMIT_GROWTH();
        KEEP_FROM_ZERO(run_power);
        KEEP_FROM_ZERO(fly_swim);
        FOLD_AXES(garden_palette_colors[HERO_CHILD + SET_RUN],
                  garden_palette_colors[HERO_CHILD + SET_SWIM]);
        rest_run_power = 1.0F - run_power;
        rest_fly_swim = 1.0F - fly_swim;
        rest_growth = 1.0F - growth;
        run_power_share = (1.0F / (run_power + fly_swim)) * run_power;
        fly_swim_share = (1.0F / (run_power + fly_swim)) * fly_swim;
        if (alignment >= 0.0F) {
            if (alignment > 1.0F) {
                alignment = 1.0F;
            }
            rest_alignment = 1.0F - alignment;
            for (i = CHAO_PALETTE_BYTES - 1; i >= 0; i--) {
                aligned = AXIS_COLOR();
                aligned = (aligned * growth) + (*hero * rest_growth);
                neutral = (*young * rest_growth) + (*grown * growth);
                *destination =
                    (neutral * rest_alignment) + (aligned * alignment);
                normal++;
                run_power_lean++;
                fly_swim_lean++;
                hero++;
                grown++;
                young++;
                destination++;
            }
            UNREACHED_TEST(rest_growth && destination);
        } else {
            if (alignment < -1.0F) {
                alignment = -1.0F;
            }
            alignment *= -1.0F;
            rest_alignment = 1.0F - alignment;
            for (i = CHAO_PALETTE_BYTES - 1; i >= 0; i--) {
                aligned = *dark;
                neutral = (*young * rest_growth) + (*grown++ * growth);
                *destination =
                    (neutral * rest_alignment) + (aligned * alignment);
                dark++;
                young++;
                destination++;
            }
        }
        njSetPaletteData(slot * CHAO_PALETTE_COLORS, CHAO_PALETTE_COLORS,
                         buffer);
        syFree(buffer);
    } else {
        buffer = syMalloc(CHAO_PALETTE_BYTES);
        REQUIRE_SCRATCH(buffer);
        young = garden_palette_colors[first + SET_YOUNG];
        normal = garden_palette_colors[first + SET_NORMAL];
        run_power_lean = garden_palette_colors[first + SET_POWER];
        fly_swim_lean = garden_palette_colors[first + SET_FLY];
        destination = buffer;
        run_power = parameters->run_power;
        fly_swim = parameters->fly_swim;
        growth = parameters->evolution;
        LIMIT_GROWTH();
        KEEP_FROM_ZERO(run_power);
        UNREACHED_TEST(fly_swim_lean);
        KEEP_FROM_ZERO(fly_swim);
        FOLD_AXES(garden_palette_colors[first + SET_RUN],
                  garden_palette_colors[first + SET_SWIM]);
        rest_run_power = 1.0F - run_power;
        run_power_share = (1.0F / (run_power + fly_swim)) * run_power;
        UNREACHED_TEST(run_power_share);
        fly_swim_share = (1.0F / (run_power + fly_swim)) * fly_swim;
        rest_fly_swim = 1.0F - fly_swim;
        rest_growth = 1.0F - growth;
        for (i = CHAO_PALETTE_BYTES - 1; i >= 0; i--) {
            aligned = AXIS_COLOR();
            *destination = (aligned * growth) + (*young * rest_growth);
            normal++;
            run_power_lean++;
            fly_swim_lean++;
            young++;
            destination++;
        }
        UNREACHED_TEST(run_power_share && fly_swim_share && rest_fly_swim &&
                       rest_growth && destination);
        njSetPaletteData(slot * CHAO_PALETTE_COLORS, CHAO_PALETTE_COLORS,
                         buffer);
        syFree(buffer);
    }
}

/* unlike chao_blend_growing_palette, chao_blend_child_palette writes
 * CHAO_PALETTE_BANKS colors from entry slot * CHAO_PALETTE_BANKS, not the
 * slot's CHAO_PALETTE_COLORS. no code calls either */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void chao_blend_child_palette(int slot, Float alignment)
{
    Uint8 *buffer = syMalloc(CHAO_PALETTE_BYTES);
    Uint8 *base = garden_palette_colors[NEUTRAL_YOUNG];
    Uint8 *target = garden_palette_colors[NEUTRAL_GROWN];
    Uint8 *destination = buffer;
    int i;
    REQUIRE_SCRATCH(buffer);
    LIMIT_AXIS(alignment);
    if (alignment < 0.0F) {
        alignment = -alignment;
        target = garden_palette_colors[HERO_CHILD + SET_YOUNG];
    }
    for (i = CHAO_PALETTE_BYTES - 1; i >= 0; i--) {
        *destination++ = *base + (int)((*target - *base) * alignment);
        base++;
        target++;
    }
    njSetPaletteData(slot * CHAO_PALETTE_BANKS, CHAO_PALETTE_BANKS, buffer);
    syFree(buffer);
}
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void chao_blend_growing_palette(int slot, Float alignment, Float growth)
{
    Uint8 *buffer = syMalloc(CHAO_PALETTE_BYTES);
    Uint8 *base = garden_palette_colors[NEUTRAL_YOUNG];
    Uint8 *target = garden_palette_colors[NEUTRAL_GROWN];
    Uint8 *adult = garden_palette_colors[HERO_CHILD + SET_NORMAL];
    Uint8 *destination = buffer;
    int i;
    REQUIRE_SCRATCH(buffer);
    LIMIT_GROWTH();
    LIMIT_AXIS(alignment);
    if (alignment < 0.0F) {
        target = garden_palette_colors[HERO_CHILD + SET_YOUNG];
        alignment = -alignment;
    }
    for (i = CHAO_PALETTE_BYTES - 1; i >= 0; i--) {
        *destination = *base + (int)((*adult - *base) * growth);
        base++;
        adult++;
        *destination =
            *destination + (int)((*target - *destination) * alignment);
        destination++;
        target++;
    }
    njSetPaletteData(slot * CHAO_PALETTE_COLORS, CHAO_PALETTE_COLORS, buffer);
    syFree(buffer);
}

void chao_bind_palette(struct task *chao, int slot)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    int set = CHILD_TEXTURES;
    int i;
    int texture;
    switch (parameters->type) {
    case CHAO_CHILD:
        set = CHILD_TEXTURES;
        break;
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
        set = HERO_TEXTURES + HERO_NORMAL;
        break;
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
        set = HERO_TEXTURES + HERO_SWIM;
        break;
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
        set = HERO_TEXTURES + HERO_FLY;
        break;
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
        set = HERO_TEXTURES + HERO_RUN;
        break;
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
        set = HERO_TEXTURES + HERO_POWER;
        break;
    default:
        return;
    }
    for (i = 0; i < CHAO_PALETTE_BANKS; i++) {
        texture = palette_textures[set][i];
        njSetPaletteBankNum(texture, (slot * CHAO_PALETTE_BANKS) + i);
    }
}
void garden_draw_palette_grid(void) /* NOLINT(misc-use-internal-linkage) */
{
    Uint32 colors[PALETTE_ENTRIES];
    int i;
    /* @bug uninitialized: the grid draws all of colors at 0x0c523700, and
     * the read at 0x0c5236da sets only its first half */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it reads every entry it draws */
    njGetPaletteData(0, PALETTE_ENTRIES, colors);
#else
    njGetPaletteData(0, PALETTE_ENTRIES / 2, colors);
#endif
    for (i = 0; i < PALETTE_ENTRIES; i++) {
        garden_draw_plain_rect(
            ((i % GRID_COLUMNS) * CELL_WIDTH) + GRID_LEFT,
            ((i / GRID_COLUMNS) * CELL_HEIGHT) + GRID_TOP,
            (((i % GRID_COLUMNS) + 1) * CELL_WIDTH) + GRID_LEFT,
            (((i / GRID_COLUMNS) + 1) * CELL_HEIGHT) + (GRID_TOP - ROW_GAP),
            colors[i]);
    }
}

static const char *palette_files[CHAO_PALETTES] = {
    "AL_NC00.PVP", "AL_NC01.PVP", "AL_HCZ.PVP", "AL_HCN.PVP", "AL_HCS.PVP",
    "AL_HCF.PVP",  "AL_HCR.PVP",  "AL_HCP.PVP", "AL_DC.PVP",  "AL_HNZ.PVP",
    "AL_HNN.PVP",  "AL_HNS.PVP",  "AL_HNF.PVP", "AL_HNR.PVP", "AL_HNP.PVP",
    "AL_HSZ.PVP",  "AL_HSN.PVP",  "AL_HSS.PVP", "AL_HSF.PVP", "AL_HSR.PVP",
    "AL_HSP.PVP",  "AL_HFZ.PVP",  "AL_HFN.PVP", "AL_HFS.PVP", "AL_HFF.PVP",
    "AL_HFR.PVP",  "AL_HFP.PVP",  "AL_HRZ.PVP", "AL_HRN.PVP", "AL_HRS.PVP",
    "AL_HRF.PVP",  "AL_HRR.PVP",  "AL_HRP.PVP", "AL_HPZ.PVP", "AL_HPN.PVP",
    "AL_HPS.PVP",  "AL_HPF.PVP",  "AL_HPR.PVP", "AL_HPP.PVP"};
