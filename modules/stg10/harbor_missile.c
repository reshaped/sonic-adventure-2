/* the missile, MHMISSILE in Metal Harbor's object list: the rocket the stage
 * ends on. a player who grips one of its handholds before its count runs out
 * rides it up. the missile throws the rider off toward a point above the
 * harbor */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/rocket_thrust.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stg10/clouds.h"
#include "stg10/harbor_missile.h"
#include "stg10/sea.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the missile's record, in task->motion */
struct harbor_missile {
    Float speed; /* what a rising step adds to the work's pos.y */
    Float ground_y; /* the pos.y the missile starts at */
    struct task *clouds; /* the clouds the missile rises through, or null */
    /* the index of the missile's camera mode on SLOT's camera's stack, or -1
     * for none */
    Sint32 camera;
    /* the work's timer at the grip, where the countdown stops */
    Sint32 gripped_at;
    /* the stream voice_play gave VOICE_HURRY, or -1 for none */
    Sint32 voice;
};
#define MISSILE(task) ((struct harbor_missile *)(task)->motion.p)
/* the slot of the player the missile serves, in its work's tick. that is the
 * player that came near, and the rider from the grip on */
#define SLOT(work) ((work)->tick)
/* the steps since the grip, counted in HARBOR_MISSILE_MODE_GRIPPED and
 * HARBOR_MISSILE_MODE_LIFT_OFF */
#define GRIP_STEPS(task) ((task)->unk_3c.l)

/* the work of the missile's camera modes, in their camera_slot's work */
struct harbor_missile_camera {
    /* the circling camera's steps, from 0 as the missile sets the mode */
    Sint32 steps;
    /* the missile's pos as a follow camera starts, which nothing reads */
    NJS_POINT3 pos;
};
#define CAMERA(slot) ((struct harbor_missile_camera *)(slot)->work)

/* a term of the shake: amp times the sine of freq times the step. freq is in
 * Angle units a step */
struct shake_wave {
    Angle freq;
    Float amp;
};
#define SHAKE_WAVES 4

/* the point a follow_span's pos lies off. SPAN_GROUND's is the point at y 0
 * under the player point */
enum follow_span_kind { SPAN_GROUND, SPAN_PLAYER };

/* a follow camera's player point lies follow_height above the center of its
 * player's first shape. a span places the camera while that point's height
 * lies from low to high */
struct follow_span {
    /* the point pos lies off. the offset turns by the missile's ang.y and its
     * handhold's */
    enum follow_span_kind kind;
    Float low;
    Float high;
    /* the camera's target goes from target_from at low to target_to at high.
     * a null target stands for the player point */
    NJS_POINT3 *target_from;
    NJS_POINT3 *target_to;
    /* the camera's pos goes from pos_from at low to pos_to at high */
    NJS_POINT3 pos_from;
    NJS_POINT3 pos_to;
};
#define LIFT_SPANS 4
#define THROWN_SPANS 1

/* the missile's shapes: its body, then the lower handhold and the upper one.
 * a handhold's rider hangs HANG_DROP below its center. its ang.y turns the
 * rider and the follow cameras */
#define BODY_SHAPE 0
#define FIRST_HANDHOLD 1
#define HANDHOLDS 2
#define SHAPES (FIRST_HANDHOLD + HANDHOLDS)
/* the gripped handhold's shape, by its number in the work's submode */
#define HANDHOLD(work) (harbor_missile_shapes[(work)->submode + FIRST_HANDHOLD])
#define UPPER_HANDHOLD 1
#define HANG_DROP 10.0F
/* the animation PLAYER_REQUEST_ANIM gives the rider */
#define HOLD_ANIM 172

/* a player within scl.x + START_MARGIN of the missile starts the count */
#define START_MARGIN 100.0F
#define COUNT_STEPS 900
#define HUNDREDTHS_PER_STEP (100.0F / STEPS_PER_SECOND)
/* the count's voices. VOICE_START plays as the count starts, at
 * VOICE_START_PRIORITY. VOICE_HURRY plays at the count's step COUNT_HURRY.
 * VOICE_GO plays at the grip, or as the count runs out. those two play at
 * VOICE_PRIORITY */
#define VOICE_START 1920
#define VOICE_HURRY 1921
#define VOICE_GO 1922
#define COUNT_HURRY 240
#define VOICE_START_PRIORITY 1
#define VOICE_PRIORITY 0
/* a grip of the upper handhold scores GRIP_SCORE_UPPER. a grip of the lower
 * one scores by the count's step at the grip. before COUNT_FAST it scores
 * GRIP_SCORE_FAST, before COUNT_SOON GRIP_SCORE_SOON, and before COUNT_LATE
 * GRIP_SCORE_LATE. a later grip scores nothing */
#define GRIP_SCORE_UPPER 1500
#define GRIP_SCORE_FAST 800
#define GRIP_SCORE_SOON 400
#define GRIP_SCORE_LATE 200
#define COUNT_FAST 540
#define COUNT_SOON 660
#define COUNT_LATE 780
/* a gripped missile lifts off once GRIP_STEPS passes GRIP_WAIT, or once its
 * timer passes COUNT_STEPS */
#define GRIP_WAIT 30
/* the speed a rising step gains, up to harbor_missile_top_speed */
#define RISE_ACCEL 0.02F
/* a rise is HARBOR_MISSILE_MODE_LIFT_OFF, HARBOR_MISSILE_MODE_EMPTY or
 * HARBOR_MISSILE_MODE_THROWN. through its first LAUNCH_STEPS, the camera
 * circles the pad. the camera's shake then runs LAUNCH_SHAKE_PACE times as
 * fast, and LAUNCH_SHAKE_SCALE times as far. an empty missile's screen fades to
 * white over those steps. the missile draws its flying model only past them */
#define LAUNCH_STEPS 240
#define LAUNCH_SHAKE_PACE 2
#define LAUNCH_SHAKE_SCALE 3.0F
/* the rider is thrown off once it hangs THROW_HEIGHT above throw_target. above
 * y JUMP_OFF_HEIGHT, a press of A on its pad throws it off sooner */
#define THROW_HEIGHT 1100.0F
#define JUMP_OFF_HEIGHT 2800.0F
/* the thrown missile ends once its handhold, less HANG_DROP, passes
 * ENDING_HEIGHT above throw_target */
#define ENDING_HEIGHT 2000.0F
/* an empty missile ends once its timer passes EMPTY_STEPS. at its step
 * MISS_STEP, the nearest player misses. that player doesn't while a miss is
 * under way, or once it has missed */
#define EMPTY_STEPS 600
#define MISS_STEP 120
/* the missile's task ends once HARBOR_MISSILE_MODE_END's timer passes
 * END_STEPS */
#define END_STEPS 30
/* at the rise's step BREAK_STEP, the gantry breaks into harbor_missile_parts.
 * the pieces fly at BREAK_SPEED across toward circle_pos, and at BREAK_LIFT
 * up. each gains up to BREAK_SPREAD either way along each axis at random.
 * they fall toward y BREAK_FLOOR, and pass through it. they trail dust but
 * for an empty missile's */
#define BREAK_STEP 38
#define BREAK_SPEED 3.2F
#define BREAK_LIFT 0.35F
#define BREAK_SPREAD 1.3F
#define BREAK_FLOOR (-330.0F)
/* debris_break_sized spins each piece at random. the range shrinks to 0 as the
 * radius of the piece's model grows to BREAK_RADIUS */
#define BREAK_RADIUS 45.0F
#define PARTS 45
/* above the clouds, the missile turns its handhold toward throw_target, by at
 * most TURN_MAX Angle units a step */
#define TURN_MAX 18
/* the waves the missile raises in the sea. a wave comes only while fewer
 * than WAVES_MAX run, and by WAVE_CHANCE */
#define WAVES_MAX 3
#define WAVE_CHANCE 0.1F
/* these waves come while the count runs or the missile is gripped. one may
 * come at each WAVE_WAIT_STEPS of the timer. it stands WAVE_HEIGHT high. its
 * rate is WAVE_RATE, plus up to WAVE_RATE_RANGE at random */
#define WAVE_HEIGHT 150.0F
#define WAVE_WAIT_STEPS 30
#define WAVE_RATE 640
#define WAVE_RATE_RANGE 768.0F
/* in a rise, a wave may come at each WAVE_FLIGHT_STEPS of game_state.steps.
 * it stands WAVE_FLIGHT_HEIGHT high. its rate is WAVE_FLIGHT_RATE, plus up to
 * WAVE_FLIGHT_RATE_RANGE at random. past the rise's step WAVE_STEADY_STEPS,
 * a wave comes without WAVE_CHANCE */
#define WAVE_FLIGHT_HEIGHT 800.0F
#define WAVE_FLIGHT_STEPS 50
#define WAVE_FLIGHT_RATE 512
#define WAVE_FLIGHT_RATE_RANGE 512.0F
#define WAVE_STEADY_STEPS 255
/* a wave's center lies up to WAVE_SPREAD / 2 from the sea's origin along each
 * axis. it lives WAVE_LIFE steps, plus up to WAVE_LIFE_RANGE */
#define WAVE_SPREAD 1000.0F
#define WAVE_LIFE 160
#define WAVE_LIFE_RANGE 60.0F
/* the missile's sounds in metal_harbor_sound_list. it rumbles while the count
 * runs and while it is gripped, and roars as it rises. each step plays the
 * rumble or the roar again. SOUND_UPDATES is sound_play_for's duration, in
 * sound updates */
#define SOUND_RUMBLE 10
#define SOUND_ROAR 11
#define SOUND_UPDATES 10
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
/* a step of HARBOR_MISSILE_MODE_EMPTY, or of a lift-off's first
 * LAUNCH_STEPS, plays a blast by BLAST_CHANCE. the blast is SOUND_BLAST plus
 * a roll below BLAST_ROLL, taken mod SOUND_BLASTS */
#define SOUND_BLAST 7
#define SOUND_BLASTS 3
#define BLAST_CHANCE 0.05F
#define BLAST_ROLL 100.0F
/* through a rise's first LAUNCH_STEPS, every step makes a wide puff of
 * thrust. after, a step makes a puff by PUFF_CHANCE. a puff starts PUFF_RISE
 * above the missile's pos */
#define PUFF_CHANCE 0.7F
#define PUFF_RISE 50.0F
#define PUFF_SPEED_SHARE 0.6F
/* the puffs' floor lies PUFF_FLOOR_RISE above ground_y, and no lower than
 * puff_floor_min */
#define PUFF_FLOOR_RISE 40.0F
/* in HARBOR_MISSILE_MODE_LIFT_OFF and HARBOR_MISSILE_MODE_EMPTY, player 0's
 * pad shakes each SHAKE_STEPS of game_state.steps. it shakes only while
 * player 0 is less than SHAKE_ABOVE above camera 0. the shake's power is
 * SHAKE_POWER within SHAKE_NEAR of camera 0. it falls by SHAKE_FALLOFF of
 * that for each unit farther. the pad shakes only above SHAKE_POWER_MIN */
#define SHAKE_STEPS 25
#define SHAKE_ABOVE 100.0F
#define SHAKE_NEAR 80.0F
#define SHAKE_FALLOFF 0.002F
#define SHAKE_POWER 5.0F
#define SHAKE_POWER_MIN 1.0F
#define SHAKE_FREQ 15
#define SHAKE_TIME 1
/* while the count runs and the missile is gripped, the pad shakes each
 * WAIT_SHAKE_STEPS of game_state.steps, at WAIT_SHAKE_POWER */
#define WAIT_SHAKE_STEPS 18
#define WAIT_SHAKE_POWER 1
#define WAIT_SHAKE_FREQ 20
/* each shake's frequency gains up to SHAKE_FREQ_RANGE at random */
#define SHAKE_FREQ_RANGE 5.0F
/* in HARBOR_MISSILE_MODE_LIFT_OFF, the countdown shows through the first
 * BLINK_SHOWN of every BLINK_STEPS steps of GRIP_STEPS, until BLINK_END */
#define BLINK_STEPS 30
#define BLINK_SHOWN 15
#define BLINK_END 180
#define BLACK 0xff000000
/* the most the countdown's two digits of seconds or of hundredths show */
#define COUNTDOWN_MAX 99

/* @hack spelling: the parts through a walking pointer, where three
 * assignments would do */
#define VECTOR_SCALE(v, scale)                                                 \
    {                                                                          \
        Float *pd = &(v)->x;                                                   \
        *pd++ *= (scale);                                                      \
        *pd++ *= (scale);                                                      \
        *pd *= (scale);                                                        \
    }
/* @hack spelling: the parts through walking pointers, where three
 * assignments would do */
#define VECTOR_DIFFERENCE(out, a, b)                                           \
    {                                                                          \
        Float *pd = &(out)->x;                                                 \
        Float *pa = &(a)->x;                                                   \
        Float *pb = &(b)->x;                                                   \
        *pd = *pa++ - *pb++;                                                   \
        pd++;                                                                  \
        *pd = *pa++ - *pb++;                                                   \
        pd++;                                                                  \
        *pd = *pa - *pb;                                                       \
    }

#ifdef NONMATCHING
/* the plain form the hack tag names */
#define harbor_missile_wave_sum_outside harbor_missile_wave_sum
#else
/* @hack spelling: the step calls harbor_missile_wave_sum by a second name,
 * which symbols.txt places at its address, where its own name would do */
Float harbor_missile_wave_sum_outside(struct shake_wave *waves, int count,
                                      Uint16 step);
#endif

#include "stg10/harbor_missile/text.inc"
#include "stg10/structures/harbor_missile.h"
#include "stg10/structures/harbor_missile_flying.h"
#include "stg10/structures/harbor_missile_piece1.h"
#include "stg10/structures/harbor_missile_piece2.h"
#include "stg10/structures/harbor_missile_piece3.h"
#include "stg10/structures/harbor_missile_piece4.h"

NJS_TEXLIST *harbor_missile_texlists[8] = {
#include "stg10/harbor_missile/harbor_missile_texlists.inc"
};
struct task *harbor_missile_task = {
#include "stg10/harbor_missile/harbor_missile_task.inc"
};
/* the back colors njSetBackColor takes once the missile is above the clouds */
static Uint32 sky_color_0 = 0xff00265e;
static Uint32 sky_color_1 = 0xff00265e;
static Uint32 sky_color_2 = 0xff00265e;
static Float dat_8c5e6ed4 = 5.0F;
static Float harbor_missile_top_speed = 5.0F;
/* the point the rider is thrown toward */
static NJS_POINT3 throw_target = {
#include "stg10/harbor_missile/throw_target.inc"
};
/* the speed of the throw along x and z */
static Float throw_speed = 1.8F;
/* a puff of thrust's speed is up to puff_spread_xz / 2 either way along x
 * and z. along y it is up to puff_spread_y / 2 either way, less puff_fall. it
 * also takes PUFF_SPEED_SHARE of the missile's speed along y. the puff's
 * scale is puff_scale_min, plus up to puff_scale_range at random */
static Float puff_spread_xz = 10.0F;
static Float puff_spread_y = 20.0F;
static Float puff_fall = 50.0F;
static Float puff_scale_min = 60.0F;
static Float puff_scale_range = 30.0F;
/* clang-format off */
static struct collision_shape harbor_missile_shapes[SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 460.0F, 0.0F}, 75.0F, 385.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 321.0F, -88.25F}, 10.0F, 0.0F, 0.0F, 0, {0, QUARTER_TURN, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {88.25F, 616.75F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the gantry about the missile, in pieces at their places in the world */
static struct debris_part harbor_missile_parts[PARTS] = {
#include "stg10/harbor_missile/harbor_missile_parts.inc"
};
/* the shake of SLOT's camera, a sum of SHAKE_WAVES terms along each axis.
 * the wait_ waves shake it while the count runs or the missile is gripped. the
 * launch_ waves shake it in HARBOR_MISSILE_MODE_LIFT_OFF and
 * HARBOR_MISSILE_MODE_EMPTY */
static struct shake_wave launch_shake_y[SHAKE_WAVES] = {
    {11261, 0.3F},
    {3728, 0.19F},
    {485, 0.3F},
    {137, 0.2F},
};
static struct shake_wave launch_shake_x[SHAKE_WAVES] = {
    {11261, 0.4F},
    {2048, 0.19F},
    {408, 0.3F},
    {105, 0.2F},
};
static struct shake_wave launch_shake_z[SHAKE_WAVES] = {
    {4674, 0.31F},
    {3728, 0.3F},
    {306, 0.3F},
    {121, 0.1F},
};
static struct shake_wave wait_shake_y[SHAKE_WAVES] = {
    {5630, 0.3F},
    {1864, 0.19F},
    {242, 0.3F},
    {68, 0.2F},
};
static struct shake_wave wait_shake_x[SHAKE_WAVES] = {
    {5630, 0.4F},
    {1024, 0.19F},
    {204, 0.3F},
    {52, 0.2F},
};
static struct shake_wave wait_shake_z[SHAKE_WAVES] = {
    {2337, 0.31F},
    {1864, 0.3F},
    {153, 0.3F},
    {60, 0.1F},
};
/* the circling camera looks at circle_target, which rises circle_rise a
 * step. the camera turns about the target's start by circle_turn a step. it
 * stands at circle_pos at its step CIRCLE_START */
static NJS_POINT3 circle_target = {
#include "stg10/harbor_missile/circle_target.inc"
};
static NJS_POINT3 circle_pos = {
#include "stg10/harbor_missile/circle_pos.inc"
};
static Angle circle_turn = 32;
static Float circle_rise = 0.15F;
#define CIRCLE_START 120
/* a switch nothing sets. while it isn't 0, the lifting missile doesn't rise.
 * a follow camera then stands at the first span's pos_from off the player
 * point. it looks at the player point where the switch is 1, and at
 * throw_target otherwise */
static Sint32 harbor_missile_debug_view = 0;
/* the follow cameras' spans in HARBOR_MISSILE_MODE_LIFT_OFF and in
 * HARBOR_MISSILE_MODE_THROWN */
static struct follow_span lift_spans[LIFT_SPANS] = {
#include "stg10/harbor_missile/lift_spans.inc"
};
static struct follow_span thrown_spans[THROWN_SPANS] = {
#include "stg10/harbor_missile/thrown_spans.inc"
};
static Float follow_height = 5.0F;
/* a follow camera turns by the missile's ang.y, its handhold's and
 * follow_turn_offset. follow_turn keeps that turn for a step without a
 * missile */
static Angle follow_turn_offset = 0;
static Angle follow_turn = 0;
static Float puff_floor_min = -138.0F;
/* the countdown draws from countdown_pos, in the screen's pixels, at
 * countdown_scale. the colon lies countdown_seconds_gap right of the
 * seconds, and the hundredths countdown_colon_gap right of the colon */
static Float countdown_scale = 0.7F;
static Float countdown_seconds_gap = 32.0F;
static Float countdown_colon_gap = 32.0F;
static Uint32 countdown_color = 0xffff1010;
static NJS_POINT3 countdown_pos = {300.0F, 50.0F, 0.0F};
/* the roar's volume once the rider is thrown */
static Sint32 roar_volume = 35;

static void harbor_missile_destroy(struct task *task);
static void harbor_missile_step(struct task *task);
static void harbor_missile_draw(struct task *task);

/* the sum of count terms of waves at step */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do once the step calls
 * the sum by this name */
Float harbor_missile_wave_sum(struct shake_wave *waves, int count, Uint16 step)
{
    Float sum = 0.0F;
    int time = step;

    while (count--) {
        sum += njSin(waves->freq * time) * waves->amp;
        waves++;
    }
    return sum;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the camera mode of a rise's first LAUNCH_STEPS, which circles the pad */
static void harbor_missile_camera_circle(struct camera_stack *stack,
                                         struct camera_slot *slot)
{
    struct harbor_missile_camera *camera = CAMERA(slot);
    NJS_POINT3 target;
    NJS_POINT3 pos;
    NJS_VECTOR offset;
    Float *from;
    Float *sub;
    Float *out;
    int steps = camera->steps;

    target = circle_target;
    target.y += circle_rise * steps;
    /* @hack spelling: the parts through walking pointers, where three
     * assignments would do */
    out = (Float *)&offset;
    from = (Float *)&circle_pos;
    sub = (Float *)&circle_target;
    *out++ = *from++ - *sub++;
    *out++ = *from++ - *sub++;
    *out = *from - *sub;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, (circle_turn * steps) - (circle_turn * CIRCLE_START));
    njCalcVector(NULL, &offset, &offset);
    njPopMatrix(1);
    /* @hack spelling: the parts through walking pointers, where three
     * assignments would do */
    out = (Float *)&pos;
    from = (Float *)&offset;
    sub = (Float *)&circle_target;
    *out++ = *from++ + *sub++;
    *out = *from++ + *sub++;
    out++;
    *out = *from + *sub;
    camera_state.target = target;
    camera_state.pos = pos;
    camera->steps++;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

/* a camera mode that follows its player up through count spans. the span
 * that holds the player point's height places the camera. where none does,
 * the first span's pos_from and target_from place it */
static void harbor_missile_camera_follow(struct camera_stack *stack,
                                         struct camera_slot *slot,
                                         struct follow_span *spans, int count)
{
    struct harbor_missile_camera *camera = CAMERA(slot);
    NJS_POINT3 player;
    NJS_POINT3 target;
    NJS_POINT3 pos;
    /* @hack redundant: unused, bytes nothing reads, which only size the
     * frame */
    Uint8 unused[12];
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    struct follow_span *span;
    Float along;
    Float short_of;
    Angle ang;
    int found;
    Float y;
    int view;
    /* @hack redundant: steps, a read of the camera's work that nothing
     * uses */
    int steps = camera->steps;
    int i;

    player = player_work[camera_current_index]->collision->shapes->center;
    player.y += follow_height;
    y = player.y;
    found = 0;
    along = 0.0F;
    for (i = 0; i < count; i++) {
        if (spans[i].low <= y && y <= spans[i].high) {
            found = i;
            along = (y - spans[i].low) / (spans[i].high - spans[i].low);
            break;
        }
    }
    span = &spans[found];
    if (harbor_missile_task) {
        /* @hack spelling: follow_turn set inside the assignment to ang,
         * where a statement of its own would do */
        follow_turn = ang = harbor_missile_task->work->ang.y +
                            HANDHOLD(harbor_missile_task->work).ang.y +
                            follow_turn_offset;
    } else {
        ang = follow_turn;
    }
    from = span->target_from;
    if (!from) {
        from = &player;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!(to = span->target_to)) {
        to = &player;
    }
    short_of = 1.0F - along;
    pos.x = (along * span->pos_to.x) + (span->pos_from.x * short_of);
    pos.y = (along * span->pos_to.y) + (span->pos_from.y * short_of);
    pos.z = (along * span->pos_to.z) + (span->pos_from.z * short_of);
    target.x = (along * to->x) + (from->x * short_of);
    target.y = (along * to->y) + (from->y * short_of);
    target.z = (along * to->z) + (from->z * short_of);
    switch (span->kind) {
    case SPAN_GROUND:
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslate(NULL, player.x, 0.0F, player.z);
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrix(1);
        break;
    case SPAN_PLAYER:
    default:
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &player);
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrix(1);
        break;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((view = harbor_missile_debug_view) != 0) {
        if (view == 1) {
            target = player;
        } else {
            target = throw_target;
        }
        pos = spans->pos_from;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &player);
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrix(1);
    }
    camera_state.target = target;
    camera_state.pos = pos;
    /* @hack spelling: the two stores chained, where a statement each would
     * do */
    camera_state.type = camera_state.ang.z = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

static void harbor_missile_camera_lift(struct camera_stack *stack,
                                       struct camera_slot *slot)
{
    harbor_missile_camera_follow(stack, slot, lift_spans, LIFT_SPANS);
}

static void harbor_missile_camera_thrown(struct camera_stack *stack,
                                         struct camera_slot *slot)
{
    harbor_missile_camera_follow(stack, slot, thrown_spans, THROWN_SPANS);
}

void harbor_missile_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct harbor_missile));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = harbor_missile_draw;
    task->step = harbor_missile_step;
    task->destructor = harbor_missile_destroy;
    collision_init(task, harbor_missile_shapes, SHAPES, COLLISION_OBJECT);
    if (harbor_missile_task == NULL) {
        harbor_missile_task = task;
    }
    MISSILE(task)->ground_y = work->pos.y;
    MISSILE(task)->camera = -1;
    MISSILE(task)->voice = -1;
}

static void harbor_missile_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (MISSILE(task)->camera >= 0) {
        camera_remove_mode(SLOT(work), MISSILE(task)->camera);
    }
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_EMPTY ||
        work->mode == HARBOR_MISSILE_MODE_THROWN ||
        work->mode == HARBOR_MISSILE_MODE_GRIPPED ||
        work->mode == HARBOR_MISSILE_MODE_COUNT) {
        njSetBackColor(BLACK, BLACK, BLACK);
        camera_stop_shake(SLOT(work));
    }
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_GRIPPED) {
        player_request(SLOT(work), PLAYER_REQUEST_RELEASE, 0);
    }
    if (MISSILE(task)->clouds) {
        task_destroy(MISSILE(task)->clouds);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    if (harbor_missile_task == task) {
        harbor_missile_task = NULL;
    }
    sea_waves_clear();
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_THROWN ||
        (work->mode == HARBOR_MISSILE_MODE_END && work->timer == 0)) {
        music_fade_in(VOLUME_FULL);
        music_play_name("m_harb1.adx");
    } else {
        music_set_name("m_harb1.adx");
    }
}

/* take a grip where the record's latest touch, its own_shape and hit, is a
 * player's on a handhold. the work's submode then takes the handhold's
 * number, 0 or 1, and SLOT the player's slot. returns whether a player
 * gripped */
static Bool harbor_missile_take_grip(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision = work->collision;
    int player;

    /* @hack spelling: player set inside the test, where a statement of its own
     * would do */
    if ((collision->flags & COLLISION_TOUCHED) &&
        collision->own_shape > BODY_SHAPE &&
        (player = player_slot_of(collision->hit->owner)) >= 0) {
        /* @hack spelling: work->collision read again, where collision would
         * do */
        work->submode = work->collision->own_shape - FIRST_HANDHOLD;
        SLOT(work) = player;
        return TRUE;
    }
    return FALSE;
}

/* draw seconds and hundredths, two digits each with a colon between, from
 * pos on the screen */
static void harbor_missile_countdown_draw_at(int seconds, int hundredths,
                                             NJS_POINT3 *pos)
{
    struct hud_number number;

    number.flags = HUD_NUMBER_ZEROS | HUD_NUMBER_CLAMPED | HUD_NUMBER_SCALED;
    number.place = 0;
    number.unk_02 = 0;
    number.max = COUNTDOWN_MAX;
    number.value = seconds;
    number.pos.x = pos->x;
    number.pos.y = pos->y;
    number.pos.z = 0.0F;
    number.scale = countdown_scale;
    number.color = countdown_color;
    njFogDisable();
    hud_number_draw(&number);
    number.pos.x += countdown_seconds_gap;
    number.max = 0;
    number.value = 0;
    number.flags = HUD_NUMBER_COLONS | HUD_NUMBER_SCALED;
    hud_number_draw(&number);
    number.pos.x += countdown_colon_gap;
    number.flags = HUD_NUMBER_ZEROS | HUD_NUMBER_CLAMPED | HUD_NUMBER_SCALED;
    number.max = COUNTDOWN_MAX;
    number.value = hundredths;
    hud_number_draw(&number);
    njFogEnable();
}

/* draw the count's steps left as seconds and hundredths. from the grip on, it
 * draws those left at the grip */
static void harbor_missile_countdown_draw(struct task *task)
{
    struct task_work *work = task->work;
    int steps;
    int seconds;
    int hundredths;
    /* @hack spelling: scaled, a Float local for the hundredths, where the
     * product converted in place would do */
    Float scaled;

    if (work->mode == HARBOR_MISSILE_MODE_COUNT) {
        steps = work->timer;
    } else {
        steps = MISSILE(task)->gripped_at;
    }
    steps = COUNT_STEPS - steps;
    if (steps < 0) {
        steps = 0;
    }
    if (steps > COUNT_STEPS) {
        steps = COUNT_STEPS;
    }
    seconds = steps / STEPS_PER_SECOND;
    hundredths = steps % STEPS_PER_SECOND;
    scaled = hundredths * HUNDREDTHS_PER_STEP;
    hundredths = scaled;
    harbor_missile_countdown_draw_at(seconds, hundredths, &countdown_pos);
}

/* start wave as a wave of height, at a random center. its rate is base, plus
 * up to range at random */
#define WAVE_START(wave, height, base, range)                                  \
    {                                                                          \
        (wave)->pos.x = ((rand() * RAND_SCALE) - 0.5F) * WAVE_SPREAD;          \
        (wave)->pos.y = ((rand() * RAND_SCALE) - 0.5F) * WAVE_SPREAD;          \
        (wave)->pos.z = (height);                                              \
        (wave)->rate = (int)(rand() * ((range) * RAND_SCALE)) + (base);        \
        (wave)->life =                                                         \
            (int)(rand() * (WAVE_LIFE_RANGE * RAND_SCALE)) + WAVE_LIFE;        \
    }
#define RISE(work, task)                                                       \
    {                                                                          \
        (work)->pos.y += MISSILE(task)->speed;                                 \
        MISSILE(task)->speed += RISE_ACCEL;                                    \
        if (MISSILE(task)->speed > harbor_missile_top_speed) {                 \
            MISSILE(task)->speed = harbor_missile_top_speed;                   \
        }                                                                      \
    }
/* set *out to the gripped handhold's center in world space */
#define HANDHOLD_POINT(work, out)                                              \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njUnitMatrix(NULL);                                                    \
        njTranslateV(NULL, &(work)->pos);                                      \
        njRotateY(NULL, (work)->ang.y);                                        \
        njCalcPoint(NULL, &HANDHOLD(work).center, (out));                      \
        njPopMatrix(1);                                                        \
    }
#define VOICE_STOP(task)                                                       \
    if (MISSILE(task)->voice != -1) {                                          \
        voice_stop(MISSILE(task)->voice);                                      \
        MISSILE(task)->voice = -1;                                             \
    }

/* @hack spelling: an inline function that walks the parts through pointers,
 * where three assignments would do */
#pragma inline(vector_difference)

static void vector_difference(NJS_VECTOR *out, NJS_VECTOR *minuend,
                              NJS_VECTOR *subtrahend)
{
    Float *pa = &minuend->x;
    Float *pb = &subtrahend->x;
    Float *pd = &out->x;

    *pd++ = *pa++ - *pb++;
    *pd++ = *pa++ - *pb++;
    *pd = *pa - *pb;
}

static void harbor_missile_step(struct task *task)
{
    struct task_work *work = task->work;
    struct sea_wave *wave;
    int phase;
    Float floor;
    Float power;
    Float speed;
    NJS_VECTOR vec;
    NJS_VECTOR dir;
    Float scale;
    Float length;
    Float distance;
    Float falloff;
    int heading;
    Sint16 turn;
    int player;
    int score;
    struct camera_slot *slot;
    struct harbor_missile_camera *camera;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- STARTRANGE(+100)";
    }
    if (work->mode == HARBOR_MISSILE_MODE_WAIT && placement_try_leave(task)) {
        return;
    }
    if (game_state.cleared) {
        return;
    }
    switch (work->mode) {
    case HARBOR_MISSILE_MODE_WAIT:
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((player = player_near(&work->pos, work->scl.x + START_MARGIN))) {
            work->mode = HARBOR_MISSILE_MODE_COUNT;
            work->timer = 0;
            SLOT(work) = player - 1;
            voice_play(VOICE_START, VOICE_START_PRIORITY);
        }
        break;
    case HARBOR_MISSILE_MODE_COUNT:
        if (harbor_missile_take_grip(task)) {
            MISSILE(task)->speed = 0.0F;
            score = 0;
            if (work->submode != UPPER_HANDHOLD) {
                if (work->timer < COUNT_FAST) {
                    score = GRIP_SCORE_FAST;
                } else if (work->timer < COUNT_SOON) {
                    score = GRIP_SCORE_SOON;
                } else if (work->timer < COUNT_LATE) {
                    score = GRIP_SCORE_LATE;
                }
            } else {
                score = GRIP_SCORE_UPPER;
            }
            if (score) {
                technique_score_start(score);
            }
            player_request(SLOT(work), PLAYER_REQUEST_ANIM, HOLD_ANIM);
            work->mode = HARBOR_MISSILE_MODE_GRIPPED;
            MISSILE(task)->gripped_at = work->timer;
            GRIP_STEPS(task) = 0;
            VOICE_STOP(task);
            voice_play(VOICE_GO, VOICE_PRIORITY);
        } else {
            if (work->timer == COUNT_HURRY) {
                VOICE_STOP(task);
                MISSILE(task)->voice = voice_play(VOICE_HURRY, VOICE_PRIORITY);
            }
            work->timer++;
            if (work->timer % WAVE_WAIT_STEPS == 0 &&
                rand() * RAND_SCALE < WAVE_CHANCE &&
                sea_wave_count < WAVES_MAX) {
                wave = sea_wave_add();
                if (wave) {
                    WAVE_START(wave, WAVE_HEIGHT, WAVE_RATE, WAVE_RATE_RANGE);
                }
            }
            if (work->timer > COUNT_STEPS) {
                VOICE_STOP(task);
                voice_play(VOICE_GO, VOICE_PRIORITY);
                work->mode = HARBOR_MISSILE_MODE_EMPTY;
                work->timer = 0;
                MISSILE(task)->speed = 0.0F;
            }
        }
        break;
    case HARBOR_MISSILE_MODE_GRIPPED:
        work->timer++;
        if (work->timer % WAVE_WAIT_STEPS == 0 &&
            rand() * RAND_SCALE < WAVE_CHANCE && sea_wave_count < WAVES_MAX) {
            wave = sea_wave_add();
            if (wave) {
                WAVE_START(wave, WAVE_HEIGHT, WAVE_RATE, WAVE_RATE_RANGE);
            }
        }
        HANDHOLD_POINT(work, &vec);
        vec.y -= HANG_DROP;
        player_set_pos(SLOT(work), vec.x, vec.y, vec.z);
        player_work[SLOT(work)]->ang.y =
            -work->ang.y - HANDHOLD(work).ang.y + HALF_TURN;
        player_work[SLOT(work)]->ang.z = 0;
        player_work[SLOT(work)]->ang.x = 0;
        if (GRIP_STEPS(task) > GRIP_WAIT || work->timer > COUNT_STEPS) {
            /* @hack redundant: the case's mode tested again */
            if (work->mode == HARBOR_MISSILE_MODE_GRIPPED) {
                music_fade_out();
                work->mode = HARBOR_MISSILE_MODE_LIFT_OFF;
                player_request(SLOT(work), PLAYER_REQUEST_ANIM, HOLD_ANIM);
            }
            work->timer = 0;
            MISSILE(task)->speed = 0.0F;
        }
        break;
    case HARBOR_MISSILE_MODE_EMPTY:
        RISE(work, task);
        if (work->timer > EMPTY_STEPS) {
            work->mode = HARBOR_MISSILE_MODE_END;
            work->timer = 0;
        } else {
            sound_play_for(SOUND_ROAR, work, SOUND_PRIORITY, SOUND_VOLUME,
                           SOUND_UPDATES);
            if (rand() * RAND_SCALE < BLAST_CHANCE) {
                sound_play(
                    ((int)(rand() * (BLAST_ROLL * RAND_SCALE)) % SOUND_BLASTS) +
                        SOUND_BLAST,
                    NULL, 0, 0);
            }
            if (work->timer == MISS_STEP) {
                player = player_nearest(&work->pos);
                if (kill_areas_task->child == NULL &&
                    !(player_records[player]->flags & PLAYER_MISSED)) {
                    player_miss(player);
                }
            }
            work->timer++;
        }
        break;
    case HARBOR_MISSILE_MODE_LIFT_OFF:
        if (harbor_missile_debug_view == 0) {
            RISE(work, task);
        }
        HANDHOLD_POINT(work, &vec);
        if (work->timer < LAUNCH_STEPS) {
            sound_play_for(SOUND_ROAR, work, SOUND_PRIORITY, SOUND_VOLUME,
                           SOUND_UPDATES);
            if (rand() * RAND_SCALE < BLAST_CHANCE) {
                sound_play(
                    ((int)(rand() * (BLAST_ROLL * RAND_SCALE)) % SOUND_BLASTS) +
                        SOUND_BLAST,
                    NULL, 0, 0);
            }
        } else {
            /* @hack redundant: the roar's point, between the handhold and
             * pos, stored in dir through mid, where nothing reads it */
            NJS_VECTOR *mid = &dir;

            *mid = vec;
            mid->y = (vec.y + work->pos.y) * 0.5F;
            sound_play_for(SOUND_ROAR, work, SOUND_PRIORITY, SOUND_VOLUME,
                           SOUND_UPDATES);
        }
        vec.y -= HANG_DROP;
        player_set_pos(SLOT(work), vec.x, vec.y, vec.z);
        player_work[SLOT(work)]->ang.y =
            -work->ang.y - HANDHOLD(work).ang.y + HALF_TURN;
        player_work[SLOT(work)]->ang.z = 0;
        player_work[SLOT(work)]->ang.x = 0;
        if (vec.y > throw_target.y + THROW_HEIGHT ||
            (vec.y > JUMP_OFF_HEIGHT &&
             (player_pad_records[SLOT(work)].press & PDD_DGT_TA))) {
            VECTOR_DIFFERENCE(&dir, &throw_target, &vec);
            dir.y = 0.0F;
            distance = njScalor(&dir);
            VECTOR_SCALE(&dir, throw_speed / distance);
            dir.y = MISSILE(task)->speed;
            speed = njUnitVector(&dir);
            player_request(SLOT(work), PLAYER_REQUEST_RELEASE, 0);
            player_fling(SLOT(work), speed, &dir);
            if (MISSILE(task)->camera >= 0) {
                camera_remove_mode(SLOT(work), MISSILE(task)->camera);
                MISSILE(task)->camera = -1;
            }
            work->mode = HARBOR_MISSILE_MODE_THROWN;
        } else {
            work->timer++;
        }
        break;
    case HARBOR_MISSILE_MODE_THROWN:
        RISE(work, task);
        HANDHOLD_POINT(work, &vec);
        dir = vec;
        dir.y = (vec.y + work->pos.y) * 0.5F;
        sound_play_at_for(SOUND_ROAR, work, SOUND_PRIORITY, roar_volume,
                          SOUND_UPDATES, &dir);
        vec.y -= HANG_DROP;
        if (vec.y > throw_target.y + ENDING_HEIGHT) {
            work->mode = HARBOR_MISSILE_MODE_END;
            work->timer = 0;
        } else {
            work->timer++;
        }
        break;
    case HARBOR_MISSILE_MODE_END:
        if (!work->timer) {
            music_stop();
            music_fade_in(VOLUME_FULL);
            music_play_name("m_harb2.adx");
        }
        if (work->timer++ > END_STEPS) {
            task_kill(task);
        }
        break;
    }
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_EMPTY) {
        phase = work->timer;
        if (work->timer < LAUNCH_STEPS) {
            phase *= LAUNCH_SHAKE_PACE;
        }
        vec.x =
            harbor_missile_wave_sum_outside(launch_shake_x, SHAKE_WAVES, phase);
        vec.y =
            harbor_missile_wave_sum_outside(launch_shake_y, SHAKE_WAVES, phase);
        vec.z =
            harbor_missile_wave_sum_outside(launch_shake_z, SHAKE_WAVES, phase);
        if (work->timer < LAUNCH_STEPS) {
            VECTOR_SCALE(&vec, LAUNCH_SHAKE_SCALE);
        }
        camera_set_shake(SLOT(work), &vec);
        /* @hack type: game_state's steps read as game_state_steps, where
         * game_state.steps would do */
        if (game_state_steps % SHAKE_STEPS == 0) {
            distance =
                njDistanceP2P(&camera_views[0]->pos, &player_work[0]->pos);
            falloff = (distance - SHAKE_NEAR) * SHAKE_FALLOFF;
            if (falloff > 1.0F) {
                falloff = 1.0F;
            }
            if (falloff < 0.0F) {
                falloff = 0.0F;
            }
            power = (1.0F - falloff) * SHAKE_POWER;
            if (power > SHAKE_POWER_MIN &&
                camera_views[0]->pos.y + SHAKE_ABOVE > player_work[0]->pos.y) {
                vibration_start(
                    0, power,
                    (int)(rand() * (SHAKE_FREQ_RANGE * RAND_SCALE)) +
                        SHAKE_FREQ,
                    SHAKE_TIME);
            }
        }
    }
    if (work->mode == HARBOR_MISSILE_MODE_GRIPPED ||
        work->mode == HARBOR_MISSILE_MODE_COUNT) {
        phase = work->timer;
        vec.x =
            harbor_missile_wave_sum_outside(wait_shake_x, SHAKE_WAVES, phase);
        vec.y =
            harbor_missile_wave_sum_outside(wait_shake_y, SHAKE_WAVES, phase);
        vec.z =
            harbor_missile_wave_sum_outside(wait_shake_z, SHAKE_WAVES, phase);
        camera_set_shake(SLOT(work), &vec);
        sound_play_for(SOUND_RUMBLE, task, SOUND_PRIORITY, SOUND_VOLUME,
                       SOUND_UPDATES);
        /* @hack type: game_state's steps read as game_state_steps, where
         * game_state.steps would do */
        if (game_state_steps % WAIT_SHAKE_STEPS == 0) {
            vibration_start(0, WAIT_SHAKE_POWER,
                            (int)(rand() * (SHAKE_FREQ_RANGE * RAND_SCALE)) +
                                WAIT_SHAKE_FREQ,
                            SHAKE_TIME);
        }
    }
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_GRIPPED) {
        GRIP_STEPS(task)++;
    }
    if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
        work->mode == HARBOR_MISSILE_MODE_EMPTY ||
        work->mode == HARBOR_MISSILE_MODE_THROWN) {
        vec.x = ((rand() * RAND_SCALE) - 0.5F) * puff_spread_xz;
        vec.z = ((rand() * RAND_SCALE) - 0.5F) * puff_spread_xz;
        vec.y = (puff_spread_y * ((rand() * RAND_SCALE) - 0.5F)) - puff_fall +
                (MISSILE(task)->speed * PUFF_SPEED_SHARE);
        dir = work->pos;
        dir.y += PUFF_RISE;
        floor = MISSILE(task)->ground_y + PUFF_FLOOR_RISE;
        if (puff_floor_min > floor) {
            floor = puff_floor_min;
        }
        if (work->timer < LAUNCH_STEPS) {
            rocket_thrust_settle_wide_create(
                &dir, &vec,
                puff_scale_min + (puff_scale_range * rand() * RAND_SCALE),
                floor);
        } else if (rand() * RAND_SCALE < PUFF_CHANCE) {
            rocket_thrust_settle_create(
                &dir, &vec,
                puff_scale_min + (puff_scale_range * rand() * RAND_SCALE),
                floor);
        }
        if (work->timer == BREAK_STEP) {
            VECTOR_DIFFERENCE(&vec, &circle_pos, &work->pos);
            vec.y = 0.0F;
            length = njScalor(&vec);
            scale = BREAK_SPEED / length;
            vec.x *= scale;
            vec.y *= scale;
            vec.z *= scale;
            vec.y += BREAK_LIFT;
            debris_break_sized(
                &work->pos, work->ang.y, harbor_missile_parts, PARTS, 0.0F,
                BREAK_SPREAD, BREAK_FLOOR, &vec,
                (work->mode == HARBOR_MISSILE_MODE_EMPTY ? 0 : DEBRIS_TRAIL) |
                    DEBRIS_UNPLACED | DEBRIS_THROUGH_FLOOR,
                BREAK_RADIUS);
        }
        if (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
            work->mode == HARBOR_MISSILE_MODE_THROWN) {
            if (work->timer > HARBOR_MISSILE_CLOUD_STEPS) {
                if (MISSILE(task)->clouds == NULL) {
                    clouds_create(0, &work->pos, &MISSILE(task)->clouds);
                }
                /* @hack redundant: the cloud test made again, which the
                 * test above has passed already */
                if (work->timer > HARBOR_MISSILE_CLOUD_STEPS) {
                    njSetBackColor(sky_color_0, sky_color_1, sky_color_2);
                    if (MISSILE(task)->camera == -1) {
                        MISSILE(task)->camera = camera_top(SLOT(work)) + 1;
                        slot = camera_set_user_mode(
                            SLOT(work), MISSILE(task)->camera,
                            work->mode == HARBOR_MISSILE_MODE_THROWN
                                ? harbor_missile_camera_thrown
                                : harbor_missile_camera_lift);
                        if (slot) {
                            camera = CAMERA(slot);
                            camera->steps = 0;
                            camera->pos = work->pos;
                            camera_set_adjust(SLOT(work), MISSILE(task)->camera,
                                              CAMERA_ADJUST_NONE);
                        } else {
                            MISSILE(task)->camera = -1;
                        }
                    }
                    vector_difference(&vec, &throw_target, &work->pos);
                    heading = (Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN) -
                              HANDHOLD(work).ang.y;
                    turn = heading - work->ang.y;
                    if (turn > TURN_MAX) {
                        turn = TURN_MAX;
                    }
                    if (turn < -TURN_MAX) {
                        turn = -TURN_MAX;
                    }
                    work->ang.y += turn;
                }
            }
        }
        if (work->timer < LAUNCH_STEPS) {
            if (MISSILE(task)->camera == -1) {
                MISSILE(task)->camera = camera_top(SLOT(work)) + 1;
                slot = camera_set_user_mode(SLOT(work), MISSILE(task)->camera,
                                            harbor_missile_camera_circle);
                if (slot) {
                    camera = CAMERA(slot);
                    camera->steps = 0;
                    camera_set_adjust(SLOT(work), MISSILE(task)->camera,
                                      CAMERA_ADJUST_NONE);
                } else {
                    MISSILE(task)->camera = -1;
                }
            }
        } else if (work->timer == LAUNCH_STEPS) {
            if (MISSILE(task)->camera >= 0) {
                camera_remove_mode(SLOT(work), MISSILE(task)->camera);
                MISSILE(task)->camera = -1;
            }
        }
        if ((work->timer > WAVE_STEADY_STEPS ||
             rand() * RAND_SCALE < WAVE_CHANCE) &&
            sea_wave_count < WAVES_MAX &&
            game_state.steps % WAVE_FLIGHT_STEPS == 0) {
            wave = sea_wave_add();
            if (wave) {
                WAVE_START(wave, WAVE_FLIGHT_HEIGHT, WAVE_FLIGHT_RATE,
                           WAVE_FLIGHT_RATE_RANGE);
            }
        }
    }
    if (work->mode != HARBOR_MISSILE_MODE_LIFT_OFF &&
        work->mode != HARBOR_MISSILE_MODE_THROWN) {
        collision_register(task);
    }
}

static void harbor_missile_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &harbor_missile_object;
    NJS_COLOR color;
    int alpha;
    Float fade;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    if ((work->mode == HARBOR_MISSILE_MODE_LIFT_OFF ||
         work->mode == HARBOR_MISSILE_MODE_EMPTY ||
         work->mode == HARBOR_MISSILE_MODE_THROWN) &&
        work->timer > LAUNCH_STEPS) {
        njSetTexture(&harbor_missile_flying_texlist);
        texture_anim_apply(harbor_missile_flying_object->model,
                           &harbor_missile_flame_anim, game_state.steps);
        njCnkSimpleDrawObject(harbor_missile_flying_object);
    } else {
        njSetTexture(&harbor_missile_texlist);
        njCnkSimpleDrawObject(object);
    }
    njPopMatrix(1);
    if (work->mode == HARBOR_MISSILE_MODE_EMPTY) {
        fade = work->timer * (1.0F / LAUNCH_STEPS);
        alpha = fade * 255.0F;
        if (alpha < 0) {
            alpha = 0;
        }
        if (alpha > 255) {
            alpha = 255;
        }
        color.argb.a = alpha;
        color.argb.r = 255;
        color.argb.g = 255;
        color.argb.b = 255;
        screen_fill(&color);
    }
    if (work->mode == HARBOR_MISSILE_MODE_GRIPPED ||
        work->mode == HARBOR_MISSILE_MODE_COUNT ||
        (work->mode == HARBOR_MISSILE_MODE_LIFT_OFF &&
         GRIP_STEPS(task) < BLINK_END &&
         GRIP_STEPS(task) % BLINK_STEPS < BLINK_SHOWN)) {
        harbor_missile_countdown_draw(task);
    }
}
