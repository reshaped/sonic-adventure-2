#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "effect/burst.h"
#include "event/effect_track.h"
#include "event/mirror.h"
#include "event/scene.h"
#include "event/screen_sprite.h"
#include "shared/black_smoke.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/flame.h"
#include "shared/flash.h"
#include "shared/lens_flare.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket_thrust.h"
#include "shared/sparkle.h"
#include "shared/splash.h"
#include "shared/warp_hole.h"
#include "shared/white_breath.h"
#include "system/util.h"

#define SPRITES 64
/* a sprite's ids run from LOWEST_ID. an id matches within ID_TOLERANCE */
#define LOWEST_ID 1.0F
#define ID_TOLERANCE 0.3F
/* added to a float before it converts to an integer. a whole number held a
 * hair low then still converts to itself */
#define TRUNCATION_BIAS 0.1F
#define MIN_SCALE 0.0001F

/* a sprite at the root of motion, one of scene's motions. id 0 marks a free
 * slot */
struct motion_sprite {
    Sint32 id, pattern, texture, scene, motion;
    Float size, start_frame;
};

/* the kinds of a pattern's steps, which scale a motion sprite. FADE_IN scales
 * it from 0 to 1 over the step, and FADE_OUT from 1 to 0. HOLD holds 1 after
 * a FADE_IN, and 0 after a FADE_OUT or an OFF. OFF has no scale of its own */
enum fade_kind { HOLD, FADE_IN, FADE_OUT, OFF };
struct fade_step {
    enum fade_kind kind;
    Sint32 frames;
};
struct fade_pattern {
    struct fade_step *steps;
    Sint32 count;
};

#ifdef NONMATCHING
/* flash_manager_create as an event_particle_effect: it takes the point's three
 * floats and the scale, and returns no particle */
static struct particle *flash_at(NJS_POINT3 *pos, NJS_VECTOR *speed,
                                 Float scale)
{
    flash_manager_create(pos->x, pos->y, pos->z, scale);
    return NULL;
}
#endif
event_particle_effect *event_particles[EVENT_PARTICLES] = {
    NULL,
    dust_create,
    sparkle_create,
    ring_sparkle_create_long,
    dirt_create,
    burst_create_wobbling,
    rocket_thrust_create,
    white_breath_fade_create,
    NULL,
    NULL,
    splash_create,
    black_smoke_sinking_create,
    dust_short_create,
    white_breath_short_create,
    flame_create,
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. flash_at passes
     * flash_manager_create the point and the scale it takes */
    flash_at,
#else
    /* @bug mismatched-call: 0x8c506aaa calls flash_manager_create with a
     * point, a velocity and one float. it takes four floats: x, y, z and
     * scale. the flash takes the effect's scale for its x, and nothing sets
     * the other three */
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    (event_particle_effect *)flash_manager_create,
#endif
    warp_hole_particle_create,
    NULL,
    NULL,
};

#define UP(n) {FADE_IN, (n)}
#define DOWN(n) {FADE_OUT, (n)}
#define WAIT(n) {HOLD, (n)}
#define DARK {OFF, 0}

/* the steps of patterns 1 to 12, a row each. sprite_scale plays the next
 * row's first step before a pattern's own, so the rows keep their order */
/* clang-format off */
static struct fade_step fade_steps[] = {
    UP(2), DOWN(3), WAIT(3),
    UP(5), DARK,
    UP(10), DARK,
    UP(5), DOWN(2), WAIT(3),
    UP(12), DARK,
    UP(2), DARK, UP(2), DARK, WAIT(5),
    UP(4), DARK, UP(4), DARK, UP(4), DARK, WAIT(10),
    UP(15), DARK, WAIT(3),
    UP(13), DARK, UP(5), DARK, WAIT(3),
    UP(8), DOWN(8),
    UP(12), DOWN(12), WAIT(5),
    UP(8), DOWN(1)};
/* clang-format on */
static struct fade_pattern patterns[] = {
    {NULL, 0},
    {&fade_steps[0], 3},
    {&fade_steps[3], 2},
    {&fade_steps[5], 2},
    {&fade_steps[7], 3},
    {&fade_steps[10], 2},
    {&fade_steps[12], 5},
    {&fade_steps[17], 7},
    {&fade_steps[24], 3},
    {&fade_steps[27], 5},
    {&fade_steps[32], 2},
    {&fade_steps[34], 3},
    {&fade_steps[37], 2},
};
#define PATTERNS ((int)ARRAY_COUNT(patterns))

static NJS_POINT3 flare_position;
static Bool flare_on;
static struct motion_sprite sprites[SPRITES];

static void hide_sprites(Float id);
static void start_sprite(struct event_effect *effect);
static void draw_sprites(void);
static Float sprite_scale(int slot);

void event_effects_reset(void)
{
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    memset(sprites, 0, sizeof(sprites));
#else
    /* @hack type: the sprites cleared as Sint32 words, counting down, where
     * memset would do */
    Sint32 *word = (Sint32 *)sprites;
    Sint32 left;
    for (left = SPRITES * (sizeof(struct motion_sprite) / sizeof(*word));
         left != 0; left--) {
        *word++ = 0;
    }
#endif
}

void event_effect_fire(struct event_effect *effect)
{
    NJS_POINT3 position;

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: a negative kind fires nothing */
    if (effect->kind < 0) {
        return;
    }
#endif
    /* @bug memory: the test at 0x8c506a24 lets a negative kind through, and
     * 0x8c506aa4 calls through the word it indexes before event_particles */
    if (effect->kind == 0 || effect->kind >= EVENT_PARTICLES) {
        return;
    }
    event_motion_position(
        event_scenes[event_scene_index].motions[effect->motion],
        event_scene_frame, &position);
    switch (effect->kind) {
    case EVENT_FLARE_ON:
        event_flare_on();
        event_flare_place(&position);
        break;
    case EVENT_FLARE_OFF:
        event_flare_off();
        break;
    case EVENT_MOTION_SPRITE_ON:
        start_sprite(effect);
        break;
    case EVENT_MOTION_SPRITE_OFF:
        hide_sprites(effect->speed.y);
        break;
    default:
        event_particles[effect->kind](&position, &effect->speed, effect->scale);
        break;
    }
}

void event_flare_place(NJS_POINT3 *position)
{
    flare_position = *position;
}

void event_flare_on(void)
{
    flare_on = TRUE;
}

void event_flare_off(void)
{
    flare_on = FALSE;
}

void event_flare_draw(void)
{
    if (flare_on) {
        lens_flare_draw(&flare_position);
    }
}

void event_effects_draw(void)
{
    event_flare_draw();
    draw_sprites();
}

static void hide_sprites(Float id)
{
    int i;

    if (id + ID_TOLERANCE < LOWEST_ID) {
        return;
    }
    for (i = 0; i < SPRITES; i++) {
        if ((Float)sprites[i].id >= id - ID_TOLERANCE &&
            (Float)sprites[i].id <= id + ID_TOLERANCE) {
            sprites[i].id = 0;
        }
    }
}

static void start_sprite(struct event_effect *effect)
{
    int i;

    for (i = 0; i < SPRITES; i++) {
        if (sprites[i].id == 0) {
            sprites[i].id = effect->speed.y + TRUNCATION_BIAS;
            sprites[i].texture = effect->scale + TRUNCATION_BIAS;
            sprites[i].pattern = effect->speed.x + TRUNCATION_BIAS;
            sprites[i].scene = event_scene_index;
            sprites[i].motion = effect->motion;
            sprites[i].start_frame = effect->frame;
            sprites[i].size = effect->speed.z;
            return;
        }
    }
}

static void draw_sprites(void)
{
    NJS_SPRITE sprite;
    NJS_TEXANIM anim;
    struct motion_sprite *slot;
    int i;

    for (i = 0; i < SPRITES; i++) {
        slot = &sprites[i];
        if (slot->id == 0) {
            continue;
        }
        if (slot->scene != event_scene_index) {
            hide_sprites(slot->id);
            continue;
        }
        event_motion_position(
            event_scenes[event_scene_index].motions[slot->motion],
            event_scene_frame, &sprite.p);
        sprite.sx = sprite.sy = sprite_scale(i) * slot->size;
        sprite.ang = 0;
        sprite.tlist = event_scene_file->texlist;
        sprite.tanim = &anim;
        anim.texid = slot->texture;
        anim.sx = event_scene_file->texture_sizes[anim.texid].width;
        anim.sy = event_scene_file->texture_sizes[anim.texid].height;
        anim.cx = anim.sx / 2;
        anim.cy = anim.sy / 2;
        anim.u1 = 0;
        anim.v1 = 0;
        anim.u2 = EVENT_UV_LAST;
        anim.v2 = EVENT_UV_LAST;
        if (sprite.sx > MIN_SCALE) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            njDrawSprite3D(&sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        }
    }
}

static Float sprite_scale(int slot)
{
    struct fade_step *steps;
    Float elapsed;
    Float step_end;
    Float step_start;
    Float from;
    Float to;
    int pattern;
    int i;
    Bool faded_in;

    pattern = sprites[slot].pattern;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: the test refuses
     * pattern 13. faded_in starts FALSE so that it holds a value on each path
     * the compiler sees. no HOLD reads that start: the loop starts on a
     * FADE_IN, or pattern 12's guard below returns before it */
    faded_in = FALSE;
    if (pattern > 0 && pattern < PATTERNS) {
#else
    /* @bug memory: the test at 0x8c506d96 admits pattern 13, past the end of
     * patterns, and 0x8c506dbc reads its count */
    if (pattern > 0 && pattern <= PATTERNS) {
#endif
        steps = patterns[pattern].steps;
        elapsed = event_frame - sprites[slot].start_frame;
        step_end = 0.0F;
#ifdef NONMATCHING
        /* the plain form the hack tag names */
        i = patterns[pattern].count;
#else
        /* @hack redundant: a loop that sums the steps' frames into length,
         * which nothing reads. it leaves i at the count, where
         * i = patterns[pattern].count would do */
        {
            int length = 0;
            for (i = 0; i < patterns[pattern].count; i++) {
                length += steps[i].frames;
            }
            (void)length;
        }
#endif
#ifdef NONMATCHING
        /* this side keeps the result of the bug tagged below, without its
         * read. fade_steps ends with pattern 12's row. the image takes that
         * row's next step from patterns[0], as a HOLD of 0 frames. while
         * elapsed is 0 or more, the loop passes it at once. below 0, the image
         * divides by its 0 frames into a scale that draws nothing */
        if (pattern == PATTERNS - 1 && elapsed >= 0.0F) {
            i = 0;
        } else if (pattern == PATTERNS - 1) {
            return 0.0F;
        }
#endif
        /* @bug memory: for pattern 12, the loop's first read at 0x8c506ddc
         * takes the step past the end of fade_steps, which is patterns[0] */
        for (;;) {
            step_start = step_end;
            step_end += (Float)steps[i].frames;
            if (step_end > elapsed) {
                break;
            }
            switch (steps[i].kind) {
            case FADE_IN:
                faded_in = TRUE;
                break;
            case FADE_OUT:
            case OFF:
                faded_in = FALSE;
                break;
            default:
                break;
            }
            i++;
            if (i >= patterns[pattern].count) {
                i = 0;
            }
        }
        /* @hack spelling: step_start reused for the frames into the step,
         * where a local of its own would do */
        step_start = elapsed - step_start;
        switch (steps[i].kind) {
        case FADE_IN:
            from = 0.0F;
            to = 1.0F;
            break;
        case FADE_OUT:
            from = 1.0F;
            to = 0.0F;
            break;
        case HOLD:
            /* @bug uninitialized: a HOLD that no FADE_IN, FADE_OUT or OFF came
             * before tests faded_in unset at 0x8c506e9a */
            if (faded_in) {
                from = 1.0F;
                to = 1.0F;
            } else {
                from = 0.0F;
                to = 0.0F;
            }
            break;
        default: /* NOLINT(clang-diagnostic-sometimes-uninitialized) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: an OFF
             * step scales by 0 */
            return 0.0F;
#else
            /* @bug uninitialized: an OFF step sets neither from nor to, and
             * 0x8c506eaa interpolates between them */
            break;
#endif
        }
        return from + ((step_start / (Float)steps[i].frames) * (to - from));
    }
    /* @bug uninitialized: a pattern the test at 0x8c506d96 refuses returns
     * no value, which draw_sprites uses */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a pattern the test refuses scales
     * by 0 */
    return 0.0F;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
