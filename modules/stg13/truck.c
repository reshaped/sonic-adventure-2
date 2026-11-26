/* the truck, TRUCK in City Escape's object list: a G.U.N. truck with a trailer.
 * it drives a path behind the players, and crashes at its end */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "draw/trail.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/earth_shake.h"
#include "shared/flash.h"
#include "shared/modifier_volume.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg13/car.h"
#include "stg13/truck.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a truck's modes, in its work's mode. a throw in MODE_DRIVE sets MODE_HIT,
 * which lasts a step. MODE_RECOVER then lowers hit_fade to 0, and returns to
 * MODE_DRIVE. the truck stays in MODE_CRASHED once it passes an end of its
 * path */
enum truck_mode { MODE_DRIVE, MODE_HIT, MODE_RECOVER, MODE_CRASHED };

/* the work's submode: 1 while the truck is in the air, and 0 on the ground. a
 * truck on the ground takes off once lift reaches TAKE_OFF_LIFT. a truck in the
 * air lands once lift falls under AIR_LIFT. a settling truck stays on the
 * ground, and a crashed truck's submode stays as it was */
#define AIRBORNE(work) ((work)->submode)
/* the truck's speed along its path, in units a step, below 0 as it backs. it is
 * a Float kept in the pointer task->extra. task_destroy frees a non-null extra,
 * so the destructor clears the word */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)
/* a countdown, which each step lowers by 1 while it is above 0. where it is
 * still above 0, any touch on the child that step throws player 0. a throw sets
 * it to HIT_COUNTDOWN. any touch then throws in the HIT_COUNTDOWN - 1 steps
 * after the throw */
#define HIT_STEPS(task) ((task)->unk_3c.l)
#define HIT_COUNTDOWN 10

/* the first BLINKING_LAMPS of the LAMPS lamps blink. the others glow white */
#define LAMPS 6
#define BLINKING_LAMPS 4
/* the points each lamp's trail keeps */
#define TRAIL_POINTS 15

/* a wobble's term: amp degrees times the sine of freq times the time */
struct wave {
    Angle freq;
    Float amp;
};

struct truck_spec {
    /* the degrees the wheels turn for each unit the truck moves */
    Float wheel_turn;
    /* the shapes the child starts with, and how many */
    struct collision_shape *shapes;
    int count;
    /* the shadow's half width in x. in z, how far its top's front and back
     * edges lie inside the span from 0 to -SHADOW_LENGTH. y isn't read */
    NJS_VECTOR shadow;
    /* a point of the body, of which only x counts. tilt_lift, at least 0, is
     * how far the roll about z moves it in y, less tilt_slack */
    NJS_VECTOR tilt_probe;
};

/* a truck's record, in task->motion. segment and fraction place the truck on
 * its path, as truck_path_advance moves them. path_ang turns the path's points
 * about y, around path_center. path_pos is where path_center then lies.
 * truck_init sets path_pos to path_center and path_ang to 0, which leave the
 * points where they are */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's -fpack-struct=4 caps each member's alignment at 4. pack(8) lifts the
 * cap for the matrices, which GCC aligns to 8. they lie at 32 and 96, so no
 * member moves. the record grows from 388 bytes to 392, and only syCalloc's
 * request reads its size */
#pragma pack(push, 8)
#endif
#endif
struct truck {
    /* the lift's speed in y. x and z stay 0 */
    NJS_VECTOR spd;
    int segment;
    Float fraction;
    Uint8 unk_14[0xc];
    /* matrix carries a point from the truck's box into the world, and inverse
     * carries it back. the box's axes run from pos, turned by ang.y and then
     * by ang.x, and moved to BOX_SHAPE's center. they take no roll, lift or
     * tilt_lift. task->force points at matrix */
    NJS_MATRIX matrix;
    NJS_MATRIX inverse;
    Uint8 unk_a0[4];
    NJS_POINT3 path_pos;
    Angle path_ang;
    NJS_POINT3 path_center;
    struct path *path;
    /* each part's wobble, which the waves set while lift is under AIR_LIFT */
    struct angles front_wobble;
    struct angles body_wobble;
    struct angles back_wobble;
    struct angles trailer_wobble;
    Uint8 unk_f4[4];
    /* how high the body rides over its place. a body falling below -sink_limit
     * bounces */
    Float lift;
    Uint8 unk_fc[4];
    NJS_POINT3 last_pos; /* the work's pos at the step before */
    /* how far the scrolling parts' textures scroll. scroll_u stays 0 */
    Sint16 scroll_u;
    Sint16 scroll_v;
    /* 1 at a throw, falling to 0 over hit_fade_steps steps */
    Float hit_fade;
    /* the trailer's angles, less the truck's */
    struct angles trailer_ang;
    /* once crashed: the truck's speeds summed since the crash, or since its
     * speed last turned back. each step adds recoil_pull times -recoil to the
     * speed. once recoil passes 0, it returns to 0, and the speed turns back */
    Float recoil;
    /* how far the truck and its trailer lie aside of the path, along the side
     * truck_place gives */
    Float offset;
    Float trailer_offset;
    /* the back's and the front's sways, and their speeds. x and y are offsets,
     * and z is a roll about z, in degrees */
    NJS_VECTOR back_sway;
    NJS_VECTOR front_sway;
    NJS_VECTOR back_sway_spd;
    NJS_VECTOR front_sway_spd;
    /* roof_hop is how far the roof hops above its rest, and roof_hop_spd its
     * upward speed */
    Float roof_hop_spd;
    Float roof_hop;
    /* how far the body rises as it rolls, as truck_spec's tilt_probe says */
    Float tilt_lift;
    Uint8 unk_168[4];
    /* the lamps' trails, one for each of lamp_points */
    struct trail *trails[LAMPS];
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct truck */
#pragma pack(pop)
#endif
#endif

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define TRUCK(task) ((struct truck *)*(void **)&(task)->motion)

#include "stg13/truck/text.inc"
#include "stg13/chase_models/truck.h"

NJS_TEXLIST *truck_texlists[9] = {
#include "stg13/truck/truck_texlists.inc"
};

/* the point the shadow's foot shrinks toward, at the foot's height, in the
 * space the shadow draws in. truck_draw_shadow sets it */
static NJS_POINT3 shadow_focus;

/* lamp_texlist holds the glow's texture, then the trail's */
#define GLOW_TEXTURE 0
static struct trail_style lamp_trail_style = {
#include "stg13/truck/lamp_trail_style.inc"
};
/* the engine's pitch is lowest at a speed of pitch_base_speed or less, and
 * highest at pitch_top_speed or more */
static Float pitch_top_speed = 9.0F;
static Float pitch_base_speed = 5.0F;
static int engine_volume = 20;
static int landing_volume = 20;
static int hit_fade_steps = 300;
static Float recoil_pull = 0.05F;
static Float dat_8c683e30 = 2.0F;
static int dat_8c683e34 = 25;
/* the degrees the crashed truck turns about y and z for each unit of its speed.
 * its trailer turns back about y by a fifth as many, which
 * ANGLE_PER_FIFTH_DEGREE gives in Angle units */
static Float crash_turn_y = 0.7F;
static Float crash_turn_z = -0.1F;
#define ANGLE_PER_FIFTH_DEGREE (FULL_TURN / 1800.0F)
/* what BRAKE_SPEED is for a truck on the ground under MISSION_HD */
static Float hd_brake_speed = 5.0F;
/* each step, the lift's speed loses lift_gravity. below 0, the speed also
 * gains lift_pull times the lift's depth. a lift falling below -sink_limit
 * bounces: its speed turns up, keeping lift_bounce of its size. the speed then
 * keeps drag of itself, and the lift moves by it */
static Float lift_gravity = 0.1F;
static Float lift_pull = 0.01F;
static Float lift_bounce = 0.7F;
static Float drag = 0.99F;
static Float dat_8c683e54 = 1.0F;
/* scroll_v is scroll_scale times the sum of pos's distance from the origin and
 * the lift's size */
static Float scroll_scale = 7.0F;
static Float sink_limit = 10.0F;
/* body_drop, below 0, lowers the body's draw and the exhaust's puffs from the
 * lift */
static Float body_drop = -5.0F;
/* the crash's debris. each piece lies a random share of debris_radius out from
 * the center of its ring. its speed ahead is the truck's, plus
 * debris_speed_least and a random share of debris_speed_spread. its scale is
 * debris_scale_least plus a random share of debris_scale_spread */
static Float debris_radius = 75.0F;
static Float debris_speed_least = 0.0F;
static Float debris_speed_spread = 5.0F;
static Float debris_scale_least = 10.0F;
static Float debris_scale_spread = 10.0F;
/* player 0's distances from the truck that the speed's rules test.
 * near_distance is for braking and resuming. far_distance is for speeding up on
 * the ground, and air_far_distance in the air. MISSION_HD takes the hd_ ones,
 * but far_distance on the ground */
static Float near_distance = 138.0F;
static Float far_distance = 150.0F;
static Float air_far_distance = 300.0F;
static Float hd_near_distance = 120.0F;
static Float dat_8c683e88 = 130.0F;
static Float hd_air_far_distance = 250.0F;
/* shadow_focus before ang.y turns it */
static NJS_POINT3 shadow_focus_offset = {0.0F, -200.0F, -100.0F};
static Float tilt_slack = 5.0F;
/* the truck's shapes, centered in the truck's own axes. truck_specs gives a
 * driving truck's child the first two, HURT_SHAPE and PUSH_SHAPE. both hurt
 * what they touch while the truck is on the ground, and only PUSH_SHAPE
 * pushes. a crashed truck's child takes CRASH_SHAPE alone, which pushes and
 * doesn't hurt. BOX_SHAPE is no child's: its center places the box of the
 * truck's matrices */
#define HURT_SHAPE 0
#define PUSH_SHAPE 1
#define CRASH_SHAPE 2
#define BOX_SHAPE 3
#define TRUCK_SHAPES 4
/* the driving shapes' attack, which they hold only on the ground */
#define TRUCK_ATTACK SHAPE_ATTACK_LEVEL(3)
static struct collision_shape truck_shapes[TRUCK_SHAPES] = {
#include "stg13/truck/truck_shapes.inc"
};
/* @hack type: an array of one, where a record would do */
static struct truck_spec truck_specs[1] = {
#include "stg13/truck/truck_specs.inc"
};
/* the point, in the truck's own axes, that the crashed truck turns about */
static NJS_POINT3 crash_pivot = {
#include "stg13/truck/crash_pivot.inc"
};
/* the index in truck_paths of the path the truck drives */
#define DRIVEN_PATH 0
static struct path *truck_paths[2] = {
#include "stg13/truck/truck_paths.inc"
};
/* the waves whose sums give each part's wobble about x, about y and about z,
 * in degrees */
#define WOBBLE_GAIN 1.7F
#define WOBBLE_WAVES 3
static struct wave wobble_x_waves[WOBBLE_WAVES] = {
    {520, 0.3F * WOBBLE_GAIN},
    {67, 0.6F * WOBBLE_GAIN},
    {8, 0.7F * WOBBLE_GAIN},
};
static struct wave wobble_y_waves[WOBBLE_WAVES] = {
    {142, 0.1F * WOBBLE_GAIN},
    {14, 0.18F * WOBBLE_GAIN},
    {1, 0.2F * WOBBLE_GAIN},
};
static struct wave wobble_z_waves[WOBBLE_WAVES] = {
    {488, 0.28F * WOBBLE_GAIN},
    {78, 0.6F * 0.7F * WOBBLE_GAIN},
    {5, 0.6F * 0.7F * WOBBLE_GAIN},
};
/* where the exhaust puffs and the wheels' dust rise, in the truck's own axes.
 * on odd steps each rises mirrored in x */
static NJS_POINT3 exhaust_point = {
#include "stg13/truck/exhaust_point.inc"
};
static NJS_POINT3 dust_point = {
#include "stg13/truck/dust_point.inc"
};
/* the half size of each lamp's glow, and the lamps in the front part's axes */
static Float lamp_size = 18.0F;
static NJS_POINT3 lamp_points[LAMPS] = {
#include "stg13/truck/lamp_points.inc"
};

#undef TRUCK_STORAGE

/* the vector helpers take each vector as its three Floats, x, y and z.
 * vec_between sets out to to - from, and vec_offset sets out to base + delta */
#pragma inline(vec_between, vec_scale, vec_offset, vec_add)

/* @hack type: the Floats walked through a Float pointer, where an NJS_POINT3
 * read and written member by member would do */
static void vec_between(Float *from, Float *to, Float *out)
{
    *out++ = *to++ - *from++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}

/* @hack type: the Floats walked through a Float pointer, where an NJS_POINT3
 * read and written member by member would do */
static void vec_scale(Float *vec, Float scale)
{
    *vec++ *= scale;
    *vec++ *= scale;
    *vec *= scale;
}

/* @hack type: the Floats walked through a Float pointer, where an NJS_POINT3
 * read and written member by member would do */
static void vec_offset(Float *base, Float *delta, Float *out)
{
    *out++ = *delta++ + *base++;
    *out++ = *delta++ + *base++;
    *out = *delta + *base;
}

/* @hack type: the Floats walked through a Float pointer, where an NJS_POINT3
 * read and written member by member would do */
static void vec_add(Float *add, Float *sum)
{
    *sum++ += *add++;
    *sum++ += *add++;
    *sum += *add;
}

#define SHADOW_CORNERS 4
/* the shadow's top is 2 * half_x across. its front edge lies SHADOW_FRONT_Y up,
 * at a z of -half_z. its back edge lies SHADOW_BACK_Y up, at a z of half_z -
 * SHADOW_LENGTH. the model's radius is twice the length of (half_x, half_z),
 * plus SHADOW_REACH */
#define SHADOW_FRONT_Y 30.0F
#define SHADOW_BACK_Y 130.0F
#define SHADOW_LENGTH 900.0F
#define SHADOW_REACH 300.0F
/* the foot is the top shrunk to SHADOW_SHRINK of its size about the focus, at
 * the focus's height. the foot then moves SHADOW_SLANT along x and along z */
#define SHADOW_SHRINK 0.12F
#define SHADOW_SLANT 100.0F

/* draw the truck's shadow as a modifier volume, in the current matrix's space.
 * ang turns its top about z, then x, then y. the call sets shadow_focus to
 * shadow_focus_offset turned by ang->y. the volume is a copy of
 * modifier_volume_box_model at WORK_AREA, over what was there. the call never
 * returns where NJD_CE doesn't end the box's vertices */
static void truck_draw_shadow(Float half_x, Float half_z, struct angles *ang)
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
    Float focus_x;
    Float focus_z;

    radius =
        (njSqrt((half_x * half_x) + (half_z * half_z)) * 2.0F) + SHADOW_REACH;
    /* @hack type: the box's model read through
     * modifier_volume_box_model_array, its second name, where
     * modifier_volume_box_model would do */
    model = *modifier_volume_box_model_array;
    model.vlist = (Sint32 *)WORK_AREA;
    model.r = radius;
    corners[0].x = -half_x;
    corners[0].y = SHADOW_FRONT_Y;
    corners[0].z = -half_z;
    corners[1].x = -half_x;
    corners[1].y = SHADOW_BACK_Y;
    corners[1].z = half_z - SHADOW_LENGTH;
    corners[2].x = half_x;
    corners[2].y = SHADOW_FRONT_Y;
    corners[2].z = -half_z;
    corners[3].x = half_x;
    corners[3].y = SHADOW_BACK_Y;
    corners[3].z = half_z - SHADOW_LENGTH;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateEx(&ang->x, ZXY_TURN);
    njCalcPoints(NULL, corners, points, SHADOW_CORNERS);
    njUnitMatrix(NULL);
    njRotateY(NULL, ang->y);
    njCalcPoint(NULL, &shadow_focus_offset, &shadow_focus);
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
            /* @hack type: the focus read through a Float view of it, x then z
             * into locals of their own and y last, where shadow_focus.x, .y
             * and .z read in place would do */
            focus_x = ((Float *)&shadow_focus)[0];
            vx = ((vx - focus_x) * SHADOW_SHRINK) + focus_x + SHADOW_SLANT;
            focus_z = ((Float *)&shadow_focus)[2];
            vz = ((vz - focus_z) * SHADOW_SHRINK) + focus_z + SHADOW_SLANT;
            vy = ((Float *)&shadow_focus)[1];
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

/* the sum of count waves at time, in degrees */
static Float truck_sum_waves(struct wave *waves, int count, Uint16 time)
{
    Float sum = 0.0F;
    /* @hack spelling: at, an unsigned local for time, where time read in
     * place would do */
    Uint32 at = time;

    while (count--) {
        sum += njSin(waves->freq * at) * waves->amp;
        waves++;
    }
    return sum;
}

#define EXTEND_SPACING 100.0F

/* set *out to point index of the count points. an index from 0 to count - 1
 * returns 0. an index past either end returns 1. *out then lies on a line
 * through two of the points at that end. it lies EXTEND_SPACING past the end
 * for each index past it. past the last point the line runs from point count -
 * 3 through the last. before the first it runs from point 2 through point 0.
 * count must be 3 or more, and each line's two points must lie apart, unchecked
 */
static Bool truck_path_point(NJS_POINT3 *points, int count, int index,
                             NJS_POINT3 *out)
{
    /* @hack spelling: result, a local for each branch's result, where a
     * return in each branch would do */
    Bool result;
    NJS_VECTOR dir;

    if (index >= count) {
        vec_between(&points[count - 3].x, &points[count - 1].x, &dir.x);
        vec_scale(&dir.x,
                  (index - count + 1) * EXTEND_SPACING / njScalor(&dir));
        vec_offset(&points[count - 1].x, &dir.x, &out->x);
        result = TRUE;
    } else if (index < 0) {
        vec_between(&points[2].x, &points[0].x, &dir.x);
        vec_scale(&dir.x, -index * EXTEND_SPACING / njScalor(&dir));
        vec_offset(&points[0].x, &dir.x, &out->x);
        result = TRUE;
    } else {
        *out = points[index];
        result = FALSE;
    }
    return result;
}

/* move a place on path by dist: ahead for a dist above 0, and back otherwise.
 * *segment is the point that starts the place's segment. *fraction is how far
 * along the segment the place lies, from 0 to 1. the call moves both, and sets
 * *out to the new place. it passes the segments from the old place's through
 * the new place's. it returns 1 where one of them has a point past an end, and
 * 0 otherwise.
 *
 * path must be a PATH_LAYOUT_POS path without PATH_CLOSED. on another the call
 * returns 0 and changes nothing. no two neighbouring points may coincide, or a
 * fraction divides by 0 */
static Bool truck_path_advance(struct path *path, int *segment, Float *fraction,
                               Float dist, NJS_POINT3 *out)
{
    Bool result = FALSE;
    int count;
    NJS_POINT3 *points;
    int i;
    /* @hack spelling: j, the next point's index, stepped beside i, where i +
     * 1 at each use would do */
    int j;
    Float len;
    Float along;
    Float part;
    Float done;
    Float left;
    Float rest;
    Float weight;
    NJS_POINT3 start;
    NJS_POINT3 end;

    if (!path->flags) {
        points = path->points;
        count = path->count;
        if (dist > 0.0F) {
            i = *segment;
            j = i + 1;
            along = *fraction;
            for (;;) {
                if (truck_path_point(points, count, i, &start)) {
                    result = TRUE;
                }
                if (truck_path_point(points, count, j, &end)) {
                    result = TRUE;
                }
                len = njDistanceP2P(&start, &end);
                /* @hack spelling: done, a local for the part already passed,
                 * where part = len - len * along would do */
                done = len * along;
                part = len - done;
                if (part < 0.0F) {
                    part = 0.0F;
                }
                if (dist > part) {
                    dist -= part;
                } else {
                    rest = (part - dist) / len;
                    along = 1.0F - rest;
                    if (along < 0.0F) {
                        along = 0.0F;
                        rest = 1.0F;
                    }
                    *segment = i;
                    *fraction = along;
                    out->x = (start.x * rest) + (end.x * along);
                    out->y = (start.y * rest) + (end.y * along);
                    out->z = (start.z * rest) + (end.z * along);
                    return result;
                }
                i++;
                along = 0.0F;
                j++;
            }
        } else {
            i = *segment;
            j = i + 1;
            along = *fraction;
            for (;;) {
                if (truck_path_point(points, count, i, &start)) {
                    result = TRUE;
                }
                if (truck_path_point(points, count, j, &end)) {
                    result = TRUE;
                }
                len = njDistanceP2P(&start, &end);
                done = len * along;
                /* @hack spelling: left, a local for len - done, where the
                 * test on len - done itself would do */
                left = len - done;
                if (left < 0.0F) {
                    done = len;
                }
                if (-dist > done) {
                    dist += done;
                } else {
                    done += dist;
                    weight = done / len;
                    rest = 1.0F - weight;
                    if (rest < 0.0F) {
                        weight = 1.0F;
                        rest = 0.0F;
                    }
                    *segment = i;
                    *fraction = weight;
                    out->x = (start.x * rest) + (end.x * weight);
                    out->y = (start.y * rest) + (end.y * weight);
                    out->z = (start.z * rest) + (end.z * weight);
                    return result;
                }
                i--;
                along = 1.0F;
                j--;
            }
        }
    }
    return FALSE;
}

/* set center to the middle of the box that holds path's points */
static void truck_path_center(struct path *path, NJS_POINT3 *center)
{
    NJS_POINT3 *points = path->points;
    int i;
    /* @hack spelling: x, y and z, locals for the point's parts, where
     * points[i] read in place would do */
    Float x;
    Float y;
    Float z;
    Float max_x;
    Float max_y;
    Float max_z;
    Float min_x;
    Float min_y;
    Float min_z;

#ifdef NONMATCHING
    /* GCC can't see that the first point sets every bound. the truck's path
     * holds 88 points, so these zeros are never read */
    max_x = max_y = max_z = min_x = min_y = min_z = 0.0F;
#endif
    for (i = 0; i < path->count; i++) {
        if (i == 0) {
            max_x = min_x = points[i].x;
            max_y = min_y = points[i].y;
            max_z = min_z = points[i].z;
        } else {
            x = points[i].x;
            y = points[i].y;
            z = points[i].z;
            if (max_x < x) {
                max_x = x;
            }
            if (max_y < y) {
                max_y = y;
            }
            if (max_z < z) {
                max_z = z;
            }
            if (min_x > x) {
                min_x = x;
            }
            if (min_y > y) {
                min_y = y;
            }
            if (min_z > z) {
                min_z = z;
            }
        }
    }
    center->x = (max_x + min_x) * 0.5F;
    center->y = (max_y + min_y) * 0.5F;
    center->z = (max_z + min_z) * 0.5F;
}

/* a part's place on the path lies PART_SPREAD times the part's z from the
 * truck's own place. each part's z lies below 0, so each place lies behind.
 * TRAILER_BACK, below 0 too, moves the trailer's place further back */
#define PART_SPREAD 1.2F
#define TRAILER_BACK (-90.0F)

/* move the truck's place on path by dist, as truck_path_advance moves *segment
 * and *fraction, and return its result. the call pops Ninja's matrix stack once
 * more than it pushes.
 *
 * the front and back parts take places on the path, as PART_SPREAD says. the
 * heading runs from the back's place to the front's, and *ang takes its angles,
 * with z 0. *pos lies midway between the two places, moved offset along the
 * side. the side is the heading turned a quarter turn about y, and leveled. a
 * non-null side takes it unscaled.
 *
 * a non-null trailer_ang takes the trailer's heading, from the trailer's place
 * to the back's. for it, the back's place moves offset along the side. the
 * trailer's place moves trailer_offset along the side of its own heading */
static Bool truck_place(Float offset, Float trailer_offset, struct path *path,
                        int *segment, Float *fraction, Float dist,
                        NJS_POINT3 *pos, struct angles *ang,
                        struct angles *trailer_ang, NJS_VECTOR *side)
{
    Bool result;
    /* @hack spelling: object, a local for each part, where the part read in
     * place would do */
    NJS_CNK_OBJECT *object;
    int segment_trailer;
    int segment_rear;
    int segment_front;
    Float fraction_trailer;
    Float fraction_rear;
    Float fraction_front;
    Float scale1;
    Float scale2;
    NJS_VECTOR v1;
    NJS_VECTOR v2;
    /* @hack redundant: unused, bytes nothing reads, which only size the
     * frame */
    Uint8 unused[12];
    NJS_VECTOR side1;
    NJS_VECTOR side2;
    NJS_POINT3 front;
    NJS_POINT3 rear;
    NJS_POINT3 trailer;
    NJS_POINT3 center;

    result = truck_path_advance(path, segment, fraction, dist, &center);
    fraction_front = fraction_rear = fraction_trailer = *fraction;
    segment_front = segment_rear = segment_trailer = *segment;
    object = &truck_front_object;
    truck_path_advance(path, &segment_front, &fraction_front,
                       object->pos[2] * PART_SPREAD, &front);
    object = &truck_back_object;
    truck_path_advance(path, &segment_rear, &fraction_rear,
                       object->pos[2] * PART_SPREAD, &rear);
    if (trailer_ang) {
        object = &truck_trailer_object;
        truck_path_advance(path, &segment_trailer, &fraction_trailer,
                           (object->pos[2] * PART_SPREAD) + TRAILER_BACK,
                           &trailer);
    }
    vec_between(&rear.x, &front.x, &v1.x);
    vec_between(&trailer.x, &rear.x, &v2.x);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, QUARTER_TURN);
    njCalcVector(NULL, &v1, &side1);
    njCalcVector(NULL, &v2, &side2);
    njPopMatrixEx();
    side1.y = 0.0F;
    side2.y = 0.0F;
    if (side) {
        *side = side1;
    }
    scale1 = offset / njScalor(&side1);
    scale2 = trailer_offset / njScalor(&side2);
    /* @hack spelling: side1 scaled through a walk written out, where
     * vec_scale would do */
    {
        Float *walk = &side1.x;
        *walk++ *= scale1;
        *walk++ *= scale1;
        *walk *= scale1;
    }
    /* @hack spelling: side2 scaled through a walk written out, where
     * vec_scale would do */
    {
        Float *walk = &side2.x;
        *walk++ *= scale2;
        *walk++ *= scale2;
        *walk *= scale2;
    }
    /* @hack spelling: z squared before x, where x before z would do */
    ang->x = (Angle)(atan2f(-v1.y, njSqrt((v1.z * v1.z) + (v1.x * v1.x))) *
                     ANGLE_PER_RADIAN);
    ang->y = (Angle)(atan2f(v1.x, v1.z) * ANGLE_PER_RADIAN);
    ang->z = 0;
    /* @hack spelling: v1 halved through a walk written out, where vec_scale
     * would do */
    {
        Float *walk = &v1.x;
        *walk++ *= 0.5F;
        *walk++ *= 0.5F;
        *walk *= 0.5F;
    }
    vec_offset(&rear.x, &v1.x, &pos->x);
    vec_add(&side1.x, &pos->x);
    if (trailer_ang) {
        vec_add(&side1.x, &rear.x);
        vec_add(&side2.x, &trailer.x);
        vec_between(&trailer.x, &rear.x, &v2.x);
        /* @hack spelling: z squared before x, where x before z would do */
        trailer_ang->x =
            (Angle)(atan2f(-v2.y, njSqrt((v2.z * v2.z) + (v2.x * v2.x))) *
                    ANGLE_PER_RADIAN);
        trailer_ang->y = (Angle)(atan2f(v2.x, v2.z) * ANGLE_PER_RADIAN);
        trailer_ang->z = 0;
    }
    /* @bug logic: pops Ninja's matrix stack a second time, though the call
     * pushed it once. the pop takes off a matrix the caller pushed, or none
     * where the stack holds only its base. the pops are at 0x8c5065ea and
     * 0x8c5067e0 */
    njPopMatrixEx();
    return result;
}

/* after truck_put_on_path, the truck settles for SETTLE_STEPS steps. a settling
 * truck takes the heading its place gives at once, without a roll. no bend in
 * the path lifts it or kicks its sways and roof. each settling step moves the
 * truck, and then sets its speed to scl.x before the speed's rules run */
#define SETTLE_STEPS 10

/* set the work's pos and ang, and last_pos, from the truck's place on its path.
 * the place stays where it is, and the truck then settles */
static void truck_put_on_path(struct task *task)
{
    struct task_work *work = task->work;
    struct angles ang;
    struct angles trailer_ang;
    NJS_POINT3 pos;

    truck_place(TRUCK(task)->offset, TRUCK(task)->trailer_offset,
                TRUCK(task)->path, &TRUCK(task)->segment,
                &TRUCK(task)->fraction, 0.0F, &pos, &ang, &trailer_ang, NULL);
    ang.y += TRUCK(task)->path_ang;
    work->ang = ang;
    /* @hack redundant: the trailer's angles less the truck's worked out into
     * trailer_ang, which nothing reads */
    trailer_ang.y += TRUCK(task)->path_ang;
    trailer_ang.x -= work->ang.x;
    trailer_ang.y -= work->ang.y;
    trailer_ang.z -= work->ang.z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &TRUCK(task)->path_pos);
    njRotateY(NULL, TRUCK(task)->path_ang);
    njTranslate(NULL, -TRUCK(task)->path_center.x, -TRUCK(task)->path_center.y,
                -TRUCK(task)->path_center.z);
    njCalcPoint(NULL, &pos, &work->pos);
    njPopMatrixEx();
    TRUCK(task)->last_pos = work->pos;
    work->tick = SETTLE_STEPS;
}

static void truck_set_matrices(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njTranslateEx(&truck_shapes[BOX_SHAPE].center);
    njGetMatrix(&TRUCK(task)->matrix);
    njInvertMatrix(NULL);
    njGetMatrix(&TRUCK(task)->inverse);
    njPopMatrixEx();
}

/* three copies of vec_between, each with its pointers stepped in another
 * order */
#pragma inline(vec_between_to_steps, vec_between_out_steps,                    \
               vec_between_from_steps)

/* @hack spelling: to stepped on lines of its own twice and from once, where
 * vec_between would do */
static void vec_between_to_steps(Float *from, Float *to, Float *out)
{
    *out++ = *to - *from;
    to++;
    from++;
    *out++ = *to - *from++;
    to++;
    *out = *to - *from;
}

/* @hack spelling: out stepped on lines of its own twice and to once, where
 * vec_between would do */
static void vec_between_out_steps(Float *from, Float *to, Float *out)
{
    *out = *to - *from++;
    to++;
    out++;
    *out = *to++ - *from++;
    out++;
    *out = *to - *from;
}

/* @hack spelling: from stepped on lines of its own twice and out once, where
 * vec_between would do */
static void vec_between_from_steps(Float *from, Float *to, Float *out)
{
    *out = *to++ - *from;
    from++;
    out++;
    *out++ = *to++ - *from;
    from++;
    *out = *to - *from;
}

static void truck_step(struct task *task);
static void truck_destroy(struct task *task);
static void truck_draw(struct task *task);

/* the truck's first step moves at START_SCALE times scl.x. settling then sets
 * the speed to scl.x */
#define START_SCALE 1.2F
/* the truck's place starts at its path's second point */
#define START_SEGMENT 1

void truck_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    int i;
    /* @hack redundant: unused, bytes nothing reads, which only size the
     * frame */
    Uint8 unused[12];

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: an empty else after the return, where none would do */
    if (!player_near(&work->pos, work->scl.y)) {
        return;
    } else {
    }
#ifdef DREAMCAST
#ifdef NONMATCHING
    /* this side checks the 392 bytes that pack(8) gives struct truck, the size
     * syCalloc requests below */
    _Static_assert(sizeof(struct truck) == 392,
                   "struct truck changed its size");
#endif
#endif
    task->motion.p = syCalloc(1, sizeof(struct truck));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = truck_draw;
    task->step = truck_step;
    task->destructor = truck_destroy;
    TRUCK(task)->path = truck_paths[DRIVEN_PATH];
    truck_path_center(TRUCK(task)->path, &TRUCK(task)->path_center);
    TRUCK(task)->path_pos = TRUCK(task)->path_center;
    TRUCK(task)->path_ang = 0;
    TRUCK(task)->segment = START_SEGMENT;
    TRUCK(task)->fraction = 0.0F;
    truck_put_on_path(task);
    SPEED(task) = work->scl.x * START_SCALE;
    child = model_object_create(task, NULL, NULL, truck_specs[0].shapes,
                                truck_specs[0].count, 0);
    if (child) {
        child->work->pos = work->pos;
        child->work->ang.y = work->ang.y;
        child->work->ang.x = 0;
        child->work->ang.z = 0;
    }
    car_set_truck(task);
    for (i = 0; i < LAMPS; i++) {
        TRUCK(task)->trails[i] = trail_alloc(&lamp_trail_style, TRAIL_POINTS);
    }
    truck_set_matrices(task);
    task->force.p = &TRUCK(task)->matrix;
}

static void truck_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task itself would do */
    struct task *owner = task;
    int i;

    for (i = 0; i < LAMPS; i++) {
        trail_free(&lamp_trail_style, TRUCK(owner)->trails[i]);
    }
    car_set_truck(NULL);
    syFree(owner->motion.p);
    owner->motion.p = NULL;
    owner->force.p = NULL;
    owner->extra.p = NULL;
}

/* the turns' divisors, and their limits in Angle units */
#define GROUND_DIVISOR_XY 3
#define GROUND_LIMIT_XY ((int)(8.0F * ANGLE_PER_DEGREE))
#define GROUND_DIVISOR_Z 8
#define GROUND_LIMIT_Z ((int)(2.0F * ANGLE_PER_DEGREE))
#define AIR_DIVISOR_X 20
#define AIR_DIVISOR_Y 8
#define AIR_DIVISOR_Z 10
#define AIR_LIMIT_XZ ((int)(0.5F * ANGLE_PER_DEGREE))
#define AIR_LIMIT_Y ((int)(1.3F * ANGLE_PER_DEGREE))

/* set ang to base turned toward ang. each angle turns by the short turn between
 * the two over a divisor, held to a limit. truck_turn_on_ground takes the
 * ground's divisors and limits, and truck_turn_in_air the air's */
static void truck_turn_on_ground(struct angles *ang, struct angles *base)
{
    Sint16 turn;

    /* @hack spelling: each turn cut to a Sint16 and then divided, in two
     * statements, where turn = (Sint16)(ang->x - base->x) / GROUND_DIVISOR_XY
     * would do */
    turn = ang->x - base->x;
    turn = turn / GROUND_DIVISOR_XY;
    if (turn > GROUND_LIMIT_XY) {
        turn = GROUND_LIMIT_XY;
    }
    if (turn < -GROUND_LIMIT_XY) {
        turn = -GROUND_LIMIT_XY;
    }
    ang->x = base->x + turn;
    turn = ang->y - base->y;
    turn = turn / GROUND_DIVISOR_XY;
    if (turn > GROUND_LIMIT_XY) {
        turn = GROUND_LIMIT_XY;
    }
    if (turn < -GROUND_LIMIT_XY) {
        turn = -GROUND_LIMIT_XY;
    }
    ang->y = base->y + turn;
    turn = ang->z - base->z;
    turn = turn / GROUND_DIVISOR_Z;
    if (turn > GROUND_LIMIT_Z) {
        turn = GROUND_LIMIT_Z;
    }
    if (turn < -GROUND_LIMIT_Z) {
        turn = -GROUND_LIMIT_Z;
    }
    ang->z = base->z + turn;
}

static void truck_turn_in_air(struct angles *ang, struct angles *base)
{
    Sint16 turn;

    /* @hack spelling: each turn cut to a Sint16 and then divided, in two
     * statements, where turn = (Sint16)(ang->x - base->x) / AIR_DIVISOR_X
     * would do */
    turn = ang->x - base->x;
    turn = turn / AIR_DIVISOR_X;
    if (turn > AIR_LIMIT_XZ) {
        turn = AIR_LIMIT_XZ;
    }
    if (turn < -AIR_LIMIT_XZ) {
        turn = -AIR_LIMIT_XZ;
    }
    ang->x = base->x + turn;
    turn = ang->y - base->y;
    turn = turn / AIR_DIVISOR_Y;
    if (turn > AIR_LIMIT_Y) {
        turn = AIR_LIMIT_Y;
    }
    if (turn < -AIR_LIMIT_Y) {
        turn = -AIR_LIMIT_Y;
    }
    ang->y = base->y + turn;
    turn = ang->z - base->z;
    turn = turn / AIR_DIVISOR_Z;
    if (turn > AIR_LIMIT_XZ) {
        turn = AIR_LIMIT_XZ;
    }
    if (turn < -AIR_LIMIT_XZ) {
        turn = -AIR_LIMIT_XZ;
    }
    ang->z = base->z + turn;
}

/* a touch throws player 0 only while the truck's speed is HIT_SPEED or more.
 * the throw aims at a place on the path. the truck would reach it in
 * THROW_STEPS steps at its speed plus scl.x. either throw then adds THROW_RISE
 * to the player's upward speed */
#define HIT_SPEED 0.1F
#define THROW_STEPS 60.0F
#define THROW_RISE 1.0F
/* where that place lies on the path, the throw's direction runs from player 0
 * to it. the direction's y is held from 0 to THROW_Y_MOST. the player faces
 * along it, and takes a speed along it of THROW_SPEED plus THROW_SPEED_SHARE of
 * the truck's. the player also moves THROW_LIFT up, and along z by that speed's
 * z, but not along x. the truck then backs off at BACK_OFF_SPEED, less
 * BACK_OFF_SHARE of its speed's size */
#define THROW_Y_MOST 1.0F
#define THROW_SPEED 3.0F
#define THROW_SPEED_SHARE 0.05F
#define THROW_LIFT 10.0F
#define BACK_OFF_SPEED (-2.0F)
#define BACK_OFF_SHARE 0.1F
/* where that place lies past an end, the player goes along the truck's heading.
 * its speed is the truck's plus THROW_AHEAD. it faces along that, with
 * THROW_FACE_Y for its y, and the truck keeps its speed */
#define THROW_AHEAD 10.0F
#define THROW_FACE_Y 0.1F
/* the throw's flash at player 0: its scale, no sparks, and its depth bias */
#define FLASH_SCALE 6.0F
#define SPARKS_NONE 0
#define FLASH_DEPTH (-8.0F)
/* as the crashed truck's recoil passes 0, its speed turns back and keeps
 * RECOIL_KEEP of its size. a crashed truck keeps CRASH_KEEP of its speed a
 * step */
#define RECOIL_KEEP 0.8F
#define CRASH_KEEP 0.97F
/* the steer is the offset the truck steers toward. it is how far along
 * truck_place's side from pos player 0 lay STEER_PAST steps ago, held to
 * STEER_LIMIT. it is 0 where player 0's slot is empty. it is 0 too where the
 * truck's roll lies STEER_ROLL or more from level */
#define STEER_ROLL ((int)ANGLE_PER_DEGREE)
#define STEER_PAST 30
#define STEER_LIMIT 41.0F
/* the step takes player 0's distance as the truck's distance from where player
 * 0 lay DISTANCE_PAST steps ago. where player 0's slot is empty, it takes the
 * truck's distance from player_work[0]'s pos */
#define DISTANCE_PAST 8
/* the truck leans into a turn by LEAN times the turn left to make */
#define LEAN 2.0F
/* lift's bounds for AIRBORNE. the turns take the truck as in the air while lift
 * is above AIR_LIFT. the wobble moves only while lift is under it */
#define AIR_LIFT 1.0F
#define TAKE_OFF_LIFT 60.0F
/* the trailer's wobble time is the wheels' turn, work->timer, over WOBBLE_TIME.
 * the back's time runs WOBBLE_LAG past the trailer's. the body's runs
 * WOBBLE_LAG past the back's, and the front's past the body's. truck_sum_waves
 * keeps a time's low 16 bits, so only WOBBLE_LAG's, 0xc000, count */
#define WOBBLE_TIME 60
#define WOBBLE_LAG 0x3c000
/* where the path bends down, the lift's speed gains the upward speed that the
 * truck's move lost. it does so only while segment is LIFT_SEGMENTS or less, or
 * player 0 lies within air_far_distance */
#define LIFT_SEGMENTS 6
/* each step that isn't settling kicks the sways and the roof. the kick is the
 * change in how far the truck moves up. the front sway's upward speed takes
 * FRONT_KICK of it, the back sway's BACK_KICK, and the roof's ROOF_KICK. a kick
 * below 0 pushes them down */
#define FRONT_KICK 0.2F
#define BACK_KICK 0.15F
#define ROOF_KICK 0.06F
/* a bounce of a truck that hasn't crashed plays LAND_SOUND where the lift's
 * speed leaves faster than LAND_SPEED. it then shakes the ground within
 * SHAKE_RANGE, at LAND_POWER times that speed, plus LAND_POWER_LEAST */
#define LAND_SPEED 1.0F
#define LAND_SOUND 7
#define SHAKE_RANGE 600.0F
#define LAND_POWER 0.7F
#define LAND_POWER_LEAST 0.5F
/* for player 0's distance d, its controller rumbles at the shake's power times
 * 1 - d / SHAKE_RANGE. a rumble over RUMBLE_MOST is held to it, and one under
 * RUMBLE_LEAST is dropped. over RUMBLE_STRONG, the rumble loses
 * RUMBLE_STRONG_LESS of its power, and takes the strong frequency and time */
#define RUMBLE_LEAST 1.0F
#define RUMBLE_MOST 6.0F
#define RUMBLE_STRONG 4.0F
#define RUMBLE_STRONG_LESS 3.0F
#define RUMBLE_STRONG_FREQ 35
#define RUMBLE_STRONG_TIME 1
#define RUMBLE_FREQ 30
/* each bounce of a truck that hasn't crashed sets roof_hop_spd to ROOF_LAND of
 * the lift's speed. each sway's roll speed becomes LAND_ROLL of the lift's
 * speed, keeping its own sign */
#define ROOF_LAND 0.5F
#define LAND_ROLL 0.01F
/* each sway, and the roof's hop, is a damped spring. its speed keeps _KEEP of
 * itself a step, less _PULL of the sway or hop. a sway's x and roll take the
 * _XZ ones, and its y the _Y ones. the roof bounces off its rest, keeping
 * ROOF_BOUNCE of its speed */
#define BACK_PULL_XZ 0.02F
#define BACK_KEEP_XZ 0.985F
#define BACK_PULL_Y 0.018F
#define BACK_KEEP_Y 0.988F
#define FRONT_PULL_XZ 0.019F
#define FRONT_KEEP_XZ 0.98F
#define FRONT_PULL_Y 0.016F
#define FRONT_KEEP_Y 0.985F
#define ROOF_PULL 0.06F
#define ROOF_KEEP 0.98F
#define ROOF_BOUNCE 0.9F
/* a truck that speeds up adds SPEED_INCREMENT to its speed, then multiplies it
 * by SPEED_GROWTH. on the ground, a backing truck first stops. the speed is
 * held to HD_TOP_SPEED on the ground under MISSION_HD, and to TOP_SPEED
 * otherwise */
#define TOP_SPEED 12.0F
#define HD_TOP_SPEED 14.0F
#define SPEED_INCREMENT 0.01F
#define SPEED_GROWTH 1.015F
/* on the ground, a truck within far_distance of player 0 brakes, cruises or
 * resumes. at BRAKE_SPEED or more, it brakes where player 0 is nearer than
 * near_distance. it brakes only while faster than BRAKE_SHARE of scl.x. braking
 * keeps BRAKE_KEEP of the speed a step. otherwise it cruises, moving
 * CRUISE_SHARE of the way to scl.x. a slower truck resumes, moving RESUME_SHARE
 * of the way. it resumes only where player 0 is beyond near_distance and player
 * 0's stick is free. under MISSION_HD the HD_ ones, hd_brake_speed and
 * hd_near_distance take their places.
 *
 * in the air, a truck brakes as on the ground, but at any speed. it takes
 * BRAKE_SHARE and BRAKE_KEEP under MISSION_HD too. it speeds up where player 0
 * is beyond air_far_distance, or hd_air_far_distance under MISSION_HD.
 * otherwise it keeps drag of its speed */
#define BRAKE_SHARE 0.38F
#define HD_BRAKE_SHARE 0.43F
#define BRAKE_KEEP 0.95F
#define HD_BRAKE_KEEP 0.97F
#define BRAKE_SPEED 0.01F
#define CRUISE_SHARE 0.01F
#define HD_CRUISE_SHARE 0.015F
#define RESUME_SHARE 0.02F
#define HD_RESUME_SHARE 0.025F
/* as the truck lands, each sway's y speed turns up, and becomes LANDING_SWAY
 * times its size */
#define LANDING_SWAY 1.5F
/* on the ground, offset moves STEER_SHARE of the way to the steer each step,
 * by at most STEER_MOST. the sways' x speeds take FRONT_LEAN and BACK_LEAN of
 * that move, the other way. trailer_offset moves TRAILER_FOLLOW of the way to
 * offset. once crashed, the target is CRASH_STEER_KEEP of the steer. offset
 * moves CRASH_FOLLOW of the way to it, and trailer_offset CRASH_TRAILER_FOLLOW
 */
#define STEER_SHARE 0.3F
#define STEER_MOST 0.5F
#define FRONT_LEAN 0.01F
#define BACK_LEAN 0.0063F
#define TRAILER_FOLLOW 0.01F
#define CRASH_STEER_KEEP 0.95F
#define CRASH_TRAILER_FOLLOW 0.08F
#define CRASH_FOLLOW 0.5F
/* each step of a truck that hasn't crashed plays the engine's sound again, to
 * last ENGINE_STEPS more steps. the pitch's share runs from 0 at
 * pitch_base_speed or less to 1 at pitch_top_speed or more. the pitch is
 * PITCH_RANGE times that share, less PITCH_MIDDLE, held from PITCH_LOW to
 * PITCH_HIGH */
#define ENGINE_SOUND 10
#define ENGINE_PRIORITY 1
#define ENGINE_STEPS 10
#define PITCH_RANGE 32768.0F
#define PITCH_MIDDLE 16384
#define PITCH_LOW (-16383)
#define PITCH_HIGH 16383
/* a crash holds the truck's speed to CRASH_SPEED, then turns it back */
#define CRASH_SPEED 5.0F
/* the crash's DEBRIS pieces stand DEBRIS_ANGLE apart round the upper half of a
 * ring, DEBRIS_AHEAD ahead of pos. the ring is DEBRIS_FLAT as high as it is
 * wide. a piece falls at DEBRIS_FALL, and moves DEBRIS_ASIDE to either side,
 * each at HALF_CHANCE */
#define DEBRIS 18
#define DEBRIS_ANGLE (FULL_TURN / (2 * DEBRIS))
#define DEBRIS_AHEAD 10.0F
#define DEBRIS_FALL 0.1F
#define DEBRIS_FLAT 0.38F
#define DEBRIS_ASIDE 0.5F
#define HALF_CHANCE 0.5F
/* the landing's and the crash's sounds play at SOUND_PRIORITY, and the crash's
 * at CRASH_VOLUME */
#define SOUND_PRIORITY 0
#define CRASH_VOLUME 0
#define CRASH_SOUND 13
#define CRASH_SHAKE_RANGE 650.0F
#define CRASH_SHAKE_POWER 10.0F
#define CRASH_RUMBLE_POWER 2
#define CRASH_RUMBLE_FREQ 25
#define CRASH_RUMBLE_TIME 1
/* a truck that hasn't crashed puffs from the exhaust while hit_fade lies under
 * PUFF_FADE. each step puffs at a chance of PUFF_CHANCE. a step that doesn't
 * puff raises the wheels' dust instead, while lift is at most body_drop +
 * DUST_LIFT. a puff takes the lift's speed, EXHAUST_RISE faster up. the dust
 * takes it DUST_RISE faster up, and DUST_ASIDE outward along x */
#define PUFF_FADE 0.5F
#define PUFF_CHANCE 0.7F
#define EXHAUST_RISE 3.3F
#define EXHAUST_SCALE_LEAST 1.2F
#define DUST_LIFT 1.5F
#define DUST_RISE 0.3F
#define DUST_ASIDE 0.8F
#define DUST_SCALE_LEAST 5.0F
#define DUST_SCALE_SPREAD 3.0F

/* v1 to v4 are scratch, which each part of the step reuses */
static void truck_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    struct angles trailer_ang;
    NJS_VECTOR v1;
    NJS_VECTOR v2;
    NJS_VECTOR v3;
    NJS_LINE line;
    NJS_POINT3 nearest;
    NJS_VECTOR v4;
    /* @hack redundant: unused, bytes nothing reads, which only size the
     * frame */
    Uint8 unused[176];
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    struct task_work *child_work;
    /* @hack type: i a Sint32, where an int would do */
    Sint32 i;
    int segments;
    int hits;
    Bool struck;
    Bool at_end;
    int segment;
    int time;
    int value;
    Angle angle;
    Float fraction;
    Float amount;
    Float steer;
    Float rise;
    Float player_dist;
    Float rumble;
    Float fade_per_step;
    Float spread;
    Float speed;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches. it leaves its strings in the
     * object and its loop's body in the code, and the image holds no other
     * code of the block. the calls before the loop, with 0 for each null
     * matrix, build the same code around the block */
    if (0) {
        (void)"<- CARSPEED";
        (void)"<- START COLSIZE";
        (void)"CEN:(%5.3f,%5.3f,%5.3f)";
        njPushMatrixEx();
        njTranslate(0, work->pos.x, work->pos.y, work->pos.z);
        njTranslateV(0, &work->pos);
        njTranslateV(0, &work->pos);
        njTranslateV(0, &TRUCK(task)->path_pos);
        to = (NJS_POINT3 *)TRUCK(task)->path->points + 1;
        from = to - 1;
        segments = TRUCK(task)->path->count - 1;
        for (i = 0; i < segments; i++, from++, to++) {
            line_draw_points(from, to);
        }
        njPopMatrixEx();
        TRUCK(task)->path = truck_paths[DRIVEN_PATH];
        truck_path_center(TRUCK(task)->path, &TRUCK(task)->path_center);
    } else {
        struck = FALSE;
        if (HIT_STEPS(task)) {
            HIT_STEPS(task)--;
        }
        if (task->child && (hits = collision_get_hit_count(task->child)) != 0 &&
            SPEED(task) >= HIT_SPEED) {
            for (i = 0; i < hits; i++) {
                if (collision_get_hit(task->child, i)->task ==
                        player_tasks[0] ||
                    HIT_STEPS(task)) {
                    fraction = TRUCK(task)->fraction;
                    segment = TRUCK(task)->segment;
                    /* @hack spelling: struck reused for whether the throw's
                     * place lies past an end, where a local of its own would
                     * do */
                    struck = truck_path_advance(
                        TRUCK(task)->path, &segment, &fraction,
                        (SPEED(task) + work->scl.x) * THROW_STEPS, &v1);
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njTranslateV(NULL, &TRUCK(task)->path_pos);
                    njRotateY(NULL, TRUCK(task)->path_ang);
                    njTranslate(NULL, -TRUCK(task)->path_center.x,
                                -TRUCK(task)->path_center.y,
                                -TRUCK(task)->path_center.z);
                    njCalcPoint(NULL, &v1, &v1);
                    njPopMatrixEx();
                    if (struck) {
                        v3.x = 0.0F;
                        v3.z = SPEED(task) + THROW_AHEAD;
                        v3.y = 0.0F;
                        njPushMatrixEx();
                        njUnitMatrix(NULL);
                        njRotateZXY(NULL, work->ang.x, work->ang.y,
                                    work->ang.z);
                        njCalcVector(NULL, &v3, &v3);
                        njPopMatrixEx();
                        player_face_along(0, v3.x, THROW_FACE_Y, v3.z);
                        player_set_speed(0, v3.x, v3.y + THROW_RISE, v3.z);
                    } else {
                        vec_between_to_steps(&player_work[0]->pos.x, &v1.x,
                                             &v2.x);
                        if (v2.y > THROW_Y_MOST) {
                            v2.y = THROW_Y_MOST;
                        }
                        if (v2.y < 0.0F) {
                            v2.y = 0.0F;
                        }
                        angle = (Angle)(atan2f(v2.x, v2.z) * ANGLE_PER_RADIAN);
                        if (SPEED(task) > 0.0F) {
                            amount = njScalor(&v2);
                            vec_scale(&v2.x,
                                      ((SPEED(task) * THROW_SPEED_SHARE) +
                                       THROW_SPEED) /
                                          amount);
                            player_work[0]->ang.y = QUARTER_TURN - angle;
                            player_set_pos(0, player_work[0]->pos.x,
                                           player_work[0]->pos.y + THROW_LIFT,
                                           player_work[0]->pos.z + v2.z);
                            player_set_speed(0, v2.x, v2.y + THROW_RISE, v2.z);
                            SPEED(task) =
                                BACK_OFF_SPEED -
                                (_builtin_fabsf(SPEED(task)) * BACK_OFF_SHARE);
                        }
                    }
                    flash_manager_create_sparks_depth(
                        player_work[0]->pos.x, player_work[0]->pos.y,
                        player_work[0]->pos.z, FLASH_SCALE, SPARKS_NONE,
                        FLASH_DEPTH);
                    struck = TRUE;
                    HIT_STEPS(task) = HIT_COUNTDOWN;
                    break;
                }
            }
        }
        fade_per_step = 1.0F / hit_fade_steps;
        switch (work->mode) {
        case MODE_DRIVE:
            if (struck) {
                work->mode = MODE_HIT;
                TRUCK(task)->hit_fade = 1.0F;
            }
            break;
        case MODE_HIT:
            TRUCK(task)->hit_fade += fade_per_step;
            if (TRUCK(task)->hit_fade > 1.0F) {
                TRUCK(task)->hit_fade = 1.0F;
                work->mode = MODE_RECOVER;
            }
            break;
        case MODE_RECOVER:
            TRUCK(task)->hit_fade -= fade_per_step;
            if (TRUCK(task)->hit_fade < 0.0F) {
                TRUCK(task)->hit_fade = 0.0F;
                work->mode = MODE_DRIVE;
            }
            break;
        /* @hack spelling: the last case without its break, where one would end
         * it */
        case MODE_CRASHED:
            TRUCK(task)->recoil += SPEED(task);
            SPEED(task) += -(TRUCK(task)->recoil * recoil_pull);
            if (TRUCK(task)->recoil > 0.0F) {
                TRUCK(task)->recoil = 0.0F;
                SPEED(task) = -(_builtin_fabsf(SPEED(task)) * RECOIL_KEEP);
                /* @hack redundant: a test of the speed with an empty body */
                if (SPEED(task) < 5.0F) {
                }
            }
            SPEED(task) *= CRASH_KEEP;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &crash_pivot, &v1);
            work->ang.y += (Angle)(_builtin_fabsf(SPEED(task)) * crash_turn_y *
                                   ANGLE_PER_DEGREE);
            TRUCK(task)->trailer_ang.y -=
                (Angle)(_builtin_fabsf(SPEED(task)) * crash_turn_y *
                        ANGLE_PER_FIFTH_DEGREE);
            work->ang.z += (Angle)(_builtin_fabsf(SPEED(task)) * crash_turn_z *
                                   ANGLE_PER_DEGREE);
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &crash_pivot, &v2);
            njPopMatrixEx();
            vec_between_out_steps(&v2.x, &v1.x, &v3.x);
            vec_add(&v3.x, &work->pos.x);
        }
    }
    /* @hack spelling: speed, a local for SPEED(task), where SPEED(task)
     * passed in place would do */
    speed = SPEED(task);
    at_end = truck_place(TRUCK(task)->offset, TRUCK(task)->trailer_offset,
                         TRUCK(task)->path, &TRUCK(task)->segment,
                         &TRUCK(task)->fraction, speed, &pos, &ang,
                         &trailer_ang, &v1);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &TRUCK(task)->path_pos);
    njRotateY(NULL, TRUCK(task)->path_ang);
    njTranslate(NULL, -TRUCK(task)->path_center.x, -TRUCK(task)->path_center.y,
                -TRUCK(task)->path_center.z);
    njCalcPoint(NULL, &pos, &pos);
    njCalcVector(NULL, &v1, &v1);
    njPopMatrixEx();
    ang.y += TRUCK(task)->path_ang;
    trailer_ang.y += TRUCK(task)->path_ang;
    trailer_ang.x -= ang.x;
    trailer_ang.y -= ang.y;
    trailer_ang.z -= ang.z;
    /* @hack spelling: angle, a local for ang.z, where (Sint16)work->ang.z
     * tested in place would do */
    angle = work->ang.z;
    if ((Sint16)angle < STEER_ROLL && (Sint16)angle > -STEER_ROLL) {
        /* @hack type: v3, a vector, passed for the angles player_get_past
         * writes and nothing reads, where a struct angles would do */
        if (player_get_past(0, STEER_PAST, &v2, (struct angles *)&v3)) {
            line.px = pos.x;
            line.py = pos.y;
            line.pz = pos.z;
            line.vx = v1.x;
            line.vy = v1.y;
            line.vz = v1.z;
            njDistanceP2L(&v2, &line, &nearest);
            vec_between_from_steps(&pos.x, &nearest.x, &v4.x);
            amount = njScalor(&v4);
            if (amount > STEER_LIMIT) {
                amount = STEER_LIMIT;
            }
            steer = njInnerProduct(&v1, &v4) > 0.0F ? amount : -amount;
        } else {
            steer = 0.0F;
        }
    } else {
        steer = 0.0F;
    }
    if (!work->tick) {
        ang.z = (ang.y - work->ang.y) * LEAN;
        if (TRUCK(task)->lift > AIR_LIFT) {
            truck_turn_in_air(&ang, &work->ang);
        } else {
            truck_turn_on_ground(&ang, &work->ang);
        }
        trailer_ang.z = (trailer_ang.y - TRUCK(task)->trailer_ang.y) * LEAN;
        trailer_ang.z -= ang.z;
        if (TRUCK(task)->lift > AIR_LIFT) {
            truck_turn_in_air(&trailer_ang, &TRUCK(task)->trailer_ang);
        } else {
            truck_turn_on_ground(&trailer_ang, &TRUCK(task)->trailer_ang);
        }
    } else {
        ang.z = 0;
        trailer_ang.z = 0;
    }
    if (work->mode != MODE_CRASHED) {
        vec_between_to_steps(&work->pos.x, &pos.x, &v1.x);
        amount = njScalor(&v1);
        work->timer +=
            (Angle)(amount * truck_specs[0].wheel_turn * ANGLE_PER_DEGREE);
    }
    if (TRUCK(task)->lift < AIR_LIFT) {
        time = work->timer / WOBBLE_TIME;
        TRUCK(task)->trailer_wobble.x =
            truck_sum_waves(wobble_x_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->trailer_wobble.y =
            truck_sum_waves(wobble_y_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->trailer_wobble.z =
            truck_sum_waves(wobble_z_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        time += WOBBLE_LAG;
        TRUCK(task)->back_wobble.x =
            truck_sum_waves(wobble_x_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->back_wobble.y =
            truck_sum_waves(wobble_y_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->back_wobble.z =
            truck_sum_waves(wobble_z_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        time += WOBBLE_LAG;
        TRUCK(task)->body_wobble.x =
            truck_sum_waves(wobble_x_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->body_wobble.y =
            truck_sum_waves(wobble_y_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->body_wobble.z =
            truck_sum_waves(wobble_z_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        time += WOBBLE_LAG;
        TRUCK(task)->front_wobble.x =
            truck_sum_waves(wobble_x_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->front_wobble.y =
            truck_sum_waves(wobble_y_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
        TRUCK(task)->front_wobble.z =
            truck_sum_waves(wobble_z_waves, WOBBLE_WAVES, time) *
            ANGLE_PER_DEGREE;
    }
    /* @hack type: v2, a vector, passed for the angles player_get_past writes
     * and nothing reads, where a struct angles would do */
    if (!player_get_past(0, DISTANCE_PAST, &v1, (struct angles *)&v2)) {
        v1 = player_work[0]->pos;
    }
    player_dist = njDistanceP2P(&v1, &work->pos);
    vec_between_out_steps(&work->pos.x, &pos.x, &v1.x);
    vec_between_from_steps(&TRUCK(task)->last_pos.x, &work->pos.x, &v2.x);
    /* how much more the truck moves up this step than it did the step before */
    rise = v1.y - v2.y;
    if (rise < 0.0F && !work->tick &&
        (TRUCK(task)->segment <= LIFT_SEGMENTS ||
         player_dist < air_far_distance)) {
        TRUCK(task)->spd.y += -rise;
    }
    if (!work->tick) {
        TRUCK(task)->front_sway_spd.y += rise * FRONT_KICK;
        TRUCK(task)->back_sway_spd.y += rise * BACK_KICK;
        TRUCK(task)->roof_hop_spd += rise * ROOF_KICK;
    }
    TRUCK(task)->spd.y += -lift_gravity;
    if (TRUCK(task)->lift < 0.0F) {
        TRUCK(task)->spd.y += -(TRUCK(task)->lift * lift_pull);
    }
    if (TRUCK(task)->lift < -sink_limit && TRUCK(task)->spd.y < 0.0F) {
        TRUCK(task)->spd.y = -(TRUCK(task)->spd.y * lift_bounce);
        if (work->mode != MODE_CRASHED) {
            if (TRUCK(task)->spd.y > LAND_SPEED) {
                sound_play_at(LAND_SOUND, NULL, SOUND_PRIORITY, landing_volume,
                              &work->pos);
                shake_create(&work->pos, SHAKE_RANGE,
                             (TRUCK(task)->spd.y * LAND_POWER) +
                                 LAND_POWER_LEAST);
                /* @hack spelling: the rumble worked out in three statements,
                 * where one product would do */
                amount = njDistanceP2P(&work->pos, &player_work[0]->pos);
                amount *= 1.0F / SHAKE_RANGE;
                rumble = (TRUCK(task)->spd.y * LAND_POWER) + LAND_POWER_LEAST;
                rumble *= 1.0F - amount;
                if (rumble > RUMBLE_MOST) {
                    rumble = RUMBLE_MOST;
                }
                if (rumble >= RUMBLE_LEAST) {
                    if (rumble > RUMBLE_STRONG) {
                        vibration_start(0, rumble - RUMBLE_STRONG_LESS,
                                        RUMBLE_STRONG_FREQ, RUMBLE_STRONG_TIME);
                    } else {
                        vibration_start(0, rumble, RUMBLE_FREQ,
                                        VIBRATION_UNTIMED);
                    }
                }
            }
            TRUCK(task)->roof_hop_spd = TRUCK(task)->spd.y * ROOF_LAND;
            /* @hack spelling: each roll speed's sign tested as -(spd *
             * 0.95F) + spd > 0, where spd > 0 would do */
            TRUCK(task)->front_sway_spd.z =
                -(TRUCK(task)->front_sway_spd.z * 0.95F) +
                            TRUCK(task)->front_sway_spd.z >
                        0.0F
                    ? TRUCK(task)->spd.y * LAND_ROLL
                    : -(TRUCK(task)->spd.y * LAND_ROLL);
            TRUCK(task)->back_sway_spd.z =
                -(TRUCK(task)->back_sway_spd.z * 0.95F) +
                            TRUCK(task)->back_sway_spd.z >
                        0.0F
                    ? TRUCK(task)->spd.y * LAND_ROLL
                    : -(TRUCK(task)->spd.y * LAND_ROLL);
        }
    }
    TRUCK(task)->spd.y *= drag;
    TRUCK(task)->lift += TRUCK(task)->spd.y;
    vec_add(&TRUCK(task)->back_sway_spd.x, &TRUCK(task)->back_sway.x);
    TRUCK(task)->back_sway_spd.x =
        -(TRUCK(task)->back_sway.x * BACK_PULL_XZ) +
        (TRUCK(task)->back_sway_spd.x * BACK_KEEP_XZ);
    TRUCK(task)->back_sway_spd.y = -(TRUCK(task)->back_sway.y * BACK_PULL_Y) +
                                   (TRUCK(task)->back_sway_spd.y * BACK_KEEP_Y);
    TRUCK(task)->back_sway_spd.z =
        -(TRUCK(task)->back_sway.z * BACK_PULL_XZ) +
        (TRUCK(task)->back_sway_spd.z * BACK_KEEP_XZ);
    vec_add(&TRUCK(task)->front_sway_spd.x, &TRUCK(task)->front_sway.x);
    TRUCK(task)->front_sway_spd.x =
        -(TRUCK(task)->front_sway.x * FRONT_PULL_XZ) +
        (TRUCK(task)->front_sway_spd.x * FRONT_KEEP_XZ);
    TRUCK(task)->front_sway_spd.y =
        -(TRUCK(task)->front_sway.y * FRONT_PULL_Y) +
        (TRUCK(task)->front_sway_spd.y * FRONT_KEEP_Y);
    TRUCK(task)->front_sway_spd.z =
        -(TRUCK(task)->front_sway.z * FRONT_PULL_XZ) +
        (TRUCK(task)->front_sway_spd.z * FRONT_KEEP_XZ);
    TRUCK(task)->roof_hop_spd = -(TRUCK(task)->roof_hop * ROOF_PULL) +
                                (TRUCK(task)->roof_hop_spd * ROOF_KEEP);
    TRUCK(task)->roof_hop += TRUCK(task)->roof_hop_spd;
    if (TRUCK(task)->roof_hop < 0.0F) {
        TRUCK(task)->roof_hop_spd =
            _builtin_fabsf(TRUCK(task)->roof_hop_spd) * ROOF_BOUNCE;
    }
    v1 = truck_specs[0].tilt_probe;
    v1.y = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njCalcVector(NULL, &v1, &v1);
    njPopMatrixEx();
    /* @hack spelling: rise reused for the tilt's height, where a local of its
     * own would do */
    rise = _builtin_fabsf(v1.y) - tilt_slack;
    if (rise < 0.0F) {
        rise = 0.0F;
    }
    TRUCK(task)->tilt_lift = rise;
    /* @hack spelling: value, a local for the scroll, where the product stored
     * in place would do */
    value = (njScalor(&work->pos) + _builtin_fabsf(TRUCK(task)->lift)) *
            scroll_scale;
    TRUCK(task)->scroll_v = value;
    TRUCK(task)->last_pos = work->pos;
    if (work->mode != MODE_CRASHED) {
        work->ang = ang;
        TRUCK(task)->trailer_ang = trailer_ang;
        work->pos = pos;
        if (TRUCK(task)->lift < AIR_LIFT ||
            (!AIRBORNE(work) && TRUCK(task)->lift < TAKE_OFF_LIFT) ||
            work->tick) {
            if (work->tick) {
                SPEED(task) = work->scl.x;
            }
            /* @hack type: game_state.mission read through game_state_mission,
             * its volatile second name, in both of the chain's tests of it,
             * where game_state.mission would do */
            if (player_dist > far_distance) {
                if (SPEED(task) < 0.0F) {
                    SPEED(task) = 0.0F;
                }
                SPEED(task) += SPEED_INCREMENT;
                SPEED(task) *= SPEED_GROWTH;
                if (game_state_mission == MISSION_HD) {
                    if (SPEED(task) > HD_TOP_SPEED) {
                        SPEED(task) = HD_TOP_SPEED;
                    }
                } else {
                    if (SPEED(task) > TOP_SPEED) {
                        SPEED(task) = TOP_SPEED;
                    }
                }
            } else if (game_state_mission == MISSION_HD) {
                if (SPEED(task) >= hd_brake_speed) {
                    if (player_dist < hd_near_distance &&
                        SPEED(task) > work->scl.x * HD_BRAKE_SHARE) {
                        SPEED(task) *= HD_BRAKE_KEEP;
                    } else {
                        /* @hack spelling: the sum assigned back, where
                         * += would do */
                        SPEED(task) =
                            SPEED(task) +
                            ((work->scl.x - SPEED(task)) * HD_CRUISE_SHARE);
                    }
                } else if (player_dist > hd_near_distance &&
                           !player_records[0]->stick_lock) {
                    /* @hack spelling: the sum assigned back, where +=
                     * would do */
                    SPEED(task) = SPEED(task) + ((work->scl.x - SPEED(task)) *
                                                 HD_RESUME_SHARE);
                }
            } else {
                if (SPEED(task) >= BRAKE_SPEED) {
                    if (player_dist < near_distance &&
                        SPEED(task) > work->scl.x * BRAKE_SHARE) {
                        SPEED(task) *= BRAKE_KEEP;
                    } else {
                        /* @hack spelling: the sum assigned back, where
                         * += would do */
                        SPEED(task) =
                            SPEED(task) +
                            ((work->scl.x - SPEED(task)) * CRUISE_SHARE);
                    }
                } else if (player_dist > near_distance &&
                           !player_records[0]->stick_lock) {
                    /* @hack spelling: the sum assigned back, where +=
                     * would do */
                    SPEED(task) = SPEED(task) +
                                  ((work->scl.x - SPEED(task)) * RESUME_SHARE);
                }
            }
            /* @hack spelling: an empty else, where none would do */
            if (AIRBORNE(work)) {
                TRUCK(task)->front_sway_spd.y =
                    _builtin_fabsf(TRUCK(task)->front_sway_spd.y) *
                    LANDING_SWAY;
                TRUCK(task)->back_sway_spd.y =
                    _builtin_fabsf(TRUCK(task)->back_sway_spd.y) * LANDING_SWAY;
            } else {
            }
            amount = (steer - TRUCK(task)->offset) * STEER_SHARE;
            if (amount > STEER_MOST) {
                amount = STEER_MOST;
            }
            if (amount < -STEER_MOST) {
                amount = -STEER_MOST;
            }
            TRUCK(task)->front_sway_spd.x += -(amount * FRONT_LEAN);
            TRUCK(task)->back_sway_spd.x += -(amount * BACK_LEAN);
            /* @hack spelling: the sum assigned back, where += would do */
            TRUCK(task)->offset = amount + TRUCK(task)->offset;
            /* @hack spelling: the sum assigned back, where += would do */
            TRUCK(task)->trailer_offset =
                TRUCK(task)->trailer_offset +
                ((TRUCK(task)->offset - TRUCK(task)->trailer_offset) *
                 TRAILER_FOLLOW);
            AIRBORNE(work) = 0;
        } else {
            /* @hack type: game_state.mission read through game_state_mission,
             * its volatile second name, where game_state.mission would do */
            if (game_state_mission == MISSION_HD) {
                if (player_dist < hd_near_distance &&
                    SPEED(task) > work->scl.x * BRAKE_SHARE) {
                    SPEED(task) *= BRAKE_KEEP;
                } else if (player_dist > hd_air_far_distance) {
                    SPEED(task) += SPEED_INCREMENT;
                    SPEED(task) *= SPEED_GROWTH;
                    if (SPEED(task) > TOP_SPEED) {
                        SPEED(task) = TOP_SPEED;
                    }
                } else {
                    SPEED(task) *= drag;
                }
            } else {
                if (player_dist < near_distance &&
                    SPEED(task) > work->scl.x * BRAKE_SHARE) {
                    SPEED(task) *= BRAKE_KEEP;
                } else if (player_dist > air_far_distance) {
                    SPEED(task) += SPEED_INCREMENT;
                    SPEED(task) *= SPEED_GROWTH;
                    if (SPEED(task) > TOP_SPEED) {
                        SPEED(task) = TOP_SPEED;
                    }
                } else {
                    SPEED(task) *= drag;
                }
            }
            AIRBORNE(work) = 1;
        }
        amount = (_builtin_fabsf(SPEED(task)) - pitch_base_speed) /
                 (pitch_top_speed - pitch_base_speed);
        if (amount < 0.0F) {
            amount = 0.0F;
        }
        if (amount > 1.0F) {
            amount = 1.0F;
        }
        /* @hack spelling: amount scaled in a statement of its own, where the
         * product converted in place would do */
        amount = amount * PITCH_RANGE;
        value = (int)amount - PITCH_MIDDLE;
        if (value > PITCH_HIGH) {
            value = PITCH_HIGH;
        }
        if (value <= PITCH_LOW) {
            value = PITCH_LOW;
        }
        sound_play_at_for(ENGINE_SOUND, work, ENGINE_PRIORITY, engine_volume,
                          ENGINE_STEPS, &work->pos);
        sound_set_pitch(work, SOUND_ANY_ID, value);
    } else {
        steer *= CRASH_STEER_KEEP;
        /* @hack spelling: the sum assigned back, where += would do */
        TRUCK(task)->trailer_offset =
            TRUCK(task)->trailer_offset +
            ((steer - TRUCK(task)->trailer_offset) * CRASH_TRAILER_FOLLOW);
        /* @hack spelling: the sum assigned back, where += would do */
        TRUCK(task)->offset = TRUCK(task)->offset +
                              ((steer - TRUCK(task)->offset) * CRASH_FOLLOW);
    }
    if (work->tick) {
        work->tick--;
    }
    if (at_end && work->mode != MODE_CRASHED) {
        if (SPEED(task) > CRASH_SPEED) {
            SPEED(task) = CRASH_SPEED;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        /* @hack spelling: 0 for the null matrix, where NULL would do */
        njRotateY(0, work->ang.y);
        for (i = 0; i < DEBRIS; i++) {
            spread = rand() * RAND_SCALE;
            angle = i * DEBRIS_ANGLE;
            v1.x = rand() * RAND_SCALE > HALF_CHANCE ? DEBRIS_ASIDE
                                                     : -DEBRIS_ASIDE;
            v1.y = -DEBRIS_FALL;
            v1.z = SPEED(task) + (rand() * debris_speed_spread * RAND_SCALE) +
                   debris_speed_least;
            v2.x = njCos(angle) * debris_radius * spread;
            v2.y = njSin(angle) * debris_radius * spread * DEBRIS_FLAT;
            v2.z = DEBRIS_AHEAD;
            njCalcVector(NULL, &v1, &v1);
            njCalcPoint(NULL, &v2, &v2);
            dust_create(&v2, &v1,
                        (debris_scale_spread * rand() * RAND_SCALE) +
                            debris_scale_least);
        }
        njPopMatrixEx();
        work->mode = MODE_CRASHED;
        TRUCK(task)->recoil = 0.0F;
        SPEED(task) = -_builtin_fabsf(SPEED(task));
        sound_play(CRASH_SOUND, NULL, SOUND_PRIORITY, CRASH_VOLUME);
        shake_create(&work->pos, CRASH_SHAKE_RANGE, CRASH_SHAKE_POWER);
        vibration_start(0, CRASH_RUMBLE_POWER, CRASH_RUMBLE_FREQ,
                        CRASH_RUMBLE_TIME);
        if (task->child) {
            task_kill(task->child);
        }
        {
            struct task *child = model_object_create(
                task, NULL, NULL, &truck_shapes[CRASH_SHAPE], 1, 0);

            if (child) {
                child->work->pos = work->pos;
                child->work->ang.y = work->ang.y;
                child->work->ang.x = 0;
                child->work->ang.z = 0;
            }
        }
    }
    if (work->mode != MODE_CRASHED) {
        if (TRUCK(task)->hit_fade < PUFF_FADE) {
            if (rand() * RAND_SCALE < PUFF_CHANCE) {
                v1 = exhaust_point;
                if (game_state.steps & 1) {
                    v1.x = -v1.x;
                }
                v2 = TRUCK(task)->spd;
                /* @hack redundant: v2 scaled by 1, which changes nothing */
                {
                    Float *walk = &v2.x;

                    *walk++ *= 1.0F;
                    *walk++ *= 1.0F;
                    *walk *= 1.0F;
                }
                v2.y += EXHAUST_RISE;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslate(NULL, work->pos.x,
                            work->pos.y + TRUCK(task)->lift + body_drop,
                            work->pos.z);
                njRotateZXY(NULL, work->ang.x, work->ang.y, work->ang.z);
                njCalcPoint(NULL, &v1, &v1);
                njPopMatrixEx();
                white_breath_short_create(
                    &v1, &v2, (rand() * RAND_SCALE) + EXHAUST_SCALE_LEAST);
            } else if (!(TRUCK(task)->lift > body_drop + DUST_LIFT)) {
                v1 = dust_point;
                v2 = TRUCK(task)->spd;
                /* @hack redundant: v2 scaled by 1, which changes nothing */
                {
                    Float *walk = &v2.x;

                    *walk++ *= 1.0F;
                    *walk++ *= 1.0F;
                    *walk *= 1.0F;
                }
                if (game_state.steps & 1) {
                    v2.x -= DUST_ASIDE;
                    v1.x = -v1.x;
                } else {
                    v2.x += DUST_ASIDE;
                }
                v2.y += DUST_RISE;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
                njRotateZXY(NULL, work->ang.x, work->ang.y, work->ang.z);
                njCalcPoint(NULL, &v1, &v1);
                njPopMatrixEx();
                dust_short_create(&v1, &v2,
                                  (rand() * (DUST_SCALE_SPREAD * RAND_SCALE)) +
                                      DUST_SCALE_LEAST);
            }
        }
        if (task->child) {
            child_work = task->child->work;
            if (AIRBORNE(work)) {
                /* @hack spelling: the attack cleared by levels = levels &
                 * ~SHAPE_ATTACK, where &= would do */
                child_work->collision->shapes[HURT_SHAPE].levels =
                    child_work->collision->shapes[HURT_SHAPE].levels &
                    ~SHAPE_ATTACK;
                child_work->collision->shapes[PUSH_SHAPE].levels =
                    child_work->collision->shapes[PUSH_SHAPE].levels &
                    ~SHAPE_ATTACK;
            } else {
                child_work->collision->shapes[HURT_SHAPE].levels =
                    (child_work->collision->shapes[HURT_SHAPE].levels &
                     ~SHAPE_ATTACK) |
                    TRUCK_ATTACK;
                child_work->collision->shapes[PUSH_SHAPE].levels =
                    (child_work->collision->shapes[PUSH_SHAPE].levels &
                     ~SHAPE_ATTACK) |
                    TRUCK_ATTACK;
            }
            child_work->collision->shapes[HURT_SHAPE].ang.y = work->ang.y;
            child_work->collision->shapes[PUSH_SHAPE].ang.y = work->ang.y;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &truck_shapes[HURT_SHAPE].center, &v1);
            child_work->collision->shapes[HURT_SHAPE].center = v1;
            njCalcPoint(NULL, &truck_shapes[PUSH_SHAPE].center, &v1);
            child_work->collision->shapes[PUSH_SHAPE].center = v1;
            njPopMatrixEx();
            collision_fit_radius(task->child);
        }
    }
    truck_set_matrices(task);
}

/* the lamps blink as blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE) gives.
 * the body shows truck_texlist, its lit textures, while that level lies above
 * BLINK_LIT and the truck hasn't crashed */
#define BLINK_PERIOD 50
#define BLINK_REST 20
#define BLINK_FADE 8
#define BLINK_LIT 0.5F
/* a glow turns GLOW_TURN for each unit of its lamp's distance from the camera.
 * that distance counts GLOW_Z_SHARE of the lamp's depth. the glows draw
 * GLOW_DEPTH nearer, as particle_draw_quad takes its depth */
#define GLOW_Z_SHARE 0.001F
#define GLOW_TURN (3.0F * ANGLE_PER_DEGREE)
#define GLOW_DEPTH 15.0F
/* particle_draw_quad's size_limit for none */
#define NO_SIZE_LIMIT 0.0F
/* a blinking lamp's glow fades from GLOW_DIM to GLOW_LIT as it blinks. the
 * others glow at GLOW_LIT */
#define GLOW_DIM 0x10ffffff
#define GLOW_LIT 0xffffffff

/* the lamps' glows and trails draw only for camera 0, and only until the truck
 * crashes. they draw additively, with fog off. the draw leaves fog on, and
 * colors blended by source alpha, whatever held before */
static void truck_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float glow;
    Float ride;
    Float drop;
    NJS_TEXLIST *texture;

    glow = blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE);
    if (work->mode != MODE_CRASHED && glow > BLINK_LIT) {
        texture = &truck_texlist;
    } else {
        texture = &truck_unlit_texlist;
    }
    if (work->mode == MODE_CRASHED) {
        glow = 0.0F;
    }
    njPushMatrixEx();
    ride = TRUCK(task)->lift + sink_limit;
    njTranslate(NULL, work->pos.x,
                work->pos.y + ride + body_drop + TRUCK(task)->tilt_lift,
                work->pos.z);
    truck_draw_shadow(truck_specs[0].shadow.x, truck_specs[0].shadow.z,
                      &work->ang);
    njRotateEx(&work->ang.x, ZXY_TURN);
    njPushMatrixEx();
    njTranslateV(NULL, &truck_object.model->center);
    njRotateZXY(NULL, TRUCK(task)->body_wobble.x, TRUCK(task)->body_wobble.y,
                TRUCK(task)->body_wobble.z);
    njTranslate(NULL, -truck_object.model->center.x,
                -truck_object.model->center.y, -truck_object.model->center.z);
    njSetTexture(&truck_scroll_texlist_1);
    texture_anim_scroll(truck_scroll_object_1->model, &truck_scroll_anim_1,
                        TRUCK(task)->scroll_u, TRUCK(task)->scroll_v);
    njCnkSimpleDrawObject(truck_scroll_object_1);
    njSetTexture(&truck_scroll_texlist_2);
    texture_anim_scroll(truck_scroll_object_2->model, &truck_scroll_anim_2,
                        TRUCK(task)->scroll_u, TRUCK(task)->scroll_v);
    njCnkSimpleDrawObject(truck_scroll_object_2);
    njSetTexture(&truck_scroll_texlist_3);
    texture_anim_scroll(truck_scroll_object_3->model, &truck_scroll_anim_3,
                        TRUCK(task)->scroll_u, TRUCK(task)->scroll_v);
    njCnkSimpleDrawObject(truck_scroll_object_3);
    njSetTexture(texture);
    njCnkSimpleDrawModel(truck_object.model);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslate(NULL, TRUCK(task)->front_sway.x, TRUCK(task)->front_sway.y,
                0.0F);
    njTranslateV(NULL, (NJS_VECTOR *)truck_front_object.pos);
    njTranslateV(NULL, &truck_front_object.model->center);
    njRotateZXY(NULL, TRUCK(task)->front_wobble.x, TRUCK(task)->front_wobble.y,
                TRUCK(task)->front_wobble.z +
                    (int)(TRUCK(task)->front_sway.z * ANGLE_PER_DEGREE));
    njTranslate(NULL, -truck_front_object.model->center.x,
                -truck_front_object.model->center.y,
                -truck_front_object.model->center.z);
    njCnkSimpleDrawModel(truck_front_object.model);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    if (camera_current_index == 0 && work->mode != MODE_CRASHED) {
        NJS_POINT3 view;
        struct trail_style style;
        Uint32 color;
        int i;
        Angle angle;

        style = lamp_trail_style;
        style.new_color = color_blend(style.old_color, style.new_color, glow);
        color = color_blend(GLOW_DIM, GLOW_LIT, glow);
        njSetTexture(&lamp_texlist);
        for (i = 0; i < LAMPS; i++) {
            njCalcPoint(NULL, &lamp_points[i], &view);
            view.z *= GLOW_Z_SHARE;
            angle = (int)(njScalor(&view) * GLOW_TURN);
            if (i < BLINKING_LAMPS) {
                particle_draw_quad(&lamp_points[i], GLOW_TEXTURE, QUARTER_TURN,
                                   angle, lamp_size, lamp_size, color,
                                   PARTICLE_BLEND_ADD, GLOW_DEPTH,
                                   NO_SIZE_LIMIT);
                trail_draw(&style, TRUCK(task)->trails[i], &lamp_points[i]);
            } else {
                particle_draw_quad(&lamp_points[i], GLOW_TEXTURE, QUARTER_TURN,
                                   angle, lamp_size, lamp_size, GLOW_LIT,
                                   PARTICLE_BLEND_ADD, GLOW_DEPTH,
                                   NO_SIZE_LIMIT);
                trail_draw(&lamp_trail_style, TRUCK(task)->trails[i],
                           &lamp_points[i]);
            }
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    njPopMatrix(1);
    njSetTexture(&truck_roof_texlist);
    njPushMatrixEx();
    /* @hack redundant: a block about the roof's draw, which only scopes it */
    {
        njTranslateV(NULL, (NJS_VECTOR *)truck_roof_object.pos);
        njTranslate(NULL, TRUCK(task)->front_sway.x,
                    TRUCK(task)->front_sway.y + TRUCK(task)->roof_hop, 0.0F);
        njTranslateV(NULL, &truck_roof_object.model->center);
        njRotateZXY(NULL, TRUCK(task)->front_wobble.x,
                    TRUCK(task)->front_wobble.y,
                    TRUCK(task)->front_wobble.z +
                        (int)(TRUCK(task)->front_sway.z * ANGLE_PER_DEGREE));
        njTranslate(NULL, -truck_roof_object.model->center.x,
                    -truck_roof_object.model->center.y,
                    -truck_roof_object.model->center.z);
        texture_anim_scroll(truck_roof_object.model, &truck_roof_anim,
                            TRUCK(task)->scroll_u, TRUCK(task)->scroll_v);
        njCnkSimpleDrawModel(truck_roof_object.model);
    }
    njPopMatrixEx();
    njSetTexture(texture);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)truck_back_object.pos);
    njTranslate(NULL, TRUCK(task)->back_sway.x, TRUCK(task)->back_sway.y, 0.0F);
    njTranslateV(NULL, &truck_back_object.model->center);
    njRotateZXY(NULL, TRUCK(task)->back_wobble.x, TRUCK(task)->back_wobble.y,
                TRUCK(task)->back_wobble.z +
                    (int)(TRUCK(task)->back_sway.z * ANGLE_PER_DEGREE));
    njTranslate(NULL, -truck_back_object.model->center.x,
                -truck_back_object.model->center.y,
                -truck_back_object.model->center.z);
    njCnkSimpleDrawModel(truck_back_object.model);
    njPopMatrix(1);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)truck_trailer_object.pos);
    njRotateZXY(NULL, TRUCK(task)->trailer_ang.x, TRUCK(task)->trailer_ang.y,
                TRUCK(task)->trailer_ang.z);
    njTranslateV(NULL, &truck_trailer_object.model->center);
    njRotateZXY(NULL, TRUCK(task)->trailer_wobble.x,
                TRUCK(task)->trailer_wobble.y, TRUCK(task)->trailer_wobble.z);
    njTranslate(NULL, -truck_trailer_object.model->center.x,
                -truck_trailer_object.model->center.y,
                -truck_trailer_object.model->center.z);
    njCnkSimpleDrawModel(truck_trailer_object.model);
    njPopMatrix(1);
    /* the wheels stay down as the body sinks, and rise with it once the lift
     * passes 0 */
    drop = ride;
    if (drop > sink_limit) {
        drop = sink_limit;
    }
    njTranslate(NULL, 0.0F, -drop, 0.0F);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)truck_front_wheel_1.pos);
    njRotateX(NULL, work->timer);
    njRotateY(NULL, truck_front_wheel_1.ang[1]);
    njCnkSimpleDrawModel(truck_front_wheel_1.model);
    njPopMatrix(1);
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)truck_front_wheel_2.pos);
    njRotateX(NULL, work->timer);
    njRotateY(NULL, truck_front_wheel_2.ang[1]);
    njCnkSimpleDrawModel(truck_front_wheel_2.model);
    njPopMatrix(1);
    njPushMatrixEx();
    njRotateZXY(NULL, TRUCK(task)->trailer_ang.x, TRUCK(task)->trailer_ang.y,
                TRUCK(task)->trailer_ang.z);
    njTranslateV(NULL, (NJS_VECTOR *)truck_rear_wheel_1.pos);
    njRotateX(NULL, work->timer);
    njRotateY(NULL, truck_rear_wheel_1.ang[1]);
    njCnkSimpleDrawModel(truck_rear_wheel_1.model);
    njPopMatrix(1);
    njPushMatrixEx();
    njRotateZXY(NULL, TRUCK(task)->trailer_ang.x, TRUCK(task)->trailer_ang.y,
                TRUCK(task)->trailer_ang.z);
    njTranslateV(NULL, (NJS_VECTOR *)truck_rear_wheel_2.pos);
    njRotateX(NULL, work->timer);
    njRotateY(NULL, truck_rear_wheel_2.ang[1]);
    njCnkSimpleDrawModel(truck_rear_wheel_2.model);
    njPopMatrix(1);
    njPopMatrixEx();
}
