/* the goal ring: GOALRING in a stage's object list, a ring whose touch clears
 * the stage. under MISSION_CHAO a Chao is the goal. under MISSION_RINGS, and
 * for a GOAL_RING_GOAL under MISSION_CHAO, the touch sends the player back to
 * the start instead */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/goal_ring.h"
#include "shared/item_box.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/ring_sparkle.h"
#include "shared/splash.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/goal_ring/text.inc"

#define KIND(work) ((work)->ang.x % GOAL_RING_KINDS)
#define SIGN(work) ((work)->tick)
#define TOUCHED(work) ((work)->submode)
#define FRAME(work) ((work)->scl.y) /* the Chao's frame in its motion */
/* the steps a touched GOAL_RING_GOAL has risen */
#define RISEN(work) ((work)->timer)

/* the work's mode. an untouched MODE_PLACED ring leaves range as
 * placement_try_leave decides, and a MODE_KEPT ring never leaves */
enum goal_ring_mode { MODE_PLACED, MODE_KEPT };

/* the sign a ring shows, in SIGN(work) */
enum goal_ring_sign { SIGN_NONE, SIGN_GOAL, SIGN_BACK };

/* goal_ring_init holds a MODE_KEPT ring back until player 0 has a work whose
 * timer is KEPT_RING_WAIT or more */
#define KEPT_RING_WAIT 120

/* the submode that tells item_box_step its box is a child task */
#define BOX_CHILD 1

/* the rim's radius, which the ring's shape takes too */
#define RING_RADIUS 25.0F
/* with chance SPARKLE_CHANCE on each step, a GOAL_RING_GOAL picks a random
 * point of its rim's upper half. a sparkle starts at the point only where its
 * unit direction from pos has a y below SPARKLE_TOP */
#define SPARKLE_CHANCE 0.9F
#define SPARKLE_TOP 0.9F
#define SPARKLE_SCALE 1.5F

/* on each step while game_state.time_stopped is 0, a ring or a Chao plays its
 * sound at its pos. SOUND_RING's volume falls from 0 by SOUND_FADE for each
 * step the ring has risen. the Chao's sounds play at CHAO_VOLUME, less a fade
 * that is always 0, since a Chao never rises */
#define SOUND_RING 0x1012
#define SOUND_CHAO 0x100d
#define SOUND_CHAO_TOUCHED 0x100e
#define SOUND_PRIORITY 1
#define CHAO_VOLUME 30
#define SOUND_DURATION 30
#define SOUND_FADE 2

/* a touched GOAL_RING_GOAL rises and narrows for RISE_STEPS steps, and then
 * ends */
#define RISE_STEPS 60

/* an untouched Chao with pairs of tears left sheds a pair a step. it sheds
 * only on the first TEAR_STEPS steps of every TEAR_PERIOD. its pairs left go
 * back to TEAR_PAIRS on each step its eye moves down in the world. the eye is
 * tear_points[0] */
#define TEAR_PAIRS 6
#define TEAR_PERIOD 6
#define TEAR_STEPS 3
/* each tear's speed in y takes a share of how far the eye rose since the last
 * step. the share is TEAR_CARRY_0 for the tear from tear_points[0], and
 * TEAR_CARRY_1 for the other */
#define TEAR_CARRY_0 0.8F
#define TEAR_CARRY_1 0.5F

enum goal_ring_shape { RING_SHAPE, CHAO_SHAPE, GOAL_RING_SHAPES };
#define CHAO_RADIUS 8.0F

/* the Chao's mark: a quad above its head, in texture MARK_TEXTURE of the
 * Chao's texture list */
#define MARK_TEXTURE 3
#define MARK_HALF_SIZE 1.5F
#define MARK_COLOR 0xffffffff

/* Ninja's clip test reads a model's r without the current matrix's scale. the
 * shadow, stretched by shadow_scale, draws with its model's r
 * SHADOW_RADIUS_SCALE times its own */
#define SHADOW_RADIUS_SCALE 35.0F

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's -fpack-struct=4 caps each member's alignment at 4. pack(8) lifts the
 * cap for the matrix, which GCC aligns to 8. the matrix comes first, so no
 * member moves. the record grows from 76 bytes to 80, and only syCalloc's
 * request reads its size */
#pragma pack(push, 8)
#endif
#endif
/* the record goal_ring_init gives a ring in task->motion. only a Chao fills
 * it */
struct goal_ring_chao {
    NJS_MATRIX head; /* the head's matrix in the world at the last draw */
    Bool head_set;
    Float eye_y; /* the eye's y in the world at the last step */
    Sint32 pairs_left; /* the pairs of tears left to shed */
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct goal_ring_chao */
#pragma pack(pop)
#endif
#endif

#define CHAO(task) ((struct goal_ring_chao *)(task)->motion.p)

static void goal_ring_destroy(struct task *task);
static void goal_ring_step(struct task *task);
static void goal_ring_draw(struct task *task);

/* the models below are the ring's, the two signs' and the Chao's two trees.
 * the Chao draws chao_object before a player's touch, and chao_touched_object
 * after. a tree's root is the Chao's body, and the root's child is its head.
 * the 5 of chao_node_5 is the index of that node's keys in its tree's motion */
#define GOAL_RING_TEXTURES 2
static NJS_TEXNAME goal_ring_texname[GOAL_RING_TEXTURES] = {
#include "shared/goal_ring/goal_ring_texname.inc"
};
static NJS_TEXLIST goal_ring_texlist = {
#include "shared/goal_ring/goal_ring_texlist.inc"
};
static Sint16 goal_ring_polygons[355] = {
#include "shared/goal_ring/goal_ring_polygons.inc"
};
static Sint32 goal_ring_vertices[681] = {
#include "shared/goal_ring/goal_ring_vertices.inc"
};
static NJS_CNK_MODEL goal_ring_model = {
#include "shared/goal_ring/goal_ring_model.inc"
};
static NJS_CNK_OBJECT goal_ring_object = {
#include "shared/goal_ring/goal_ring_object.inc"
};
#define SIGN_TEXTURES 1
static NJS_TEXNAME goal_sign_texname[SIGN_TEXTURES] = {
#include "shared/goal_ring/goal_sign_texname.inc"
};
static NJS_TEXLIST goal_sign_texlist = {
#include "shared/goal_ring/goal_sign_texlist.inc"
};
static Sint16 goal_sign_polygons[39] = {
#include "shared/goal_ring/goal_sign_polygons.inc"
};
static Sint32 goal_sign_vertices[51] = {
#include "shared/goal_ring/goal_sign_vertices.inc"
};
static NJS_CNK_MODEL goal_sign_model = {
#include "shared/goal_ring/goal_sign_model.inc"
};
static NJS_CNK_OBJECT goal_sign_object = {
#include "shared/goal_ring/goal_sign_object.inc"
};
static NJS_TEXNAME back_sign_texname[SIGN_TEXTURES] = {
#include "shared/goal_ring/back_sign_texname.inc"
};
static NJS_TEXLIST back_sign_texlist = {
#include "shared/goal_ring/back_sign_texlist.inc"
};
static Sint16 back_sign_polygons[39] = {
#include "shared/goal_ring/back_sign_polygons.inc"
};
static Sint32 back_sign_vertices[51] = {
#include "shared/goal_ring/back_sign_vertices.inc"
};
static NJS_CNK_MODEL back_sign_model = {
#include "shared/goal_ring/back_sign_model.inc"
};
static NJS_CNK_OBJECT back_sign_object = {
#include "shared/goal_ring/back_sign_object.inc"
};
#define CHAO_TEXTURES 4
static NJS_TEXNAME chao_texname[CHAO_TEXTURES] = {
#include "shared/goal_ring/chao_texname.inc"
};
static NJS_TEXLIST chao_texlist = {
#include "shared/goal_ring/chao_texlist.inc"
};
static Sint16 chao_node_8_polygons[131] = {
#include "shared/goal_ring/chao_node_8_polygons.inc"
};
static Sint32 chao_node_8_vertices[99] = {
#include "shared/goal_ring/chao_node_8_vertices.inc"
};
static NJS_CNK_MODEL chao_node_8_model = {
#include "shared/goal_ring/chao_node_8_model.inc"
};
static NJS_CNK_OBJECT chao_node_8_object = {
#include "shared/goal_ring/chao_node_8_object.inc"
};
static Sint16 chao_node_7_polygons[127] = {
#include "shared/goal_ring/chao_node_7_polygons.inc"
};
static Sint32 chao_node_7_vertices[87] = {
#include "shared/goal_ring/chao_node_7_vertices.inc"
};
static NJS_CNK_MODEL chao_node_7_model = {
#include "shared/goal_ring/chao_node_7_model.inc"
};
static NJS_CNK_OBJECT chao_node_7_object = {
#include "shared/goal_ring/chao_node_7_object.inc"
};
static Sint16 chao_node_6_polygons[133] = {
#include "shared/goal_ring/chao_node_6_polygons.inc"
};
static Sint32 chao_node_6_vertices[87] = {
#include "shared/goal_ring/chao_node_6_vertices.inc"
};
static NJS_CNK_MODEL chao_node_6_model = {
#include "shared/goal_ring/chao_node_6_model.inc"
};
static NJS_CNK_OBJECT chao_node_6_object = {
#include "shared/goal_ring/chao_node_6_object.inc"
};
static Sint16 chao_node_5_polygons[95] = {
#include "shared/goal_ring/chao_node_5_polygons.inc"
};
static Sint32 chao_node_5_vertices[69] = {
#include "shared/goal_ring/chao_node_5_vertices.inc"
};
static NJS_CNK_MODEL chao_node_5_model = {
#include "shared/goal_ring/chao_node_5_model.inc"
};
static NJS_CNK_OBJECT chao_node_5_object = {
#include "shared/goal_ring/chao_node_5_object.inc"
};
static Sint16 chao_node_4_polygons[131] = {
#include "shared/goal_ring/chao_node_4_polygons.inc"
};
static Sint32 chao_node_4_vertices[99] = {
#include "shared/goal_ring/chao_node_4_vertices.inc"
};
static NJS_CNK_MODEL chao_node_4_model = {
#include "shared/goal_ring/chao_node_4_model.inc"
};
static NJS_CNK_OBJECT chao_node_4_object = {
#include "shared/goal_ring/chao_node_4_object.inc"
};
static Sint16 chao_node_3_polygons[25] = {
#include "shared/goal_ring/chao_node_3_polygons.inc"
};
static Sint32 chao_node_3_vertices[27] = {
#include "shared/goal_ring/chao_node_3_vertices.inc"
};
static NJS_CNK_MODEL chao_node_3_model = {
#include "shared/goal_ring/chao_node_3_model.inc"
};
static NJS_CNK_OBJECT chao_node_3_object = {
#include "shared/goal_ring/chao_node_3_object.inc"
};
static Sint16 chao_node_2_polygons[25] = {
#include "shared/goal_ring/chao_node_2_polygons.inc"
};
static Sint32 chao_node_2_vertices[27] = {
#include "shared/goal_ring/chao_node_2_vertices.inc"
};
static NJS_CNK_MODEL chao_node_2_model = {
#include "shared/goal_ring/chao_node_2_model.inc"
};
static NJS_CNK_OBJECT chao_node_2_object = {
#include "shared/goal_ring/chao_node_2_object.inc"
};
static Sint16 chao_head_polygons[343] = {
#include "shared/goal_ring/chao_head_polygons.inc"
};
static Sint32 chao_head_vertices[237] = {
#include "shared/goal_ring/chao_head_vertices.inc"
};
static NJS_CNK_MODEL chao_head_model = {
#include "shared/goal_ring/chao_head_model.inc"
};
static NJS_CNK_OBJECT chao_head_object = {
#include "shared/goal_ring/chao_head_object.inc"
};
static Sint16 chao_polygons[185] = {
#include "shared/goal_ring/chao_polygons.inc"
};
static Sint32 chao_vertices[141] = {
#include "shared/goal_ring/chao_vertices.inc"
};
static NJS_CNK_MODEL chao_model = {
#include "shared/goal_ring/chao_model.inc"
};
static NJS_CNK_OBJECT chao_object = {
#include "shared/goal_ring/chao_object.inc"
};
static NJS_MKEY_F chao_pos_keys[2] = {
#include "shared/goal_ring/chao_pos_keys.inc"
};
static NJS_MKEY_A chao_ang_keys[2] = {
#include "shared/goal_ring/chao_ang_keys.inc"
};
static NJS_MKEY_A chao_head_ang_keys[10] = {
#include "shared/goal_ring/chao_head_ang_keys.inc"
};
static NJS_MKEY_A chao_node_2_ang_keys[10] = {
#include "shared/goal_ring/chao_node_2_ang_keys.inc"
};
static NJS_MKEY_A chao_node_3_ang_keys[10] = {
#include "shared/goal_ring/chao_node_3_ang_keys.inc"
};
static NJS_MKEY_A chao_node_4_ang_keys[10] = {
#include "shared/goal_ring/chao_node_4_ang_keys.inc"
};
static NJS_MKEY_A chao_node_5_ang_keys[10] = {
#include "shared/goal_ring/chao_node_5_ang_keys.inc"
};
static NJS_MKEY_A chao_node_6_ang_keys[2] = {
#include "shared/goal_ring/chao_node_6_ang_keys.inc"
};
static NJS_MKEY_A chao_node_7_ang_keys[2] = {
#include "shared/goal_ring/chao_node_7_ang_keys.inc"
};
static NJS_MKEY_A chao_node_8_ang_keys[10] = {
#include "shared/goal_ring/chao_node_8_ang_keys.inc"
};
static NJS_MDATA2 chao_motion_data[9] = {
#include "shared/goal_ring/chao_motion_data.inc"
};
/* @hack type: chao_motion an array of one, where a motion would do */
static NJS_MOTION chao_motion[1] = {
#include "shared/goal_ring/chao_motion.inc"
};
static NJS_TEXNAME chao_touched_texname[CHAO_TEXTURES] = {
#include "shared/goal_ring/chao_touched_texname.inc"
};
static NJS_TEXLIST chao_touched_texlist = {
#include "shared/goal_ring/chao_touched_texlist.inc"
};
static Sint16 chao_touched_node_8_polygons[131] = {
#include "shared/goal_ring/chao_touched_node_8_polygons.inc"
};
static Sint32 chao_touched_node_8_vertices[99] = {
#include "shared/goal_ring/chao_touched_node_8_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_8_model = {
#include "shared/goal_ring/chao_touched_node_8_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_8_object = {
#include "shared/goal_ring/chao_touched_node_8_object.inc"
};
static Sint16 chao_touched_node_7_polygons[127] = {
#include "shared/goal_ring/chao_touched_node_7_polygons.inc"
};
static Sint32 chao_touched_node_7_vertices[87] = {
#include "shared/goal_ring/chao_touched_node_7_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_7_model = {
#include "shared/goal_ring/chao_touched_node_7_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_7_object = {
#include "shared/goal_ring/chao_touched_node_7_object.inc"
};
static Sint16 chao_touched_node_6_polygons[133] = {
#include "shared/goal_ring/chao_touched_node_6_polygons.inc"
};
static Sint32 chao_touched_node_6_vertices[87] = {
#include "shared/goal_ring/chao_touched_node_6_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_6_model = {
#include "shared/goal_ring/chao_touched_node_6_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_6_object = {
#include "shared/goal_ring/chao_touched_node_6_object.inc"
};
static Sint16 chao_touched_node_5_polygons[95] = {
#include "shared/goal_ring/chao_touched_node_5_polygons.inc"
};
static Sint32 chao_touched_node_5_vertices[69] = {
#include "shared/goal_ring/chao_touched_node_5_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_5_model = {
#include "shared/goal_ring/chao_touched_node_5_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_5_object = {
#include "shared/goal_ring/chao_touched_node_5_object.inc"
};
static Sint16 chao_touched_node_4_polygons[131] = {
#include "shared/goal_ring/chao_touched_node_4_polygons.inc"
};
static Sint32 chao_touched_node_4_vertices[99] = {
#include "shared/goal_ring/chao_touched_node_4_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_4_model = {
#include "shared/goal_ring/chao_touched_node_4_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_4_object = {
#include "shared/goal_ring/chao_touched_node_4_object.inc"
};
static Sint16 chao_touched_node_3_polygons[25] = {
#include "shared/goal_ring/chao_touched_node_3_polygons.inc"
};
static Sint32 chao_touched_node_3_vertices[27] = {
#include "shared/goal_ring/chao_touched_node_3_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_3_model = {
#include "shared/goal_ring/chao_touched_node_3_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_3_object = {
#include "shared/goal_ring/chao_touched_node_3_object.inc"
};
static Sint16 chao_touched_node_2_polygons[25] = {
#include "shared/goal_ring/chao_touched_node_2_polygons.inc"
};
static Sint32 chao_touched_node_2_vertices[27] = {
#include "shared/goal_ring/chao_touched_node_2_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_node_2_model = {
#include "shared/goal_ring/chao_touched_node_2_model.inc"
};
static NJS_CNK_OBJECT chao_touched_node_2_object = {
#include "shared/goal_ring/chao_touched_node_2_object.inc"
};
static Sint16 chao_touched_head_polygons[343] = {
#include "shared/goal_ring/chao_touched_head_polygons.inc"
};
static Sint32 chao_touched_head_vertices[237] = {
#include "shared/goal_ring/chao_touched_head_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_head_model = {
#include "shared/goal_ring/chao_touched_head_model.inc"
};
static NJS_CNK_OBJECT chao_touched_head_object = {
#include "shared/goal_ring/chao_touched_head_object.inc"
};
static Sint16 chao_touched_polygons[185] = {
#include "shared/goal_ring/chao_touched_polygons.inc"
};
static Sint32 chao_touched_vertices[141] = {
#include "shared/goal_ring/chao_touched_vertices.inc"
};
static NJS_CNK_MODEL chao_touched_model = {
#include "shared/goal_ring/chao_touched_model.inc"
};
static NJS_CNK_OBJECT chao_touched_object = {
#include "shared/goal_ring/chao_touched_object.inc"
};
static NJS_MKEY_F chao_touched_pos_keys[20] = {
#include "shared/goal_ring/chao_touched_pos_keys.inc"
};
static NJS_MKEY_A chao_touched_ang_keys[2] = {
#include "shared/goal_ring/chao_touched_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_head_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_head_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_2_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_2_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_3_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_3_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_4_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_4_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_5_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_5_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_6_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_6_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_7_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_7_ang_keys.inc"
};
static NJS_MKEY_A chao_touched_node_8_ang_keys[20] = {
#include "shared/goal_ring/chao_touched_node_8_ang_keys.inc"
};
static NJS_MDATA2 chao_touched_motion_data[9] = {
#include "shared/goal_ring/chao_touched_motion_data.inc"
};
/* @hack type: chao_touched_motion an array of one, where a motion would
 * do */
static NJS_MOTION chao_touched_motion[1] = {
#include "shared/goal_ring/chao_touched_motion.inc"
};

NJS_TEXLIST *goal_ring_texlists[7] = {
#include "shared/goal_ring/goal_ring_texlists.inc"
};
static Float dat_goal_ring_0[8] = {0.3F, 0.1F, 1.0F, 1.0F,
                                   0.5F, 1.0F, 1.0F, 1.0F};

/* clang-format off */
static struct collision_shape goal_ring_shapes[GOAL_RING_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, RING_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F}, CHAO_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a GOAL_RING_GOAL's turn about y on each step: ring_spin before its touch,
 * ring_spin_touched after */
static Angle ring_spin = 0x180;
static Angle ring_spin_touched = 0x350;
static char *goal_ring_kind_labels[GOAL_RING_KINDS] = {
#include "shared/goal_ring/goal_ring_kind_labels.inc"
};
static Float tear_scale = 0.1F;
/* a sparkle's speed level and away from the ring's center */
static Float sparkle_speed = 0.1F;
/* a sparkle's speed up */
static Float sparkle_rise = 0.03F;
/* a ring shows its sign on the first sign_shown steps of every sign_period */
static Uint32 sign_shown = 25;
static Uint32 sign_period = 40;
/* how far a touched GOAL_RING_GOAL rises on each step */
static Float ring_rise = 0.2F;
/* the frames the Chao's motion advances on each step: chao_frame_step before
 * a player's touch, chao_touched_frame_step after */
static Float chao_frame_step = 0.12F;
static Float chao_touched_frame_step = 0.33F;
static Float dat_goal_ring_1 = 1.35F;
static Float dat_goal_ring_2 = 0.5F;
static Float dat_goal_ring_3 = 0.986F;
/* where a pair's two tears start, in the head's space */
static NJS_POINT3 tear_points[2] = {
#include "shared/goal_ring/tear_points.inc"
};
/* the two tears' speeds, in the head's space */
static NJS_VECTOR tear_speeds[2] = {{0.2F, 0.07F, 0.02F},
                                    {-0.2F, 0.07F, 0.02F}};
/* the mark's place in the head's space */
static NJS_POINT3 mark_point = {0.0F, 5.0F, 0.0F};
/* the turn of an untouched Chao's mark, as particle_draw_quad's ang. a touched
 * Chao's mark takes 0 */
static Angle mark_turn = FULL_TURN - EIGHTH_TURN;
/* where goal_ring_mark_head keeps the head's matrix. the draw points it at
 * the record's head while it draws a Chao, and leaves it null otherwise */
static NJS_MATRIX *head_matrix = {
#include "shared/goal_ring/head_matrix.inc"
};
/* the shadow's scale along x, y and z, under the ring's matrix */
static NJS_VECTOR shadow_scale = {30.0F, 5.0F, 10.0F};

void goal_ring_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *box;
    /* @hack spelling: kinds, a local for GOAL_RING_KINDS, where KIND(work)
     * would do */
    Sint32 kinds = GOAL_RING_KINDS;

    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- GOALTYPE";
        (void)"<- ANGLE Y";
    }
    switch (player_character(0)) {
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        if (game_state.mission != MISSION_RINGS) {
            if (work->ang.x % kinds != GOAL_RING_CHAO) {
                return;
            }
        } else {
            work->ang.x = GOAL_RING_BACKONDMG;
            work->mode = MODE_KEPT;
            if (player_work[0] == NULL ||
                player_work[0]->timer < KEPT_RING_WAIT) {
                return;
            }
        }
        break;
    }
    if (game_state.mission != MISSION_CHAO && KIND(work) == GOAL_RING_CHAO) {
        if (task->child == NULL) {
            box = task_create_child(TASK_WORK, item_box_step, task);
            if (box != NULL) {
                box->work->scl.x = BOX_1UP;
                box->work->submode = BOX_CHILD;
            }
        }
        if (task->child != NULL) {
            task->child->work->pos = work->pos;
        }
        return;
    }
    if (work->mode == MODE_PLACED && placement_try_leave(task)) {
        return;
    }
#ifdef DREAMCAST
#ifdef NONMATCHING
    /* this side checks that the record keeps SHC's 80 bytes */
    _Static_assert(sizeof(struct goal_ring_chao) == 80,
                   "struct goal_ring_chao changed its size");
#endif
#endif
    task->motion.p = syCalloc(1, sizeof(struct goal_ring_chao));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw and the destructor set before the step, where
     * struct task's member order would set the step first */
    task->draw = goal_ring_draw;
    task->destructor = goal_ring_destroy;
    task->step = goal_ring_step;
    SIGN(work) = SIGN_NONE;
    if (work->ang.x % kinds == GOAL_RING_CHAO) {
        collision_init(task, &goal_ring_shapes[CHAO_SHAPE], 1,
                       COLLISION_OBJECT);
    } else {
        collision_init(task, &goal_ring_shapes[RING_SHAPE], 1,
                       COLLISION_OBJECT);
    }
    TOUCHED(work) = FALSE;
}

static void goal_ring_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

static void goal_ring_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    struct task *player_task;
    int player;
    Float length;
    Float last_eye_y;
    Float rise;

    if (!TOUCHED(work) && work->mode == MODE_PLACED &&
        placement_try_leave(task)) {
        return;
    }
    if (game_state.steps % sign_period < sign_shown) {
        switch (game_state.mission) {
        case MISSION_RINGS:
        case MISSION_CHAO:
            SIGN(work) = SIGN_BACK;
            break;
        default:
            SIGN(work) = SIGN_GOAL;
            break;
        }
    } else {
        SIGN(work) = SIGN_NONE;
    }
    /* @hack redundant: a block no path reaches, which stores the kind to the
     * sign */
    if (0) {
        SIGN(work) = KIND(work);
    }
    if (!game_state.time_stopped) {
        if (KIND(work) != GOAL_RING_GOAL) {
            if (TOUCHED(work)) {
                FRAME(work) += chao_touched_frame_step;
                if (FRAME(work) > (Float)(chao_touched_motion->nbFrame - 1)) {
                    FRAME(work) = 0.0F;
                }
                sound_play_at_for(SOUND_CHAO_TOUCHED, task, SOUND_PRIORITY,
                                  CHAO_VOLUME, SOUND_DURATION, &work->pos);
            } else {
                FRAME(work) += chao_frame_step;
                if (FRAME(work) > (Float)(chao_motion->nbFrame - 1)) {
                    FRAME(work) = 0.0F;
                }
                sound_play_at_for(SOUND_CHAO, task, SOUND_PRIORITY,
                                  CHAO_VOLUME - (RISEN(work) * SOUND_FADE),
                                  SOUND_DURATION, &work->pos);
            }
        } else {
            if (TOUCHED(work)) {
                work->ang.y += ring_spin_touched;
                RISEN(work)++;
                work->pos.y += ring_rise;
            } else {
                work->ang.y += ring_spin;
            }
            if (((Float)rand() * RAND_SCALE) < SPARKLE_CHANCE) {
                pos.x = 0.0F;
                pos.y = RING_RADIUS;
                pos.z = 0.0F;
                spd.x = 0.0F;
                spd.y = 1.0F;
                spd.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateEx(&work->pos);
                njRotateY(NULL, work->ang.y);
                njRotateZ(NULL, (Angle)((((Float)rand() * RAND_SCALE) - 0.5F) *
                                        HALF_TURN));
                njCalcVector(NULL, &spd, &spd);
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrixEx();
                if (njAbs(spd.y) < SPARKLE_TOP) {
                    spd.y = 0.0F;
                    length = njScalor(&spd);
                    spd.x *= sparkle_speed / length;
                    spd.y = sparkle_rise;
                    spd.z *= sparkle_speed / length;
                    ring_sparkle_create_long(&pos, &spd, SPARKLE_SCALE);
                }
            }
            sound_play_at_for(SOUND_RING, work, SOUND_PRIORITY,
                              -(RISEN(work) * SOUND_FADE), SOUND_DURATION,
                              &work->pos);
        }
    }
    /* @hack spelling: the flag compared with FALSE, where !TOUCHED(work)
     * would do */
    if (TOUCHED(work) == FALSE &&
        (player_task = collision_next_player(task)) != NULL &&
        (player = player_slot_of(player_task)) != -1) {
        /* @hack spelling: the flag set in each branch, where once after the
         * test would do */
        if (game_state.mission != MISSION_RINGS &&
            (game_state.mission != MISSION_CHAO ||
             KIND(work) != GOAL_RING_GOAL)) {
            stage_clear(player);
            TOUCHED(work) = TRUE;
            messenger_set_event(player, MESSENGER_EVENT_GOAL_RING);
        } else {
            stage_go_back(player);
            TOUCHED(work) = TRUE;
        }
        FRAME(work) = 0.0F;
    } else {
        collision_register(task);
    }
    if (KIND(work) != GOAL_RING_GOAL && CHAO(task)->head_set &&
        !TOUCHED(work) && !game_state.time_stopped) {
        njPushMatrixEx();
        njSetMatrix(NULL, &CHAO(task)->head);
        /* @hack spelling: the tears' points kept in spd and their speeds in
         * pos, where points in pos and speeds in spd would do */
        njCalcPoint(NULL, &tear_points[0], &spd);
        njCalcVector(NULL, &tear_speeds[0], &pos);
        last_eye_y = CHAO(task)->eye_y;
        CHAO(task)->eye_y = spd.y;
        rise = spd.y - last_eye_y;
        if (last_eye_y > spd.y) {
            CHAO(task)->pairs_left = TEAR_PAIRS;
        }
        if (CHAO(task)->pairs_left &&
            game_state.steps % TEAR_PERIOD < TEAR_STEPS) {
            CHAO(task)->pairs_left--;
            pos.y += rise * TEAR_CARRY_0;
            splash_violet_create(&spd, &pos, tear_scale);
            njCalcPoint(NULL, &tear_points[1], &spd);
            njCalcVector(NULL, &tear_speeds[1], &pos);
            pos.y += rise * TEAR_CARRY_1;
            splash_violet_create(&spd, &pos, tear_scale);
        }
        njPopMatrixEx();
    }
    /* @hack spelling: the flag compared with TRUE, where TOUCHED(work) alone
     * would do */
    if (TOUCHED(work) == TRUE && RISEN(work) >= RISE_STEPS) {
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    }
}

/* the Chao's motion callback, which njCnkEasyDrawMotion calls at each node.
 * at the head it draws the mark, and keeps the head's matrix at head_matrix
 * unless that is null */
static void goal_ring_mark_head(NJS_CNK_OBJECT *object)
{
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (object == &chao_touched_head_object) {
        particle_draw_quad(&mark_point, MARK_TEXTURE, QUARTER_TURN, 0,
                           MARK_HALF_SIZE, MARK_HALF_SIZE, MARK_COLOR, 0, 0.0F,
                           0.0F);
        if (head_matrix != NULL) {
            njGetMatrix(head_matrix);
        }
    }
    if (object == &chao_head_object) {
        particle_draw_quad(&mark_point, MARK_TEXTURE, QUARTER_TURN, mark_turn,
                           MARK_HALF_SIZE, MARK_HALF_SIZE, MARK_COLOR, 0, 0.0F,
                           0.0F);
        if (head_matrix != NULL) {
            njGetMatrix(head_matrix);
            /* @hack spelling: a jump to the return, where falling through to
             * it would do */
            goto done;
        }
    }
done:
    return;
}

static void goal_ring_draw(struct task *task)
{
#ifdef DREAMCAST
#ifdef NONMATCHING
    /* under -fpack-struct=4 the struct below aligns only to 4, and GCC warns
     * that its NJS_MATRIX members want 8. the draw hands Ninja no member: it
     * hands a pointer into storage, rounded down to 8 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wif-not-aligned"
#endif
#endif
    /* @hack type: the matrix's pointer and the space it points into kept in
     * one struct, where a local pointer and an array of 17 Floats would do */
    struct {
        NJS_MATRIX *matrix;
        union {
            NJS_MATRIX matrix;
            struct {
                Float alignment;
                NJS_MATRIX matrix;
            } shifted;
        } storage;
    } aligned;
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the diagnostic push above the aligned union */
#pragma GCC diagnostic pop
#endif
#endif
    struct task_work *work = task->work;
    Float scale;
    Float radius;
    /* @hack spelling: shadow, a local for &shadow_object, where shadow_object
     * read in place would do */
    NJS_CNK_OBJECT *shadow;

    /* aligned.matrix holds the draw's starting matrix on an 8-byte boundary.
     * shifted.matrix lies 4 bytes into storage. its address rounded down to 8
     * still leaves the whole matrix inside storage */
    aligned.matrix =
        (NJS_MATRIX *)((Uint32)&aligned.storage.shifted.matrix & ~7);
    njGetMatrix(aligned.matrix);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (objects_fog_off) {
        njFogDisable();
    }
    /* @hack spelling: a switch of one case and a default, where an if and an
     * else would do */
    switch (KIND(work)) {
    case GOAL_RING_CHAO:
        /* @hack spelling: the record's address, where its head's would
         * do */
        head_matrix = task->motion.p;
        njCnkSetMotionCallback(goal_ring_mark_head);
        if (TOUCHED(work)) {
            njSetTexture(&chao_touched_texlist);
            njCnkEasyDrawMotion(&chao_touched_object, chao_touched_motion,
                                FRAME(work));
        } else {
            njSetTexture(&chao_texlist);
            njCnkEasyDrawMotion(&chao_object, chao_motion, FRAME(work));
        }
        njCnkSetMotionCallback(NULL);
        /* goal_ring_mark_head kept the head's matrix with the draw's
         * starting matrix applied. the starting matrix's inverse takes it
         * back out, which leaves the head's matrix in the world */
        njPushMatrixEx();
        njSetMatrix(NULL, aligned.matrix);
        njInvertMatrix(NULL);
        njMultiMatrix(NULL, &CHAO(task)->head);
        njGetMatrix(&CHAO(task)->head);
        njPopMatrixEx();
        CHAO(task)->head_set = TRUE;
        head_matrix = NULL;
        break;
    default:
        njSetTexture(&goal_ring_texlist);
        if (TOUCHED(work)) {
            scale = 1.0F - (RISEN(work) * (1.0F / RISE_STEPS));
            if (scale < 0.0F) {
                scale = 0.0F;
            }
            njScale(NULL, scale, 1.0F, scale);
        }
        njPushMatrixEx();
        njScaleEx(&shadow_scale);
        shadow = &shadow_object;
        radius = shadow->model->r;
        shadow->model->r *= SHADOW_RADIUS_SCALE;
        njCnkModDrawObject(shadow);
        shadow->model->r = radius;
        njPopMatrixEx();
        njCnkSimpleDrawModel(&goal_ring_model);
        switch (SIGN(work)) {
        case SIGN_GOAL:
            njSetTexture(&goal_sign_texlist);
            njCnkSimpleDrawModel(&goal_sign_model);
            break;
        case SIGN_BACK:
            njSetTexture(&back_sign_texlist);
            njCnkSimpleDrawModel(&back_sign_model);
            break;
        /* @hack redundant: an empty default */
        default:
            break;
        }
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrixEx();
}
