/* parked cars, CARKAZ in a stage's object list. a car stands and pushes the
 * players out of its box. a crash throws it as a wreck, which tumbles and
 * bounces, then fades out */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/cars.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/earth_shake.h"
#include "shared/flash.h"
#include "shared/glow_light.h"
#include "shared/modifier_volume.h"
#include "shared/parked_car.h"
#include "shared/stage_sounds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define CAR_KINDS 16
#include "shared/parked_car/text.inc"

/* a car's scl.x and scl.y each gain TRUNCATE_SLACK before they truncate */
#define TRUNCATE_SLACK 0.1F
#define KIND(work)                                                             \
    (&car_kinds[(Uint8)((work)->scl.y + TRUNCATE_SLACK) % CAR_KINDS])
/* where a car's scl.y + 0.1 truncates to KIND_RANDOM, the car's kind is
 * random. it is one of the KIND_RANDOM_COUNT kinds from KIND_RANDOM_FIRST */
#define KIND_RANDOM 0
#define KIND_RANDOM_FIRST 2
#define KIND_RANDOM_COUNT 7

/* a kind's flags */
/* the setup gives a car of the kind CAR_NO_PUSH_OUT */
#define CAR_NO_PUSH 0x2
/* the wreck turns half a turn about z, the car's long axis, and none about x
 * or y. no kind has the flag */
#define CAR_ROLL_ONLY 0x4
/* the wreck's collision record is a COLLISION_OBJECT, not a
 * COLLISION_OBJECT_MUTUAL. no kind has the flag */
#define CAR_WRECK_OBJECT 0x8
/* the car has a siren, and the glows of two beacons and two lamps */
#define CAR_POLICE 0x10
#define CAR_DPT 0x20 /* the car has the glows of a beacon and two lamps */
#define CAR_BEACONS (CAR_POLICE | CAR_DPT)

/* a car's work's submode */
/* the car pushes no player out of its box. a player in its box crashes it with
 * any touch */
#define CAR_NO_PUSH_OUT 0x1
/* a player in the box leaves through the box's nearer side. parked_car_init
 * gives every car the bit, and nothing clears it. without it, a player in the
 * box's +z half would leave through a side in x */
#define CAR_NEARER_SIDE 0x4
/* the last draw found the car out of view, and its shapes are off */
#define CAR_UNSEEN 0x10
#define CAR_RANDOM 0x20 /* the setup picked the car's kind at random */
/* the car neither draws nor pushes, and its shapes are off. nothing in this
 * object sets the bit */
#define CAR_HIDDEN 0x80

/* a car's record, in task->motion. the car draws height + lift + DRAW_RISE
 * above its pos. nothing here sets lift or ang */
struct car {
    Uint8 unk_00[0x1c];
    struct angles ang; /* what the draw adds to the work's ang */
    Uint8 unk_28[4];
    Float height;
    Uint8 unk_30[0x10];
    Float lift;
    Uint8 unk_44[4];
};
#define CAR(task) ((struct car *)(task)->motion.p)
#define DRAW_RISE 1.0F

/* how hard a crash throws a car. a wreck keeps its crash's force in its
 * work's mode */
enum crash_force { CRASH_NUDGE, CRASH_HIT, CRASH_FLAME_HIT };

#define WRECK(task) ((struct wreck *)(task)->motion.p)
/* the kind a wreck was made of */
#define WRECK_KIND(task) ((struct car_kind *)(task)->extra.p)
/* the steps left before a wreck starts to fade, in task->unk_3c */
#define LIFE(task) ((task)->unk_3c.l)

/* a crash a player makes scores CRASH_SCORE points */
#define CRASH_SCORE 20
/* a crash plays CRASH_SOUND or CRASH_SOUND_2, at random. a wreck's landing
 * plays LAND_SOUND */
#define CRASH_SOUND 0
#define CRASH_SOUND_2 1
#define LAND_SOUND 2
#define SOUND_PRIORITY 0
/* particle_draw_quad's size_limit for none */
#define NO_SIZE_LIMIT 0.0F

/* a random Float from -0.5 up to 0.5 */
#define RANDOM_HALF() ((rand() * RAND_SCALE) - 0.5F)

static void parked_car_draw_shadow(Float half_x, Float half_z,
                                   struct angles *ang);
static void parked_car_setup(struct task *task);
static void parked_car_destroy(struct task *task);
static void parked_car_step(struct task *task);
static void parked_car_draw(struct task *task);
static struct task *parked_car_wreck_create(
    struct car_kind *kind, NJS_POINT3 *pos, struct angles *ang, NJS_VECTOR *spd,
    NJS_POINT3 *floor_pos, struct angles *floor_ang, Sint8 force, int life);
static void parked_car_wreck_step(struct task *task);
static void parked_car_wreck_draw(struct task *task);
static void parked_car_wreck_destroy(struct task *task);

/* the one-piece models of the five cars without beacons, which stg13/car.c
 * includes too */
#include "shared/car_models/taxi_fixed_model.h"
#include "shared/car_models/fixed_models.h"

static NJS_TEXNAME police_car_texnames[12] = {
#include "shared/parked_car/police_car_texnames.inc"
};
static NJS_TEXLIST police_car_texlist = {
#include "shared/parked_car/police_car_texlist.inc"
};
static Sint16 police_car_beacon_1_polygons[25] = {
#include "shared/parked_car/police_car_beacon_1_polygons.inc"
};
static Sint32 police_car_beacon_1_vertices[19] = {
#include "shared/parked_car/police_car_beacon_1_vertices.inc"
};
static NJS_CNK_MODEL police_car_beacon_1_model = {
#include "shared/parked_car/police_car_beacon_1_model.inc"
};
static NJS_CNK_OBJECT police_car_beacon_1_object = {
#include "shared/parked_car/police_car_beacon_1_object.inc"
};
static Sint16 police_car_beacon_0_polygons[25] = {
#include "shared/parked_car/police_car_beacon_0_polygons.inc"
};
static Sint32 police_car_beacon_0_vertices[19] = {
#include "shared/parked_car/police_car_beacon_0_vertices.inc"
};
static NJS_CNK_MODEL police_car_beacon_0_model = {
#include "shared/parked_car/police_car_beacon_0_model.inc"
};
static NJS_CNK_OBJECT police_car_beacon_0_object = {
#include "shared/parked_car/police_car_beacon_0_object.inc"
};
static Sint16 police_car_light_polygons[71] = {
#include "shared/parked_car/police_car_light_polygons.inc"
};
static Sint32 police_car_light_vertices[35] = {
#include "shared/parked_car/police_car_light_vertices.inc"
};
static NJS_CNK_MODEL police_car_light_model = {
#include "shared/parked_car/police_car_light_model.inc"
};
static NJS_CNK_OBJECT police_car_light_object = {
#include "shared/parked_car/police_car_light_object.inc"
};
static Sint16 police_car_polygons[1029] = {
#include "shared/parked_car/police_car_polygons.inc"
};
static Sint32 police_car_vertices[723] = {
#include "shared/parked_car/police_car_vertices.inc"
};
static NJS_CNK_MODEL police_car_model = {
#include "shared/parked_car/police_car_model.inc"
};
static NJS_CNK_OBJECT police_car_object = {
#include "shared/parked_car/police_car_object.inc"
};
static NJS_TEXNAME dpt_car_texnames[8] = {
#include "shared/parked_car/dpt_car_texnames.inc"
};
static NJS_TEXLIST dpt_car_texlist = {
#include "shared/parked_car/dpt_car_texlist.inc"
};
static Sint16 dpt_car_beacon_polygons[27] = {
#include "shared/parked_car/dpt_car_beacon_polygons.inc"
};
static Sint32 dpt_car_beacon_vertices[19] = {
#include "shared/parked_car/dpt_car_beacon_vertices.inc"
};
static NJS_CNK_MODEL dpt_car_beacon_model = {
#include "shared/parked_car/dpt_car_beacon_model.inc"
};
static NJS_CNK_OBJECT dpt_car_beacon_object = {
#include "shared/parked_car/dpt_car_beacon_object.inc"
};
static Sint16 dpt_car_light_polygons[101] = {
#include "shared/parked_car/dpt_car_light_polygons.inc"
};
static Sint32 dpt_car_light_vertices[43] = {
#include "shared/parked_car/dpt_car_light_vertices.inc"
};
static NJS_CNK_MODEL dpt_car_light_model = {
#include "shared/parked_car/dpt_car_light_model.inc"
};
static NJS_CNK_OBJECT dpt_car_light_object = {
#include "shared/parked_car/dpt_car_light_object.inc"
};
static Sint16 dpt_car_polygons[861] = {
#include "shared/parked_car/dpt_car_polygons.inc"
};
static Sint32 dpt_car_vertices[635] = {
#include "shared/parked_car/dpt_car_vertices.inc"
};
static NJS_CNK_MODEL dpt_car_model = {
#include "shared/parked_car/dpt_car_model.inc"
};
static NJS_CNK_OBJECT dpt_car_object = {
#include "shared/parked_car/dpt_car_object.inc"
};
NJS_TEXLIST *parked_car_texlists[10] = {
#include "shared/parked_car/parked_car_texlists.inc"
};

/* dat_parked_car_0 to dat_parked_car_20 hold the values of stg13/car.c's
 * car_bob_gravity to car_engine_volume, in order. no code here reads them, nor
 * dat_parked_car_21, dat_parked_car_23 to dat_parked_car_27 or
 * parked_car_wobble_waves */
static Float dat_parked_car_0 = 0.052F;
static Float dat_parked_car_1 = 0.035F;
static Float dat_parked_car_2 = 0.3F;
static Float dat_parked_car_3 = 0.99F;
static Sint32 dat_parked_car_4 = 2;
static Float dat_parked_car_5 = 1.5F;
static Float dat_parked_car_6 = 1.0F;
static Float dat_parked_car_7 = 0.5F;
static Float dat_parked_car_8 = 3.0F;
static Float dat_parked_car_9 = 60.0F;
static Float dat_parked_car_10 = 0.5F;
/* while cars_come_back is set, a crash kills the car's task alone, so its
 * placement makes the car again. the flag also makes each car blink in over
 * its first BLINK_STEPS steps. no code sets the flag */
static Bool cars_come_back = FALSE;
static Float dat_parked_car_11 = 2.0F;
static Float dat_parked_car_12 = 0.3F;
static Float dat_parked_car_13 = 0.92F;
static Float dat_parked_car_14 = 0.9F;
static Float dat_parked_car_15 = 0.5F;
static Float dat_parked_car_16 = 0.2F;
static Float dat_parked_car_17 = 1.0F;
static Float dat_parked_car_18 = 2.0F;
static Float dat_parked_car_19 = 1.3F;
static Float crash_flash_scale = 6.0F;
static Sint32 dat_parked_car_20 = -20;
static Sint32 crash_volume = 60;
static Sint16 land_volume = 30;
/* the point the shadow's foot shrinks toward, at the foot's height */
static NJS_POINT3 shadow_focus = {0.0F, -200.0F, 0.0F};

/* a car's shapes are CAR_SHAPES spheres along its length, then its wreck's
 * one sphere. players, enemies and objects can't push the car's spheres.
 * car_shapes_small's wreck sphere is the smaller */
#define CAR_SHAPES 3
#define WRECK_SHAPES 1
/* clang-format off */
static struct collision_shape car_shapes[CAR_SHAPES + WRECK_SHAPES] = {
#include "shared/parked_car/car_shapes.inc"
};
static struct collision_shape car_shapes_small[CAR_SHAPES + WRECK_SHAPES] = {
#include "shared/parked_car/car_shapes_small.inc"
};
/* clang-format on */
/* the levels of a sphere that hurts enemies */
#define HURTING_LEVELS                                                         \
    (SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |               \
     SHAPE_HURTS_ENEMIES)
/* clang-format off */
/* the shapes of stg13/car.c's long cars */
static struct collision_shape dat_parked_car_21[6] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_IN_WORLD, {0.0F, 5.0F, 45.0F},
     43.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_IN_WORLD, {0.0F, 5.0F, 0.0F},
     43.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_IN_WORLD, {0.0F, 5.0F, -45.0F},
     43.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, 0, HURTING_LEVELS, SHAPE_HURTABLE | SHAPE_IN_WORLD,
     {0.0F, 5.0F, 45.0F}, 20.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, 0, HURTING_LEVELS, SHAPE_HURTABLE | SHAPE_IN_WORLD,
     {0.0F, 5.0F, 0.0F}, 20.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, 0, HURTING_LEVELS, SHAPE_HURTABLE | SHAPE_IN_WORLD,
     {0.0F, 5.0F, -45.0F}, 20.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* TRUE once parked_car_build has compiled every kind's build */
static Bool kinds_built = FALSE;
/* clang-format off */
static struct car_kind car_kinds[CAR_KINDS] = {
#include "shared/parked_car/car_kinds.inc"
};
/* clang-format on */
/* the five cars without beacons, whose vertex colors parked_car_build tints
 * by CAR_TINT */
static NJS_CNK_OBJECT *tinted_cars[5] = {
#include "shared/parked_car/tinted_cars.inc"
};
/* the waves that rock stg13/car.c's driving cars about x, y and z, a row
 * each */
/* clang-format off */
static struct wave parked_car_wobble_waves[9] = {
    {24, 0.2F}, {14, 0.3F}, {3, 0.2F},
    {30, 0.09F}, {14, 0.08F}, {1, 0.05F},
    {29, 0.3F}, {9, 0.2F}, {2, 0.02F},
};
/* clang-format on */
/* a crash's wreck starts at the speed crash_spd. it is turned about x by half
 * the car's ang.x, then about y by its ang.y */
static NJS_VECTOR crash_spd = {-0.5F, 2.3F, -0.01F};
/* a bounce off a side adds a random turn to the wreck's spin about each
 * axis. the turn is up to half the axis's value either way, in degrees a
 * step */
static Float bounce_spin_x = 1.5F;
static Float bounce_spin_y = 0.1F;
static Float bounce_spin_z = 1.5F;
/* stg13/car.c's car_lift_slack */
static Float dat_parked_car_23 = 1.0F;
/* each glow's half width and half height */
static Float glow_half_size = 8.0F;
static Float dat_parked_car_24 = 2.0F;
static NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};
/* where a car of each kind with beacons draws its lamps' glows, in the car's
 * frame */
static NJS_POINT3 police_car_lamps[2] = {
#include "shared/parked_car/police_car_lamps.inc"
};
static NJS_POINT3 dpt_car_lamps[2] = {
#include "shared/parked_car/dpt_car_lamps.inc"
};
/* clang-format off */
static struct collision_shape dat_parked_car_25[1] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, HURTING_LEVELS, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 18.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* what each step takes from the wreck's spd.y */
static Float gravity = 0.1F;
static NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
/* stg13/car.c's car_upside_limit */
static Sint32 dat_parked_car_26 = 160;
/* a wreck turning toward its rest turns each step by the angle left over a
 * turn divisor. it adds that angle over a spin divisor to its spin. a wreck
 * on its roof near its floor takes the upside_ divisors. one that lands on its
 * wheels after a CRASH_HIT or harder takes the land_ ones */
static Float upside_turn_divisor = 70.0F;
static Float upside_spin_divisor = 80.0F;
/* stg13/car.c's car_land_limit */
static Sint32 dat_parked_car_27 = 60;
static Float land_turn_divisor = 90.0F;
static Float land_spin_divisor = 35.0F;
/* the shares of its spin a wreck keeps. on a step it falls onto its wheels, it
 * keeps land_spin_keep after a CRASH_HIT or harder. it keeps all of its spin
 * after a CRASH_NUDGE. on any other step that touches its floor, it keeps
 * skim_spin_keep. on each step it settles on its roof, it keeps
 * upside_spin_keep */
static Float skim_spin_keep = 0.995F;
static Float upside_spin_keep = 0.95F;
static Float land_spin_keep = 0.1F;
#define SHADOW_TOP 3.5F
#define SHADOW_SHRINK 0.12F
#define SHADOW_SLANT 70.0F
#define SHADOW_REACH 200.0F
#define SHADOW_CORNERS 4

/* draw a shadow under the current matrix, as a modifier volume. its top is a
 * 2 * half_x by 2 * half_z rectangle, SHADOW_TOP up. ang turns it about z,
 * then x, then y. its foot is the top shrunk to SHADOW_SHRINK
 * of its size about shadow_focus, at shadow_focus's height. the foot then
 * moves SHADOW_SLANT along x and along z. the volume is a copy of
 * modifier_volume_box_model at WORK_AREA, over what was there. the call never
 * returns where NJD_CE doesn't end the box's vertices */
static void parked_car_draw_shadow(Float half_x, Float half_z,
                                   struct angles *ang)
{
    Float vx;
    Float vy;
    Float vz;
    NJS_POINT3 corners[SHADOW_CORNERS];
    NJS_POINT3 points[SHADOW_CORNERS];
    NJS_CNK_MODEL model;
    Sint32 *src;
    Sint32 *dst;
    Sint32 word;
    /* @hack redundant: count a register int, where an int would do */
    register int count;
    int top;
    int bottom;
    Float radius;

    radius =
        (njSqrt((half_x * half_x) + (half_z * half_z)) * 2.0F) + SHADOW_REACH;
    /* @hack type: the box's model read through
     * modifier_volume_box_model_array, its second name, where
     * modifier_volume_box_model would do */
    model = *modifier_volume_box_model_array;
    model.vlist = (Sint32 *)WORK_AREA;
    model.r = radius;
    corners[0].x = -half_x;
    corners[0].y = SHADOW_TOP;
    corners[0].z = -half_z;
    corners[1].x = -half_x;
    corners[1].y = SHADOW_TOP;
    corners[1].z = half_z;
    corners[2].x = half_x;
    corners[2].y = SHADOW_TOP;
    corners[2].z = -half_z;
    corners[3].x = half_x;
    corners[3].y = SHADOW_TOP;
    corners[3].z = half_z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateEx(&ang->x, ZXY_TURN);
    njCalcPoints(NULL, corners, points, SHADOW_CORNERS);
    njPopMatrixEx();
    src = modifier_volume_box_model_array->vlist;
    dst = model.vlist;
    *dst++ = *src++;
    /* @hack spelling: the chunk's second word copied and kept in one chained
     * assignment, where two statements would do */
    word = *dst++ = *src++;
    count = (Uint16)(word >> CHUNK_HIGH_SHIFT);
    top = 0;
    bottom = 0;
    while (count--) {
        vy = ((NJS_POINT3 *)src)->y;
        src += CHUNK_V_WORDS;
        if (vy > 0.0F) {
            /* @hack spelling: the corner read x, z and y, where x, y and z
             * would do */
            vx = points[top].x;
            vz = points[top].z;
            vy = points[top].y;
            top++;
        } else {
            /* @hack spelling: vx and vz read from the corner first, where the
             * corner read in the sums would do */
            vx = points[bottom].x;
            vz = points[bottom].z;
            vx = ((vx - shadow_focus.x) * SHADOW_SHRINK) + shadow_focus.x +
                 SHADOW_SLANT;
            vz = ((vz - shadow_focus.z) * SHADOW_SHRINK) + shadow_focus.z +
                 SHADOW_SLANT;
            vy = shadow_focus.y;
            bottom++;
        }
        /* @hack type: each Float stored through a Sint32 view of itself,
         * where a store through a Float pointer would do */
        *dst++ = *(Sint32 *)&vx;
        *dst++ = *(Sint32 *)&vy;
        *dst++ = *(Sint32 *)&vz;
        top &= SHADOW_CORNERS - 1;
        bottom &= SHADOW_CORNERS - 1;
    }
    /* @hack spelling: the end word copied and kept in one chained assignment,
     * where two statements would do */
    word = *dst = *src;
    while (word != NJD_CE) { /* NOLINT(bugprone-infinite-loop) */
    }
    njCnkModDrawModel(&model);
}

/* return the slot of the player collision_next_player finds touching the car,
 * or -1 for none. *force takes the force of the crash the touches make, or -1
 * where they make none. player_slot_of must not give -1 for that player's
 * task */
static int parked_car_find_hit(struct task *task, int *force)
{
    struct task_work *work = task->work;
    int player = -1;
    struct task *hit;
    struct task_work *player_task_work;
    struct player *record;

    *force = -1;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((hit = collision_next_player(task)) != NULL) {
        player = player_slot_of(hit);
        player_task_work = player_work[player];
        record = player_records[player];
        if ((record->character == PLAYER_SONIC ||
             record->character == PLAYER_SHADOW) &&
            player_task_work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
            player_task_work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
            if (record->upgrades & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |
                                    UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
                *force = CRASH_FLAME_HIT;
            } else {
                *force = CRASH_HIT;
            }
        } else if ((record->character == PLAYER_SONIC ||
                    record->character == PLAYER_SHADOW) &&
                   player_task_work->mode == PLAYER_MODE_ROLL) {
            *force = CRASH_NUDGE;
        } else if (collision_first_with_id(task, MECH_STRIKE_SHAPE)) {
            *force = CRASH_HIT;
        }
    } else if (collision_next_attack(task)) {
        *force = CRASH_NUDGE;
    } else if (work->flags & WORK_HURT) {
        *force = CRASH_HIT;
    }
    return player;
}

#define CAR_TINT 0xffffecc8

static void parked_car_tint_models(void)
{
    int i;

    /* @hack type: i an int compared as unsigned, where a Uint32 would do */
    for (i = 0; (Uint32)i < sizeof tinted_cars / sizeof tinted_cars[0]; i++) {
        object_tree_tint(tinted_cars[i], CAR_TINT);
    }
}

Uint8 *parked_car_build(Uint8 *buf)
{
    int i;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *texlist;

    parked_car_tint_models();
    if (!kinds_built) {
        kinds_built = TRUE;
        for (i = 0; i < CAR_KINDS; i++) {
            /* @hack spelling: object and texlist, locals for the kind's
             * fields, where the fields read in place would do */
            object = car_kinds[i].object;
            texlist = car_kinds[i].texlist;
            if (object && texlist) {
                njSetTexture(texlist);
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((car_kinds[i].build = object_build(&buf, object)) == NULL) {
                    kinds_built = FALSE;
                }
            }
        }
    }
    return buf;
}

/* parked_car_random_kind picks a random whole number under RANDOM_PICKS, then
 * takes it modulo KIND_RANDOM_COUNT */
#define RANDOM_PICKS 100.0F

static int parked_car_random_kind(void)
{
    Float pick;
    int kind;

    /* @hack spelling: pick and kind, locals for the random number, where one
     * expression would do */
    pick = rand() * (RANDOM_PICKS * RAND_SCALE);
    kind = pick;
    return (kind % KIND_RANDOM_COUNT) + KIND_RANDOM_FIRST;
}

/* give each of task's shapes the center of kind's shape of the same index,
 * placed by a matrix. then fit the record's radius as collision_fit_radius
 * does. the matrix is the current one, or under own_matrix that of the work's
 * pos and ang alone. kind must hold at least as many shapes as the record */
static void parked_car_place_shapes(struct task *task, Bool own_matrix,
                                    struct car_kind *kind)
{
    struct task_work *work = task->work;
    int i;

    if (own_matrix) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateEx(&work->ang.x, ZXY_TURN);
    }
    for (i = 0; i < work->collision->count; i++) {
        njCalcPoint(NULL, &kind->shapes[i].center,
                    &work->collision->shapes[i].center);
    }
    if (own_matrix) {
        njPopMatrixEx();
    }
    collision_fit_radius(task);
}

#define CAR_SINK (-1.5F)
/* a siren of any submode but 0 never leaves range */
#define SIREN_STAYS 1

static void parked_car_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct task *siren;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct car));
    if (!task->motion.p) {
        return;
    }
    if ((int)(work->scl.y + TRUNCATE_SLACK) == KIND_RANDOM) {
        work->submode |= CAR_RANDOM;
    }
    if (work->submode & CAR_RANDOM) {
        work->scl.y = parked_car_random_kind();
    }
    if (KIND(work)->flags & CAR_NO_PUSH) {
        work->submode |= CAR_NO_PUSH_OUT;
    }
    /* @hack spelling: draw set before step, not in the members' order */
    task->draw = parked_car_draw;
    task->step = parked_car_step;
    task->destructor = parked_car_destroy;
    /* @hack type: scl.x converted to a Uint32 for the test, where an int would
     * do */
    if ((Uint32)(work->scl.x + TRUNCATE_SLACK) != 0) {
        work->submode |= CAR_NO_PUSH_OUT;
    }
    CAR(task)->height = CAR_SINK;
    collision_init(task, KIND(work)->shapes, KIND(work)->shape_count,
                   COLLISION_OBJECT);
    /* @bug memory: goes on where collision_init fails, which leaves the work's
     * collision null. parked_car_place_shapes then reads the record's count
     * through null, at 0x4c8 of the object's code. parked_car_step tests the
     * record at 0x7ec, but reads its flags untested at 0xc06 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init fails, the setup
     * destroys the car, and set.c makes it again on a later pass */
    if (work->collision == NULL) {
        task_destroy(task);
        return;
    }
#endif
    parked_car_place_shapes(task, TRUE, KIND(work));
    if (KIND(work)->flags & CAR_POLICE) {
        siren = task_create_child(TASK_WORK, siren_init, task);
        if (siren) {
            siren->work->pos = work->pos;
            siren->work->submode = SIREN_STAYS;
        }
    }
}

static void parked_car_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place would
     * do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

/* @hack spelling: VECTOR_SUM to VECTOR_SCALE walk the parts through
 * pointers, where three assignments would do */
#define VECTOR_SUM(out, a, b)                                                  \
    {                                                                          \
        Float *pd = &(out)->x;                                                 \
        Float *pa = &(a)->x;                                                   \
        Float *pb = &(b)->x;                                                   \
        *pd++ = *pa++ + *pb++;                                                 \
        *pd++ = *pa++ + *pb++;                                                 \
        *pd = *pa + *pb;                                                       \
    }
#define VECTOR_DIFFERENCE(out, a, b)                                           \
    {                                                                          \
        Float *pd = &(out)->x;                                                 \
        Float *pa = &(a)->x;                                                   \
        Float *pb = &(b)->x;                                                   \
        *pd++ = *pa++ - *pb++;                                                 \
        *pd++ = *pa++ - *pb++;                                                 \
        *pd = *pa - *pb;                                                       \
    }
#define VECTOR_SUB(v, sub)                                                     \
    {                                                                          \
        Float *pd = &(v)->x;                                                   \
        Float *pa = &(sub)->x;                                                 \
        *pd++ -= *pa++;                                                        \
        *pd++ -= *pa++;                                                        \
        *pd -= *pa;                                                            \
    }
#define VECTOR_ADD(v, add)                                                     \
    {                                                                          \
        Float *pd = &(v)->x;                                                   \
        Float *pa = &(add)->x;                                                 \
        *pd++ += *pa++;                                                        \
        *pd++ += *pa++;                                                        \
        *pd += *pa;                                                            \
    }
#define VECTOR_SCALE(k, v)                                                     \
    {                                                                          \
        Float *pd = &(v)->x;                                                   \
        *pd++ *= (k);                                                          \
        *pd++ *= (k);                                                          \
        *pd *= (k);                                                            \
    }

/* @hack spelling: vector_add, an inline function that walks the parts
 * through pointers, where three assignments would do */
#pragma inline(vector_add)
static void vector_add(NJS_VECTOR *vector, NJS_VECTOR *add)
{
    Float *dst = &vector->x;
    Float *src = &add->x;

    *dst++ += *src++;
    *dst++ += *src++;
    *dst += *src;
}

/* a car's work's tick counts its steps, up to TICK_MAX. no touch crashes the
 * car until its tick passes CRASH_TICK */
#define TICK_MAX 125
#define CRASH_TICK 20
/* the box's half height is BOX_RAISE more than the kind's */
#define BOX_RAISE 10.0F
/* the wreck gains HITTER_SPEED_PART of the speed of the task in the car's
 * first hit. it also gains HITTER_LIFT along y. where that task is null or has
 * no motion, the wreck gains CRASH_LIFT along y alone */
#define HITTER_SPEED_PART 0.3F
#define HITTER_LIFT 0.7F
#define CRASH_LIFT 1.1F
/* a wreck whose squared speed in x and z lies below CRASH_SLOW takes a random
 * speed there. a CRASH_NUDGE keeps NUDGE_LIFT_PART of the wreck's speed along
 * y. every wreck rises at CRASH_MIN_LIFT or faster */
#define CRASH_SLOW 0.5F
#define NUDGE_LIFT_PART 0.7F
#define CRASH_MIN_LIFT 2.0F
/* a player's crash makes a flash FLASH_MARGIN beyond the radius of the
 * player's first shape, toward the car. the flash throws CRASH_SPARKS sparks a
 * step */
#define FLASH_MARGIN 5.0F
#define CRASH_SPARKS 0
/* a player's crash leaves the player PLAYER_SPEED_PART of its speed, and
 * PLAYER_HOP more along y */
#define PLAYER_SPEED_PART 0.8F
#define PLAYER_HOP 0.6F
#define SHAKE_RANGE 300.0F
#define SHAKE_POWER 0.3F
/* a car pushes only the players in slots 0 and 1 */
#define CAR_PLAYERS 2

/* the car pushes each player in its box out through a side. the test takes
 * the player's center a step of its speed ahead. the player's record's push
 * takes the move to the side, with any move down cut to 0. the step judges a
 * crash only while a player lies in the box or none touches the car */
static void parked_car_step(struct task *task)
{
    struct task_work *work = task->work;
    struct car_kind *kind = KIND(work);
    Bool inside;
    Angle pitch;
    Sint16 *low_half;
    struct task *player_task;
    int i;
    int force;
    int player;
    Float ax;
    Float ay;
    Float az;
    Float scale;
    /* @hack redundant: unused_0 and unused_1, locals nothing reads, which size
     * the frame */
    NJS_VECTOR unused_0;
    NJS_VECTOR unused_1;

    /* @hack spelling: an else after the return, where the child's test on its
     * own would do */
    if (placement_try_leave(task)) {
        return;
    } else if (task->child) {
        task->child->work->pos = work->pos;
        task->child->work->submode = SIREN_STAYS;
    }
    if (work->tick < TICK_MAX) {
        work->tick++;
    }
    if (!(work->submode & (CAR_HIDDEN | CAR_UNSEEN))) {
        if (work->collision) {
            for (i = 0; i < work->collision->count; i++) {
                work->collision->shapes[i].flags &= ~SHAPE_OFF;
            }
        }
        inside = FALSE;
        {
            NJS_VECTOR box;

            box = kind->box;
            box.y += BOX_RAISE;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateZXY(NULL, work->ang.x, work->ang.y, work->ang.z);
            /* @hack spelling: the kind found again, where kind would do */
            parked_car_place_shapes(task, FALSE, KIND(work));
            njInvertMatrix(NULL);
            for (i = 0; i < CAR_PLAYERS; i++) {
                if (player_work[i]) {
                    NJS_VECTOR next;
                    NJS_POINT3 out;
                    NJS_VECTOR push;

                    VECTOR_SUM(
                        &next, &player_work[i]->collision->shapes[0].center,
                        &((struct player_motion *)player_tasks[i]->motion.p)
                             ->spd);
                    njCalcPoint(NULL, &next, &next);
                    ax = _builtin_fabsf(next.x);
                    ay = _builtin_fabsf(next.y);
                    az = _builtin_fabsf(next.z);
                    if (box.x > ax && box.y > ay && box.z > az) {
                        if ((!(work->submode & CAR_NEARER_SIDE) &&
                             next.z > 0.0F) ||
                            box.x - ax < box.z - az) {
                            if (!(work->submode & CAR_NO_PUSH_OUT)) {
                                next.x = next.x > 0.0F ? box.x : -box.x;
                                njPushMatrixEx();
                                njInvertMatrix(NULL);
                                njCalcPoint(NULL, &next, &out);
                                njPopMatrixEx();
                                VECTOR_SUB(&out, &player_work[i]
                                                      ->collision->shapes[0]
                                                      .center);
                                VECTOR_SUB(
                                    &out,
                                    &((struct player_motion *)player_tasks[i]
                                          ->motion.p)
                                         ->spd);
                                VECTOR_ADD(&out, &player_work[i]->pos);
                                VECTOR_DIFFERENCE(&push, &out,
                                                  &player_work[i]->pos);
                                if (push.y < 0.0F) {
                                    push.y = 0.0F;
                                }
                                player_records[i]->push = push;
                            }
                            inside = TRUE;
                        } else {
                            if (!(work->submode & CAR_NO_PUSH_OUT)) {
                                next.z = next.z > 0.0F ? box.z : -box.z;
                                njPushMatrixEx();
                                njInvertMatrix(NULL);
                                njCalcPoint(NULL, &next, &out);
                                njPopMatrixEx();
                                VECTOR_SUB(&out, &player_work[i]
                                                      ->collision->shapes[0]
                                                      .center);
                                VECTOR_SUB(
                                    &out,
                                    &((struct player_motion *)player_tasks[i]
                                          ->motion.p)
                                         ->spd);
                                VECTOR_ADD(&out, &player_work[i]->pos);
                                VECTOR_DIFFERENCE(&push, &out,
                                                  &player_work[i]->pos);
                                if (push.y < 0.0F) {
                                    push.y = 0.0F;
                                }
                                player_records[i]->push = push;
                            }
                            inside = TRUE;
                        }
                    }
                }
            }
        }
        njPopMatrixEx();
        if (work->tick > CRASH_TICK &&
            work->collision->flags & COLLISION_TOUCHED &&
            (inside || !collision_next_player(task))) {
            player = parked_car_find_hit(task, &force);
            if (force < 0 && (player < 0 || work->submode & CAR_NO_PUSH_OUT)) {
                force = CRASH_NUDGE;
            }
            if (force >= 0) {
                NJS_VECTOR spd;

                pitch = work->ang.x;
                /* @hack type: the low half of pitch halved through a Sint16
                 * view and a Float local, where masks would do */
                low_half = (Sint16 *)&pitch;
                {
                    Float low = *low_half;

                    *low_half = low * 0.5F;
                }
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateY(NULL, work->ang.y);
                njRotateX(NULL, pitch);
                njCalcVector(NULL, &crash_spd, &spd);
                njPopMatrixEx();
                if (work->collision->hits[0].task &&
                    work->collision->hits[0].task->motion.p) {
                    NJS_VECTOR hitter_spd;

                    hitter_spd =
                        TASK_MOTION_OF(work->collision->hits[0].task)->spd;
                    VECTOR_SCALE(HITTER_SPEED_PART, &hitter_spd);
                    VECTOR_ADD(&spd, &hitter_spd);
                    spd.y += HITTER_LIFT;
                } else {
                    spd.y += CRASH_LIFT;
                }
                if ((spd.x * spd.x) + (spd.z * spd.z) < CRASH_SLOW) {
                    spd.x = RANDOM_HALF();
                    spd.z = RANDOM_HALF();
                }
                if (force == CRASH_NUDGE) {
                    spd.y *= NUDGE_LIFT_PART;
                }
                spd.y = _builtin_fabsf(spd.y);
                if (spd.y < CRASH_MIN_LIFT) {
                    spd.y = CRASH_MIN_LIFT;
                }
                spd.x += RANDOM_HALF();
                spd.z += RANDOM_HALF();
                if (player >= 0) {
                    NJS_VECTOR to_car;
                    NJS_POINT3 flash_pos;
                    struct player_motion *motion;

                    score_add(CRASH_SCORE);
                    /* @hack spelling: the difference through walking
                     * pointers, where three assignments would do */
                    /* @hack spelling: pd and pb stepped in statements of
                     * their own after x, where steps in the loads would
                     * do */
                    {
                        Float *pd = &to_car.x;
                        Float *pa = &work->pos.x;
                        Float *pb =
                            &player_work[player]->collision->shapes[0].center.x;

                        *pd = *pa++ - *pb;
                        pd++;
                        pb++;
                        *pd++ = *pa++ - *pb++;
                        *pd = *pa - *pb;
                    }
                    scale = (player_work[player]->collision->shapes[0].radius +
                             FLASH_MARGIN) /
                            njScalor(&to_car);
                    VECTOR_SCALE(scale, &to_car);
                    /* @hack spelling: the sum through walking pointers,
                     * where three assignments would do */
                    /* @hack spelling: the cursors started from
                     * (&flash_pos)->x and the like, where &flash_pos.x would
                     * do */
                    /* @hack spelling: pd and pa stepped in statements of
                     * their own after x, where steps in the loads would
                     * do */
                    {
                        Float *pd = &(&flash_pos)->x;
                        Float *pa = &(&to_car)->x;
                        Float *pb =
                            &(&player_work[player]->collision->shapes[0].center)
                                 ->x;

                        *pd = *pa + *pb++;
                        pd++;
                        pa++;
                        *pd++ = *pa++ + *pb++;
                        *pd = *pa + *pb;
                    }
                    flash_manager_create_sparks(flash_pos.x, flash_pos.y,
                                                flash_pos.z, crash_flash_scale,
                                                CRASH_SPARKS);
                    /* @hack redundant: player_task, which has its address
                     * taken and which nothing reads */
                    (void)&player_task;
                    /* @hack spelling: the assignment and the call joined by a
                     * comma, where two statements would do */
                    motion = (struct player_motion *)(player_task =
                                                          player_tasks[player])
                                 ->motion.p,
                    player_set_speed(player, motion->spd.x * PLAYER_SPEED_PART,
                                     (motion->spd.y * PLAYER_SPEED_PART) +
                                         PLAYER_HOP,
                                     motion->spd.z * PLAYER_SPEED_PART);
                }
                parked_car_wreck_create(kind, &work->pos, &work->ang, &spd,
                                        &work->pos, &work->ang, force, 0);
                sound_play_at(rand() * RAND_SCALE > 0.5F ? CRASH_SOUND
                                                         : CRASH_SOUND_2,
                              NULL, SOUND_PRIORITY, crash_volume, &work->pos);
                task->draw = NULL;
                if (!cars_come_back && task->placement) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
                shake_create(&work->pos, SHAKE_RANGE, SHAKE_POWER);
            } else {
                collision_register(task);
            }
        } else {
            collision_register(task);
        }
    } else if (work->collision) {
        for (i = 0; i < work->collision->count; i++) {
            work->collision->shapes[i].flags |= SHAPE_OFF;
        }
        collision_register(task);
    }
}

/* draw object and its sibling, which must not be null, each turned about x
 * by ang */
static void parked_car_wreck_draw_lights(NJS_CNK_OBJECT *object, Angle ang)
{
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateX(NULL, ang);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    object = object->sibling;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateX(NULL, ang);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

/* each channel of a 32-bit color runs from 0 to CHANNEL_MAX */
#define CHANNEL_MAX 255
#define WHITE 0xffffff

/* the color of a glow whose beam points along dir. *depth takes the absolute
 * z of dir as the current matrix turns it. *color takes WHITE under an alpha
 * of *depth times CHANNEL_MAX, up to CHANNEL_MAX */
static void parked_car_glow_color(NJS_VECTOR *dir, Float *depth, Uint32 *color)
{
    NJS_VECTOR turned;
    Float z;
    int alpha;

    njCalcVector(NULL, dir, &turned);
    z = _builtin_fabsf(turned.z);
    *depth = z;
    alpha = z * CHANNEL_MAX;
    if (alpha < 0) {
        alpha = 0;
    }
    if (alpha > CHANNEL_MAX) {
        alpha = CHANNEL_MAX;
    }
    /* @hack spelling: alpha reused for the color, where the color stored in
     * place would do */
    alpha = (alpha << ALPHA_SHIFT) + WHITE;
    *color = alpha;
}

/* parked_car_glow_color, but *depth takes the turned z itself, or 0 where it
 * lies below 0 */
static void parked_car_glow_color_ahead(NJS_VECTOR *dir, Float *depth,
                                        Uint32 *color)
{
    NJS_VECTOR turned;
    Float z;
    int alpha;

    njCalcVector(NULL, dir, &turned);
    z = turned.z;
    if (z < 0.0F) {
        z = 0.0F;
    }
    *depth = z;
    alpha = z * CHANNEL_MAX;
    if (alpha < 0) {
        alpha = 0;
    }
    if (alpha > CHANNEL_MAX) {
        alpha = CHANNEL_MAX;
    }
    /* @hack spelling: alpha reused for the color, where the color stored in
     * place would do */
    alpha = (alpha << ALPHA_SHIFT) + WHITE;
    *color = alpha;
}

/* while cars_come_back is set, a car draws on none of its first BLINK_HIDDEN
 * steps. it then draws on every other step up to BLINK_STEPS */
#define BLINK_HIDDEN 10
#define BLINK_STEPS 20
/* a car's view radius is its wreck model's radius plus VIEW_MARGIN. the car
 * draws only while its pos lies no more than its view radius behind the camera.
 * the pos must also lie no more than VIEW_FAR ahead */
#define VIEW_MARGIN 5.0F
#define VIEW_FAR 700.0F
/* while the camera lies within a car's view radius, the car draws
 * see-through, with NEAR_COLOR's alpha. it then draws neither its light nor
 * its glows */
#define NEAR_COLOR 0x60ffffff
/* the beacons' turn, in Angle units a step */
#define BEACON_TURN 1280
/* a lamp's glow turns LAMP_TURN for each unit of the lamp's distance from the
 * camera. that distance counts only LAMP_Z_PART of the lamp's depth. the glows
 * draw GLOW_DEPTH nearer, as particle_draw_quad's depth */
#define LAMP_Z_PART 0.001F
#define LAMP_TURN (3.0F * ANGLE_PER_DEGREE)
#define GLOW_DEPTH 10.0F
/* the glows' textures: the beacons' in their car's list, and the lamps' in
 * glow_texlist */
#define BEACON_0_TEXTURE 0
#define BEACON_1_TEXTURE 1
#define LAMP_TEXTURE 0

/* draw the car over its shadow. a car with beacons also draws its light and
 * the glows of its beacons and lamps. a beacon's glow is brightest where its
 * turning beam points along the line of sight. a lamp's glow is brightest
 * seen from ahead of the car */
static void parked_car_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct car_kind *kind = KIND(work);
    /* @hack redundant: unused, a local nothing reads, which sizes the frame */
    char unused[12];
    NJS_CNK_OBJECT *object;
    Uint32 color;
    Float depth;
    Angle spin;
    NJS_POINT3 *lamps;
    Angle turn_0;
    Angle turn_1;
    Float radius;
    Float z;
    Bool near;

    if (cars_come_back) {
        if (work->tick < BLINK_HIDDEN) {
            return;
        }
        if (work->tick < BLINK_STEPS && game_state.steps & 1) {
            return;
        }
    }
    if (work->submode & CAR_HIDDEN) {
        return;
    }
    /* @hack redundant: a test of kind, which KIND never makes null */
    if (!kind) {
        return;
    }
    {
        NJS_POINT3 pos;

        njCalcPoint(NULL, &work->pos, &pos);
        z = pos.z;
        radius = kind->wreck_object->model->r + VIEW_MARGIN;
        if (z < -radius || z > VIEW_FAR) {
            work->submode |= CAR_UNSEEN;
            return;
        }
        work->submode &= ~CAR_UNSEEN;
        /* @hack spelling: near set in an if and an else, where the comparison
         * assigned would do */
        if (njScalor2(&pos) < radius * radius) {
            near = TRUE;
        } else {
            near = FALSE;
        }
    }
    njPushMatrixEx();
    /* @hack spelling: radius reused for the record's height, where the height
     * read in place would do */
    radius = CAR(task)->height;
    object = kind->object;
    if (near) {
        struct angles ang;

        ang.x = work->ang.x + CAR(task)->ang.x;
        ang.y = work->ang.y + CAR(task)->ang.y;
        ang.z = work->ang.z + CAR(task)->ang.z;
        njTranslate(NULL, work->pos.x,
                    work->pos.y + radius + CAR(task)->lift + DRAW_RISE,
                    work->pos.z);
        parked_car_draw_shadow(kind->shadow_x, kind->shadow_z, &ang);
        njRotateEx(&ang.x, ZXY_TURN);
        njSetTexture(kind->texlist);
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        /* @hack spelling: the model read through kind, where object would
         * do */
        model_draw_tinted(kind->object->model, NEAR_COLOR);
        draw_attr_restore();
        draw_control_restore();
    } else {
        struct angles ang;
        NJS_VECTOR beam_0;
        NJS_VECTOR beam_1;
        /* @hack redundant: beacon, which takes each beacon's place and which
         * nothing reads */
        NJS_POINT3 beacon;
        NJS_VECTOR back;
        NJS_POINT3 lamp_0;
        NJS_POINT3 lamp_1;

        /* @hack redundant: a block no path reaches, which leaves its strings in
         * the object */
        if (0) {
            (void)CAR_EDITOR_MODE_NOPUSHOUT;
            (void)CAR_EDITOR_MODE_PUSHOUT;
            (void)CAR_EDITOR_CAR_TYPE;
        }
        ang.x = work->ang.x + CAR(task)->ang.x;
        ang.y = work->ang.y + CAR(task)->ang.y;
        ang.z = work->ang.z + CAR(task)->ang.z;
        njTranslate(NULL, work->pos.x,
                    work->pos.y + radius + CAR(task)->lift + DRAW_RISE,
                    work->pos.z);
        parked_car_draw_shadow(kind->shadow_x, kind->shadow_z, &ang);
        njRotateEx(&ang.x, ZXY_TURN);
        njSetTexture(kind->texlist);
        if (kind->build) {
            njDirectDrawObject(kind->build);
        } else {
            /* @hack spelling: the model read through kind, where object
             * would do */
            njCnkEasyDrawModel(kind->object->model);
        }
        object = object->child;
        if (object && kind->flags & CAR_BEACONS) {
            beam_0.x = 0.0F;
            beam_0.y = 0.0F;
            beam_0.z = 1.0F;
            beam_1.x = 1.0F;
            beam_1.y = 0.0F;
            beam_1.z = 0.0F;
            spin = game_state.steps * BEACON_TURN;
            njFogDisable();
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)object->pos);
            njCnkEasyDrawModel(object->model);
            matrix_load_parent();
            njUnitMatrix(NULL);
            njRotateY(NULL, spin);
            njCalcVector(NULL, &beam_0, &beam_0);
            njCalcVector(NULL, &beam_1, &beam_1);
            matrix_load_parent();
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            /* @hack redundant: a second test of object, which the test above
             * made */
            if (object && (object = object->sibling) != NULL) {
                beacon = *(NJS_POINT3 *)object->pos;
                /* @hack spelling: beacon.y added -1, where 1 taken from it
                 * would do */
                beacon.y += -1.0F;
                parked_car_glow_color(&beam_0, &depth, &color);
                njTranslateEx((NJS_VECTOR *)object->pos);
                particle_draw_quad(&origin, BEACON_0_TEXTURE, QUARTER_TURN, 0,
                                   glow_half_size, glow_half_size, color,
                                   PARTICLE_BLEND_ADD | PARTICLE_BASE,
                                   GLOW_DEPTH, NO_SIZE_LIMIT);
                matrix_load_parent();
            }
            if (object && (object = object->sibling) != NULL) {
                beacon = *(NJS_POINT3 *)object->pos;
                /* @hack spelling: beacon.y added -1, where 1 taken from it
                 * would do */
                beacon.y += -1.0F;
                parked_car_glow_color(&beam_1, &depth, &color);
                njTranslateEx((NJS_VECTOR *)object->pos);
                particle_draw_quad(&origin, BEACON_1_TEXTURE, QUARTER_TURN, 0,
                                   glow_half_size, glow_half_size, color,
                                   PARTICLE_BLEND_ADD | PARTICLE_BASE,
                                   GLOW_DEPTH, NO_SIZE_LIMIT);
                matrix_load_parent();
            }
            njPopMatrixEx();
            if (kind->flags & CAR_DPT) {
                lamps = dpt_car_lamps;
            } else {
                lamps = police_car_lamps;
            }
            njCalcPoint(NULL, &lamps[0], &lamp_0);
            njCalcPoint(NULL, &lamps[1], &lamp_1);
            lamp_0.z *= LAMP_Z_PART;
            lamp_1.z *= LAMP_Z_PART;
            turn_0 = njScalor(&lamp_0) * LAMP_TURN;
            turn_1 = njScalor(&lamp_1) * LAMP_TURN;
            back.x = 0.0F;
            back.y = 0.0F;
            back.z = -1.0F;
            njSetTexture(&glow_texlist);
            parked_car_glow_color_ahead(&back, &depth, &color);
            particle_draw_quad(&lamps[0], LAMP_TEXTURE, QUARTER_TURN, turn_0,
                               glow_half_size, glow_half_size, color,
                               PARTICLE_BLEND_ADD, GLOW_DEPTH, NO_SIZE_LIMIT);
            particle_draw_quad(&lamps[1], LAMP_TEXTURE, QUARTER_TURN, turn_1,
                               glow_half_size, glow_half_size, color,
                               PARTICLE_BLEND_ADD, GLOW_DEPTH, NO_SIZE_LIMIT);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
            njFogEnable();
        }
    }
    njPopMatrixEx();
}

void parked_car_init(struct task *task)
{
    struct task_work *work = task->work;

    work->submode = CAR_NEARER_SIDE;
    task->step = parked_car_setup;
}

/* a wreck's spin makes a random whole count of turns about each axis. the
 * count is under HALF_TURNS_X half turns about x, and HALF_TURNS_Z about z. it
 * is under TURNS_Y whole turns about y. a toss counts under the TOSS_ bounds
 * instead. x or z takes a half turn more, at random, where their sum is even. x
 * takes it where it has none */
#define HALF_TURNS_X 3.0F
#define TURNS_Y 2.5F
#define HALF_TURNS_Z 2.0F
#define TOSS_HALF_TURNS_X 1.5F
#define TOSS_TURNS_Y 1.25F
#define TOSS_HALF_TURNS_Z 1.1F
/* each turn about x or z falls short of its count by SPIN_SHORT Angle units.
 * it falls short by up to SPIN_SLACK times RAND_MAX more, at random. the turn
 * about y falls short by a random amount up to SPIN_SLACK_Y times RAND_MAX. a
 * negative count's turn runs long by the same amounts */
#define SPIN_SHORT 0x400
#define SPIN_SLACK (1.0F / 12.0F)
#define SPIN_SLACK_Y (1.0F / 18.0F)

/* set the wreck's spin. the wreck makes its turns while it falls back to its
 * starting height in its floor's frame. the half turns about x and z
 * sum to an odd count, so the wreck turns over. after a CRASH_NUDGE, the
 * wreck makes half a turn about z alone, either way at random. the one call
 * passes toss FALSE, so the toss bounds never apply */
static void parked_car_wreck_set_spin(struct task *task, Bool toss,
                                      Sint16 force, Uint32 flags)
{
    NJS_VECTOR spd;
    NJS_VECTOR fall;
    int steps;
    int x;
    int y;
    int z;
    int sign;

    fall.x = 0.0F;
    fall.y = -gravity;
    fall.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -WRECK(task)->ang.x);
    njRotateY(NULL, -WRECK(task)->ang.y);
    njTranslate(NULL, -WRECK(task)->pos.x, -WRECK(task)->pos.y,
                -WRECK(task)->pos.z);
    njCalcVector(NULL, &WRECK(task)->spd, &spd);
    njCalcVector(NULL, &fall, &fall);
    njPopMatrixEx();
    steps = -(spd.y / (fall.y * 0.5F));
    if (steps < 1) {
        steps = 1;
    }
    if (flags & CAR_ROLL_ONLY) {
        x = 0;
        y = 0;
        z = 1;
    } else {
        if (toss) {
            x = rand() * RAND_SCALE * TOSS_HALF_TURNS_X;
            y = rand() * RAND_SCALE * TOSS_TURNS_Y;
            z = rand() * RAND_SCALE * TOSS_HALF_TURNS_Z;
        } else {
            x = rand() * RAND_SCALE * HALF_TURNS_X;
            y = rand() * RAND_SCALE * TURNS_Y;
            z = rand() * RAND_SCALE * HALF_TURNS_Z;
        }
        if (!((x + z) & 1)) {
            if (x != 0) {
                if (rand() * RAND_SCALE < 0.5F) {
                    x++;
                } else {
                    z++;
                }
            } else {
                x++;
            }
        }
    }
    if (force <= CRASH_NUDGE) {
        sign = rand() * RAND_SCALE < 0.5F ? 1 : -1;
        /* @hack redundant: 0 divided by steps, where 0 would do */
        WRECK(task)->spin.x = 0 / steps;
        WRECK(task)->spin.y = 0 / steps;
        WRECK(task)->spin.z = sign * HALF_TURN / steps;
    } else {
        if (rand() * RAND_SCALE < 0.5F) {
            x = -x;
        }
        if (rand() * RAND_SCALE < 0.5F) {
            z = -z;
        }
        WRECK(task)->spin.x =
            ((x * HALF_TURN) - (int)(rand() * SPIN_SLACK) - SPIN_SHORT) / steps;
        WRECK(task)->spin.y =
            ((y * FULL_TURN) - (int)(rand() * SPIN_SLACK_Y)) / steps;
        WRECK(task)->spin.z =
            ((z * HALF_TURN) - (int)(rand() * SPIN_SLACK) - SPIN_SHORT) / steps;
    }
}

#define PROBE_RISE 15.0F
#define FLAT_LEAN 0.01F

/* probe the surfaces round the wreck, from PROBE_RISE above it. after a
 * probe finds a floor, the next looks one step of speed ahead in x and z. a
 * floor found becomes the wreck's frame, whose y axis is the floor's normal.
 * the normal's lean is its length in x and z. where the lean is FLAT_LEAN or
 * less, the frame takes no turn about y */
static void parked_car_wreck_find_floor(struct task *task)
{
    struct task_work *work = task->work;
    struct surface_side *floor = &WRECK(task)->probe.sides[SURFACE_NEG_Y];
    NJS_VECTOR normal;
    Float lean;

    if (floor->found) {
        /* @hack redundant: the speed multiplied by 1, where the speed would
         * do */
        WRECK(task)->probe.pos.x = work->pos.x + (WRECK(task)->spd.x * 1.0F);
        WRECK(task)->probe.pos.z = work->pos.z + (WRECK(task)->spd.z * 1.0F);
        WRECK(task)->probe.pos.y = work->pos.y + PROBE_RISE;
    } else {
        WRECK(task)->probe.pos = work->pos;
        WRECK(task)->probe.pos.y += PROBE_RISE;
    }
    surface_probe_sides(&WRECK(task)->probe);
    if (floor->found) {
        WRECK(task)->pos.x = work->pos.x;
        WRECK(task)->pos.z = work->pos.z;
        WRECK(task)->pos.y = floor->pos;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, floor->ang_z);
        njRotateX(NULL, floor->ang_x);
        njCalcVector(NULL, &up, &normal);
        lean = njSqrt((normal.x * normal.x) + (normal.z * normal.z));
        if (lean > FLAT_LEAN) {
            WRECK(task)->ang.y = atan2f(normal.x, normal.z) * ANGLE_PER_RADIAN;
        } else {
            WRECK(task)->ang.y = 0;
        }
        /* @hack spelling: the lean computed again, where lean would do */
        WRECK(task)->ang.x =
            atan2f(njSqrt((normal.x * normal.x) + (normal.z * normal.z)),
                   normal.y) *
            ANGLE_PER_RADIAN;
        WRECK(task)->ang.z = 0;
        njPopMatrixEx();
    }
}

#define WRECK_LEVEL 3
/* a wreck made with no life lives WRECK_LIFE steps, then fades over WRECK_FADE
 * steps. one made with a life fades over LIFE_FADE steps. the work's submode
 * keeps the fade's length, which tells the two kinds of wreck apart */
#define WRECK_LIFE 500
#define WRECK_FADE 55
#define LIFE_FADE 15

/* make a wreck of kind, thrown at spd by a crash of force, and return its
 * task. the call returns null where the heap can't give the task or its
 * record. the wreck's pos is its model's center, with the model at pos and
 * turned by ang. floor_pos and floor_ang make its frame until a probe finds a
 * floor. the first probe runs at once. the one call passes life 0, so
 * every wreck takes WRECK_LIFE */
static struct task *parked_car_wreck_create(
    struct car_kind *kind, NJS_POINT3 *pos, struct angles *ang, NJS_VECTOR *spd,
    NJS_POINT3 *floor_pos, struct angles *floor_ang, Sint8 force, int life)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, WRECK_LEVEL, parked_car_wreck_step,
                       CAR_CRASH_TASK_NAME);
    if (!task) {
        return NULL;
    }
    work = task->work;
    task->motion.p = syCalloc(1, sizeof(struct wreck));
    if (!task->motion.p) {
        task_kill(task);
        return NULL;
    }
    work->mode = force;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateZXY(NULL, ang->x, ang->y, ang->z);
    njCalcPoint(NULL, &kind->wreck_object->model->center, &work->pos);
    njPopMatrixEx();
    work->ang = *ang;
    WRECK(task)->spd = *spd;
    WRECK(task)->pos = *floor_pos;
    WRECK(task)->ang = *floor_ang;
    task->draw = parked_car_wreck_draw;
    task->destructor = parked_car_wreck_destroy;
    task->extra.p = kind;
    parked_car_wreck_set_spin(task, FALSE, work->mode, kind->flags);
    collision_init(task, &kind->shapes[kind->shape_count], WRECK_SHAPES,
                   life || kind->flags & CAR_WRECK_OBJECT
                       ? COLLISION_OBJECT
                       : COLLISION_OBJECT_MUTUAL);
    parked_car_place_shapes(task, TRUE, kind);
    if (life) {
        LIFE(task) = life;
        work->submode = LIFE_FADE;
    } else {
        LIFE(task) = WRECK_LIFE;
        work->submode = WRECK_FADE;
    }
    parked_car_wreck_find_floor(task);
    return task;
}

/* a wreck made with no life throws dust while faster than DUST_GRAVITIES times
 * gravity. its chance of a puff each step is RISE_DUST_CHANCE while it rises,
 * and FALL_DUST_CHANCE while it falls. a puff moves at
 * DUST_SPEED_PART of the wreck's speed. a falling wreck's puff lies up to
 * half FALL_DUST_SPREAD from the wreck along each axis */
#define DUST_GRAVITIES 3.9F
#define RISE_DUST_CHANCE 0.03F
#define FALL_DUST_CHANCE 0.1F
#define RISE_DUST_SPREAD 13.0F
#define FALL_DUST_SPREAD 11.0F
#define DUST_SPEED_PART 0.7F
#define RISE_DUST_SCALE 15.0F
#define FALL_DUST_SCALE 10.0F
#define DUST_DEPTH 10.0F
/* a wreck probes where its work's timer plus its life is a multiple of its
 * period. the period is PROBE_MASK + 1 for a wreck made with no life. it is
 * LIFE_PROBE_MASK + 1 for one made with a life. the latter's period is
 * UNSEEN_PROBE_MASK + 1 while its record's unseen passes UNSEEN_PROBES */
#define PROBE_MASK 3
#define UNSEEN_PROBE_MASK 31
#define LIFE_PROBE_MASK 7
#define UNSEEN_PROBES 3
/* a side the probe found bounces a wreck that passes it. the side's normal
 * must lie more than WALL_NORMAL along its axis, and its flags must lack
 * GROUND_DEFLECTS. the bounce keeps WALL_BOUNCE of the wreck's speed across the
 * side */
#define WALL_NORMAL 0.9F
#define WALL_BOUNCE 0.7F
/* a wreck lands where its center falls below its riding height in its floor's
 * frame. the riding height is sqrt(corner.y * corner.y + top * top). corner is
 * the point (box.x + CORNER_X, 0, box.z + CORNER_Z) of the kind's box. top is
 * the y of the point (0, box.y, 0). the wreck's ang turns each into its floor's
 * frame */
#define CORNER_X 5.0F
#define CORNER_Z 8.0F
/* a wreck that lands past its first LANDED_STEPS steps keeps no more than
 * LANDED_LIFE steps of life. the cap doesn't apply once its fade has begun */
#define LANDED_STEPS 5
#define LANDED_LIFE 25
#define ROOF_LIFE 5
/* a landing leaves the wreck's center REST_GAP above its riding height */
#define REST_GAP 0.11F
/* a landing faster than LAND_SPEED sounds. its volume is land_volume, less
 * LAND_VOLUME_DROP, plus LAND_VOLUME_RATE for each unit of speed past
 * LAND_SPEED. it is at most LAND_VOLUME_MAX more than land_volume. a wreck
 * with a life sounds LIFE_VOLUME_DROP quieter */
#define LAND_SPEED 0.2F
#define LAND_VOLUME_RATE 2.0F
#define LAND_VOLUME_DROP 20.0F
#define LAND_VOLUME_MAX 5
#define LIFE_VOLUME_DROP 10
/* a wreck that falls onto its wheels bounces up at BOUNCE of its speed into
 * its floor. after a CRASH_HIT or harder, it bounces at HIT_BOUNCE at least.
 * after a CRASH_NUDGE, it bounces at NUDGE_BOUNCE at least */
#define BOUNCE 0.6F
#define HIT_BOUNCE 1.0F
#define NUDGE_BOUNCE 0.38F
/* a wreck that falls onto its roof bounces up at ROOF_SPEED_PART of its
 * falling speed. it keeps that part of its speed along its floor too. after a
 * CRASH_FLAME_HIT the part is FLAME_ROOF_SPEED_PART */
#define ROOF_SPEED_PART 0.15F
#define FLAME_ROOF_SPEED_PART 0.4F
/* a settling wreck judges which way to turn from its angles SPIN_AHEAD steps
 * of spin ahead */
#define SPIN_AHEAD 15
/* each step turns a settling wreck's ang.y YAW_EASE of the way toward its
 * frame's */
#define YAW_EASE 0.05F
/* a wreck on its roof settles while its center lies within UPSIDE_REACH
 * above its riding height. after a CRASH_NUDGE the reach is
 * NUDGE_UPSIDE_REACH, and the wreck also settles once its fade has begun */
#define UPSIDE_REACH 3.5F
#define NUDGE_UPSIDE_REACH 2.0F
/* a wreck loses its speed on a step that meets four tests. its center lies
 * under REST_HEIGHT in its frame. the y of its turned up vector lies below
 * UPSIDE_NORMAL. its spin about each axis lies within REST_SPIN. its speed
 * lies below REST_GRAVITIES times gravity */
#define REST_HEIGHT 1.0F
#define UPSIDE_NORMAL (-0.9F)
#define REST_SPIN 128
#define REST_GRAVITIES 3.0F
#define UNSEEN_DRAWS 20

/* as SCALE_ANGLE, through the same two locals */
/* @hack spelling: value multiplied as value = value * k, where
 * SCALE_ANGLE's value *= k would do */
#define SCALE_ANGLE2(ang, k)                                                   \
    {                                                                          \
        low_half = (Sint16 *)&(ang);                                           \
        value = *low_half;                                                     \
        value = value * (k);                                                   \
        *low_half = value;                                                     \
    }

/* a wreck falls, and bounces off the sides its probe found and off its
 * floor. one that lands on its wheels after a CRASH_HIT or harder turns
 * toward its roof. one on its roof near its floor settles in its floor's
 * frame. its pitch and roll each turn toward the nearer of upright and half a
 * turn. a wreck ends once its fade is over. it also ends once UNSEEN_DRAWS
 * draws in a row find it out of view */
static void parked_car_wreck_step(struct task *task)
{
    struct task_work *work = task->work;
    struct surface_side *wall;
    NJS_POINT3 *place;
    NJS_VECTOR local_spd;
    NJS_VECTOR corner;
    NJS_VECTOR up_turned;
    NJS_POINT3 local_pos;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float height;
    Float top;
    Float value;
    Sint16 *low_half;
    Bool upside;
    int volume;
    int louder;

    /* @hack redundant: the reads of rand, bounce_spin_x, bounce_spin_y and
     * njCalcVector, which nothing uses */
    (void)rand;
    (void)bounce_spin_x;
    (void)bounce_spin_y;
    (void)njCalcVector;
    if (njScalor(&WRECK(task)->spd) > gravity * DUST_GRAVITIES) {
        if (WRECK(task)->spd.y > 0.0F) {
            if (rand() * RAND_SCALE < RISE_DUST_CHANCE &&
                work->submode == WRECK_FADE) {
                /* @hack spelling: place, a pointer to pos, where pos itself
                 * would do */
                place = &pos;
                *place = work->pos;
                place->x += RANDOM_HALF() * RISE_DUST_SPREAD;
                place->y += RANDOM_HALF() * RISE_DUST_SPREAD;
                place->z += RANDOM_HALF() * RISE_DUST_SPREAD;
                spd.x = WRECK(task)->spd.x * DUST_SPEED_PART;
                spd.y = WRECK(task)->spd.y * DUST_SPEED_PART;
                spd.z = WRECK(task)->spd.z * DUST_SPEED_PART;
                /* @bug logic: makes the puff at the wreck's pos, not at the
                 * jittered copy the lines above made. the falling path makes
                 * its puff at its jittered copy. the calls are the JSRs at
                 * 0x1dca and 0x1e76 of the object's code */
                dust_steady_create(&work->pos, &spd, RISE_DUST_SCALE,
                                   DUST_DEPTH);
            }
        } else if (rand() * RAND_SCALE < FALL_DUST_CHANCE &&
                   work->submode == WRECK_FADE) {
            pos = work->pos;
            pos.x += RANDOM_HALF() * FALL_DUST_SPREAD;
            pos.y += RANDOM_HALF() * FALL_DUST_SPREAD;
            pos.z += RANDOM_HALF() * FALL_DUST_SPREAD;
            spd.x = WRECK(task)->spd.x * DUST_SPEED_PART;
            spd.y = WRECK(task)->spd.y * DUST_SPEED_PART;
            spd.z = WRECK(task)->spd.z * DUST_SPEED_PART;
            dust_steady_create(&pos, &spd, FALL_DUST_SCALE, DUST_DEPTH);
        }
    }
    if (LIFE(task)) {
        LIFE(task)--;
        if (!LIFE(task) && !work->timer) {
            work->timer++;
        }
    }
    if (work->submode == WRECK_FADE) {
        if (!((work->timer + LIFE(task)) & PROBE_MASK)) {
            parked_car_wreck_find_floor(task);
        }
    } else if (WRECK(task)->unseen > UNSEEN_PROBES) {
        if (!((work->timer + LIFE(task)) & UNSEEN_PROBE_MASK)) {
            parked_car_wreck_find_floor(task);
        }
    } else if (!((work->timer + LIFE(task)) & LIFE_PROBE_MASK)) {
        parked_car_wreck_find_floor(task);
    }
    WRECK(task)->spd.y -= gravity;
    vector_add(&work->pos, &WRECK(task)->spd);

    wall = &WRECK(task)->probe.sides[SURFACE_NEG_X];
    if (work->pos.x < wall->pos && wall->found &&
        wall->normal.x > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        WRECK(task)->spd.x = _builtin_fabsf(WRECK(task)->spd.x) * WALL_BOUNCE;
        WRECK(task)->spin.x +=
            (Angle)((RANDOM_HALF() * bounce_spin_x) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.y +=
            (Angle)((RANDOM_HALF() * bounce_spin_y) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.z +=
            (Angle)((RANDOM_HALF() * bounce_spin_z) * ANGLE_PER_DEGREE);
    }
    wall = &WRECK(task)->probe.sides[SURFACE_POS_X];
    if (work->pos.x > wall->pos && wall->found &&
        wall->normal.x < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        WRECK(task)->spd.x =
            -(_builtin_fabsf(WRECK(task)->spd.x) * WALL_BOUNCE);
        WRECK(task)->spin.x +=
            (Angle)((RANDOM_HALF() * bounce_spin_x) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.y +=
            (Angle)((RANDOM_HALF() * bounce_spin_y) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.z +=
            (Angle)((RANDOM_HALF() * bounce_spin_z) * ANGLE_PER_DEGREE);
    }
    wall = &WRECK(task)->probe.sides[SURFACE_NEG_Z];
    if (work->pos.z < wall->pos && wall->found &&
        wall->normal.z > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        WRECK(task)->spd.z = _builtin_fabsf(WRECK(task)->spd.z) * WALL_BOUNCE;
        WRECK(task)->spin.x +=
            (Angle)((RANDOM_HALF() * bounce_spin_x) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.y +=
            (Angle)((RANDOM_HALF() * bounce_spin_y) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.z +=
            (Angle)((RANDOM_HALF() * bounce_spin_z) * ANGLE_PER_DEGREE);
    }
    wall = &WRECK(task)->probe.sides[SURFACE_POS_Z];
    if (work->pos.z > wall->pos && wall->found &&
        wall->normal.z < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        WRECK(task)->spd.z =
            -(_builtin_fabsf(WRECK(task)->spd.z) * WALL_BOUNCE);
        WRECK(task)->spin.x +=
            (Angle)((RANDOM_HALF() * bounce_spin_x) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.y +=
            (Angle)((RANDOM_HALF() * bounce_spin_y) * ANGLE_PER_DEGREE);
        WRECK(task)->spin.z +=
            (Angle)((RANDOM_HALF() * bounce_spin_z) * ANGLE_PER_DEGREE);
    }

    work->ang.x += WRECK(task)->spin.x;
    work->ang.y += WRECK(task)->spin.y;
    work->ang.z += WRECK(task)->spin.z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -WRECK(task)->ang.x);
    njRotateY(NULL, -WRECK(task)->ang.y);
    njTranslate(NULL, -WRECK(task)->pos.x, -WRECK(task)->pos.y,
                -WRECK(task)->pos.z);
    njCalcPoint(NULL, &work->pos, &local_pos);
    njCalcVector(NULL, &WRECK(task)->spd, &local_spd);
    njRotateEx(&work->ang.x, ZXY_TURN);
    /* @hack spelling: spd and pos reused for the box's corner and for up,
     * where locals of their own would do */
    spd.x = WRECK_KIND(task)->box.x + CORNER_X;
    spd.y = 0.0F;
    spd.z = WRECK_KIND(task)->box.z + CORNER_Z;
    pos.x = 0.0F;
    pos.y = 1.0F;
    pos.z = 0.0F;
    njCalcVector(NULL, &spd, &corner);
    njCalcVector(NULL, &pos, &up_turned);
    njPopMatrixEx();
    top = up_turned.y * WRECK_KIND(task)->box.y;
    height = njSqrt((corner.y * corner.y) + (top * top));
    upside = up_turned.y < 0.0F;

    if (local_pos.y < height) {
        if (!work->timer && WRECK(task)->steps > LANDED_STEPS &&
            LIFE(task) > LANDED_LIFE) {
            LIFE(task) = LANDED_LIFE;
        }
        local_pos.y = height + REST_GAP;
        value = _builtin_fabsf(local_spd.y);
        if (value > LAND_SPEED && local_spd.y < 0.0F) {
            louder =
                ((value - LAND_SPEED) * LAND_VOLUME_RATE) - LAND_VOLUME_DROP;
            if (louder > LAND_VOLUME_MAX) {
                louder = LAND_VOLUME_MAX;
            }
            /* @hack spelling: volume set and then added to, where the sum
             * would do */
            volume = land_volume;
            volume += louder;
            if (work->submode == WRECK_FADE) {
                sound_play_at(LAND_SOUND, NULL, SOUND_PRIORITY, volume,
                              &work->pos);
            } else {
                sound_play_at(LAND_SOUND, NULL, SOUND_PRIORITY,
                              volume - LIFE_VOLUME_DROP, &work->pos);
            }
        }
        if (!upside && local_spd.y < 0.0F) {
            local_spd.y = _builtin_fabsf(local_spd.y) * BOUNCE;
            /* @hack spelling: two tests of the mode in a row, where one
             * would do */
            if (work->mode >= CRASH_HIT) {
                if (local_spd.y < HIT_BOUNCE) {
                    local_spd.y = HIT_BOUNCE;
                }
            } else if (local_spd.y < NUDGE_BOUNCE) {
                local_spd.y = NUDGE_BOUNCE;
            }
            if (work->mode >= CRASH_HIT) {
                Sint16 turn;
                Sint16 spin_add;
                Sint16 x_ahead;
                Sint16 z_ahead;
                Float cos_x;
                Float cos_z;
                Bool x_decides;

                SCALE_ANGLE(WRECK(task)->spin.x, land_spin_keep);
                SCALE_ANGLE(WRECK(task)->spin.y, land_spin_keep);
                SCALE_ANGLE(WRECK(task)->spin.z, land_spin_keep);
                turn = WRECK(task)->spin.x;
                x_ahead =
                    work->ang.x + (turn * SPIN_AHEAD) - WRECK(task)->ang.x;
                cos_x = njCos(x_ahead);
                turn = WRECK(task)->spin.z;
                z_ahead = work->ang.z + (turn * SPIN_AHEAD);
                cos_z = njCos(z_ahead);
                /* one axis of x and z turns toward the nearer of upright and
                 * half a turn. it is the one whose turn ahead lies nearer a
                 * quarter turn. the other axis turns toward the other of the
                 * two, so the wreck ends on its roof */
                /* @hack spelling: x_decides, set in an if and an else and then
                 * tested, where the comparison tested in place would do */
                if (_builtin_fabsf(cos_z) > _builtin_fabsf(cos_x)) {
                    x_decides = TRUE;
                } else {
                    x_decides = FALSE;
                }
                if (x_decides) {
                    Bool flip = FALSE;

                    turn = x_ahead;
                    if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
                        turn = WRECK(task)->ang.x - work->ang.x - HALF_TURN;
                        flip = TRUE;
                    } else {
                        turn = WRECK(task)->ang.x - work->ang.x;
                    }
                    spin_add = turn;
                    SCALE_ANGLE(turn, 1.0F / land_turn_divisor);
                    work->ang.x += turn;
                    SCALE_ANGLE(spin_add, 1.0F / land_spin_divisor);
                    WRECK(task)->spin.x += spin_add;
                    turn = z_ahead;
                    if (!flip) {
                        turn = -work->ang.z - HALF_TURN;
                    } else {
                        turn = -work->ang.z;
                    }
                    spin_add = turn;
                    SCALE_ANGLE(turn, 1.0F / land_turn_divisor);
                    work->ang.z += turn;
                    SCALE_ANGLE(spin_add, 1.0F / land_spin_divisor);
                    WRECK(task)->spin.z += spin_add;
                } else {
                    Bool flip = FALSE;

                    turn = z_ahead;
                    if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
                        turn = -work->ang.z - HALF_TURN;
                        flip = TRUE;
                    } else {
                        turn = -work->ang.z;
                    }
                    spin_add = turn;
                    SCALE_ANGLE(turn, 1.0F / land_turn_divisor);
                    work->ang.z += turn;
                    SCALE_ANGLE(spin_add, 1.0F / land_spin_divisor);
                    WRECK(task)->spin.z += spin_add;
                    turn = x_ahead;
                    if (!flip) {
                        turn = WRECK(task)->ang.x - work->ang.x - HALF_TURN;
                    } else {
                        turn = WRECK(task)->ang.x - work->ang.x;
                    }
                    spin_add = turn;
                    SCALE_ANGLE(turn, 1.0F / land_turn_divisor);
                    work->ang.x += turn;
                    SCALE_ANGLE(spin_add, 1.0F / land_spin_divisor);
                    WRECK(task)->spin.x += spin_add;
                }
                turn = work->ang.y - WRECK(task)->ang.y;
                SCALE_ANGLE2(turn, YAW_EASE);
                work->ang.y -= turn;
            }
        } else {
            if (local_spd.y < 0.0F) {
                /* @bug logic: never cuts the life to ROOF_LIFE. the test at
                 * 0x27aa of the object's code asks for more than LANDED_LIFE.
                 * the landing's own cap, stored at 0x23b8, has already left
                 * no more than that */
                if (!work->timer && WRECK(task)->steps > LANDED_STEPS &&
                    LIFE(task) > LANDED_LIFE) {
                    LIFE(task) = ROOF_LIFE;
                }
                /* @hack spelling: the three products in each arm, where the
                 * factor alone in each arm would do */
                if (work->mode <= CRASH_HIT) {
                    value = ROOF_SPEED_PART;
                    local_spd.y = _builtin_fabsf(local_spd.y) * value;
                    local_spd.x *= value;
                    local_spd.z *= value;
                } else {
                    value = FLAME_ROOF_SPEED_PART;
                    local_spd.y = _builtin_fabsf(local_spd.y) * value;
                    local_spd.x *= value;
                    local_spd.z *= value;
                }
            }
            SCALE_ANGLE(WRECK(task)->spin.x, skim_spin_keep);
            SCALE_ANGLE(WRECK(task)->spin.y, skim_spin_keep);
            SCALE_ANGLE(WRECK(task)->spin.z, skim_spin_keep);
        }
    }
    if (upside &&
        ((work->mode >= CRASH_HIT && local_pos.y < height + UPSIDE_REACH) ||
         (work->mode < CRASH_HIT &&
          (work->timer || local_pos.y < height + NUDGE_UPSIDE_REACH)))) {
        Sint16 turn;
        Sint16 spin_add;
        Sint16 x_ahead;
        Sint16 z_ahead;

        SCALE_ANGLE(WRECK(task)->spin.x, upside_spin_keep);
        SCALE_ANGLE(WRECK(task)->spin.y, upside_spin_keep);
        SCALE_ANGLE(WRECK(task)->spin.z, upside_spin_keep);
        turn = WRECK(task)->spin.x;
        x_ahead = work->ang.x + (turn * SPIN_AHEAD) - WRECK(task)->ang.x;
        /* @hack redundant: the cosines of x_ahead and z_ahead, which nothing
         * reads */
        njCos(x_ahead);
        turn = WRECK(task)->spin.z;
        z_ahead = work->ang.z + (turn * SPIN_AHEAD);
        njCos(z_ahead);
        turn = x_ahead;
        if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
            turn = WRECK(task)->ang.x - work->ang.x - HALF_TURN;
        } else {
            turn = WRECK(task)->ang.x - work->ang.x;
        }
        spin_add = turn;
        SCALE_ANGLE(turn, 1.0F / upside_turn_divisor);
        work->ang.x += turn;
        SCALE_ANGLE(spin_add, 1.0F / upside_spin_divisor);
        WRECK(task)->spin.x += spin_add;
        turn = work->ang.y - WRECK(task)->ang.y;
        SCALE_ANGLE2(turn, YAW_EASE);
        work->ang.y -= turn;
        turn = z_ahead;
        if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
            turn = -work->ang.z - HALF_TURN;
        } else {
            turn = -work->ang.z;
        }
        spin_add = turn;
        SCALE_ANGLE(turn, 1.0F / upside_turn_divisor);
        work->ang.z += turn;
        SCALE_ANGLE(spin_add, 1.0F / upside_spin_divisor);
        WRECK(task)->spin.z += spin_add;
    }
    /* @bug logic: never stops a wreck. the test at 0x2a50 of the object's code
     * takes the center's height in the frame for its height above its riding
     * height. the landing leaves the center at least its riding height up.
     * on its roof, that is over 6.3 for every kind's box */
    if (local_pos.y < REST_HEIGHT && up_turned.y < UPSIDE_NORMAL &&
        (Sint16)WRECK(task)->spin.x < REST_SPIN &&
        (Sint16)WRECK(task)->spin.x > -REST_SPIN &&
        (Sint16)WRECK(task)->spin.y < REST_SPIN &&
        (Sint16)WRECK(task)->spin.y > -REST_SPIN &&
        (Sint16)WRECK(task)->spin.z < REST_SPIN &&
        (Sint16)WRECK(task)->spin.z > -REST_SPIN &&
        njScalor(&local_spd) < gravity * REST_GRAVITIES) {
        local_spd.x = 0.0F;
        local_spd.y = 0.0F;
        local_spd.z = 0.0F;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&WRECK(task)->pos);
    njRotateY(NULL, WRECK(task)->ang.y);
    njRotateX(NULL, WRECK(task)->ang.x);
    njCalcPoint(NULL, &local_pos, &work->pos);
    njCalcVector(NULL, &local_spd, &WRECK(task)->spd);
    njPopMatrixEx();
    if (WRECK(task)->unseen < UNSEEN_DRAWS) {
        parked_car_place_shapes(task, TRUE, WRECK_KIND(task));
        if (work->timer) {
            work->timer++;
            if (work->timer > work->submode) {
                if (task->placement) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
            } else {
                collision_register(task);
            }
        } else {
            collision_register(task);
        }
        WRECK(task)->steps++;
    } else if (task->placement) {
        placement_kill(task);
    } else {
        task_kill(task);
    }
}

/* a wreck draws only while part of a ball about its pos lies ahead of the
 * camera. the ball's radius is the model's plus WRECK_VIEW_MARGIN. the pos must
 * also lie no more than WRECK_VIEW_FAR ahead, or LIFE_VIEW_FAR for a wreck with
 * a life. a fading wreck with a life draws only from LIFE_VIEW_NEAR ahead */
#define WRECK_VIEW_MARGIN 30.0F
#define WRECK_VIEW_FAR 1000.0F
#define LIFE_VIEW_FAR 700.0F
#define LIFE_VIEW_NEAR 15.0F
/* a fading wreck's alpha falls from 1 by FADE_DEPTH times the share of its fade
 * gone. that of a wreck with a life falls by LIFE_FADE_DEPTH instead. it is 0
 * on the fade's last step. the wreck draws nothing at an alpha of FADE_CUT or
 * less, which only that 0 reaches */
#define FADE_DEPTH 0.92F
#define LIFE_FADE_DEPTH 0.9F
#define FADE_CUT 0.05F
/* a fading wreck's tint has its alpha in the alpha, green and blue channels.
 * its red channel is half the alpha plus RED_BASE */
#define RED_BASE 128
/* the wreck's light and first beacon turn about x by LIGHT_TURN Angle units
 * a step */
#define LIGHT_TURN 240

static void parked_car_wreck_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object read through the task's kind before kind is set,
     * where kind would do */
    NJS_CNK_OBJECT *object = WRECK_KIND(task)->wreck_object;
    struct car_kind *kind;
    NJS_CNK_OBJECT *child;
    /* @hack redundant: unused, a local nothing reads, which sizes the frame */
    char unused[12];
    Float depth;
    Float alpha;
    int color;
    int red;
    Angle spin;

    if (work->submode == WRECK_FADE) {
        if (!sphere_in_front(&work->pos, object->model->r + WRECK_VIEW_MARGIN,
                             &depth) ||
            depth > WRECK_VIEW_FAR) {
            WRECK(task)->unseen++;
            return;
        }
    } else if (!sphere_in_front(&work->pos,
                                object->model->r + WRECK_VIEW_MARGIN, &depth) ||
               depth > LIFE_VIEW_FAR ||
               (depth < LIFE_VIEW_NEAR && work->timer)) {
        WRECK(task)->unseen++;
        return;
    }
    WRECK(task)->unseen = 0;
    kind = WRECK_KIND(task);
    if (work->timer) {
        if (work->timer < work->submode) {
            if (work->submode == WRECK_FADE) {
                alpha =
                    1.0F - ((Float)work->timer / work->submode * FADE_DEPTH);
            } else {
                alpha = 1.0F -
                        ((Float)work->timer / work->submode * LIFE_FADE_DEPTH);
            }
        } else {
            alpha = 0.0F;
        }
        if (alpha <= FADE_CUT) {
            return;
        }
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        color = alpha * CHANNEL_MAX;
        if (color > CHANNEL_MAX) {
            color = CHANNEL_MAX;
        }
        if (color < 0) {
            color = 0;
        }
        red = (color >> 1) + RED_BASE;
        if (red > CHANNEL_MAX) {
            red = CHANNEL_MAX;
        }
        if (red < 0) {
            red = 0;
        }
        color = (color << ALPHA_SHIFT) + (red << RED_SHIFT) +
                (color << GREEN_SHIFT) + color;
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        parked_car_draw_shadow(kind->shadow_x, kind->shadow_z, &work->ang);
        njRotateEx(&work->ang.x, ZXY_TURN);
        njSetTexture(kind->wreck_texlist);
        njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                    -object->model->center.z);
        model_draw_tinted(object->model, color);
        child = object->child;
        if (child) {
            spin = game_state.steps * LIGHT_TURN;
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)child->pos);
            njRotateX(NULL, spin);
            model_draw_tinted(child->model, color);
            njPopMatrixEx();
            child = child->sibling;
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)child->pos);
            njRotateX(NULL, spin);
            model_draw_tinted(child->model, color);
            njPopMatrixEx();
        }
        njPopMatrixEx();
        draw_attr_restore();
        draw_control_restore();
    } else {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        parked_car_draw_shadow(kind->shadow_x, kind->shadow_z, &work->ang);
        njRotateEx(&work->ang.x, ZXY_TURN);
        njSetTexture(kind->wreck_texlist);
        njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                    -object->model->center.z);
        /* @hack spelling: the build read through the task's kind, where kind
         * would do */
        if (WRECK_KIND(task)->build) {
            njDirectDrawObject(kind->build);
        } else {
            njCnkEasyDrawModel(object->model);
            child = object->child;
            if (child) {
                parked_car_wreck_draw_lights(child,
                                             game_state.steps * LIGHT_TURN);
            }
        }
        njPopMatrixEx();
    }
}

static void parked_car_wreck_destroy(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
    task->extra.p = NULL;
}
