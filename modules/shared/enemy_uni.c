/* the uni, E UNI in a stage's object list: a rolling ball that four spikes
 * circle */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_uni.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* a uni's work->mode */
enum uni_mode {
    MODE_SETUP,
    MODE_ROLL, /* the uni rolls along its heading */
    MODE_TURN, /* the uni stands, and turns toward aim_y */
    MODE_CHASE, /* the uni rolls, and turns toward the player it found */
    MODE_DEFEATED /* a hit beat the uni, and the next step breaks it */
};

/* a spike's work->mode. nothing sets a UNIUNI spike's mode to
 * MODE_SPIKE_BREAK */
enum spike_mode {
    MODE_SPIKE_CIRCLE,
    MODE_SPIKE_BREAK /* the spike's next step ends it */
};

/* in the uni's work flags: a hit beat the uni. at its next step, each spike
 * bursts and ends */
#define UNI_BEATEN 0x1000

/* the uni's uni_kind, in its work's submode */
#define KIND(work) ((work)->submode)
/* the bits of ang.y that hold the uni's heading. its low byte holds the
 * uni's group */
#define HEADING_BITS 0xff00

/* the steps since the uni last sounded its spikes, in its enemy_work */
#define SOUND_STEPS(enemy) ((enemy)->unk_04e)
/* what a GHORA spike's draw adds to its strips' own v, in its enemy_work */
#define SCROLL_V(enemy) ((enemy)->unk_050)

/* the uni's range is scl.z + RANGE. near_distance2 and sight_distance2 hold
 * the range squared */
#define RANGE 80.0F
/* a new heading aims at a random point of a square about home, AIM_SPREAD
 * of the range across */
#define AIM_SPREAD 0.2F
/* spd.x is the uni's speed along its heading, and spd.z what each step adds
 * to it. uni_roll sets spd.z to ROLL_ACCEL. it caps spd.x at ROLL_SPEED
 * before the add, so the uni rolls at up to ROLL_SPEED + ROLL_ACCEL.
 * ROLL_SPEED_MAX bounds spd.x either way */
#define ROLL_ACCEL 0.02F
#define ROLL_SPEED 0.4F
#define ROLL_SPEED_MAX 3.4F
/* each step takes GRAVITY from spd.y, and DRAG times spd.y */
#define GRAVITY 0.4F
#define DRAG 0.01F
/* a turn moves ang.y TURN_SPEED a step toward aim_y. it ends once the turn
 * left is below TURN_DONE */
#define TURN_SPEED 0x180
#define TURN_DONE 0x800
/* the radii the setups give the uni's and the spikes' collision records, in
 * place of collision_init's. a spike's setup gives SPIKE_FIRST_RECORD_RADIUS,
 * then SPIKE_RECORD_RADIUS */
#define UNI_RECORD_RADIUS 5.0F
#define SPIKE_FIRST_RECORD_RADIUS 3.0F
#define SPIKE_RECORD_RADIUS 10.0F
/* the points score_add_enemy adds for a beaten uni: SCORE, or SCORE_KNOCK
 * where player_touch_attacks returns TRUE */
#define SCORE 100
#define SCORE_KNOCK 200
/* the ball a magic hand makes of a beaten uni grows to BALL_SCALE */
#define BALL_SCALE 0.25F
/* the index in lights of the light the uni and its spikes draw in */
#define UNI_LIGHT 8
/* while the uni has a spike, it sounds its kind's spike sound, and starts
 * SOUND_STEPS over at 0. a UNIUNI sounds at the step that finds the count
 * past UNIUNI_SOUND_AFTER, every 52 steps. a GHORA sounds at the step that
 * finds it at GHORA_SOUND_AT, every 43 */
#define UNIUNI_SPIKE_SOUND 0x4008
#define UNIUNI_SOUND_AFTER 50
#define GHORA_SPIKE_SOUND 0x4009
#define GHORA_SOUND_AT 42
/* the scale of the burst a spike leaves as it breaks or as the uni is
 * beaten */
#define BURST_SCALE 6.0F
/* a spike circles SPIKE_RADIUS from the uni. a GHORA's moves toward
 * GHORA_REACH while the uni chases, and back otherwise, by GHORA_REACH_STEP a
 * step */
#define SPIKE_RADIUS 12.0F
#define GHORA_REACH 50.0F
#define GHORA_REACH_STEP 0.1F
/* the count of the uni's spikes, and how far a spike circles in a step, in
 * ang.y */
#define SPIKES 4
#define UNIUNI_SPIKE_TURN 0x280
#define GHORA_SPIKE_TURN 0x180
/* the tilt of a UNIUNI's spikes, about x */
#define SPIKE_TILT 0x1c00
/* the frames a UNIUNI spike's motion moves in a step */
#define SPIKE_FRAME_STEP 0.08F
/* what SCROLL_V gains in a step */
#define GHORA_SCROLL 24
/* ghora_spike_polygons is an array of halfwords. a strip there is its
 * length, then its vertices. a vertex is three halfwords: its index, u and v.
 * the draw adds the scroll to each v of the spike's first three strips.
 * GHORA_SPIKE_FIRST_V is the index of the first of those v's. NEXT_V steps
 * from a v to the next vertex's, and NEXT_STRIP_V to the next strip's first */
#define GHORA_SPIKE_FIRST_V 14
#define NEXT_V 3
#define NEXT_STRIP_V (NEXT_V + 1)
/* a GHORA spike's tick runs its glow, which shows while tick is below
 * GLOW_STEPS. the glow shows ghora_texlist's textures from GLOW_FIRST_TEXTURE
 * on, each for 1 << GLOW_TEXTURE_SHIFT steps. at GLOW_STEPS, tick skips ahead
 * by GLOW_SKIP plus up to GLOW_SKIP_RANGE at random. the glow then stays dark
 * until tick, a Uint8, wraps to 0 */
#define GLOW_STEPS 16
#define GLOW_FIRST_TEXTURE 4
#define GLOW_TEXTURE_SHIFT 1
#define GLOW_SKIP 176.0F
#define GLOW_SKIP_RANGE 48.0F
/* the glow's scale, and its height above the spike's pos */
#define GLOW_SCALE 0.18F
#define GLOW_HEIGHT 2.5F
/* each step of a chase makes a white puff with a chance of PUFF_CHANCE. it
 * leaves from one of the PUFFS points of uni_puff_points, along that point's
 * direction, at PUFF_SPEED */
#define PUFFS 6
#define PUFF_CHANCE 0.6F
#define PUFF_SPEED 0.4F
#define PUFF_SCALE 0.11F

/* the kinds' textures and models lie ahead of the shapes in the object's
 * data */
/* clang-format off */
#include "shared/enemy_uni/text.inc"
#include "shared/uni_art/uniuni.h"
#include "shared/uni_art/ghora.h"

static struct collision_shape uni_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* the pieces a beaten uni breaks into, null-ended, for enemy_break */
static NJS_CNK_MODEL *uniuni_pieces[4] = {
#include "shared/enemy_uni/uniuni_pieces.inc"
};
static NJS_CNK_MODEL *ghora_pieces[4] = {
#include "shared/enemy_uni/ghora_pieces.inc"
};

/* the task a uni makes as it loads its kind's textures. each is null while
 * its kind's textures aren't loaded */
static struct task *uniuni_texture_task = NULL;
static struct task *ghora_texture_task = NULL;

/* the kinds' titles, by uni_kind */
/* Preserve the original Uniuni and Ghora readout titles. */
static char *uni_titles[UNI_KINDS] = {
#include "shared/enemy_uni/uni_titles.inc"
};

/* where a chase's puffs leave the uni, in its axes as drawn, and the
 * direction of each */
KEPT static NJS_POINT3 uni_puff_points[PUFFS] = {
#include "shared/enemy_uni/uni_puff_points.inc"
};
KEPT static NJS_VECTOR uni_puff_directions[PUFFS] = {
#include "shared/enemy_uni/uni_puff_directions.inc"
};

/* clang-format off */
static struct collision_shape uniuni_spike_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_UNK_800000 | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape ghora_spike_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_UNK_800000 | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* the glow a GHORA spike shows above itself. the draw sets its texture
 * list, texture, scale and point */
/* @hack type: ghora_glow_cell as an array of one, where the cell alone would
 * do */
static NJS_TEXANIM ghora_glow_cell[1] = {
#include "shared/enemy_uni/ghora_glow_cell.inc"
};
static NJS_SPRITE ghora_glow = {
#include "shared/enemy_uni/ghora_glow.inc"
};

static void uni_draw(struct task *task);
static void uni_destroy(struct task *task);
static void uniuni_spikes_create(struct task *parent);
static void ghora_spikes_create(struct task *parent);

static void uniuni_texture_destroy(struct task *task)
{
    njReleaseTexture(&uniuni_texlist);
    uniuni_texture_task = NULL;
}

#ifdef NONMATCHING
/* this side gives ghora's holder a destroy of its own, which uni_setup's side
 * sets: it releases ghora_texlist and empties ghora_texture_task */
static void ghora_texture_destroy(struct task *task)
{
    njReleaseTexture(&ghora_texlist);
    ghora_texture_task = NULL;
}
#endif

static void uni_texture_step(struct task *task)
{
}

static void uni_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    struct angles ang;

    enemy = enemy_work_alloc_grouped(task);
    KIND(work) = njRoundOff(work->scl.x);
    if (KIND(work) >= UNI_KINDS || KIND(work) < 0) {
        KIND(work) = UNI_UNIUNI;
    }
    collision_init(task, uni_shapes, 1, COLLISION_ENEMY);
    collision_set_radius(task, UNI_RECORD_RADIUS);
    work->mode = MODE_TURN;
    work->timer = 0;
    enemy->flags |= ENEMY_HOLD_AT_LEDGE | ENEMY_TILT | ENEMY_RELEASES_ANIMAL;
    enemy->shadow_size = 6.0F;
    enemy->shadow_ratio = 1.0F;
    enemy->sight_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = 0.0F;
    enemy->spd.z = 0.0F;
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 10.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = 12.0F;
    enemy->radius = 18.0F;
    enemy->bottom = -20.0F;
    enemy->friction = 1.0F;
    enemy->floor_bounce = 0.2F;
    enemy->turn_speed = TURN_SPEED;
    enemy->aim_y = work->ang.y;
    enemy_set_home(work, enemy);
    work->pos.y =
        surface_probe_floor(work->pos.x, work->pos.y, work->pos.z, &ang);
    work->ang.z = 0;
    work->ang.x = 0;
    if (KIND(work) == UNI_UNIUNI) {
        enemy->motion.object = &uniuni_object;
    } else {
        enemy->motion.object = &ghora_object;
    }
    enemy->flags |= ENEMY_BURST;
    if (KIND(work) == UNI_GHORA) {
        if (ghora_texture_task == NULL) {
            texture_load_pvm(UNI_GHORA_TEXTURE_ARCHIVE, &ghora_texlist);
            ghora_texture_task =
                task_create(0, 0, uni_texture_step, UNI_TEXTURE_TASK_NAME);
            /* @bug memory: nothing releases ghora_texlist. ghora_texture_task
             * takes uniuni_texture_destroy as its destructor (MOV.L at 0x172
             * of the object's code). that destroy releases uniuni_texlist and
             * empties uniuni_texture_task. ghora_texture_task still points to
             * the freed task */
#ifdef NONMATCHING
            /* this side repairs the tag above: the holder takes
             * ghora_texture_destroy */
            ghora_texture_task->destructor = ghora_texture_destroy;
#else
            ghora_texture_task->destructor = uniuni_texture_destroy;
#endif
        }
    } else {
        if (uniuni_texture_task == NULL) {
            texture_load_pvm(UNI_UNIUNI_TEXTURE_ARCHIVE, &uniuni_texlist);
            uniuni_texture_task =
                task_create(0, 0, uni_texture_step, UNI_TEXTURE_TASK_NAME);
            uniuni_texture_task->destructor = uniuni_texture_destroy;
        }
    }
    task->draw = uni_draw;
    task->destructor = uni_destroy;
    if (KIND(work) == UNI_UNIUNI) {
        uniuni_spikes_create(task);
    } else {
        ghora_spikes_create(task);
    }
}

static void uni_pick_heading(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    Float range;

    range = njSqrt(enemy->near_distance2);
    pos.x =
        enemy->home.x + (range * ((rand() * RAND_SCALE) - 0.5F) * AIM_SPREAD);
    pos.z =
        enemy->home.z + (range * ((rand() * RAND_SCALE) - 0.5F) * AIM_SPREAD);
    enemy->aim_y = (Angle)(atan2f(pos.x - work->pos.x, pos.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
}

/* the uni turns toward a new heading once it strays out of range or touches
 * an object. its x and z are then set back to last_pos's */
static void uni_roll(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack redundant: player, which only the test below reads */
    struct task_work *player = player_work[0];
    struct collision_work *collision;
    struct collision_work *hit;
    Float distance;

    /* @hack redundant: a test of player's mode, in a block no path reaches */
    if (0) {
        if (player->mode) {
        }
    }
    collision = work->collision;
    /* @hack spelling: the hit read again through work->collision, where
     * collision would do */
    hit = work->collision->hit;
    enemy->spd.z = ROLL_ACCEL;
    if (enemy->spd.x > ROLL_SPEED) {
        enemy->spd.x = ROLL_SPEED;
    }
    distance = enemy_home_distance2(work, enemy);
    /* @bug logic: no ledge turns the uni. this test of ENEMY_LEDGE_HOLD_OFF
     * never passes (TST at 0x338 of the object's code). enemy_meet_stage's
     * hold at a ledge sets the flag, but uni_step clears it at the end of the
     * same step (AND at 0xb82 of the object's code) */
    if ((enemy->flags & ENEMY_LEDGE_HOLD_OFF) ||
        distance > enemy->near_distance2 ||
        ((collision->flags & COLLISION_TOUCHED) &&
         hit->kind == COLLISION_OBJECT)) {
        uni_pick_heading(work, enemy);
        work->mode = MODE_TURN;
        work->timer = 0;
        work->pos.x = enemy->last_pos.x;
        work->pos.z = enemy->last_pos.z;
    }
}

static void uni_turn(struct task_work *work, struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        enemy->spd.x = 0.0F;
        work->ang.y = angle_approach(work->ang.y, enemy->aim_y, TURN_SPEED);
        /* @bug logic: the turn ends at its first step where aim_y lies the
         * negative way round from ang.y. the test takes angle_difference's
         * signed turn, and any turn below 0 passes it (CMP/GE at 0x3bc of
         * the object's code) */
        if (angle_difference(work->ang.y, enemy->aim_y) < TURN_DONE) {
            work->mode = MODE_ROLL;
            work->timer = 0;
        }
    }
}

static void uni_chase(struct task_work *work, struct enemy_work *enemy)
{
    int found;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((found = enemy_find_player(work, enemy)) == 0) {
        work->mode = MODE_ROLL;
        work->timer = 0;
        return;
    }
    enemy_turn_to_player(work, enemy, found - 1);
}

static void uni_break(struct task_work *work, struct enemy_work *enemy)
{
    if (KIND(work) == UNI_UNIUNI) {
        enemy_break(work, uniuni_pieces, &uniuni_texlist);
    } else {
        enemy_break(work, ghora_pieces, &ghora_texlist);
    }
}

static void uni_check_hit(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    NJS_VECTOR offset;
    NJS_VECTOR scl;
    int score;
    int player;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_DEFEATED;
        enemy->unk_09c = 0.35F;
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
        work->flags |= UNI_BEATEN;
        if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
            player = enemy->shot_by;
            offset.x = 0.0F;
            offset.y = 0.0F;
            offset.z = 0.0F;
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            switch (KIND(work)) {
            case UNI_UNIUNI:
                enemy_ball_create(player, &uniuni_object, &uniuni_texlist, &scl,
                                  &offset);
                break;
            case UNI_GHORA:
                enemy_ball_create(player, &ghora_object, &ghora_texlist, &scl,
                                  &offset);
                break;
            }
        }
    }
}

static void uni_check_players(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local distance, where enemy_home_distance2's result
     * compared in place would do */
    Float distance;

    distance = enemy_home_distance2(work, enemy);
    /* @hack spelling: the empty arm and the else, where one test of both
     * would do */
    if (distance > enemy->near_distance2) {
    } else if (enemy_find_player(work, enemy)) {
        work->timer = 0;
        work->mode = MODE_CHASE;
    }
}

static void uni_point_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

static void uni_vector_to_world(struct task_work *work, NJS_VECTOR *vec)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCalcVector(NULL, vec, vec);
    njPopMatrixEx();
}

static void uni_move(struct task_work *work, struct enemy_work *enemy)
{
    Float last = enemy->spd.x;

    if (!game_state.time_stopped) {
        if (enemy->spd.x < 0.0F && enemy->spd.z < 0.0F) {
            enemy->spd.z = -enemy->spd.z;
        }
        enemy->spd.x += enemy->spd.z;
        if (enemy->spd.x < 0.0F && last > 0.0F) {
            enemy->spd.x = 0.0F;
        }
        if (enemy->spd.x > ROLL_SPEED_MAX) {
            enemy->spd.x = ROLL_SPEED_MAX;
        }
        if (enemy->spd.x < -ROLL_SPEED_MAX) {
            enemy->spd.x = -ROLL_SPEED_MAX;
        }
        enemy->spd.y += -GRAVITY - (enemy->spd.y * DRAG);
        work->pos.x += enemy->spd.x * njCos(work->ang.y);
        work->pos.y += enemy->spd.y;
        work->pos.z -= enemy->spd.x * njSin(work->ang.y);
    }
}

static void uni_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles ang;
    NJS_VECTOR scl;
    int kind;

    light_apply(UNI_LIGHT);
    /* @hack redundant: a block no path reaches */
    if (0) {
        ang.z = 0;
    }
    if (KIND(work) == UNI_GHORA) {
        njSetTexture(&ghora_texlist);
    } else {
        njSetTexture(&uniuni_texlist);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCnkSimpleDrawObject(enemy->motion.object);
    njPopMatrixEx();
    /* @hack redundant: an empty loop that never runs */
    while (0) {
    }
    enemy_draw_shadow(work, enemy);
    /* @hack redundant: a readout no path reaches, kept for its strings and
     * the stretch of its code that SHC still places in the object */
    if (0) {
        /* @hack redundant: stand-ins for the readout's first lines, which the
         * image holds no code for, kept for the stack frame they give the
         * draw */
        kind = KIND(work);
        if (kind >= UNI_KINDS || kind < 0) {
            kind = UNI_UNIUNI;
        }
        njPrintC(NJM_LOCATION(4, 16), uni_titles[kind]);
        switch (kind) {
        case UNI_UNIUNI:
        case UNI_GHORA:
            njPrintC(NJM_LOCATION(17, 19), UNI_DEBUG_KIND);
            njPrintC(NJM_LOCATION(17, 21), UNI_DEBUG_RANGE);
            break;
        }
        njPrintC(NJM_LOCATION(17, 17), UNI_DEBUG_YAW);
        njPrintH(NJM_LOCATION(26, 17), work->ang.y & HEADING_BITS, 4);
        njPrintC(NJM_LOCATION(30, 17), UNI_DEBUG_EMERALD);
        njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
        /* @hack spelling: one assignment to the three, and one to scl.z and
         * scl.x, where a statement for each would do */
        ang.x = ang.y = ang.z = 0;
        scl.z = scl.x = work->scl.z + RANGE;
        scl.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

static void uni_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* the part of the step that every live mode runs */
static void uni_act(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    uni_move(work, enemy);
    enemy_meet_stage(work, enemy);
    uni_check_hit(task, work, enemy);
    if (task->child) {
        if (KIND(work) == UNI_UNIUNI) {
            if (SOUND_STEPS(enemy)++ > UNIUNI_SOUND_AFTER) {
                sound_play_at(UNIUNI_SPIKE_SOUND, NULL, 0, 0, &work->pos);
                SOUND_STEPS(enemy) = 0;
            }
        } else if (KIND(work) == UNI_GHORA) {
            if (SOUND_STEPS(enemy)++ >= GHORA_SOUND_AT) {
                sound_play_at(GHORA_SPIKE_SOUND, NULL, 0, 0, &work->pos);
                SOUND_STEPS(enemy) = 0;
            }
        }
    }
}

static void uni_puff(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float chance;
    int i;

    chance = rand() * RAND_SCALE;
    if (chance < PUFF_CHANCE) {
        /* @hack redundant: an empty block no path reaches */
        if (0) {
        }
        /* chance lies below PUFF_CHANCE, and njRoundOff truncates, so i
         * lies below PUFFS */
        chance *= PUFFS / PUFF_CHANCE;
        i = njRoundOff(chance);
        pos = uni_puff_points[i];
        uni_point_to_world(work, &pos);
        spd = uni_puff_directions[i];
        uni_vector_to_world(work, &spd);
        njUnitVector(&spd);
        spd.x *= PUFF_SPEED;
        spd.y *= PUFF_SPEED;
        spd.z *= PUFF_SPEED;
        white_breath_short_create(&pos, &spd, PUFF_SCALE);
    }
}

void uni_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (work->mode != MODE_SETUP && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SETUP:
        uni_setup(task);
        return;
    case MODE_ROLL:
        uni_roll(work, enemy);
        uni_check_players(work, enemy);
        uni_act(task, work, enemy);
        break;
    case MODE_TURN:
        uni_turn(work, enemy);
        uni_check_players(work, enemy);
        uni_act(task, work, enemy);
        break;
    case MODE_CHASE:
        uni_roll(work, enemy);
        uni_chase(work, enemy);
        uni_act(task, work, enemy);
        uni_puff(work, enemy);
        break;
    case MODE_DEFEATED:
        uni_break(work, enemy);
        placement_kill(task);
        return;
    }
    work->timer++;
    /* while the uni turns, players, enemies and objects can't push it */
    if (work->mode == MODE_TURN) {
        work->collision->shapes->push |= PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
    } else {
        work->collision->shapes->push &= ~PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
    }
    enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
    if (!game_state.time_stopped) {
        enemy_save_pos(work, enemy);
    }
    collision_register(task);
}

/* scl.x is the spike's distance from the uni, ang.y its heading from the
 * uni and ang.x its tilt */
static void uniuni_spike_place(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *parent = task->parent->work;
    NJS_VECTOR pos;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    pos.x = 0.0F;
    pos.y = 0.0F;
    pos.z = work->scl.x;
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &pos, &pos);
    njAddVector(&pos, &parent->pos);
    work->pos = pos;
    njPopMatrixEx();
    work->ang.y += UNIUNI_SPIKE_TURN;
}

/* scl.y is the frame of the spike's motion */
static void uniuni_spike_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_VECTOR scl;
    struct angles ang;

    light_apply(UNI_LIGHT);
    njSetTexture(&uniuni_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njCnkSimpleDrawMotion(&uniuni_spike_object, &uniuni_spike_motion,
                          work->scl.y);
    njPopMatrixEx();
    enemy_draw_shadow(work, enemy);
    /* @hack redundant: a block no path reaches */
    if (0) {
        ang.x = 0;
        ang.y = 0;
        ang.z = 0;
        scl.x = work->scl.z + RANGE;
        scl.z = work->scl.z + RANGE;
        scl.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

static void uniuni_spike_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* a spike ends with its uni. it bursts as the uni is beaten */
static void uniuni_spike_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (task->parent == NULL) {
        task_kill(task);
        return;
    } else if (task->parent->work->flags & UNI_BEATEN) {
        burst_create_wobbling(&work->pos, &enemy->spd, BURST_SCALE);
        task_kill(task);
        return;
    } else if (task->parent->step == task_destroy) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case MODE_SPIKE_CIRCLE:
        uniuni_spike_place(task);
        break;
    case MODE_SPIKE_BREAK:
        task_kill(task);
        return;
        /* @hack redundant: the break after the return */
        break;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer++;
        work->scl.y += SPIKE_FRAME_STEP;
        while (work->scl.y >= uniuni_spike_motion.nbFrame - 1) {
            work->scl.y -= uniuni_spike_motion.nbFrame - 1;
        }
    }
    /* @hack redundant: a block no path reaches */
    if (0) {
        switch (work->mode) {
        case MODE_SPIKE_CIRCLE:
            uniuni_spike_place(task);
            break;
        case MODE_SPIKE_BREAK:
            task_kill(task);
            break;
        }
        work->timer++;
    }
}

static void uniuni_spike_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    collision_init(task, uniuni_spike_shapes, 1, COLLISION_ENEMY);
    /* @hack redundant: the first collision_set_radius, whose radius the
     * second replaces */
    collision_set_radius(task, SPIKE_FIRST_RECORD_RADIUS);
    /* @hack spelling: the record read again through task->work, where work
     * would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, SPIKE_RECORD_RADIUS);
    enemy = enemy_work_alloc(task);
    enemy->shadow_size = 4.0F;
    enemy->shadow_ratio = 1.0F;
    work->scl.x = SPIKE_RADIUS;
    work->mode = MODE_SPIKE_CIRCLE;
    enemy->damage = 2.0F;
    enemy_set_home(work, enemy);
    task->destructor = uniuni_spike_destroy;
    task->draw = uniuni_spike_draw;
}

/* the spikes start at world headings of one, three, five and seven eighths
 * of a turn. those at one and five eighths tilt by SPIKE_TILT, and the other
 * two by -SPIKE_TILT. a spike's submode is its index, which nothing reads */
static void uniuni_spikes_create(struct task *parent)
{
    struct task *child;
    struct task_work *work;
    int i;
    Angle tilt;
    Angle heading;

    if (parent) {
        tilt = SPIKE_TILT;
        heading = EIGHTH_TURN;
        for (i = SPIKES; i > 0; i--) {
            child = task_create_child(TASK_WORK, uniuni_spike_step, parent);
            if (child) {
                work = child->work;
                work->submode = i - 1;
                work->ang.x = tilt;
                work->ang.y = heading;
                uniuni_spike_setup(child);
            }
            if (i == SPIKES - 1) {
                tilt = -tilt;
                heading += QUARTER_TURN;
            }
            heading += HALF_TURN;
        }
    }
}

/* as uniuni_spike_place. the draw turns the spike by ang.z, a quarter turn
 * past its heading */
static void ghora_spike_place(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *parent = task->parent->work;
    NJS_VECTOR pos;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    pos.x = 0.0F;
    pos.y = 0.0F;
    pos.z = work->scl.x;
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &pos, &pos);
    njAddVector(&pos, &parent->pos);
    work->pos = pos;
    njPopMatrixEx();
    work->ang.z = work->ang.y + QUARTER_TURN;
    work->ang.y += GHORA_SPIKE_TURN;
}

/* each vertex takes its own v, as the model holds it, plus the scroll */
static void ghora_spike_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    Sint16 *vertex_v;
    int scroll;

    light_apply(UNI_LIGHT);
    njFogDisable();
    njSetTexture(&ghora_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    scroll = SCROLL_V(enemy);
    vertex_v = &ghora_spike_polygons[GHORA_SPIKE_FIRST_V];
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 7;
    vertex_v += NEXT_STRIP_V;
    *vertex_v = scroll + 151;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 151;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 151;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 151;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 151;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_STRIP_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 7;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    vertex_v += NEXT_V;
    *vertex_v = scroll + 47;
    njRotateY(NULL, work->ang.z);
    njCnkSimpleDrawObject(&ghora_spike_object);
    njPopMatrixEx();
    if (work->tick < GLOW_STEPS) {
        ghora_glow.tlist = &ghora_texlist;
        ghora_glow_cell->texid =
            (work->tick >> GLOW_TEXTURE_SHIFT) + GLOW_FIRST_TEXTURE;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(ghora_glow.tlist);
        /* @hack spelling: one assignment to sx and sy, where a statement for
         * each would do */
        ghora_glow.sx = ghora_glow.sy = GLOW_SCALE;
        ghora_glow.p = work->pos;
        ghora_glow.p.y += GLOW_HEIGHT;
        njDrawSprite3D(&ghora_glow, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        /* @hack redundant: a block no path reaches */
        if (0) {
            if (&njColorBlendingMode) {
            }
        }
    }
    enemy_draw_shadow(work, enemy);
    light_apply(light_after_draw);
    njFogEnable();
}

/* a GHORA spike breaks where it touches another record or a wall */
static void ghora_spike_reach(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    if (task->parent->work->mode == MODE_CHASE) {
        work->scl.x =
            float_approach(work->scl.x, GHORA_REACH, GHORA_REACH_STEP);
    } else {
        work->scl.x =
            float_approach(work->scl.x, SPIKE_RADIUS, GHORA_REACH_STEP);
    }
    ghora_spike_place(task);
    enemy_meet_stage(work, enemy);
    if ((work->collision->flags & COLLISION_TOUCHED) ||
        (enemy->flags & ENEMY_AT_WALL)) {
        work->mode = MODE_SPIKE_BREAK;
        return;
    }
    SCROLL_V(enemy) += GHORA_SCROLL;
    if (work->tick == GLOW_STEPS) {
        work->tick += (rand() * (GLOW_SKIP_RANGE * RAND_SCALE)) + GLOW_SKIP;
    }
    work->tick++;
}

static void ghora_spike_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* as uniuni_spike_step, but a spike that breaks bursts as it ends */
static void ghora_spike_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (task->parent == NULL) {
        task_kill(task);
        return;
    } else if (task->parent->work->flags & UNI_BEATEN) {
        burst_create_wobbling(&work->pos, &enemy->spd, BURST_SCALE);
        task_kill(task);
        return;
    } else if (task->parent->step == task_destroy) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case MODE_SPIKE_CIRCLE:
        ghora_spike_reach(task, work, enemy);
        break;
    case MODE_SPIKE_BREAK:
        burst_create_wobbling(&work->pos, &enemy->spd, BURST_SCALE);
        task_kill(task);
        return;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    /* @hack redundant: a block no path reaches */
    if (0) {
        switch (work->mode) {
        case MODE_SPIKE_CIRCLE:
            ghora_spike_reach(task, work, enemy);
            break;
        case MODE_SPIKE_BREAK:
            task_kill(task);
            break;
        }
        work->timer++;
    }
}

static void ghora_spike_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    collision_init(task, ghora_spike_shapes, 1, COLLISION_ENEMY);
    /* @hack redundant: the first collision_set_radius, whose radius the
     * second replaces */
    collision_set_radius(task, SPIKE_FIRST_RECORD_RADIUS);
    /* @hack spelling: the record read again through task->work, where work
     * would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, SPIKE_RECORD_RADIUS);
    enemy = enemy_work_alloc(task);
    enemy->shadow_size = 3.0F;
    enemy->shadow_ratio = 1.0F;
    work->scl.x = SPIKE_RADIUS;
    work->mode = MODE_SPIKE_CIRCLE;
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = 3.0F;
    enemy->radius = 3.0F;
    enemy->bottom = -3.0F;
    enemy->wall_bounce = 0.7F;
    enemy->floor_bounce = 0.5F;
    enemy->friction = 0.5F;
    enemy->unk_0e4 = 0.7F;
    enemy->damage = 2.0F;
    enemy_set_home(work, enemy);
    task->destructor = ghora_spike_destroy;
    task->draw = ghora_spike_draw;
}

/* the spikes start untilted, at world headings of one, three, five and
 * seven eighths of a turn. a spike's submode is its index, which nothing
 * reads */
static void ghora_spikes_create(struct task *parent)
{
    struct task *child;
    struct task_work *work;
    int i;
    Angle heading;

    if (parent) {
        heading = EIGHTH_TURN;
        /* @hack spelling: the count tested against -1, where i >= 0 would
         * do */
        for (i = SPIKES - 1; i > -1; i--) {
            child = task_create_child(TASK_WORK, ghora_spike_step, parent);
            if (child) {
                work = child->work;
                work->submode = i;
                work->ang.y = heading;
                ghora_spike_setup(child);
            }
            heading += QUARTER_TURN;
        }
    }
}
