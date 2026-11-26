/* ControlRouge, the task that steers player 1 against player 0 in the fight
 * of Knuckles and Rouge. player 1 is the rival. the task also blows the
 * arena's wind, and draws the vent the wind rises from */
#include "stg33/rival_hunter/text.inc"

#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/bossinit.h"
#include "camera/bosspoint.h"
#include "camera/camera.h"
#include "camera/stack.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/messenger.h"
#include "shared/rival_heading.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg33/rival_hunter.h"
#include "stg33/object_archives.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

struct vent_texture_names {
    char chrboss_k[12];
    char chrboss_n[12];
    char chrboss_u[12];
    char chrboss_c[12];
    char chrboss_l[12];
    char chrboss_e[12];
    char chrboss_s[12];
    char chrboss_r[12];
    char chrboss_o[12];
    char chrboss_g[12];
    char meteo128_10[12];
    char meteo128_11[12];
    char meteo64_1[12];
    char meteo64_3[12];
    char miu128_meh004[16];
    char miu128_meh009[16];
    char miu64_cg020[12];
};
static const struct vent_texture_names vent_texture_names;
static NJS_TEXNAME title_knuckles_textures[7] = {
#include "stg33/rival_hunter/title_knuckles_textures.inc"
};

static NJS_TEXLIST title_knuckles_texlist = {
#include "stg33/rival_hunter/title_knuckles_texlist.inc"
};

static NJS_TEXNAME title_rouge_textures[5] = {
#include "stg33/rival_hunter/title_rouge_textures.inc"
};

static NJS_TEXLIST title_rouge_texlist = {
#include "stg33/rival_hunter/title_rouge_texlist.inc"
};

static NJS_TEXNAME vent_textures[7] = {
#include "stg33/rival_hunter/vent_textures.inc"
};

NJS_TEXLIST vent_texlist = {
#include "stg33/rival_hunter/vent_texlist.inc"
};

static Sint16 vent_positive_z_inner_polygons[181] = {
#include "stg33/rival_hunter/vent_positive_z_inner_polygons.inc"
};

static Sint32 vent_positive_z_inner_vertices[163] = {
#include "stg33/rival_hunter/vent_positive_z_inner_vertices.inc"
};

static NJS_CNK_MODEL vent_positive_z_inner_mesh = {
#include "stg33/rival_hunter/vent_positive_z_inner_mesh.inc"
};

static NJS_CNK_OBJECT vent_positive_z_inner = {
#include "stg33/rival_hunter/vent_positive_z_inner.inc"
};

static Sint16 vent_positive_z_polygons[173] = {
#include "stg33/rival_hunter/vent_positive_z_polygons.inc"
};

static Sint32 vent_positive_z_vertices[131] = {
#include "stg33/rival_hunter/vent_positive_z_vertices.inc"
};

static NJS_CNK_MODEL vent_positive_z_mesh = {
#include "stg33/rival_hunter/vent_positive_z_mesh.inc"
};

static NJS_CNK_OBJECT vent_positive_z = {
#include "stg33/rival_hunter/vent_positive_z.inc"
};

static Sint16 vent_negative_x_inner_polygons[181] = {
#include "stg33/rival_hunter/vent_negative_x_inner_polygons.inc"
};

static Sint32 vent_negative_x_inner_vertices[163] = {
#include "stg33/rival_hunter/vent_negative_x_inner_vertices.inc"
};

static NJS_CNK_MODEL vent_negative_x_inner_mesh = {
#include "stg33/rival_hunter/vent_negative_x_inner_mesh.inc"
};

static NJS_CNK_OBJECT vent_negative_x_inner = {
#include "stg33/rival_hunter/vent_negative_x_inner.inc"
};

static Sint16 vent_negative_x_polygons[173] = {
#include "stg33/rival_hunter/vent_negative_x_polygons.inc"
};

static Sint32 vent_negative_x_vertices[131] = {
#include "stg33/rival_hunter/vent_negative_x_vertices.inc"
};

static NJS_CNK_MODEL vent_negative_x_mesh = {
#include "stg33/rival_hunter/vent_negative_x_mesh.inc"
};

static NJS_CNK_OBJECT vent_negative_x = {
#include "stg33/rival_hunter/vent_negative_x.inc"
};

static Sint16 vent_negative_z_inner_polygons[181] = {
#include "stg33/rival_hunter/vent_negative_z_inner_polygons.inc"
};

static Sint32 vent_negative_z_inner_vertices[163] = {
#include "stg33/rival_hunter/vent_negative_z_inner_vertices.inc"
};

static NJS_CNK_MODEL vent_negative_z_inner_mesh = {
#include "stg33/rival_hunter/vent_negative_z_inner_mesh.inc"
};

static NJS_CNK_OBJECT vent_negative_z_inner = {
#include "stg33/rival_hunter/vent_negative_z_inner.inc"
};

static Sint16 vent_negative_z_polygons[173] = {
#include "stg33/rival_hunter/vent_negative_z_polygons.inc"
};

static Sint32 vent_negative_z_vertices[131] = {
#include "stg33/rival_hunter/vent_negative_z_vertices.inc"
};

static NJS_CNK_MODEL vent_negative_z_mesh = {
#include "stg33/rival_hunter/vent_negative_z_mesh.inc"
};

static NJS_CNK_OBJECT vent_negative_z = {
#include "stg33/rival_hunter/vent_negative_z.inc"
};

static Sint16 vent_positive_x_inner_polygons[181] = {
#include "stg33/rival_hunter/vent_positive_x_inner_polygons.inc"
};

static Sint32 vent_positive_x_inner_vertices[163] = {
#include "stg33/rival_hunter/vent_positive_x_inner_vertices.inc"
};

static NJS_CNK_MODEL vent_positive_x_inner_mesh = {
#include "stg33/rival_hunter/vent_positive_x_inner_mesh.inc"
};

static NJS_CNK_OBJECT vent_positive_x_inner = {
#include "stg33/rival_hunter/vent_positive_x_inner.inc"
};

static Sint16 vent_positive_x_polygons[173] = {
#include "stg33/rival_hunter/vent_positive_x_polygons.inc"
};

static Sint32 vent_positive_x_vertices[131] = {
#include "stg33/rival_hunter/vent_positive_x_vertices.inc"
};

static NJS_CNK_MODEL vent_positive_x_mesh = {
#include "stg33/rival_hunter/vent_positive_x_mesh.inc"
};

static NJS_CNK_OBJECT vent_positive_x = {
#include "stg33/rival_hunter/vent_positive_x.inc"
};

static NJS_CNK_OBJECT vent_object = {
#include "stg33/rival_hunter/vent_object.inc"
};

/* Opening tracks follow the vent model's depth-first panel order. */
static NJS_MKEY_F vent_root_position[2] = {
#include "stg33/rival_hunter/vent_root_position.inc"
};

static NJS_MKEY_A vent_root_angle[2] = {
#include "stg33/rival_hunter/vent_root_angle.inc"
};

static NJS_MKEY_F vent_positive_x_position[20] = {
#include "stg33/rival_hunter/vent_positive_x_position.inc"
};

static NJS_MKEY_A vent_positive_x_angle[2] = {
#include "stg33/rival_hunter/vent_positive_x_angle.inc"
};

static NJS_MKEY_F vent_positive_x_inner_position[16] = {
#include "stg33/rival_hunter/vent_positive_x_inner_position.inc"
};

static NJS_MKEY_A vent_positive_x_inner_angle[2] = {
#include "stg33/rival_hunter/vent_positive_x_inner_angle.inc"
};

static NJS_MKEY_F vent_negative_z_position[20] = {
#include "stg33/rival_hunter/vent_negative_z_position.inc"
};

static NJS_MKEY_A vent_negative_z_angle[2] = {
#include "stg33/rival_hunter/vent_negative_z_angle.inc"
};

static NJS_MKEY_F vent_negative_z_inner_position[16] = {
#include "stg33/rival_hunter/vent_negative_z_inner_position.inc"
};

static NJS_MKEY_A vent_negative_z_inner_angle[2] = {
#include "stg33/rival_hunter/vent_negative_z_inner_angle.inc"
};

static NJS_MKEY_F vent_negative_x_position[20] = {
#include "stg33/rival_hunter/vent_negative_x_position.inc"
};

static NJS_MKEY_A vent_negative_x_angle[2] = {
#include "stg33/rival_hunter/vent_negative_x_angle.inc"
};

static NJS_MKEY_F vent_negative_x_inner_position[16] = {
#include "stg33/rival_hunter/vent_negative_x_inner_position.inc"
};

static NJS_MKEY_A vent_negative_x_inner_angle[2] = {
#include "stg33/rival_hunter/vent_negative_x_inner_angle.inc"
};

static NJS_MKEY_F vent_positive_z_position[20] = {
#include "stg33/rival_hunter/vent_positive_z_position.inc"
};

static NJS_MKEY_A vent_positive_z_angle[2] = {
#include "stg33/rival_hunter/vent_positive_z_angle.inc"
};

static NJS_MKEY_F vent_positive_z_inner_position[16] = {
#include "stg33/rival_hunter/vent_positive_z_inner_position.inc"
};

static NJS_MKEY_A vent_positive_z_inner_angle[2] = {
#include "stg33/rival_hunter/vent_positive_z_inner_angle.inc"
};

static NJS_MDATA2 vent_tracks[9] = {
#include "stg33/rival_hunter/vent_tracks.inc"
};

static NJS_MOTION vent_motion[1] = {
#include "stg33/rival_hunter/vent_motion.inc"
};

#define CONTROL_LEVEL 0

/* player 1's gauge, which a boss gauge shows. rival_hunter_create sets it to
 * START_GAUGE, and a start over fills it to FULL_GAUGE. below WEAK_GAUGE,
 * player 1 fights in MODE_WEAK */
#define START_GAUGE 3.0F
#define FULL_GAUGE 4.0F
#define WEAK_GAUGE 3.0F
/* past CHASE_STEPS, MODE_CHASE draws its next move, a whole number below
 * CHASE_SPREAD. a gauge of LOW_GAUGE or less adds LOW_RAISE to the draw. any
 * other gauge below MIDDLE_GAUGE adds MIDDLE_RAISE. in the wind the draw is
 * CHASE_JUMP, whatever the gauge. a draw below CHASE_JUMP circles, and any
 * other jumps */
#define CHASE_SPREAD 2.999F
#define CHASE_JUMP 3
#define LOW_GAUGE 2.0F
#define LOW_RAISE 3.0F
#define MIDDLE_GAUGE 4.0F
#define MIDDLE_RAISE 2.0F

/* player 1 counts player 0 close within NEAR, or within WEAK_NEAR while its
 * gauge is below WEAK_GAUGE. player 0's jump or glide threatens player 1 only
 * within REACH. MODE_CHASE and MODE_CIRCLE punch at a threat from beyond
 * REACH, and jump at one from within it. MODE_WEAK jumps at player 0 only
 * within FAR */
#define WEAK_NEAR 50.0F
#define NEAR 25.0F
#define REACH 20.0F
#define FAR 60.0F
/* rival_over_player's reach across x and z. MODE_PLAYER_CLIMBS also offers
 * the earthquake within OVER_RADIUS of arena_center. that distance counts all
 * three axes */
#define OVER_RADIUS 10.0F
/* player_above's reach across x and z */
#define ABOVE_RADIUS 15.0F
/* MODE_WEAK offers the earthquake where player 0 stands at least HIGH_GROUND
 * above a player 1 on its feet */
#define HIGH_GROUND 10.0F
/* players_apart counts the two apart beyond APART_HEIGHT in height. a player
 * stands by a wall of the arena's box within EDGE_MARGIN of it */
#define APART_HEIGHT 50.0F
#define EDGE_MARGIN 20.0F
/* player 1 faces player 0 while its ang.y lies within FACING of the heading
 * to player 0 */
#define FACING 0x3400

/* the strokes the task gives player 1's stick: all the way, or a turn's.
 * where player 1 doesn't face player 0, MODE_CHASE and MODE_WEAK turn the
 * stick toward it. the stick then lies at most TURN_STEP off player 1's
 * ang.y */
#define RUN_STROKE 1.0F
#define TURN_STROKE 0.2F
#define TURN_STEP 0x180

/* a mode's length drawn at random: least steps, and up to below spread
 * more */
#define RANDOM_STEPS(least, spread)                                            \
    ((least) + ((Float)rand() * ((spread) * RAND_SCALE)))
/* MODE_CHASE draws its next move past CHASE_STEPS, and MODE_WEAK tries its
 * ring attacks past WEAK_STEPS. MODE_COMBO ends past COMBO_STEPS, and
 * MODE_PUNCHES past PUNCHES_STEPS. MODE_PLAYER_CLIMBS ends at
 * PLAYER_CLIMBS_STEPS, and MODE_EARTHQUAKE at EARTHQUAKE_STEPS */
#define CHASE_STEPS 300
#define WEAK_STEPS 120
#define COMBO_STEPS 360
#define PUNCHES_STEPS 120
#define PLAYER_CLIMBS_STEPS 600
#define EARTHQUAKE_STEPS 121
/* MODE_RECOVER's wait after the intro, and after a ring attack in the wind */
#define RECOVER_STEPS 60
#define WIND_RECOVER_STEPS 120
/* MODE_CLIMB's wait for a climb from a chase */
#define CLIMB_STEPS 60
/* the wait a start over sets. MODE_WAIT, which the start over takes, never
 * reads it, and gives the intro INTRO_STEPS */
#define START_WAIT 60
/* MODE_CIRCLE's wait. after MODE_PUNCHES it is drawn by SHORT_CIRCLE_LEAST
 * and SHORT_CIRCLE_SPREAD. after the earthflame its spread is
 * EARTHFLAME_CIRCLE_SPREAD */
#define CIRCLE_LEAST 120.0F
#define CIRCLE_SPREAD 360.0F
#define SHORT_CIRCLE_LEAST 60.0F
#define SHORT_CIRCLE_SPREAD 120.0F
#define EARTHFLAME_CIRCLE_SPREAD 180.0F
/* MODE_GLIDE's wait, and MODE_CLIMB's for a climb from a glide */
#define GLIDE_LEAST 180.0F
#define GLIDE_SPREAD 240.0F
#define CLIMB_LEAST 180.0F
#define CLIMB_SPREAD 60.0F
/* MODE_CIRCLE heads for a point CIRCLE_RADIUS from player 0. the point turns
 * about player 0 by 1 << CIRCLE_TURN_SHIFT each step */
#define CIRCLE_TURN_SHIFT 8
#define CIRCLE_RADIUS 80.0F
/* a jump tops out where player 1's record has a spd.y of 0. player 1 must
 * then lie more than JUMP_TOP_HEIGHT above the ground below player 0.
 * MODE_JUMP and MODE_JUMP_TOP count no landing in their first TAKE_OFF_STEPS
 * steps */
#define TAKE_OFF_STEPS 2
#define JUMP_TOP_HEIGHT 30.0F
/* MODE_GLIDE_AT lets go of jump, then presses it again at its step
 * GLIDE_PRESS_STEP and holds it */
#define GLIDE_PRESS_STEP 5
/* MODE_COMBO presses action where the record's steps under COMBO_MASK are
 * COMBO_PUNCH_1, COMBO_PUNCH_2 or COMBO_PUNCH_3. MODE_PUNCHES and
 * MODE_HURT_PUNCHES press it every 8th step, where the steps under PUNCH_MASK
 * are 0. a press can end MODE_COMBO or MODE_PUNCHES where player 1 faces
 * player 0. MODE_PUNCHES then takes MODE_CHASE where the steps under COMBO_MASK
 * are 0. MODE_COMBO does so at COMBO_PUNCH_3, unless player_above holds */
#define COMBO_MASK 127
#define COMBO_PUNCH_1 15
#define COMBO_PUNCH_2 40
#define COMBO_PUNCH_3 78
#define PUNCH_MASK 7
/* MODE_COMBO offers player 1 the spiral for SPIRAL_OFFER_STEPS, by its
 * spin_steps. it does so at a punch where player_above holds */
#define SPIRAL_OFFER_STEPS 30
/* MODE_CLIMB holds player 1 between CLIMB_LOW and CLIMB_HIGH on its wall */
#define CLIMB_HIGH 160.0F
#define CLIMB_LOW 140.0F
/* MODE_ANIM_23 runs player 1 to the arena's center from below
 * CENTER_RUN_LOW */
#define CENTER_RUN_LOW (-192.0F)
/* once player 1's gauge is empty, each step turns player 0's look to player
 * 1 for LOOK_STEPS steps */
#define LOOK_STEPS 3
/* as the task gives player 1 a ring level, it sets player 1's
 * game_state.ring_level_timer to RING_LEVEL_STEPS. the level offers its
 * attack only once the timer has run down to 0 */
#define RING_LEVEL_STEPS 120

/* after the intro, each step moves the record's camera_target. in the wind,
 * the target eases toward a goal. it eases only while player 0 stands beyond
 * CAMERA_WIND_CENTER of the arena's center across x and z. the goal lies
 * CAMERA_WIND_AHEAD beyond player 0 from the center, and CAMERA_WIND_DROP
 * below it. past CAMERA_WIND_FAR from the center, the goal weighs CAMERA_EASE
 * in the new target, and the old target CAMERA_KEEP. nearer, the goal weighs
 * its distance times CAMERA_WIND_RATE, and the old target the rest.
 *
 * out of the wind, the target lies CAMERA_LIFT above player 1 while player
 * 1 is near player 0. beyond CAMERA_FAR across x and z, it lies
 * CAMERA_FAR_AHEAD out from player 0 toward player 1. it then lies
 * CAMERA_FAR_DROP below player 0. beyond CAMERA_HIGH above or below, it lies
 * CAMERA_HIGH_AHEAD out, with its height CAMERA_ABOVE or CAMERA_BELOW off
 * player 0's */
#define CAMERA_WIND_CENTER 20.0F
#define CAMERA_WIND_AHEAD 30.0F
#define CAMERA_WIND_DROP 15.0F
#define CAMERA_WIND_FAR 100.0F
#define CAMERA_EASE 0.1F
#define CAMERA_KEEP 0.9F
#define CAMERA_WIND_RATE 0.001F
#define CAMERA_FAR 150.0F
#define CAMERA_FAR_AHEAD 20.0F
#define CAMERA_FAR_DROP 5.0F
#define CAMERA_HIGH 80.0F
#define CAMERA_HIGH_AHEAD 40.0F
#define CAMERA_ABOVE 98.0F
#define CAMERA_BELOW (-62.0F)
#define CAMERA_LIFT 18.0F

/* while the wind blows, the vent's frame moves VENT_FRAME_STEP a step toward
 * its last frame. otherwise it moves toward 0. the vent stands at the floor
 * of the arena's box, VENT_HEIGHT */
#define VENT_FRAME_STEP 0.2F
#define VENT_HEIGHT (-340.0F)

/* the intro lasts until the record's steps pass INTRO_STEPS, counting those
 * MODE_WAIT ran. player 1 says its line at step INTRO_VOICE_STEP */
#define INTRO_STEPS 480
#define INTRO_VOICE_STEP 120
/* the intro's BOSSINIT camera, by camera_bossinit's fields. it looks at the
 * record's camera_target, which MODE_WAIT puts INTRO_CAMERA_LIFT above the
 * arena's floor. the camera stands off the target a half turn from player
 * 1's ang.y */
#define INTRO_CAMERA_SPEED 1.0F
#define INTRO_CAMERA_START 160.0F
#define INTRO_CAMERA_DIST 10.0F
#define INTRO_CAMERA_TURN 0x6000
#define INTRO_CAMERA_LIFT 7.5F
/* the BOSSPOINT camera the intro's end gives camera 0, by camera_bosspoint's
 * fields. it keeps the record's camera_target beyond player 0 */
#define PLAY_CAMERA_DIST 70.0F
#define PLAY_CAMERA_HEIGHT 50.0F
#define PLAY_CAMERA_PART 0.5F
#define PLAY_CAMERA_LIFT 0.0F
/* after a retry, the intro's end summons a messenger MESSENGER_LIFT above
 * the arena's center. it shows the stage's message for the retries. where
 * player 0 is a Rouge, it shows the one RETRY_MESSAGES further on */
#define MESSENGER_LIFT 300.0F
#define NO_MESSAGE (-1)
enum retry_message {
    ONE_RETRY_MESSAGE,
    TWO_RETRIES_MESSAGE,
    MORE_RETRIES_MESSAGE,
    RETRY_MESSAGES
};

/* the lines each hunter says at the intro, and as the task gives it a ring
 * level. both hunters' wind lines play as the wind starts. player 1 says its
 * lines as a rival's voice, at RIVAL_VOICE_PRIORITY */
#define VOICE_KNUCKLES_INTRO 0x83b
#define VOICE_ROUGE_INTRO 0x892
#define VOICE_KNUCKLES_EARTHQUAKE 0x840
#define VOICE_ROUGE_EARTHQUAKE 0x897
#define VOICE_KNUCKLES_EARTHFLAME 0x841
#define VOICE_ROUGE_EARTHFLAME 0x898
#define VOICE_KNUCKLES_WIND 0x843
#define VOICE_ROUGE_WIND 0x89a
#define NO_VOICE (-1)

/* the wind's cycle, in steps: it blows from WIND_START to WIND_END, and the
 * cycle starts over after WIND_CYCLE */
#define WIND_START 1200
#define WIND_END 4200
#define WIND_CYCLE 5400
/* each of the wind's particles is a child of EffectSpriteKvsRMan, a task at
 * level PARTICLE_LEVEL. a particle lives
 * PARTICLE_STEPS, and rises PARTICLE_RISE a step. it shows each of
 * stg33_objtex_texlist's textures for PARTICLE_FRAME_STEPS. it starts at
 * random in a square PARTICLE_SPREAD wide about the arena's center. its
 * height starts from PARTICLE_LOW up to PARTICLE_HEIGHT higher. its scale is
 * PARTICLE_SCALE, and up to PARTICLE_SCALE_SPREAD more */
#define PARTICLE_LEVEL 4
#define PARTICLE_STEPS 60
#define PARTICLE_FRAME_STEPS 3
#define PARTICLE_RISE 3.0F
#define PARTICLE_SPREAD 260.0F
#define PARTICLE_LOW (-460.0F)
#define PARTICLE_HEIGHT 170.0F
#define PARTICLE_SCALE 0.4F
#define PARTICLE_SCALE_SPREAD 0.1F
#define QUAD_VERTICES 4
/* rand() * TURN_PER_RAND spans a whole turn, which a particle's roll takes at
 * random */
#define TURN_PER_RAND 2.0F
/* the wind shakes player 0's pad every 64th step, where game_state.steps
 * under WIND_SHAKE_MASK is 0 */
#define WIND_SHAKE_MASK 0x3f
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* ControlRouge's modes */
enum rival_mode {
    /* wait for STAGE_PLAY, then take both pads and the pause, and start the
     * intro */
    MODE_WAIT,
    /* the title and camera 0's BOSSINIT shot. the intro's end gives back
     * player 0's pad and the pause, and takes MODE_RECOVER. after a retry, the
     * end also summons a messenger */
    MODE_INTRO,
    /* steer at player 0, or in the wind along the nearer wall. punch or jump
     * at a threat. below WEAK_GAUGE, the mode takes MODE_WEAK and still
     * chases on that step */
    MODE_CHASE,
    /* steer as MODE_CHASE does. past WEAK_STEPS, give player 1 a ring attack,
     * or jump at player 0 */
    MODE_WEAK,
    MODE_JUMP, /* hold jump until player 1 tops out or lands */
    MODE_CIRCLE, /* run round player 0, for the record's wait */
    MODE_COMBO, /* punch three times every 128 steps */
    MODE_PUNCHES, /* punch every 8th step */
    /* run at right angles to the line to player 0, for the record's wait.
     * no code sets the mode */
    MODE_DODGE,
    /* wait the record's wait, or until player 1 is hit. a threat takes
     * MODE_PUNCHES */
    MODE_RECOVER,
    /* steer at player 0, punching every 8th step while player 1's hit lasts.
     * no code sets the mode */
    MODE_HURT_PUNCHES,
    /* taken where player 0 climbs: run to the arena's center, and offer the
     * earthquake there. its case falls through into MODE_EARTHQUAKE's */
    MODE_PLAYER_CLIMBS,
    /* wait for the earthquake's offer, then press action to take it. a threat
     * holds the mode open, and within REACH draws a press too. a hit ends the
     * mode */
    MODE_EARTHQUAKE,
    /* wait for the earthflame's offer, then press action to take it. the
     * offer ends the mode under a threat too, and a threat within REACH draws
     * a press. a hit ends the mode only once the earthflame's level is
     * spent */
    MODE_EARTHFLAME,
    MODE_JUMP_TOP, /* press jump again at the top, to glide */
    MODE_GLIDE, /* glide at the corner rival_steer_to_corner picks */
    /* climb the wall for the record's wait, then glide at player 0 */
    MODE_CLIMB,
    MODE_GLIDE_AT, /* glide at player 0, and dive once over it */
    MODE_DIVE, /* dive at player 0 until player 1 lands */
    MODE_LANDED, /* wait out a dig, then take MODE_JUMP_TOP or MODE_CHASE */
    /* taken while player 1 plays ANIM_UNK_23, as in an updraft's fall. below
     * CENTER_RUN_LOW, run to the arena's center. higher, run away from player 0
     * along the axis on which player 1 stands farther out. on the side where z
     * is above 0, run toward it instead */
    MODE_ANIM_23,
    /* MODE_CHASE without its test of the gauge, after a jump that lands
     * before its second press */
    MODE_CHASE_AGAIN,
    MODE_STOPPED /* either player has missed: no mode steers player 1 */
};

/* ControlRouge's record, the task's extra */
struct rival_hunter {
    Sint8 mode; /* an enum rival_mode */
    /* TRUE once the intro has ended, and FALSE until then. the intro's end
     * summons a messenger only while it is FALSE, which it always is there.
     * MODE_INTRO follows only MODE_WAIT, which the record starts in and a
     * start over takes. a start over also clears the flag */
    Sint8 intro_done;
    Sint8 wind; /* TRUE while the wind blows */
    Uint8 unk_03;
    /* the steps the mode has run, counted at each step's end. most changes of
     * mode set it to 0 */
    Sint16 steps;
    Sint16 wait; /* the steps a timed mode lasts */
    Sint32 wind_steps; /* the steps of the wind's cycle so far */
    Float vent_frame;
    /* the point camera 0 looks at. through the intro it lies
     * INTRO_CAMERA_LIFT above the arena's floor. its x and z are player 1's as
     * the intro starts. after the intro, camera 0 keeps it beyond player 0 */
    NJS_POINT3 camera_target;
};

static void rival_draw(struct task *task);
static void rival_step(struct task *task);
static void release_knuckles_title(struct task *task);
static void release_rouge_title(struct task *task);
static void wind_particle_draw(struct task *task);
static void wind_particle_step(struct task *task);
static void wind_manager_step(struct task *task);
static void wind_manager_destroy(struct task *task);
static void wind_particle_create(void);

/* @hack type: arena_center an array of one, where one point would do */
static NJS_POINT3 arena_center[1] = {
#include "stg33/rival_hunter/arena_center.inc"
};
/* the arena's box, about arena_center: its highest corner, then its
 * lowest */
static NJS_POINT3 arena_bounds[2] = {
#include "stg33/rival_hunter/arena_bounds.inc"
};
static Float dat_8c53f150 = 140.0F;
/* the arena's corners at height 0. rival_steer_to_corner reads the first
 * corner_count */
static NJS_POINT3 corners[4] = {
#include "stg33/rival_hunter/corners.inc"
};
static Sint32 corner_count = 4;
/* The title letters in display order, with their chrboss_ texture indices. */
#define KNUCKLES_LETTERS 8
#define ROUGE_LETTERS 5
static struct boss_title_letter title_knuckles_letters[KNUCKLES_LETTERS] = {
#include "stg33/rival_hunter/title_knuckles_letters.inc"
};
static struct boss_title_letter title_rouge_letters[ROUGE_LETTERS] = {
#include "stg33/rival_hunter/title_rouge_letters.inc"
};
static struct boss_title title_knuckles = {
#include "stg33/rival_hunter/title_knuckles.inc"
};
static struct boss_title title_rouge = {
#include "stg33/rival_hunter/title_rouge.inc"
};
/* the wind lifts a player that lies less than wind_height above the arena's
 * floor. the lift is wind_strength for each unit below that height, and at
 * most wind_max a step */
static Float wind_height = 170.0F;
static Float wind_strength = 0.03F;
static Float wind_max = 0.8F;
/* EffectSpriteKvsRMan, the task of the wind's particles, or null */
static struct task *wind_manager = NULL;
/* a wind particle's quad, before its scale */
static NJS_TEXTURE_VTX wind_quad[QUAD_VERTICES] = {
#include "stg33/rival_hunter/wind_quad.inc"
};

/* @hack spelling: STEP_HEADING, RIVAL_HEADING with its product by
 * ANGLE_PER_RADIAN written as a product by FULL_TURN behind a unary plus and
 * then one by 1 / (2 pi), where RIVAL_HEADING would do */
#define STEP_HEADING(dx, dz)                                                   \
    (QUARTER_TURN -                                                            \
     (Angle)(+(atan2f((dx), (dz)) * FULL_TURN) * (1.0F / (2.0F * NJD_PI))))

static void rival_draw(struct task *task)
{
    struct rival_hunter *control = task->extra.p;
    Float frame;

    if (game_state.winner_posed == 1) {
        frame = 0.0F;
    } else {
        frame = control->vent_frame;
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&vent_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, VENT_HEIGHT, 0.0F);
    njCnkEasyDrawMotion(&vent_object, vent_motion, frame);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* hold player 1 within the arena's box. it returns TRUE where it moved
 * player 1 */
static Bool keep_rival_in_arena(struct rival_hunter *control,
                                struct task_work *rival)
{
    NJS_POINT3 pos;
    Bool moved = FALSE;

    pos = rival->pos;
    if (pos.x > arena_center[0].x + arena_bounds[0].x) {
        pos.x = arena_center[0].x + arena_bounds[0].x;
        moved = TRUE;
    }
    if (pos.x < arena_center[0].x + arena_bounds[1].x) {
        pos.x = arena_center[0].x + arena_bounds[1].x;
        moved = TRUE;
    }
    if (pos.y > arena_center[0].y + arena_bounds[0].y) {
        pos.y = arena_center[0].y + arena_bounds[0].y;
        moved = TRUE;
    }
    if (pos.y < arena_center[0].y + arena_bounds[1].y) {
        pos.y = arena_center[0].y + arena_bounds[1].y;
        moved = TRUE;
    }
    if (pos.z > arena_center[0].z + arena_bounds[0].z) {
        pos.z = arena_center[0].z + arena_bounds[0].z;
        moved = TRUE;
    }
    if (pos.z < arena_center[0].z + arena_bounds[1].z) {
        pos.z = arena_center[0].z + arena_bounds[1].z;
        moved = TRUE;
    }
    /* @hack spelling: TRUE returned in moved's arm, where moved returned
     * after the arm would do */
    if (moved) {
        player_set_pos(RIVAL_SLOT, pos.x, pos.y, pos.z);
        return TRUE;
    }
    return FALSE;
}

/* keep_rival_in_arena, for player 0 */
static Bool keep_player_in_arena(struct rival_hunter *control,
                                 struct task_work *player)
{
    NJS_POINT3 pos;
    Bool moved = FALSE;

    pos = player->pos;
    if (pos.x > arena_center[0].x + arena_bounds[0].x) {
        pos.x = arena_center[0].x + arena_bounds[0].x;
        moved = TRUE;
    }
    if (pos.x < arena_center[0].x + arena_bounds[1].x) {
        pos.x = arena_center[0].x + arena_bounds[1].x;
        moved = TRUE;
    }
    if (pos.y > arena_center[0].y + arena_bounds[0].y) {
        pos.y = arena_center[0].y + arena_bounds[0].y;
        moved = TRUE;
    }
    if (pos.y < arena_center[0].y + arena_bounds[1].y) {
        pos.y = arena_center[0].y + arena_bounds[1].y;
        moved = TRUE;
    }
    if (pos.z > arena_center[0].z + arena_bounds[0].z) {
        pos.z = arena_center[0].z + arena_bounds[0].z;
        moved = TRUE;
    }
    if (pos.z < arena_center[0].z + arena_bounds[1].z) {
        pos.z = arena_center[0].z + arena_bounds[1].z;
        moved = TRUE;
    }
    /* @hack spelling: TRUE returned in moved's arm, where moved returned
     * after the arm would do */
    if (moved) {
        player_set_pos(PLAYER_SLOT, pos.x, pos.y, pos.z);
        return TRUE;
    }
    return FALSE;
}

static Bool rival_faces_player(struct rival_hunter *control,
                               struct task_work *player,
                               struct task_work *rival)
{
    Angle heading;

    heading = RIVAL_HEADING_TOWARD(rival, player);
    /* @hack spelling: TRUE returned from the test's arm, where the
     * test's value returned would do */
    if (angle_distance(heading, rival->ang.y) < FACING) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 1 faces player 0, or lies within WEAK_NEAR of it with a
 * gauge below WEAK_GAUGE */
static Bool rival_engaged(struct rival_hunter *control,
                          struct task_work *player, struct task_work *rival)
{
    Float distance = njDistanceP2P(&player->pos, &rival->pos);

    /* @hack spelling: TRUE returned from each test's arm, where the two
     * tests joined by || and returned would do */
    if (player_records[RIVAL_SLOT]->gauge < WEAK_GAUGE &&
        distance < WEAK_NEAR) {
        return TRUE;
    }
    if (rival_faces_player(control, player, rival)) {
        return TRUE;
    }
    return FALSE;
}

static Bool player_close(struct rival_hunter *control, struct task_work *player,
                         struct task_work *rival)
{
    Float distance = njDistanceP2P(&player->pos, &rival->pos);

    /* @hack spelling: TRUE returned from the test's arm, where the
     * test's value returned would do */
    if ((player_records[RIVAL_SLOT]->gauge < WEAK_GAUGE &&
         distance < WEAK_NEAR) ||
        distance <= NEAR) {
        return TRUE;
    }
    return FALSE;
}

static Bool players_far(struct task_work *player, struct task_work *rival)
{
    Float distance = njDistanceP2P(&player->pos, &rival->pos);

    /* @hack spelling: TRUE returned from the test's arm, where the
     * test's value returned would do */
    if (distance > FAR) {
        return TRUE;
    }
    return FALSE;
}

/* the walls of the arena's box, as bits */
#define WALL_X_HIGH 0x1
#define WALL_X_LOW 0x2
#define WALL_Z_HIGH 0x4
#define WALL_Z_LOW 0x8

/* FALSE where the two players stand by one wall of the arena's box, at
 * heights within APART_HEIGHT. TRUE otherwise. a player by two walls counts
 * only the first in the order of the WALL_ bits */
static Bool players_apart(struct task_work *player, struct task_work *rival)
{
    /* @bug uninitialized: players_apart reads walls before anything sets it,
     * where 0 was meant. the ORs at 0x8c500ba6 and 0x8c500bb6 read it, or
     * where neither runs, the ANDs below. in the shipped code walls holds the
     * address of player 0's work there. the heap gives a work on a 32-byte
     * boundary, so the WALL_ bits read clear, as they would from 0 */
#ifdef NONMATCHING
    /* this side keeps the result of the tag above: it starts walls at the
     * address the shipped code holds there */
    int walls = (int)player;
#else
    int walls;
#endif
    Float height = njAbs(player->pos.y - rival->pos.y);

    if (height > APART_HEIGHT) {
        return TRUE;
    }
    /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
    if (player->pos.x > arena_bounds[0].x - EDGE_MARGIN) {
        walls |= WALL_X_HIGH;
    } else if (player->pos.x < arena_bounds[1].x + EDGE_MARGIN) {
        walls |= WALL_X_LOW;
    } else if (player->pos.z > arena_bounds[0].z - EDGE_MARGIN) {
        walls |= WALL_Z_HIGH;
    } else if (player->pos.z < arena_bounds[1].z + EDGE_MARGIN) {
        walls |= WALL_Z_LOW;
    }
    /* NOLINTEND(clang-diagnostic-uninitialized) */
    if (rival->pos.x > arena_bounds[0].x - EDGE_MARGIN) {
        walls &= WALL_X_HIGH;
    } else if (rival->pos.x < arena_bounds[1].x + EDGE_MARGIN) {
        walls &= WALL_X_LOW;
    } else if (rival->pos.z > arena_bounds[0].z - EDGE_MARGIN) {
        walls &= WALL_Z_HIGH;
    } else if (rival->pos.z < arena_bounds[1].z + EDGE_MARGIN) {
        walls &= WALL_Z_LOW;
    } else {
        walls = 0;
    }
    /* @hack spelling: TRUE returned from the test's arm, where the
     * test's value returned would do */
    if (walls == 0) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where rival_engaged holds while player 0 attacks. an attack is one of
 * player 0's three attacks or three running attacks, its spiral or its dive.
 * within REACH, a jump or a glide is one too */
static Bool player_threatens(struct rival_hunter *control,
                             struct task_work *player, struct task_work *rival)
{
    if (rival_engaged(control, player, rival)) {
        switch (player->mode) {
        case PLAYER_MODE_JUMP:
        case PLAYER_MODE_HUNTER_GLIDE:
            if (njDistanceP2P(&player->pos, &rival->pos) > REACH) {
                break;
            }
            /* fall through */
        case PLAYER_MODE_HUNTER_ATTACK_1:
        case PLAYER_MODE_HUNTER_ATTACK_2:
        case PLAYER_MODE_HUNTER_ATTACK_3:
        case PLAYER_MODE_HUNTER_RUN_ATTACK_1:
        case PLAYER_MODE_HUNTER_RUN_ATTACK_2:
        case PLAYER_MODE_HUNTER_RUN_ATTACK_3:
        case PLAYER_MODE_HUNTER_SPIRAL:
        case PLAYER_MODE_HUNTER_DIVE:
            return TRUE;
        }
    }
    return FALSE;
}

/* TRUE while player 1 plays ANIM_UNK_23. a hunter plays it in an updraft,
 * such as the wind's, and in a current or a bump */
static Bool rival_in_anim_23(struct rival_hunter *control,
                             struct player *rival_record)
{
    /* @hack spelling: a switch with one case, where an if would do */
    switch (rival_record->anim.current) {
    case ANIM_UNK_23:
        return TRUE;
    default:
        return FALSE;
    }
}

/* steer player 1 at the corner nearest player 0 among the first and those
 * ahead of player 0. a corner lies ahead within a quarter turn of player 0's
 * ang.y */
static void rival_steer_to_corner(struct rival_hunter *control,
                                  struct task_work *player,
                                  struct task_work *rival)
{
    Sint32 i;
    Sint32 nearest;
    NJS_POINT3 pos;
    Angle heading;

    pos.x = player->pos.x;
    pos.y = 0.0F;
    pos.z = player->pos.z;
    /* @bug logic: the steer takes the first corner, though it lies behind
     * player 0, where no corner ahead lies nearer. nearest starts as that
     * corner, whatever its heading, at 0x8c500d26. the loop passes over each
     * corner behind at 0x8c500d7e. a corner ahead takes nearest's place only
     * where it lies nearer, at 0x8c500da6 */
    nearest = 0;
    /* @hack spelling: i set before the loop, where the for's first clause
     * would do */
    i = 0;
    /* @hack redundant: heading set before the loop, which sets it again
     * before any read */
    heading = QUARTER_TURN;
    for (; i < corner_count; ++i) {
        heading = RIVAL_HEADING(corners[i].x - pos.x, corners[i].z - pos.z);
        if (angle_distance(heading, player->ang.y) > QUARTER_TURN) {
            continue;
        }
        /* @hack spelling: nearest set by a ?: that may keep it, where an if
         * would do */
        nearest = njDistanceP2P(&pos, &corners[i]) <
                          njDistanceP2P(&pos, &corners[nearest])
                      ? i
                      : nearest;
        /* @hack redundant: a continue that ends the loop's body */
        continue; /* NOLINT(readability-redundant-control-flow) */
    }
    heading = RIVAL_HEADING(corners[nearest].x - rival->pos.x,
                            corners[nearest].z - rival->pos.z);
    pad_sticks[RIVAL_SLOT].ang = heading;
    pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
}

/* TRUE where player 1 lies within OVER_RADIUS of player 0 across x and z */
static Bool rival_over_player(struct rival_hunter *control,
                              struct task_work *player, struct task_work *rival)
{
    NJS_POINT3 pos;
    NJS_POINT3 rival_pos;

    pos.x = player->pos.x;
    pos.y = 0.0F;
    pos.z = player->pos.z;
    rival_pos.x = rival->pos.x;
    rival_pos.y = 0.0F;
    rival_pos.z = rival->pos.z;
    /* @hack spelling: TRUE returned from the test's arm, where the
     * test's value returned would do */
    if (njDistanceP2P(&pos, &rival_pos) < OVER_RADIUS) {
        return TRUE;
    }
    return FALSE;
}

/* blow the wind as its cycle says. each step of the wind makes a particle
 * and lifts both players, as wind_height says */
static void wind_step(struct rival_hunter *control, struct task_work *player,
                      struct task_work *rival)
{
    /* @hack type: blew a Sint8, where a Bool would do */
    Sint8 blew = control->wind;
    Float lift;

    control->wind = FALSE;
    /* @hack spelling: each test after a return the else of the test before,
     * where tests of their own would do */
    if (control->wind_steps < WIND_START) {
        return;
    } else if (control->wind_steps > WIND_CYCLE) {
        control->wind_steps = 0;
        return;
    } else if (control->wind_steps <= WIND_END) {
        control->wind = TRUE;
        /* @hack spelling: a goto past the voices, where a test of !blew
         * around them would do */
        if (blew) {
            goto blow;
        }
        voice_play_priority_2(VOICE_KNUCKLES_WIND);
        voice_play_priority_2(VOICE_ROUGE_WIND);
    blow:
        wind_particle_create();
        lift =
            (wind_height - player->pos.y + arena_bounds[1].y) * wind_strength;
        if (lift > wind_max) {
            lift = wind_max;
        }
        if (lift > 0.0F) {
            player_updraft(PLAYER_SLOT, 0.0F, lift, 0.0F);
        }
        lift = (wind_height - rival->pos.y + arena_bounds[1].y) * wind_strength;
        if (lift > wind_max) {
            lift = wind_max;
        }
        if (lift > 0.0F) {
            player_updraft(RIVAL_SLOT, 0.0F, lift, 0.0F);
        }
    }
}

/* give player 1 the earthquake's ring level alone, and take
 * MODE_EARTHQUAKE. player 1 says its earthquake line */
static void rival_offer_earthquake(struct rival_hunter *control,
                                   struct player *rival_record)
{
    int i;

    control->steps = 0;
    control->mode = MODE_EARTHQUAKE;
    game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_EARTHQUAKE] = 1;
    game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_EARTHFLAME] = 0;
    game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_POWER_FLASH] = 0;
    for (i = 0; i < VOICES; i++) {
        /* @hack spelling: voice_can_start's result compared with TRUE, where
         * the result alone would do */
        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
            voice_stop(i);
        }
    }
    switch (rival_record->character) {
    case PLAYER_KNUCKLES:
        voice_play(VOICE_KNUCKLES_EARTHQUAKE, RIVAL_VOICE_PRIORITY);
        break;
    case PLAYER_ROUGE:
        voice_play(VOICE_ROUGE_EARTHQUAKE, RIVAL_VOICE_PRIORITY);
        break;
    }
    game_state.ring_level_timer[RIVAL_SLOT] = RING_LEVEL_STEPS;
}

/* TRUE where player 0 is in the air and player 1 on its feet. the two must
 * lie within ABOVE_RADIUS of each other across x and z */
static Bool player_above(struct rival_hunter *control, struct task_work *player,
                         struct task_work *rival)
{
    NJS_VECTOR delta;

    if (!(player->flags & PLAYER_WORK_GROUNDED) &&
        (rival->flags & PLAYER_WORK_GROUNDED)) {
        delta = player->pos;
        njSubVector(&delta, &rival->pos);
        delta.y = 0.0F;
        /* @hack spelling: TRUE returned from the test's arm, where the
         * test's value returned would do */
        if (njScalor(&delta) < ABOVE_RADIUS) {
            return TRUE;
        }
    }
    return FALSE;
}

/* steer player 1 as MODE_CHASE and MODE_WEAK do. in the wind it runs along
 * the wall it stands nearer, and otherwise at player 0. where player 1
 * doesn't face player 0, the stick turns toward it by TURN_STEP at
 * TURN_STROKE */
#define CHASE_STEER(control, player, rival)                                    \
    {                                                                          \
        Angle heading;                                                         \
                                                                               \
        /* @hack spelling: wind compared with TRUE, where its truth            \
         * would do */                                                         \
        if ((control)->wind == TRUE) {                                         \
            if (njAbs((rival)->pos.x) > njAbs((rival)->pos.z)) {               \
                if (angle_distance((rival)->ang.y, 0) < QUARTER_TURN) {        \
                    heading = QUARTER_TURN;                                    \
                } else {                                                       \
                    heading = -QUARTER_TURN;                                   \
                }                                                              \
            } else {                                                           \
                if (angle_distance((rival)->ang.y, QUARTER_TURN) <             \
                    QUARTER_TURN) {                                            \
                    heading = 0;                                               \
                } else {                                                       \
                    heading = -HALF_TURN;                                      \
                }                                                              \
            }                                                                  \
            pad_sticks[RIVAL_SLOT].ang = heading;                              \
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;                        \
        } else if (rival_faces_player((control), (player), (rival))) {         \
            heading = STEP_HEADING((player)->pos.x - (rival)->pos.x,           \
                                   (player)->pos.z - (rival)->pos.z);          \
            pad_sticks[RIVAL_SLOT].ang = heading;                              \
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;                        \
        } else {                                                               \
            heading = STEP_HEADING((player)->pos.x - (rival)->pos.x,           \
                                   (player)->pos.z - (rival)->pos.z);          \
            heading = angle_approach((rival)->ang.y, heading, TURN_STEP);      \
            pad_sticks[RIVAL_SLOT].ang = heading;                              \
            pad_sticks[RIVAL_SLOT].stroke = TURN_STROKE;                       \
        }                                                                      \
    }

static void rival_step(struct task *task)
{
    struct rival_hunter *control = task->extra.p;
    struct task_work *player;
    struct task_work *rival;
    struct player *player_record;
    struct player *rival_record;

    if (game_state.mini_event_running == 1 ||
        (game_state.time_stopped & TIME_STOPPED_BIT(RIVAL_SLOT)) ||
        game_state.cleared == 1) {
        return;
    }
    player = player_work[PLAYER_SLOT];
    rival = player_work[RIVAL_SLOT];
    player_record = player_records[PLAYER_SLOT];
    rival_record = player_records[RIVAL_SLOT];
#ifdef NONMATCHING
    /* this side repairs the tag in MODE_WAIT: while slot 1 holds no player,
     * the step returns before it reads the rival's work or record */
    if (rival == NULL) {
        return;
    }
#endif
    if ((player_record->flags & PLAYER_MISSED) ||
        (rival_record->flags & PLAYER_MISSED)) {
        control->mode = MODE_STOPPED;
    }
    switch (control->mode) {
    case MODE_WAIT:
        if (stage_get_mode() == STAGE_PLAY) {
            struct camera_bossinit camera;

            pad_disable_player(PAD_BOTH_PLAYERS);
            pause_disable();
            control->mode = MODE_INTRO;
            control->wait = INTRO_STEPS;
            if (rival_record->kind == PLAYER_KNUCKLES) {
                boss_title_create(NULL, &title_knuckles);
            } else if (rival_record->kind == PLAYER_ROUGE) {
                boss_title_create(NULL, &title_rouge);
            }
            camera.speed = INTRO_CAMERA_SPEED;
            /* @bug memory: reads slot 1's work through rival untested, here
             * at 0x8c5011ec, though the camera's move at the step's end tests
             * rival for null at 0x8c502d18. the other modes read it untested
             * too, as do keep_rival_in_arena at 0x8c5007c4 and wind_step at
             * 0x8c500f34. an empty slot 1 also leaves rival_record null, which
             * the step reads at 0x8c501110 */
            camera.ang = rival->ang.y - HALF_TURN;
            control->camera_target = rival->pos;
            control->camera_target.y = arena_bounds[1].y + INTRO_CAMERA_LIFT;
            camera.target = &control->camera_target;
            camera.start_dist = INTRO_CAMERA_START;
            camera.dist = INTRO_CAMERA_DIST;
            camera.turn = INTRO_CAMERA_TURN;
            camera_bossinit_start(0, &camera);
        }
        break;
    case MODE_INTRO:
        if (control->steps > control->wait) {
            /* @hack spelling: intro_done compared with FALSE, where its
             * negation would do */
            if (control->intro_done == FALSE) {
                NJS_POINT3 pos;
                int message = NO_MESSAGE;

                pos = arena_center[0];
                pos.y += MESSENGER_LIFT;
                control->intro_done = TRUE;
                switch (stage_get_retries()) {
                case 0:
                    break;
                case 1:
                    message = ONE_RETRY_MESSAGE;
                    break;
                case 2:
                    message = TWO_RETRIES_MESSAGE;
                    break;
                /* @hack redundant: case 3, which default covers */
                case 3:
                default:
                    message = MORE_RETRIES_MESSAGE;
                    break;
                }
                if (message != NO_MESSAGE) {
                    if (player_record->character == PLAYER_ROUGE) {
                        message += RETRY_MESSAGES;
                    }
                    messenger_create(&pos, 0, PLAYER_SLOT, message,
                                     MESSENGER_SUMMON, NULL);
                }
            }
            pad_enable_player(PLAYER_SLOT);
            pause_enable();
            control->steps = 0;
            control->mode = MODE_RECOVER;
            control->wait = RECOVER_STEPS;
            camera_bossinit_end(0);
            {
                struct camera_bosspoint camera;

                camera.target = &control->camera_target;
                camera.dist = PLAY_CAMERA_DIST;
                camera.height = PLAY_CAMERA_HEIGHT;
                camera.part = PLAY_CAMERA_PART;
                camera.adjust = CAMERA_ADJUST_RELATIVE3;
                camera.collides = 1;
                camera.trigger_push = 0;
                camera.lift = PLAY_CAMERA_LIFT;
                camera.level = 0;
                camera_bosspoint_start(0, &camera);
            }
        } else if (control->steps == INTRO_VOICE_STEP) {
            int voice = NO_VOICE;
            int i;

            if (rival_record->kind == PLAYER_KNUCKLES) {
                voice = VOICE_KNUCKLES_INTRO;
            } else if (rival_record->kind == PLAYER_ROUGE) {
                voice = VOICE_ROUGE_INTRO;
            }
            if (voice != NO_VOICE) {
                for (i = 0; i < VOICES; i++) {
                    /* @hack spelling: voice_can_start's result compared with
                     * TRUE, where the result alone would do */
                    if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                voice_play(voice, RIVAL_VOICE_PRIORITY);
            }
        }
        break;
    case MODE_CHASE:
        if (rival_record->gauge < WEAK_GAUGE) {
            control->mode = MODE_WEAK;
        }
        /* fall through */
    case MODE_CHASE_AGAIN:
        if (rival_in_anim_23(control, rival_record)) {
            control->steps = 0;
            control->mode = MODE_ANIM_23;
            break;
        }
        if (player_threatens(control, player, rival)) {
            if (njDistanceP2P(&player->pos, &rival->pos) > REACH) {
                control->steps = 0;
                control->mode = MODE_COMBO;
                pad_action_pressed[RIVAL_SLOT] = 1;
                pad_action_held[RIVAL_SLOT] = 1;
            } else {
                control->steps = 0;
                control->mode = MODE_JUMP;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
                /* @hack redundant: a break that ends the arm, where the
                 * break after the if and its else would do */
                break;
            }
            break;
        }
        CHASE_STEER(control, player, rival);
        if (rival->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->mode = MODE_CLIMB;
            control->steps = 0;
            control->wait = CLIMB_STEPS;
            break;
        }
        if (player_close(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_COMBO;
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
            break;
        }
        if (control->steps > CHASE_STEPS) {
            int choice = (int)((Float)rand() * (CHASE_SPREAD * RAND_SCALE));

            /* @hack spelling: wind compared with TRUE, where its truth
             * would do */
            if (control->wind == TRUE) {
                choice = CHASE_JUMP;
            } else if (rival_record->gauge <= LOW_GAUGE) {
                /* @hack type: the raise added to choice as a Float, where an
                 * int would do */
                choice += LOW_RAISE;
            } else if (rival_record->gauge < MIDDLE_GAUGE) {
                /* @hack type: the raise added to choice as a Float, where an
                 * int would do */
                choice += MIDDLE_RAISE;
            }
            switch (choice) {
            case 0:
            case 1:
            case 2:
                control->steps = 0;
                control->mode = MODE_CIRCLE;
                control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
                break;
            default:
                control->steps = 0;
                control->mode = MODE_JUMP;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
                break;
            }
        }
        break;
    case MODE_WEAK:
        if (rival_record->gauge >= WEAK_GAUGE) {
            control->mode = MODE_CHASE;
        }
        CHASE_STEER(control, player, rival);
        if (rival->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->mode = MODE_CLIMB;
            control->steps = 0;
            control->wait = CLIMB_STEPS;
            break;
        }
        if (!players_far(player, rival) &&
            player_threatens(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_JUMP;
            pad_jump_pressed[RIVAL_SLOT] = 1;
            pad_jump_held[RIVAL_SLOT] = 1;
            break;
            /* @hack spelling: the test of the steps the else of a test whose
             * arm ends in a break, where a test of its own would do */
        } else if (control->steps > WEAK_STEPS) {
            if (!(player_record->flags & PLAYER_MISSED) &&
                !(rival_record->flags & PLAYER_MISSED) &&
                players_apart(player, rival)) {
                int i;

                control->steps = 0;
                control->mode = MODE_EARTHFLAME;
                game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_EARTHQUAKE] = 0;
                game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_EARTHFLAME] = 1;
                game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_POWER_FLASH] = 0;
                for (i = 0; i < VOICES; i++) {
                    /* @hack spelling: voice_can_start's result compared with
                     * TRUE, where the result alone would do */
                    if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                switch (rival_record->character) {
                case PLAYER_KNUCKLES:
                    voice_play(VOICE_KNUCKLES_EARTHFLAME, RIVAL_VOICE_PRIORITY);
                    break;
                case PLAYER_ROUGE:
                    voice_play(VOICE_ROUGE_EARTHFLAME, RIVAL_VOICE_PRIORITY);
                    break;
                }
                game_state.ring_level_timer[RIVAL_SLOT] = RING_LEVEL_STEPS;
                break;
            }
            if (player->mode == PLAYER_MODE_HUNTER_CLIMB ||
                (player->pos.y >= rival->pos.y + HIGH_GROUND &&
                 (rival->flags & PLAYER_WORK_GROUNDED))) {
                rival_offer_earthquake(control, rival_record);
                break;
            }
            if (!players_far(player, rival)) {
                control->steps = 0;
                control->mode = MODE_JUMP;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
                /* @hack redundant: a break that ends the arm, where the
                 * break after the test would do */
                break;
            }
        }
        break;
    case MODE_CIRCLE:
        /* @hack spelling: wind compared with TRUE, where its truth would do */
        if (control->wind == TRUE) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if (player->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_PLAYER_CLIMBS;
            break;
        }
        if (player_close(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_COMBO;
            break;
        }
        if (player_threatens(control, player, rival)) {
            if (njDistanceP2P(&player->pos, &rival->pos) > REACH) {
                control->steps = 0;
                control->mode = MODE_PUNCHES;
                pad_action_pressed[RIVAL_SLOT] = 1;
                pad_action_held[RIVAL_SLOT] = 1;
            } else {
                control->steps = 0;
                control->mode = MODE_JUMP;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
                /* @hack redundant: a break that ends the arm, where the
                 * break after the if and its else would do */
                break;
            }
            break;
        }
        if (control->steps > control->wait) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        {
            NJS_POINT3 pos;
            Angle heading;

            pos.x =
                player->pos.x +
                (njSin(game_state.steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS);
            pos.y = player->pos.y;
            pos.z =
                player->pos.z +
                (njCos(game_state.steps << CIRCLE_TURN_SHIFT) * CIRCLE_RADIUS);
            heading = STEP_HEADING(pos.x - rival->pos.x, pos.z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        break;
    case MODE_JUMP:
        if (control->steps < TAKE_OFF_STEPS ||
            !(rival->flags & PLAYER_WORK_GROUNDED)) {
            pad_jump_held[RIVAL_SLOT] = 1;
            if (rival_record->spd.y == 0.0F &&
                rival->pos.y - player_record->ground.below_y >
                    JUMP_TOP_HEIGHT) {
                control->mode = MODE_JUMP_TOP;
                control->steps = 0;
            }
        } else {
            control->mode = MODE_CHASE;
            control->steps = 0;
        }
        break;
    case MODE_JUMP_TOP:
        if (control->steps < TAKE_OFF_STEPS ||
            !(rival->flags & PLAYER_WORK_GROUNDED)) {
            if (rival_record->spd.y == 0.0F &&
                rival->pos.y - player_record->ground.below_y >
                    JUMP_TOP_HEIGHT) {
                control->mode = MODE_GLIDE;
                control->steps = 0;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
                control->wait = RANDOM_STEPS(GLIDE_LEAST, GLIDE_SPREAD);
            }
        } else {
            control->mode = MODE_CHASE_AGAIN;
        }
        break;
    case MODE_GLIDE:
        if (rival->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->mode = MODE_CLIMB;
            control->steps = 0;
            control->wait = RANDOM_STEPS(CLIMB_LEAST, CLIMB_SPREAD);
            break;
        }
        if (player->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_PLAYER_CLIMBS;
            break;
        }
        if (rival->flags & PLAYER_WORK_GROUNDED) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if (control->steps > control->wait) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        rival_steer_to_corner(control, player, rival);
        pad_jump_pressed[RIVAL_SLOT] = 0;
        pad_jump_held[RIVAL_SLOT] = 1;
        break;
    case MODE_GLIDE_AT:
        if (rival->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->mode = MODE_CLIMB;
            control->steps = 0;
            control->wait = RANDOM_STEPS(CLIMB_LEAST, CLIMB_SPREAD);
            break;
        }
        if (player->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_PLAYER_CLIMBS;
            break;
        }
        if (rival->flags & PLAYER_WORK_GROUNDED) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if (control->steps > control->wait) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        {
            Angle heading = STEP_HEADING(player->pos.x - rival->pos.x,
                                         player->pos.z - rival->pos.z);

            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        if (rival_over_player(control, player, rival)) {
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
            control->mode = MODE_DIVE;
            control->steps = 0;
            break;
        }
        if (control->steps < GLIDE_PRESS_STEP) {
            pad_jump_pressed[RIVAL_SLOT] = 0;
            pad_jump_held[RIVAL_SLOT] = 0;
        } else if (control->steps == GLIDE_PRESS_STEP) {
            pad_jump_pressed[RIVAL_SLOT] = 1;
            pad_jump_held[RIVAL_SLOT] = 1;
        } else {
            pad_jump_pressed[RIVAL_SLOT] = 0;
            pad_jump_held[RIVAL_SLOT] = 1;
        }
        break;
    case MODE_COMBO:
        if (player_record->last_flags & PLAYER_WORK_GROUNDED) {
            Angle heading = STEP_HEADING(player->pos.x - rival->pos.x,
                                         player->pos.z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = TURN_STROKE;
        } else if (player->mode == PLAYER_MODE_FALL) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if ((control->steps & COMBO_MASK) == COMBO_PUNCH_1 ||
            (control->steps & COMBO_MASK) == COMBO_PUNCH_2 ||
            (control->steps & COMBO_MASK) == COMBO_PUNCH_3) {
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
            if (player_above(control, player, rival)) {
                /* @hack spelling: player 1's record read again through its
                 * task's extra, where rival_record would do */
                struct player *hunter =
                    player_tasks[rival_record->slot]->extra.p;
                hunter->by_character.hunter.spin_steps = SPIRAL_OFFER_STEPS;
            } else if (rival_faces_player(control, player, rival) &&
                       (control->steps & COMBO_MASK) == COMBO_PUNCH_3) {
                control->mode = MODE_CHASE;
                control->steps = 0;
            }
        } else if (control->steps > COMBO_STEPS ||
                   !(player_record->last_flags & PLAYER_WORK_GROUNDED)) {
            control->steps = 0;
            control->mode = MODE_CIRCLE;
            control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
        }
        break;
    case MODE_PUNCHES:
        if (player_record->last_flags & PLAYER_WORK_GROUNDED) {
            Angle heading = STEP_HEADING(player->pos.x - rival->pos.x,
                                         player->pos.z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = TURN_STROKE;
        } else if (player->mode == PLAYER_MODE_FALL) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if (!(control->steps & PUNCH_MASK)) {
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
            if (rival_faces_player(control, player, rival) &&
                !(control->steps & COMBO_MASK)) {
                control->mode = MODE_CHASE;
                control->steps = 0;
            }
        } else if (control->steps > PUNCHES_STEPS) {
            control->steps = 0;
            control->mode = MODE_CIRCLE;
            control->wait =
                RANDOM_STEPS(SHORT_CIRCLE_LEAST, SHORT_CIRCLE_SPREAD);
        }
        break;
    case MODE_DIVE:
        if (!(rival_record->last_flags & PLAYER_WORK_GROUNDED)) {
            Angle heading = STEP_HEADING(player->pos.x - rival->pos.x,
                                         player->pos.z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
            pad_action_pressed[RIVAL_SLOT] = 0;
            pad_action_held[RIVAL_SLOT] = 1;
        } else {
            control->mode = MODE_LANDED;
            control->steps = 0;
        }
        break;
    case MODE_LANDED:
        if (rival->mode >= PLAYER_MODE_HUNTER_DIG &&
            rival->mode <= PLAYER_MODE_HUNTER_DIG_FAIL) {
            break;
        }
        if (!(rival_record->last_flags & PLAYER_WORK_GROUNDED)) {
            control->mode = MODE_JUMP_TOP;
            control->steps = 0;
        } else {
            control->mode = MODE_CHASE;
            control->steps = 0;
        }
        break;
    case MODE_DODGE:
        /* @hack spelling: wind compared with TRUE, where its truth would do */
        if (control->wind == TRUE) {
            control->mode = MODE_CHASE;
            control->steps = 0;
        } else if (player->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_PLAYER_CLIMBS;
        } else if (control->steps > control->wait) {
            control->mode = MODE_CHASE;
            control->steps = 0;
        } else {
            /* @hack redundant: unused, a local nothing reads, which only
             * sizes the frame */
            NJS_VECTOR unused;
            NJS_VECTOR delta;
            Angle heading;

            delta = player->pos;
            njSubVector(&delta, &rival->pos);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, QUARTER_TURN);
            njCalcVector(NULL, &delta, &delta);
            njPopMatrixEx();
            njUnitVector(&delta);
            heading = STEP_HEADING(delta.x, delta.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        break;
    case MODE_RECOVER:
        if (control->steps > control->wait || rival->timer > 0) {
            control->mode = MODE_CHASE;
            control->steps = 0;
        } else if (player_threatens(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_PUNCHES;
        }
        break;
    case MODE_HURT_PUNCHES:
        /* @hack spelling: wind compared with TRUE, where its truth would do */
        if (control->wind == TRUE) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
            /* @hack spelling: the heading in the else of a test whose arm
             * ends in a break, where a block of its own would do */
        } else {
            Angle heading = STEP_HEADING(player->pos.x - rival->pos.x,
                                         player->pos.z - rival->pos.z);

            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        if (rival->timer <= 0) {
            control->steps = 0;
            control->mode = MODE_CIRCLE;
            control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
        } else if (!(control->steps & PUNCH_MASK)) {
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
            control->steps = 0;
        }
        break;
    case MODE_CLIMB:
        if (player->mode == PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_PLAYER_CLIMBS;
            pad_jump_pressed[RIVAL_SLOT] = 1;
            pad_jump_held[RIVAL_SLOT] = 1;
        } else if (rival->mode != PLAYER_MODE_HUNTER_CLIMB) {
            control->steps = 0;
            control->mode = MODE_CHASE;
        } else if (player_above(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_GLIDE_AT;
            pad_jump_pressed[RIVAL_SLOT] = 1;
            pad_jump_held[RIVAL_SLOT] = 1;
        } else {
            if (rival->pos.y > CLIMB_HIGH) {
                pad_sticks[RIVAL_SLOT].ang = 0;
                pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
            } else if (rival->pos.y < CLIMB_LOW) {
                pad_sticks[RIVAL_SLOT].ang = -HALF_TURN;
                pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
            }
            if (control->steps >= control->wait) {
                control->steps = 0;
                control->mode = MODE_GLIDE_AT;
                pad_jump_pressed[RIVAL_SLOT] = 1;
                pad_jump_held[RIVAL_SLOT] = 1;
            }
        }
        break;
    case MODE_PLAYER_CLIMBS:
        /* @hack spelling: wind compared with TRUE, where its truth would do */
        if (control->wind == TRUE) {
            if (rival->flags & PLAYER_WORK_GROUNDED) {
                rival_offer_earthquake(control, rival_record);
            } else {
                control->mode = MODE_CHASE;
                control->steps = 0;
            }
            break;
            /* @hack spelling: the heading in the else of a test whose arm
             * ends in a break, where a block of its own would do */
        } else {
            Angle heading = STEP_HEADING(arena_center->x - rival->pos.x,
                                         arena_center->z - rival->pos.z);
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        if (rival->timer > 0) {
            control->mode = MODE_CHASE;
            control->steps = 0;
        } else if (player_close(control, player, rival) &&
                   player_threatens(control, player, rival)) {
            control->steps = 0;
            control->mode = MODE_PUNCHES;
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
        } else {
            Float distance = njDistanceP2P(&arena_center[0], &rival->pos);
            if (distance < OVER_RADIUS) {
                rival_offer_earthquake(control, rival_record);
            } else if (control->steps >= PLAYER_CLIMBS_STEPS) {
                control->steps = 0;
                control->mode = MODE_CIRCLE;
                control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
            }
        }
        /* @bug logic: MODE_PLAYER_CLIMBS ends on its first step without a
         * threat, where game_state.ring_level_timer[1] is 0, its usual value.
         * the case falls through into MODE_EARTHQUAKE's, which ends the mode
         * on a step without a threat at 0x8c5028cc. it does so where the
         * timer is 0 or less, or the steps reach EARTHQUAKE_STEPS. so the test
         * of PLAYER_CLIMBS_STEPS at 0x8c50284c holds only where player 0
         * threatens player 1 on every step past EARTHQUAKE_STEPS */
        /* fall through */
    case MODE_EARTHQUAKE:
        if (rival->timer > 0) {
            control->mode = MODE_CHASE;
            control->steps = 0;
        } else if (player_threatens(control, player, rival)) {
            if (njDistanceP2P(&player->pos, &rival->pos) < REACH) {
                pad_action_pressed[RIVAL_SLOT] = 1;
                pad_action_held[RIVAL_SLOT] = 1;
            }
        } else if (game_state.ring_level_timer[RIVAL_SLOT] <= 0 ||
                   control->steps >= EARTHQUAKE_STEPS) {
            control->steps = 0;
            /* @hack spelling: wind compared with TRUE, where its truth
             * would do */
            if (control->wind != TRUE) {
                control->mode = MODE_CIRCLE;
                control->wait = RANDOM_STEPS(CIRCLE_LEAST, CIRCLE_SPREAD);
            } else {
                control->mode = MODE_RECOVER;
                control->wait = WIND_RECOVER_STEPS;
            }
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
        }
        break;
    case MODE_EARTHFLAME:
        if (rival->timer > 0 &&
            game_state.ring_levels[RIVAL_SLOT][RING_LEVEL_EARTHFLAME] == 0) {
            control->mode = MODE_CHASE;
            control->steps = 0;
            break;
        }
        if (game_state.ring_level_timer[RIVAL_SLOT] <= 0) {
            /* @hack spelling: wind compared with TRUE, where its truth
             * would do */
            if (control->wind == TRUE) {
                control->steps = 0;
                control->wait = WIND_RECOVER_STEPS;
                control->mode = MODE_RECOVER;
            } else {
                control->steps = 0;
                control->mode = MODE_CIRCLE;
                control->wait =
                    RANDOM_STEPS(CIRCLE_LEAST, EARTHFLAME_CIRCLE_SPREAD);
            }
            pad_action_pressed[RIVAL_SLOT] = 1;
            pad_action_held[RIVAL_SLOT] = 1;
        } else if (player_threatens(control, player, rival)) {
            if (njDistanceP2P(&player->pos, &rival->pos) < REACH) {
                pad_action_pressed[RIVAL_SLOT] = 1;
                pad_action_held[RIVAL_SLOT] = 1;
            }
        }
        break;
    case MODE_ANIM_23:
        if (!rival_in_anim_23(control, rival_record)) {
            control->steps = 0;
            control->mode = MODE_CHASE;
        } else {
            /* @hack redundant: unused, a local nothing reads, which only
             * sizes the frame */
            NJS_POINT3 unused;
            Angle heading;

            if (rival->pos.y < CENTER_RUN_LOW) {
                heading = STEP_HEADING(arena_center->x - rival->pos.x,
                                       arena_center->z - rival->pos.z);
            } else if (njAbs(rival->pos.x) > njAbs(rival->pos.z)) {
                if (rival->pos.x > 0.0F) {
                    if (player->pos.x > rival->pos.x) {
                        heading = -HALF_TURN;
                    } else {
                        heading = 0;
                    }
                } else {
                    if (player->pos.x < rival->pos.x) {
                        heading = 0;
                    } else {
                        heading = -HALF_TURN;
                    }
                }
            } else {
                if (rival->pos.z > 0.0F) {
                    /* @bug logic: on the side where z is above 0, player 1
                     * runs toward player 0, where on the other three sides it
                     * runs away. the test at 0x8c502b6e, player 0's z below
                     * player 1's, takes -QUARTER_TURN, toward lower z */
                    if (player->pos.z < rival->pos.z) {
                        heading = -QUARTER_TURN;
                    } else {
                        heading = QUARTER_TURN;
                    }
                } else {
                    if (player->pos.z < rival->pos.z) {
                        heading = QUARTER_TURN;
                    } else {
                        heading = -QUARTER_TURN;
                    }
                }
            }
            pad_sticks[RIVAL_SLOT].ang = heading;
            pad_sticks[RIVAL_SLOT].stroke = RUN_STROKE;
        }
        break;
    }
    keep_player_in_arena(control, player);
    keep_rival_in_arena(control, rival);
    wind_step(control, player, rival);
    /* @hack spelling: wind compared with FALSE, where its negation would
     * do */
    if (control->wind == FALSE) {
        control->vent_frame =
            float_approach(control->vent_frame, 0.0F, VENT_FRAME_STEP);
    } else {
        control->vent_frame =
            float_approach(control->vent_frame, vent_motion[0].nbFrame - 1.0F,
                           VENT_FRAME_STEP);
    }
    if (!game_state.steps ||
        (stage_get_mode() == STAGE_FADE_IN && control->mode != MODE_WAIT &&
         control->mode != MODE_INTRO)) {
        rival_record->gauge = FULL_GAUGE;
        if (task->child) {
            task_destroy(task->child);
        }
        control->mode = MODE_WAIT;
        control->steps = 0;
        control->wait = START_WAIT;
        control->intro_done = FALSE;
        control->wind_steps = 0;
    } else {
        control->steps++;
        control->wind_steps++;
    }
    if (rival_record->gauge > 0.0F && !task->child) {
        boss_gauge_create(task, &player_records[RIVAL_SLOT]->gauge);
    }
    if (!(rival_record->gauge > 0.0F)) {
        player_look_at_task(PLAYER_SLOT, player_tasks[RIVAL_SLOT], NULL,
                            LOOK_STEPS);
    }
    if (rival && control->mode != MODE_INTRO && control->mode != MODE_WAIT) {
        NJS_VECTOR delta;
        Float dy;

        delta = rival->pos;
        njSubVector(&delta, &player->pos);
        dy = delta.y;
        delta.y = 0.0F;
        /* @hack spelling: wind compared with TRUE, where its truth would do */
        if (control->wind == TRUE) {
            delta = player->pos;
            njSubVector(&delta, &arena_center[0]);
            delta.y = 0.0F;
            if (njScalor(&delta) > CAMERA_WIND_CENTER) {
                njUnitVector(&delta);
                delta.x = player->pos.x + (delta.x * CAMERA_WIND_AHEAD);
                delta.y = player->pos.y - CAMERA_WIND_DROP;
                delta.z = player->pos.z + (delta.z * CAMERA_WIND_AHEAD);
                if (njScalor(&delta) > CAMERA_WIND_FAR) {
                    delta.x = (delta.x * CAMERA_EASE) +
                              (control->camera_target.x * CAMERA_KEEP);
                    delta.y = (delta.y * CAMERA_EASE) +
                              (control->camera_target.y * CAMERA_KEEP);
                    delta.z = (delta.z * CAMERA_EASE) +
                              (control->camera_target.z * CAMERA_KEEP);
                } else {
                    Float rate = njScalor(&delta) * CAMERA_WIND_RATE;
                    Float rest = 1.0F - rate;
                    delta.x =
                        (delta.x * rate) + (control->camera_target.x * rest);
                    delta.y =
                        (delta.y * rate) + (control->camera_target.y * rest);
                    delta.z =
                        (delta.z * rate) + (control->camera_target.z * rest);
                }
                control->camera_target = delta;
            }
        } else if (njScalor(&delta) > CAMERA_FAR) {
            njUnitVector(&delta);
            delta.x *= CAMERA_FAR_AHEAD;
            delta.y -= CAMERA_FAR_DROP;
            delta.z *= CAMERA_FAR_AHEAD;
            njAddVector(&delta, &player->pos);
            control->camera_target = delta;
        } else if (njAbs(dy) > CAMERA_HIGH) {
            njUnitVector(&delta);
            delta.x *= CAMERA_HIGH_AHEAD;
            delta.z *= CAMERA_HIGH_AHEAD;
            if (dy > 0.0F) {
                delta.y = CAMERA_ABOVE;
            } else {
                delta.y = CAMERA_BELOW;
            }
            njAddVector(&delta, &player->pos);
            control->camera_target = delta;
        } else {
            control->camera_target = rival->pos;
            control->camera_target.y += CAMERA_LIFT;
        }
    }
}

static void release_knuckles_title(struct task *task)
{
    njReleaseTexture(&title_knuckles_texlist);
}

static void release_rouge_title(struct task *task)
{
    njReleaseTexture(&title_rouge_texlist);
}

void rival_hunter_create(void)
{
    struct task *task;
    struct rival_hunter *control;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(task = task_create(0, CONTROL_LEVEL, rival_step, "ControlRouge"))) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(control = syCalloc(1, sizeof(struct rival_hunter)))) {
        task_kill(task);
        return;
    }
    task->extra.p = control;
    player_records[RIVAL_SLOT]->gauge = START_GAUGE;
    boss_gauge_create(task, &player_records[RIVAL_SLOT]->gauge);
    control->mode = MODE_WAIT;
    task->draw = rival_draw;
    if (player_records[RIVAL_SLOT]->kind == PLAYER_KNUCKLES) {
        texture_load_pvm(KNUCKLES_TITLE_ARCHIVE, &title_knuckles_texlist);
        task->destructor = release_knuckles_title;
    } else if (player_records[RIVAL_SLOT]->kind == PLAYER_ROUGE) {
        texture_load_pvm(ROUGE_TITLE_ARCHIVE, &title_rouge_texlist);
        task->destructor = release_rouge_title;
    }
}

/* draw a wind particle. the manager's first child sets the draw up for all
 * of them. its last child puts it back */
static void wind_particle_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale = work->scl.x;

    if (task->parent->child == task) {
        njPushMatrixEx();
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&stg33_objtex_texlist);
    } else {
        matrix_load_parent();
    }
    njSetTextureNum(work->timer / PARTICLE_FRAME_STEPS);
    njTranslateEx(&work->pos);
    njScale(NULL, scale, scale, scale);
    njRotateEx(&camera_views[camera_current_index]->ang.x, 0);
    njRotateZ(NULL, work->ang.z);
    njDrawTexture3DEx(wind_quad, QUAD_VERTICES, TRUE);
    if (task->parent->child->prev == task) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njFogEnable();
        njPopMatrixEx();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
}

static void wind_particle_step(struct task *task)
{
    struct task_work *work = task->work;

    if (++work->timer >= PARTICLE_STEPS) {
        work->timer = PARTICLE_STEPS - 1;
        task_kill(task);
    } else {
        work->pos.y += PARTICLE_RISE;
    }
}

static void wind_manager_step(struct task *task)
{
}

static void wind_manager_destroy(struct task *task)
{
    if (wind_manager == task) {
        wind_manager = NULL;
    }
}

/* make a wind particle under EffectSpriteKvsRMan, which it makes first
 * where there is none. it also shakes player 0's pad, as WIND_SHAKE_MASK
 * says */
static void wind_particle_create(void)
{
    struct task *task;
    struct task_work *work;

    if (!wind_manager) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((wind_manager = task_create(0, PARTICLE_LEVEL, wind_manager_step,
                                        "EffectSpriteKvsRMan"))) {
            wind_manager->destructor = wind_manager_destroy;
        } else {
            return;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task =
             task_create_child(TASK_WORK, wind_particle_step, wind_manager))) {
        work = task->work;
        work->pos.x = -(PARTICLE_SPREAD / 2) +
                      ((Float)rand() * (PARTICLE_SPREAD * RAND_SCALE));
        work->pos.y =
            PARTICLE_LOW + ((Float)rand() * (PARTICLE_HEIGHT * RAND_SCALE));
        work->pos.z = -(PARTICLE_SPREAD / 2) +
                      ((Float)rand() * (PARTICLE_SPREAD * RAND_SCALE));
        work->scl.x = PARTICLE_SCALE +
                      ((Float)rand() * (PARTICLE_SCALE_SPREAD * RAND_SCALE));
        work->ang.z = (Float)rand() * TURN_PER_RAND;
        task->draw = wind_particle_draw;
    }
    if (!(game_state.steps & WIND_SHAKE_MASK)) {
        vibration_start(PLAYER_SLOT, VIBRATION_POWER, VIBRATION_FREQ,
                        VIBRATION_UNTIMED);
    }
}

static const struct vent_texture_names vent_texture_names = {
#include "stg33/rival_hunter/vent_texture_names.inc"
};
