/* the effect object: EFFOBJ0 in a stage's object list. it throws one kind of
 * effect by chance */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/burst.h"
#include "object/set.h"
#include "shared/black_smoke.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/effect_object.h"
#include "shared/flame.h"
#include "shared/flash.h"
#include "shared/ring_sparkle.h"
#include "shared/rocket_thrust.h"
#include "shared/spark_point.h"
#include "shared/spark_sprite.h"
#include "shared/sparkle.h"
#include "shared/splash.h"
#include "shared/stage_texture_groups.h"
#include "shared/warp_hole.h"
#include "shared/white_breath.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the effect_object_kind in the low byte of ang.x */
#define KIND(work) ((Uint8)(work)->ang.x)
/* the chance of a throw on each step, out of CHANCE_MAX */
#define CHANCE(work) ((Uint8)((work)->ang.x >> 8))
#define CHANCE_MAX 255

#define SCALE_BIAS 0.05F
#define SCALE_MAX 20.0F
/* VARIED's factor runs from FACTOR_LEAST up to FACTOR_LEAST + FACTOR_RANDOM */
#define FACTOR_LEAST 0.9F
#define FACTOR_RANDOM 0.2F
#define VARIED(scale)                                                          \
    ((scale) * (FACTOR_LEAST + (rand() * (FACTOR_RANDOM * RAND_SCALE))))
#define SPARK_FLOOR_BELOW 0.1F

static void effect_object_destroy(struct task *task);
static void effect_object_step(struct task *task);

void effect_object_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, for the SET editor's labels it
     * leaves in the constants */
    if (0) {
        (void)"<- EFF[  ]/RND[   ]";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- SPD";
        (void)"<- SPDRND";
        (void)"<- SCL";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = effect_object_step;
    task->destructor = effect_object_destroy;
}

static void effect_object_destroy(struct task *task)
{
}

static void effect_object_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: chance, a local for the throw's chance, where
     * CHANCE(work) worked out in the test would do */
    Float chance;
    Float scale;
    NJS_VECTOR spd;

    if (placement_try_leave(task)) {
        return;
    }
    chance = CHANCE(work) * (1.0F / CHANCE_MAX);
    if (game_state.time_stopped != 0 || chance <= rand() * RAND_SCALE) {
        return;
    }
    spd.y = work->scl.x + (((rand() * RAND_SCALE) - 0.5F) * work->scl.y);
    spd.x = ((rand() * RAND_SCALE) - 0.5F) * work->scl.y;
    spd.z = ((rand() * RAND_SCALE) - 0.5F) * work->scl.y;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCalcVector(NULL, &spd, &spd);
    njPopMatrix(1);
    scale = work->scl.z + SCALE_BIAS;
    if (scale > SCALE_MAX) {
        scale = SCALE_MAX;
    }
    switch (KIND(work)) {
    case EFFECT_OBJECT_SPLASH:
        splash_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_BURST:
        burst_create_wobbling(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_SPARKLE:
        sparkle_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_SMOKE_SINKING:
        black_smoke_sinking_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_RING_SPARKLE:
        ring_sparkle_create_long(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_THRUST:
        rocket_thrust_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_DUST_SHORT:
        dust_short_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_BREATH_SHORT:
        white_breath_short_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_DIRT:
        dirt_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_SPARK_POINT:
        spark_point_create(&work->pos, &spd, 0.0F,
                           work->pos.y - SPARK_FLOOR_BELOW);
        break;
    case EFFECT_OBJECT_SPARK_SPRITE:
        spark_sprite_create(&work->pos, &spd, 0.0F,
                            work->pos.y - SPARK_FLOOR_BELOW);
        break;
    case EFFECT_OBJECT_FLAME:
        flame_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_DUST:
        dust_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_BREATH_FADE:
        white_breath_fade_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_SMOKE_RISING:
        black_smoke_rising_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_FLASH:
        flash_manager_create(work->pos.x, work->pos.y, work->pos.z,
                             VARIED(scale));
        break;
    case EFFECT_OBJECT_WARP_HOLE:
        warp_hole_particle_create(&work->pos, &spd, VARIED(scale));
        break;
    case EFFECT_OBJECT_BREATH_TINTED:
        white_breath_tinted_create(&work->pos, &spd, VARIED(scale), 0.0F);
        break;
    default:
        /* @hack redundant: a block no path reaches, whose code the image
         * doesn't hold */
        if (0) {
            njPrintD(0, 0, 0);
            njPrintD(0, 0, 0);
            njPrintD(0, 0, 0);
            work->scl.z = rand() * (0.2F * RAND_SCALE);
        }
    }
}

/* the kinds' names in the SET editor, in enum effect_object_kind's order */
static const char *effect_object_names[EFFECT_OBJECT_BREATH_TINTED + 1] = {
    "SPLASH", "BOMB",  "KIRAN", "MUDDYH", "RING",   "ROCKET",
    "SMOKEH", "SNOWH", "TUTI",  "LNSPK",  "SPSPK",  "FLAME",
    "SMOKE",  "SNOW",  "MUDDY", "FLASH",  "WPBALL", "POISON",
};

NJS_TEXLIST *object_effect_texlists[] = {
    &splash_texlist,
    &burst_texlist,
    &sparkle_texlist,
    &ring_sparkle_texlist,
    &rocket_thrust_texlist,
    &dust_texlist,
    &white_breath_texlist,
    &dirt_texlist,
    &spark_sprite_texlist,
    &flame_texlist,
    &flash_texlist,
    &warp_hole_texlist,
    NULL,
};
