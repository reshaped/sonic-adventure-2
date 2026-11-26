/* what Tails's and Eggman's mechs fire, the effects they make, and their
 * gauge */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/powder.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "player/walker_lock_assets.h"
#include "player/walker_weapons.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the size of the extra that tracer_task_create, puff_task_create and
 * shot_task_create allocate. struct tracer, struct puff, struct homing, struct
 * laser and struct ricochet are each EXTRA_SIZE bytes */
#define EXTRA_SIZE 32

/* the task levels of the tasks made without a parent */
#define TRACER_LEVEL 3
#define SHOT_LEVEL 3
#define PUFF_LEVEL 5
#define JET_LEVEL 3

/* with either upgrade, a player's tracers are paler, and its ricochets take a
 * cell of its character's own */
#define LASER_UPGRADES                                                         \
    (UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER) |                               \
     UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN))

/* the models of the rounds, the lasers and the missiles, by their slots in
 * model_table. a laser has three models, in the three slots from
 * EGGMAN_LASER_MODELS or TAILS_LASER_MODELS on */
#define BULLET_MODEL 267
#define EGGMAN_SHELL_MODEL 272
#define TAILS_SHELL_MODEL 327
#define EGGMAN_LASER_MODELS 288
#define TAILS_LASER_MODELS 322
#define EGGMAN_MISSILE_MODEL 292
#define TAILS_MISSILE_MODEL 326

/* the first of each effect's textures in a mech's effects_texlist. a puff
 * shows each of its textures for 1 << PUFF_TEXTURE_SHIFT steps */
#define DUST_FIRST_TEXTURE 5
#define SMOKE_FIRST_TEXTURE 13
#define FLASH_FIRST_TEXTURE 21
#define JET_FIRST_TEXTURE 29
#define EXPLOSION_FIRST_TEXTURE 33
#define PUFF_TEXTURE_SHIFT 2

#define QUAD_VERTICES 4

/* a tracer is a streak along one move of a shot. it fades out over TRACER_STEPS
 * steps. its draw stretches tracer_strip from pos, turned by ang, to length */
#define TRACER_STEPS 14
#define TRACER_STRIP_VERTICES 8
#define TRACER_COLORS 16
struct tracer {
    Sint8 bright; /* 1 where the player has a LASER_UPGRADES upgrade, else 0 */
    Sint8 player;
    Sint16 steps; /* the steps taken, which pick its colors */
    struct angles ang;
    NJS_POINT3 pos;
    Float length;
};

/* a round is a bullet or a shell. its work keeps its speed in scl. its work's
 * mode is ROUND_FLY, or ROUND_HIT once it has hit something */
#define ROUND_FLY 0
#define ROUND_HIT 1
/* a round's shape ignores the players through its first ROUND_GRACE_STEPS
 * steps */
#define ROUND_GRACE_STEPS 2
/* a round heads straight along x where its speed's z lies within ROUND_TINY_Z
 * of 0. it heads the way the speed's x points */
#define ROUND_TINY_Z 1e-5F
/* a round's enemy_work has a top and a radius of ROUND_SIZE, and a bottom of
 * -ROUND_SIZE. its damage is ROUND_DAMAGE, and its wall_bounce and floor_bounce
 * ROUND_BOUNCE */
#define ROUND_SIZE 3.0F
#define ROUND_DAMAGE 2.0F
#define ROUND_BOUNCE 0.8F
/* a round's hit gives off ROUND_HIT_POWDER specks of powder. they move at
 * BULLET_POWDER_SHARE of a bullet's speed, or SHELL_POWDER_SHARE of a shell's
 */
#define ROUND_HIT_POWDER 16
#define BULLET_POWDER_SHARE 0.2F
#define SHELL_POWDER_SHARE 0.4F
#define ROUND_HIT_SOUND 0x200b
/* the radius of the shape of a round, a volley shot and a missile */
#define SHOT_RADIUS 2.0F

/* each of a lock-on line's first LOCKON_LINE_STEPS steps keeps LOCKON_LINE_KEEP
 * of its red, green and blue. its next step ends it */
#define LOCKON_LINE_STEPS 5
#define LOCKON_LINE_KEEP 0.9F
/* the mech's lock_window picks the color the line starts in, one of
 * LOCKON_LINE_BANDS in lockon_line_colors. above LOCKON_LINE_BAND it picks the
 * first. above half of LOCKON_LINE_BAND it picks the second. above a quarter it
 * picks the third, and above an eighth the fourth. at an eighth or less it
 * picks the last */
#define LOCKON_LINE_BANDS 5
#define LOCKON_LINE_BAND 120
struct lockon_line {
    Sint32 steps_left; /* the steps it has left to fade */
    NJS_POINT3 points[2];
    NJS_COLOR colors[2];
};

/* a puff is a quad that faces the camera. it lasts PUFF_STEPS steps. each step
 * moves it by spd, and then multiplies spd by PUFF_FRICTION */
#define PUFF_STEPS 32
#define PUFF_FRICTION 0.95F
#define FLASH_POWDER 8 /* the specks of powder a flash gives off */
/* a puff that rises moves against world_gravity by PUFF_LIFT of it. one that
 * sinks moves with it by as much */
#define PUFF_LIFT 0.08F
/* a back blast's flash starts BACK_FLASH_BEHIND behind pos, and flies back at
 * BACK_FLASH_SPEED a step */
#define BACK_FLASH_BEHIND 15.0F
#define BACK_FLASH_SPEED 0.7F
/* the shares of spd that a back blast's smoke, and a muzzle blast's flash and
 * smoke, move at */
#define BACK_SMOKE_SHARE 0.2F
#define MUZZLE_FLASH_SHARE 0.2F
#define MUZZLE_SMOKE_SHARE 0.05F
/* the sizes of the puffs of a back blast, of a muzzle blast, and of
 * walker_puff_create */
#define BACK_FLASH_SIZE 0.3F
#define BACK_SMOKE_SIZE 0.2F
#define MUZZLE_FLASH_SIZE 0.2F
#define MUZZLE_SMOKE_SIZE 0.3F
#define PUFF_DUST_SIZE 0.15F
/* walker_ground_dust_create makes DUST_NEAR_PUFFS puffs of DUST_NEAR_SIZE where
 * the ground lies within DUST_NEAR. it makes DUST_FAR_PUFFS of DUST_FAR_SIZE
 * where the ground lies farther, but within DUST_REACH. each puff starts
 * DUST_START out from the point below, and moves out at DUST_SPEED */
#define DUST_NEAR 30.0F
#define DUST_REACH 40.0F
#define DUST_NEAR_PUFFS 2
#define DUST_NEAR_SIZE 0.3F
#define DUST_FAR_PUFFS 1
#define DUST_FAR_SIZE 0.2F
#define DUST_START 6.0F
#define DUST_SPEED 1.0F
/* walker_foot_dust_create makes FOOT_DUST_PUFFS puffs of FOOT_DUST_SIZE. each
 * moves at a random share of the call's spd. the share runs from
 * FOOT_DUST_SHARE_MIN up to FOOT_DUST_SHARE_SPAN more */
#define FOOT_DUST_PUFFS 2
#define FOOT_DUST_SIZE 0.2F
#define FOOT_DUST_SHARE_MIN 0.35F
#define FOOT_DUST_SHARE_SPAN 0.15F
/* what a puff keeps in its task's extra */
struct puff {
    Sint8 texture; /* the first of its textures */
    Sint8 player;
    Sint16 steps; /* the steps taken, which pick its texture */
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float size;
};

/* a flame's work, in place of its task's task_work. spd lies where
 * task_work's ang does, and life, life_step and scale where its scl does */
struct jet_work {
    Sint8 kind; /* JET_ */
    Sint8 player;
    Uint8 unk_02;
    Uint8 texture; /* the flame draws texture JET_FIRST_TEXTURE + texture */
    Sint16 unk_04;
    Uint16 timer; /* the steps that faded it. nothing reads it */
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    /* from 1 down to 0, the flame's brightness and opacity */
    Float life;
    Float life_step; /* what each step that fades the flame takes from life */
    Float scale;
};
#define JET(task) ((struct jet_work *)(task)->work)
/* a flame's texture is rand() times JET_TEXTURE_SPAN * RAND_SCALE, 0 to 3 */
#define JET_TEXTURE_SPAN 3.99F
/* each byte of a flame's color is its life times JET_BRIGHTEST */
#define JET_BRIGHTEST 255.0F
/* a flame's step destroys it once its life is JET_GONE or less */
#define JET_GONE 0.05F
/* each kind's fade is what a step takes from its life. its scale runs from a
 * least up to a span more. JET_SCALED's scale is SMALL_'s times the scale
 * walker_jet_create takes. OTHER_ is JET_LASTING's, and any other kind's */
#define SCALED_FADE 0.25F
#define SMALL_FADE 0.17F
#define SMALL_SCALE_MIN 0.12F
#define SMALL_SCALE_SPAN 0.03F
#define LARGE_FADE 0.12F
#define LARGE_SCALE_MIN 0.27F
#define LARGE_SCALE_SPAN 0.05F
#define OTHER_FADE 0.1F
#define OTHER_SCALE_MIN 0.15F
#define OTHER_SCALE_SPAN 0.04F

/* an explosion is a struct puff that stands still: nothing sets or reads its
 * spd. it is a sprite of EXPLOSION_SIZE that shows EXPLOSION_STEPS textures in
 * turn, one a step. its task's unk_3c.l holds the angle the sprite turns by */
#define EXPLOSION_STEPS 16
#define EXPLOSION_SIZE 0.25F

/* the gauge is a frame, a bar inside it, and a meter above the bar.
 * GAUGE_START is the x on the screen where the bar starts in a one-player game.
 * a two-player game puts slot 0's at GAUGE_START_LEFT and the other's at
 * GAUGE_START_RIGHT. a full bar is GAUGE_LENGTH long */
#define GAUGE_START 244.0F
#define GAUGE_START_LEFT 100.0F
#define GAUGE_START_RIGHT 388.0F
#define GAUGE_LENGTH 152.0F
/* the meter's top and bottom y, and its z */
/* The frame is a strip with two vertices per column. Six segments sit
 * between its end caps, and both caps extend past the bar. */
#define GAUGE_FRAME_VERTICES 18
/* the global texture indices of the frame and the bar */
#define GAUGE_FRAME_TEXTURE 5033
#define GAUGE_BAR_TEXTURE 3340
/* a full gauge, and a full one with UPGRADE_EWALKER_EXTRASHIELD */
#define GAUGE_FULL 10.0F
#define GAUGE_FULL_SHIELDED 12.0F
/* the bar is GAUGE_BLACK_COLOR at 0 or below. above 0, each _TOP is the most
 * the gauge shows in the color of its name. above GAUGE_CYAN_TOP the bar is
 * GAUGE_BLUE_COLOR. The meter draws an orange strip followed by a black strip.
 */
#define GAUGE_RED_TOP 2.0F
#define GAUGE_YELLOW_TOP 6.0F
#define GAUGE_CYAN_TOP 10.0F
#define GAUGE_BLACK_COLOR 0xff000000
#define GAUGE_RED_COLOR 0xffff0000
#define GAUGE_YELLOW_COLOR 0xffffff00
#define GAUGE_CYAN_COLOR 0xff00ffff
#define GAUGE_BLUE_COLOR 0xff2244ff

/* clang-format off */
/* a quad of the meter in color, from start to the bar's full length */
/* the meter of a bar that starts at start, its orange quad and then its black
 * one */
/* clang-format on */

/* what a homing shot keeps in its task's extra. the homing shots are a
 * volley's, a seeker and the missiles */
struct homing {
    Uint8 unk_00;
    Sint8 victim;
    /* the steps until a volley shot next swerves. a missile counts it down
     * once its timer reaches MISSILE_HOLD_STEPS, and starts to turn once it
     * runs out. a seeker doesn't read it */
    Sint16 count;
    NJS_VECTOR dir; /* the unit vector it heads along */
    NJS_POINT3 target; /* its mark, the point it homes on */
    Float speed;
};

#define VOLLEY_SHOTS 16
/* within HOMING_REACH of its mark, a missile sets WORK_HURT in the victim's
 * work. a volley shot sets it within HOMING_REACH of the center of the victim's
 * first shape */
#define HOMING_REACH 4.0F
/* a homing shot ends once placement_try_leave_by finds it out of a range of
 * 500. the call takes the range squared */
#define HOMING_RANGE (500.0F * 500.0F)
/* a homing shot registers its shape only while its timer is above
 * HOMING_ARMED_ABOVE. HOMING_RECORD_RADIUS is its collision record's radius */
#define HOMING_ARMED_ABOVE 4
#define HOMING_RECORD_RADIUS 7.0F
/* a homing shot adds BEHIND_NUDGE of the victim's gun_down to the way to a mark
 * straight behind it. the nudge gives the shot a side to turn to */
#define BEHIND_NUDGE 0.1F
/* a volley shot sets off VOLLEY_SPREAD_MIN up to VOLLEY_SPREAD_SPAN more off
 * the volley's dir */
#define VOLLEY_SPREAD_MIN 0x1800
#define VOLLEY_SPREAD_SPAN 0x1000
/* a volley shot takes a new swerve once its count runs out. it then takes a new
 * count, from VOLLEY_SWERVE_WAIT_MIN up to VOLLEY_SWERVE_WAIT_SPAN more. it
 * takes a new speed too, from VOLLEY_SPEED_MIN up to VOLLEY_SPEED_SPAN more */
#define VOLLEY_SWERVE_WAIT_MIN 16.0F
#define VOLLEY_SWERVE_WAIT_SPAN 24.0F
#define VOLLEY_SPEED_MIN 3.5F
#define VOLLEY_SPEED_SPAN 3.0F
/* a swerve turns the shot's heading up to VOLLEY_SWERVE_TILT either way about
 * its side. it then turns it up to VOLLEY_SWERVE_ROLL either way about the
 * heading. the shot's work keeps the swerve in scl */
#define VOLLEY_SWERVE_TILT 0x1000
#define VOLLEY_SWERVE_ROLL 0x800
/* a volley shot's next heading mixes the way to its mark with its heading plus
 * its swerve. the shot's timer, at most VOLLEY_AIM_MOST, weighs the way to the
 * mark. what is left of VOLLEY_WEIGHT_TOTAL weighs the other */
#define VOLLEY_AIM_MOST 15.0F
#define VOLLEY_WEIGHT_TOTAL 70.0F
/* a seeker's next heading mixes the way to its mark and its heading, weighed by
 * SEEKER_AIM and SEEKER_KEEP. its work's scl.x holds its speed. the speed eases
 * from SEEKER_START_SPEED toward SEEKER_TOP_SPEED by SEEKER_SPEED_STEP a
 * step. its draw scales a debug sphere by SEEKER_DRAW_SCALE */
#define SEEKER_AIM 0.5F
#define SEEKER_KEEP 9.5F
#define SEEKER_START_SPEED 2.0F
#define SEEKER_TOP_SPEED 5.0F
#define SEEKER_SPEED_STEP 0.04F
#define SEEKER_DRAW_SCALE 10.0F

/* the laser's modes, in its work's mode. its work's scl.x holds its shape's
 * radius. LASER_FIRING widens it by LASER_WIDEN a step toward LASER_WIDTH,
 * and LASER_FADING narrows it by LASER_NARROW to 0 */
#define LASER_FIRING 0
#define LASER_FADING 1
#define LASER_WIDTH 35.0F
#define LASER_WIDEN 0.5F
#define LASER_NARROW 1.0F
#define LASER_RECORD_RADIUS 520.0F /* the radius of its collision record */
/* the laser's draw scales its models across by LASER_MODEL_SCALE times its
 * radius */
#define LASER_MODEL_SCALE 0.1F
/* what the laser keeps in its task's extra. player is the slot of the mech
 * that fires it. origin is the point its shape starts from, which each step in
 * LASER_FIRING sets */
struct laser {
    Uint8 unk_00;
    Sint8 player;
    Uint8 unk_02[14];
    NJS_POINT3 origin;
    Uint8 unk_1c[4];
};

#define MISSILES 18
/* a missile sets off at MISSILE_SPEED_MIN up to MISSILE_SPEED_SPAN more a step.
 * it homes on where the victim stood MISSILE_MARK_LAG steps before */
#define MISSILE_SPEED_MIN 2.5F
#define MISSILE_SPEED_SPAN 2.0F
#define MISSILE_MARK_LAG 2
/* a missile holds its heading while its timer is below MISSILE_HOLD_STEPS, and
 * then until its count runs out. missile i's count starts at MISSILE_HOLD_BASE,
 * and MISSILE_HOLD_STAGGER more for each missile before it */
#define MISSILE_HOLD_STEPS 40
#define MISSILE_HOLD_BASE 4
#define MISSILE_HOLD_STAGGER 5
/* once a missile's count runs out, it turns at each step. its next heading
 * mixes the way to its mark and its heading, weighed by MISSILE_TURN and
 * MISSILE_KEEP. its cone is the ways within MISSILE_CONE of its heading. once
 * its timer less its count passes MISSILE_CONE_AFTER, a mark outside the cone
 * doesn't turn it */
#define MISSILE_TURN 1.5F
#define MISSILE_KEEP 8.5F
#define MISSILE_CONE_AFTER 120
#define MISSILE_CONE ((Angle)(25 * ANGLE_PER_DEGREE))
/* a missile bursts within MISSILE_REACH of the land, and at the step that finds
 * its timer above MISSILE_LAST_TIMER */
#define MISSILE_LAST_TIMER 420
#define MISSILE_REACH 3.0F

/* what a ricochet keeps in its task's extra */
struct ricochet {
    Uint8 unk_00;
    Sint8 player;
    Uint8 unk_02[2];
    NJS_VECTOR spd;
    NJS_POINT3 last_pos; /* where its last move, and its tracer, start */
    Uint8 unk_1c[4];
};
#define RICOCHET_SPEED 3.0F
/* a ricochet turns the dir it takes about y by RICOCHET_TURN less up to
 * RICOCHET_TURN_SPAN. it turns dir about x and z by up to RICOCHET_TILT either
 * way */
#define RICOCHET_TURN 0xa000
#define RICOCHET_TURN_SPAN 0x1000
#define RICOCHET_TILT 0x1000
/* a ricochet ends once placement_try_leave_by finds it out of a range of 400,
 * which the call takes squared */
#define RICOCHET_RANGE (400.0F * 400.0F)
/* the cell of ricochet_cells a ricochet draws: RICOCHET_PLAIN_CELL, or with a
 * LASER_UPGRADES upgrade, Tails's or Eggman's own */
#define RICOCHET_PLAIN_CELL 0
#define RICOCHET_TAILS_CELL 1
#define RICOCHET_EGGMAN_CELL 2

static void dust_draw(struct task *task);
static void explosion_create(int player, NJS_POINT3 *pos);

/* the sides of a triangular prism one unit long along z, which a tracer
 * stretches. its draw colors the two ends */
static NJS_POLYGON_VTX tracer_strip[TRACER_STRIP_VERTICES] = {
#include "player/walker_weapons/tracer_strip.inc"
};
/* the tracers' colors, from opaque to clear. a tracer's end at to takes the
 * color its steps pick. its end at from takes the next */
static Uint32 tracer_colors[TRACER_COLORS] = {
    0xffcf9f76, 0xefcf9f76, 0xdfcf9f76, 0xcfcf9f76, 0xbfcf9f76, 0xafcf9f76,
    0x9fcf9f76, 0x8fcf9f76, 0x7fcf9f76, 0x6fcf9f76, 0x5fcf9f76, 0x4fcf9f76,
    0x3fcf9f76, 0x2fcf9f76, 0x1fcf9f76, 0x00cf9f76,
};
static Uint32 tracer_bright_colors[TRACER_COLORS] = {
    0xffefffff, 0xefefffff, 0xdfefffff, 0xcfefffff, 0xbfefffff, 0xafefffff,
    0x9fefffff, 0x8fefffff, 0x7fefffff, 0x6fefffff, 0x5fefffff, 0x4fefffff,
    0x3fefffff, 0x2fefffff, 0x1fefffff, 0x00efffff,
};
/* the shape of a bullet and of a shell */
static struct collision_shape round_shape[1] = {
#include "player/walker_weapons/round_shape.inc"
};
/* the lock-on line's colors, one for each band of lock_window. SHC refuses
 * braces around each color, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR lockon_line_colors[LOCKON_LINE_BANDS] = {
    0xff401010, 0xff180818, 0xff080418, 0xff040414, 0xff020210,
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
/* the quads of a puff and of a jet's flame, 32 wide and high. each shows the
 * whole texture, and a jet's draw sets its quad's colors */
static NJS_TEXTURE_VTX puff_quad[QUAD_VERTICES] = {
#include "player/walker_weapons/puff_quad.inc"
};
static NJS_TEXTURE_VTX jet_quad[QUAD_VERTICES] = {
#include "player/walker_weapons/jet_quad.inc"
};
/* the explosion's sprite. its draw sets the cell's texture, and the sprite's
 * texture list, scale, place and angle */
/* @hack type: an array of one, where an object would do */
static NJS_TEXANIM explosion_cell[1] = {
#include "player/walker_weapons/explosion_cell.inc"
};
static NJS_SPRITE explosion_sprite = {
#include "player/walker_weapons/explosion_sprite.inc"
};
/* the gauge's frames, bars and meters. gauge_frame, gauge_bar and gauge_meter
 * are a one-player game's. the _left and _right ones are a two-player game's,
 * slot 0's and the other's. the draw sets each bar's color and the x of its
 * right end. it sets the x where each meter's orange quad meets its black
 * one */
static NJS_TEXTURE_VTX gauge_frame[GAUGE_FRAME_VERTICES] = {
#include "player/walker_weapons/gauge_frame.inc"
};
static NJS_TEXTURE_VTX gauge_frame_left[GAUGE_FRAME_VERTICES] = {
#include "player/walker_weapons/gauge_frame_left.inc"
};
static NJS_TEXTURE_VTX gauge_frame_right[GAUGE_FRAME_VERTICES] = {
#include "player/walker_weapons/gauge_frame_right.inc"
};
static NJS_TEXTURE_VTX gauge_bar[QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_bar.inc"
};
static NJS_TEXTURE_VTX gauge_bar_left[QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_bar_left.inc"
};
static NJS_TEXTURE_VTX gauge_bar_right[QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_bar_right.inc"
};
static NJS_POLYGON_VTX gauge_meter[2 * QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_meter.inc"
};
static NJS_POLYGON_VTX gauge_meter_left[2 * QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_meter_left.inc"
};
static NJS_POLYGON_VTX gauge_meter_right[2 * QUAD_VERTICES] = {
#include "player/walker_weapons/gauge_meter_right.inc"
};
/* the shape of a volley shot */
static struct collision_shape volley_shape[1] = {
#include "player/walker_weapons/volley_shape.inc"
};
/* a volley shot's sprite, of texture 3 of the victim's effects_texlist. its
 * draw sets p and tlist */
static NJS_TEXANIM volley_cell = {
#include "player/walker_weapons/volley_cell.inc"
};
/* @hack type: an array of one, where an object would do */
static NJS_SPRITE volley_sprite[1] = {
#include "player/walker_weapons/volley_sprite.inc"
};
/* a seeker's shape, which takes MECH_STRIKE_SHAPE's id */
static struct collision_shape seeker_shape[1] = {
#include "player/walker_weapons/seeker_shape.inc"
};
/* the laser's shape, a capsule whose radius each step of the laser sets. Its
 * half-length is measured between its round ends. */
static struct collision_shape laser_shape[1] = {
#include "player/walker_weapons/laser_shape.inc"
};
/* the shape of a missile */
static struct collision_shape missile_shape[1] = {
#include "player/walker_weapons/missile_shape.inc"
};
/* the arena of STAGE_VERSUS_MECH_2, a cylinder about the y axis. arena_floor is
 * the center of its floor, and arena_radius the radius of its wall */
/* @hack type: an array of one, where an object would do */
static NJS_POINT3 arena_floor[] = {
    {0.0F, -50.0F, 0.0F},
};
static Float arena_radius = 190.0F;
/* a ricochet's cells and its sprite, whose draw sets p and tlist. the cells are
 * textures 3, 4 and 51 of the player's effects_texlist */
static NJS_TEXANIM ricochet_cells[3] = {
#include "player/walker_weapons/ricochet_cells.inc"
};
/* @hack type: an array of one, where an object would do */
static NJS_SPRITE ricochet_sprite[1] = {
#include "player/walker_weapons/ricochet_sprite.inc"
};

/* a task in TRACER_LEVEL, named exec, with no work and an extra of EXTRA_SIZE
 * bytes, uncleared. it returns null where the heap can't give the task or the
 * extra */
static struct task *tracer_task_create(void (*step)(struct task *task))
{
    struct task *task = task_create(0, TRACER_LEVEL, step, "exec");
    void *extra;

    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((extra = syMalloc(EXTRA_SIZE)) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = extra;
    }
    return task;
}

/* tracer_task_create, in PUFF_LEVEL */
static struct task *puff_task_create(void (*step)(struct task *task))
{
    struct task *task = task_create(0, PUFF_LEVEL, step, "exec");
    void *extra;

    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((extra = syMalloc(EXTRA_SIZE)) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = extra;
    }
    return task;
}

/* tracer_task_create, with a work: parent's child where parent isn't null, and
 * a task in SHOT_LEVEL where it is */
static struct task *shot_task_create(struct task *parent,
                                     void (*step)(struct task *task))
{
    struct task *task;
    void *extra;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, step, parent);
    } else {
        task = task_create(TASK_WORK, SHOT_LEVEL, step, "exec");
    }
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((extra = syMalloc(EXTRA_SIZE)) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = extra;
    }
    return task;
}

/* set ang to point along dir: its x to the pitch, its y to the heading and its
 * z to 0. a zero dir leaves ang as it was */
static void angles_along(NJS_VECTOR *dir, struct angles *ang)
{
    Float length = njScalor(dir);

    if (length != 0.0F) {
        ang->y = (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN);
        ang->x = (Angle)(asinf(-dir->y / length) * ANGLE_PER_RADIAN);
        ang->z = 0;
    }
}

static void tracer_draw(struct task *task)
{
    struct tracer *tracer = task->extra.p;
    Uint32 *colors;

    if (tracer->bright) {
        colors = tracer_bright_colors;
    } else {
        colors = tracer_colors;
    }
    colors += tracer->steps;
    tracer_strip[0].col = tracer_strip[2].col = tracer_strip[4].col =
        tracer_strip[6].col = *colors++;
    tracer_strip[1].col = tracer_strip[3].col = tracer_strip[5].col =
        tracer_strip[7].col = *colors;
    /* a run of tracers next to each other in the level's ring draws as one
     * batch. the first sets an additive blend, turns the fog off and pushes the
     * matrix. each later one starts from the pushed matrix again. the last pops
     * the matrix and turns the fog on. it leaves the source blend at
     * NJD_COLOR_BLENDING_SRCALPHA and the destination's at
     * NJD_COLOR_BLENDING_INVSRCALPHA */
    if (task_levels[TRACER_LEVEL] == task || task->prev->draw != tracer_draw) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njFogDisable();
        njPushMatrixEx();
    }
    njTranslateEx(&tracer->pos);
    njRotateY(NULL, tracer->ang.y);
    njRotateX(NULL, tracer->ang.x);
    njScale(NULL, 1.0F, 1.0F, tracer->length);
    njDrawPolygon3DEx(tracer_strip, TRACER_STRIP_VERTICES, 1);
    if (task_levels[TRACER_LEVEL]->prev == task ||
        task->next->draw != tracer_draw) {
        njPopMatrixEx();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    } else {
        matrix_load_parent();
    }
}

static void tracer_step(struct task *task)
{
    struct tracer *tracer = task->extra.p;

    if (++tracer->steps >= TRACER_STEPS) {
        task_kill(task);
    }
}

static void tracer_destroy(struct task *task)
{
}

void walker_tracer_create(int player, NJS_POINT3 *from, NJS_POINT3 *to)
{
    NJS_VECTOR line;
    struct task *task;
    struct tracer *tracer;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = tracer_task_create(tracer_step)) == NULL) {
        return;
    }
    tracer = task->extra.p;
    tracer->player = player;
    tracer->steps = 0;
    /* @hack spelling: the player read back from the tracer, where player
     * would do */
    if (player_records[tracer->player]->upgrades & LASER_UPGRADES) {
        tracer->bright = 1;
    } else {
        tracer->bright = 0;
    }
    tracer->pos = *from;
    line = *to;
    njSubVector(&line, from);
    angles_along(&line, &tracer->ang);
    tracer->length = njScalor(&line);
    task->destructor = tracer_destroy;
    task->draw = tracer_draw;
}

static void bullet_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @bug logic: draws every bullet with the textures of player 0's mech
     * (MOV.L at 0x8c097c82). the bullet's step takes its own player from its
     * parent, by player_slot_of (JSR at 0x8c097e34) */
    struct player *record = player_tasks[0]->extra.p;

    njSetTexture(record->by_character.mech.texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawObject(model_table[BULLET_MODEL].object);
    njPopMatrixEx();
}

static void bullet_check_hit(struct task *task, struct task_work *work)
{
    if ((work->flags & (WORK_HURT | WORK_UNK_8)) ||
        (work->collision->flags & COLLISION_PUSHED) ||
        collision_first_with_id(task, WALKER_LASER_SHAPE)) {
        work->mode = ROUND_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

/* @hack spelling: an inline call that returns work, where the move and work
 * would do */
#pragma inline(shot_move)
static struct task_work *shot_move(struct task_work *work)
{
    njAddVector(&work->pos, &work->scl);
    return work;
}

/* @hack redundant: work declared register, where a plain parameter would do */
static void bullet_move(struct task *task, register struct task_work *work,
                        struct enemy_work *enemy)
{
    struct task_work *moved;

    bullet_check_hit(task, work);
    moved = shot_move(work);
    enemy_meet_stage(moved, enemy);
    if (enemy->flags & (ENEMY_ON_FLOOR | ENEMY_AT_WALL)) {
        work->mode = ROUND_HIT;
    }
}

static void bullet_destroy(struct task *task)
{
}

static void bullet_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    int player;
    NJS_VECTOR spd;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: a kill and a return of its own for each test, where
     * one test of both would do */
    if (game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    if (game_state.steps <= 1) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case ROUND_FLY:
        bullet_move(task, work, enemy);
        break;
    case ROUND_HIT:
        spd.x = enemy->spd.x * BULLET_POWDER_SHARE;
        spd.y = enemy->spd.y * BULLET_POWDER_SHARE;
        spd.z = enemy->spd.z * BULLET_POWDER_SHARE;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (task->parent == NULL || (player = player_slot_of(task)) == -1) {
            player = 0;
        }
        powder_create(player, ROUND_HIT_POWDER, &work->pos, &spd);
        gunfire_create_unaimed(&work->pos);
        sound_play_at(ROUND_HIT_SOUND, NULL, 0, 0, &work->pos);
        task_kill(task);
        return;
    }
    if (work->timer++ == ROUND_GRACE_STEPS) {
        work->collision->shapes->flags &= ~SHAPE_IGNORE_PLAYERS;
    }
    collision_register(task);
}

static void bullet_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    work->id = WORK_ID_ATTACK;
    collision_init(task, round_shape, 1, COLLISION_ATTACK);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->shapes->flags |= SHAPE_IGNORE_PLAYERS;
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, njScalor(&work->scl) + SHOT_RADIUS);
    work->ang.x =
        (Angle)(asinf(-work->scl.y / njScalor(&work->scl)) * ANGLE_PER_RADIAN);
    if (_builtin_fabsf(work->scl.z) < ROUND_TINY_Z) {
        if (work->scl.x < 0.0F) {
            work->ang.y = -QUARTER_TURN;
        } else {
            work->ang.y = QUARTER_TURN;
        }
    } else {
        work->ang.y =
            (Angle)(atan2f(work->scl.x, work->scl.z) * ANGLE_PER_RADIAN);
    }
    enemy = enemy_work_alloc(task);
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = ROUND_SIZE;
    enemy->radius = ROUND_SIZE;
    enemy->bottom = -ROUND_SIZE;
    enemy->damage = ROUND_DAMAGE;
    enemy->wall_bounce = ROUND_BOUNCE;
    enemy->floor_bounce = ROUND_BOUNCE;
    enemy->friction = 1.0F;
    enemy->unk_0e4 = 0.8F;
    enemy->wall_turn = 0;
    enemy->spd = work->scl;
    enemy_set_home(work, enemy);
    task->destructor = bullet_destroy;
    task->draw = bullet_draw;
}

void walker_bullet_create(struct task *parent, NJS_VECTOR *spd, NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, bullet_step, parent);
    } else {
        task = task_create(TASK_WORK, SHOT_LEVEL, bullet_step,
                           "WalkerBulletExecutor");
    }
    if (task != NULL) {
        task->work->pos = *pos;
        task->work->scl = *spd;
        bullet_init(task);
    }
    gunfire_create(pos, spd);
}

static void bazooka_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @bug logic: draws every shell with the textures and the model of
     * player 0's mech (MOV.L at 0x8c09809a). the shell's step takes its own
     * player from its parent, by player_slot_of (JSR at 0x8c098264) */
    struct player *record = player_tasks[0]->extra.p;

    njSetTexture(record->by_character.mech.texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njRotateX(NULL, work->ang.x);
    if (record->character == PLAYER_EGGMAN_MECH) {
        njCnkEasyDrawModel(model_table[EGGMAN_SHELL_MODEL].object->model);
    } else if (record->character == PLAYER_TAILS_MECH) {
        njCnkEasyDrawModel(model_table[TAILS_SHELL_MODEL].object->model);
    }
    njPopMatrixEx();
}

/* bullet_check_hit, for a shell */
static void bazooka_check_hit(struct task *task, struct task_work *work)
{
    if ((work->flags & (WORK_HURT | WORK_UNK_8)) ||
        (work->collision->flags & COLLISION_PUSHED) ||
        collision_first_with_id(task, WALKER_LASER_SHAPE)) {
        work->mode = ROUND_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

/* bullet_move, with gunfire_create_brief's flash at each move */
/* @hack redundant: work declared register, where a plain parameter would do */
static void bazooka_move(struct task *task, register struct task_work *work,
                         struct enemy_work *enemy)
{
    struct task_work *moved;

    bazooka_check_hit(task, work);
    moved = shot_move(work);
    gunfire_create_brief(&moved->pos, &moved->scl);
    enemy_meet_stage(moved, enemy);
    if (enemy->flags & (ENEMY_ON_FLOOR | ENEMY_AT_WALL)) {
        work->mode = ROUND_HIT;
    }
}

static void bazooka_destroy(struct task *task)
{
}

static void bazooka_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    int player;
    NJS_VECTOR spd;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: a kill and a return of its own for each test, where
     * one test of both would do */
    if (game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    if (game_state.steps <= 1) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case ROUND_FLY:
        bazooka_move(task, work, enemy);
        break;
    case ROUND_HIT:
        spd.x = enemy->spd.x * SHELL_POWDER_SHARE;
        spd.y = enemy->spd.y * SHELL_POWDER_SHARE;
        spd.z = enemy->spd.z * SHELL_POWDER_SHARE;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (task->parent == NULL || (player = player_slot_of(task)) == -1) {
            player = 0;
        }
        powder_create(player, ROUND_HIT_POWDER, &work->pos, &spd);
        explosion_create(player, &work->pos);
        sound_play_at(ROUND_HIT_SOUND, NULL, 0, 0, &work->pos);
        task_kill(task);
        return;
    }
    if (work->timer++ == ROUND_GRACE_STEPS) {
        work->collision->shapes->flags &= ~SHAPE_IGNORE_PLAYERS;
    }
    collision_register(task);
}

/* bullet_init, for a shell */
static void bazooka_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    work->id = WORK_ID_ATTACK;
    collision_init(task, round_shape, 1, COLLISION_ATTACK);
    work->collision->shapes->flags |= SHAPE_IGNORE_PLAYERS;
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, njScalor(&work->scl) + SHOT_RADIUS);
    work->ang.x =
        (Angle)(asinf(-work->scl.y / njScalor(&work->scl)) * ANGLE_PER_RADIAN);
    if (_builtin_fabsf(work->scl.z) < ROUND_TINY_Z) {
        if (work->scl.x < 0.0F) {
            work->ang.y = -QUARTER_TURN;
        } else {
            work->ang.y = QUARTER_TURN;
        }
    } else {
        work->ang.y =
            (Angle)(atan2f(work->scl.x, work->scl.z) * ANGLE_PER_RADIAN);
    }
    enemy = enemy_work_alloc(task);
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = ROUND_SIZE;
    enemy->radius = ROUND_SIZE;
    enemy->bottom = -ROUND_SIZE;
    enemy->damage = ROUND_DAMAGE;
    enemy->wall_bounce = ROUND_BOUNCE;
    enemy->floor_bounce = ROUND_BOUNCE;
    enemy->friction = 1.0F;
    enemy->unk_0e4 = 0.8F;
    enemy->wall_turn = 0;
    enemy->spd = work->scl;
    enemy_set_home(work, enemy);
    task->destructor = bazooka_destroy;
    task->draw = bazooka_draw;
}

void walker_bazooka_create(struct task *parent, NJS_VECTOR *spd,
                           NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, bazooka_step, parent);
    } else {
        task = task_create(TASK_WORK, SHOT_LEVEL, bazooka_step,
                           "WalkerBazookaExecutor");
    }
    if (task != NULL) {
        task->work->pos = *pos;
        task->work->scl = *spd;
        bazooka_init(task);
    }
}

static void lockon_line_draw(struct task *task)
{
    struct lockon_line *line = task->extra.p;

    line_queue(line->points, line->colors, LINE_ADDITIVE);
}

static void lockon_line_destroy(struct task *task)
{
}

static void lockon_line_step(struct task *task)
{
    struct lockon_line *line = task->extra.p;
    Uint32 color;
    Uint32 red;
    Uint32 green;
    Uint32 blue;

    /* @hack spelling: a kill and a return of its own for each test, where
     * one test of the three would do */
    if (game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    if (game_state.steps <= 1) {
        task_kill(task);
        return;
    }
    if (line->steps_left-- <= 0) {
        task_kill(task);
    } else {
        color = line->colors[0].color;
        red = (color & 0xff0000) >> 16;
        green = (color & 0xff00) >> 8;
        blue = color & 0xff;
        red = njRoundOff(red * LOCKON_LINE_KEEP);
        green = njRoundOff(green * LOCKON_LINE_KEEP);
        blue = njRoundOff(blue * LOCKON_LINE_KEEP);
        color = (color & 0xff000000) | (red << 16) | (green << 8) | blue;
        line->colors[0].color = line->colors[1].color = color;
    }
}

void walker_lockon_line_create(struct task *parent, NJS_POINT3 *from,
                               NJS_POINT3 *to)
{
    struct player *record;
    struct task *task;
    struct lockon_line *line;
    int band;

    record = parent->extra.p;
    task = task_create_child(0, lockon_line_step, parent);
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((line = syMalloc(sizeof(struct lockon_line))) == NULL) {
            task_destroy(task);
            return;
        }
        task->extra.p = line;
        line->steps_left = LOCKON_LINE_STEPS;
        line->points[0] = *from;
        line->points[1] = *to;
        if (record->by_character.mech.lock_window > LOCKON_LINE_BAND) {
            band = 0;
        } else if (record->by_character.mech.lock_window >
                   LOCKON_LINE_BAND / 2) {
            band = 1;
        } else if (record->by_character.mech.lock_window >
                   LOCKON_LINE_BAND / 4) {
            band = 2;
        } else if (record->by_character.mech.lock_window >
                   LOCKON_LINE_BAND / 8) {
            band = 3;
        } else {
            band = 4;
        }
        line->colors[0] = line->colors[1] = lockon_line_colors[band];
        task->destructor = lockon_line_destroy;
        task->draw = lockon_line_draw;
    }
}

static void puff_draw(struct task *task)
{
    struct puff *puff = task->extra.p;
    struct player *record = player_tasks[puff->player]->extra.p;
    Float size = puff->size;

    /* a run of puffs next to each other in the level's ring draws as one
     * batch. the first turns the fog off, and sets an alpha blend and the
     * effects_texlist of its own mech. it then pushes the matrix. each later
     * one starts from the pushed matrix again. the last clears the constant
     * material, turns the fog on and pops the matrix */
    if (task_levels[PUFF_LEVEL] == task ||
        (task->prev->draw != puff_draw && task->prev->draw != dust_draw)) {
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njSetTexture(record->by_character.mech.effects_texlist);
        njPushMatrixEx();
    }
    /* @hack spelling: the offset that steps picks, then the first texture,
     * where the first texture first would do */
    njSetTextureNum((puff->steps >> PUFF_TEXTURE_SHIFT) + puff->texture);
    njTranslateEx(&puff->pos);
    njScale(NULL, size, size, size);
    njRotateEx(&camera_views[camera_current_index]->ang.x, 0);
    njDrawTexture3DEx(puff_quad, QUAD_VERTICES, 1);
    if (task_levels[PUFF_LEVEL]->prev == task ||
        (task->next->draw != puff_draw && task->next->draw != dust_draw)) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njFogEnable();
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
}

static void puff_destroy(struct task *task)
{
}

/* task_kill destroys the task only at its next step. a step that kills a puff
 * takes its draw away too */
static void puff_step(struct task *task)
{
    struct puff *puff = task->extra.p;

    /* @hack spelling: a kill and a clear of the draw in each arm, where one
     * would do after them */
    if (game_state.winner_posed == 1) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    njAddVector(&puff->pos, &puff->spd);
    if (++puff->steps >= PUFF_STEPS) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    puff->spd.x *= PUFF_FRICTION;
    puff->spd.y *= PUFF_FRICTION;
    puff->spd.z *= PUFF_FRICTION;
}

void walker_back_blast_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos)
{
    struct task *task;
    struct puff *puff;

    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->spd = *spd;
        njUnitVector(&puff->spd);
        puff->pos.x = pos->x - (puff->spd.x * BACK_FLASH_BEHIND);
        puff->pos.y = pos->y - (puff->spd.y * BACK_FLASH_BEHIND);
        puff->pos.z = pos->z - (puff->spd.z * BACK_FLASH_BEHIND);
        puff->spd.x *= -BACK_FLASH_SPEED;
        puff->spd.y *= -BACK_FLASH_SPEED;
        puff->spd.z *= -BACK_FLASH_SPEED;
        puff->size = BACK_FLASH_SIZE;
        puff->steps = 0;
        puff->texture = FLASH_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
        powder_create(player, FLASH_POWDER, &puff->pos, &puff->spd);
    }
    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->spd = *spd;
        puff->pos = *pos;
        puff->spd.x *= BACK_SMOKE_SHARE;
        puff->spd.y *= BACK_SMOKE_SHARE;
        puff->spd.z *= BACK_SMOKE_SHARE;
        puff->size = BACK_SMOKE_SIZE;
        puff->steps = 0;
        puff->texture = SMOKE_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
    }
}

void walker_muzzle_blast_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos)
{
    struct task *task;
    struct puff *puff;

    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->spd = *spd;
        puff->pos = *pos;
        puff->spd.x *= MUZZLE_FLASH_SHARE;
        puff->spd.y *= MUZZLE_FLASH_SHARE;
        puff->spd.z *= MUZZLE_FLASH_SHARE;
        puff->size = MUZZLE_FLASH_SIZE;
        puff->steps = 0;
        puff->texture = FLASH_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
        powder_create(player, FLASH_POWDER, &puff->pos, &puff->spd);
    }
    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->pos = *pos;
        puff->spd.x =
            (spd->x * MUZZLE_SMOKE_SHARE) - (world_gravity.x * PUFF_LIFT);
        puff->spd.y =
            (spd->y * MUZZLE_SMOKE_SHARE) - (world_gravity.y * PUFF_LIFT);
        puff->spd.z =
            (spd->z * MUZZLE_SMOKE_SHARE) - (world_gravity.z * PUFF_LIFT);
        puff->size = MUZZLE_SMOKE_SIZE;
        puff->steps = 0;
        puff->texture = SMOKE_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
    }
    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->pos = *pos;
        puff->spd.x =
            (spd->x * MUZZLE_SMOKE_SHARE) + (world_gravity.x * PUFF_LIFT);
        puff->spd.y =
            (spd->y * MUZZLE_SMOKE_SHARE) + (world_gravity.y * PUFF_LIFT);
        puff->spd.z =
            (spd->z * MUZZLE_SMOKE_SHARE) + (world_gravity.z * PUFF_LIFT);
        puff->size = MUZZLE_SMOKE_SIZE;
        puff->steps = 0;
        puff->texture = SMOKE_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
    }
}

void walker_puff_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos)
{
    struct task *task;
    struct puff *puff;

    task = puff_task_create(puff_step);
    if (task != NULL) {
        puff = task->extra.p;
        puff->player = player;
        puff->pos = *pos;
        puff->spd.x = spd->x - (world_gravity.x * PUFF_LIFT);
        puff->spd.y = spd->y - (world_gravity.y * PUFF_LIFT);
        puff->spd.z = spd->z - (world_gravity.z * PUFF_LIFT);
        puff->size = PUFF_DUST_SIZE;
        puff->steps = 0;
        puff->texture = DUST_FIRST_TEXTURE;
        task->destructor = puff_destroy;
        task->draw = puff_draw;
    }
}

static void dust_draw(struct task *task)
{
    puff_draw(task);
}

static void dust_destroy(struct task *task)
{
}

/* puff_step, for dust */
static void dust_step(struct task *task)
{
    struct puff *puff = task->extra.p;

    /* @hack spelling: a kill and a clear of the draw in each arm, where one
     * would do after them */
    if (game_state.winner_posed == 1) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    njAddVector(&puff->pos, &puff->spd);
    if (++puff->steps >= PUFF_STEPS) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    puff->spd.x *= PUFF_FRICTION;
    puff->spd.y *= PUFF_FRICTION;
    puff->spd.z *= PUFF_FRICTION;
}

void walker_ground_dust_create(int player, NJS_POINT3 *pos,
                               struct ground_probe *ground)
{
    NJS_POINT3 below;
    Float height;
    Float size;
    int count;
    struct task *task;
    struct puff *puff;

    below.x = pos->x;
    below.y = ground->below_y;
    below.z = pos->z;
    /* @hack spelling: the assignment inside the test, and an else after
     * the return, where a statement of its own and a plain if would do */
    if ((height = njDistanceP2P(pos, &below)) > DUST_REACH) {
        return;
    } else if (height > DUST_NEAR) {
        count = DUST_FAR_PUFFS;
        size = DUST_FAR_SIZE;
    } else {
        count = DUST_NEAR_PUFFS;
        size = DUST_NEAR_SIZE;
    }
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, ground->below_ang_z);
    njRotateX(NULL, ground->below_ang_x);
    for (; count > 0; count--) {
        task = puff_task_create(dust_step);
        if (task != NULL) {
            puff = task->extra.p;
            puff->player = player;
            puff->spd.x = DUST_SPEED;
            puff->spd.y = 0.0F;
            puff->spd.z = 0.0F;
            njRotateY(NULL, RANDOM_TURN());
            njCalcVector(NULL, &puff->spd, &puff->spd);
            puff->pos.x = below.x + (puff->spd.x * DUST_START);
            puff->pos.y = below.y + (puff->spd.y * DUST_START);
            puff->pos.z = below.z + (puff->spd.z * DUST_START);
            puff->size = size;
            puff->steps = 0;
            puff->texture = DUST_FIRST_TEXTURE;
            task->destructor = dust_destroy;
            task->draw = dust_draw;
        }
    }
    njPopMatrixEx();
}

void walker_foot_dust_create(int player, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct puff *puff;
    Float share;
    /* @hack spelling: locals for the size and the count, and a while loop,
     * where the constants and a for loop would do */
    Float size = FOOT_DUST_SIZE;
    int count = FOOT_DUST_PUFFS;

    while (count > 0) {
        task = puff_task_create(dust_step);
        if (task != NULL) {
            puff = task->extra.p;
            share = FOOT_DUST_SHARE_MIN +
                    (rand() * (FOOT_DUST_SHARE_SPAN * RAND_SCALE));
            puff->player = player;
            puff->spd.x = spd->x * share;
            puff->spd.y = spd->y * share;
            puff->spd.z = spd->z * share;
            puff->pos = *pos;
            puff->size = size;
            puff->steps = 0;
            puff->texture = DUST_FIRST_TEXTURE;
            task->destructor = dust_destroy;
            task->draw = dust_draw;
        }
        count--;
    }
}

/* a run of flames among their parent's children draws as one batch. the first
 * turns the fog off, and sets an additive blend and the effects_texlist of its
 * own mech. it then pushes the matrix. each later one starts from the pushed
 * matrix again. the last clears the constant material, turns the fog on and
 * pops the matrix. it leaves the source blend at NJD_COLOR_BLENDING_SRCALPHA
 * and the destination's at NJD_COLOR_BLENDING_INVSRCALPHA */
static void jet_draw(struct task *task)
{
    struct jet_work *jet = JET(task);
    Float scale = jet->scale;
    /* @bug memory: reads through null for a flame that walker_jet_create made
     * while the player's slot held no task (TST at 0x8c0992b2). it reads the
     * slot's task while the slot still holds none (MOV.L at 0x8c098f42). it
     * reads the flame's null parent too (MOV.L at 0x8c098f48 and at
     * 0x8c099088), which jet_step tests for null (TST at 0x8c0990e8) */
    struct player *record = player_tasks[jet->player]->extra.p;
    /* @hack spelling: one local for the alpha and the color, where a local
     * for each would do */
    Uint32 color;
    int i;
    NJS_TEXTURE_VTX *vertex;

    if (task->parent->child == task || task->prev->draw != jet_draw) {
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(record->by_character.mech.effects_texlist);
        njPushMatrixEx();
    }
    color = (Uint32)(jet->life * JET_BRIGHTEST);
    color = (color << 24) | (color << 16) | (color << 8) | color;
    /* @hack spelling: the counter set ahead of the for, where its first
     * clause would do */
    vertex = jet_quad;
    i = QUAD_VERTICES;
    for (; i > 0; i--) {
        vertex->col = color;
        vertex++;
    }
    njSetTextureNum(jet->texture + JET_FIRST_TEXTURE);
    njTranslateEx(&jet->pos);
    njScale(NULL, scale, scale, scale);
    njRotateEx(&camera_views[camera_current_index]->ang.x, 0);
    njDrawTexture3DEx(jet_quad, QUAD_VERTICES, 1);
    if (task->parent->child->prev == task || task->next->draw != jet_draw) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
}

static void jet_destroy(struct task *task)
{
}

/* task_kill destroys the task only at its next step. a step that kills a
 * flame takes its draw away too */
static void jet_step(struct task *task)
{
    struct jet_work *jet = JET(task);

    if (task->parent == NULL && placement_try_leave(task)) {
        return;
    }
    if (game_state.winner_posed == 1) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    if (jet->life <= JET_GONE) {
        task_destroy(task);
        return;
    }
    if (!(game_state.time_stopped & TIME_STOPPED_BIT(jet->player))) {
        jet->life -= jet->life_step;
        if (jet->life < 0.0F) {
            jet->life = 0.0F;
        }
        njAddVector(&jet->pos, &jet->spd);
        jet->timer++;
    }
}

static void jet_init(struct task *task)
{
    struct jet_work *jet = JET(task);

    jet->life = 1.0F;
    switch (jet->kind) {
    case JET_SCALED:
        jet->life_step = SCALED_FADE;
        /* @hack spelling: the scale's factor first, where the scale first
         * would do */
        jet->scale =
            (SMALL_SCALE_MIN + (rand() * (SMALL_SCALE_SPAN * RAND_SCALE))) *
            jet->scale;
        break;
    case JET_SMALL:
        jet->life_step = SMALL_FADE;
        jet->scale =
            SMALL_SCALE_MIN + (rand() * (SMALL_SCALE_SPAN * RAND_SCALE));
        break;
    case JET_LARGE:
        jet->life_step = LARGE_FADE;
        jet->scale =
            LARGE_SCALE_MIN + (rand() * (LARGE_SCALE_SPAN * RAND_SCALE));
        break;
    case JET_LASTING:
    default:
        jet->life_step = OTHER_FADE;
        jet->scale =
            OTHER_SCALE_MIN + (rand() * (OTHER_SCALE_SPAN * RAND_SCALE));
        break;
    }
    task->destructor = jet_destroy;
    task->draw = jet_draw;
}

void walker_jet_create(int player, int kind, NJS_POINT3 *pos, NJS_VECTOR *spd,
                       Float scale)
{
    struct task *parent;
    struct task *task;
    struct jet_work *jet;
    /* @hack spelling: speed, a local for the flame's speed, where jet->spd
     * would do */
    NJS_VECTOR *speed;

    parent = player_tasks[player];
    if (parent != NULL) {
        task = task_create_child(TASK_WORK, jet_step, parent);
    } else {
        task = task_create(TASK_WORK, JET_LEVEL, jet_step, "WalkerJetExecutor");
    }
    if (task != NULL) {
        jet = JET(task);
        speed = &JET(task)->spd;
        jet->player = player;
        jet->pos = *pos;
        *speed = *spd;
        jet->kind = kind;
        jet->scale = scale;
        jet_init(task);
        /* @hack spelling: task->work read again, where jet would do */
        JET(task)->texture = rand() * (JET_TEXTURE_SPAN * RAND_SCALE);
    }
}

static void explosion_draw(struct task *task)
{
    struct puff *explosion = task->extra.p;
    struct player *record = player_tasks[explosion->player]->extra.p;

    explosion_sprite.tlist = record->by_character.mech.effects_texlist;
    explosion_cell->texid = explosion->texture + explosion->steps;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    /* @hack spelling: the texture list read back from the sprite, where the
     * record's would do */
    njSetTexture(explosion_sprite.tlist);
    njFogDisable();
    explosion_sprite.sx = explosion_sprite.sy = explosion->size;
    explosion_sprite.p = explosion->pos;
    explosion_sprite.ang = task->unk_3c.l;
    njDrawSprite3D(&explosion_sprite, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_ANGLE);
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void explosion_destroy(struct task *task)
{
}

/* task_kill destroys the task only at its next step. a step that kills an
 * explosion takes its draw away too */
static void explosion_step(struct task *task)
{
    struct puff *explosion = task->extra.p;

    /* @hack spelling: an else that returns between two kills, where one test
     * of both would do */
    if (++explosion->steps >= EXPLOSION_STEPS) {
        task_kill(task);
    } else if (game_state.winner_posed != 1) {
        return;
    } else {
        task_kill(task);
    }
    task->draw = NULL;
}

/* make an explosion at pos, drawn with the effects_texlist of the mech in slot
 * player. where the heap can't give it, nothing is made */
static void explosion_create(int player, NJS_POINT3 *pos)
{
    struct task *task;
    struct puff *explosion;

    task = puff_task_create(explosion_step);
    if (task != NULL) {
        explosion = task->extra.p;
        explosion->player = player;
        explosion->pos = *pos;
        explosion->size = EXPLOSION_SIZE;
        explosion->steps = 0;
        explosion->texture = EXPLOSION_FIRST_TEXTURE;
        task->unk_3c.l = RANDOM_TURN();
        task->destructor = explosion_destroy;
        task->draw = explosion_draw;
    }
}

void walker_gauge_draw(int player)
{
    struct player *record = player_tasks[player]->extra.p;
    NJS_TEXTURE_VTX *frame;
    NJS_TEXTURE_VTX *bar;
    NJS_POLYGON_VTX *meter;
    Float full;
    Float gauge;
    Float fraction;
    Uint32 color;

    if (!game_state.unk_0b) {
        return;
    }
    if (game_state.cleared == 1) {
        return;
    }
    if (game_state.solo_player) {
        return;
    }
    if (game_state.mini_event_running == 1) {
        return;
    }
    if (player_work[player]->id != WORK_ID_PLAYER) {
        return;
    }
    njFogDisable();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(record->by_character.mech.effects_texlist);
    if (game_state.two_players) {
        if (record->slot == 0) {
            frame = gauge_frame_left;
        } else {
            frame = gauge_frame_right;
        }
    } else {
        frame = gauge_frame;
    }
    njDrawTexture(frame, GAUGE_FRAME_VERTICES, GAUGE_FRAME_TEXTURE, 1);
    if (game_state.two_players) {
        if (record->slot == 0) {
            bar = gauge_bar_left;
            bar[2].x = bar[3].x = GAUGE_START_LEFT;
        } else {
            bar = gauge_bar_right;
            bar[2].x = bar[3].x = GAUGE_START_RIGHT;
        }
    } else {
        bar = gauge_bar;
        bar[2].x = bar[3].x = GAUGE_START;
    }
    if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD)) {
        full = GAUGE_FULL_SHIELDED;
    } else {
        full = GAUGE_FULL;
    }
    gauge = record->gauge;
    if (gauge <= 0.0F) {
        color = GAUGE_BLACK_COLOR;
        gauge = 0.0F;
    } else if (gauge <= GAUGE_RED_TOP) {
        color = GAUGE_RED_COLOR;
    } else if (gauge <= GAUGE_YELLOW_TOP) {
        color = GAUGE_YELLOW_COLOR;
    } else if (gauge <= GAUGE_CYAN_TOP) {
        color = GAUGE_CYAN_COLOR;
    } else {
        color = GAUGE_BLUE_COLOR;
    }
    gauge /= full;
    bar[0].col = bar[1].col = bar[2].col = bar[3].col = color;
    bar[2].x = bar[3].x += GAUGE_LENGTH * gauge;
    njDrawTexture(bar, QUAD_VERTICES, GAUGE_BAR_TEXTURE, 1);
    if (game_state.two_players) {
        if (record->slot == 0) {
            meter = gauge_meter_left;
            meter[2].x = meter[3].x = GAUGE_START_LEFT;
            meter[4].x = meter[5].x = GAUGE_START_LEFT;
        } else {
            meter = gauge_meter_right;
            meter[2].x = meter[3].x = GAUGE_START_RIGHT;
            meter[4].x = meter[5].x = GAUGE_START_RIGHT;
        }
    } else {
        meter = gauge_meter;
        meter[2].x = meter[3].x = GAUGE_START;
        meter[4].x = meter[5].x = GAUGE_START;
    }
    gauge = record->gauge;
    /* @hack spelling: fraction set from gauge and then less the whole
     * units, where one statement would do */
    fraction = gauge;
    fraction -= njRoundOff(gauge);
    meter[2].x = meter[3].x = meter[4].x = meter[5].x +=
        GAUGE_LENGTH * fraction;
    njDrawPolygon(meter, QUAD_VERTICES, 0);
    njDrawPolygon(&meter[QUAD_VERTICES], QUAD_VERTICES, 0);
    njFogEnable();
}

static void volley_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *shot = task->extra.p;
    struct player *record = player_tasks[shot->victim]->extra.p;

    njFogDisable();
    if (record->by_character.mech.effects_texlist != NULL) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(record->by_character.mech.effects_texlist);
        volley_sprite->p = work->pos;
        volley_sprite->tlist = record->by_character.mech.effects_texlist;
        njDrawSprite3D(volley_sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

static void volley_step(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *shot = task->extra.p;
    struct player *record;
    struct task_work *victim;
    Angle ang;
    NJS_POINT3 center;
    NJS_VECTOR along;
    NJS_VECTOR dir;
    /* @hack spelling: turned, a local for the turned heading and then for
     * the pos before the move, where a local for each would do */
    NJS_VECTOR turned;
    NJS_VECTOR side;
    Float distance;
    Float speed;
    Float weight;

    /* @hack spelling: a kill and a return of its own for each test, where one
     * test of the three would do */
    if (task->parent == NULL) {
        task_kill(task);
        return;
    }
    if (game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    if (game_state.steps <= 1) {
        task_kill(task);
        return;
    }
#ifdef NONMATCHING
    /* this side repairs the two tags at the step's end: a shot whose
     * victim's slot is empty ends here, and loses its draw, which reads the
     * victim's record too */
    if (player_tasks[shot->victim] == NULL) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
#endif
    record = player_tasks[shot->victim]->extra.p;
    dir = shot->target;
    njSubVector(&dir, &work->pos);
    distance = njScalor(&dir);
    if (distance == 0.0F) {
        dir = shot->dir;
    } else {
        njUnitVector(&dir);
        if (njInnerProduct(&dir, &shot->dir) == -1.0F) {
            dir.x += record->by_character.mech.gun_down.x * BEHIND_NUDGE;
            dir.y += record->by_character.mech.gun_down.y * BEHIND_NUDGE;
            dir.z += record->by_character.mech.gun_down.z * BEHIND_NUDGE;
        }
        if (shot->count-- <= 0) {
            shot->count = (rand() * (VOLLEY_SWERVE_WAIT_SPAN * RAND_SCALE)) +
                          VOLLEY_SWERVE_WAIT_MIN;
            shot->speed =
                (rand() * (VOLLEY_SPEED_SPAN * RAND_SCALE)) + VOLLEY_SPEED_MIN;
            turned = shot->dir;
            side.y = 0.0F;
            side.x = turned.z;
            side.z = -turned.x;
            njUnitVector(&side);
            ang = VOLLEY_SWERVE_TILT -
                  (rand() * (2 * VOLLEY_SWERVE_TILT * RAND_SCALE));
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotate(NULL, &side, ang);
            njCalcVector(NULL, &turned, &turned);
            ang = VOLLEY_SWERVE_ROLL -
                  (rand() * (2 * VOLLEY_SWERVE_ROLL * RAND_SCALE));
            njUnitMatrix(NULL);
            njRotate(NULL, &shot->dir, ang);
            njCalcVector(NULL, &turned, &work->scl);
            njPopMatrixEx();
        }
        along = shot->dir;
        njAddVector(&along, &work->scl);
        njUnitVector(&along);
        weight = work->timer;
        if (weight > VOLLEY_AIM_MOST) {
            weight = VOLLEY_AIM_MOST;
        }
        dir.x = (dir.x * weight) + (along.x * (VOLLEY_WEIGHT_TOTAL - weight));
        dir.y = (dir.y * weight) + (along.y * (VOLLEY_WEIGHT_TOTAL - weight));
        dir.z = (dir.z * weight) + (along.z * (VOLLEY_WEIGHT_TOTAL - weight));
        njUnitVector(&dir);
        shot->dir = dir;
    }
    speed = shot->speed;
    dir.x *= speed;
    dir.y *= speed;
    dir.z *= speed;
    turned = work->pos;
    njAddVector(&work->pos, &dir);
    walker_tracer_create(shot->victim, &turned, &work->pos);
    victim = player_work[shot->victim];
    if (victim != NULL) {
        collision_get_shape_center(victim, victim->collision->shapes, &center);
    }
    if (placement_try_leave_by(task, HOMING_RANGE)) {
        task_kill(task);
        return;
    }
    /* @hack spelling: an else after the return, where a plain if would do */
    if (work->collision->flags & COLLISION_TOUCHED) {
        task_kill(task);
        return;
        /* @bug uninitialized: reads center unset where the victim's slot
         * holds no work. the test above then skips collision_get_shape_center
         * (TST at 0x8c099b7e). the distance call takes center as the frame
         * left it (JSR at 0x8c099bb8) */
    } else if (njDistanceP2P(&center, &work->pos) < HOMING_REACH) {
        task_kill(task);
        /* @bug memory: sets WORK_HURT through the victim's work untested
         * (MOV.W at 0x8c099bd2), though the test above (TST at 0x8c099b7e)
         * expects the work may be null */
        player_work[shot->victim]->flags |= WORK_HURT;
        return;
    }
    if (work->timer > HOMING_ARMED_ABOVE) {
        collision_register(task);
    }
    work->timer++;
}

static void volley_destroy(struct task *task)
{
}

static void volley_init(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *shot = task->extra.p;
    Angle ang;
    NJS_VECTOR side;
    NJS_VECTOR dir;
    struct task_work *victim;

    collision_init(task, volley_shape, 1, COLLISION_ATTACK);
    work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, HOMING_RECORD_RADIUS);
    njPushMatrixEx();
    dir = shot->dir;
    side.y = 0.0F;
    side.x = dir.z;
    side.z = -dir.x;
    njUnitVector(&side);
    ang = (rand() * (VOLLEY_SPREAD_SPAN * RAND_SCALE)) + VOLLEY_SPREAD_MIN;
    njUnitMatrix(NULL);
    njRotate(NULL, &side, ang);
    njCalcVector(NULL, &dir, &dir);
    ang = work->tick * FULL_TURN / work->submode;
    njUnitMatrix(NULL);
    njRotate(NULL, &shot->dir, ang);
    njCalcVector(NULL, &dir, &shot->dir);
    njPopMatrixEx();
    victim = player_work[shot->victim];
    if (victim != NULL) {
        collision_get_shape_center(victim, victim->collision->shapes,
                                   &shot->target);
    }
    shot->speed =
        (rand() * (VOLLEY_SPEED_SPAN * RAND_SCALE)) + VOLLEY_SPEED_MIN;
    shot->count = (rand() * (VOLLEY_SWERVE_WAIT_SPAN * RAND_SCALE)) +
                  VOLLEY_SWERVE_WAIT_MIN;
    task->destructor = volley_destroy;
    task->draw = volley_draw;
}

void walker_volley_create(struct task *parent, int victim, NJS_VECTOR *dir,
                          NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;
    struct homing *shot;
    int i;

    if (parent == NULL) {
        return;
    }
    for (i = 0; i < VOLLEY_SHOTS; i++) {
        task = shot_task_create(parent, volley_step);
        if (task == NULL) {
            return;
        }
        work = task->work;
        shot = task->extra.p;
        /* the shot's work holds its index in tick, and the volley's count of
         * shots in submode. volley_init turns the shot by the two */
        work->tick = i;
        work->submode = VOLLEY_SHOTS;
        shot->victim = victim;
        work->pos = *pos;
        shot->dir = *dir;
        volley_init(task);
    }
}

static void seeker_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *shot = task->extra.p;
    struct player *record = player_tasks[shot->victim]->extra.p;
    NJS_VECTOR scl;

    njFogDisable();
    if (record->by_character.mech.effects_texlist != NULL) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        scl.x = scl.y = scl.z = SEEKER_DRAW_SCALE;
        njSetTexture(record->by_character.mech.effects_texlist);
        debug_draw_model(&work->pos, &work->ang, &scl, DEBUG_MODEL_SPHERE);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

static void seeker_step(struct task *task)
{
    struct homing *shot = task->extra.p;
    struct task_work *work = task->work;
    struct task_work *victim = player_work[shot->victim];
    struct player *record;
    NJS_VECTOR dir;
    Float distance;
    Float speed;

    if (task->parent == NULL || victim == NULL ||
        game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    record = player_tasks[shot->victim]->extra.p;
    collision_get_shape_center(victim, victim->collision->shapes,
                               &shot->target);
    dir = shot->target;
    njSubVector(&dir, &work->pos);
    distance = njScalor(&dir);
    if (distance == 0.0F) {
        dir = shot->dir;
    } else {
        njUnitVector(&dir);
        if (njInnerProduct(&dir, &shot->dir) == -1.0F) {
            dir.x += record->by_character.mech.gun_down.x * BEHIND_NUDGE;
            dir.y += record->by_character.mech.gun_down.y * BEHIND_NUDGE;
            dir.z += record->by_character.mech.gun_down.z * BEHIND_NUDGE;
        }
        dir.x = (dir.x * SEEKER_AIM) + (shot->dir.x * SEEKER_KEEP);
        dir.y = (dir.y * SEEKER_AIM) + (shot->dir.y * SEEKER_KEEP);
        dir.z = (dir.z * SEEKER_AIM) + (shot->dir.z * SEEKER_KEEP);
        njUnitVector(&dir);
        shot->dir = dir;
    }
    speed = work->scl.x =
        float_approach(work->scl.x, SEEKER_TOP_SPEED, SEEKER_SPEED_STEP);
    dir.x *= speed;
    dir.y *= speed;
    dir.z *= speed;
    njAddVector(&work->pos, &dir);
    if (placement_try_leave_by(task, HOMING_RANGE)) {
        task_kill(task);
        return;
    }
    if (work->collision->flags & COLLISION_TOUCHED) {
        task_kill(task);
        return;
    }
    if (work->timer > HOMING_ARMED_ABOVE) {
        collision_register(task);
    }
    work->timer++;
}

static void seeker_destroy(struct task *task)
{
}

static void seeker_init(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *shot = task->extra.p;

    collision_init(task, seeker_shape, 1, COLLISION_ATTACK);
    work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, HOMING_RECORD_RADIUS);
    njUnitVector(&shot->dir);
    work->scl.x = SEEKER_START_SPEED;
    task->destructor = seeker_destroy;
    task->draw = seeker_draw;
}

void walker_seeker_create(struct task *parent, int victim, NJS_VECTOR *dir,
                          NJS_POINT3 *pos)
{
    struct task *task = shot_task_create(parent, seeker_step);
    struct homing *shot;
    struct task_work *work;

    if (task != NULL) {
        shot = task->extra.p;
        work = task->work;
        shot->victim = victim;
        work->pos = *pos;
        shot->dir = *dir;
        seeker_init(task);
    }
}

static void laser_draw(struct task *task)
{
    struct laser *laser = task->extra.p;
    struct task_work *work = task->work;
    struct player *record = player_tasks[laser->player]->extra.p;
    struct game_state *state;
    NJS_CNK_OBJECT *first;
    NJS_CNK_OBJECT *second;
    NJS_CNK_OBJECT *third;
    NJS_VECTOR scl;
    /* @hack spelling: copy, a second local for &game_state, where state
     * alone would do */
    struct game_state *copy;

    njFogDisable();
    if (record->by_character.mech.effects_texlist != NULL) {
        njSetTexture(record->by_character.mech.texlist);
        njPushMatrixEx();
        njTranslateEx(&laser->origin);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        scl.x = scl.z = work->collision->shapes->radius * LASER_MODEL_SCALE;
        scl.y = 1.0F;
        njScaleEx(&scl);
        njRotateZ(NULL, HALF_TURN + QUARTER_TURN);
        if (record->character == PLAYER_EGGMAN_MECH) {
            first = model_table[EGGMAN_LASER_MODELS].object;
            second = model_table[EGGMAN_LASER_MODELS + 1].object;
            third = model_table[EGGMAN_LASER_MODELS + 2].object;
        } else {
            first = model_table[TAILS_LASER_MODELS].object;
            second = model_table[TAILS_LASER_MODELS + 1].object;
            third = model_table[TAILS_LASER_MODELS + 2].object;
        }
        /* @hack spelling: copy set beside the first call in a comma
         * expression, and state set inside its argument, where state set in
         * a statement of its own would do */
        copy = &game_state,
        texture_anim_apply(first->model, &laser_anim_a, (state = copy)->steps);
        njCnkEasyDrawObject(first);
        texture_anim_apply(second->model, &laser_anim_b, state->steps);
        njCnkEasyDrawObject(second);
        texture_anim_apply(third->model, &laser_anim_c, state->steps);
        njCnkEasyDrawObject(third);
        njPopMatrixEx();
    }
    njFogEnable();
}

static void laser_step(struct task *task)
{
    struct laser *laser = task->extra.p;
    struct task_work *player = player_work[laser->player];
    struct task_work *work = task->work;
    struct player *record;
    /* @hack spelling: pos, one local for the laser's direction and then for
     * its offset, where a local for each would do */
    NJS_VECTOR pos;

    /* @hack spelling: a kill and a return of its own for each test, where one
     * test of both would do */
    if (player == NULL) {
        task_kill(task);
        return;
    }
    if (game_state.winner_posed == 1 || stage_get_mode() == STAGE_FADE_IN) {
        task_kill(task);
        return;
    }
    record = player_tasks[laser->player]->extra.p;
    if (work->mode == LASER_FIRING && player->mode == PLAYER_MODE_MECH_LASER) {
        work->scl.x = work->collision->shapes->radius = float_approach(
            work->collision->shapes->radius, LASER_WIDTH, LASER_WIDEN);
        pos = record->laser_dir;
        work->ang.x = (Angle)(acosf(pos.y) * ANGLE_PER_RADIAN);
        work->ang.y = (Angle)(atan2f(pos.x, pos.z) * ANGLE_PER_RADIAN);
        laser->origin = record->laser_origin;
    } else {
        work->scl.x = work->collision->shapes->radius =
            float_approach(work->collision->shapes->radius, 0.0F, LASER_NARROW);
        work->mode = LASER_FADING;
        if (work->scl.x <= 0.0F) {
            task_kill(task);
            return;
        }
    }
    pos.x = pos.z = 0.0F;
    pos.y = work->scl.x + work->collision->shapes->size_y;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&laser->origin);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &pos, &work->pos);
    njPopMatrixEx();
    if (work->mode == LASER_FIRING) {
        collision_register(task);
    }
    if (task->draw == NULL) {
        task->draw = laser_draw;
    }
}

static void laser_destroy(struct task *task)
{
}

static void laser_init(struct task *task)
{
    struct task_work *work = task->work;

    collision_init(task, laser_shape, 1, COLLISION_ENEMY);
    work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, LASER_RECORD_RADIUS);
    work->id = WORK_ID_ATTACK;
    task->destructor = laser_destroy;
}

void walker_laser_create(int player)
{
    struct task *task = shot_task_create(player_tasks[player], laser_step);
    /* @hack spelling: laser, a local for the extra, where the extra read in
     * place would do */
    struct laser *laser;

    if (task != NULL) {
        laser = task->extra.p;
        laser->player = player;
        laser_init(task);
    }
}

static void missile_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *record;
    NJS_CNK_OBJECT *object;
    int player;

    player = player_slot_of(task->parent);
    if (player == -1) {
        return;
    }
    record = player_tasks[player]->extra.p;
    /* of a run of missiles among their parent's children, the first turns the
     * fog off. the last turns it on */
    if (task->parent == NULL || task->parent->child == task ||
        task->prev->draw != missile_draw) {
        njFogDisable();
    }
    if (record->by_character.mech.texlist != NULL) {
        if (record->character == PLAYER_EGGMAN_MECH) {
            object = model_table[EGGMAN_MISSILE_MODEL].object;
        } else {
            object = model_table[TAILS_MISSILE_MODEL].object;
        }
        njSetTexture(record->by_character.mech.texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njCnkEasyDrawObject(object);
        njPopMatrixEx();
    }
    if (task->parent == NULL || task->parent->child->prev == task ||
        task->next->draw != missile_draw) {
        njFogEnable();
    }
}

static void missile_step(struct task *task)
{
    struct homing *missile = task->extra.p;
    struct task_work *work = task->work;
    struct task_work *victim = player_work[missile->victim];
    struct player *record;
    Float product;
    Float distance;
    Float speed;
    NJS_VECTOR dir;
    NJS_VECTOR to;
    NJS_POINT3 last_pos;
    struct surface_probe probe;

    /* @hack spelling: a kill and a return of its own for each test, where one
     * test of both would do */
    if (task->parent == NULL || victim == NULL ||
        game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    if (stage_get_mode() == STAGE_FADE_IN) {
        task_kill(task);
        return;
    }
    record = player_tasks[missile->victim]->extra.p;
    player_get_past(missile->victim, MISSILE_MARK_LAG, &missile->target, NULL);
    to = missile->target;
    njSubVector(&to, &work->pos);
    distance = njScalor(&to);
    if (distance == 0.0F || work->timer < MISSILE_HOLD_STEPS) {
        to = missile->dir;
    } else {
        njUnitVector(&to);
        product = njInnerProduct(&to, &missile->dir);
        if (product <= -1.0F) {
            to.x += record->by_character.mech.gun_down.x * BEHIND_NUDGE;
            to.y += record->by_character.mech.gun_down.y * BEHIND_NUDGE;
            to.z += record->by_character.mech.gun_down.z * BEHIND_NUDGE;
        } else if (work->timer - missile->count > MISSILE_CONE_AFTER &&
                   njCos(MISSILE_CONE) > product) {
            to = missile->dir;
        }
        work->scl = to;
        dir = missile->dir;
        if (missile->count-- <= 0) {
            /* @hack spelling: turn and keep, locals for the weights, where
             * the constants would do */
            Float turn;
            Float keep;

            missile->count = 0;
            turn = MISSILE_TURN;
            keep = MISSILE_KEEP;
            to.x = (to.x * turn) + (dir.x * keep);
            to.y = (to.y * turn) + (dir.y * keep);
            to.z = (to.z * turn) + (dir.z * keep);
            njUnitVector(&to);
            missile->dir = to;
            /* @hack redundant: the count cleared again, as the arm's first
             * line clears it */
            missile->count = 0;
        } else {
            to = dir;
        }
    }
    speed = missile->speed;
    to.x *= speed;
    to.y *= speed;
    to.z *= speed;
    last_pos = work->pos;
    njAddVector(&work->pos, &to);
    probe.pos = work->pos;
    if (surface_probe_sides(&probe)) {
        if (work->pos.x - probe.sides[SURFACE_NEG_X].pos < MISSILE_REACH ||
            work->pos.y - probe.sides[SURFACE_NEG_Y].pos < MISSILE_REACH ||
            work->pos.z - probe.sides[SURFACE_NEG_Z].pos < MISSILE_REACH ||
            probe.sides[SURFACE_POS_X].pos - work->pos.x < MISSILE_REACH ||
            probe.sides[SURFACE_POS_Y].pos - work->pos.y < MISSILE_REACH ||
            probe.sides[SURFACE_POS_Z].pos - work->pos.z < MISSILE_REACH) {
            explosion_create(player_slot_of(task->parent), &work->pos);
            task_kill(task);
            return;
        }
    }
    walker_tracer_create(missile->victim, &last_pos, &work->pos);
    work->ang.z = (Angle)(asinf(-missile->dir.y) * ANGLE_PER_RADIAN);
    work->ang.y =
        (Angle)(atan2f(missile->dir.z, -missile->dir.x) * ANGLE_PER_RADIAN);
    if (placement_try_leave_by(task, HOMING_RANGE)) {
        task_kill(task);
        return;
    }
    /* @hack spelling: an else after the return, where a plain if would do */
    if (work->collision->flags & COLLISION_TOUCHED) {
        explosion_create(player_slot_of(task->parent), &work->pos);
        task_kill(task);
        return;
    } else if (njDistanceP2P(&missile->target, &work->pos) < HOMING_REACH) {
        task_kill(task);
        player_work[missile->victim]->flags |= WORK_HURT;
        return;
    }
    if (work->timer > MISSILE_LAST_TIMER) {
        explosion_create(player_slot_of(task->parent), &work->pos);
        task_kill(task);
        return;
    } else if (work->timer > HOMING_ARMED_ABOVE) {
        collision_register(task);
    }
    work->timer++;
}

/* missile_step for STAGE_VERSUS_MECH_2, where the arena's wall bursts a missile
 * in place of the land */
static void missile_step_in_arena(struct task *task)
{
    struct homing *missile = task->extra.p;
    struct task_work *work = task->work;
    struct task_work *victim = player_work[missile->victim];
    struct player *record;
    Float product;
    Float distance;
    Float speed;
    NJS_VECTOR dir;
    NJS_VECTOR to;
    NJS_POINT3 last_pos;
    NJS_VECTOR off;

    /* @hack spelling: a kill and a return of its own for each test, where one
     * test of both would do */
    if (task->parent == NULL || victim == NULL) {
        task_kill(task);
        return;
    }
    if (game_state.winner_posed == 1 || stage_get_mode() == STAGE_FADE_IN) {
        task_kill(task);
        return;
    }
    record = player_tasks[missile->victim]->extra.p;
    player_get_past(missile->victim, MISSILE_MARK_LAG, &missile->target, NULL);
    to = missile->target;
    njSubVector(&to, &work->pos);
    distance = njScalor(&to);
    if (distance == 0.0F || work->timer < MISSILE_HOLD_STEPS) {
        to = missile->dir;
    } else {
        njUnitVector(&to);
        product = njInnerProduct(&to, &missile->dir);
        if (product <= -1.0F) {
            to.x += record->by_character.mech.gun_down.x * BEHIND_NUDGE;
            to.y += record->by_character.mech.gun_down.y * BEHIND_NUDGE;
            to.z += record->by_character.mech.gun_down.z * BEHIND_NUDGE;
        } else if (work->timer - missile->count > MISSILE_CONE_AFTER &&
                   njCos(MISSILE_CONE) > product) {
            to = missile->dir;
        }
        work->scl = to;
        dir = missile->dir;
        if (missile->count-- <= 0) {
            /* @hack spelling: turn and keep, locals for the weights, where
             * the constants would do */
            Float turn;
            Float keep;

            missile->count = 0;
            turn = MISSILE_TURN;
            keep = MISSILE_KEEP;
            to.x = (to.x * turn) + (dir.x * keep);
            to.y = (to.y * turn) + (dir.y * keep);
            to.z = (to.z * turn) + (dir.z * keep);
            njUnitVector(&to);
            missile->dir = to;
            /* @hack redundant: the count cleared again, as the arm's first
             * line clears it */
            missile->count = 0;
        } else {
            to = dir;
        }
    }
    speed = missile->speed;
    to.x *= speed;
    to.y *= speed;
    to.z *= speed;
    last_pos = work->pos;
    njAddVector(&work->pos, &to);
    off = work->pos;
    njSubVector(&off, arena_floor);
    off.y = 0.0F;
    /* @bug logic: tests off.y against the floor's y (FCMP/GT at 0x8c09ae0a)
     * after the line above zeroes off.y (FMOV.S at 0x8c09adf2). the test
     * never holds, so a missile below the floor flies on */
    if (njScalor(&off) > arena_radius || off.y < arena_floor->y) {
        explosion_create(player_slot_of(task->parent), &work->pos);
        task_kill(task);
        return;
    }
    walker_tracer_create(missile->victim, &last_pos, &work->pos);
    work->ang.z = (Angle)(asinf(-missile->dir.y) * ANGLE_PER_RADIAN);
    work->ang.y =
        (Angle)(atan2f(missile->dir.z, -missile->dir.x) * ANGLE_PER_RADIAN);
    if (placement_try_leave_by(task, HOMING_RANGE)) {
        task_kill(task);
        return;
    }
    /* @hack spelling: an else after the return, where a plain if would do */
    if (work->collision->flags & COLLISION_TOUCHED) {
        explosion_create(player_slot_of(task->parent), &work->pos);
        task_kill(task);
        return;
    } else if (njDistanceP2P(&missile->target, &work->pos) < HOMING_REACH) {
        task_kill(task);
        player_work[missile->victim]->flags |= WORK_HURT;
        return;
    }
    if (work->timer > MISSILE_LAST_TIMER) {
        explosion_create(player_slot_of(task->parent), &work->pos);
        task_kill(task);
        return;
    } else if (work->timer > HOMING_ARMED_ABOVE) {
        collision_register(task);
    }
    work->timer++;
}

static void missile_destroy(struct task *task)
{
}

/* a missile's step until it sets off. it waits out its tick, then sets off from
 * its mech's missile point. it then takes its own step, draw and destructor */
static void missile_launch(struct task *task)
{
    struct task_work *work = task->work;
    struct homing *missile = task->extra.p;
    int player = player_slot_of(task->parent);
    struct player *record;

    if (player == -1) {
        task_kill(task);
        return;
    }
    record = player_tasks[player]->extra.p;
    if (work->tick-- > 0) {
        return;
    }
    work->pos = record->by_character.mech
                    .missile_points[work->submode % PLAYER_MISSILE_POINTS];
    missile->dir = record->by_character.mech.missile_dir;
    collision_init(task, missile_shape, 1, COLLISION_ATTACK);
    work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, HOMING_RECORD_RADIUS);
    work->ang.z = (Angle)(asinf(-missile->dir.y) * ANGLE_PER_RADIAN);
    work->ang.y =
        (Angle)(atan2f(missile->dir.z, -missile->dir.x) * ANGLE_PER_RADIAN);
    missile->speed =
        (rand() * (MISSILE_SPEED_SPAN * RAND_SCALE)) + MISSILE_SPEED_MIN;
    work->scl = missile->dir;
    missile->count = (work->submode * MISSILE_HOLD_STAGGER) + MISSILE_HOLD_BASE;
    if (stage_current_id() == STAGE_VERSUS_MECH_2) {
        task->step = missile_step_in_arena;
    } else {
        task->step = missile_step;
    }
    task->destructor = missile_destroy;
    task->draw = missile_draw;
}

void walker_missiles_create(struct task *parent, int victim)
{
    struct task *task;
    struct task_work *work;
    struct homing *missile;
    int i;

    if (parent == NULL) {
        return;
    }
    for (i = MISSILES - 1; i >= 0; i--) {
        task = shot_task_create(parent, missile_launch);
        if (task != NULL) {
            missile = task->extra.p;
            work = task->work;
            /* the missile's work holds the steps it waits in tick, and its
             * index in submode */
            work->tick = i;
            work->submode = i;
            missile->victim = victim;
        }
    }
}

static void ricochet_draw(struct task *task)
{
    struct ricochet *ricochet = task->extra.p;
#ifdef NONMATCHING
    /* this side takes the record only once the guard below has found a task
     * in the slot */
    struct player *record;
#else
    struct player *record = player_tasks[ricochet->player]->extra.p;
#endif
    struct task_work *work = task->work;
    int cell;

    njFogDisable();
    /* @bug memory: reads the record through null (MOV.L at 0x8c09b208),
     * though ricochet_step tests it for null (TST at 0x8c09b2ac). that step's
     * task_kill destroys the task only at its next step, so the draw still
     * runs on the null record */
#ifdef NONMATCHING
    /* this side repairs the tag above: a ricochet whose player's slot holds
     * no task draws nothing. the record is null only there, since every task
     * in a slot holds its player's record */
    if (player_tasks[ricochet->player] == NULL) {
        njFogEnable();
        return;
    }
    record = player_tasks[ricochet->player]->extra.p;
#endif
    if (record->by_character.mech.effects_texlist != NULL) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(record->by_character.mech.effects_texlist);
        ricochet_sprite->p = work->pos;
        ricochet_sprite->tlist = record->by_character.mech.effects_texlist;
        if (player_records[ricochet->player]->upgrades & LASER_UPGRADES) {
            if (player_records[ricochet->player]->character ==
                PLAYER_EGGMAN_MECH) {
                cell = RICOCHET_EGGMAN_CELL;
            } else {
                cell = RICOCHET_TAILS_CELL;
            }
        } else {
            cell = RICOCHET_PLAIN_CELL;
        }
        njDrawSprite3D(ricochet_sprite, cell,
                       NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

static void ricochet_step(struct task *task)
{
    struct ricochet *ricochet = task->extra.p;
    struct player *record = player_tasks[ricochet->player]->extra.p;
    struct task_work *work = task->work;
    /* @hack redundant: a local that only sizes the frame */
    NJS_POINT3 unused;

    /* @hack spelling: a kill and a return of its own for each test, where one
     * test of both would do */
    if (record == NULL) {
        task_kill(task);
        return;
    }
    if (game_state.winner_posed == 1) {
        task_kill(task);
        return;
    }
    ricochet->last_pos = work->pos;
    njAddVector(&work->pos, &ricochet->spd);
    walker_tracer_create(ricochet->player, &ricochet->last_pos, &work->pos);
    if (placement_try_leave_by(task, RICOCHET_RANGE)) {
        task_kill(task);
    } else {
        work->timer++;
    }
}

void walker_ricochet_create(int player, NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    struct task *task;
    struct ricochet *ricochet;
    struct task_work *work;

    task = shot_task_create(NULL, ricochet_step);
    if (task != NULL) {
        ricochet = task->extra.p;
        work = task->work;
        ricochet->player = player;
        /* @hack spelling: one chained copy, where two copies of *pos would
         * do */
        work->pos = ricochet->last_pos = *pos;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL,
                  RICOCHET_TURN - (rand() * (RICOCHET_TURN_SPAN * RAND_SCALE)));
        njRotateX(NULL,
                  RICOCHET_TILT - (rand() * (2 * RICOCHET_TILT * RAND_SCALE)));
        njRotateZ(NULL,
                  RICOCHET_TILT - (rand() * (2 * RICOCHET_TILT * RAND_SCALE)));
        njCalcVector(NULL, dir, &ricochet->spd);
        njPopMatrixEx();
        ricochet->spd.x *= RICOCHET_SPEED;
        ricochet->spd.y *= RICOCHET_SPEED;
        ricochet->spd.z *= RICOCHET_SPEED;
        task->draw = ricochet_draw;
    }
}
