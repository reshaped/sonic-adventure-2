/* the helpers util.h and angle.h declare, and some no header declares */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "camera/camera.h"
#include "collision/polygon.h"
#include "collision/world.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define TREE_NODES 32

/* the bytes of string_vformat's digit buffer, its null included */
#define DIGITS_SIZE 64

/* the screen work_on_screen tests, in pixels: its width and its middle's
 * height. the margin it widens by is MARGIN_REACH over the distance from the
 * camera */
#define SCREEN_WIDTH 640.0F
#define SCREEN_MIDDLE_Y 240.0F
#define MARGIN_REACH 3000.0F

/* a vertex of an NJD_CV_VN chunk: its point, then its normal */
#define VERTEX_FLOATS 6
#define NORMAL_FLOATS 3
/* the words of a vertex chunk ahead of its vertices */
#define CHUNK_HEAD 2
/* the vertices of a vertex list's first chunk */
#define CHUNK_VERTICES(vlist) ((Float *)((vlist) + CHUNK_HEAD))
/* the high half of a chunk's word, sign-extended. in its first word it
 * counts the words that follow that one, and in a vertex chunk's second
 * word the chunk's vertices */
/* @hack spelling: the mask before the shift, where the shift alone would do */
#define HIGH_HALF(word) ((Sint16)(((word) & 0xffff0000) >> 16))

/* step times (u + 3) / 4, for u random from 0 up to 1 */
#define WAVE_STEP(step)                                                        \
    (((step) * ((rand() / (RAND_MAX + 1.0F)) + 3.0F)) * 0.25F)
/* the heights below which JIGGLE_WAVE_SWAY and JIGGLE_WAVE_SWAY_WIDE sway a
 * point's z. then the wide sway's size, and its share of phase */
#define SWAY_BELOW (-3.0F)
#define SWAY_WIDE_BELOW (-25.0F)
#define SWAY_WIDE_SIZE 5.0F
#define SWAY_WIDE_RATE 0.5F

/* a half circle's length over its diameter */
#define HALF_CIRCLE_RATIO 1.570796F
/* what the bends scale a bent point's offset in x by, and every point's y */
#define BEND_ACROSS 0.8F
#define BEND_STRETCH 1.15F
/* the bits of phase that pick JIGGLE_BEND's next phase_step */
#define BEND_STEP_BITS 0x70000
/* JIGGLE_BEND_EASED's shares of a point's own place and of its bent place, on
 * each step */
#define BEND_KEEP 0.9F
#define BEND_EASE 0.1F

/* what JIGGLE_EASE_A, JIGGLE_EASE_B and JIGGLE_BEND_EASED_BACK keep of a
 * point's own place on each step */
#define EASE_A_KEEP 0.9F
#define EASE_B_KEEP 0.5F
#define BEND_BACK_KEEP 0.9F

/* JIGGLE_SQUASH's box about the y axis. its top and bottom, its sides in x and
 * half its depth in z */
#define SQUASH_TOP 5.0F
#define SQUASH_BOTTOM 0.36F
#define SQUASH_RIGHT 0.37F
#define SQUASH_LEFT (-0.6F)
#define SQUASH_HALF_DEPTH 1.0F
/* a point in the box moves toward +x by the swing's size times its height times
 * SQUASH_ACROSS. it moves down by the swing's size times its height times
 * SQUASH_DOWN. its z scales by 1 plus the swing's size times SQUASH_OUT */
#define SQUASH_ACROSS 0.5F
#define SQUASH_DOWN 0.1F
#define SQUASH_OUT 0.2F

/* @hack spelling: bend doubled and halved, where bend itself would do */
#define LAST_BEND(bend) (((bend) + (bend)) >> 1)

/* the swing kinds' tuning. each kind steps phase by its STEP, and multiplies
 * param by its DAMP, on each step. its LIMIT clips the swing's size. the bend
 * is the angle whose tangent is the swing times its SLOPE. param becomes 0 once
 * its size falls below SWING_STOP */
#define SWING_UP_STEP 0x400
#define SWING_UP_DAMP 0.99F
#define SWING_UP_LIMIT 1.4F
#define SWING_UP_SLOPE 0.6666667F
#define SWING_DOWN_STEP 0x800
#define SWING_DOWN_DAMP 0.98F
#define SWING_DOWN_LIMIT 1.8F
#define SWING_DOWN_SLOPE 0.6666667F
#define SWING_X_STEP 0x400
#define SWING_X_DAMP 0.99F
#define SWING_X_LIMIT 1.0F
#define SWING_X_SLOPE 0.4F
#define SWING_XZ_STEP 0x600
#define SWING_XZ_DAMP 0.97F
#define SWING_XZ_LIMIT 1.0F
#define SWING_XZ_SLOPE 0.5F
#define SWING_STOP 0.001F
/* what a swing kind keeps of a point's own place on each step, while
 * by_kind.steps counts and after */
#define SWING_KEEP 0.9F
#define SWING_KEEP_AFTER 0.1F
/* JIGGLE_FLUTTER_UP's tuning. phase steps by FLUTTER_STEP up to twice that,
 * at random. each segment's phase lags the segment before by FLUTTER_LAG. a
 * segment's bend is the angle whose tangent is njSin of its phase times
 * FLUTTER_SLOPE. FLUTTER_STEPS is the count it leaves in by_kind.steps */
#define FLUTTER_STEP 2048.0F
#define FLUTTER_LAG 0x1000
#define FLUTTER_SLOPE 0.5F
#define FLUTTER_STEPS 120
/* JIGGLE_FLUTTER_UP's shares of a point's own place and of its bent place, on
 * each step */
#define FLUTTER_KEEP 0.9F
#define FLUTTER_EASE 0.1F

/* the words of a chain's points, or of their speeds */
#define CHAIN_WORDS (JIGGLE_CHAIN_POINTS * sizeof(NJS_POINT3) / sizeof(Float))

/* jiggle_chain's modes: the copy eases toward the chain's shape, and the
 * chain's x is capped at param */
#define CHAIN_EASED 0x1
#define CHAIN_CAPPED 0x2
/* jiggle_chain scales the way from point i - 1 to point i by the link's rise.
 * it then raises that way's y to at least CHAIN_SAG to the power 2i */
#define CHAIN_SAG 0.94F
/* without a push, each step keeps CHAIN_KEEP of a point's z and multiplies its
 * y by CHAIN_RISE. it then adds the point's speed. point 1 adds no speed: it
 * keeps CHAIN_KEEP of its x and adds CHAIN_FIRST_LEAN. once the chain settles,
 * point i keeps CHAIN_SETTLED_KEEP of its x in place of its x speed. it adds i
 * to the power CHAIN_LEAN_POWER times CHAIN_SETTLED_LEAN */
#define CHAIN_KEEP 0.9F
#define CHAIN_RISE 1.1F
#define CHAIN_FIRST_LEAN 0.3F
#define CHAIN_SETTLED_KEEP 0.96F
#define CHAIN_SETTLED_LEAN 0.012F
#define CHAIN_LEAN_POWER 0.17F
/* the first point whose x speed the test for a settled chain reads */
#define CHAIN_SETTLE_FROM 2
/* JIGGLE_CHAIN_EASED's shares of a point's own place and of the chain's place,
 * on each step */
#define CHAIN_EASE_KEEP 0.92F
#define CHAIN_EASE 0.08F

/* the chain jiggle_create starts each jiggle with */
static NJS_POINT3 chain_start[JIGGLE_CHAIN_POINTS] = {
    {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F},  {0.0F, 2.0F, 0.0F},
    {0.0F, 3.0F, 0.0F}, {0.0F, 4.0F, 0.0F},  {0.0F, 5.0F, 0.0F},
    {0.0F, 6.0F, 0.0F}, {0.0F, 7.0F, 0.0F},  {0.0F, 8.0F, 0.0F},
    {0.0F, 9.0F, 0.0F}, {0.0F, 10.0F, 0.0F},
};

/* JIGGLE_SQUASH's tuning. param shrinks by squash_damp while its size is
 * above squash_damp_reach, and by squash_damp_small once it isn't. it
 * becomes 0 below squash_stop. the swing clips its size to squash_limit */
static Angle squash_phase_step = 0x480;
static Float squash_damp_reach = 0.01F;
static Float squash_damp = 0.92F;
static Float squash_damp_small = 0.9F;
static Float squash_stop = 0.0001F;
static Float squash_limit = 0.9F;

/* the chain kinds' tuning. a link's rise is the difference of its ends' y
 * in chain_heights. a force longer than chain_push pushes the chain. without
 * one, each point i steps its speed by chain_step times chain_step_falloff
 * to the power i. point 1's speed steps toward 0. a later point's x speed steps
 * toward the x of the point before less its own. its y and z speeds step toward
 * 0. that x speed then loses i times chain_lean_far beyond chain_reach of the y
 * axis. within that reach it loses i times chain_lean_near. it also takes
 * chain_drag of the x speed of the point before. the chain settles only under a
 * force no longer than chain_settle_force. then it settles once its points from
 * the third on move slower than chain_settle_speed in x */
static NJS_POINT3 chain_heights[JIGGLE_CHAIN_POINTS] = {
    {0.0F, -2.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, {0.0F, 3.0F, 0.0F},
    {0.0F, 6.0F, 0.0F},  {0.0F, 9.0F, 0.0F},  {0.0F, 11.0F, 0.0F},
    {0.0F, 13.0F, 0.0F}, {0.0F, 15.0F, 0.0F}, {0.0F, 17.0F, 0.0F},
    {0.0F, 19.0F, 0.0F}, {0.0F, 20.0F, 0.0F},
};
static Float chain_step_falloff = 0.5F;
static Float chain_step = 0.07F;
static Float chain_settle_speed = 4.0F;
static Float chain_lean_near = -0.0001F;
static Float chain_lean_far = -0.8F;
static Float chain_drag = 0.17F;
static Float chain_settle_force = 0.001F;
static Float chain_push = 0.03F;
static Float chain_reach = 12.0F;

static Uint32 saved_attr_and;
static Uint32 saved_attr_or;
static Uint32 saved_control_3d;
static NJS_MATRIX tree_matrices[TREE_NODES];

/* nothing calls the functions declared below, work_on_screen to string_format.
 * SHC leaves out a static function that nothing calls */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* TRUE where work->pos projects inside the 640 by 480 screen, widened on
 * every side by a margin. camera_task_work must not be null. at a distance
 * of 0 from the camera, the margin is unset */
Bool work_on_screen(struct task_work *work);
/* blend the points of from's first chunk toward to's into out's, as
 * points_blend_sine does. each chunk must hold six floats to a vertex, a
 * point and then its normal. the count blended is from's index offset, not
 * its count of vertices */
void model_blend(NJS_CNK_MODEL *from, NJS_CNK_MODEL *to, NJS_CNK_MODEL *out,
                 Float blend);
/* out's point = from's * (1 - weight) + to's * weight, for count vertices of
 * six floats, a point and a normal. the normals are left as they are. weight
 * is blend, or the sine of blend quarter turns for points_blend_sine, or
 * blend squared for points_blend_square */
void points_blend(Float *from, Float *to, Float *out, Float blend,
                  Uint32 count);
void points_blend_square(Float *from, Float *to, Float *out, Float blend,
                         Uint32 count);
/* object's place in the walk from root, as tree_list_nodes lists it, or -1
 * where the walk never reaches it */
int tree_node_index(NJS_OBJECT *root, NJS_OBJECT *object);
/* world_sweep_ball, skipping nothing. world_sweep_all_hit returns TRUE where
 * the ball meets any object, and FALSE otherwise */
Bool world_sweep_all_hit(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                         Float radius);
int world_sweep_all(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                    Float radius);
/* set or clear flags in the evalflags of each object of a null-ended list */
void evalflags_set_all(NJS_OBJECT **objects, Uint32 flags);
void evalflags_clear_all(NJS_OBJECT **objects, Uint32 flags);
/* compute the matrices of object and its descendants into tree_matrices,
 * in walk order. the walk starts from base, or from Ninja's current matrix
 * where base is null. a tree of more than TREE_NODES writes past the table,
 * and the next call overwrites it. the result is the entry at index, and
 * tree_get_matrix copies one out */
NJS_MATRIX *tree_calc_matrices(NJS_MATRIX *base, NJS_OBJECT *object, int index);
void tree_get_matrix(NJS_MATRIX *out, int index);
/* set sprite to draw texanim's cells of texlist, at the origin, unscaled and
 * unturned */
void sprite_init(NJS_SPRITE *sprite, NJS_TEXLIST *texlist,
                 NJS_TEXANIM *texanim);
/* string_vformat with the arguments after format */
int string_format(char *buffer, const char *format, ...);
/* NOLINTEND(misc-use-internal-linkage) */

static void points_blend_sine(Float *from, Float *to, Float *out, Float blend,
                              Uint32 count);
static NJS_OBJECT *tree_find_node(NJS_OBJECT *node, NJS_OBJECT *object,
                                  int *count);
static int tree_collect_nodes(NJS_OBJECT **nodes, int *count);
static void jiggle_bend(Float *from, Float *to, struct jiggle *jiggle,
                        Float curl);
static void jiggle_ease(Float *to, Float *target, struct jiggle *jiggle,
                        Float keep);
static void jiggle_bend_eased(Float *from, Float *to, struct jiggle *jiggle,
                              Float curl);
static void jiggle_chain(Float *from, Float *to, struct jiggle *jiggle,
                         int mode);

Angle angle_approach(Angle from, Angle to, Angle step)
{
    Sint16 difference;

    from = (Uint16)from;
    to = (Uint16)to;
    difference = to - from;
    if (difference <= step && difference >= -step) {
        return to;
    }
    /* @hack spelling: the sign of difference tested by its bit, where
     * difference < 0 would do */
    if (difference & 0x8000) {
        to = from - step;
    } else {
        to = from + step;
    }
    return (Uint16)to;
}

/* @hack spelling: angle_difference expanded where angle_unwrap calls it,
 * where a call would do */
#pragma inline(angle_difference)

Angle angle_difference(Angle from, Angle to)
{
    Sint16 turn;

    from = (Uint16)from;
    to = (Uint16)to;
    turn = to - from;
    return turn;
}

Angle angle_distance(Angle from, Angle to)
{
    Uint16 distance;

    from = (Uint16)from;
    to = (Uint16)to;
    distance = to - from;
    if ((Sint16)distance < 0) {
        distance = -distance;
    }
    return distance;
}

Angle angle_unwrap(Angle from, Angle to)
{
    Angle difference;
    Angle unwrapped;

    difference = angle_difference(from, to);
    if (difference > 0) {
        unwrapped = to;
        while (from > unwrapped) {
            unwrapped += FULL_TURN;
        }
    } else if (difference < 0) {
        unwrapped = to;
        while (from < unwrapped) {
            unwrapped -= FULL_TURN;
        }
    } else {
        unwrapped = from;
    }
    return unwrapped;
}

Angle vector_angle(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *axis)
{
    NJS_VECTOR cross;
    Float cosine;
    Angle angle;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((cosine = njInnerProduct(first, second)) <= -1.0F) {
        return HALF_TURN;
    } else if (cosine >= 1.0F) {
        return 0;
    } else {
        angle = NJM_RAD_ANG(acosf(cosine));
        njOuterProduct(first, second, &cross);
        if (axis != NULL && njInnerProduct(axis, &cross) < 0.0F) {
            angle = FULL_TURN - angle;
        }
        return angle;
    }
}

Bool work_on_screen(struct task_work *work)
{
    NJS_POINT2 point;
    Float margin;
    Float distance;
    Float across;
#ifdef NONMATCHING
    /* the differences this side squares for the distance the tag below keeps */
    Float dx;
    Float dy;
    Float dz;
#endif

    njPushMatrix(NULL);
    njProjectScreen(NULL, &work->pos, &point);
    njPopMatrix(1);
    distance = njDistanceP2P(&camera_task_work->pos, &work->pos);
    /* @bug uninitialized: at a distance of 0 the BT at 0x8c02d4b8 skips the
     * division that sets margin. the FADD at 0x8c02d4ca then reads FR4 as
     * njDistanceP2P left it: the squared distance, which is below 0.025 */
#ifdef NONMATCHING
    /* this side keeps the tag's value, the squared distance. it sums it as
     * njDistanceP2P's FMUL and two FMACs do, from dy's square */
    dx = camera_task_work->pos.x - work->pos.x;
    dy = camera_task_work->pos.y - work->pos.y;
    dz = camera_task_work->pos.z - work->pos.z;
    margin = __builtin_fmaf(dz, dz, __builtin_fmaf(dx, dx, dy * dy));
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (distance != 0.0F) {
        margin = MARGIN_REACH / distance;
    }
    across = (point.x + margin) / (SCREEN_WIDTH + (2.0F * margin));
    if (across < 0.0F || across > 1.0F) {
        return FALSE;
    }
    if ((SCREEN_MIDDLE_Y - point.y) * (SCREEN_MIDDLE_Y - point.y) <
        (margin + SCREEN_MIDDLE_Y) * (margin + SCREEN_MIDDLE_Y)) {
        return TRUE;
    }
    return FALSE;
}

void model_blend(NJS_CNK_MODEL *from, NJS_CNK_MODEL *to, NJS_CNK_MODEL *out,
                 Float blend)
{
    Uint32 count;
    Float *source;
    Float *toward;
    Float *dest;

    /* @hack spelling: the arguments through locals, where the call could
     * take them straight */
    /* @bug logic: takes the count of points from the low half of the
     * chunk's second word, its index offset (EXTU.W at 0x8c02d51a).
     * jiggle_create takes a chunk's count from the high half */
    count = (Uint16)from->vlist[1];
    source = CHUNK_VERTICES(from->vlist);
    toward = CHUNK_VERTICES(to->vlist);
    dest = CHUNK_VERTICES(out->vlist);
    points_blend_sine(source, toward, dest, blend, count);
}

void points_blend(Float *from, Float *to, Float *out, Float blend, Uint32 count)
{
    Uint32 i;
    Float keep = 1.0F - blend;

    for (i = count; i > 0; i--) {
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        from += NORMAL_FLOATS;
        to += NORMAL_FLOATS;
        out += NORMAL_FLOATS;
    }
}

static void points_blend_sine(Float *from, Float *to, Float *out, Float blend,
                              Uint32 count)
{
    Uint32 i;
    Float keep;

    blend = njSin((Angle)(blend * QUARTER_TURN));
    keep = 1.0F - blend;
    for (i = count; i > 0; i--) {
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        from += NORMAL_FLOATS;
        to += NORMAL_FLOATS;
        out += NORMAL_FLOATS;
    }
}

void points_blend_square(Float *from, Float *to, Float *out, Float blend,
                         Uint32 count)
{
    Uint32 i;
    Float keep;

    blend = blend * blend;
    keep = 1.0F - blend;
    for (i = count; i > 0; i--) {
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        *out++ = (*from++ * keep) + (*to++ * blend);
        from += NORMAL_FLOATS;
        to += NORMAL_FLOATS;
        out += NORMAL_FLOATS;
    }
}

int tree_node_index(NJS_OBJECT *root, NJS_OBJECT *object)
{
    NJS_OBJECT *node;
    int count;
    NJS_OBJECT *found;

    count = 0;
    /* @hack spelling: root through node, where root itself would do */
    node = root;
    found = tree_find_node(node, object, &count);
    if (found == NULL) {
        return -1;
    }
    return count;
}

/* object when the walk from node reaches it, and null otherwise. *count
 * gains 1 for each node the walk passes before object */
static NJS_OBJECT *tree_find_node(NJS_OBJECT *node, NJS_OBJECT *object,
                                  int *count)
{
    NJS_OBJECT *next;

    if (node == object) {
        return object;
    }
    (*count)++;
    /* @hack spelling: each child and its walk's result through next, where
     * the result compared in place would do */
    if ((next = node->child) != NULL) {
        next = tree_find_node(next, object, count);
        if (next == object) {
            return object;
        }
    }
    if ((next = node->sibling) != NULL) {
        next = tree_find_node(next, object, count);
        if (next == object) {
            return object;
        }
    }
    return NULL;
}

Float float_wrap(Float value, Float period)
{
    if (period == 0.0F) {
        return 0.0F;
    }
    return value - (floorf(value / period) * period);
}

int tree_list_nodes(NJS_OBJECT *root, NJS_OBJECT **nodes)
{
    int count;

    count = 0;
    if (nodes != NULL) {
        nodes[count] = root;
    }
    return tree_collect_nodes(nodes, &count);
}

/* list the walk from the node at nodes[*count] into the entries after it,
 * and return the new *count. each node goes into nodes when the walk reaches
 * it, and the walk reads it back from there */
static int tree_collect_nodes(NJS_OBJECT **nodes, int *count)
{
    /* @bug memory: reads the node through nodes untested (MOV.L
     * @(R0,R13),R12 at 0x8c02d792), where each store below tests nodes for
     * null (TST R13,R13 at 0x8c02d79e and at 0x8c02d7b4), as
     * tree_list_nodes does (TST R14,R14 at 0x8c02d72c) */
    NJS_OBJECT *node = nodes[*count];
    NJS_OBJECT *next;

    (*count)++;
    next = node->child;
    if (next != NULL) {
        if (nodes != NULL) {
            nodes[*count] = next;
        }
        tree_collect_nodes(nodes, count);
    }
    next = node->sibling;
    if (next != NULL) {
        if (nodes != NULL) {
            nodes[*count] = next;
        }
        tree_collect_nodes(nodes, count);
    }
    return *count;
}

void vector_get_angles(NJS_VECTOR *dir, NJS_VECTOR *from, Angle angles[3])
{
    Float across = njSqrt((dir->x * dir->x) + (dir->z * dir->z));

    angles[1] = NJM_RAD_ANG(atan2f(dir->x, -dir->z));
    angles[2] = NJM_RAD_ANG(atan2f(-dir->y, across));
    if (from != NULL) {
        Float from_across = njSqrt((from->x * from->x) + (from->z * from->z));

        angles[1] -= NJM_RAD_ANG(atan2f(from->x, -from->z));
        angles[2] -= NJM_RAD_ANG(atan2f(-from->y, from_across));
    }
}

void draw_attr_save(void)
{
    saved_attr_and = _nj_constant_attr_and_;
    saved_attr_or = _nj_constant_attr_or_;
}

void draw_attr_restore(void)
{
    _nj_constant_attr_and_ = saved_attr_and;
    _nj_constant_attr_or_ = saved_attr_or;
}

void draw_attr_set(Uint32 and_attr, Uint32 or_attr)
{
    _nj_constant_attr_and_ |= and_attr;
    _nj_constant_attr_or_ |= or_attr;
}

void draw_attr_clear(Uint32 and_attr, Uint32 or_attr)
{
    _nj_constant_attr_and_ &= ~and_attr;
    _nj_constant_attr_or_ &= ~or_attr;
}

void draw_control_save(void)
{
    saved_control_3d = _nj_control_3d_flag_;
}

void draw_control_restore(void)
{
    _nj_control_3d_flag_ = saved_control_3d;
}

void draw_control_set(Uint32 bits)
{
    _nj_control_3d_flag_ |= bits;
}

void draw_control_clear(Uint32 bits)
{
    _nj_control_3d_flag_ &= ~bits;
}

Bool world_sweep_all_hit(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                         Float radius)
{
    return world_sweep_ball(pos, speed, ang, 0, radius) != 0;
}

Bool world_sweep_ball_hit(NJS_POINT3 *pos, NJS_VECTOR *speed,
                          struct angles *ang, int skip, Float radius)
{
    return world_sweep_ball(pos, speed, ang, skip, radius) != 0;
}

int world_sweep_all(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                    Float radius)
{
    return world_sweep_ball(pos, speed, ang, 0, radius);
}

int world_sweep_ball(NJS_POINT3 *pos, NJS_VECTOR *speed, struct angles *ang,
                     int skip, Float radius)
{
    struct polygon_ball hit;
    struct polygon_ball best;
    struct angles offset;
    struct polygon_ball *ball;
    struct world_object *objects;
    struct task *owner;
    struct task_force *force;
    Uint16 i;
    int result;
    Angle slope_x;
    Angle slope_z;
    Angle flipped_x;
    Angle flipped_z;
    Angle gap_x;
    Angle gap_z;
    Float nearest;
    Float distance;

    result = 0;
    objects = world_objects;
    ball = &hit;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (ang == NULL) {
        ball->up.x = 0.0F;
        ball->up.y = 1.0F;
        ball->up.z = 0.0F;
    } else {
        /* @hack spelling: up and unit_y in a scope of their own, where the
         * function's would do */
        NJS_VECTOR up;
        NJS_VECTOR unit_y;

        unit_y.x = 0.0F;
        unit_y.y = 1.0F;
        unit_y.z = 0.0F;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateZ(NULL, ang->z);
        njRotateX(NULL, ang->x);
        njRotateY(NULL, ang->y);
        njCalcVector(NULL, &unit_y, &up);
        njPopMatrix(1);
        ball->up = up;
    }
    ball->pos = *pos;
    ball->radius = radius;
    ball->speed = *speed;
    polygon_collect_near(ball);
    for (i = 0; i < world_count; i++) {
        /* @hack spelling: turn and moved in the loop's scope, where the
         * function's would do */
        struct angles turn;
        NJS_VECTOR moved;

        if (objects[i].flags & skip) {
            continue;
        }
        if (polygon_sweep(ball, objects[i].object, objects[i].flags, 0) != 0) {
            if (ang != NULL && ball->hit.normal.y < 0.0F) {
                slope_x = NJM_RAD_ANG(asinf(ball->hit.normal.z));
                slope_z = -NJM_RAD_ANG(asinf(ball->hit.normal.x));
                flipped_x = (Uint16)(HALF_TURN - slope_x);
                flipped_z = (Uint16)(HALF_TURN - slope_z);
                gap_x = angle_distance(ang->x, flipped_x);
                gap_z = angle_distance(ang->z, flipped_z);
                if (gap_x < gap_z) {
                    ball->hit.ang_x = flipped_x;
                    ball->hit.ang_z = slope_z;
                } else {
                    ball->hit.ang_x = slope_x;
                    ball->hit.ang_z = flipped_z;
                }
            }
            owner = objects[i].owner;
            if (owner != NULL) {
                force = owner->force.p;
                /* @bug uninitialized: turn is unset for an object whose owner
                 * has no force, since the BT at 0x8c02db58 skips every store
                 * to it. turn then holds what its frame slot last held. that
                 * is an earlier object's turn, or else up's floats where ang
                 * isn't null. where that object is the nearest, offset takes
                 * turn (0x8c02dbe0 to 0x8c02dbee, 0x8c02dc10 to 0x8c02dc1c).
                 * *ang then adds offset (0x8c02dcb8 to 0x8c02dcce) */
#ifdef NONMATCHING
                /* this side repairs the defect the tag above names. C can't
                 * give the frame slot's floats, so an object whose owner has no
                 * force turns by 0 on this side, as one with no owner does */
                turn.x = 0;
                turn.y = 0;
                turn.z = 0;
#endif
                if (force != NULL) {
                    ball->hit.pos.x += force->speed.x;
                    ball->hit.pos.y += force->speed.y;
                    ball->hit.pos.z += force->speed.z;
                    turn = force->turn;
                }
            } else {
                turn.x = 0;
                turn.y = 0;
                turn.z = 0;
            }
            moved.x = ball->hit.pos.x - pos->x;
            moved.y = ball->hit.pos.y - pos->y;
            moved.z = ball->hit.pos.z - pos->z;
            if (result == 0) {
                best = hit;
                nearest = njScalor(&moved);
                offset = turn;
            } else {
                distance = njScalor(&moved);
                if (nearest > distance) {
                    best = hit;
                    nearest = distance;
                    offset = turn;
                }
            }
            if (njInnerProduct(&ball->hit.normal, &world_gravity) > -0.5F) {
                result |= SWEEP_WALL;
            } else {
                result |= SWEEP_FLOOR;
            }
        }
    }
    if (result != 0) {
        /* @hack spelling: best read through ball, where best itself would
         * do */
        ball = &best;
        *pos = ball->hit.pos;
        *speed = ball->hit.speed;
        if (ang != NULL) {
            ang->x = ball->hit.ang_x + offset.x;
            ang->y += offset.y;
            ang->z = ball->hit.ang_z + offset.z;
        }
    }
    return result;
}

void evalflags_set_all(NJS_OBJECT **objects, Uint32 flags)
{
    while (*objects != NULL) {
        (*objects)->evalflags |= flags;
        objects++;
    }
}

void evalflags_clear_all(NJS_OBJECT **objects, Uint32 flags)
{
    while (*objects != NULL) {
        (*objects)->evalflags &= ~flags;
        objects++;
    }
}

/* store object's matrix at *matrices, and its descendants' after it, in
 * walk order. a node's matrix is its parent's, translated, turned and scaled
 * by the node's pos, ang and scl. *matrices moves past each. the result is
 * object's next sibling, for the caller to take next */
static NJS_OBJECT *tree_calc_node(NJS_MATRIX **matrices, NJS_OBJECT *object,
                                  NJS_MATRIX *parent)
{
    NJS_MATRIX matrix;
    NJS_OBJECT *child;

    njSetMatrix(&matrix, parent);
    njTranslate(&matrix, object->pos[0], object->pos[1], object->pos[2]);
    njRotateZ(&matrix, object->ang[2]);
    njRotateY(&matrix, object->ang[1]);
    njRotateX(&matrix, object->ang[0]);
    njScaleV(&matrix, (NJS_VECTOR *)object->scl);
    njSetMatrix(*matrices, &matrix);
    (*matrices)++;
    child = object->child;
    while (child != NULL) {
        child = tree_calc_node(matrices, child, &matrix);
    }
    return object->sibling;
}

NJS_MATRIX *tree_calc_matrices(NJS_MATRIX *base, NJS_OBJECT *object, int index)
{
    NJS_MATRIX *matrices;
    NJS_MATRIX matrix;

    if (base == NULL) {
        njGetMatrix(&matrix);
    } else {
        njSetMatrix(&matrix, base);
    }
    matrices = tree_matrices;
    tree_calc_node(&matrices, object, &matrix);
    return &tree_matrices[index];
}

void tree_get_matrix(NJS_MATRIX *out, int index)
{
    njSetMatrix(out, &tree_matrices[index]);
}

void sprite_init(NJS_SPRITE *sprite, NJS_TEXLIST *texlist, NJS_TEXANIM *texanim)
{
    sprite->tlist = texlist;
    sprite->tanim = texanim;
    sprite->ang = 0;
    sprite->sx = 1.0F;
    sprite->sy = 1.0F;
    sprite->p.x = 0.0F;
    sprite->p.y = 0.0F;
    sprite->p.z = 0.0F;
}

/* write value's digits back from end, a single 0 for 0, and return the
 * first. format_hex and format_hex_upper write hex digits */
static char *format_decimal(char *end, Uint32 value)
{
    do {
        *--end = (char)((value % 10) + '0');
        value /= 10;
    } while (value != 0);
    return end;
}

static char *format_hex(char *end, Uint32 value)
{
    int digit;

    do {
        digit = value & 15;
        *--end = (char)((digit < 10 ? '0' : 'a' - 10) + digit);
        value >>= 4;
    } while (value != 0);
    return end;
}

static char *format_hex_upper(char *end, Uint32 value)
{
    int digit;

    do {
        digit = value & 15;
        *--end = (char)((digit < 10 ? '0' : 'A' - 10) + digit);
        value >>= 4;
    } while (value != 0);
    return end;
}

/* SHC's va_list is a char pointer. va_arg moves it but never writes through
 * it, so clang-tidy would have it point to const */
int string_vformat(char *buffer, const char *format,
                   va_list args) /* NOLINT(readability-non-const-parameter) */
{
    char *out;
    char *field;
    char *end;
    int ch;
    int sign;
    Bool zero;
    int width;
    Bool left;
    int length;
    int pad;
    int value;
    char text[DIGITS_SIZE];

    out = buffer;
    end = &text[sizeof(text) - 1];
    *end = '\0';
    while ((ch = *format++) != 0) {
        if (ch == '%') {
            width = 0;
            left = FALSE;
            sign = 0;
            zero = FALSE;
            /* @hack spelling: the flags read by a goto back to flag, where a
             * loop would do */
        flag:
            ch = *format++;
            if (ch == '-') {
                left = TRUE;
                goto flag;
            } else if (ch == '+') {
                sign = 1;
                goto flag;
            } else if (ch == '0') {
                zero = TRUE;
                goto flag;
            }
            while (ch >= '0' && ch <= '9') {
                width = (width * 10) + ch - '0';
                ch = *format++;
            }
            switch (ch) {
            case 's':
                field = va_arg(args, char *);
                break;
            case 'c':
                field = end;
                *--field = va_arg(args, int);
                break;
            case 'u':
                field = format_decimal(end, va_arg(args, Uint32));
                if (zero) {
                    while (field > end - width + sign) {
                        *--field = '0';
                    }
                }
                if (sign) {
                    *--field = '+';
                }
                break;
            case 'd':
                value = va_arg(args, int);
                if (value >= 0) {
                    field = format_decimal(end, value);
                } else {
                    sign = 1;
                    field = format_decimal(end, -value);
                }
                if (zero) {
                    while (field > end - width + sign) {
                        *--field = '0';
                    }
                }
                if (sign) {
                    *--field = value < 0 ? '-' : '+';
                }
                break;
            case 'x':
                field = format_hex(end, va_arg(args, Uint32));
                if (zero) {
                    while (field > end - width + sign) {
                        *--field = '0';
                    }
                }
                break;
            case 'X':
                field = format_hex_upper(end, va_arg(args, Uint32));
                if (zero) {
                    while (field > end - width + sign) {
                        *--field = '0';
                    }
                }
                break;
            default:
                (void)va_arg(args, int);
                field = end;
                break;
            }
            length = strlen(field);
            pad = width - length;
            if (left) {
                strcpy(out, field);
                out += length;
                while (pad > 0) {
                    *out++ = ' ';
                    pad--;
                }
            } else {
                while (pad > 0) {
                    *out++ = ' ';
                    pad--;
                }
                strcpy(out, field);
                out += length;
            }
        } else {
            *out++ = ch;
        }
    }
    *out = '\0';
    return out - buffer;
}

int string_format(char *buffer, const char *format, ...)
{
    va_list args;
    int length;

    va_start(args, format);
    length = string_vformat(buffer, format, args);
    va_end(args);
    return length;
}

Float float_approach(Float from, Float to, Float step)
{
    Float result;

    /* @hack spelling: one return, of result, where a return in each branch
     * would do */
    if (from > to) {
        result = from - step;
        if (to > result) {
            result = to;
        }
    } else if (to > from) {
        result = from + step;
        if (result > to) {
            result = to;
        }
    } else {
        result = from;
    }
    return result;
}

struct jiggle *jiggle_create(NJS_CNK_OBJECT *source)
{
    /* @hack redundant: source_model, a volatile copy of source's model that
     * nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    NJS_CNK_MODEL *volatile source_model;
    Sint32 *chunk;
    /* @hack spelling: NJD_CE through end, where the constant itself would
     * do */
    Sint32 end = NJD_CE;
    Sint32 *vlist;
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *copy;
    struct jiggle *jiggle;
    int vertex_count;
    int size;
    int words;
    int i;
    Sint32 *speed;

    source_model = source->model;
    chunk = source->model->vlist;
    vertex_count = HIGH_HALF(chunk[1]);
    size = 0;
    while (*chunk != end) {
        words = HIGH_HALF(*chunk) + 1;
        size += words * 4;
        chunk += words;
    }
    size += 4; /* the end chunk's one word */
    if ((vlist = syMalloc(size)) == NULL) {
        return NULL;
    }
    if ((model = syMalloc(sizeof(NJS_CNK_MODEL))) == NULL) {
        syFree(vlist);
        return NULL;
    }
    if ((copy = syMalloc(sizeof(NJS_CNK_OBJECT))) == NULL) {
        syFree(model);
        syFree(vlist);
        return NULL;
    }
    if ((jiggle = syMalloc(sizeof(struct jiggle))) == NULL) {
        syFree(copy);
        syFree(model);
        syFree(vlist);
        return NULL;
    }
    jiggle->param = 0.9F;
    jiggle->by_kind.angle = 0x1000;
    jiggle->phase = 0;
    jiggle->phase_step = 0x400;
    jiggle->copy = copy;
    jiggle->source = source;
    jiggle->vertex_count = vertex_count;
    njMemCopy4(jiggle->points, chain_start, CHAIN_WORDS);
    /* @hack type: the speeds cleared as Sint32 words, counted down, where a
     * loop that sets each speed's Floats to 0 would do */
    speed = (Sint32 *)jiggle->speeds;
    i = CHAIN_WORDS;
    do {
        *speed++ = 0;
    } while (--i);
    *copy = *source;
    *model = *source->model;
    copy->model = model;
    njMemCopy4(vlist, source->model->vlist, (size + 3) >> 2);
    model->vlist = vlist;
    return jiggle;
}

void jiggle_free(struct jiggle *jiggle)
{
    if (jiggle != NULL) {
        if (jiggle->copy != NULL) {
            if (jiggle->copy->model != NULL) {
                if (jiggle->copy->model->vlist != NULL) {
                    syFree(jiggle->copy->model->vlist);
                    jiggle->copy->model->vlist = NULL;
                }
                syFree(jiggle->copy->model);
                jiggle->copy->model = NULL;
            }
            syFree(jiggle->copy);
            jiggle->copy = NULL;
        }
        syFree(jiggle);
    }
}

void jiggle_step(struct jiggle *jiggle)
{
    Float *from;
    Float *to;

    from = CHUNK_VERTICES(jiggle->source->model->vlist);
    to = CHUNK_VERTICES(jiggle->copy->model->vlist);
    switch (jiggle->kind) {
    case JIGGLE_WAVE_CENTERED: {
        Float scale;
        Float base;
        Float x;
        Float y;
        Float z;
        Float radius;
        int count;
        Angle angle;
        Angle phase;

        phase = jiggle->phase;
        jiggle->phase += jiggle->phase_step;
        base = jiggle->param;
        angle = jiggle->by_kind.angle;
        radius = jiggle->copy->model->r;
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++ - radius;
            z = *from++;
            from += NORMAL_FLOATS;
            scale = (njSin((angle * y) + phase) * (1.0F - base)) + base;
            *to = scale * x;
            to++;
            *to = radius + (y * scale);
            to++;
            *to = scale * z;
            to += 1 + NORMAL_FLOATS;
        }
        break;
    }
    case JIGGLE_WAVE:
    case JIGGLE_WAVE_TWIN: {
        Float scale;
        Float base;
        Float x;
        Float y;
        Float z;
        int count;
        Angle angle;
        Angle phase;

        phase = jiggle->phase;
        jiggle->phase += WAVE_STEP(jiggle->phase_step);
        base = jiggle->param;
        angle = jiggle->by_kind.angle;
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            scale = (njSin((angle * y) + phase) * (1.0F - base)) + base;
            *to = scale * x;
            to++;
            *to = scale * y;
            to++;
            *to = scale * z;
            to += 1 + NORMAL_FLOATS;
        }
        break;
    }
    case JIGGLE_WAVE_SWAY: {
        Float scale;
        Float base;
        Float x;
        Float y;
        Float z;
        int count;
        Angle angle;
        Angle phase;

        phase = jiggle->phase;
        jiggle->phase += WAVE_STEP(jiggle->phase_step);
        base = jiggle->param;
        angle = jiggle->by_kind.angle;
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            scale = (njSin((angle * y) + phase) * (1.0F - base)) + base;
            *to = scale * x;
            to++;
            if (y < SWAY_BELOW) {
                *to = y;
                to++;
                if (x > 0.0F) {
                    *to = z + njSin((angle * y) + phase);
                } else {
                    *to = z - njSin((angle * y) + phase);
                }
            } else {
                *to = scale * y;
                to++;
                *to = scale * z;
            }
            to += 1 + NORMAL_FLOATS;
        }
        break;
    }
    case JIGGLE_WAVE_SWAY_WIDE: {
        Float scale;
        Float base;
        Float x;
        Float y;
        Float z;
        int count;
        Angle angle;
        Angle phase;

        phase = jiggle->phase;
        jiggle->phase += WAVE_STEP(jiggle->phase_step);
        base = jiggle->param;
        angle = jiggle->by_kind.angle;
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            scale = (njSin((angle * y) + phase) * (1.0F - base)) + base;
            *to = scale * x;
            to++;
            if (y < SWAY_WIDE_BELOW) {
                *to = y;
                to++;
                if (x > 0.0F) {
                    *to = z + (njSin((angle * y) + (phase * SWAY_WIDE_RATE)) *
                               SWAY_WIDE_SIZE);
                } else {
                    *to = z - (njSin((angle * y) + (phase * SWAY_WIDE_RATE)) *
                               SWAY_WIDE_SIZE);
                }
            } else {
                *to = scale * y;
                to++;
                *to = scale * z;
            }
            to += 1 + NORMAL_FLOATS;
        }
        break;
    }
    case JIGGLE_BEND:
        jiggle_bend(from, to, jiggle, 1.0F);
        switch (jiggle->phase & BEND_STEP_BITS) {
        case 0x00000:
            jiggle->phase_step = 0x300;
            break;
        case 0x10000:
            jiggle->phase_step = 0x220;
            break;
        case 0x20000:
            jiggle->phase_step = 0x2c0;
            break;
        case 0x30000:
            jiggle->phase_step = 0x280;
            break;
        case 0x40000:
            jiggle->phase_step = 0x140;
            break;
        case 0x50000:
            jiggle->phase_step = 0x1c0;
            break;
        case 0x60000:
            jiggle->phase_step = 0x240;
            break;
        case 0x70000:
            jiggle->phase_step = 0x2a0;
            break;
        }
        break;
    case JIGGLE_EASE_A: {
        Float *toward;

        toward = CHUNK_VERTICES(jiggle->shape_a->model->vlist);
        jiggle_ease(to, toward, jiggle, EASE_A_KEEP);
        break;
    }
    case JIGGLE_EASE_B: {
        Float *toward;

        toward = CHUNK_VERTICES(jiggle->shape_b->model->vlist);
        jiggle_ease(to, toward, jiggle, EASE_B_KEEP);
        break;
    }
    case JIGGLE_BEND_EASED_BACK:
        jiggle_ease(to, from, jiggle, BEND_BACK_KEEP);
        /* fall through */
    case JIGGLE_BEND_EASED:
        jiggle_bend_eased(from, to, jiggle, 1.0F);
        break;
    case JIGGLE_CHAIN_EASED:
        jiggle_chain(from, to, jiggle, CHAIN_EASED);
        break;
    case JIGGLE_SQUASH: {
        Float swing;
        Float size;
        Float x;
        Float y;
        Float z;
        int count;
        Angle angle;
        Angle phase;

        phase = jiggle->phase;
        jiggle->phase += squash_phase_step;
        size = jiggle->param;
        if (_builtin_fabsf(jiggle->param) > squash_damp_reach) {
            jiggle->param *= squash_damp;
        } else {
            jiggle->param *= squash_damp_small;
        }
        if (_builtin_fabsf(jiggle->param) < squash_stop) {
            jiggle->param = 0.0F;
        }
        if (size > squash_limit) {
            size = squash_limit;
        } else if (size < -squash_limit) {
            size = -squash_limit;
        }
        angle = jiggle->by_kind.angle;
        swing = njSin(angle + phase) * size;
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (y < SQUASH_TOP && y > SQUASH_BOTTOM && x < SQUASH_RIGHT &&
                x > SQUASH_LEFT && z < SQUASH_HALF_DEPTH &&
                z > -SQUASH_HALF_DEPTH) {
                *to = x + _builtin_fabsf(y * swing * SQUASH_ACROSS);
                to++;
                *to = y - (_builtin_fabsf(y * swing) * SQUASH_DOWN);
                to++;
                *to = z + (_builtin_fabsf(swing * SQUASH_OUT) * z);
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = x;
                to++;
                *to = y;
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    case JIGGLE_SWING_UP: {
        /* @hack spelling: amount, the swing and then the share of its place
         * each point keeps, where a local for each would do */
        Float amount;
        Float size;
        Float x;
        Float y;
        Float z;
        Float s1;
        Float s2;
        Float s3;
        Float s4;
        Float c1;
        Float c2;
        Float c3;
        Float c4;
        int count;
        Angle phase;
        Angle bend;

        phase = jiggle->phase;
        jiggle->phase += SWING_UP_STEP;
        size = jiggle->param;
        jiggle->param *= SWING_UP_DAMP;
        if (_builtin_fabsf(jiggle->param) < SWING_STOP) {
            jiggle->param = 0.0F;
        }
        if (size > SWING_UP_LIMIT) {
            size = SWING_UP_LIMIT;
        } else if (size < -SWING_UP_LIMIT) {
            size = -SWING_UP_LIMIT;
        }
        amount = njSin(phase) * size;
        bend = NJM_RAD_ANG(atanf(amount * SWING_UP_SLOPE));
        s1 = njSin(bend >> 2);
        s2 = njSin(bend >> 1);
        s3 = njSin(bend);
        s4 = njSin(LAST_BEND(bend));
        c1 = _builtin_fabsf(njCos(bend >> 2));
        c2 = _builtin_fabsf(njCos(bend >> 1));
        c3 = _builtin_fabsf(njCos(bend));
        c4 = _builtin_fabsf(njCos(LAST_BEND(bend)));
        /* @hack spelling: amount set in each arm, where one store after the
         * test would do */
        if (jiggle->by_kind.steps <= 0) {
            jiggle->by_kind.steps = 0;
            amount = SWING_KEEP;
        } else {
            jiggle->by_kind.steps--;
            amount = SWING_KEEP;
        }
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (y < 5.0F && y > -1.0F && x < 2.2F && x > 0.0F &&
                (z < -1.0F || z > 1.0F)) {
                /* @bug logic: the limb tests its lowest bound first, y above
                 * 0.8 (FCMP/GT at 0x8c02f13c), so its tests of y above 1.5
                 * and 1.6 (0x8c02f1a6, 0x8c02f20c) never hold. every point
                 * above 0.8 bends as the fourth segment. the other limb tests
                 * its bounds from the highest down */
                if (y > 0.8F) {
                    x = ((s3 * (1.5F - 0.8F)) - ((y - 0.8F) * s4) +
                         (s2 * (1.6F - 1.5F)) - (s1 * 1.6F)) +
                        x;
                    y = ((y - 0.8F) * c4) - (c3 * (1.5F - 0.8F)) -
                        (c2 * (1.6F - 1.5F)) + (c1 * 1.6F);
                } else if (y > 1.5F) {
                    x = ((s2 * (1.6F - 1.5F)) - ((y - 1.5F) * s3) -
                         (s1 * 1.6F)) +
                        x;
                    y = ((y - 1.5F) * c3) - (c2 * (1.6F - 1.5F)) + (c1 * 1.6F);
                } else if (y > 1.6F) {
                    x = x - ((y - 1.6F) * s2) - (s1 * 1.6F);
                    y = (c1 * 1.6F) + ((y - 1.6F) * c2);
                } else {
                    x = x - (y * s1);
                    y = y * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else if (y < 5.0F && y > -1.0F && x < 1.5F && x > -5.0F) {
                if (y > 3.0F) {
                    x = x - ((y - 3.0F) * s4) - (s3 * (3.0F - 2.5F)) -
                        (s2 * (2.5F - 1.8F)) - (s1 * 1.8F);
                    y = (c3 * (3.0F - 2.5F)) + ((y - 3.0F) * c4) +
                        (c2 * (2.5F - 1.8F)) + (c1 * 1.8F);
                } else if (y > 2.5F) {
                    x = x - ((y - 2.5F) * s3) - (s2 * (2.5F - 1.8F)) -
                        (s1 * 1.8F);
                    y = (c2 * (2.5F - 1.8F)) + ((y - 2.5F) * c3) + (c1 * 1.8F);
                } else if (y > 1.8F) {
                    x = x - ((y - 1.8F) * s2) - (s1 * 1.8F);
                    y = (c1 * 1.8F) + ((y - 1.8F) * c2);
                } else {
                    x = x - (y * s1);
                    y = y * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    case JIGGLE_FLUTTER_UP: {
        Float swing;
        Float x;
        Float y;
        Float z;
        Float s1;
        Float s2;
        Float s3;
        Float s4;
        Float c1;
        Float c2;
        Float c3;
        Float c4;
        int count;
        Angle phase;
        Angle bend;

        jiggle->by_kind.steps = FLUTTER_STEPS;
        from = CHUNK_VERTICES(jiggle->shape_a->model->vlist);
        jiggle->phase +=
            (rand() * (FLUTTER_STEP / (RAND_MAX + 1))) + FLUTTER_STEP;
        phase = jiggle->phase;
        swing = njSin(phase);
        bend = NJM_RAD_ANG(atanf(swing * FLUTTER_SLOPE));
        s1 = njSin(bend);
        c1 = _builtin_fabsf(njCos(bend));
        phase -= FLUTTER_LAG;
        swing = njSin(phase);
        bend = NJM_RAD_ANG(atanf(swing * FLUTTER_SLOPE));
        s2 = njSin(bend);
        c2 = _builtin_fabsf(njCos(bend));
        phase -= FLUTTER_LAG;
        swing = njSin(phase);
        bend = NJM_RAD_ANG(atanf(swing * FLUTTER_SLOPE));
        s3 = njSin(bend);
        c3 = _builtin_fabsf(njCos(bend));
        phase -= FLUTTER_LAG;
        swing = njSin(phase);
        bend = NJM_RAD_ANG(atanf(swing * FLUTTER_SLOPE));
        s4 = njSin(bend);
        c4 = _builtin_fabsf(njCos(bend));
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (y < 5.0F && y > -1.0F && x < 2.2F && x > -0.1F &&
                (z < -1.4F || z > 1.4F)) {
                /* @bug logic: the limb tests its lowest bound first, y above
                 * 0 (FCMP/GT at 0x8c02f7de), so its tests of y above 1.5 and
                 * 1.6 (0x8c02f848, 0x8c02f8b4) never hold. every point above
                 * 0 bends as the fourth segment. the other limb tests its
                 * bounds from the highest down */
                if (y > 0.0F) {
                    x = ((s3 * (1.5F - 0.0F)) - ((y - 0.0F) * s4) +
                         (s2 * (1.6F - 1.5F)) - (s1 * (1.6F - -2.0F))) +
                        x;
                    y = (((y - 0.0F) * c4) - (c3 * (1.5F - 0.0F)) -
                         (c2 * (1.6F - 1.5F)) + (c1 * (1.6F - -2.0F))) -
                        2.0F;
                } else if (y > 1.5F) {
                    x = ((s2 * (1.6F - 1.5F)) - ((y - 1.5F) * s3) -
                         (s1 * (1.6F - -2.0F))) +
                        x;
                    y = (((y - 1.5F) * c3) - (c2 * (1.6F - 1.5F)) +
                         (c1 * (1.6F - -2.0F))) -
                        2.0F;
                } else if (y > 1.6F) {
                    x = x - ((y - 1.6F) * s2) - (s1 * (1.6F - -2.0F));
                    y = ((c1 * (1.6F - -2.0F)) + ((y - 1.6F) * c2)) - 2.0F;
                } else {
                    x = x - ((y + 2.0F) * s1);
                    y = ((y + 2.0F) * c1) - 2.0F;
                }
                *to = (x * FLUTTER_EASE) + (*to * FLUTTER_KEEP);
                to++;
                *to = (y * FLUTTER_EASE) + (*to * FLUTTER_KEEP);
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else if (y < 5.0F && y > -2.0F && x < 0.0F && x > -5.0F) {
                if (y > 3.0F) {
                    x = x - ((y - 3.0F) * s4) - (s3 * (3.0F - 2.5F)) -
                        (s2 * (2.5F - 0.0F)) - (s1 * (0.0F - -2.0F));
                    y = ((c3 * (3.0F - 2.5F)) + ((y - 3.0F) * c4) +
                         (c2 * (2.5F - 0.0F)) + (c1 * (0.0F - -2.0F))) -
                        2.0F;
                } else if (y > 2.5F) {
                    x = x - ((y - 2.5F) * s3) - (s2 * (2.5F - 0.0F)) -
                        (s1 * (0.0F - -2.0F));
                    y = ((c2 * (2.5F - 0.0F)) + ((y - 2.5F) * c3) +
                         (c1 * (0.0F - -2.0F))) -
                        2.0F;
                } else if (y > 0.0F) {
                    x = x - ((y - 0.0F) * s2) - (s1 * (0.0F - -2.0F));
                    y = ((c1 * (0.0F - -2.0F)) + ((y - 0.0F) * c2)) - 2.0F;
                } else {
                    x = x - ((y + 2.0F) * s1);
                    y = ((y + 2.0F) * c1) - 2.0F;
                }
                *to = (x * FLUTTER_EASE) + (*to * FLUTTER_KEEP);
                to++;
                *to = (y * FLUTTER_EASE) + (*to * FLUTTER_KEEP);
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = x;
                to++;
                *to = y;
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    case JIGGLE_SWING_DOWN: {
        Float swing;
        Float size;
        Float x;
        Float y;
        Float z;
        Float s1;
        Float s2;
        Float s3;
        Float s4;
        Float c1;
        Float c2;
        Float c3;
        Float c4;
        int count;
        Angle phase;
        Angle bend;

        phase = jiggle->phase;
        jiggle->phase += SWING_DOWN_STEP;
        size = jiggle->param;
        jiggle->param *= SWING_DOWN_DAMP;
        if (_builtin_fabsf(jiggle->param) < SWING_STOP) {
            jiggle->param = 0.0F;
        }
        if (size > SWING_DOWN_LIMIT) {
            size = SWING_DOWN_LIMIT;
        } else if (size < -SWING_DOWN_LIMIT) {
            size = -SWING_DOWN_LIMIT;
        }
        swing = njSin(phase) * size;
        bend = NJM_RAD_ANG(atanf(swing * SWING_DOWN_SLOPE));
        s1 = njSin(bend >> 2);
        s2 = njSin(bend >> 1);
        s3 = njSin(bend);
        s4 = njSin(LAST_BEND(bend));
        c1 = _builtin_fabsf(njCos(bend >> 2));
        c2 = _builtin_fabsf(njCos(bend >> 1));
        c3 = _builtin_fabsf(njCos(bend));
        c4 = _builtin_fabsf(njCos(LAST_BEND(bend)));
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (y < 0.8F && y > -5.0F && x < 2.0F && x > -2.0F &&
                (z > 1.58F || z < -1.58F)) {
                if (y < -2.9F) {
                    to[0] = ((s3 * (2.9F - 1.9F)) - ((y - -2.9F) * s4) +
                             (s2 * (1.9F - 1.4F)) + (s1 * (1.4F - 0.0F))) +
                            x;
                    to[1] = ((y - -2.9F) * c4) - (c3 * (2.9F - 1.9F)) -
                            (c2 * (1.9F - 1.4F)) - (c1 * (1.4F - 0.0F));
                } else if (y < -1.9F) {
                    to[0] = ((s2 * (1.9F - 1.4F)) - ((y - -1.9F) * s3) +
                             (s1 * (1.4F - 0.0F))) +
                            x;
                    to[1] = ((y - -1.9F) * c3) - (c2 * (1.9F - 1.4F)) -
                            (c1 * (1.4F - 0.0F));
                } else if (y < -1.4F) {
                    /* @bug logic: x measures the segment from -1.9 (FADD at
                     * 0x8c02ff2a), where y measures it from its start at
                     * -1.4 (FADD at 0x8c02ff44), as every other segment
                     * measures both */
                    to[0] = ((s1 * (1.4F - 0.0F)) - ((y - -1.9F) * s2)) + x;
                    to[1] = ((y - -1.4F) * c2) - (c1 * (1.4F - 0.0F));
                } else {
                    to[0] = x - (y * s1);
                    to[1] = y * c1;
                }
                to += 2;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else if (y < -1.9F && y > -5.0F) {
                if (y < -2.9F) {
                    to[0] = ((s3 * (2.9F - 1.9F)) - ((y - -2.9F) * s4) +
                             (s2 * (1.9F - 1.4F)) + (s1 * (1.4F - 0.0F))) +
                            x;
                    to[1] = ((y - -2.9F) * c4) - (c3 * (2.9F - 1.9F)) -
                            (c2 * (1.9F - 1.4F)) - (c1 * (1.4F - 0.0F));
                } else if (y < -1.9F) {
                    to[0] = ((s2 * (1.9F - 1.4F)) - ((y - -1.9F) * s3) +
                             (s1 * (1.4F - 0.0F))) +
                            x;
                    to[1] = ((y - -1.9F) * c3) - (c2 * (1.9F - 1.4F)) -
                            (c1 * (1.4F - 0.0F));
                }
                to += 2;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = x;
                to++;
                *to = y;
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    case JIGGLE_SWING_X: {
        /* @hack spelling: amount, the swing and then the share of its place
         * each point keeps, where a local for each would do */
        Float amount;
        Float size;
        Float x;
        Float y;
        Float z;
        Float s1;
        Float s2;
        Float s3;
        Float s4;
        Float c1;
        Float c2;
        Float c3;
        Float c4;
        int count;
        Angle phase;
        Angle bend;

        phase = jiggle->phase;
        jiggle->phase += SWING_X_STEP;
        size = jiggle->param;
        jiggle->param *= SWING_X_DAMP;
        if (_builtin_fabsf(jiggle->param) < SWING_STOP) {
            jiggle->param = 0.0F;
        }
        if (size > SWING_X_LIMIT) {
            size = SWING_X_LIMIT;
        } else if (size < -SWING_X_LIMIT) {
            size = -SWING_X_LIMIT;
        }
        amount = njSin(phase) * size;
        bend = NJM_RAD_ANG(atanf(amount * SWING_X_SLOPE));
        s1 = njSin(bend >> 2);
        s2 = njSin(bend >> 1);
        s3 = njSin(bend);
        s4 = njSin(LAST_BEND(bend));
        c1 = _builtin_fabsf(njCos(bend >> 2));
        c2 = _builtin_fabsf(njCos(bend >> 1));
        c3 = _builtin_fabsf(njCos(bend));
        c4 = _builtin_fabsf(njCos(LAST_BEND(bend)));
        if (jiggle->by_kind.steps <= 0) {
            jiggle->by_kind.steps = 0;
            amount = SWING_KEEP_AFTER;
        } else {
            jiggle->by_kind.steps--;
            amount = SWING_KEEP;
        }
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (x < -1.0F && (z < -1.0F || z > 1.0F)) {
                if (x < -2.8F) {
                    y = ((s3 * (2.8F - 2.2F)) - ((x - -2.8F) * s4) +
                         (s2 * (2.2F - 1.6F)) + (s1 * 1.6F)) +
                        y;
                    x = ((x - -2.8F) * c4) - (c3 * (2.8F - 2.2F)) -
                        (c2 * (2.2F - 1.6F)) - (c1 * 1.6F);
                } else if (x < -2.2F) {
                    y = ((s2 * (2.2F - 1.6F)) - ((x - -2.2F) * s3) +
                         (s1 * 1.6F)) +
                        y;
                    x = ((x - -2.2F) * c3) - (c2 * (2.2F - 1.6F)) - (c1 * 1.6F);
                } else if (x < -1.6F) {
                    y = ((s1 * 1.6F) - ((x - -1.6F) * s2)) + y;
                    x = ((x - -1.6F) * c2) - (c1 * 1.6F);
                } else {
                    y = y - (x * s1);
                    x = x * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else if (x < -0.1F) {
                if (x < -3.5F) {
                    y = ((s3 * (3.5F - 2.5F)) - ((x - -3.5F) * s4) +
                         (s2 * (2.5F - 1.5F)) + (s1 * 1.5F)) +
                        y;
                    x = ((x - -3.5F) * c4) - (c3 * (3.5F - 2.5F)) -
                        (c2 * (2.5F - 1.5F)) - (c1 * 1.5F);
                } else if (x < -2.5F) {
                    y = ((s2 * (2.5F - 1.5F)) - ((x - -2.5F) * s3) +
                         (s1 * 1.5F)) +
                        y;
                    x = ((x - -2.5F) * c3) - (c2 * (2.5F - 1.5F)) - (c1 * 1.5F);
                } else if (x < -1.5F) {
                    y = ((s1 * 1.5F) - ((x - -1.5F) * s2)) + y;
                    x = ((x - -1.5F) * c2) - (c1 * 1.5F);
                } else {
                    y = y - (x * s1);
                    x = x * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    case JIGGLE_SWING_XZ: {
        /* @hack spelling: amount, the swing and then the share of its place
         * each point keeps, where a local for each would do */
        Float amount;
        Float size;
        Float x;
        Float y;
        Float z;
        Float s1;
        Float s2;
        Float s3;
        Float s4;
        Float c1;
        Float c2;
        Float c3;
        Float c4;
        Float p1;
        Float p2;
        Float p3;
        Float q1;
        Float q2;
        Float q3;
        int count;
        Angle phase;
        Angle bend;

        phase = jiggle->phase;
        jiggle->phase += SWING_XZ_STEP;
        size = jiggle->param;
        jiggle->param *= SWING_XZ_DAMP;
        if (_builtin_fabsf(jiggle->param) < SWING_STOP) {
            jiggle->param = 0.0F;
        }
        if (size > SWING_XZ_LIMIT) {
            size = SWING_XZ_LIMIT;
        } else if (size < -SWING_XZ_LIMIT) {
            size = -SWING_XZ_LIMIT;
        }
        amount = njSin(phase) * size;
        bend = NJM_RAD_ANG(atanf(amount * SWING_XZ_SLOPE));
        s1 = njSin(bend >> 2);
        s2 = njSin(bend >> 1);
        s3 = njSin(bend);
        s4 = njSin(LAST_BEND(bend));
        c1 = _builtin_fabsf(njCos(bend >> 2));
        c2 = _builtin_fabsf(njCos(bend >> 1));
        c3 = _builtin_fabsf(njCos(bend));
        c4 = _builtin_fabsf(njCos(LAST_BEND(bend)));
        p1 = s1 * 1.6F;
        p2 = s2 * (2.3F - 1.6F);
        p3 = s3 * (3.0F - 2.3F);
        q1 = c1 * 1.6F;
        q2 = c2 * (2.3F - 1.6F);
        q3 = c3 * (3.0F - 2.3F);
        if (jiggle->by_kind.steps <= 0) {
            jiggle->by_kind.steps = 0;
            amount = SWING_KEEP_AFTER;
        } else {
            jiggle->by_kind.steps--;
            amount = SWING_KEEP;
        }
        for (count = jiggle->vertex_count; count > 0; count--) {
            x = *from++;
            y = *from++;
            z = *from++;
            from += NORMAL_FLOATS;
            if (z < -1.5F || z > 1.5F) {
                if (z > 3.0F) {
                    x = x - ((z - 3.0F) * s4) - p1 - p2 - p3;
                    z = q1 + ((z - 3.0F) * c4) + q2 + q3;
                } else if (z < -3.0F) {
                    x = x + ((z - -3.0F) * s4) - p1 - p2 - p3;
                    z = ((z - -3.0F) * c4) - q1 - q2 - q3;
                } else if (z > 2.3F) {
                    x = x - ((z - 2.3F) * s3) - p1 - p2;
                    z = q1 + ((z - 2.3F) * c3) + q2;
                } else if (z < -2.3F) {
                    x = x + ((z - -2.3F) * s3) - p1 - p2;
                    z = ((z - -2.3F) * c3) - q1 - q2;
                } else if (z > 1.6F) {
                    x = x - ((z - 1.6F) * s2) - p1;
                    z = q1 + ((z - 1.6F) * c2);
                } else if (z < -1.6F) {
                    x = x + ((z - -1.6F) * s2) - p1;
                    z = ((z - -1.6F) * c2) - q1;
                } else if (z > 0.0F) {
                    x = x - (z * s1);
                    z = z * c1;
                } else {
                    x = x + (z * s1);
                    z = z * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = y;
                to++;
                *to = (*to * amount) + (z * (1.0F - amount));
                to += 1 + NORMAL_FLOATS;
            } else if (x < -1.0F) {
                if (x < -6.0F) {
                    y = ((s3 * (6.0F - 2.5F)) - ((x - -6.0F) * s4) +
                         (s2 * (2.5F - 1.8F)) + (s1 * 1.8F)) +
                        y;
                    x = ((x - -6.0F) * c4) - (c3 * (6.0F - 2.5F)) -
                        (c2 * (2.5F - 1.8F)) - (c1 * 1.8F);
                } else if (x < -2.5F) {
                    y = ((s2 * (2.5F - 1.8F)) - ((x - -2.5F) * s3) +
                         (s1 * 1.8F)) +
                        y;
                    x = ((x - -2.5F) * c3) - (c2 * (2.5F - 1.8F)) - (c1 * 1.8F);
                } else if (x < -1.8F) {
                    y = ((s1 * 1.8F) - ((x - -1.8F) * s2)) + y;
                    x = ((x - -1.8F) * c2) - (c1 * 1.8F);
                } else {
                    y = y - (x * s1);
                    x = x * c1;
                }
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            } else {
                *to = (*to * amount) + (x * (1.0F - amount));
                to++;
                *to = (*to * amount) + (y * (1.0F - amount));
                to++;
                *to = z;
                to += 1 + NORMAL_FLOATS;
            }
        }
        break;
    }
    /* @hack spelling: JIGGLE_CHAIN and JIGGLE_CHAIN_CAPPED last, where the
     * kinds' order would do */
    case JIGGLE_CHAIN:
        jiggle_chain(from, to, jiggle, 0);
        break;
    case JIGGLE_CHAIN_CAPPED:
        jiggle_chain(from, to, jiggle, CHAIN_CAPPED);
        break;
    }
}

/* bend from's points into to, as util.h's bend kinds say, and step phase by
 * phase_step. each arc's length is its share of the height times (curl *
 * HALF_CIRCLE_RATIO + 1 - curl). at a curl of 1, each arc is a half circle
 * across its share. the lower arc takes the points from height 0 up to its
 * length. the upper arc takes those above. z stays */
static void jiggle_bend(Float *from, Float *to, struct jiggle *jiggle,
                        Float curl)
{
    Uint32 count;
    int phase;
    int angle;
    Float base;
    Float height;
    Float lower;
    Float upper;
    Float lower_arc;
    Float upper_arc;
    Float split;
    Float rest;
    Float above;
    Float across;
    Float along;
    Float sine;
    Float cosine;
    Float half_sine;
    Float half_cosine;
    Float x;
    Float y;
    Float z;

    height = jiggle->copy->model->r * 2.0F;
    phase = (Uint16)(jiggle->phase + jiggle->by_kind.angle);
    jiggle->phase += jiggle->phase_step;
    /* @hack redundant: base, a height of 0 that the bend adds and
     * subtracts */
    base = 0.0F;
    if (phase < HALF_TURN) {
        lower = (Float)phase * height / HALF_TURN;
    } else {
        lower = (Float)(phase - HALF_TURN) * height / HALF_TURN;
    }
    upper = height - lower;
    lower_arc = (curl * lower * HALF_CIRCLE_RATIO) + ((1.0F - curl) * lower);
    upper_arc = (curl * upper * HALF_CIRCLE_RATIO) + ((1.0F - curl) * upper);
    /* @hack spelling: count set before the loop, where the for's first
     * clause would do */
    count = jiggle->vertex_count;
    split = lower_arc + base;
    rest = lower - lower_arc;
    for (; count > 0; count--) {
        x = *from++;
        y = *from++;
        z = *from++;
        from += NORMAL_FLOATS;
        if (y < 0.0F) {
            across = 0.0F;
            along = y;
        } else {
            above = y - base;
            if (y < split) {
                if (lower_arc != 0.0F) {
                    angle = (int)(above / lower_arc * HALF_TURN);
                    sine = njSin(angle);
                    cosine = njCos(angle);
                    across = curl * lower * sine * 0.5F;
                    along = base + ((1.0F - cosine) * lower * 0.5F);
                    angle = (angle + QUARTER_TURN) >> 1;
                    half_sine = njSin(angle);
                    half_cosine = njCos(angle);
                    if (phase < HALF_TURN) {
                        along -= half_sine * half_cosine * x * 2.0F;
                    } else {
                        across = -across;
                        along += half_sine * half_cosine * x * 2.0F;
                    }
                    across -= half_cosine * half_cosine * x * 2.0F;
                } else {
                    along = y;
                    across = 0.0F;
                }
            } else {
                if (upper_arc != 0.0F) {
                    angle = (int)((above - lower_arc) / upper_arc * HALF_TURN);
                    angle = FULL_TURN - angle;
                    sine = njSin(angle);
                    cosine = njCos(angle);
                    across = curl * upper * sine * 0.5F;
                    along = base + ((1.0F - cosine) * upper * 0.5F) + lower;
                    angle = (QUARTER_TURN - angle) >> 1;
                    half_sine = njSin(angle);
                    half_cosine = njCos(angle);
                    if (phase < HALF_TURN) {
                        along += half_sine * half_cosine * x * 2.0F;
                    } else {
                        along -= half_sine * half_cosine * x * 2.0F;
                        across = -across;
                    }
                    across -= half_cosine * half_cosine * x * 2.0F;
                } else {
                    across = 0.0F;
                    along = rest + y;
                }
            }
        }
        *to++ = x + (across * BEND_ACROSS);
        *to++ = along * BEND_STRETCH;
        *to++ = z;
        to += NORMAL_FLOATS;
    }
}

/* ease each point of to toward target's point at the same index. each keeps
 * keep of its own place and takes 1 - keep of target's. phase steps by 1 */
static void jiggle_ease(Float *to, Float *target, struct jiggle *jiggle,
                        Float keep)
{
    Uint32 count;
    Float rest = 1.0F - keep;

    jiggle->phase++;
    for (count = jiggle->vertex_count; count > 0; count--) {
        Float x;
        Float kept_x;
        Float y;
        Float kept_y;
        Float z;
        Float kept_z;

        kept_x = to[0] * keep;
        kept_y = to[1] * keep;
        kept_z = to[2] * keep;
        x = *target++ * rest;
        y = *target++ * rest;
        z = *target++ * rest;
        *to++ = x + kept_x;
        *to++ = y + kept_y;
        *to++ = z + kept_z;
        target += NORMAL_FLOATS;
        to += NORMAL_FLOATS;
    }
}

/* ease the points of to toward jiggle_bend's bend of from's points. each keeps
 * BEND_KEEP of its own place, and takes BEND_EASE of its bent one. phase steps
 * by 1 */
static void jiggle_bend_eased(Float *from, Float *to, struct jiggle *jiggle,
                              Float curl)
{
    Uint32 count;
    int phase;
    int angle;
    Float base;
    Float height;
    Float lower;
    Float upper;
    Float lower_arc;
    Float upper_arc;
    Float split;
    Float rest;
    Float above;
    Float across;
    Float along;
    Float sine;
    Float cosine;
    Float half_sine;
    Float half_cosine;
    Float x;
    Float y;
    Float z;

    height = jiggle->copy->model->r * 2.0F;
    phase = (Uint16)(jiggle->phase + jiggle->by_kind.angle);
    jiggle->phase++;
    /* @hack redundant: base, a height of 0 that the bend adds and
     * subtracts */
    base = 0.0F;
    if (phase < HALF_TURN) {
        lower = (Float)phase * height / HALF_TURN;
    } else {
        lower = (Float)(phase - HALF_TURN) * height / HALF_TURN;
    }
    upper = height - lower;
    lower_arc = (curl * lower * HALF_CIRCLE_RATIO) + ((1.0F - curl) * lower);
    upper_arc = (curl * upper * HALF_CIRCLE_RATIO) + ((1.0F - curl) * upper);
    /* @hack spelling: count set before the loop, where the for's first
     * clause would do */
    count = jiggle->vertex_count;
    split = lower_arc + base;
    rest = lower - lower_arc;
    for (; count > 0; count--) {
        x = *from++;
        y = *from++;
        z = *from++;
        from += NORMAL_FLOATS;
        if (y < 0.0F) {
            across = 0.0F;
            along = y;
        } else {
            above = y - base;
            if (y < split) {
                if (lower_arc != 0.0F) {
                    angle = (int)(above / lower_arc * HALF_TURN);
                    sine = njSin(angle);
                    cosine = njCos(angle);
                    across = curl * lower * sine * 0.5F;
                    along = base + ((1.0F - cosine) * lower * 0.5F);
                    angle = (angle + QUARTER_TURN) >> 1;
                    half_sine = njSin(angle);
                    half_cosine = njCos(angle);
                    if (phase < HALF_TURN) {
                        along -= half_sine * half_cosine * x * 2.0F;
                    } else {
                        across = -across;
                        along += half_sine * half_cosine * x * 2.0F;
                    }
                    across -= half_cosine * half_cosine * x * 2.0F;
                } else {
                    along = y;
                    across = 0.0F;
                }
            } else {
                if (upper_arc != 0.0F) {
                    angle = (int)((above - lower_arc) / upper_arc * HALF_TURN);
                    angle = FULL_TURN - angle;
                    sine = njSin(angle);
                    cosine = njCos(angle);
                    across = curl * upper * sine * 0.5F;
                    along = base + ((1.0F - cosine) * upper * 0.5F) + lower;
                    angle = (QUARTER_TURN - angle) >> 1;
                    half_sine = njSin(angle);
                    half_cosine = njCos(angle);
                    if (phase < HALF_TURN) {
                        along += half_sine * half_cosine * x * 2.0F;
                    } else {
                        along -= half_sine * half_cosine * x * 2.0F;
                        across = -across;
                    }
                    across -= half_cosine * half_cosine * x * 2.0F;
                } else {
                    across = 0.0F;
                    along = rest + y;
                }
            }
        }
        to[0] =
            (to[0] * BEND_KEEP) + ((x + (across * BEND_ACROSS)) * BEND_EASE);
        to[1] = (along * (BEND_EASE * BEND_STRETCH)) + (to[1] * BEND_KEEP);
        to[2] = (z * BEND_EASE) + (to[2] * BEND_KEEP);
        to += VERTEX_FLOATS;
    }
}

/* move the chain, then to's points with it, as util.h's chain kinds say. the
 * chain's tuning above sets the amounts. mode holds CHAIN_EASED and
 * CHAIN_CAPPED */
static void jiggle_chain(Float *from, Float *to, struct jiggle *jiggle,
                         int mode)
{
    int i;
    Bool settled;
    int lo;
    int hi;
    int count;
    Float fastest;
    Float lowest;
    Float step;
    Float part;
    Float x;
    Float y;
    Float z;
    Float place_x;
    Float place_y;
    NJS_POINT3 next;
    NJS_POINT3 *points;

    /* @hack spelling: the chain's points through points, where
     * jiggle->points would do */
    points = jiggle->points;
    settled = FALSE;
    if (njScalor(&jiggle->force) <= chain_settle_force) {
        fastest = 0.0F;
        for (i = CHAIN_SETTLE_FROM; i < JIGGLE_CHAIN_POINTS; i++) {
            if (_builtin_fabsf(jiggle->speeds[i].x) > fastest) {
                fastest = _builtin_fabsf(jiggle->speeds[i].x);
            }
        }
        if (chain_settle_speed > fastest) {
            settled = TRUE;
        }
    }
    for (i = 1; i < JIGGLE_CHAIN_POINTS; i++) {
        lowest = powf(CHAIN_SAG, (Float)i);
        if (njScalor(&jiggle->force) <= chain_push) {
            if (i == 1) {
                jiggle->speeds[1].x = float_approach(
                    jiggle->speeds[1].x, 0.0F, chain_step_falloff * chain_step);
                jiggle->speeds[1].y = float_approach(
                    jiggle->speeds[1].y, 0.0F, chain_step_falloff * chain_step);
                jiggle->speeds[1].z = float_approach(
                    jiggle->speeds[1].z, 0.0F, chain_step_falloff * chain_step);
            } else {
                step = powf(chain_step_falloff, (Float)i) * chain_step;
                if (_builtin_fabsf(points[i].x) > chain_reach) {
                    jiggle->speeds[i].x =
                        float_approach(jiggle->speeds[i].x,
                                       points[i - 1].x - points[i].x, step) -
                        ((Float)i * chain_lean_far) +
                        (chain_drag * jiggle->speeds[i - 1].x);
                } else {
                    jiggle->speeds[i].x =
                        float_approach(jiggle->speeds[i].x,
                                       points[i - 1].x - points[i].x, step) -
                        ((Float)i * chain_lean_near) +
                        (chain_drag * jiggle->speeds[i - 1].x);
                }
                jiggle->speeds[i].y =
                    float_approach(jiggle->speeds[i].y, 0.0F, step);
                jiggle->speeds[i].z =
                    float_approach(jiggle->speeds[i].z, 0.0F, step);
            }
        } else {
            jiggle->speeds[i].x = jiggle->force.y;
            jiggle->speeds[i].y = jiggle->force.x;
            jiggle->speeds[i].z = -jiggle->force.z;
        }
        next = points[i];
        if (njScalor(&jiggle->force) <= chain_push) {
            if (i == 1) {
                next.x = (CHAIN_KEEP * next.x) +
                         (powf((Float)i, CHAIN_LEAN_POWER) * CHAIN_FIRST_LEAN);
                next.y *= CHAIN_RISE;
                next.z *= CHAIN_KEEP;
            } else {
                if (settled) {
                    next.x =
                        (CHAIN_SETTLED_KEEP * next.x) +
                        (powf((Float)i, CHAIN_LEAN_POWER) * CHAIN_SETTLED_LEAN);
                } else {
                    next.x += jiggle->speeds[i].x;
                }
                next.y = (CHAIN_RISE * next.y) + jiggle->speeds[i].y;
                next.z = (CHAIN_KEEP * next.z) + jiggle->speeds[i].z;
            }
        } else {
            next.x += jiggle->speeds[i].x;
            next.y += jiggle->speeds[i].y;
            next.z += jiggle->speeds[i].z;
        }
        lowest = lowest * lowest;
        njSubVector(&next, &points[i - 1]);
        njUnitVector(&next);
        next.x *= chain_heights[i].y - chain_heights[i - 1].y;
        next.y *= chain_heights[i].y - chain_heights[i - 1].y;
        next.z *= chain_heights[i].y - chain_heights[i - 1].y;
        if (next.y < lowest) {
            next.y = lowest;
        }
        njUnitVector(&next);
        njAddVector(&next, &points[i - 1]);
        if (mode & CHAIN_CAPPED) {
            if (next.x > jiggle->param) {
                next.x = jiggle->param;
            }
        }
        points[i] = next;
    }
    for (count = jiggle->vertex_count; count > 0; count--) {
        x = *from++;
        y = *from++;
        z = *from++;
        from += NORMAL_FLOATS;
        if (y < 0.0F) {
            y = 0.0F;
        }
        lo = (int)floorf(y);
        hi = (int)ceilf(y);
        part = njFraction(y);
        place_x = x + (points[hi].x * part) + (points[lo].x * (1.0F - part));
        place_y = (part * points[hi].y) + (points[lo].y * (1.0F - part));
        if (mode & CHAIN_EASED) {
            to[0] = (place_x * CHAIN_EASE) + (CHAIN_EASE_KEEP * to[0]);
            to[1] = (place_y * CHAIN_EASE) + (CHAIN_EASE_KEEP * to[1]);
            to[2] = (z * CHAIN_EASE) + (CHAIN_EASE_KEEP * to[2]);
        } else {
            to[0] = place_x;
            to[1] = place_y;
            to[2] = z;
        }
        to += VERTEX_FLOATS;
    }
}
