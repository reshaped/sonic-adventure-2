/* the mech bosses' beam. a charge grows at a boss's muzzle. a shot homes in on
 * a target, leaves a trail, and sets off a bomb where it hits. Hot Shot and
 * Flying Dog fire it through shared/mech_boss_beam_gun.h */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "shared/gun_mech/bomb_art.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_beam.h"
#include "shared/mech_boss_beam_glow.h"
#include "shared/mech_boss_sounds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#define BOMB_VOLUME 100
#define TORUS_VOLUME 120
/* the torus plays the bomb's sound again, at a random pitch from TORUS_PITCH
 * up to TORUS_PITCH + TORUS_PITCH_SPREAD */
#define TORUS_PITCH 256.0F
#define TORUS_PITCH_SPREAD 256.0F
/* the charge's sound plays as two voices, keyed by the sound's task and by
 * MECH_BOSS_SECOND_KEY of that task. they start CHARGE_DETUNE below and above
 * the sound's own pitch, at CHARGE_VOLUME. every CHARGE_SOUND_STEPS steps, a
 * pitch below CHARGE_PITCH_TOP gains CHARGE_PITCH_STEP, and both voices take
 * it. a volume below SOUND_LOUDEST gains CHARGE_VOLUME_STEP the same way */
#define CHARGE_DETUNE 64
#define CHARGE_SOUND_STEPS 40
#define CHARGE_PITCH_STEP 0x100
#define CHARGE_PITCH_TOP 0x800
#define CHARGE_VOLUME (-20)
#define CHARGE_VOLUME_STEP 10

/* the pad's shake as the shot hits, and as the torus starts */
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 25
#define VIBRATION_TIME 1

/* the task level of the trail's pieces, the bomb, the torus and the pillar.
 * none of them has a parent */
#define EFFECT_LEVEL 3

/* the charge's work's timer starts at CHARGE_TIMER, and nothing counts it
 * down. while the charge grows, its draw's alpha is the timer times
 * ALPHA_PER_STEP, less 1, held to charge_color.a. at CHARGE_TIMER that is
 * 1.4, above charge_color.a, so the alpha is always charge_color.a */
#define CHARGE_TIMER 120
#define ALPHA_PER_STEP 0.02F
/* while the charge grows, each step moves its offset by charge_tuning.grow
 * times (-DRIFT_X, -DRIFT_Y, 0) */
#define DRIFT_X 1.2F
#define DRIFT_Y 0.5F
/* a charge's reach is REACH_PER_SCALE times charge_tuning.full_scale. the
 * charge throws three glows a step while its scale is above reach times
 * THREE_GLOWS. it throws two while its scale is above reach times TWO_GLOWS.
 * otherwise it throws one */
#define REACH_PER_SCALE 8.0F
#define THREE_GLOWS 0.6F
#define TWO_GLOWS 0.3F
/* a glow's accel is random, from GLOW_ACCEL up to GLOW_ACCEL +
 * GLOW_ACCEL_SPREAD. its range is GLOW_RANGE_PER_ACCEL times its accel. it
 * starts at a random offset from the charge, up to reach times its range
 * along each axis */
#define GLOW_ACCEL 0.02F
#define GLOW_ACCEL_SPREAD 0.05F
#define GLOW_RANGE_PER_ACCEL 50.0F

/* the target's share of the shot's heading gains HOMING_GAIN each step, up
 * to 1 */
#define HOMING_GAIN 0.05F
/* a shot fired within a step of its target takes NEAR_SCALE times the way
 * there as its spd. the step reads only spd's heading, so the scale changes
 * nothing */
#define NEAR_SCALE 0.5F

/* a piece of the trail is TRAIL_WIDTH across, and TRAIL_LENGTH times the
 * shot's move. it lives TRAIL_STEPS steps */
#define TRAIL_WIDTH 5.0F
#define TRAIL_LENGTH 1.1F
#define TRAIL_STEPS 16
#define TRAIL_POINTS 8

/* the bomb starts to fade BOMB_HOLD_STEPS steps after it reaches full size */
#define BOMB_HOLD_STEPS 30
/* the bomb, the torus and the pillar keep an offset in scl.z. their draws add
 * it to each channel of their materials' colors. it starts at START_OFFSET,
 * and falls by the tuning's fades. each ends once its offset falls below
 * END_OFFSET */
#define START_OFFSET 1.0F
#define END_OFFSET (-1.0F)

/* the draws offset each material's diffuse color by the constant material.
 * they give each strip NJD_FST_UA and NJD_FST_IS: translucent, without its
 * specular part */
#define DRAW_BITS_OFF NJD_CONTROL_3D_CONSTANT_MATERIAL
#define DRAW_BITS_ON                                                           \
    (NJD_CONTROL_3D_CNK_CONSTANT_ATTR | NJD_CONTROL_3D_OFFSET_MATERIAL)
#define DRAW_ATTR (NJD_FST_UA | NJD_FST_IS)

#define BOMB(task) ((struct bomb_work *)(task)->work)
#define CHARGE_SOUND(task) ((struct charge_sound_work *)(task)->work)

/* what the charge's step does */
enum charge_mode {
    MODE_GROW,
    MODE_WAIT, /* stops the sound and shrinks at timer 0. nothing sets it */
    MODE_SHRINK
};

/* the charge's record, in task->extra */
struct charge {
    Uint16 frame; /* the texture animation's frame, which each step counts */
    NJS_POINT3 *pos; /* the point it follows */
    struct angles *ang; /* the angles that turn its offset */
    NJS_POINT3 offset;
    struct task *sound;
};

/* the shot's record, in task->extra */
struct shot {
    Uint8 unk_00[4];
    NJS_POINT3 *target;
    /* the target's share of the shot's heading. its last move's heading has
     * the rest */
    Float homing;
    Sint32 *busy; /* the word its bomb sets to 0 as it ends */
    NJS_VECTOR spd; /* its last move */
};

/* the charge sound's work, laid over the struct task_work that
 * task_create_child allocates */
struct charge_sound_work {
    Sint8 mode;
    Uint8 unk_01[5];
    Uint16 timer; /* the steps to the next rise */
    /* the pitch both voices take at a rise. it starts as the charge's ang.x,
     * which task_create_child copies */
    Sint32 pitch;
    Sint32 unk_0c;
    Sint32 volume;
};

/* the bomb's work, laid over the struct task_work that task_create
 * allocates. the torus and the pillar keep their scale and offset in scl as
 * the bomb does */
struct bomb_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer; /* the steps left before it fades */
    Sint32 *busy;
    Uint32 frame; /* the texture animation's frame */
    Angle unk_10;
    NJS_POINT3 pos;
    /* its scale in x, which it draws in all three axes, and its offset in z */
    NJS_POINT3 scl;
    struct collision_work *collision;
};

struct beam_charge_tuning {
    /* the timer mech_boss_beam_charge_end sets, which nothing reads after */
    Sint16 end_timer;
    Float full_scale;
    Float start_scale;
    Float grow; /* what its scale gains each step */
    Float shrink; /* what its scale loses each step as it ends */
    Float end_scale; /* the scale below which it ends */
};
static struct beam_charge_tuning charge_tuning = {
    60, 3.0F, 0.05F, 0.01F, 0.1F, 1.0F,
};
static Sint16 charge_anim_uvs[474] = {
#include "shared/mech_boss_beam/charge_anim_uvs.inc"
};
static struct texture_anim charge_anim = {
#include "shared/mech_boss_beam/charge_anim.inc"
};
/* the offset the charge's draw adds to its materials' colors */
static NJS_ARGB charge_color = {-0.2F, 0.0F, 0.4F, 0.0F};
/* the shot's tuning, of which only speed is read */
struct beam_shot_tuning {
    Float unk_00[4];
    Float speed; /* the most the shot moves in a step */
    Float unk_14[3];
};
/* clang-format off */
static struct beam_shot_tuning shot_tuning = {
    {0.0F, 0.0F, 0.0F, 0.0F}, 10.0F, {0.4F, 0.08F, 20.0F},
};
static struct collision_shape shot_shape = {
    0, SHAPE_SPHERE, PUSH_PLAYERS, SHAPE_HURTS_PLAYERS | SHAPE_HURTS_OBJECTS,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the whole of e102newbeam, drawn 64 pixels square about its center */
static NJS_TEXANIM shot_cell = {
#include "shared/mech_boss_beam/shot_cell.inc"
};
static NJS_SPRITE shot_sprite = {
#include "shared/mech_boss_beam/shot_sprite.inc"
};
/* a piece of the trail: a prism of three sides along z from 0 to 1. it draws as
 * a strip of triangles. the even points stand at z = 1, the odd at z = 0 */
static NJS_POLYGON_VTX trail_points[TRAIL_POINTS] = {
#include "shared/mech_boss_beam/trail_points.inc"
};
/* the colors of a piece of the trail, one for each step of its life. each is
 * white, with an alpha 16 below the one before. the last is clear */
static Uint32 trail_colors[TRAIL_STEPS] = {
    0xffffffff, 0xefffffff, 0xdfffffff, 0xcfffffff, 0xbfffffff, 0xafffffff,
    0x9fffffff, 0x8fffffff, 0x7fffffff, 0x6fffffff, 0x5fffffff, 0x4fffffff,
    0x3fffffff, 0x2fffffff, 0x1fffffff, 0x00ffffff,
};

static void charge_destroy(struct task *task);
static void charge_step(struct task *task);
static void charge_draw(struct task *task);
static void shot_destroy(struct task *task);
static void shot_step(struct task *task);
static void shot_draw(struct task *task);
static void direction_angles(NJS_VECTOR *dir, struct angles *ang);
static void trail_piece_draw(struct task *task);
static void trail_piece_step(struct task *task);
static void trail_piece_destroy(struct task *task);
static void trail_piece_create(struct task *parent, NJS_POINT3 *from,
                               NJS_POINT3 *to);
static void beam_bomb_destroy(struct task *task);
static void beam_bomb_step(struct task *task);
static void beam_bomb_draw(struct task *task);
static void beam_bomb_create(NJS_POINT3 *pos, Sint32 *busy);
static void torus_destroy(struct task *task);
static void torus_step(struct task *task);
static void torus_draw(struct task *task);
static void torus_create(NJS_POINT3 *pos, Float size);
static void pillar_step(struct task *task);
static void pillar_draw(struct task *task);
static void pillar_destroy(struct task *task);
static struct task *pillar_create(NJS_POINT3 *pos, Float size);
static void charge_sound_step(struct task *task);
static void charge_sound_destroy(struct task *task);
static struct task *charge_sound_create(struct task *parent);

struct task *mech_boss_beam_charge_create(struct task *parent, NJS_POINT3 *pos,
                                          NJS_POINT3 *offset,
                                          struct angles *ang)
{
    struct task *task;
    struct task_work *work;
    struct charge *charge;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, charge_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = task->work;
        charge = syCalloc(1, sizeof(struct charge));
        if (charge == NULL) {
            task_kill(task);
            return NULL;
        }
        task->extra.p = charge;
        charge->pos = pos;
        charge->ang = ang;
        charge->offset = *offset;
        work->scl.x = charge_tuning.start_scale;
        work->timer = CHARGE_TIMER;
        charge->sound = charge_sound_create(task);
        task->draw = charge_draw;
        task->destructor = charge_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* for a null parent the image returns R0 as the caller left it, which C
     * can't give. this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void charge_destroy(struct task *task)
{
}

void mech_boss_beam_charge_end(struct task *task)
{
    struct task_work *work;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct charge *charge;

    work = task->work;
    /* @hack redundant: charge, which nothing reads */
    charge = task->extra.p;
    work->timer = charge_tuning.end_timer;
    work->mode = MODE_SHRINK;
    work->submode = 0;
}

static void charge_step(struct task *task)
{
    struct task_work *work;
    struct charge *charge;
    Float reach;
    Float range;
    /* @hack type: count and i Sint8s, where ints would do */
    Sint8 count;
    Sint8 i;

    work = task->work;
    charge = task->extra.p;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    switch (work->mode) {
    case MODE_GROW:
        work->scl.x += charge_tuning.grow;
        if (work->scl.x > charge_tuning.full_scale) {
            work->scl.x = charge_tuning.full_scale;
        } else {
            /* @hack spelling: the drift added negated, where -= would do */
            charge->offset.y += -charge_tuning.grow * DRIFT_Y;
            charge->offset.x += -charge_tuning.grow * DRIFT_X;
        }
        break;
    case MODE_WAIT:
        if (work->timer == 0) {
            task_kill(charge->sound);
            work->mode = MODE_SHRINK;
            work->submode = 0;
        }
        break;
    case MODE_SHRINK:
        work->scl.x -= charge_tuning.shrink;
        if (work->scl.x < charge_tuning.end_scale) {
            task_kill(task);
        }
        break;
    }
    work->ang = *charge->ang;
    {
        NJS_VECTOR origin = {0.0F, 0.0F, 0.0F};

        njPushMatrix(&_nj_unit_matrix_);
        njTranslateV(NULL, charge->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, QUARTER_TURN);
        njTranslateV(NULL, &charge->offset);
        njCalcPoint(NULL, &origin, &work->pos);
        njPopMatrix(1);
    }
    if (work->mode != MODE_SHRINK) {
        struct mech_boss_beam_glow_spec spec;

        reach = charge_tuning.full_scale * REACH_PER_SCALE;
        spec.target = &work->pos;
        spec.radius = &work->scl.x;
        /* @bug logic: the charge never throws more than one glow. its scale
         * stays at or below charge_tuning.full_scale, 3. the tests at 0x24e and
         * 0x262 of the object's code need it above 14.4 and 7.2 */
        if (work->scl.x > reach * THREE_GLOWS) {
            count = 3;
        } else if (work->scl.x > reach * TWO_GLOWS) {
            count = 2;
        } else {
            count = 1;
        }
        /* @hack spelling: the loop as a do-while under a test of count, where
         * a for would do */
        i = 0;
        if (count > 0) {
            do {
                spec.accel = RANDOM_FROM(GLOW_ACCEL, GLOW_ACCEL_SPREAD);
                range = spec.accel * GLOW_RANGE_PER_ACCEL;
                spec.offset.x =
                    (reach - (rand() * reach * (2.0F * RAND_SCALE))) * range;
                spec.offset.y =
                    (reach - (rand() * reach * (2.0F * RAND_SCALE))) * range;
                spec.offset.z =
                    (reach - (rand() * reach * (2.0F * RAND_SCALE))) * range;
                mech_boss_beam_glow_create(task, &spec);
                i++;
            } while (i < count);
        }
    }
    charge->frame++;
}

static void charge_draw(struct task *task)
{
    struct charge *charge;
    struct task_work *work;
    NJS_VECTOR scale;
    NJS_ARGB *color;
    Float alpha;

    work = task->work;
    charge = task->extra.p;
    njSetTexture(&grobo_eff2_texlist);
    njFogDisable();
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateX(NULL, QUARTER_TURN);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_BITS_OFF);
    draw_control_set(DRAW_BITS_ON);
    draw_attr_set(0, DRAW_ATTR);
    scale.x = scale.y = scale.z = work->scl.x;
    njScaleEx(&scale);
    /* @hack spelling: color, a pointer to charge_color, where charge_color
     * named in place would do */
    color = &charge_color;
    if (work->mode == MODE_GROW) {
        alpha = (work->timer * ALPHA_PER_STEP) - 1.0F;
        if (alpha > color->a) {
            alpha = color->a;
        }
    } else {
        alpha = color->a;
    }
    constant_material_set(alpha, color->r, color->g, color->b);
    texture_anim_apply(&mech_boss_beam_charge_model, &charge_anim,
                       charge->frame);
    njCnkEasyDrawModel(&mech_boss_beam_charge_model);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_control_restore();
    draw_attr_restore();
    njFogEnable();
}

struct task *mech_boss_beam_fire(struct task *parent,
                                 struct mech_boss_beam_aim *aim, Sint32 *busy)
{
    struct task *task;
    struct shot *shot;
    struct task_work *work;
    Float distance;
    NJS_VECTOR to_target;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, shot_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = task->work;
        shot = syCalloc(1, sizeof(struct shot));
        if (shot == NULL) {
            task_kill(task);
            return NULL;
        }
        task->extra.p = shot;
        work->pos = *aim->pos;
        shot->target = aim->target;
        work->id = WORK_ID_ATTACK;
        /* @hack spelling: the target read back from the record, where
         * aim->target would do */
        to_target.x = shot->target->x - work->pos.x;
        to_target.y = shot->target->y - work->pos.y;
        to_target.z = shot->target->z - work->pos.z;
        distance = njScalor(&to_target);
        if (distance > shot_tuning.speed) {
            NJS_VECTOR dir = {-1.0F, 0.0F, 0.0F};
            NJS_VECTOR spd;

            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
            njCalcVector(NULL, &dir, &spd);
            njPopMatrix(1);
            shot->spd.x = spd.x * shot_tuning.speed;
            shot->spd.y = spd.y * shot_tuning.speed;
            shot->spd.z = spd.z * shot_tuning.speed;
        } else {
            shot->spd.x = to_target.x * NEAR_SCALE;
            shot->spd.y = to_target.y * NEAR_SCALE;
            shot->spd.z = to_target.z * NEAR_SCALE;
        }
        shot->homing = 0.0F;
        shot->busy = busy;
        collision_init(task, &shot_shape, 1, COLLISION_ENEMY);
        /* @hack spelling: task->work read again, where work would do */
        task->work->collision->flags &= ~COLLISION_TARGET;
        task->destructor = shot_destroy;
        task->draw = shot_draw;
    }
#ifdef NONMATCHING
    /* where the call makes the shot, the image returns R0 as its last call left
     * it. C can't give that, so this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void shot_destroy(struct task *task)
{
}

static void shot_step(struct task *task)
{
    struct task_work *work;
    struct shot *shot;
    Float distance;
    Float speed;
    Float rest;
    NJS_VECTOR to_target;
    NJS_VECTOR dir;
    NJS_VECTOR move;
    NJS_POINT3 from;

    work = task->work;
    shot = task->extra.p;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    to_target = *shot->target;
    speed = shot_tuning.speed;
    njSubVector(&to_target, &work->pos);
    distance = njScalor(&to_target);
    if (distance > speed) {
        njUnitVector(&to_target);
        dir = shot->spd;
        njUnitVector(&dir);
        if (shot->homing < 1.0F) {
            rest = 1.0F - shot->homing;
        } else {
            shot->homing = 1.0F;
            rest = 0.0F;
        }
        move.x = (to_target.x * shot->homing) + (dir.x * rest);
        move.y = (to_target.y * shot->homing) + (dir.y * rest);
        move.z = (to_target.z * shot->homing) + (dir.z * rest);
        move.x *= speed;
        move.y *= speed;
        move.z *= speed;
    } else {
        /* @hack spelling: the move copied a field at a time, where
         * move = to_target would do */
        move.x = to_target.x;
        move.y = to_target.y;
        move.z = to_target.z;
    }
    from = work->pos;
    work->pos.x += move.x;
    work->pos.y += move.y;
    work->pos.z += move.z;
    trail_piece_create(NULL, &from, &work->pos);
    shot->spd = move;
    shot->homing += HOMING_GAIN;
    if (collision_next_player(task) || collision_next_object(task)) {
        beam_bomb_create(&work->pos, shot->busy);
        vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_TIME);
        task_kill(task);
    }
    collision_register(task);
}

static void shot_draw(struct task *task)
{
    struct task_work *work;

    work = task->work;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    /* @hack type: the sprite's pos written through the sprite as a point,
     * where shot_sprite.p would do */
    *(NJS_POINT3 *)&shot_sprite = work->pos;
    njSetTexture(&grobo_eff_texlist);
    njDrawSprite3D(&shot_sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

/* the angles that turn +z to point along dir: ang.y about y, then ang.x about
 * x. ang.z becomes 0. a dir of length 0 leaves ang as it was */
static void direction_angles(NJS_VECTOR *dir, struct angles *ang)
{
    Float length;

    length = njScalor(dir);
    if (length != 0.0F) {
        ang->y = atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN;
        ang->x = asinf(-dir->y / length) * ANGLE_PER_RADIAN;
        ang->z = 0;
    }
}

#ifdef NONMATCHING
/* trail_colors[index]. past its end, it returns the two words the image reads
 * there instead. they are the first two of the bomb's model's polygon list: a
 * clear color and an opaque white */
static Uint32 trail_color(int index)
{
    static const Uint32 past[2] = {0x00042113, 0xffffffff};

    return index < TRAIL_STEPS ? trail_colors[index]
                               : past[index - TRAIL_STEPS];
}
#endif

static void trail_piece_draw(struct task *task)
{
    struct task_work *work;

    work = task->work;
    /* @bug memory: reads past trail_colors on a piece's last two draws. at a
     * timer of 15 the read at 0x976 of the object's code takes entry 16. the
     * step that kills the piece, at a timer of TRAIL_STEPS, leaves it one more
     * draw, whose reads at 0x960 and 0x976 take entries 16 and 17. they are
     * the first two words of the bomb's model's polygon list */
#ifdef NONMATCHING
    /* this side keeps the tag's result: past the table, trail_color gives the
     * two words the image reads there */
    trail_points[0].col = trail_points[2].col = trail_points[4].col =
        trail_points[6].col = trail_color(work->timer);
    trail_points[1].col = trail_points[3].col = trail_points[5].col =
        trail_points[7].col = trail_color(work->timer + 1);
#else
    trail_points[0].col = trail_points[2].col = trail_points[4].col =
        trail_points[6].col = trail_colors[work->timer];
    trail_points[1].col = trail_points[3].col = trail_points[5].col =
        trail_points[7].col = trail_colors[work->timer + 1];
#endif
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njScale(NULL, TRAIL_WIDTH, TRAIL_WIDTH, work->scl.x);
    njDrawPolygon3DEx(trail_points, TRAIL_POINTS, 1);
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void trail_piece_step(struct task *task)
{
    struct task_work *work;

    work = task->work;
    if (!game_state.time_stopped && !game_state.unk_09) {
        if (++work->timer >= TRAIL_STEPS) {
            task_kill(task);
        }
    }
}

static void trail_piece_destroy(struct task *task)
{
}

/* create a piece of the trail along the move from *from to *to. it is
 * parent's child, or a task of EFFECT_LEVEL for a null parent */
static void trail_piece_create(struct task *parent, NJS_POINT3 *from,
                               NJS_POINT3 *to)
{
    struct task_work *work;
    struct task *task;
    NJS_VECTOR move;

    if (parent == NULL) {
        task = task_create(TASK_WORK, EFFECT_LEVEL, trail_piece_step,
                           "BossGroboEBeamCExec");
    } else {
        task = task_create_child(TASK_WORK, trail_piece_step, parent);
    }
    if (task != NULL) {
        work = task->work;
        work->timer = 0;
        work->mode = 0;
        work->pos = *from;
        move = *to;
        njSubVector(&move, from);
        direction_angles(&move, &work->ang);
        work->scl.x = njScalor(&move) * TRAIL_LENGTH;
        task->destructor = trail_piece_destroy;
        task->draw = trail_piece_draw;
    }
}

static void beam_bomb_destroy(struct task *task)
{
    struct bomb_work *work;
    Sint32 *busy;

    work = BOMB(task);
    /* @hack spelling: busy, a local for work->busy, where work->busy would
     * do */
    busy = work->busy;
    *busy = 0;
}

static void beam_bomb_step(struct task *task)
{
    struct bomb_work *work;
    struct bomb_tuning *tuning;
    Float size;

    work = BOMB(task);
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    /* @hack spelling: size and fade read by the tuning's name, and swell and
     * grow through tuning, where one of the two would do */
    size = mech_boss_beam_bomb_tuning.size;
    tuning = &mech_boss_beam_bomb_tuning;
    if (work->scl.x >= size) {
        work->scl.x += tuning->swell;
        if (work->timer > 0) {
            work->timer--;
        } else if ((work->scl.z -= mech_boss_beam_bomb_tuning.fade) <
                   END_OFFSET) {
            task_kill(task);
        }
    } else {
        work->scl.x += tuning->grow;
        if (work->scl.x >= size) {
            work->scl.x = size;
            work->timer = BOMB_HOLD_STEPS;
            pillar_create(&work->pos, size);
            torus_create(&work->pos, size);
        }
    }
    texture_anim_apply(&mech_boss_beam_bomb_model, &mech_boss_beam_bomb_anim,
                       work->frame++);
    collision_register(task);
}

static void beam_bomb_draw(struct task *task)
{
    struct task_work *work;

    work = task->work;
    njFogDisable();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_BITS_OFF);
    draw_control_set(DRAW_BITS_ON);
    draw_attr_set(0, DRAW_ATTR);
    njSetTexture(&grobo_eff2_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njScale(NULL, work->scl.x, work->scl.x, work->scl.x);
    constant_material_set(work->scl.z, work->scl.z, work->scl.z, work->scl.z);
    njCnkSimpleDrawModel(&mech_boss_beam_bomb_model);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    /* @bug logic: leaves DRAW_ATTR set for the draws after it. the bomb's,
     * the torus's and the pillar's draws save the attributes and set them, but
     * restore only the control bits, where charge_draw restores both. here the
     * set is at 0xc70 of the object's code, and the restore at 0xcc8 */
    draw_control_restore();
    njFogEnable();
}

/* create a bomb at pos, a task with no parent. it sets *busy to 0 as it
 * ends */
static void beam_bomb_create(NJS_POINT3 *pos, Sint32 *busy)
{
    struct task *task;
    struct bomb_work *work;

    task = task_create(TASK_WORK, EFFECT_LEVEL, beam_bomb_step,
                       "BossGroboEBombExec");
    if (task != NULL) {
        work = BOMB(task);
        work->pos = *pos;
        work->scl.x = 0.0F;
        work->scl.z = START_OFFSET;
        work->id = WORK_ID_ATTACK;
        work->busy = busy;
        collision_init(task, &mech_boss_beam_bomb_shape, 1, COLLISION_ENEMY);
        /* @hack spelling: task->work read again, where work would do */
        task->work->collision->flags &= ~COLLISION_TARGET;
        sound_play_at(MECH_BOSS_SOUND_BEAM_BOMB, NULL, 0, BOMB_VOLUME,
                      &work->pos);
        task->destructor = beam_bomb_destroy;
        task->draw = beam_bomb_draw;
    }
}

static void torus_destroy(struct task *task)
{
}

static void torus_step(struct task *task)
{
    struct task_work *work;

    work = task->work;
    if (!game_state.time_stopped && !game_state.unk_09) {
        work->scl.z -= mech_boss_beam_bomb_tuning.torus_fade;
        work->scl.x += mech_boss_beam_bomb_tuning.swell;
        if (work->scl.z < END_OFFSET) {
            task_kill(task);
        }
    }
}

static void torus_draw(struct task *task)
{
    struct task_work *work;

    work = task->work;
    njFogDisable();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_BITS_OFF);
    draw_control_set(DRAW_BITS_ON);
    draw_attr_set(0, DRAW_ATTR);
    njSetTexture(&grobo_eff2_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njScale(NULL, work->scl.x, 1.0F, work->scl.x);
    constant_material_set(work->scl.z, work->scl.z, work->scl.z, work->scl.z);
    njCnkSimpleDrawObject(&mech_boss_beam_torus_object);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    /* @bug logic: leaves DRAW_ATTR set, as beam_bomb_draw does: the set at
     * 0xe50 of the object's code, the restore at 0xea8 */
    draw_control_restore();
    njFogEnable();
}

static void torus_create(NJS_POINT3 *pos, Float size)
{
    struct task *task;
    struct task_work *work;

    task =
        task_create(TASK_WORK, EFFECT_LEVEL, torus_step, "BossGroboETorusExec");
    if (task != NULL) {
        work = task->work;
        work->pos = *pos;
        work->scl.x = size;
        work->scl.z = START_OFFSET;
        sound_play_at(MECH_BOSS_SOUND_BEAM_BOMB, NULL, 0, TORUS_VOLUME,
                      &work->pos);
        sound_set_pitch(NULL, MECH_BOSS_SOUND_BEAM_BOMB,
                        RANDOM_FROM(TORUS_PITCH, TORUS_PITCH_SPREAD));
        vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_TIME);
        task->destructor = torus_destroy;
        task->draw = torus_draw;
    }
}

static void pillar_step(struct task *task)
{
    struct task_work *work;

    work = task->work;
    if (!game_state.time_stopped && !game_state.unk_09) {
        work->scl.z -= mech_boss_beam_bomb_tuning.pillar_fade;
        work->pos.y += mech_boss_beam_bomb_tuning.pillar_rise;
        if (work->scl.z < END_OFFSET) {
            task_kill(task);
        }
    }
}

static void pillar_draw(struct task *task)
{
    struct task_work *work;

    work = task->work;
    njFogDisable();
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_BITS_OFF);
    draw_control_set(DRAW_BITS_ON);
    draw_attr_set(0, DRAW_ATTR);
    njSetTexture(&grobo_eff2_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njScale(NULL, work->scl.x, work->scl.x, work->scl.x);
    constant_material_set(work->scl.z, work->scl.z, work->scl.z, work->scl.z);
    njCnkSimpleDrawObject(&mech_boss_beam_pillar_object);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    /* @bug logic: leaves DRAW_ATTR set, as beam_bomb_draw does: the set at
     * 0xfb6 of the object's code, the restore at 0x100e */
    draw_control_restore();
    njFogEnable();
}

static void pillar_destroy(struct task *task)
{
}

static struct task *pillar_create(NJS_POINT3 *pos, Float size)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, EFFECT_LEVEL, pillar_step,
                       "BossGroboEHasiraExec");
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    work->pos = *pos;
    work->scl.x = size;
    work->scl.z = START_OFFSET;
    task->draw = pillar_draw;
    task->destructor = pillar_destroy;
    return task;
}

static void charge_sound_step(struct task *task)
{
    struct charge_sound_work *work;

    work = CHARGE_SOUND(task);
    if (game_state.time_stopped) {
        if (--work->timer == 0) {
            work->timer = CHARGE_SOUND_STEPS;
        }
    } else if (!game_state.unk_09) {
        if (--work->timer == 0) {
            if (work->pitch < CHARGE_PITCH_TOP) {
                work->pitch += CHARGE_PITCH_STEP;
                sound_set_pitch(task, SOUND_ANY_ID, work->pitch);
                sound_set_pitch(MECH_BOSS_SECOND_KEY(task), SOUND_ANY_ID,
                                work->pitch);
            }
            if (work->volume < SOUND_LOUDEST) {
                work->volume += CHARGE_VOLUME_STEP;
                sound_set_volume(task, SOUND_ANY_ID, work->volume);
                sound_set_volume(MECH_BOSS_SECOND_KEY(task), SOUND_ANY_ID,
                                 work->volume);
            }
            work->timer = CHARGE_SOUND_STEPS;
        }
    }
}

static void charge_sound_destroy(struct task *task)
{
    sound_stop_key(task);
    sound_stop_key(MECH_BOSS_SECOND_KEY(task));
    /* @hack redundant: the return after sound_stop_key, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static struct task *charge_sound_create(struct task *parent)
{
    struct task *task;
    struct charge_sound_work *work;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, charge_sound_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = CHARGE_SOUND(task);
    work->volume = CHARGE_VOLUME;
    sound_play_held(MECH_BOSS_SOUND_BEAM_CHARGE, task, MECH_BOSS_SOUND_PRIORITY,
                    0);
    sound_set_pitch(task, SOUND_ANY_ID, -CHARGE_DETUNE);
    sound_play_held(MECH_BOSS_SOUND_BEAM_CHARGE, MECH_BOSS_SECOND_KEY(task),
                    MECH_BOSS_SOUND_PRIORITY, 0);
    sound_set_pitch(MECH_BOSS_SECOND_KEY(task), SOUND_ANY_ID, CHARGE_DETUNE);
    sound_set_volume(task, SOUND_ANY_ID, work->volume);
    sound_set_volume(MECH_BOSS_SECOND_KEY(task), SOUND_ANY_ID, work->volume);
    work->timer = CHARGE_SOUND_STEPS;
    task->destructor = charge_sound_destroy;
    return task;
}
