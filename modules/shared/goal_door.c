/* the goal door, GOALDOOR in a stage's object list. a hit is a touch by a
 * task whose work's id is WORK_ID_ATTACK. the door breaks at the fourth hit
 * since its previous break or its task's start. it breaks three times. after
 * the last break it draws nothing. from the 120th step after that break, each
 * of its steps calls stage_clear for player 0 */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/chunk.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/black_smoke.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/goal_door.h"
#include "shared/spark_point.h"
#include "shared/stage_util.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/goal_door/text.inc"

/* the work's mode: the count of the door's breaks */
enum goal_door_mode {
    MODE_WHOLE,
    MODE_BROKEN_ONCE,
    MODE_BROKEN_TWICE,
    MODE_GONE
};
/* the mode a break sets for the door's next step to enter. that step makes it
 * the work's mode and the placement's state, then runs the new mode's step.
 * NO_MODE leaves the mode as it is */
#define NEXT_MODE(work) ((work)->submode)
#define NO_MODE (-1)

/* the door tests only a step's latest touch for a hit. HITS counts the hits
 * since the previous break, from 0 at goal_door_init. the door breaks when
 * HITS reaches break_hits */
#define HITS(work) ((work)->timer)
/* a hit sets COOLDOWN to HIT_STEPS. while COOLDOWN is above 0, each step
 * takes 1 from it instead of testing for a hit. the draw tints the door with
 * hit_tint at a hit. the tint fades out as COOLDOWN falls to TINT_END */
#define COOLDOWN(task) ((task)->unk_3c.l)
#define HIT_STEPS 15
#define TINT_END 11
#define TINT_FADE (1.0F / (HIT_STEPS - TINT_END))
/* MODE_GONE's steps so far, in COOLDOWN's field. the last break starts the
 * count at 0. goal_door_init does too, so a door whose task starts in
 * MODE_GONE bursts again. once the count reaches GONE_END, each step calls
 * stage_clear for player 0 */
#define GONE_STEPS(task) ((task)->unk_3c.l)
#define GONE_END 120

/* the door's kind indexes the kind tables. it comes from the work's scl.z,
 * but with one kind every door is of kind 0 */
#define DOOR_KINDS 1
#define KIND(work) ((int)_builtin_fabsf((work)->scl.z) % DOOR_KINDS)

/* a door draws DOOR_LIFT above its pos. the whole door's glow draws
 * GLOW_HEIGHT above that and GLOW_DEPTH out from the door, along its +z.
 * GLOW_SCALE scales the glow's x and y */
#define DOOR_LIFT 0.01F
#define GLOW_HEIGHT 25.0F
#define GLOW_DEPTH 0.01F
#define GLOW_SCALE 1.33F

/* a color byte's full value, as a Float */
#define CHANNEL_FULL 255.0F

/* the first two breaks play BREAK_SOUND. the last plays LAST_BREAK_SOUND, of
 * the stage's sound list. each of MODE_GONE's two bursts plays
 * GONE_BURST_SOUND */
#define BREAK_SOUND 0x8015
#define LAST_BREAK_SOUND 11
#define GONE_BURST_SOUND 0x100c
/* a break raises dust of BREAK_DUST_SCALE where the hitting task stands. the
 * last break also makes a burst of scale last_burst_scale, LAST_BURST_HEIGHT
 * above the door's pos */
#define BREAK_DUST_SCALE 12.0F
#define LAST_BURST_HEIGHT 25.0F
/* the last break probes the floor from PROBE_HEIGHT above the door's pos */
#define PROBE_HEIGHT 1.0F
/* the sparks bounce off a height SPARK_DROP below the door's pos */
#define SPARK_DROP 20.0F

/* a random factor from 1 - p up to 1 + p, never 1 + p */
#define RANDOM_FACTOR(p) RANDOM_FROM(1.0F - (p), 2.0F * (p))
/* TRUE at random, with the chance p from 0 to 1 */
#define CHANCE(p) ((Float)rand() * RAND_SCALE < (p))

/* a piece that a break or a hit throws */
struct goal_door_piece {
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    Bool live;
    Bool scaled; /* TRUE where the draw scales the piece by scale */
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scale;
    NJS_VECTOR spd;
    struct angles spin; /* what each step adds to ang */
    Sint32 timer; /* the steps since the throw */
    Sint32 life; /* the piece ends once timer reaches it */
    /* the fraction of its life after which the piece's alpha falls. NO_FADE
     * marks a piece that never fades. a bug in goal_door_throw_fading makes
     * the field 0 in every fading piece */
    Sint32 fade_start;
    /* @hack type: color a Sint32, where a Uint32 would do */
    Sint32 color; /* a fading piece's tint, which darkens over its life */
    Bool trails; /* TRUE where the piece trails dust. no throw sets it TRUE */
};
/* the pieces of a throw's task, PIECES of them at task->extra. LIVE_PIECES
 * counts the live ones, and the task ends once none is left */
#define PIECES 32
#define TASK_PIECES(task) ((struct goal_door_piece *)(task)->extra.p)
#define LIVE_PIECES(task) ((task)->unk_3c.l)
#define NO_FADE 1.0F
/* a fading piece draws under FADE_CONTROL, with every strip's flags replaced
 * by FADE_ATTR. FADE_ATTR makes each strip translucent */
#define FADE_CONTROL NJD_CONTROL_3D_CNK_CONSTANT_ATTR
#define FADE_ATTR NJD_FST_UA
/* a piece flies out from the door. each unit of its x across the door turns
 * it aside by about yaw_per_x * YAW_FACTOR. each unit of its height tilts it
 * by about pitch_per_y * PITCH_FACTOR. both turns are in Angle units */
#define YAW_FACTOR 0.04F
#define PITCH_FACTOR 0.02F
/* the tint of a chip, a piece of chip_glass_object or chip_door_object */
#define CHIP_TINT 0xffff4040
/* a scrap starts SCRAP_LOW up to SCRAP_HIGH above the door's pos. it starts
 * within SCRAP_HALF_WIDTH of that pos across the door. with the chance
 * SMALL_CHANCE, a scrap draws at about SMALL_SCALE. with the chance
 * MIDDLE_CHANCE, it draws at about MIDDLE_SCALE. any other draws at its
 * model's size */
#define SCRAPS 10
#define SCRAP_HALF_WIDTH 25.0F
#define SCRAP_LOW 10.0F
#define SCRAP_HIGH 50.0F
#define SMALL_CHANCE 0.25F
#define SMALL_SCALE 0.3F
#define MIDDLE_CHANCE 0.25F
#define MIDDLE_SCALE 0.6F

static void goal_door_destroy(struct task *task);
static void goal_door_step(struct task *task);
static void goal_door_draw(struct task *task);
static void goal_door_apply_mode(struct task *task);
/* NOLINTBEGIN(misc-use-internal-linkage) */
void goal_door_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */
static void goal_door_whole_step(struct task *task);
static void goal_door_start_gone(struct task *task);
static void goal_door_broken_once_step(struct task *task);
static void goal_door_broken_twice_step(struct task *task);
static void goal_door_gone_step(struct task *task);
static void goal_door_throw_fading(struct task *task, NJS_CNK_OBJECT *object,
                                   NJS_TEXLIST *texlist);
static void goal_door_throw_solid(struct task *task, NJS_CNK_OBJECT *object,
                                  NJS_TEXLIST *texlist);
static void goal_door_throw_scrap(struct task *task, NJS_CNK_MODEL *model,
                                  NJS_TEXLIST *texlist);
static void goal_door_pieces_destroy(struct task *task);
static void goal_door_pieces_step(struct task *task);
static void goal_door_pieces_draw(struct task *task);
static void goal_door_draw_children_tinted(NJS_CNK_OBJECT *object,
                                           Uint32 color);

/* the whole door and its glow. the draw places the two models itself, so no
 * code reads the nodes of whole_object's tree */
static NJS_TEXNAME whole_texnames[7] = {
#include "shared/goal_door/whole_texnames.inc"
};
static NJS_TEXLIST whole_texlist = {
#include "shared/goal_door/whole_texlist.inc"
};
static Sint16 whole_glow_polygons[25] = {
#include "shared/goal_door/whole_glow_polygons.inc"
};
static Sint32 whole_glow_vertices[19] = {
#include "shared/goal_door/whole_glow_vertices.inc"
};
static NJS_CNK_MODEL whole_glow_model = {
#include "shared/goal_door/whole_glow_model.inc"
};
static NJS_CNK_OBJECT whole_glow_object = {
#include "shared/goal_door/whole_glow_object.inc"
};
static Sint16 whole_door_polygons[1807] = {
#include "shared/goal_door/whole_door_polygons.inc"
};
static Sint32 whole_door_vertices[1715] = {
#include "shared/goal_door/whole_door_vertices.inc"
};
static NJS_CNK_MODEL whole_door_model = {
#include "shared/goal_door/whole_door_model.inc"
};
static NJS_CNK_OBJECT whole_door_object = {
#include "shared/goal_door/whole_door_object.inc"
};
static NJS_CNK_OBJECT whole_object = {
#include "shared/goal_door/whole_object.inc"
};
/* the door broken once: three models, the children of broken_once_object */
static NJS_TEXNAME broken_once_texnames[7] = {
#include "shared/goal_door/broken_once_texnames.inc"
};
static NJS_TEXLIST broken_once_texlist = {
#include "shared/goal_door/broken_once_texlist.inc"
};
static Sint16 broken_once_0_polygons[617] = {
#include "shared/goal_door/broken_once_0_polygons.inc"
};
static Sint32 broken_once_0_vertices[483] = {
#include "shared/goal_door/broken_once_0_vertices.inc"
};
static NJS_CNK_MODEL broken_once_0_model = {
#include "shared/goal_door/broken_once_0_model.inc"
};
static NJS_CNK_OBJECT broken_once_0_object = {
#include "shared/goal_door/broken_once_0_object.inc"
};
static Sint16 broken_once_1_polygons[1645] = {
#include "shared/goal_door/broken_once_1_polygons.inc"
};
static Sint32 broken_once_1_vertices[1555] = {
#include "shared/goal_door/broken_once_1_vertices.inc"
};
static NJS_CNK_MODEL broken_once_1_model = {
#include "shared/goal_door/broken_once_1_model.inc"
};
static NJS_CNK_OBJECT broken_once_1_object = {
#include "shared/goal_door/broken_once_1_object.inc"
};
static Sint16 broken_once_2_polygons[311] = {
#include "shared/goal_door/broken_once_2_polygons.inc"
};
static Sint32 broken_once_2_vertices[279] = {
#include "shared/goal_door/broken_once_2_vertices.inc"
};
static NJS_CNK_MODEL broken_once_2_model = {
#include "shared/goal_door/broken_once_2_model.inc"
};
static NJS_CNK_OBJECT broken_once_2_object = {
#include "shared/goal_door/broken_once_2_object.inc"
};
static NJS_CNK_OBJECT broken_once_object = {
#include "shared/goal_door/broken_once_object.inc"
};
/* the first break's pieces of glass: the children of break_1_glass_object,
 * each at its piece's place on the door */
static NJS_TEXNAME break_1_glass_texnames[1] = {
#include "shared/goal_door/break_1_glass_texnames.inc"
};
static NJS_TEXLIST break_1_glass_texlist = {
#include "shared/goal_door/break_1_glass_texlist.inc"
};
static Sint16 break_1_glass_0_polygons[33] = {
#include "shared/goal_door/break_1_glass_0_polygons.inc"
};
static Sint32 break_1_glass_0_vertices[27] = {
#include "shared/goal_door/break_1_glass_0_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_0_model = {
#include "shared/goal_door/break_1_glass_0_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_0_object = {
#include "shared/goal_door/break_1_glass_0_object.inc"
};
static Sint16 break_1_glass_1_polygons[39] = {
#include "shared/goal_door/break_1_glass_1_polygons.inc"
};
static Sint32 break_1_glass_1_vertices[35] = {
#include "shared/goal_door/break_1_glass_1_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_1_model = {
#include "shared/goal_door/break_1_glass_1_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_1_object = {
#include "shared/goal_door/break_1_glass_1_object.inc"
};
static Sint16 break_1_glass_2_polygons[23] = {
#include "shared/goal_door/break_1_glass_2_polygons.inc"
};
static Sint32 break_1_glass_2_vertices[15] = {
#include "shared/goal_door/break_1_glass_2_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_2_model = {
#include "shared/goal_door/break_1_glass_2_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_2_object = {
#include "shared/goal_door/break_1_glass_2_object.inc"
};
static Sint16 break_1_glass_3_polygons[25] = {
#include "shared/goal_door/break_1_glass_3_polygons.inc"
};
static Sint32 break_1_glass_3_vertices[19] = {
#include "shared/goal_door/break_1_glass_3_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_3_model = {
#include "shared/goal_door/break_1_glass_3_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_3_object = {
#include "shared/goal_door/break_1_glass_3_object.inc"
};
static Sint16 break_1_glass_4_polygons[25] = {
#include "shared/goal_door/break_1_glass_4_polygons.inc"
};
static Sint32 break_1_glass_4_vertices[19] = {
#include "shared/goal_door/break_1_glass_4_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_4_model = {
#include "shared/goal_door/break_1_glass_4_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_4_object = {
#include "shared/goal_door/break_1_glass_4_object.inc"
};
static Sint16 break_1_glass_5_polygons[23] = {
#include "shared/goal_door/break_1_glass_5_polygons.inc"
};
static Sint32 break_1_glass_5_vertices[15] = {
#include "shared/goal_door/break_1_glass_5_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_5_model = {
#include "shared/goal_door/break_1_glass_5_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_5_object = {
#include "shared/goal_door/break_1_glass_5_object.inc"
};
static Sint16 break_1_glass_6_polygons[23] = {
#include "shared/goal_door/break_1_glass_6_polygons.inc"
};
static Sint32 break_1_glass_6_vertices[15] = {
#include "shared/goal_door/break_1_glass_6_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_6_model = {
#include "shared/goal_door/break_1_glass_6_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_6_object = {
#include "shared/goal_door/break_1_glass_6_object.inc"
};
static Sint16 break_1_glass_7_polygons[23] = {
#include "shared/goal_door/break_1_glass_7_polygons.inc"
};
static Sint32 break_1_glass_7_vertices[15] = {
#include "shared/goal_door/break_1_glass_7_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_7_model = {
#include "shared/goal_door/break_1_glass_7_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_7_object = {
#include "shared/goal_door/break_1_glass_7_object.inc"
};
static Sint16 break_1_glass_8_polygons[33] = {
#include "shared/goal_door/break_1_glass_8_polygons.inc"
};
static Sint32 break_1_glass_8_vertices[19] = {
#include "shared/goal_door/break_1_glass_8_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_8_model = {
#include "shared/goal_door/break_1_glass_8_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_8_object = {
#include "shared/goal_door/break_1_glass_8_object.inc"
};
static Sint16 break_1_glass_9_polygons[35] = {
#include "shared/goal_door/break_1_glass_9_polygons.inc"
};
static Sint32 break_1_glass_9_vertices[23] = {
#include "shared/goal_door/break_1_glass_9_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_9_model = {
#include "shared/goal_door/break_1_glass_9_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_9_object = {
#include "shared/goal_door/break_1_glass_9_object.inc"
};
static Sint16 break_1_glass_10_polygons[33] = {
#include "shared/goal_door/break_1_glass_10_polygons.inc"
};
static Sint32 break_1_glass_10_vertices[27] = {
#include "shared/goal_door/break_1_glass_10_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_10_model = {
#include "shared/goal_door/break_1_glass_10_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_10_object = {
#include "shared/goal_door/break_1_glass_10_object.inc"
};
static Sint16 break_1_glass_11_polygons[33] = {
#include "shared/goal_door/break_1_glass_11_polygons.inc"
};
static Sint32 break_1_glass_11_vertices[19] = {
#include "shared/goal_door/break_1_glass_11_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_11_model = {
#include "shared/goal_door/break_1_glass_11_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_11_object = {
#include "shared/goal_door/break_1_glass_11_object.inc"
};
static Sint16 break_1_glass_12_polygons[33] = {
#include "shared/goal_door/break_1_glass_12_polygons.inc"
};
static Sint32 break_1_glass_12_vertices[19] = {
#include "shared/goal_door/break_1_glass_12_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_12_model = {
#include "shared/goal_door/break_1_glass_12_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_12_object = {
#include "shared/goal_door/break_1_glass_12_object.inc"
};
static Sint16 break_1_glass_13_polygons[25] = {
#include "shared/goal_door/break_1_glass_13_polygons.inc"
};
static Sint32 break_1_glass_13_vertices[19] = {
#include "shared/goal_door/break_1_glass_13_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_13_model = {
#include "shared/goal_door/break_1_glass_13_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_13_object = {
#include "shared/goal_door/break_1_glass_13_object.inc"
};
static Sint16 break_1_glass_14_polygons[23] = {
#include "shared/goal_door/break_1_glass_14_polygons.inc"
};
static Sint32 break_1_glass_14_vertices[15] = {
#include "shared/goal_door/break_1_glass_14_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_14_model = {
#include "shared/goal_door/break_1_glass_14_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_14_object = {
#include "shared/goal_door/break_1_glass_14_object.inc"
};
static Sint16 break_1_glass_15_polygons[23] = {
#include "shared/goal_door/break_1_glass_15_polygons.inc"
};
static Sint32 break_1_glass_15_vertices[15] = {
#include "shared/goal_door/break_1_glass_15_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_15_model = {
#include "shared/goal_door/break_1_glass_15_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_15_object = {
#include "shared/goal_door/break_1_glass_15_object.inc"
};
static Sint16 break_1_glass_16_polygons[23] = {
#include "shared/goal_door/break_1_glass_16_polygons.inc"
};
static Sint32 break_1_glass_16_vertices[15] = {
#include "shared/goal_door/break_1_glass_16_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_16_model = {
#include "shared/goal_door/break_1_glass_16_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_16_object = {
#include "shared/goal_door/break_1_glass_16_object.inc"
};
static Sint16 break_1_glass_17_polygons[23] = {
#include "shared/goal_door/break_1_glass_17_polygons.inc"
};
static Sint32 break_1_glass_17_vertices[15] = {
#include "shared/goal_door/break_1_glass_17_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_17_model = {
#include "shared/goal_door/break_1_glass_17_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_17_object = {
#include "shared/goal_door/break_1_glass_17_object.inc"
};
static Sint16 break_1_glass_18_polygons[33] = {
#include "shared/goal_door/break_1_glass_18_polygons.inc"
};
static Sint32 break_1_glass_18_vertices[27] = {
#include "shared/goal_door/break_1_glass_18_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_18_model = {
#include "shared/goal_door/break_1_glass_18_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_18_object = {
#include "shared/goal_door/break_1_glass_18_object.inc"
};
static Sint16 break_1_glass_19_polygons[25] = {
#include "shared/goal_door/break_1_glass_19_polygons.inc"
};
static Sint32 break_1_glass_19_vertices[19] = {
#include "shared/goal_door/break_1_glass_19_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_19_model = {
#include "shared/goal_door/break_1_glass_19_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_19_object = {
#include "shared/goal_door/break_1_glass_19_object.inc"
};
static Sint16 break_1_glass_20_polygons[33] = {
#include "shared/goal_door/break_1_glass_20_polygons.inc"
};
static Sint32 break_1_glass_20_vertices[19] = {
#include "shared/goal_door/break_1_glass_20_vertices.inc"
};
static NJS_CNK_MODEL break_1_glass_20_model = {
#include "shared/goal_door/break_1_glass_20_model.inc"
};
static NJS_CNK_OBJECT break_1_glass_20_object = {
#include "shared/goal_door/break_1_glass_20_object.inc"
};
static NJS_CNK_OBJECT break_1_glass_object = {
#include "shared/goal_door/break_1_glass_object.inc"
};
/* the first break's other pieces: the children of break_1_door_object, each
 * at its piece's place on the door */
static NJS_TEXNAME break_1_door_texnames[1] = {
#include "shared/goal_door/break_1_door_texnames.inc"
};
static NJS_TEXLIST break_1_door_texlist = {
#include "shared/goal_door/break_1_door_texlist.inc"
};
static Sint16 break_1_door_0_polygons[25] = {
#include "shared/goal_door/break_1_door_0_polygons.inc"
};
static Sint32 break_1_door_0_vertices[19] = {
#include "shared/goal_door/break_1_door_0_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_0_model = {
#include "shared/goal_door/break_1_door_0_model.inc"
};
static NJS_CNK_OBJECT break_1_door_0_object = {
#include "shared/goal_door/break_1_door_0_object.inc"
};
static Sint16 break_1_door_1_polygons[45] = {
#include "shared/goal_door/break_1_door_1_polygons.inc"
};
static Sint32 break_1_door_1_vertices[35] = {
#include "shared/goal_door/break_1_door_1_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_1_model = {
#include "shared/goal_door/break_1_door_1_model.inc"
};
static NJS_CNK_OBJECT break_1_door_1_object = {
#include "shared/goal_door/break_1_door_1_object.inc"
};
static Sint16 break_1_door_2_polygons[29] = {
#include "shared/goal_door/break_1_door_2_polygons.inc"
};
static Sint32 break_1_door_2_vertices[23] = {
#include "shared/goal_door/break_1_door_2_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_2_model = {
#include "shared/goal_door/break_1_door_2_model.inc"
};
static NJS_CNK_OBJECT break_1_door_2_object = {
#include "shared/goal_door/break_1_door_2_object.inc"
};
static Sint16 break_1_door_3_polygons[29] = {
#include "shared/goal_door/break_1_door_3_polygons.inc"
};
static Sint32 break_1_door_3_vertices[23] = {
#include "shared/goal_door/break_1_door_3_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_3_model = {
#include "shared/goal_door/break_1_door_3_model.inc"
};
static NJS_CNK_OBJECT break_1_door_3_object = {
#include "shared/goal_door/break_1_door_3_object.inc"
};
static Sint16 break_1_door_4_polygons[29] = {
#include "shared/goal_door/break_1_door_4_polygons.inc"
};
static Sint32 break_1_door_4_vertices[23] = {
#include "shared/goal_door/break_1_door_4_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_4_model = {
#include "shared/goal_door/break_1_door_4_model.inc"
};
static NJS_CNK_OBJECT break_1_door_4_object = {
#include "shared/goal_door/break_1_door_4_object.inc"
};
static Sint16 break_1_door_5_polygons[25] = {
#include "shared/goal_door/break_1_door_5_polygons.inc"
};
static Sint32 break_1_door_5_vertices[19] = {
#include "shared/goal_door/break_1_door_5_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_5_model = {
#include "shared/goal_door/break_1_door_5_model.inc"
};
static NJS_CNK_OBJECT break_1_door_5_object = {
#include "shared/goal_door/break_1_door_5_object.inc"
};
static Sint16 break_1_door_6_polygons[29] = {
#include "shared/goal_door/break_1_door_6_polygons.inc"
};
static Sint32 break_1_door_6_vertices[23] = {
#include "shared/goal_door/break_1_door_6_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_6_model = {
#include "shared/goal_door/break_1_door_6_model.inc"
};
static NJS_CNK_OBJECT break_1_door_6_object = {
#include "shared/goal_door/break_1_door_6_object.inc"
};
static Sint16 break_1_door_7_polygons[29] = {
#include "shared/goal_door/break_1_door_7_polygons.inc"
};
static Sint32 break_1_door_7_vertices[23] = {
#include "shared/goal_door/break_1_door_7_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_7_model = {
#include "shared/goal_door/break_1_door_7_model.inc"
};
static NJS_CNK_OBJECT break_1_door_7_object = {
#include "shared/goal_door/break_1_door_7_object.inc"
};
static Sint16 break_1_door_8_polygons[45] = {
#include "shared/goal_door/break_1_door_8_polygons.inc"
};
static Sint32 break_1_door_8_vertices[31] = {
#include "shared/goal_door/break_1_door_8_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_8_model = {
#include "shared/goal_door/break_1_door_8_model.inc"
};
static NJS_CNK_OBJECT break_1_door_8_object = {
#include "shared/goal_door/break_1_door_8_object.inc"
};
static Sint16 break_1_door_9_polygons[39] = {
#include "shared/goal_door/break_1_door_9_polygons.inc"
};
static Sint32 break_1_door_9_vertices[27] = {
#include "shared/goal_door/break_1_door_9_vertices.inc"
};
static NJS_CNK_MODEL break_1_door_9_model = {
#include "shared/goal_door/break_1_door_9_model.inc"
};
static NJS_CNK_OBJECT break_1_door_9_object = {
#include "shared/goal_door/break_1_door_9_object.inc"
};
static NJS_CNK_OBJECT break_1_door_object = {
#include "shared/goal_door/break_1_door_object.inc"
};
/* the door broken twice: three models, the children of broken_twice_object */
static NJS_TEXNAME broken_twice_texnames[7] = {
#include "shared/goal_door/broken_twice_texnames.inc"
};
static NJS_TEXLIST broken_twice_texlist = {
#include "shared/goal_door/broken_twice_texlist.inc"
};
static Sint16 broken_twice_0_polygons[205] = {
#include "shared/goal_door/broken_twice_0_polygons.inc"
};
static Sint32 broken_twice_0_vertices[235] = {
#include "shared/goal_door/broken_twice_0_vertices.inc"
};
static NJS_CNK_MODEL broken_twice_0_model = {
#include "shared/goal_door/broken_twice_0_model.inc"
};
static NJS_CNK_OBJECT broken_twice_0_object = {
#include "shared/goal_door/broken_twice_0_object.inc"
};
static Sint16 broken_twice_1_polygons[1493] = {
#include "shared/goal_door/broken_twice_1_polygons.inc"
};
static Sint32 broken_twice_1_vertices[1455] = {
#include "shared/goal_door/broken_twice_1_vertices.inc"
};
static NJS_CNK_MODEL broken_twice_1_model = {
#include "shared/goal_door/broken_twice_1_model.inc"
};
static NJS_CNK_OBJECT broken_twice_1_object = {
#include "shared/goal_door/broken_twice_1_object.inc"
};
static Sint16 broken_twice_2_polygons[409] = {
#include "shared/goal_door/broken_twice_2_polygons.inc"
};
static Sint32 broken_twice_2_vertices[395] = {
#include "shared/goal_door/broken_twice_2_vertices.inc"
};
static NJS_CNK_MODEL broken_twice_2_model = {
#include "shared/goal_door/broken_twice_2_model.inc"
};
static NJS_CNK_OBJECT broken_twice_2_object = {
#include "shared/goal_door/broken_twice_2_object.inc"
};
static NJS_CNK_OBJECT broken_twice_object = {
#include "shared/goal_door/broken_twice_object.inc"
};
/* the second break's pieces of glass: the children of break_2_glass_object,
 * each at its piece's place on the door */
static NJS_TEXNAME break_2_glass_texnames[1] = {
#include "shared/goal_door/break_2_glass_texnames.inc"
};
static NJS_TEXLIST break_2_glass_texlist = {
#include "shared/goal_door/break_2_glass_texlist.inc"
};
static Sint16 break_2_glass_0_polygons[29] = {
#include "shared/goal_door/break_2_glass_0_polygons.inc"
};
static Sint32 break_2_glass_0_vertices[23] = {
#include "shared/goal_door/break_2_glass_0_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_0_model = {
#include "shared/goal_door/break_2_glass_0_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_0_object = {
#include "shared/goal_door/break_2_glass_0_object.inc"
};
static Sint16 break_2_glass_1_polygons[23] = {
#include "shared/goal_door/break_2_glass_1_polygons.inc"
};
static Sint32 break_2_glass_1_vertices[15] = {
#include "shared/goal_door/break_2_glass_1_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_1_model = {
#include "shared/goal_door/break_2_glass_1_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_1_object = {
#include "shared/goal_door/break_2_glass_1_object.inc"
};
static Sint16 break_2_glass_2_polygons[29] = {
#include "shared/goal_door/break_2_glass_2_polygons.inc"
};
static Sint32 break_2_glass_2_vertices[23] = {
#include "shared/goal_door/break_2_glass_2_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_2_model = {
#include "shared/goal_door/break_2_glass_2_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_2_object = {
#include "shared/goal_door/break_2_glass_2_object.inc"
};
static Sint16 break_2_glass_3_polygons[25] = {
#include "shared/goal_door/break_2_glass_3_polygons.inc"
};
static Sint32 break_2_glass_3_vertices[19] = {
#include "shared/goal_door/break_2_glass_3_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_3_model = {
#include "shared/goal_door/break_2_glass_3_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_3_object = {
#include "shared/goal_door/break_2_glass_3_object.inc"
};
static Sint16 break_2_glass_4_polygons[29] = {
#include "shared/goal_door/break_2_glass_4_polygons.inc"
};
static Sint32 break_2_glass_4_vertices[23] = {
#include "shared/goal_door/break_2_glass_4_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_4_model = {
#include "shared/goal_door/break_2_glass_4_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_4_object = {
#include "shared/goal_door/break_2_glass_4_object.inc"
};
static Sint16 break_2_glass_5_polygons[29] = {
#include "shared/goal_door/break_2_glass_5_polygons.inc"
};
static Sint32 break_2_glass_5_vertices[23] = {
#include "shared/goal_door/break_2_glass_5_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_5_model = {
#include "shared/goal_door/break_2_glass_5_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_5_object = {
#include "shared/goal_door/break_2_glass_5_object.inc"
};
static Sint16 break_2_glass_6_polygons[35] = {
#include "shared/goal_door/break_2_glass_6_polygons.inc"
};
static Sint32 break_2_glass_6_vertices[23] = {
#include "shared/goal_door/break_2_glass_6_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_6_model = {
#include "shared/goal_door/break_2_glass_6_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_6_object = {
#include "shared/goal_door/break_2_glass_6_object.inc"
};
static Sint16 break_2_glass_7_polygons[23] = {
#include "shared/goal_door/break_2_glass_7_polygons.inc"
};
static Sint32 break_2_glass_7_vertices[15] = {
#include "shared/goal_door/break_2_glass_7_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_7_model = {
#include "shared/goal_door/break_2_glass_7_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_7_object = {
#include "shared/goal_door/break_2_glass_7_object.inc"
};
static Sint16 break_2_glass_8_polygons[39] = {
#include "shared/goal_door/break_2_glass_8_polygons.inc"
};
static Sint32 break_2_glass_8_vertices[27] = {
#include "shared/goal_door/break_2_glass_8_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_8_model = {
#include "shared/goal_door/break_2_glass_8_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_8_object = {
#include "shared/goal_door/break_2_glass_8_object.inc"
};
static Sint16 break_2_glass_9_polygons[39] = {
#include "shared/goal_door/break_2_glass_9_polygons.inc"
};
static Sint32 break_2_glass_9_vertices[27] = {
#include "shared/goal_door/break_2_glass_9_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_9_model = {
#include "shared/goal_door/break_2_glass_9_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_9_object = {
#include "shared/goal_door/break_2_glass_9_object.inc"
};
static Sint16 break_2_glass_10_polygons[23] = {
#include "shared/goal_door/break_2_glass_10_polygons.inc"
};
static Sint32 break_2_glass_10_vertices[15] = {
#include "shared/goal_door/break_2_glass_10_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_10_model = {
#include "shared/goal_door/break_2_glass_10_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_10_object = {
#include "shared/goal_door/break_2_glass_10_object.inc"
};
static Sint16 break_2_glass_11_polygons[25] = {
#include "shared/goal_door/break_2_glass_11_polygons.inc"
};
static Sint32 break_2_glass_11_vertices[19] = {
#include "shared/goal_door/break_2_glass_11_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_11_model = {
#include "shared/goal_door/break_2_glass_11_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_11_object = {
#include "shared/goal_door/break_2_glass_11_object.inc"
};
static Sint16 break_2_glass_12_polygons[29] = {
#include "shared/goal_door/break_2_glass_12_polygons.inc"
};
static Sint32 break_2_glass_12_vertices[23] = {
#include "shared/goal_door/break_2_glass_12_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_12_model = {
#include "shared/goal_door/break_2_glass_12_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_12_object = {
#include "shared/goal_door/break_2_glass_12_object.inc"
};
static Sint16 break_2_glass_13_polygons[23] = {
#include "shared/goal_door/break_2_glass_13_polygons.inc"
};
static Sint32 break_2_glass_13_vertices[15] = {
#include "shared/goal_door/break_2_glass_13_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_13_model = {
#include "shared/goal_door/break_2_glass_13_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_13_object = {
#include "shared/goal_door/break_2_glass_13_object.inc"
};
static Sint16 break_2_glass_14_polygons[23] = {
#include "shared/goal_door/break_2_glass_14_polygons.inc"
};
static Sint32 break_2_glass_14_vertices[15] = {
#include "shared/goal_door/break_2_glass_14_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_14_model = {
#include "shared/goal_door/break_2_glass_14_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_14_object = {
#include "shared/goal_door/break_2_glass_14_object.inc"
};
static Sint16 break_2_glass_15_polygons[23] = {
#include "shared/goal_door/break_2_glass_15_polygons.inc"
};
static Sint32 break_2_glass_15_vertices[15] = {
#include "shared/goal_door/break_2_glass_15_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_15_model = {
#include "shared/goal_door/break_2_glass_15_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_15_object = {
#include "shared/goal_door/break_2_glass_15_object.inc"
};
static Sint16 break_2_glass_16_polygons[41] = {
#include "shared/goal_door/break_2_glass_16_polygons.inc"
};
static Sint32 break_2_glass_16_vertices[27] = {
#include "shared/goal_door/break_2_glass_16_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_16_model = {
#include "shared/goal_door/break_2_glass_16_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_16_object = {
#include "shared/goal_door/break_2_glass_16_object.inc"
};
static Sint16 break_2_glass_17_polygons[31] = {
#include "shared/goal_door/break_2_glass_17_polygons.inc"
};
static Sint32 break_2_glass_17_vertices[27] = {
#include "shared/goal_door/break_2_glass_17_vertices.inc"
};
static NJS_CNK_MODEL break_2_glass_17_model = {
#include "shared/goal_door/break_2_glass_17_model.inc"
};
static NJS_CNK_OBJECT break_2_glass_17_object = {
#include "shared/goal_door/break_2_glass_17_object.inc"
};
static NJS_CNK_OBJECT break_2_glass_object = {
#include "shared/goal_door/break_2_glass_object.inc"
};
/* the second break's other pieces: the children of break_2_door_object,
 * each at its piece's place on the door */
static NJS_TEXNAME break_2_door_texnames[6] = {
#include "shared/goal_door/break_2_door_texnames.inc"
};
static NJS_TEXLIST break_2_door_texlist = {
#include "shared/goal_door/break_2_door_texlist.inc"
};
static Sint16 break_2_door_0_polygons[219] = {
#include "shared/goal_door/break_2_door_0_polygons.inc"
};
static Sint32 break_2_door_0_vertices[203] = {
#include "shared/goal_door/break_2_door_0_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_0_model = {
#include "shared/goal_door/break_2_door_0_model.inc"
};
static NJS_CNK_OBJECT break_2_door_0_object = {
#include "shared/goal_door/break_2_door_0_object.inc"
};
static Sint16 break_2_door_1_polygons[219] = {
#include "shared/goal_door/break_2_door_1_polygons.inc"
};
static Sint32 break_2_door_1_vertices[203] = {
#include "shared/goal_door/break_2_door_1_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_1_model = {
#include "shared/goal_door/break_2_door_1_model.inc"
};
static NJS_CNK_OBJECT break_2_door_1_object = {
#include "shared/goal_door/break_2_door_1_object.inc"
};
static Sint16 break_2_door_2_polygons[29] = {
#include "shared/goal_door/break_2_door_2_polygons.inc"
};
static Sint32 break_2_door_2_vertices[23] = {
#include "shared/goal_door/break_2_door_2_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_2_model = {
#include "shared/goal_door/break_2_door_2_model.inc"
};
static NJS_CNK_OBJECT break_2_door_2_object = {
#include "shared/goal_door/break_2_door_2_object.inc"
};
static Sint16 break_2_door_3_polygons[29] = {
#include "shared/goal_door/break_2_door_3_polygons.inc"
};
static Sint32 break_2_door_3_vertices[23] = {
#include "shared/goal_door/break_2_door_3_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_3_model = {
#include "shared/goal_door/break_2_door_3_model.inc"
};
static NJS_CNK_OBJECT break_2_door_3_object = {
#include "shared/goal_door/break_2_door_3_object.inc"
};
static Sint16 break_2_door_4_polygons[39] = {
#include "shared/goal_door/break_2_door_4_polygons.inc"
};
static Sint32 break_2_door_4_vertices[27] = {
#include "shared/goal_door/break_2_door_4_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_4_model = {
#include "shared/goal_door/break_2_door_4_model.inc"
};
static NJS_CNK_OBJECT break_2_door_4_object = {
#include "shared/goal_door/break_2_door_4_object.inc"
};
static Sint16 break_2_door_5_polygons[25] = {
#include "shared/goal_door/break_2_door_5_polygons.inc"
};
static Sint32 break_2_door_5_vertices[19] = {
#include "shared/goal_door/break_2_door_5_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_5_model = {
#include "shared/goal_door/break_2_door_5_model.inc"
};
static NJS_CNK_OBJECT break_2_door_5_object = {
#include "shared/goal_door/break_2_door_5_object.inc"
};
static Sint16 break_2_door_6_polygons[39] = {
#include "shared/goal_door/break_2_door_6_polygons.inc"
};
static Sint32 break_2_door_6_vertices[27] = {
#include "shared/goal_door/break_2_door_6_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_6_model = {
#include "shared/goal_door/break_2_door_6_model.inc"
};
static NJS_CNK_OBJECT break_2_door_6_object = {
#include "shared/goal_door/break_2_door_6_object.inc"
};
static Sint16 break_2_door_7_polygons[25] = {
#include "shared/goal_door/break_2_door_7_polygons.inc"
};
static Sint32 break_2_door_7_vertices[19] = {
#include "shared/goal_door/break_2_door_7_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_7_model = {
#include "shared/goal_door/break_2_door_7_model.inc"
};
static NJS_CNK_OBJECT break_2_door_7_object = {
#include "shared/goal_door/break_2_door_7_object.inc"
};
static Sint16 break_2_door_8_polygons[29] = {
#include "shared/goal_door/break_2_door_8_polygons.inc"
};
static Sint32 break_2_door_8_vertices[23] = {
#include "shared/goal_door/break_2_door_8_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_8_model = {
#include "shared/goal_door/break_2_door_8_model.inc"
};
static NJS_CNK_OBJECT break_2_door_8_object = {
#include "shared/goal_door/break_2_door_8_object.inc"
};
static Sint16 break_2_door_9_polygons[31] = {
#include "shared/goal_door/break_2_door_9_polygons.inc"
};
static Sint32 break_2_door_9_vertices[27] = {
#include "shared/goal_door/break_2_door_9_vertices.inc"
};
static NJS_CNK_MODEL break_2_door_9_model = {
#include "shared/goal_door/break_2_door_9_model.inc"
};
static NJS_CNK_OBJECT break_2_door_9_object = {
#include "shared/goal_door/break_2_door_9_object.inc"
};
static NJS_CNK_OBJECT break_2_door_object = {
#include "shared/goal_door/break_2_door_object.inc"
};
/* the last break's pieces: the children of break_3_object, each at its
 * piece's place on the door */
static NJS_TEXNAME break_3_texnames[8] = {
#include "shared/goal_door/break_3_texnames.inc"
};
static NJS_TEXLIST break_3_texlist = {
#include "shared/goal_door/break_3_texlist.inc"
};
static Sint16 break_3_0_polygons[305] = {
#include "shared/goal_door/break_3_0_polygons.inc"
};
static Sint32 break_3_0_vertices[287] = {
#include "shared/goal_door/break_3_0_vertices.inc"
};
static NJS_CNK_MODEL break_3_0_model = {
#include "shared/goal_door/break_3_0_model.inc"
};
static NJS_CNK_OBJECT break_3_0_object = {
#include "shared/goal_door/break_3_0_object.inc"
};
static Sint16 break_3_1_polygons[571] = {
#include "shared/goal_door/break_3_1_polygons.inc"
};
static Sint32 break_3_1_vertices[495] = {
#include "shared/goal_door/break_3_1_vertices.inc"
};
static NJS_CNK_MODEL break_3_1_model = {
#include "shared/goal_door/break_3_1_model.inc"
};
static NJS_CNK_OBJECT break_3_1_object = {
#include "shared/goal_door/break_3_1_object.inc"
};
static Sint16 break_3_2_polygons[305] = {
#include "shared/goal_door/break_3_2_polygons.inc"
};
static Sint32 break_3_2_vertices[291] = {
#include "shared/goal_door/break_3_2_vertices.inc"
};
static NJS_CNK_MODEL break_3_2_model = {
#include "shared/goal_door/break_3_2_model.inc"
};
static NJS_CNK_OBJECT break_3_2_object = {
#include "shared/goal_door/break_3_2_object.inc"
};
static Sint16 break_3_3_polygons[557] = {
#include "shared/goal_door/break_3_3_polygons.inc"
};
static Sint32 break_3_3_vertices[499] = {
#include "shared/goal_door/break_3_3_vertices.inc"
};
static NJS_CNK_MODEL break_3_3_model = {
#include "shared/goal_door/break_3_3_model.inc"
};
static NJS_CNK_OBJECT break_3_3_object = {
#include "shared/goal_door/break_3_3_object.inc"
};
static Sint16 break_3_4_polygons[865] = {
#include "shared/goal_door/break_3_4_polygons.inc"
};
static Sint32 break_3_4_vertices[819] = {
#include "shared/goal_door/break_3_4_vertices.inc"
};
static NJS_CNK_MODEL break_3_4_model = {
#include "shared/goal_door/break_3_4_model.inc"
};
static NJS_CNK_OBJECT break_3_4_object = {
#include "shared/goal_door/break_3_4_object.inc"
};
static Sint16 break_3_5_polygons[395] = {
#include "shared/goal_door/break_3_5_polygons.inc"
};
static Sint32 break_3_5_vertices[343] = {
#include "shared/goal_door/break_3_5_vertices.inc"
};
static NJS_CNK_MODEL break_3_5_model = {
#include "shared/goal_door/break_3_5_model.inc"
};
static NJS_CNK_OBJECT break_3_5_object = {
#include "shared/goal_door/break_3_5_object.inc"
};
static Sint16 break_3_6_polygons[567] = {
#include "shared/goal_door/break_3_6_polygons.inc"
};
static Sint32 break_3_6_vertices[515] = {
#include "shared/goal_door/break_3_6_vertices.inc"
};
static NJS_CNK_MODEL break_3_6_model = {
#include "shared/goal_door/break_3_6_model.inc"
};
static NJS_CNK_OBJECT break_3_6_object = {
#include "shared/goal_door/break_3_6_object.inc"
};
static Sint16 break_3_7_polygons[653] = {
#include "shared/goal_door/break_3_7_polygons.inc"
};
static Sint32 break_3_7_vertices[555] = {
#include "shared/goal_door/break_3_7_vertices.inc"
};
static NJS_CNK_MODEL break_3_7_model = {
#include "shared/goal_door/break_3_7_model.inc"
};
static NJS_CNK_OBJECT break_3_7_object = {
#include "shared/goal_door/break_3_7_object.inc"
};
static NJS_CNK_OBJECT break_3_object = {
#include "shared/goal_door/break_3_object.inc"
};
/* the two scrap models. every break throws scraps of both. the throw places
 * them itself, so no code reads their nodes */
static NJS_TEXNAME scrap_0_texnames[1] = {
#include "shared/goal_door/scrap_0_texnames.inc"
};
static NJS_TEXLIST scrap_0_texlist = {
#include "shared/goal_door/scrap_0_texlist.inc"
};
static Sint16 scrap_0_polygons[63] = {
#include "shared/goal_door/scrap_0_polygons.inc"
};
static Sint32 scrap_0_vertices[51] = {
#include "shared/goal_door/scrap_0_vertices.inc"
};
static NJS_CNK_MODEL scrap_0_model = {
#include "shared/goal_door/scrap_0_model.inc"
};
static NJS_CNK_OBJECT scrap_0_object = {
#include "shared/goal_door/scrap_0_object.inc"
};
static NJS_TEXNAME scrap_1_texnames[1] = {
#include "shared/goal_door/scrap_1_texnames.inc"
};
static NJS_TEXLIST scrap_1_texlist = {
#include "shared/goal_door/scrap_1_texlist.inc"
};
static Sint16 scrap_1_polygons[129] = {
#include "shared/goal_door/scrap_1_polygons.inc"
};
static Sint32 scrap_1_vertices[119] = {
#include "shared/goal_door/scrap_1_vertices.inc"
};
static NJS_CNK_MODEL scrap_1_model = {
#include "shared/goal_door/scrap_1_model.inc"
};
static NJS_CNK_OBJECT scrap_1_object = {
#include "shared/goal_door/scrap_1_object.inc"
};
/* the chips that a hit short of a break throws from a broken door. an even
 * count of hits throws chip_glass_object's children, and an odd count
 * chip_door_object's. each child lies at its chip's place on the door */
static NJS_TEXNAME chip_glass_texnames[1] = {
#include "shared/goal_door/chip_glass_texnames.inc"
};
static NJS_TEXLIST chip_glass_texlist = {
#include "shared/goal_door/chip_glass_texlist.inc"
};
static Sint16 chip_glass_0_polygons[33] = {
#include "shared/goal_door/chip_glass_0_polygons.inc"
};
static Sint32 chip_glass_0_vertices[27] = {
#include "shared/goal_door/chip_glass_0_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_0_model = {
#include "shared/goal_door/chip_glass_0_model.inc"
};
static NJS_CNK_OBJECT chip_glass_0_object = {
#include "shared/goal_door/chip_glass_0_object.inc"
};
static Sint16 chip_glass_1_polygons[25] = {
#include "shared/goal_door/chip_glass_1_polygons.inc"
};
static Sint32 chip_glass_1_vertices[19] = {
#include "shared/goal_door/chip_glass_1_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_1_model = {
#include "shared/goal_door/chip_glass_1_model.inc"
};
static NJS_CNK_OBJECT chip_glass_1_object = {
#include "shared/goal_door/chip_glass_1_object.inc"
};
static Sint16 chip_glass_2_polygons[23] = {
#include "shared/goal_door/chip_glass_2_polygons.inc"
};
static Sint32 chip_glass_2_vertices[15] = {
#include "shared/goal_door/chip_glass_2_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_2_model = {
#include "shared/goal_door/chip_glass_2_model.inc"
};
static NJS_CNK_OBJECT chip_glass_2_object = {
#include "shared/goal_door/chip_glass_2_object.inc"
};
static Sint16 chip_glass_3_polygons[33] = {
#include "shared/goal_door/chip_glass_3_polygons.inc"
};
static Sint32 chip_glass_3_vertices[19] = {
#include "shared/goal_door/chip_glass_3_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_3_model = {
#include "shared/goal_door/chip_glass_3_model.inc"
};
static NJS_CNK_OBJECT chip_glass_3_object = {
#include "shared/goal_door/chip_glass_3_object.inc"
};
static Sint16 chip_glass_4_polygons[23] = {
#include "shared/goal_door/chip_glass_4_polygons.inc"
};
static Sint32 chip_glass_4_vertices[15] = {
#include "shared/goal_door/chip_glass_4_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_4_model = {
#include "shared/goal_door/chip_glass_4_model.inc"
};
static NJS_CNK_OBJECT chip_glass_4_object = {
#include "shared/goal_door/chip_glass_4_object.inc"
};
static Sint16 chip_glass_5_polygons[23] = {
#include "shared/goal_door/chip_glass_5_polygons.inc"
};
static Sint32 chip_glass_5_vertices[15] = {
#include "shared/goal_door/chip_glass_5_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_5_model = {
#include "shared/goal_door/chip_glass_5_model.inc"
};
static NJS_CNK_OBJECT chip_glass_5_object = {
#include "shared/goal_door/chip_glass_5_object.inc"
};
static Sint16 chip_glass_6_polygons[23] = {
#include "shared/goal_door/chip_glass_6_polygons.inc"
};
static Sint32 chip_glass_6_vertices[15] = {
#include "shared/goal_door/chip_glass_6_vertices.inc"
};
static NJS_CNK_MODEL chip_glass_6_model = {
#include "shared/goal_door/chip_glass_6_model.inc"
};
static NJS_CNK_OBJECT chip_glass_6_object = {
#include "shared/goal_door/chip_glass_6_object.inc"
};
static NJS_CNK_OBJECT chip_glass_object = {
#include "shared/goal_door/chip_glass_object.inc"
};
static NJS_TEXNAME chip_door_texnames[1] = {
#include "shared/goal_door/chip_door_texnames.inc"
};
static NJS_TEXLIST chip_door_texlist = {
#include "shared/goal_door/chip_door_texlist.inc"
};
static Sint16 chip_door_0_polygons[25] = {
#include "shared/goal_door/chip_door_0_polygons.inc"
};
static Sint32 chip_door_0_vertices[19] = {
#include "shared/goal_door/chip_door_0_vertices.inc"
};
static NJS_CNK_MODEL chip_door_0_model = {
#include "shared/goal_door/chip_door_0_model.inc"
};
static NJS_CNK_OBJECT chip_door_0_object = {
#include "shared/goal_door/chip_door_0_object.inc"
};
static Sint16 chip_door_1_polygons[29] = {
#include "shared/goal_door/chip_door_1_polygons.inc"
};
static Sint32 chip_door_1_vertices[23] = {
#include "shared/goal_door/chip_door_1_vertices.inc"
};
static NJS_CNK_MODEL chip_door_1_model = {
#include "shared/goal_door/chip_door_1_model.inc"
};
static NJS_CNK_OBJECT chip_door_1_object = {
#include "shared/goal_door/chip_door_1_object.inc"
};
static Sint16 chip_door_2_polygons[45] = {
#include "shared/goal_door/chip_door_2_polygons.inc"
};
static Sint32 chip_door_2_vertices[31] = {
#include "shared/goal_door/chip_door_2_vertices.inc"
};
static NJS_CNK_MODEL chip_door_2_model = {
#include "shared/goal_door/chip_door_2_model.inc"
};
static NJS_CNK_OBJECT chip_door_2_object = {
#include "shared/goal_door/chip_door_2_object.inc"
};
static Sint16 chip_door_3_polygons[29] = {
#include "shared/goal_door/chip_door_3_polygons.inc"
};
static Sint32 chip_door_3_vertices[23] = {
#include "shared/goal_door/chip_door_3_vertices.inc"
};
static NJS_CNK_MODEL chip_door_3_model = {
#include "shared/goal_door/chip_door_3_model.inc"
};
static NJS_CNK_OBJECT chip_door_3_object = {
#include "shared/goal_door/chip_door_3_object.inc"
};
static NJS_CNK_OBJECT chip_door_object = {
#include "shared/goal_door/chip_door_object.inc"
};

/* the kind tables, indexed by KIND. for each mode but MODE_GONE, they give
 * what the door draws and the texture list it draws with */
static NJS_CNK_MODEL *whole_models[DOOR_KINDS] = {
#include "shared/goal_door/whole_models.inc"
};
static NJS_TEXLIST *whole_texlists[DOOR_KINDS] = {
#include "shared/goal_door/whole_texlists.inc"
};
static NJS_CNK_OBJECT *broken_once_objects[DOOR_KINDS] = {
#include "shared/goal_door/broken_once_objects.inc"
};
static NJS_TEXLIST *broken_once_texlists[DOOR_KINDS] = {
#include "shared/goal_door/broken_once_texlists.inc"
};
static NJS_CNK_OBJECT *broken_twice_objects[DOOR_KINDS] = {
#include "shared/goal_door/broken_twice_objects.inc"
};
static NJS_TEXLIST *broken_twice_texlists[DOOR_KINDS] = {
#include "shared/goal_door/broken_twice_texlists.inc"
};
/* each kind's name */
static char *kind_names[DOOR_KINDS] = {
#include "shared/goal_door/kind_names.inc"
};
NJS_TEXLIST *goal_door_texlists[13] = {
#include "shared/goal_door/goal_door_texlists.inc"
};

/* clang-format off */
static struct collision_shape goal_door_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(1), SHAPE_HURTABLE,
    {0.0F, 25.0F, 0.0F}, 25.1F, 25.1F, 7.0F, 0, {0, 0, 0},
};
/* clang-format on */
static int break_hits = 4;
static Float piece_gravity = 0.06F;
static Float piece_drag = 0.9775F;
static Float spin_drag = 0.98F;
static int break_volume = 125;
static int last_break_volume = 125;
static int gone_burst_volume = 125;
/* each mode's step, indexed by goal_door_mode */
static void (*goal_door_mode_steps[])(struct task *task) = {
    goal_door_whole_step,
    goal_door_broken_once_step,
    goal_door_broken_twice_step,
    goal_door_gone_step,
};
static Uint32 hit_tint = 0xffff4040;
/* the whole door's glow repeats every glow_rise + glow_fall steps of
 * game_state.steps. it brightens over the first glow_rise and dims over the
 * rest. the glow's tint holds one level, from 0 to CHANNEL_FULL, in all four
 * bytes */
static int glow_rise = 15;
static int glow_fall = 45;
/* a broken door's models pulse between their own colors and pulse_tint. the
 * pulse's angle climbs by pulse_rate a step, so a pulse takes FULL_TURN /
 * pulse_rate steps */
static Uint32 pulse_tint = 0xffff6060;
static Angle pulse_rate = 0x300;
/* a broken door smokes and sparks from points on it, its emitters, while
 * game_state.time_stopped is clear. each step, each emitter puffs smoke with
 * the chance smoke_chance. a puff's scale is smoke_scale plus up to
 * smoke_scale_spread. the puff flies out from the door, turned aside by up to
 * smoke_spread / 2 either way */
static Float smoke_chance = 0.07F;
static Float smoke_scale = 3.0F;
static Float smoke_scale_spread = 1.0F;
static Angle smoke_spread = EIGHTH_TURN;
/* emitter i can spark on the first spark_steps of every spark_periods[i]
 * steps of game_state.steps. on each of those, it sparks with the chance
 * spark_chance */
static Float spark_chance = 0.85F;
static Uint32 spark_steps = 12;
static Float spark_spread = 0.333F;
/* the emitters' places on the door, x across it and y up from its pos. no code
 * reads z. a door broken once uses the first ONCE_EMITTERS, and a door broken
 * twice all TWICE_EMITTERS */
#define ONCE_EMITTERS 2
#define TWICE_EMITTERS 4
static NJS_POINT3 emitters[TWICE_EMITTERS] = {
#include "shared/goal_door/emitters.inc"
};
static Uint32 spark_periods[TWICE_EMITTERS] = {123, 100, 145, 171};
/* the smoke's and the sparks' speeds: z out from the door, and y up. no code
 * reads x */
static NJS_VECTOR smoke_spd = {0.0F, 0.15F, 0.2F};
static NJS_VECTOR spark_spd = {0.0F, 0.125F, 0.25F};

static Float last_burst_scale = 25.0F;
/* MODE_GONE's two bursts, 0 and 1. burst n goes off on the step that finds
 * GONE_STEPS at burst_n_step. it lies burst_n_x across the door and burst_n_y
 * above the door's pos */
static Float burst_0_x = -15.0F;
static Float burst_0_y = 35.0F;
static Float burst_0_scale = 15.0F;
static int burst_0_step = 10;
static Float burst_1_x = 10.0F;
static Float burst_1_y = 10.0F;
static Float burst_1_scale = 10.0F;
static int burst_1_step = 20;
/* the puff of dust a piece may raise at its throw flies the piece's way at
 * about puff_speed. a fading piece flies at about fading_speed */
static Float puff_speed = 0.25F;
static Float fading_speed = 0.75F;
/* each axis of a piece starts to spin at about piece_spin a step, either
 * way */
static Angle piece_spin = 0x180;
static int piece_life = 60;
static Float piece_fade_start = 0.2F;

static Float dat_goal_door_1 = 1.0F;
static Float dat_goal_door_2 = 7.5F;
/* a piece's puff of dust lies about puff_radius from the piece, at a scale of
 * about puff_scale */
static Float puff_radius = 10.0F;
static Float puff_scale = 7.5F;
/* each step, a piece that trails dust raises a puff with the chance
 * trail_chance. the puff's scale is 0.9 of trail_scale, plus up to 0.2 */
static Float trail_chance = 0.1F;
static Float trail_scale = 10.0F;

static Angle yaw_per_x = 0x1000;
static Angle pitch_per_y = 0x1000;
/* goal_door_throw_solid's pieces fly at about solid_speed, and scraps at
 * about scrap_speed */
static Float solid_speed = 1.0F;
static Float scrap_speed = 1.2F;

void goal_door_init(struct task *task)
{
    /* @hack spelling: the stores of draw, step and destructor inside the
     * test's branch, and an else that returns, where an early return would
     * do */
    if (!placement_try_leave(task)) {
        task->draw = goal_door_draw;
        task->step = goal_door_step;
        task->destructor = goal_door_destroy;
    } else {
        return;
    }
    if (placement_get_state(task) != MODE_GONE) {
        collision_init(task, &goal_door_shape, 1, COLLISION_OBJECT);
    }
    /* @hack redundant: placement_get_state called again, where the first
     * call's result would do */
    task->work->mode = placement_get_state(task);
    NEXT_MODE(task->work) = NO_MODE;
    COOLDOWN(task) = 0;
    HITS(task->work) = 0;
}

static void goal_door_destroy(struct task *task)
{
}

static void goal_door_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        goal_door_apply_mode(task);
        goal_door_mode_steps[task->work->mode](task);
    }
}

/* a gone door draws nothing */
static void goal_door_draw(struct task *task)
{
    struct task_work *work;
    /* @hack redundant: the first values of color, shade, rise, fall, phase,
     * level and pulse, which no path reads */
    /* @hack type: color an int, where a Uint32 would do */
    int color = 0;
    Uint32 shade = 0;
    int rise = 0;
    int fall = 0;
    int phase = 0;
    Uint8 level = 0;
    Float pulse = 0.0F;

    work = task->work;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, DOOR_LIFT, 0.0F);
    njRotateY(NULL, work->ang.y);
    /* @hack redundant: a branch no path takes, which stores TINT_FADE in
     * scl.x */
    if (0) {
        task->work->scl.x = TINT_FADE;
    }
    /* @hack spelling: task->work read in place in the switch and its cases,
     * where work would do */
    switch (task->work->mode) {
    case MODE_WHOLE:
        njSetTexture(whole_texlists[KIND(task->work)]);
        /* @hack redundant: a test of COOLDOWN for 0, whose branch draws as
         * the last branch does */
        if (COOLDOWN(task) == 0) {
            njCnkEasyDrawModel(whole_models[KIND(task->work)]);
        } else if (COOLDOWN(task) >= TINT_END) {
            color = color_blend(COLOR_NONE, hit_tint,
                                (COOLDOWN(task) - TINT_END) * TINT_FADE);
            model_draw_tinted(whole_models[KIND(task->work)], color);
        } else {
            njCnkEasyDrawModel(whole_models[KIND(task->work)]);
        }
        rise = glow_rise;
        fall = glow_fall;
        /* @hack redundant: a branch no path takes, which stores fall in
         * smoke_spread */
        if (0) {
            smoke_spread = fall;
        }
        /* @hack redundant: work read from task again, though nothing reads it
         * after */
        work = task->work;
        /* @hack type: rise converted to a Uint32 in the modulo, where rise +
         * fall would do */
        phase = game_state.steps % ((Uint32)rise + fall);
        if (phase < rise) {
            level = (Float)phase / (Float)(rise - 1) * CHANNEL_FULL;
        } else {
            level = (Float)(rise + fall - phase - 1) / (Float)(fall - 1) *
                    CHANNEL_FULL;
        }
        color = (level << 24) | (level << 16) | (level << 8) | level;
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, GLOW_HEIGHT, GLOW_DEPTH);
        njScale(NULL, GLOW_SCALE, GLOW_SCALE, 1.0F);
        model_draw_tinted_additive(&whole_glow_model, color);
        njPopMatrixEx();
        break;
    case MODE_BROKEN_ONCE:
        /* @hack spelling: pulse, a local here and in the next case, where its
         * value passed in place would do */
        pulse = (njCos(game_state.steps * pulse_rate) + 1.0F) * 0.5F;
        shade = color_blend(COLOR_NONE, pulse_tint, pulse);
        njSetTexture(broken_once_texlists[KIND(task->work)]);
        /* @hack redundant: a test of COOLDOWN for 0, whose branch draws as
         * the last branch does */
        if (COOLDOWN(task) == 0) {
            goal_door_draw_children_tinted(
                broken_once_objects[KIND(task->work)], shade);
        } else if (COOLDOWN(task) >= TINT_END) {
            color = color_blend(shade, hit_tint,
                                (COOLDOWN(task) - TINT_END) * TINT_FADE);
            goal_door_draw_children_tinted(
                broken_once_objects[KIND(task->work)], color);
        } else {
            goal_door_draw_children_tinted(
                broken_once_objects[KIND(task->work)], shade);
        }
        break;
    case MODE_BROKEN_TWICE:
        pulse = (njCos(game_state.steps * pulse_rate) + 1.0F) * 0.5F;
        shade = color_blend(COLOR_NONE, pulse_tint, pulse);
        njSetTexture(broken_twice_texlists[KIND(task->work)]);
        /* @hack redundant: a test of COOLDOWN for 0, whose branch draws as
         * the last branch does */
        if (COOLDOWN(task) == 0) {
            goal_door_draw_children_tinted(
                broken_twice_objects[KIND(task->work)], shade);
        } else if (COOLDOWN(task) >= TINT_END) {
            color = color_blend(shade, hit_tint,
                                (COOLDOWN(task) - TINT_END) * TINT_FADE);
            goal_door_draw_children_tinted(
                broken_twice_objects[KIND(task->work)], color);
        } else {
            goal_door_draw_children_tinted(
                broken_twice_objects[KIND(task->work)], shade);
        }
        break;
    }
    njPopMatrixEx();
}

static void goal_door_apply_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        placement_set_state(task, task->work->mode);
        NEXT_MODE(task->work) = NO_MODE;
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void goal_door_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void goal_door_whole_step(struct task *task)
{
    struct task_work *work;
    struct task *hit;
    NJS_VECTOR spd;

    work = task->work;
    if (COOLDOWN(task) > 0) {
        COOLDOWN(task)--;
        /* @hack spelling: hit assigned inside the test, where a statement of
         * its own would do */
    } else if ((work->collision->flags & COLLISION_TOUCHED) &&
               (hit = work->collision->hit->owner) && hit->work &&
               hit->work->id == WORK_ID_ATTACK) {
        COOLDOWN(task) = HIT_STEPS;
        /* @hack spelling: task->work read in place in HITS, where work would
         * do */
        if (++HITS(task->work) == break_hits) {
            spd.x = 0.0F;
            spd.y = 0.0F;
            spd.z = 0.0F;
            dust_sand_create(&hit->work->pos, &spd, BREAK_DUST_SCALE);
            sound_play(BREAK_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, break_volume);
            goal_door_throw_fading(task, &break_1_glass_object,
                                   &break_1_glass_texlist);
            goal_door_throw_fading(task, &break_1_door_object,
                                   &break_1_door_texlist);
            goal_door_throw_scrap(task, &scrap_0_model, &scrap_0_texlist);
            goal_door_throw_scrap(task, &scrap_1_model, &scrap_1_texlist);
            goal_door_set_next_mode(task, MODE_BROKEN_ONCE);
            HITS(task->work) = 0;
            return;
        }
    }
    collision_register(task);
}

/* start MODE_GONE's count of steps, and play LAST_BREAK_SOUND. the caller
 * sets the mode itself */
static void goal_door_start_gone(struct task *task)
{
    struct task_work *work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR spd;

    work = task->work;
    /* @hack spelling: pos, a copy of the work's pos, where work->pos read in
     * place would do */
    pos = work->pos;
    /* @hack redundant: spd, set and never read */
    spd.x = 0.0F;
    spd.y = 0.7F;
    spd.z = 0.0F;
    /* @hack redundant: a probe of the floor below the door, whose height and
     * tilt nothing reads */
    surface_probe_floor(pos.x, pos.y + PROBE_HEIGHT, pos.z, &ang);
    sound_play(LAST_BREAK_SOUND, task, 0, 0);
    sound_set_volume(task, SOUND_ANY_ID, last_break_volume);
    GONE_STEPS(task) = 0;
}

static void goal_door_broken_once_step(struct task *task)
{
    struct task_work *work;
    struct task *hit;
    /* @hack redundant: x and z, a second pair of locals for polar_xz, where
     * vx and vz would do */
    Float x;
    Float z;
    Float vx;
    Float vz;
    NJS_POINT3 pos;
    NJS_POINT3 off;
    NJS_VECTOR spd;
    Angle ang;
    int i;

    work = task->work;
    if (!game_state.time_stopped) {
        for (i = 0; i < ONCE_EMITTERS; i++) {
            if (CHANCE(smoke_chance)) {
                pos = work->pos;
                point_turn_y(emitters[i].x, 0.0F, work->ang.y, &off);
                pos.x += off.x;
                pos.y += emitters[i].y;
                pos.z += off.z;
                spd = smoke_spd;
                ang = (Float)work->ang.y +
                      ((Float)smoke_spread * (Float)rand() * RAND_SCALE) -
                      /* NOLINTNEXTLINE(bugprone-integer-division) */
                      (Float)(smoke_spread / 2);
                polar_xz(ang, spd.z, &vx, &vz);
                spd.x = vx * RANDOM_FACTOR(0.2F);
                spd.z = vz * RANDOM_FACTOR(0.2F);
                spd.y *= RANDOM_FACTOR(0.2F);
                black_smoke_rising_create(
                    &pos, &spd,
                    smoke_scale +
                        ((Float)rand() * smoke_scale_spread * RAND_SCALE));
            }
            if (game_state.steps % spark_periods[i] < spark_steps &&
                CHANCE(spark_chance)) {
                pos = work->pos;
                point_turn_y(emitters[i].x, 0.0F, work->ang.y, &off);
                pos.x += off.x;
                pos.y += emitters[i].y;
                pos.z += off.z;
                spd = spark_spd;
                polar_xz(work->ang.y, spd.z, &x, &z);
                spd.x = x * RANDOM_FACTOR(0.2F);
                spd.z = z * RANDOM_FACTOR(0.2F);
                spd.y *= RANDOM_FACTOR(0.2F);
                spark_point_create(&pos, &spd, spark_spread,
                                   work->pos.y - SPARK_DROP);
            }
        }
    }
    if (COOLDOWN(task) > 0) {
        COOLDOWN(task)--;
        /* @hack spelling: hit assigned inside the test, where a statement of
         * its own would do */
    } else if ((work->collision->flags & COLLISION_TOUCHED) &&
               (hit = work->collision->hit->owner) && hit->work &&
               hit->work->id == WORK_ID_ATTACK) {
        COOLDOWN(task) = HIT_STEPS;
        /* @hack spelling: task->work read in place in HITS, where work would
         * do */
        if (++HITS(task->work) == break_hits) {
            /* @hack spelling: pos cleared to serve as the dust's speed, where
             * spd would do */
            pos.x = 0.0F;
            pos.y = 0.0F;
            pos.z = 0.0F;
            dust_sand_create(&hit->work->pos, &pos, BREAK_DUST_SCALE);
            sound_play(BREAK_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, break_volume);
            goal_door_throw_fading(task, &break_2_glass_object,
                                   &break_2_glass_texlist);
            goal_door_throw_fading(task, &break_2_door_object,
                                   &break_2_door_texlist);
            goal_door_throw_scrap(task, &scrap_0_model, &scrap_0_texlist);
            goal_door_throw_scrap(task, &scrap_1_model, &scrap_1_texlist);
            goal_door_set_next_mode(task, MODE_BROKEN_TWICE);
            HITS(task->work) = 0;
            return;
        }
        if (HITS(task->work) % 2 == 0) {
            goal_door_throw_fading(task, &chip_glass_object,
                                   &chip_glass_texlist);
        } else {
            goal_door_throw_fading(task, &chip_door_object, &chip_door_texlist);
        }
    }
    collision_register(task);
}

static void goal_door_broken_twice_step(struct task *task)
{
    struct task_work *work;
    struct task *hit;
    /* @hack redundant: x and z, a second pair of locals for polar_xz, where
     * vx and vz would do */
    Float x;
    Float z;
    Float vx;
    Float vz;
    NJS_POINT3 pos;
    NJS_POINT3 off;
    NJS_VECTOR spd;
    Angle ang;
    int i;

    work = task->work;
    if (!game_state.time_stopped) {
        for (i = 0; i < TWICE_EMITTERS; i++) {
            if (CHANCE(smoke_chance)) {
                pos = work->pos;
                point_turn_y(emitters[i].x, 0.0F, work->ang.y, &off);
                pos.x += off.x;
                pos.y += emitters[i].y;
                pos.z += off.z;
                spd = smoke_spd;
                ang = (Float)work->ang.y +
                      ((Float)smoke_spread * (Float)rand() * RAND_SCALE) -
                      /* NOLINTNEXTLINE(bugprone-integer-division) */
                      (Float)(smoke_spread / 2);
                polar_xz(ang, spd.z, &vx, &vz);
                spd.x = vx * RANDOM_FACTOR(0.2F);
                spd.z = vz * RANDOM_FACTOR(0.2F);
                spd.y *= RANDOM_FACTOR(0.2F);
                black_smoke_rising_create(
                    &pos, &spd,
                    smoke_scale +
                        ((Float)rand() * smoke_scale_spread * RAND_SCALE));
            }
            if (game_state.steps % spark_periods[i] < spark_steps &&
                CHANCE(spark_chance)) {
                pos = work->pos;
                point_turn_y(emitters[i].x, 0.0F, work->ang.y, &off);
                pos.x += off.x;
                pos.y += emitters[i].y;
                pos.z += off.z;
                spd = spark_spd;
                polar_xz(work->ang.y, spd.z, &x, &z);
                spd.x = x * RANDOM_FACTOR(0.2F);
                spd.z = z * RANDOM_FACTOR(0.2F);
                spd.y *= RANDOM_FACTOR(0.2F);
                spark_point_create(&pos, &spd, spark_spread,
                                   work->pos.y - SPARK_DROP);
            }
        }
    }
    if (COOLDOWN(task) > 0) {
        COOLDOWN(task)--;
        /* @hack spelling: hit assigned inside the test, where a statement of
         * its own would do */
    } else if ((work->collision->flags & COLLISION_TOUCHED) &&
               (hit = work->collision->hit->owner) && hit->work &&
               hit->work->id == WORK_ID_ATTACK) {
        COOLDOWN(task) = HIT_STEPS;
        /* @hack spelling: task->work read in place in HITS, where work would
         * do */
        if (++HITS(task->work) == break_hits) {
            /* @hack spelling: pos cleared to serve as the dust's speed, where
             * spd would do */
            pos.x = 0.0F;
            pos.y = 0.0F;
            pos.z = 0.0F;
            dust_sand_create(&hit->work->pos, &pos, BREAK_DUST_SCALE);
            goal_door_start_gone(task);
            goal_door_throw_solid(task, &break_3_object, &break_3_texlist);
            goal_door_throw_scrap(task, &scrap_0_model, &scrap_0_texlist);
            goal_door_throw_scrap(task, &scrap_1_model, &scrap_1_texlist);
            /* @hack spelling: off as the burst's speed, and spd as its place,
             * where spd and pos would do */
            off.x = off.y = off.z = 0.0F;
            spd = work->pos;
            spd.y += LAST_BURST_HEIGHT;
            burst_create_wobbling(&spd, &off, last_burst_scale);
            goal_door_set_next_mode(task, MODE_GONE);
            HITS(task->work) = 0;
            return;
        }
        if (HITS(task->work) % 2 == 0) {
            goal_door_throw_fading(task, &chip_glass_object,
                                   &chip_glass_texlist);
        } else {
            goal_door_throw_fading(task, &chip_door_object, &chip_door_texlist);
        }
    }
    collision_register(task);
}

static void goal_door_gone_step(struct task *task)
{
    struct task_work *work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    NJS_POINT3 off;

    work = task->work;
    if (GONE_STEPS(task) == burst_0_step) {
        spd.x = spd.y = spd.z = 0.0F;
        pos = work->pos;
        point_turn_y(burst_0_x, 0.0F, work->ang.y, &off);
        pos.x += off.x;
        pos.y += burst_0_y;
        pos.z += off.z;
        burst_create_wobbling(&pos, &spd, burst_0_scale);
        sound_play(GONE_BURST_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, gone_burst_volume);
    }
    if (GONE_STEPS(task) == burst_1_step) {
        spd.x = spd.y = spd.z = 0.0F;
        pos = work->pos;
        point_turn_y(burst_1_x, 0.0F, work->ang.y, &off);
        pos.x += off.x;
        pos.y += burst_1_y;
        pos.z += off.z;
        burst_create_wobbling(&pos, &spd, burst_1_scale);
        sound_play(GONE_BURST_SOUND, task, 0, 0);
        sound_set_volume(task, SOUND_ANY_ID, gone_burst_volume);
    }
    if (++GONE_STEPS(task) >= GONE_END) {
        stage_clear(0);
    }
    collision_register(task);
}

/* throw a piece for each child of object, at most PIECES, in a new child task
 * of task. each piece starts at its child's place on the door, and draws the
 * child's model with texlist. a chip raises a puff of dust, and any other
 * piece does half the time. where the heap has no room for the task or its
 * pieces, the call throws nothing */
static void goal_door_throw_fading(struct task *task, NJS_CNK_OBJECT *object,
                                   NJS_TEXLIST *texlist)
{
    struct task_work *work;
    NJS_CNK_OBJECT *node;
    struct task *child;
    struct goal_door_piece *piece;
    struct goal_door_piece *cur;
    Float radius;
    Float scale;
    /* @hack type: ang, a struct angles for the turn and the tilt, where two
     * Angles would do */
    struct angles ang;
    NJS_VECTOR puff_base;
    NJS_VECTOR puff_spd;
    NJS_POINT3 puff_pos;
    NJS_POINT3 off;
    int i;

    work = task->work;
    node = object->child;
    /* @hack spelling: child assigned inside the test, where a statement of
     * its own would do */
    if (!(child = task_create_child(TASK_WORK, goal_door_pieces_step, task))) {
        return;
    }
    /* @hack spelling: the pieces assigned to extra inside the test, where a
     * statement of its own would do */
    if (!(child->extra.p = syCalloc(PIECES, sizeof(struct goal_door_piece)))) {
        task_kill(child);
        return;
    }
    child->draw = goal_door_pieces_draw;
    child->destructor = goal_door_pieces_destroy;
    LIVE_PIECES(child) = 0;
    piece = TASK_PIECES(child);
    /* @hack spelling: the steps written i = i + 1 and piece = piece + 1, where
     * ++ would do */
    for (i = 0; i < PIECES; i = i + 1) {
        /* @hack spelling: cur, a copy of piece, where piece read in place would
         * do */
        cur = piece;
        cur->live = FALSE;
        piece = piece + 1;
    }
    piece = TASK_PIECES(child);
    for (; node; node = node->sibling, piece++) {
        point_turn_y(node->pos[0], node->pos[2], work->ang.y, &off);
        piece->pos.x = work->pos.x + off.x;
        piece->pos.y = work->pos.y + node->pos[1];
        piece->pos.z = work->pos.z + off.z;
        piece->ang.x = node->ang[0];
        piece->ang.y = work->ang.y + node->ang[1];
        piece->ang.z = node->ang[2];
        ang.y = node->pos[0] * yaw_per_x * YAW_FACTOR;
        ang.y *= RANDOM_FACTOR(0.05F);
        ang.y ^= HALF_TURN;
        ang.x = node->pos[1] * pitch_per_y * PITCH_FACTOR;
        ang.x *= RANDOM_FACTOR(0.05F);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y + ang.y);
        njRotateX(NULL, ang.x);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(puff_speed * RANDOM_FACTOR(0.1F)));
        njGetTranslation(NULL, &puff_base);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(fading_speed * RANDOM_FACTOR(0.2F)));
        njGetTranslation(NULL, &piece->spd);
        njPopMatrixEx();
        piece->spin.x = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.x = -piece->spin.x;
        }
        piece->spin.y = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.y = -piece->spin.y;
        }
        piece->spin.z = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.z = -piece->spin.z;
        }
        piece->timer = 0;
        piece->life = piece_life;
        piece->live = TRUE;
        piece->scaled = FALSE;
        piece->trails = FALSE;
        /* @bug logic: a fading piece's alpha falls from its throw, not from a
         * fifth of its life. the Sint32 field fade_start cuts
         * piece_fade_start's 0.2 to 0, at 0x1aaa of the object's code. the
         * shipped field is an integer too, since the draw reads it as one at
         * 0x2c80 of the object's code */
        piece->fade_start = piece_fade_start;
        if (object == &chip_glass_object || object == &chip_door_object) {
            piece->color = CHIP_TINT;
        } else {
            piece->color = COLOR_NONE;
        }
        piece->model = node->model;
        piece->texlist = texlist;
        njPopMatrixEx();
        /* @hack spelling: puff_pos, a copy of the piece's pos, where piece->pos
         * passed in place would do */
        puff_pos = piece->pos;
        puff_spd.x = puff_base.x * RANDOM_FACTOR(0.1F);
        puff_spd.y = puff_base.y * RANDOM_FACTOR(0.1F);
        puff_spd.z = puff_base.z * RANDOM_FACTOR(0.1F);
        radius = puff_radius * RANDOM_FACTOR(0.1F);
        scale = puff_scale * RANDOM_FACTOR(0.2F);
        if (object == &chip_glass_object || object == &chip_door_object ||
            CHANCE(0.5F)) {
            dust_behind(&puff_pos, &puff_spd, radius, scale);
        }
        /* @hack spelling: the count and its test as two statements, where
         * ++ in the test would do */
        LIVE_PIECES(child)++;
        if (LIVE_PIECES(child) >= PIECES) {
            break;
        }
    }
}

/* throw pieces as goal_door_throw_fading does, but solid: they fly at
 * solid_speed and don't fade. each raises its puff half the time */
static void goal_door_throw_solid(struct task *task, NJS_CNK_OBJECT *object,
                                  NJS_TEXLIST *texlist)
{
    struct task_work *work;
    NJS_CNK_OBJECT *node;
    struct task *child;
    struct goal_door_piece *piece;
    struct goal_door_piece *cur;
    Float radius;
    Float scale;
    /* @hack type: ang, a struct angles for the turn and the tilt, where two
     * Angles would do */
    struct angles ang;
    NJS_VECTOR puff_base;
    NJS_VECTOR puff_spd;
    NJS_POINT3 puff_pos;
    NJS_POINT3 off;
    int i;

    work = task->work;
    node = object->child;
    /* @hack spelling: child assigned inside the test, where a statement of
     * its own would do */
    if (!(child = task_create_child(TASK_WORK, goal_door_pieces_step, task))) {
        return;
    }
    /* @hack spelling: the pieces assigned to extra inside the test, where a
     * statement of its own would do */
    if (!(child->extra.p = syCalloc(PIECES, sizeof(struct goal_door_piece)))) {
        task_kill(child);
        return;
    }
    child->draw = goal_door_pieces_draw;
    child->destructor = goal_door_pieces_destroy;
    LIVE_PIECES(child) = 0;
    piece = TASK_PIECES(child);
    /* @hack spelling: the steps written i = i + 1 and piece = piece + 1, where
     * ++ would do */
    for (i = 0; i < PIECES; i = i + 1) {
        /* @hack spelling: cur, a copy of piece, where piece read in place would
         * do */
        cur = piece;
        cur->live = FALSE;
        piece = piece + 1;
    }
    piece = TASK_PIECES(child);
    for (; node; node = node->sibling, piece++) {
        point_turn_y(node->pos[0], node->pos[2], work->ang.y, &off);
        piece->pos.x = work->pos.x + off.x;
        piece->pos.y = work->pos.y + node->pos[1];
        piece->pos.z = work->pos.z + off.z;
        piece->ang.x = node->ang[0];
        piece->ang.y = work->ang.y + node->ang[1];
        piece->ang.z = node->ang[2];
        ang.y = node->pos[0] * yaw_per_x * YAW_FACTOR;
        ang.y *= RANDOM_FACTOR(0.05F);
        ang.y ^= HALF_TURN;
        ang.x = node->pos[1] * pitch_per_y * PITCH_FACTOR;
        ang.x *= RANDOM_FACTOR(0.05F);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y + ang.y);
        njRotateX(NULL, ang.x);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(puff_speed * RANDOM_FACTOR(0.1F)));
        njGetTranslation(NULL, &puff_base);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(solid_speed * RANDOM_FACTOR(0.2F)));
        njGetTranslation(NULL, &piece->spd);
        njPopMatrixEx();
        piece->spin.x = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.x = -piece->spin.x;
        }
        piece->spin.y = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.y = -piece->spin.y;
        }
        piece->spin.z = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            piece->spin.z = -piece->spin.z;
        }
        piece->timer = 0;
        piece->life = piece_life;
        piece->live = TRUE;
        piece->scaled = FALSE;
        piece->trails = FALSE;
        piece->fade_start = NO_FADE;
        piece->color = COLOR_NONE;
        piece->model = node->model;
        piece->texlist = texlist;
        njPopMatrixEx();
        /* @hack spelling: puff_pos, a copy of the piece's pos, where piece->pos
         * passed in place would do */
        puff_pos = piece->pos;
        puff_spd.x = puff_base.x * RANDOM_FACTOR(0.1F);
        puff_spd.y = puff_base.y * RANDOM_FACTOR(0.1F);
        puff_spd.z = puff_base.z * RANDOM_FACTOR(0.1F);
        radius = puff_radius * RANDOM_FACTOR(0.1F);
        scale = puff_scale * RANDOM_FACTOR(0.2F);
        if (CHANCE(0.5F)) {
            dust_behind(&puff_pos, &puff_spd, radius, scale);
        }
        /* @hack spelling: the count and its test as two statements, where
         * ++ in the test would do */
        LIVE_PIECES(child)++;
        if (LIVE_PIECES(child) >= PIECES) {
            break;
        }
    }
}

/* throw SCRAPS solid pieces of model, each turned at random */
static void goal_door_throw_scrap(struct task *task, NJS_CNK_MODEL *model,
                                  NJS_TEXLIST *texlist)
{
    struct task_work *work;
    struct task *child;
    struct goal_door_piece *piece;
    struct goal_door_piece *cur;
    int i;
    Float radius;
    Float scale;
    /* @hack type: ang, a struct angles for the turn and the tilt, where two
     * Angles would do */
    struct angles ang;
    NJS_VECTOR puff_base;
    NJS_VECTOR puff_spd;
    NJS_POINT3 puff_pos;
    NJS_POINT3 off;
    NJS_POINT3 at;
    Float roll;

    work = task->work;
    /* @hack spelling: child assigned inside the test, where a statement of
     * its own would do */
    if (!(child = task_create_child(TASK_WORK, goal_door_pieces_step, task))) {
        return;
    }
    /* @hack spelling: the pieces assigned to extra inside the test, where a
     * statement of its own would do */
    if (!(child->extra.p = syCalloc(PIECES, sizeof(struct goal_door_piece)))) {
        task_kill(child);
        return;
    }
    child->draw = goal_door_pieces_draw;
    child->destructor = goal_door_pieces_destroy;
    LIVE_PIECES(child) = 0;
    piece = TASK_PIECES(child);
    /* @hack spelling: the steps written i = i + 1 and piece = piece + 1, where
     * ++ would do */
    for (i = 0; i < PIECES; i = i + 1) {
        /* @hack spelling: cur, a copy of piece, where piece read in place would
         * do */
        cur = piece;
        cur->live = FALSE;
        piece = piece + 1;
    }
    /* @hack spelling: piece = piece + 1, where piece++ would do */
    for (i = 0, piece = TASK_PIECES(child); i < SCRAPS;
         i++, piece = piece + 1) {
        /* @hack spelling: cur, a copy of piece, where piece read in place would
         * do */
        cur = piece;
        at.x = RANDOM_FROM(-SCRAP_HALF_WIDTH, 2.0F * SCRAP_HALF_WIDTH);
        at.y = RANDOM_FROM(SCRAP_LOW, SCRAP_HIGH - SCRAP_LOW);
        at.z = 0.0F;
        point_turn_y(at.x, at.z, work->ang.y, &off);
        cur->pos.x = work->pos.x + off.x;
        cur->pos.y = work->pos.y + at.y;
        cur->pos.z = work->pos.z + off.z;
        cur->ang.x = RANDOM_TURN();
        cur->ang.y = (Float)work->ang.y + RANDOM_TURN();
        cur->ang.z = RANDOM_TURN();
        ang.y = at.x * yaw_per_x * YAW_FACTOR;
        ang.y *= RANDOM_FACTOR(0.05F);
        ang.y ^= HALF_TURN;
        ang.x = at.y * pitch_per_y * PITCH_FACTOR;
        ang.x *= RANDOM_FACTOR(0.05F);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y + ang.y);
        njRotateX(NULL, ang.x);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(puff_speed * RANDOM_FACTOR(0.1F)));
        njGetTranslation(NULL, &puff_base);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, -(scrap_speed * RANDOM_FACTOR(0.2F)));
        njGetTranslation(NULL, &cur->spd);
        njPopMatrixEx();
        cur->spin.x = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            cur->spin.x = -cur->spin.x;
        }
        cur->spin.y = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            cur->spin.y = -cur->spin.y;
        }
        cur->spin.z = (Float)piece_spin * RANDOM_FACTOR(0.1F);
        if (CHANCE(0.5F)) {
            cur->spin.z = -cur->spin.z;
        }
        roll = (Float)rand() * RAND_SCALE;
        if (roll < SMALL_CHANCE) {
            cur->scale.x = RANDOM_FACTOR(0.25F) * SMALL_SCALE;
            cur->scale.y = RANDOM_FACTOR(0.25F) * SMALL_SCALE;
            cur->scale.z = RANDOM_FACTOR(0.25F) * SMALL_SCALE;
            cur->scaled = TRUE;
        } else if (roll < SMALL_CHANCE + MIDDLE_CHANCE) {
            cur->scale.x = RANDOM_FACTOR(0.25F) * MIDDLE_SCALE;
            cur->scale.y = RANDOM_FACTOR(0.25F) * MIDDLE_SCALE;
            cur->scale.z = RANDOM_FACTOR(0.25F) * MIDDLE_SCALE;
            cur->scaled = TRUE;
        } else {
            cur->scaled = FALSE;
        }
        cur->timer = 0;
        cur->life = piece_life;
        cur->live = TRUE;
        cur->trails = FALSE;
        cur->fade_start = NO_FADE;
        cur->color = COLOR_NONE;
        cur->model = model;
        cur->texlist = texlist;
        njPopMatrixEx();
        /* @hack spelling: puff_pos, a copy of the scrap's pos, where cur->pos
         * passed in place would do */
        puff_pos = cur->pos;
        puff_spd.x = puff_base.x * RANDOM_FACTOR(0.1F);
        puff_spd.y = puff_base.y * RANDOM_FACTOR(0.1F);
        puff_spd.z = puff_base.z * RANDOM_FACTOR(0.1F);
        radius = puff_radius * RANDOM_FACTOR(0.1F);
        scale = puff_scale * RANDOM_FACTOR(0.2F);
        if (CHANCE(0.5F)) {
            dust_behind(&puff_pos, &puff_spd, radius, scale);
        }
        /* @hack spelling: the count written x = x + 1, where ++ would do */
        LIVE_PIECES(child) = LIVE_PIECES(child) + 1;
        /* @hack redundant: a test for a full task, which SCRAPS pieces never
         * fill */
        if (LIVE_PIECES(child) >= PIECES) {
            break;
        }
    }
}

static void goal_door_pieces_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void goal_door_pieces_step(struct task *task)
{
    struct goal_door_piece *pieces;
    struct goal_door_piece *piece;
    int i;

    for (i = 0, pieces = TASK_PIECES(task); i < PIECES; i++, pieces++) {
        /* @hack spelling: piece, a copy of pieces, where pieces read in place
         * would do */
        piece = pieces;
        if (piece->live) {
            piece->pos.x += piece->spd.x;
            piece->pos.y += piece->spd.y;
            piece->pos.z += piece->spd.z;
            piece->spd.y -= piece_gravity;
            piece->spd.x *= piece_drag;
            piece->spd.y *= piece_drag;
            piece->spd.z *= piece_drag;
            piece->ang.x += piece->spin.x;
            piece->ang.y += piece->spin.y;
            piece->ang.z += piece->spin.z;
            /* @hack spelling: the spin's scaling written x = x * spin_drag,
             * where *= would do */
            piece->spin.x = piece->spin.x * spin_drag;
            piece->spin.y = piece->spin.y * spin_drag;
            piece->spin.z = piece->spin.z * spin_drag;
            if (piece->trails && CHANCE(trail_chance)) {
                dust_behind(&piece->pos, &piece->spd, 0.0F,
                            (0.9F * trail_scale) +
                                (rand() * (0.2F * RAND_SCALE)));
            }
            if (++piece->timer >= piece->life) {
                piece->live = FALSE;
                LIVE_PIECES(task)--;
            }
        }
    }
    if (LIVE_PIECES(task) <= 0) {
        task_kill(task);
    }
}

/* every piece draws with the first piece's texlist, since a throw gives all
 * its pieces one texlist */
static void goal_door_pieces_draw(struct task *task)
{
    struct goal_door_piece *pieces;
    struct goal_door_piece *piece;
    int i;
    Float age;
    Float fade;
    Uint8 alpha;
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint32 color;

    /* @hack spelling: the first piece read through TASK_PIECES in place,
     * where pieces set before the loop would do */
    njSetTexture(TASK_PIECES(task)->texlist);
    /* @hack spelling: pieces = pieces + 1, where pieces++ would do */
    for (i = 0, pieces = TASK_PIECES(task); i < PIECES;
         i++, pieces = pieces + 1) {
        /* @hack spelling: piece, a copy of pieces, where pieces read in place
         * in the draw would do */
        piece = pieces;
#ifdef NONMATCHING
        /* GCC can't see that color is set wherever it is read. the draw reads
         * it under the test of fade_start that sets it */
        color = 0;
#endif
        if (piece->live) {
            if (piece->fade_start != NO_FADE) {
                age = (Float)piece->timer / (Float)piece->life;
                fade = 1.0F - age;
                if (piece->fade_start > age) {
                    alpha = CHANNEL_FULL;
                } else {
                    alpha = fade / (1.0F - piece->fade_start) * CHANNEL_FULL;
                }
                red = ((piece->color >> 16) & 0xff) * fade;
                green = ((piece->color >> 8) & 0xff) * fade;
                blue = (piece->color & 0xff) * fade;
                color = ((Uint32)alpha << 24) | ((Uint32)red << 16) |
                        ((Uint32)green << 8) | (Uint32)blue;
            }
            njPushMatrixEx();
            njTranslateEx(&piece->pos);
            njRotateXYZ(NULL, piece->ang.x, piece->ang.y, piece->ang.z);
            if (piece->scaled) {
                if (piece->fade_start != NO_FADE) {
                    draw_control_save();
                    draw_attr_save();
                    draw_control_set(FADE_CONTROL);
                    njSetConstantAttr(CHUNK_TYPE_BITS, FADE_ATTR);
                    model_draw_tinted_scaled_v(piece->model, color,
                                               &piece->scale);
                    draw_attr_restore();
                    draw_control_restore();
                } else {
                    model_draw_scaled_v(piece->model, &piece->scale);
                }
            } else {
                if (piece->fade_start != NO_FADE) {
                    draw_control_save();
                    draw_attr_save();
                    draw_control_set(FADE_CONTROL);
                    njSetConstantAttr(CHUNK_TYPE_BITS, FADE_ATTR);
                    model_draw_tinted(piece->model, color);
                    draw_attr_restore();
                    draw_control_restore();
                } else {
                    njCnkEasyDrawModel(piece->model);
                }
            }
            njPopMatrixEx();
        }
    }
}

/* each child's model draws at the current matrix. the children's own
 * transforms, and their children, don't apply */
static void goal_door_draw_children_tinted(NJS_CNK_OBJECT *object, Uint32 color)
{
    NJS_CNK_OBJECT *child;

    for (child = object->child; child; child = child->sibling) {
        model_draw_tinted(child->model, color);
    }
}
