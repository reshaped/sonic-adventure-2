/* City Escape's cars. lines of cars drive paths: CARMAN, CARMAN_NEAR and
 * CARMANC in its object list. CARKAZ and CARKAZ_S are parked cars. a car pushes
 * the players out of its box. a crash throws it as a wreck, which tumbles and
 * bounces, then fades out. the truck crashes each car it meets */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/cars.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/earth_shake.h"
#include "shared/flash.h"
#include "shared/modifier_volume.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg13/car.h"
#include "stg13/crash_piece.h"
#include "stg13/road_objects.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg13/car/text.inc"

/* a kind's flags */
/* a car of the kind turns back at its path's end, and forward again at its
 * start. one on a path takes a COLLISION_OBJECT_MUTUAL record, unless its kind
 * has CAR_WRECK_OBJECT */
#define CAR_SHUTTLES 0x1
/* the setup gives a car of the kind CAR_NO_PUSH_OUT */
#define CAR_NO_PUSH 0x2
/* the wreck turns half a turn about z, the car's long axis, and none about x
 * or y */
#define CAR_ROLL_ONLY 0x4
/* the wreck's collision record is a COLLISION_OBJECT, not a
 * COLLISION_OBJECT_MUTUAL */
#define CAR_WRECK_OBJECT 0x8

#define CAR_KINDS 30
/* a car's scl.x, scl.y and scl.z each gain TRUNCATE_SLACK before they
 * truncate */
#define TRUNCATE_SLACK 0.1F
#define KIND(work)                                                             \
    (&car_kinds[(Uint8)((work)->scl.y + TRUNCATE_SLACK) % CAR_KINDS])
/* where a car's scl.y + 0.1 truncates to KIND_RANDOM, its kind is random. it
 * is one of the KIND_RANDOM_COUNT kinds from KIND_RANDOM_FIRST. where it
 * truncates to KIND_RANDOM_NO_PUSH, the kind is one of the next
 * KIND_RANDOM_COUNT, which push no player. a car on a path picks again as it
 * starts the path over */
#define KIND_RANDOM 0
#define KIND_RANDOM_NO_PUSH 1
#define KIND_RANDOM_FIRST 6
#define KIND_RANDOM_COUNT 6
/* a parked car's kind lies PARKED_KIND_OFFSET past the one its scl.y names,
 * but for KIND_CABLE_CAR and KIND_CABLE_CAR_SHUTTLE. kinds 18 to 29 are kinds
 * 6 to 17 with one-piece models */
#define PARKED_KIND_OFFSET 12.0F
#define KIND_CABLE_CAR 2
#define KIND_CABLE_CAR_SHUTTLE 3

/* a car's work's submode */
/* the car pushes no player out of its box. a player in its box crashes it
 * with any touch */
#define CAR_NO_PUSH_OUT 0x1
/* the car is a line's child, and stands while its line waits. its setup
 * places it at the segment its work's timer holds, LINE_FRACTION along it. it
 * never tests its own range */
#define CAR_IN_LINE 0x2
/* the car is parked, on no path. a player in its box leaves through the box's
 * nearer side. without the bit, a player in the box's +z half leaves through a
 * side in x */
#define CAR_PARKED 0x4
/* the car's path stays where its points lie. without the bit, the path's
 * center moves to the line's pos. the path then turns by the line's ang.y */
#define CAR_PATH_IN_PLACE 0x8
/* the car's last draw found it out of view. it then drives on, but its bob
 * only fades. it pushes no player, and its shapes are off */
#define CAR_UNSEEN 0x10
/* the setup picked the car's kind at random, as KIND_RANDOM or
 * KIND_RANDOM_NO_PUSH asks */
#define CAR_RANDOM 0x20
#define CAR_RANDOM_NO_PUSH 0x40
/* the car neither draws nor pushes, and its shapes are off. nothing in this
 * object sets the bit */
#define CAR_HIDDEN 0x80

/* a car's modes, in its work's mode */
enum car_mode { MODE_FORWARD, MODE_BACKWARD };
/* a line's modes, in its work's mode. a CARMANC line waits while no player is
 * near it */
enum traffic_mode { MODE_GO, MODE_WAIT };

/* a car's record, in task->motion. segment and fraction place the car on its
 * path, as path_move moves them. center is the middle of the box that holds
 * the path's points. the points turn by path_ang about center, which then
 * moves to path_pos. a line keeps its path and its place on it in the same
 * record */
struct car {
    /* the bob's speed in y. x and z stay 0 */
    NJS_VECTOR spd;
    int segment;
    Float fraction;
    Uint8 unk_14[0x10];
    NJS_POINT3 path_pos;
    Angle path_ang;
    NJS_POINT3 center;
    struct path *path;
    /* the rocking the draw adds to the work's ang */
    struct angles wobble;
    Uint8 unk_50[4];
    /* how high the body rides over its pos as it bobs */
    Float height;
    Uint8 unk_58[4];
    NJS_POINT3 last_pos; /* the work's pos at the step before */
    /* how far the body rises as it tilts about z */
    Float lift;
    /* the car's speed along its path, in units a step, below 0 as it backs */
    Float speed;
};
#define CAR(task) ((struct car *)(task)->motion.p)
/* a line keeps its path's length in task->unk_3c */
/* @hack type: the length read as a Float through unk_3c's address, where
 * (task)->unk_3c.f would do */
#define LINE_LENGTH(task) (*(Float *)&(task)->unk_3c)
/* a line's car finds in its task->unk_3c the fraction its setup starts at */
/* @hack type: the fraction read as a Float through unk_3c's address, where
 * (task)->unk_3c.f would do */
#define LINE_FRACTION(task) (*(Float *)&(task)->unk_3c)
/* a car on a path keeps in task->unk_3c whether it is slowing. the SLOW_
 * constants say when */
#define SLOWING(task) ((task)->unk_3c.l)

/* how hard a crash throws a car. a wreck keeps its crash's force in its
 * work's mode */
enum crash_force { CRASH_NUDGE, CRASH_HIT, CRASH_FLAME_HIT };

#define WRECK(task) ((struct wreck *)(task)->motion.p)
/* the kind a wreck was made of */
#define WRECK_KIND(task) ((struct car_kind *)(task)->extra.p)
/* the steps left before a wreck starts to fade, in task->unk_3c */
#define LIFE(task) ((task)->unk_3c.l)

/* each car's model with its wheels as children, then each car's model in one
 * piece. the cable car's model and the lines' paths follow */
/* clang-format off */
static NJS_TEXNAME taxi_wheeled_textures[8] = {
#include "stg13/car/taxi_wheeled_textures.inc"
};

static NJS_TEXLIST taxi_wheeled_texlist = {
#include "stg13/car/taxi_wheeled_texlist.inc"
};

static Sint16 taxi_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/taxi_wheeled_rear_wheels_polygons.inc"
};

static Sint32 taxi_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/taxi_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL taxi_wheeled_rear_wheels_model = {
#include "stg13/car/taxi_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT taxi_wheeled_rear_wheels = {
#include "stg13/car/taxi_wheeled_rear_wheels.inc"
};

static Sint16 taxi_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/taxi_wheeled_front_wheels_polygons.inc"
};

static Sint32 taxi_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/taxi_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL taxi_wheeled_front_wheels_model = {
#include "stg13/car/taxi_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT taxi_wheeled_front_wheels = {
#include "stg13/car/taxi_wheeled_front_wheels.inc"
};

static Sint16 taxi_wheeled_body_polygons[959] = {
#include "stg13/car/taxi_wheeled_body_polygons.inc"
};

static Sint32 taxi_wheeled_body_vertices[731] = {
#include "stg13/car/taxi_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL taxi_wheeled_body_model = {
#include "stg13/car/taxi_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT taxi_wheeled_object = {
#include "stg13/car/taxi_wheeled_object.inc"
};
static NJS_TEXNAME white_car_wheeled_textures[8] = {
#include "stg13/car/white_car_wheeled_textures.inc"
};

static NJS_TEXLIST white_car_wheeled_texlist = {
#include "stg13/car/white_car_wheeled_texlist.inc"
};

static Sint16 white_car_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/white_car_wheeled_front_wheels_polygons.inc"
};

static Sint32 white_car_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/white_car_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL white_car_wheeled_front_wheels_model = {
#include "stg13/car/white_car_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT white_car_wheeled_front_wheels = {
#include "stg13/car/white_car_wheeled_front_wheels.inc"
};

static Sint16 white_car_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/white_car_wheeled_rear_wheels_polygons.inc"
};

static Sint32 white_car_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/white_car_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL white_car_wheeled_rear_wheels_model = {
#include "stg13/car/white_car_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT white_car_wheeled_rear_wheels = {
#include "stg13/car/white_car_wheeled_rear_wheels.inc"
};

static Sint16 white_car_wheeled_body_polygons[941] = {
#include "stg13/car/white_car_wheeled_body_polygons.inc"
};

static Sint32 white_car_wheeled_body_vertices[675] = {
#include "stg13/car/white_car_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL white_car_wheeled_body_model = {
#include "stg13/car/white_car_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT white_car_wheeled_object = {
#include "stg13/car/white_car_wheeled_object.inc"
};
static NJS_TEXNAME black_car_wheeled_textures[8] = {
#include "stg13/car/black_car_wheeled_textures.inc"
};

static NJS_TEXLIST black_car_wheeled_texlist = {
#include "stg13/car/black_car_wheeled_texlist.inc"
};

static Sint16 black_car_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/black_car_wheeled_front_wheels_polygons.inc"
};

static Sint32 black_car_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/black_car_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL black_car_wheeled_front_wheels_model = {
#include "stg13/car/black_car_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT black_car_wheeled_front_wheels = {
#include "stg13/car/black_car_wheeled_front_wheels.inc"
};

static Sint16 black_car_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/black_car_wheeled_rear_wheels_polygons.inc"
};

static Sint32 black_car_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/black_car_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL black_car_wheeled_rear_wheels_model = {
#include "stg13/car/black_car_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT black_car_wheeled_rear_wheels = {
#include "stg13/car/black_car_wheeled_rear_wheels.inc"
};

static Sint16 black_car_wheeled_body_polygons[921] = {
#include "stg13/car/black_car_wheeled_body_polygons.inc"
};

static Sint32 black_car_wheeled_body_vertices[667] = {
#include "stg13/car/black_car_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL black_car_wheeled_body_model = {
#include "stg13/car/black_car_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT black_car_wheeled_object = {
#include "stg13/car/black_car_wheeled_object.inc"
};
static NJS_TEXNAME blue_car_wheeled_textures[8] = {
#include "stg13/car/blue_car_wheeled_textures.inc"
};

static NJS_TEXLIST blue_car_wheeled_texlist = {
#include "stg13/car/blue_car_wheeled_texlist.inc"
};

static Sint16 blue_car_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/blue_car_wheeled_front_wheels_polygons.inc"
};

static Sint32 blue_car_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/blue_car_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL blue_car_wheeled_front_wheels_model = {
#include "stg13/car/blue_car_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT blue_car_wheeled_front_wheels = {
#include "stg13/car/blue_car_wheeled_front_wheels.inc"
};

static Sint16 blue_car_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/blue_car_wheeled_rear_wheels_polygons.inc"
};

static Sint32 blue_car_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/blue_car_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL blue_car_wheeled_rear_wheels_model = {
#include "stg13/car/blue_car_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT blue_car_wheeled_rear_wheels = {
#include "stg13/car/blue_car_wheeled_rear_wheels.inc"
};

static Sint16 blue_car_wheeled_body_polygons[947] = {
#include "stg13/car/blue_car_wheeled_body_polygons.inc"
};

static Sint32 blue_car_wheeled_body_vertices[795] = {
#include "stg13/car/blue_car_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL blue_car_wheeled_body_model = {
#include "stg13/car/blue_car_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT blue_car_wheeled_object = {
#include "stg13/car/blue_car_wheeled_object.inc"
};
static NJS_TEXNAME red_car_wheeled_textures[9] = {
#include "stg13/car/red_car_wheeled_textures.inc"
};

static NJS_TEXLIST red_car_wheeled_texlist = {
#include "stg13/car/red_car_wheeled_texlist.inc"
};

static Sint16 red_car_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/red_car_wheeled_front_wheels_polygons.inc"
};

static Sint32 red_car_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/red_car_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL red_car_wheeled_front_wheels_model = {
#include "stg13/car/red_car_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT red_car_wheeled_front_wheels = {
#include "stg13/car/red_car_wheeled_front_wheels.inc"
};

static Sint16 red_car_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/red_car_wheeled_rear_wheels_polygons.inc"
};

static Sint32 red_car_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/red_car_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL red_car_wheeled_rear_wheels_model = {
#include "stg13/car/red_car_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT red_car_wheeled_rear_wheels = {
#include "stg13/car/red_car_wheeled_rear_wheels.inc"
};

static Sint16 red_car_wheeled_body_polygons[943] = {
#include "stg13/car/red_car_wheeled_body_polygons.inc"
};

static Sint32 red_car_wheeled_body_vertices[675] = {
#include "stg13/car/red_car_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL red_car_wheeled_body_model = {
#include "stg13/car/red_car_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT red_car_wheeled_object = {
#include "stg13/car/red_car_wheeled_object.inc"
};
static NJS_TEXNAME civic_wheeled_textures[7] = {
#include "stg13/car/civic_wheeled_textures.inc"
};

static NJS_TEXLIST civic_wheeled_texlist = {
#include "stg13/car/civic_wheeled_texlist.inc"
};

static Sint16 civic_wheeled_front_wheels_polygons[39] = {
#include "stg13/car/civic_wheeled_front_wheels_polygons.inc"
};

static Sint32 civic_wheeled_front_wheels_vertices[35] = {
#include "stg13/car/civic_wheeled_front_wheels_vertices.inc"
};

static NJS_CNK_MODEL civic_wheeled_front_wheels_model = {
#include "stg13/car/civic_wheeled_front_wheels_model.inc"
};

static NJS_CNK_OBJECT civic_wheeled_front_wheels = {
#include "stg13/car/civic_wheeled_front_wheels.inc"
};

static Sint16 civic_wheeled_rear_wheels_polygons[39] = {
#include "stg13/car/civic_wheeled_rear_wheels_polygons.inc"
};

static Sint32 civic_wheeled_rear_wheels_vertices[35] = {
#include "stg13/car/civic_wheeled_rear_wheels_vertices.inc"
};

static NJS_CNK_MODEL civic_wheeled_rear_wheels_model = {
#include "stg13/car/civic_wheeled_rear_wheels_model.inc"
};

static NJS_CNK_OBJECT civic_wheeled_rear_wheels = {
#include "stg13/car/civic_wheeled_rear_wheels.inc"
};

static Sint16 civic_wheeled_body_polygons[851] = {
#include "stg13/car/civic_wheeled_body_polygons.inc"
};

static Sint32 civic_wheeled_body_vertices[683] = {
#include "stg13/car/civic_wheeled_body_vertices.inc"
};

static NJS_CNK_MODEL civic_wheeled_body_model = {
#include "stg13/car/civic_wheeled_body_model.inc"
};

static NJS_CNK_OBJECT civic_wheeled_object = {
#include "stg13/car/civic_wheeled_object.inc"
};
#include "shared/car_models/taxi_fixed_model.h"
static NJS_TEXNAME white_car_fixed_textures[8] = {
#include "stg13/car/white_car_fixed_textures.inc"
};

static NJS_TEXLIST white_car_fixed_texlist = {
#include "stg13/car/white_car_fixed_texlist.inc"
};

static Sint16 white_car_fixed_body_polygons[991] = {
#include "stg13/car/white_car_fixed_body_polygons.inc"
};

static Sint32 white_car_fixed_body_vertices[739] = {
#include "stg13/car/white_car_fixed_body_vertices.inc"
};

static NJS_CNK_MODEL white_car_fixed_body_model = {
#include "stg13/car/white_car_fixed_body_model.inc"
};

static NJS_CNK_OBJECT white_car_fixed_object = {
#include "stg13/car/white_car_fixed_object.inc"
};
#include "shared/car_models/fixed_models.h"

static NJS_TEXNAME cable_car_textures[14] = {
#include "stg13/car/cable_car_textures.inc"
};

static NJS_TEXLIST cable_car_texlist = {
#include "stg13/car/cable_car_texlist.inc"
};

static Sint16 cable_car_body_polygons[1251] = {
#include "stg13/car/cable_car_body_polygons.inc"
};

static Sint32 cable_car_body_vertices[1279] = {
#include "stg13/car/cable_car_body_vertices.inc"
};

static NJS_CNK_MODEL cable_car_body_model = {
#include "stg13/car/cable_car_body_model.inc"
};

static NJS_CNK_OBJECT cable_car_object = {
#include "stg13/car/cable_car_object.inc"
};
/* the lines' thirteen paths, each an open line through its points.
 * car_path_1 is the truck's too */

static NJS_POINT3 car_path_0_points[9] = {
#include "stg13/car/car_path_0_points.inc"
};

static struct path car_path_0 = {
#include "stg13/car/car_path_0.inc"
};

static NJS_POINT3 car_path_1_points[67] = {
#include "stg13/car/car_path_1_points.inc"
};

struct path car_path_1 = {
#include "stg13/car/car_path_1.inc"
};

static NJS_POINT3 car_path_2_points[102] = {
#include "stg13/car/car_path_2_points.inc"
};

static struct path car_path_2 = {
#include "stg13/car/car_path_2.inc"
};

static NJS_POINT3 car_path_3_points[59] = {
#include "stg13/car/car_path_3_points.inc"
};

static struct path car_path_3 = {
#include "stg13/car/car_path_3.inc"
};

static NJS_POINT3 car_path_4_points[35] = {
#include "stg13/car/car_path_4_points.inc"
};

static struct path car_path_4 = {
#include "stg13/car/car_path_4.inc"
};

static NJS_POINT3 car_path_5_points[10] = {
#include "stg13/car/car_path_5_points.inc"
};

static struct path car_path_5 = {
#include "stg13/car/car_path_5.inc"
};

static NJS_POINT3 car_path_6_points[51] = {
#include "stg13/car/car_path_6_points.inc"
};

static struct path car_path_6 = {
#include "stg13/car/car_path_6.inc"
};

static NJS_POINT3 car_path_7_points[55] = {
#include "stg13/car/car_path_7_points.inc"
};

static struct path car_path_7 = {
#include "stg13/car/car_path_7.inc"
};

static NJS_POINT3 car_path_8_points[22] = {
#include "stg13/car/car_path_8_points.inc"
};

static struct path car_path_8 = {
#include "stg13/car/car_path_8.inc"
};

static NJS_POINT3 car_path_9_points[12] = {
#include "stg13/car/car_path_9_points.inc"
};

static struct path car_path_9 = {
#include "stg13/car/car_path_9.inc"
};

static NJS_POINT3 car_path_10_points[14] = {
#include "stg13/car/car_path_10_points.inc"
};

static struct path car_path_10 = {
#include "stg13/car/car_path_10.inc"
};

static NJS_POINT3 car_path_11_points[15] = {
#include "stg13/car/car_path_11_points.inc"
};

static struct path car_path_11 = {
#include "stg13/car/car_path_11.inc"
};

static NJS_POINT3 car_path_12_points[6] = {
#include "stg13/car/car_path_12_points.inc"
};

static struct path car_path_12 = {
#include "stg13/car/car_path_12.inc"
};
/* clang-format on */

NJS_TEXLIST *car_texlists[17] = {
#include "stg13/car/car_texlists.inc"
};
/* each step takes car_bob_gravity from the bob's speed. below 0, the bob also
 * adds car_bob_spring of its depth to its speed. a body falling below
 * BOB_BOUNCE_HEIGHT bounces up at car_bob_bounce of its speed. the speed then
 * keeps car_bob_damping of itself */
static Float car_bob_gravity = 0.052F;
static Float car_bob_spring = 0.035F;
static Float car_bob_bounce = 0.3F;
static Float car_bob_damping = 0.99F;
/* the truck's crash throws car_debris_count pieces of debris at
 * car_debris_carry times its velocity. each piece also flies toward the
 * camera, at random up to car_debris_toward plus car_debris_toward_truck
 * times the truck's speed. it rises at car_debris_lift, plus up to
 * car_debris_lift_random more at random. each axis of its speed then changes
 * at random by up to 0.5 either way. car_debris_spread scales a place across
 * the truck's way that nothing reads */
static Sint32 car_debris_count = 2;
static Float car_debris_carry = 1.5F;
static Float car_debris_toward = 1.0F;
static Float car_debris_lift = 0.5F;
static Float car_debris_lift_random = 3.0F;
static Float car_debris_spread = 60.0F;
static Float car_debris_toward_truck = 0.5F;
/* while cars_come_back is set, a crash kills the car's task alone, so its
 * placement makes the car again. no code sets the flag */
static Bool cars_come_back = FALSE;
/* the truck's crash adds car_wreck_carry times the truck's velocity to its
 * wreck's speed. with a chance of WRECK_CARRY_CHANCE, the factor is
 * car_wreck_carry_random plus up to car_wreck_carry_random_add instead. with a
 * chance of WRECK_TOWARD_CHANCE, the wreck also flies toward the camera. it
 * flies at random up to car_wreck_toward plus car_wreck_toward_truck times the
 * truck's speed. it rises at car_wreck_lift plus up to car_wreck_lift_random
 * more, at random. with a chance of WRECK_LIFT_CHANCE, it rises at the two's
 * sum instead. each axis of its speed then changes at random by up to half
 * car_wreck_jitter either way */
static Float car_wreck_toward = 2.0F;
static Float car_wreck_toward_truck = 0.3F;
static Float car_wreck_carry = 0.92F;
static Float car_wreck_carry_random = 0.9F;
static Float car_wreck_carry_random_add = 0.5F;
static Float car_wreck_lift = 0.2F;
static Float car_wreck_lift_random = 1.0F;
static Float car_wreck_jitter = 2.0F;
/* the most the truck's crash kicks a wreck sideways, past WRECK_KICK_LEAST */
static Float car_wreck_kick = 1.3F;
static Float car_crash_flash_scale = 6.0F;
/* the volume of a driving car's engine, near a player */
static Sint32 car_engine_volume = -20;
static Sint32 car_crash_volume = 60;
static Sint16 car_land_volume = 30;
/* the truck's box, in the truck's own axes */
/* clang-format off */
static struct collision_shape car_truck_shape[1] = {
    {0, SHAPE_BOX2,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_NO_OWN_HITS,
     {0.0F, 0.0F, -100.0F}, 250.0F, 45.0F, 210.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the truck car_set_truck names, or null */
static struct task *car_truck = NULL;
/* the point the shadow's foot shrinks toward, at the foot's height */
static NJS_POINT3 car_shadow_focus = {0.0F, -200.0F, 0.0F};
/* a car's shapes are two spheres along its length, then as many for its
 * wreck. players, enemies and objects can't push the car's spheres, and the
 * wreck's hurt enemies. car_shapes_2 holds the same shapes again. the cable
 * cars' car_shapes_long hold three larger spheres of each */
static struct collision_shape car_shapes[4] = {
#include "stg13/car/car_shapes.inc"
};
static struct collision_shape car_shapes_2[4] = {
#include "stg13/car/car_shapes_2.inc"
};
static struct collision_shape car_shapes_long[6] = {
#include "stg13/car/car_shapes_long.inc"
};
/* TRUE once car_build has run */
static Bool car_kinds_built = FALSE;
static struct car_kind car_kinds[CAR_KINDS] = {
#include "stg13/car/car_kinds.inc"
};
/* the lines' paths. a line's scl.z, truncated, picks one, modulo CAR_PATHS */
#define CAR_PATHS 13
static struct path *car_paths[CAR_PATHS] = {
#include "stg13/car/car_paths.inc"
};
/* the waves whose sums rock a car about x, y and z. their time is the turn of
 * the car's wheels */
#define WOBBLE_WAVES 3
static struct wave car_wobble_x_waves[WOBBLE_WAVES] = {
    {24, 0.2F},
    {14, 0.3F},
    {3, 0.2F},
};
static struct wave car_wobble_y_waves[WOBBLE_WAVES] = {
    {30, 0.09F},
    {14, 0.08F},
    {1, 0.05F},
};
static struct wave car_wobble_z_waves[WOBBLE_WAVES] = {
    {29, 0.3F},
    {9, 0.2F},
    {2, 0.02F},
};
/* a crash's wreck starts at the speed car_crash_spd. it is turned about x by
 * CRASH_PITCH_PART of the car's ang.x, then about y by its ang.y */
static NJS_VECTOR car_crash_spd = {-0.5F, 2.3F, -0.01F};
/* the span of the random turn a wreck's spin takes about each axis, in degrees
 * a step. the turn, centered on 0, comes as the wreck lands on its wheels
 * after a CRASH_HIT or harder */
static Float car_land_spin_x = 1.5F;
static Float car_land_spin_y = 0.1F;
static Float car_land_spin_z = 1.5F;
/* a tilt about z lifts a car's body by the height it gives the box's corner,
 * less car_lift_slack. that corner is (box.x, 0, box.z). the lift is never
 * below 0 */
static Float car_lift_slack = 1.0F;
/* clang-format off */
static struct collision_shape dat_8c67e3c8[1] = {
    {0, SHAPE_SPHERE,
     PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(1) |
     SHAPE_HURT | SHAPE_HURTS_ENEMIES,
     SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 18.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* what each step takes from the wreck's spd.y */
static Float car_gravity = 0.1F;
static NJS_VECTOR car_up = {0.0F, 1.0F, 0.0F};
/* EASE_TURN eases a settling wreck's spin with a limit and a divisor. on its
 * roof, those about x and z are car_upside_limit and car_upside_divisor. on
 * its wheels, those about each axis are car_land_limit and car_land_divisor */
static Sint32 car_upside_limit = 160;
static Float car_upside_divisor = 80.0F;
static Sint32 car_land_limit = 60;
static Float car_land_divisor = 30.0F;
/* a wreck keeps car_upside_spin_keep of its spin on each step it settles on
 * its roof. it keeps car_land_spin_keep on a step it lands on its wheels after
 * a CRASH_HIT or harder */
static Float car_upside_spin_keep = 0.96F;
static Float car_land_spin_keep = 0.1F;

static void car_destroy(struct task *task);
static void car_step(struct task *task);
static void car_draw(struct task *task);
static struct task *car_wreck_create(struct car_kind *kind, NJS_POINT3 *pos,
                                     struct angles *ang, NJS_VECTOR *spd,
                                     NJS_POINT3 *floor_pos,
                                     struct angles *floor_ang, Sint8 force,
                                     int life);
static void car_wreck_step(struct task *task);
static void car_wreck_draw(struct task *task);
static void car_wreck_destroy(struct task *task);

void car_set_truck(struct task *task)
{
    car_truck = task;
}

#define SHADOW_TOP 3.5F
#define SHADOW_SHRINK 0.12F
#define SHADOW_SLANT 70.0F
#define SHADOW_REACH 200.0F
#define SHADOW_CORNERS 4

/* draw a shadow under the current matrix, as a modifier volume. its top is a
 * 2 * half_x by 2 * half_z rectangle, SHADOW_TOP up. ang turns it about z,
 * then x, then y. its foot is the top shrunk to SHADOW_SHRINK of its size about
 * car_shadow_focus, at car_shadow_focus's height. the foot then moves
 * SHADOW_SLANT along x and along z. the volume is a copy of
 * modifier_volume_box_model at WORK_AREA, over what was there. the call never
 * returns where NJD_CE doesn't end the box's vertices */
static void car_draw_shadow(Float half_x, Float half_z, struct angles *ang)
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
            vx = ((vx - car_shadow_focus.x) * SHADOW_SHRINK) +
                 car_shadow_focus.x + SHADOW_SLANT;
            vz = ((vz - car_shadow_focus.z) * SHADOW_SHRINK) +
                 car_shadow_focus.z + SHADOW_SLANT;
            vy = car_shadow_focus.y;
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
static int car_find_hit(struct task *task, int *force)
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

Uint8 *car_build(Uint8 *buf)
{
    int i;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *texlist;
    Uint8 *end;

    if (!car_kinds_built) {
        car_kinds_built = TRUE;
        for (i = 0; i < CAR_KINDS; i++) {
            /* @hack spelling: object and texlist, locals for the kind's
             * fields, where the fields read in place would do */
            object = car_kinds[i].object;
            texlist = car_kinds[i].texlist;
            car_kinds[i].build = NULL;
            if (object && texlist) {
                njSetTexture(texlist);
                end = buf + njCnkDirectObjectCompileSize(object);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(object, buf);
                    car_kinds[i].build = (NJS_DIRECT_HEAD *)buf;
                    buf = end;
                }
            }
        }
    }
    return buf;
}

/* car_random_kind and the debris' pick of a model each take a random whole
 * number under RANDOM_PICKS. each takes it modulo its count */
#define RANDOM_PICKS 100.0F

static int car_random_kind(void)
{
    Float pick;
    int kind;

    /* @hack spelling: pick and kind, locals for the random number, where one
     * expression would do */
    pick = rand() * (RANDOM_PICKS * RAND_SCALE);
    kind = pick;
    return (kind % KIND_RANDOM_COUNT) + KIND_RANDOM_FIRST;
}

/* the sum of count waves at time, in degrees */
static Float wave_sum(struct wave *waves, int count, Uint16 time)
{
    Float sum = 0.0F;
    Uint32 at = time;

    while (count--) {
        sum += njSin(waves->freq * at) * waves->amp;
        waves++;
    }
    return sum;
}

/* the vector helpers take each vector as its three Floats, x, y and z.
 * points_difference sets out to to - from, and points_sum sets out to base +
 * delta. vector_scale_by multiplies vec by scale. a helper whose name extends
 * another's gives the same result. its tag says how its spelling differs */
#pragma inline(points_difference, vector_scale_by, points_sum)
/* @hack spelling: points_difference, vector_scale_by and points_sum take the
 * parts through walking pointers, where three assignments would do */
static void points_difference(Float *from, Float *to, Float *out)
{
    *out++ = *to++ - *from++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}

static void vector_scale_by(Float *vec, Float scale)
{
    *vec++ *= scale;
    *vec++ *= scale;
    *vec *= scale;
}

static void points_sum(Float *base, Float *delta, Float *out)
{
    *out++ = *delta++ + *base++;
    *out++ = *delta++ + *base++;
    *out = *delta + *base;
}

/* @hack spelling: the factor first, where vector_scale_by's order would do */
#pragma inline(vector_scale_by_swapped)
static void vector_scale_by_swapped(Float scale, Float *vec)
{
    *vec++ *= scale;
    *vec++ *= scale;
    *vec *= scale;
}

#define EXTEND_STEP 100.0F

/* set *out to point index of the count points, and return FALSE. an index
 * past either end returns TRUE, and extends the path in steps of EXTEND_STEP.
 * past the last point, *out lies on the line from point count - 3 through the
 * last. before the first, it lies on the line from point 2 through point 0.
 * count must be 3 or more, unchecked */
static Bool path_point(NJS_POINT3 *points, int count, int index,
                       NJS_POINT3 *out)
{
    Bool beyond;
    NJS_VECTOR dir;

    if (index >= count) {
        points_difference(&points[count - 3].x, &points[count - 1].x, &dir.x);
        vector_scale_by(&dir.x,
                        (index - count + 1) * EXTEND_STEP / njScalor(&dir));
        points_sum(&points[count - 1].x, &dir.x, &out->x);
        beyond = TRUE;
    } else if (index < 0) {
        points_difference(&points[2].x, &points[0].x, &dir.x);
        vector_scale_by(&dir.x, -index * EXTEND_STEP / njScalor(&dir));
        points_sum(&points[0].x, &dir.x, &out->x);
        beyond = TRUE;
    } else {
        *out = points[index];
        beyond = FALSE;
    }
    return beyond;
}

/* move a place on path by dist: ahead for a dist above 0, and back otherwise.
 * *index is the point that starts the place's segment. *fraction is how far
 * along the segment the place lies, from 0 to 1. the call moves both, and sets
 * *out to the place. it returns TRUE where a segment it passes reaches past
 * an end, and FALSE otherwise.
 *
 * path must be a PATH_LAYOUT_POS path without PATH_CLOSED. on another the call
 * returns FALSE and changes nothing. no two neighbouring points may coincide,
 * or a fraction divides by 0 */
static Bool path_move(struct path *path, int *index, Float *fraction,
                      Float dist, NJS_POINT3 *out)
{
    Bool beyond = FALSE;
    int count;
    NJS_POINT3 *points;
    int i;
    int j;
    Float len;
    Float frac;
    Float part;
    Float done;
    Float left;
    Float rest;
    NJS_POINT3 p0;
    NJS_POINT3 p1;

    if (!path->flags) {
        points = path->points;
        count = path->count;
        if (dist > 0.0F) {
            i = *index;
            j = i + 1;
            frac = *fraction;
            for (;;) {
                if (path_point(points, count, i, &p0)) {
                    beyond = TRUE;
                }
                if (path_point(points, count, j, &p1)) {
                    beyond = TRUE;
                }
                len = njDistanceP2P(&p0, &p1);
                done = len * frac;
                part = len - done;
                if (part < 0.0F) {
                    part = 0.0F;
                }
                if (dist > part) {
                    dist -= part;
                } else {
                    rest = (part - dist) / len;
                    frac = 1.0F - rest;
                    if (frac < 0.0F) {
                        frac = 0.0F;
                        rest = 1.0F;
                    }
                    *index = i;
                    *fraction = frac;
                    out->x = (p0.x * rest) + (p1.x * frac);
                    out->y = (p0.y * rest) + (p1.y * frac);
                    out->z = (p0.z * rest) + (p1.z * frac);
                    return beyond;
                }
                i++;
                frac = 0.0F;
                j++;
            }
        } else {
            i = *index;
            j = i + 1;
            frac = *fraction;
            for (;;) {
                if (path_point(points, count, i, &p0)) {
                    beyond = TRUE;
                }
                if (path_point(points, count, j, &p1)) {
                    beyond = TRUE;
                }
                len = njDistanceP2P(&p0, &p1);
                done = len * frac;
                left = len - done;
                if (left < 0.0F) {
                    done = len;
                }
                if (-dist > done) {
                    dist += done;
                } else {
                    done += dist;
                    frac = done / len;
                    rest = 1.0F - frac;
                    if (rest < 0.0F) {
                        frac = 1.0F;
                        rest = 0.0F;
                    }
                    *index = i;
                    *fraction = frac;
                    out->x = (p0.x * rest) + (p1.x * frac);
                    out->y = (p0.y * rest) + (p1.y * frac);
                    out->z = (p0.z * rest) + (p1.z * frac);
                    return beyond;
                }
                i--;
                frac = 1.0F;
                j--;
            }
        }
    }
    return FALSE;
}

/* set *center to the middle of the box that holds path's points. path must
 * hold a point or more */
static void path_center(struct path *path, NJS_POINT3 *center)
{
    NJS_POINT3 *points = path->points;
    int i;
    Float x;
    Float y;
    Float z;
    /* @hack spelling: the maxima declared before the minima, where either
     * order would do */
    Float max_x;
    Float max_y;
    Float max_z;
    Float min_x;
    Float min_y;
    Float min_z;

#ifdef NONMATCHING
    /* GCC can't see that the first point sets every bound, and warns that one
     * may be read unset. each of car_paths holds 6 points or more, so these
     * zeros are never read */
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

/* move a car's place on path by dist, as path_move moves *index and
 * *fraction, and return its result. kind's front and back then take their
 * places from there. *pos takes the midpoint of those two places. *ang takes
 * the heading from the back's place to the front's, with z 0 */
static Bool car_place_on_path(struct car_kind *kind, struct path *path,
                              int *index, Float *fraction, Float dist,
                              NJS_POINT3 *pos, struct angles *ang)
{
    Bool beyond;
    int back_index;
    int front_index;
    Float back_along;
    Float front_along;
    NJS_VECTOR way;
    NJS_POINT3 front;
    NJS_POINT3 back;
    NJS_POINT3 place;

    beyond = path_move(path, index, fraction, dist, &place);
    /* @hack spelling: the copies chained back first, where front first would
     * do */
    front_along = back_along = *fraction;
    front_index = back_index = *index;
    path_move(path, &front_index, &front_along, kind->front, &front);
    path_move(path, &back_index, &back_along, kind->back, &back);
    /* @hack spelling: way's address converted for the output, where &way.x
     * would do */
    points_difference(&back.x, &front.x, (Float *)&way);
    /* @hack spelling: way.z squared first, where way.x first would do */
    ang->x = atan2f(-way.y, njSqrt((way.z * way.z) + (way.x * way.x))) *
             ANGLE_PER_RADIAN;
    ang->y = atan2f(way.x, way.z) * ANGLE_PER_RADIAN;
    ang->z = 0;
    vector_scale_by_swapped(0.5F, &way.x);
    points_sum(&back.x, &way.x, &pos->x);
    return beyond;
}

/* set the work's pos and ang to the car's place and heading on its path. the
 * record's segment and fraction give the place. last_pos takes the same pos */
static void car_place(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: ang declared before pos, where either order would
     * do */
    struct angles ang;
    NJS_POINT3 pos;

    car_place_on_path(KIND(work), CAR(task)->path, &CAR(task)->segment,
                      &CAR(task)->fraction, 0.0F, &pos, &ang);
    ang.y += CAR(task)->path_ang;
    work->ang = ang;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &CAR(task)->path_pos);
    njRotateY(NULL, CAR(task)->path_ang);
    njTranslate(NULL, -CAR(task)->center.x, -CAR(task)->center.y,
                -CAR(task)->center.z);
    njCalcPoint(NULL, &pos, &work->pos);
    njPopMatrixEx();
    CAR(task)->last_pos = work->pos;
}

/* give each of task's shapes the center of kind's shape of the same index,
 * placed by a matrix. then fit the record's radius to the shapes. the matrix is
 * the current one, or under own_matrix that of the work's pos and ang alone.
 * the record must hold at least kind's shape_count shapes */
static void car_place_shapes(struct task *task, Bool own_matrix,
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
    for (i = 0; i < kind->shape_count; i++) {
        njCalcPoint(NULL, &kind->shapes[i].center,
                    &work->collision->shapes[i].center);
    }
    if (own_matrix) {
        njPopMatrixEx();
    }
    collision_fit_radius(task);
}

/* the height a parked car's body starts at */
#define CAR_SINK (-1.5F)

static void car_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct car_kind *kind;

    if (!(work->submode & CAR_IN_LINE) && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct car));
    if (!task->motion.p) {
        return;
    }
    if ((int)(work->scl.y + TRUNCATE_SLACK) == KIND_RANDOM) {
        work->submode |= CAR_RANDOM;
    }
    if ((int)(work->scl.y + TRUNCATE_SLACK) == KIND_RANDOM_NO_PUSH) {
        work->submode |= CAR_RANDOM_NO_PUSH;
    }
    if (work->submode & CAR_RANDOM) {
        work->scl.y = car_random_kind();
    }
    if (work->submode & CAR_RANDOM_NO_PUSH) {
        work->scl.y = car_random_kind() + KIND_RANDOM_COUNT;
    }
    if (work->submode & CAR_PARKED &&
        (int)(work->scl.y + TRUNCATE_SLACK) != KIND_CABLE_CAR &&
        (int)(work->scl.y + TRUNCATE_SLACK) != KIND_CABLE_CAR_SHUTTLE) {
        work->scl.y += PARKED_KIND_OFFSET;
    }
    if (KIND(work)->flags & CAR_NO_PUSH) {
        work->submode |= CAR_NO_PUSH_OUT;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = car_draw;
    task->step = car_step;
    task->destructor = car_destroy;
    if (!(work->submode & CAR_PARKED)) {
        CAR(task)->path =
            car_paths[(Uint8)(work->scl.z + TRUNCATE_SLACK) % CAR_PATHS];
        path_center(CAR(task)->path, &CAR(task)->center);
        if (work->submode & CAR_IN_LINE && task->parent) {
            if (work->submode & CAR_PATH_IN_PLACE) {
                /* @hack spelling: car, a local for CAR(task), where
                 * CAR(task) on both sides would do */
                struct car *car = CAR(task);

                car->path_pos = car->center;
                CAR(task)->path_ang = 0;
            } else {
                CAR(task)->path_pos = task->parent->work->pos;
                CAR(task)->path_ang = task->parent->work->ang.y;
            }
        }
        CAR(task)->segment = 0;
        CAR(task)->fraction = 0.0F;
        if (work->submode & CAR_IN_LINE) {
            CAR(task)->segment = work->timer;
            CAR(task)->fraction = LINE_FRACTION(task);
        }
        car_place(task);
    } else {
        /* @hack type: scl.x converted to a Uint32 for the test, where an int
         * would do */
        if ((Uint32)(work->scl.x + TRUNCATE_SLACK)) {
            work->submode |= CAR_NO_PUSH_OUT;
        }
        CAR(task)->height = CAR_SINK;
    }
    kind = KIND(work);
    collision_init(task, kind->shapes, kind->shape_count,
                   kind->flags & CAR_SHUTTLES &&
                           !(kind->flags & CAR_WRECK_OBJECT) &&
                           !(work->submode & CAR_PARKED)
                       ? COLLISION_OBJECT_MUTUAL
                       : COLLISION_OBJECT);
    /* @bug memory: goes on where collision_init fails, which leaves the work's
     * collision null. car_place_shapes then reads the record's shapes through
     * null (0x8c50186e). car_step tests the record (0x8c50247e), but reads its
     * flags untested (0x8c502aee) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init fails, the setup
     * destroys the car. set.c makes a parked car again on a later pass, and a
     * line goes on with one car fewer */
    if (work->collision == NULL) {
        task_destroy(task);
        return;
    }
#endif
    car_place_shapes(task, TRUE, KIND(work));
    CAR(task)->speed = work->mode == MODE_BACKWARD ? -work->scl.x : work->scl.x;
}

static void car_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place would
     * do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

/* about each axis, car_ease_fast turns 1 / EASE_FAST_PARTS_ of the way from
 * base toward ang. it turns no more than EASE_FAST_MAX_. car_ease_slow does so
 * by the EASE_SLOW_ constants */
#define EASE_FAST_PARTS_X 3
#define EASE_FAST_PARTS_Y 5
#define EASE_FAST_PARTS_Z 8
#define EASE_FAST_MAX_X NJM_DEG_ANG(8)
#define EASE_FAST_MAX_Y NJM_DEG_ANG(3)
#define EASE_FAST_MAX_Z NJM_DEG_ANG(2)
#define EASE_SLOW_PARTS_X 10
#define EASE_SLOW_PARTS_Y 10
#define EASE_SLOW_PARTS_Z 18
#define EASE_SLOW_MAX_X NJM_DEG_ANG(0.6)
#define EASE_SLOW_MAX_Y NJM_DEG_ANG(0.5)
#define EASE_SLOW_MAX_Z NJM_DEG_ANG(0.2)

/* set ang to base turned part of the way toward ang */
static void car_ease_fast(struct angles *ang, struct angles *base)
{
    Sint16 turn;

    turn = ang->x - base->x;
    /* @hack spelling: the division written out, where /= would do */
    turn = turn / EASE_FAST_PARTS_X;
    if (turn > EASE_FAST_MAX_X) {
        turn = EASE_FAST_MAX_X;
    }
    if (turn < -EASE_FAST_MAX_X) {
        turn = -EASE_FAST_MAX_X;
    }
    ang->x = base->x + turn;
    turn = ang->y - base->y;
    turn = turn / EASE_FAST_PARTS_Y;
    if (turn > EASE_FAST_MAX_Y) {
        turn = EASE_FAST_MAX_Y;
    }
    if (turn < -EASE_FAST_MAX_Y) {
        turn = -EASE_FAST_MAX_Y;
    }
    ang->y = base->y + turn;
    turn = ang->z - base->z;
    turn = turn / EASE_FAST_PARTS_Z;
    if (turn > EASE_FAST_MAX_Z) {
        turn = EASE_FAST_MAX_Z;
    }
    if (turn < -EASE_FAST_MAX_Z) {
        turn = -EASE_FAST_MAX_Z;
    }
    ang->z = base->z + turn;
}

/* car_ease_fast, with the EASE_SLOW_ constants */
static void car_ease_slow(struct angles *ang, struct angles *base)
{
    Sint16 turn;

    turn = ang->x - base->x;
    /* @hack spelling: the division written out, where /= would do */
    turn = turn / EASE_SLOW_PARTS_X;
    if (turn > EASE_SLOW_MAX_X) {
        turn = EASE_SLOW_MAX_X;
    }
    if (turn < -EASE_SLOW_MAX_X) {
        turn = -EASE_SLOW_MAX_X;
    }
    ang->x = base->x + turn;
    turn = ang->y - base->y;
    turn = turn / EASE_SLOW_PARTS_Y;
    if (turn > EASE_SLOW_MAX_Y) {
        turn = EASE_SLOW_MAX_Y;
    }
    if (turn < -EASE_SLOW_MAX_Y) {
        turn = -EASE_SLOW_MAX_Y;
    }
    ang->y = base->y + turn;
    turn = ang->z - base->z;
    turn = turn / EASE_SLOW_PARTS_Z;
    if (turn > EASE_SLOW_MAX_Z) {
        turn = EASE_SLOW_MAX_Z;
    }
    if (turn < -EASE_SLOW_MAX_Z) {
        turn = -EASE_SLOW_MAX_Z;
    }
    ang->z = base->z + turn;
}

/* vector_subtract subtracts sub from vec, and vector_add_to adds add to
 * vec */
#pragma inline(vector_scale_by_pointer, vector_subtract, vector_add_to)

/* @hack spelling: the factor read through a pointer at each part, where
 * vector_scale_by would do */
static void vector_scale_by_pointer(Float *vec, const Float *scale)
{
    *vec++ *= *scale;
    *vec++ *= *scale;
    *vec *= *scale;
}

/* @hack spelling: the parts through walking pointers, where three assignments
 * would do */
static void vector_subtract(Float *vec, Float *sub)
{
    *vec++ -= *sub++;
    *vec++ -= *sub++;
    *vec -= *sub;
}

/* @hack spelling: the parts through walking pointers, where three assignments
 * would do */
static void vector_add_to(Float *add, Float *vec)
{
    *vec++ += *add++;
    *vec++ += *add++;
    *vec += *add;
}

/* @hack spelling: car_debris_carry read in a helper of its own, where
 * vector_scale_by would do */
#pragma inline(vector_scale_debris_carry)
static void vector_scale_debris_carry(Float *vec)
{
    *vec++ *= car_debris_carry;
    *vec++ *= car_debris_carry;
    *vec *= car_debris_carry;
}

/* @hack spelling: out and delta stepped on lines of their own after the first
 * part, and out after the second, where points_sum would do */
#pragma inline(points_sum_out_apart)
static void points_sum_out_apart(Float *base, Float *delta, Float *out)
{
    *out = *delta + *base++;
    out++;
    delta++;
    *out = *delta++ + *base++;
    out++;
    *out = *delta + *base;
}

#pragma inline(vector_add_to_apart, vector_subtract_swapped)

/* @hack spelling: each part as *vec = *vec + *add, not +=, with vec stepped
 * on lines of its own, where vector_add_to would do */
static void vector_add_to_apart(Float *add, Float *vec)
{
    *vec = *vec + *add++;
    vec++;
    *vec = *vec + *add++;
    vec++;
    *vec = *vec + *add;
}

/* @hack spelling: sub taken first, where vector_subtract would do */
static void vector_subtract_swapped(Float *sub, Float *vec)
{
    *vec++ -= *sub++;
    *vec++ -= *sub++;
    *vec -= *sub;
}

#pragma inline(points_difference_to_from_apart, points_difference_out_apart)

/* @hack spelling: to and from stepped on lines of their own after the first
 * part, and from after the second, where points_difference would do */
static void points_difference_to_from_apart(Float *from, Float *to, Float *out)
{
    *out++ = *to - *from;
    to++;
    from++;
    *out++ = *to++ - *from;
    from++;
    *out = *to - *from;
}

/* @hack spelling: out stepped on a line of its own after the first part,
 * where points_difference would do */
static void points_difference_out_apart(Float *from, Float *to, Float *out)
{
    *out = *to++ - *from++;
    out++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}

/* the truck's speed along its frame's z, in its task's extra */
/* @hack type: the speed read as a Float through extra's address, where
 * (task)->extra.f would do */
#define TRUCK_SPEED(task) (*(Float *)&(task)->extra)
/* the two matrices at the truck's task->force: its frame, then the inverse */
#define TRUCK_FRAME 0
#define TRUCK_INVERSE 1
/* @hack spelling: car_truck_shape read through its first element's address,
 * where car_truck_shape itself would do */
#define TRUCK_SHAPE (&car_truck_shape[0])

/* each step a car eases its speed SPEED_EASE of the way toward scl.x, or
 * toward -scl.x backward. once its body bobs SLOW_HEIGHT or more above its pos,
 * it keeps SLOW_KEEP of its speed instead. it slows until its body comes back
 * under RESUME_HEIGHT. it doesn't slow until its tick passes SETTLE_TICKS */
#define SPEED_EASE 0.012F
#define SLOW_KEEP 0.985F
#define SLOW_HEIGHT 8.0F
#define RESUME_HEIGHT 1.0F
#define SETTLE_TICKS 2
/* an unseen car keeps UNSEEN_KEEP of its bob's speed and height each step */
#define UNSEEN_KEEP 0.98F
/* a car tilts about z by TILT_TURN times its step's turn about y */
#define TILT_TURN 2.0F
/* the wobble's time is the wheels' turn, the work's timer, over
 * WOBBLE_TIME */
#define WOBBLE_TIME 60
#define BOB_BOUNCE_HEIGHT (-2.0F)
/* while a player lies within ENGINE_RANGE of a driving car, each step plays
 * its engine sound again. a play lasts ENGINE_UPDATES + 1 sound updates */
#define ENGINE_SOUND 9
#define ENGINE_PRIORITY 1
#define ENGINE_UPDATES 10
#define ENGINE_RANGE 300.0F
/* a car that starts its path over starts RESTART_FRACTION of the way along its
 * first segment */
#define RESTART_FRACTION 0.01F
/* a car's work's tick counts its steps, up to TICK_MAX, and starts over with
 * its path. no touch crashes the car until its tick passes CRASH_TICK */
#define TICK_MAX 125
#define CRASH_TICK 20
/* the box's half height is BOX_RAISE more than the kind's */
#define BOX_RAISE 10.0F
/* a car pushes only the players in slots 0 and 1 */
#define CAR_PLAYERS 2
/* the speed in the motion record of the player in slot */
#define PLAYER_SPD(slot)                                                       \
    (((struct player_motion *)player_tasks[slot]->motion.p)->spd)
/* the truck's crash throws debris with a chance of DEBRIS_CHANCE. each piece
 * is a random one of the kinds road_objects_lookup reads. the first starts to
 * fade DEBRIS_DELAY steps on, and each after it DEBRIS_DELAY_STEP later. a
 * piece's size, as crash_piece_create takes it, is DEBRIS_SIZE_X,
 * DEBRIS_SIZE_Y and DEBRIS_SIZE_Z */
#define DEBRIS_CHANCE 0.5F
#define DEBRIS_DELAY 300
#define DEBRIS_DELAY_STEP 30
#define DEBRIS_SIZE_X 10.0F
#define DEBRIS_SIZE_Y 15.0F
#define DEBRIS_SIZE_Z 10.0F
#define WRECK_TOWARD_CHANCE 0.3F
#define WRECK_CARRY_CHANCE 0.3F
#define WRECK_LIFT_CHANCE 0.2F
/* a wreck of the truck's crash lives up to TRUCK_WRECK_LIFE steps, then
 * fades */
#define TRUCK_WRECK_LIFE 100
#define WRECK_KICK_LEAST 0.3F
/* the wreck gains HITTER_SPEED_PART of the speed of the task in the car's
 * first hit. it also gains HITTER_LIFT along y. where that task is null or has
 * no motion, the wreck gains CRASH_LIFT along y alone. a CRASH_NUDGE keeps
 * NUDGE_LIFT_PART of the wreck's speed along y */
#define HITTER_SPEED_PART 0.9F
#define HITTER_LIFT 0.38F
#define CRASH_LIFT 0.63F
#define NUDGE_LIFT_PART 0.7F
/* a crash a player makes scores CRASH_SCORE points */
#define CRASH_SCORE 20
#define CRASH_PITCH_PART 0.5F
/* a player's crash makes a flash FLASH_MARGIN beyond the radius of the
 * player's first shape, toward the car. the flash throws CRASH_SPARKS sparks a
 * step */
#define FLASH_MARGIN 5.0F
#define CRASH_SPARKS 0
/* a player's crash leaves the player PLAYER_SPEED_PART of its speed, and
 * PLAYER_HOP more along y */
#define PLAYER_SPEED_PART 0.8F
#define PLAYER_HOP 0.6F
/* a crash plays CRASH_SOUND or CRASH_SOUND_2, with an even chance of each. a
 * wreck's landing plays LAND_SOUND */
#define CRASH_SOUND 3
#define CRASH_SOUND_2 4
#define LAND_SOUND 5
#define SOUND_PRIORITY 0
#define EVEN_CHANCE 0.5F
#define SHAKE_RANGE 300.0F
#define SHAKE_POWER 0.3F
/* a random Float from -0.5 up to but not including 0.5 */
#define RANDOM_HALF() ((rand() * RAND_SCALE) - 0.5F)

/* a car on a path drives it, unless its line's mode is MODE_WAIT. at the
 * path's end a CAR_SHUTTLES car turns back, and any other starts over. a car in
 * view pushes the players out of its box. a touch may crash it, and the truck
 * crashes it whatever its tick */
static void car_step(struct task *task)
{
    struct task_work *work = task->work;
    struct car_kind *kind = KIND(work);
    NJS_POINT3 pos;
    struct angles ang;
    Bool beyond;
    int time;
    int i;
    int force;
    int player;
    Bool inside;
    Bool truck_hit;
    int count;
    int life;
    int pick;
    NJS_TEXLIST *debris_texlist;
    NJS_CNK_OBJECT *debris_object;
    NJS_DIRECT_HEAD *debris_build;
    NJS_MATRIX *matrices;
    Float height;
    Float speed;
    Float ax;
    Float ay;
    Float az;
    /* @hack spelling: box, a pointer to impact for the box's first reads,
     * where impact itself would do */
    NJS_VECTOR *box;
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    Angle pitch;
    Sint16 *low_half;

    if (!(work->submode & CAR_IN_LINE) && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which draws the path and
     * picks the path again. the object keeps its code from the loop on */
    if (0) {
        /* @hack redundant: the path display reserves an unused buffer */
        char text[256];

        njPushMatrixEx();
        /* @hack redundant: a second push and its pop */
        njPushMatrixEx();
        njPopMatrixEx();
        to = (NJS_POINT3 *)CAR(task)->path->points + 1;
        from = CAR(task)->path->points;
        /* @hack spelling: count decremented on a line of its own, where
         * count's assignment less 1 would do */
        count = CAR(task)->path->count;
        count--;
        /* @hack redundant: a store to box that never runs */
        if (0) {
            box = to;
        }
        /* @hack redundant: the same store again, in a block of its own */
        if (0) {
            box = to;
        }
        /* @hack redundant: the same store again, in a block of its own */
        if (0) {
            box = to;
        }
        /* @hack redundant: the same store again, in a block of its own */
        if (0) {
            box = to;
        }
        /* @hack redundant: an empty test of kind */
        if (kind) {
        }
        /* @hack redundant: an unreachable test of the line drawer */
        if (0) {
            if (&line_draw_points != NULL) {
            }
        }
        for (i = 0; i < count; i++) {
            line_draw_points(from, to);
            from++;
            to++;
        }
        /* @hack redundant: a test of box that never runs */
        if (0) {
            if (box) {
            }
        }
        njPopMatrixEx();
        CAR(task)->path =
            car_paths[(Uint8)(work->scl.z + TRUNCATE_SLACK) % CAR_PATHS];
        path_center(CAR(task)->path, &CAR(task)->center);
    }
    {
        NJS_VECTOR travel;
        NJS_VECTOR kick;
        NJS_VECTOR side;
        NJS_VECTOR impact;
        NJS_VECTOR point;
        NJS_VECTOR delta;
        NJS_VECTOR size;
        NJS_POINT3 debris_pos;
        struct angles debris_ang;

        /* @hack redundant: a store to beyond, which never runs */
        if (0) {
            beyond = work->mode;
        }
        /* @hack redundant: reads of beyond, which never run */
        if (0) {
            /* NOLINTBEGIN(misc-redundant-expression) */
            if (beyond || beyond || beyond || beyond || beyond || beyond ||
                beyond) {
            }
            /* NOLINTEND(misc-redundant-expression) */
        }
        if (!(work->submode & CAR_PARKED) &&
            !(work->submode & CAR_IN_LINE && task->parent->work->mode)) {
            /* @hack redundant: a store to life, which never runs */
            if (0) {
                life = work->mode;
            }
            /* @hack redundant: reads of life, which never run */
            if (0) {
                if (life || (life & 1)) {
                }
            }
            if (CAR(task)->height < RESUME_HEIGHT ||
                (!SLOWING(task) && CAR(task)->height < SLOW_HEIGHT) ||
                work->tick <= SETTLE_TICKS) {
                CAR(task)->speed =
                    (((work->mode == MODE_BACKWARD ? -work->scl.x
                                                   : work->scl.x) -
                      CAR(task)->speed) *
                     SPEED_EASE) +
                    CAR(task)->speed;
                SLOWING(task) = FALSE;
            } else {
                CAR(task)->speed *= SLOW_KEEP;
                SLOWING(task) = TRUE;
            }
            if (work->submode & CAR_UNSEEN) {
                beyond = car_place_on_path(
                    kind, CAR(task)->path, &CAR(task)->segment,
                    &CAR(task)->fraction, CAR(task)->speed, &pos, &ang);
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &CAR(task)->path_pos);
                njRotateY(NULL, CAR(task)->path_ang);
                njTranslate(NULL, -CAR(task)->center.x, -CAR(task)->center.y,
                            -CAR(task)->center.z);
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrixEx();
                ang.y += CAR(task)->path_ang;
                CAR(task)->last_pos = work->pos;
                work->pos = pos;
                work->ang = ang;
                CAR(task)->lift = 0.0F;
                CAR(task)->spd.x *= UNSEEN_KEEP;
                CAR(task)->spd.y *= UNSEEN_KEEP;
                CAR(task)->spd.z *= UNSEEN_KEEP;
                CAR(task)->height *= UNSEEN_KEEP;
            } else {
                beyond = car_place_on_path(
                    kind, CAR(task)->path, &CAR(task)->segment,
                    &CAR(task)->fraction, CAR(task)->speed, &pos, &ang);
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &CAR(task)->path_pos);
                njRotateY(NULL, CAR(task)->path_ang);
                njTranslate(NULL, -CAR(task)->center.x, -CAR(task)->center.y,
                            -CAR(task)->center.z);
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrixEx();
                ang.y += CAR(task)->path_ang;
                if (work->tick > SETTLE_TICKS) {
                    if (CAR(task)->height > RESUME_HEIGHT) {
                        car_ease_slow(&ang, &work->ang);
                    } else {
                        car_ease_fast(&ang, &work->ang);
                    }
                    ang.z = (ang.y - work->ang.y) * TILT_TURN;
                } else {
                    ang.z = 0;
                }
                points_difference_to_from_apart(&work->pos.x, &pos.x,
                                                &travel.x);
                speed = njScalor(&travel);
                work->timer +=
                    (Angle)(speed * kind->wheel_turn * ANGLE_PER_DEGREE);
                travel = kind->box;
                travel.y = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateZ(NULL, work->ang.z);
                njCalcVector(NULL, &travel, &travel);
                height = njAbs(travel.y) - car_lift_slack;
                if (height < 0.0F) {
                    height = 0.0F;
                }
                CAR(task)->lift = height;
                njPopMatrixEx();
                time = work->timer / WOBBLE_TIME;
                CAR(task)->wobble.x =
                    wave_sum(car_wobble_x_waves, WOBBLE_WAVES, time) *
                    ANGLE_PER_DEGREE;
                CAR(task)->wobble.y =
                    wave_sum(car_wobble_y_waves, WOBBLE_WAVES, time) *
                    ANGLE_PER_DEGREE;
                CAR(task)->wobble.z =
                    wave_sum(car_wobble_z_waves, WOBBLE_WAVES, time) *
                    ANGLE_PER_DEGREE;
                points_difference_out_apart(&work->pos.x, &pos.x, &travel.x);
                points_difference(&CAR(task)->last_pos.x, &work->pos.x,
                                  &kick.x);
                height = travel.y - kick.y;
                if (height < 0.0F && work->tick > SETTLE_TICKS) {
                    CAR(task)->spd.y += -height;
                }
                CAR(task)->spd.y += -car_bob_gravity;
                if (CAR(task)->height < 0.0F) {
                    CAR(task)->spd.y += -(CAR(task)->height * car_bob_spring);
                }
                if (CAR(task)->height < BOB_BOUNCE_HEIGHT &&
                    CAR(task)->spd.y < 0.0F) {
                    CAR(task)->spd.y = -(CAR(task)->spd.y * car_bob_bounce);
                }
                CAR(task)->spd.y *= car_bob_damping;
                CAR(task)->height += CAR(task)->spd.y;
                CAR(task)->last_pos = work->pos;
                work->pos = pos;
                work->ang = ang;
            }
            if (player_near(&work->pos, ENGINE_RANGE)) {
                sound_play_at_for(ENGINE_SOUND, work, ENGINE_PRIORITY,
                                  car_engine_volume, ENGINE_UPDATES,
                                  &work->pos);
            }
            if (beyond) {
                work->tick = 0;
                if (work->submode & CAR_IN_LINE) {
                    work->submode &= ~CAR_HIDDEN;
                }
                if (kind->flags & CAR_SHUTTLES) {
                    if (work->mode != MODE_BACKWARD) {
                        work->mode = MODE_BACKWARD;
                    }
                } else {
                    if (work->submode & CAR_RANDOM) {
                        work->scl.y = car_random_kind();
                    }
                    if (work->submode & CAR_RANDOM_NO_PUSH) {
                        work->scl.y = car_random_kind() + KIND_RANDOM_COUNT;
                    }
                    if (work->submode & CAR_PARKED &&
                        (int)(work->scl.y + TRUNCATE_SLACK) != KIND_CABLE_CAR &&
                        (int)(work->scl.y + TRUNCATE_SLACK) !=
                            KIND_CABLE_CAR_SHUTTLE) {
                        work->scl.y += PARKED_KIND_OFFSET;
                    }
                    CAR(task)->segment = 0;
                    CAR(task)->fraction = RESTART_FRACTION;
                    car_place(task);
                }
            }
            if (CAR(task)->segment <= 0 && work->mode == MODE_BACKWARD) {
                work->mode = MODE_FORWARD;
            }
        }
        if (work->tick < TICK_MAX) {
            ++work->tick;
        }
        if (!(work->submode & (CAR_HIDDEN | CAR_UNSEEN))) {
            if (work->collision) {
                for (i = 0; i < work->collision->count; i++) {
                    work->collision->shapes[i].flags &= ~SHAPE_OFF;
                }
            }
            inside = FALSE;
            truck_hit = FALSE;
            box = &impact;
            *box = kind->box;
            box->y = box->y + BOX_RAISE;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateZXY(NULL, work->ang.x, work->ang.y, work->ang.z);
            /* @hack spelling: the kind found again, where kind would do */
            car_place_shapes(task, FALSE, KIND(work));
            njInvertMatrix(NULL);
            for (i = 0; i < CAR_PLAYERS; i++) {
                if (player_work[i]) {
                    points_sum(&PLAYER_SPD(i).x,
                               &player_work[i]->collision->shapes[0].center.x,
                               &point.x);
                    njCalcPoint(NULL, &point, &point);
                    ax = njAbs(point.x);
                    ay = njAbs(point.y);
                    az = njAbs(point.z);
                    if (box->x > ax && box->y > ay && box->z > az) {
                        if ((!(work->submode & CAR_PARKED) && point.z > 0.0F) ||
                            impact.x - ax < impact.z - az) {
                            if (!(work->submode & CAR_NO_PUSH_OUT)) {
                                point.x = point.x > 0.0F ? impact.x : -impact.x;
                                njPushMatrixEx();
                                njInvertMatrix(NULL);
                                njCalcPoint(NULL, &point, &delta);
                                njPopMatrixEx();
                                vector_subtract_swapped(
                                    &player_work[i]
                                         ->collision->shapes[0]
                                         .center.x,
                                    &delta.x);
                                vector_subtract(&delta.x, &PLAYER_SPD(i).x);
                                vector_add_to_apart(&player_work[i]->pos.x,
                                                    &delta.x);
                                points_difference(&player_work[i]->pos.x,
                                                  &delta.x, &size.x);
                                if (size.y < 0.0F) {
                                    size.y = 0.0F;
                                }
                                player_records[i]->push = size;
                            }
                        } else {
                            if (!(work->submode & CAR_NO_PUSH_OUT)) {
                                point.z = point.z > 0.0F ? impact.z : -impact.z;
                                njPushMatrixEx();
                                njInvertMatrix(NULL);
                                njCalcPoint(NULL, &point, &delta);
                                njPopMatrixEx();
                                vector_subtract_swapped(
                                    &player_work[i]
                                         ->collision->shapes[0]
                                         .center.x,
                                    &delta.x);
                                vector_subtract(&delta.x, &PLAYER_SPD(i).x);
                                vector_add_to(&player_work[i]->pos.x, &delta.x);
                                points_difference(&player_work[i]->pos.x,
                                                  &delta.x, &size.x);
                                if (size.y < 0.0F) {
                                    size.y = 0.0F;
                                }
                                player_records[i]->push = size;
                            }
                        }
                        inside = TRUE;
                    }
                }
            }
            njPopMatrixEx();
            if (car_truck && car_truck->child && car_truck->force.p) {
                matrices = car_truck->force.p;
                travel.x = 0.0F;
                travel.z = TRUCK_SPEED(car_truck);
                travel.y = 0.0F;
                njPushMatrixEx();
                njSetMatrix(NULL, &matrices[TRUCK_FRAME]);
                njCalcVector(NULL, &travel, &travel);
                njSetMatrix(NULL, &matrices[TRUCK_INVERSE]);
                njCalcPoint(NULL, &work->pos, &impact);
                if (njAbs(impact.x) < TRUCK_SHAPE->radius &&
                    njAbs(impact.y) < TRUCK_SHAPE->size_y &&
                    njAbs(impact.z) < TRUCK_SHAPE->size_z) {
                    truck_hit = TRUE;
                    if (TRUCK_SHAPE->size_y - njAbs(impact.y) >
                            TRUCK_SHAPE->size_z - njAbs(impact.z) ||
                        impact.y < 0.0F) {
                        impact.z = TRUCK_SHAPE->size_z;
                    } else {
                        impact.y = TRUCK_SHAPE->size_y;
                    }
                }
                njSetMatrix(NULL, &matrices[TRUCK_FRAME]);
                if (truck_hit) {
                    njCalcPoint(NULL, &impact, &work->pos);
                }
                /* @bug logic: loses the random kick it takes here. kick.x takes
                 * 1 (0x8c502aae) over it (0x8c502a34, 0x8c502a98), so each
                 * wreck the truck throws gains 1 along the truck's x axis */
                if (impact.x >= 0.0F) {
                    kick.x = -(rand() * car_wreck_kick * RAND_SCALE) -
                             WRECK_KICK_LEAST;
                } else {
                    kick.x = WRECK_KICK_LEAST +
                             (rand() * car_wreck_kick * RAND_SCALE);
                }
                /* @hack spelling: each pair of zeros stored by one chained
                 * assignment, where a statement for each would do */
                kick.z = kick.y = 0.0F;
                /* @hack spelling: kick.x set between the two pairs of zeros,
                 * where setting it ahead of both would do */
                kick.x = 1.0F;
                kick.z = kick.y = 0.0F;
                njCalcVector(NULL, &kick, &kick);
                /* no path sets side before this turn. the turned side reaches
                 * only debris_pos below, which nothing reads */
                njCalcVector(NULL, &side, &side);
                njPopMatrixEx();
            }
            if ((work->tick > CRASH_TICK &&
                 work->collision->flags & COLLISION_TOUCHED &&
                 (inside || !collision_next_player(task))) ||
                truck_hit) {
                if (truck_hit) {
                    player = -1;
                    force = CRASH_FLAME_HIT;
                    if (rand() * RAND_SCALE < DEBRIS_CHANCE) {
                        count = road_objects_lookup_count();
                        for (i = 0; i < car_debris_count; i++) {
                            pick = (int)(rand() * (RANDOM_PICKS * RAND_SCALE)) %
                                   count;
                            if (road_objects_lookup(pick, &debris_texlist,
                                                    &debris_object,
                                                    &debris_build)) {
                                point = travel;
                                vector_scale_debris_carry(&point.x);
                                points_difference_to_from_apart(
                                    &work->pos.x, &camera_views[0]->pos.x,
                                    &delta.x);
                                vector_scale_by(
                                    &delta.x, rand() *
                                                  (car_debris_toward +
                                                   (TRUCK_SPEED(car_truck) *
                                                    car_debris_toward_truck)) *
                                                  RAND_SCALE /
                                                  njScalor(&delta));
                                vector_add_to(&delta.x, &point.x);
                                delta = side;
                                vector_scale_by(&delta.x,
                                                RANDOM_HALF() *
                                                    car_debris_spread);
                                /* @hack redundant: debris_pos, which the sum
                                 * sets and nothing reads */
                                points_sum_out_apart(&work->pos.x, &delta.x,
                                                     &debris_pos.x);
                                point.x += RANDOM_HALF();
                                point.y += car_debris_lift +
                                           (rand() * RAND_SCALE) +
                                           (car_debris_lift_random * rand() *
                                            RAND_SCALE) -
                                           0.5F;
                                point.z += RANDOM_HALF();
                                debris_ang.x = 0;
                                debris_ang.y = work->ang.y;
                                debris_ang.z = 0;
                                size.x = DEBRIS_SIZE_X;
                                size.y = DEBRIS_SIZE_Y;
                                size.z = DEBRIS_SIZE_Z;
                                crash_piece_create(
                                    debris_texlist, debris_object, debris_build,
                                    &size, &work->pos, &debris_ang, &point,
                                    DEBRIS_DELAY + (i * DEBRIS_DELAY_STEP));
                            }
                        }
                    }
                } else {
                    player = car_find_hit(task, &force);
                    if (force < 0 &&
                        (player < 0 || work->submode & CAR_NO_PUSH_OUT)) {
                        force = CRASH_FLAME_HIT;
                    }
                }
                if (force >= 0) {
                    Float value;

                    pitch = work->ang.x;
                    SCALE_ANGLE(pitch, CRASH_PITCH_PART);
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y);
                    njRotateX(NULL, pitch);
                    njCalcVector(NULL, &car_crash_spd, &impact);
                    njPopMatrixEx();
                    if (truck_hit) {
                        if (rand() * RAND_SCALE < WRECK_TOWARD_CHANCE) {
                            points_difference(&work->pos.x,
                                              &camera_views[0]->pos.x,
                                              &point.x);
                            speed = rand() * RAND_SCALE;
                            vector_scale_by(&point.x,
                                            speed *
                                                (car_wreck_toward +
                                                 (car_wreck_toward_truck *
                                                  TRUCK_SPEED(car_truck))) /
                                                njScalor(&point));
                            vector_add_to(&point.x, &impact.x);
                        }
                        if (rand() * RAND_SCALE < WRECK_CARRY_CHANCE) {
                            vector_scale_by(
                                &travel.x,
                                car_wreck_carry_random +
                                    (rand() * car_wreck_carry_random_add *
                                     RAND_SCALE));
                        } else {
                            vector_scale_by_pointer(&travel.x,
                                                    &car_wreck_carry);
                        }
                        vector_add_to(&travel.x, &impact.x);
                        if (rand() * RAND_SCALE < WRECK_LIFT_CHANCE) {
                            impact.y += car_wreck_lift + car_wreck_lift_random;
                        } else {
                            impact.y +=
                                car_wreck_lift +
                                (rand() * car_wreck_lift_random * RAND_SCALE);
                        }
                        impact.x += RANDOM_HALF() * car_wreck_jitter;
                        impact.y += RANDOM_HALF() * car_wreck_jitter;
                        impact.z += RANDOM_HALF() * car_wreck_jitter;
                        vector_add_to(&kick.x, &impact.x);
                    } else {
                        if (work->collision->hits[0].task &&
                            work->collision->hits[0].task->motion.p) {
                            point =
                                TASK_MOTION_OF(work->collision->hits[0].task)
                                    ->spd;
                            vector_scale_by_swapped(HITTER_SPEED_PART,
                                                    &point.x);
                            vector_add_to(&point.x, &impact.x);
                            impact.y += HITTER_LIFT;
                        } else {
                            impact.y += CRASH_LIFT;
                        }
                        if (force == CRASH_NUDGE) {
                            impact.y *= NUDGE_LIFT_PART;
                        }
                    }
                    if (player >= 0) {
                        score_add(CRASH_SCORE);
                        points_difference_out_apart(
                            &player_work[player]->collision->shapes[0].center.x,
                            &work->pos.x, &point.x);
                        vector_scale_by(
                            &point.x,
                            (player_work[player]->collision->shapes[0].radius +
                             FLASH_MARGIN) /
                                njScalor(&point));
                        points_sum(
                            &player_work[player]->collision->shapes[0].center.x,
                            &point.x, &delta.x);
                        flash_manager_create_sparks(delta.x, delta.y, delta.z,
                                                    car_crash_flash_scale,
                                                    CRASH_SPARKS);
                        player_set_speed(
                            player, PLAYER_SPD(player).x * PLAYER_SPEED_PART,
                            PLAYER_HOP +
                                (PLAYER_SPD(player).y * PLAYER_SPEED_PART),
                            PLAYER_SPD(player).z * PLAYER_SPEED_PART);
                    }
                    car_wreck_create(kind, &work->pos, &work->ang, &impact,
                                     &work->pos, &work->ang, force,
                                     truck_hit ? TRUCK_WRECK_LIFE : 0);
                    /* @hack type: the key passed as 0, where NULL would do */
                    sound_play_at(
                        rand() * RAND_SCALE > EVEN_CHANCE ? CRASH_SOUND
                                                          : CRASH_SOUND_2,
                        0, SOUND_PRIORITY, car_crash_volume, &work->pos);
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
}

/* draw a wreck_object's wheels, object and its sibling, each turned about x
 * by turn. the sibling must not be null */
static void car_draw_wheels(NJS_CNK_OBJECT *object, Angle turn)
{
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateX(NULL, turn);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    object = object->sibling;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateX(NULL, turn);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

/* while cars_come_back is set, a car blinks in. it draws on none of its first
 * BLINK_HIDDEN steps, and then on every other step up to BLINK_STEPS */
#define BLINK_HIDDEN 10
#define BLINK_STEPS 20
/* a car draws only while part of a ball about its pos lies ahead of the
 * camera. the ball's radius is its wreck_object's plus VIEW_MARGIN. the pos
 * must also lie no more than VIEW_FAR ahead */
#define VIEW_MARGIN 30.0F
#define VIEW_FAR 700.0F
#define NEAR_COLOR 0x80ffffff
/* a car draws its body height + lift + BODY_RISE above its pos */
#define BODY_RISE 1.0F

/* draw the car over its shadow. a car on a path nearer than its kind's
 * near_range draws wreck_object. the model's wheels turn with the work's
 * timer. nearer than the model's radius, the car draws it see-through, with
 * NEAR_COLOR's alpha. a parked car always draws its kind's object */
static void car_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct car_kind *kind = KIND(work);
    NJS_CNK_OBJECT *object;
    Float radius;
    Float depth;
    Float height;
    /* @hack redundant: unused, a local that only sizes the frame */
    char unused[12];
    struct angles ang;

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
    /* @hack spelling: radius assigned inside the call's argument, where a
     * statement of its own would do */
    if (!sphere_in_front(&work->pos,
                         (radius = kind->wreck_object->model->r) + VIEW_MARGIN,
                         &depth) ||
        depth > VIEW_FAR) {
        work->submode |= CAR_UNSEEN;
        return;
    }
    work->submode &= ~CAR_UNSEEN;
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODE(NOPUSHOUT)";
        (void)"<- MODE(PUSHOUT)";
        (void)"<- CAR TYPE";
        (void)"<- CARSPEED";
        (void)"<- PATH TYPE";
    }
    njPushMatrixEx();
    height = CAR(task)->height;
    if (depth < kind->near_range && kind->object &&
        !(work->submode & CAR_PARKED)) {
        njSetTexture(kind->wreck_texlist);
        object = kind->wreck_object;
        if (object) {
            ang.x = work->ang.x + CAR(task)->wobble.x;
            ang.y = work->ang.y + CAR(task)->wobble.y;
            ang.z = work->ang.z + CAR(task)->wobble.z;
            njTranslate(NULL, work->pos.x,
                        work->pos.y + height + CAR(task)->lift + BODY_RISE,
                        work->pos.z);
            car_draw_shadow(kind->shadow_x, kind->shadow_z, &ang);
            njRotateEx(&ang.x, ZXY_TURN);
            if (depth < radius) {
                draw_control_save();
                draw_attr_save();
                draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
                njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
                model_draw_tinted(object->model, NEAR_COLOR);
                draw_attr_restore();
                draw_control_restore();
            } else {
                njCnkEasyDrawModel(object->model);
            }
            object = object->child;
            if (object) {
                car_draw_wheels(object, work->timer);
            }
        }
    } else {
        ang.x = work->ang.x + CAR(task)->wobble.x;
        ang.y = work->ang.y + CAR(task)->wobble.y;
        ang.z = work->ang.z + CAR(task)->wobble.z;
        njTranslate(NULL, work->pos.x,
                    work->pos.y + height + CAR(task)->lift + BODY_RISE,
                    work->pos.z);
        car_draw_shadow(kind->shadow_x, kind->shadow_z, &ang);
        njRotateEx(&ang.x, ZXY_TURN);
        if (depth < radius && !(work->submode & CAR_PARKED)) {
            njSetTexture(kind->wreck_texlist);
            object = kind->wreck_object;
            draw_control_save();
            draw_attr_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            model_draw_tinted(object->model, NEAR_COLOR);
            draw_attr_restore();
            draw_control_restore();
            object = object->child;
            if (object) {
                car_draw_wheels(object, work->timer);
            }
        } else {
            njSetTexture(kind->texlist);
            if (kind->build) {
                njDirectDrawObject(kind->build);
            } else {
                njCnkEasyDrawObject(kind->object);
            }
        }
    }
    njPopMatrixEx();
}

/* the sum of the lengths of path's segments. the path's points must be
 * NJS_POINT3s */
static Float path_length(struct path *path)
{
    NJS_POINT3 *points = path->points;
    Float length = 0.0F;
    int i;

    for (i = 0; i < path->count - 1; i++) {
        /* @hack type: i + 1 converted to a Sint32, where i + 1 would do */
        length += njDistanceP2P(&points[i], &points[(Sint32)(i + 1)]);
    }
    return length;
}

/* traffic_init divides its path's length into as many spaces as it has cars,
 * plus LINE_SPARE_SPACES. it places each car a whole count of spaces from the
 * path's start. each car then moves ahead up to LINE_JITTER of a space, at
 * random */
#define LINE_SPARE_SPACES 0.8F
#define LINE_JITTER 0.3F

static void traffic_destroy(struct task *task);
static void traffic_draw(struct task *task);
static void traffic_step(struct task *task);
static void traffic_wait_destroy(struct task *task);
static void traffic_wait_draw(struct task *task);
static void traffic_wait_step(struct task *task);

void traffic_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *car;
    NJS_POINT3 pos;
    struct angles ang;
    int count;
    Float space;
    Float place;
    Float last;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct car));
    if (!task->motion.p) {
        return;
    }
    task->draw = traffic_draw;
    task->step = traffic_step;
    task->destructor = traffic_destroy;
    CAR(task)->path =
        car_paths[(Uint8)(work->scl.z + TRUNCATE_SLACK) % CAR_PATHS];
    LINE_LENGTH(task) = path_length(CAR(task)->path);
    path_center(CAR(task)->path, &CAR(task)->center);
    CAR(task)->segment = 0;
    CAR(task)->fraction = 0.0F;
    /* @hack redundant: the state read again, where the first read would do */
    if (task->placement && placement_get_state(task)) {
        count = placement_get_state(task);
    } else {
        count = work->scl.y + TRUNCATE_SLACK;
    }
    space = LINE_LENGTH(task) / (count + LINE_SPARE_SPACES);
    last = 0.0F;
    while (count--) {
        place = (count * space) + (rand() * space * (LINE_JITTER * RAND_SCALE));
        car_place_on_path(&car_kinds[(Uint8)work->ang.x % CAR_KINDS],
                          CAR(task)->path, &CAR(task)->segment,
                          &CAR(task)->fraction, place - last, &pos, &ang);
        car = task_create_child(TASK_WORK, car_setup, task);
        if (car) {
            car->work->scl = work->scl;
            car->work->scl.y = (Uint8)work->ang.x % CAR_KINDS;
            car->work->timer = CAR(task)->segment;
            LINE_FRACTION(car) = CAR(task)->fraction;
            car->work->submode |= CAR_IN_LINE;
        }
        last = place;
    }
}

/* keep in the placement's state how many cars the line still has */
static void traffic_destroy(struct task *task)
{
    struct task *car;
    struct task *first;
    int count;

    if (task->placement) {
        car = task->child;
        count = 0;
        first = car;
        while (car) {
            car = car->next;
            ++count;
            if (car == first) {
                break;
            }
        }
        placement_set_state(task, count);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void traffic_draw(struct task *task)
{
}

/* the sphere traffic_step's dead block draws at the line's pos. its radius
 * swings MARK_PULSE either way about MARK_SIZE, and the swing's phase turns
 * MARK_RATE Angle units a step */
#define MARK_SIZE 40.0F
#define MARK_PULSE 20.0F
#define MARK_RATE 300.0F

static void traffic_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    int i;
    int count;
    struct path *path;
    NJS_VECTOR scale;
    /* @hack redundant: unused, a local that only sizes the frame */
    char unused[252];
    Float pulse;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which draws the path's
     * segments and a mark. the object keeps the code from its loop on */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateEx(&work->ang.x, ZXY_TURN);
        path = CAR(task)->path;
        to = (NJS_POINT3 *)path->points + 1;
        from = path->points;
        count = path->count - 1;
        for (i = 0; i < count; ++i) {
            line_draw_points(from, to);
            ++from;
            ++to;
        }
        njPopMatrixEx();
        (void)"CEN:(%5.3f,%5.3f,%5.3f)";
        (void)"<- CAR NUM";
        pulse = MARK_SIZE +
                (njSin((Angle)(game_state.steps * MARK_RATE)) * MARK_PULSE);
        scale.x = pulse;
        scale.y = pulse;
        scale.z = pulse;
        debug_draw_model(&work->pos, NULL, &scale, DEBUG_MODEL_SPHERE);
    }
}

void traffic_wait_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *car;
    NJS_POINT3 pos;
    struct angles ang;
    int count;
    Float space;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct car));
    if (!task->motion.p) {
        return;
    }
    task->draw = traffic_wait_draw;
    task->step = traffic_wait_step;
    task->destructor = traffic_wait_destroy;
    CAR(task)->path =
        car_paths[(Uint8)(work->scl.z + TRUNCATE_SLACK) % CAR_PATHS];
    LINE_LENGTH(task) = path_length(CAR(task)->path);
    path_center(CAR(task)->path, &CAR(task)->center);
    CAR(task)->segment = 0;
    CAR(task)->fraction = 0.0F;
    work->mode = MODE_WAIT;
    /* @hack redundant: the state read again, where the first read would do */
    if (task->placement && placement_get_state(task)) {
        count = placement_get_state(task);
    } else {
        count = work->scl.y + TRUNCATE_SLACK;
    }
    space = LINE_LENGTH(task) / count;
    while (count--) {
        car_place_on_path(&car_kinds[(Uint8)work->ang.x % CAR_KINDS],
                          CAR(task)->path, &CAR(task)->segment,
                          &CAR(task)->fraction, space, &pos, &ang);
        car = task_create_child(TASK_WORK, car_setup, task);
        if (car) {
            car->work->scl = work->scl;
            car->work->scl.y = (Uint8)work->ang.x;
            car->work->timer = CAR(task)->segment;
            LINE_FRACTION(car) = CAR(task)->fraction;
            car->work->submode |= CAR_IN_LINE | CAR_PATH_IN_PLACE;
        }
    }
}

/* traffic_destroy's twin */
static void traffic_wait_destroy(struct task *task)
{
    struct task *car;
    struct task *first;
    int count;

    if (task->placement) {
        car = task->child;
        count = 0;
        first = car;
        while (car) {
            car = car->next;
            ++count;
            if (car == first) {
                break;
            }
        }
        placement_set_state(task, count);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void traffic_wait_draw(struct task *task)
{
}

/* the line's mode is MODE_GO while player_near finds a player near its pos,
 * and MODE_WAIT otherwise. the low 16 bits of ang.z give the reach */
static void traffic_wait_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    int i;
    int count;
    struct path *path;
    NJS_VECTOR scale;
    /* @hack redundant: unused, a local that only sizes the frame */
    char unused[252];

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which draws the path's
     * segments and a mark. the object keeps the code from its loop on */
    if (0) {
        /* @hack spelling: the path and from set in one expression, where
         * two statements would do */
        path = CAR(task)->path, from = path->points;
        to = from + 1;
        count = path->count - 1;
        for (i = 0; i < count; ++i) {
            line_draw_points(from, to);
            from = from + 1;
            to = to + 1;
        }
        njPopMatrixEx();
        (void)"<- COL POS X";
        (void)"<- COL POS Y";
        (void)"<- COL POS Z";
        (void)"<- COL SIZE";
        scale.x = (Uint16)work->ang.z;
        scale.y = (Uint16)work->ang.z;
        scale.z = (Uint16)work->ang.z;
        debug_draw_model(&work->pos, NULL, &scale, DEBUG_MODEL_SPHERE);
    }
    /* @hack type: the reach widened to a Sint32 before its conversion, where
     * the Uint16 converted would do */
    if (player_near(&work->pos, (Sint32)(Uint16)work->ang.z)) {
        work->mode = MODE_GO;
    } else {
        work->mode = MODE_WAIT;
    }
}

void car_init(struct task *task)
{
    struct task_work *work = task->work;

    work->submode = CAR_PARKED;
    task->step = car_setup;
}

/* a wreck's spin makes a random whole count of turns about each axis. the
 * count is under HALF_TURNS_X half turns about x, and HALF_TURNS_Z about z. it
 * is under TURNS_Y whole turns about y. a toss counts under the TOSS_ bounds
 * instead. where the counts about x and z sum to an even number, one gains a
 * half turn. x gains it where its count is 0, and otherwise x or z gains it at
 * random */
#define HALF_TURNS_X 3.0F
#define TURNS_Y 2.5F
#define HALF_TURNS_Z 2.0F
#define TOSS_HALF_TURNS_X 1.5F
#define TOSS_TURNS_Y 1.25F
#define TOSS_HALF_TURNS_Z 1.1F
/* the whole turn about x or z is its count of half turns less SPIN_SHORT
 * Angle units. it is less again by a random amount up to SPIN_SLACK times
 * RAND_MAX. the whole turn about y is its count of turns less a random amount.
 * that amount is up to SPIN_SLACK_Y times RAND_MAX. so a count below 0 turns
 * further than its count, and any other less far */
#define SPIN_SHORT 0x400
#define SPIN_SLACK (1.0F / 12.0F)
#define SPIN_SLACK_Y (1.0F / 18.0F)

/* set the wreck's spin. the wreck makes its turns while it falls back to its
 * starting height in its floor's frame. the half turns about x and z sum to an
 * odd count, so the wreck turns over. after a CRASH_NUDGE, the wreck makes half
 * a turn about z alone, either way at random. the one call passes toss FALSE,
 * so the toss bounds never apply */
static void car_wreck_set_spin(struct task *task, Bool toss, Sint16 force,
                               Uint32 flags)
{
    NJS_VECTOR spd;
    NJS_VECTOR fall;
    int steps;
    int x;
    int y;
    int z;
    int sign;

    fall.x = 0.0F;
    fall.y = -car_gravity;
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
                if (rand() * RAND_SCALE < EVEN_CHANCE) {
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
        sign = rand() * RAND_SCALE < EVEN_CHANCE ? 1 : -1;
        /* @hack redundant: 0 divided by steps, where 0 would do */
        WRECK(task)->spin.x = 0 / steps;
        WRECK(task)->spin.y = 0 / steps;
        WRECK(task)->spin.z = sign * HALF_TURN / steps;
    } else {
        if (rand() * RAND_SCALE < EVEN_CHANCE) {
            x = -x;
        }
        if (rand() * RAND_SCALE < EVEN_CHANCE) {
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
static void car_wreck_find_floor(struct task *task)
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
        njCalcVector(NULL, &car_up, &normal);
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
/* a wreck made with no life lives up to WRECK_LIFE steps, then fades over
 * WRECK_FADE steps. one made with a life fades over LIFE_FADE steps. the work's
 * submode keeps the fade's length, which tells the two kinds of wreck apart */
#define WRECK_LIFE 500
#define WRECK_FADE 55
#define LIFE_FADE 15

/* make a wreck of kind, thrown at spd by a crash of force, and return its
 * task. the call returns null where the heap can't give the task or its
 * record. the wreck's pos is its model's center, with the model at pos and
 * turned by ang. floor_pos and floor_ang make its frame until a probe finds a
 * floor. the first probe runs at once. a wreck made with a life takes a
 * COLLISION_OBJECT record */
static struct task *car_wreck_create(struct car_kind *kind, NJS_POINT3 *pos,
                                     struct angles *ang, NJS_VECTOR *spd,
                                     NJS_POINT3 *floor_pos,
                                     struct angles *floor_ang, Sint8 force,
                                     int life)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, WRECK_LEVEL, car_wreck_step,
                       "ObjectCarCrashExec");
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
    task->draw = car_wreck_draw;
    task->destructor = car_wreck_destroy;
    task->extra.p = kind;
    car_wreck_set_spin(task, FALSE, work->mode, kind->flags);
    collision_init(task, &kind->shapes[kind->shape_count], kind->shape_count,
                   life || kind->flags & CAR_WRECK_OBJECT
                       ? COLLISION_OBJECT
                       : COLLISION_OBJECT_MUTUAL);
    /* @bug memory: goes on where collision_init fails, which leaves the work's
     * collision null. car_place_shapes then reads the record's shapes through
     * null (0x8c50186e), here and at each of the wreck's steps */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init fails, the car
     * leaves no wreck, and the call returns null as for a failed task */
    if (work->collision == NULL) {
        task_destroy(task);
        return NULL;
    }
#endif
    car_place_shapes(task, TRUE, kind);
    if (life) {
        LIFE(task) = life;
        work->submode = LIFE_FADE;
    } else {
        LIFE(task) = WRECK_LIFE;
        work->submode = WRECK_FADE;
    }
    car_wreck_find_floor(task);
    return task;
}

/* a wreck made with no life throws dust while faster than DUST_GRAVITIES times
 * car_gravity. its chance of a puff each step is RISE_DUST_CHANCE while it
 * rises, and FALL_DUST_CHANCE while it falls. a puff moves at DUST_SPEED_PART
 * of the wreck's speed. a falling wreck's puff lies up to half
 * FALL_DUST_SPREAD from the wreck along each axis */
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
 * LIFE_PROBE_MASK + 1 for one made with a life. it is UNSEEN_PROBE_MASK + 1
 * instead after more than UNSEEN_PROBES unseen draws in a row */
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
 * frame. that height is box.y plus how far the box's corner lies from y = 0.
 * the corner is (box.x, 0, box.z), turned by the wreck's ang. the wreck is on
 * its roof while the corner lies below y = 0. otherwise it is on its wheels. a
 * landing past its first LANDED_STEPS steps cuts its life to LANDED_LIFE steps,
 * unless its fade has begun */
#define LANDED_STEPS 5
#define LANDED_LIFE 25
#define ROOF_LIFE 5
/* a wreck on its roof whose riding height passes ON_SIDE_HEIGHT lands as one
 * on its wheels does */
#define ON_SIDE_HEIGHT 20.0F
/* a landing faster than LAND_SPEED sounds. its volume is car_land_volume, less
 * LAND_VOLUME_DROP, plus LAND_VOLUME_RATE for each unit of speed past
 * LAND_SPEED. it is at most LAND_VOLUME_MAX more than car_land_volume. a wreck
 * with a life sounds LIFE_VOLUME_DROP quieter */
#define LAND_SPEED 0.2F
#define LAND_VOLUME_RATE 2.0F
#define LAND_VOLUME_DROP 20.0F
#define LAND_VOLUME_MAX 5
#define LIFE_VOLUME_DROP 10
/* a wreck that lands on its wheels bounces up at BOUNCE of its speed across its
 * floor. after a CRASH_HIT or harder, it bounces at HIT_BOUNCE at least. after
 * a CRASH_NUDGE, it bounces at NUDGE_BOUNCE at least */
#define BOUNCE 0.6F
#define HIT_BOUNCE 0.8F
#define NUDGE_BOUNCE 0.3F
/* a wreck that lands on its roof bounces up at ROOF_SPEED_PART of its speed
 * across its floor. it keeps that part of its speed along its floor too. after
 * a CRASH_FLAME_HIT the part is FLAME_ROOF_SPEED_PART */
#define ROOF_SPEED_PART 0.1F
#define FLAME_ROOF_SPEED_PART 0.4F
/* a settling wreck judges which way to turn from its angles SPIN_AHEAD steps
 * of spin ahead */
#define SPIN_AHEAD 15
/* a wreck on its roof settles while its center lies within UPSIDE_REACH above
 * its riding height. it must also rise at SETTLE_GRAVITIES times car_gravity
 * or less. after a CRASH_NUDGE the reach is NUDGE_UPSIDE_REACH, and the wreck
 * also settles once its fade has begun */
#define UPSIDE_REACH 8.0F
#define NUDGE_UPSIDE_REACH 1.0F
#define SETTLE_GRAVITIES 2.0F
/* a wreck settling on its roof turns its ang.y toward its frame's. it eases as
 * EASE_TURN eases, with YAW_LIMIT and YAW_EASE */
#define YAW_LIMIT 40
#define YAW_EASE 0.05F
#define EASE_FINE_STEP 2
/* a wreck loses its speed on a step that meets three tests. its center lies
 * under REST_HEIGHT in its frame. its spin about each axis lies within
 * REST_SPIN. its speed lies below REST_GRAVITIES times car_gravity */
#define REST_HEIGHT 1.0F
#define REST_SPIN 128
#define REST_GRAVITIES 3.0F
#define SPEED_KEEP 0.995F
/* a landing on its wheels turns a wreck about z only while its spin there lies
 * within SPIN_Z_QUIET */
#define SPIN_Z_QUIET 0x600
/* the truck pushes a wreck out of its box. with a chance of WRECK_FLIP_CHANCE,
 * the push reverses the wreck's spin about y and z. otherwise its spin about
 * x gains ROCK_TURN degrees times the cosine of its ang.x. the gain takes the
 * sign of the sine of ang.x. it is 0 where that sine lies beyond ROCK_SINE
 * either way */
#define WRECK_FLIP_CHANCE 0.03F
#define ROCK_SINE 0.8F
#define ROCK_TURN 3.0F
/* a wreck ends once UNSEEN_DRAWS draws in a row find it out of view. the truck
 * pushes it only while fewer than TRUCK_DRAWS have */
#define UNSEEN_DRAWS 20
#define TRUCK_DRAWS 3
/* cut turn, a Sint16, to the part of it one step takes. past limit either
 * way, turn becomes turn times k, as SCALE_ANGLE multiplies. within limit, it
 * becomes EASE_FINE_STEP with turn's sign, or 0 where it lies within
 * EASE_FINE_STEP */
#define EASE_TURN(turn, limit, k)                                              \
    {                                                                          \
        if ((turn) > (limit) || (turn) < -(limit)) {                           \
            SCALE_ANGLE(turn, k);                                              \
        } else if ((turn) < -EASE_FINE_STEP) {                                 \
            (turn) = -EASE_FINE_STEP;                                          \
        } else if ((turn) > EASE_FINE_STEP) {                                  \
            (turn) = EASE_FINE_STEP;                                           \
        } else {                                                               \
            (turn) = 0;                                                        \
        }                                                                      \
    }

/* vector_add adds add to vector, and vector_scale multiplies vector by
 * scale */
#pragma inline(vector_add)
/* @hack spelling: the parts through walking pointers, where three assignments
 * would do */
static void vector_add(NJS_VECTOR *vector, NJS_VECTOR *add)
{
    Float *dst = &vector->x;
    Float *src = &add->x;

    *dst++ += *src++;
    *dst++ += *src++;
    *dst += *src;
}

#pragma inline(vector_scale)
/* @hack spelling: the parts through a walking pointer, where three
 * assignments would do */
static void vector_scale(Float scale, NJS_VECTOR *vector)
{
    Float *dst = &vector->x;

    *dst++ *= scale;
    *dst++ *= scale;
    *dst *= scale;
}

/* a wreck falls, and bounces off the sides its probe found and off its
 * floor. one that lands on its wheels after a CRASH_HIT or harder turns toward
 * its roof. one on its roof near its floor settles in its floor's frame. its
 * pitch and roll each turn toward the nearer of upright and half a turn. a
 * wreck ends once its fade is over */
static void car_wreck_step(struct task *task)
{
    struct task_work *work = task->work;
    struct surface_side *wall;
    NJS_POINT3 *place;
    NJS_VECTOR local_spd;
    NJS_VECTOR corner;
    NJS_POINT3 local_pos;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float height;
    Float value;
    Bool upside;
    Sint16 volume;
    Sint16 *low_half;
    int louder;

    if (njScalor(&WRECK(task)->spd) > car_gravity * DUST_GRAVITIES) {
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
                 * its puff at its jittered copy (0x8c50454a, 0x8c5045f6) */
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
            car_wreck_find_floor(task);
        }
    } else if (WRECK(task)->unseen > UNSEEN_PROBES) {
        if (!((work->timer + LIFE(task)) & UNSEEN_PROBE_MASK)) {
            car_wreck_find_floor(task);
        }
    } else if (!((work->timer + LIFE(task)) & LIFE_PROBE_MASK)) {
        car_wreck_find_floor(task);
    }
    WRECK(task)->spd.y -= car_gravity;
    vector_add(&work->pos, &WRECK(task)->spd);

    wall = &WRECK(task)->probe.sides[SURFACE_NEG_X];
    if (work->pos.x < wall->pos && wall->found &&
        wall->normal.x > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        WRECK(task)->spd.x = njAbs(WRECK(task)->spd.x) * WALL_BOUNCE;
    }
    wall = &WRECK(task)->probe.sides[SURFACE_POS_X];
    if (work->pos.x > wall->pos && wall->found &&
        wall->normal.x < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        WRECK(task)->spd.x = -(njAbs(WRECK(task)->spd.x) * WALL_BOUNCE);
    }
    wall = &WRECK(task)->probe.sides[SURFACE_NEG_Z];
    if (work->pos.z < wall->pos && wall->found &&
        wall->normal.z > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        WRECK(task)->spd.z = njAbs(WRECK(task)->spd.z) * WALL_BOUNCE;
    }
    wall = &WRECK(task)->probe.sides[SURFACE_POS_Z];
    if (work->pos.z > wall->pos && wall->found &&
        wall->normal.z < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        WRECK(task)->spd.z = -(njAbs(WRECK(task)->spd.z) * WALL_BOUNCE);
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
    /* @hack spelling: spd reused for the box's corner, where a local of its
     * own would do */
    spd = WRECK_KIND(task)->box;
    spd.y = 0.0F;
    njCalcVector(NULL, &spd, &corner);
    njPopMatrixEx();
    height = njAbs(corner.y) + WRECK_KIND(task)->box.y;
    upside = corner.y < 0.0F;

    if (local_pos.y < height) {
        if (!work->timer && WRECK(task)->steps > LANDED_STEPS &&
            LIFE(task) > LANDED_LIFE) {
            LIFE(task) = LANDED_LIFE;
        }
        local_pos.y = height;
        value = njAbs(local_spd.y);
        if (value > LAND_SPEED) {
            louder =
                ((value - LAND_SPEED) * LAND_VOLUME_RATE) - LAND_VOLUME_DROP;
            if (louder > LAND_VOLUME_MAX) {
                louder = LAND_VOLUME_MAX;
            }
            volume = car_land_volume + louder;
            if (work->submode == WRECK_FADE) {
                sound_play_at(LAND_SOUND, NULL, SOUND_PRIORITY, volume,
                              &work->pos);
            } else {
                sound_play_at(LAND_SOUND, NULL, SOUND_PRIORITY,
                              volume - LIFE_VOLUME_DROP, &work->pos);
            }
        }
        if (!upside || height > ON_SIDE_HEIGHT) {
            local_spd.y = njAbs(local_spd.y) * BOUNCE;
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
                Bool flip = FALSE;

                SCALE_ANGLE(WRECK(task)->spin.x, car_land_spin_keep);
                SCALE_ANGLE(WRECK(task)->spin.y, car_land_spin_keep);
                SCALE_ANGLE(WRECK(task)->spin.z, car_land_spin_keep);
                turn = WRECK(task)->spin.x;
                turn = (Sint16)work->ang.x + (turn * SPIN_AHEAD) -
                       WRECK(task)->ang.x;
                if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
                    flip = TRUE;
                    turn = -HALF_TURN;
                } else {
                    turn = 0;
                }
                turn = (Sint16)work->ang.x + turn - WRECK(task)->ang.x;
                EASE_TURN(turn, car_land_limit, 1.0F / car_land_divisor);
                turn +=
                    (Angle)(RANDOM_HALF() * car_land_spin_x * ANGLE_PER_DEGREE);
                WRECK(task)->spin.x -= turn;
                turn = (Sint16)work->ang.y - WRECK(task)->ang.y;
                EASE_TURN(turn, car_land_limit, 1.0F / car_land_divisor);
                turn +=
                    (Angle)(RANDOM_HALF() * car_land_spin_y * ANGLE_PER_DEGREE);
                WRECK(task)->spin.y -= turn;
                turn = WRECK(task)->spin.z;
                if (turn < SPIN_Z_QUIET && turn > -SPIN_Z_QUIET) {
                    turn = work->ang.z;
                    if (!flip) {
                        turn -= HALF_TURN;
                    }
                    EASE_TURN(turn, car_land_limit, 1.0F / car_land_divisor);
                    turn += (Angle)(RANDOM_HALF() * car_land_spin_z *
                                    ANGLE_PER_DEGREE);
                    WRECK(task)->spin.z -= turn;
                }
            }
        } else {
            /* @bug logic: never cuts the life to ROOF_LIFE. the test
             * (0x8c504c88) asks for more than LANDED_LIFE. the landing's own
             * cap (0x8c5048e2) has already left no more than that */
            if (!work->timer && WRECK(task)->steps > LANDED_STEPS &&
                LIFE(task) > LANDED_LIFE) {
                LIFE(task) = ROOF_LIFE;
            }
            /* @hack spelling: the three products in each arm, where the
             * factor alone in each arm would do */
            if (work->mode <= CRASH_HIT) {
                local_spd.y = njAbs(local_spd.y) * ROOF_SPEED_PART;
                local_spd.x *= ROOF_SPEED_PART;
                local_spd.z *= ROOF_SPEED_PART;
            } else {
                local_spd.y = njAbs(local_spd.y) * FLAME_ROOF_SPEED_PART;
                local_spd.x *= FLAME_ROOF_SPEED_PART;
                local_spd.z *= FLAME_ROOF_SPEED_PART;
            }
        }
    }
    /* @bug logic: never stops a wreck. the test (0x8c504d04) takes the
     * center's height in the frame for its height above its riding height.
     * the landing leaves the center at least its riding height up, which is 7
     * or more for every kind's box */
    if (local_pos.y < REST_HEIGHT && (Sint16)WRECK(task)->spin.x < REST_SPIN &&
        (Sint16)WRECK(task)->spin.x > -REST_SPIN &&
        (Sint16)WRECK(task)->spin.y < REST_SPIN &&
        (Sint16)WRECK(task)->spin.y > -REST_SPIN &&
        (Sint16)WRECK(task)->spin.z < REST_SPIN &&
        (Sint16)WRECK(task)->spin.z > -REST_SPIN &&
        njScalor(&local_spd) < car_gravity * REST_GRAVITIES) {
        local_spd.x = 0.0F;
        local_spd.y = 0.0F;
        local_spd.z = 0.0F;
    }
    /* @hack redundant: the wreck's speed scaled, though the njCalcVector
     * below overwrites it */
    vector_scale(SPEED_KEEP, &WRECK(task)->spd);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&WRECK(task)->pos);
    njRotateY(NULL, WRECK(task)->ang.y);
    njRotateX(NULL, WRECK(task)->ang.x);
    njCalcPoint(NULL, &local_pos, &work->pos);
    njCalcVector(NULL, &local_spd, &WRECK(task)->spd);
    njPopMatrixEx();
    if (upside &&
        ((work->mode >= CRASH_HIT && local_pos.y < height + UPSIDE_REACH &&
          local_spd.y <= car_gravity * SETTLE_GRAVITIES) ||
         (work->mode < CRASH_HIT &&
          (work->timer || (local_pos.y < height + NUDGE_UPSIDE_REACH &&
                           local_spd.y <= car_gravity * SETTLE_GRAVITIES))))) {
        Sint16 turn;

        SCALE_ANGLE(WRECK(task)->spin.x, car_upside_spin_keep);
        SCALE_ANGLE(WRECK(task)->spin.y, car_upside_spin_keep);
        SCALE_ANGLE(WRECK(task)->spin.z, car_upside_spin_keep);
        turn = WRECK(task)->spin.x;
        turn = work->ang.x + (turn * SPIN_AHEAD) - WRECK(task)->ang.x;
        if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
            turn = -HALF_TURN;
        } else {
            turn = 0;
        }
        turn = work->ang.x + turn - WRECK(task)->ang.x;
        EASE_TURN(turn, car_upside_limit, 1.0F / car_upside_divisor);
        WRECK(task)->spin.x -= turn;
        turn = work->ang.y - WRECK(task)->ang.y;
        EASE_TURN(turn, YAW_LIMIT, YAW_EASE);
        work->ang.y -= turn;
        turn = WRECK(task)->spin.z;
        turn = work->ang.z + (turn * SPIN_AHEAD);
        if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
            turn = -HALF_TURN;
        } else {
            turn = 0;
        }
        turn = work->ang.z + turn;
        EASE_TURN(turn, car_upside_limit, 1.0F / car_upside_divisor);
        WRECK(task)->spin.z -= turn;
        /* @hack redundant: an empty else, where none would do */
    } else {
    }
    if (WRECK(task)->unseen < TRUCK_DRAWS) {
        struct task *truck = car_truck;

        if (truck && truck->child && truck->force.p) {
            NJS_MATRIX *matrices = truck->force.p;
            /* @hack type: the truck's shape through a pointer to the array,
             * where a pointer to the shape would do */
            struct collision_shape(*shape)[1];
            Bool inside = FALSE;

            njPushMatrixEx();
            njSetMatrix(NULL, &matrices[TRUCK_INVERSE]);
            njCalcPoint(NULL, &work->pos, &spd);
            shape = &car_truck_shape;
            if (njAbs(spd.x) < (*shape)[0].radius &&
                njAbs(spd.y) < (*shape)[0].size_y &&
                njAbs(spd.z) < (*shape)[0].size_z) {
                inside = TRUE;
                if ((*shape)[0].size_y - njAbs(spd.y) >
                        (*shape)[0].size_z - njAbs(spd.z) ||
                    spd.y < 0.0F) {
                    spd.z = (*shape)[0].size_z;
                } else {
                    spd.y = (*shape)[0].size_y;
                }
            }
            if (inside) {
                njSetMatrix(NULL, &matrices[TRUCK_FRAME]);
                njCalcPoint(NULL, &spd, &spd);
                work->pos.x = spd.x;
                work->pos.z = spd.z;
                if (work->pos.y < spd.y) {
                    work->pos.y = spd.y;
                }
                if (rand() * RAND_SCALE < WRECK_FLIP_CHANCE) {
                    WRECK(task)->spin.y = -WRECK(task)->spin.y;
                    WRECK(task)->spin.z = -WRECK(task)->spin.z;
                } else {
                    Float sine = njSin(work->ang.x);

                    if (njAbs(sine) < ROCK_SINE) {
                        Float turn = njCos(work->ang.x) * ROCK_TURN;

                        if (sine > 0.0F) {
                            WRECK(task)->spin.x +=
                                (Angle)(turn * ANGLE_PER_DEGREE);
                        } else {
                            WRECK(task)->spin.x +=
                                (Angle)(-turn * ANGLE_PER_DEGREE);
                        }
                    }
                }
            }
            njPopMatrixEx();
        }
    }
    if (WRECK(task)->unseen < UNSEEN_DRAWS) {
        car_place_shapes(task, TRUE, WRECK_KIND(task));
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
/* each channel of a 32-bit color runs from 0 to CHANNEL_MAX */
#define CHANNEL_MAX 255
/* a fading wreck's tint has its alpha in the alpha, green and blue channels.
 * its red channel is half the alpha plus RED_BASE */
#define RED_BASE 128
/* the wreck's wheels turn about x by WHEEL_TURN Angle units a step */
#define WHEEL_TURN 240

static void car_wreck_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object read through the task's kind before kind is set,
     * where kind would do */
    NJS_CNK_OBJECT *object = WRECK_KIND(task)->wreck_object;
    struct car_kind *kind;
    NJS_CNK_OBJECT *child;
    /* @hack redundant: unused, a local that only sizes the frame */
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
        car_draw_shadow(kind->shadow_x, kind->shadow_z, &work->ang);
        njRotateEx(&work->ang.x, ZXY_TURN);
        njSetTexture(kind->wreck_texlist);
        njTranslate(NULL, -object->model->center.x, -object->model->center.y,
                    -object->model->center.z);
        model_draw_tinted(object->model, color);
        child = object->child;
        if (child) {
            /* @hack type: the steps read as game_state_steps, where
             * game_state.steps would do */
            spin = game_state_steps * WHEEL_TURN;
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
        car_draw_shadow(kind->shadow_x, kind->shadow_z, &work->ang);
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
                /* @hack type: the steps read as game_state_steps, where
                 * game_state.steps would do */
                car_draw_wheels(child, (Angle)game_state_steps * WHEEL_TURN);
            }
        }
        njPopMatrixEx();
    }
}

static void car_wreck_destroy(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
    /* extra holds the wreck's kind, an entry of car_kinds that task_destroy
     * mustn't free */
    task->extra.p = NULL;
}
