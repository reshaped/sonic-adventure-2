#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/arena.h"
#include "boss_last2/body.h"
#include "boss_last2/stage.h"
#include "boss_last2/super.h"
#include "boss_last2/super_effects.h"
#include "camera/camera.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "cri_adxt.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/speed_lines.h"
#include "event/mini_event.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "draw/chunk.h"
#include "draw/fade.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/messenger.h"
#include "shared/ring_sparkle.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the bytes a super form's record takes, as each maker allocates it: struct
 * player through by_character.super */
#define SUPER_RECORD_SIZE 0x328
/* @hack type: the record a second time, through the address of its first
 * field, where super alone would do */
#define COMMON_PLAYER(super) ((struct player *)&(super)->slot)
/* the level of the manager's task and of the effect textures' */
#define MANAGER_LEVEL 0
/* the shapes a record has, and the index of the one the attacking modes turn
 * on */
#define SUPER_SHAPES 2
#define ATTACK_SHAPE 1

/* the forms' models lie in model_table SUPER_SONIC_MODELS past Sonic's, and
 * SUPER_SHADOW_MODELS past Shadow's. Shadow's own lie SHADOW_MODELS past
 * Sonic's. a part's number counts from its character's first model */
#define SUPER_SONIC_MODELS 328
#define SHADOW_MODELS 65
#define SUPER_SHADOW_MODELS 290
/* whether object is part n of Sonic's model or of Super Sonic's, or of
 * Shadow's or Super Shadow's */
#define SONIC_PART(object, n)                                                  \
    ((object) == model_table[(n)].object ||                                    \
     (object) == model_table[(n) + SUPER_SONIC_MODELS].object)
#define SHADOW_PART(object, n)                                                 \
    ((object) == model_table[SHADOW_MODELS + (n)].object ||                    \
     (object) ==                                                               \
         model_table[SHADOW_MODELS + (n) + SUPER_SHADOW_MODELS].object)
/* the parts the motion callbacks name. they are the head, a wrist and its
 * ring, the shoes, and Super Shadow's jet flares */
#define HEAD_PART 3
#define LEFT_SHOE_PART 11
#define RIGHT_SHOE_PART 12
#define WRIST_PART 23
#define RING_PART 24
#define LEFT_FLARE_PART 25
#define RIGHT_FLARE_PART 26

/* the slots of model_table that the makers and the draw name. the look turns
 * each form's head and eyes. the jiggle copies the quills, the child of each
 * QUILLS node. super_draw draws that child with the copy's model */
#define SUPER_SONIC_HEAD 331
#define SUPER_SONIC_EYE_0 332
#define SUPER_SONIC_EYE_1 333
#define SUPER_SONIC_QUILLS 349
#define SUPER_SHADOW_HEAD 358
#define SUPER_SHADOW_EYE_0 359
#define SUPER_SHADOW_EYE_1 360
#define SUPER_SHADOW_QUILLS 376
/* Super Shadow's jets and their flares, two slots each. his draw shows the
 * first on odd steps and the second on even ones */
#define RIGHT_JET 382
#define RIGHT_FLARE 384
#define LEFT_JET 386
#define LEFT_FLARE 388

/* the jiggle's param and phase_step as a maker starts them */
#define JIGGLE_SIZE 0.4F
#define JIGGLE_PHASE_STEP 0x400
/* each step the quills' swing is the head's move times the form's
 * _SWING_PER_MOVE. a swing of _FLICK or more pushes the jiggle's phase on by
 * _FLICK_STEP plus up to _FLICK_SPREAD. a swing past QUILLS_RESET, but short
 * of that, sets the phase to QUILLS_RESET_PHASE */
#define SONIC_SWING_PER_MOVE 0.25F
#define SONIC_FLICK 0.8F
#define SONIC_FLICK_STEP 8192.0F
#define SONIC_FLICK_SPREAD 8192.0F
#define SHADOW_SWING_PER_MOVE 0.2F
#define SHADOW_FLICK 0.64F
#define SHADOW_FLICK_STEP 6144.0F
#define SHADOW_FLICK_SPREAD 4096.0F
#define QUILLS_RESET 0.4F
#define QUILLS_RESET_PHASE 0x4000

/* the animations of the forms' tables that this source names, beside
 * player/player.h's, by their numbers there. super_draw turns the attack's
 * trail alike for each move's animation and the one after it */
#define ANIM_HOVER 87
#define ANIM_FLY 88
#define ANIM_FLY_FAST 89 /* the homing's */
#define ANIM_RISE 90
#define ANIM_DIVE 92
#define ANIM_FLY_UP 94
#define ANIM_FLY_DOWN 96
/* the end of a hurt. the hover doesn't steer while it plays */
#define ANIM_RECOVER 99
/* the switch's poses: the arriving player's, under the flash, and the
 * leaving player's, once the screen is black */
#define ANIM_ARRIVE 100
#define ANIM_LEAVE 107
/* what the miss goes on to once it has lasted MISS_BURST_STEPS, with a burst
 * of sparkles */
#define ANIM_MISS_BURST 104
/* the pose of the active player whose hit on the weak point starts a
 * switch */
#define ANIM_SONIC_HIT 109
#define ANIM_SHADOW_HIT 116
/* with ANIM_WIN, ANIM_LOSE and ANIM_ARRIVE, these keep the record out of the
 * collision under the time stop. no code shows more of their roles.
 * ANIM_UNK_112 and ANIM_UNK_113 lie past SUPER_SONIC_ACTIONS */
#define ANIM_UNK_101 101
#define ANIM_UNK_102 102
#define ANIM_UNK_112 112
#define ANIM_UNK_113 113

/* the switch flash's frames. Super Sonic's textures in super_effect_textures
 * are FLASH_FRAMES from FLASH_TEXTURE, and Super Shadow's the next
 * FLASH_FRAMES */
#define FLASH_FRAMES 6
#define FLASH_TEXTURE 3

/* the jump or the action held in SUPER_HOVER or SUPER_FLY starts a flight up
 * or down. it starts at LAUNCH_SPEED or faster. SUPER_RISE and SUPER_DIVE ease
 * toward RISE_SPEED by RISE_EASE a step. SUPER_FLY_UP and SUPER_FLY_DOWN ease
 * toward FLY_UP_SPEED by FLY_UP_EASE */
#define LAUNCH_SPEED 2.7F
#define RISE_SPEED 4.0F
#define RISE_EASE 0.1F
#define FLY_UP_SPEED 2.0F
#define FLY_UP_EASE 0.4F
/* the speed ahead from which super_flight_anim would give ANIM_FLY_FAST,
 * where its test passed. it never does */
#define FAST_FLIGHT 2.0F
/* the homing attack flies at HOMING_SPEED for a weak point within
 * HOMING_REACH. it ends at the latest once it has lasted HOMING_STEPS */
#define HOMING_REACH 200.0F
#define HOMING_SPEED 10.0F
#define HOMING_STEPS 120
/* the miss bursts into MISS_SPARKLES sparkles once it has lasted
 * MISS_BURST_STEPS. a sparkle starts MISS_RADIUS plus up to
 * MISS_RADIUS_SPREAD from unk_230, at a random angle. on each axis it drifts
 * at its MISS_DRIFT_ plus or less up to its _SPREAD. its scale is MISS_SCALE
 * plus up to 1 */
#define MISS_BURST_STEPS 40
#define MISS_SPARKLES 15
#define MISS_RADIUS 5.5F
#define MISS_RADIUS_SPREAD 4.0F
#define MISS_DRIFT_X (-0.1F)
#define MISS_DRIFT_X_SPREAD 0.3F
#define MISS_DRIFT_Y 0.1F
#define MISS_DRIFT_Y_SPREAD 0.15F
#define MISS_DRIFT_Z (-0.1F)
#define MISS_DRIFT_Z_SPREAD 0.2F
#define MISS_SCALE 2.0F

/* the cries a hit draws */
#define SONIC_HURT_CRY 0x300b
#define SHADOW_HURT_CRY 0x301d
/* each start of an attacking mode from SUPER_HOVER or SUPER_FLY plays
 * SOUND_SUPER_BURST at BURST_VOLUME, and vibrates the pad. a rise, a dive
 * or a flight up or down vibrates it by LAUNCH_RUMBLE. the homing vibrates
 * it by HOMING_RUMBLE */
#define BURST_VOLUME 76
#define LAUNCH_RUMBLE(pad) vibration_start((pad), 4, 15, VIBRATION_UNTIMED)
#define HOMING_RUMBLE(pad) vibration_start((pad), 3, 7, VIBRATION_UNTIMED)
/* the attack and guard of an attacking player's first shape */
#define ATTACK_LEVEL 2
#define ATTACK_GUARD 1
/* each step of a form faster than sprint_speed draws a random count of speed
 * lines below SPEED_LINES */
#define SPEED_LINES 3.0F

/* the fight's clock, in game_state.steps. SOUND_LATE plays at LATE_STEPS.
 * from DRAIN_STEPS on, each ring the active player would lose takes both
 * players' rings instead */
#define STEPS_PER_MINUTE (60 * STEPS_PER_SECOND)
#define LATE_STEPS (4 * STEPS_PER_MINUTE)
#define DRAIN_STEPS (5 * STEPS_PER_MINUTE)

/* a switch that a hit starts leaves the players RING_BUDGET rings between
 * them. from the fourth hit on it leaves LATE_RING_BUDGET. in play, the
 * active player loses a ring to the other each time ring_timer passes
 * RING_STEPS. a voice warns at RINGS_LOW and RINGS_LAST, and at none the
 * player misses */
#define RING_BUDGET 50
#define LATE_RING_BUDGET 40
#define RING_STEPS 60
#define RINGS_LOW 15
#define RINGS_LAST 4
#define MOST_TURNS 127
/* a switch's fade to black starts once switch_timer passes
 * SWITCH_FADE_STEPS. after the fade in, play goes on once switch_timer passes
 * SWITCH_WAIT_STEPS */
#define SWITCH_FADE_STEPS 120
#define SWITCH_WAIT_STEPS 180

/* the voices of the switch and the rings, which play at VOICE_PRIORITY. a
 * player's LEAVE voice plays as its turn ends. its FIRST_LEAVE voice follows
 * its first turn, and its HIT_LEAVE voice a turn that a hit ends. its ARRIVE
 * voice plays as a switch hands it the fight, and its FIRST_ARRIVE voice the
 * first time. its RINGS voices warn at RINGS_LOW and RINGS_LAST */
#define VOICE_PRIORITY 3
#define SONIC_FIRST_LEAVE_VOICE 2155
#define SONIC_HIT_LEAVE_VOICE 2157
#define SONIC_LEAVE_VOICE 2156
#define SHADOW_FIRST_LEAVE_VOICE 2158
#define SHADOW_HIT_LEAVE_VOICE 2160
#define SHADOW_LEAVE_VOICE 2159
#define SONIC_FIRST_ARRIVE_VOICE 2161
#define SONIC_ARRIVE_VOICE 2162
#define SHADOW_FIRST_ARRIVE_VOICE 2163
#define SHADOW_ARRIVE_VOICE 2164
#define SONIC_RINGS_LOW_VOICE 2153
#define SONIC_RINGS_LAST_VOICE 2154
#define SHADOW_RINGS_LOW_VOICE 2151
#define SHADOW_RINGS_LAST_VOICE 2152
/* each form's cry as it misses */
#define SONIC_MISS_VOICE 0x61f
#define SHADOW_MISS_VOICE 0x61c
/* a voice local holds NO_VOICE where no voice may play, and ANY_VOICE where
 * one may */
#define NO_VOICE (-1)
#define ANY_VOICE (-2)
/* the radio step's message, or NO_MESSAGE for none */
#define NO_MESSAGE (-1)

/* the super forms' own modes, beside player/player.h's. SUPER_RISE,
 * SUPER_DIVE, SUPER_FLY_UP, SUPER_FLY_DOWN and SUPER_HOMING attack.
 * SUPER_SWITCH holds the player still for its pose in a switch */
enum super_mode {
    SUPER_HOVER = 59,
    SUPER_FLY = 60,
    SUPER_RISE = 61,
    SUPER_DIVE = 62,
    SUPER_FLY_UP = 63,
    SUPER_FLY_DOWN = 64,
    SUPER_SWITCH = 66,
    SUPER_HOMING = 67
};

/* the work flags a request, an event's release and the free move clear */
#define MOVE_FLAGS (PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL)
/* the forms' animations, and the parts the look turns: the head and two
 * eyes */
#define SUPER_SONIC_ACTIONS 110
#define SUPER_SHADOW_ACTIONS 117
#define LOOK_PARTS 3
static struct player_action super_sonic_actions[SUPER_SONIC_ACTIONS] = {
#include "boss_last2/super/super_sonic_actions.inc"
};
static struct player_action super_shadow_actions[SUPER_SHADOW_ACTIONS] = {
#include "boss_last2/super/super_shadow_actions.inc"
};
static struct collision_shape super_shapes[SUPER_SHAPES] = {
#include "boss_last2/super/super_shapes.inc"
};
struct finalhazard_players *finalhazard_players = NULL;
/* the task whose destructor releases super_effect_textures, or null while
 * they aren't loaded */
static struct task *effect_texture_owner = NULL;
/* super_confine_players keeps both players within flight_radius of
 * flight_center across x and z. it keeps their x at flight_x_limit or less,
 * and their y between flight_floor and flight_ceiling */
static NJS_POINT3 flight_center = {0, 0, 0};
static Float flight_radius = 1500;
static Float flight_ceiling = 300;
static Float flight_floor = -500;
static Float flight_x_limit = 250;
/* in play, an active Super Sonic whose z falls below sonic_switch_z can start
 * a switch. so can an active Super Shadow whose z rises above
 * shadow_switch_z */
static Float sonic_switch_z = -50;
static Float shadow_switch_z = 50;
static NJS_CNK_OBJECT *super_sonic_look[LOOK_PARTS];
static NJS_CNK_OBJECT *super_shadow_look[LOOK_PARTS];
/* the record super_draw draws, for the motion callbacks */
static struct player *drawn_player;
/* @hack redundant: a word that nothing reads, which places inverse_view after
 * it as the image does */
static Uint32 unused_before_inverse;
/* the inverse of the matrix super_draw starts with, the view's. it takes the
 * callbacks' points back to the world */
static NJS_MATRIX inverse_view;
/* @hack redundant: a matrix that nothing reads, which places the matrices
 * after it as the image does */
static NJS_MATRIX unused_matrix;
/* the matrices of parts that the motion callbacks keep. no code reads the
 * first four. Super Shadow's draw draws his jets and flares at the rest */
static NJS_MATRIX super_wrist;
static NJS_MATRIX super_ring;
static NJS_MATRIX super_left_shoe;
static NJS_MATRIX super_right_shoe;
static NJS_MATRIX shadow_left_jet;
static NJS_MATRIX shadow_right_jet;
static NJS_MATRIX shadow_left_flare;
static NJS_MATRIX shadow_right_flare;
static void effect_textures_destroy(struct task *task);
static void effect_textures_step(struct task *task);
static void super_step(struct task *task);
static void super_select_mode(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *super);
static void super_destroy(struct task *task);
static void super_place_shapes(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);
static void super_afterimage(struct task_work *work, struct player *record,
                             struct player *super);
static void super_sonic_joint(NJS_CNK_OBJECT *object);
static void super_shadow_joint(NJS_CNK_OBJECT *object);
static void super_draw(struct task *task);
static void super_free_move(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *super);
static int super_take_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *super);
static int super_stop_flight(struct task_work *work, struct player *record);
static void super_flight_anim(struct player *record);
static int super_attack_weak_point(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record, struct player *super);
static void super_accelerate(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
static void super_coast(struct task_work *work, struct player_motion *motion,
                        struct player *record);
static int super_clear_voices(void);
static void super_confine_players(void);
static void super_radio_step(struct task *task,
                             struct finalhazard_players *players);
static void super_manager_step(struct task *task);
static void super_manager_destroy(struct task *task);
static void super_manager_start(void);

static void effect_textures_destroy(struct task *task)
{
    njReleaseTexture(&super_effect_textures);
    effect_texture_owner = NULL;
}

static void effect_textures_step(struct task *task)
{
}

void super_sonic_create(int player)
{
    struct task *task;
    struct task_work *work;
    struct player *super;
    struct player *record;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            super_step, "SuperSonic")) == NULL) {
        return;
    }
    if ((super = syCalloc(1, SUPER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    record = COMMON_PLAYER(super);
    task->extra.p = super;
    work = task->work;
    record->slot = player;
    record->character = PLAYER_SUPER_SONIC;
    work->submode = 0;
    work->mode = SUPER_HOVER;
    record->mode_steps = 0;
    player_register(player, task);
    record->anim.actions = super_sonic_actions;
    record->anim.next = ANIM_STAND;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. where
     * collision_init fails, it unregisters the player and destroys the task */
    if (!collision_init(task, super_shapes, SUPER_SHAPES, COLLISION_PLAYER)) {
        player_unregister(player);
        task_destroy(task);
        return;
    }
#else
    /* @bug memory: reads the collision record through null where
     * collision_init fails, at 0x8c5006e2. super_step tests the record at
     * 0x8c500abc, but super_place_shapes reads through it untested at
     * 0x8c501c4a, and super_step writes through it at 0x8c500c8a and
     * 0x8c5013d6 */
    collision_init(task, super_shapes, SUPER_SHAPES, COLLISION_PLAYER);
#endif
    work->collision->shapes->radius = record->consts.radius;
    super->by_character.super.textures = texture_load_pvm_alloc("SSONICTEX");
    life_icon_load(record->character);
    super->by_character.super.models = model_file_read("SSONICMDL.PRS");
    super->by_character.super.motions = motion_file_read("SSONICMTN.PRS");
#ifdef NONMATCHING
    /* this side destroys the player where its textures, models or motions
     * didn't load. it does the same where its quills' model has no child, which
     * the lines below read. super_destroy frees what did load */
    if (!super->by_character.super.textures ||
        !super->by_character.super.models ||
        !super->by_character.super.motions ||
        !model_table[SUPER_SONIC_QUILLS].object ||
        !model_table[SUPER_SONIC_QUILLS].object->child) {
        task->destructor = super_destroy;
        task_destroy(task);
        return;
    }
#endif
    player_anim_step(&record->anim);
    super_sonic_look[0] = model_table[SUPER_SONIC_HEAD].object;
    super_sonic_look[1] = model_table[SUPER_SONIC_EYE_0].object;
    super_sonic_look[2] = model_table[SUPER_SONIC_EYE_1].object;
    player_look_start(player, super_sonic_look);
    super->by_character.super.quills =
        jiggle_create(model_table[SUPER_SONIC_QUILLS].object->child);
#ifdef NONMATCHING
    /* this side sets the quills' kind only where jiggle_create made them. the
     * matching side writes through the null */
    if (super->by_character.super.quills != NULL)
#endif
    {
        super->by_character.super.quills->kind = JIGGLE_SWING_X;
        super->by_character.super.quills->param = JIGGLE_SIZE;
        super->by_character.super.quills->by_kind.steps = 0;
        super->by_character.super.quills->phase_step = JIGGLE_PHASE_STEP;
        super->by_character.super.quills->shape_a = NULL;
    }
    super->by_character.super.last_head = work->pos;
    action_window_start(record->slot);
    task->destructor = super_destroy;
    task->draw = super_draw;
    /* @hack redundant: a test of game_state.demo whose two arms store the
     * same */
    if (game_state.demo) {
        record->upgrades = UPGRADE_BIT(UPGRADE_UNK_15);
    } else {
        record->upgrades = UPGRADE_BIT(UPGRADE_UNK_15);
    }
    super_manager_start();
    if (effect_texture_owner == NULL) {
        texture_load_pvm("SSONEFFTEX", &super_effect_textures);
        effect_texture_owner =
            task_create(0, MANAGER_LEVEL, effect_textures_step, "ManTex");
#ifdef NONMATCHING
        /* this side releases the effect textures and returns where task_create
         * gives them no owner. the matching side writes through the null */
        if (!effect_texture_owner) {
            njReleaseTexture(&super_effect_textures);
            return;
        }
#endif
        effect_texture_owner->destructor = effect_textures_destroy;
    }
}

void super_shadow_create(int player)
{
    struct task *task;
    struct task_work *work;
    struct player *super;
    struct player *record;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            super_step, "SuperSonic")) == NULL) {
        return;
    }
    if ((super = syCalloc(1, SUPER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    record = COMMON_PLAYER(super);
    task->extra.p = super;
    work = task->work;
    record->slot = player;
    record->character = PLAYER_SUPER_SHADOW;
    work->submode = 0;
    work->mode = SUPER_HOVER;
    record->mode_steps = 0;
    player_register(player, task);
    record->anim.actions = super_shadow_actions;
    record->anim.next = ANIM_STAND;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. where
     * collision_init fails, it unregisters the player and destroys the task */
    if (!collision_init(task, super_shapes, SUPER_SHAPES, COLLISION_PLAYER)) {
        player_unregister(player);
        task_destroy(task);
        return;
    }
#else
    /* @bug memory: reads the collision record through null where
     * collision_init fails, at 0x8c50094a. the step goes on through it as
     * super_sonic_create's tag says */
    collision_init(task, super_shapes, SUPER_SHAPES, COLLISION_PLAYER);
#endif
    work->collision->shapes->radius = record->consts.radius;
    super->by_character.super.textures = texture_load_pvm_alloc("SSHADOWTEX");
    life_icon_load(record->character);
    super->by_character.super.models = model_file_read("SSHADOWMDL.PRS");
    super->by_character.super.motions = motion_file_read("SSHADOWMTN.PRS");
#ifdef NONMATCHING
    /* this side destroys the player where its textures, models or motions
     * didn't load. it does the same where its quills' model has no child, which
     * the lines below read. super_destroy frees what did load */
    if (!super->by_character.super.textures ||
        !super->by_character.super.models ||
        !super->by_character.super.motions ||
        !model_table[SUPER_SHADOW_QUILLS].object ||
        !model_table[SUPER_SHADOW_QUILLS].object->child) {
        task->destructor = super_destroy;
        task_destroy(task);
        return;
    }
#endif
    player_anim_step(&record->anim);
    super_shadow_look[0] = model_table[SUPER_SHADOW_HEAD].object;
    super_shadow_look[1] = model_table[SUPER_SHADOW_EYE_0].object;
    super_shadow_look[2] = model_table[SUPER_SHADOW_EYE_1].object;
    player_look_start(player, super_shadow_look);
    super->by_character.super.quills =
        jiggle_create(model_table[SUPER_SHADOW_QUILLS].object->child);
#ifdef NONMATCHING
    /* this side sets the quills' kind only where jiggle_create made them. the
     * matching side writes through the null */
    if (super->by_character.super.quills != NULL)
#endif
    {
        super->by_character.super.quills->kind = JIGGLE_SWING_XZ;
        super->by_character.super.quills->param = JIGGLE_SIZE;
        super->by_character.super.quills->by_kind.steps = 0;
        super->by_character.super.quills->phase_step = JIGGLE_PHASE_STEP;
        super->by_character.super.quills->shape_a = NULL;
    }
    super->by_character.super.last_head = work->pos;
    action_window_start(record->slot);
    task->destructor = super_destroy;
    task->draw = super_draw;
    /* @hack redundant: a test of game_state.demo whose two arms store the
     * same */
    if (game_state.demo) {
        record->upgrades = UPGRADE_BIT(UPGRADE_UNK_15);
    } else {
        record->upgrades = UPGRADE_BIT(UPGRADE_UNK_15);
    }
    super_manager_start();
    if (effect_texture_owner == NULL) {
        texture_load_pvm("SSONEFFTEX", &super_effect_textures);
        effect_texture_owner =
            task_create(0, MANAGER_LEVEL, effect_textures_step, "ManTex");
#ifdef NONMATCHING
        /* this side releases the effect textures and returns where task_create
         * gives them no owner. the matching side writes through the null */
        if (!effect_texture_owner) {
            njReleaseTexture(&super_effect_textures);
            return;
        }
#endif
        effect_texture_owner->destructor = effect_textures_destroy;
    }
}

static void super_step(struct task *task)
{
    struct player *super;
    struct task_work *work;
    struct player_motion *motion;
    struct player *record;

    super = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    record = COMMON_PLAYER(super);
#ifdef NONMATCHING
    /* this side steps no player while either player or finalhazard_players is
     * missing, which the step reads */
    if (!finalhazard_players || !player_work[0] || !player_work[1]) {
        return;
    }
#endif
    player_ground_step(task, work, motion, record);

    /* @hack redundant: an arm that never runs, and an empty one */
    /* @hack type: game_state's time_stopped read as game_state_time_stopped,
     * where game_state.time_stopped would do */
    if (0) {
        work->mode = PLAYER_MODE_STAND;
    } else if (!(game_state_time_stopped & TIME_STOPPED_BIT(record->slot))) {
        super_select_mode(work, motion, record, super);
    } else {
    }
    if (work->collision != NULL) {
        int attack;
        int guard;
        struct collision_shape *shape;

        shape = work->collision->shapes;
        shape->flags &= ~SHAPE_HURT_NO_PUSH;
        if (record->flags & PLAYER_INVINCIBLE) {
            attack = INVINCIBLE_LEVEL;
            guard = INVINCIBLE_LEVEL;
        } else if (work->flags & PLAYER_WORK_ATTACK) {
            guard = ATTACK_GUARD;
            attack = ATTACK_LEVEL;
            shape->flags |= SHAPE_HURT_NO_PUSH;
        } else {
            attack = 0;
            guard = 0;
        }
        shape->levels =
            (shape->levels & ~SHAPE_ATTACK) | (attack & SHAPE_ATTACK);
        shape->levels = (shape->levels & ~SHAPE_GUARD) |
                        ((guard << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
        if (work->mode == SUPER_RISE || work->mode == SUPER_DIVE ||
            work->mode == SUPER_FLY_UP || work->mode == SUPER_FLY_DOWN ||
            work->mode == SUPER_HOMING) {
            shape[ATTACK_SHAPE].flags &= ~SHAPE_OFF;
        } else {
            shape[ATTACK_SHAPE].flags |= SHAPE_OFF;
        }
    }
    /* @hack type: game_state's time_stopped read as game_state_time_stopped,
     * where game_state.time_stopped would do */
    if (game_state_time_stopped & TIME_STOPPED_BIT(record->slot)) {
        super_place_shapes(work, motion, record);
        surface_probe_player(work, record);
        if (record->anim.current != ANIM_WIN &&
            record->anim.current != ANIM_ARRIVE &&
            record->anim.current != ANIM_UNK_101 &&
            record->anim.current != ANIM_UNK_102 &&
            record->anim.current != ANIM_UNK_112 &&
            record->anim.current != ANIM_UNK_113 &&
            record->anim.current != ANIM_LOSE) {
            collision_register(task);
            collision_set_radius(task, record->consts.height +
                                           njScalor(&record->spd));
        } else {
            work->collision->hit_shape = -1;
            work->collision->hit = NULL;
            work->collision->flags &= ~COLLISION_TOUCH_FLAGS;
        }
        player_finish_step(work, motion, record);
        return;
    }
    super->by_character.super.flash_frame++;
    if (super->by_character.super.flash_frame >= FLASH_FRAMES) {
        super->by_character.super.flash_frame = 0;
    }
    if (work->mode != PLAYER_MODE_HURT && work->mode != PLAYER_MODE_UNK_35 &&
        work->mode != PLAYER_MODE_STAND && work->mode != SUPER_HOVER &&
        work->mode != PLAYER_MODE_MISS) {
        if (njScalor(&record->spd) > record->consts.sprint_speed) {
            speed_lines_create(record->slot,
                               (int)(rand() * (SPEED_LINES * RAND_SCALE)),
                               &record->unk_218, &motion->spd);
        }
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        work->mode = SUPER_HOVER;
        /* fall through */
    case SUPER_HOVER:
        if (record->anim.current != ANIM_RECOVER) {
            player_turn_to_stick(work, motion, record);
            super_accelerate(work, motion, record);
        } else {
            super_coast(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case SUPER_FLY:
        super_accelerate(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        super_flight_anim(record);
        break;
    case SUPER_RISE:
        record->spd.y = float_approach(record->spd.y, RISE_SPEED, RISE_EASE);
        super_accelerate(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case SUPER_DIVE:
        record->spd.y = float_approach(record->spd.y, -RISE_SPEED, RISE_EASE);
        super_accelerate(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case SUPER_FLY_UP:
        record->spd.y =
            float_approach(record->spd.y, FLY_UP_SPEED, FLY_UP_EASE);
        super_accelerate(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case SUPER_FLY_DOWN:
        record->spd.y =
            float_approach(record->spd.y, -FLY_UP_SPEED, FLY_UP_EASE);
        super_accelerate(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case SUPER_HOMING:
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        super_afterimage(work, record, super);
        break;
    case PLAYER_MODE_HURT:
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_ANIM:
        break;
    case SUPER_SWITCH:
        break;
    case PLAYER_MODE_UNK_35:
        super_coast(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_EVENT:
        break;
    case PLAYER_MODE_MISS:
        if (super->by_character.super.flight_steps++ > MISS_BURST_STEPS &&
            record->anim.current != ANIM_MISS_BURST) {
            Angle ang_x;
            Angle ang_y;
            int i;
            NJS_VECTOR spd;
            NJS_POINT3 pos;
            Float radius;

            record->anim.start_frame = record->anim.frame;
            record->anim.next = ANIM_MISS_BURST;
            i = MISS_SPARKLES;
            do {
                spd.x = MISS_DRIFT_X -
                        (rand() * (MISS_DRIFT_X_SPREAD * RAND_SCALE));
                spd.y = MISS_DRIFT_Y -
                        (rand() * (MISS_DRIFT_Y_SPREAD * RAND_SCALE));
                spd.z = MISS_DRIFT_Z +
                        (rand() * (MISS_DRIFT_Z_SPREAD * RAND_SCALE));
                radius =
                    MISS_RADIUS + (rand() * (MISS_RADIUS_SPREAD * RAND_SCALE));
                ang_x =
                    (Angle)(QUARTER_TURN - (rand() * (HALF_TURN * RAND_SCALE)));
                ang_y = (Angle)RANDOM_TURN();
                pos.x = radius * njCos(ang_x) * njSin(ang_y);
                pos.y = radius * njSin(ang_x);
                pos.z = radius * njCos(ang_x) * njCos(ang_y);
                njAddVector(&pos, &record->unk_230);
                ring_sparkle_create_long(&pos, &spd,
                                         MISS_SCALE + (rand() * RAND_SCALE));
            } while (--i);
        }
        player_turn_upright(work, motion, record);
        super_coast(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FREE_MOVE:
        super_free_move(work, motion, record, super);
    }
    player_anim_step(&record->anim);
    super_place_shapes(work, motion, record);
    if (super->by_character.super.quills != NULL) {
        Float random;
        Float swing;
        NJS_VECTOR head_travel;

        random = rand() * RAND_SCALE;
        if (record->character == PLAYER_SUPER_SONIC) {
            head_travel = record->unk_218;
            njSubVector(&head_travel, &super->by_character.super.last_head);
            swing = njScalor(&head_travel) * SONIC_SWING_PER_MOVE;
            super->by_character.super.last_head = record->unk_218;
            if (_builtin_fabsf(swing) >
                _builtin_fabsf(super->by_character.super.quills->param)) {
                super->by_character.super.quills->param = swing;
            }
            if (_builtin_fabsf(swing) >= SONIC_FLICK) {
                super->by_character.super.quills->phase +=
                    SONIC_FLICK_STEP + (SONIC_FLICK_SPREAD * random);
            } else if (_builtin_fabsf(swing) > QUILLS_RESET) {
                super->by_character.super.quills->phase = QUILLS_RESET_PHASE;
            }
            jiggle_step(super->by_character.super.quills);
        }
        if (record->character == PLAYER_SUPER_SHADOW) {
            head_travel = record->unk_218;
            njSubVector(&head_travel, &super->by_character.super.last_head);
            swing = njScalor(&head_travel) * SHADOW_SWING_PER_MOVE;
            super->by_character.super.last_head = record->unk_218;
            if (_builtin_fabsf(swing) >
                _builtin_fabsf(super->by_character.super.quills->param)) {
                super->by_character.super.quills->param = swing;
            }
            if (_builtin_fabsf(swing) >= SHADOW_FLICK) {
                super->by_character.super.quills->phase +=
                    SHADOW_FLICK_STEP + (SHADOW_FLICK_SPREAD * random);
            } else if (_builtin_fabsf(swing) > QUILLS_RESET) {
                super->by_character.super.quills->phase = QUILLS_RESET_PHASE;
            }
            jiggle_step(super->by_character.super.quills);
        }
    }
    if (player_free_move_forced(record->slot)) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        work->flags &= ~(MOVE_FLAGS | PLAYER_WORK_REQUEST);
    }
    surface_probe_player(work, record);
    if (record->anim.current != ANIM_WIN && record->anim.current != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        collision_register(task);
        collision_set_radius(task,
                             record->consts.height + njScalor(&record->spd));
    } else {
        work->collision->hit_shape = -1;
        work->collision->hit = NULL;
        work->collision->flags &= ~COLLISION_TOUCH_FLAGS;
    }
    player_finish_step(work, motion, record);
}

static void super_select_mode(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *super)
{
    /* @bug memory: reads a null record, which the test lets through, TST
     * R14,R14 at 0x8c50142a. player_take_hit reads it, and so does the test
     * of its character where the hit counts, MOV.B @(1,R14),R0 at
     * 0x8c5014b4 */
    if (record != NULL && (record->flags & PLAYER_INVINCIBLE)) {
        work->flags |= PLAYER_WORK_UNK_8000;
        work->timer = 0;
    } else if (work->timer == 0) {
        player_take_hit(work, record);
        if (work->timer) {
            if (record->character == PLAYER_SUPER_SONIC) {
                sound_play_at(SONIC_HURT_CRY, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->character == PLAYER_SUPER_SHADOW) {
                sound_play_at(SHADOW_HURT_CRY, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            work->flags |= PLAYER_WORK_UNK_8000;
            work->flags &= ~(PLAYER_WORK_REQUEST | PLAYER_WORK_ATTACK);
            /* @hack redundant: the test, where the clear alone would do */
            if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
                record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
            }
            work->mode = PLAYER_MODE_HURT;
            record->anim.next = ANIM_HURT;
            work->flags &= ~WORK_HURT;
            record->spd.x = -record->consts.running_speed;
        } else {
            work->flags &= ~PLAYER_WORK_UNK_8000;
        }
    } else {
        work->timer--;
        work->flags &= ~WORK_HURT;
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        work->mode = SUPER_HOVER;
        /* fall through */
    case SUPER_HOVER:
        if (super_take_request(work, motion, record, super)) {
            break;
        }

#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side. the test
         * returns for each of the three animations */
        if (record->anim.current == ANIM_RECOVER ||
            record->anim.current == ANIM_SONIC_HIT ||
            record->anim.current == ANIM_SHADOW_HIT) {
#else
        /* @bug logic: the stick, the jump and the action interrupt
         * ANIM_RECOVER, ANIM_SONIC_HIT and ANIM_SHADOW_HIT. the test that
         * returns asks one animation to be all three, at 0x8c5015f2 to
         * 0x8c501600 */
        /* NOLINTNEXTLINE(*redundant-expression,*overlap-compare) */
        if (record->anim.current == ANIM_RECOVER &&
            record->anim.current == ANIM_SONIC_HIT &&
            record->anim.current == ANIM_SHADOW_HIT) {
#endif
            return;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = SUPER_FLY;
            record->anim.next = ANIM_FLY;
            break;
        }
        if (pad_jump_held[record->slot]) {
            work->mode = SUPER_RISE;
            record->anim.next = ANIM_RISE;
            work->flags |= PLAYER_WORK_ATTACK;
            if (record->spd.y < LAUNCH_SPEED) {
                record->spd.y = LAUNCH_SPEED;
            }
            sound_play_at(SOUND_SUPER_BURST, NULL, 0, BURST_VOLUME, &work->pos);
            LAUNCH_RUMBLE(0);
            break;
        }
        if (pad_action_held[record->slot]) {
            work->mode = SUPER_DIVE;
            record->anim.next = ANIM_DIVE;
            work->flags |= PLAYER_WORK_ATTACK;
            if (record->spd.y > -LAUNCH_SPEED) {
                record->spd.y = -LAUNCH_SPEED;
            }
            sound_play_at(SOUND_SUPER_BURST, NULL, 0, BURST_VOLUME, &work->pos);
            LAUNCH_RUMBLE(0);
            break;
        }
        break;
    case SUPER_FLY:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (super_stop_flight(work, record)) {
            break;
        }
        if ((pad_jump_pressed[record->slot] ||
             pad_action_pressed[record->slot]) &&
            super_attack_weak_point(work, motion, record, super)) {
            sound_play_at(SOUND_SUPER_BURST, NULL, 0, BURST_VOLUME, &work->pos);
            HOMING_RUMBLE(record->slot);
            super->by_character.super.flight_steps = 0;
            break;
        }
        if (pad_jump_held[record->slot]) {
            work->mode = SUPER_FLY_UP;
            record->anim.next = ANIM_FLY_UP;
            work->flags |= PLAYER_WORK_ATTACK;
            if (record->spd.y < LAUNCH_SPEED) {
                record->spd.y = LAUNCH_SPEED;
            }
            sound_play_at(SOUND_SUPER_BURST, NULL, 0, BURST_VOLUME, &work->pos);
            LAUNCH_RUMBLE(record->slot);
            break;
        }
        if (pad_action_held[record->slot]) {
            work->mode = SUPER_FLY_DOWN;
            record->anim.next = ANIM_FLY_DOWN;
            work->flags |= PLAYER_WORK_ATTACK;
            if (record->spd.y > -LAUNCH_SPEED) {
                record->spd.y = -LAUNCH_SPEED;
            }
            sound_play_at(SOUND_SUPER_BURST, NULL, 0, BURST_VOLUME, &work->pos);
            LAUNCH_RUMBLE(record->slot);
            break;
        }
        break;
    case SUPER_RISE:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = SUPER_FLY_UP;
            record->anim.next = ANIM_FLY_UP;
            break;
        }
        if (!pad_jump_held[record->slot]) {
            work->mode = SUPER_HOVER;
            record->anim.next = ANIM_HOVER;
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        break;
    case SUPER_DIVE:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = SUPER_FLY_DOWN;
            record->anim.next = ANIM_FLY_DOWN;
            break;
        }
        if (!pad_action_held[record->slot]) {
            work->mode = SUPER_HOVER;
            record->anim.next = ANIM_HOVER;
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        break;
    case SUPER_FLY_UP:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (super_stop_flight(work, record)) {
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        if (!pad_jump_held[record->slot]) {
            work->mode = SUPER_FLY;
            super_flight_anim(record);
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        break;
    case SUPER_FLY_DOWN:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (super_stop_flight(work, record)) {
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        if (!pad_action_held[record->slot]) {
            work->mode = SUPER_FLY;
            super_flight_anim(record);
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        break;
    case SUPER_HOMING:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (super_stop_flight(work, record)) {
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        if (!(pad_jump_held[record->slot] || pad_action_held[record->slot]) ||
            !super_attack_weak_point(work, motion, record, super) ||
            super->by_character.super.flight_steps++ > HOMING_STEPS) {
            work->mode = SUPER_FLY;
            super_flight_anim(record);
            work->flags &= ~PLAYER_WORK_ATTACK;
            super->by_character.super.flight_steps = 0;
            break;
        }
        break;
    case PLAYER_MODE_BRAKE:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.next != ANIM_BRAKE) {
            work->mode = SUPER_HOVER;
            record->mode_steps = 0;
            record->anim.next = ANIM_HOVER;
        }
        return;
    case PLAYER_MODE_HURT:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        /* @hack spelling: the unsigned timer tested as <= 0 in the second
         * arm, where == 0 would do */
        if (record->anim.next != ANIM_HURT &&
            record->anim.next != ANIM_HURT_LAND) {
            if (super_stop_flight(work, record)) {
                work->mode = SUPER_HOVER;
                record->anim.next = ANIM_RECOVER;
                break;
            }
        } else if (work->timer <= 0) {
            work->mode = SUPER_HOVER;
            record->anim.next = ANIM_RECOVER;
        }
        break;
    case PLAYER_MODE_PLAY_ANIM:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        break;
    case PLAYER_MODE_UNK_35:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            work->mode = SUPER_HOVER;
            record->mode_steps = 0;
            record->anim.next = ANIM_HOVER;
            break;
        }
        break;
    case SUPER_SWITCH:
        if (super_take_request(work, motion, record, super)) {
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_DONE)) {
            break;
        }
        work->mode = SUPER_HOVER;
        record->mode_steps = 0;
        record->anim.next = ANIM_HOVER;
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            work->mode = SUPER_HOVER;
            record->mode_steps = 0;
            record->anim.next = ANIM_HOVER;
            work->flags &= ~MOVE_FLAGS;
            break;
        }
        break;
    case PLAYER_MODE_FREE_MOVE:
        if (player_pad_records[record->slot].press & PDD_DGT_TA) {
            work->mode = SUPER_HOVER;
            record->anim.next = ANIM_HOVER;
        }
        break;
    }
}

static void super_destroy(struct task *task)
{
    struct task_work *work;
    struct player *super;
    struct player *record;

    work = task->work;
    super = task->extra.p;
    record = COMMON_PLAYER(super);
    if (super->by_character.super.quills != NULL) {
        jiggle_free(super->by_character.super.quills);
        super->by_character.super.quills = NULL;
    }
    player_unregister(record->slot);
    sound_stop_key(work);
#ifdef NONMATCHING
    /* this side releases only textures that loaded. a create that fails runs
     * this destroy with any of the three files missing */
    if (super->by_character.super.textures)
#endif
    {
        njReleaseTexture(super->by_character.super.textures);
        syFree(super->by_character.super.textures);
    }
    super->by_character.super.textures = NULL;
    life_icon_release(record->character);
#ifdef NONMATCHING
    /* this side frees only models that loaded, as for the textures above */
    if (super->by_character.super.models)
#endif
    {
        model_file_free(super->by_character.super.models);
    }
    super->by_character.super.models = NULL;
#ifdef NONMATCHING
    /* this side frees only motions that loaded, as for the textures above */
    if (super->by_character.super.motions)
#endif
    {
        motion_file_free(super->by_character.super.motions);
    }
    super->by_character.super.motions = NULL;
}

static void super_place_shapes(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    struct collision_shape *shape;
    NJS_VECTOR center;

    shape = work->collision->shapes;
    if (work->mode != SUPER_HOMING) {
        work->ang.x = work->ang.z = motion->aim_x = motion->aim_z = 0;
    }
    center.x = center.z = 0.0F;
    center.y = record->consts.center_height;
    turn_to_world(work, &center);
    shape->center = center;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
    shape[ATTACK_SHAPE].center = shape->center;
}

static void super_afterimage(struct task_work *work, struct player *record,
                             struct player *super)
{
    NJS_CNK_OBJECT *object;
    int action;

    if (!(game_state.steps & 1)) {
        action = record->anim.current;
        object = model_table[record->anim.actions[action].model].object;
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        player_afterimage(
            object,
            motion_table[record->anim
                             .actions[record->anim.actions[action].motion]
                             .motion]
                .motion,
            super->by_character.super.textures, record->anim.frame, 0,
            record->slot);
        njPopMatrixEx();
    }
}

/* the motion callbacks of the forms' models, which super_draw gives its
 * draws. a callback keeps some parts' origins and axes in drawn_player, in the
 * world. at other parts it keeps the current matrix. it turns part 2 about x
 * by drawn_player's lean in PLAYER_MODE_RUN */
static void super_sonic_joint(NJS_CNK_OBJECT *object)
{
    struct player *super = drawn_player;
    struct task_work *work;
    NJS_VECTOR joint_pos;
    NJS_VECTOR joint_axis;

    joint_pos.z = 0.0F;
    joint_pos.y = 0.0F;
    joint_pos.x = 0.0F;
    if (SONIC_PART(object, 1)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_230);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2cc);
    } else if (SONIC_PART(object, 2)) {
        work = player_work[super->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, super->lean);
        }
    } else if (SONIC_PART(object, HEAD_PART)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_218);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2b4);
    } else if (SONIC_PART(object, 8)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_224);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2c0);
    } else if (SONIC_PART(object, 15)) {
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_1e8);
        joint_axis.z = 1.0F;
        joint_axis.y = 0.0F;
        joint_axis.x = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_26c);
        joint_axis.y = -1.0F;
        joint_axis.x = 0.0F;
        joint_axis.z = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_284);
    } else if (SONIC_PART(object, 16)) {
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_1f4);
        joint_axis.z = -1.0F;
        joint_axis.y = 0.0F;
        joint_axis.x = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_278);
        joint_axis.y = -1.0F;
        joint_axis.x = 0.0F;
        joint_axis.z = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_290);
    } else if (SONIC_PART(object, 9)) {
        joint_axis.x = -1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_200);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_29c);
    } else if (SONIC_PART(object, 10)) {
        joint_axis.x = -1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_20c);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2a8);
    }
    if (SONIC_PART(object, WRIST_PART)) {
        njGetMatrix(&super_wrist);
    } else if (SONIC_PART(object, RING_PART)) {
        njGetMatrix(&super_ring);
    }
    if (SONIC_PART(object, LEFT_SHOE_PART)) {
        njGetMatrix(&super_left_shoe);
    } else if (SONIC_PART(object, RIGHT_SHOE_PART)) {
        njGetMatrix(&super_right_shoe);
    }
}

static void super_shadow_joint(NJS_CNK_OBJECT *object)
{
    struct player *super = drawn_player;
    struct task_work *work;
    NJS_VECTOR joint_pos;
    NJS_VECTOR joint_axis;

    joint_pos.z = 0.0F;
    joint_pos.y = 0.0F;
    joint_pos.x = 0.0F;
    if (SHADOW_PART(object, 1)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_230);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2cc);
    } else if (SHADOW_PART(object, 2)) {
        work = player_work[super->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, super->lean);
        }
    } else if (SHADOW_PART(object, HEAD_PART)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_218);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2b4);
    } else if (SHADOW_PART(object, 8)) {
        joint_axis.x = 1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_224);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2c0);
    } else if (SHADOW_PART(object, 15)) {
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_1e8);
        joint_axis.z = 1.0F;
        joint_axis.y = 0.0F;
        joint_axis.x = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_26c);
        joint_axis.y = -1.0F;
        joint_axis.x = 0.0F;
        joint_axis.z = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_284);
    } else if (SHADOW_PART(object, 16)) {
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_1f4);
        joint_axis.z = -1.0F;
        joint_axis.y = 0.0F;
        joint_axis.x = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_278);
        joint_axis.y = -1.0F;
        joint_axis.x = 0.0F;
        joint_axis.z = 0.0F;
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_290);
    } else if (SHADOW_PART(object, 9)) {
        joint_axis.x = -1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_200);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_29c);
        njGetMatrix(&shadow_left_jet);
    } else if (SHADOW_PART(object, 10)) {
        joint_axis.x = -1.0F;
        joint_axis.z = 0.0F;
        joint_axis.y = 0.0F;
        njCalcPoint(NULL, &joint_pos, &joint_pos);
        njCalcPoint(&inverse_view, &joint_pos, &super->unk_20c);
        njCalcVector(NULL, &joint_axis, &joint_axis);
        njCalcVector(&inverse_view, &joint_axis, &super->unk_2a8);
        njGetMatrix(&shadow_right_jet);
    } else if (SHADOW_PART(object, LEFT_FLARE_PART)) {
        njGetMatrix(&shadow_left_flare);
    } else if (SHADOW_PART(object, RIGHT_FLARE_PART)) {
        njGetMatrix(&shadow_right_flare);
    }
    if (SHADOW_PART(object, WRIST_PART)) {
        njGetMatrix(&super_wrist);
    } else if (SHADOW_PART(object, RING_PART)) {
        njGetMatrix(&super_ring);
    }
    if (SHADOW_PART(object, LEFT_SHOE_PART)) {
        njGetMatrix(&super_left_shoe);
    } else if (SHADOW_PART(object, RIGHT_SHOE_PART)) {
        njGetMatrix(&super_right_shoe);
    }
}

/* the glow's level grows by BYTE_TOP over each GLOW_RISE_STEPS of the clock
 * past LATE_STEPS. the glow draws only once it is above 0. its opaque
 * vertices then take level + GLOW_FLOOR in each byte of their colors, up to
 * WHITE */
#define GLOW_RISE_STEPS 18000
#define GLOW_FLOOR 128
#define BYTE_TOP 255
#define ALPHA 0xff000000 /* a color's alpha byte */
#define WHITE (-1) /* BYTE_TOP in each byte */
/* the word of the glow's chunk that holds a vertex's color. it counts from
 * the vertex src has stepped to. the chunk's two head words lie before the
 * first vertex. a vertex's three words of point lie before its color */
#define GLOW_COLOR (2 + 3)
/* the glow draws at the head's place, unk_218, turned GLOW_TILT about z. it
 * then moves GLOW_DROP along y, scales by GLOW_WIDTH and GLOW_HEIGHT, and
 * moves GLOW_RISE along y */
#define GLOW_TILT 0xd800
#define GLOW_DROP (-14.0F)
#define GLOW_WIDTH 1.2F
#define GLOW_HEIGHT 1.3F
#define GLOW_RISE 5.0F
/* the switch flash draws FLASH_DROP along y from the form's place */
#define FLASH_DROP (-3.0F)
/* the angles the attack's trail turns about z by, for each move. each holds
 * through the move's animation and the one after it */
#define TRAIL_RISE (HALF_TURN + QUARTER_TURN)
#define TRAIL_DIVE QUARTER_TURN
#define TRAIL_FLY_UP 0xf4a0
#define TRAIL_FLY_DOWN 0x105b
/* whether a player casts a shadow under its selected light. LIGHT_UNK_1 and
 * LIGHT_VOLUME cast none */
#define CASTS_SHADOW(slot)                                                     \
    (light_selected[(slot)] != LIGHT_UNK_1 &&                                  \
     light_selected[(slot)] != LIGHT_VOLUME)

static void super_draw(struct task *task)
{
    struct player *super = task->extra.p;
    struct task_work *work = task->work;
    struct player *record = COMMON_PLAYER(super);
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
#ifdef NONMATCHING
    /* this side starts trail and model at null. a character other than the two
     * supers sets neither, and the code below reads both */
    NJS_CNK_OBJECT *trail = NULL;
    NJS_CNK_MODEL *model = NULL;
#else
    NJS_CNK_OBJECT *trail;
    NJS_CNK_MODEL *model;
#endif
    int action;
    int base;
    int i;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    struct chunk_vertex_d8 *src;
    struct chunk_vertex_d8 *dst;
#else
    /* @hack type: src and dst as words, stepped a vertex's CHUNK_D8_WORDS
     * at a time, where struct chunk_vertex_d8 pointers would do */
    Sint32 *src;
    Sint32 *dst;
#endif
    Sint32 color;
    Sint32 level;
    Uint32 frame;
    Sint16 id;
    NJS_CNK_OBJECT *draw;

    /* @hack redundant: a local that only sizes the frame */
    Float unused[4]; /* NOLINT(clang-diagnostic-unused-variable) */

    /* @hack redundant: an arm that never runs, a read of super_flash's
     * model */
    if (0) {
        part = &super_flash;
        if (part->model) {
        }
    }
#ifdef NONMATCHING
    /* this side draws no more while finalhazard_players or the effect textures'
     * owner is missing. the draws below read both */
    if (!finalhazard_players || !effect_texture_owner) {
        return;
    }
#endif
    /* @hack type: game_state's cleared read as game_state_cleared, where
     * game_state.cleared would do */
    if (finalhazard_players->active != record->slot &&
        game_state_cleared != 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    if (CASTS_SHADOW(record->slot)) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    drawn_player = record;
    njGetMatrix(&inverse_view);
    njInvertMatrix(&inverse_view);
    njCnkSetMotionCallback(record->character == PLAYER_SUPER_SONIC
                               ? super_sonic_joint
                               : super_shadow_joint);
    action = record->anim.current;
    njSetTexture(super->by_character.super.textures);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, HALF_TURN - work->ang.y);
    njScaleEx(&work->scl);
    if (super->by_character.super.quills != NULL) {
        if (record->character == PLAYER_SUPER_SONIC) {
            model = model_table[SUPER_SONIC_QUILLS].object->child->model;
            model_table[SUPER_SONIC_QUILLS].object->child->model =
                super->by_character.super.quills->copy->model;
        } else if (record->character == PLAYER_SUPER_SHADOW) {
            model = model_table[SUPER_SHADOW_QUILLS].object->child->model;
            model_table[SUPER_SHADOW_QUILLS].object->child->model =
                super->by_character.super.quills->copy->model;
        }
    }
    if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
        player_hide_event_models(record->slot);
        njCnkSimpleDrawMotion(model_table[record->anim.actions->model].object,
                              record->event->motion, mini_event_frame);
        player_draw_event_parts(record->slot);
    } else {
        object = model_table[record->anim.actions[action].model].object;
        if (record->anim.mode != PLAYER_ANIM_BLEND) {
            /* @hack spelling: draw, a copy of object, where object would
             * do */
            draw = object;
            njCnkSimpleDrawMotion(
                draw,
                motion_table[record->anim
                                 .actions[record->anim.actions[action].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(object, record->anim.blend,
                                  record->anim.frame);
        }
        if (record->character == PLAYER_SUPER_SHADOW &&
            (action == ANIM_RUN || action == ANIM_SPRINT ||
             action == ANIM_FULL_SPEED)) {
            /* @hack redundant: a sine and a cosine of the step's phase, which
             * nothing reads */
            njSin((game_state.steps & 0x1f) << 11);
            njCos((game_state.steps & 0x1f) << 11);
            draw_control_clear(DRAW_CONTROL_SHADOW);
            njPushMatrixEx();
            if (game_state.steps & 1) {
                part = model_table[RIGHT_JET].object;
            } else {
                part = model_table[RIGHT_JET + 1].object;
            }
            njSetMatrix(NULL, &shadow_right_jet);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[LEFT_JET].object;
            } else {
                part = model_table[LEFT_JET + 1].object;
            }
            njSetMatrix(NULL, &shadow_left_jet);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[RIGHT_FLARE].object;
            } else {
                part = model_table[RIGHT_FLARE + 1].object;
            }
            njSetMatrix(NULL, &shadow_right_flare);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[LEFT_FLARE].object;
            } else {
                part = model_table[LEFT_FLARE + 1].object;
            }
            njSetMatrix(NULL, &shadow_left_flare);
            njCnkEasyDrawObject(part);
            njPopMatrixEx();
            if (CASTS_SHADOW(record->slot)) {
                draw_control_set(DRAW_CONTROL_SHADOW);
            }
        }
    }
    if (super->by_character.super.quills != NULL) {
        if (record->character == PLAYER_SUPER_SONIC) {
            model_table[SUPER_SONIC_QUILLS].object->child->model = model;
        } else if (record->character == PLAYER_SUPER_SHADOW) {
            model_table[SUPER_SHADOW_QUILLS].object->child->model = model;
        }
    }
    if (record->anim.current == ANIM_ARRIVE) {
        njSetTexture(&super_effect_textures);
        if (record->character == PLAYER_SUPER_SHADOW) {
            base = FLASH_FRAMES;
        } else {
            base = 0;
        }
        /* @hack spelling: id, the texture's word read and stored back
         * through a local, where one assignment would do */
        id = SUPER_FLASH_TEXTURE;
        id = (id & NJD_FFM_MASK) |
             (super->by_character.super.flash_frame + base + FLASH_TEXTURE);
        SUPER_FLASH_TEXTURE = id;
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, FLASH_DROP, 0.0F);
        njCnkEasyDrawObject(&super_flash);
        njPopMatrixEx();
    } else if (!(work->flags & PLAYER_WORK_ATTACK) &&
               game_state.winner_posed != 1) {
        level = ((Sint32)game_state.steps - LATE_STEPS) * BYTE_TOP /
                GLOW_RISE_STEPS;
        if (level <= 0) {
            level = 0;
        } else {
            level += GLOW_FLOOR;
            if (level > BYTE_TOP) {
                level = WHITE;
            } else {
                level |= (Uint32)level << 24 | (Uint32)level << 16 |
                         (Uint32)level << 8;
            }
        }
        if (level != 0) {
#ifdef NONMATCHING
            /* the plain form of the hack tagged at src's declaration */
            src = super_glow_source.vertices;
            dst = super_glow_vertices.vertices;
            for (i = 0; i < SUPER_GLOW_VERTICES; i++) {
                color = src->color;
                if ((color & ALPHA) == ALPHA) {
                    dst->color = level;
                }
                src++;
                dst++;
            }
#else
            src = (Sint32 *)&super_glow_source;
            dst = (Sint32 *)&super_glow_vertices;
            for (i = 0; i < SUPER_GLOW_VERTICES; i++) {
                color = src[GLOW_COLOR];
                if ((color & ALPHA) == ALPHA) {
                    dst[GLOW_COLOR] = level;
                }
                src += CHUNK_D8_WORDS;
                dst += CHUNK_D8_WORDS;
            }
#endif
            njSetTexture(&super_effect_textures);
            texture_anim_apply(&super_glow_model, &super_glow_animation,
                               game_state.steps);
            matrix_load_parent();
            njTranslateEx(&record->unk_218);
            njRotateZ(NULL, GLOW_TILT);
            njTranslate(NULL, 0.0F, GLOW_DROP, 0.0F);
            njScale(NULL, GLOW_WIDTH, GLOW_HEIGHT, GLOW_WIDTH);
            njTranslate(NULL, 0.0F, GLOW_RISE, 0.0F);
            njCnkSimpleDrawObject(&super_glow);
        }
    }
    njPopMatrixEx();
    if (work->flags & PLAYER_WORK_ATTACK) {
        njPushMatrixEx();
        njSetTexture(&super_effect_textures);
        njTranslateEx(&record->unk_230);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScaleEx(&work->scl);
        frame = game_state.steps;
        if (record->character == PLAYER_SUPER_SONIC) {
            trail = &super_sonic_trail;
            texture_anim_apply(trail->model, &super_sonic_trail_animation,
                               frame);
            /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
        } else if (record->character == PLAYER_SUPER_SHADOW) {
            trail = &super_shadow_trail;
            texture_anim_apply(trail->model, &super_shadow_trail_animation,
                               frame);
        }
        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
        switch (record->anim.current) {
        case ANIM_RISE:
        case ANIM_RISE + 1:
            njRotateZ(NULL, TRAIL_RISE);
            break;
        case ANIM_DIVE:
        case ANIM_DIVE + 1:
            njRotateZ(NULL, TRAIL_DIVE);
            break;
        case ANIM_FLY_UP:
        case ANIM_FLY_UP + 1:
            njRotateZ(NULL, TRAIL_FLY_UP);
            break;
        case ANIM_FLY_DOWN:
        case ANIM_FLY_DOWN + 1:
            njRotateZ(NULL, TRAIL_FLY_DOWN);
            break;
        }
#ifdef NONMATCHING
        /* this side draws no trail where neither super character set it */
        if (trail)
#endif
        {
            njCnkSimpleDrawObject(trail);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
}

/* PLAYER_MODE_FREE_MOVE's step, by the player's pad. B puts the player back
 * where stage_place_player puts it. the stick moves it FREE_MOVE_STEP a step.
 * while X is held it moves up or down, and else across as camera 0 sees it. a
 * stick within PAD_DEAD_ZONE of center on both axes moves nothing */
static void super_free_move(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *super)
{
    Float x;
    Float y;
    Float sign;
    Angle ang;

    if (player_pad_records[record->slot].press & PDD_DGT_TB) {
        stage_place_player(record->slot);
    }
    /* @hack type: the stick's axes made an int on their way to a Float,
     * where the Float alone would do */
    if (player_pad_records[record->slot].on & PDD_DGT_TX) {
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        if (x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
            y < -PAD_DEAD_ZONE) {
            if (y != 0.0F) {
                sign = y / njSqrt(y * y);
            } else {
                sign = 0.0F;
            }
            work->pos.y -= sign * FREE_MOVE_STEP;
        }
    } else {
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        /* @hack spelling: the return and the else, where the move under the
         * negated test would do */
        if (x <= PAD_DEAD_ZONE && x >= -PAD_DEAD_ZONE && y <= PAD_DEAD_ZONE &&
            y >= -PAD_DEAD_ZONE) {
            return;
        } else {
            ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN_F) -
                  camera_views[0]->ang.y;
            work->pos.x += njCos(ang) * FREE_MOVE_STEP;
            work->pos.z += njSin(ang) * FREE_MOVE_STEP;
        }
    }
}

static int super_take_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *super)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return 0;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &= ~MOVE_FLAGS;
        break;
    case PLAYER_REQUEST_RELEASE:
        if (record->request_arg) {
            work->mode = SUPER_HOVER;
            record->anim.next = record->request_arg;
            record->mode_steps = 0;
        } else {
            work->mode = SUPER_HOVER;
            record->mode_steps = 0;
            record->anim.next = ANIM_HOVER;
        }
        work->flags &= ~MOVE_FLAGS;
        break;
    case PLAYER_REQUEST_SCARE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &= ~MOVE_FLAGS;
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &=
            ~(WORK_HURT | PLAYER_WORK_UNK_8 | WORK_UNDER | WORK_ON_TOP);
        record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER |
                           PLAYER_MISSED | PLAYER_INVINCIBLE);
        work->mode = SUPER_HOVER;
        record->mode_steps = 0;
        record->anim.next = ANIM_HOVER;
        work->flags &= ~MOVE_FLAGS;
        break;
    case PLAYER_REQUEST_MISS:
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &= ~MOVE_FLAGS;
        if (record->character == PLAYER_SUPER_SONIC) {
            voice_play_priority_2(SONIC_MISS_VOICE);
        } else if (record->character == PLAYER_SUPER_SHADOW) {
            voice_play_priority_2(SHADOW_MISS_VOICE);
        }
        super->by_character.super.flight_steps = 0;
        break;
    default:
        return 0;
    }
    return 1;
}

static int super_stop_flight(struct task_work *work, struct player *record)
{
    if (player_read_stick(work, record, NULL, NULL) == 0 &&
        record->spd.x == 0.0F) {
        work->mode = SUPER_HOVER;
        record->mode_steps = 0;
        record->anim.next = ANIM_HOVER;
        return 1;
    }
    return 0;
}

static void super_flight_anim(struct player *record)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. the flight's
     * animation follows its speed outside the three animations */
    if (record->anim.current != ANIM_RECOVER &&
        record->anim.current != ANIM_SONIC_HIT &&
        record->anim.current != ANIM_SHADOW_HIT) {
#else
    /* @bug logic: the flight's animation never follows its speed. the test
     * asks one animation to be ANIM_RECOVER, ANIM_SONIC_HIT and
     * ANIM_SHADOW_HIT at once, at 0x8c5030a6 to 0x8c5030b6 */
    /* NOLINTNEXTLINE(*redundant-expression,*overlap-compare) */
    if (record->anim.current == ANIM_RECOVER &&
        record->anim.current == ANIM_SONIC_HIT &&
        record->anim.current == ANIM_SHADOW_HIT) {
#endif
        if (record->spd.x >= FAST_FLIGHT) {
            record->anim.next = ANIM_FLY_FAST;
        } else {
            record->anim.next = ANIM_FLY;
        }
    }
}

static int super_attack_weak_point(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record, struct player *super)
{
    struct task *task;
    NJS_VECTOR toward_target;

    if (!(task = finalhazard_weak_point()) || !task->work ||
        (task->work->flags & WORK_HURT)) {
        return 0;
    }
    toward_target = task->work->pos;
    njSubVector(&toward_target, &work->pos);
    if (njScalor(&toward_target) > HOMING_REACH) {
        return 0;
    }
    njUnitVector(&toward_target);
    player_face_along(record->slot, toward_target.x, toward_target.y,
                      toward_target.z);
    toward_target.x *= HOMING_SPEED;
    toward_target.y *= HOMING_SPEED;
    toward_target.z *= HOMING_SPEED;
    motion->spd = toward_target;
    turn_from_world(work, &toward_target);
    record->spd = toward_target;
    work->mode = SUPER_HOMING;
    record->anim.next = ANIM_FLY_FAST;
    work->flags |= PLAYER_WORK_ATTACK;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it returns TRUE */
    return TRUE;
#else
    /* @bug uninitialized: the homing's start returns no value, so its
     * callers test R0 as the store to work's flags at 0x8c5031ba leaves it */
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* the stick steers a player within STEER_TURN of its heading, and turns one
 * past it. a still player pivots in place. one from halfway between
 * trot_speed and running_speed up brakes. one slower, from trot_speed up,
 * keeps HARD_TURN_PUSH of its push for a turn past HARD_TURN */
#define STEER_TURN 0x1000
#define HARD_TURN EIGHTH_TURN
#define HARD_TURN_PUSH 0.8F
/* the stick's push below top_speed, as a share of ground_accel. below
 * trot_speed, a stick past WALK_STICK pushes by its strength. so does any
 * stick below CRAWL_SHARE of trot_speed, at MIN_STICK at least. below
 * running_speed any stick pushes by its strength, past TROT_STICK or not.
 * below sprint_speed a stick past RUN_STICK pushes by its strength, and any
 * other by RUN_SHARE of that */
#define WALK_STICK 0.6F
#define CRAWL_SHARE 0.4F
#define MIN_STICK 0.5F
#define TROT_STICK 0.7F
#define RUN_STICK 0.9F
#define RUN_SHARE 0.3F
/* the share of ground_accel that a stroke pushes a fast player by */
#define FAST_PUSH 0.4F
/* a pushed player faster than top_speed meets TOP_RESIST times resist.x of
 * the speed past it */
#define TOP_RESIST 1.7F

static void super_accelerate(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR push;
    NJS_VECTOR dir;
    NJS_VECTOR ahead;
    NJS_PLANE wall;
    NJS_PLANE ground;
    NJS_LINE line;
    Angle angle;
    Float strength;
    int held;
    int distance;

    acc = motion->acc;
    push.x = push.y = push.z = 0.0F;
    turn_from_world(work, &acc);
    held = player_read_stick(work, record, &angle, &strength);

    if (!held && record->spd.x > record->consts.running_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (held && record->spd.x > record->consts.top_speed) {
        acc.x += TOP_RESIST * (record->consts.resist.x *
                               (record->spd.x - record->consts.top_speed));
    } else if (record->spd.x > record->consts.top_speed) {
        acc.x += record->consts.resist.x *
                 (record->spd.x - record->consts.top_speed);
    } else if (record->spd.x < 0.0F) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;

    if (!held) {
        if (record->spd.x > 0.0F) {
            push.x = record->consts.ground_drag;
        } else if (record->spd.x < 0.0F) {
            push.x = -record->consts.ground_drag;
        }
        record->drive = 0.0F;
    } else {
        if (record->spd.x < record->consts.top_speed) {
            if (record->spd.x < record->consts.trot_speed) {
                if (strength > WALK_STICK) {
                    strength *= record->consts.ground_accel;
                } else if (record->spd.x <
                           record->consts.trot_speed * CRAWL_SHARE) {
                    if (strength < MIN_STICK) {
                        strength = MIN_STICK;
                    }
                    strength *= record->consts.ground_accel;
                } else {
                    strength = 0.0F;
                }
            } else if (record->spd.x < record->consts.running_speed) {
                if (strength > TROT_STICK) {
                    strength *= record->consts.ground_accel;
                } else if (/* @bug logic: always holds, at 0x8c503408, so
                            * the push never drops to 0. the bound below is
                            * running_speed, and this arm runs only below
                            * running_speed. the brake's bound is the
                            * midpoint of trot_speed and running_speed */
                           record->spd.x < (record->consts.running_speed +
                                            record->consts.running_speed) *
                                               0.5F) {
                    strength *= record->consts.ground_accel;
                } else {
                    strength = 0.0F;
                }
            } else if (record->spd.x < record->consts.sprint_speed) {
                if (strength > RUN_STICK) {
                    strength *= record->consts.ground_accel;
                } else {
                    strength *= record->consts.ground_accel * RUN_SHARE;
                }
            } else {
                strength *= record->consts.ground_accel;
            }
        } else if (record->spd.x >= record->consts.top_speed &&
                   record->level < 0.0F) {
            strength *= record->consts.ground_accel;
        } else {
            strength *= record->consts.ground_accel * FAST_PUSH;
        }

        /* a stick into the wall the player touches steers along the line
         * where the wall meets the ground */
        if (njScalor(&record->wall_normal) > 0.0F) {
            ahead.x = 1.0F;
            ahead.y = ahead.z = 0.0F;
            matrix_push_unit();
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -angle);
            njCalcVector(NULL, &ahead, &dir);
            njPopMatrixEx();
            if (njInnerProduct(&dir, &record->wall_normal) < 0.0F) {
                wall.px = 0.0F;
                wall.py = 0.0F;
                wall.pz = 0.0F;
                wall.vx = record->wall_normal.x;
                wall.vy = record->wall_normal.y;
                wall.vz = record->wall_normal.z;
                ground.px = 0.0F;
                ground.py = 0.0F;
                ground.pz = 0.0F;
                ground.vx = record->ground_normal.x;
                ground.vy = record->ground_normal.y;
                ground.vz = record->ground_normal.z;
                njDistancePL2PL(&wall, &ground, &line);
                if (njInnerProduct((NJS_VECTOR *)&line.vx, &dir) < 0.0F) {
                    line.vx = -line.vx;
                    line.vy = -line.vy;
                    line.vz = -line.vz;
                }
                angle -= vector_angle(&dir, (NJS_VECTOR *)&line.vx,
                                      &record->ground_normal);
            }
        }

        distance = angle_distance(work->ang.y, angle);
        if (record->spd.x == 0.0F && distance > STEER_TURN) {
            motion->aim_y = angle;
            strength = 0.0F;
            player_turn_on_course(work, motion, record, motion->aim_y);
        } else if (record->spd.x >= (record->consts.trot_speed +
                                     record->consts.running_speed) *
                                        0.5F &&
                   distance > STEER_TURN) {
            motion->aim_y = angle;
            strength = record->consts.ground_drag;
            player_turn_toward(work, motion, record, motion->aim_y);
        } else if (record->spd.x >= record->consts.trot_speed &&
                   distance < STEER_TURN) {
            motion->aim_y = angle;
            player_turn_at_speed(work, motion, record, motion->aim_y);
        } else if (record->consts.trot_speed <= record->spd.x &&
                   record->spd.x <= record->consts.sprint_speed &&
                   distance > HARD_TURN) {
            motion->aim_y = angle;
            strength *= HARD_TURN_PUSH;
            player_turn_toward(work, motion, record, motion->aim_y);
        } else {
            motion->aim_y = angle;
            player_turn_toward(work, motion, record, motion->aim_y);
        }
        record->drive = strength;
        push.x = strength;
    }

    njAddVector(&acc, &push);
    record->accel = acc;
}

static void super_coast(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR push;

    acc = motion->acc;
    push.x = push.y = push.z = 0.0F;
    turn_from_world(work, &acc);

    if (record->spd.x > record->consts.running_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (record->spd.x < 0.0F) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;

    if (record->spd.x > 0.0F) {
        push.x = record->consts.ground_drag;
    } else if (record->spd.x < 0.0F) {
        push.x = -record->consts.ground_drag;
    }
    record->drive = 0.0F;

    njAddVector(&acc, &push);
    record->accel = acc;
}

/* stop each stream's voice of VOICE_PRIORITY, and return 0. it returns
 * NO_VOICE at the first stream that plays another voice, and stops no more */
static int super_clear_voices(void)
{
    int i;
    int busy = 0;

    if (sound_streams != NULL) {
        for (i = 0; i < VOICES; i++) {
            switch (sound_streams->voices[i].status) {
            case ADXT_STAT_STOP:
            case ADXT_STAT_PLAYEND:
                continue;
            }
            if (sound_streams->voices[i].priority == VOICE_PRIORITY) {
                voice_stop(i);
                continue;
            }
            busy = NO_VOICE;
            break;
        }
    }
    return busy;
}

void finalhazard_switch_players(void)
{
    int voice;
    int player;
    int other;

    finalhazard_players->transition = SWITCH_FADE_OUT;
    finalhazard_players->switch_timer = 0;
    pause_disable();
    pad_disable_player(PAD_BOTH_PLAYERS);
    game_state.idle_dim = 0;
    if (finalhazard_players->turns[finalhazard_players->active] == 0) {
        if (finalhazard_players->active == 0) {
            voice = SONIC_FIRST_LEAVE_VOICE;
        } else {
            voice = SHADOW_FIRST_LEAVE_VOICE;
        }
    } else if (finalhazard_players->hit_switch == 1) {
        if (finalhazard_players->active == 0) {
            voice = SONIC_HIT_LEAVE_VOICE;
        } else {
            voice = SHADOW_HIT_LEAVE_VOICE;
        }
    } else {
        if (finalhazard_players->active == 0) {
            voice = SONIC_LEAVE_VOICE;
        } else {
            voice = SHADOW_LEAVE_VOICE;
        }
    }
    if (voice != NO_VOICE && super_clear_voices() == 0 &&
        game_state.cleared != 1 && game_state.winner_posed != 1) {
        voice_play(voice, VOICE_PRIORITY);
    }
    if (finalhazard_players->hit_switch == 1) {
        finalhazard_players->hits++;
        /* @hack spelling: two arms that store RING_BUDGET, where one would
         * do */
        /* @hack redundant: the labels 4 and 5 beside default */
        switch (finalhazard_players->hits) {
        case 0:
        case 1:
            finalhazard_players->ring_budget = RING_BUDGET;
            break;
        case 2:
        case 3:
            finalhazard_players->ring_budget = RING_BUDGET;
            break;
        case 4:
        case 5:
        default:
            finalhazard_players->ring_budget = LATE_RING_BUDGET;
            break;
        }
        if (finalhazard_players->active == 0) {
            player = 0;
            other = 1;
        } else {
            player = 1;
            other = 0;
        }
        ring_add(other,
                 finalhazard_players->ring_budget - ring_count_int(other));
        ring_add(player, -ring_count_int(player));
        switch (player_records[player]->character) {
        case PLAYER_SUPER_SONIC:
            player_records[player]->anim.next = ANIM_SONIC_HIT;
            break;
        case PLAYER_SUPER_SHADOW:
            player_records[player]->anim.next = ANIM_SHADOW_HIT;
            break;
        }
    }
    finalhazard_players->hit_switch = 0;
    if (finalhazard_players->turns[finalhazard_players->active] < MOST_TURNS) {
        finalhazard_players->turns[finalhazard_players->active]++;
    }
}

/* a player outside flight_radius goes back to CONFINE_INSET inside it */
#define CONFINE_INSET 1.0F

static void super_confine_players(void)
{
    int i;
    NJS_POINT3 pos;
    NJS_POINT3 center;
    Float dx;
    Float dz;
    Float scale;

    if (finalhazard_players->transition == SWITCH_LOST) {
        return;
    }
    for (i = 0; i < FINALHAZARD_PLAYERS; i++) {
        if (player_tasks[i] == NULL) {
            continue;
        }
        pos = player_work[i]->pos;
        center = flight_center;
        pos.y = 0.0F;
        center.y = 0.0F;
        if (njDistanceP2P(&pos, &center) > flight_radius) {
            dx = pos.x - center.x;
            dz = pos.z - center.z;
            scale = (flight_radius - CONFINE_INSET) *
                    njInvertSqrt((dx * dx) + (dz * dz));
            pos.x = center.x + (dx * scale);
            pos.z = center.z + (scale * dz);
        }
        if (pos.x > flight_x_limit) {
            pos.x = flight_x_limit;
        }
        pos.y = player_work[i]->pos.y;
        if (pos.y > flight_ceiling) {
            pos.y = flight_ceiling;
        }
        if (pos.y < flight_floor) {
            pos.y = flight_floor;
        }
        player_set_pos(i, pos.x, pos.y, pos.z);
    }
}

/* the radio's timed messages, as RADIO_MESSAGE plays them. a message's task
 * is a child of task, killed as the next message starts. the clear or a
 * winner's pose kills it for good */
/* from seconds on the clock, play message number sonic for Super Sonic. play
 * the one after it for Super Shadow. next is the count of timed messages
 * played after it */
#define RADIO_MESSAGE(next, seconds, sonic)                                    \
    if (elapsed_steps >= (seconds) * STEPS_PER_SECOND) {                       \
        if (players->active == 0) {                                            \
            message = (sonic);                                                 \
        } else {                                                               \
            message = (sonic) + 1;                                             \
        }                                                                      \
        players->radio_message = (next);                                       \
    }

static void super_radio_step(struct task *task,
                             struct finalhazard_players *players)
{
    NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};
    int elapsed_steps;
    int message;

    elapsed_steps = game_state.steps;
    message = NO_MESSAGE;
    if (game_state.cleared == 1 || game_state.winner_posed == 1) {
        if (task->child != NULL) {
            task_kill(task->child);
        }
        return;
    }
    switch (players->radio_message) {
    case 0:
        RADIO_MESSAGE(1, 30, 4);
        break;
    case 1:
        RADIO_MESSAGE(2, 60, 14);
        break;
    case 2:
        RADIO_MESSAGE(3, 90, 6);
        break;
    case 3:
        RADIO_MESSAGE(4, 120, 8);
        break;
    case 4:
        RADIO_MESSAGE(5, 150, 10);
        break;
    case 5:
        RADIO_MESSAGE(6, 180, 12);
        break;
    case 6:
        RADIO_MESSAGE(7, 210, 16);
        break;
    case 7:
        RADIO_MESSAGE(8, 240, 18);
        break;
    case 8:
        RADIO_MESSAGE(9, 270, 20);
        break;
    case 9:
        RADIO_MESSAGE(10, 290, 22);
        break;
    }
    if (message != NO_MESSAGE) {
        if (task->child != NULL) {
            task_kill(task->child);
        }
        messenger_create(&pos, 0, 0, message, MESSENGER_TALK_WINDOW, task);
    }
}

/* the camera's motions in a switch, in motion_table, by the arriving player.
 * the first plays through the fade in, and the second as the player poses */
#define SONIC_FADE_IN_CAMERA 108
#define SONIC_ARRIVE_CAMERA 106
#define SHADOW_FADE_IN_CAMERA 115
#define SHADOW_ARRIVE_CAMERA 111
#define SWITCH_CAMERA_FLAGS                                                    \
    (CAMERA_MOTION_HOLD | CAMERA_MOTION_FOLLOWS | CAMERA_MOTION_TURNS)
/* the frames a step the fade in's motion plays, and the pose's */
#define FADE_IN_CAMERA_SPEED 1.0F
#define ARRIVE_CAMERA_SPEED 0.5F
/* the switch's fade to opaque black, and back out of it */
#define FADE_TO_BLACK() fade_to_color_step(0xff, 0, 0, 0)
#define FADE_FROM_BLACK() fade_to_color_step(0, 0, 0, 0)
/* the player who loses its last ring drifts at an acceleration of
 * (LOST_ACC_X, LOST_ACC_Y). it sparkles every LOST_SPARKLE_MASK + 1 steps.
 * a sparkle starts LOST_RADIUS plus up to LOST_RADIUS_SPREAD from unk_230, at
 * a random angle. on each axis it drifts at its LOST_DRIFT_ plus or less up
 * to its _SPREAD. its scale is LOST_SCALE plus up to LOST_SCALE_SPREAD */
#define LOST_ACC_X 0.2F
#define LOST_ACC_Y 0.01F
#define LOST_SPARKLE_MASK 3
#define LOST_RADIUS 4.0F
#define LOST_RADIUS_SPREAD 3.0F
#define LOST_DRIFT_X (-0.2F)
#define LOST_DRIFT_X_SPREAD 0.1F
#define LOST_DRIFT_Y 0.08F
#define LOST_DRIFT_Y_SPREAD 0.04F
#define LOST_DRIFT_Z (-0.05F)
#define LOST_DRIFT_Z_SPREAD 0.1F
#define LOST_SCALE 1.0F
#define LOST_SCALE_SPREAD 0.5F

static void super_manager_step(struct task *task)
{
    struct finalhazard_players *players;
    struct finalhazard_players *active;
    int player;
    int other;
    int voice;
    struct game_state *state;

    players = task->extra.p;
#ifdef NONMATCHING
    /* this side steps nothing while either player is missing, which the step
     * reads */
    if (!player_work[0] || !player_work[1]) {
        return;
    }
#endif
    if (stage_get_mode() == STAGE_FADE_IN) {
        players->active = 0;
        players->transition = SWITCH_PLAY;
        players->switch_timer = 0;
        players->ring_budget = RING_BUDGET;
        /* @hack spelling: the fields cleared in two chains, where a statement
         * each would do */
        players->turns[0] = players->hit_switch = players->hits = 0;
        players->radio_message = players->turns[1] = 0;
    }
    pad_share_first();
    /* @hack spelling: state, game_state's address in a local, where
     * game_state would do */
    state = &game_state;

    /* @hack spelling: active, a copy of players, where players would do */
    active = players;
    if (active->active == 0) {
        player = 0;
        other = 1;
    } else {
        player = 1;
        other = 0;
    }
    state->active_player = player;
    if (state->steps == LATE_STEPS) {
        sound_play(SOUND_LATE, NULL, 0, 0);
    }

    switch (players->transition) {
    case SWITCH_FADE_OUT:
        if (players->switch_timer > SWITCH_FADE_STEPS && FADE_TO_BLACK()) {
            struct camera_motion param;

            players->transition = SWITCH_FADE_IN;
            players->switch_timer = 0;
            if (players->active == 0) {
                players->active = 1;
            } else {
                players->active = 0;
            }
            player_stop(player_motion[0], player_records[0]);
            stage_place_player_again(0);
            player_stop(player_motion[1], player_records[1]);
            stage_place_player_again(1);
            player_work[player]->mode = SUPER_SWITCH;
            player_records[player]->anim.next = ANIM_LEAVE;
            param.origin = &player_work[players->active]->pos;
            param.ang = HALF_TURN - player_work[players->active]->ang.y;
            param.motion = NULL;
            param.flags = SWITCH_CAMERA_FLAGS;
            param.done = NULL;
            if (player_records[players->active]->character ==
                PLAYER_SUPER_SONIC) {
                param.motion = motion_table[SONIC_FADE_IN_CAMERA].motion;
            } else {
                param.motion = motion_table[SHADOW_FADE_IN_CAMERA].motion;
            }
            param.speed = FADE_IN_CAMERA_SPEED;
            if (camera_top(0) > CAMERA_MOTION_SLOT) {
                camera_remove_mode(0, camera_top(0));
            }
            camera_motion_start(0, &param);
        }
        break;
    case SWITCH_FADE_IN:
        if (FADE_FROM_BLACK()) {
            struct camera_motion param;

            players->transition = SWITCH_WAIT;
            players->switch_timer = 0;
            player_work[player]->mode = SUPER_SWITCH;
            player_records[player]->anim.next = ANIM_ARRIVE;
            player_records[player]->anim.after = ANIM_HOVER;
            camera_motion_end(0);
            param.origin = &player_work[players->active]->pos;
            param.ang = HALF_TURN - player_work[players->active]->ang.y;
            param.motion = NULL;
            param.flags = SWITCH_CAMERA_FLAGS;
            param.done = NULL;
            if (player_records[players->active]->character ==
                PLAYER_SUPER_SONIC) {
                param.motion = motion_table[SONIC_ARRIVE_CAMERA].motion;
            } else {
                param.motion = motion_table[SHADOW_ARRIVE_CAMERA].motion;
            }
            param.speed = ARRIVE_CAMERA_SPEED;
            if (camera_top(0) > CAMERA_MOTION_SLOT) {
                camera_remove_mode(0, camera_top(0));
            }
            camera_motion_start(0, &param);
            if (super_clear_voices() != NO_VOICE) {
                if (players->active == 0) {
                    if (players->turns[player] <= 1) {
                        voice_play(SONIC_FIRST_ARRIVE_VOICE, VOICE_PRIORITY);
                    } else {
                        voice_play(SONIC_ARRIVE_VOICE, VOICE_PRIORITY);
                    }
                } else {
                    if (players->turns[player] <= 0) {
                        voice_play(SHADOW_FIRST_ARRIVE_VOICE, VOICE_PRIORITY);
                    } else {
                        voice_play(SHADOW_ARRIVE_VOICE, VOICE_PRIORITY);
                    }
                }
            }
        }
        break;
    case SWITCH_WAIT:
        if (players->switch_timer > SWITCH_WAIT_STEPS) {
            players->transition = SWITCH_PLAY;
            players->switch_timer = 0;
            pause_enable();
            pad_enable_player(PAD_BOTH_PLAYERS);
            state->idle_dim = 1;
            camera_motion_end(0);
        }
        break;
    case SWITCH_LOST:
        if (!(state->steps & LOST_SPARKLE_MASK)) {
            NJS_POINT3 pos;
            NJS_VECTOR spd;
            NJS_POINT3 off;
            Float radius;
            Angle ang_x;
            Angle ang_y;
            Angle angle;

            pos = player_records[player]->unk_230;
            spd.x =
                LOST_DRIFT_X - (rand() * (LOST_DRIFT_X_SPREAD * RAND_SCALE));
            spd.y =
                LOST_DRIFT_Y - (rand() * (LOST_DRIFT_Y_SPREAD * RAND_SCALE));
            spd.z =
                (rand() * (LOST_DRIFT_Z_SPREAD * RAND_SCALE)) + LOST_DRIFT_Z;
            radius = (rand() * (LOST_RADIUS_SPREAD * RAND_SCALE)) + LOST_RADIUS;
            ang_x = QUARTER_TURN - (rand() * (HALF_TURN * RAND_SCALE));
            ang_y = RANDOM_TURN();

            /* @hack spelling: angle, a copy of ang_y, where ang_y would do */
            angle = ang_y;
            off.x = radius * njCos(ang_x) * njSin(angle);
            off.y = radius * njSin(ang_x);
            off.z = radius * njCos(ang_x) * njCos(angle);
            njAddVector(&off, &pos);
            ring_sparkle_create_long(
                &off, &spd,
                (rand() * (LOST_SCALE_SPREAD * RAND_SCALE)) + LOST_SCALE);
        }
        player_motion[player]->acc.x = LOST_ACC_X;
        player_motion[player]->acc.y = LOST_ACC_Y;
        break;
    case SWITCH_PLAY:
        if (finalhazard_debug_attack == 0 && game_state.cleared != 1 &&
            game_state.winner_posed != 1) {
            players->ring_timer++;
            if (players->ring_timer > RING_STEPS) {
                voice = ANY_VOICE;
                if (super_clear_voices() == NO_VOICE) {
                    voice = NO_VOICE;
                }
                if (state->cleared == 1) {
                    voice = NO_VOICE;
                }
                players->ring_timer = 0;
                if (state->steps >= DRAIN_STEPS) {
                    ring_add(0, -ring_count_int(0));
                    ring_add(1, -ring_count_int(1));
                    players->ring_budget = 0;
                } else {
                    ring_add(player, -1);
                    ring_add(other, players->ring_budget -
                                        ring_count_int(player) -
                                        ring_count_int(other));
                }
                /* @hack type: the ring counts narrowed to Sint16, where
                 * ring_count_int's int would do */
                if ((Sint16)ring_count_int(player) == RINGS_LOW) {
                    /* @hack spelling: the empty arm, where a test of voice
                     * != NO_VOICE would do */
                    if (voice == NO_VOICE) {
                    } else {
                        if (players->active == 0) {
                            voice = SONIC_RINGS_LOW_VOICE;
                        } else {
                            voice = SHADOW_RINGS_LOW_VOICE;
                        }
                    }
                    if (voice >= 0) {
                        voice_play(voice, VOICE_PRIORITY);
                    }
                } else if ((Sint16)ring_count_int(player) == RINGS_LAST) {
                    /* @hack spelling: the empty arm, where a test of voice
                     * != NO_VOICE would do */
                    if (voice == NO_VOICE) {
                    } else {
                        if (players->active == 0) {
                            voice = SONIC_RINGS_LAST_VOICE;
                        } else {
                            voice = SHADOW_RINGS_LAST_VOICE;
                        }
                    }
                    if (voice >= 0) {
                        voice_play(voice, VOICE_PRIORITY);
                    }
                } else if ((Sint16)ring_count_int(player) == 0) {
                    player_miss(player);
                    players->transition = SWITCH_LOST;
                    pad_disable_player(PAD_BOTH_PLAYERS);
                    camera_push_mode(0, CAMERA_MODE_LEAVE);
                    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
                }
            }
        }
        {
            struct task *boss;
            struct player *record;

            if (!((boss = finalhazard_weak_point()) && boss->work &&
                  (boss->work->flags & WORK_HURT)) &&
                players->hit_switch != 1) {
                if (player_records[0]->anim.current != ANIM_SONIC_HIT &&
                    player_records[0]->anim.next != ANIM_SONIC_HIT) {
                    /* @hack spelling: record set inside the test, and the
                     * empty arm, where tests of player_records[1] would do */
                    if ((record = player_records[1])->anim.current ==
                            ANIM_SHADOW_HIT ||
                        record->anim.next == ANIM_SHADOW_HIT) {
                    } else {
                        if (players->active == 0) {
                            if (player_work[0] &&
                                player_work[0]->pos.z < sonic_switch_z) {
                                finalhazard_switch_players();
                            }
                        } else {
                            if (player_work[1] &&
                                player_work[1]->pos.z > shadow_switch_z) {
                                finalhazard_switch_players();
                            }
                        }
                    }
                }
            }
        }
        /* fall through */
    default:
        if (pad_player_is_enabled(0) && pad_player_is_enabled(1)) {
            if (players->active == 0) {
                pad_enable_player(0);
                pad_disable_player(1);
            } else {
                pad_enable_player(1);
                pad_disable_player(0);
            }
        }
        break;
    }
    super_confine_players();
    super_radio_step(task, players);
    players->switch_timer++;
}

static void super_manager_destroy(struct task *task)
{
    struct finalhazard_players *players = task->extra.p;

    if (players == finalhazard_players) {
        finalhazard_players = NULL;
    }
}

static void super_manager_start(void)
{
    struct task *task;
    struct finalhazard_players *players;

    if (finalhazard_players == NULL) {
        if ((task = task_create(0, MANAGER_LEVEL, super_manager_step,
                                "LastBoss2GamePlayersManagerExecutor"))) {
            if (!(players = syCalloc(1, sizeof(struct finalhazard_players)))) {
                task_kill(task);
                return;
            }
            task->extra.p = players;
            finalhazard_players = players;
            task->destructor = super_manager_destroy;
        }
    }
}
