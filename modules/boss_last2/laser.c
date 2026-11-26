#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "boss_last2/attack_art.h"
#include "boss_last2/arena.h"
#include "boss_last2/body.h"
#include "boss_last2/laser.h"
#include "boss_last2/panels.h"
#include "boss_last2/stage.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "player/player.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_sphere.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/vector.h"

/* a beam's modes, in its work's mode */
enum laser_mode {
    LASER_EXTEND,
    LASER_HOLD,
    LASER_TRACK,
    LASER_RETRACT,
    LASER_WAIT
};
/* bits of a beam's work flags. LASER_STOP has the beam fire no more once it
 * has drawn back, and LASER_STOPPED says it has stopped */
#define LASER_STOP 0x100
#define LASER_STOPPED 0x200
/* each step a beam plays its sounds anew, each for SOUND_DURATION. the
 * charge plays at CHARGE_VOLUME, but at HELD_CHARGE_VOLUME while the beam
 * holds or tracks. the beam's sound plays at BEAM_VOLUME, and its hum at
 * HUM_VOLUME. the hum of a beam at y = 0 or above plays at
 * UPPER_HUM_PITCH */
#define SOUND_DURATION 5
#define CHARGE_VOLUME 120
#define HELD_CHARGE_VOLUME 60
#define BEAM_VOLUME 120
#define HUM_VOLUME 10
#define UPPER_HUM_PITCH 0x3000
/* the length of each piece of a beam, a cap or a link, at a scale of 1. a
 * beam of CAPS pieces is its two caps alone, which each take CAP_SHARE of its
 * length. it draws back to wait once each is down to LEAST_CAP of a piece */
#define PIECE_LENGTH 78.5F
#define CAPS 2
#define CAP_SHARE 0.5F
#define LEAST_CAP 0.1F
/* a pulse holds at its reach for HOLD_STEPS. a beam that has drawn back
 * fires again after WAIT_STEPS */
#define HOLD_STEPS 18
#define WAIT_STEPS 30
/* the least x laser_aim_point and laser_limit_height count a point at */
#define MIN_X (-300.0F)
/* laser_aim_point's aim lies halfway from the player to the target while
 * the two are less than NEAR_TARGET apart. out to FAR_TARGET apart, it lies
 * AIM_SPAN from the player. farther apart, it lies NEAR_TARGET short of the
 * target */
#define NEAR_TARGET 400.0F
#define FAR_TARGET 600.0F
#define AIM_SPAN 200.0F
/* laser_limit_height's plane rises at AIM_SLOPE toward +x. it holds an aim
 * only within LIMIT_DEPTH of z = 0 */
#define AIM_SLOPE 0x1555
#define LIMIT_DEPTH 600.0F
/* a sweep's aim circles the beam's line, about a center SWEEP_RADIUS off the
 * point laser_aim_point gives */
#define SWEEP_RADIUS 50.0F
/* while the Finalhazard is stunned, a beam's sweep and tracking speeds keep
 * STUN_DRAG of themselves each step */
#define STUN_DRAG 0.8F
/* a beam pushes its player within BEAM_RADIUS of it off at
 * FINALHAZARD_PUSH_SPEED */
#define BEAM_RADIUS 15.0F

/* a beam, in its task's extra */
struct laser {
    Sint8 player; /* the player it aims at, active as the attack began */
    Sint8 kind; /* a FINALHAZARD_LASER_ kind */
    /* the pieces drawn from start to end: the start cap, the links and the
     * end cap. each piece is PIECE_LENGTH long, but the last link is the
     * work's scl.x of that. where the caps are alone, each is scl.x of a
     * piece */
    Sint8 pieces;
    Float length; /* from start to end */
    Float reach; /* the length it extends to */
    /* the most aim moves toward the sweep in a step, which grows to
     * tracking_limit */
    Float tracking_speed;
    Float unused_10;
    Angle spin; /* the caps' rotors' turn */
    Angle sweep; /* aim's place on the sweep's circle */
    Angle sweep_speed;
    const NJS_POINT3 *target; /* the attack's target's pos */
    NJS_POINT3 aim; /* the point the beam faces */
    NJS_POINT3 sweep_center;
    /* the active player's pos, last taken in play while the Finalhazard
     * wasn't stunned */
    NJS_POINT3 player_position;
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_POINT3 nearest; /* the point nearest player, where its hum plays */
};

/* the holder's work: a task_work with count for submode, and the beams for
 * ang.x and ang.y */
struct laser_pair {
    Sint8 mode;
    Sint8 count;
    Uint8 unused[6];
    struct task *beams[2];
};
#define PAIR(task) ((struct laser_pair *)(task)->work)

static const NJS_VECTOR extension_axis = {0, 0, 0};
static const NJS_VECTOR retraction_axis = {0, 0, 0};
static const NJS_VECTOR tracking_axis = {0, 0, 0};
static const NJS_VECTOR debug_scale = {10, 10, 10};
static const NJS_VECTOR link_axis = {0, 0, 0};

/* the caps' and the links' uvs, laid out as struct texture_anim says, with a
 * key each. the start cap and the links scroll by SCROLL, and the end cap by
 * SCROLL_BACK. nothing reads the records' frames */
static Sint16 cap_inner_uvs[180] = {
#include "boss_last2/laser/cap_inner_uvs.inc"
};
static struct texture_anim cap_inner_scroll = {
#include "boss_last2/laser/cap_inner_scroll.inc"
};

static Sint16 cap_outer_uvs[156] = {
#include "boss_last2/laser/cap_outer_uvs.inc"
};
static struct texture_anim cap_outer_scroll = {
#include "boss_last2/laser/cap_outer_scroll.inc"
};

static Sint16 link_inner_uvs[40] = {
#include "boss_last2/laser/link_inner_uvs.inc"
};
static struct texture_anim link_inner_scroll = {
#include "boss_last2/laser/link_inner_scroll.inc"
};

static Sint16 link_outer_uvs[40] = {
#include "boss_last2/laser/link_outer_uvs.inc"
};
static struct texture_anim link_outer_scroll = {
#include "boss_last2/laser/link_outer_scroll.inc"
};

static Float extension_speed = 30.0F;
/* the top of a beam's tracking_speed, which the test menu sets */
static Float tracking_limit = 2.8F;
static Float tracking_accel = 0.1F;
static Float beam_reach = 1570.0F;
static Sint16 scroll_speed = 27;
static Sint16 unused_scroll = 0;
static NJS_POINT3 idle_aim = {0, 0, 400};
static Angle spin_step = 0x800;
/* @hack type: the sweep's limit and step Uint32, compared unsigned with
 * sweep_speed, where Angles compared signed would do: sweep_speed is never
 * below 0 */
static Uint32 sweep_limit = 0x140;
static Uint32 sweep_accel = 0x20;
static NJS_POLYGON_VTX debug_plane[8] = {
#include "boss_last2/laser/debug_plane.inc"
};
static Angle plane_angle = 0x1555;
static Float plane_height = 40.0F;
static Float plane_depth = 100.0F;
static Sint32 debug_plane_visible = 0;
/* the settings the test menu's lasers page shows: tracking_limit alone */
enum laser_setting { CANNON_SPEED };
static Sint8 selection = CANNON_SPEED;
/* @hack spelling: settings, a copy of the settings' count that the page
 * reads, where a constant would do */
static Sint8 settings = 1;
static Sint8 repeat_delay;

/* the uvs' scroll, which moves scroll_speed a step and wraps at
 * SCROLL_MASK + 1. SCROLL_BACK runs the other way */
#define SCROLL_MASK 0x1fff
#define SCROLL ((scroll_speed * game_state.steps) & SCROLL_MASK)
#define SCROLL_BACK (-(scroll_speed * game_state.steps) & SCROLL_MASK)

static void laser_draw_start(Angle spin)
{
    /* @hack spelling: model, one local for the two models in turn, where
     * each model's address would do */
    NJS_CNK_MODEL *model = &laser_cap_outer;
    texture_anim_scroll(model, &cap_outer_scroll, 0, SCROLL);
    njCnkSimpleDrawModel(model);
    njPushMatrixEx();
    njRotateX(NULL, spin);
    njCnkSimpleDrawModel(&laser_cap_rotor);
    njPopMatrixEx();
    model = &laser_cap_inner;
    texture_anim_scroll(model, &cap_inner_scroll, 0, SCROLL);
    njCnkSimpleDrawModel(model);
}

static void laser_draw_end(Angle spin)
{
    /* @hack spelling: model, one local for the two models in turn, where
     * each model's address would do */
    NJS_CNK_MODEL *model = &laser_cap_outer;
    texture_anim_scroll(model, &cap_outer_scroll, 0, SCROLL_BACK);
    njCnkSimpleDrawModel(model);
    njPushMatrixEx();
    njRotateX(NULL, spin);
    njCnkSimpleDrawModel(&laser_cap_rotor);
    njPopMatrixEx();
    model = &laser_cap_inner;
    texture_anim_scroll(model, &cap_inner_scroll, 0, SCROLL_BACK);
    njCnkSimpleDrawModel(model);
}

static void laser_draw_link(void)
{
    /* @hack spelling: model, one local for the two models in turn, where
     * each model's address would do */
    NJS_CNK_MODEL *model = &laser_link_outer;
    texture_anim_scroll(model, &link_outer_scroll, 0, SCROLL);
    njCnkSimpleDrawModel(model);
    model = &laser_link_inner;
    texture_anim_scroll(model, &link_inner_scroll, 0, SCROLL);
    njCnkSimpleDrawModel(model);
}

/* set aim on the way from player to target, as NEAR_TARGET says. player
 * counts as held to MIN_X or more in x. it also counts as held to the active
 * player's half of the arena. player 0's half has z of 0 or more, and player
 * 1's of 0 or less */
static void laser_aim_point(const NJS_POINT3 *target, const NJS_POINT3 *player,
                            NJS_POINT3 *aim)
{
    NJS_POINT3 from;
    NJS_VECTOR offset;
    Float distance;
    if (!target) {
        return;
    }
    from = *player;
    if (finalhazard_players->active == 0) {
        if (player->z < 0.0F) {
            from.z = 0.0F;
        }
    } else {
        if (player->z > 0.0F) {
            from.z = 0.0F;
        }
    }
    if (player->x < MIN_X) {
        from.x = MIN_X;
    }
    vector_diff(target, &from, &offset);
    distance = njScalor(&offset);
    if (distance > FAR_TARGET) {
        /* @hack spelling: -NEAR_TARGET added, where NEAR_TARGET taken away
         * would do */
        distance += -NEAR_TARGET;
    } else if (distance < NEAR_TARGET) {
        distance *= 0.5F;
    } else {
        distance = AIM_SPAN;
    }
    last_boss_vector_length(&offset, distance, &offset);
    vector_sum(&from, &offset, aim);
}

/* hold aim on origin's side of a plane, which holds the z axis. it stays
 * above for an origin above y = 0, else below. the plane's height counts x
 * from MIN_X at least */
static void laser_limit_height(const NJS_POINT3 *origin, NJS_POINT3 *aim)
{
    Float x;
    Float height;
    if (_builtin_fabsf(aim->z) > LIMIT_DEPTH) {
        return;
    }
    if (aim->x > MIN_X) {
        x = aim->x;
    } else {
        x = MIN_X;
    }
    height = njTan(AIM_SLOPE) * x;
    if (origin->y > 0.0F) {
        if (height > aim->y) {
            aim->y = height;
        }
    } else {
        if (aim->y > height) {
            aim->y = height;
        }
    }
}

static void laser_face(const NJS_POINT3 *origin, const NJS_POINT3 *aim,
                       struct angles *facing)
{
    NJS_VECTOR delta;
    Float distance;
    vector_diff(aim, origin, &delta);
    distance = njScalor(&delta);
    if (distance != 0.0F) {
        facing->y = atan2f(delta.x, delta.z) * ANGLE_PER_RADIAN;
        facing->x = asinf(-(delta.y * njInvertSqrt(distance) *
                            njInvertSqrt(distance))) *
                    ANGLE_PER_RADIAN;
    }
}

/* pan the hum of a beam below y = 0 to the center. raise that of any
 * other */
#define TUNE_HUM(task, work)                                                   \
    do {                                                                       \
        if ((work)->pos.y < 0.0F) {                                            \
            sound_set_pan((task), SOUND_LASER_HUM, 0);                         \
        } else {                                                               \
            sound_set_pitch((task), SOUND_LASER_HUM, UPPER_HUM_PITCH);         \
        }                                                                      \
    } while (0)
#define LASER_HUM(task, work, beam)                                            \
    do {                                                                       \
        sound_play_at_for(SOUND_LASER_HUM, (task), 1, HUM_VOLUME,              \
                          SOUND_DURATION, &(beam)->nearest);                   \
        TUNE_HUM(task, work);                                                  \
    } while (0)
#define IN_PIECES(length)                                                      \
    ((length) * njInvertSqrt(PIECE_LENGTH) * njInvertSqrt(PIECE_LENGTH))
#define LAST_LINK_SCALE(beam)                                                  \
    IN_PIECES((beam)->length - (((beam)->pieces - 1) * PIECE_LENGTH))
/* turn v from a beam's own frame, along whose -x the beam runs, to the
 * world's */
#define TURN_BEAM_VECTOR(work, v)                                              \
    do {                                                                       \
        matrix_push_unit();                                                    \
        njRotateY(NULL, (work)->ang.y);                                        \
        njRotateX(NULL, (work)->ang.x);                                        \
        njRotateY(NULL, QUARTER_TURN);                                         \
        njCalcVector(NULL, &(v), &(v));                                        \
        njPopMatrix(1);                                                        \
    } while (0)

static void laser_extend(struct task *task)
{
    struct task_work *work = task->work;
    struct laser *beam = task->extra.p;
    NJS_VECTOR tip;
    sound_play_at_for(SOUND_LASER_CHARGE, task, 1, CHARGE_VOLUME,
                      SOUND_DURATION, &work->pos);
    LASER_HUM(task, work, beam);
    sound_play_at_for(SOUND_LASER_BEAM, task, 1, BEAM_VOLUME, SOUND_DURATION,
                      &work->pos);
    TUNE_HUM(task, work);
    beam->length += extension_speed;
    if (beam->pieces == CAPS) {
        work->scl.x = IN_PIECES(beam->length) * CAP_SHARE;
        if (work->scl.x >= 1.0F) {
            beam->pieces++;
            /* @hack spelling: -1.0F added, where 1.0F taken away would do */
            work->scl.x += -1.0F;
        }
    } else {
        work->scl.x += IN_PIECES(extension_speed);
        if (beam->length >= beam->reach) {
            beam->length = beam->reach;
            work->scl.x = LAST_LINK_SCALE(beam);
            if (beam->kind == FINALHAZARD_LASER_PULSE) {
                work->timer = HOLD_STEPS;
                work->mode = LASER_HOLD;
            } else {
                beam->sweep = 0;
                beam->sweep_speed = 0;
                beam->sweep_center = beam->aim;
                work->mode = LASER_TRACK;
            }
        } else if (beam->length > beam->pieces * PIECE_LENGTH) {
            beam->pieces++;
            work->scl.x = LAST_LINK_SCALE(beam);
        }
    }
    tip = extension_axis;
    tip.x = -beam->length;
    TURN_BEAM_VECTOR(work, tip);
    vector_sum(&work->pos, &tip, &beam->end);
}

static void laser_hold(struct task *task)
{
    struct laser *beam = task->extra.p;
    struct task_work *work = task->work;
    LASER_HUM(task, work, beam);
    sound_play_at_for(SOUND_LASER_BEAM, task, 1, BEAM_VOLUME, SOUND_DURATION,
                      &work->pos);
    TUNE_HUM(task, work);
    sound_play_at_for(SOUND_LASER_CHARGE, task, 1, HELD_CHARGE_VOLUME,
                      SOUND_DURATION, &work->pos);
    if (--work->timer == 0) {
        if (beam->kind == FINALHAZARD_LASER_PULSE) {
            work->mode = LASER_RETRACT;
        } else {
            work->mode = LASER_TRACK;
            beam->sweep = 0;
            beam->sweep_speed = 0;
            beam->sweep_center = beam->aim;
        }
    }
}

static void laser_retract(struct task *task)
{
    struct laser *beam = task->extra.p;
    struct task_work *work = task->work;
    NJS_VECTOR offset;
    LASER_HUM(task, work, beam);
    sound_play_at_for(SOUND_LASER_BEAM, task, 1, BEAM_VOLUME, SOUND_DURATION,
                      &beam->start);
    TUNE_HUM(task, work);
    beam->length -= extension_speed;
    if (beam->pieces == CAPS) {
        work->scl.x = IN_PIECES(beam->length) * CAP_SHARE;
        if (work->scl.x <= LEAST_CAP) {
            work->timer = WAIT_STEPS;
            work->scl.x = 0.0F;
            beam->length = 0.0F;
            work->mode = LASER_WAIT;
            work->submode = 0;
            if (work->flags & LASER_STOP) {
                work->flags |= LASER_STOPPED;
            }
        }
    } else {
        work->scl.x -= IN_PIECES(extension_speed);
        if (beam->length <= (beam->pieces - 1) * PIECE_LENGTH) {
            beam->pieces--;
            if (beam->pieces == CAPS) {
                work->scl.x = IN_PIECES(beam->length) * CAP_SHARE;
            } else {
                work->scl.x = LAST_LINK_SCALE(beam);
            }
        }
    }
    offset = retraction_axis;
    offset.x = -beam_reach + beam->length;
    TURN_BEAM_VECTOR(work, offset);
    vector_sum(&work->pos, &offset, &beam->start);
}

static void laser_wait(struct task *task)
{
    struct task_work *work = task->work;
    struct laser *beam = task->extra.p;
    if (!(work->flags & LASER_STOPPED)) {
        sound_play_at_for(SOUND_LASER_CHARGE, task, 1, CHARGE_VOLUME,
                          SOUND_DURATION, &work->pos);
    }
    if (finalhazard_players->transition != SWITCH_PLAY) {
        return;
    }
    if (FINALHAZARD_STUNNED() == 1) {
        return;
    }
    if (!(work->flags & LASER_STOP)) {
        if (--work->timer == 0) {
            laser_aim_point(beam->target, &player_work[beam->player]->pos,
                            &beam->aim);
            laser_limit_height(&work->pos, &beam->aim);
            laser_face(&work->pos, &beam->aim, &work->ang);
            beam->start = work->pos;
            beam->end = work->pos;
            work->mode = LASER_EXTEND;
            work->submode = 0;
        }
    } else {
        work->flags |= LASER_STOPPED;
    }
}

static void laser_track(struct task *task)
{
    struct laser *beam = task->extra.p;
    struct task_work *work = task->work;
    /* @hack redundant: three vectors of the frame that nothing reads or
     * writes */
    NJS_VECTOR unused[3];
    NJS_POINT3 center;
    NJS_POINT3 start;
    NJS_POINT3 point;
    NJS_VECTOR axis;
    NJS_VECTOR offset;
    /* @hack spelling: turn and speed, copies of beam's sweep and
     * tracking_speed, where the fields would do */
    Angle turn;
    Float speed;
    Float distance;

    LASER_HUM(task, work, beam);
    sound_play_at_for(SOUND_LASER_CHARGE, task, 1, HELD_CHARGE_VOLUME,
                      SOUND_DURATION, &work->pos);
    if (FINALHAZARD_STUNNED() == 0) {
        laser_aim_point(beam->target, &beam->player_position,
                        &beam->sweep_center);
        laser_limit_height(&work->pos, &beam->sweep_center);
    }
    center = beam->sweep_center;
    if (work->pos.y > beam->target->y) {
        center.y += SWEEP_RADIUS;
    } else {
        center.y -= SWEEP_RADIUS;
    }
    start = beam->start;
    axis.x = center.x - start.x;
    axis.y = center.y - start.y;
    axis.z = center.z - start.z;
    njUnitVector(&axis);
    point.x = 0.0F;
    if (work->pos.y > 0.0F) {
        point.y = -SWEEP_RADIUS;
    } else {
        point.y = SWEEP_RADIUS;
    }
    point.z = 0.0F;
    turn = beam->sweep;
    matrix_push_unit();
    njTranslateEx(&center);
    njRotate(NULL, &axis, turn);
    njCalcPoint(NULL, &point, &point);
    njPopMatrixEx();
    distance = vector_distance(&beam->aim, &point);
    if (distance > beam->tracking_speed) {
        speed = beam->tracking_speed;
        vector_diff(&point, &beam->aim, &offset);
        last_boss_vector_length(&offset, speed, &offset);
        njAddVector(&beam->aim, &offset);
    } else {
        beam->aim = point;
    }
    laser_limit_height(&work->pos, &beam->aim);
    laser_face(&work->pos, &beam->aim, &work->ang);
    center = tracking_axis;
    center.x = -beam->length;
    TURN_BEAM_VECTOR(work, center);
    vector_sum(&work->pos, &center, &beam->end);
    if ((Uint32)beam->sweep_speed < sweep_limit) {
        beam->sweep_speed += sweep_accel;
        if ((Uint32)beam->sweep_speed > sweep_limit) {
            beam->sweep_speed = sweep_limit;
        }
    }
    beam->sweep += beam->sweep_speed;
    if (beam->sweep > FULL_TURN) {
        beam->sweep -= FULL_TURN;
    }
    if (work->flags & LASER_STOP) {
        work->mode = LASER_RETRACT;
    }
}

static void laser_contact(struct task *task)
{
    struct laser *beam = task->extra.p;
    NJS_VECTOR direction;
    NJS_LINE axis;
    NJS_POINT3 closest;
    Float distance;
    Float length;
    Float from_start;
    Float from_end;

    if (beam->length == 0.0F) {
        return;
    }
    if (player_work[finalhazard_players->active]->flags & WORK_HURT) {
        return;
    }
    vector_diff(&beam->end, &beam->start, &direction);
    axis.px = beam->start.x;
    axis.py = beam->start.y;
    axis.pz = beam->start.z;
    axis.vx = direction.x;
    axis.vy = direction.y;
    axis.vz = direction.z;
    distance = njDistanceP2L(&player_work[beam->player]->pos, &axis, &closest);
    length = njScalor(&direction);
    from_start = vector_distance(&beam->start, &closest);
    from_end = vector_distance(&beam->end, &closest);
    if (from_start > length) {
        beam->nearest = beam->end;
    } else if (from_end > length) {
        beam->nearest = beam->start;
        return;
    } else {
        beam->nearest = closest;
        if (distance > BEAM_RADIUS) {
            return;
        }
        finalhazard_push_player(NULL, FINALHAZARD_PUSH_SPEED);
        /* @bug mismatched-call: the call at 0x8c50adfc passes nothing, and
         * finalhazard_hurt at 0x8c504b58 takes an argument, as its call at
         * 0x8c504da2 passes one */
#ifdef NONMATCHING
        /* this side keeps the tag's result, as finalhazard_hurt reads no
         * argument. it calls the function by its own name: this side's objects
         * don't define finalhazard_laser_hurt, which only symbols.txt places */
        if (finalhazard_hurt(NULL) == 1)
#else
        if (finalhazard_laser_hurt() == 1)
#endif
        {
            return;
        }
        player_work[finalhazard_players->active]->flags |= WORK_HURT;
        finalhazard_stun();
    }
}

static void finalhazard_laser_step(struct task *task)
{
    struct task_work *work = task->work;
    struct laser *beam = task->extra.p;
    if (game_state.time_stopped != 0) {
        return;
    }
    switch (work->mode) {
    case LASER_EXTEND:
        laser_extend(task);
        break;
    case LASER_HOLD:
        laser_hold(task);
        break;
    case LASER_TRACK:
        laser_track(task);
        break;
    case LASER_RETRACT:
        laser_retract(task);
        break;
    case LASER_WAIT:
        laser_wait(task);
        break;
    }
    if (FINALHAZARD_STUNNED() == 0) {
        if (beam->tracking_speed < tracking_limit) {
            beam->tracking_speed += tracking_accel;
            if (beam->tracking_speed > tracking_limit) {
                beam->tracking_speed = tracking_limit;
            }
        }
    }
    if (finalhazard_players->transition == SWITCH_PLAY &&
        FINALHAZARD_STUNNED() == 0) {
        beam->player_position = player_work[finalhazard_players->active]->pos;
    } else if (FINALHAZARD_STUNNED() == 1) {
        beam->sweep_speed *= STUN_DRAG;
        beam->tracking_speed *= STUN_DRAG;
    }
    beam->spin += spin_step;
    laser_contact(task);
}

static void finalhazard_laser_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct laser *beam = task->extra.p;
    NJS_POINT3 position;
    NJS_VECTOR offset;
    Float scale;
    Float advance;
    Sint8 link;
    /* @hack type: index, an int copy of link, where link would do */
    int index;
    light_apply(LAST_BOSS_EFFECT_LIGHT);
    njSetTexture(&finalhazard_effect_texlist);
    njPushMatrixEx();
    position = beam->start;
    /* @hack spelling: a do-while under an if, where a for loop would do */
    link = 0;
    if (beam->pieces > 0) {
        do {
            if (beam->pieces > 2) {
                if (link == beam->pieces - 2) {
                    scale = work->scl.x;
                } else {
                    scale = 1.0F;
                }
            } else {
                scale = work->scl.x;
            }
            index = link;
            advance = -(scale * PIECE_LENGTH);
            if (scale > 0.0F) {
                njPushMatrixEx();
                njTranslateEx(&position);
                njRotateY(NULL, work->ang.y);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, QUARTER_TURN);
                if (index == 0) {
                    njScale(NULL, scale, 1.0F, 1.0F);
                    laser_draw_start(beam->spin);
                } else if (index == beam->pieces - 1) {
                    njTranslate(NULL, advance, 0.0F, 0.0F);
                    njRotateZ(NULL, HALF_TURN);
                    njScale(NULL, scale, 1.0F, 1.0F);
                    laser_draw_end(beam->spin);
                } else {
                    njScale(NULL, scale, 1.0F, 1.0F);
                    laser_draw_link();
                }
                njPopMatrixEx();
            }
            matrix_push_unit();
            offset = link_axis;
            offset.x = advance;
            njRotateY(NULL, work->ang.y);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, QUARTER_TURN);
            njCalcVector(NULL, &offset, &offset);
            njAddVector(&position, &offset);
            njPopMatrixEx();
            link++;
        } while (link < beam->pieces);
    }
    njPopMatrixEx();
}

static void finalhazard_laser_destroy(struct task *task)
{
}

static struct task *laser_create(struct task *parent, Sint8 kind,
                                 const NJS_POINT3 *origin,
                                 const NJS_POINT3 *target)
{
    struct task *task;
    struct task_work *work;
    struct laser *beam;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, finalhazard_laser_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    task->draw = finalhazard_laser_draw;
    task->destructor = finalhazard_laser_destroy;
    beam = syCalloc(1, sizeof(*beam));
    if (!beam) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = beam;
    work->pos = *origin;
    work->scl.x = 0.0F;
    beam->pieces = CAPS;
    beam->length = 0.0F;
    beam->kind = kind;
    beam->reach = beam_reach;
    work->timer = 1;
    work->mode = LASER_WAIT;
    beam->player = finalhazard_players->active;
    beam->target = target;
    beam->start = work->pos;
    beam->end = work->pos;
    beam->player_position = player_work[finalhazard_players->active]->pos;
    laser_aim_point(beam->target, &beam->player_position, &beam->aim);
    laser_limit_height(&work->pos, &beam->aim);
    laser_face(&work->pos, &beam->aim, &work->ang);
    return task;
}

static void laser_stop(struct task *task)
{
    struct task_work *work;
    struct laser *beam;
    if (!task) {
        return;
    }
    work = task->work;
    beam = task->extra.p;
    if (!work || !beam) {
        return;
    }
    work->flags |= LASER_STOP;
}

static Sint8 laser_stopped(struct task *task)
{
    struct task_work *work;
    struct laser *beam;
    if (!task) {
        return -1;
    }
    work = task->work;
    beam = task->extra.p;
    if (!work || !beam) {
        return -1;
    }
    if (work->flags & LASER_STOPPED) {
        return 1;
    }
    return 0;
}

static struct task *pair_beam(struct task *task, Sint8 index)
{
    struct task *beam;
    /* @bug uninitialized: an index other than 0 or 1 returns beam unset. the
     * branches at 0x8c50b304 and 0x8c50b30e pass over both loads, and the
     * return at 0x8c50b316 hands back the index, which R0 still holds */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other index returns null */
    if (index != 0 && index != 1) {
        return NULL;
    }
#endif
    if (index == 0) {
        beam = PAIR(task)->beams[0];
    }
    if (index == 1) {
        beam = PAIR(task)->beams[1];
    }
    return beam;
}

static void laser_pair_step(struct task *task)
{
    /* @hack redundant: the lookups of both beams and a test of the pause
     * flags, none of which does anything */
    pair_beam(task, 0);
    pair_beam(task, 1);
    if (game_state.time_stopped == 0 && game_state.unk_09 == 0) {
        /* @hack redundant: a test of task, in a block no path reaches */
        if (0) {
            if (task) {
            }
        }
    }
}

static void laser_pair_destroy(struct task *task)
{
    struct laser_pair *pair = PAIR(task);
    struct task *first = pair_beam(task, 0);
    struct task *second = pair_beam(task, 1);
    if (first) {
        task_kill(first);
        pair->beams[0] = NULL;
    }
    if (second) {
        task_kill(second);
        pair->beams[1] = NULL;
    }
}

struct task *finalhazard_fire_lasers(struct task *parent,
                                     const struct finalhazard_attack *attack)
{
    struct task *task;
    struct task *beam;
    struct laser_pair *pair;
    NJS_POINT3 *target;
    if (!parent) {
        return NULL;
    }
#ifdef NONMATCHING
    /* this side fires nothing for an attack the header rules out. it refuses a
     * missing target, target work or first point. it refuses a count other than
     * 1 or 2, and a count of 2 without a second point */
    if (!attack || !attack->target || !attack->target->work || !attack->first ||
        (attack->count != 1 && attack->count != 2) ||
        (attack->count == 2 && !attack->second)) {
        return NULL;
    }
#endif
    task = task_create_child(TASK_WORK, laser_pair_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = laser_pair_destroy;
    pair = PAIR(task);
    pair->count = attack->count;
    target = &attack->target->work->pos;
    if (attack->count == 1) {
        if (attack->second) {
            if (!finalhazard_panel_cannon(attack->target)) {
                beam =
                    laser_create(parent, attack->kind, attack->first, target);
            } else {
                beam =
                    laser_create(parent, attack->kind, attack->second, target);
            }
            pair->beams[0] = beam;
            pair->beams[1] = NULL;
        } else {
            beam = laser_create(parent, attack->kind, attack->first, target);
            pair->beams[0] = beam;
            pair->beams[1] = NULL;
        }
    } else {
        beam = laser_create(parent, attack->kind, attack->first, target);
        pair->beams[0] = beam;
        if (attack->second) {
            beam = laser_create(parent, attack->kind, attack->second, target);
            pair->beams[1] = beam;
        } else {
            pair->beams[1] = NULL;
        }
    }
#ifdef NONMATCHING
    /* this side destroys the holder and returns null where a beam the attack
     * names didn't start. the matching side leaves the beam out */
    if (!pair->beams[0] || (pair->count == 2 && !pair->beams[1])) {
        task_destroy(task);
        return NULL;
    }
#endif
    return task;
}

void finalhazard_lasers_retract(struct task *task)
{
    struct laser_pair *pair;
    Sint8 i;
    if (task) {
        pair = PAIR(task);
        for (i = 0; i < pair->count; i++) {
            laser_stop(pair_beam(task, i));
        }
    }
}

Sint8 finalhazard_lasers_finished(struct task *task)
{
    struct laser_pair *pair;
    int result = 0;
    Sint8 i;
    if (!task) {
        return -1;
    }
    pair = PAIR(task);
    for (i = 0; i < pair->count; i++) {
        /* @bug logic: the result is the last beam's alone, while the first
         * may still fire. each pass sets result at 0x8c50b500 over the one
         * before, and the loop at 0x8c50b50a ends at the return at
         * 0x8c50b50c */
        result = laser_stopped(pair_beam(task, i));
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns the first beam's
         * result that isn't 1 */
        if (result != 1) {
            return result;
        }
#endif
    }
    return result;
}

void finalhazard_debug_lasers(void)
{
    PDS_PERIPHERAL *pads;
    Sint8 i;
    for (i = 0; i < settings; i++) {
        if (i == selection) {
            njPrintColor(FINALHAZARD_MENU_SELECTED);
        }
        switch (i) {
        case CANNON_SPEED:
            njPrint(FINALHAZARD_SETTING_LINE(i), "CANNON SPEED %.3f",
                    tracking_limit);
            break;
        }
        if (i == selection) {
            njPrintColor(FINALHAZARD_MENU_TEXT);
        }
    }
    if (player_pad_records[finalhazard_players->active].press & PDD_DGT_TY) {
        selection++;
        if (selection == settings) {
            selection = CANNON_SPEED;
        }
    }
    /* @hack spelling: the assignment of pads inside the test, where a
     * statement of its own would do */
    if (pads = player_pad_records,
        pads[finalhazard_players->active].on & PDD_DGT_TL) {
        if (repeat_delay-- > 0) {
            return;
        }
        if (selection == CANNON_SPEED) {
            if (tracking_limit > 0.0F) {
                tracking_limit -= FINALHAZARD_MENU_SPEED_STEP;
            }
        }
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    } else if (pads[finalhazard_players->active].on & PDD_DGT_TR) {
        if (repeat_delay-- > 0) {
            return;
        }
        if (selection == CANNON_SPEED) {
            if (tracking_limit < FINALHAZARD_MENU_TOP_SPEED) {
                tracking_limit += FINALHAZARD_MENU_SPEED_STEP;
            }
        }
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    } else {
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    }
}
