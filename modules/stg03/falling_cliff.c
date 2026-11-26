/* the falling cliff, GAKE in the stage's object list. its six pieces fall
 * away once a player comes near, each after a delay of its own */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/modifier_volume.h"
#include "sound/sound.h"
#include "stg03/falling_cliff.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#define PIECES 6
/* piece n sets bit n of the cliff's submode as it lands. the pads stop
 * shaking once the submode is ALL_DOWN */
#define ALL_DOWN ((1 << PIECES) - 1)

/* the cliff's work's mode: MODE_WAIT until a player comes near, then
 * MODE_FALL. in MODE_FALL, the work's timer counts the steps the cliff runs */
enum falling_cliff_mode { MODE_WAIT, MODE_FALL };

/* the squared range placement_try_leave_by takes, 5000.001 squared */
#define LEAVE_RANGE 25000010.0F
/* the placement's state the cliff sets. the cliff ends once it is 0 */
#define STATE_LIVE 1
/* a player within |scl.x| of the point TRIGGER_HEIGHT above the cliff's pos
 * wakes it */
#define TRIGGER_HEIGHT 520.0F
/* while THROUGH_STOP isn't 0, the cliff and its pieces run through a time
 * stop. task_create sets it to 0, and the cliff sets it to 0 again as it
 * wakes */
#define THROUGH_STOP(task) ((task)->unk_3c.l)
/* once awake, the cliff shakes the pads of the players near it. it shakes
 * them whenever game_state.steps is a multiple of RUMBLE_STEPS */
#define RUMBLE_STEPS 13
#define RUMBLE_POWER 1
#define RUMBLE_FREQ 15
#define RUMBLE_TIME 1
/* a player is near within RUMBLE_REACH of the cliff across the ground. its x
 * must also lie between RUMBLE_X_MIN and RUMBLE_X_MAX */
#define RUMBLE_REACH 1000.0F
#define RUMBLE_X_MIN 9900.0F
#define RUMBLE_X_MAX 10790.0F

/* a piece shakes for SHAKE_STEPS steps, then falls. the shake moves it along
 * y by SHAKE_SIZE times a sine whose angle gains SHAKE_TURN a step. a falling
 * piece shakes by FALL_SHAKE_SIZE times a sine whose angle gains
 * FALL_SHAKE_TURN a step */
#define SHAKE_STEPS 90
#define SHAKE_SIZE 4.0F
#define SHAKE_TURN (40.0F * ANGLE_PER_DEGREE)
#define FALL_SHAKE_SIZE 0.18F
#define FALL_SHAKE_TURN (48.0F * ANGLE_PER_DEGREE)
/* the shake fades as the sine of FADE_FROM degrees plus FADE_TURN times the
 * shake's share of SHAKE_STEPS. it runs out at a share of 1 / FADE_RATE, two
 * thirds, where the sine is 0 */
#define FADE_RATE 1.5F
#define FADE_FROM 90.0F
#define FADE_TURN 135.0F
/* a falling piece's speed gains FALL_GAIN of its top_speed a step, up to
 * top_speed itself */
#define FALL_GAIN 0.003F
/* a piece reads points from its drawn model's vertex list. it sheds dust and
 * dirt at those that lie at DUST_HEIGHT or higher. each puff starts DUST_DROP
 * below its point. a shaking piece sheds dust at every such point once, on
 * its first step. a falling piece sheds dust at each by a chance of
 * DUST_CHANCE a step. where it doesn't, it sheds dirt there by a chance of
 * DIRT_CHANCE */
#define DUST_HEIGHT 500.0F
#define DUST_DROP 10.0F
#define DUST_CHANCE 0.01F
#define DIRT_CHANCE 0.06F
/* dust moves up to SHAKE_DUST_SPREAD along x, y and z from a shaking piece.
 * from a falling piece it moves up to FALL_DUST_SPREAD. it moves with the
 * piece along y too, and a shaking piece's dust SHAKE_DUST_LIFT faster still.
 * dirt moves with the piece along y, and within half DIRT_SPREAD either way
 * along x and z. a puff's size is its kind's _SIZE, plus up to the kind's
 * _SIZE_RANDOM */
#define SHAKE_DUST_SPREAD 0.1F
#define SHAKE_DUST_LIFT 0.02F
#define SHAKE_DUST_SIZE 18.0F
#define SHAKE_DUST_SIZE_RANDOM 3.0F
#define FALL_DUST_SPREAD 0.01F
#define FALL_DUST_SIZE 6.0F
#define FALL_DUST_SIZE_RANDOM 0.4F
#define DIRT_SPREAD 1.5F
#define DIRT_SIZE 1.5F
#define DIRT_SIZE_RANDOM 2.0F
/* a piece's sounds play SOUND_HEIGHT above its drawn object's origin. the
 * shake sound plays on the shake's SHAKE_SOUND_STEP-th step */
#define SOUND_HEIGHT 500.0F
#define SHAKE_SOUND 5
#define SHAKE_SOUND_STEP 2
#define SHAKE_VOLUME 50
#define FALL_SOUND 8
#define FALL_SOUND_PRIORITY 1
#define FALL_SOUND_DURATION 30
/* a piece's work keeps WORLD_ADDED only while a player lies within
 * REACH_MARGIN of its collision model's sphere */
#define REACH_MARGIN 32.0F
/* a piece draws a shadow with each child of its drawn object that has the
 * shadow's radius. that is a model radius within SHADOW_RADIUS_RANGE of
 * SHADOW_RADIUS */
#define SHADOW_RADIUS 120.0F
#define SHADOW_RADIUS_RANGE 5.0F
/* the matrices a piece's draw pushes: the piece's, and its children's */
#define DRAW_PUSHES 2

/* a cliff whose ang.x has SHORT_FALLS gives its pieces short, slow falls */
#define SHORT_FALLS 0x1

/* the index of the piece a task runs, in its work's tick */
#define PIECE(work) ((work)->tick)
#define FORCES(task) ((struct task_force *)(task)->force.p)

/* a piece's fall. start and end are offsets along y from the cliff's pos */
struct falling_cliff_fall {
    Float start;
    Float end; /* where it lands */
    Float top_speed; /* its fastest fall, a speed along y below 0 */
    Sint32 delay; /* the piece shakes once the cliff's timer passes it */
};

/* a piece's state as it shakes and falls */
struct falling_cliff_piece {
    Float offset; /* along y from the cliff's pos, before the shake */
    Float shake; /* the shake's offset along y, on top of offset */
    Float speed;
    Sint32 steps; /* the steps since the cliff's timer passed its delay */
};

/* the cliff's record, at task->extra */
struct falling_cliff {
    /* each piece's copy of its collision object, which moves as it falls */
    NJS_OBJECT copies[PIECES];
    struct falling_cliff_piece pieces[PIECES];
};

#define CLIFF(task) ((struct falling_cliff *)(task)->extra.p)

static const char cliff_two_player_label[20] = "<- 1P or<2P>mode (";
static const char cliff_one_player_label[20] = "<-<1P>or 2P mode (";
static const char cliff_start_range_label[16] = "<- START RANGE";
static const char cliff_texture_jungle128_13[16] = {
#include "stg03/falling_cliff/cliff_texture_jungle128_13.inc"
};
static const char cliff_texture_jungle128_5[12] = {
#include "stg03/falling_cliff/cliff_texture_jungle128_5.inc"
};
static const char cliff_texture_jungle256_6[12] = {
#include "stg03/falling_cliff/cliff_texture_jungle256_6.inc"
};
static const char cliff_texture_miu128_green007[16] = {
#include "stg03/falling_cliff/cliff_texture_miu128_green007.inc"
};
static const char cliff_texture_miu64_green001[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green001.inc"
};
static const char cliff_texture_miu64_green002[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green002.inc"
};
static const char cliff_texture_miu64_green003[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green003.inc"
};
static const char cliff_texture_miu64_green004[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green004.inc"
};
static const char cliff_texture_miu64_green006[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green006.inc"
};
static const char cliff_texture_miu64_kage001[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_kage001.inc"
};
static const char cliff_texture_jungle64_1[12] = {
#include "stg03/falling_cliff/cliff_texture_jungle64_1.inc"
};
static const char cliff_texture_miu128_green005[16] = {
#include "stg03/falling_cliff/cliff_texture_miu128_green005.inc"
};
static const char cliff_texture_miu64_green005[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green005.inc"
};
static const char cliff_texture_miu64_green007[16] = {
#include "stg03/falling_cliff/cliff_texture_miu64_green007.inc"
};
static const char cliff_texture_miu128_green001[16] = {
#include "stg03/falling_cliff/cliff_texture_miu128_green001.inc"
};
static const char cliff_texture_miu128_green009[16] = {
#include "stg03/falling_cliff/cliff_texture_miu128_green009.inc"
};

/* The six collision meshes move with the falling platforms. */
static NJS_MATERIAL falling_cliff_collision_0_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_0_materials.inc"
};

static Sint16 falling_cliff_collision_0_indices_0[72] = {
#include "stg03/falling_cliff/falling_cliff_collision_0_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_0_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_0_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_0_points[31] = {
#include "stg03/falling_cliff/falling_cliff_collision_0_points.inc"
};

static NJS_VECTOR falling_cliff_collision_0_normals[31] = {
#include "stg03/falling_cliff/falling_cliff_collision_0_normals.inc"
};

static NJS_MODEL falling_cliff_collision_0_model = {
#include "stg03/falling_cliff/falling_cliff_collision_0_model.inc"
};

static NJS_OBJECT falling_cliff_collision_0 = {
#include "stg03/falling_cliff/falling_cliff_collision_0.inc"
};

static NJS_MATERIAL falling_cliff_collision_1_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_1_materials.inc"
};

static Sint16 falling_cliff_collision_1_indices_0[53] = {
#include "stg03/falling_cliff/falling_cliff_collision_1_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_1_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_1_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_1_points[24] = {
#include "stg03/falling_cliff/falling_cliff_collision_1_points.inc"
};

static NJS_VECTOR falling_cliff_collision_1_normals[24] = {
#include "stg03/falling_cliff/falling_cliff_collision_1_normals.inc"
};

static NJS_MODEL falling_cliff_collision_1_model = {
#include "stg03/falling_cliff/falling_cliff_collision_1_model.inc"
};

static NJS_OBJECT falling_cliff_collision_1 = {
#include "stg03/falling_cliff/falling_cliff_collision_1.inc"
};

static NJS_MATERIAL falling_cliff_collision_2_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_2_materials.inc"
};

static Sint16 falling_cliff_collision_2_indices_0[45] = {
#include "stg03/falling_cliff/falling_cliff_collision_2_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_2_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_2_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_2_points[23] = {
#include "stg03/falling_cliff/falling_cliff_collision_2_points.inc"
};

static NJS_VECTOR falling_cliff_collision_2_normals[23] = {
#include "stg03/falling_cliff/falling_cliff_collision_2_normals.inc"
};

static NJS_MODEL falling_cliff_collision_2_model = {
#include "stg03/falling_cliff/falling_cliff_collision_2_model.inc"
};

static NJS_OBJECT falling_cliff_collision_2 = {
#include "stg03/falling_cliff/falling_cliff_collision_2.inc"
};

static NJS_MATERIAL falling_cliff_collision_3_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_3_materials.inc"
};

static Sint16 falling_cliff_collision_3_indices_0[41] = {
#include "stg03/falling_cliff/falling_cliff_collision_3_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_3_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_3_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_3_points[18] = {
#include "stg03/falling_cliff/falling_cliff_collision_3_points.inc"
};

static NJS_VECTOR falling_cliff_collision_3_normals[18] = {
#include "stg03/falling_cliff/falling_cliff_collision_3_normals.inc"
};

static NJS_MODEL falling_cliff_collision_3_model = {
#include "stg03/falling_cliff/falling_cliff_collision_3_model.inc"
};

static NJS_OBJECT falling_cliff_collision_3 = {
#include "stg03/falling_cliff/falling_cliff_collision_3.inc"
};

static NJS_MATERIAL falling_cliff_collision_4_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_4_materials.inc"
};

static Sint16 falling_cliff_collision_4_indices_0[65] = {
#include "stg03/falling_cliff/falling_cliff_collision_4_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_4_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_4_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_4_points[29] = {
#include "stg03/falling_cliff/falling_cliff_collision_4_points.inc"
};

static NJS_VECTOR falling_cliff_collision_4_normals[29] = {
#include "stg03/falling_cliff/falling_cliff_collision_4_normals.inc"
};

static NJS_MODEL falling_cliff_collision_4_model = {
#include "stg03/falling_cliff/falling_cliff_collision_4_model.inc"
};

static NJS_OBJECT falling_cliff_collision_4 = {
#include "stg03/falling_cliff/falling_cliff_collision_4.inc"
};

static NJS_MATERIAL falling_cliff_collision_5_materials[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_5_materials.inc"
};

static Sint16 falling_cliff_collision_5_indices_0[91] = {
#include "stg03/falling_cliff/falling_cliff_collision_5_indices_0.inc"
};

static NJS_MESHSET falling_cliff_collision_5_meshsets[1] = {
#include "stg03/falling_cliff/falling_cliff_collision_5_meshsets.inc"
};

static NJS_POINT3 falling_cliff_collision_5_points[34] = {
#include "stg03/falling_cliff/falling_cliff_collision_5_points.inc"
};

static NJS_VECTOR falling_cliff_collision_5_normals[34] = {
#include "stg03/falling_cliff/falling_cliff_collision_5_normals.inc"
};

static NJS_MODEL falling_cliff_collision_5_model = {
#include "stg03/falling_cliff/falling_cliff_collision_5_model.inc"
};

static NJS_OBJECT falling_cliff_collision_5 = {
#include "stg03/falling_cliff/falling_cliff_collision_5.inc"
};

/* The display hierarchy and texture slots for cliff piece 0. */

static NJS_TEXNAME falling_cliff_textures_0[10] = {
#include "stg03/falling_cliff/falling_cliff_textures_0.inc"
};

static NJS_TEXLIST falling_cliff_texlist_0 = {
#include "stg03/falling_cliff/falling_cliff_texlist_0.inc"
};

static Sint16 falling_cliff_object_0_node_4_polygons[315] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_4_polygons.inc"
};

static Sint32 falling_cliff_object_0_node_4_vertices[307] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_0_node_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_0_node_4 = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_4.inc"
};

static Sint16 falling_cliff_object_0_node_3_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_0_node_3_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_0_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_0_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_3.inc"
};

static Sint16 falling_cliff_object_0_node_2_polygons[469] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_0_node_2_vertices[383] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_0_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_0_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_2.inc"
};

static Sint16 falling_cliff_object_0_node_1_polygons[429] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_0_node_1_vertices[395] = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_0_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_0_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_0_node_1.inc"
};

static Sint16 falling_cliff_object_0_polygons[237] = {
#include "stg03/falling_cliff/falling_cliff_object_0_polygons.inc"
};

static Sint32 falling_cliff_object_0_vertices[123] = {
#include "stg03/falling_cliff/falling_cliff_object_0_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_0_model = {
#include "stg03/falling_cliff/falling_cliff_object_0_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_0 = {
#include "stg03/falling_cliff/falling_cliff_object_0.inc"
};

/* The display hierarchy and texture slots for cliff piece 1. */

static NJS_TEXNAME falling_cliff_textures_1[11] = {
#include "stg03/falling_cliff/falling_cliff_textures_1.inc"
};

static NJS_TEXLIST falling_cliff_texlist_1 = {
#include "stg03/falling_cliff/falling_cliff_texlist_1.inc"
};

static Sint16 falling_cliff_object_1_node_5_polygons[317] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_5_polygons.inc"
};

static Sint32 falling_cliff_object_1_node_5_vertices[267] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_5_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_node_5_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_5_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1_node_5 = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_5.inc"
};

static Sint16 falling_cliff_object_1_node_4_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_4_polygons.inc"
};

static Sint32 falling_cliff_object_1_node_4_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_node_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1_node_4 = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_4.inc"
};

static Sint16 falling_cliff_object_1_node_3_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_1_node_3_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_3.inc"
};

static Sint16 falling_cliff_object_1_node_2_polygons[315] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_1_node_2_vertices[307] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_2.inc"
};

static Sint16 falling_cliff_object_1_node_1_polygons[247] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_1_node_1_vertices[179] = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_1_node_1.inc"
};

static Sint16 falling_cliff_object_1_polygons[227] = {
#include "stg03/falling_cliff/falling_cliff_object_1_polygons.inc"
};

static Sint32 falling_cliff_object_1_vertices[131] = {
#include "stg03/falling_cliff/falling_cliff_object_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_1 = {
#include "stg03/falling_cliff/falling_cliff_object_1.inc"
};

/* The display hierarchy and texture slots for cliff piece 2. */

static NJS_TEXNAME falling_cliff_textures_2[9] = {
#include "stg03/falling_cliff/falling_cliff_textures_2.inc"
};

static NJS_TEXLIST falling_cliff_texlist_2 = {
#include "stg03/falling_cliff/falling_cliff_texlist_2.inc"
};

static Sint16 falling_cliff_object_2_node_3_polygons[317] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_2_node_3_vertices[267] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_2_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_2_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_3.inc"
};

static Sint16 falling_cliff_object_2_node_2_polygons[265] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_2_node_2_vertices[243] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_2_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_2_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_2.inc"
};

static Sint16 falling_cliff_object_2_node_1_polygons[247] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_2_node_1_vertices[179] = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_2_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_2_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_2_node_1.inc"
};

static Sint16 falling_cliff_object_2_polygons[193] = {
#include "stg03/falling_cliff/falling_cliff_object_2_polygons.inc"
};

static Sint32 falling_cliff_object_2_vertices[123] = {
#include "stg03/falling_cliff/falling_cliff_object_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_2 = {
#include "stg03/falling_cliff/falling_cliff_object_2.inc"
};

/* The display hierarchy and texture slots for cliff piece 3. */

static NJS_TEXNAME falling_cliff_textures_3[9] = {
#include "stg03/falling_cliff/falling_cliff_textures_3.inc"
};

static NJS_TEXLIST falling_cliff_texlist_3 = {
#include "stg03/falling_cliff/falling_cliff_texlist_3.inc"
};

static Sint16 falling_cliff_object_3_node_4_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_4_polygons.inc"
};

static Sint32 falling_cliff_object_3_node_4_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_3_node_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_3_node_4 = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_4.inc"
};

static Sint16 falling_cliff_object_3_node_3_polygons[315] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_3_node_3_vertices[307] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_3_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_3_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_3.inc"
};

static Sint16 falling_cliff_object_3_node_2_polygons[317] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_3_node_2_vertices[267] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_3_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_3_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_2.inc"
};

static Sint16 falling_cliff_object_3_node_1_polygons[429] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_3_node_1_vertices[395] = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_3_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_3_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_3_node_1.inc"
};

static Sint16 falling_cliff_object_3_polygons[227] = {
#include "stg03/falling_cliff/falling_cliff_object_3_polygons.inc"
};

static Sint32 falling_cliff_object_3_vertices[135] = {
#include "stg03/falling_cliff/falling_cliff_object_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_3 = {
#include "stg03/falling_cliff/falling_cliff_object_3.inc"
};

/* The display hierarchy and texture slots for cliff piece 4. */

static NJS_TEXNAME falling_cliff_textures_4[11] = {
#include "stg03/falling_cliff/falling_cliff_textures_4.inc"
};

static NJS_TEXLIST falling_cliff_texlist_4 = {
#include "stg03/falling_cliff/falling_cliff_texlist_4.inc"
};

static Sint16 falling_cliff_object_4_node_4_polygons[429] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_4_polygons.inc"
};

static Sint32 falling_cliff_object_4_node_4_vertices[395] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_4_node_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_4_node_4 = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_4.inc"
};

static Sint16 falling_cliff_object_4_node_3_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_4_node_3_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_4_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_4_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_3.inc"
};

static Sint16 falling_cliff_object_4_node_2_polygons[317] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_4_node_2_vertices[267] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_4_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_4_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_2.inc"
};

static Sint16 falling_cliff_object_4_node_1_polygons[487] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_4_node_1_vertices[395] = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_4_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_4_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_4_node_1.inc"
};

static Sint16 falling_cliff_object_4_polygons[281] = {
#include "stg03/falling_cliff/falling_cliff_object_4_polygons.inc"
};

static Sint32 falling_cliff_object_4_vertices[175] = {
#include "stg03/falling_cliff/falling_cliff_object_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_4 = {
#include "stg03/falling_cliff/falling_cliff_object_4.inc"
};

/* The display hierarchy and texture slots for cliff piece 5. */

static NJS_TEXNAME falling_cliff_textures_5[9] = {
#include "stg03/falling_cliff/falling_cliff_textures_5.inc"
};

static NJS_TEXLIST falling_cliff_texlist_5 = {
#include "stg03/falling_cliff/falling_cliff_texlist_5.inc"
};

static Sint16 falling_cliff_object_5_node_4_polygons[315] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_4_polygons.inc"
};

static Sint32 falling_cliff_object_5_node_4_vertices[307] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_4_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_5_node_4_model = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_4_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_5_node_4 = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_4.inc"
};

static Sint16 falling_cliff_object_5_node_3_polygons[317] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_3_polygons.inc"
};

static Sint32 falling_cliff_object_5_node_3_vertices[267] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_3_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_5_node_3_model = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_3_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_5_node_3 = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_3.inc"
};

static Sint16 falling_cliff_object_5_node_2_polygons[425] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_2_polygons.inc"
};

static Sint32 falling_cliff_object_5_node_2_vertices[295] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_2_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_5_node_2_model = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_2_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_5_node_2 = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_2.inc"
};

static Sint16 falling_cliff_object_5_node_1_polygons[429] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_1_polygons.inc"
};

static Sint32 falling_cliff_object_5_node_1_vertices[395] = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_1_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_5_node_1_model = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_1_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_5_node_1 = {
#include "stg03/falling_cliff/falling_cliff_object_5_node_1.inc"
};

static Sint16 falling_cliff_object_5_polygons[605] = {
#include "stg03/falling_cliff/falling_cliff_object_5_polygons.inc"
};

static Sint32 falling_cliff_object_5_vertices[371] = {
#include "stg03/falling_cliff/falling_cliff_object_5_vertices.inc"
};

static NJS_CNK_MODEL falling_cliff_object_5_model = {
#include "stg03/falling_cliff/falling_cliff_object_5_model.inc"
};

static NJS_CNK_OBJECT falling_cliff_object_5 = {
#include "stg03/falling_cliff/falling_cliff_object_5.inc"
};

/* the pieces' falls, by piece. a cliff whose ang.x has SHORT_FALLS takes
 * falls_short. any other takes falls_hd under MISSION_HD, and falls
 * otherwise */
/* clang-format off */
static struct falling_cliff_fall falls[PIECES] = {
    {0.0F, -1980.0F,   -0.9F,   0},
    {0.0F, -1810.0F,  -0.93F,  32},
    {0.0F, -1620.0F,  -0.92F,  48},
    {0.0F, -1270.0F,   -0.9F,  64},
    {0.0F,  -400.0F,   -1.0F, 128},
    {0.0F,  -340.0F,  -0.81F, 176},
};
static struct falling_cliff_fall falls_hd[PIECES] = {
    {0.0F, -1980.0F,  -1.75F,   0},
    {0.0F, -1810.0F,  -1.15F,  22},
    {0.0F, -1620.0F,  -1.08F,  48},
    {0.0F, -1270.0F,   -1.0F,  64},
    {0.0F,  -400.0F,   -1.1F, 128},
    {0.0F,  -340.0F,  -0.91F, 176},
};
static struct falling_cliff_fall falls_short[PIECES] = {
    {0.0F,  -400.0F,   -0.3F,  60},
    {0.0F,  -370.0F,  -0.28F, 180},
    {0.0F,  -330.0F,  -0.27F, 256},
    {0.0F,  -300.0F,  -0.26F,  96},
    {0.0F,  -260.0F,  -0.29F, 400},
    {0.0F,  -180.0F, -0.275F, 512},
};
/* clang-format on */
NJS_TEXLIST *falling_cliff_texlists[7] = {
#include "stg03/falling_cliff/falling_cliff_texlists.inc"
};
KEPT static NJS_OBJECT *collisions[PIECES] = {
#include "stg03/falling_cliff/collisions.inc"
};
KEPT static NJS_CNK_OBJECT *objects[PIECES] = {
#include "stg03/falling_cliff/objects.inc"
};
static Sint32 fall_volume = 30;
/* the shadow is modifier_volume_cylinder_model, in the space of the child it
 * draws with. shadow_offset moves it there, and shadow_scale scales it */
static NJS_VECTOR shadow_offset = {15.0F, 7.5F, 0.0F};
static NJS_VECTOR shadow_scale = {4.0F, 0.5F, 5.0F};

static void falling_cliff_destroy(struct task *task);
static void falling_cliff_step(struct task *task);
static void falling_cliff_piece_init(struct task *task);
static void falling_cliff_piece_destroy(struct task *task);
static void falling_cliff_piece_step(struct task *task);
static void falling_cliff_piece_draw(struct task *task);

void falling_cliff_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *piece;
    int i;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. these name the SET editor
     * options */
    if (0) {
        (void)cliff_two_player_label;
        (void)cliff_one_player_label;
        (void)cliff_start_range_label;
    }
    if (placement_try_leave_by(task, LEAVE_RANGE)) {
        return;
    }
    task->step = falling_cliff_step;
    task->destructor = falling_cliff_destroy;
    task->extra.p = syMalloc(sizeof(struct falling_cliff));
    for (i = 0; i < PIECES; i++) {
        piece = task_create_child(TASK_WORK | TASK_FORCE,
                                  falling_cliff_piece_init, task);
        if (piece != NULL) {
            PIECE(piece->work) = i;
        }
    }
    work->timer = 0;
    work->mode = MODE_WAIT;
    if (task->placement != NULL) {
        placement_set_state(task, STATE_LIVE);
    }
}

static void falling_cliff_destroy(struct task *task)
{
    task_destroy_children(task);
    syFree(task->extra.p);
    task->extra.p = NULL;
}

/* 1 where the cliff at center shakes the pad of a player at pos, and 0
 * otherwise */
static Float falling_cliff_rumbles(NJS_POINT3 *pos, NJS_POINT3 *center)
{
    Float dx = pos->x - center->x;
    Float dz = pos->z - center->z;
    Float distance;

    distance = njSqrt((dx * dx) + (dz * dz));
    if (RUMBLE_REACH > distance && pos->x > RUMBLE_X_MIN &&
        pos->x < RUMBLE_X_MAX) {
        return 1.0F;
    }
    return 0.0F;
}

static void falling_cliff_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    NJS_POINT3 trigger;
    /* @hack redundant: unused, bytes nothing reads, which only size the
     * frame */
    Uint8 unused[24];

    if (placement_try_leave_by(task, LEAVE_RANGE)) {
        return;
    }
    /* @hack redundant: a loop that never runs, around a store of
     * TRIGGER_HEIGHT */
    while (0) {
        trigger.y = TRIGGER_HEIGHT;
    }
    if (task->placement != NULL && !placement_get_state(task)) {
        task_kill(task);
    }
    /* @hack spelling: the mode tested as a truth value, where a comparison
     * with MODE_WAIT would do */
    if (work->mode) {
        if (!THROUGH_STOP(task) && game_state.time_stopped) {
            return;
        }
        work->timer++;
        if (game_state.steps % RUMBLE_STEPS) {
            return;
        }
        if (work->submode == ALL_DOWN) {
            return;
        }
        /* @hack spelling: the assignments inside the tests, where statements
         * of their own would do */
        if ((player = player_work[0]) != NULL &&
            falling_cliff_rumbles(&player->pos, &work->pos) != 0.0F) {
            vibration_start(0, RUMBLE_POWER, RUMBLE_FREQ, RUMBLE_TIME);
        }
        if ((player = player_work[1]) != NULL &&
            falling_cliff_rumbles(&player->pos, &work->pos) != 0.0F) {
            vibration_start(1, RUMBLE_POWER, RUMBLE_FREQ, RUMBLE_TIME);
        }
    } else {
        trigger = work->pos;
        /* the trigger takes falls_short[0]'s start whatever the cliff's
         * falls, but every fall starts at 0 */
        trigger.y += falls_short[0].start + TRIGGER_HEIGHT;
        if (player_near(&trigger, _builtin_fabsf(work->scl.x))) {
            work->mode = MODE_FALL;
            THROUGH_STOP(task) = 0;
        }
    }
}

/* a piece's first step. it puts in the world a copy of the piece's collision
 * object, placed with the cliff */
static void falling_cliff_piece_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    int i;

    task->draw = falling_cliff_piece_draw;
    task->step = falling_cliff_piece_step;
    task->destructor = falling_cliff_piece_destroy;
    /* @hack redundant: a jump to the statement after it */
    goto place;
place:
    work->pos = parent->work->pos;
    work->ang = parent->work->ang;
    i = PIECE(work);
    object = &CLIFF(parent)->copies[i];
    *object = *collisions[i];
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, (NJS_POINT3 *)collisions[PIECE(work)]->pos,
                (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    object->ang[1] = work->ang.y + collisions[PIECE(work)]->ang[1];
    flags = WORLD_SOLID | WORLD_MOVING;
    /* @hack spelling: the first test's body left empty, where the other tests
     * nested under its negation would do */
    if (object->model->r > WORLD_SMALL_SIZE_CLASS_1_MAX) {
    } else if (object->model->r > WORLD_SMALL_SIZE_CLASS_2_MAX) {
        flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(1);
    } else if (object->model->r > WORLD_SMALL_SIZE_CLASS_3_MAX) {
        flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(2);
    } else {
        flags = WORLD_SOLID | WORLD_MOVING | WORLD_SIZE_CLASS(3);
    }
    world_add(flags, task, object);
    /* @hack spelling: one conditional inside another, where an if for each
     * case would do */
    /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
    CLIFF(parent)->pieces[PIECE(work)].offset =
        (work->ang.x & SHORT_FALLS)        ? falls_short[PIECE(work)].start
        : game_state.mission == MISSION_HD ? falls_hd[PIECE(work)].start
                                           : falls[PIECE(work)].start;
    /* NOLINTEND(readability-avoid-nested-conditional-operator) */
    CLIFF(parent)->pieces[PIECE(work)].speed = 0.0F;
    CLIFF(parent)->pieces[PIECE(work)].steps = 0;
    CLIFF(parent)->pieces[PIECE(work)].shake = 0.0F;
}

static void falling_cliff_piece_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;

    world_remove(task, &CLIFF(parent)->copies[PIECE(work)]);
}

/* the dust a piece sheds on a step */
enum falling_cliff_dust { DUST_NONE, DUST_SHAKE, DUST_FALL };

static void falling_cliff_piece_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct falling_cliff_fall *fall;
    struct falling_cliff_piece *piece;
    NJS_OBJECT *object;
    NJS_CNK_OBJECT *shape;
    NJS_CNK_MODEL *model;
    /* @bug logic: vertex steps through the vertex list a word a vertex, at
     * 0x99e and 0xb46 of the object's code. piece 0's model holds NJD_CV_D8
     * vertices of CHUNK_D8_WORDS words. so the dust rises from points read
     * across the list's first quarter, colors among them */
    Sint32 *vertex;
    NJS_POINT3 *point;
    int dust;
    int count;
    int i;
    Float share;
    NJS_POINT3 center;
    NJS_POINT3 last;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    dust = DUST_NONE;
    /* @hack spelling: one conditional inside another, where an if for each
     * case would do */
    /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
    fall = (work->ang.x & SHORT_FALLS)        ? &falls_short[PIECE(work)]
           : game_state.mission == MISSION_HD ? &falls_hd[PIECE(work)]
                                              : &falls[PIECE(work)];
    /* NOLINTEND(readability-avoid-nested-conditional-operator) */
    piece = &CLIFF(parent)->pieces[PIECE(work)];
    if ((THROUGH_STOP(parent) || !game_state.time_stopped) &&
        parent_work->timer > fall->delay) {
        piece->steps++;
        if (piece->steps > SHAKE_STEPS) {
            piece->offset += piece->speed;
            if (fall->end > piece->offset) {
                piece->offset = fall->end;
                parent->work->submode |= 1 << PIECE(work);
            } else {
                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njTranslate(NULL, work->pos.x,
                            work->pos.y + piece->offset + piece->shake,
                            work->pos.z);
                njRotateY(NULL, work->ang.y);
                shape = objects[PIECE(work)];
                njCalcPoint(NULL, (NJS_POINT3 *)shape->pos, &center);
                njPopMatrixEx();
                center.y += SOUND_HEIGHT;
                sound_play_at_for(FALL_SOUND, work, FALL_SOUND_PRIORITY,
                                  fall_volume, FALL_SOUND_DURATION, &center);
                dust = DUST_FALL;
            }
            piece->speed += fall->top_speed * FALL_GAIN;
            if (piece->speed < fall->top_speed) {
                piece->speed = fall->top_speed;
            }
            piece->shake =
                njSin((Angle)((piece->steps - SHAKE_STEPS) * FALL_SHAKE_TURN)) *
                FALL_SHAKE_SIZE;
        } else {
            share = piece->steps * (1.0F / SHAKE_STEPS);
            dust = DUST_SHAKE;
            piece->shake =
                (share * FADE_RATE > 1.0F
                     ? 0.0F
                     : njSin((Angle)((FADE_FROM + (FADE_TURN * share)) *
                                     ANGLE_PER_DEGREE))) *
                njSin((Angle)(piece->steps * SHAKE_TURN)) * SHAKE_SIZE;
            if (piece->steps == SHAKE_SOUND_STEP) {
                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njTranslate(NULL, work->pos.x,
                            work->pos.y + piece->offset + piece->shake,
                            work->pos.z);
                njRotateY(NULL, work->ang.y);
                shape = objects[PIECE(work)];
                njCalcPoint(NULL, (NJS_POINT3 *)shape->pos, &center);
                njPopMatrixEx();
                center.y += SOUND_HEIGHT;
                sound_play_at(SHAKE_SOUND, NULL, 0, SHAKE_VOLUME, &center);
            }
        }
    }
    object = &CLIFF(parent)->copies[PIECE(work)];
    last = *(NJS_POINT3 *)object->pos;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + piece->offset + piece->shake,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, (NJS_POINT3 *)collisions[PIECE(work)]->pos,
                (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    center = object->model->center;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateY(NULL, object->ang[1]);
    njCalcPoint(NULL, &center, &center);
    /* a player riding the piece moves as its copy did */
    FORCES(task)[0].speed.y = object->pos[1] - last.y;
    FORCES(task)[1].speed.y = object->pos[1] - last.y;
    if (THROUGH_STOP(parent) || !game_state.time_stopped) {
        shape = objects[PIECE(work)];
        /* @hack spelling: the model set inside the vertex's assignment, where
         * a statement of its own would do */
        vertex = (model = shape->model)->vlist + CHUNK_HEAD_WORDS;
        count = CHUNK_VERTEX_COUNT(model->vlist);
        switch (dust) {
        case DUST_SHAKE:
            if (piece->steps == 1) {
                for (i = 0; i < count; i++, vertex++) {
                    point = (NJS_POINT3 *)vertex;
                    if (point->y >= DUST_HEIGHT) {
                        njCalcPoint(NULL, point, &pos);
                        pos.y -= DUST_DROP;
                        /* @hack redundant: the 0.0F added to spd.x and
                         * spd.z */
                        spd.x =
                            0.0F + (rand() * (SHAKE_DUST_SPREAD * RAND_SCALE));
                        spd.y = (rand() * (SHAKE_DUST_SPREAD * RAND_SCALE)) +
                                FORCES(task)->speed.y + SHAKE_DUST_LIFT;
                        spd.z =
                            0.0F + (rand() * (SHAKE_DUST_SPREAD * RAND_SCALE));
                        dust_create(&pos, &spd,
                                    SHAKE_DUST_SIZE +
                                        (rand() * (SHAKE_DUST_SIZE_RANDOM *
                                                   RAND_SCALE)));
                    }
                }
            }
            break;
        case DUST_FALL:
            for (i = 0; i < count; i++, vertex++) {
                point = (NJS_POINT3 *)vertex;
                if (point->y >= DUST_HEIGHT) {
                    if (DUST_CHANCE > rand() * RAND_SCALE) {
                        njCalcPoint(NULL, point, &pos);
                        pos.y -= DUST_DROP;
                        /* @hack redundant: the 0.0F added to each part of
                         * spd */
                        spd.x =
                            0.0F + (rand() * (FALL_DUST_SPREAD * RAND_SCALE));
                        spd.y = (rand() * (FALL_DUST_SPREAD * RAND_SCALE)) +
                                FORCES(task)->speed.y + 0.0F;
                        spd.z =
                            0.0F + (rand() * (FALL_DUST_SPREAD * RAND_SCALE));
                        dust_create(&pos, &spd,
                                    FALL_DUST_SIZE +
                                        (rand() *
                                         (FALL_DUST_SIZE_RANDOM * RAND_SCALE)));
                    } else if (DIRT_CHANCE > rand() * RAND_SCALE) {
                        njCalcPoint(NULL, point, &pos);
                        pos.y -= DUST_DROP;
                        /* @hack redundant: the 0.0F added to spd.x and
                         * spd.z */
                        spd.x = 0.0F +
                                (((rand() * RAND_SCALE) - 0.5F) * DIRT_SPREAD);
                        spd.y = FORCES(task)->speed.y;
                        spd.z = 0.0F +
                                (((rand() * RAND_SCALE) - 0.5F) * DIRT_SPREAD);
                        dirt_create(&pos, &spd,
                                    DIRT_SIZE + (rand() * (DIRT_SIZE_RANDOM *
                                                           RAND_SCALE)));
                    }
                }
            }
            break;
        }
    }
    njPopMatrix(1);
    if (player_near(&center, object->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void falling_cliff_piece_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;
    struct falling_cliff_piece *piece;
    NJS_CNK_OBJECT *object;
    Uint32 depth;

    draw_control_set(DRAW_CONTROL_SHADOW);
    piece = &CLIFF(parent)->pieces[PIECE(work)];
    njSetTexture(falling_cliff_texlists[PIECE(work)]);
    /* @hack redundant: a loop that never runs, around a use of the texture
     * lists' address */
    while (0) {
        piece = (void *)falling_cliff_texlists;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y + piece->offset + piece->shake,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    object = objects[PIECE(work)];
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateEx(object->ang, 0);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    while (object) {
        njTranslateEx((NJS_VECTOR *)object->pos);
        njRotateEx(object->ang, 0);
        njCnkEasyDrawModel(object->model);
        if (SHADOW_RADIUS_RANGE >
            _builtin_fabsf(object->model->r - SHADOW_RADIUS)) {
            njTranslateEx(&shadow_offset);
            njScaleEx(&shadow_scale);
            njCnkModDrawModel(&modifier_volume_cylinder_model);
        }
        matrix_load_parent();
        object = object->sibling;
    }
    /* @hack redundant: a push no path reaches */
    if (0) {
        njPushMatrix(NULL);
    }
    depth = DRAW_PUSHES;
    /* @hack redundant: a loop that never runs, which halves depth */
    while (0) {
        depth >>= 1;
    }
    njPopMatrix(depth);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}
