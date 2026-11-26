/* small helpers the game shares, and jiggles, which move the points of a
 * copy of a chunk model */
#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdarg.h>
#include <stdlib.h>

#include "system/angle.h"

/* the count of array's elements. array must be an array whose size is in
 * scope: a pointer gives a wrong count */
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/* rand() * RAND_SCALE stays below one after conversion to Float. */
#if RAND_MAX > 16777215
#include <float.h>
/* Large integer results can round upward when converted to a Float. */
#define RAND_SCALE ((1.0F - FLT_EPSILON) / ((Float)RAND_MAX + 1.0F))
#else
#define RAND_SCALE (1.0F / (RAND_MAX + 1U))
#endif
/* a random Float from 0 up to 1, never 1 */
#define RANDOM_UNIT() (rand() * RAND_SCALE)
/* a random Float from least up to least + spread, never least + spread */
#define RANDOM_FROM(least, spread)                                             \
    ((rand() * ((spread) * RAND_SCALE)) + (least))
/* a random Float from 0 up to count less 0.0001. cast to a whole number, it
 * is a random index below count */
#define RANDOM_BELOW(count) (((count) - 0.0001F) * rand() * RAND_SCALE)
/* a random angle from 0 up to FULL_TURN, never FULL_TURN, as a Float */
#define RANDOM_TURN() (rand() * (FULL_TURN * RAND_SCALE))
/* a random Angle from 0 up to two turns, never two turns */
#define RANDOM_TWO_TURNS() ((Angle)(rand() * (2 * FULL_TURN * RAND_SCALE)))
/* a random Angle from -limit up to limit degrees */
#define RANDOM_ANGLE(limit)                                                    \
    ((Angle)((((Float)rand() * (2.0F * (limit) * RAND_SCALE)) - (limit)) *     \
             ANGLE_PER_DEGREE))
/* a random whole number from least up to least + span, never least + span */
#define RANDOM_COUNT(least, span)                                              \
    /* @hack type: the sum narrowed to a Uint16, where the int would do */     \
    ((Uint16)((int)(rand() * ((span) * RAND_SCALE)) + (least)))

#define JIGGLE_CHAIN_POINTS 11

/* a jiggle holds copy, a copy of the chunk object source. each jiggle_step
 * moves copy's points as kind says. the points are those of the first chunk of
 * each model's vertex list. that chunk must hold six floats to a vertex, a
 * point and then its normal. no kind moves a normal */
struct jiggle {
    Sint8 kind; /* one of the JIGGLE_ kinds below */
    Sint16 vertex_count; /* the count of vertices in source's first chunk */
    /* the kind's parameter: a scale, a cap or a swing's size, as each kind
     * below says */
    Float param;
    union {
        Angle angle; /* the angle the wave, bend and squash kinds take */
        Sint32 steps; /* the steps the swing kinds count down */
    } by_kind;
    /* the angle a kind's wave, bend or swing has reached, never wrapped at a
     * turn. a random step passes it through a Float, which stops it at
     * 0x7fffffff */
    Angle phase;
    Angle phase_step; /* what the wave kinds and JIGGLE_BEND step phase by */
    NJS_CNK_OBJECT *source;
    /* source, copied with its model and its vertex list. the copy shares
     * source's polygon list, child and sibling */
    NJS_CNK_OBJECT *copy;
    /* objects whose models hold other shapes of source's points */
    NJS_CNK_OBJECT *shape_a;
    NJS_CNK_OBJECT *shape_b;
    NJS_VECTOR force; /* what pushes a chain kind's chain */
    /* on Tails's tail jiggles, how far he turned about y since their last
     * step. no kind reads it */
    Angle turn;
    /* a chain kind's chain. points[0] is the origin, which no kind moves */
    NJS_POINT3 points[JIGGLE_CHAIN_POINTS];
    NJS_VECTOR speeds[JIGGLE_CHAIN_POINTS];
};

/* a jiggle's kinds. jiggle_step moves nothing for any other kind. the kinds'
 * heights, regions, rates and limits are their own, fixed in util.c.
 *
 * a wave kind scales each point of source by a weight w. at the point's
 * height y, w is param + (1 - param) * njSin(by_kind.angle * y + phase).
 * JIGGLE_WAVE_CENTERED scales about the point r up the y axis, r being the
 * radius of the copy's model. it measures y from there too, and steps phase
 * by phase_step. the other wave kinds scale about the origin. they step phase
 * by phase_step times a random factor from 3/4 up to 1 */
#define JIGGLE_WAVE_CENTERED 0
#define JIGGLE_WAVE 1
/* JIGGLE_WAVE, but below a fixed height a point keeps its y and z. only its x
 * is scaled there. its z then sways by njSin(by_kind.angle * y + phase). the
 * sway is toward +z where its x is above 0, and toward -z elsewhere */
#define JIGGLE_WAVE_SWAY 2
#define JIGGLE_WAVE_TWIN 3 /* the same as JIGGLE_WAVE */
/* JIGGLE_WAVE_SWAY, but from a lower height, with a wider sway that runs at
 * half of phase */
#define JIGGLE_WAVE_SWAY_WIDE 4

/* the bend kinds bend source's points of height 0 and up into an S in x and y.
 * the S is as tall as twice the radius of the copy's model. the S has two arcs.
 * over each half turn of phase + by_kind.angle, the lower arc grows from 0 to
 * the S's height. the S faces the other way in the second half of each turn.
 * every point's y also stretches by a fixed factor.
 *
 * JIGGLE_BEND gives the copy the bent points, and steps phase by phase_step.
 * it then sets phase_step to one of eight fixed steps, by phase. so a
 * phase_step the caller sets lasts one step */
#define JIGGLE_BEND 5
/* the copy eases a tenth of the way back to source's points, then as under
 * JIGGLE_BEND_EASED. phase steps by 2 */
#define JIGGLE_BEND_EASED_BACK 10
/* the copy eases a tenth of the way to the bent points on each step. phase
 * steps by 1 */
#define JIGGLE_BEND_EASED 11

/* the chain kinds move the chain, then bend the copy along it. each point of
 * the chain but points[0] moves, then lies 1 from the one before. while force
 * is longer than a fixed length, it pushes each point. the push is force with
 * its x and y swapped and its z negated. otherwise each swings back.
 *
 * the copy's point for a point (x, y, z) of source is (x + p.x, p.y, z). p is
 * the chain's place at index y, between points[floor(y)] and
 * points[ceil(y)]. a y below 0 counts as 0. no point of source may lie higher
 * than JIGGLE_CHAIN_POINTS - 1 */
#define JIGGLE_CHAIN 6
/* JIGGLE_CHAIN, but once a point of the chain is placed, an x above param
 * becomes param */
#define JIGGLE_CHAIN_CAPPED 7
/* JIGGLE_CHAIN, but the copy eases part of the way to where the chain puts
 * it on each step */
#define JIGGLE_CHAIN_EASED 12

/* JIGGLE_EASE_A moves the copy a tenth of the way to shape_a's points on
 * each step. JIGGLE_EASE_B moves it half the way to shape_b's. both step
 * phase by 1 */
#define JIGGLE_EASE_A 8
#define JIGGLE_EASE_B 9

/* JIGGLE_SQUASH gives the copy source's points. it squashes those in a fixed
 * box about the y axis by a swing. the swing is njSin(by_kind.angle + phase)
 * times param, its size clipped to a fixed limit. each step adds a fixed amount
 * to phase, and shrinks param toward 0 */
#define JIGGLE_SQUASH 13

/* the swing kinds bend source's points in fixed regions, each region as a limb
 * of four segments. a swing of njSin(phase) times param gives an angle. the
 * swing's size is clipped to a limit of the kind's own. from the limb's axis,
 * its first three segments lie at a quarter, a half and all of that angle. the
 * fourth lies at that angle too. each step shrinks param toward 0, and steps
 * phase by an amount of the kind's own. the copy's other points move to
 * source's as the bent ones move.
 *
 * JIGGLE_SWING_UP, JIGGLE_SWING_X and JIGGLE_SWING_XZ count by_kind.steps
 * down to 0. while it counts, the copy eases a tenth of the way to the bent
 * points on each step. after that it eases nine tenths of the way, but
 * under JIGGLE_SWING_UP a tenth still.
 *
 * the first limb of JIGGLE_SWING_UP, and that of JIGGLE_FLUTTER_UP, bend no
 * point as a second or third segment. a point above the first segment bends
 * as the fourth */
#define JIGGLE_SWING_UP 14 /* limbs up y */
/* limbs up y, bent from shape_a's points. each segment's angle follows njSin
 * of a phase of its own, with no param or limit. each segment's phase lags the
 * one before by a fixed amount. a wave so runs out along each limb. phase steps
 * by a random amount. the copy eases a tenth of the way to the bent points, and
 * its other points take shape_a's. by_kind.steps becomes a fixed count, for a
 * swing kind to count down */
#define JIGGLE_FLUTTER_UP 15
#define JIGGLE_SWING_DOWN 16 /* limbs down y, which the copy takes whole */
#define JIGGLE_SWING_X 17 /* limbs along -x */
#define JIGGLE_SWING_XZ 18 /* limbs along -x, and along z on both sides */

/* the angle between first and second, from 0 to 0x8000. both must be unit
 * vectors, which the call doesn't scale. where axis isn't null and points
 * against first x second, the result is 0x10000 less that angle. an inner
 * product of first and second of 1 or more gives 0, whatever axis is. one of -1
 * or less gives 0x8000 */
Angle vector_angle(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *axis);
/* set angles[1] and angles[2] to the angles about y and z that dir points
 * at, less those that a non-null from points at. angles[0] keeps its
 * value */
void vector_get_angles(NJS_VECTOR *dir, NJS_VECTOR *from, Angle angles[3]);
/* value wrapped into 0 up to period, or 0 for a period of 0 */
Float float_wrap(Float value, Float period);
/* from, moved by step toward to and not past it. step must not be
 * negative */
Float float_approach(Float from, Float to, Float step);

/* a tree's walk takes each node, then its child's tree, then its next
 * sibling's. tree_list_nodes lists the walk from root into nodes and returns
 * the count. nodes can't be null: the walk reads each node back from it */
int tree_list_nodes(NJS_OBJECT *root, NJS_OBJECT **nodes);

/* draw_attr_save keeps _nj_constant_attr_and_ and _nj_constant_attr_or_ in
 * one slot, and draw_attr_restore puts back what the slot holds. a save
 * overwrites the last, so saves don't nest. draw_attr_set sets and_attr's
 * bits in _nj_constant_attr_and_ and or_attr's in _nj_constant_attr_or_.
 * draw_attr_clear clears them */
void draw_attr_save(void);
void draw_attr_restore(void);
void draw_attr_set(Uint32 and_attr, Uint32 or_attr);
void draw_attr_clear(Uint32 and_attr, Uint32 or_attr);
/* the same for _nj_control_3d_flag_, in a slot of its own */
void draw_control_save(void);
void draw_control_restore(void);
void draw_control_set(Uint32 bits);
void draw_control_clear(Uint32 bits);
/* the draw control bits for the modifier volumes. NJD_CONTROL_3D_SHADOW lets
 * the volumes change a model's polygons. NJD_CONTROL_3D_TRANS_MODIFIER lets a
 * volume change translucent polygons as well as opaque ones */
#define DRAW_CONTROL_SHADOW                                                    \
    (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* the bits that change the color a strip takes from its material's diffuse.
 * NJD_CONTROL_3D_OFFSET_MATERIAL adds the constant material to the diffuse.
 * NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL has a textured strip take only the
 * diffuse's alpha */
#define DRAW_CONTROL_MATERIAL_EDITS                                            \
    (NJD_CONTROL_3D_OFFSET_MATERIAL | NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL)
/* the bits that take each material chunk's diffuse from the constant material,
 * and pass each strip chunk's header through njSetConstantAttr's masks */
#define DRAW_CONTROL_CONSTANT                                                  \
    (NJD_CONTROL_3D_CNK_CONSTANT_ATTR | NJD_CONTROL_3D_CONSTANT_MATERIAL)
/* every strip flag but NJD_FST_UA. a draw that forces NJD_FST_UA alone on
 * clears these from _nj_constant_attr_or_ */
#define STRIP_FLAGS_BUT_UA                                                     \
    (NJD_FST_IL | NJD_FST_IS | NJD_FST_IA | NJD_FST_DB | NJD_FST_FL |          \
     NJD_FST_ENV)

/* world_sweep_ball's result bits. SWEEP_FLOOR marks a surface met whose normal
 * lies within 60 degrees of up, against world_gravity. SWEEP_WALL marks any
 * other */
#define SWEEP_FLOOR 0x1
#define SWEEP_WALL 0x2

/* sweep a ball of radius from *pos along *speed. it passes each world
 * object whose flags share a bit with skip. the result has SWEEP_FLOOR or
 * SWEEP_WALL for each object the ball meets, and is 0 for none. on a meet,
 * *pos and *speed take the nearest object's hit. an object whose owner has a
 * force moves its hit by the force's speed.
 *
 * the sweep lists world_objects and world_count anew, by
 * polygon_collect_near. they then hold the objects within radius of *pos,
 * however long *speed is. each object the ball meets overwrites
 * polygon_contacts.
 *
 * ang may be null. otherwise the ball's up is the y axis turned by *ang. on
 * a meet, ang->x and ang->z take the slope of the nearest object's surface.
 * ang->y keeps its own. each then adds the turn of the object's force. where
 * the nearest object has an owner but no force, *ang adds an unset turn.
 *
 * on a surface facing down, call a half turn less a slope its flip. one of
 * ang->x and ang->z takes its flip in place of its slope. it is the one that
 * lies nearer its flip, or ang->z on a tie */
int world_sweep_ball(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                     int skip, Float radius);
/* world_sweep_ball, but TRUE where the ball meets any object, and FALSE
 * otherwise */
Bool world_sweep_ball_hit(NJS_POINT3 *pos, NJS_VECTOR *speed,
                          struct angles *ang, int skip, Float radius);

/* printf's formatting into buffer, with the flags -, + and 0 and a field
 * width. its conversions are %s, %c, %u, %d, %x and %X. any other, %% too,
 * takes an int and writes only the width's padding. buffer must hold the
 * result and its null, which nothing checks. a format that ends inside a
 * conversion reads past its null. a zero-padded number wider than 63 writes
 * before the formatter's digit buffer. the result is the length written,
 * less the null */
int string_vformat(char *buffer, const char *format, va_list args);

/* a jiggle of source, from the Shinobi heap, or null where the heap runs
 * out. copy, its model and the model's vertex list are copies of source's,
 * in a block each. param is 0.9, by_kind.angle 0x1000, phase 0 and
 * phase_step 0x400. the chain lies still, up y from the origin. kind, the
 * shapes, force and turn are left for the caller to set. a call that fails
 * frees the blocks it took */
struct jiggle *jiggle_create(NJS_CNK_OBJECT *source);
/* free jiggle, and the copy, model and vertex list jiggle_create made for
 * it, each unless null. source and the shapes are left alone */
void jiggle_free(struct jiggle *jiggle);
/* move the copy's points for one step, as jiggle's kind says. each model the
 * kind reads must hold vertex_count vertices in its first chunk */
void jiggle_step(struct jiggle *jiggle);

#endif
