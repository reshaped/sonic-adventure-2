#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/attack_art.h"
#include "boss_last2/arena.h"
#include "boss_last2/camera.h"
#include "boss_last2/joints.h"
#include "boss_last2/panel_sparks.h"
#include "boss_last2/panels.h"
#include "boss_last2/stage.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/last_boss_sphere.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

/* the weak point's modes, in its work's mode. it waits hidden in
 * PANEL_START until finalhazard_panel_move first moves it */
enum panel_mode {
    PANEL_START,
    PANEL_GROW,
    PANEL_ACTIVE,
    PANEL_SHRINK,
    PANEL_HIDDEN,
    PANEL_HIT
};
/* a work flag, set while the active player is within PROXIMITY_RADIUS of the
 * weak point. as the player comes that near, the Finalhazard breathes at
 * FINALHAZARD_JOINT_VOICE, at BREATH_VOLUME for BREATH_DURATION */
#define PLAYER_NEAR 0x100
#define PROXIMITY_RADIUS 300.0F
#define BREATH_VOLUME 70
#define BREATH_DURATION 120
/* a hit throws the player off at FINALHAZARD_PUSH_SPEED. it shakes the weak
 * point for HIT_STEPS before it shrinks */
#define HIT_STEPS 90
/* the weak point's hum plays for HUM_DURATION each step it plays. the hum's
 * volume is HUM_VOLUME while the weak point shows. as it grows, the volume
 * rises with fade by HUM_PER_FADE for each unit, up to HUM_VOLUME. as it
 * shrinks, the volume falls with fade from SHRINK_VOLUME. as a hit shakes
 * it, the volume moves against fade. no volume falls below LEAST_VOLUME */
#define HUM_DURATION 5
#define HUM_VOLUME 70
#define SHRINK_VOLUME 60
#define HUM_PER_FADE 120.0F
#define LEAST_VOLUME (-128)
/* fade offsets the weak point's alpha. FADE_HIDDEN hides it, and at
 * FADE_SHOWN it shows as modeled. as it shows or shakes, fade wobbles about
 * FADE_SHOWN by a sine whose phase moves PULSE_STEP each step */
#define FADE_HIDDEN (-1.0F)
#define FADE_SHOWN 0.0F
#define PULSE_STEP 0x400
/* an armed weak point's attack and guard. a safe one's attack is
 * SAFE_ATTACK, and its guard SAFE_GUARD, which no attack beats */
#define ARMED_ATTACK 1
#define ARMED_GUARD 0
#define SAFE_ATTACK 0
#define SAFE_GUARD 3
/* the collision record's radius */
#define RECORD_RADIUS 200.0F
/* a row or column of no slot */
#define NO_SLOT (-1)
/* a spark dies once its brightness, fading, is below SPARK_DARK */
#define SPARK_DARK .05F

/* the weak point, in its task's extra */
struct panel {
    /* its slot */
    Sint8 row;
    Sint8 col;
    /* its next slot, as panels.h says, or NO_SLOT for none */
    Sint8 next_row;
    Sint8 next_col;
    struct angles wobble; /* the phases of its scale's wobble on each axis */
    Angle swing;
    Float amplitude; /* the height's wobble */
    NJS_POINT3 front; /* the point before it, where its hit's sparks start */
    Float fade;
};

/* clang-format off */
static const struct collision_shape panel_shape = {
    0, SHAPE_SPHERE, PUSH_PLAYERS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(ARMED_ATTACK) | SHAPE_HURTS_PLAYERS,
    SHAPE_UNTURNED | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
        SHAPE_IGNORE_PICKUPS,
    {0, 0, 0}, 20, 0, 0, 0, {0, 0, 0}};
/* clang-format on */
/* the weak point's origin on its slot, and its front from there */
static const NJS_POINT3 panel_origin = {0, 1, 0};
static const NJS_VECTOR panel_front = {0, 15, 0};

/* a slot: its base, its angles in degrees, and its pos */
/* clang-format off */
#define SLOT(base, ax, ay, az, x, y, z)                                        \
    {(base), {NJM_DEG_ANG(ax), NJM_DEG_ANG(ay), NJM_DEG_ANG(az)},              \
     {(x), (y), (z)}}
struct panel_slot finalhazard_panel_slots[PANEL_ROWS][PANEL_COLUMNS] = {
    {SLOT(0, -130, -4, 180, 20, -15, 24),
     SLOT(1, -60, -8, 180, 18, 27, 35),
     SLOT(2, -143, -15, -168, 68, -36, 43),
     SLOT(3, -56, -20, 175, 153, 48, 72),
     SLOT(3, -154, -31, 178, 117, -60, 85),
     SLOT(3, -87, -17, 180, 72, -12, 134),
     SLOT(3, -140, -6, -195, 10, -39, 115),
     SLOT(3, -46, -18, 180, 29, 48, 109)},
    {SLOT(0, 114, 6, 180, 25, -8, -20),
     SLOT(1, 106, 12, 185, 23, -12, -31),
     SLOT(2, 38, 11, -198, 64, 47, -32),
     SLOT(3, 146, 16, 193, 125, -42, -76),
     SLOT(3, 60, 10, -198, 100, 49, -90),
     SLOT(3, 106, 0, -200, 40, -4, -114),
     SLOT(3, 50, 4, 180, 14, 63, -90),
     SLOT(4, 130, -12, -16, 38, 14, -33)}};
/* clang-format on */
/* each slot's cannon, as finalhazard_panel_cannon gives it */
static Sint8 slot_cannons[PANEL_ROWS][PANEL_COLUMNS] = {
    {0, 1, 0, 1, 0, 0, 0, 1}, {0, 0, 1, 0, 1, 1, 1, 0}};
static Float grow_step = .02F;
static Float unused_growth = .02F;
static Float shrink_step = .02F;
static Float unused_extent = 256.0F;

static void panel_proximity(struct task *task)
{
    struct task_work *work = task->work;
    Float distance = vector_distance(
        &player_work[finalhazard_players->active]->pos, &work->pos);
    Sint16 flags = work->flags;
    /* @hack redundant: the flag cleared ahead of the test, whose arms set or
     * clear it anyway */
    flags &= ~PLAYER_NEAR;
    if (distance < PROXIMITY_RADIUS) {
        flags |= PLAYER_NEAR;
    } else {
        flags &= ~PLAYER_NEAR;
    }
    if (!(work->flags & PLAYER_NEAR) && (flags & PLAYER_NEAR)) {
        sound_play_at_for(
            SOUND_BREATH, task, 1, BREATH_VOLUME, BREATH_DURATION,
            &finalhazard_joint_positions[FINALHAZARD_JOINT_VOICE]);
        sound_set_pitch(task, SOUND_BREATH,
                        njSin(RANDOM_TURN()) * FINALHAZARD_BREATH_PITCH);
    }
    work->flags = flags;
}

/* a column picked at random, one other than previous where previous isn't
 * NO_SLOT */
static Sint8 choose_column(Sint8 previous)
{
    Sint8 count;
    Sint8 selection;
    Sint8 i;
    int slot;
    /* @hack spelling: count set by an if and an else, where a conditional
     * expression would do */
    if (previous < 0) {
        count = PANEL_COLUMNS;
    } else {
        count = PANEL_COLUMNS - 1;
    }
    selection = njRoundOff((Float)rand() * count * RAND_SCALE);
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    return previous >= 0 && selection >= previous ? selection + 1 : selection;
#else
    /* @hack spelling: a walk over the columns that steps past previous, where
     * the return above would do. the walk returns, since selection is below
     * count */
    slot = 0;
    for (i = 0; i < count; i++, slot++) {
        if (i == previous) {
            slot++;
        }
        if (selection == i) {
            return slot;
        }
    }
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* set the weak point's pos and front at the slot of row and col. a row or col
 * of NO_SLOT leaves them as they are. front is pos plus panel_front turned by
 * the slot's matrix */
static void position_panel(struct task *task, Sint8 row, Sint8 col)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    struct panel_slot *slot;
    Angle *angles;
    NJS_POINT3 origin;
    NJS_POINT3 world;
    NJS_VECTOR offset;
    NJS_VECTOR direction;
    if (row >= 0 && col >= 0) {
        slot = &finalhazard_panel_slots[row][col];
        origin = panel_origin;
        matrix_push_unit();
        /* @hack spelling: angles, a pointer to slot->angles taken before the
         * matrix loads, where &slot->angles.x in the call would do */
        angles = &slot->angles.x;
        njSetMatrix(NULL, &finalhazard_panel_bases[slot->base]);
        njTranslateEx(&slot->pos);
        njRotateEx(angles, 0);
        njCalcPoint(NULL, &origin, &world);
        njCalcPoint(&finalhazard_joint_inverse, &world, &work->pos);
        /* @bug logic: turns panel_front by the slot's matrix, the view's turn
         * included, and adds it to pos, a world point. so the front lies
         * panel_front's length off pos in a direction that turns with the
         * camera. the turn is at 0x8c506648 */
        offset = panel_front;
        njCalcVector(NULL, &offset, &direction);
        vector_sum(&work->pos, &direction, &panel->front);
        njPopMatrixEx();
    }
}

static void panel_check_hit(struct task *task)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    struct collision_hit *hit;
    struct panel_slot *slot;
    NJS_VECTOR direction;
    if (!(work->flags & WORK_HURT)) {
        hit = collision_next_player_hit(task);
        if (hit && collision_get_guard(task, 0) <
                       collision_get_attack(hit->task, hit->hit_shape)) {
            vector_diff(&player_work[finalhazard_players->active]->pos,
                        &work->pos, &direction);
            njUnitVector(&direction);
            /* @hack redundant: x and z scaled by 5, which
             * finalhazard_push_player's scaling to the speed undoes */
            direction.x *= 5.0F;
            direction.y = 0.0F;
            direction.z *= 5.0F;
            finalhazard_push_player(&direction, FINALHAZARD_PUSH_SPEED);
            collision_set_attack(task, 0, SAFE_ATTACK);
            collision_set_guard(task, 0, SAFE_GUARD);
            slot = &finalhazard_panel_slots[panel->row][panel->col];
            finalhazard_hit_sparks(task, &panel->front, &slot->angles);
            work->flags |= WORK_HURT;
            work->timer = HIT_STEPS;
            sound_play(SOUND_WEAK_POINT_HIT, NULL, 0, 0);
            sound_set_volume(NULL, SOUND_WEAK_POINT_HIT, SOUND_LOUDEST);
            work->mode = PANEL_HIT;
        }
    }
}

#define LIMIT_VOLUME(volume, maximum)                                          \
    do {                                                                       \
        if ((volume) < LEAST_VOLUME)                                           \
            (volume) = LEAST_VOLUME;                                           \
        if ((volume) > (maximum))                                              \
            (volume) = (maximum);                                              \
    } while (0)

static void panel_grow(struct task *task)
{
    struct panel *panel = task->extra.p;
    struct task_work *work = task->work;
    Sint16 volume;
    panel->fade += grow_step;
    work->scl.y += grow_step;
    if (work->scl.y >= 1.0F) {
        work->scl.y = 1.0F;
    }
    position_panel(task, panel->row, panel->col);
    volume = HUM_VOLUME + (panel->fade * HUM_PER_FADE);
    LIMIT_VOLUME(volume, HUM_VOLUME);
    sound_play_at_for(SOUND_WEAK_POINT_HUM, task, 1, volume, HUM_DURATION,
                      &work->pos);
    if (panel->fade >= FADE_SHOWN) {
        panel->fade = FADE_SHOWN;
        work->ang.x = 0;
        work->scl.y = 1.0F;
        work->scl.z = 1.0F;
        work->scl.x = 1.0F;
        panel->wobble.x = 0;
        panel->wobble.y = 0;
        panel->wobble.z = QUARTER_TURN;
        panel->swing = 0;
        panel->amplitude = 0.0F;
        collision_set_attack(task, 0, ARMED_ATTACK);
        collision_set_guard(task, 0, ARMED_GUARD);
        work->mode = PANEL_ACTIVE;
        work->submode = 0;
    }
}

/* move the phase angle by base plus up to range, wrapped past FULL_TURN */
#define ADVANCE(angle, base, range)                                            \
    do {                                                                       \
        (angle) += (base) + ((Float)rand() * ((range) * RAND_SCALE));          \
        if ((angle) > FULL_TURN)                                               \
            (angle) -= FULL_TURN;                                              \
    } while (0)
/* a showing weak point's wobble. fade pulses by SHOWN_PULSE. x and z scale by
 * SHOWN_SIDE either way, and y by the amplitude. the amplitude is
 * SHOWN_AMPLITUDE plus SHOWN_SWING times the sine of a random angle. that
 * angle lies from SWING_LEAST up to twice that. each axis's phase moves by its
 * _STEP plus up to its _SPREAD each step */
#define SHOWN_PULSE .1F
#define SHOWN_SIDE .01F
#define SHOWN_AMPLITUDE .075F
#define SHOWN_SWING .025F
#define SWING_LEAST 288.0F
#define SHOWN_X_STEP 192.0F
#define SHOWN_X_SPREAD 96.0F
#define SHOWN_Y_STEP 352.0F
#define SHOWN_Y_SPREAD 320.0F
#define SHOWN_Z_STEP 160.0F
#define SHOWN_Z_SPREAD 144.0F
/* a hit's shake. fade pulses by SHAKE_PULSE, and x and z scale by SHAKE_SIDE
 * either way. y's scale falls by SHAKE_SQUASH each step of the shake. each
 * axis's phase moves as the wobble's do, faster. once the shake ends, fade
 * falls by shrink_step each step. the weak point shrinks once fade is below
 * SHAKE_END_FADE */
#define SHAKE_PULSE .15F
#define SHAKE_SIDE .1F
#define SHAKE_SQUASH .00777F
#define SHAKE_X_STEP 896.0F
#define SHAKE_X_SPREAD 1536.0F
#define SHAKE_Y_STEP 1600.0F
#define SHAKE_Y_SPREAD 2048.0F
#define SHAKE_Z_STEP 512.0F
#define SHAKE_Z_SPREAD 832.0F
#define SHAKE_END_FADE .05F

static void panel_wobble(struct task *task)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    panel->fade = FADE_SHOWN + (njSin(work->ang.x) * SHOWN_PULSE);
    work->ang.x += PULSE_STEP;
    position_panel(task, panel->row, panel->col);
    panel->swing = SWING_LEAST + ((Float)rand() * (SWING_LEAST * RAND_SCALE));
    /* @hack redundant: the wrap of swing, which is below twice SWING_LEAST
     * and never needs it */
    if (panel->swing > FULL_TURN) {
        panel->swing -= FULL_TURN;
    }
    panel->amplitude = SHOWN_AMPLITUDE + (njSin(panel->swing) * SHOWN_SWING);
    ADVANCE(panel->wobble.y, SHOWN_Y_STEP, SHOWN_Y_SPREAD);
    ADVANCE(panel->wobble.x, SHOWN_X_STEP, SHOWN_X_SPREAD);
    ADVANCE(panel->wobble.z, SHOWN_Z_STEP, SHOWN_Z_SPREAD);
    work->scl.x = 1.0F + (njSin(panel->wobble.x) * SHOWN_SIDE);
    work->scl.y = 1.0F + (njSin(panel->wobble.y) * panel->amplitude);
    work->scl.z = 1.0F + (njCos(panel->wobble.z) * SHOWN_SIDE);
    sound_play_at_for(SOUND_WEAK_POINT_HUM, task, 1, HUM_VOLUME, HUM_DURATION,
                      &work->pos);
    panel_proximity(task);
}

static void panel_shrink(struct task *task)
{
    struct panel *panel = task->extra.p;
    struct task_work *work = task->work;
    struct collision_shape *shape;
    Sint16 volume;
    panel->fade -= shrink_step;
    work->scl.y -= shrink_step;
    if (work->scl.y < 0.0F) {
        work->scl.y = 0.0F;
    }
    volume = SHRINK_VOLUME + (panel->fade * HUM_PER_FADE);
    LIMIT_VOLUME(volume, SHRINK_VOLUME);
    sound_play_at_for(SOUND_WEAK_POINT_HUM, task, 1, volume, HUM_DURATION,
                      &work->pos);
    position_panel(task, panel->row, panel->col);
    if (panel->fade <= FADE_HIDDEN) {
        panel->fade = FADE_HIDDEN;
        work->scl.y = 0.0F;
        work->scl.x = 1.0F;
        work->scl.z = 1.0F;
        panel->amplitude = 0.0F;
        panel->swing = 0;
        if (panel->next_col >= 0) {
            shape = collision_get_shape(task, 0);
            shape->flags &= ~SHAPE_OFF;
            panel->row = panel->next_row;
            panel->col = panel->next_col;
            panel->next_col = NO_SLOT;
            panel->next_row = NO_SLOT;
            finalhazard_camera_blend();
            work->mode = PANEL_GROW;
            work->submode = 0;
        } else {
            shape = collision_get_shape(task, 0);
            shape->flags |= SHAPE_OFF;
            work->mode = PANEL_HIDDEN;
            work->submode = 0;
        }
        finalhazard_panel_rearm(task);
    }
}

static void panel_recoil(struct task *task)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    Sint16 volume;
    panel->fade = FADE_SHOWN + (njSin(work->ang.x) * SHAKE_PULSE);
    volume = HUM_VOLUME - (panel->fade * HUM_PER_FADE);
    LIMIT_VOLUME(volume, HUM_VOLUME);
    sound_play_at_for(SOUND_WEAK_POINT_HUM, task, 1, volume, HUM_DURATION,
                      &work->pos);
    work->ang.x += PULSE_STEP;
    position_panel(task, panel->row, panel->col);
    ADVANCE(panel->wobble.y, SHAKE_Y_STEP, SHAKE_Y_SPREAD);
    ADVANCE(panel->wobble.x, SHAKE_X_STEP, SHAKE_X_SPREAD);
    ADVANCE(panel->wobble.z, SHAKE_Z_STEP, SHAKE_Z_SPREAD);
    work->scl.x = 1.0F + (njSin(panel->wobble.x) * SHAKE_SIDE);
    work->scl.y = 1.0F - ((Float)(HIT_STEPS - work->timer) * SHAKE_SQUASH);
    work->scl.z = 1.0F + (njCos(panel->wobble.z) * SHAKE_SIDE);
    if (work->timer > 0) {
        work->timer--;
    } else {
        /* @hack spelling: shrink_step read through *&, where shrink_step
         * would do */
        panel->fade -= *&shrink_step;
        if (panel->fade < SHAKE_END_FADE) {
            work->mode = PANEL_SHRINK;
            work->submode = 0;
        }
    }
}

static void panel_step(struct task *task)
{
    struct task_work *work = task->work;
    if (!game_state.time_stopped) {
        switch (work->mode) {
        case PANEL_START:
            break;
        case PANEL_GROW:
            panel_grow(task);
            break;
        case PANEL_ACTIVE:
            panel_wobble(task);
            break;
        case PANEL_SHRINK:
            panel_shrink(task);
            break;
        case PANEL_HIDDEN:
            break;
        case PANEL_HIT:
            panel_recoil(task);
            break;
        }
        panel_check_hit(task);
        collision_register(task);
    }
}

static void panel_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    struct panel_slot *slot;
    Angle *angles;
    draw_attr_save();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_OFFSET_MATERIAL |
                     NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_set(1, NJD_FST_UA);
    light_apply(LAST_BOSS_EFFECT_LIGHT);
    constant_material_set(panel->fade, 1, 1, 1);
    njPushMatrixEx();
    slot = &finalhazard_panel_slots[panel->row][panel->col];
    /* @hack spelling: angles, a pointer to slot->angles taken before the
     * matrix loads, where &slot->angles.x in the call would do */
    angles = &slot->angles.x;
    njSetMatrix(NULL, &finalhazard_panel_bases[slot->base]);
    njTranslateEx(&slot->pos);
    njRotateEx(angles, 0);
    njScale(NULL, work->scl.x, work->scl.y, work->scl.z);
    njSetTexture(&finalhazard_model_texlist);
    njCnkSimpleDrawObject(&finalhazard_panel_model);
    njPopMatrixEx();
    constant_material_set(0, 0, 0, 0);
    draw_attr_restore();
    draw_control_restore();
    light_apply(light_after_draw);
}

static void panel_destroy(struct task *task)
{
}

struct task *finalhazard_panel_create(struct task *parent, Sint8 row)
{
    struct task *task;
    struct task_work *work;
    struct panel *panel;
    struct collision_shape *shape;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, panel_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = panel_draw;
    task->destructor = panel_destroy;
    work = task->work;
    panel = syCalloc(1, sizeof(*panel));
    if (!panel) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = panel;
    work->mode = PANEL_START;
    panel->row = row;
    panel->fade = FADE_HIDDEN;
    work->scl.y = 0;
    work->scl.x = 1;
    work->scl.z = 1;
    collision_init(task, &panel_shape, 1, COLLISION_ENEMY);
    /* @bug memory: writes the guard through an invalid shape at 0x8c507022
     * where collision_init, at 0x8c507008, couldn't allocate the record.
     * collision_get_shape, at 0x8c50701a, then has no shape to give */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. it destroys the task and returns
     * null where collision_init made no record */
    if (!work->collision) {
        task_destroy(task);
        return NULL;
    }
#endif
    collision_set_radius(task, RECORD_RADIUS);
    shape = collision_get_shape(task, 0);
    shape->levels =
        (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(SAFE_GUARD);
    return task;
}

void finalhazard_panel_hide(struct task *task)
{
    struct panel *panel = task->extra.p;
    struct task_work *work = task->work;
    panel->next_col = NO_SLOT;
    panel->next_row = NO_SLOT;
    finalhazard_panel_rearm(task);
    work->mode = PANEL_SHRINK;
    work->submode = 0;
}

void finalhazard_panel_move(struct task *task, Sint8 row)
{
    struct task_work *work = task->work;
    struct panel *panel = task->extra.p;
    Sint8 next_row;
    Sint8 next_col;
    struct collision_shape *shape;
    finalhazard_panel_rearm(task);
    if (row != panel->row) {
        next_row = row;
        next_col = choose_column(NO_SLOT);
    } else {
        next_col = choose_column(panel->col);
        next_row = row;
    }
    if (work->mode == PANEL_ACTIVE) {
        panel->next_row = next_row;
        panel->next_col = next_col;
        work->mode = PANEL_SHRINK;
        work->submode = 0;
    } else if (work->mode == PANEL_SHRINK) {
        panel->col = next_col;
        panel->row = next_row;
        /* @hack redundant: the test below that the mode isn't PANEL_SHRINK,
         * which the branch above rules out */
    } else if (work->mode != PANEL_SHRINK) {
        panel->col = next_col;
        panel->row = next_row;
        work->scl.x = 1.0F;
        work->scl.z = 1.0F;
        shape = collision_get_shape(task, 0);
        shape->flags &= ~SHAPE_OFF;
        finalhazard_camera_blend();
        work->mode = PANEL_GROW;
        work->submode = 0;
    }
    collision_set_attack(task, 0, SAFE_ATTACK);
    collision_set_guard(task, 0, SAFE_GUARD);
}

int finalhazard_panel_hit(struct task *task)
{
    struct task_work *work;
    if (!task) {
        return -1;
    }
    work = task->work;
    /* @hack spelling: 1 and 0 returned by an if, where returning the flag's
     * test would do */
    if (work->flags & WORK_HURT) {
        return 1;
    }
    return 0;
}

void finalhazard_panel_rearm(struct task *task)
{
    struct task_work *work;
    if (task) {
        work = task->work;
        collision_set_attack(task, 0, ARMED_ATTACK);
        collision_set_guard(task, 0, ARMED_GUARD);
        work->flags &= ~WORK_HURT;
    }
}

int finalhazard_panel_cannon(struct task *task)
{
    struct panel *panel;
    if (!task) {
        return -1;
    }
    panel = task->extra.p;
    return slot_cannons[panel->row][panel->col];
}

void finalhazard_show_weak_points(struct task *parent)
{
    struct task *task;
    struct panel *panel;
    Sint8 row;
    Sint8 col;
    for (row = 0; row < PANEL_ROWS; row++) {
        for (col = 0; col < PANEL_COLUMNS; col++) {
            task = finalhazard_panel_create(parent, row);
            /* @bug memory: goes through a null task at 0x8c5071be and
             * 0x8c5071d6 where finalhazard_panel_create returns null from its
             * test at 0x8c506f42 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it returns at the first
             * weak point that finalhazard_panel_create can't make */
            if (!task) {
                return;
            }
#endif
            task->work->mode = PANEL_GROW;
            panel = task->extra.p;
            panel->col = col;
        }
    }
}

void finalhazard_panel_spark_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    njAddVector(&motion->spd, &motion->acc);
    njAddVector(&work->pos, &motion->spd);
    {
        /* @hack spelling: drag, a pointer to the field, where the field in
         * each product would do */
        const Float *drag = &finalhazard_spark_tuning.drag;
        motion->acc.x *= *drag;
        motion->acc.y *= *drag;
        motion->acc.z *= *drag;
    }
    if (SPARK_SIZE(work) < finalhazard_spark_tuning.size_limit) {
        SPARK_SIZE(work) += SPARK_GROWTH(work);
    }
    if (work->timer == 0) {
        SPARK_BRIGHTNESS(work) -= finalhazard_spark_tuning.fade;
        if (SPARK_BRIGHTNESS(work) < SPARK_DARK) {
            task_kill(task);
        }
    } else {
        work->timer--;
    }
}
