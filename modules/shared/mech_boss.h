/* the mech bosses' helpers: the collision, textures and effects that Big Foot,
 * Hot Shot and Flying Dog share */
#ifndef SHARED_MECH_BOSS_H
#define SHARED_MECH_BOSS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "system/angle.h"

struct task;

/* the bosses' texture lists. mech_boss_textures_load loads grobo_texlist,
 * grobo_title_moji_texlist, grobo_eff2_texlist and the given boss's title.
 * shared/gun_mech/stage_textures.h's texture groups load jet1_texlist */
extern NJS_TEXLIST grobo_texlist;
extern NJS_TEXLIST jet1_texlist;
extern NJS_TEXLIST bigfoot_title_texlist;
extern NJS_TEXLIST hotshot_title_texlist;
extern NJS_TEXLIST fdog_title_texlist;
extern NJS_TEXLIST grobo_title_moji_texlist;
extern NJS_TEXLIST grobo_eff2_texlist;

/* the three mech bosses, as the boss arguments of these calls number them */
enum mech_boss { MECH_BOSS_BIG_FOOT, MECH_BOSS_HOT_SHOT, MECH_BOSS_FLYING_DOG };

/* a hurt boss blinks with a period of MECH_BOSS_BLINK_STEPS steps, and draws
 * in light MECH_BOSS_HURT_LIGHT for the first half of each */
#define MECH_BOSS_BLINK_STEPS 8
#define MECH_BOSS_HURT_LIGHT 4
/* the most a boss's sway turns its shapes and its capsule about x and about
 * z. the sway also moves the shapes up or down by up to MECH_BOSS_BOB_HEIGHT.
 * Big Foot's and Hot Shot's draws turn and move the body by the same amounts */
#define MECH_BOSS_SWAY_X_ANGLE (0.8F * ANGLE_PER_DEGREE)
#define MECH_BOSS_SWAY_Z_ANGLE (1.5F * ANGLE_PER_DEGREE)
#define MECH_BOSS_BOB_HEIGHT 0.9F
/* a boss's draw turns its body about the point MECH_BOSS_PIVOT_HEIGHT above
 * its work's pos */
#define MECH_BOSS_PIVOT_HEIGHT 35.0F
/* the steps a boss's throw locks player 0's stick */
#define MECH_BOSS_LAUNCH_LOCK_STEPS 60
/* vibration_start's power, frequency and time for a boss's rumble, and for
 * its crash */
#define MECH_BOSS_RUMBLE_POWER 1
#define MECH_BOSS_RUMBLE_FREQ 25
#define MECH_BOSS_RUMBLE_TIME 1
#define MECH_BOSS_CRASH_POWER 4
#define MECH_BOSS_CRASH_FREQ 45
#define MECH_BOSS_CRASH_TIME 2
/* a boss's opening lasts MECH_BOSS_INTRO_STEPS. as it ends, the messenger may
 * show any of the first MECH_BOSS_MESSAGES stage messages */
#define MECH_BOSS_INTRO_STEPS 450
#define MECH_BOSS_MESSAGES 3
/* while a boss charges, camera 0's BOSSPOINT mode takes level 1 or level 0.
 * it takes level 1 while player 0 stands on the ground within
 * MECH_BOSS_CAMERA_NEAR of the boss. the distance is across the ground */
#define MECH_BOSS_CAMERA_NEAR 100.0F
/* the pods in each of a boss's two banks */
#define MECH_BOSS_PODS 4
/* the missiles a boss fires before each rest */
#define MECH_BOSS_VOLLEY 4
/* the push of each of a boss's shapes, and the hurts and flags most of them
 * take */
#define MECH_BOSS_PUSH (PUSH_FIRM(PUSH_ALL_BUT_PICKUPS) | PUSH_PLAYERS)
#define MECH_BOSS_HURTS                                                        \
    (SHAPE_HURTS_OBJECTS | SHAPE_HURTS_PLAYERS | SHAPE_GUARD_LEVEL(3))
#define MECH_BOSS_SHAPE_FLAGS (SHAPE_TURN_YZX | SHAPE_HURTABLE)
/* a boss's steer. its turn speed loses MECH_BOSS_TURN_DRAG of itself each
 * step. the body rolls toward MECH_BOSS_ROLL_PER_TURN degrees for each degree a
 * step it turns. it pitches toward MECH_BOSS_PITCH_PER_SPEED2 degrees for each
 * unit its squared speed across the ground gains in a step */
#define MECH_BOSS_TURN_DRAG 0.06F
#define MECH_BOSS_ROLL_PER_TURN 16.0F
#define MECH_BOSS_PITCH_PER_SPEED2 70.0F
/* the speed, in x and y, at which a hit on a boss's capsule throws player 0,
 * as player_launch_speed takes it. Hot Shot throws harder while it flies */
#define MECH_BOSS_THROW_X (-2.5F)
#define MECH_BOSS_THROW_Y 1.8F

/* a shape's attack and guard, each a level from 0 to 3 */
struct mech_boss_levels {
    Sint8 attack;
    Sint8 guard;
};

/* how mech_boss_ring places its puffs */
struct mech_boss_ring {
    /* how many puffs: 4, 6, 8 or 16. any other count spaces the puffs by an
     * angle the call leaves unset */
    Sint8 count;
    Float radius;
    Float scale; /* each puff's, and what spd is scaled by */
    NJS_VECTOR spd; /* each puff's speed, turned with the puff about y */
};

/* the frame in which mech_boss_smoke_points places its puffs */
struct mech_boss_smoke {
    Sint8 count; /* how many points */
    /* the frame's angles, which turn it by njRotateY, njRotateZ and
     * njRotateX, in that order */
    struct angles ang;
    NJS_POINT3 pos; /* where the frame's origin lies */
};

/* make spec's count puffs of dust_create in a ring about pos, level in y.
 * the ring starts at a random angle. each puff stands radius out from pos,
 * and moves at spd times scale, turned with it about y */
void mech_boss_ring(NJS_POINT3 *pos, struct mech_boss_ring *spec);
/* give each shape of task's collision record an attack of 0 and a guard of
 * 3. it keeps each shape's other bits of levels. task's work and its record
 * must not be null, unchecked */
void mech_boss_guard_all(struct task *task);
/* give each shape of task's collision record the attack and guard of its
 * entry of levels, in order. it keeps each shape's other bits of levels.
 * levels must hold an entry for each shape. task's work and its record
 * must not be null, unchecked */
void mech_boss_set_levels(struct task *task, struct mech_boss_levels *levels);
/* give each shape of task's collision record the angles (x, y, z). task's
 * work and its record must not be null, unchecked. no code in the game's
 * images calls mech_boss_set_shape_angles */
void mech_boss_set_shape_angles(struct task *task, Angle x, Angle y, Angle z);
/* add 1 to *counter, which wraps to 0 where it reaches count */
void mech_boss_count(Uint8 *counter, Uint8 count);
/* make a puff of black_smoke_rising_create for each of smoke's count points. a
 * point is a place in smoke's frame. each puff starts 1 below its point along
 * the frame's y. it starts rising at 1 a step up the world's y. it takes a
 * scale of 5. Ninja's matrix stack must have room for two matrices,
 * unchecked */
void mech_boss_smoke_points(struct mech_boss_smoke *smoke, NJS_POINT3 *points);
/* load the textures the three bosses share, boss's title, and the power
 * gauge's, as texture_load_pvm loads them. boss must be one of enum
 * mech_boss. any other loads no title */
void mech_boss_textures_load(Sint8 boss);
/* release the textures mech_boss_textures_load loaded for boss */
void mech_boss_textures_release(Sint8 boss);
/* where game_state.winner_posed is 1, put task's work at *pos and return TRUE.
 * the work then faces player 0, as mover_turn_to_player turns it. otherwise
 * return FALSE and change nothing */
Bool mech_boss_pose_for_winner(struct task *task, NJS_POINT3 *pos);

#endif
