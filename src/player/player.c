/* the players: their tables, their records, and the steps every character
 * shares */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "effect/barrier.h"
#include "enemy/enemy.h"
#include "event/mini_event.h"
#include "hud/hud.h"
#include "object/fall.h"
#include "player/action.h"
#include "player/eggman.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "player/sonic.h"
#include "player/tails.h"
#include "player/walker.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/relocate.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

#ifdef NONMATCHING
/* this side turns a vector by zam_vec3_turn_y, zam_vec3_turn_yxz and
 * zam_vec3_turn_zxy, in place of Ninja's matrices. for a finite vector they
 * give the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

/* the steps of each player's past that past_pos and past_ang keep */
#define PAST_STEPS 256

/* below this, a pointer in a model or motion file is still an offset from
 * the file's start */
#define FILE_OFFSET_LIMIT 0x0c000000

/* a character past the mechs. character_constants holds its row, but no
 * source makes a player of it */
#define PLAYER_CHARACTER_8 8

/* how far a touched task may lie from the player's heading to be offered */
#define OFFER_RANGE 0x3000
/* farther than any touched task, for the search of the nearest */
#define TOUCHED_FAR 1000000.0F

/* the gravity, resist.x and air_resist of a player on a stage in space. every
 * character but Tails and Eggman takes them */
#define SPACE_GRAVITY 0.07F
#define SPACE_RESIST_X (-0.007F)
#define SPACE_AIR_RESIST (-0.019F)

/* the steps a player's eyes stay under water before its breath task starts */
#define BREATH_STEPS 1140

/* a breath task's extra. the task, BreathCounterP, counts down a player's
 * breath under water */
struct breath {
    Uint8 slot; /* the player's slot */
    /* 1 once the drown has paused the music. the step that sets it also
     * makes breath_music_resume the destructor, in place of breath_destroy */
    Sint8 music_paused;
    Uint8 unk_02[2];
    Sint32 steps; /* the steps the task has taken */
};
#define BREATH(task) ((struct breath *)(task)->extra.p)

/* the bits of game_state.steps that pick the heading a confused player's
 * stick asks for. each pick lasts 8 steps. where the stick's own heading is a,
 * the four give QUARTER_TURN + a, HALF_TURN - a, a + HALF_TURN and
 * QUARTER_TURN - a */
#define CONFUSED_WAYS 0x18
#define CONFUSED_TURNED 0x00
#define CONFUSED_MIRRORED 0x08
#define CONFUSED_REVERSED 0x10
#define CONFUSED_FLIPPED 0x18
/* while world_gravity.y is above this, player_read_stick mirrors the stick's
 * heading a to HALF_TURN - a. it does so only off STAGE_MAD_SPACE, and where
 * the player touches no SHAPE_ID_LINE line */
#define GRAVITY_UP_Y 0.1F
/* a touched SHAPE_ID_LINE line whose up has a y above this stands upright.
 * the pad's y1 then picks the way along it, not the stick's heading */
#define LINE_UPRIGHT_Y 0.99F
/* the turns from a way past which a heading grades PLAYER_WALL_AWAY,
 * PLAYER_WALL_ACROSS and PLAYER_WALL_ASKEW. player_face_offer,
 * player_stick_into_wall and player_face_wall grade within TURN_HEAD_ON_WIDE
 * head on, and the others within TURN_ASKEW. player_stick_back takes a stick
 * past TURN_AWAY from the heading for a turn back */
#define TURN_AWAY 0x6000
#define TURN_ACROSS 0x3400
#define TURN_HEAD_ON_WIDE 0x1c00
#define TURN_ASKEW 0x1400

/* the bits of turn_to_target's how. TARGET_FAST and TARGET_AT_ONCE choose the
 * step of the turn. under TARGET_LANDED, a target whose enemy_work lacks
 * ENEMY_ON_FLOOR leaves the turn to the stick. under TARGET_AHEAD, a target
 * more than TARGET_AHEAD_TURN off the heading turns nothing. TARGET_SLOW_STICK
 * makes each turn by the stick player_turn_to_stick_at_speed's, in place of
 * player_turn_to_stick's */
#define TARGET_FAST 0x1
#define TARGET_LANDED 0x2
#define TARGET_SLOW_STICK 0x4
#define TARGET_AHEAD 0x8
#define TARGET_AT_ONCE 0x10
#define TARGET_AHEAD_TURN 0x2c00
/* turn_to_target's step: TARGET_STEP, TARGET_FAST_STEP under TARGET_FAST, or
 * HALF_TURN under TARGET_AT_ONCE */
#define TARGET_STEP 0x400
#define TARGET_FAST_STEP 0x1000
/* the most player_turn_to_touch turns the player a step */
#define TOUCH_STEP 0x1000

/* the speed steps brake a player in flat water only where its surface lies
 * more than this above the player's pos */
#define WATER_BRAKE_DEPTH 20.0F

static void player_accel_cling(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);
static void player_accel_level(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);
static void player_roll_downhill(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
static void player_steer_downhill(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record);
static void player_follow_path(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);
static void player_turn_slowly(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, Angle heading);
static void player_limit_speed(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);
static void player_stop_creep(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
static void player_slide_along_wall(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record);
static int player_collide_ground(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
static int player_collide_world(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record);
static void ball_fix_z_normal(struct task_work *work,
                              struct polygon_ball *ball);
static int player_step_up(int slot, struct task_work *work,
                          struct polygon_ball *ball,
                          struct world_object *world);
static int player_hit_wall(struct task_work *work, NJS_VECTOR *spd,
                           struct player *record, struct polygon_ball *ball,
                           NJS_POINT3 *from, NJS_POINT3 *offset,
                           NJS_POINT3 *held_offset, int result, Sint32 flags);
static void ball_drop(struct player_motion *motion, struct player *record,
                      struct polygon_ball *ball, NJS_VECTOR *drop);
static void player_tilt_to_ground(struct task_work *work, struct player *record,
                                  struct polygon_ball *ball, Sint32 flags);
static Bool player_ride_task(struct task_work *work, struct player *record,
                             struct polygon_ball *ball, NJS_POINT3 *offset);
static void player_slip_off(struct task_work *work,
                            struct player_motion *motion,
                            struct polygon_ball *ball);
static void player_set_ground(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, Sint32 flags);
static int ball_stop_at_walls(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct polygon_ball *ball,
                              struct world_object *world);
static Bool ball_runs_into_wall(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record,
                                struct polygon_ball *ball);
static Bool ball_stop_at_ground(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record,
                                struct polygon_ball *ball,
                                struct world_object *world);
static int ball_slide_along_walls(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record,
                                  struct polygon_ball *ball, Sint32 flags);
static void player_deflect(struct task_work *work, struct player *record,
                           struct polygon_ball *ball, NJS_VECTOR *way);
static void unmasked_deflect(struct task_work *work, struct player *record,
                             struct polygon_ball *ball, NJS_VECTOR *way);
static int player_collide_unmasked(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record);
static void ground_turn_to_world(NJS_VECTOR *normal, NJS_VECTOR *vector,
                                 Angle ang_y);
static void player_anim_play_next(struct player_anim *anim, Uint16 index);
static void player_anim_start(struct player_anim *anim);
static void player_anim_blend_from(struct player_anim *anim);
static void player_anim_make_blend(struct player_anim *anim);
static void player_anim_free_blend(struct player_anim *anim);
static void player_fall_in_area(struct task *parent, int slot);
#ifdef NONMATCHING
/* this side splashes inside player_sink_in_water, at pos, once its tasks
 * exist */
static void player_sink_in_water(struct task *parent, int slot,
                                 NJS_POINT3 *pos);
#else
static void player_sink_in_water(struct task *parent, int slot);
#endif
static void player_sink(struct task *parent, int slot, Float surface);
static void player_keep_sunk(struct task *parent, int slot, Float surface);
static void breath_step(struct task *task);
static void player_targets_free(struct player *record);

/* each player's pos and ang at each of its last PAST_STEPS steps, from
 * syCalloc. slot 0's entries come first, then slot 1's, and on. past_latest
 * holds the index of each slot's latest entry, which wraps */
/* @hack type: flat arrays indexed by slot * PAST_STEPS, where pointers to
 * rows of PAST_STEPS would do */
static NJS_POINT3 *past_pos = NULL;
static struct angles *past_ang = NULL;
/* TRUE once player_approach_touched has stepped a player toward its offer
 * this step. player_finish_step sets it to FALSE */
static Bool approach_stepped = FALSE;
/* the motions the characters share, from PLCOMMTN.PRS */
static struct motion_entry *common_motions = NULL;

struct model_entry model_table[MODEL_SLOTS];
struct motion_entry motion_table[MOTION_SLOTS];
struct task *player_tasks[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
struct player_motion *player_motion[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
static Uint8 past_latest[PLAYER_SLOTS];

void players_init(void)
{
    int i;

    for (i = 0; i < PLAYER_SLOTS; i++) {
        player_tasks[i] = NULL;
        player_work[i] = NULL;
        player_motion[i] = NULL;
        player_records[i] = NULL;
    }
    for (i = 0; i < MODEL_SLOTS; i++) {
        model_table[i].index = i;
        model_table[i].object = NULL;
    }
    for (i = 0; i < MOTION_SLOTS; i++) {
        motion_table[i].index = i;
        motion_table[i].nodes = 0;
        motion_table[i].motion = NULL;
    }
    if (common_motions == NULL) {
        common_motions = motion_file_read("PLCOMMTN.PRS");
    }
    if (past_pos == NULL) {
        past_pos = syCalloc(1, sizeof(NJS_POINT3[PLAYER_SLOTS][PAST_STEPS]));
    }
    if (past_ang == NULL) {
        past_ang = syCalloc(1, sizeof(struct angles[PLAYER_SLOTS][PAST_STEPS]));
    }
}

void players_exit(void)
{
    if (common_motions != NULL) {
        motion_file_free(common_motions);
        common_motions = NULL;
    }
    if (past_pos != NULL) {
        syFree(past_pos);
        past_pos = NULL;
    }
    if (past_ang != NULL) {
        syFree(past_ang);
        past_ang = NULL;
    }
}

struct model_entry *model_file_read(const char *name)
{
    struct model_entry *list;
    NJS_CNK_OBJECT *object;
    int i;

    if ((list = loader_read_prs_alloc(name)) == NULL) {
        return NULL;
    }
    /* @hack spelling: the test for the list's end at the top of the loop's
     * body, where the for's own test would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MODEL_SLOTS &&
            model_table[list[i].index].object == NULL) {
            object = list[i].object;
            if ((Uint32)object < FILE_OFFSET_LIMIT && object != NULL) {
                object = (NJS_CNK_OBJECT *)((Uint32)object + (Uint32)list);
                list[i].object = object;
            }
            model_table[list[i].index].object = object;
        }
        if (list[i].object != NULL) {
            object = list[i].object;
            if ((Uint32)object < FILE_OFFSET_LIMIT && object != NULL) {
                object = (NJS_CNK_OBJECT *)((Uint32)object + (Uint32)list);
                list[i].object = object;
            }
            relocate_cnk_object(object, (Uint32)list);
        }
    }
    return list;
}

void model_file_free(struct model_entry *list)
{
    int i;

    /* @hack spelling: the test for the list's end at the top of the loop's
     * body, where the for's own test would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MODEL_SLOTS &&
            model_table[list[i].index].object == list[i].object) {
            model_table[list[i].index].object = NULL;
        }
    }
    loader_free(list);
}

struct motion_entry *motion_file_read(const char *name)
{
    struct motion_entry *list;
    NJS_MOTION *motion;
    int i;

    if ((list = loader_read_prs_alloc(name)) == NULL) {
        return NULL;
    }
    /* @hack spelling: the test for the list's end at the top of the loop's
     * body, where the for's own test would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MOTION_SLOTS &&
            motion_table[list[i].index].motion == NULL) {
            motion = list[i].motion;
            if ((Uint32)motion < FILE_OFFSET_LIMIT && motion != NULL) {
                motion = (NJS_MOTION *)((Uint32)motion + (Uint32)list);
                list[i].motion = motion;
            }
            motion_table[list[i].index].nodes = list[i].nodes;
            motion_table[list[i].index].motion = motion;
        }
        if (list[i].motion != NULL) {
            motion = list[i].motion;
            if ((Uint32)motion < FILE_OFFSET_LIMIT && motion != NULL) {
                motion = (NJS_MOTION *)((Uint32)motion + (Uint32)list);
                list[i].motion = motion;
            }
            relocate_motion(motion, (Uint32)list, list[i].nodes);
        }
    }
    return list;
}

void motion_file_free(struct motion_entry *list)
{
    int i;

    /* @hack spelling: the test for the list's end at the top of the loop's
     * body, where the for's own test would do */
    for (i = 0;; i++) {
        if (list[i].index == -1) {
            break;
        }
        if (list[i].index >= 0 && list[i].index < MOTION_SLOTS &&
            motion_table[list[i].index].motion == list[i].motion) {
            motion_table[list[i].index].motion = NULL;
            motion_table[list[i].index].nodes = 0;
        }
    }
    loader_free(list);
}

int player_free_move_forced(int slot)
{
    return 0;
}

void *fun_8c038e08(void)
{
    return syCalloc(1, 0x308);
}

static void player_set_gravity(struct player *record)
{
    Float gravity;

    switch (stage_current_id()) {
    case STAGE_FINAL_RUSH:
    case STAGE_FINAL_CHASE:
    case STAGE_METEOR_HERD:
    case STAGE_MAD_SPACE:
    case STAGE_COSMIC_WALL:
        /* @hack spelling: SPACE_GRAVITY in a local before the characters'
         * test, where the store could take the constant */
        gravity = SPACE_GRAVITY;
        switch (record->character) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
        case PLAYER_EGGMAN_MECH:
        case PLAYER_TAILS_MECH:
            record->consts.gravity = gravity;
            break;
        default:
            record->consts.gravity =
                character_constants[record->character].gravity;
            break;
        }
        break;
    default:
        record->consts.gravity = character_constants[record->character].gravity;
        break;
    }
}

void player_register(int slot, struct task *task)
{
    struct task_work *work = task->work;
    struct player_motion *motion = task->motion.p;
    struct player *record = task->extra.p;

    record->ball = syCalloc(1, sizeof(struct polygon_ball));
    player_tasks[slot] = task;
    player_work[slot] = work;
    player_motion[slot] = motion;
    player_records[slot] = record;
    record->anim.mode = PLAYER_ANIM_NEW;
    record->anim.speed = &record->spd;
    record->anim.pace = &record->anim_speed;
    record->kind = record->character;
    record->consts = character_constants[record->character];
    player_set_gravity(record);
    switch (stage_current_id()) {
    case STAGE_FINAL_RUSH:
    case STAGE_FINAL_CHASE:
    case STAGE_METEOR_HERD:
    case STAGE_MAD_SPACE:
    case STAGE_COSMIC_WALL:
        /* @hack spelling: three arms that store the same, where one arm for
         * the six characters would do */
        switch (record->character) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
            record->consts.resist.x = SPACE_RESIST_X;
            record->consts.air_resist = SPACE_AIR_RESIST;
            break;
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            record->consts.resist.x = SPACE_RESIST_X;
            record->consts.air_resist = SPACE_AIR_RESIST;
            break;
        case PLAYER_EGGMAN_MECH:
        case PLAYER_TAILS_MECH:
            record->consts.resist.x = SPACE_RESIST_X;
            record->consts.air_resist = SPACE_AIR_RESIST;
            break;
        }
        break;
    }
    record->ground.unk_20 = record->consts.radius * 0.2F;
    player_stop(NULL, record);
    record->grip_scale = 1.0F;
    work->id = WORK_ID_PLAYER;
    player_targets_alloc(record);
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    switch (record->character) {
    case PLAYER_SONIC:
        if (game_flags[UPGRADE_SONIC_SHOES] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_SHOES);
        }
        if (game_flags[UPGRADE_SONIC_SPARKLE] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_SPARKLE);
        }
        if (game_flags[UPGRADE_SONIC_MAGICWRIST] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_MAGICWRIST);
        }
        if (game_flags[UPGRADE_SONIC_FLAMERING] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_FLAMERING);
        }
        if (game_flags[UPGRADE_SONIC_RUBBERUNIT] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT);
        }
        if (game_flags[UPGRADE_SONIC_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SONIC_MELODY);
        }
        break;
    case PLAYER_SHADOW:
        if (game_flags[UPGRADE_SHADOW_SHOES] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SHADOW_SHOES);
        }
        if (game_flags[UPGRADE_SHADOW_SPARKLE] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SHADOW_SPARKLE);
        }
        if (game_flags[UPGRADE_SHADOW_FLAMERING] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING);
        }
        if (game_flags[UPGRADE_SHADOW_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_SHADOW_MELODY);
        }
        break;
    case PLAYER_TAILS_MECH:
        if (game_flags[UPGRADE_TWALKER_BOOSTER] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER);
        }
        if (game_flags[UPGRADE_TWALKER_HYPERCANNON] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON);
        }
        if (game_flags[UPGRADE_TWALKER_LAZERBALSTER] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER);
        }
        if (game_flags[UPGRADE_TWALKER_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_TWALKER_MELODY);
        }
        break;
    case PLAYER_EGGMAN_MECH:
        if (game_flags[UPGRADE_EWALKER_JETENGINE] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE);
        }
        if (game_flags[UPGRADE_EWALKER_BAZOOKA] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA);
        }
        if (game_flags[UPGRADE_EWALKER_POWERGUN] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN);
        }
        if (game_flags[UPGRADE_EWALKER_EXTRASHIELD] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD);
        }
        if (game_flags[UPGRADE_EWALKER_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_EWALKER_MELODY);
        }
        break;
    case PLAYER_KNUCKLES:
        if (game_flags[UPGRADE_KNUCKLES_CLAWS] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS);
        }
        if (game_flags[UPGRADE_KNUCKLES_SUNGLASS] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_KNUCKLES_SUNGLASS);
        }
        if (game_flags[UPGRADE_KNUCKLES_GROVES] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES);
        }
        if (game_flags[UPGRADE_KNUCKLES_AIR] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_KNUCKLES_AIR);
        }
        if (game_flags[UPGRADE_KNUCKLES_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_KNUCKLES_MELODY);
        }
        break;
    case PLAYER_ROUGE:
        if (game_flags[UPGRADE_ROUGE_NAILS] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_ROUGE_NAILS);
        }
        if (game_flags[UPGRADE_ROUGE_SCOPE] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_ROUGE_SCOPE);
        }
        if (game_flags[UPGRADE_ROUGE_BOOTS] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_ROUGE_BOOTS);
        }
        if (game_flags[UPGRADE_ROUGE_MELODY] != 0) {
            record->upgrades |= UPGRADE_BIT(UPGRADE_ROUGE_MELODY);
        }
        break;
    }
}

void player_unregister(int slot)
{
    /* @hack redundant: the reads of the slot's work and motion, which
     * nothing uses */
    struct task_work *work =
        player_work[slot]; /* NOLINT(clang-diagnostic-unused-variable) */
    struct player_motion *motion =
        player_motion[slot]; /* NOLINT(clang-diagnostic-unused-variable) */
    struct player *record = player_records[slot];

    player_tasks[slot] = NULL;
    player_work[slot] = NULL;
    player_motion[slot] = NULL;
    player_records[slot] = NULL;
    if (record->ball != NULL) {
        syFree(record->ball);
        record->ball = NULL;
    }
    record->anim.mode = PLAYER_ANIM_END;
    if (record->anim.blend != NULL) {
        player_anim_step(&record->anim);
    }
    player_targets_free(record);
}

/* record the work's pos and ang as the latest of the player's past */
static void player_note_past(Uint8 slot)
{
    struct task_work *work = player_work[slot];
    Uint8 latest = ++past_latest[slot];

    past_pos[(slot * PAST_STEPS) + latest] = work->pos;
    past_ang[(slot * PAST_STEPS) + latest] = work->ang;
}

void player_finish_step(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    NJS_POINT3 eyes;
    struct task *breath;

    work->flags &=
        ~(PLAYER_WORK_REQUEST | PLAYER_WORK_UNK_8 | WORK_UNDER | WORK_ON_TOP);
    work->submode = 0;
    motion->acc.x = motion->acc.y = motion->acc.z = 0.0F;
    record->slip = record->push.x = record->push.y = record->push.z =
        record->accel.x = record->accel.y = record->accel.z = 0.0F;
    if (record->stick_lock > 0) {
        record->stick_lock -= 1;
    } else {
        record->stick_lock = 0;
    }
    if (record->confused_steps > 0) {
        record->confused_steps -= 1;
    } else {
        record->confused_steps = 0;
    }
    player_note_past(record->slot);
    {
        /* @hack spelling: record copied to same, through which the count
         * is cleared, where record would do */
        struct player *same = record;

        same->target_count = 0;
    }
    if ((record->character == PLAYER_KNUCKLES &&
         (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_AIR))) ||
        game_state.cleared == 1 || (record->flags & PLAYER_MISSED)) {
        record->underwater_steps = 0;
    } else if (record->ground.last_above_flags & WORLD_FLAT) {
        eyes.z = 0.0F;
        eyes.x = 0.0F;
        eyes.y = record->consts.target_height;
        turn_to_world(work, &eyes);
        njAddVector(&eyes, &work->pos);
        if (record->ground.above_y >= eyes.y) {
            record->underwater_steps += 1;
            if (record->underwater_steps == BREATH_STEPS) {
                breath =
                    task_create(TASK_EXTRA, 0, breath_step, "BreathCounterP");
                if (breath != NULL) {
                    BREATH(breath)->slot = record->slot;
                }
            }
        } else {
            record->underwater_steps = 0;
        }
    } else {
        record->underwater_steps = 0;
    }
    approach_stepped = FALSE;
    record->faced = NULL;
    record->unk_0ac = 0;
    if (record->action == PLAYER_MODE_ASK_HINT) {
        record->action = 0;
    }
}

/* the action flags that the task of other gives the player of work and record,
 * by other's id. only PLAYER_SONIC to PLAYER_ROUGE and PLAYER_CHARACTER_8 take
 * any */
static int task_action_flags(struct task_work *other, struct task_work *work,
                             struct player *record)
{
    int flags = 0;
    Uint8 id = other->id;
    struct task *task;

    switch (record->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_TAILS:
    case PLAYER_EGGMAN:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
    case PLAYER_CHARACTER_8:
        /* @hack spelling: an if for each id in the image's order, where a
         * switch would do */
        if (id == FALL_KIND_NORMAL) {
            flags = ACTION_FLAG_200;
        } else if (id == FALL_KIND_11) {
            task = player_tasks[record->slot];
            if (collision_first_with_id(task, SHAPE_ID_ACTION_400) != NULL ||
                collision_first_with_id(task, SHAPE_ID_NO_ACTION_400) == NULL ||
                (work->flags & PLAYER_WORK_HOLDING)) {
                flags = ACTION_FLAG_400;
            }
        } else if (id == FALL_KIND_HEAVY) {
            flags = ACTION_FLAG_200;
        } else if (id == FALL_KIND_SHAKE) {
            flags = ACTION_FLAG_800;
        } else if (id == FALL_KIND_BURIED) {
            flags = ACTION_FLAG_1000;
        }
        break;
    }
    return flags;
}

/* the nearest task that task's record touched within range of the heading
 * ang, or null. ang is a heading as atan2f(x, z) gives one, which is
 * QUARTER_TURN less a work's ang.y. a task whose first shape has
 * SHAPE_ID_NO_OFFER is passed over */
static struct task *nearest_touched_within(struct task *task, Angle ang,
                                           Angle range)
{
    struct task_work *work;
    struct collision_work *collision;
    NJS_POINT3 *pos;
    NJS_POINT3 *hit_pos;
    struct collision_hit *hit;
    struct collision_hit *end;
    struct task *nearest;
    Float least;
    Float distance;
    Float dx;
    Float dz;
    Angle heading;

    /* @hack spelling: work set inside collision's statement, where a
     * statement of its own would do */
    collision = (work = task->work)->collision;
    least = TOUCHED_FAR;
    end = collision->hits + COLLISION_HITS;
    pos = &work->pos;
    nearest = NULL;
    for (hit = collision->hits; hit < end; hit++) {
        if (hit->task == NULL) {
            break;
        }
        /* @hack type: the shape's id read as a Uint8, where the Sint8 it is
         * would do */
        if ((Uint8)hit->task->work->collision->shapes->id ==
            SHAPE_ID_NO_OFFER) {
            continue;
        }
        hit_pos = &hit->task->work->pos;
        dx = hit_pos->x - pos->x;
        dz = hit_pos->z - pos->z;
        heading = (Angle)(atan2f(dx, dz) * ANGLE_PER_RADIAN);
        if (range >= angle_distance(ang, heading)) {
            distance = njDistanceP2P(pos, hit_pos);
            if (least > distance) {
                nearest = hit->task;
                least = distance;
            }
        }
    }
    return nearest;
}

/* the action flags of the player's offer, its record's hold_offer. without
 * an offer, the nearest task it touched within OFFER_RANGE of its heading
 * becomes the offer. that task's work must have FALL_OFFERABLE, lack
 * WORK_HELD_BY_PLAYER, and give flags */
static int player_offer_flags(struct task_work *work, struct player *record)
{
    int flags = 0;
    struct task *task;
    struct task_work *other;
    Sint16 other_flags;

    if (record->hold_offer != NULL) {
        flags |= task_action_flags(record->hold_offer->work, work, record);
    } else {
        task = nearest_touched_within(player_tasks[record->slot],
                                      QUARTER_TURN - work->ang.y, OFFER_RANGE);
        /* @hack spelling: the two failures leave by goto for one return,
         * where a return of 0 at each would do */
        if (task == NULL) {
            goto none;
        }
        other = task->work;
        if (other->flags & FALL_OFFERABLE) {
            /* @hack spelling: the flags copied to other_flags, where the
             * work's read in place would do */
            other_flags = other->flags;
            if (other_flags & WORK_HELD_BY_PLAYER) {
                goto none;
            }
            flags |= task_action_flags(other, work, record);
            if (flags) {
                record->hold_offer = task;
            }
        }
    }
    return flags;
none:
    return 0;
}

int player_action_flags(struct task_work *work, struct player *record)
{
    int flags = 0;
    struct task_work *held;
    void *held_motion;
    int collision_flags;

    if (work->flags & PLAYER_WORK_HOLDING) {
        held = record->held->work;
        held_motion = record->held->motion.p;
        if (record->hold_offer == NULL) {
            record->hold_offer = record->held;
        }
        if (record->held->step == task_destroy || held_motion == NULL ||
            held == NULL) {
            return 0;
        }
        /* @hack spelling: the record's test in an if of its own, where the
         * tests above could take it */
        if (held->collision == NULL) {
            return 0;
        }
        flags |= task_action_flags(held, work, record);
    } else {
        collision_flags = work->collision->flags;
        if ((collision_flags & COLLISION_TOUCHED) &&
            !(work->flags & WORK_HURT) && (collision_flags & COLLISION_SIDE)) {
            flags |= player_offer_flags(work, record);
        }
    }
    return flags;
}

int player_normal_kind_flag(struct task_work *work, struct player *record)
{
    int flags = 0;
    struct task_work *other;
    void *other_motion;
    Uint8 offer_id;
    Uint8 held_id;

    if (work->flags & PLAYER_WORK_HOLDING) {
        other = record->held->work;
        other_motion = record->held->motion.p;
        if (other_motion == NULL || other == NULL || other->collision == NULL) {
            return 0;
        }
        /* @hack spelling: held_id and offer_id, a local for each arm, where
         * one would do */
        held_id = other->id;
        if (held_id == FALL_KIND_NORMAL) {
            flags = ACTION_FLAG_200;
        } else if (held_id == FALL_KIND_HEAVY) {
            /* @hack redundant: a store of the 0 flags holds */
            flags = 0;
        }
    } else {
        if ((work->collision->flags & COLLISION_TOUCHED) &&
            !(work->flags & WORK_HURT) &&
            (work->collision->flags & COLLISION_SIDE)) {
            /* @hack spelling: the offer's test in an if of its own, where
             * the tests above could take it */
            if (record->hold_offer != NULL) {
                other = record->hold_offer->work;
                offer_id = other->id;
                if (offer_id == FALL_KIND_NORMAL) {
                    flags = ACTION_FLAG_200;
                } else if (offer_id == FALL_KIND_HEAVY) {
                    /* @hack redundant: a store of the 0 flags holds */
                    flags = 0;
                }
            }
        }
    }
    return flags;
}

Bool player_touches_shakable(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    if (record->hold_offer == NULL && player_offer_flags(work, record) == 0) {
        return FALSE;
    }
    if (record->hold_offer->work->id != FALL_KIND_SHAKE) {
        return FALSE;
    }
    return TRUE;
}

Bool player_dir_from_offer(struct task_work *work, struct player *record,
                           NJS_VECTOR *dir)
{
    if (record->hold_offer == NULL && player_offer_flags(work, record) == 0) {
        return FALSE;
    }
    *dir = work->pos;
    njSubVector(dir, &record->hold_offer->work->pos);
    if (njScalor(dir) <= 0.0F) {
        return FALSE;
    }
    njUnitVector(dir);
    return TRUE;
}

void player_shake_touched(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    NJS_VECTOR push;
    struct player_anim *anim;
    Float half;
    Float frames;
    Float strength;

    if (!player_touches_shakable(work, motion, record)) {
        return;
    }
    if (!player_dir_from_offer(work, record, &push)) {
        return;
    }
    push.y = 0.0F;
    if (njScalor(&push) != 0.0F) {
        njUnitVector(&push);
    }
    anim = &record->anim;
    frames = (Float)motion_table[anim->actions[anim->current].motion]
                 .motion->nbFrame;
    half = frames * 0.5F;
    if (anim->frame <= half) {
        strength = anim->frame / half;
    } else {
        frames -= anim->frame;
        strength = frames / half;
    }
    push.x = -(push.x * strength);
    push.y = -(push.y * strength);
    push.z = -(push.z * strength);
    TASK_MOTION_OF(record->hold_offer)->acc = push;
}

Bool player_approach_touched(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    NJS_VECTOR away;
    NJS_VECTOR offset;
    struct collision_work *collision;
    struct task_work *other;
    NJS_POINT3 *center;
    Float reach;

    if (player_dir_from_offer(work, record, &away)) {
        motion->aim_y = angle_approach(
            motion->aim_y, (Angle)(atan2f(-away.z, -away.x) * ANGLE_PER_RADIAN),
            0x800);
        work->ang.y = angle_approach(work->ang.y, motion->aim_y, 0xc00);
        collision = record->hold_offer->work->collision;
        if (collision == NULL) {
            return FALSE;
        }
        other = collision->owner->work;
        offset = other->pos;
        center = &work->collision->shapes->center;
        offset.x -= center->x;
        offset.y = 0.0F;
        offset.z -= center->z;
        switch (other->id) {
        case FALL_KIND_NORMAL:
            reach = 2.22F;
            break;
        case FALL_KIND_11:
            reach = 2.55F;
            break;
        case FALL_KIND_HEAVY:
            reach = 12.5F;
            break;
        case FALL_KIND_SHAKE:
            reach = 2.25F;
            break;
        case FALL_KIND_BURIED:
            reach = 2.22F;
            break;
        default:
            return FALSE;
        }
        reach += work->collision->shapes->radius;
        if (!(reach > njScalor(&offset))) {
            work->pos.x -= away.x;
            work->pos.y -= away.y;
            work->pos.z -= away.z;
            approach_stepped = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

/* the flags a player's collision record needs for player_last_side_touch and
 * player_last_side_pusher to scan its touches. its work must also lack
 * WORK_HURT */
#define SIDE_PUSHED (COLLISION_SIDE | COLLISION_PUSHED)

struct task *player_last_side_touch(struct task_work *work,
                                    struct player *record)
{
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    struct collision_shape *shape;
    struct task *found = NULL;

    if ((work->collision->flags & SIDE_PUSHED) != SIDE_PUSHED ||
        (work->flags & WORK_HURT)) {
        return NULL;
    }
    /* @hack spelling: collision set inside hit's statement, where a
     * statement of its own would do */
    hit = (collision = work->collision)->hits;
    for (end = collision->hits + COLLISION_HITS; hit < end; hit++) {
        /* @hack spelling: the end's test returns found from an else, where a
         * break would do */
        if (hit->hit_shape != HIT_UNUSED) {
            shape = &hit->task->work->collision->shapes[hit->hit_shape];
            if (shape->flags & SHAPE_UNK_2000) {
                continue;
            }
            if (!(hit->task->work->collision->flags & COLLISION_SIDE)) {
                continue;
            }
            found = hit->task;
        } else {
            return found;
        }
    }
    return found;
}

/* player_last_side_touch, but taking a touched shape that has PUSH_PLAYERS
 * where that takes one without SHAPE_UNK_2000 */
static struct task *player_last_side_pusher(struct task_work *work,
                                            struct player *record)
{
    struct collision_work *collision;
    struct collision_hit *hit;
    struct collision_hit *end;
    struct collision_shape *shape;
    struct task *found = NULL;

    if ((work->collision->flags & SIDE_PUSHED) != SIDE_PUSHED ||
        (work->flags & WORK_HURT)) {
        return NULL;
    }
    /* @hack spelling: collision set inside hit's statement, where a
     * statement of its own would do */
    hit = (collision = work->collision)->hits;
    for (end = collision->hits + COLLISION_HITS; hit < end; hit++) {
        /* @hack spelling: the end's test returns found from an else, where a
         * break would do */
        if (hit->hit_shape != HIT_UNUSED) {
            shape = &hit->task->work->collision->shapes[hit->hit_shape];
            if (!(shape->push & PUSH_PLAYERS)) {
                continue;
            }
            if (!(hit->task->work->collision->flags & COLLISION_SIDE)) {
                continue;
            }
            found = hit->task;
        } else {
            return found;
        }
    }
    return found;
}

Bool player_upright(struct task_work *work)
{
    NJS_VECTOR up;
    NJS_VECTOR turned;

    up.x = up.z = 0.0F;
    up.y = 1.0F;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_yxz(work->ang.x, -work->ang.y, work->ang.z, &up.x, &turned.x);
#else
    matrix_push_unit();
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -work->ang.y);
    njCalcVector(NULL, &up, &turned);
    njPopMatrixEx();
#endif
    njCalcVector(NULL, &turned, &up);
    if (stage_current_id() == STAGE_METEOR_HERD || up.y <= 0.0F) {
        return FALSE;
    }
    return TRUE;
}

Bool player_read_stick(struct task_work *work, struct player *record,
                       Angle *angle, Float *strength)
{
    NJS_VECTOR up;
    NJS_VECTOR side;
    NJS_VECTOR top;
    NJS_VECTOR ahead;
    struct task *line;
    Angle turn;
    struct pad_stick *sticks;
    Angle back;

    /* @hack spelling: the tests nest, where returns at the top would do */
    if (!(work->flags & PLAYER_WORK_NO_CONTROL)) {
        /* @hack spelling: sticks, a local for pad_sticks, where the table
         * read in place would do */
        sticks = pad_sticks;
        if (sticks[record->slot].stroke == 0.0F) {
            return FALSE;
        }
        if ((work->flags & PLAYER_WORK_ON_PATH) || record->stick_lock == 0) {
            if (angle != NULL) {
                *angle = sticks[record->slot].ang;
                if (record->confused_steps > 0) {
                    switch (game_state.steps & CONFUSED_WAYS) {
                    case CONFUSED_TURNED:
                        *angle = QUARTER_TURN + *angle;
                        break;
                    case CONFUSED_MIRRORED:
                        *angle = HALF_TURN - *angle;
                        break;
                    case CONFUSED_REVERSED:
                        *angle = *angle + HALF_TURN;
                        break;
                    case CONFUSED_FLIPPED:
                    default:
                        *angle = QUARTER_TURN - *angle;
                        break;
                    }
                }
                if (stage_current_id() != STAGE_MAD_SPACE) {
                    if ((line =
                             collision_first_with_id(player_tasks[record->slot],
                                                     SHAPE_ID_LINE)) != NULL) {
                        up.x = 0.0F;
                        up.y = 1.0F;
                        up.z = 0.0F;
                        njPushMatrixEx();
                        njUnitMatrix(NULL);
                        njRotateY(NULL, line->work->ang.y);
                        njRotateZ(NULL, line->work->ang.z);
                        njCalcVector(NULL, &up, &up);
                        if (up.y < 0.0F) {
                            up.x = -up.x;
                            up.y = -up.y;
                            up.z = -up.z;
                        }
                        side.x = 1.0F;
                        side.y = 0.0F;
                        side.z = 0.0F;
                        njUnitMatrix(NULL);
                        njRotateZ(NULL, work->ang.z);
                        njRotateX(NULL, work->ang.x);
                        njCalcVector(NULL, &side, &side);
                        top.x = 0.0F;
                        top.y = 1.0F;
                        top.z = 0.0F;
                        njUnitMatrix(NULL);
                        njRotateZ(NULL, work->ang.z);
                        njRotateX(NULL, work->ang.x);
                        njCalcVector(NULL, &top, &top);
                        njPopMatrixEx();
                        turn = vector_angle(&side, &up, &top);
                        /* @hack spelling: back, set ahead of the tests, where
                         * each arm could add the half turn itself */
                        back = turn + HALF_TURN;
                        if (up.y > LINE_UPRIGHT_Y) {
                            /* @bug logic: the test reads the pad of slot 0's
                             * player whatever the slot, where every other read
                             * of a stick here takes the player's own
                             * (0x8c039eae) */
                            /* @hack type: y1 converted through int, where the
                             * Sint16 converts the same */
                            if ((int)player_pad_records[0].y1 < 0.0F) {
                                *angle = turn;
                            } else {
                                *angle = back;
                            }
                        } else {
                            ahead.x = 1.0F;
                            ahead.y = 0.0F;
                            ahead.z = 0.0F;
#ifdef NONMATCHING
                            /* for a finite vector, the kernel gives the bits of
                             * the matrix calls below, as engine/dc/turn.h
                             * says */
                            zam_vec3_turn_y(*angle, &ahead.x, &ahead.x);
#else
                            njPushMatrixEx();
                            njUnitMatrix(NULL);
                            njRotateY(NULL, *angle);
                            njCalcVector(NULL, &ahead, &ahead);
                            njPopMatrixEx();
#endif
                            if (njInnerProduct(&ahead, &up) >= 0.0F) {
                                *angle = turn;
                            } else {
                                *angle = back;
                            }
                        }
                    } else if (world_gravity.y > GRAVITY_UP_Y) {
                        *angle = HALF_TURN - *angle;
                    }
                }
            }
            if (strength != NULL) {
                *strength = pad_sticks[record->slot].stroke;
            }
            return TRUE;
        }
    }
    return FALSE;
}

Bool player_stick_back(struct task_work *work, struct player *record)
{
    Angle angle;
    Angle turn;

    if (!(work->flags & PLAYER_WORK_NO_CONTROL)) {
        if (!(work->flags & PLAYER_WORK_ON_PATH)) {
            if (player_read_stick(work, record, &angle, NULL) &&
                collision_first_with_id(player_tasks[record->slot],
                                        SHAPE_ID_LINE) == NULL) {
                turn = angle_distance(work->ang.y, angle);
                if (turn > TURN_AWAY) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

Bool player_jump_pressed(struct task_work *work, struct player *record)
{
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return FALSE;
    }
    if (pad_jump_pressed[record->slot]) {
        return TRUE;
    }
    return FALSE;
}

Bool player_count_jump_hold(struct task_work *work, struct player *record)
{
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return FALSE;
    }
    if (pad_jump_held[record->slot] &&
        record->jump_steps++ < record->consts.jump_hold_steps) {
        return TRUE;
    }
    return FALSE;
}

int player_face_offer(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    NJS_VECTOR away;
    Angle ang = work->ang.y;
    Angle turn;

    if (!player_dir_from_offer(work, record, &away)) {
        collision_get_normal(work, &away);
    }
    turn = angle_distance(ang,
                          (Angle)(atan2f(-away.z, -away.x) * ANGLE_PER_RADIAN));
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_HEAD_ON_WIDE) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

int player_stick_into_wall(struct task_work *work, struct player *record)
{
    /* @hack redundant: a vector nothing reads */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    Angle angle;
    Angle turn;

    if (njScalor(&record->wall_normal) == 0.0F ||
        !player_read_stick(work, record, &angle, NULL)) {
        return PLAYER_WALL_NONE;
    }
    turn = angle_distance(
        angle, (Angle)(atan2f(-record->wall_normal.z, -record->wall_normal.x) *
                       ANGLE_PER_RADIAN));
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_HEAD_ON_WIDE) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

int player_face_wall(struct task_work *work, struct player *record)
{
    /* @hack redundant: a vector nothing reads */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    Angle ang;
    Angle turn;

    if (njScalor(&record->wall_normal) == 0.0F) {
        return PLAYER_WALL_NONE;
    }
    ang = work->ang.y;
    turn = angle_distance(
        ang, (Angle)(atan2f(-record->wall_normal.z, -record->wall_normal.x) *
                     ANGLE_PER_RADIAN));
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_HEAD_ON_WIDE) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

int player_stick_into_offer(struct task_work *work, struct player *record)
{
    Angle angle;
    NJS_VECTOR away;
    Angle turn;

    if ((work->flags & PLAYER_WORK_NO_CONTROL) ||
        !player_read_stick(work, record, &angle, NULL)) {
        return PLAYER_WALL_NONE;
    }
    if (!player_dir_from_offer(work, record, &away)) {
        collision_get_normal(work, &away);
    }
    turn = angle_distance(angle,
                          (Angle)(atan2f(-away.z, -away.x) * ANGLE_PER_RADIAN));
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_ASKEW) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

int player_move_into_offer(struct task_work *work, struct player_motion *motion,
                           struct player *record)
{
    NJS_VECTOR dir;
    NJS_VECTOR back;
    Angle turn;

    if (!player_dir_from_offer(work, record, &back)) {
        collision_get_normal(work, &back);
    }
    back.x = -motion->spd.x;
    back.y = -motion->spd.y;
    back.z = -motion->spd.z;
    if (njScalor(&back) != 0.0F) {
        njUnitVector(&back);
    }
    /* @bug uninitialized: the motion is measured against dir, which no path
     * sets. the way from the offer or the wall went into back, and the
     * motion's reverse then overwrote it (0x8c03a3c0) */
    /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
    turn = (Angle)(acosf(njInnerProduct(&dir, &back)) * ANGLE_PER_RADIAN);
    /* NOLINTEND(clang-diagnostic-uninitialized) */
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_ASKEW) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

int player_stick_ahead(struct task_work *work, struct player *record)
{
    Angle angle;
    /* @hack redundant: a vector nothing reads */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    Angle turn;

    if ((work->flags & PLAYER_WORK_NO_CONTROL) ||
        !player_read_stick(work, record, &angle, NULL)) {
        return PLAYER_WALL_NONE;
    }
    turn = angle_distance(angle, work->ang.y);
    if (turn > TURN_AWAY) {
        return PLAYER_WALL_AWAY;
    }
    if (turn > TURN_ACROSS) {
        return PLAYER_WALL_ACROSS;
    }
    if (turn > TURN_ASKEW) {
        return PLAYER_WALL_ASKEW;
    }
    return PLAYER_WALL_HEAD_ON;
}

Bool player_has_target(int slot)
{
    struct near_target *near;

    if (stage_current_id() == STAGE_CHAO) {
        return FALSE;
    }
    if (slot == 0) {
        near = near_targets;
    } else {
        near = near_targets_player1;
    }
    if (near->work == NULL) {
        return FALSE;
    }
    return TRUE;
}

/* turn the player toward the nearest of its near targets that is an item or
 * an enemy. the TARGET_ bits of how say how it turns. a target whose work's id
 * is FALL_KIND_NORMAL is passed over. on STAGE_CHAO, a player without a near
 * target turns by its stick */
static void turn_to_target(struct task_work *work, struct player_motion *motion,
                           struct player *record, int how)
{
    struct near_target *near;
    struct task_work *other;
    struct task_work *nearest;
    int kind;
    Float least;
    NJS_VECTOR way;
    struct collision_shape *shape;
    struct collision_shape *own;
    struct enemy_work *enemy;
    Angle heading;
    Angle step;

    if (record->slot == 0) {
        near = near_targets;
    } else {
        near = near_targets_player1;
    }
    if (near->work == NULL && stage_current_id() == STAGE_CHAO) {
        if (how & TARGET_SLOW_STICK) {
            player_turn_to_stick_at_speed(work, motion, record);
        } else {
            player_turn_to_stick(work, motion, record);
        }
    } else {
        nearest = NULL;
        least = 0.0F;
        for (; near->work != NULL; near++) {
            other = near->work;
            kind = other->collision->kind;
            if ((kind == COLLISION_ITEM || kind == COLLISION_ENEMY) &&
                other->id != FALL_KIND_NORMAL) {
                if (nearest == NULL || least > near->distance) {
                    nearest = other;
                    least = near->distance;
                }
            }
        }
        if (nearest == NULL) {
            return;
        }
        enemy = nearest->collision->owner->motion.p;
        if (enemy == NULL ||
            (!(enemy->flags & ENEMY_ON_FLOOR) && (how & TARGET_LANDED))) {
            player_turn_to_stick_at_speed(work, motion, record);
            return;
        }
        shape = nearest->collision->shapes;
        own = work->collision->shapes;
        way.x = nearest->pos.x + shape->center.x - own->center.x;
        way.y = nearest->pos.y + shape->center.y - own->center.y;
        way.z = nearest->pos.z + shape->center.z - own->center.z;
        heading = (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN) & ANGLE_MASK;
        if (world_gravity.y > 0.0F) {
            heading = HALF_TURN - heading;
        }
        if ((how & TARGET_AHEAD) &&
            angle_distance(work->ang.y, heading) > TARGET_AHEAD_TURN) {
            return;
        }
        motion->aim_y = heading;
        if (how & TARGET_AT_ONCE) {
            step = HALF_TURN;
        } else if (how & TARGET_FAST) {
            step = TARGET_FAST_STEP;
        } else {
            step = TARGET_STEP;
        }
        work->ang.y = angle_approach(work->ang.y, heading, step);
        record->faced = nearest->collision->owner;
    }
}

void player_turn_to_target(struct task_work *work, struct player_motion *motion,
                           struct player *record)
{
    turn_to_target(work, motion, record, TARGET_FAST);
}

void player_turn_to_landed_target(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record)
{
    turn_to_target(work, motion, record, TARGET_LANDED | TARGET_SLOW_STICK);
}

void player_turn_to_target_ahead(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    turn_to_target(work, motion, record, TARGET_SLOW_STICK | TARGET_AHEAD);
}

void player_face_target(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    turn_to_target(work, motion, record, TARGET_AT_ONCE);
}

void player_turn_to_touch(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    struct task *task;
    struct collision_hit *hit;
    struct task *other;
    struct task_work *other_work;
    struct collision_shape *shape;
    NJS_VECTOR way;
    Angle heading;
    Angle step;

    task = player_tasks[record->slot];
    /* @hack spelling: the touch's turn in an else, where a return after the
     * stick's turn would do */
    if ((hit = collision_first_with_id_hit(task, SHAPE_ID_FACED)) == NULL) {
        player_turn_to_stick(work, motion, record);
    } else {
        other = hit->task;
        other_work = other->work;
        /* @hack redundant: the player's own shape of the touch, which
         * nothing uses */
        collision_get_shape(task, hit->own_shape);
        shape = collision_get_shape(other, hit->hit_shape);
        collision_get_shape_center(other_work, shape, &way);
        way.x -= work->pos.x;
        way.z -= work->pos.z;
        heading = (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN) & ANGLE_MASK;
        motion->aim_y = heading;
        /* @hack spelling: step, a local for the turn's step, where TOUCH_STEP
         * would do */
        step = TOUCH_STEP;
        work->ang.y = angle_approach(work->ang.y, heading, step);
        record->faced = other;
    }
}

/* the stick steers a player within STEER_TURN of its heading, and turns one
 * past it. a still player pivots in place, but a still mech keeps its
 * heading. one from halfway between trot_speed and running_speed up brakes.
 * one slower, from trot_speed up, keeps HARD_TURN_PUSH of its push for a turn
 * past HARD_TURN */
#define STEER_TURN 0x1000
#define HARD_TURN EIGHTH_TURN
#define HARD_TURN_PUSH 0.8F
/* the share of ground_accel that a stroke pushes a fast player by */
#define FAST_PUSH 0.4F
/* on ground whose level is above TOP_RESIST_LEVEL, a pushed player faster
 * than top_speed meets TOP_RESIST times resist.x of the speed past it */
#define TOP_RESIST_LEVEL 0.96F
#define TOP_RESIST 1.7F
/* a player that runs along its path presses into the ground by this */
#define PATH_PRESS 0.8F
/* below this level, a player without the stick rolls downhill. one slower
 * than trot_speed takes ROLL_GAIN times the slope's pull */
#define ROLL_LEVEL 0.71F
#define ROLL_GAIN 10.0F
/* below this level, on GROUND_GUIDES ground, a pushed player steers
 * downhill */
#define STEER_LEVEL 0.94F
/* faster than this, a player on a wall or a ceiling holds to it. its fall
 * then runs into the ground in place of along the slope */
#define HOLD_SPEED 1.16F
/* without the stick, an acceleration ahead within this of 0 leaves a still or
 * slow player still */
#define STILL_ACC 0.051F
/* on GROUND_CLING ground, the pad turns the player by up to CLING_TURN below
 * trot_speed. the turn halves past each of trot_speed, running_speed and
 * sprint_speed */
#define CLING_TURN 0x800

/* the slope turns read the way down: world_gravity in the player's own axes.
 * its y is -1 on level ground. a slope turns the player where that y lies
 * between SLOPE_LEVEL_Y and 0. player_steer_downhill tests only the first
 * bound, and player_roll_downhill on STAGE_RELAY_SONIC takes
 * SLOPE_LEVEL_Y_RELAY for it. no slope turns a player faster than trot_speed
 * whose motion rises steeply: its unit motion meets world_gravity below
 * RISING_STEEP.
 *
 * a step turns aim_y toward the way down by up to the way down's z times a
 * factor. the factor is SLOPE_TURN, or SLOPE_TURN_BALL under
 * PLAYER_WORK_BALL. player_steer_downhill's is SLOPE_STEER, or under
 * PLAYER_WORK_BALL SLOPE_STEER_BALL times spd.x / trot_speed.
 * player_accel_cling's is SLOPE_TURN_CLING */
#define SLOPE_LEVEL_Y (-0.87F)
#define SLOPE_LEVEL_Y_RELAY (-0.9F)
#define RISING_STEEP (-0.86F)
#define SLOPE_TURN 1536.0F
#define SLOPE_TURN_BALL 3072.0F
#define SLOPE_STEER 2048.0F
#define SLOPE_STEER_BALL 4096.0F
#define SLOPE_TURN_CLING 512.0F

void player_accel_ground(struct task_work *work, struct player_motion *motion,
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
    Bool pushed;
    int off;
    struct player *mech;
    Float x;
    Float y;
    Float z;
    Float grip;
    Float excess;

    /* @hack spelling: the second test in an else after the first's return,
     * where an if of its own would do */
    if (record->ground_flags & GROUND_CLING) {
        player_accel_cling(work, motion, record);
        return;
    } else if (record->ground_flags & WORLD_UPRIGHT) {
        player_accel_level(work, motion, record);
        return;
    }

    acc = motion->acc;
    push.x = push.y = push.z = 0.0F;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    pushed = player_read_stick(work, record, &angle, &strength);
    njOuterProduct(&record->ball->up, &motion->spd, &dir);

    if (record->level < 0.1F && _builtin_fabsf(dir.y) > 0.6F &&
        record->spd.x > HOLD_SPEED) {
        acc.x = acc.y = acc.z = 0.0F;
        acc.y -= record->consts.gravity * 1.0F;
    } else if (record->level < -0.4F && record->spd.x > HOLD_SPEED) {
        acc.y -= record->consts.gravity * 5.0F;
    } else if (record->level < -0.3F && record->spd.x > HOLD_SPEED) {
        acc.y -= record->consts.gravity * 0.8F;
    } else if (record->level < -0.1F && record->spd.x > HOLD_SPEED) {
        acc.y -= record->consts.gravity * 0.4F;
    } else if (record->level < 0.5F &&
               record->spd.x < record->consts.running_speed &&
               record->spd.x > -record->consts.running_speed) {
        acc.x *= 4.225F;
        acc.z *= 4.225F;
    } else if (record->level < 0.7F &&
               record->spd.x < record->consts.running_speed &&
               record->spd.x > -record->consts.running_speed) {
        acc.x *= 1.0F;
        acc.z *= 2.0F;
    } else if (record->level < 0.87F &&
               record->spd.x < record->consts.trot_speed &&
               record->spd.x > -record->consts.running_speed) {
        acc.x *= 1.0F;
        acc.z *= 1.4F;
    }

    if (!pushed && record->spd.x > record->consts.running_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (pushed && record->spd.x > record->consts.top_speed &&
               record->level > TOP_RESIST_LEVEL) {
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

    if (!pushed) {
        if (record->level < ROLL_LEVEL) {
            player_roll_downhill(work, motion, record);
        } else if (record->spd.x > 0.0F) {
            push.x = record->consts.ground_drag;
        } else if (record->spd.x < 0.0F) {
            push.x = -record->consts.ground_drag;
        }
        record->drive = 0.0F;
    } else {
        if (work->flags & PLAYER_WORK_ON_PATH) {
            player_follow_path(work, motion, record);
            angle = motion->aim_y;
            if (work->flags & PLAYER_WORK_ON_PATH) {
                push.y -= PATH_PRESS;
            } else {
                push.y = 0.0F;
            }
        } else if (record->level < STEER_LEVEL &&
                   (record->ground_flags & GROUND_GUIDES)) {
            player_steer_downhill(work, motion, record);
        }

        if (record->spd.x < record->consts.top_speed) {
            if (record->spd.x < record->consts.trot_speed) {
                if (strength > 0.6F) {
                    strength *= record->consts.ground_accel;
                } else if (record->spd.x < record->consts.trot_speed * 0.4F) {
                    if (strength < 0.5F) {
                        strength = 0.5F;
                    }
                    strength *= record->consts.ground_accel;
                } else {
                    strength = 0.0F;
                }
            } else if (record->spd.x < record->consts.running_speed) {
                if (strength > 0.7F) {
                    strength *= record->consts.ground_accel;
                } else if (/* @bug logic: always holds, at 0x8c03ac76, so
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
                if (strength > 0.9F) {
                    strength *= record->consts.ground_accel;
                } else {
                    strength *= record->consts.ground_accel * 0.3F;
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
#ifdef NONMATCHING
            /* for a finite vector, the kernel gives the bits of the matrix
             * calls below, as engine/dc/turn.h says */
            zam_vec3_turn_yxz(work->ang.x, -angle, work->ang.z, &ahead.x,
                              &dir.x);
#else
            matrix_push_unit();
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -angle);
            njCalcVector(NULL, &ahead, &dir);
            njPopMatrixEx();
#endif
            if (njInnerProduct(&dir, &record->wall_normal) < 0.0F) {
#ifdef NONMATCHING
                /* this side keeps what the image's frame leaves a mech's
                 * wall slide: under walker_step's call, wall.px lies in the
                 * word that player_slide_y stands for */
                player_slide_y = 0.0F;
#endif
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

        if (record->character == PLAYER_EGGMAN_MECH ||
            record->character == PLAYER_TAILS_MECH) {
            mech = player_tasks[record->slot]->extra.p;
            off = angle_distance(work->ang.y, angle);
            if (record->spd.x == 0.0F && off > STEER_TURN) {
                angle = motion->aim_y = work->ang.y;
            } else if (record->spd.x >= (record->consts.trot_speed +
                                         record->consts.running_speed) *
                                            0.5F &&
                       off > STEER_TURN) {
                motion->aim_y = angle;
                strength = record->consts.ground_drag;
                player_turn_at_speed(work, motion, record, motion->aim_y);
            } else if (record->spd.x >= record->consts.trot_speed &&
                       off < STEER_TURN) {
                motion->aim_y = angle;
                player_turn_at_speed(work, motion, record, motion->aim_y);
            } else if (record->spd.x >= record->consts.full_speed &&
                       (record->ground_flags & GROUND_GUIDES)) {
                motion->aim_y = angle;
                player_turn_at_speed(work, motion, record, motion->aim_y);
            } else if (record->consts.trot_speed <= record->spd.x &&
                       record->spd.x <= record->consts.sprint_speed &&
                       off > HARD_TURN) {
                motion->aim_y = angle;
                strength *= HARD_TURN_PUSH;
                if (mech->by_character.mech.aiming == 1) {
                    player_turn_at_speed(work, motion, record, motion->aim_y);
                } else {
                    player_turn_toward(work, motion, record, motion->aim_y);
                }
            } else {
                motion->aim_y = angle;
                if (mech->by_character.mech.aiming == 1) {
                    player_turn_at_speed(work, motion, record, motion->aim_y);
                } else {
                    player_turn_toward(work, motion, record, motion->aim_y);
                }
            }
        } else {
            off = angle_distance(work->ang.y, angle);
            if (record->spd.x == 0.0F && off > STEER_TURN) {
                motion->aim_y = angle;
                strength = 0.0F;
                player_turn_on_course(work, motion, record, motion->aim_y);
            } else if (record->spd.x >= (record->consts.trot_speed +
                                         record->consts.running_speed) *
                                            0.5F &&
                       off > STEER_TURN) {
                motion->aim_y = angle;
                strength = record->consts.ground_drag;
                player_turn_toward(work, motion, record, motion->aim_y);
            } else if (record->spd.x >= record->consts.trot_speed &&
                       off < STEER_TURN) {
                motion->aim_y = angle;
                player_turn_at_speed(work, motion, record, motion->aim_y);
            } else if (record->spd.x >= record->consts.full_speed &&
                       (record->ground_flags & GROUND_GUIDES)) {
                motion->aim_y = angle;
                player_turn_at_speed(work, motion, record, motion->aim_y);
            } else if (record->consts.trot_speed <= record->spd.x &&
                       record->spd.x <= record->consts.sprint_speed &&
                       off > HARD_TURN) {
                motion->aim_y = angle;
                strength *= HARD_TURN_PUSH;
                player_turn_toward(work, motion, record, motion->aim_y);
            } else {
                motion->aim_y = angle;
                player_turn_toward(work, motion, record, motion->aim_y);
            }
        }
        record->drive = strength;
        push.x = strength;
    }

    x = acc.x;
    y = acc.y;
    if (record->level < ROLL_LEVEL &&
        record->spd.x < record->consts.trot_speed &&
        record->spd.x > -record->consts.trot_speed && !pushed) {
        /* @hack spelling: acc.x set from x, the copy of it, where acc.x *= 10
         * would do */
        acc.x = x * ROLL_GAIN;
        acc.z *= ROLL_GAIN;
    } else if (record->spd.x == 0.0F) {
        grip = y * record->consts.still_grip * record->grip_scale;
        acc.x += push.x;
        if (!pushed && grip > acc.x && acc.x > -grip) {
            acc.x = 0.0F;
        }
        if (!pushed && acc.x < STILL_ACC && acc.x > -STILL_ACC) {
            acc.x = 0.0F;
        }
    } else if (push.x < 0.0F) {
        if (record->ground_flags & GROUND_GRIP_LEAST) {
            if (y < 0.0F) {
                grip = y * record->consts.moving_grip * record->grip_scale;
            } else {
                grip = 0.0F;
            }
            if (grip > 0.0F && -push.x > grip) {
                x = push.x + x;
                excess = x - grip;
                if (x < 0.0F && -grip > x) {
                    record->slip = excess;
                    x = -grip;
                } else if (x > 0.0F && x > grip) {
                    record->slip = excess;
                    x = grip;
                }
                acc.x = x;
            } else if (!pushed && record->spd.x <= record->consts.trot_speed &&
                       x < STILL_ACC && x > -STILL_ACC) {
                acc.x = 0.0F;
            } else {
                acc.x += push.x;
            }
        } else {
            if (record->spd.x > 0.0F) {
                x += push.x;
                if (x * push.x < 0.0F) {
                    x = 0.0F;
                }
            } else if (!pushed && record->spd.x <= record->consts.trot_speed &&
                       x < STILL_ACC && x > -STILL_ACC) {
                x = 0.0F;
            } else {
                x = y * record->consts.still_grip * record->grip_scale;
                x += push.x;
                if (x >= 0.0F) {
                    x = 0.0F;
                }
            }
            acc.x = x;
        }
    } else {
        grip = y * record->consts.still_grip * record->grip_scale;
        if (record->spd.x < 0.0F && x < STILL_ACC && x > -STILL_ACC) {
            acc.x += push.x;
            if (acc.x * push.x < 0.0F) {
                acc.x = 0.0F;
            }
        } else if (grip > 0.0F && push.x > grip) {
            x = push.x + x;
            if (x > grip) {
                record->slip = x - grip;
                x = grip;
            }
            acc.x = x;
        } else if (!pushed && record->spd.x <= record->consts.trot_speed &&
                   x < STILL_ACC && x > -STILL_ACC) {
            acc.x = 0.0F;
        } else {
            acc.x += push.x;
        }
    }

    y = acc.y;
    if (record->spd.z == 0.0F) {
        grip = y * record->consts.still_grip * record->grip_scale;
        if (grip > acc.z && acc.z > -grip) {
            acc.z = 0.0F;
        }
    } else {
        if (y < 0.0F) {
            grip = y * record->consts.side_grip * record->grip_scale;
        } else {
            grip = 0.0F;
        }
        if (record->ground_flags & GROUND_GRIP_LEAST) {
            z = push.z + acc.z;
            if (z > 0.0F) {
                z -= grip;
                if (z < 0.0F) {
                    z = 0.0F;
                }
            } else if (z < 0.0F) {
                z += grip;
                if (z > 0.0F) {
                    z = 0.0F;
                }
            }
            acc.z = z;
        } else {
            if (acc.z > 0.0F) {
                push.z -= grip;
            } else if (acc.z < 0.0F) {
                push.z += grip;
            }
            z = acc.z + push.z;
            if (acc.z != 0.0F && push.z != 0.0F && z * acc.z < 0.0F) {
                z = 0.0F;
            }
            acc.z = z;
        }
    }
    acc.y += push.y;
    record->accel = acc;
}

/* player_accel_ground on GROUND_CLING ground */
static void player_accel_cling(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR push;
    Angle angle;
    Float strength;
    Bool pushed;
    int off;
    Angle turn;
    Float x;
    Float y;
    Float z;
    Float grip;
    Float excess;

    acc = motion->acc;
    push.x = push.y = push.z = 0.0F;
    acc.x -= record->ground_normal.x * record->consts.gravity;
    acc.y -= record->ground_normal.y * record->consts.gravity;
    acc.z -= record->ground_normal.z * record->consts.gravity;
    turn_from_world(work, &acc);
    pushed = player_read_stick(work, record, &angle, &strength);

    if (!pushed && record->spd.x > record->consts.running_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (pushed && record->spd.x > record->consts.top_speed &&
               record->level > TOP_RESIST_LEVEL) {
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

    if (!pushed) {
        if (record->level < ROLL_LEVEL) {
            player_roll_downhill(work, motion, record);
        } else if (record->spd.x > 0.0F) {
            push.x = record->consts.ground_drag;
        } else if (record->spd.x < 0.0F) {
            push.x = -record->consts.ground_drag;
        }
        record->drive = 0.0F;
    } else {
        if (work->flags & PLAYER_WORK_ON_PATH) {
            player_follow_path(work, motion, record);
            angle = motion->aim_y;
            push.y -= PATH_PRESS;
        }
        if (record->spd.x < record->consts.top_speed) {
            if (record->spd.x < record->consts.trot_speed) {
                if (strength > 0.6F) {
                    strength *= record->consts.ground_accel;
                } else if (record->spd.x < record->consts.trot_speed * 0.4F) {
                    if (strength < 0.5F) {
                        strength = 0.5F;
                    }
                    strength *= record->consts.ground_accel;
                } else {
                    strength = 0.0F;
                }
            } else if (record->spd.x < record->consts.running_speed) {
                if (strength > 0.7F) {
                    strength *= record->consts.ground_accel;
                } else if (/* @bug logic: always holds, at 0x8c03b78c, so
                            * the push never drops to 0. the bound below is
                            * running_speed, and this arm runs only below
                            * running_speed. the bound of
                            * player_accel_ground's brake is the midpoint of
                            * trot_speed and running_speed */
                           record->spd.x < (record->consts.running_speed +
                                            record->consts.running_speed) *
                                               0.5F) {
                    strength *= record->consts.ground_accel;
                } else {
                    strength = 0.0F;
                }
            } else if (record->spd.x < record->consts.sprint_speed) {
                if (strength > 0.9F) {
                    strength *= record->consts.ground_accel;
                } else {
                    strength *= record->consts.ground_accel * 0.3F;
                }
            } else {
                strength *= record->consts.ground_accel;
            }
        } else {
            strength *= record->consts.ground_accel;
        }

        off = angle_distance(work->ang.y, angle);
        if (record->spd.x == 0.0F && off > STEER_TURN) {
            motion->aim_y = angle;
            strength = 0.0F;
            player_turn_on_course(work, motion, record, motion->aim_y);
        } else {
            NJS_VECTOR dir;
            NJS_VECTOR side;
            NJS_VECTOR front;
            Angle stick;

            /* @hack type: the pad's axes converted through int, where each
             * Sint16 converts the same */
            stick =
                (Angle)(atan2f(
                            -(Float)(int)player_pad_records[record->slot].x1,
                            -(Float)(int)player_pad_records[record->slot].y1) *
                        ANGLE_PER_RADIAN);
            {
                NJS_PLANE plane;
                NJS_VECTOR view;

                view = camera_views[record->slot]->dir;
                plane.px = 0.0F;
                plane.py = 0.0F;
                plane.pz = 0.0F;
                plane.vx = record->ground_normal.x;
                plane.vy = record->ground_normal.y;
                plane.vz = record->ground_normal.z;
                njDistanceP2PL(&view, &plane, &dir);
            }
            njUnitVector(&dir);
            matrix_push_unit();
            njRotate(NULL, &record->ground_normal, stick);
            njCalcVector(NULL, &dir, &front);
            njPopMatrixEx();
            side.x = 1.0F;
            side.y = 0.0F;
            side.z = 0.0F;
            turn_to_world(work, &side);
            turn = vector_angle(&front, &side, &record->ground_normal);
            /* @hack redundant: a call of angle_approach that no path reaches */
            if (0) {
                angle_approach(0, 0, 0);
            }
            {
                NJS_VECTOR side_cross;
                NJS_VECTOR front_cross;

                /* @hack redundant: a test of the camera's heading against
                 * the player's, which nothing reads */
                if (angle_distance(
                        0, angle_difference(
                               work->ang.y,
                               0xc000 - camera_views[record->slot]->ang.y)) >
                    QUARTER_TURN) {
                }
                /* the speed limits the turn only where the player's heading
                 * and the pad's way lie on one side of the camera's view */
                njOuterProduct(&side, &dir, &side_cross);
                njOuterProduct(&front, &dir, &front_cross);
                /* @hack spelling: the test leaves the steps by goto, where an
                 * if around them would do */
                if (njInnerProduct(&side_cross, &front_cross) < 0.0F) {
                    goto aim;
                }
                if (record->spd.x > record->consts.full_speed) {
                    acc.z += njSin(turn);
                } else if (record->spd.x > record->consts.sprint_speed) {
                    turn = angle_approach(0, turn, CLING_TURN / 8);
                } else if (record->spd.x > record->consts.running_speed) {
                    turn = angle_approach(0, turn, CLING_TURN / 4);
                } else if (record->spd.x > record->consts.trot_speed) {
                    turn = angle_approach(0, turn, CLING_TURN / 2);
                } else {
                    turn = angle_approach(0, turn, CLING_TURN);
                }
            }
        aim:
            motion->aim_y = (Uint16)(work->ang.y + turn);
            {
                NJS_VECTOR gravity;
                NJS_VECTOR spd;
                Angle aim;

                /* @hack spelling: a loop of one pass that the gravity's test
                 * leaves by break, where an if around the rest would do */
                do {
                    gravity = world_gravity;
                    spd = motion->spd;
                    if (njScalor(&spd) > record->consts.trot_speed) {
                        njUnitVector(&spd);
                        if (njInnerProduct(&world_gravity, &spd) <
                            RISING_STEEP) {
                            break;
                        }
                        turn_from_world(work, &gravity);
                        if (gravity.x < 0.0F) {
                            gravity.x = -gravity.x;
                        }
                        aim =
                            work->ang.y + (Angle)(atan2f(gravity.z, gravity.x) *
                                                  ANGLE_PER_RADIAN);
                        if (gravity.z < 0.0F) {
                            gravity.z = -gravity.z;
                        }
                        motion->aim_y = angle_approach(
                            motion->aim_y, aim,
                            (Angle)(gravity.z * SLOPE_TURN_CLING));
                    }
                } while (0);
            }
            player_turn_on_course(work, motion, record, motion->aim_y);
        }
        record->drive = strength;
        push.x = strength;
    }

    y = acc.y;
    if (record->spd.x == 0.0F) {
        grip = y * record->consts.still_grip * record->grip_scale;
        acc.x += push.x;
        if (!pushed && grip > acc.x && acc.x > -grip) {
            acc.x = 0.0F;
        }
        if (!pushed && acc.x < STILL_ACC && acc.x > -STILL_ACC) {
            acc.x = 0.0F;
        }
    } else {
        x = acc.x;
        if (push.x < 0.0F) {
            if (record->ground_flags & GROUND_GRIP_LEAST) {
                if (y < 0.0F) {
                    grip = y * record->consts.moving_grip * record->grip_scale;
                } else {
                    grip = 0.0F;
                }
                if (grip > 0.0F && -push.x > grip) {
                    x = push.x + x;
                    excess = x - grip;
                    if (x < 0.0F && -grip > x) {
                        record->slip = excess;
                        x = -grip;
                    } else if (x > 0.0F && x > grip) {
                        record->slip = excess;
                        x = grip;
                    }
                    acc.x = x;
                } else if (!pushed &&
                           record->spd.x <= record->consts.trot_speed &&
                           x < STILL_ACC && x > -STILL_ACC) {
                    acc.x = 0.0F;
                } else {
                    acc.x += push.x;
                }
            } else {
                if (record->spd.x > 0.0F) {
                    x += push.x;
                    if (x * push.x < 0.0F) {
                        x = 0.0F;
                    }
                } else if (!pushed &&
                           record->spd.x <= record->consts.trot_speed &&
                           x < STILL_ACC && x > -STILL_ACC) {
                    x = 0.0F;
                } else {
                    x = y * record->consts.still_grip * record->grip_scale;
                    x += push.x;
                    if (x >= 0.0F) {
                        x = 0.0F;
                    }
                }
                acc.x = x;
            }
        } else {
            grip = y * record->consts.still_grip * record->grip_scale;
            if (record->spd.x < 0.0F && x < STILL_ACC && x > -STILL_ACC) {
                acc.x += push.x;
                if (acc.x * push.x < 0.0F) {
                    acc.x = 0.0F;
                }
            } else if (grip > 0.0F && push.x > grip) {
                x = push.x + x;
                if (x > grip) {
                    x -= grip;
                    record->slip = x;
                    x = grip;
                }
                acc.x = x;
            } else if (!pushed && record->spd.x <= record->consts.trot_speed &&
                       x < STILL_ACC && x > -STILL_ACC) {
                acc.x = 0.0F;
            } else {
                acc.x += push.x;
            }
        }
    }

    y = acc.y;
    if (record->spd.z == 0.0F) {
        grip = y * record->consts.still_grip * record->grip_scale;
        if (grip > acc.z && acc.z > -grip) {
            acc.z = 0.0F;
        }
    } else {
        if (y < 0.0F) {
            grip = y * record->consts.side_grip * record->grip_scale;
        } else {
            grip = 0.0F;
        }
        if (record->ground_flags & GROUND_GRIP_LEAST) {
            z = push.z + acc.z;
            if (z > 0.0F) {
                z -= grip;
                if (z < 0.0F) {
                    z = 0.0F;
                }
            } else if (z < 0.0F) {
                z += grip;
                if (z > 0.0F) {
                    z = 0.0F;
                }
            }
            acc.z = z;
        } else {
            if (acc.z > 0.0F) {
                push.z -= grip;
            } else if (acc.z < 0.0F) {
                push.z += grip;
            }
            z = acc.z + push.z;
            if (acc.z != 0.0F && push.z != 0.0F && z * acc.z < 0.0F) {
                z = 0.0F;
            }
            acc.z = z;
        }
    }
    acc.y += push.y;
    record->accel = acc;
}

/* player_accel_ground on WORLD_UPRIGHT ground, whose slope doesn't pull the
 * player. resist.x holds back a speed back whole. without the stick it holds
 * back a speed ahead past running_speed whole. with the stick it holds back
 * only the part past sprint_speed. each step takes away half the speed
 * across */
static void player_accel_level(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR push;
    Angle angle;
    Float strength;
    Bool pushed;
    int off;
    Float x;
    Float y;
    Float sum;
    Float grip;

    acc = motion->acc;
    push.x = push.y = push.z = 0.0F;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    pushed = player_read_stick(work, record, &angle, &strength);

    if (!pushed && record->spd.x > record->consts.running_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (record->spd.x > record->consts.sprint_speed) {
        acc.x += record->consts.resist.x *
                 (record->spd.x - record->consts.sprint_speed);
    } else if (record->spd.x < 0.0F) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;

    if (!pushed) {
        if (record->spd.x > 0.0F) {
            push.x = record->consts.ground_drag;
        } else if (record->spd.x < 0.0F) {
            push.x = -record->consts.ground_drag;
        }
        record->drive = 0.0F;
    } else {
        if (record->consts.running_speed > record->spd.x) {
            strength *= record->consts.ground_accel;
        } else {
            strength *= record->consts.ground_accel * FAST_PUSH;
        }
        off = angle_distance(work->ang.y, angle);
        if (record->spd.x == 0.0F && off > STEER_TURN) {
            motion->aim_y = angle;
            strength = 0.0F;
            player_turn_on_course(work, motion, record, motion->aim_y);
        } else if (record->spd.x >= (record->consts.trot_speed +
                                     record->consts.running_speed) *
                                        0.5F &&
                   off > STEER_TURN) {
            motion->aim_y = angle;
            strength = record->consts.ground_drag;
            player_turn_toward(work, motion, record, motion->aim_y);
        } else if (record->spd.x >= record->consts.trot_speed &&
                   off < STEER_TURN) {
            motion->aim_y = angle;
            player_turn_at_speed(work, motion, record, motion->aim_y);
        } else if (record->consts.trot_speed <= record->spd.x &&
                   record->spd.x <= record->consts.sprint_speed &&
                   off > HARD_TURN) {
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

    y = acc.y;
    if (record->spd.x == 0.0F) {
        grip = y * record->consts.still_grip * record->grip_scale;
        acc.x += push.x;
        if (grip > acc.x && acc.x > -grip) {
            acc.x = 0.0F;
        }
    } else {
        x = acc.x;
        if (push.x < 0.0F) {
            if (record->spd.x > 0.0F) {
                sum = x + push.x;
                if (sum * push.x < 0.0F) {
                    sum = 0.0F;
                }
            } else {
                sum = y * record->consts.still_grip * record->grip_scale;
                sum += push.x + x;
                if (sum >= 0.0F) {
                    sum = 0.0F;
                }
            }
            acc.x = sum;
        } else {
            grip = y * record->consts.still_grip * record->grip_scale;
            if (record->spd.x < 0.0F) {
                acc.x += push.x;
                if (acc.x * push.x < 0.0F) {
                    acc.x = 0.0F;
                }
            } else if (grip > 0.0F && push.x > grip) {
                sum = push.x + x;
                if (sum > grip) {
                    sum -= grip;
                    record->slip = sum;
                    sum = grip;
                }
                acc.x = sum;
            } else {
                acc.x += push.x;
            }
        }
    }

    if (record->spd.z == 0.0F) {
        grip = acc.y * record->consts.still_grip * record->grip_scale;
        if (grip > acc.z && acc.z > -grip) {
            acc.z = 0.0F;
        }
    } else {
        acc.z = -(record->spd.z * 0.5F);
    }
    acc.y += push.y;
    record->accel = acc;
}

void player_accel_air(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    NJS_VECTOR acc;
    Angle angle;
    Float strength;
    Angle off;

    acc = motion->acc;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.air_resist * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (player_read_stick(work, record, &angle, &strength) == FALSE) {
        strength = 0.0F;
        motion->aim_y = work->ang.y;
    } else {
        off = angle_distance(work->ang.y, angle);
        if (record->spd.x > record->consts.running_speed && off > TURN_AWAY) {
            strength = record->consts.air_brake * strength;
        } else if (off > STEER_TURN) {
            motion->aim_y = angle;
            strength = 0.0F;
        } else {
            motion->aim_y = angle;
            if (motion->spd.y < 0.0F) {
                strength = record->consts.air_accel * strength * 2.0F;
            } else {
                strength = record->consts.air_accel * strength;
            }
        }
        player_turn_toward(work, motion, record, motion->aim_y);
    }
    acc.x += strength;
    record->accel = acc;
}

void player_accel_air_weightless(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    NJS_VECTOR acc;
    Angle angle;
    Float strength;
    Angle off;

    acc = motion->acc;
    turn_from_world(work, &acc);
    acc.x += record->consts.air_resist * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (player_read_stick(work, record, &angle, &strength) == FALSE) {
        strength = 0.0F;
        motion->aim_y = work->ang.y;
    } else {
        off = angle_distance(work->ang.y, angle);
        if (record->spd.x > record->consts.running_speed && off > TURN_AWAY) {
            strength = record->consts.air_brake * strength;
        } else if (off > STEER_TURN) {
            motion->aim_y = angle;
            strength = 0.0F;
        } else {
            motion->aim_y = angle;
            strength = record->consts.air_accel * strength;
        }
        player_turn_toward(work, motion, record, motion->aim_y);
    }
    acc.x += strength;
    record->accel = acc;
}

/* the characters' constants. defined here, after player_set_gravity's reads
 * of it, where the image's pools hold the table's words */
/* clang-format off */
struct character_constants character_constants[CHARACTER_KINDS] = {
    /* PLAYER_SONIC */
    {60, 2.0F, 16.0F, 16.0F, 3.0F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.2825F, 0.3F, 4.0F,
     10.0F, 0.08F, 7.0F, 5.4F},
    /* PLAYER_SHADOW */
    {60, 2.0F, 16.0F, 16.0F, 3.0F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.2825F, 0.3F, 4.0F,
     10.0F, 0.08F, 7.0F, 5.4F},
    /* PLAYER_TAILS */
    {40, 1.5F, 16.0F, 16.0F, 0.6F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 4.0F, 0.076F, 0.048F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.01F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 3.5F,
     9.0F, 0.08F, 6.0F, 4.5F},
    /* PLAYER_EGGMAN */
    {0, 3.0F, 16.0F, 16.0F, 3.0F, 1.0F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.076F, 0.06F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 8.5F,
     15.5F, 0.08F, 14.5F, 10.0F},
    /* PLAYER_KNUCKLES */
    {60, 2.0F, 16.0F, 16.0F, 2.5F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     3.1F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 4.0F,
     11.4F, 0.08F, 9.0F, 5.7F},
    /* PLAYER_ROUGE */
    {60, 2.0F, 16.0F, 16.0F, 2.5F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     3.1F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 4.0F,
     11.4F, 0.08F, 9.0F, 5.7F},
    /* PLAYER_TAILS_MECH */
    {16, 3.0F, 16.0F, 16.0F, 1.0F, 1.0F, 2.6F, 3.0F, 0.23F, 0.46F, 1.39F,
     1.8F, 2.0F, 3.0F, 0.19F, 0.1F, 0.031F, -0.1F, -0.18F, -0.17F,
     -0.028F, {-0.014F, -0.02F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 8.0F,
     21.0F, 0.2F, 20.0F, 15.0F},
    /* PLAYER_EGGMAN_MECH */
    {16, 3.0F, 16.0F, 16.0F, 1.0F, 1.0F, 2.6F, 3.0F, 0.23F, 0.46F, 1.39F,
     1.8F, 2.0F, 3.0F, 0.19F, 0.1F, 0.031F, -0.1F, -0.18F, -0.17F,
     -0.028F, {-0.014F, -0.02F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 8.0F,
     21.0F, 0.2F, 20.0F, 15.0F},
    /* PLAYER_CHARACTER_8 */
    {60, 1.5F, 16.0F, 16.0F, 0.05F, 0.5F, 1.3F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.013F, 0.045F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.3375F, 0.3F, 3.5F,
     10.0F, 0.04F, 7.0F, 5.0F},
    /* 9 */
    {60, 2.0F, 16.0F, 16.0F, 3.0F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.2825F, 0.3F, 4.0F,
     10.0F, 0.08F, 7.0F, 5.4F},
    /* 10 */
    {60, 2.0F, 16.0F, 16.0F, 3.0F, 0.6F, 1.66F, 3.0F, 0.23F, 0.46F, 1.39F,
     2.3F, 3.7F, 5.09F, 0.076F, 0.05F, 0.031F, -0.06F, -0.18F, -0.17F,
     -0.028F, {-0.008F, -0.01F, -0.4F}, -0.1F, -0.6F, -0.2825F, 0.3F, 4.0F,
     10.0F, 0.08F, 7.0F, 5.4F},
};
/* clang-format on */

/* the speed ahead along a path past which resist.x holds a player back */
static Float path_top_speed = 5.0F;

void player_accel_path(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    Float strength;
    Angle angle;
    Bool pushed;
    NJS_VECTOR acc;
    struct path_sample sample;
    NJS_VECTOR ahead;
    Angle turn;

    acc = motion->acc;
    acc.x -= record->ground_normal.x * record->consts.gravity;
    acc.y -= record->ground_normal.y * record->consts.gravity;
    acc.z -= record->ground_normal.z * record->consts.gravity;
    turn_from_world(work, &acc);
    pushed = player_read_stick(work, record, &angle, &strength);
    if (record->spd.x > path_top_speed) {
        acc.x += record->consts.resist.x * (record->spd.x - path_top_speed);
    } else if (record->spd.x < 0.0F) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    path_find_nearest(record->path, &work->pos, &sample.pos,
                      &record->path_distance);
    ahead.x = 1.0F;
    ahead.y = ahead.z = 0.0F;
    turn_to_world(work, &ahead);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        turn = vector_angle(&sample.ahead, &ahead, &record->ball->up);
        motion->aim_y = (Uint16)(work->ang.y + turn);
        player_turn_on_course(work, motion, record, motion->aim_y);
    }
    if (!pushed) {
        record->drive = 0.0F;
    } else {
        /* @hack type: y1 converted through int, where the Sint16 converts
         * the same */
        record->drive =
            strength *
            njSin(0xc000 -
                  (Angle)(atan2f((int)player_pad_records[record->slot].y1,
                                 -player_pad_records[record->slot].x1) *
                          ANGLE_PER_RADIAN));
        acc.z += record->drive;
    }
    record->accel = acc;
}

void player_accel_push(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    Float accel;
    NJS_VECTOR acc;
    Float limit;
    int dir;
    int grade;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    grade = player_stick_into_offer(work, record);
    if (grade >= PLAYER_WALL_HEAD_ON) {
        dir = 1;
    } else if (grade == PLAYER_WALL_AWAY) {
        dir = -1;
    } else {
        dir = 0;
    }
    if (!player_dir_from_offer(work, record, &acc)) {
        collision_get_normal(work, &acc);
    }
    motion->aim_y =
        (Angle)(atan2f(-acc.z, -acc.x) * ANGLE_PER_RADIAN) & ANGLE_MASK;
    work->ang.y = motion->aim_y;
    acc.x = acc.y = acc.z = 0.0F;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.resist.x * record->spd.x;
    if (dir == 0) {
        accel = record->consts.ground_drag;
    } else {
        player_read_stick(work, record, NULL, &accel);
        accel = record->consts.ground_accel * accel;
    }
    acc.x += accel;
    if (record->spd.x == 0.0F) {
        limit = acc.y * record->consts.still_grip;
        if (dir == 0 && limit > acc.x && acc.x > -limit) {
            acc.x = 0.0F;
        }
    }
    if (dir > 0) {
        record->accel.x = acc.x;
    } else if (dir < 0) {
        record->accel.x = -acc.x;
    } else {
        acc.x = _builtin_fabsf(acc.x);
        if (record->spd.x > 0.0F) {
            record->accel.x = -acc.x;
        } else if (record->spd.x < 0.0F) {
            record->accel.x = acc.x;
        }
    }
}

void player_accel_drive(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    NJS_VECTOR acc;
    Float across;
    Float down;
    Float push;
    Angle angle;

    acc = motion->acc;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.resist.x * record->spd.x * 0.5F;
    acc.y += record->consts.resist.y * record->spd.y * 0.5F;
    acc.z += record->consts.resist.z * record->spd.z * 0.5F;
    if (player_read_stick(work, record, NULL, NULL) == FALSE) {
        push = 0.0F;
        motion->aim_y = work->ang.y;
    } else {
        /* @hack type: the axes converted through int, where each Sint16
         * converts the same */
        across = (int)player_pad_records[record->slot].x1;
        down = (int)player_pad_records[record->slot].y1;
        /* @hack redundant: across moved toward 0, which nothing reads */
        if (across >= 0.0F) {
            across -= PAD_DEAD_ZONE;
        } else {
            across += PAD_DEAD_ZONE;
        }
        if (down >= 0.0F) {
            down -= PAD_DEAD_ZONE;
        } else {
            down += PAD_DEAD_ZONE;
        }
        angle = work->ang.y + (player_pad_records[record->slot].x1 << 2);
        push = down;
        if (down > 0.0F) {
            push = 0.0F;
        }
        push = push * push * (1.0F / (PAD_STICK_REACH * PAD_STICK_REACH));
        if (push > 1.0F) {
            push = 1.0F;
        }
        motion->aim_y = angle;
        work->ang.y = angle;
        push = record->consts.air_accel * push;
    }
    acc.x += push;
    record->accel = acc;
}

void player_accel_glide(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    Float strength;
    Angle angle;
    NJS_VECTOR acc;
    Float gravity;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc = motion->acc;
    gravity = record->consts.gravity * 0.05F;
    acc.x += world_gravity.x * gravity;
    acc.y += world_gravity.y * gravity;
    acc.z += world_gravity.z * gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.resist.x * record->spd.x * 0.5F;
    acc.y += record->consts.resist.y * record->spd.y * 0.5F;
    acc.z += record->consts.resist.z * record->spd.z * 0.5F;
    if (player_read_stick(work, record, &angle, &strength) == FALSE) {
        strength = 0.0F;
        motion->aim_y = work->ang.y;
    } else {
        strength = record->consts.air_accel * strength;
        motion->aim_y = angle;
        player_turn_toward(work, motion, record, motion->aim_y);
    }
    acc.x += strength;
    record->accel = acc;
}

void player_accel_climb(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    Angle angle;
    Float strength;
    Float x;
    Float y;
    NJS_VECTOR acc;
    NJS_VECTOR view;
    NJS_VECTOR up;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    if (player_read_stick(work, record, NULL, NULL)) {
        if (work->id != WORK_ID_RIVAL) {
            /* @hack type: the axes converted through int, where each Sint16
             * converts the same */
            x = (int)player_pad_records[record->slot].x1;
            y = (int)player_pad_records[record->slot].y1;
            acc.x = -(y * (0.8F / PAD_STICK_MAX));
            if (acc.x > 1.0F) {
                acc.x = 1.0F;
            }
            acc.y = 0.0F;
            acc.z = x * (0.8F / PAD_STICK_MAX);
            if (acc.z > 1.0F) {
                acc.z = 1.0F;
            }
        } else {
            player_read_stick(work, record, &angle, &strength);
            x = strength * njSin(angle);
            y = strength * njCos(angle);
            acc.x = -(y * 0.8F);
            if (acc.x > 1.0F) {
                acc.x = 1.0F;
            }
            acc.y = 0.0F;
            acc.z = x * 0.8F;
            if (acc.z > 1.0F) {
                acc.z = 1.0F;
            }
        }
    } else {
        acc.x = acc.y = acc.z = 0.0F;
    }
    if (record->ground_flags & GROUND_CLIMB_REVERSED) {
        up.x = up.z = 0.0F;
        up.y = 1.0F;
        turn_to_world(work, &up);
        view = camera_views[record->slot]->dir;
        njUnitVector(&view);
        if (njInnerProduct(&view, &up) > 0.0F) {
            acc.z = -acc.z;
        }
    }
    record->accel = acc;
}

/* a board's speed eases toward the way the board points. each step it keeps
 * a share of itself, and takes the rest of the same speed turned along the
 * board. the share is BOARD_KEEP below BOARD_EASE_SPEED. from there to
 * BOARD_EASE_TOP it grows by BOARD_KEEP_GAIN for each BOARD_EASE_SPEED of
 * speed. past BOARD_EASE_TOP it is BOARD_KEEP_TOP */
#define BOARD_EASE_SPEED 11.5F
#define BOARD_EASE_TOP 14.72F
#define BOARD_KEEP 0.6F
#define BOARD_KEEP_GAIN 0.3F
#define BOARD_KEEP_TOP 0.9F
/* the stick aims the board up to BOARD_AIM off its heading at
 * BOARD_EASE_SPEED. the aim falls by BOARD_AIM_FALL for each BOARD_EASE_SPEED
 * faster, and grows as much for each slower. it stays within BOARD_AIM_LEAST
 * and BOARD_AIM_MOST, and scales by the stick's push across. the heading turns
 * toward it by a sixteenth of the gap and BOARD_TURN more */
#define BOARD_AIM 0x200
#define BOARD_AIM_FALL 256.0F
#define BOARD_AIM_LEAST 0x120
#define BOARD_AIM_MOST 0x280
#define BOARD_TURN 0x100
/* a board the stick leans by more than BOARD_LEAN meets resist.x of its speed
 * ahead, past BOARD_LEAN_SPEED */
#define BOARD_LEAN_SPEED 3.68F
#define BOARD_LEAN 0.05F
/* the shares of resist.x a board's speed across meets. it meets
 * BOARD_RESIST_ACROSS while the board runs ahead past its top, and
 * BOARD_RESIST_BACK while it runs back. a speed back meets BOARD_RESIST_ACROSS
 * of it too */
#define BOARD_RESIST_ACROSS 1.5F
#define BOARD_RESIST_BACK 2.0F
/* under PLAYER_ANIM_DRIVEN, the push takes BOARD_DRIVEN_SHARE of the stroke.
 * it takes up to BOARD_DRIVEN_GAIN more as the animation plays */
#define BOARD_DRIVEN_SHARE 0.5F
#define BOARD_DRIVEN_GAIN 0.005F

void player_accel_board(struct task_work *work, struct player_motion *motion,
                        struct player *record, Float top)
{
    Float strength;
    Angle angle;
    Bool pushed;
    NJS_VECTOR acc;
    NJS_VECTOR push;
    NJS_VECTOR dir;
    Angle turn;
    Angle step;
    Angle off;
    Float power;
    Float keep;
    Float rest;
    Float limit;
    Float x;

    acc = motion->acc;
    push.z = 0.0F;
    push.y = 0.0F;
    push.x = 0.0F;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.y += -record->consts.gravity;
    turn_from_world(work, &acc);
    pushed = player_read_stick(work, record, &angle, &strength);
    if (record->spd.x > top) {
        acc.x += record->consts.resist.x * record->spd.x;
        acc.z +=
            BOARD_RESIST_ACROSS * (record->consts.resist.x * record->spd.z);
    } else if (record->spd.x < 0.0F) {
        acc.x +=
            BOARD_RESIST_ACROSS * (record->consts.resist.x * record->spd.x);
        acc.z += BOARD_RESIST_BACK * (record->consts.resist.x * record->spd.z);
    }
    acc.y += record->consts.resist.y * record->spd.y;
    dir.x = dir.z = 0.0F;
    dir.y = -1.0F;
    turn_from_world(work, &dir);
    /* @hack redundant: the heading of the way down, which nothing reads */
    if ((dir.x * dir.x) + (dir.z * dir.z) > 0.01F) {
        atan2f(dir.z, dir.x);
    }
    if (pushed == FALSE) {
        record->drive = 0.0F;
    } else {
        off = angle_distance(work->ang.y, angle);
        if (record->spd.x == 0.0F && off > STEER_TURN) {
            motion->aim_y = angle;
            player_turn_on_course(work, motion, record, motion->aim_y);
            record->drive = 0.0F;
        } else {
            /* @hack type: y1 converted through int, where the Sint16
             * converts the same */
            turn = (HALF_TURN + QUARTER_TURN) -
                   (Angle)(atan2f((int)player_pad_records[record->slot].y1,
                                  -player_pad_records[record->slot].x1) *
                           ANGLE_PER_RADIAN);
            record->drive = strength * njSin(turn);
            if (record->anim.mode == PLAYER_ANIM_DRIVEN) {
                x = record->anim.frame /
                    (Float)
                        motion_table[record->anim.actions[record->anim.current]
                                         .motion]
                            .motion->nbFrame;
                strength *= BOARD_DRIVEN_SHARE;
                strength += BOARD_DRIVEN_GAIN * x;
            }
            power = strength * njSin(turn);
            step = BOARD_AIM - (Angle)((record->spd.x - BOARD_EASE_SPEED) *
                                       (BOARD_AIM_FALL / BOARD_EASE_SPEED));
            if (step < BOARD_AIM_LEAST) {
                step = BOARD_AIM_LEAST;
            } else if (step > BOARD_AIM_MOST) {
                step = BOARD_AIM_MOST;
            }
            step = (Angle)(step * power) + work->ang.y;
            if (record->spd.x > BOARD_LEAN_SPEED && power > BOARD_LEAN) {
                acc.x += record->consts.resist.x * record->spd.x;
            }
            motion->aim_y = step;
            angle = angle_distance(work->ang.y, motion->aim_y) >> 4;
            work->ang.y =
                angle_approach(work->ang.y, motion->aim_y, angle + BOARD_TURN);
        }
        if (record->spd.x < record->consts.top_speed) {
            strength *= record->consts.ground_accel;
        } else {
            strength *= record->consts.ground_accel * FAST_PUSH;
        }
        if (record->spd.x <= record->consts.trot_speed) {
            push.x = strength;
        }
    }
    dir.x = njSqrt((record->spd.x * record->spd.x) +
                   (record->spd.z * record->spd.z));
    dir.y = record->spd.y;
    dir.z = 0.0F;
    if (record->spd.x < 0.0F) {
        dir.x = -dir.x;
    }
    keep = BOARD_KEEP;
    if (record->spd.x < BOARD_EASE_SPEED) {
        rest = 0.4F;
    } else if (record->spd.x < BOARD_EASE_TOP) {
        keep += (record->spd.x - BOARD_EASE_SPEED) *
                (BOARD_KEEP_GAIN / BOARD_EASE_SPEED);
        rest = 1.0F - keep;
    } else {
        keep = BOARD_KEEP_TOP;
        rest = 0.1F;
    }
    record->spd.x = (keep * record->spd.x) + (dir.x * rest);
    record->spd.y = (keep * record->spd.y) + (dir.y * rest);
    record->spd.z = (keep * record->spd.z) + (dir.z * rest);
    if (push.x < 0.0F) {
        if (record->spd.x > 0.0F) {
            x = acc.x + push.x;
            if (x * push.x < 0.0F) {
                x = 0.0F;
            }
            acc.x = x;
        } else {
            if (acc.y < 0.0F) {
                limit = acc.y * record->consts.still_grip * record->grip_scale;
            } else {
                limit = 0.0F;
            }
            x = limit + push.x;
            if (x >= 0.0F) {
                x = 0.0F;
            }
            acc.x = x;
        }
    } else {
        if (acc.y < 0.0F) {
            limit = acc.y * record->consts.still_grip * record->grip_scale;
        } else {
            limit = 0.0F;
        }
        acc.x += push.x;
        if (acc.x > 0.0F) {
            if (limit > acc.x) {
                acc.x = 0.0F;
            } else {
                acc.x -= limit;
            }
        } else if (acc.x < 0.0F) {
            if (acc.x > -limit) {
                acc.x = 0.0F;
            } else {
                acc.x += limit;
            }
        }
    }
    if (record->spd.z == 0.0F) {
        /* @hack spelling: limit made in two statements, where one would do */
        limit = acc.y * record->consts.still_grip;
        limit = limit * record->grip_scale;
        if (limit < 0.0F) {
            limit = 0.0F;
        }
        acc.z += push.z;
        if (limit > acc.z && acc.z > -limit) {
            acc.z = 0.0F;
        }
    } else {
        limit = acc.y * record->consts.side_grip * record->grip_scale;
        if (limit < 0.0F) {
            limit = 0.0F;
        }
        acc.z += push.z;
        if (acc.z > 0.0F) {
            if (limit > acc.z) {
                acc.z = 0.0F;
            } else {
                acc.z -= limit;
            }
        } else if (acc.z < 0.0F) {
            if (acc.z > -limit) {
                acc.z = 0.0F;
            } else {
                acc.z += limit;
            }
        }
    }
    record->accel = acc;
}

void player_accel_board_air(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    Float strength;
    Angle angle;
    NJS_VECTOR acc;
    NJS_VECTOR down;
    Angle off;

    acc = motion->acc;
    acc.y += -record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.air_resist * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (player_read_stick(work, record, &angle, &strength) == FALSE) {
        strength = 0.0F;
        if (record->spd.x <= record->consts.trot_speed) {
            down.x = 0.0F;
            down.y = 1.0F;
            down.z = 0.0F;
            matrix_push_unit();
            njRotateZ(NULL, record->ground.below_ang_z);
            njRotateX(NULL, record->ground.below_ang_x);
            njCalcVector(NULL, &down, &down);
            njPopMatrixEx();
            down.y = 0.0F;
            if (njScalor(&down) > 0.01F) {
                motion->aim_y = angle_approach(
                    work->ang.y,
                    (Angle)(atan2f(down.z, down.x) * ANGLE_PER_RADIAN), 0x200);
                player_turn_on_course(work, motion, record, motion->aim_y);
            }
        } else {
            motion->aim_y = work->ang.y;
        }
    } else {
        off = angle_distance(work->ang.y, angle);
        if (record->spd.x > record->consts.running_speed && off > TURN_AWAY) {
            strength = record->consts.air_brake * strength;
        } else {
            motion->aim_y = angle;
            strength = record->consts.air_accel * strength;
        }
        player_turn_at_speed(work, motion, record, motion->aim_y);
    }
    acc.x += strength;
    record->accel = acc;
}

void player_accel_board_trick(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    NJS_VECTOR acc;

    acc = motion->acc;
    acc.y += -record->consts.gravity - 0.05F;
    turn_from_world(work, &acc);
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    motion->aim_y = work->ang.y;
    record->accel = acc;
}

/* a braking board turns by up to BRAKE_TURN toward the way its speed runs.
 * it then turns by up to BRAKE_DOWNHILL_TURN toward the way down. ground
 * tilted less than BRAKE_FLAT has no way down, and the board keeps its
 * heading */
#define BRAKE_TURN 0x200
#define BRAKE_DOWNHILL_TURN 0x400
#define BRAKE_FLAT 0x400

void player_accel_board_brake(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    Angle angle;
    Float strength;
    NJS_VECTOR acc;
    NJS_VECTOR push;
    NJS_VECTOR dir;
    Angle heading;
    Float keep;
    Float rest;
    Float limit;
    Float x;

    acc = motion->acc;
    push.z = 0.0F;
    push.y = 0.0F;
    push.x = 0.0F;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.y += -record->consts.gravity;
    turn_from_world(work, &acc);
    player_read_stick(work, record, &angle, &strength);
    if (record->spd.x > 0.0F) {
        acc.x += record->consts.resist.x * record->spd.x;
        acc.z +=
            BOARD_RESIST_ACROSS * (record->consts.resist.x * record->spd.z);
    } else if (record->spd.x < 0.0F) {
        acc.x +=
            BOARD_RESIST_ACROSS * (record->consts.resist.x * record->spd.x);
        acc.z += BOARD_RESIST_BACK * (record->consts.resist.x * record->spd.z);
    }
    acc.y += record->consts.resist.y * record->spd.y;
    dir.x = dir.z = 0.0F;
    dir.y = -1.0F;
    turn_from_world(work, &dir);
    if (_builtin_fabsf(dir.y) < njCos(BRAKE_FLAT)) {
        heading = work->ang.y;
    } else {
        heading =
            (Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN) + work->ang.y;
    }
    dir = record->spd;
    if (record->spd.x > record->consts.trot_speed) {
        turn_to_world(work, &dir);
        motion->aim_y =
            (Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN) & ANGLE_MASK;
    }
    work->ang.y = angle_approach(work->ang.y, motion->aim_y, BRAKE_TURN);
    turn_from_world(work, &dir);
    record->spd = dir;
    dir = record->spd;
    turn_to_world(work, &dir);
    motion->aim_y = heading;
    work->ang.y = angle_approach(work->ang.y, heading, BRAKE_DOWNHILL_TURN);
    turn_from_world(work, &dir);
    keep = 0.8F;
    rest = 1.0F - keep;
    record->spd.x = (keep * record->spd.x) + (dir.x * rest);
    record->spd.y = (keep * record->spd.y) + (dir.y * rest);
    record->spd.z = (keep * record->spd.z) + (dir.z * rest);
    /* @hack redundant: each push.x stored twice, where once would do */
    if (record->spd.x > 2.2F) {
        push.x = -(record->spd.x * 0.03F);
        push.x = -(record->spd.x * 0.03F);
    } else if (record->spd.x > 0.5F) {
        push.x = -(record->spd.x * 0.015F);
        push.x = -(record->spd.x * 0.015F);
    }
    if (push.x < 0.0F) {
        if (record->spd.x > 0.0F) {
            x = acc.x + push.x;
            if (x * push.x < 0.0F) {
                x = 0.0F;
            }
            acc.x = x;
        } else {
            if (acc.y < 0.0F) {
                limit = acc.y * record->consts.still_grip * record->grip_scale;
            } else {
                limit = 0.0F;
            }
            x = limit + push.x;
            if (x >= 0.0F) {
                x = 0.0F;
            }
            acc.x = x;
        }
    } else {
        if (acc.y < 0.0F) {
            limit = acc.y * record->consts.still_grip * record->grip_scale;
        } else {
            limit = 0.0F;
        }
        acc.x += push.x;
        if (acc.x > 0.0F) {
            if (limit > acc.x) {
                acc.x = 0.0F;
            } else {
                acc.x -= limit;
            }
        } else if (acc.x < 0.0F) {
            if (acc.x > -limit) {
                acc.x = 0.0F;
            } else {
                acc.x += limit;
            }
        }
    }
    if (record->spd.z == 0.0F) {
        limit = acc.y * record->consts.still_grip * record->grip_scale;
        if (limit < 0.0F) {
            limit = 0.0F;
        }
        acc.z += push.z;
        if (limit > acc.z && acc.z > -limit) {
            acc.z = 0.0F;
        }
    } else {
        limit = acc.y * record->consts.side_grip * record->grip_scale;
        if (limit < 0.0F) {
            limit = 0.0F;
        }
        acc.z += push.z;
        if (acc.z > 0.0F) {
            if (limit > acc.z) {
                acc.z = 0.0F;
            } else {
                acc.z -= limit;
            }
        } else if (acc.z < 0.0F) {
            if (acc.z > -limit) {
                acc.z = 0.0F;
            } else {
                acc.z += limit;
            }
        }
    }
    record->accel = acc;
}

void player_accel_brake(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR brake;
    Float grip;
    Float x;

    acc = motion->acc;
    brake.z = 0.0F;
    brake.y = 0.0F;
    brake.x = 0.0F;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.resist.x * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (record->spd.x > 0.0F) {
        brake.x += record->consts.brake_drag * record->grip_scale;
    } else if (record->spd.x < 0.0F) {
        brake.x -= record->consts.brake_drag * record->grip_scale;
    }
    if (record->spd.z > 0.0F) {
        brake.z += record->consts.brake_drag * record->grip_scale;
    } else if (record->spd.z < 0.0F) {
        brake.z -= record->consts.brake_drag * record->grip_scale;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        if (record->spd.x == 0.0F) {
            grip = acc.y * record->consts.still_grip * record->grip_scale;
            if (grip > acc.x && acc.x > -grip) {
                acc.x = 0.0F;
            }
        } else {
            if (acc.y < 0.0F) {
                grip = acc.y * record->consts.moving_grip * record->grip_scale;
            } else {
                grip = 0.0F;
            }
            x = acc.x + brake.x;
            if (x > 0.0F) {
                x -= grip;
                if (x < 0.0F) {
                    x = 0.0F;
                }
            } else if (x < 0.0F) {
                x += grip;
                if (x > 0.0F) {
                    x = 0.0F;
                }
            }
            acc.x = x;
            if (record->spd.x > 0.0F && -acc.x > record->spd.x) {
                acc.x = -record->spd.x;
            } else if (record->spd.x < 0.0F && -acc.x < record->spd.x) {
                acc.x = -record->spd.x;
            }
        }
        if (record->spd.z == 0.0F) {
            grip = acc.y * record->consts.still_grip * record->grip_scale;
            if (grip > acc.z && acc.z > -grip) {
                acc.z = 0.0F;
            }
        } else {
            if (acc.y < 0.0F) {
                grip = acc.y * record->consts.side_grip * record->grip_scale;
            } else {
                grip = 0.0F;
            }
            if (acc.z > 0.0F) {
                brake.z -= grip;
            } else if (acc.z < 0.0F) {
                brake.z += grip;
            }
            acc.z += brake.z;
            if (record->spd.z > 0.0F && -acc.z > record->spd.z) {
                acc.z = -record->spd.z;
            } else if (record->spd.z < 0.0F && -acc.z < record->spd.z) {
                acc.z = -record->spd.z;
            }
        }
    }
    player_turn_toward(work, motion, record, motion->aim_y);
    record->accel = acc;
}

void player_accel_launched(struct task_work *work, struct player_motion *motion,
                           struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR brake;
    NJS_VECTOR push;
    struct polygon_ball *ball;
    Float scale;
    Float grip;

    acc = motion->acc;
    brake.z = 0.0F;
    brake.y = 0.0F;
    brake.x = 0.0F;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    if (work->flags & PLAYER_WORK_GROUNDED) {
        ball = record->ball;
        scale =
            record->consts.top_speed / (record->consts.gravity * (5.0F / 7.0F));
        push.x = ball->hit.normal_speed.x / scale;
        push.y = ball->hit.normal_speed.y / scale;
        push.z = ball->hit.normal_speed.z / scale;
        ball->hit.normal_speed.y = 0.0F;
        ball->hit.normal_speed.x = 0.0F;
        ball->hit.normal_speed.z = 0.0F;
        turn_from_world(work, &push);
        acc.y += push.y;
        acc.z += push.z;
    }
    acc.x += record->consts.resist.x * record->spd.x;
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    if (work->flags & PLAYER_WORK_GROUNDED) {
        if (record->spd.x == 0.0F) {
            grip = acc.y * record->consts.still_grip * record->grip_scale;
            if (grip > acc.x && acc.x > -grip) {
                acc.x = 0.0F;
            }
        } else {
            if (acc.y < 0.0F) {
                grip = acc.y * record->consts.moving_grip * record->grip_scale;
            } else {
                grip = 0.0F;
            }
            if (acc.x > 0.0F) {
                brake.x -= grip;
            } else if (acc.x < 0.0F) {
                brake.x += grip;
            }
            acc.x += brake.x;
            if (record->spd.x > 0.0F && -acc.x > record->spd.x) {
                acc.x = -record->spd.x;
            } else if (record->spd.x < 0.0F && -acc.x < record->spd.x) {
                acc.x = -record->spd.x;
            }
        }
        if (record->spd.z == 0.0F) {
            grip = acc.y * record->consts.still_grip * record->grip_scale;
            if (grip > acc.z && acc.z > -grip) {
                acc.z = 0.0F;
            }
        } else {
            if (acc.y < 0.0F) {
                grip = acc.y * record->consts.side_grip * record->grip_scale;
            } else {
                grip = 0.0F;
            }
            if (acc.z > 0.0F) {
                brake.z -= grip;
            } else if (acc.z < 0.0F) {
                brake.z += grip;
            }
            acc.z += brake.z;
            if (record->spd.z > 0.0F && -acc.z > record->spd.z) {
                acc.z = -record->spd.z;
            } else if (record->spd.z < 0.0F && -acc.z < record->spd.z) {
                acc.z = -record->spd.z;
            }
        }
    }
    record->accel = acc;
}

/* the part of its speed ahead that a player under PLAYER_WORK_BALL loses
 * each step on a slope, in place of resist.x */
#define ROLL_RESIST 0.0002F

void player_accel_roll(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_VECTOR acc;
    NJS_VECTOR push;
    struct polygon_ball *ball;
    Float scale;

    acc = motion->acc;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    /* @hack spelling: the level compared with minus zero, which tests as 0
     * does */
    if (record->spd.x > record->consts.running_speed && record->level < -0.0F) {
        acc.y = -(acc.y * 8.0F);
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        ball = record->ball;
        scale =
            record->consts.top_speed / (record->consts.gravity * (5.0F / 7.0F));
        push.x = ball->hit.normal_speed.x / scale;
        push.y = ball->hit.normal_speed.y / scale;
        push.z = ball->hit.normal_speed.z / scale;
        ball->hit.normal_speed.y = 0.0F;
        ball->hit.normal_speed.x = 0.0F;
        ball->hit.normal_speed.z = 0.0F;
        turn_from_world(work, &push);
        acc.y += push.y;
        acc.z += push.z;
    }
    if (work->flags & PLAYER_WORK_BALL) {
        if (record->level > 0.98F) {
            acc.x += record->consts.resist.x * record->spd.x;
        } else {
            acc.x += -(record->spd.x * ROLL_RESIST);
        }
    } else {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    acc.y += record->consts.resist.y * record->spd.y;
    acc.z += record->consts.resist.z * record->spd.z;
    record->accel = acc;
}

void player_accel_gravity(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    NJS_VECTOR acc;

    acc = motion->acc;
    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    record->accel = acc;
}

void player_turn_to_stick(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    NJS_VECTOR forward;
    NJS_VECTOR axis;
    NJS_VECTOR heading;
    Angle stick;
    Angle angle;
    Angle turn;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    /* @hack spelling: the stick's test in an else after the path's return,
     * where an if of its own would do */
    if (work->flags & PLAYER_WORK_ON_PATH) {
        player_follow_path(work, motion, record);
        return;
    } else if (!player_read_stick(work, record, &angle, NULL)) {
        if (work->flags & PLAYER_WORK_GROUNDED) {
            /* @hack redundant: both arms roll the player downhill, where one
             * call would do */
            if (record->spd.x > 2.0F) {
                player_roll_downhill(work, motion, record);
            } else {
                player_roll_downhill(work, motion, record);
            }
            return;
        } else {
            /* @hack spelling: the aim's store in an else after the roll's
             * return, where a statement after the if would do */
            motion->aim_y = work->ang.y;
        }
    } else if (record->ground_flags & GROUND_CLING) {
        /* @hack type: the pad's axes converted through int, where each
         * Sint16 converts the same */
        stick = (Angle)(atan2f((int)player_pad_records[record->slot].x1,
                               (int)player_pad_records[record->slot].y1) *
                        ANGLE_PER_RADIAN);
        axis.x = 0.0F;
        axis.y = 0.0F;
        axis.z = -1.0F;
        matrix_push_unit();
        njRotateY(NULL, camera_views[record->slot]->ang.y);
        njRotateX(NULL, camera_views[record->slot]->ang.x);
        njCalcVector(NULL, &axis, &forward);
        njPopMatrixEx();
        matrix_push_unit();
        njRotate(NULL, &record->ground_normal, stick);
        njCalcVector(NULL, &forward, &heading);
        njPopMatrixEx();
        axis.x = 1.0F;
        axis.y = 0.0F;
        axis.z = 0.0F;
        turn_to_world(work, &axis);
        turn = vector_angle(&axis, &heading, &record->ground_normal);
        motion->aim_y = (Uint16)(work->ang.y + turn);
        player_turn_at_speed(work, motion, record, motion->aim_y);
    } else {
        motion->aim_y = angle;
    }
    player_turn_toward(work, motion, record, motion->aim_y);
}

void player_turn_to_stick_at_speed(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record)
{
    Angle angle;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    if (!player_read_stick(work, record, &angle, NULL)) {
        motion->aim_y = work->ang.y;
    } else {
        motion->aim_y = angle;
    }
    player_turn_at_speed(work, motion, record, motion->aim_y);
}

void player_turn_to_stick_slowly(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    Angle angle;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    if (!player_read_stick(work, record, &angle, NULL)) {
        motion->aim_y = work->ang.y;
    } else {
        motion->aim_y = angle;
    }
    player_turn_slowly(work, motion, record, motion->aim_y);
}

/* turn the motion's aim_y down the slope the player stands on. the player turns
 * toward aim_y by player_turn_toward. a way down that lies behind the player is
 * mirrored ahead. the aim then turns only to the side the slope falls to. off a
 * slope, aim_y takes the heading. nothing changes under stick_lock, on
 * WORLD_UPRIGHT ground, or for a motion that rises steeply */
static void player_roll_downhill(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    NJS_VECTOR down;
    NJS_VECTOR spd;
    Angle angle;
    Angle step;

    if (record->stick_lock != 0) {
        return;
    }
    if (record->ground_flags & WORLD_UPRIGHT) {
        return;
    }
    down = world_gravity;
    spd = motion->spd;
    if (njScalor(&spd) > record->consts.trot_speed) {
        njUnitVector(&spd);
        if (njInnerProduct(&world_gravity, &spd) < RISING_STEEP) {
            return;
        }
    }
    turn_from_world(work, &down);
    if (down.y < 0.0F &&
        (down.y > SLOPE_LEVEL_Y || (down.y > SLOPE_LEVEL_Y_RELAY &&
                                    stage_current_id() == STAGE_RELAY_SONIC))) {
        if (down.x < 0.0F) {
            down.x = -down.x;
        }
        angle =
            work->ang.y + (Angle)(atan2f(down.z, down.x) * ANGLE_PER_RADIAN);
        if (down.z < 0.0F) {
            down.z = -down.z;
        }
        if (work->flags & PLAYER_WORK_BALL) {
            step = (Angle)(down.z * SLOPE_TURN_BALL);
        } else {
            step = (Angle)(down.z * SLOPE_TURN);
        }
        motion->aim_y = angle_approach(motion->aim_y, angle, step);
    } else {
        motion->aim_y = work->ang.y;
    }
    player_turn_toward(work, motion, record, motion->aim_y);
}

Float player_turn_along_slope(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    NJS_VECTOR down;
    NJS_VECTOR spd;
    Angle angle;
    Angle step;
    Float ahead;

    if (record->stick_lock == 0 && !(record->ground_flags & WORLD_UPRIGHT)) {
        down = world_gravity;
        spd = motion->spd;
        if (njScalor(&spd) > record->consts.trot_speed) {
            njUnitVector(&spd);
            if (njInnerProduct(&world_gravity, &spd) < RISING_STEEP) {
                return 0.0F;
            }
        }
        turn_from_world(work, &down);
#ifdef NONMATCHING
        /* this side repairs the tag below: it returns 0, which the callers
         * read as a slope that isn't behind. where the image returns ahead
         * unset, it returns what FR15 held at the call, which C can't
         * reproduce */
        ahead = 0.0F;
#endif
        /* @bug uninitialized: ahead is returned unset where the way down's y
         * lies outside the slope's range, and aim_y took the heading
         * (0x8c03e7c6) */
        /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
        if (down.y < 0.0F && down.y > SLOPE_LEVEL_Y) {
            angle = (Angle)(atan2f(down.z, down.x) * ANGLE_PER_RADIAN);
            ahead = njCos(angle);
            if (ahead < 0.0F) {
                angle += HALF_TURN;
            }
            angle += work->ang.y;
            if (down.z < 0.0F) {
                down.z = -down.z;
            }
            if (work->flags & PLAYER_WORK_BALL) {
                step = (Angle)(down.z * SLOPE_TURN_BALL);
            } else {
                step = (Angle)(down.z * SLOPE_TURN);
            }
            motion->aim_y = angle_approach(motion->aim_y, angle, step);
        } else {
            motion->aim_y = work->ang.y;
        }
        player_turn_toward(work, motion, record, motion->aim_y);
        return ahead;
        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    }
    /* @bug uninitialized: no value is returned under stick_lock or on
     * WORLD_UPRIGHT ground (0x8c03e6d6, 0x8c03e6e6). the characters' steps
     * take what FR0 holds for the result, and compare it with 0 (0x8c07b13e,
     * 0x8c088bc2, 0x8c09c364) */
#ifdef NONMATCHING
    /* this side keeps the tag's outcome. the image returns what FR0 held at
     * the call, where each caller has just found a speed above a bound. each
     * caller tests the result for a value below 0, which neither that speed
     * nor this side's 0 is */
    return 0.0F;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* player_roll_downhill's turn for a player faster than trot_speed, by
 * player_turn_on_course. it turns on WORLD_UPRIGHT ground too, wherever the way
 * down's y lies above SLOPE_LEVEL_Y. nothing changes under stick_lock or for a
 * motion that rises steeply */
static void player_steer_downhill(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record)
{
    NJS_VECTOR down;
    NJS_VECTOR spd;
    Angle angle;
    Angle step;

    if (record->stick_lock != 0) {
        return;
    }
    down = world_gravity;
    spd = motion->spd;
    if (njScalor(&spd) > record->consts.trot_speed) {
        njUnitVector(&spd);
        if (njInnerProduct(&world_gravity, &spd) < RISING_STEEP) {
            return;
        }
    } else {
        return;
    }
    turn_from_world(work, &down);
    if (down.y > SLOPE_LEVEL_Y) {
        if (down.x < 0.0F) {
            down.x = -down.x;
        }
        angle =
            work->ang.y + (Angle)(atan2f(down.z, down.x) * ANGLE_PER_RADIAN);
        if (down.z < 0.0F) {
            down.z = -down.z;
        }
        if (work->flags & PLAYER_WORK_BALL) {
            step = (Angle)(down.z * record->spd.x * SLOPE_STEER_BALL /
                           record->consts.trot_speed);
        } else {
            step = (Angle)(down.z * SLOPE_STEER);
        }
        motion->aim_y = angle_approach(motion->aim_y, angle, step);
    } else {
        motion->aim_y = work->ang.y;
    }
    player_turn_on_course(work, motion, record, motion->aim_y);
}

void player_slide_downhill(struct task_work *work, struct player_motion *motion,
                           struct player *record)
{
    NJS_VECTOR slide;
    NJS_PLANE plane;
    Angle angle;

    if (record->stick_lock != 0) {
        return;
    }
    slide = world_gravity;
    if (work->flags & PLAYER_WORK_ON_GROUND) {
        plane.px = plane.py = plane.pz = 0.0F;
        plane.vx = record->ground_normal.x;
        plane.vy = record->ground_normal.y;
        plane.vz = record->ground_normal.z;
        njDistanceP2PL(&world_gravity, &plane, &slide);
        if (njScalor(&slide) != 0.0F) {
            njUnitVector(&slide);
        } else {
            slide = world_gravity;
        }
    }
    slide.x *= record->consts.gravity * 0.4F;
    slide.y *= record->consts.gravity * 0.4F;
    slide.z *= record->consts.gravity * 0.4F;
    njAddVector(&slide, &motion->acc);
    njAddVector(&motion->spd, &slide);
    slide = motion->spd;
    slide.x *= record->consts.resist.x;
    slide.y *= record->consts.resist.x;
    slide.z *= record->consts.resist.x;
    njAddVector(&motion->spd, &slide);
    slide = motion->spd;
    turn_from_world(work, &slide);
    record->spd = slide;
    player_limit_speed(work, motion, record);
    player_stop_creep(work, motion, record);
    slide = motion->spd;
    if (njScalor(&slide) != 0.0F) {
        njUnitVector(&slide);
        angle = (Angle)(atan2f(slide.z, slide.x) * ANGLE_PER_RADIAN);
        motion->aim_y = angle;
        work->ang.y = angle;
    }
}

/* the most player_turn_upright tilts a player a step */
#define UPRIGHT_STEP 0x800

void player_turn_upright(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    NJS_VECTOR spd;
    NJS_VECTOR middle;
    Float squared;

    if (work->flags & PLAYER_WORK_GROUNDED) {
        return;
    }
    spd = record->spd;
    if (!(record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15))) {
        squared = (spd.x * spd.x) + (spd.y * spd.y) + (spd.z * spd.z);
        if (squared > record->consts.full_speed * record->consts.full_speed) {
            return;
        }
    }
    turn_to_world(work, &spd);
    middle.x = middle.z = 0.0F;
    middle.y = record->consts.height * 0.5F;
    turn_to_world(work, &middle);
    njAddVector(&middle, &work->pos);
    work->ang.x =
        angle_approach(work->ang.x, world_gravity_ang_x, UPRIGHT_STEP);
    work->ang.z =
        angle_approach(work->ang.z, world_gravity_ang_z, UPRIGHT_STEP);
    turn_from_world(work, &spd);
    record->spd = spd;
    work->pos = middle;
    middle.x = middle.z = 0.0F;
    middle.y = record->consts.height * 0.5F;
    turn_to_world(work, &middle);
    njSubVector(&work->pos, &middle);
}

/* the share of its own spd that a turn keeps. the rest it takes of the speed
 * the player had in the world, turned into its new axes. a turn keeps
 * TURN_KEEP_AIR in the air, and TURN_KEEP_STILL on the ground without the
 * stick. with the stick it keeps TURN_KEEP_LEVEL where the record's level is
 * above TURN_LEVEL, or TURN_KEEP_STEEP on steeper ground. player_turn_toward
 * scales its share on the ground by a grip_scale below 1 */
#define TURN_KEEP_AIR 0.1F
#define TURN_KEEP_STILL 0.05F
#define TURN_KEEP_LEVEL 0.99F
#define TURN_KEEP_STEEP 0.5F
#define TURN_LEVEL 0.4F
/* the step of a turn toward a heading. player_turn_toward steps by
 * TOWARD_STEP while the heading lies more than TOWARD_FAR away. nearer, it
 * steps by a quarter of the gap down to TOWARD_NEAR, and by an eighth within.
 * player_turn_on_course steps by COURSE_STEP. player_turn_at_speed steps by
 * SPEED_STEP. past full_speed its step falls by SPEED_STEP_FALL times the root
 * of SPEED_STEP_SCALE of the speed past it */
#define TOWARD_FAR EIGHTH_TURN
#define TOWARD_NEAR 0x1000
#define TOWARD_STEP 0x800
#define COURSE_STEP EIGHTH_TURN
#define SPEED_STEP 0x100
#define SPEED_STEP_FALL 256.0F
#define SPEED_STEP_SCALE 0.0625F

void player_turn_toward(struct task_work *work, struct player_motion *motion,
                        struct player *record, Angle heading)
{
    NJS_VECTOR spd;
    Angle gap;
    Float keep;
    Float rest;
    Float x;
    Float share;

    spd = record->spd;
    turn_to_world(work, &spd);
    gap = angle_distance(work->ang.y, heading);
    if (gap > TOWARD_FAR) {
        work->ang.y = angle_approach(work->ang.y, heading, TOWARD_STEP);
    } else if (gap > TOWARD_NEAR) {
        gap >>= 2;
        work->ang.y = angle_approach(work->ang.y, heading, gap);
    } else {
        gap >>= 3;
        work->ang.y = angle_approach(work->ang.y, heading, gap);
    }
    turn_from_world(work, &spd);
    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        rest = 1.0F - TURN_KEEP_AIR;
        keep = TURN_KEEP_AIR;
    } else if (!player_read_stick(work, record, NULL, NULL)) {
        keep = TURN_KEEP_STILL;
        if (record->grip_scale < 1.0F) {
            keep *= record->grip_scale;
        }
        rest = 1.0F - keep;
    } else {
        /* @hack spelling: the stick's mix reached by goto past the return,
         * where an else holding it would do */
        goto pushed;
    }
    record->spd.x = (record->spd.x * keep) + (spd.x * rest);
    record->spd.y = (record->spd.y * keep) + (spd.y * rest);
    record->spd.z = (record->spd.z * keep) + (spd.z * rest);
/* @hack redundant: the label end, which nothing jumps to, keeps SHC from
 * sharing the last product and store of the mix above with the mix after
 * pushed */
end: /* NOLINT(clang-diagnostic-unused-label) */
    return;
pushed:
    x = spd.x;
    if (record->level > TURN_LEVEL) {
        keep = TURN_KEEP_LEVEL;
        if (record->grip_scale < 1.0F) {
            keep *= record->grip_scale;
        }
    } else {
        keep = TURN_KEEP_STEEP;
        if (record->grip_scale < 1.0F) {
            keep *= record->grip_scale;
        }
    }
    share = 1.0F - keep;
    x *= share;
    record->spd.x = (record->spd.x * keep) + x;
    record->spd.y = (record->spd.y * keep) + (spd.y * share);
    record->spd.z = (record->spd.z * keep) + (spd.z * share);
}

void player_turn_on_course(struct task_work *work, struct player_motion *motion,
                           struct player *record, Angle heading)
{
    NJS_VECTOR spd;

    spd = record->spd;
    turn_to_world(work, &spd);
    work->ang.y = angle_approach(work->ang.y, heading, COURSE_STEP);
    turn_from_world(work, &spd);
    record->spd = spd;
}

void player_turn_at_speed(struct task_work *work, struct player_motion *motion,
                          struct player *record, Angle heading)
{
    NJS_VECTOR spd;
    Angle step;
    Float keep;
    Float rest;

    spd = record->spd;
    turn_to_world(work, &spd);
    step = SPEED_STEP;
    if (record->spd.x > record->consts.full_speed) {
        step -= njSqrt((record->spd.x - record->consts.full_speed) *
                       SPEED_STEP_SCALE) *
                SPEED_STEP_FALL;
    }
    work->ang.y = angle_approach(work->ang.y, heading, step);
    turn_from_world(work, &spd);
    if (record->level > TURN_LEVEL) {
        keep = TURN_KEEP_LEVEL;
        rest = 1.0F - keep;
    } else {
        keep = TURN_KEEP_STEEP;
        rest = 1.0F - keep;
    }
    record->spd.x = (record->spd.x * keep) + (spd.x * rest);
    record->spd.y = (record->spd.y * keep) + (spd.y * rest);
    record->spd.z = (record->spd.z * keep) + (spd.z * rest);
}

/* player_turn_slowly's step, and a rival's */
#define TURN_SLOW_STEP 0x30
#define TURN_SLOW_STEP_RIVAL 0x38

/* player_turn_at_speed by TURN_SLOW_STEP, or TURN_SLOW_STEP_RIVAL for a
 * WORK_ID_RIVAL work */
static void player_turn_slowly(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, Angle heading)
{
    NJS_VECTOR spd;
    Angle step;
    Float keep;
    Float rest;

    spd = record->spd;
    turn_to_world(work, &spd);
    if (work->id == WORK_ID_RIVAL) {
        step = TURN_SLOW_STEP_RIVAL;
    } else {
        step = TURN_SLOW_STEP;
    }
    work->ang.y = angle_approach(work->ang.y, heading, step);
    turn_from_world(work, &spd);
    if (record->level > TURN_LEVEL) {
        keep = TURN_KEEP_LEVEL;
        rest = 1.0F - keep;
    } else {
        keep = TURN_KEEP_STEEP;
        rest = 1.0F - keep;
    }
    record->spd.x = (record->spd.x * keep) + (spd.x * rest);
    record->spd.y = (record->spd.y * keep) + (spd.y * rest);
    record->spd.z = (record->spd.z * keep) + (spd.z * rest);
}

/* without the stick, a speed ahead below CREEP_SHARE of trot_speed stops on
 * ground whose level is above CREEP_LEVEL. along a path the margin is
 * PATH_CREEP, at any level */
#define CREEP_SHARE 0.87F
#define CREEP_LEVEL 0.7F
#define PATH_CREEP 0.01F
/* deep WORLD_FLAT_THICK water brakes a player from this share of trot_speed
 * up */
#define WATER_BRAKE_SHARE 0.8F

#ifdef NONMATCHING
/* an address in main RAM, read as a float, lies below 0 and within 2^-101 of
 * it. this one stands for an address that the image reads as a float: it has
 * such an address's sign, and lies in the middle of their range */
#define POINTER_AS_FLOAT (-0x1p-102F)
/* what a call before player_collide leaves in the word of the stack that
 * player_collide's normal takes its y from: player_apply_push the speed ahead
 * that it turns into the world's axes, and player_apply_accel a task's
 * address, which is POINTER_AS_FLOAT here. the next call of player_collide
 * takes this as its normal's y, as the image's frame gives it, and clears
 * it */
static Float collide_y;
#endif

void player_apply_accel(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    NJS_VECTOR spd;
    Float old;
    Float creep;

    old = record->spd.x;
    record->spd.x += record->accel.x;
    creep = record->consts.trot_speed * CREEP_SHARE;
    if (old > 0.0F) {
        if (record->spd.x < 0.0F) {
            record->spd.x = 0.0F;
        } else if (!player_read_stick(work, record, NULL, NULL) &&
                   record->spd.x < creep && record->level > CREEP_LEVEL) {
            record->spd.x = 0.0F;
        }
    } else if (old < 0.0F) {
        /* @hack spelling: the speed compared with minus zero, which tests as
         * 0 does */
        if (record->spd.x > -0.0F) {
            record->spd.x = 0.0F;
        } else if (!player_read_stick(work, record, NULL, NULL) &&
                   record->spd.x > -creep && record->level > CREEP_LEVEL) {
            record->spd.x = 0.0F;
        }
    }
    record->spd.y += record->accel.y;
    old = record->spd.z;
    record->spd.z += record->accel.z;
    if (old > 0.0F) {
        if (old < creep && old == record->spd.z) {
            record->spd.z = 0.0F;
        } else if (old >= creep && record->spd.z < creep) {
            record->spd.z = 0.0F;
        }
    } else if (old < 0.0F) {
        if (old > -creep && old == record->spd.z) {
            record->spd.z = 0.0F;
        } else if (old <= -creep && record->spd.z > -creep) {
            record->spd.z = 0.0F;
        }
    }
    if ((record->ground.above_flags & WORLD_FLAT_THICK) &&
        record->ground.above_y > work->pos.y + WATER_BRAKE_DEPTH) {
        if (record->spd.x >= record->consts.trot_speed * WATER_BRAKE_SHARE) {
            record->spd.x += record->consts.brake_drag;
        }
    } else if ((record->ground.above_flags & WORLD_FLAT) &&
               record->ground.above_y > work->pos.y + WATER_BRAKE_DEPTH) {
        if (record->spd.x >=
            (record->consts.running_speed + record->consts.trot_speed) * 0.5F) {
            record->spd.x += record->consts.brake_drag;
        }
    }
    spd = record->spd;
    if ((record->ground_flags & WORLD_UPRIGHT) &&
        (work->mode != PLAYER_MODE_JUMP || motion->spd.y > 0.0F)) {
        ground_turn_to_world(&record->ground_normal, &spd, work->ang.y);
    } else {
        turn_to_world(work, &spd);
    }
    motion->spd = spd;
    player_limit_speed(work, motion, record);
    player_stop_creep(work, motion, record);
#ifdef NONMATCHING
    /* this side gives player_collide the sign of the address that
     * player_stop_creep's call leaves it, as collide_y says */
    collide_y = POINTER_AS_FLOAT;
#endif
}

void player_apply_path_accel(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    NJS_VECTOR spd;
    Float old;
    Float creep;

    old = record->spd.x;
    record->spd.x += record->accel.x;
    creep = PATH_CREEP;
    if (old > 0.0F) {
        if (record->spd.x < 0.0F) {
            record->spd.x = 0.0F;
        } else if (!player_read_stick(work, record, NULL, NULL) &&
                   record->spd.x < creep) {
            record->spd.x = 0.0F;
        }
    } else if (old < 0.0F) {
        /* @hack spelling: the speed compared with minus zero, which tests as
         * 0 does */
        if (record->spd.x > -0.0F) {
            record->spd.x = 0.0F;
        } else if (!player_read_stick(work, record, NULL, NULL) &&
                   record->spd.x > -creep) {
            record->spd.x = 0.0F;
        }
    }
    record->spd.y += record->accel.y;
    old = record->spd.z;
    record->spd.z += record->accel.z;
    if (old > 0.0F) {
        if (old < creep && old == record->spd.z) {
            record->spd.z = 0.0F;
        } else if (old >= creep && record->spd.z < creep) {
            record->spd.z = 0.0F;
        }
    } else if (old < 0.0F) {
        if (old > -creep && old == record->spd.z) {
            record->spd.z = 0.0F;
        } else if (old <= -creep && record->spd.z > -creep) {
            record->spd.z = 0.0F;
        }
    }
    if ((record->ground.above_flags & WORLD_FLAT_THICK) &&
        record->ground.above_y > work->pos.y + WATER_BRAKE_DEPTH) {
        if (record->spd.x >= record->consts.trot_speed * WATER_BRAKE_SHARE) {
            record->spd.x += record->consts.brake_drag;
        }
    } else if ((record->ground.above_flags & WORLD_FLAT) &&
               record->ground.above_y > work->pos.y + WATER_BRAKE_DEPTH) {
        if (record->spd.x >=
            (record->consts.running_speed + record->consts.trot_speed) * 0.5F) {
            record->spd.x += record->consts.brake_drag;
        }
    }
    spd = record->spd;
    turn_to_world(work, &spd);
    motion->spd = spd;
    player_limit_speed(work, motion, record);
    player_stop_creep(work, motion, record);
}

/* hold the motion's spd within the constants' speed_limit along the world's
 * x and z, and fall_limit along its y */
static void player_limit_speed(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    /* @hack redundant: both arms hold the speed alike, where one would do */
    if (work->flags & PLAYER_WORK_HOLDING) {
        if (record->consts.speed_limit < motion->spd.x) {
            motion->spd.x = record->consts.speed_limit;
        }
        if (motion->spd.x < -record->consts.speed_limit) {
            motion->spd.x = -record->consts.speed_limit;
        }
        if (record->consts.speed_limit < motion->spd.z) {
            motion->spd.z = record->consts.speed_limit;
        }
        if (motion->spd.z < -record->consts.speed_limit) {
            motion->spd.z = -record->consts.speed_limit;
        }
        if (record->consts.fall_limit < motion->spd.y) {
            motion->spd.y = record->consts.fall_limit;
        }
        if (motion->spd.y < -record->consts.fall_limit) {
            motion->spd.y = -record->consts.fall_limit;
        }
    } else {
        if (record->consts.speed_limit < motion->spd.x) {
            motion->spd.x = record->consts.speed_limit;
        }
        if (motion->spd.x < -record->consts.speed_limit) {
            motion->spd.x = -record->consts.speed_limit;
        }
        if (record->consts.speed_limit < motion->spd.z) {
            motion->spd.z = record->consts.speed_limit;
        }
        if (motion->spd.z < -record->consts.speed_limit) {
            motion->spd.z = -record->consts.speed_limit;
        }
        if (record->consts.fall_limit < motion->spd.y) {
            motion->spd.y = record->consts.fall_limit;
        }
        if (motion->spd.y < -record->consts.fall_limit) {
            motion->spd.y = -record->consts.fall_limit;
        }
    }
}

/* while the player takes no stick, make 0 each part of the motion's spd
 * less than CREEP_SPEED from 0 */
#define CREEP_SPEED 0.02F

static void player_stop_creep(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    if (!player_read_stick(work, record, NULL, NULL)) {
        if (motion->spd.x < CREEP_SPEED && motion->spd.x > -CREEP_SPEED) {
            motion->spd.x = 0.0F;
        }
        if (motion->spd.y < CREEP_SPEED && motion->spd.y > -CREEP_SPEED) {
            motion->spd.y = 0.0F;
        }
        if (motion->spd.z < CREEP_SPEED && motion->spd.z > -CREEP_SPEED) {
            motion->spd.z = 0.0F;
        }
    }
}

void player_stop(struct player_motion *motion, struct player *record)
{
    if (record != NULL) {
        record->spd.z = 0.0F;
        record->spd.y = 0.0F;
        record->spd.x = 0.0F;
    }
    if (motion != NULL) {
        motion->spd.z = 0.0F;
        motion->spd.y = 0.0F;
        motion->spd.x = 0.0F;
    }
}

/* on STAGE_WILD_CANYON and STAGE_WILD_CANYON_2P, a push moves no faster than
 * this, in place of the constants' push_speed */
#define PUSH_SLOW_SPEED 0.1F
/* the speed below which a push or a pull stops once it has slowed to it */
#define PUSH_STOP_SPEED 0.1F

void player_apply_push(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    struct task *pushed;
    struct task_motion *other;
    NJS_VECTOR spd;
    Float old;

    if (record->held == NULL) {
        pushed = work->collision->hit->owner;
    } else {
        pushed = record->held;
    }
    other = pushed->motion.p;
    if (!(player_action_flags(work, record) & ACTION_FLAG_400)) {
        player_stop(motion, record);
        return;
    }
    old = record->spd.x;
    record->spd.x += record->accel.x;
    if (old > 0.0F) {
        if (record->spd.x < 0.0F) {
            record->spd.x = 0.0F;
        } else if (old >= PUSH_STOP_SPEED && record->spd.x < PUSH_STOP_SPEED) {
            record->spd.x = 0.0F;
        }
    } else if (old < 0.0F) {
        /* @hack spelling: the speed compared with minus zero, which tests as
         * 0 does */
        if (record->spd.x > -0.0F) {
            record->spd.x = 0.0F;
        } else if (old <= -PUSH_STOP_SPEED &&
                   record->spd.x > -PUSH_STOP_SPEED) {
            record->spd.x = 0.0F;
        }
    }
    if (stage_current_id() == STAGE_WILD_CANYON ||
        stage_current_id() == STAGE_WILD_CANYON_2P) {
        if (record->spd.x > PUSH_SLOW_SPEED) {
            record->spd.x = PUSH_SLOW_SPEED;
        }
        if (record->spd.x < -PUSH_SLOW_SPEED) {
            record->spd.x = -PUSH_SLOW_SPEED;
        }
    } else {
        if (record->consts.push_speed < record->spd.x) {
            record->spd.x = record->consts.push_speed;
        }
        if (record->spd.x < -record->consts.push_speed) {
            record->spd.x = -record->consts.push_speed;
        }
    }
    record->spd.y += record->accel.y;
    old = record->spd.z;
    record->spd.z += record->accel.z;
    if (old > 0.0F) {
        if (record->spd.z < 0.0F) {
            record->spd.z = 0.0F;
        } else if (old >= PUSH_STOP_SPEED && record->spd.z < PUSH_STOP_SPEED) {
            record->spd.z = 0.0F;
        }
    } else if (old < 0.0F) {
        /* @hack spelling: the speed compared with minus zero, which tests as
         * 0 does */
        if (record->spd.z > -0.0F) {
            record->spd.z = 0.0F;
        } else if (old <= -PUSH_STOP_SPEED &&
                   record->spd.z > -PUSH_STOP_SPEED) {
            record->spd.z = 0.0F;
        }
    }
    spd = record->spd;
#ifdef NONMATCHING
    /* this side gives player_collide the speed ahead, as collide_y says */
    collide_y = spd.x;
#endif
    turn_to_world(work, &spd);
    motion->spd = spd;
    if (other != NULL) {
        other->acc.x = spd.x;
        other->acc.y = 0.0F;
        other->acc.z = spd.z;
    }
    player_limit_speed(work, motion, record);
}

void player_apply_climb_accel(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    NJS_VECTOR spd;

    record->spd = record->accel;
    spd = record->spd;
    turn_to_world(work, &spd);
    motion->spd = spd;
    player_limit_speed(work, motion, record);
}

/* a glide's speed ahead gains GLIDE_GAIN a step below GLIDE_SLOW, from 0 where
 * it is below 0. it never passes GLIDE_TOP */
#define GLIDE_SLOW 1.5F
#define GLIDE_TOP 3.5F
#define GLIDE_GAIN 0.15F

void player_apply_glide_accel(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    NJS_VECTOR spd;

    record->spd.x += record->accel.x;
    record->spd.y += record->accel.y;
    record->spd.z += record->accel.z;
    if (record->spd.x < GLIDE_SLOW) {
        if (record->spd.x < 0.0F) {
            record->spd.x = 0.0F;
        }
        record->spd.x += GLIDE_GAIN;
    }
    if (record->spd.x > GLIDE_TOP) {
        record->spd.x = GLIDE_TOP;
    }
    spd = record->spd;
    turn_to_world(work, &spd);
    motion->spd = spd;
    player_limit_speed(work, motion, record);
}

#ifdef NONMATCHING
/* this side's stand-in for the word of the stack that the slide below reads
 * its normal's y from, as player.h says */
Float player_slide_y;
#endif

/* turn the motion's spd, at its own length, along the line where the touched
 * wall meets the ground. it runs along the line the way nearer the motion. the
 * player turns by as much. it turns about where that leaves it more than
 * TURN_AWAY from its aim. aim_y then takes the heading, and wall_normal the
 * wall's normal. only a player on the ground slides. it must not be rising, and
 * its motion must run at 1 or more. its record must have been pushed at a side
 * while its work lacks WORK_HURT */
static void player_slide_along_wall(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record)
{
    NJS_VECTOR normal;
    NJS_PLANE wall;
    NJS_PLANE ground;
    NJS_LINE line;
    NJS_VECTOR before;
    NJS_VECTOR after;
    Float speed;
    int flags;

    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        return;
    }
    if (njScalor(&motion->spd) < 1.0F) {
        return;
    }
    if (record->spd.y > 0.0F) {
        return;
    }
    flags = work->collision->flags;
    if (!(flags & COLLISION_PUSHED) || (work->flags & WORK_HURT) ||
        !(flags & COLLISION_SIDE)) {
        return;
    }
    /* @bug uninitialized: collision_get_normal sets the normal's x and z
     * alone, and the length and the unit vector then read its y, which no
     * path sets (0x8c03f99e) */
#ifdef NONMATCHING
    /* this side keeps the tag's result where the image's frames give it, and
     * repairs the tag elsewhere with a y of 0, a normal that lies level. the
     * image's y is a word of the stack below the frame of player_collide,
     * the one caller, which player_slide_y stands for. where a box is met at
     * running speed, no call of player_collide's reaches that word, so it
     * holds what the step's calls before player_collide left. in a mech's
     * step those calls put five of their locals there, and each site that
     * sets player_slide_y names its own. Prison Lane's attract demo meets
     * the first: a strike test's y of 16.6 stands the normal all but
     * upright, and the mech stops against a gate that a level normal slides
     * it along. this side plays that demo as the image does, as recorded
     * and with its input varied, through three of the five. the frames
     * alone give the two whose sites say so. a call between a site and the
     * slide that runs over the word isn't followed past those runs, and no
     * other character's step is read: its slide lies level */
    normal.y = player_slide_y;
#endif
    collision_get_normal(work, &normal);
    if (njScalor(&normal) == 0.0F) {
        normal.x = 0.0F;
        normal.y = 0.0F;
        normal.z = 0.0F;
    } else {
        njUnitVector(&normal);
        record->wall_normal = normal;
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
        before = motion->spd;
        if (njScalor(&before) != 0.0F) {
            njUnitVector(&before);
        }
        if (njInnerProduct((NJS_VECTOR *)&line.vx, &before) < 0.0F) {
            line.vx = -line.vx;
            line.vy = -line.vy;
            line.vz = -line.vz;
        }
        speed = njScalor(&motion->spd);
        before = motion->spd;
        motion->spd.x = speed * line.vx;
        motion->spd.y = speed * line.vy;
        motion->spd.z = speed * line.vz;
        after = motion->spd;
        njUnitVector(&before);
        njUnitVector(&after);
        work->ang.y -= vector_angle(&before, &after, &record->ground_normal);
        if (angle_distance(motion->aim_y, work->ang.y) > TURN_AWAY) {
            work->ang.y += HALF_TURN;
        }
        motion->aim_y = work->ang.y;
    }
}

/* a player at this spd.x crashes into any shape whose id isn't
 * SHAPE_ID_NO_CRASH, head on or not */
#define CRASH_ANY_SPEED 15.0F
/* the inner product of the motion's way and a shape's normal below which a
 * player at crash_speed meets the shape head on, and crashes */
#define CRASH_HEAD_ON (-0.98F)
/* how far a WORK_ID_STOP_RUN task's ang.y may lie from the player's heading for
 * the task to stop it */
#define STOP_RUN_TURN 0x3000

int player_collide(struct task_work *work, struct player_motion *motion,
                   struct player *record)
{
    NJS_VECTOR dir;
    NJS_VECTOR normal;
    struct collision_shape *own;
    struct collision_work *touched;
    struct collision_work *collision;
    struct collision_shape *shape;
    struct collision_work *hit;
    struct task_work *other;
    struct task_work
        *pusher; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float x;
    Float z;
    Float facing;
    Float reach;
    Float across;
    Angle turn;
    Angle from;
    int result;
    int ret;

    result = 0;
    collision = work->collision;
    record->wall_normal.x = record->wall_normal.y = record->wall_normal.z =
        0.0F;
    record->last_ground_flags = record->ground_flags;
    record->ground_flags = 0;
    if (player_last_side_pusher(work, record)) {
        own = collision->shapes;
        touched = player_last_side_pusher(work, record)->work->collision;
        /* @hack redundant: the pusher's work, which nothing reads */
        pusher = touched->owner->work;
        shape = &touched->shapes[touched->hit_shape];
        dir = motion->spd;
        /* @bug uninitialized: collision_get_normal sets the normal's x and
         * z alone, and the length, the unit vector and wall_normal then take
         * its y, which no path sets (0x8c03fbda) */
#ifdef NONMATCHING
        /* this side keeps the tag's result on the calls traced. the image's
         * y is a word of the stack that the call before this one leaves,
         * which collide_y stands for. after a player_apply_push that turns
         * its speed, it is the speed ahead that turn_to_world copied there.
         * after player_apply_accel, it is R10 as player_stop_creep's call of
         * player_read_stick pushed it: the stepping task's next, since no
         * step in 1ST_READ that calls player_collide uses R10. an address's
         * bits lie within 2^-101 of 0 as a float, so the normal's length is
         * as with 0, but they lie below 0. where the motion has no part
         * along the normal's x and z, that sign decides the facing below.
         * City Escape's attract demo meets it at its step 2428: with a y of
         * 0 there, its player lies up to 0.051 from the image's path for 174
         * steps. player_apply_accel comes just before 179 of the 204 calls
         * in 1ST_READ's sources, and player_apply_push before 16. the other
         * nine, in sonic.c and knuckles.c, aren't traced. they take 0, or
         * POINTER_AS_FLOAT where player_apply_accel ran since the last
         * player_collide: three of sonic.c's functions call it and return
         * to the step, and what they leave in the word isn't read. nor is
         * what super.c's step keeps in R10 before its ten */
        normal.y = collide_y;
#endif
        collision_get_normal(work, &normal);
        if (njScalor(&normal) == 0.0F) {
            normal.x = 0.0F;
            normal.y = 0.0F;
            normal.z = 0.0F;
        } else {
            njUnitVector(&normal);
        }
        record->wall_normal = normal;
        if (njScalor(&dir) == 0.0F) {
            dir.x = 0.0F;
            dir.y = 0.0F;
            dir.z = 0.0F;
        } else {
            njUnitVector(&dir);
        }
        facing = njInnerProduct(&dir, &normal);
        /* @hack type: the shape's id read as a Uint8 in this test and the
         * next, where the Sint8 it is would do */
        if (record->spd.x >= CRASH_ANY_SPEED &&
            (Uint8)shape->id != SHAPE_ID_NO_CRASH) {
            result = PLAYER_CRASHED;
            player_stop(motion, record);
        } else if (record->spd.x >= record->consts.crash_speed &&
                   facing < CRASH_HEAD_ON &&
                   (Uint8)shape->id != SHAPE_ID_NO_CRASH) {
            result = PLAYER_CRASHED;
            motion->spd.x = motion->spd.z = 0.0F;
        } else if (record->spd.x >= record->consts.running_speed &&
                   facing < 0.0F) {
            result = PLAYER_BUMPED;
            if ((shape->form == SHAPE_SPHERE ||
                 shape->form == SHAPE_CYLINDER) &&
                shape->radius > own->radius &&
                touched->kind == COLLISION_OBJECT &&
                (shape->levels & SHAPE_ATTACK) <=
                    ((own->levels & SHAPE_GUARD) >> 2)) {
                reach = own->radius + shape->radius;
                x = dir.x;
                z = dir.z;
                across = njSqrt((x * x) + (z * z));
                turn = (Angle)(acosf(across / reach) * ANGLE_PER_RADIAN);
                from = (Angle)(atan2f(normal.z, normal.x) * ANGLE_PER_RADIAN) +
                       HALF_TURN;
                if (angle_difference(from, (Angle)(atan2f(dir.z, dir.x) *
                                                   ANGLE_PER_RADIAN)) < 0) {
                    turn = 0x10000 - turn;
                }
                matrix_push_unit();
                njRotateY(NULL, -turn);
                dir = motion->spd;
                njCalcVector(NULL, &dir, &normal);
                njPopMatrixEx();
                work->ang.y += turn;
                motion->spd.x = normal.x;
                motion->spd.z = normal.z;
            } else if (shape->form == SHAPE_BOX || shape->form == SHAPE_BOX2 ||
                       shape->form == SHAPE_AXIS_BOX) {
                if (collision != NULL && collision->hit_count > 0 &&
                    !(work->flags & WORK_HURT) &&
                    (collision->flags & SIDE_PUSHED) == SIDE_PUSHED &&
                    (!(collision->hit->shapes[collision->hit_shape].flags &
                       SHAPE_UNK_2000) ||
                     (work->flags & PLAYER_WORK_HOLDING))) {
                    motion->spd.x = motion->spd.z = 0.0F;
                } else {
                    player_slide_along_wall(work, motion, record);
                }
            }
        } else {
            result = PLAYER_BUMPED;
            if (facing > 0.0F) {
                if (player_action_flags(work, record) & ACTION_FLAG_400) {
                    motion->spd.x = -(record->spd.x * normal.x);
                    motion->spd.z = -(record->spd.x * normal.z);
                } else if (collision != NULL && collision->hit_count > 0 &&
                           !(work->flags & WORK_HURT) &&
                           (collision->flags & SIDE_PUSHED) == SIDE_PUSHED &&
                           (!(collision->hit->shapes[collision->hit_shape]
                                  .flags &
                              SHAPE_UNK_2000) ||
                            (work->flags & PLAYER_WORK_HOLDING))) {
                    dir.x += normal.x;
                    dir.y = 0.0F;
                    dir.z += normal.z;
                    if (njScalor(&dir) != 0.0F) {
                        njUnitVector(&dir);
                        motion->spd.x = record->spd.x * dir.x;
                        motion->spd.z = record->spd.x * dir.z;
                    } else {
                        motion->spd.z = motion->spd.x = 0.0F;
                    }
                } else {
#ifdef NONMATCHING
                    /* this side repairs this slide's read: on the path
                     * traced, player_action_flags' call of
                     * player_offer_flags has pushed its return address into
                     * the word that player_slide_y stands for. the address
                     * lies within 2^-101 of 0 as a float, and turns the
                     * slide as 0 does */
                    player_slide_y = 0.0F;
#endif
                    player_slide_along_wall(work, motion, record);
                }
            }
        }
    } else if (collision->flags & COLLISION_TOUCHED) {
        hit = collision->hit;
        if (hit != NULL && hit->owner != NULL) {
            /* @hack spelling: other set inside its test, where a statement
             * before it would do */
            if ((other = hit->owner->work) != NULL &&
                other->id == WORK_ID_STOP_RUN) {
                if ((work->flags & PLAYER_WORK_ON_GROUND) &&
                    !(record->spd.y > 0.0F) &&
                    (record->spd.x >= record->consts.running_speed ||
                     (work->flags & PLAYER_WORK_BALL))) {
                    if (angle_distance(QUARTER_TURN - work->ang.y,
                                       other->ang.y) < STOP_RUN_TURN) {
                        result = PLAYER_RUN_STOPPED;
                        player_stop(motion, record);
                    }
                }
            }
        }
        /* @hack redundant: a store that nothing reads, which SHC drops */
        ret = (collision->flags & COLLISION_TOUCHED) &&
              (work->flags & PLAYER_WORK_ON_GROUND);
    }
    if (work->flags & PLAYER_WORK_ON_SHAPE) {
        ret = player_collide_ground(work, motion, record);
        if (result == 0) {
            result = ret;
        }
        if (!(work->flags & PLAYER_WORK_ON_GROUND)) {
            matrix_push_unit();
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            dir.z = 0.0F;
            dir.x = 0.0F;
            dir.y = 1.0F;
            njCalcVector(NULL, &dir, &normal);
            record->ground_normal = normal;
            dir.z = 0.0F;
            dir.x = 0.0F;
            dir.y = -0.5F;
            njCalcVector(NULL, &dir, &normal);
            njPopMatrixEx();
            work->pos.y += normal.y;
            if (record->spd.y <= 0.0F) {
                normal = motion->spd;
                turn_from_world(work, &normal);
                normal.y = 0.0F;
                record->spd = normal;
                turn_to_world(work, &normal);
                motion->spd = normal;
            } else {
                work->flags &= ~PLAYER_WORK_ON_SHAPE;
            }
        }
    } else {
        ret = player_collide_ground(work, motion, record);
        if (result == 0) {
            result = ret;
        }
    }
#ifdef NONMATCHING
    /* this side: what a call leaves reaches only the player_collide that
     * follows it */
    collide_y = 0.0F;
#endif
    return result;
}

/* the collision with the world: player_collide_unmasked's in
 * PLAYER_MODE_SWIM or under PLAYER_WORK_BALL, and
 * player_collide_world's otherwise */
static int player_collide_ground(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    int flags = work->flags;

    /* @hack spelling: a test of the flags that always holds, since it ors in
     * PLAYER_WORK_ON_SHAPE, which the image keeps */
    if (((flags & PLAYER_WORK_ON_GROUND) | PLAYER_WORK_ON_SHAPE) &&
        ((flags & PLAYER_WORK_BALL) || work->mode == PLAYER_MODE_SWIM)) {
        return player_collide_unmasked(work, motion, record);
    }
    return player_collide_world(work, motion, record);
}

/* TRUE once player_collide_world has listed a WORLD_UPRIGHT object this step.
 * player_step_up then turns its probes by the surface's normal in place of
 * the work's angles */
static Bool upright_ground = FALSE;
/* TRUE once a test of a sweep's walls has met two walls or more at once this
 * step. no player ON_BOARD sets it. player_collide_world, player_hit_wall and
 * unmasked_push_off_wall then take away the ball's speed along the surface
 * that places the player */
static Bool between_walls = FALSE;

/* the room of player_collide_world's lists of objects, which hold
 * WORLD_LISTED - 1 at most */
#define WORLD_LISTED 32
/* the bits of mask, the tests of a surface that player_collide_world's stand
 * test reads. MASK_NEAR, MASK_AHEAD and MASK_INTO start set. each clears where
 * its test fails:
 * - MASK_NEAR, once an earlier object has stood the player this step. it
 *   clears where another object's touched point lies more than the player's
 *   height from it, or behind the surface below NEAR_FACING;
 * - MASK_AHEAD, for a player on a shape or stood this step, off a path. it
 *   clears where the point lies behind the way the player runs, below
 *   AHEAD_FACING;
 * - MASK_INTO, for a player that began the step on the ground, off a path and
 *   off WORLD_UPRIGHT ground. it clears where the motion runs away from the
 *   point.
 *
 * MASK_NO_STEP is set where player_step_up need not lift the ball onto the
 * surface. MASK_FLOOR is set on a surface too steep to stand on that faces up
 * against world_gravity. that surface's turn is then measured from the way up,
 * against world_gravity, not from the player's up */
#define MASK_NEAR 0x1
#define MASK_NO_STEP 0x2
#define MASK_AHEAD 0x4
#define MASK_INTO 0x8
#define MASK_FLOOR 0x10
#define NEAR_FACING (-0.2F)
#define AHEAD_FACING (-0.4F)
/* the most a surface's normal may turn from the player's up for the player
 * to stand on it at any heading */
#define STAND_SLOPE_TURN 0x2c00
/* TRUE where a surface is ground to a ball. its normal lies turn from the
 * ball's up. it is ground where turn is within STAND_SLOPE_TURN, or where the
 * normal points further against world_gravity than the up does */
#define IS_GROUND(turn, up, normal)                                            \
    ((turn) <= STAND_SLOPE_TURN ||                                             \
     njInnerProduct(&world_gravity, (up)) >                                    \
         njInnerProduct(&world_gravity, (normal)))
/* a player that a surface stands keeps PLAYER_WORK_ON_GROUND where its spd.x
 * is STICK_SPEED or more, ahead or back. it keeps it too where its up meets
 * world_gravity below STICK_TILT, and on a WORLD_UPRIGHT object.
 * STICK_SPEED_TILTED bounds an arm that no player reaches, as its tag says */
#define STICK_SPEED 1.85F
#define STICK_SPEED_TILTED 1.16F
#define STICK_TILT (-0.34F)
/* a surface the stand test passes is still a wall to a player at running_speed
 * or more. its unit motion must meet the surface's normal past this */
#define WALL_ALONG_MOTION 0.52F
/* a surface pitched within FLIP_PITCH_RANGE of FLIP_PITCH is upside down
 * where it is rolled more than FLIP_ROLL from level. the player then flips its
 * tilt and turns about */
#define FLIP_PITCH 0xc800
#define FLIP_PITCH_RANGE 0x1000
#define FLIP_ROLL 0x7000
/* the most hit speed the ball keeps in a block that no path reaches */
#define HIT_SPEED_MOST 2.1F
/* a player that no surface stands may step back STAY_BACK of its motion, which
 * stops. it must have begun the step under PLAYER_WORK_ON_GROUND, and take no
 * stick or a stroke of STAY_STROKE or less. its level must be above STAY_LEVEL
 */
#define STAY_BACK 0.5F
#define STAY_STROKE 0.9F
#define STAY_LEVEL 0.7F

/* a polygon_ball as player_collide_world's slope test reads it: hit.point
 * and hit.speed at their places in the ball */
struct ball_hit_view {
    Uint8 unk_00[0x54];
    NJS_POINT3 point;
    Uint8 unk_60[0xc];
    NJS_VECTOR speed;
};

/* move the player against the world's surfaces, and return player_collide's
 * result of the walls the ball meets, or 0. the work loses
 * PLAYER_WORK_ON_GROUND, and last_flags takes the work's flags as they were. a
 * player began the step on the ground where last_flags has a bit of
 * PLAYER_WORK_GROUNDED. an object has stood it this step once the work has
 * PLAYER_WORK_ON_GROUND again.
 *
 * the player's ball, of half its height, sweeps from its middle, dropped by
 * ball_drop, along the motion's spd and the record's push. every object near it
 * is listed, up to WORLD_LISTED - 1. no WORLD_FLAT object is listed, nor in the
 * air a GROUND_NO_LANDING one, nor a task's whose work lacks WORLD_ADDED. each
 * that the sweep touches takes ball_stop_at_walls's test for a player that
 * began the step on the ground, and ball_stop_at_ground's for any other. the
 * result starts as the greatest that ball_stop_at_walls gives. with no object
 * listed, the player moves by the motion and the push. it then loses its
 * ground_task and ground_object.
 *
 * the ball then meets each listed object in turn. a surface that the stand test
 * passes stands the player, but a rising one that began the step on the ground.
 * ball_slide_along_walls first slides the player along the walls it runs into
 * there. its result, which can be whatever R0 held, replaces a result of 0, and
 * any result with PLAYER_CRASHED. the player then tilts toward the surface, or
 * levels on a WORLD_UPRIGHT object. it takes the surface's normal for
 * ground_normal, moves onto it, and rides its task by player_ride_task. it
 * keeps PLAYER_WORK_ON_GROUND as STICK_SPEED says, or else slips off where it
 * moves ahead. player_hit_wall meets every other surface as a wall. it meets a
 * surface WALL_ALONG_MOTION makes one as a wall too.
 *
 * where no surface stood the player, it steps back as STAY_BACK says. it
 * doesn't on GROUND_SLIPPERY ground, under PLAYER_WORK_NO_CONTROL, with
 * stick_lock, rising, or past running_speed. any other player moves by the
 * ball's speed */
static int player_collide_world(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record)
{
    NJS_VECTOR offset;
    NJS_VECTOR normal;
    NJS_VECTOR position;
    NJS_POINT3 old_pos;
    struct world_object candidates[WORLD_LISTED];
    struct world_object sorted[WORLD_LISTED];
    struct polygon_ball *ball;
    struct world_object *world;
    struct world_object *entry;
    struct world_object *list;
    struct polygon_contact *contact;
    struct task *owner;
    NJS_OBJECT *object;
    int old_flags;
    int i;
    int j;
    int placed;
    int count;
    int saved;
    Bool hit;
    Bool grounded;
    int result;
    int found;
    Angle angle;
    int mask;
    Angle pitch;
    Angle roll;
    Angle pitch_distance; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Angle roll_distance;
    Float length;
    Float dot;
    Float magnitude;
    Float excess;
    Float gap;
    Float strength;

    ball = record->ball;
    result = 0;
    upright_ground = FALSE;
    between_walls = FALSE;
    old_flags = record->last_flags;
    record->last_flags = work->flags;
    work->flags &= ~PLAYER_WORK_ON_GROUND;
    ball->up.z = 0.0F;
    ball->up.x = 0.0F;
    ball->up.y = 1.0F;
    ball->hit.ang_x = work->ang.x;
    ball->hit.ang_z = work->ang.z;
    offset.x = offset.z = 0.0F;
    offset.y = record->consts.height * 0.5F;
    normal = offset;
    if (record->last_ground_flags & WORLD_UPRIGHT) {
        ground_turn_to_world(&record->ground_normal, &ball->up, work->ang.y);
        ground_turn_to_world(&record->ground_normal, &offset, work->ang.y);
        ball_drop(motion, record, ball, &normal);
        ground_turn_to_world(&record->ground_normal, &normal, work->ang.y);
    } else {
        turn_to_world(work, &ball->up);
        turn_to_world(work, &offset);
        ball_drop(motion, record, ball, &normal);
        turn_to_world(work, &normal);
    }
    ball->pos.x = work->pos.x + normal.x;
    ball->pos.y = work->pos.y + normal.y;
    ball->pos.z = work->pos.z + normal.z;
    ball->radius = record->consts.height * 0.5F;
    ball->speed = motion->spd;
    njAddVector(&ball->speed, &record->push);
    old_pos = work->pos;
    polygon_collect_near(ball);
    if (world_count == 0) {
        njAddVector(&work->pos, &record->push);
        njAddVector(&work->pos, &motion->spd);
        player_set_ground(work, motion, record, 0);
        record->ground_task = NULL;
        record->ground_object = NULL;
        return result;
    }
    grounded = FALSE;
    hit = FALSE;
    world = world_objects;
    object = NULL;
    saved = 0;
    count = 0;
    /* @hack spelling: i counted by i = i + 1, where i++ would do. each test
     * of an object holds the rest in its if, where a continue would do */
    for (i = 0; i < world_count; i = i + 1, world++) {
        if (!(world->flags & WORLD_FLAT)) {
            if ((record->last_flags & PLAYER_WORK_GROUNDED) ||
                !(world->flags & GROUND_NO_LANDING)) {
                if (world->owner != NULL &&
                    !(world->owner->work->flags & WORLD_ADDED)) {
                    continue;
                }
                if (polygon_sweep(ball, world->object, world->flags, 0)) {
                    if (record->last_flags & PLAYER_WORK_GROUNDED) {
                        if ((found = ball_stop_at_walls(work, motion, record,
                                                        ball, world)) != 0) {
                            saved = count;
                            hit = TRUE;
                            if (result < found) {
                                result = found;
                            }
                        }
                    } else if (ball_stop_at_ground(work, motion, record, ball,
                                                   world) == TRUE) {
                        hit = TRUE;
                        saved = count;
                    }
                }
                candidates[count] = *world;
                count++;
                if (world->flags & WORLD_UPRIGHT) {
                    upright_ground = TRUE;
                }
                if (count >= WORLD_LISTED - 1) {
                    break;
                }
            }
        }
    }
    /* @hack redundant: a store that nothing reads, which SHC drops */
    angle = hit ? work->ang.x : work->ang.z;
    if (count == 0) {
        njAddVector(&work->pos, &record->push);
        njAddVector(&work->pos, &motion->spd);
        player_set_ground(work, motion, record, 0);
        record->ground_task = NULL;
        record->ground_object = NULL;
        return result;
    }
    /* the order the ball meets the objects in. the one ball_stop_at_walls or
     * ball_stop_at_ground took comes first. the rest follow in the order listed
     * while the player rides a task, or else in reverse. the object the player
     * stood on the step before goes last */
    placed = 0;
    if (hit == TRUE) {
        sorted[placed] = candidates[saved];
        placed++;
    }
    if (record->ground_task != NULL) {
        for (j = 0; j < count; j++) {
            if (hit == TRUE && j == saved) {
                hit = FALSE;
            } else {
                entry = &candidates[j];
                if (record->ground_object == entry->object) {
                    sorted[count - 1] = *entry;
                } else {
                    sorted[placed] = *entry;
                    placed++;
                }
            }
        }
    } else {
        for (j = count - 1; j >= 0; j--) {
            if (hit == TRUE && j == saved) {
                hit = FALSE;
            } else {
                entry = &candidates[j];
                if (record->ground_object == entry->object) {
                    sorted[count - 1] = *entry;
                } else {
                    sorted[placed] = *entry;
                    placed++;
                }
            }
        }
    }
    ball->pos.x = work->pos.x + normal.x;
    ball->pos.y = work->pos.y + normal.y;
    ball->pos.z = work->pos.z + normal.z;
    ball->speed = motion->spd;
    njAddVector(&ball->speed, &record->push);
    record->ground_task = NULL;
    list = sorted;
    for (i = 0; i < count; i++) {
        entry = &list[i];
        owner = entry->owner;
        if (!polygon_sweep(ball, entry->object, entry->flags, 0)) {
            continue;
        }
        mask = MASK_NEAR;
        if ((work->flags & PLAYER_WORK_ON_GROUND) && object != entry->object) {
            if (upright_ground == TRUE) {
                position.x =
                    ball->hit.point.x + ball->hit.speed.x - work->pos.x;
                position.y =
                    ball->hit.point.y + ball->hit.speed.y - work->pos.y;
                position.z =
                    ball->hit.point.z + ball->hit.speed.z - work->pos.z;
            } else {
                position.x = ball->hit.pos.x + ball->hit.speed.x - offset.x -
                             work->pos.x;
                position.y = ball->hit.pos.y + ball->hit.speed.y - offset.y -
                             work->pos.y;
                position.z = ball->hit.pos.z + ball->hit.speed.z - offset.z -
                             work->pos.z;
            }
            length = njScalor(&position);
            if (length != 0.0F) {
                njUnitVector(&position);
                if ((entry->flags & WORLD_UPRIGHT) && position.y > 0.0F) {
                    /* @bug logic: dot takes an angle here, never below 0,
                     * where the other arm gives the inner product that the
                     * test of -0.2 below reads (0x8c040866) */
                    dot = (Angle)(acosf(position.y) * ANGLE_PER_RADIAN);
                } else {
                    dot = njInnerProduct(&position, &ball->hit.normal);
                }
                if (dot < NEAR_FACING || length > record->consts.height) {
                    mask = 0;
                }
            }
        }
        mask |= MASK_AHEAD;
        /* @hack spelling: a loop of one pass that its tests leave by break,
         * where an if around the rest would do */
        do {
            NJS_VECTOR push;
            NJS_VECTOR move;

            if (!(work->flags & PLAYER_WORK_GROUNDED) ||
                (work->flags & PLAYER_WORK_ON_PATH)) {
                break;
            }
            push.x = ball->hit.point.x + ball->hit.speed.x - ball->pos.x;
            if (upright_ground) {
                push.y = 0.0F;
            } else {
                push.y = ball->hit.point.y + ball->hit.speed.y - ball->pos.y;
            }
            push.z = ball->hit.point.z + ball->hit.speed.z - ball->pos.z;
            if (njScalor(&push) != 0.0F) {
                njUnitVector(&push);
                if (record->spd.x >= 0.0F) {
                    move.x = 1.0F;
                } else {
                    move.x = -1.0F;
                }
                move.y = 0.0F;
                move.z = 0.0F;
                turn_to_world(work, &move);
                if (upright_ground && _builtin_fabsf(move.y) == 1.0F) {
                    break;
                }
                njUnitVector(&move);
                if (njInnerProduct(&push, &move) < AHEAD_FACING) {
                    mask &= ~MASK_AHEAD;
                }
            }
        } while (0);
        mask |= MASK_INTO;
        /* @hack spelling: a loop of one pass that its test leaves by break,
         * where an if around the rest would do */
        do {
            NJS_VECTOR push;
            NJS_VECTOR move;

            if (!(record->last_flags & PLAYER_WORK_GROUNDED) ||
                (work->flags & PLAYER_WORK_ON_PATH) ||
                (record->last_ground_flags & WORLD_UPRIGHT)) {
                break;
            }
            move = motion->spd;
            if (njScalor(&move) != 0.0F) {
                njUnitVector(&move);
                push.x = ball->hit.point.x + ball->hit.speed.x - ball->pos.x +
                         normal.x;
                push.y = ball->hit.point.y + ball->hit.speed.y - ball->pos.y +
                         normal.y;
                push.z = ball->hit.point.z + ball->hit.speed.z - ball->pos.z +
                         normal.z;
                if ((length = njScalor(&push)) != 0.0F && length > 1.0F) {
                    njUnitVector(&push);
                    if (njInnerProduct(&push, &move) < 0.0F) {
                        mask &= ~MASK_INTO;
                    }
                }
            }
        } while (0);
        angle = vector_angle(&ball->up, &ball->hit.normal, NULL);
        if (angle > STAND_SLOPE_TURN) {
            NJS_VECTOR push;

            if (njInnerProduct(&world_gravity, &ball->hit.normal) < 0.0F) {
                push.x = -(world_gravity.x * 1.0F);
                push.y = -(world_gravity.y * 1.0F);
                push.z = -(world_gravity.z * 1.0F);
                angle = vector_angle(&push, &ball->hit.normal, NULL);
                mask |= MASK_NO_STEP;
                mask |= MASK_FLOOR;
            } else {
                player_step_up(record->slot, work, ball, entry);
                angle = vector_angle(&ball->up, &ball->hit.normal, NULL);
                mask |= MASK_NO_STEP;
            }
        } else {
            NJS_VECTOR push;
            NJS_VECTOR move;

            /* @hack type: the hit's speed and point read through
             * ball_hit_view, where ball->hit's members would do */
            move = ((struct ball_hit_view *)ball)->speed;
            push = ((struct ball_hit_view *)ball)->point;
            njSubVector(&push, &work->pos);
            if (njScalor(&move) <= 0.0F ||
                ((record->last_flags & PLAYER_WORK_GROUNDED) &&
                 njInnerProduct(&push, &move) > 0.0F)) {
                mask |= MASK_NO_STEP;
            }
        }
        if (!(work->flags & PLAYER_WORK_ON_SHAPE) &&
            !(entry->flags & STAND_REFUSES) &&
            ((angle <= STAND_SLOPE_TURN &&
              (player_read_stick(work, record, NULL, NULL) ||
               njInnerProduct(&world_gravity, &ball->hit.normal) < 0.0F ||
               (record->last_flags & PLAYER_WORK_GROUNDED)) &&
              (mask & (MASK_NEAR | MASK_AHEAD)) == (MASK_NEAR | MASK_AHEAD)) ||
             njInnerProduct(&world_gravity, &ball->up) >
                 njInnerProduct(&world_gravity, &ball->hit.normal) ||
             ((entry->flags & WORLD_UPRIGHT) &&
              (mask & (MASK_NEAR | MASK_AHEAD)) == (MASK_NEAR | MASK_AHEAD))) &&
            (!(old_flags & PLAYER_WORK_GROUNDED) ||
             (record->last_flags & PLAYER_WORK_GROUNDED)) &&
            (mask & MASK_INTO) &&
            (njInnerProduct(&world_gravity, &ball->hit.normal) <= 0.0F ||
             (record->last_flags & PLAYER_WORK_GROUNDED)) &&
            ((mask & MASK_NO_STEP) ||
             player_step_up(record->slot, work, ball, entry))) {
            if (njScalor(&motion->spd) >= record->consts.running_speed &&
                ((work->mode != PLAYER_MODE_JUMP &&
                  !(entry->flags & WORLD_UPRIGHT)) ||
                 (work->mode == PLAYER_MODE_JUMP &&
                  (entry->flags & WORLD_UPRIGHT)))) {
                position = motion->spd;
                njUnitVector(&position);
                if (njInnerProduct(&position, &ball->hit.normal) >
                        WALL_ALONG_MOTION &&
                    !(mask & MASK_FLOOR)) {
                    result = player_hit_wall(work, &motion->spd, record, ball,
                                             &old_pos, &offset, &normal, result,
                                             entry->flags);
                    continue;
                }
            }
            if (record->spd.y <= 0.0F ||
                (record->spd.y > 0.0F &&
                 !(record->last_flags & PLAYER_WORK_GROUNDED)) ||
                (mask & MASK_FLOOR)) {
                NJS_PLANE plane;
                NJS_POINT3 point;
                struct world_object swap;

                grounded = TRUE;
                object = entry->object;
                found = ball_slide_along_walls(work, motion, record, ball,
                                               entry->flags);
                if (found) {
                    if (!result) {
                        result = found;
                    } else if (found == PLAYER_CRASHED) {
                        result = found;
                    }
                }
                if (entry->flags & WORLD_UPRIGHT) {
                    work->ang.x = work->ang.z = 0;
                } else if (entry->flags & GROUND_SLIPPERY) {
                    if (motion->spd.x != 0.0F || motion->spd.z != 0.0F ||
                        !(record->last_flags & PLAYER_WORK_ON_GROUND) ||
                        njInnerProduct(&world_gravity, &ball->hit.normal) <
                            njInnerProduct(&world_gravity, &ball->up)) {
                        player_tilt_to_ground(work, record, ball, entry->flags);
                    }
                } else if (mask & MASK_FLOOR) {
                    if (record->last_flags & PLAYER_WORK_ON_GROUND) {
                        if (angle_distance(work->ang.z, ball->hit.ang_z) >
                            QUARTER_TURN) {
                            motion->aim_y = work->ang.y =
                                angle_difference(HALF_TURN, work->ang.y);
                        }
                    }
                    work->ang.x = ball->hit.ang_x;
                    work->ang.z = ball->hit.ang_z;
                } else if (motion->spd.x != 0.0F || motion->spd.z != 0.0F ||
                           !(record->last_flags & PLAYER_WORK_ON_GROUND) ||
                           njInnerProduct(&world_gravity, &ball->hit.normal) <
                               njInnerProduct(&world_gravity, &ball->up)) {
                    pitch = ball->hit.ang_x;
                    roll = ball->hit.ang_z;
                    /* @hack redundant: pitch's distance, which nothing
                     * reads, where no call would do */
                    pitch_distance = angle_distance(work->ang.x, pitch);
                    roll_distance = angle_distance(work->ang.z, roll);
                    if (roll_distance > QUARTER_TURN) {
                        ball->hit.ang_z = roll - HALF_TURN;
                    }
                    if (ball->hit.normal.y <= 0.0F &&
                        angle_distance(ball->hit.ang_z, 0) < QUARTER_TURN) {
                        ball->hit.ang_x = HALF_TURN - pitch;
                    }
                    if (angle_distance(FLIP_PITCH, ball->hit.ang_x) <
                            FLIP_PITCH_RANGE &&
                        angle_distance(0, ball->hit.ang_z) > FLIP_ROLL) {
                        ball->hit.ang_x = -ball->hit.ang_x - HALF_TURN;
                        ball->hit.ang_z = -ball->hit.ang_z + HALF_TURN;
                        motion->aim_y = work->ang.y =
                            angle_difference(HALF_TURN, work->ang.y);
                    }
                    player_tilt_to_ground(work, record, ball, entry->flags);
                    /* @hack redundant: a store that nothing reads, which SHC
                     * drops */
                    angle = pitch ? ball->hit.ang_x : ball->hit.ang_z;
                }
                if (!result) {
                    ball->up.x = ball->up.z = 0.0F;
                    ball->up.y = 1.0F;
                    ball->hit.ang_x = work->ang.x;
                    ball->hit.ang_z = work->ang.z;
                    if (entry->flags & WORLD_UPRIGHT) {
                        ground_turn_to_world(&ball->hit.normal, &ball->up,
                                             work->ang.y);
                    } else {
                        turn_to_world(work, &ball->up);
                    }
                    offset.x = offset.z = 0.0F;
                    offset.y = record->consts.height * 0.5F;
                    normal = offset;
                    if (entry->flags & WORLD_UPRIGHT) {
                        ground_turn_to_world(&ball->hit.normal, &offset,
                                             work->ang.y);
                    } else {
                        turn_to_world(work, &offset);
                    }
                    ball_drop(motion, record, ball, &normal);
                    if (entry->flags & WORLD_UPRIGHT) {
                        ground_turn_to_world(&ball->hit.normal, &normal,
                                             work->ang.y);
                    } else {
                        turn_to_world(work, &normal);
                    }
                }
                motion->aim_x = ball->hit.ang_x;
                motion->aim_z = ball->hit.ang_z;
                record->ground_normal = ball->hit.normal;
                if (entry->flags & WORLD_UPRIGHT) {
                    position = ball->hit.speed;
                    if (njScalor(&position) != 0.0F) {
                        length = njScalor(&ball->speed);
                        njUnitVector(&position);
                        ball->hit.speed.x = position.x * length;
                        ball->hit.speed.y = position.y * length;
                        ball->hit.speed.z = position.z * length;
                    }
                }
                /* @hack redundant: a block no path reaches, which the image
                 * keeps */
                if (0 &&
                    (magnitude = njScalor(&ball->hit.speed)) > HIT_SPEED_MOST) {
                    njUnitVector(&ball->hit.speed);
                    magnitude = njScalor(&ball->speed);
                    excess = magnitude - HIT_SPEED_MOST;
                    ball->speed.x = ball->hit.speed.x * excess;
                    ball->speed.y = ball->hit.speed.y * excess;
                    ball->speed.z = ball->hit.speed.z * excess;
                    magnitude = HIT_SPEED_MOST;
                    ball->hit.speed.x *= magnitude;
                    ball->hit.speed.y *= magnitude;
                    ball->hit.speed.z *= magnitude;
                    swap = list[count - 1];
                    list[count - 1] = *entry;
                    *entry = swap;
                    i = -1;
                } else {
                    ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
                }
                if (between_walls) {
                    ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z =
                        0.0F;
                }
                ball->pos.x = ball->hit.pos.x + ball->hit.speed.x;
                ball->pos.y = ball->hit.pos.y + ball->hit.speed.y;
                ball->pos.z = ball->hit.pos.z + ball->hit.speed.z;
                contact = &polygon_contacts[ball->hit_index];
                plane.vx = ball->up.x;
                plane.vy = ball->up.y;
                plane.vz = ball->up.z;
                plane.px = contact->hit.point.x;
                plane.py = contact->hit.point.y;
                plane.pz = contact->hit.point.z;
                njDistanceP2PL(&ball->pos, &plane, &point);
                ball->pos.x = offset.x + point.x;
                ball->pos.y = offset.y + point.y;
                ball->pos.z = offset.z + point.z;
                if (owner != NULL) {
                    record->ground_task = owner;
                    if (!player_ride_task(work, record, ball, &offset)) {
                        owner = NULL;
                    }
                } else {
                    record->ground_task = NULL;
                }
                position.x = ball->pos.x - offset.x;
                position.y = ball->pos.y - offset.y;
                position.z = ball->pos.z - offset.z;
                gap = vector_distance(&work->pos, &position);
                if (owner != NULL || record->spd.x != 0.0F ||
                    record->spd.z != 0.0F || gap > 1.0F || approach_stepped) {
                    if (gap > 0.0001F || approach_stepped) {
                        work->pos = position;
                    }
                }
                if (record->spd.x >= STICK_SPEED ||
                    record->spd.x <= -STICK_SPEED ||
                    njInnerProduct(&world_gravity, &ball->up) < STICK_TILT ||
                    (entry->flags & WORLD_UPRIGHT)) {
                    work->flags |= PLAYER_WORK_ON_GROUND;
                    player_set_ground(work, motion, record, entry->flags);
                } else if (/* @bug logic: no player reaches this arm. its
                            * tilt test repeats the first arm's, which took
                            * every player that passes it (0x8c041416) */
                           (record->spd.x >= STICK_SPEED_TILTED ||
                            record->spd.x <= -STICK_SPEED_TILTED) &&
                           njInnerProduct(&world_gravity, &ball->up) <
                               STICK_TILT) {
                    work->flags |= PLAYER_WORK_ON_GROUND;
                    player_set_ground(work, motion, record, entry->flags);
                } else {
                    player_set_ground(work, motion, record, 0);
                    if (record->spd.x > 0.0F) {
                        player_slip_off(work, motion, ball);
                    }
                }
            } else {
                result =
                    player_hit_wall(work, &motion->spd, record, ball, &old_pos,
                                    &offset, &normal, result, entry->flags);
                continue;
            }
        } else {
            result = player_hit_wall(work, &motion->spd, record, ball, &old_pos,
                                     &offset, &normal, result, entry->flags);
        }
    }
    if (grounded != TRUE) {
        strength = 0.0F;
        if ((record->last_flags & PLAYER_WORK_ON_GROUND) &&
            !(record->last_flags & PLAYER_WORK_NO_CONTROL) &&
            (!player_read_stick(work, record, NULL, &strength) ||
             (strength <= STAY_STROKE && !record->stick_lock)) &&
            record->spd.y <= 0.0F &&
            record->spd.x <= record->consts.running_speed &&
            !(record->last_ground_flags & GROUND_SLIPPERY) &&
            record->level > STAY_LEVEL) {
            work->pos.x -= motion->spd.x * STAY_BACK;
            work->pos.y -= motion->spd.y * STAY_BACK;
            work->pos.z -= motion->spd.z * STAY_BACK;
            motion->spd.x = 0.0F;
            motion->spd.y = 0.0F;
            motion->spd.z = 0.0F;
        } else {
            work->pos.x += ball->speed.x;
            work->pos.y += ball->speed.y;
            work->pos.z += ball->speed.z;
        }
        player_set_ground(work, motion, record, 0);
    } else {
        record->ground_object = object;
        work->pos.x += ball->speed.x;
        work->pos.y += ball->speed.y;
        work->pos.z += ball->speed.z;
    }
    return result;
}

/* a hit's normal lies along the world's z axis where its z is Z_NORMAL or more
 * from 0. its tilt about z is then undefined. ball_fix_z_normal then clears
 * the x and y of the hit's front and normal, and gives the hit the work's
 * ang.z */
#define Z_NORMAL 0.999999F

static void ball_fix_z_normal(struct task_work *work, struct polygon_ball *ball)
{
    if (_builtin_fabsf(ball->hit.normal.z) >= Z_NORMAL) {
        ball->hit.front.y = 0.0F;
        ball->hit.front.x = 0.0F;
        ball->hit.normal.y = 0.0F;
        ball->hit.normal.x = 0.0F;
        ball->hit.ang_z = work->ang.z;
    }
}

/* player_step_up's foot and radius for a player in slot PLAYER_SLOTS, once a
 * WORLD_UPRIGHT object is listed. that slot is past the last, and no player
 * has it */
static Float step_foot_slot_8 = 5.2F;
static Float step_radius_slot_8 = 4.0F;

/* player_step_up's foot, above the ball's drop, and the radii of its ball. the
 * radius is STEP_RADIUS_SLOW up to trot_speed, and grows to STEP_RADIUS_FAST at
 * full_speed. it is STEP_RADIUS_FAST too after WORLD_UPRIGHT ground, and on a
 * WORLD_UPRIGHT object for a player that isn't rising */
#define STEP_FOOT 4.0F
#define STEP_RADIUS_SLOW 4.1F
#define STEP_RADIUS_FAST 5.1F

/* lift the ball of the player in slot onto the surface it hit, by STEP_FOOT,
 * and sweep world's object again. the ball is sized as STEP_RADIUS_SLOW says.
 * once player_collide_world has listed a WORLD_UPRIGHT object this step, the
 * ball starts at the hit's pos less its radius along the up. that holds off a
 * path. otherwise it starts at the player's pos moved as the hit moved the
 * ball.
 *
 * it returns 1, keeping the new hit, where its own stand test passes. the new
 * surface must be ground, as IS_GROUND says, or a WORLD_UPRIGHT object's. it
 * must also not face along world_gravity, unless the player began the step on
 * the ground. the ball then takes back its radius, speed and pos. its hit
 * slides onto the new surface, unless ball_runs_into_wall found the ball runs
 * into a wall. it returns 0 otherwise, with the ball as it was */
static int player_step_up(int slot, struct task_work *work,
                          struct polygon_ball *ball, struct world_object *world)
{
    struct player_motion *motion;
    struct player *record;
    struct polygon_ball saved;
    NJS_VECTOR radius;
    NJS_VECTOR foot;
    NJS_VECTOR dir;
    NJS_POINT3 cross;
    NJS_PLANE plane;
    Bool stopped;
    Angle angle;
    Float length;

    motion = player_motion[slot];
    record = player_records[slot];
    ball_fix_z_normal(work, ball);
    stopped = ball_runs_into_wall(work, motion, record, ball);
    saved = *ball;
    foot.x = foot.z = 0.0F;
    foot.y = STEP_FOOT;
    if (upright_ground && record->slot == PLAYER_SLOTS) {
        foot.y = step_foot_slot_8;
    }
    ball_drop(motion, record, ball, &foot);
    if (!upright_ground || (work->flags & PLAYER_WORK_ON_PATH)) {
        turn_to_world(work, &foot);
    } else {
        ground_turn_to_world(&ball->hit.normal, &foot, work->ang.y);
    }
    radius.x = radius.z = 0.0F;
    radius.y = ball->radius;
    if (upright_ground && record->slot == PLAYER_SLOTS) {
        radius.y = step_radius_slot_8;
    }
    if (!upright_ground || (work->flags & PLAYER_WORK_ON_PATH)) {
        turn_to_world(work, &radius);
    } else {
        ground_turn_to_world(&ball->hit.normal, &radius, work->ang.y);
    }
    if (!upright_ground || (work->flags & PLAYER_WORK_ON_PATH)) {
        ball->pos.x = ball->hit.pos.x - ball->pos.x + work->pos.x + foot.x;
        ball->pos.y = ball->hit.pos.y - ball->pos.y + work->pos.y + foot.y;
        ball->pos.z = ball->hit.pos.z - ball->pos.z + work->pos.z + foot.z;
    } else if (world->flags & WORLD_UPRIGHT) {
        ball->pos.x = ball->hit.speed.x + ball->hit.pos.x - radius.x + foot.x;
        ball->pos.y = ball->hit.speed.y + ball->hit.pos.y - radius.y + foot.y;
        ball->pos.z = ball->hit.speed.z + ball->hit.pos.z - radius.z + foot.z;
        ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
    } else {
        ball->pos.x = ball->hit.pos.x - radius.x + foot.x;
        ball->pos.y = ball->hit.pos.y - radius.y + foot.y;
        ball->pos.z = ball->hit.pos.z - radius.z + foot.z;
        angle = vector_angle(&ball->up, &ball->hit.normal, NULL);
        if (angle >= STAND_SLOPE_TURN) {
            dir = ball->hit.speed;
            if (njScalor(&dir) >= 1.0F) {
                njUnitVector(&dir);
                length = njScalor(&ball->speed);
                ball->speed.x = dir.x * length;
                ball->speed.y = dir.y * length;
                ball->speed.z = dir.z * length;
            }
        }
    }
    if (record->spd.x >= record->consts.full_speed ||
        (record->last_ground_flags & WORLD_UPRIGHT) ||
        (record->spd.y <= 0.0F && (world->flags & WORLD_UPRIGHT))) {
        ball->radius = STEP_RADIUS_FAST;
    } else if (record->consts.trot_speed <= record->spd.x &&
               record->spd.x <= record->consts.full_speed) {
        ball->radius =
            ((record->spd.x - record->consts.trot_speed) /
             (record->consts.full_speed - record->consts.trot_speed)) +
            STEP_RADIUS_SLOW;
    } else if (record->spd.x < record->consts.trot_speed) {
        /* @hack redundant: two arms that set the same radius, where one
         * would do */
        ball->radius = STEP_RADIUS_SLOW;
    } else {
        ball->radius = STEP_RADIUS_SLOW;
    }
    if (polygon_sweep(ball, world->object, world->flags, 0) == 0) {
        *ball = saved;
        return 0;
    }
    angle = vector_angle(&ball->up, &ball->hit.normal, NULL);
    if ((IS_GROUND(angle, &ball->up, &ball->hit.normal) ||
         (world->flags & WORLD_UPRIGHT)) &&
        (njInnerProduct(&world_gravity, &ball->hit.normal) <= 0.0F ||
         (record->last_flags & PLAYER_WORK_GROUNDED))) {
        ball->radius = saved.radius;
        ball->speed = saved.speed;
        if (stopped) {
            ball->hit.speed = saved.hit.speed;
            ball->hit.pos = saved.hit.pos;
        } else {
            radius.x = radius.z = 0.0F;
            radius.y = ball->radius;
            if (upright_ground && record->slot == PLAYER_SLOTS) {
                radius.y = step_radius_slot_8;
            }
            if (!upright_ground || (work->flags & PLAYER_WORK_ON_PATH)) {
#ifdef NONMATCHING
                /* for a finite vector, the kernel gives the bits of the matrix
                 * calls below, as engine/dc/turn.h says */
                zam_vec3_turn_yxz(ball->hit.ang_x, -work->ang.y,
                                  ball->hit.ang_z, &radius.x, &radius.x);
#else
                matrix_push_unit();
                njRotateZ(NULL, ball->hit.ang_z);
                njRotateX(NULL, ball->hit.ang_x);
                njRotateY(NULL, -work->ang.y);
                njCalcVector(NULL, &radius, &radius);
                njPopMatrixEx();
#endif
            } else {
                ground_turn_to_world(&ball->hit.normal, &radius, work->ang.y);
            }
            plane.vx = ball->hit.normal.x;
            plane.vy = ball->hit.normal.y;
            plane.vz = ball->hit.normal.z;
            plane.px = ball->hit.point.x;
            plane.py = ball->hit.point.y;
            plane.pz = ball->hit.point.z;
            njDistanceP2PL(&saved.hit.pos, &plane, &cross);
            ball->hit.pos.x = radius.x + cross.x;
            ball->hit.pos.y = radius.y + cross.y;
            ball->hit.pos.z = radius.z + cross.z;
            ball->hit.speed.x += ball->hit.point.x - saved.hit.point.x;
            ball->hit.speed.y += ball->hit.point.y - saved.hit.point.y;
            ball->hit.speed.z += ball->hit.point.z - saved.hit.point.z;
        }
        ball->pos = saved.pos;
    } else {
        *ball = saved;
        return 0;
    }
    ball_fix_z_normal(work, ball);
    return 1;
}

/* player_hit_wall crashes a player at crash_speed into a GROUND_CRASHES wall
 * within this of the way back */
#define CRASH_TURN 0x1000
/* player_hit_wall crashes a player in the air where the way back meets a
 * wall's normal by this. no inner product of unit vectors reaches it */
#define CRASH_BOUND 4.0F
/* a wall whose normal lies this far or more from the ball's up is a ceiling.
 * player_hit_wall ends a rising player's rise at one. unmasked_stop_at_walls,
 * ball_runs_into_wall and unmasked_slide_along_walls pass over one */
#define CEILING_TURN 0x7000
/* in PLAYER_MODE_FALL, and in a Sonic's PLAYER_MODE_SONIC_HOMING,
 * player_hit_wall holds the ball's speed to this */
#define FALL_HIT_SPEED 3.0F

/* meet a wall of an object with flags that the player's ball hit. it returns
 * result, or PLAYER_BUMPED or PLAYER_CRASHED in its place. wall_normal takes
 * the wall's normal.
 *
 * once an object has stood the player this step, the wall is measured against
 * the way back to from. from is where the player began the step. a
 * GROUND_DEFLECTS wall within a quarter turn deflects the player by
 * player_deflect. at CRASH_ANY_SPEED the player stops, and crashes into a
 * GROUND_CRASHES wall. at crash_speed it crashes into a GROUND_CRASHES wall
 * within CRASH_TURN. any other wall within a quarter turn deflects it.
 *
 * otherwise the wall is measured against the way back along spd, and in
 * PLAYER_MODE_UNK_38 the player stops. a GROUND_DEFLECTS wall within a quarter
 * turn deflects the player. a rising player that meets a ceiling loses the
 * speed along its up, and its jump_steps reach jump_hold_steps. Knuckles and
 * Rouge bump into a wall they can climb within TURN_ACROSS. any other wall
 * within a quarter turn deflects a player that began the step on the ground.
 *
 * the player then takes the hit's pos less offset, or less held_offset in
 * PLAYER_MODE_UNK_38. the ball takes the hit's pos and speed, as FALL_HIT_SPEED
 * holds it. a GROUND_DEFLECTS wall past a quarter turn returns at once, moving
 * nothing. in the second case, so does any wall at a quarter turn or more,
 * unless the player falls faster than its gravity */
static int player_hit_wall(struct task_work *work, NJS_VECTOR *spd,
                           struct player *record, struct polygon_ball *ball,
                           NJS_POINT3 *from, NJS_POINT3 *offset,
                           NJS_POINT3 *held_offset, int result, Sint32 flags)
{
    NJS_VECTOR way;
    Angle turn;
    Float facing;
    Float bound;

    record->wall_normal = ball->hit.normal;
    if (work->flags & PLAYER_WORK_ON_GROUND) {
        way.x = from->x - work->pos.x;
        way.y = from->y - work->pos.y;
        way.z = from->z - work->pos.z;
        if (njScalor(&way) != 0.0F) {
            njUnitVector(&way);
            turn = vector_angle(&way, &ball->hit.normal, NULL);
            if (flags & GROUND_DEFLECTS) {
                if (turn <= QUARTER_TURN) {
                    result = PLAYER_BUMPED;
                    player_deflect(work, record, ball, &way);
                } else {
                    return result;
                }
            } else if (record->spd.x >= CRASH_ANY_SPEED) {
                if (flags & GROUND_CRASHES) {
                    result = PLAYER_CRASHED;
                }
                ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z =
                    0.0F;
                player_stop(NULL, record);
            } else if (turn <= CRASH_TURN) {
                if (record->spd.x >= record->consts.crash_speed) {
                    if (flags & GROUND_CRASHES) {
                        result = PLAYER_CRASHED;
                    } else {
                        result = PLAYER_BUMPED;
                        player_deflect(work, record, ball, &way);
                    }
                } else {
                    result = PLAYER_BUMPED;
                    player_deflect(work, record, ball, &way);
                }
            } else if (turn <= QUARTER_TURN) {
                result = PLAYER_BUMPED;
                player_deflect(work, record, ball, &way);
            }
        }
    } else if (work->mode == PLAYER_MODE_UNK_38) {
        result = PLAYER_BUMPED;
        ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
        player_stop(NULL, record);
    } else {
        bound = CRASH_BOUND;
        if (approach_stepped) {
            way.x = -1.0F;
            way.y = 0.0F;
            way.z = 0.0F;
            turn_to_world(work, &way);
        } else {
            way.x = -spd->x;
            way.y = -spd->y;
            way.z = -spd->z;
        }
        if (njScalor(&way) != 0.0F) {
            njUnitVector(&way);
            turn = vector_angle(&way, &ball->hit.normal, NULL);
            facing = njInnerProduct(&way, &ball->hit.normal);
            if (flags & GROUND_DEFLECTS) {
                if (turn <= QUARTER_TURN) {
                    result = PLAYER_BUMPED;
                    player_deflect(work, record, ball, &way);
                } else {
                    return result;
                }
            } else if (/* @bug logic: facing, an inner product of unit
                        * vectors, never reaches CRASH_BOUND, so neither this
                        * arm nor the later one that tests it runs
                        * (0x8c041f0e, 0x8c041fd6) */
                       record->spd.x >= CRASH_ANY_SPEED && facing >= bound) {
                result = PLAYER_CRASHED;
                ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z =
                    0.0F;
                player_stop(NULL, record);
            } else if (record->spd.y > 0.0F &&
                       njInnerProduct(&ball->up, &ball->hit.normal) <
                           njCos(CEILING_TURN)) {
                way = ball->hit.speed;
                result = PLAYER_BUMPED;
                turn_from_world(work, &way);
                way.y = 0.0F;
                turn_to_world(work, &way);
                ball->hit.speed = way;
                way = *spd;
                turn_from_world(work, &way);
                way.y = 0.0F;
                turn_to_world(work, &way);
                *spd = way;
                record->jump_steps = record->consts.jump_hold_steps;
            } else if (facing >= bound) {
                if (record->spd.x >= record->consts.crash_speed) {
                    result = PLAYER_CRASHED;
                } else {
                    result = PLAYER_BUMPED;
                }
            } else if ((record->character == PLAYER_KNUCKLES ||
                        record->character == PLAYER_ROUGE) &&
                       turn <= TURN_ACROSS &&
                       !(flags & (GROUND_DEFLECTS | WORLD_NO_CLIMB))) {
                result = PLAYER_BUMPED;
            } else if (turn >= QUARTER_TURN) {
                if (!(record->spd.y < -record->consts.gravity)) {
                    return result;
                }
            } else if (record->last_flags & PLAYER_WORK_GROUNDED) {
                if (between_walls) {
                    ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z =
                        0.0F;
                }
                player_deflect(work, record, ball, &way);
            }
        }
    }
    ball->pos = ball->hit.pos;
    if (work->mode == PLAYER_MODE_UNK_38) {
        work->pos.x = ball->hit.pos.x - held_offset->x;
        work->pos.y = ball->hit.pos.y - held_offset->y;
        work->pos.z = ball->hit.pos.z - held_offset->z;
    } else {
        work->pos.x = ball->hit.pos.x - offset->x;
        work->pos.y = ball->hit.pos.y - offset->y;
        work->pos.z = ball->hit.pos.z - offset->z;
    }
    if ((work->mode == PLAYER_MODE_SONIC_HOMING ||
         work->mode == PLAYER_MODE_FALL) &&
        njScalor(&ball->hit.speed) > FALL_HIT_SPEED) {
        /* @hack redundant: a length that nothing reads, where no call would
         * do */
        njScalor(&ball->hit.speed);
        njUnitVector(&ball->hit.speed);
        ball->hit.speed.x *= FALL_HIT_SPEED;
        ball->hit.speed.y *= FALL_HIT_SPEED;
        ball->hit.speed.z *= FALL_HIT_SPEED;
        ball->speed = ball->hit.speed;
    } else {
        ball->speed = ball->hit.speed;
    }
    return result;
}

/* player_hit_wall for player_collide_unmasked, which pushes the player out of a
 * wall and never crashes it. wall_normal takes the wall's normal. a wall within
 * a quarter turn of the way back deflects the player by unmasked_deflect. one
 * at a quarter turn or more returns at once, moving nothing. once an object has
 * stood the player this step, the way back runs to from. where from is the
 * player's pos, it runs against spd. for any other player it runs against spd.
 *
 * a deflection is PLAYER_BUMPED, but where the player wasn't stood this step
 * and the wall isn't GROUND_DEFLECTS. such a player on a shape, with
 * between_walls set, first loses the hit's speed and spd. the player then takes
 * the hit's pos less offset, and the ball the hit's pos and speed. it returns
 * result, or PLAYER_BUMPED in its place */
static int unmasked_push_off_wall(struct task_work *work, NJS_VECTOR *spd,
                                  struct player *record,
                                  struct polygon_ball *ball, NJS_POINT3 *from,
                                  NJS_POINT3 *offset, int result, Sint32 flags)
{
    NJS_VECTOR way;
    Angle turn;

    record->wall_normal = ball->hit.normal;
    if (work->flags & PLAYER_WORK_ON_GROUND) {
        way.x = from->x - work->pos.x;
        way.y = from->y - work->pos.y;
        way.z = from->z - work->pos.z;
        if (njScalor(&way) != 0.0F) {
            njUnitVector(&way);
            turn = vector_angle(&way, &ball->hit.normal, NULL);
            /* @hack spelling: each arm deflects the player itself, where one
             * call after the block would do */
            if (turn < QUARTER_TURN) {
                result = PLAYER_BUMPED;
                unmasked_deflect(work, record, ball, &way);
            } else {
                return result;
            }
        } else {
            way.x = -spd->x;
            way.y = -spd->y;
            way.z = -spd->z;
            if (njScalor(&way) == 0.0F) {
                return result;
            }
            njUnitVector(&way);
            turn = vector_angle(&way, &ball->hit.normal, NULL);
            if (turn >= QUARTER_TURN) {
                return result;
            }
            result = PLAYER_BUMPED;
            unmasked_deflect(work, record, ball, &way);
        }
    } else {
        way.x = -spd->x;
        way.y = -spd->y;
        way.z = -spd->z;
        if (njScalor(&way) != 0.0F) {
            njUnitVector(&way);
            turn = vector_angle(&way, &ball->hit.normal, NULL);
            /* @hack redundant: an inner product that nothing reads, where no
             * call would do */
            njInnerProduct(&way, &ball->hit.normal);
            if (flags & GROUND_DEFLECTS) {
                if (turn < QUARTER_TURN) {
                    result = PLAYER_BUMPED;
                    unmasked_deflect(work, record, ball, &way);
                } else {
                    return result;
                }
            } else {
                if (turn >= QUARTER_TURN) {
                    return result;
                } else {
                    if (between_walls && (work->flags & PLAYER_WORK_ON_SHAPE)) {
                        ball->hit.speed.x = ball->hit.speed.y =
                            ball->hit.speed.z = 0.0F;
                        spd->x = spd->y = spd->z = 0.0F;
                    }
                    unmasked_deflect(work, record, ball, &way);
                }
            }
        }
    }
    ball->pos = ball->hit.pos;
    work->pos.x = ball->hit.pos.x - offset->x;
    work->pos.y = ball->hit.pos.y - offset->y;
    work->pos.z = ball->hit.pos.z - offset->z;
    ball->speed = ball->hit.speed;
    return result;
}

/* ball_drop's shares of the constants' snap. d is the inner product of the
 * ball's up and world_gravity's reverse: 1 on level ground, 0 on a wall and
 * below 0 under a ceiling.
 * - DROP_UPRIGHT after WORLD_UPRIGHT ground, or DROP_UPRIGHT_RISING there for
 *   a rising player;
 * - DROP_SHORT on GROUND_GRIP_LOW ground;
 * - along a path, DROP_PATH_UNDER where d is DROP_PATH_UNDER_D or less,
 *   DROP_PATH_WALL where it is DROP_PATH_WALL_D or less, and DROP_PATH above;
 * - below full_speed, or off the ground as the step began, where d lies
 *   between DROP_WALL_LOW and DROP_WALL_HIGH: DROP_WALL, or DROP_SHORT where
 *   the unit motion's y lies DROP_WALL_ALONG or more from 0;
 * - there with d above DROP_WALL_HIGH: DROP_SLOW up to running_speed, or else
 *   spd.x / full_speed for a player that began the step on the ground */
#define DROP_UPRIGHT 0.6F
#define DROP_UPRIGHT_RISING 0.2F
#define DROP_SHORT 0.3F
#define DROP_PATH_UNDER 1.8F
#define DROP_PATH_UNDER_D (-0.6F)
#define DROP_PATH_WALL 1.65F
#define DROP_PATH_WALL_D 0.05F
#define DROP_PATH 1.4F
#define DROP_WALL 1.5F
#define DROP_WALL_LOW (-0.1F)
#define DROP_WALL_HIGH 0.2F
#define DROP_WALL_ALONG 0.25F
#define DROP_SLOW 0.5F

/* lower drop's y, a height above the player's pos, by the ball's reach below
 * the player. that reach keeps the player on the ground. it is the constants'
 * snap, times the share the DROP_ constants give, or whole where none does. a
 * rising player's drop keeps its y, but along a path and after WORLD_UPRIGHT
 * ground. drop's x and z become 0 */
static void ball_drop(struct player_motion *motion, struct player *record,
                      struct polygon_ball *ball, NJS_VECTOR *drop)
{
    NJS_VECTOR spd;
    Float snap;
    Float dot;

    snap = record->consts.snap;
    drop->x = drop->z = 0.0F;
    if ((record->last_ground_flags & WORLD_UPRIGHT) && motion->spd.y <= 0.0F) {
        drop->y -= snap * DROP_UPRIGHT;
    } else if ((record->last_ground_flags & WORLD_UPRIGHT) &&
               motion->spd.y > 0.0F) {
        drop->y -= snap * DROP_UPRIGHT_RISING;
    } else if (record->spd.y <= 0.0F) {
        if (record->last_ground_flags & GROUND_GRIP_LOW) {
            /* @hack redundant: both arms scale snap alike, where one would
             * do */
            if (record->last_flags & PLAYER_WORK_GROUNDED) {
                snap *= DROP_SHORT;
            } else {
                snap *= DROP_SHORT;
            }
        } else {
            dot = -njInnerProduct(&world_gravity, &ball->up);
            if (record->last_flags & PLAYER_WORK_ON_PATH) {
                if (dot <= DROP_PATH_UNDER_D) {
                    snap *= DROP_PATH_UNDER;
                } else if (dot <= DROP_PATH_WALL_D) {
                    snap *= DROP_PATH_WALL;
                } else {
                    snap *= DROP_PATH;
                }
            } else if (record->spd.x < record->consts.full_speed ||
                       !(record->last_flags & PLAYER_WORK_GROUNDED)) {
                if (dot > DROP_WALL_LOW) {
                    /* @hack redundant: dot tested against DROP_WALL_LOW
                     * again */
                    if (dot > DROP_WALL_LOW && dot < DROP_WALL_HIGH) {
                        spd = motion->spd;
                        if (njScalor(&spd) == 0.0F) {
                            spd.y = 0.0F;
                        } else {
                            njUnitVector(&spd);
                        }
                        if (DROP_WALL_ALONG > _builtin_fabsf(spd.y)) {
                            snap *= DROP_WALL;
                        } else {
                            snap *= DROP_SHORT;
                        }
                    } else if (record->spd.x <= record->consts.running_speed) {
                        snap *= DROP_SLOW;
                    } else if (record->last_flags & PLAYER_WORK_GROUNDED) {
                        snap *= record->spd.x / record->consts.full_speed;
                    }
                }
            }
        }
        drop->y -= snap;
    } else {
        if (record->last_flags & PLAYER_WORK_ON_PATH) {
            drop->y -= snap;
        }
    }
}

/* the steps of player_tilt_to_ground. the step is TILT_STEP below trot_speed
 * or on GROUND_CLING ground. faster, it is the surface's turn from the
 * player's up, held to TILT_TROT from trot_speed, TILT_RUNNING from
 * running_speed and TILT_SPRINT from sprint_speed. from full_speed the turn is
 * unheld */
#define TILT_STEP 0x400
#define TILT_TROT 0x1000
#define TILT_RUNNING 0x1500
#define TILT_SPRINT EIGHTH_TURN

/* turn the work's ang.x and ang.z toward the tilt of the surface the ball hit,
 * by the step TILT_STEP says. the work takes the tilt at once where the
 * player's up points further along world_gravity than the normal. it takes it
 * at once too where the step began off the ground or on a path */
static void player_tilt_to_ground(struct task_work *work, struct player *record,
                                  struct polygon_ball *ball, Sint32 flags)
{
    NJS_VECTOR normal;
    Angle step;

    if (njInnerProduct(&world_gravity, &ball->up) >
            njInnerProduct(&world_gravity, &ball->hit.normal) ||
        !(record->last_flags & PLAYER_WORK_ON_GROUND) ||
        (record->last_flags & PLAYER_WORK_ON_PATH)) {
        work->ang.x = ball->hit.ang_x;
        work->ang.z = ball->hit.ang_z;
        return;
    }
    normal = ball->hit.normal;
    turn_from_world(work, &normal);
    normal.x = njSqrt((normal.z * normal.z) + (normal.x * normal.x));
    normal.z = 0.0F;
    step = (Angle)(atan2f(normal.x, normal.y) * ANGLE_PER_RADIAN) & ANGLE_MASK;
    if (step > HALF_TURN) {
        step = FULL_TURN - step;
    }
    if (flags & GROUND_CLING) {
        step = TILT_STEP;
    } else if (record->spd.x < record->consts.full_speed) {
        if (record->spd.x >= record->consts.sprint_speed) {
            if (step > TILT_SPRINT) {
                step = TILT_SPRINT;
            }
        } else if (record->spd.x >= record->consts.running_speed) {
            if (step > TILT_RUNNING) {
                step = TILT_RUNNING;
            }
        } else if (record->spd.x >= record->consts.trot_speed) {
            if (step > TILT_TROT) {
                step = TILT_TROT;
            }
        } else {
            step = TILT_STEP;
        }
    }
    work->ang.x = angle_approach(work->ang.x, ball->hit.ang_x, step);
    work->ang.z = angle_approach(work->ang.z, ball->hit.ang_z, step);
}

/* move the player with its record's ground_task, by that task's force record
 * for the player's slot. the ball moves where the force record's apply moves
 * the player, offset by offset. it then moves by the force record's speed, and
 * the work turns by its turn. it returns TRUE where that speed isn't zero. it
 * returns FALSE, changing nothing, where the task has no force records */
static Bool player_ride_task(struct task_work *work, struct player *record,
                             struct polygon_ball *ball, NJS_POINT3 *offset)
{
    struct task_force *force;
    NJS_POINT3 pos;

    force = record->ground_task->force.p;
    if (force == NULL) {
        return FALSE;
    }
    /* @hack spelling: the first record taken in an else, where the test's
     * own step to the second would do */
    if (record->slot) {
        force = &force[1];
    } else {
        force = &force[0];
    }
    if (force->apply != NULL) {
        pos = work->pos;
        work->pos.x = ball->pos.x - offset->x;
        work->pos.y = ball->pos.y - offset->y;
        work->pos.z = ball->pos.z - offset->z;
        force->apply(player_tasks[record->slot], work, force);
        ball->pos.x = work->pos.x + offset->x;
        ball->pos.y = work->pos.y + offset->y;
        ball->pos.z = work->pos.z + offset->z;
        work->pos = pos;
    }
    ball->pos.x += force->speed.x;
    ball->pos.y += force->speed.y;
    ball->pos.z += force->speed.z;
    work->ang.x = (Uint16)(work->ang.x + force->turn.x);
    work->ang.y = (Uint16)(work->ang.y + force->turn.y);
    work->ang.z = (Uint16)(work->ang.z + force->turn.z);
    if (njScalor(&force->speed) == 0.0F) {
        return FALSE;
    }
    return TRUE;
}

/* how far player_slip_off moves the player along the hit's front. the motion
 * gains SLIP_SPEED that way, across the world's x and z */
#define SLIP_MOVE 3.0F
#define SLIP_SPEED 2.0F

/* push the player off the surface the ball hit, along the hit's front */
static void player_slip_off(struct task_work *work,
                            struct player_motion *motion,
                            struct polygon_ball *ball)
{
    work->pos.x += ball->hit.front.x * SLIP_MOVE;
    work->pos.y += ball->hit.front.y * SLIP_MOVE;
    work->pos.z += ball->hit.front.z * SLIP_MOVE;
    motion->spd.x += ball->hit.front.x * SLIP_SPEED;
    motion->spd.z += ball->hit.front.z * SLIP_SPEED;
}

/* the factors of its character's gravity, resist.y and ground_accel that a
 * player takes with its head under water. it takes the THIN_ factors under
 * WORLD_FLAT_THIN water, and the THICK_ factors under WORLD_FLAT_THICK water.
 * PLAYER_CHARACTER_8 takes THIN_GRAVITY_8 for THIN_GRAVITY */
#define THIN_GRAVITY 0.45F
#define THIN_GRAVITY_8 0.5F
#define THIN_RESIST_Y 1.5F
#define THIN_ACCEL 0.65F
#define THICK_GRAVITY 0.6F
#define THICK_RESIST_Y 2.0F
#define THICK_ACCEL 0.45F
/* the grip_scale each GROUND_GRIP_ bit gives, or GRIP_NORMAL without one */
#define GRIP_LOW 0.15F
#define GRIP_LEAST 0.1F
#define GRIP_HALF 0.5F
#define GRIP_MORE 1.5F
#define GRIP_NORMAL 1.0F

/* add flags to the record's ground_flags, and set grip_scale by the
 * GROUND_GRIP_ bits of flags. a player whose head lies under water takes its
 * gravity, resist.y and ground_accel as the THIN_ and THICK_ factors say. out
 * of WORLD_FLAT_THIN water it takes its character's own three. in
 * WORLD_FLAT_THIN water below its head it keeps those it has. the gravity is
 * player_set_gravity's, on a stage in space too */
static void player_set_ground(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, Sint32 flags)
{
    record->ground_flags |= flags;
    if ((record->ground.above_flags & WORLD_FLAT_THIN) &&
        record->ground.above_y > work->pos.y + record->consts.height) {
        player_set_gravity(record);
        if (record->character == PLAYER_CHARACTER_8) {
            record->consts.gravity *= THIN_GRAVITY_8;
        } else {
            record->consts.gravity *= THIN_GRAVITY;
        }
        record->consts.resist.y =
            character_constants[record->character].resist.y * THIN_RESIST_Y;
        record->consts.ground_accel =
            character_constants[record->character].ground_accel * THIN_ACCEL;
    } else if ((record->ground.above_flags & WORLD_FLAT_THICK) &&
               record->ground.above_y > work->pos.y + record->consts.height) {
        player_set_gravity(record);
        record->consts.gravity *= THICK_GRAVITY;
        record->consts.resist.y =
            character_constants[record->character].resist.y * THICK_RESIST_Y;
        record->consts.ground_accel =
            character_constants[record->character].ground_accel * THICK_ACCEL;
    } else if (!(record->ground.above_flags & WORLD_FLAT_THIN)) {
        player_set_gravity(record);
        record->consts.resist.y =
            character_constants[record->character].resist.y;
        record->consts.ground_accel =
            character_constants[record->character].ground_accel;
    }
    if (flags & GROUND_GRIP_LOW) {
        record->grip_scale = GRIP_LOW;
    } else if (flags & GROUND_GRIP_LEAST) {
        record->grip_scale = GRIP_LEAST;
    } else if (flags & GROUND_GRIP_HALF) {
        record->grip_scale = GRIP_HALF;
    } else if (flags & GROUND_GRIP_MORE) {
        record->grip_scale = GRIP_MORE;
    } else {
        record->grip_scale = GRIP_NORMAL;
    }
}

/* the tests of the surfaces a sweep left, from here to unmasked_deflect. a
 * surface is ground as IS_GROUND says, and any other surface is a wall. the
 * ball runs into a wall where its speed points against the wall's normal. each
 * test marks its contacts with these bits in contact_flags, a local that
 * nothing reads */
#define CONTACT_GROUND 0x2
#define CONTACT_WALL 0x8
/* farther than any contact a sweep leaves, for the search of the nearest */
#define CONTACT_FAR 1000.0F
/* a wall counts as the nearest one where their normals meet by SAME_FACING
 * or more. unmasked_stop_at_walls and ball_stop_at_ground take only
 * SAME_FACING_EXACT */
#define SAME_FACING 0.99999F
#define SAME_FACING_EXACT 1.0F
/* the motion of a player on the ground stops at a wall it meets within
 * WALL_HEAD_ON_TURN of head on. one whose ball runs into a wall faster than
 * WALL_SLAM_SPEED along the wall's normal slows to running_speed */
#define WALL_HEAD_ON_TURN 0x600
#define WALL_SLAM_SPEED 4.5F
/* a running player between two walls or more keeps this share of
 * trot_speed at most */
#define WEDGED_SHARE 0.6F
/* ball_stop_at_ground counts a surface that isn't ground as a wall the ball
 * runs into where its normal points against the ball's speed by an inner
 * product below this */
#define INTO_WALL (-0.2F)

/* TRUE for a Sonic or a Shadow whose height is below its character's row of
 * character_constants. such a player stops dead at a wall of the land */
#define SHRUNK(record)                                                         \
    (((record)->character == PLAYER_SONIC &&                                   \
      (record)->consts.height < character_constants[PLAYER_SONIC].height) ||   \
     ((record)->character == PLAYER_SHADOW &&                                  \
      (record)->consts.height < character_constants[PLAYER_SHADOW].height))
/* TRUE for a Sonic or a Shadow on its board, in a mode from
 * PLAYER_MODE_SONIC_BOARD to PLAYER_MODE_SONIC_BOARD_CRASH. two walls at once
 * leave between_walls as it is for such a player */
#define ON_BOARD(work, record)                                                 \
    (((record)->character == PLAYER_SONIC ||                                   \
      (record)->character == PLAYER_SHADOW) &&                                 \
     (work)->mode >= PLAYER_MODE_SONIC_BOARD &&                                \
     (work)->mode <= PLAYER_MODE_SONIC_BOARD_CRASH)

/* the test of world's object, which the ball's last sweep touched, for a
 * player that began the step on the ground. it returns PLAYER_CRASHED or
 * PLAYER_BUMPED as the last wall to give a result says, or 0.
 *
 * the ball's speed stops short of the nearest wall it runs into. on an object
 * without GROUND_GUIDES, a wall met within WALL_HEAD_ON_TURN of head on
 * stops the motion. it is PLAYER_CRASHED for a player at crash_speed or more
 * on a GROUND_CRASHES object. a wall the ball slams into, as WALL_SLAM_SPEED
 * says, slows a faster motion to running_speed: PLAYER_BUMPED. two walls or
 * more set between_walls. a wall that faces as the nearest one so far, as
 * SAME_FACING says, counts once.
 *
 * two cases return PLAYER_BUMPED at once. a SHRUNK player at a wall of the
 * land stops dead, ball and motion. a player in PLAYER_MODE_RUN between two
 * walls of an object without GROUND_DEFLECTS stops its ball, and its motion
 * slows to WEDGED_SHARE of trot_speed */
static int ball_stop_at_walls(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct polygon_ball *ball,
                              struct world_object *world)
{
    int walls;
    int nearest;
    int result;
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    Float facings[POLYGON_CONTACTS];
    NJS_VECTOR normal_sum;
    NJS_VECTOR dir;
    Float least;
    int i;
    Angle turn;
    struct polygon_contact *contact;
    Float facing;
    Float speed;
    Float limit;
    Float length;

    result = 0;
    walls = 0;
    nearest = -1;
    least = CONTACT_FAR;
    normal_sum.x = normal_sum.y = normal_sum.z = 0.0F;
    for (i = 0; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (IS_GROUND(turn, &ball->up, &contact->hit.normal)) {
            contact_flags[i] |= CONTACT_GROUND;
        } else {
            dir = ball->speed;
            if (njScalor(&dir) != 0.0F) {
                njUnitVector(&dir);
            }
            if ((facing = njInnerProduct(&dir, &contact->hit.normal)) < 0.0F) {
                contact_flags[i] |= CONTACT_WALL;
                /* @hack spelling: the count in the else of an empty arm, and
                 * each facing kept in facings, which nothing reads, where
                 * the test negated would do */
                if (nearest != -1 &&
                    (facings[i] = njInnerProduct(
                         &contact->hit.normal,
                         &polygon_contacts[nearest].hit.normal)) >=
                        SAME_FACING) {
                } else {
                    walls++;
                }
                if (least > distances[i]) {
                    least = distances[i];
                    nearest = i;
                }
                if (!(world->flags & GROUND_GUIDES)) {
                    if (-njCos(WALL_HEAD_ON_TURN) > facing) {
                        motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
                        /* @hack spelling: the two tests nested, where one
                         * && would do */
                        if (world->flags & GROUND_CRASHES) {
                            if (record->spd.x >= record->consts.crash_speed) {
                                result = PLAYER_CRASHED;
                            }
                        }
                    }
                    speed = njInnerProduct(&ball->speed, &contact->hit.normal);
                    if (speed < -WALL_SLAM_SPEED) {
                        limit = record->consts.running_speed;
                        if (njScalor(&motion->spd) > limit) {
                            njUnitVector(&motion->spd);
                            motion->spd.x *= limit;
                            motion->spd.y *= limit;
                            motion->spd.z *= limit;
                            result = PLAYER_BUMPED;
                        }
                    }
                }
                njAddVector(&normal_sum, &contact->hit.normal);
            }
        }
    }
    if (walls > 0) {
        if (walls >= 2) {
            /* @hack spelling: the store in the else of an empty arm, where
             * the test negated would do */
            if (ON_BOARD(work, record)) {
            } else {
                between_walls = TRUE;
            }
        }
        length = njScalor(&ball->speed);
        if (world->owner == NULL && SHRUNK(record)) {
            ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
            return PLAYER_BUMPED;
        }
        if (work->mode == PLAYER_MODE_RUN && walls >= 2 &&
            !(world->flags & GROUND_DEFLECTS)) {
            limit = record->consts.trot_speed * WEDGED_SHARE;
            ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            if (njScalor(&motion->spd) > limit) {
                njUnitVector(&motion->spd);
                motion->spd.x *= limit;
                motion->spd.y *= limit;
                motion->spd.z *= limit;
            }
            return PLAYER_BUMPED;
        }
        if (length > distances[nearest]) {
            if (njScalor(&ball->speed) == 0.0F) {
                ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            } else {
                njUnitVector(&ball->speed);
                ball->speed.x *= distances[nearest];
                ball->speed.y *= distances[nearest];
                ball->speed.z *= distances[nearest];
            }
        }
    }
    return result;
}

/* ball_stop_at_walls for player_collide_unmasked. it returns TRUE where the
 * ball's speed stops short of the nearest wall it runs into. it returns FALSE
 * where the ball runs into none or wouldn't pass it. a ceiling counts for
 * nothing. the motion keeps its speed. a SHRUNK player at a wall of the land
 * stops dead, ball and motion, and the test returns TRUE. two walls or more set
 * between_walls. a wall counts once only where it faces as the nearest does by
 * SAME_FACING_EXACT */
static Bool unmasked_stop_at_walls(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record,
                                   struct polygon_ball *ball,
                                   struct world_object *world)
{
    int walls;
    int grounds; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    NJS_VECTOR dir;
    Float least;
    int i;
    int nearest;
    Angle turn;
    struct polygon_contact *contact;
    Float length;

    walls = 0;
    grounds = 0;
    nearest = -1;
    least = CONTACT_FAR;
    for (i = 0; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (IS_GROUND(turn, &ball->up, &contact->hit.normal)) {
            contact_flags[i] |= CONTACT_GROUND;
            /* @hack redundant: a count of the ground that nothing reads,
             * which SHC drops. with it, the mark's store fills the
             * continue's delay slot */
            grounds++;
            continue;
        }
        dir = ball->speed;
        if (njScalor(&dir) != 0.0F) {
            njUnitVector(&dir);
        }
        if (turn >= CEILING_TURN) {
            continue;
        }
        if (njInnerProduct(&dir, &contact->hit.normal) < 0.0F) {
            contact_flags[i] |= CONTACT_WALL;
            /* @hack spelling: the count in the else of an empty arm, where
             * the test negated would do */
            if (nearest != -1 &&
                njInnerProduct(&contact->hit.normal,
                               &polygon_contacts[nearest].hit.normal) >=
                    SAME_FACING_EXACT) {
            } else {
                walls++;
            }
            if (least > distances[i]) {
                least = distances[i];
                nearest = i;
            }
        }
    }
    if (walls > 0) {
        if (walls >= 2) {
            /* @hack spelling: the store in the else of an empty arm, where
             * the test negated would do */
            if (ON_BOARD(work, record)) {
            } else {
                between_walls = TRUE;
            }
        }
        if (world->owner == NULL && SHRUNK(record)) {
            ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
            return TRUE;
        } else {
            /* @hack spelling: the rest in an else after the return, where
             * the if's end would do */
            length = njScalor(&ball->speed);
            if (length > distances[nearest]) {
                if (njScalor(&ball->speed) == 0.0F) {
                    ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
                } else {
                    njUnitVector(&ball->speed);
                    ball->speed.x *= distances[nearest];
                    ball->speed.y *= distances[nearest];
                    ball->speed.z *= distances[nearest];
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* TRUE where the ball's speed runs into a wall the last sweep touched, of
 * those within CEILING_TURN of the ball's up. it changes nothing */
static Bool ball_runs_into_wall(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record,
                                struct polygon_ball *ball)
{
    int walls;
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    NJS_VECTOR dir;
    Float least;
    int i;
    Angle turn;
    struct polygon_contact *contact;
    int grounds; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    walls = 0;
    grounds = 0;
    least = CONTACT_FAR;
    for (i = 0; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (IS_GROUND(turn, &ball->up, &contact->hit.normal)) {
            contact_flags[i] |= CONTACT_GROUND;
            /* @hack redundant: a count of the ground that nothing reads,
             * which SHC drops. with it, the mark's store fills the
             * continue's delay slot */
            grounds++;
            continue;
        }
        dir = ball->speed;
        if (njScalor(&dir) != 0.0F) {
            njUnitVector(&dir);
        }
        if (turn >= CEILING_TURN) {
            continue;
        }
        if (njInnerProduct(&dir, &contact->hit.normal) < 0.0F) {
            contact_flags[i] |= CONTACT_WALL;
            walls++;
            if (least > distances[i]) {
                least = distances[i];
            }
        }
    }
    return walls > 0;
}

/* the test of world's object, which the ball's last sweep touched, for a
 * player that began the step off the ground. it returns TRUE where the ball's
 * speed stops short of the nearest ground, and FALSE otherwise. the walls it
 * counts are those INTO_WALL says the ball runs into. at such a wall of the
 * land, a SHRUNK player stops dead, and the test returns TRUE. two such walls
 * or more set between_walls. they count as unmasked_stop_at_walls counts
 * them */
static Bool ball_stop_at_ground(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record,
                                struct polygon_ball *ball,
                                struct world_object *world)
{
    int walls;
    int grounds;
    int ground;
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    NJS_VECTOR dir;
    Float least;
    Float least_wall;
    int i;
    int nearest;
    Angle turn;
    struct polygon_contact *contact;
    Float length;

    walls = 0;
    grounds = 0;
    ground = -1;
    nearest = -1;
    least = CONTACT_FAR;
    /* @bug uninitialized: the loop below compares least_wall before any path
     * sets it, so which wall counts as the nearest rests on what FR13 held at
     * entry (0x8c04348c) */
#ifdef NONMATCHING
    /* this side keeps the tag's result where player_collide_world calls:
     * least_wall starts at 0.5F, which that caller's FR13 holds, since it
     * halved the player's height. the same start repairs the tag where
     * player_collide_unmasked calls, whose FR13 holds what player_collide
     * loaded on only one of its paths */
    least_wall = 0.5F;
#endif
    for (i = 0; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (IS_GROUND(turn, &ball->up, &contact->hit.normal)) {
            contact_flags[i] |= CONTACT_GROUND;
            grounds++;
            if (least > distances[i]) {
                least = distances[i];
                ground = i;
            }
        } else {
            dir = ball->speed;
            if (njScalor(&dir) != 0.0F) {
                njUnitVector(&dir);
            }
            if (njInnerProduct(&dir, &contact->hit.normal) < INTO_WALL) {
                contact_flags[i] |= CONTACT_WALL;
                /* @hack spelling: the count in the else of an empty arm,
                 * where the test negated would do */
                if (nearest != -1 &&
                    njInnerProduct(&contact->hit.normal,
                                   &polygon_contacts[nearest].hit.normal) >=
                        SAME_FACING_EXACT) {
                } else {
                    walls++;
                }
                /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
                if (distances[i] < least_wall) {
                    least_wall = distances[i];
                    nearest = i;
                }
                /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
            }
        }
    }
    if (walls > 0) {
        if (walls >= 2) {
            /* @hack spelling: the store in the else of an empty arm, where
             * the test negated would do */
            if (ON_BOARD(work, record)) {
            } else {
                between_walls = TRUE;
            }
        }
        if (SHRUNK(record)) {
            if (world->owner == NULL) {
                ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
                motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
                return TRUE;
            }
        }
    }
    if (grounds > 0) {
        length = njScalor(&ball->speed);
        if (length > distances[ground]) {
            if (njScalor(&ball->speed) == 0.0F) {
                ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            } else {
                njUnitVector(&ball->speed);
                ball->speed.x *= distances[ground];
                ball->speed.y *= distances[ground];
                ball->speed.z *= distances[ground];
            }
            return TRUE;
        }
    }
    return FALSE;
}

/* ball_slide_along_walls's player crashes or bumps into a wall whose normal
 * lies within BUMP_TURN of the motion's way back. a player faster than
 * trot_speed that bumps keeps BUMP_SHARE of trot_speed */
#define BUMP_TURN 0x1000
#define BUMP_SHARE 0.2F

/* slide the player along the nearest wall its ball runs into. its walls are
 * those the last sweep touched that the ball slams into, as WALL_SLAM_SPEED
 * says, or meets within WALL_HEAD_ON_TURN of head on. on a WORLD_UPRIGHT object
 * no surface is a wall. it returns 0, changing nothing, where it finds no wall,
 * where the ball's speed wouldn't pass the wall, or after WORLD_UPRIGHT ground.
 * on an object without WORLD_MOVING, a SHRUNK player stops dead, and it returns
 * 0. a hit's speed of zero returns 0 too.
 *
 * otherwise the hit's speed shortens to the wall's distance, and wall_normal
 * takes the wall's normal. where the motion is still, it then returns 0. a
 * player that bumps, as BUMP_TURN says, returns PLAYER_CRASHED at crash_speed
 * and PLAYER_BUMPED below it. its crash makes the hit's speed zero. any other
 * motion turns along the wall at its own length, the way the hit's speed runs.
 * the hit's speed takes the motion, and the player heads along it. the player
 * turns about where that leaves it more than TURN_AWAY from aim_y, and aim_y
 * takes the heading. that slide returns no value, as the bug tagged at its end
 * says */
static int ball_slide_along_walls(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record,
                                  struct polygon_ball *ball, Sint32 flags)
{
    NJS_PLANE wall;
    NJS_PLANE across;
    NJS_LINE line;
    NJS_VECTOR axis;
    NJS_VECTOR back;
    NJS_VECTOR normal_sum;
    NJS_VECTOR normal; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    NJS_VECTOR dir;
    int i;
    Angle turn;
    int walls = 0;
    int nearest = -1;
    Float least = CONTACT_FAR;
    Float speed;
    Float facing;
    struct polygon_contact *contact;
    int no_walls;
    Float slower;

    normal_sum.x = normal_sum.y = normal_sum.z = 0.0F;
    /* @hack redundant: a copy of normal_sum that nothing reads */
    normal = normal_sum;
    /* @hack spelling: i set ahead of no_walls, where the for could set it */
    i = 0;
    no_walls = flags & WORLD_UPRIGHT;
    for (; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (IS_GROUND(turn, &ball->up, &contact->hit.normal) || no_walls) {
            contact_flags[i] |= CONTACT_GROUND;
        } else {
            dir = ball->speed;
            if (njScalor(&dir) != 0.0F) {
                njUnitVector(&dir);
            }
            if ((facing = njInnerProduct(&dir, &contact->hit.normal)) < 0.0F) {
                speed = njInnerProduct(&ball->speed, &contact->hit.normal);
                if (facing < -njCos(WALL_HEAD_ON_TURN) ||
                    speed < -WALL_SLAM_SPEED) {
                    contact_flags[i] |= CONTACT_WALL;
                    walls++;
                    if (distances[i] < least) {
                        least = distances[i];
                        nearest = i;
                    }
                }
                njAddVector(&normal_sum, &contact->hit.normal);
            }
        }
    }
    if (walls <= 0) {
        return 0;
    }
    speed = njScalor(&ball->speed);
    if (SHRUNK(record)) {
        if (!(flags & WORLD_MOVING)) {
            ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
            motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
            return 0;
        }
    }
    if (!(speed > distances[nearest]) ||
        (record->last_ground_flags & WORLD_UPRIGHT)) {
        return 0;
    }
    if (njScalor(&ball->hit.speed) == 0.0F) {
        ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
        return 0;
    } else {
        /* @hack spelling: the rest in an else after the return, where the
         * if's end would do */
        njUnitVector(&ball->hit.speed);
        ball->hit.speed.x *= distances[nearest];
        ball->hit.speed.y *= distances[nearest];
        ball->hit.speed.z *= distances[nearest];
        contact = &polygon_contacts[nearest];
        record->wall_normal = contact->hit.normal;
        back.x = -motion->spd.x;
        back.y = -motion->spd.y;
        back.z = -motion->spd.z;
        if (njScalor(&back) == 0.0F) {
            return 0;
        }
        njUnitVector(&back);
        turn = vector_angle(&back, &record->wall_normal, NULL);
        if (turn <= BUMP_TURN) {
            if (record->spd.x >= record->consts.crash_speed) {
                ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z =
                    0.0F;
                return PLAYER_CRASHED;
            }
            if (record->spd.x > record->consts.trot_speed) {
                slower = record->consts.trot_speed * BUMP_SHARE;
                njUnitVector(&motion->spd);
                motion->spd.x *= slower;
                motion->spd.y *= slower;
                motion->spd.z *= slower;
            }
            return PLAYER_BUMPED;
        }
        njOuterProduct(&ball->hit.speed, &record->wall_normal, &axis);
        if (njScalor(&axis) == 0.0F) {
            axis = ball->up;
        } else {
            njUnitVector(&axis);
        }
        wall.px = contact->hit.point.x;
        wall.py = contact->hit.point.y;
        wall.pz = contact->hit.point.z;
        wall.vx = record->wall_normal.x;
        wall.vy = record->wall_normal.y;
        wall.vz = record->wall_normal.z;
        across.px = ball->hit.pos.x;
        across.py = ball->hit.pos.y;
        across.pz = ball->hit.pos.z;
        across.vx = axis.x;
        across.vy = axis.y;
        across.vz = axis.z;
        njDistancePL2PL(&wall, &across, &line);
        dir = ball->hit.speed;
        if (njScalor(&dir) != 0.0F) {
            njUnitVector(&dir);
        }
        if (njInnerProduct((NJS_VECTOR *)&line.vx, &dir) < 0.0F) {
            line.vx = -line.vx;
            line.vy = -line.vy;
            line.vz = -line.vz;
        }
        speed = njScalor(&motion->spd);
        motion->spd.x = line.vx * speed;
        motion->spd.y = line.vy * speed;
        motion->spd.z = line.vz * speed;
        ball->hit.speed = motion->spd;
        work->ang.y =
            (Angle)(atan2f(line.vz, line.vx) * ANGLE_PER_RADIAN) & ANGLE_MASK;
#ifdef NONMATCHING
        /* this side keeps the tag's result with the turn test's distance,
         * which it returns below */
        turn = angle_distance(motion->aim_y, work->ang.y);
#endif
        if (angle_distance(motion->aim_y, work->ang.y) > TURN_AWAY) {
            work->ang.y += HALF_TURN;
        }
        motion->aim_y = work->ang.y;
    }
    /* @bug uninitialized: the slide returns no value, and
     * player_collide_world takes R0 for its result (0x8c040db8). at the
     * return (0x8c043be6), R0 holds the distance the turn's test measured
     * (0x8c043bac). where that distance passed TURN_AWAY, it holds the
     * heading after the half turn (0x8c043bbe) */
#ifdef NONMATCHING
    /* this side keeps the tag's result: what the image leaves in R0.
     * player_collide_world tests the result for 0 and for PLAYER_CRASHED, and
     * the distance can be either */
    return turn > TURN_AWAY ? work->ang.y : turn;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* ball_slide_along_walls for player_collide_unmasked. a surface is ground where
 * its normal lies within STAND_SLOPE_TURN of the ball's up. it is ground too
 * where its normal points further up the world's y than the up does. a ceiling
 * counts for nothing. it returns 0 where ball_slide_along_walls returns 0
 * before its slide, but on a WORLD_UPRIGHT object, whose surfaces can be walls
 * here. otherwise no wall stops the slide. the hit's speed gains its
 * normal_speed in place of shortening to the wall's distance, and the motion
 * turns along the wall. the player keeps its heading. the slide returns no
 * value, as the bug tagged at its end says */
static int unmasked_slide_along_walls(struct task_work *work,
                                      struct player_motion *motion,
                                      struct player *record,
                                      struct polygon_ball *ball, Sint32 flags)
{
    NJS_PLANE wall;
    NJS_PLANE across;
    NJS_LINE line;
    NJS_VECTOR axis;
    NJS_VECTOR back;
    Float distances[POLYGON_CONTACTS];
    int contact_flags[POLYGON_CONTACTS];
    NJS_VECTOR dir;
    int i;
    Angle turn;
    int walls = 0;
    int grounds = 0; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    int nearest = -1;
    Float least = CONTACT_FAR;
    Float speed;
    Float facing;
    struct polygon_contact *contact;

    for (i = 0; i < ball->count; i++) {
        contact = &polygon_contacts[i];
        distances[i] = vector_distance(&contact->hit.pos, &ball->pos);
        contact_flags[i] = 0;
        turn = vector_angle(&ball->up, &contact->hit.normal, NULL);
        if (turn <= STAND_SLOPE_TURN || ball->up.y < contact->hit.normal.y) {
            contact_flags[i] |= CONTACT_GROUND;
            /* @hack redundant: a count of the ground that nothing reads,
             * which SHC drops. with it, the mark's store fills the
             * continue's delay slot */
            grounds++;
            continue;
        }
        dir = ball->speed;
        if (njScalor(&dir) != 0.0F) {
            njUnitVector(&dir);
        }
        if (turn >= CEILING_TURN) {
            continue;
        }
        if ((facing = njInnerProduct(&dir, &contact->hit.normal)) < 0.0F) {
            speed = njInnerProduct(&ball->speed, &contact->hit.normal);
            if (facing < -njCos(WALL_HEAD_ON_TURN) ||
                speed < -WALL_SLAM_SPEED) {
                contact_flags[i] |= CONTACT_WALL;
                walls++;
                if (distances[i] < least) {
                    least = distances[i];
                    nearest = i;
                }
            }
        }
    }
    if (walls <= 0) {
        return 0;
    }
    /* @hack redundant: a sum of the ball's speed that nothing reads, which
     * SHC drops */
    (void)(ball->speed.x + ball->speed.y + ball->speed.z);
    speed = njScalor(&ball->speed);
    if (SHRUNK(record)) {
        if (!(flags & WORLD_MOVING)) {
            ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
            motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
            return 0;
        }
    }
    if (!(speed > distances[nearest]) ||
        (record->last_ground_flags & WORLD_UPRIGHT)) {
        return 0;
    }
    if (njScalor(&ball->hit.speed) == 0.0F) {
        ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
        return 0;
    } else {
        /* @hack spelling: the sums in an else after the return, where the
         * if's end would do */
        ball->hit.speed.x += ball->hit.normal_speed.x;
        ball->hit.speed.y += ball->hit.normal_speed.y;
        ball->hit.speed.z += ball->hit.normal_speed.z;
    }
    contact = &polygon_contacts[nearest];
    record->wall_normal = contact->hit.normal;
    back.x = -motion->spd.x;
    back.y = -motion->spd.y;
    back.z = -motion->spd.z;
    if (njScalor(&back) == 0.0F) {
        return 0;
    }
    njOuterProduct(&ball->hit.speed, &record->wall_normal, &axis);
    if (njScalor(&axis) == 0.0F) {
        axis = ball->up;
    } else {
        njUnitVector(&axis);
    }
    wall.px = contact->hit.point.x;
    wall.py = contact->hit.point.y;
    wall.pz = contact->hit.point.z;
    wall.vx = contact->hit.normal.x;
    wall.vy = contact->hit.normal.y;
    wall.vz = contact->hit.normal.z;
    across.px = ball->hit.pos.x;
    across.py = ball->hit.pos.y;
    across.pz = ball->hit.pos.z;
    across.vx = axis.x;
    across.vy = axis.y;
    across.vz = axis.z;
    njDistancePL2PL(&wall, &across, &line);
    dir = ball->hit.speed;
    if (njScalor(&dir) != 0.0F) {
        njUnitVector(&dir);
    }
    if (njInnerProduct((NJS_VECTOR *)&line.vx, &dir) < 0.0F) {
        line.vx = -line.vx;
        line.vy = -line.vy;
        line.vz = -line.vz;
    }
    speed = njScalor(&motion->spd);
    motion->spd.x = line.vx * speed;
    motion->spd.y = line.vy * speed;
    motion->spd.z = line.vz * speed;
    ball->hit.speed = motion->spd;
    /* @bug uninitialized: the slide returns no value, and
     * player_collide_unmasked takes R0 for its result (0x8c045736). the
     * store of the motion's spd.z leaves R0 at 8 (0x8c044086), to the return
     * at 0x8c0440b4 */
#ifdef NONMATCHING
    /* this side keeps the tag's result: the image's 8, which
     * player_collide_unmasked takes as found */
    return 8;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* the length of a cross product at or below which player_deflect's plane
 * takes the ball's up for its normal */
#define DEFLECT_AXIS_MIN 0.15F
/* a ball's speed this long or shorter turns no heading in player_deflect */
#define DEFLECT_TURN_SPEED 0.2F

/* turn the hit's speed onto the line where the hit wall meets a plane through
 * the ball, pointing away from way along it. the plane holds way and the wall's
 * normal, or the ball's up where their cross product is DEFLECT_AXIS_MIN long
 * or less. the hit's speed keeps its length for a player that began the step
 * off the ground. for any other it takes the length of the ball's speed. after
 * GROUND_GUIDES ground the motion turns onto the line too, at its own
 * length.
 *
 * where the ball moves faster than DEFLECT_TURN_SPEED, the player's heading
 * turns as the ball's way turns. the player then turns about where its heading
 * lies more than TURN_AWAY from aim_y, and aim_y takes the heading. nothing
 * changes for a mech, or where way lies along the wall's normal. nothing
 * changes either for a player that began the step off the ground no faster than
 * trot_speed */
static void player_deflect(struct task_work *work, struct player *record,
                           struct polygon_ball *ball, NJS_VECTOR *way)
{
    NJS_PLANE wall;
    NJS_PLANE across;
    NJS_LINE line;
    NJS_VECTOR axis;
    NJS_VECTOR before;
    NJS_VECTOR after;
    Float speed;
    struct player_motion *motion;

    if (record->character == PLAYER_EGGMAN_MECH ||
        record->character == PLAYER_TAILS_MECH) {
        return;
    }
    if (!(record->last_flags & PLAYER_WORK_GROUNDED) &&
        njScalor(&record->spd) <= record->consts.trot_speed) {
        return;
    }
    if (njOuterProduct(way, &ball->hit.normal, &axis) == 0.0F) {
        return;
    }
    if (njScalor(&axis) <= DEFLECT_AXIS_MIN) {
        axis = ball->up;
    } else {
        njUnitVector(&axis);
    }
    wall.px = ball->hit.point.x;
    wall.py = ball->hit.point.y;
    wall.pz = ball->hit.point.z;
    wall.vx = ball->hit.normal.x;
    wall.vy = ball->hit.normal.y;
    wall.vz = ball->hit.normal.z;
    across.px = ball->pos.x;
    across.py = ball->pos.y;
    across.pz = ball->pos.z;
    across.vx = axis.x;
    across.vy = axis.y;
    across.vz = axis.z;
    njDistancePL2PL(&wall, &across, &line);
    before = *way;
    if (njScalor(&before) != 0.0F) {
        njUnitVector(&before);
    }
    if (njInnerProduct((NJS_VECTOR *)&line.vx, &before) > 0.0F) {
        line.vx = -line.vx;
        line.vy = -line.vy;
        line.vz = -line.vz;
    }
    if (!(record->last_flags & PLAYER_WORK_GROUNDED)) {
        speed = njScalor(&ball->hit.speed);
    } else {
        speed = njScalor(&ball->speed);
    }
    ball->hit.speed.x = speed * line.vx;
    ball->hit.speed.y = speed * line.vy;
    ball->hit.speed.z = speed * line.vz;
    if (record->last_ground_flags & GROUND_GUIDES) {
        motion = player_motion[record->slot];
        speed = njScalor(&motion->spd);
        motion->spd.x = speed * line.vx;
        motion->spd.y = speed * line.vy;
        motion->spd.z = speed * line.vz;
    }
    before = ball->speed;
    if (njScalor(&before) > DEFLECT_TURN_SPEED) {
        after = ball->hit.speed;
        njUnitVector(&before);
        njUnitVector(&after);
        work->ang.y -= vector_angle(&before, &after, &ball->up);
    }
    if (angle_distance(player_motion[record->slot]->aim_y, work->ang.y) >
        TURN_AWAY) {
        /* @hack spelling: the turn written out, where += would do */
        work->ang.y = work->ang.y + HALF_TURN;
    }
    player_motion[record->slot]->aim_y = work->ang.y;
}

/* a hit's speed shorter than this turns no motion in unmasked_deflect */
#define UNMASKED_DEFLECT_TURN_SPEED 0.4F
/* a player that began the step on the ground keeps UNMASKED_DEFLECT_BRAKE of
 * its speed as unmasked_deflect turns it, past UNMASKED_DEFLECT_BRAKE_SPEED */
#define UNMASKED_DEFLECT_BRAKE_SPEED 0.6F
#define UNMASKED_DEFLECT_BRAKE 0.96F

/* player_deflect for unmasked_push_off_wall. the player's heading turns as the
 * ball's way turned, from its speed to the hit's speed. it turns about where
 * that leaves it more than TURN_AWAY from aim_y, which then takes the heading.
 * a hit's speed UNMASKED_DEFLECT_TURN_SPEED long or more turns the motion too,
 * at its own length, as UNMASKED_DEFLECT_BRAKE_SPEED brakes it. a SHRUNK
 * player stops dead, ball and motion. nothing changes where way lies along the
 * wall's normal, or where the hit's speed is zero. nothing changes either for
 * a player that began the step off the ground no faster than trot_speed */
static void unmasked_deflect(struct task_work *work, struct player *record,
                             struct polygon_ball *ball, NJS_VECTOR *way)
{
    NJS_VECTOR dir;
    NJS_VECTOR before;
    NJS_VECTOR after;
    struct player_motion *motion;
    Float speed;

    if (!(record->last_flags & PLAYER_WORK_GROUNDED) &&
        njScalor(&record->spd) <= record->consts.trot_speed) {
        return;
    }
    if (njOuterProduct(way, &ball->hit.normal, &dir) == 0.0F) {
        return;
    }
    if (SHRUNK(record)) {
        motion = player_motion[record->slot];
        ball->hit.speed.x = ball->hit.speed.y = ball->hit.speed.z = 0.0F;
        motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
        return;
    }
    before = ball->speed;
    after = ball->hit.speed;
    if (njScalor(&after) == 0.0F) {
        return;
    }
    njUnitVector(&before);
    njUnitVector(&after);
    work->ang.y -= vector_angle(&before, &after, &ball->up);
    if (angle_distance(player_motion[record->slot]->aim_y, work->ang.y) >
        TURN_AWAY) {
        work->ang.y += HALF_TURN;
    }
    player_motion[record->slot]->aim_y = work->ang.y;
    dir = ball->hit.speed;
    if (njScalor(&dir) < UNMASKED_DEFLECT_TURN_SPEED) {
        return;
    }
    motion = player_motion[record->slot];
    speed = njScalor(&motion->spd);
    if (speed > UNMASKED_DEFLECT_BRAKE_SPEED &&
        (record->last_flags & PLAYER_WORK_GROUNDED)) {
        speed *= UNMASKED_DEFLECT_BRAKE;
    }
    njUnitVector(&dir);
    motion->spd.x = dir.x * speed;
    motion->spd.y = dir.y * speed;
    motion->spd.z = dir.z * speed;
}

/* no player climbs an object with any of these flags. a touch of one ends
 * the climb's collision step */
#define CLIMB_REFUSES (WORLD_MOVING | GROUND_DEFLECTS | WORLD_NO_CLIMB)
/* a contact whose normal's y lies below -CLIMB_FLAT_Y faces down. one above
 * CLIMB_FLAT_Y faces up, as a floor does. the player climbs a wall between them
 * whose normal meets its up by more than CLIMB_FACING. a wall whose normal lies
 * nearer square to the up pushes the player off */
#define CLIMB_FLAT_Y 0.86F
/* @hack type: one float below 0.259F, which the plain 0.259F stands for */
#define CLIMB_FACING 0.25899997F
/* how far ahead of the player, along its up, the climb probes for the wall
 * it climbs */
#define CLIMB_REACH 0.3F
/* the speed along y past which a climbing player probes for the floor below
 * it, or the one above it */
#define CLIMB_PROBE_SPEED 0.01F

int player_collide_climb(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    NJS_VECTOR up;
    struct polygon_hit hit;
    struct polygon_ball *ball;
    struct world_object *world;
    struct world_object *hit_world;
    struct polygon_contact *contact;
    struct task_force *force;
    struct task *owner;
    int i;
    Float *pos;
    Float *push;
    Sint16 j;
    int found;
    int result = 0;
    Angle ang_x;
    Angle ang_y;
    Angle ang_z;
    Float facing;
    Float speed;

    record->wall_normal.x = record->wall_normal.y = record->wall_normal.z =
        0.0F;
    record->last_ground_flags = record->ground_flags;
    record->ground_flags = 0;
    if (record->ground_task) {
        force = record->ground_task->force.p;
        if (force) {
            if (record->slot) {
                force++;
            }
            work->pos.x += force->speed.x;
            work->pos.y += force->speed.y;
            work->pos.z += force->speed.z;
        }
    }
    if (player_last_side_pusher(work, record)) {
        NJS_VECTOR normal;
        NJS_VECTOR dir;

        collision_get_normal(work, &normal);
        dir = motion->spd;
        if (njScalor(&dir) != 0.0F) {
            njUnitVector(&dir);
        }
        facing = njInnerProduct(&normal, &dir);
        if (facing < 0.0F) {
            motion->spd.x = 0.0F;
            motion->spd.z = 0.0F;
        }
        if (record->spd.x >= record->consts.running_speed) {
            result = PLAYER_CRASHED;
        } else {
            result = PLAYER_BUMPED;
        }
    }
    work->flags &= ~PLAYER_WORK_GROUNDED;
    /* @bug logic: the flags lost PLAYER_WORK_ON_SHAPE in the statement
     * above, so the arm that moves a player standing on a shape never runs
     * (the clear at 0x8c044638, the test at 0x8c04463e) */
    if (work->flags & PLAYER_WORK_ON_SHAPE) {
        work->pos.x += motion->spd.x + record->push.x;
        work->pos.y += record->push.y;
        work->pos.z += motion->spd.z + record->push.z;
        return result;
    }
    ball = record->ball;
    up.x = up.z = 0.0F;
    up.y = 1.0F;
    turn_to_world(work, &up);
    ball->radius = record->consts.height * 0.5F;
    ball->hit.ang_x = work->ang.x;
    ball->hit.ang_z = work->ang.z;
    ball->speed = motion->spd;
    njAddVector(&ball->speed, &record->push);
    polygon_collect_near(ball);
    found = 0;
    ball->pos.x = work->pos.x + motion->spd.x + (ball->radius * up.x);
    ball->pos.y = work->pos.y + motion->spd.y + (ball->radius * up.y);
    ball->pos.z = work->pos.z + motion->spd.z + (ball->radius * up.z);
    for (i = world_count, world = world_objects; i > 0; i--, world++) {
        if (polygon_sweep(ball, world->object, world->flags, 0) == 1) {
            if (world->flags & CLIMB_REFUSES) {
                return result;
            }
            for (j = ball->count, contact = polygon_contacts; j > 0;
                 j--, contact++) {
                if (contact->hit.normal.y < -CLIMB_FLAT_Y) {
                    if (!player_read_stick(work, record, NULL, NULL)) {
                        /* @hack spelling: pos and push stepped across their
                         * vectors' members, where the members would do */
                        pos = &work->pos.x;
                        push = &contact->hit.normal.x;
                        *pos += *push * 0.5F;
                        pos++;
                        push++;
                        *pos += *push * 0.5F;
                        pos++;
                        push++;
                        *pos += *push * 0.5F;
                    }
                    return result;
                }
                if (contact->hit.normal.y < CLIMB_FLAT_Y &&
                    njInnerProduct(&up, &contact->hit.normal) <= CLIMB_FACING) {
                    NJS_PLANE wall;
                    NJS_PLANE across;
                    NJS_LINE line;
                    NJS_VECTOR axis;
                    NJS_VECTOR dir;

                    if (!player_read_stick(work, record, NULL, NULL)) {
                        /* @hack spelling: pos and push stepped inside the
                         * stores, where the members would do */
                        pos = &work->pos.x;
                        push = &contact->hit.normal.x;
                        *pos++ += *push++ * 0.5F;
                        *pos++ += *push++ * 0.5F;
                        *pos += *push * 0.5F;
                    }
                    if (found) {
                        return result;
                    }
                    njOuterProduct(&ball->hit.speed, &contact->hit.normal,
                                   &axis);
                    if (njScalor(&axis) == 0.0F) {
                        axis = ball->up;
                    } else {
                        njUnitVector(&axis);
                    }
                    wall.px = contact->hit.point.x;
                    wall.py = contact->hit.point.y;
                    wall.pz = contact->hit.point.z;
                    wall.vx = contact->hit.normal.x;
                    wall.vy = contact->hit.normal.y;
                    wall.vz = contact->hit.normal.z;
                    across.px = contact->hit.pos.x;
                    across.py = contact->hit.pos.y;
                    across.pz = contact->hit.pos.z;
                    across.vx = axis.x;
                    across.vy = axis.y;
                    across.vz = axis.z;
                    njDistancePL2PL(&wall, &across, &line);
                    dir = ball->hit.speed;
                    if (njScalor(&dir) != 0.0F) {
                        njUnitVector(&dir);
                    }
                    if (njInnerProduct((NJS_VECTOR *)&line.vx, &dir) < 0.0F) {
                        line.vx = -line.vx;
                        line.vy = -line.vy;
                        line.vz = -line.vz;
                    }
                    speed = njScalor(&motion->spd);
                    motion->spd.x = line.vx * speed;
                    motion->spd.y = line.vy * speed;
                    motion->spd.z = line.vz * speed;
                    found = 1;
                    break;
                }
            }
        }
    }
    if (motion->spd.y < -CLIMB_PROBE_SPEED) {
        ball->pos.x = work->pos.x + motion->spd.x +
                      (record->consts.height * up.x * 0.25F);
        ball->pos.y = work->pos.y + motion->spd.y +
                      (((up.y * 0.25F) - 0.5F) * record->consts.height);
        ball->pos.z = work->pos.z + motion->spd.z +
                      (record->consts.height * up.z * 0.25F);
        for (i = world_count, world = world_objects; i > 0; i--, world++) {
            if (polygon_sweep(ball, world->object, world->flags, 0) == 1) {
                for (j = ball->count, contact = polygon_contacts; j > 0;
                     j--, contact++) {
                    if (contact->hit.normal.y >= CLIMB_FLAT_Y) {
                        ball->hit.normal.x = up.x;
                        ball->hit.normal.z = up.z;
                        ball->hit.normal.y = contact->hit.normal.y;
                        return result;
                    }
                }
            }
        }
    }
    {
        NJS_VECTOR normal;
        NJS_VECTOR dir;
        NJS_VECTOR side;
        NJS_VECTOR axis;
        NJS_VECTOR cross;

        found = 0;
        ball->pos.x = work->pos.x + motion->spd.x + (up.x * CLIMB_REACH);
        ball->pos.y = work->pos.y + motion->spd.y + (up.y * CLIMB_REACH);
        ball->pos.z = work->pos.z + motion->spd.z + (up.z * CLIMB_REACH);
        for (i = world_count, world = world_objects; i > 0; i--, world++) {
            if (polygon_sweep(ball, world->object, world->flags, 0) == 1) {
                if (world->flags & CLIMB_REFUSES) {
                    return result;
                }
                for (j = ball->count, contact = polygon_contacts; j > 0;
                     j--, contact++) {
                    /* @hack spelling: the tests of the normal nested, where
                     * one && would do */
                    if (contact->hit.normal.y > -CLIMB_FLAT_Y &&
                        contact->hit.normal.y < CLIMB_FLAT_Y) {
                        if (njInnerProduct(&up, &contact->hit.normal) >
                            CLIMB_FACING) {
                            if (!found) {
                                found = 1;
                                hit = contact->hit;
                                hit_world = world;
                            } else {
                                normal.x = contact->hit.point.x - hit.point.x;
                                normal.y = contact->hit.point.y - hit.point.y;
                                normal.z = contact->hit.point.z - hit.point.z;
                                if (njScalor(&normal) < 0.0001F) {
                                    dir.x = up.x - hit.normal.x;
                                    dir.y = up.y - hit.normal.y;
                                    dir.z = up.z - hit.normal.z;
                                    side.x = up.x - contact->hit.normal.x;
                                    side.y = up.y - contact->hit.normal.y;
                                    side.z = up.z - contact->hit.normal.z;
                                    if (njScalor2(&dir) < njScalor2(&side)) {
                                        hit = contact->hit;
                                        hit_world = world;
                                    }
                                } else if (njInnerProduct(&up, &normal) >=
                                           0.0F) {
                                    hit = contact->hit;
                                    hit_world = world;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (found == 1) {
            normal.x = (up.x + hit.normal.x) * 0.5F;
            normal.y = (up.y + hit.normal.y) * 0.5F;
            normal.z = (up.z + hit.normal.z) * 0.5F;
            if (_builtin_fabsf(normal.y) < 0.001F) {
                normal.y = 0.0F;
            }
            if (njScalor(&normal) == 0.0F) {
                normal.x = 1.0F;
                normal.y = normal.z = 0.0F;
            } else {
                njUnitVector(&normal);
            }
            ang_x = (Angle)(asinf(normal.z) * ANGLE_PER_RADIAN);
            ang_z = -(Angle)(atan2f(normal.x, normal.y) * ANGLE_PER_RADIAN);
            dir.x = 1.0F;
            dir.y = dir.z = 0.0F;
            matrix_push_unit();
            njRotateZ(NULL, ang_z);
            njRotateX(NULL, ang_x);
            njCalcVector(NULL, &dir, &side);
            njPopMatrixEx();
            if (normal.y * normal.y < 0.0001F) {
                axis.x = axis.z = 0.0F;
                axis.y = 1.0F;
            } else {
                if (normal.y > 0.0F) {
                    axis.x = -normal.x;
                    axis.y = ((normal.x * normal.x) + (normal.z * normal.z)) /
                             normal.y;
                    axis.z = -normal.z;
                } else {
                    axis.x = normal.x;
                    axis.y = ((-normal.x * normal.x) - (normal.z * normal.z)) /
                             normal.y;
                    axis.z = normal.z;
                }
                njUnitVector(&axis);
            }
            if (_builtin_fabsf(njOuterProduct(&axis, &side, &cross)) < 0.01F) {
                if (njInnerProduct(&side, &axis) > 0.0F) {
                    ang_y = 0;
                } else {
                    ang_y = HALF_TURN;
                }
            } else {
                njUnitVector(&cross);
                if (njInnerProduct(&normal, &cross) >= 0.0F) {
                    ang_y = (Angle)(acosf(njInnerProduct(&side, &axis)) *
                                    ANGLE_PER_RADIAN);
                } else {
                    ang_y = -(Angle)(acosf(njInnerProduct(&side, &axis)) *
                                     ANGLE_PER_RADIAN);
                }
            }
            /* @hack spelling: the angles cut to 16 bits ahead of the
             * stores, where the stores could cut them */
            ang_y = (Uint16)ang_y;
            ang_x = (Uint16)ang_x;
            ang_z = (Uint16)ang_z;
            work->ang.x = ang_x;
            work->ang.y = ang_y;
            work->ang.z = ang_z;
            motion->aim_x = work->ang.x;
            motion->aim_z = work->ang.z;
            record->ground_normal = ball->hit.normal;
            ball->pos = hit.point;
            owner = hit_world->owner;
            if (owner && owner->force.p) {
                force = owner->force.p;
                if (record->slot) {
                    force++;
                }
                njAddVector(&ball->pos, &force->speed);
                work->ang.x += force->turn.x;
                work->ang.y += force->turn.y;
                work->ang.z += force->turn.z;
            }
            record->ground_task = hit_world->owner;
            if (vector_distance(&work->pos, &ball->pos) > CLIMB_REACH) {
                work->pos = ball->pos;
            } else {
                record->spd.x = record->spd.y = record->spd.z = 0.0F;
                motion->spd.x = motion->spd.y = motion->spd.z = 0.0F;
            }
            ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
            work->flags |= PLAYER_WORK_ON_GROUND;
            record->ground_object = hit_world->object;
            record->ground_flags = hit_world->flags;
            return result;
        }
    }
    if (motion->spd.y > CLIMB_PROBE_SPEED) {
        ball->pos.x = work->pos.x + motion->spd.x -
                      (record->consts.height * up.x * 0.25F);
        ball->pos.y = work->pos.y + motion->spd.y +
                      (((up.y * 0.25F) + 0.5F) * record->consts.height);
        ball->pos.z = work->pos.z + motion->spd.z -
                      (record->consts.height * up.z * 0.25F);
        ball->radius = record->consts.height * 0.75F;
        for (i = world_count, world = world_objects; i > 0; i--, world++) {
            if (polygon_sweep(ball, world->object, world->flags, 0) == 1) {
                for (j = ball->count, contact = polygon_contacts; j > 0;
                     j--, contact++) {
                    if (contact->hit.normal.y >= CLIMB_FLAT_Y) {
                        ball->hit.normal.x = up.x;
                        ball->hit.normal.z = up.z;
                        ball->hit.normal.y = contact->hit.normal.y;
                        return result;
                    }
                }
            }
        }
    }
    return result;
}

/* the longest move along a surface that player_collide_unmasked's ball takes at
 * one hit. a longer move carries its rest on to the next pass over the
 * objects */
#define UNMASKED_STEP 4.1F

/* the collision with the world of a player under PLAYER_WORK_BALL or in
 * PLAYER_MODE_SWIM. it moves the player as player_collide_world does, with
 * these differences. the ball's up and its drop take the work's angles alone,
 * and GROUND_NO_LANDING objects are listed in the air too. the touched objects
 * take unmasked_stop_at_walls's test for a player that began the step on the
 * ground, and ball_stop_at_ground's for any other. the result takes neither's.
 *
 * the stand test reads no masks. a surface that is ground, as IS_GROUND says,
 * stands the player, but not on a shape or a STAND_REFUSES object. nor does it
 * stand a player that began the step before on the ground and this one off it.
 * a surface that faces along world_gravity stands only a player that began the
 * step on the ground. a rising one of those stands on no surface. on a surface
 * that stands it, unmasked_slide_along_walls slides the player. the player
 * tilts by player_tilt_to_ground and keeps PLAYER_WORK_ON_GROUND. a move along
 * the surface past UNMASKED_STEP sets the rest of the ball's speed aside. the
 * pass over the objects then starts over with this one last. every surface that
 * doesn't stand the player pushes it off by unmasked_push_off_wall */
static int player_collide_unmasked(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record)
{
    NJS_VECTOR offset;
    NJS_VECTOR normal;
    NJS_VECTOR position;
    NJS_POINT3 old_pos;
    struct world_object candidates[WORLD_LISTED];
    struct world_object sorted[WORLD_LISTED];
    NJS_PLANE plane;
    NJS_POINT3 point;
    struct world_object swap;
    struct polygon_ball *ball;
    struct world_object *world;
    struct world_object *entry;
    struct world_object *list;
    struct world_object *last;
    struct polygon_contact *contact;
    struct task *owner;
    NJS_OBJECT *object;
    int old_flags;
    int i;
    int j;
    int last_index;
    int placed;
    int count;
    int saved;
    Bool hit;
    Bool grounded;
    int result;
    int found;
    Angle angle;
    Angle pitch;
    Angle roll;
    Angle roll_distance;
    Float strength;
    Float length;

    ball = record->ball;
    result = 0;
    old_flags = record->last_flags;
    record->last_flags = work->flags;
    work->flags &= ~PLAYER_WORK_ON_GROUND;
    matrix_push_unit();
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -work->ang.y);
    position.x = position.z = 0.0F;
    position.y = 1.0F;
    ball->hit.ang_x = work->ang.x;
    ball->hit.ang_z = work->ang.z;
    njCalcVector(NULL, &position, &ball->up);
    position.x = position.z = 0.0F;
    position.y = record->consts.height * 0.5F;
    njCalcVector(NULL, &position, &offset);
    ball_drop(motion, record, ball, &position);
    njCalcVector(NULL, &position, &normal);
    njPopMatrixEx();
    ball->pos.x = work->pos.x + normal.x;
    ball->pos.y = work->pos.y + normal.y;
    ball->pos.z = work->pos.z + normal.z;
    ball->radius = record->consts.height * 0.5F;
    ball->speed = motion->spd;
    njAddVector(&ball->speed, &record->push);
    old_pos = work->pos;
    polygon_collect_near(ball);
    if (world_count == 0) {
        njAddVector(&work->pos, &record->push);
        njAddVector(&work->pos, &motion->spd);
        player_set_ground(work, motion, record, 0);
        record->ground_task = NULL;
        record->ground_object = NULL;
        return 0;
    }
    hit = FALSE;
    grounded = FALSE;
    world = world_objects;
    object = NULL;
    saved = 0;
    count = 0;
    /* @hack spelling: i counted by i = i + 1, where i++ would do */
    for (i = 0; i < world_count; i = i + 1, world++) {
        /* @hack spelling: the rest of the loop's body inside the test of
         * WORLD_FLAT, where a continue would do */
        if (!(world->flags & WORLD_FLAT)) {
            if (world->owner && !(world->owner->work->flags & WORLD_ADDED)) {
                continue;
            }
            if (polygon_sweep(ball, world->object, world->flags, 0)) {
                if (record->last_flags & PLAYER_WORK_GROUNDED) {
                    if (unmasked_stop_at_walls(work, motion, record, ball,
                                               world) == TRUE) {
                        hit = TRUE;
                        saved = count;
                    }
                } else if (ball_stop_at_ground(work, motion, record, ball,
                                               world) == TRUE) {
                    hit = TRUE;
                    saved = count;
                }
            }
            candidates[count] = *world;
            /* @hack spelling: count stepped inside its test, where a
             * statement before it would do */
            if (++count >= WORLD_LISTED - 1) {
                break;
            }
        }
    }
    if (count == 0) {
        njAddVector(&work->pos, &record->push);
        njAddVector(&work->pos, &motion->spd);
        player_set_ground(work, motion, record, 0);
        record->ground_task = NULL;
        record->ground_object = NULL;
        return 0;
    }
    /* the order of the objects, as in player_collide_world */
    placed = 0;
    if (hit == TRUE) {
        sorted[placed] = candidates[saved];
        placed++;
    }
    if (record->ground_task) {
        for (j = 0; j < count; j++) {
            if (hit == TRUE && j == saved) {
                hit = FALSE;
            } else {
                entry = &candidates[j];
                if (record->ground_object == entry->object) {
                    sorted[count - 1] = *entry;
                } else {
                    sorted[placed] = *entry;
                    placed++;
                }
            }
        }
    } else {
        for (j = count - 1; j >= 0; j--) {
            if (hit == TRUE && j == saved) {
                hit = FALSE;
            } else {
                entry = &candidates[j];
                if (record->ground_object == entry->object) {
                    sorted[count - 1] = *entry;
                } else {
                    sorted[placed] = *entry;
                    placed++;
                }
            }
        }
    }
    ball->pos.x = work->pos.x + normal.x;
    ball->pos.y = work->pos.y + normal.y;
    ball->pos.z = work->pos.z + normal.z;
    ball->speed = motion->spd;
    njAddVector(&ball->speed, &record->push);
    record->ground_task = NULL;
    list = sorted;
    last_index = count - 1;
    for (i = 0; i < count; i++) {
        entry = &list[i];
        owner = entry->owner;
        if (!polygon_sweep(ball, entry->object, entry->flags, 0)) {
            continue;
        }
        angle = vector_angle(&ball->up, &ball->hit.normal, NULL);
        if (!(work->flags & PLAYER_WORK_ON_SHAPE) &&
            !(entry->flags & STAND_REFUSES) &&
            IS_GROUND(angle, &ball->up, &ball->hit.normal) &&
            (!(old_flags & PLAYER_WORK_GROUNDED) ||
             (record->last_flags & PLAYER_WORK_GROUNDED)) &&
            (njInnerProduct(&world_gravity, &ball->hit.normal) <= 0.0F ||
             (record->last_flags & PLAYER_WORK_GROUNDED))) {
            if (record->spd.y <= 0.0F ||
                (record->spd.y > 0.0F &&
                 !(record->last_flags & PLAYER_WORK_GROUNDED))) {
                grounded = TRUE;
                object = entry->object;
                found = unmasked_slide_along_walls(work, motion, record, ball,
                                                   entry->flags);
                if (found) {
                    if (!result) {
                        result = found;
                    } else if (found == PLAYER_CRASHED) {
                        result = found;
                    }
                }
                ball->hit.ang_z =
                    -(Angle)(atan2f(ball->hit.normal.x, ball->hit.normal.y) *
                             ANGLE_PER_RADIAN);
                ball->hit.ang_x =
                    (Angle)(asinf(ball->hit.normal.z) * ANGLE_PER_RADIAN);
                if (motion->spd.x != 0.0F || motion->spd.z != 0.0F ||
                    !(record->last_flags & PLAYER_WORK_ON_GROUND) ||
                    njInnerProduct(&world_gravity, &ball->hit.normal) <
                        njInnerProduct(&world_gravity, &ball->up)) {
                    pitch = ball->hit.ang_x;
                    roll = ball->hit.ang_z;
                    /* @hack redundant: pitch's distance, which nothing
                     * reads, where no call would do */
                    angle_distance(work->ang.x, pitch);
                    roll_distance = angle_distance(work->ang.z, roll);
                    if (roll_distance > QUARTER_TURN) {
                        ball->hit.ang_z = roll - HALF_TURN;
                    }
                    if (ball->hit.normal.y <= 0.0F &&
                        angle_distance(ball->hit.ang_z, 0) < QUARTER_TURN) {
                        ball->hit.ang_x = HALF_TURN - pitch;
                    }
                    player_tilt_to_ground(work, record, ball, entry->flags);
                }
                if (!result) {
                    matrix_push_unit();
                    njRotateZ(NULL, work->ang.z);
                    njRotateX(NULL, work->ang.x);
                    njRotateY(NULL, -work->ang.y);
                    position.x = position.z = 0.0F;
                    position.y = 1.0F;
                    ball->hit.ang_x = work->ang.x;
                    ball->hit.ang_z = work->ang.z;
                    njCalcVector(NULL, &position, &ball->up);
                    position.x = position.z = 0.0F;
                    position.y = record->consts.height * 0.5F;
                    njCalcVector(NULL, &position, &offset);
                    ball_drop(motion, record, ball, &position);
                    njCalcVector(NULL, &position, &normal);
                    njPopMatrixEx();
                }
                motion->aim_x = ball->hit.ang_x;
                motion->aim_z = ball->hit.ang_z;
                record->ground_normal = ball->hit.normal;
                if (njScalor(&ball->hit.speed) > UNMASKED_STEP) {
                    njUnitVector(&ball->hit.speed);
                    length = njScalor(&ball->speed) - UNMASKED_STEP;
                    ball->speed.x = ball->hit.speed.x * length;
                    ball->speed.y = ball->hit.speed.y * length;
                    ball->speed.z = ball->hit.speed.z * length;
                    length = UNMASKED_STEP;
                    ball->hit.speed.x *= length;
                    ball->hit.speed.y *= length;
                    ball->hit.speed.z *= length;
                    last = &list[last_index];
                    swap = *last;
                    *last = *entry;
                    *entry = swap;
                    i = -1;
                } else {
                    ball->speed.x = ball->speed.y = ball->speed.z = 0.0F;
                }
                ball->pos.x = ball->hit.pos.x + ball->hit.speed.x;
                ball->pos.y = ball->hit.pos.y + ball->hit.speed.y;
                ball->pos.z = ball->hit.pos.z + ball->hit.speed.z;
                contact = &polygon_contacts[ball->hit_index];
                plane.vx = ball->up.x;
                plane.vy = ball->up.y;
                plane.vz = ball->up.z;
                plane.px = contact->hit.point.x;
                plane.py = contact->hit.point.y;
                plane.pz = contact->hit.point.z;
                njDistanceP2PL(&ball->pos, &plane, &point);
                ball->pos.x = offset.x + point.x;
                ball->pos.y = offset.y + point.y;
                ball->pos.z = offset.z + point.z;
                if (owner) {
                    record->ground_task = owner;
                    if (!player_ride_task(work, record, ball, &offset)) {
                        owner = NULL;
                    }
                } else {
                    record->ground_task = NULL;
                }
                position.x = ball->pos.x - offset.x;
                position.y = ball->pos.y - offset.y;
                position.z = ball->pos.z - offset.z;
                length = vector_distance(&work->pos, &position);
                if (owner || record->spd.x != 0.0F || record->spd.z != 0.0F ||
                    length > 1.0F) {
                    if (length > 0.0001F) {
                        work->pos = position;
                    }
                }
                work->flags |= PLAYER_WORK_ON_GROUND;
                player_set_ground(work, motion, record, entry->flags);
            } else {
                result = unmasked_push_off_wall(work, &motion->spd, record,
                                                ball, &old_pos, &offset, result,
                                                entry->flags);
            }
            continue;
        } else {
            /* @hack spelling: the push in an else after the continue, where
             * the if's end would do */
            result =
                unmasked_push_off_wall(work, &motion->spd, record, ball,
                                       &old_pos, &offset, result, entry->flags);
        }
    }
    if (grounded != TRUE) {
        strength = 0.0F;
        if ((record->last_flags & PLAYER_WORK_ON_GROUND) &&
            !(record->last_flags & PLAYER_WORK_NO_CONTROL) &&
            (!player_read_stick(work, record, NULL, &strength) ||
             (strength <= STAY_STROKE && !record->stick_lock)) &&
            record->spd.y <= 0.0F &&
            record->spd.x <= record->consts.running_speed &&
            record->level > STAY_LEVEL) {
            work->pos.x -= motion->spd.x * STAY_BACK;
            work->pos.y -= motion->spd.y * STAY_BACK;
            work->pos.z -= motion->spd.z * STAY_BACK;
            motion->spd.x = 0.0F;
            motion->spd.y = 0.0F;
            motion->spd.z = 0.0F;
        } else {
            work->pos.x += ball->speed.x;
            work->pos.y += ball->speed.y;
            work->pos.z += ball->speed.z;
        }
        player_set_ground(work, motion, record, 0);
    } else {
        record->ground_object = object;
        work->pos.x += ball->speed.x;
        work->pos.y += ball->speed.y;
        work->pos.z += ball->speed.z;
    }
    return result;
}

void turn_from_world(struct task_work *work, NJS_VECTOR *vector)
{
    NJS_VECTOR local;
    Float *from;

    /* @hack spelling: the copy through a pointer stepped across the
     * vector's members, where a copy of the vector would do */
    from = &vector->x;
    local.x = *from++;
    local.y = *from++;
    local.z = *from;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_zxy(-work->ang.x, work->ang.y, -work->ang.z, &local.x,
                      &vector->x);
#else
    matrix_push_unit();
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, -work->ang.x);
    njRotateZ(NULL, -work->ang.z);
    njCalcVector(NULL, &local, vector);
    njPopMatrixEx();
#endif
}

void turn_to_world(struct task_work *work, NJS_VECTOR *vector)
{
    NJS_VECTOR local;
    Float *from;

    /* @hack spelling: the copy through a pointer stepped across the
     * vector's members, where a copy of the vector would do */
    from = &vector->x;
    local.x = *from++;
    local.y = *from++;
    local.z = *from;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_yxz(work->ang.x, -work->ang.y, work->ang.z, &local.x,
                      &vector->x);
#else
    matrix_push_unit();
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -work->ang.y);
    njCalcVector(NULL, &local, vector);
    njPopMatrixEx();
#endif
}

/* turn vector in place from the world's axes into a ground's frame. the frame
 * is that of ground whose normal is normal, heading along ang_y.
 * ground_turn_to_world turns the vector back */
static void ground_turn_from_world(NJS_VECTOR *normal, NJS_VECTOR *vector,
                                   Angle ang_y)
{
    NJS_VECTOR local;
    Float *from;
    Angle ang_z;
    Angle ang_x;

    /* @hack spelling: the copy through a pointer stepped across the
     * vector's members, where a copy of the vector would do */
    from = &vector->x;
    local.x = *from++;
    local.y = *from++;
    local.z = *from;
    ang_z = -(Angle)(atan2f(normal->x, normal->y) * ANGLE_PER_RADIAN);
    ang_x = (Angle)(asinf(normal->z) * ANGLE_PER_RADIAN);
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_zxy(-ang_x, ang_y, -ang_z, &local.x, &vector->x);
#else
    matrix_push_unit();
    njRotateY(NULL, ang_y);
    njRotateX(NULL, -ang_x);
    njRotateZ(NULL, -ang_z);
    njCalcVector(NULL, &local, vector);
    njPopMatrixEx();
#endif
}

/* turn_to_world in the frame of ground whose normal is normal, heading along
 * ang_y. vector turns about y by -ang_y, then about x by asinf of the normal's
 * z. it then turns about z by -atan2f of the normal's x over its y */
static void ground_turn_to_world(NJS_VECTOR *normal, NJS_VECTOR *vector,
                                 Angle ang_y)
{
    NJS_VECTOR local;
    Float *from;
    Angle ang_z;
    Angle ang_x;

    /* @hack spelling: the copy through a pointer stepped across the
     * vector's members, where a copy of the vector would do */
    from = &vector->x;
    local.x = *from++;
    local.y = *from++;
    local.z = *from;
    ang_z = -(Angle)(atan2f(normal->x, normal->y) * ANGLE_PER_RADIAN);
    ang_x = (Angle)(asinf(normal->z) * ANGLE_PER_RADIAN);
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_yxz(ang_x, -ang_y, ang_z, &local.x, &vector->x);
#else
    matrix_push_unit();
    njRotateZ(NULL, ang_z);
    njRotateX(NULL, ang_x);
    njRotateY(NULL, -ang_y);
    njCalcVector(NULL, &local, vector);
    njPopMatrixEx();
#endif
}

/* a speed within this of 0 counts as none, in the air and in a push */
#define SPEED_NONE 0.001F
/* on the ground, a speed ahead within SPEED_STILL of 0 counts as none, and a
 * speed across within SPEED_STILL_ACROSS times the constants' ground_accel */
#define SPEED_STILL 0.01F
#define SPEED_STILL_ACROSS 0.9F
/* on STAGE_CHAO, a speed ahead below STILL_SHARE of the constants' gravity
 * stops. on ground whose level is above STILL_LEVEL, the bound is
 * STILL_LEVEL_FACTOR of gravity times the level. that bound holds where the
 * speed is no more than gravity times the sine of the slope */
#define STILL_SHARE 0.1F
#define STILL_LEVEL 0.77F
#define STILL_LEVEL_FACTOR 1.2F

void player_take_speed(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_VECTOR spd;
    Float limit;
    Float stop;

    spd = motion->spd;
    if ((work->flags & PLAYER_WORK_GROUNDED) &&
        (record->ground_flags & WORLD_UPRIGHT)) {
        ground_turn_from_world(&record->ground_normal, &spd, work->ang.y);
    } else {
        turn_from_world(work, &spd);
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        spd.y = 0.0F;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        if (!(record->last_ground_flags &
              (GROUND_GRIP_LEAST | GROUND_GRIP_LOW))) {
            limit = SPEED_STILL_ACROSS * record->consts.ground_accel;
            if (_builtin_fabsf(spd.x) < SPEED_STILL) {
                spd.x = 0.0F;
            }
            if (_builtin_fabsf(spd.z) <= limit) {
                spd.z = 0.0F;
            }
        }
    } else {
        if (_builtin_fabsf(spd.x) <= SPEED_NONE) {
            spd.x = 0.0F;
        }
        if (_builtin_fabsf(spd.z) <= SPEED_NONE) {
            spd.z = 0.0F;
        }
    }
    record->spd = spd;
    work->ang.x &= ANGLE_MASK;
    work->ang.y &= ANGLE_MASK;
    work->ang.z &= ANGLE_MASK;
    motion->aim_x &= ANGLE_MASK;
    motion->aim_y &= ANGLE_MASK;
    motion->aim_z &= ANGLE_MASK;
    if (work->flags & PLAYER_WORK_GROUNDED) {
        record->level = -njInnerProduct(&world_gravity, &record->ground_normal);
    } else {
        record->level = 1.0F;
    }
    if (stage_current_id() == STAGE_CHAO) {
        stop = record->consts.gravity * STILL_SHARE;
        if (_builtin_fabsf(record->spd.x) <=
            record->consts.gravity *
                njSqrt(1.0F - (record->level * record->level))) {
            /* @hack spelling: the level's test nested, where one && would
             * do */
            if (record->level > STILL_LEVEL) {
                stop =
                    record->consts.gravity * record->level * STILL_LEVEL_FACTOR;
            }
        }
        if (stop > _builtin_fabsf(record->spd.x)) {
            record->spd.x = 0.0F;
        }
    }
    if (record->ground_flags & WORLD_UPRIGHT) {
        record->level = 1.0F;
    }
    /* @hack redundant: the record's spd stored to itself */
    record->spd = record->spd;
}

void player_take_push_speed(struct task_work *work, NJS_VECTOR *speed,
                            struct player *record)
{
    NJS_VECTOR spd;
    struct task_work *other;

    other = work->collision->hit->owner->work;
    spd = *speed;
    other->pos.x += spd.x;
    other->pos.y += spd.y;
    other->pos.z += spd.z;
    turn_from_world(work, &spd);
    if (spd.x <= SPEED_NONE && spd.x >= -SPEED_NONE) {
        spd.x = 0.0F;
    }
    if (spd.z <= SPEED_NONE && spd.z >= -SPEED_NONE) {
        spd.z = 0.0F;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        spd.y = 0.0F;
    }
    record->spd = spd;
}

/* the most nodes of a model whose motions an anim blends */
#define BLEND_NODES 111
/* a change between two PLAYER_ANIM_LOOP_PACE animations starts the new one
 * this share of its frames further on than the old one had left to play */
#define PACE_LEAD 0.1F

void player_anim_step(struct player_anim *anim)
{
    Uint16 index = anim->next;
    NJS_MOTION *motion;
    Uint32 frames;
    Uint32 next_frames;
    int no_blend;

    no_blend = 0;
    if (anim->flags & PLAYER_ANIM_NO_BLEND) {
        no_blend = 1;
    }
    anim->last = anim->current;
    anim->last_frame = anim->frame;
    anim->flags = 0;
    if (anim->current != index && anim->mode != PLAYER_ANIM_BLEND &&
        anim->mode != PLAYER_ANIM_NEW && anim->mode != PLAYER_ANIM_END) {
        anim->mode = PLAYER_ANIM_CHANGE;
        anim->steps = 0;
        if (anim->actions[anim->next].mode == PLAYER_ANIM_LOOP_PACE &&
            anim->actions[anim->current].mode == PLAYER_ANIM_LOOP_PACE) {
            frames = motion_table[anim->actions[anim->current].motion]
                         .motion->nbFrame;
            next_frames =
                motion_table[anim->actions[anim->next].motion].motion->nbFrame;
            anim->start_frame =
                (((Float)next_frames * ((Float)frames - anim->frame)) /
                 (Float)frames) +
                ((Float)next_frames * PACE_LEAD);
            if (anim->start_frame >= (Float)next_frames) {
                anim->start_frame -= (Float)next_frames;
            }
        }
    } else {
        anim->steps++;
    }
    switch (anim->mode) {
    case PLAYER_ANIM_NEW:
        anim->frame = 0.0F;
        anim->current = index;
        anim->mode = PLAYER_ANIM_CHANGE;
        anim->frame = anim->start_frame = 0.0F;
        anim->blend = syMalloc(sizeof(NJS_MOTION));
        anim->blend->nbFrame = 2;
        anim->blend->type = NJD_MTYPE_POS_0 | NJD_MTYPE_ANG_1;
        anim->blend->inp_fn = 2;
        anim->blend->mdata = syCalloc(BLEND_NODES, sizeof(NJS_MDATA3));
        break;
    case PLAYER_ANIM_CHANGE:
        if (!no_blend && anim->actions[index].blend_step != 1.0F &&
            model_table[anim->actions[index].model].object ==
                model_table[anim->actions[anim->current].model].object) {
            player_anim_blend_from(anim);
            anim->frame = 0.0F;
        } else {
            anim->frame = 1.0F;
        }
        anim->current = index;
        anim->mode = PLAYER_ANIM_BLEND;
        /* fall through */
    case PLAYER_ANIM_BLEND:
        anim->frame += anim->actions[anim->current].blend_step;
        if (anim->frame >= 1.0F) {
            anim->mode = anim->actions[anim->current].mode;
            player_anim_free_blend(anim);
            player_anim_start(anim);
        }
        break;
    case PLAYER_ANIM_LOOP:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->actions[index].rate;
        while (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            anim->frame -= (Float)motion->nbFrame - 1.0F;
        }
        while (0.0F > anim->frame) {
            anim->frame += (Float)motion->nbFrame;
        }
        break;
    case PLAYER_ANIM_LOOP_SPEED:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->speed->x * anim->actions[index].rate;
        while (anim->frame >= (Float)motion->nbFrame) {
            anim->frame -= (Float)motion->nbFrame;
        }
        while (0.0F > anim->frame) {
            anim->frame += (Float)motion->nbFrame;
        }
        break;
    case PLAYER_ANIM_LOOP_PACE:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += *anim->pace * anim->actions[index].rate;
        while (anim->frame >= (Float)motion->nbFrame) {
            anim->frame -= (Float)motion->nbFrame;
        }
        while (0.0F > anim->frame) {
            anim->frame += (Float)motion->nbFrame;
        }
        break;
    case PLAYER_ANIM_PACE_ONCE:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += *anim->pace;
        if (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            player_anim_play_next(anim, index);
        }
        break;
    case PLAYER_ANIM_ONCE:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->actions[index].rate;
        if (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            player_anim_play_next(anim, index);
        } else if ((Float)motion->nbFrame - 1.0F <=
                   anim->frame + anim->actions[index].rate) {
            anim->flags |= PLAYER_ANIM_ENDING;
        }
        break;
    case PLAYER_ANIM_ONCE_BLEND:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->actions[index].rate;
        if (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            anim->after = anim->actions[index].next;
            anim->steps = 0;
            if (anim->actions[anim->after].blend_step != 1.0F &&
                model_table[anim->actions[index].model].object ==
                    model_table[anim->actions[anim->after].model].object) {
                index = anim->after;
                /* @hack type: index cut to its low byte here, and the next
                 * animation in the store of after below, where each whole
                 * would do */
                anim->next = (Uint8)index;
                player_anim_blend_from(anim);
                anim->frame = 0.0F;
            } else {
                anim->frame = 1.0F;
            }
            anim->current = index;
            anim->after = (Uint8)anim->actions[index].next;
            anim->mode = PLAYER_ANIM_BLEND;
            anim->frame += anim->actions[index].blend_step;
            if (anim->frame >= 1.0F) {
                anim->mode = anim->actions[index].mode;
                player_anim_free_blend(anim);
                player_anim_start(anim);
            }
        }
        break;
    case PLAYER_ANIM_BACK:
        /* @hack redundant: the motion's read, which nothing uses */
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame -= anim->actions[index].rate;
        if (0.0F > anim->frame) {
            player_anim_play_next(anim, index);
        }
        break;
    case PLAYER_ANIM_BLEND_AFTER:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->actions[index].rate;
        if (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            anim->steps = 0;
            if (anim->actions[anim->after].blend_step != 1.0F &&
                model_table[anim->actions[index].model].object ==
                    model_table[anim->actions[anim->after].model].object) {
                index = anim->after;
                /* @hack type: index cut to its low byte here, and the next
                 * animation in the store of after below, where each whole
                 * would do */
                anim->next = (Uint8)index;
                player_anim_blend_from(anim);
                anim->frame = 0.0F;
            } else {
                anim->frame = 1.0F;
            }
            anim->current = index;
            anim->after = (Uint8)anim->actions[index].next;
            anim->mode = PLAYER_ANIM_BLEND;
            anim->frame += anim->actions[index].blend_step;
            if (anim->frame >= 1.0F) {
                anim->mode = anim->actions[index].mode;
                player_anim_free_blend(anim);
                player_anim_start(anim);
            }
            anim->flags |= PLAYER_ANIM_DONE;
        }
        break;
    case PLAYER_ANIM_HOLD:
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame += anim->actions[index].rate;
        if (anim->frame >= (Float)motion->nbFrame - 1.0F) {
            anim->frame = (Float)motion->nbFrame - 1.0F;
            anim->flags |= PLAYER_ANIM_DONE;
        }
        break;
    case PLAYER_ANIM_BACK_HOLD:
        /* @hack redundant: the motion's read, which nothing uses */
        motion = motion_table[anim->actions[index].motion].motion;
        anim->frame -= anim->actions[index].rate;
        if (0.0F > anim->frame) {
            anim->frame = 0.0F;
            anim->flags |= PLAYER_ANIM_DONE;
        }
        break;
    case PLAYER_ANIM_DRIVEN:
        motion = motion_table[anim->actions[index].motion].motion;
        while (anim->frame >= (Float)motion->nbFrame) {
            anim->frame -= (Float)motion->nbFrame;
        }
        while (0.0F > anim->frame) {
            anim->frame += (Float)motion->nbFrame;
        }
        break;
    /* @hack redundant: the label of PLAYER_ANIM_END beside default, which
     * runs the jump table on to its mode */
    case PLAYER_ANIM_END:
    default:
        player_anim_free_blend(anim);
        if (anim->blend != NULL) {
            if (anim->blend->mdata != NULL) {
                syFree(anim->blend->mdata);
                anim->blend->mdata = NULL;
            }
            /* @hack redundant: blend tested again, which the test around
             * this one has found not null */
            if (anim->blend != NULL) {
                syFree(anim->blend);
                anim->blend = NULL;
            }
        }
        break;
    }
}

void player_anim_end_blend(struct player_anim *anim)
{
    if (anim->mode == PLAYER_ANIM_BLEND) {
        anim->mode = anim->actions[anim->current].mode;
        player_anim_free_blend(anim);
        player_anim_start(anim);
    }
}

/* end index, an animation that plays once. the anim plays index's next, in its
 * own mode from its start. that one's next follows it. the flags take
 * PLAYER_ANIM_DONE */
static void player_anim_play_next(struct player_anim *anim, Uint16 index)
{
    /* @hack type: the next animations cut to their low bytes, where the
     * whole indexes would do */
    anim->next = (Uint8)anim->actions[index].next;
    anim->current = anim->next;
    anim->mode = anim->actions[anim->current].mode;
    anim->after = (Uint8)anim->actions[anim->next].next;
    anim->flags |= PLAYER_ANIM_DONE;
    player_anim_start(anim);
}

/* start current at its first frame, or its last for a backward mode. a looping
 * or driven mode starts at start_frame, which then becomes 0. steps starts
 * over */
static void player_anim_start(struct player_anim *anim)
{
    if (anim->mode == PLAYER_ANIM_BACK || anim->mode == PLAYER_ANIM_BACK_HOLD) {
        anim->frame = (Float)motion_table[anim->actions[anim->current].motion]
                          .motion->nbFrame -
                      1.0F;
    } else if (anim->mode == PLAYER_ANIM_LOOP ||
               anim->mode == PLAYER_ANIM_LOOP_PACE ||
               anim->mode == PLAYER_ANIM_LOOP_SPEED ||
               anim->mode == PLAYER_ANIM_DRIVEN) {
        anim->frame = anim->start_frame;
        anim->start_frame = 0.0F;
    } else {
        anim->frame = 0.0F;
    }
    anim->steps = 0;
}

/* the bit of a motion's type for keys of scale. such a motion has three lists
 * of keys to a node */
#define MOTION_SCALES NJD_MTYPE_SCL_2

/* make the blend from current, at frame, into next, at start_frame */
static void player_anim_blend_from(struct player_anim *anim)
{
    NJS_MOTION *next;
    NJS_MOTION *prev;
    Uint16 next_lists;
    Uint16 prev_lists;
    Uint16 index = anim->next;
    Uint16 current = anim->current;

    next = motion_table[anim->actions[index].motion].motion;
    prev = motion_table[anim->actions[current].motion].motion;
    if (next->type & MOTION_SCALES) {
        next_lists = 3;
    } else {
        next_lists = 2;
    }
    if (prev->type & MOTION_SCALES) {
        prev_lists = 3;
    } else {
        prev_lists = 2;
    }
    /* @hack redundant: a test whose two arms make the same call, where the
     * call alone would do */
    if (next_lists == 3 || prev_lists == 3) {
        player_anim_make_blend(anim);
    } else {
        player_anim_make_blend(anim);
    }
}

/* narrow an angle in place to its low 16 bits, sign-extended */
/* @hack spelling: the angle read into wide and narrowed from it, where it
 * could be narrowed in place */
#define NARROW_ANGLE(ang)                                                      \
    {                                                                          \
        Angle wide = (ang);                                                    \
        (ang) = (Sint16)wide;                                                  \
    }

/* fill the blend's motion with two keys a node, for each node of next's model.
 * the first key holds the pose of current at frame. the second holds the pose
 * of next at start_frame. each is interpolated between its motion's keys. where
 * the motion has no keys for the node, the node's own pose stands in. each list
 * of keys comes from syMalloc, and a list it can't allocate is left empty. the
 * blend takes keys of scale where either motion has them. its angle keys turn
 * the short way */
static void player_anim_make_blend(struct player_anim *anim)
{
    NJS_OBJECT *nodes[BLEND_NODES];
    NJS_OBJECT *object;
    NJS_OBJECT *node;
    NJS_MOTION *next;
    NJS_MOTION *prev;
    Uint16 next_lists;
    Uint16 prev_lists;
    NJS_MDATA2 *src;
    NJS_MDATA2 *old;
    NJS_MDATA2 *dst;
    Uint32 i;
    Uint16 index = anim->next;
    Uint16 current = anim->current;

    object = (NJS_OBJECT *)model_table[anim->actions[index].model].object;
    tree_list_nodes(object, nodes);
    next = motion_table[anim->actions[index].motion].motion;
    prev = motion_table[anim->actions[current].motion].motion;
    if (next->type & MOTION_SCALES) {
        next_lists = 3;
    } else {
        next_lists = 2;
    }
    if (prev->type & MOTION_SCALES) {
        prev_lists = 3;
    } else {
        prev_lists = 2;
    }
    src = next->mdata;
    old = prev->mdata;
    dst = anim->blend->mdata;
    for (i = 0; i < (Uint32)motion_table[anim->actions[index].motion].nodes;
         i++) {
        NJS_MKEY_F *key;
        NJS_MKEY_A *akey;

        node = nodes[i];
        if ((src->p[0] != NULL || old->p[0] != NULL) &&
            (key = syMalloc(2 * sizeof(NJS_MKEY_F))) != NULL) {
            dst->p[0] = key;
            key->keyframe = 0;
            if (old->p[0] != NULL) {
                NJS_MKEY_F *keys = old->p[0];
                Uint32 nb = old->nb[0];
                Uint32 start = 0;
                Uint32 end = 0;
                Uint32 span;
                Float share;

                while (anim->frame > (Float)keys[end].keyframe) {
                    if (end >= nb - 1) {
                        end = 0;
                        start = nb - 1;
                        break;
                    }
                    start = end;
                    end++;
                }
                key->key[0] = keys[start].key[0];
                key->key[1] = keys[start].key[1];
                key->key[2] = keys[start].key[2];
                if (end != start) {
                    span = keys[end].keyframe - keys[start].keyframe;
                    if (end < start) {
                        span += prev->nbFrame;
                    }
                    share = (anim->frame - (Float)keys[start].keyframe) /
                            (Float)span;
                    key->key[0] +=
                        (keys[end].key[0] - keys[start].key[0]) * share;
                    key->key[1] +=
                        (keys[end].key[1] - keys[start].key[1]) * share;
                    key->key[2] +=
                        (keys[end].key[2] - keys[start].key[2]) * share;
                }
            } else {
                key->key[0] = node->pos[0];
                key->key[1] = node->pos[1];
                key->key[2] = node->pos[2];
            }
            key[1].keyframe = 1;
            if (src->p[0] != NULL) {
                NJS_MKEY_F *keys = src->p[0];

                if (anim->start_frame == 0.0F) {
                    key[1].key[0] = keys->key[0];
                    key[1].key[1] = keys->key[1];
                    key[1].key[2] = keys->key[2];
                } else {
                    Uint32 nb = src->nb[0];
                    Uint32 start = 0;
                    Uint32 end = 0;
                    Uint32 span;
                    Float share;

                    while (anim->start_frame > (Float)keys[end].keyframe) {
                        if (end >= nb - 1) {
                            end = 0;
                            start = nb - 1;
                            break;
                        }
                        start = end;
                        end++;
                    }
                    key[1].key[0] = keys[start].key[0];
                    key[1].key[1] = keys[start].key[1];
                    key[1].key[2] = keys[start].key[2];
                    if (end != start) {
                        span = keys[end].keyframe - keys[start].keyframe;
                        if (end < start) {
                            span += next->nbFrame;
                        }
                        share =
                            (anim->start_frame - (Float)keys[start].keyframe) /
                            (Float)span;
                        key[1].key[0] +=
                            (keys[end].key[0] - keys[start].key[0]) * share;
                        key[1].key[1] +=
                            (keys[end].key[1] - keys[start].key[1]) * share;
                        key[1].key[2] +=
                            (keys[end].key[2] - keys[start].key[2]) * share;
                    }
                }
            } else {
                key[1].key[0] = node->pos[0];
                key[1].key[1] = node->pos[1];
                key[1].key[2] = node->pos[2];
            }
            dst->nb[0] = 2;
        } else {
            dst->p[0] = NULL;
            dst->nb[0] = 0;
        }
        if ((src->p[1] != NULL || old->p[1] != NULL) &&
            (akey = syMalloc(2 * sizeof(NJS_MKEY_A))) != NULL) {
            dst->p[1] = akey;
            akey->keyframe = 0;
            if (old->p[1] != NULL) {
                NJS_MKEY_A *keys = old->p[1];
                Uint32 nb = old->nb[1];
                Uint32 start = 0;
                Uint32 end = 0;
                Uint32 span;
                Float share;

                while (anim->frame > (Float)keys[end].keyframe) {
                    if (end >= nb - 1) {
                        end = 0;
                        start = nb - 1;
                        break;
                    }
                    start = end;
                    end++;
                }
                akey->key[0] = keys[start].key[0];
                akey->key[1] = keys[start].key[1];
                akey->key[2] = keys[start].key[2];
                if (end != start) {
                    span = keys[end].keyframe - keys[start].keyframe;
                    if (end < start) {
                        span += prev->nbFrame;
                    }
                    share = (anim->frame - (Float)keys[start].keyframe) /
                            (Float)span;
                    akey->key[0] +=
                        (Angle)((Float)angle_difference(keys[start].key[0],
                                                        keys[end].key[0]) *
                                share);
                    akey->key[1] +=
                        (Angle)((Float)angle_difference(keys[start].key[1],
                                                        keys[end].key[1]) *
                                share);
                    akey->key[2] +=
                        (Angle)((Float)angle_difference(keys[start].key[2],
                                                        keys[end].key[2]) *
                                share);
                }
            } else {
                akey->key[0] = node->ang[0];
                akey->key[1] = node->ang[1];
                akey->key[2] = node->ang[2];
            }
            akey[1].keyframe = 1;
            if (src->p[1] != NULL) {
                NJS_MKEY_A *keys = src->p[1];

                if (anim->start_frame == 0.0F) {
                    akey[1].key[0] = keys->key[0];
                    akey[1].key[1] = keys->key[1];
                    akey[1].key[2] = keys->key[2];
                } else {
                    Uint32 nb = src->nb[1];
                    Uint32 start = 0;
                    Uint32 end = 0;
                    Uint32 span;
                    Float share;

                    while (anim->start_frame > (Float)keys[end].keyframe) {
                        if (end >= nb - 1) {
                            end = 0;
                            start = nb - 1;
                            break;
                        }
                        start = end;
                        end++;
                    }
                    akey[1].key[0] = keys[start].key[0];
                    akey[1].key[1] = keys[start].key[1];
                    akey[1].key[2] = keys[start].key[2];
                    if (end != start) {
                        span = keys[end].keyframe - keys[start].keyframe;
                        if (end < start) {
                            span += next->nbFrame;
                        }
                        share =
                            (anim->start_frame - (Float)keys[start].keyframe) /
                            (Float)span;
                        akey[1].key[0] +=
                            (Angle)((Float)angle_difference(keys[start].key[0],
                                                            keys[end].key[0]) *
                                    share);
                        akey[1].key[1] +=
                            (Angle)((Float)angle_difference(keys[start].key[1],
                                                            keys[end].key[1]) *
                                    share);
                        akey[1].key[2] +=
                            (Angle)((Float)angle_difference(keys[start].key[2],
                                                            keys[end].key[2]) *
                                    share);
                    }
                }
            } else {
                akey[1].key[0] = node->ang[0];
                akey[1].key[1] = node->ang[1];
                akey[1].key[2] = node->ang[2];
            }
            NARROW_ANGLE(akey->key[0]);
            NARROW_ANGLE(akey->key[1]);
            NARROW_ANGLE(akey->key[2]);
            NARROW_ANGLE(akey[1].key[0]);
            NARROW_ANGLE(akey[1].key[1]);
            NARROW_ANGLE(akey[1].key[2]);
            akey[1].key[0] = angle_unwrap(akey->key[0], akey[1].key[0]);
            akey[1].key[1] = angle_unwrap(akey->key[1], akey[1].key[1]);
            akey[1].key[2] = angle_unwrap(akey->key[2], akey[1].key[2]);
            dst->nb[1] = 2;
        } else {
            dst->p[1] = NULL;
            dst->nb[1] = 0;
        }
        if (prev_lists == 3 || next_lists == 3) {
            NJS_MDATA3 *dst3;
            NJS_MDATA3 *src3;
            NJS_MDATA3 *old3;
            NJS_MKEY_F *skey;

            src3 = (NJS_MDATA3 *)src;
            old3 = (NJS_MDATA3 *)old;
            /* @hack spelling: dst3 set after src3 and old3, where any order
             * would do */
            dst3 = (NJS_MDATA3 *)dst;
            dst3->nb[1] = dst->nb[1];
            dst3->nb[0] = dst->nb[0];
            if (((next_lists == 3 && src3->p[2] != NULL) ||
                 (prev_lists == 3 && old3->p[2] != NULL)) &&
                (skey = syMalloc(2 * sizeof(NJS_MKEY_F))) != NULL) {
                dst3->p[2] = skey;
                skey->keyframe = 0;
                if (prev_lists == 3 && old3->p[2] != NULL) {
                    NJS_MKEY_F *keys = old3->p[2];
                    Uint32 nb = old3->nb[2];
                    Uint32 start = 0;
                    Uint32 end = 0;
                    Uint32 span;
                    Float share;

                    while (anim->frame > (Float)keys[end].keyframe) {
                        if (end >= nb - 1) {
                            end = 0;
                            start = nb - 1;
                            break;
                        }
                        start = end;
                        end++;
                    }
                    skey->key[0] = keys[start].key[0];
                    skey->key[1] = keys[start].key[1];
                    skey->key[2] = keys[start].key[2];
                    if (end != start) {
                        span = keys[end].keyframe - keys[start].keyframe;
                        if (end < start) {
                            span += prev->nbFrame;
                        }
                        share = (anim->frame - (Float)keys[start].keyframe) /
                                (Float)span;
                        skey->key[0] +=
                            (keys[end].key[0] - keys[start].key[0]) * share;
                        skey->key[1] +=
                            (keys[end].key[1] - keys[start].key[1]) * share;
                        skey->key[2] +=
                            (keys[end].key[2] - keys[start].key[2]) * share;
                    }
                } else {
                    skey->key[0] = node->scl[0];
                    skey->key[1] = node->scl[1];
                    skey->key[2] = node->scl[2];
                }
                skey[1].keyframe = 1;
                if (next_lists == 3 && src3->p[2] != NULL) {
                    NJS_MKEY_F *keys = src3->p[2];

                    if (anim->start_frame == 0.0F) {
                        skey[1].key[0] = keys->key[0];
                        skey[1].key[1] = keys->key[1];
                        skey[1].key[2] = keys->key[2];
                    } else {
                        Uint32 nb = src3->nb[2];
                        Uint32 start = 0;
                        Uint32 end = 0;
                        Uint32 span;
                        Float share;

                        while (anim->start_frame > (Float)keys[end].keyframe) {
                            if (end >= nb - 1) {
                                end = 0;
                                start = nb - 1;
                                break;
                            }
                            start = end;
                            end++;
                        }
                        skey[1].key[0] = keys[start].key[0];
                        skey[1].key[1] = keys[start].key[1];
                        skey[1].key[2] = keys[start].key[2];
                        if (end != start) {
                            span = keys[end].keyframe - keys[start].keyframe;
                            if (end < start) {
                                span += next->nbFrame;
                            }
                            share = (anim->start_frame -
                                     (Float)keys[start].keyframe) /
                                    (Float)span;
                            skey[1].key[0] +=
                                (keys[end].key[0] - keys[start].key[0]) * share;
                            skey[1].key[1] +=
                                (keys[end].key[1] - keys[start].key[1]) * share;
                            skey[1].key[2] +=
                                (keys[end].key[2] - keys[start].key[2]) * share;
                        }
                    }
                } else {
                    skey[1].key[0] = node->scl[0];
                    skey[1].key[1] = node->scl[1];
                    skey[1].key[2] = node->scl[2];
                }
                dst3->nb[2] = 2;
            } else {
                dst3->p[2] = NULL;
                dst3->nb[2] = 0;
            }
        }
        if (next_lists == 3 || prev_lists == 3) {
            dst = (NJS_MDATA2 *)((NJS_MDATA3 *)dst + 1);
        } else {
            dst++;
        }
        if (next_lists == 3) {
            src = (NJS_MDATA2 *)((NJS_MDATA3 *)src + 1);
        } else {
            src++;
        }
        if (prev_lists == 3) {
            old = (NJS_MDATA2 *)((NJS_MDATA3 *)old + 1);
        } else {
            old++;
        }
    }
    if (next_lists == 3 || prev_lists == 3) {
        anim->blend->type = NJD_MTYPE_POS_0 | NJD_MTYPE_ANG_1 | NJD_MTYPE_SCL_2;
        anim->blend->inp_fn = 3;
    } else {
        anim->blend->type = NJD_MTYPE_POS_0 | NJD_MTYPE_ANG_1;
        anim->blend->inp_fn = 2;
    }
}

/* free the blend's lists of keys, two or three a node as its inp_fn says,
 * over the nodes of next's motion, and empty each list's pointer and its
 * count */
static void player_anim_free_blend(struct player_anim *anim)
{
    NJS_MOTION *motion = anim->blend;
    Uint16 lists = motion->inp_fn;
    /* the bytes from a list's pointer to its count */
    int size = lists * sizeof(void *);
    void **list = motion->mdata;
    int i;

    for (i = motion_table[anim->actions[anim->next].motion].nodes; i > 0; i--) {
        if (*list != NULL) {
            syFree(*list);
            *list = NULL;
            *(Uint32 *)((char *)list + size) = 0;
        }
        list++;
        if (*list != NULL) {
            syFree(*list);
            *list = NULL;
            *(Uint32 *)((char *)list + size) = 0;
        }
        list++;
        /* @hack spelling: two tests of lists, where one if and its else
         * would do */
        if (lists == 3) {
            if (*list != NULL) {
                syFree(*list);
                *list = NULL;
                *(Uint32 *)((char *)list + size) = 0;
            }
        }
        if (lists == 3) {
            list += 4;
        } else {
            list += 2;
        }
    }
}

/* the trail at which a missed Sonic or Shadow takes a higher ring level. at
 * TRAIL_NEAR or more it takes the second, and at TRAIL_FAR or more the third.
 * the trail is the distance between the players in slots 0 and 1 */
#define TRAIL_NEAR 1000.0F
#define TRAIL_FAR 2500.0F
/* the ring_level_timer that player_award_ring_level gives with a level, which
 * stage_clock_step counts down */
#define RING_LEVEL_STEPS 60

/* award the player in slot a ring level, by how far it trails the other of
 * slots 0 and 1. the level's byte of game_state.ring_levels is set, and the
 * other two are cleared. ring_level_timer takes RING_LEVEL_STEPS. a Sonic or a
 * Shadow takes its level by the distance between the two, as TRAIL_NEAR says.
 * a Knuckles or a Rouge in a hunt takes its level by the pieces each player
 * has taken. it takes the third where only the other has taken any, the first
 * where only it has, and the second otherwise. a piece taken with id 0 counts
 * as slot 0's, and any other as slot 1's. other characters, and a hunter
 * outside a hunt, change nothing */
static void player_award_ring_level(int slot)
{
    Float distance;
    int own;
    int other;
    int swap;
    int i;

    switch (player_records[slot]->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
        distance = njDistanceP2P(&player_work[0]->pos, &player_work[1]->pos);
        game_state.ring_levels[slot][0] = 0;
        game_state.ring_levels[slot][1] = 0;
        game_state.ring_levels[slot][2] = 0;
        if (distance >= TRAIL_FAR) {
            game_state.ring_levels[slot][2] = 1;
        } else if (distance >= TRAIL_NEAR) {
            game_state.ring_levels[slot][1] = 1;
        } else {
            game_state.ring_levels[slot][0] = 1;
        }
        game_state.ring_level_timer[slot] = RING_LEVEL_STEPS;
        break;
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        if (hunt_current) {
            own = other = 0;
            for (i = 0; i < HUNT_PIECES; i++) {
                if (hunt_current->pieces[i].kind == HUNT_PIECE_TAKEN) {
                    if (hunt_current->pieces[i].id == 0) {
                        own++;
                    } else {
                        other++;
                    }
                }
            }
            if (slot == 1) {
                swap = other;
                other = own;
                own = swap;
            }
            game_state.ring_levels[slot][0] = 0;
            game_state.ring_levels[slot][1] = 0;
            game_state.ring_levels[slot][2] = 0;
            if (own == 0 && other >= 1) {
                game_state.ring_levels[slot][2] = 1;
            } else if (own >= 1 && other == 0) {
                game_state.ring_levels[slot][0] = 1;
            } else {
                game_state.ring_levels[slot][1] = 1;
            }
            game_state.ring_level_timer[slot] = RING_LEVEL_STEPS;
        }
        break;
    }
}

/* a miss's task, or a sink's, makes its miss take effect at its 122nd step.
 * that is its first step with more than MISS_STEPS steps before it */
#define MISS_STEPS 120

/* the extra of a GamePlayerMissed task, and of the tasks of a player that
 * sinks */
struct miss {
    Sint8 slot; /* the player's slot */
    Uint8 unk_01;
    Sint16 steps; /* the steps the task has taken */
    /* a sink's: the height of the surface the player sinks through */
    Float surface;
};
#define MISS(task) ((struct miss *)(task)->extra.p)

/* the extra of a KillPlayerFallingDownStageP task */
struct kill_areas {
    Uint8 unk_00;
    /* the slots whose misses have taken effect, bit n for slot n. a step passes
     * over the player of a set bit, and clears the bit. a miss's task that is
     * this task's child sets the field to its slot's bit alone */
    Sint8 ended;
    Uint8 unk_02[2];
    struct kill_area *areas;
};

struct task *kill_areas_task = NULL;
struct kill_area *kill_areas = NULL;

static void miss_destroy(struct task *task)
{
}

static void miss_step(struct task *task)
{
    struct miss *miss = task->extra.p;
    struct kill_areas *parent;
    int slot;
    int other;

    if (task->parent != NULL) {
        parent = task->parent->extra.p;
    } else {
        parent = NULL;
    }
    /* @hack spelling: the work's mode tested in an arm of its own, where
     * one || with the clear's test would do */
    if (game_state.cleared == 1) {
        task_kill(task);
    } else if (player_work[miss->slot]->mode == PLAYER_MODE_EVENT) {
        task_kill(task);
    } else if (miss->steps++ > MISS_STEPS) {
        /* @hack spelling: slot set inside its test, where a statement of
         * its own would do */
        if ((slot = miss->slot) == 0) {
            other = 1;
        } else {
            other = 0;
        }
        if (!game_state.two_players) {
            if (player_records[other] != NULL &&
                (player_records[other]->flags & PLAYER_MISSED)) {
                task_kill(task);
                return;
            }
        }
        player_records[slot]->flags |= PLAYER_MISSED;
        if (!game_state.two_players) {
            if (player_work[slot]->id == WORK_ID_RIVAL) {
                stage_clear(player_find_rival(slot));
            } else {
                stage_end(STAGE_END_MISS);
            }
        } else if (stage_character(0) == PLAYER_EGGMAN_MECH ||
                   stage_character(0) == PLAYER_TAILS_MECH) {
            stage_clear(player_find_rival(slot));
            /* @hack redundant: PLAYER_MISSED set a second time, which the
             * store above has set already */
            player_records[slot]->flags |= PLAYER_MISSED;
        } else if (game_state.cleared != 1) {
            player_reset(slot);
            stage_place_player(slot);
            ring_add(slot, -ring_count(slot));
            camera_reset_modes(slot);
            player_award_ring_level(slot);
        }
        /* @hack spelling: the kill in each arm, where one after the if
         * would do */
        if (parent != NULL) {
            /* @bug logic: the store sets ended to the slot's bit alone
             * (0x8c047cf8), so a miss that ends before the parent's next
             * step clears the bit another slot's miss set. kill_areas_step
             * keeps ended as a set of bits, and clears only its slot's
             * (0x8c047e38) */
            parent->ended = 1 << miss->slot;
            task_kill(task);
        } else {
            task_kill(task);
        }
    }
}

static void kill_areas_destroy(struct task *task)
{
    kill_areas = NULL;
    kill_areas_task = NULL;
}

/* the reach of a surface above a player within which the player meets the
 * area */
#define KILL_AREA_REACH 30.0F

static void kill_areas_step(struct task *task)
{
    struct kill_areas *extra = task->extra.p;
    struct kill_area *area;
    struct task_work *work;
    struct player *record;
    struct height_probe probe;
    Uint32 characters;
    int slot;
    Bool sinks;

    if (game_state.cleared == 1) {
        return;
    }
    for (slot = 0; slot < 2; slot++) {
        sinks = FALSE;
        work = player_work[slot];
        area = extra->areas;
        if (area == NULL || area->object == NULL) {
            return;
        }
        /* @hack spelling: record set inside its test, where a statement of
         * its own would do */
        if ((record = player_records[slot]) != NULL &&
            (record->flags & PLAYER_MISSED)) {
            /* on these stages alone, a player that has missed already meets
             * a KILL_AREA_SINK area. it stays sunk in it */
            switch (stage_current_id()) {
            case STAGE_SAND_OCEAN:
            case STAGE_HIDDEN_BASE:
            case STAGE_SAND_OCEAN_2P:
            case STAGE_BOSS_GOLEM:
            case STAGE_BOSS_GOLEM_E:
                sinks = TRUE;
                break;
            }
        }
        if (work == NULL ||
            ((player_records[slot]->flags & PLAYER_MISSED) && !sinks) ||
            (task->child != NULL && (MISS(task->child)->slot == slot ||
                                     MISS(task->child->next)->slot == slot)) ||
            (extra->ended & (1 << slot))) {
            if (extra->ended & (1 << slot)) {
                extra->ended &= ~(1 << slot);
            }
            continue;
        }
        characters = 1 << stage_character(slot);
        probe.pos = work->pos;
        for (; area->object != NULL; area++) {
            if (area->flags & KILL_AREA_DRAWN) {
                njCnkEasyDrawObject((NJS_CNK_OBJECT *)area->object);
            } else if ((area->flags & characters) &&
                       polygon_find_model_heights(&probe, area->object) &&
                       ((probe.below.found &&
                         (area->flags & KILL_AREA_BELOW)) ||
                        (probe.above.found &&
                         ((area->flags & (KILL_AREA_BELOW | KILL_AREA_ABOVE)) ||
                          _builtin_fabsf(probe.pos.y - probe.above.pos) <=
                              KILL_AREA_REACH)))) {
                if (area->flags & KILL_AREA_SINK) {
                    if (sinks) {
                        player_keep_sunk(task, slot, probe.above.pos);
                    } else {
                        player_sink(task, slot, probe.above.pos);
                    }
                } else if (area->flags & KILL_AREA_WATER) {
#ifdef NONMATCHING
                    /* player_sink_in_water splashes on this side, once its
                     * tasks exist */
                    player_sink_in_water(task, slot, &probe.pos);
#else
                    player_splash(slot, &probe.pos, NULL);
                    player_sink_in_water(task, slot);
#endif
                } else {
                    player_fall_in_area(task, slot);
                }
            } else {
                /* @hack spelling: an empty else, which nothing needs */
            }
        }
    }
}

Bool kill_areas_start(struct kill_area *areas)
{
    struct task *task;
    struct kill_areas *extra;

    task = task_create(0, 0, kill_areas_step, "KillPlayerFallingDownStageP");
    if (task == NULL) {
        return FALSE;
    }
    extra = syCalloc(1, sizeof(struct kill_areas));
    if (extra == NULL) {
        task_destroy(task);
        return FALSE;
    }
    task->extra.p = extra;
    kill_areas_task = task;
    kill_areas = extra->areas = areas;
    task->destructor = kill_areas_destroy;
    return TRUE;
}

/* the gauge player_miss leaves a mech in a one-player game whose other
 * player has missed already */
#define MISS_SPARED_GAUGE 0.01F

void player_miss(int slot)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    struct player *other;
    struct task *task;
    int other_slot;

    if (record->gauge > 0.0F) {
        return;
    }
    if (slot == 0) {
        other_slot = 1;
    } else {
        other_slot = 0;
    }
    if (!game_state.two_players) {
        /* @hack spelling: other set inside its test, where a statement of
         * its own would do */
        if ((other = player_records[other_slot]) != NULL &&
            (other->flags & PLAYER_MISSED)) {
            record->gauge = MISS_SPARED_GAUGE;
            return;
        }
    }
    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_MISS;
    player_records[slot]->flags |= PLAYER_MISSED;
    task = task_create(TASK_EXTRA, 0, miss_step, "GamePlayerMissed");
    if (task != NULL) {
        MISS(task)->slot = slot;
    }
    if (game_state.two_players == 0) {
        pause_disable();
        stage_clock_stop();
        return;
    }
    if (stage_character(0) == PLAYER_EGGMAN_MECH ||
        stage_character(0) == PLAYER_TAILS_MECH) {
        pause_disable();
        stage_clock_stop();
    }
}

/* the voice each kind of player cries out with as it misses */
#define VOICE_MISS_SONIC 1571
#define VOICE_MISS_SHADOW 1568
#define VOICE_MISS_EGGMAN 1678
#define VOICE_MISS_TAILS 1679
#define VOICE_MISS_KNUCKLES 1304
#define VOICE_MISS_ROUGE 1305
#define VOICE_MISS_AMY 2666

/* stop the camera following the player in slot, and play its cry. a Knuckles, a
 * Rouge, a mech, a Tical or a Chaos 0 also requests PLAYER_REQUEST_RELEASE. in
 * a one-player game, the pause menu closes, and the stage's clock stops */
static void miss_cry(int slot)
{
    struct player *record = player_records[slot];
    int voice = -1;

    camera_push_mode(slot, CAMERA_MODE_LEAVE);
    camera_set_adjust(slot, camera_top(slot), CAMERA_ADJUST_NONE);
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_MISS_SONIC;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_MISS_SHADOW;
        break;
    case PLAYER_EGGMAN_MECH:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        /* fall through */
    case PLAYER_EGGMAN:
        voice = VOICE_MISS_EGGMAN;
        break;
    case PLAYER_TAILS_MECH:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        /* fall through */
    case PLAYER_TAILS:
        voice = VOICE_MISS_TAILS;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_MISS_KNUCKLES;
        /* fall through */
    case PLAYER_KIND_TICAL:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        break;
    case PLAYER_ROUGE:
        voice = VOICE_MISS_ROUGE;
        /* fall through */
    case PLAYER_KIND_CHAOS0:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_MISS_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
    if (game_state.two_players == 0) {
        pause_disable();
        stage_clock_stop();
    }
}

void player_fall_out(int slot)
{
    struct player *record = player_records[slot];
    struct task *task;

    /* @bug memory: an empty slot reads its null record's flags (0x8c048252),
     * since the test takes || where && is meant (0x8c04824e). the same ||
     * misses again a player that has missed already. player_sink_in_water's
     * step tests the same two things with && */
    if (record != NULL || !(record->flags & PLAYER_MISSED)) {
        if (kill_areas_task != NULL) {
            task = task_create_child(TASK_EXTRA, miss_step, kill_areas_task);
        } else {
            task = task_create(TASK_EXTRA, 0, miss_step, "GamePlayerMissed");
        }
        if (task != NULL) {
            MISS(task)->slot = slot;
            task->destructor = miss_destroy;
        }
        miss_cry(slot);
    }
}

#ifdef NONMATCHING
/* An area owns one miss task per player. A sinking player also needs a
 * child to move it. Leave the area clear to retry if either allocation fails.
 */
static struct task *area_miss_create(struct task *parent, int slot,
                                     void (*step)(struct task *),
                                     void (*sink_step)(struct task *))
{
    struct task *task = task_create_child(TASK_EXTRA, step, parent);
    struct task *sink;

    if (task == NULL) {
        return NULL;
    }
    MISS(task)->slot = slot;
    if (sink_step != NULL) {
        sink = task_create_child(TASK_EXTRA, sink_step, task);
        if (sink == NULL) {
            task_destroy(task);
            return NULL;
        }
        MISS(sink)->slot = slot;
    }
    task->destructor = miss_destroy;
    return task;
}
#endif

static void player_fall_in_area(struct task *parent, int slot)
{
#ifdef NONMATCHING
    /* this side repairs the tag below: area_miss_create makes the task or
     * returns null, and this side then cries no miss */
    if (area_miss_create(parent, slot, miss_step, NULL) == NULL) {
        return;
    }
#else
    struct task *task;

    task = task_create_child(TASK_EXTRA, miss_step, parent);
    if (task != NULL) {
        MISS(task)->slot = slot;
    }
    /* @bug memory: a child task_create_child failed to make is written
     * through its null (0x8c0482e8) */
    task->destructor = miss_destroy;
#endif
    miss_cry(slot);
}

/* the fall of a player sinking in water, by its spd.y. a fall below
 * WATER_FALL_LIMIT becomes WATER_FALL_SPEED at once, and its motion's acc 0.
 * any other fall takes an acc of WATER_LIFT of the constants' gravity, against
 * world_gravity. one below WATER_FALL_SLOW also keeps WATER_FALL_DRAG of itself
 * a step. a player that isn't falling is left as it is */
#define WATER_FALL_LIMIT (-3.0F)
#define WATER_FALL_SPEED (-2.0F)
#define WATER_FALL_SLOW (-0.2F)
#define WATER_FALL_DRAG 0.95F
#define WATER_LIFT 0.8F

static void water_sink_step(struct task *task)
{
    struct miss *sink = task->extra.p;
    int slot = sink->slot;
    struct player *record = player_records[slot];
    NJS_VECTOR lift;
    Float scale;
    Float rate;

    if (record != NULL && !(record->flags & PLAYER_MISSED)) {
        if (record->spd.y < WATER_FALL_LIMIT) {
            record->spd.y = WATER_FALL_SPEED;
            scale = 0.0F;
        } else {
            /* @hack spelling: the lift's share in a local, where its use
             * could take the constant */
            rate = WATER_LIFT;
            if (record->spd.y < WATER_FALL_SLOW) {
                record->spd.y *= WATER_FALL_DRAG;
            } else if (!(record->spd.y < 0.0F)) {
                return;
            }
            scale = -(record->consts.gravity * rate);
        }
        lift.x = world_gravity.x * scale;
        lift.y = world_gravity.y * scale;
        lift.z = world_gravity.z * scale;
        player_set_acc(slot, lift.x, lift.y, lift.z);
    } else if (record != NULL && (record->flags & PLAYER_MISSED)) {
        task_kill(task);
    }
}

#ifdef NONMATCHING
/* pos is where it splashes, once its tasks exist */
static void player_sink_in_water(struct task *parent, int slot, NJS_POINT3 *pos)
#else
static void player_sink_in_water(struct task *parent, int slot)
#endif
{
#ifdef NONMATCHING
    /* this side repairs the tag below: area_miss_create makes both tasks or
     * neither, and this side splashes and cries only where it made them */
    if (area_miss_create(parent, slot, miss_step, water_sink_step) == NULL) {
        return;
    }
    player_splash(slot, pos, NULL);
    miss_cry(slot);
#else
    struct task *task;
    struct task *sink;

    task = task_create_child(TASK_EXTRA, miss_step, parent);
    if (task != NULL) {
        MISS(task)->slot = slot;
    }
    /* @bug memory: a child task_create_child failed to make is written
     * through its null, and taken for the parent of the sink's task
     * (0x8c048422) */
    task->destructor = miss_destroy;
    miss_cry(slot);
    sink = task_create_child(TASK_EXTRA, water_sink_step, task);
    if (sink != NULL) {
        MISS(sink)->slot = slot;
    }
#endif
}

/* miss_cry for a player that sinks, which leaves the camera as it is. a Sonic,
 * a Shadow or a Metal Sonic also plays ANIM_SINK. a mech plays its own sinking
 * animation. each takes it through its anim's next */
static void sink_cry(int slot)
{
    struct player *record = player_records[slot];
    int voice = -1;

    camera_push_mode(slot, CAMERA_MODE_LEAVE);
    camera_set_adjust(slot, camera_top(slot), CAMERA_ADJUST_NONE);
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_MISS_SONIC;
        record->anim.next = ANIM_SINK;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_MISS_SHADOW;
        /* fall through */
    case PLAYER_KIND_METALSONIC:
        record->anim.next = ANIM_SINK;
        break;
    case PLAYER_EGGMAN_MECH:
        record->anim.next = ANIM_SINK_EGGMAN_MECH;
        /* fall through */
    case PLAYER_EGGMAN:
        voice = VOICE_MISS_EGGMAN;
        break;
    case PLAYER_TAILS_MECH:
        record->anim.next = ANIM_SINK_TAILS_MECH;
        /* fall through */
    case PLAYER_TAILS:
        voice = VOICE_MISS_TAILS;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_MISS_KNUCKLES;
        /* fall through */
    case PLAYER_KIND_TICAL:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        break;
    case PLAYER_ROUGE:
        voice = VOICE_MISS_ROUGE;
        /* fall through */
    case PLAYER_KIND_CHAOS0:
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_MISS_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
    if (game_state.two_players == 0) {
        pause_disable();
        stage_clock_stop();
    }
}

/* how far along world_gravity a sinking player moves a step. it moves
 * SINK_RATE, or SINK_DEEP_RATE once it lies SINK_DEPTH or more below the
 * surface. on STAGE_SAND_OCEAN it also drifts SINK_DRIFT along +z, unless its
 * wall_normal points against +z */
#define SINK_RATE 0.25F
#define SINK_DEEP_RATE 0.07F
#define SINK_DEPTH 10.0F
#define SINK_DRIFT 0.5F

static void sunk_step(struct task *task)
{
    struct miss *sink = task->extra.p;
    int slot = sink->slot;
    struct player *record = player_records[slot];
    NJS_POINT3 pos;
    NJS_POINT3 past;
    Float rate;

    if (record != NULL && (record->flags & PLAYER_MISSED)) {
        player_get_past(slot, 1, &past, NULL);
        pos = player_work[slot]->pos;
        if (past.y + SINK_DEPTH <= sink->surface) {
            rate = SINK_DEEP_RATE;
        } else {
            rate = SINK_RATE;
        }
        pos.x += world_gravity.x * rate;
        pos.y = past.y + (world_gravity.y * rate);
        pos.z += world_gravity.z * rate;
        if (stage_current_id() == STAGE_SAND_OCEAN) {
            NJS_POINT3 ahead = {0.0F, 0.0F, 1.0F};

            if (!(njInnerProduct(&record->wall_normal, &ahead) < 0.0F)) {
                pos.z += SINK_DRIFT;
            }
        }
        /* @bug logic: the player is put at its height of a step before,
         * and pos.y, that height moved along world_gravity, goes unused,
         * where sink_step moves the player by its y (0x8c0486ac) */
        player_set_pos(slot, pos.x, past.y, pos.z);
    } else if (record != NULL && !(record->flags & PLAYER_MISSED)) {
        task_kill(task);
    }
}

static void sink_step(struct task *task)
{
    struct miss *sink = task->extra.p;
    int slot = sink->slot;
    struct player *record = player_records[slot];
    NJS_POINT3 pos;
    Float rate;

    if (record != NULL && !(record->flags & PLAYER_MISSED)) {
        pos = player_work[slot]->pos;
        if (pos.y + SINK_DEPTH <= sink->surface) {
            rate = SINK_DEEP_RATE;
        } else {
            rate = SINK_RATE;
        }
        pos.x += world_gravity.x * rate;
        pos.y += world_gravity.y * rate;
        pos.z += world_gravity.z * rate;
        if (stage_current_id() == STAGE_SAND_OCEAN) {
            NJS_POINT3 ahead = {0.0F, 0.0F, 1.0F};

            if (!(njInnerProduct(&record->wall_normal, &ahead) < 0.0F)) {
                pos.z += SINK_DRIFT;
            }
        }
        player_set_pos(slot, pos.x, pos.y, pos.z);
    } else if (record != NULL && (record->flags & PLAYER_MISSED)) {
        task_kill(task);
    }
}

/* the step at which a sinking player cries out */
#define SINK_CRY_STEPS 60

static void sink_miss_step(struct task *task)
{
    struct miss *sink = task->extra.p;
    /* @hack spelling: the slot read through task's extra, where sink would
     * do */
    int slot = ((struct miss *)task->extra.p)->slot;
    struct kill_areas *parent;

    if (task->parent != NULL) {
        parent = task->parent->extra.p;
    } else {
        parent = NULL;
    }
    if (game_state.cleared == 1) {
        task_kill(task);
        return;
    }
    if (player_work[slot]->mode != PLAYER_MODE_UNK_38 &&
        player_work[slot]->mode != PLAYER_MODE_MISS) {
        task_kill(task);
        return;
    }
    if (sink->steps++ > MISS_STEPS) {
        player_records[slot]->flags |= PLAYER_MISSED;
        if (!game_state.two_players) {
            if (player_work[slot]->id == WORK_ID_RIVAL) {
                stage_clear(player_find_rival(slot));
            } else {
                stage_end(STAGE_END_MISS);
            }
        } else if (stage_character(0) == PLAYER_EGGMAN_MECH ||
                   stage_character(0) == PLAYER_TAILS_MECH) {
            stage_clear(player_find_rival(slot));
        } else {
            player_reset(slot);
            stage_place_player(slot);
            camera_reset_modes(slot);
            player_award_ring_level(slot);
        }
        /* @hack spelling: the kill in each arm, where one after the if
         * would do */
        if (parent != NULL) {
            /* @bug logic: the store sets ended to the slot's bit alone
             * (0x8c0488de), so a sink that ends before the parent's next
             * step clears the bit another slot's miss set. kill_areas_step
             * keeps ended as a set of bits, and clears only its slot's
             * (0x8c047e38) */
            parent->ended = 1 << sink->slot;
            task_kill(task);
            return;
        }
        task_kill(task);
        return;
    }
    if (sink->steps == SINK_CRY_STEPS) {
        sink_cry(slot);
    }
}

static void player_sink(struct task *parent, int slot, Float surface)
{
#ifdef NONMATCHING
    /* this side repairs the tag below: area_miss_create makes both tasks or
     * neither, and this side requests no sink where it made none */
    struct task *task =
        area_miss_create(parent, slot, sink_miss_step, sink_step);

    if (task == NULL) {
        return;
    }
    MISS(task->child)->surface = surface;
#else
    struct task *task;
    struct task *sink;

    task = task_create_child(TASK_EXTRA, sink_miss_step, parent);
    if (task != NULL) {
        MISS(task)->slot = slot;
    }
    /* @bug memory: a child task_create_child failed to make is written
     * through its null, and taken for the parent of the sink's task
     * (0x8c048986) */
    task->destructor = miss_destroy;
    sink = task_create_child(TASK_EXTRA, sink_step, task);
    if (sink != NULL) {
        MISS(sink)->slot = slot;
        MISS(sink)->surface = surface;
    }
#endif
    player_request(slot, PLAYER_REQUEST_SINK, 0);
}

static void player_keep_sunk(struct task *parent, int slot, Float surface)
{
    struct task *task;

    task = task_create_child(TASK_EXTRA, sunk_step, parent);
    if (task != NULL) {
        MISS(task)->slot = slot;
        MISS(task)->surface = surface;
    }
}

/* the steps of a breath task. from BREATH_COUNT_STEPS, the count over the
 * player shows BREATH_COUNTS down to 1, a number each BREATH_NUMBER_STEPS
 * steps. the player drowns at BREATH_DROWN_STEPS, and the task ends at
 * BREATH_END_STEPS */
#define BREATH_COUNT_STEPS 360
#define BREATH_COUNTS 5
#define BREATH_NUMBER_STEPS 60
#define BREATH_DROWN_STEPS 720
#define BREATH_END_STEPS 900
/* a number shows at BREATH_SCALE times its size. over the first
 * BREATH_GROWTH of its steps it grows by BREATH_GROWTH of that, and then holds
 * there */
#define BREATH_SCALE 2.0F
#define BREATH_GROWTH 0.3F
/* how far above the player's pos on the screen the count shows, in
 * pixels */
#define BREATH_RISE 96.0F
/* the underwater_steps under which a player has breathed again */
#define BREATH_TAKEN 60
/* the count's colour, an opaque yellow */
#define BREATH_COLOR 0xffffff60

/* the count over a drowning player */
/* clang-format off */
static struct hud_number breath_number = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED, 0, 0, BREATH_COUNTS, 0,
    {0.0F, 0.0F, 0.0F}, 1.0F, BREATH_COLOR,
};
/* clang-format on */

static void breath_count_draw(struct task *task)
{
    struct breath *breath = task->extra.p;
    int steps = breath->steps;
    int slot = breath->slot;
    Float part;
    struct task_work *work;
    int count;
    NJS_POINT3 pos;
    NJS_POINT2 screen;

    /* @hack spelling: steps less BREATH_COUNT_STEPS stored back to steps,
     * where a local of its own would do */
    steps = steps - BREATH_COUNT_STEPS;
    part = (Float)(steps % BREATH_NUMBER_STEPS) * (1.0F / BREATH_NUMBER_STEPS);
    /* @hack spelling: work set by a comma inside count's statement, where
     * a statement of its own would do */
    count = (work = player_work[slot], steps / BREATH_NUMBER_STEPS);
    if (count < BREATH_COUNTS && count >= 0) {
        breath_number.value = BREATH_COUNTS - count;
        if (!(part < BREATH_GROWTH)) {
            part = BREATH_GROWTH;
        }
        breath_number.scale = (part + 1.0F) * BREATH_SCALE;
        pos = work->pos;
        njProjectScreen(NULL, &pos, &screen);
        screen.y -= BREATH_RISE;
        breath_number.pos.x = screen.x;
        breath_number.pos.y = screen.y;
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        hud_number_draw(&breath_number);
        njFogEnable();
    }
}

static void breath_destroy(struct task *task)
{
    if (BREATH(task)->music_paused == 0) {
        music_drop_played();
    }
}

static void music_pause_step(struct task *task)
{
}

static void breath_music_resume(struct task *task)
{
    music_resume();
}

/* the voice each kind of player cries out with as it drowns */
#define VOICE_DROWN_SONIC 1569
#define VOICE_DROWN_SHADOW 1566
#define VOICE_DROWN_KNUCKLES 1302
#define VOICE_DROWN_ROUGE 1303
/* the sound of a player drowning, which plays at the place its record's
 * unk_218 holds */
#define SOUND_DROWN 0x7008

static void breath_step(struct task *task)
{
    struct breath *breath = task->extra.p;
    int slot = breath->slot;
    Uint32 steps = breath->steps++;
    int voice;

    if ((steps < BREATH_DROWN_STEPS &&
         player_records[slot]->underwater_steps < BREATH_TAKEN) ||
        game_state.cleared == 1) {
        task_kill(task);
        return;
    }
    /* @hack spelling: a return after the drown's arm, and the first step's
     * if apart from the chain, where an else-if chain would do */
    if (steps == BREATH_DROWN_STEPS) {
        voice = -1;
        player_request(slot, PLAYER_REQUEST_DROWN, 0);
        player_records[slot]->flags |= PLAYER_MISSED;
        switch (player_records[slot]->kind) {
        case PLAYER_SONIC:
            voice = VOICE_DROWN_SONIC;
            break;
        case PLAYER_SHADOW:
            voice = VOICE_DROWN_SHADOW;
            break;
        case PLAYER_KNUCKLES:
            voice = VOICE_DROWN_KNUCKLES;
            break;
        case PLAYER_ROUGE:
            voice = VOICE_DROWN_ROUGE;
            break;
        }
        if (voice != -1) {
            voice_play_priority_2(voice);
        }
        if (game_state.two_players == 0) {
            music_pause();
            /* @hack spelling: the flag stored through BREATH(task), where
             * breath would do */
            BREATH(task)->music_paused = 1;
            pause_disable();
            task_create(0, 2, music_pause_step, "plBGMPauseExecutor");
            task->destructor = breath_music_resume;
            stage_clock_stop();
        }
        sound_play_at(SOUND_DROWN, NULL, 0, 0, &player_records[slot]->unk_218);
        return;
    } else if (steps == BREATH_END_STEPS) {
        if (game_state.two_players == 0) {
            stage_end(STAGE_END_MISS);
        }
        task_kill(task);
        return;
    }
    if (steps < 1) {
        task->destructor = breath_destroy;
        task->draw = breath_count_draw;
        if (game_state.two_players == 0) {
            jingle_play("timer.adx");
        }
    }
}

void player_reset(int slot)
{
    struct task_work *work;
    struct player *record;
    struct collision_work *collision;

    if (player_tasks[slot] != NULL) {
        work = player_work[slot];
        record = player_records[slot];
        player_request(slot, PLAYER_REQUEST_RESET, 0);
        if (work != NULL) {
            collision = work->collision;
            if (collision != NULL) {
                collision->hit_shape = -1;
                collision->hit = NULL;
                collision->flags &= ~COLLISION_TOUCH_FLAGS;
            }
            work->flags &= ~PLAYER_RESET_WORK_FLAGS;
            work->timer = 0;
            record->ground_flags = 0;
        }
        if (record != NULL) {
            if ((record->flags & PLAYER_MISSED) && !game_state.two_players) {
                lives_add(0, -1);
            }
            record->flags &= ~PLAYER_RESET_FLAGS;
            walker_fill_gauge(record);
            player_release_held(work, player_motion[record->slot], record);
        }
    }
}

void player_knock_back(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_KNOCK_BACK;
    player_records[slot]->request_arg = 0;
    player_face_along(slot, -x, 0.0F, -z);
    player_set_speed(slot, x, y, z);
}

void player_knock_apart(int slot, int other, Float speed)
{
    struct task_work *work = player_work[slot];
    struct task_work *other_work = player_work[other];
    NJS_VECTOR away;

    away.x = work->pos.x - other_work->pos.x;
    away.y = work->pos.y - other_work->pos.y;
    away.z = work->pos.z - other_work->pos.z;
    njUnitVector(&away);
    away.x *= speed;
    away.y = 0.0F;
    away.z *= speed;
    player_knock_back(slot, away.x, away.y, away.z);
    player_knock_back(other, -away.x, -away.y, -away.z);
}

void player_look_at_task(int slot, struct task *target, NJS_POINT3 *offset,
                         int steps)
{
    struct player *record = player_records[slot];

    if (record != NULL && target != NULL) {
        record->look_mode = PLAYER_LOOK_TASK;
        record->look_task = target;
        if (offset != NULL) {
            record->look_pos = *offset;
        } else {
            record->look_pos.x = record->look_pos.y = record->look_pos.z = 0.0F;
        }
        record->look_steps = steps;
    }
}

void player_look_at_point(int slot, NJS_POINT3 *pos, int steps)
{
    struct player *record = player_records[slot];

    if (record != NULL && pos != NULL) {
        record->look_mode = PLAYER_LOOK_POINT;
        record->look_pos = *pos;
        record->look_steps = steps;
    }
}

void player_look_at_angles(int slot, struct angles *ang, int steps)
{
    struct player *record = player_records[slot];

    if (record != NULL && ang != NULL) {
        record->look_mode = PLAYER_LOOK_ANGLES;
        record->look_ang = *ang;
        record->look_steps = steps;
    }
}

void player_clear_look(int slot)
{
    struct player *record = player_records[slot];

    if (record != NULL) {
        record->look_mode = PLAYER_LOOK_NONE;
        record->look_steps = 0;
    }
}

void player_request(int slot, int request, int arg)
{
    struct task_work *work = player_work[slot];

    if (work != NULL) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = request;
        player_records[slot]->request_arg = arg;
    }
}

void player_set_speed(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR spd;

    motion->spd.x = x;
    motion->spd.y = y;
    motion->spd.z = z;
    spd.x = x;
    spd.y = y;
    spd.z = z;
    turn_from_world(work, &spd);
    if (spd.x <= SPEED_NONE && spd.x >= -SPEED_NONE) {
        spd.x = 0.0F;
    }
    if (spd.z <= SPEED_NONE && spd.z >= -SPEED_NONE) {
        spd.z = 0.0F;
    }
    record->spd = spd;
}

void player_set_local_speed(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    /* @hack redundant: a vector nothing uses, which sizes the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */

    record->spd.x = x;
    record->spd.y = y;
    record->spd.z = z;
    motion->spd = record->spd;
    turn_to_world(work, &motion->spd);
}

/* TRUE where the player's work holds PLAYER_REQUEST_RESET, waiting */
#define RESET_WAITS(work)                                                      \
    (((work)->flags & PLAYER_WORK_REQUEST) &&                                  \
     (work)->submode == PLAYER_REQUEST_RESET)

void player_bounce(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];

    if (!RESET_WAITS(work)) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_BOUNCE;
        player_set_speed(slot, x, y, z);
    }
}

void player_launch(int slot, Float speed, struct angles *ang, Sint16 lock)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR spd;

    if (!RESET_WAITS(work)) {
        record->stick_lock = lock;
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_LAUNCH;
        spd.x = 0.0F;
        spd.y = speed;
        spd.z = 0.0F;
        record->spd = spd;
        work->ang.x = ang->x;
        work->ang.y = QUARTER_TURN - ang->y;
        motion->aim_y = work->ang.y;
        work->ang.z = ang->z;
        turn_to_world(work, &spd);
        motion->spd = spd;
    }
}

void player_fling(int slot, Float speed, NJS_VECTOR *dir)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR spd;

    if (!RESET_WAITS(work)) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_FLING;
        player_set_angles(
            slot, 0, (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN), 0);
        spd = *dir;
        spd.x *= speed;
        spd.y *= speed;
        spd.z *= speed;
        motion->spd = spd;
        turn_from_world(work, &spd);
        record->spd = spd;
    }
}

void player_launch_speed(int slot, NJS_VECTOR *spd, struct angles *ang,
                         Sint16 lock)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR world;

    if (!RESET_WAITS(work)) {
        record->stick_lock = lock;
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_LAUNCH_SPEED;
        record->spd = *spd;
        work->ang.x = ang->x;
        work->ang.y = QUARTER_TURN - ang->y;
        motion->aim_y = work->ang.y;
        work->ang.z = ang->z;
        world = *spd;
        turn_to_world(work, &world);
        motion->spd = world;
    }
}

void fun_8c0492d0(int slot, NJS_VECTOR *spd, struct angles *ang, Sint16 lock)
{
    struct task_work *work = player_work[slot];

    if (!RESET_WAITS(work)) {
        player_launch_speed(slot, spd, ang, lock);
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_FREE_FALL;
    }
}

void fun_8c049304(int slot, NJS_VECTOR *spd, struct angles *ang, Sint16 lock)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR world;

    record->stick_lock = lock;
    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_LAUNCH_BALL;
    record->spd = *spd;
    work->ang.x = ang->x;
    work->ang.y = QUARTER_TURN - ang->y;
    motion->aim_y = work->ang.y;
    work->ang.z = ang->z;
    world = *spd;
    turn_to_world(work, &world);
    motion->spd = world;
}

void player_set_angles(int slot, Angle x, Angle y, Angle z)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];

    work->ang.x = x;
    work->ang.y = QUARTER_TURN - y;
    motion->aim_y = work->ang.y;
    work->ang.z = z;
}

void player_face_along(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];
    struct player_motion *motion = player_motion[slot];
    Float length = njSqrt((x * x) + (y * y) + (z * z));
    Angle ang_y = (Angle)(atan2f(z, x) * ANGLE_PER_RADIAN);
    Float sine = njSin(ang_y);
    Angle ang_x =
        (Angle)(acosf(-z / _builtin_fabsf(length * sine)) * ANGLE_PER_RADIAN);
    Float across = njSqrt((x * x) + (y * y));
    /* @hack spelling: length times sine written twice, and ang_z made in one
     * expression, where locals for the product and the two arcsines would
     * do */
    Angle ang_z =
        (Angle)(asinf(y / across) * ANGLE_PER_RADIAN) -
        (Angle)(asinf(_builtin_fabsf(length * sine) * njSin(ang_x) / across) *
                ANGLE_PER_RADIAN);

    work->ang.x = ang_x;
    work->ang.y = ang_y;
    motion->aim_y = work->ang.y;
    work->ang.z = ang_z;
}

void player_updraft(int slot, Float x, Float y, Float z)
{
    struct task_work *work = player_work[slot];

    if (!RESET_WAITS(work) && game_state.cleared != 1) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_UPDRAFT;
        player_set_acc(slot, x, y, z);
    }
}

void player_push(int slot, Float x, Float y, Float z)
{
    struct player *record = player_records[slot];

    x /= record->consts.gravity;
    y /= record->consts.gravity;
    z /= record->consts.gravity;
    player_set_acc(slot, x, y, z);
}

void player_set_acc(int slot, Float x, Float y, Float z)
{
    struct player_motion *motion = player_motion[slot];

    motion->acc.x = x;
    motion->acc.y = y;
    motion->acc.z = z;
}

void player_set_pos(int slot, Float x, Float y, Float z)
{
    Uint8 latest = past_latest[slot];

    /* @hack spelling: the slot's return ahead of the stores, where one if
     * round them would do */
    if (player_work[slot] == NULL) {
        return;
    }
    player_work[slot]->pos.x = x;
    player_work[slot]->pos.y = y;
    player_work[slot]->pos.z = z;
    past_pos[(slot * PAST_STEPS) + latest].x = x;
    past_pos[(slot * PAST_STEPS) + latest].y = y;
    past_pos[(slot * PAST_STEPS) + latest].z = z;
}

/* the gap player_push_out_of leaves between the two shapes */
#define PUSH_OUT_GAP 0.2F

void player_push_out_of(struct task_work *work)
{
    struct task_work *player;
    struct collision_shape *shape;
    NJS_VECTOR away;
    Float gap;
    int i;

    /* @hack spelling: a loop over slot 0 alone, where its body would do */
    for (i = 0; i < 1; i++) {
        player = player_work[i];
        shape = player->collision->shapes;
        away.x = shape->center.x - work->pos.x;
        away.y = shape->center.y - work->pos.y;
        away.z = shape->center.z - work->pos.z;
        gap = njScalor(&away);
        gap -= shape->radius;
        gap -= work->collision->shapes->radius;
        if (gap < 0.0F) {
            if (njScalor(&away) == 0.0F) {
                away.x = 1.0F;
                away.y = away.z = 0.0F;
            } else {
                njUnitVector(&away);
            }
            gap = -PUSH_OUT_GAP - gap;
            player->pos.x += away.x * gap;
            player->pos.y += away.y * gap;
            player->pos.z += away.z * gap;
        }
    }
}

int player_near(NJS_POINT3 *pos, Float range)
{
    struct task_work **works;
    struct task_work *work;
    struct collision_work *collision;
    Float *from;
    NJS_VECTOR to;
    Float x;
    Float y;
    Float z;
    Float gap;
    int i;

    /* @hack spelling: the works walked by a pointer and counted from 1, and
     * pos and the center read through a pointer stepped across their
     * members, where indexes and the members would do */
    i = 1;
    from = &pos->x;
    x = *from++;
    y = *from++;
    z = *from;
    for (works = player_work; works < &player_work[2]; i++) {
        work = *works++;
        if (work == NULL) {
            continue;
        }
        collision = work->collision;
        if (collision == NULL) {
            continue;
        }
        from = &collision->shapes->center.x;
        to.x = *from++ - x;
        to.y = *from++ - y;
        to.z = *from - z;
        gap = njScalor(&to) - range;
        if (gap < 0.0F) {
            return i;
        }
    }
    return 0;
}

int player_in_cylinder(NJS_POINT3 *pos, Float radius, Float height)
{
    struct task_work *work;
    NJS_VECTOR to;
    Float off;
    int i;

    /* @hack spelling: a loop over slot 0 alone, where its body would do,
     * whose failed tests continue to the loop's test */
    for (i = 0; i < 1; i++) {
        work = player_work[i];
        to.x = work->pos.x - pos->x;
        to.y = work->pos.y - pos->y;
        to.z = work->pos.z - pos->z;
        off = (to.x * to.x) + (to.z * to.z) - (radius * radius);
        if (off > 0.0F) {
            continue;
        }
        off = to.y;
        if (off < 0.0F) {
            off = -off;
        }
        if (off > height) {
            continue;
        }
        return i + 1;
    }
    return 0;
}

/* the least speed ahead a player takes onto a path by player_run_path, and
 * what player_boost_path adds to its speed */
#define PATH_JOIN_SPEED 5.0F

void player_run_path(int slot, struct path *path)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (work != NULL) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_RUN_PATH;
        record->path = path;
        if (record->spd.x < PATH_JOIN_SPEED) {
            record->spd.x = PATH_JOIN_SPEED;
        }
        path_find_nearest(path, &work->pos, NULL, &record->path_distance);
    }
}

void player_boost_path(int slot, struct path *path)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_BOOST_PATH;
    record->path = path;
    record->spd.x += PATH_JOIN_SPEED;
}

void player_join_path(int slot, struct path *path)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (work != NULL) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_JOIN_PATH;
        record->path = path;
        path_find_nearest(path, &work->pos, NULL, &record->path_distance);
    }
}

void player_hang_path(int slot, struct path *path, int point)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_HANG_PATH;
    record->path = path;
    record->path_point = point;
    path_point_distance(path, point, &record->path_distance);
}

void player_ledge_path(int slot, struct path *path, int point,
                       struct angles *ang)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_LEDGE_PATH;
    record->path = path;
    record->path_point = point;
    path_point_distance(path, point, &record->path_distance);
    player_set_angles(slot, ang->x, ang->y, ang->z);
}

void player_rail_path(int slot, struct path *path, Float distance)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    unsigned int point;

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_RAIL_PATH;
    record->path = path;
    record->path_distance = distance;
    path_segment_at(path, distance, &point);
    record->path_point = point;
}

void player_rail_path_beneath(int slot, struct path *path, Float distance)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    unsigned int point;

    work->flags |= PLAYER_WORK_REQUEST;
    work->submode = PLAYER_REQUEST_RAIL_PATH_BENEATH;
    record->path = path;
    record->path_distance = distance;
    path_segment_at(path, distance, &point);
    record->path_point = point;
}

void player_current_path(int slot, struct path *path)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (work != NULL) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_CURRENT_PATH;
        record->path = path;
    }
}

int players_on_path(struct path *path)
{
    struct task_work **works;
    struct player **records;
    struct task_work *work;
    int i = 1;
    int players = 0;

    /* @hack spelling: the tables walked by two pointers, with the count and
     * the second pointer stepped as i = i + 1 and records = records + 1,
     * where an index would do */
    for (works = player_work, records = player_records; works < &player_work[2];
         i = i + 1, records = records + 1) {
        /* @hack spelling: work set inside its test, where a statement of its
         * own would do */
        if ((work = *works++) != NULL) {
            if (work->flags & PLAYER_WORK_ON_PATH) {
                if ((*records)->path == path) {
                    players += i;
                }
            }
        }
    }
    return players;
}

void player_accel_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    NJS_VECTOR acc;
    Float push;
    Bool pushed;
    Float limit;

    if (work->flags & PLAYER_WORK_NO_CONTROL) {
        return;
    }
    acc.x = acc.y = acc.z = 0.0F;
    acc.x += world_gravity.x * record->consts.gravity;
    acc.y += world_gravity.y * record->consts.gravity;
    acc.z += world_gravity.z * record->consts.gravity;
    turn_from_world(work, &acc);
    acc.x += record->consts.resist.x * record->spd.x;
    /* @hack spelling: pushed set inside its test, where a statement of its
     * own would do */
    if ((pushed = player_read_stick(work, record, NULL, &push)) == FALSE) {
        push = record->consts.ground_drag;
    } else {
        push = record->consts.ground_accel * push;
    }
    acc.x += push;
    if (record->spd.x == 0.0F) {
        limit = acc.y * record->consts.still_grip;
        if (pushed == FALSE && acc.x < limit && acc.x > -limit) {
            acc.x = 0.0F;
        }
    }
    record->accel = acc;
}

/* player_follow_path holds a player with the stick at no less than this
 * times its full_speed */
#define FOLLOW_SPEED 1.2F
/* how much more the way along the path weighs in the aim than the way to
 * its place ahead */
#define FOLLOW_ALONG 9.0F
/* below this length, the cross product of two ways counts as none, and the
 * ball's up stands in for it */
#define FOLLOW_CROSS_NONE 0.1F
/* the inner product of player_follow_path's two turns' axes from which the
 * axes count as one */
#define FOLLOW_AXES_ONE 0.9999F
/* the turn past which player_follow_path turns by the angle between its
 * heading and the path's way, in place of its share of the turn */
#define FOLLOW_TURN_WIDE 0x400

/* keep a player under PLAYER_WORK_ON_PATH on its path while its stick is held.
 * spd.x rises to FOLLOW_SPEED times full_speed where it is below. path_distance
 * steps by spd.x from the place nearest the player. it steps ahead or back as
 * the player faces along the path.
 *
 * the motion's aim_y takes the way to the path's place ahead, in the plane of
 * the ball's up. FOLLOW_ALONG weights that way toward the way the path runs
 * there. ang.y takes the aim, less a lag toward the old heading. the lag is
 * half the gap where the turns from the heading and from the aim to the path's
 * way go opposite ways. it is a sixteenth of the gap where they go one way
 * about axes that FOLLOW_AXES_ONE doesn't count as one, and a 128th once it
 * does. past FOLLOW_TURN_WIDE it is a sixteenth of the turn from the heading to
 * the path's way.
 *
 * the player loses PLAYER_WORK_ON_PATH where the stick lets go, or where it
 * leaves the path's ends. it loses it too where the aim or the heading has no
 * length in the plane */
static void player_follow_path(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    struct path_sample sample;
    NJS_VECTOR ahead;
    NJS_POINT3 last;
    NJS_VECTOR side;
    NJS_VECTOR along;
    NJS_VECTOR to;
    NJS_PLANE plane;
    NJS_VECTOR axis;
    NJS_VECTOR aim_axis;
    int held;
    Angle aim;
    Angle step;
    Float facing;

    held = pad_sticks[record->slot].stroke != 0.0F;
    if (record->consts.full_speed * FOLLOW_SPEED > record->spd.x) {
        if (!held) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            return;
        }
        record->spd.x = record->consts.full_speed * FOLLOW_SPEED;
    } else if (!held) {
        work->flags &= ~PLAYER_WORK_ON_PATH;
        return;
    }
    path_find_nearest(record->path, &work->pos, &sample.pos,
                      &record->path_distance);
    ahead.x = 1.0F;
    ahead.z = 0.0F;
    ahead.y = 0.0F;
    turn_to_world(work, &ahead);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        facing = njInnerProduct(&ahead, &sample.ahead);
        if (facing >= 0.0F) {
            record->path_distance += record->spd.x;
        } else {
            record->path_distance -= record->spd.x;
            ahead.x = -ahead.x;
            ahead.y = -ahead.y;
            ahead.z = -ahead.z;
        }
    }
    last = sample.pos;
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        plane.vx = record->ball->up.x;
        plane.vy = record->ball->up.y;
        plane.vz = record->ball->up.z;
        plane.px = plane.py = plane.pz = 0.0F;
        to.x = sample.pos.x - work->pos.x;
        to.y = sample.pos.y - work->pos.y;
        to.z = sample.pos.z - work->pos.z;
        side.x = 1.0F;
        side.z = 0.0F;
        side.y = 0.0F;
        turn_to_world(work, &side);
        along.x = sample.pos.x - last.x;
        along.y = sample.pos.y - last.y;
        along.z = sample.pos.z - last.z;
        njUnitVector(&to);
        njUnitVector(&along);
        if (njInnerProduct(&side, &along) < 0.0F) {
            along.x = -along.x;
            along.y = -along.y;
            along.z = -along.z;
        }
        /* @hack redundant: the way to the place ahead times 1 */
        to.x = (to.x * 1.0F) + (along.x * FOLLOW_ALONG);
        to.y = (to.y * 1.0F) + (along.y * FOLLOW_ALONG);
        to.z = (to.z * 1.0F) + (along.z * FOLLOW_ALONG);
        njDistanceP2PL(&to, &plane, &side);
        njDistanceP2PL(&ahead, &plane, &along);
        if (njScalor(&along) == 0.0F || njScalor(&side) == 0.0F) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            return;
        }
        njUnitVector(&side);
        njUnitVector(&along);
        aim = vector_angle(&side, &along, &record->ball->up);
        aim = (Uint16)(work->ang.y + aim);
        if (angle_distance(aim, work->ang.y) > QUARTER_TURN) {
            aim += HALF_TURN;
        }
        motion->aim_y = aim;
        step = angle_distance(aim, work->ang.y);
        njOuterProduct(&ahead, &sample.ahead, &axis);
        if (njScalor(&axis) <= FOLLOW_CROSS_NONE) {
            axis = record->ball->up;
        } else {
            njUnitVector(&axis);
        }
        njOuterProduct(&side, &sample.ahead, &aim_axis);
        if (njScalor(&aim_axis) <= FOLLOW_CROSS_NONE) {
            aim_axis = record->ball->up;
        } else {
            njUnitVector(&aim_axis);
        }
        /* @hack spelling: step = step >> n, where step >>= n would do */
        if (njInnerProduct(&axis, &aim_axis) < 0.0F) {
            step = step >> 1;
        } else if (njInnerProduct(&axis, &aim_axis) <= FOLLOW_AXES_ONE) {
            step = step >> 4;
        } else if (step > FOLLOW_TURN_WIDE) {
            step = (Angle)(acosf(njInnerProduct(&sample.ahead, &ahead)) *
                           ANGLE_PER_RADIAN);
            step = step >> 4;
        } else {
            step = step >> 7;
        }
        work->ang.y = angle_approach(aim, work->ang.y, step);
    } else {
        work->flags &= ~PLAYER_WORK_ON_PATH;
    }
}

/* the speed from which a slowing player on a path stops below it */
#define PATH_STOP_SPEED 0.1F

void player_apply_path_speed(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    Float last;
    NJS_VECTOR spd;

    last = record->spd.x;
    record->spd.x += record->accel.x;
    if (last > 0.0F) {
        if (record->spd.x < 0.0F) {
            record->spd.x = 0.0F;
        } else if (last >= PATH_STOP_SPEED && record->spd.x < PATH_STOP_SPEED) {
            record->spd.x = 0.0F;
        }
    } else if (last < 0.0F) {
        /* @hack type: -0.0F, where 0.0F compares the same */
        if (record->spd.x > -0.0F) {
            record->spd.x = 0.0F;
        } else if (last <= -PATH_STOP_SPEED &&
                   record->spd.x > -PATH_STOP_SPEED) {
            record->spd.x = 0.0F;
        }
    }
    record->spd.y = 0.0F;
    record->spd.z = 0.0F;
    spd = record->spd;
    turn_to_world(work, &spd);
    motion->spd = spd;
    player_limit_speed(work, motion, record);
}

/* how far a turn toward the way along the path steps the heading */
#define PATH_TURN_STEP 0x800

void player_move_on_path(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    struct path_sample sample;
    NJS_VECTOR ahead;
    /* @hack redundant: a vector nothing uses, which places the locals after
     * it in the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    NJS_VECTOR side;
    NJS_VECTOR up;
    NJS_VECTOR along;
    NJS_VECTOR way;
    NJS_PLANE plane;
    int backward;
    Angle aim;
    Float facing;

    work->flags &=
        ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_SHAPE | PLAYER_WORK_ON_GROUND);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        ahead.x = 1.0F;
        ahead.z = 0.0F;
        ahead.y = 0.0F;
        turn_to_world(work, &ahead);
        /* @hack spelling: the inner product in a local, where the test could
         * take the call */
        facing = njInnerProduct(&ahead, &sample.ahead);
        if (facing < 0.0F) {
            record->path_distance -= record->spd.x;
            backward = 1;
        } else {
            record->path_distance += record->spd.x;
            backward = 0;
        }
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        return;
    }
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        side.x = 1.0F;
        side.z = 0.0F;
        side.y = 0.0F;
        matrix_push_unit();
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njCalcVector(NULL, &side, &side);
        njPopMatrixEx();
        up.y = 1.0F;
        up.z = 0.0F;
        up.x = 0.0F;
        turn_to_world(work, &up);
        plane.px = plane.py = plane.pz = 0.0F;
        plane.vx = up.x;
        plane.vy = up.y;
        plane.vz = up.z;
        way = sample.ahead;
        if (backward == 1) {
            way.x = -way.x;
            way.y = -way.y;
            way.z = -way.z;
        }
        njDistanceP2PL(&way, &plane, &along);
        njUnitVector(&along);
        aim = vector_angle(&side, &along, &up);
        if (angle_distance(motion->aim_y, aim) > QUARTER_TURN) {
            aim += HALF_TURN;
        }
        motion->aim_y = -aim;
        work->ang.y = angle_approach(work->ang.y, aim, PATH_TURN_STEP);
        work->ang.x = sample.ang_x;
        work->ang.z = sample.ang_z;
        work->pos = sample.pos;
        /* @hack spelling: flags = flags | the bits, where |= would do */
        work->flags =
            work->flags | (PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_GROUND);
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
    }
}

void player_move_on_path_tilted(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record)
{
    struct path_sample sample;
    NJS_VECTOR ahead;
    /* @hack redundant: a vector nothing uses, which places the locals after
     * it in the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    NJS_VECTOR side;
    NJS_VECTOR up;
    NJS_VECTOR along;
    NJS_VECTOR way;
    NJS_PLANE plane;
    int backward;
    Angle aim;
    Float facing;

    work->flags &=
        ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_SHAPE | PLAYER_WORK_ON_GROUND);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        ahead.x = 1.0F;
        ahead.z = 0.0F;
        ahead.y = 0.0F;
        turn_to_world(work, &ahead);
        /* @hack spelling: the inner product in a local, where the test could
         * take the call */
        facing = njInnerProduct(&ahead, &sample.ahead);
        if (facing < 0.0F) {
            record->path_distance -= record->spd.x;
            backward = 1;
        } else {
            record->path_distance += record->spd.x;
            backward = 0;
        }
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        return;
    }
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        work->ang.x = sample.ang_x;
        work->ang.z = sample.ang_z;
        side.x = 1.0F;
        side.z = 0.0F;
        side.y = 0.0F;
        matrix_push_unit();
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njCalcVector(NULL, &side, &side);
        njPopMatrixEx();
        up.y = 1.0F;
        up.z = 0.0F;
        up.x = 0.0F;
        turn_to_world(work, &up);
        plane.px = plane.py = plane.pz = 0.0F;
        plane.vx = up.x;
        plane.vy = up.y;
        plane.vz = up.z;
        way = sample.ahead;
        if (backward == 1) {
            way.x = -way.x;
            way.y = -way.y;
            way.z = -way.z;
        }
        njDistanceP2PL(&side, &plane, &along);
        njUnitVector(&along);
        side = along;
        njDistanceP2PL(&way, &plane, &along);
        njUnitVector(&along);
        aim = -vector_angle(&side, &along, &up);
        if (angle_distance(motion->aim_y, aim) > QUARTER_TURN) {
            aim += HALF_TURN;
        }
        motion->aim_y = aim;
        work->ang.y = angle_approach(work->ang.y, aim, PATH_TURN_STEP);
        work->pos = sample.pos;
        work->flags |= PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_GROUND;
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
    }
}

/* the most of its speed a player moving toward its path's place moves a
 * step */
#define PATH_CHASE_FACTOR 1.3F

void player_move_toward_path(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    struct path_sample sample;
    NJS_VECTOR ahead;
    NJS_VECTOR side;
    NJS_VECTOR up;
    NJS_VECTOR along;
    NJS_VECTOR way;
    NJS_PLANE plane;
    int backward;
    Angle aim;
    Float reach;

    work->flags &=
        ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_SHAPE | PLAYER_WORK_ON_GROUND);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        ahead = sample.ahead;
        turn_from_world(work, &ahead);
        if (ahead.x < 0.0F) {
            record->path_distance -= record->spd.x;
            backward = 1;
        } else {
            record->path_distance += record->spd.x;
            backward = 0;
        }
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        return;
    }
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        side.x = 1.0F;
        side.z = 0.0F;
        side.y = 0.0F;
        matrix_push_unit();
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njCalcVector(NULL, &side, &side);
        njPopMatrixEx();
        up.y = 1.0F;
        up.z = 0.0F;
        up.x = 0.0F;
        turn_to_world(work, &up);
        plane.px = plane.py = plane.pz = 0.0F;
        plane.vx = up.x;
        plane.vy = up.y;
        plane.vz = up.z;
        way = sample.ahead;
        if (backward == 1) {
            way.x = -way.x;
            way.y = -way.y;
            way.z = -way.z;
        }
        njDistanceP2PL(&way, &plane, &along);
        njUnitVector(&along);
        aim = vector_angle(&side, &along, &up);
        if (angle_distance(motion->aim_y, aim) > QUARTER_TURN) {
            aim += HALF_TURN;
        }
        motion->aim_y = -aim;
        work->ang.y = angle_approach(work->ang.y, aim, PATH_TURN_STEP);
        ahead.x = 0.0F;
        ahead.y = 1.0F;
        ahead.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, sample.slope_z);
        njRotateX(NULL, sample.slope_x);
        njCalcVector(NULL, &ahead, &ahead);
        njPopMatrixEx();
        work->ang.x = (Angle)(asinf(-ahead.z) * ANGLE_PER_RADIAN);
        work->ang.z = -(Angle)(atan2f(-ahead.x, -ahead.y) * ANGLE_PER_RADIAN);
        ahead = sample.pos;
        njSubVector(&ahead, &work->pos);
        if (njScalor(&ahead) >
            _builtin_fabsf(record->spd.x) * PATH_CHASE_FACTOR) {
            reach = _builtin_fabsf(record->spd.x) * PATH_CHASE_FACTOR;
            njUnitVector(&ahead);
            ahead.x *= reach;
            ahead.y *= reach;
            ahead.z *= reach;
        }
        njAddVector(&work->pos, &ahead);
        work->flags |= PLAYER_WORK_ON_PATH | PLAYER_WORK_ON_GROUND;
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
    }
}

/* how far short of its path's ends player_slide_on_path and
 * player_shift_on_path hold a player */
#define PATH_END_MARGIN 0.01F

Bool player_slide_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    struct path_sample sample;
    NJS_VECTOR ahead;
    NJS_VECTOR front;
    NJS_VECTOR slope;
    NJS_VECTOR side;
    NJS_VECTOR up;
    NJS_VECTOR along;
    NJS_VECTOR way;
    NJS_PLANE plane;
    struct path *path = record->path;
    Angle aim;
    Float facing;

    record->drive = 0.0F;
    sample.distance = record->path_distance;
    if (path_get_sample(path, &sample)) {
        front.x = 1.0F;
        front.z = 0.0F;
        front.y = 0.0F;
        turn_to_world(work, &front);
        facing = njInnerProduct(&front, &sample.ahead);
        if (facing < 0.0F) {
            record->path_distance -= record->spd.x;
            ahead.x = -1.0F;
            ahead.y = 0.0F;
            ahead.z = 0.0F;
        } else {
            record->path_distance += record->spd.x;
            ahead.x = 1.0F;
            ahead.y = 0.0F;
            ahead.z = 0.0F;
        }
        record->drive = 1.0F;
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        record->drive = 0.0F;
        return FALSE;
    }
    if (path->count == 1) {
        record->path_distance = 0.0F;
        record->drive = 0.0F;
    } else if (path->length <= record->path_distance) {
        record->path_distance = path->length - PATH_END_MARGIN;
        record->drive = 0.0F;
    } else if (record->path_distance <= 0.0F) {
        record->path_distance = PATH_END_MARGIN;
        record->drive = 0.0F;
    }
    sample.distance = record->path_distance;
    if (path_get_sample(path, &sample)) {
        side = ahead;
        turn_to_world(work, &side);
        up.y = 1.0F;
        up.z = 0.0F;
        up.x = 0.0F;
        turn_to_world(work, &up);
        plane.px = plane.py = plane.pz = 0.0F;
        plane.vx = up.x;
        plane.vy = up.y;
        plane.vz = up.z;
        way = sample.ahead;
        if (ahead.x == -1.0F) {
            way.x = -way.x;
            way.y = -way.y;
            way.z = -way.z;
        }
        njDistanceP2PL(&way, &plane, &along);
        njUnitVector(&along);
        aim = work->ang.y + vector_angle(&side, &along, &up);
        /* @hack redundant: the slope turned into front, which nothing
         * reads */
        slope.z = 0.0F;
        slope.x = 0.0F;
        slope.y = 1.0F;
        matrix_push_unit();
        njRotateZ(NULL, sample.ang_z);
        njRotateX(NULL, sample.ang_x);
        njCalcVector(NULL, &slope, &front);
        njPopMatrixEx();
        if (angle_distance(motion->aim_y, aim) > QUARTER_TURN) {
            aim += HALF_TURN;
        }
        motion->aim_y = aim;
        work->ang.y = angle_approach(work->ang.y, aim, PATH_TURN_STEP);
        work->ang.x = sample.ang_x;
        work->ang.z = sample.ang_z;
        work->pos = sample.pos;
        return TRUE;
    }
    return FALSE;
}

Bool player_shift_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    struct path_sample sample;
    struct path *path = record->path;

    record->drive = 0.0F;
    sample.distance = record->path_distance;
    if (path_get_sample(path, &sample)) {
        record->path_distance += record->spd.x;
        record->drive = 1.0F;
    } else {
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        record->drive = 0.0F;
        return FALSE;
    }
    if (path->count == 1) {
        record->path_distance = 0.0F;
        record->drive = 0.0F;
    } else if (path->length <= record->path_distance) {
        record->path_distance = path->length - PATH_END_MARGIN;
        record->drive = 0.0F;
    } else if (record->path_distance <= 0.0F) {
        record->path_distance = PATH_END_MARGIN;
        record->drive = 0.0F;
    }
    sample.distance = record->path_distance;
    if (path_get_sample(path, &sample)) {
        work->pos = sample.pos;
        return TRUE;
    }
    return FALSE;
}

int player_character(int slot)
{
    struct player *record = player_records[slot];

    if (record == NULL) {
        return -1;
    }
    return record->character;
}

Bool player_ground_speed(int slot, Float *speed)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    /* @hack spelling: the store in the test's arm and the FALSE in its else,
     * where a return of FALSE ahead would do */
    if (work != NULL && (work->flags & PLAYER_WORK_GROUNDED)) {
        *speed = record->spd.x;
    } else {
        return FALSE;
    }
    return TRUE;
}

Bool player_get_past(int slot, int back, NJS_POINT3 *pos, struct angles *ang)
{
    Uint8 index = past_latest[slot] - back;

    if (player_work[slot] == NULL) {
        return FALSE;
    }
    if (pos != NULL) {
        *pos = past_pos[(slot * PAST_STEPS) + index];
    }
    if (ang != NULL) {
        *ang = past_ang[(slot * PAST_STEPS) + index];
    }
    return TRUE;
}

Bool player_set_past(int slot, int back, NJS_POINT3 *pos, struct angles *ang)
{
    Uint8 index = past_latest[slot] - back;

    if (player_work[slot] == NULL) {
        return FALSE;
    }
    if (pos != NULL) {
        past_pos[(slot * PAST_STEPS) + index] = *pos;
    }
    if (ang != NULL) {
        past_ang[(slot * PAST_STEPS) + index] = *ang;
    }
    return TRUE;
}

/* the ids from WORK_ID_PLAYER up to this whose works' tasks hold a player's
 * record as their extra */
#define WORK_ID_RECORDS 4

int player_slot_of(struct task *task)
{
    /* @hack type: id volatile, where a plain int would do */
    volatile int id;
    struct player *record;

    if (task == NULL) {
        return -1;
    }
    if (task->parent != NULL) {
        task = task->parent;
    } else if (task->work != NULL && task->work->id == WORK_ID_PLAYER_PROXY) {
        return task->work->submode;
    }
    if ((id = task->work->id) == 0) {
        return -1;
    }
    /* @hack redundant: an arm that never runs, ahead of the id's other
     * test */
    if (0) {
        task->work->mode = 0;
    } else if (id > WORK_ID_RECORDS) {
        return -1;
    }
    record = task->extra.p;
    return record->slot;
}

int player_character_of(struct task *task)
{
    int i;
    int character = -1;
    struct player *record;

    if (task == NULL) {
        return -1;
    }
    if (task->parent != NULL) {
        task = task->parent;
    } else if (task->work != NULL && task->work->id == WORK_ID_PLAYER_PROXY) {
        return task->work->submode;
    }
    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_tasks[i] == task) {
            record = player_records[i];
            if (record != NULL) {
                character = record->character;
                break;
            }
        }
    }
    return character;
}

/* how many slots hold a task */
static int player_count(void)
{
    int i;
    int count = 0;

    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_tasks[i] != NULL) {
            count++;
        }
    }
    return count;
}

/* the first slot whose record's character is character, or -1 */
static int player_find_character(int character)
{
    int i;

    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_records[i] != NULL) {
            if (player_records[i]->character == character) {
                return i;
            }
        }
    }
    return -1;
}

/* farther than any player, for player_nearest's search */
#define NEAREST_FAR 10000000.0F

int player_nearest(NJS_POINT3 *pos)
{
    NJS_VECTOR to;
    Float distance;
    Float nearest;
    int slot;
    int i;

    /* @hack spelling: slot set ahead of nearest, where either order would
     * do */
    slot = 0;
    nearest = NEAREST_FAR;
    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_work[i] != NULL) {
            to = *pos;
            njSubVector(&to, &player_work[i]->pos);
            distance = njScalor(&to);
            if (distance < nearest) {
                nearest = distance;
                slot = i;
            }
        }
    }
    return slot;
}

int player_find_rival(int slot)
{
    int i;
    int id;

    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (i != slot && player_work[i] != NULL) {
            id = player_work[i]->id;
            if (id == WORK_ID_RIVAL || id == WORK_ID_PLAYER) {
                return i;
            }
        }
    }
    return -1;
}

Bool player_on_task(int slot, struct task *task)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (record != NULL && work != NULL &&
        (work->flags & PLAYER_WORK_ON_GROUND) && record->ground_task == task) {
        return TRUE;
    }
    return FALSE;
}

/* the characters a player can be, PLAYER_SONIC to PLAYER_EGGMAN_MECH. the
 * three rows of character_constants past them have no player */
#define PLAYER_CHARACTERS 8
/* by character: the kind of what a player of it holds, as
 * player_set_held_kind sets it */
static Sint8 held_kinds[PLAYER_CHARACTERS];
/* each slot's task's step and draw, as player_pause took them */
static void (*paused_steps[PLAYER_SLOTS])(struct task *task);
static void (*paused_draws[PLAYER_SLOTS])(struct task *task);

void player_pause(int slot)
{
    struct task *task = player_tasks[slot];

    if (task != NULL) {
        paused_steps[slot] = task->step;
        paused_draws[slot] = task->draw;
        task->step = NULL;
        task->draw = NULL;
    }
}

void player_resume(int slot)
{
    struct task *task = player_tasks[slot];

    if (task != NULL) {
        task->step = paused_steps[slot];
        task->draw = paused_draws[slot];
    }
}

/* how long a speed up lasts */
#define SPEED_UP_STEPS 600

/* the extra of a PlayerSpeedUp task */
struct speed_up {
    Uint8 slot; /* the player's slot */
    Uint16 steps; /* the steps since the speed up began */
    /* the record's constants as they were before the speed up doubled
     * them */
    Float ground_accel;
    Float top_speed;
    Float still_grip;
};
#define SPEED_UP(task) ((struct speed_up *)(task)->extra.p)

static void speed_up_step(struct task *task)
{
    struct player *record = player_records[SPEED_UP(task)->slot];

    if (record == NULL || SPEED_UP(task)->steps++ >= SPEED_UP_STEPS) {
        task_kill(task);
    }
}

static void speed_up_destroy(struct task *task)
{
    struct player *record = player_records[SPEED_UP(task)->slot];

    if (record != NULL) {
        record->consts.ground_accel = SPEED_UP(task)->ground_accel;
        record->consts.top_speed = SPEED_UP(task)->top_speed;
        record->consts.still_grip = SPEED_UP(task)->still_grip;
    }
}

/* the voice each kind of player plays as it speeds up or turns invincible */
#define VOICE_POWER_SONIC 0x613
#define VOICE_POWER_SHADOW 0x610
#define VOICE_POWER_KNUCKLES 0x508
#define VOICE_POWER_ROUGE 0x509
#define VOICE_POWER_TAILS 0x683
#define VOICE_POWER_EGGMAN 0x682
#define VOICE_POWER_AMY 0xa75

static void speed_up_start(struct task *task)
{
    struct player *record = player_records[SPEED_UP(task)->slot];
    int voice;

    if (record == NULL) {
        task_kill(task);
        return;
    }
    SPEED_UP(task)->ground_accel = record->consts.ground_accel;
    SPEED_UP(task)->top_speed = record->consts.top_speed;
    SPEED_UP(task)->still_grip = record->consts.still_grip;
    /* @hack spelling: x = x * 2, where x *= 2 would do */
    record->consts.ground_accel = record->consts.ground_accel * 2.0F;
    record->consts.top_speed = record->consts.top_speed * 2.0F;
    record->consts.still_grip = record->consts.still_grip * 2.0F;
    jingle_play("speedup.adx");
    voice = -1;
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_POWER_SONIC;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_POWER_SHADOW;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_POWER_KNUCKLES;
        break;
    case PLAYER_ROUGE:
        voice = VOICE_POWER_ROUGE;
        break;
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
        voice = VOICE_POWER_TAILS;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_EGGMAN:
        voice = VOICE_POWER_EGGMAN;
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_POWER_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
    task->step = speed_up_step;
    task->destructor = speed_up_destroy;
}

void player_speed_up(int slot)
{
    struct task *task;

    if (slot >= 1 && (slot > PLAYER_SLOTS - 1 || player_work[slot] == NULL)) {
        slot = 0;
    }
    task = task_create(TASK_EXTRA, 2, speed_up_start, "PlayerSpeedUp");
    if (task != NULL) {
        SPEED_UP(task)->slot = slot;
    }
}

/* the voice each kind of player plays as it takes a barrier */
#define VOICE_BARRIER_SONIC 0x611
#define VOICE_BARRIER_SHADOW 0x60e
#define VOICE_BARRIER_KNUCKLES 0x4fe
#define VOICE_BARRIER_ROUGE 0x4ff
#define VOICE_BARRIER_TAILS 0x681
#define VOICE_BARRIER_EGGMAN 0x680
#define VOICE_BARRIER_AMY 0xa75

/* the power-ups' test of slot means to take a slot past 7 for slot 0, as
 * player_speed_up's does. it means to take one from 1 to 7 without a work for
 * slot 0 too. it joins its tests with && where || is meant. a slot past 7 then
 * reads past player_work's end, and a slot from 1 to 7 is never taken for slot
 * 0. the word read past the end differs from link to link. the non-matching
 * side reads none: it takes a slot past 7 for slot 0, and leaves any other as
 * the image does */
void player_give_magnetic_barrier(int slot)
{
    struct player *record;
    struct task *task;
    int voice;

    /* @bug memory: a slot past 7 reads past player_work's end, as the
     * power-ups' comment says (0x8c04b432) */
    /* @hack redundant: slot >= 1, which the test after it makes
     * needless */
#ifdef NONMATCHING
    /* this side repairs the tag's read: it takes a slot past 7 for slot 0, and
     * leaves any other as the image does */
    if (slot > PLAYER_SLOTS - 1) {
        slot = 0;
    }
#else
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (slot >= 1 && slot > PLAYER_SLOTS - 1 && player_work[slot] == NULL) {
        slot = 0;
    }
#endif
    record = player_records[slot];
    if (record == NULL) {
        return;
    }
    if (record->flags & PLAYER_MAGNETIC_BARRIER) {
        return;
    }
    task = magnetic_barrier_create(slot);
    if (task == NULL) {
        return;
    }
    record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
    record->flags |= PLAYER_MAGNETIC_BARRIER;
    voice = -1;
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_BARRIER_SONIC;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_BARRIER_SHADOW;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_BARRIER_KNUCKLES;
        break;
    case PLAYER_ROUGE:
        voice = VOICE_BARRIER_ROUGE;
        break;
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
        voice = VOICE_BARRIER_TAILS;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_EGGMAN:
        voice = VOICE_BARRIER_EGGMAN;
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_BARRIER_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
}

void player_give_barrier(int slot)
{
    struct player *record;
    struct task *task;
    int voice;

    /* @bug memory: a slot past 7 reads past player_work's end, as the
     * power-ups' comment says (0x8c04b53a) */
    /* @hack redundant: slot >= 1, which the test after it makes
     * needless */
#ifdef NONMATCHING
    /* this side repairs the tag's read: it takes a slot past 7 for slot 0, and
     * leaves any other as the image does */
    if (slot > PLAYER_SLOTS - 1) {
        slot = 0;
    }
#else
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (slot >= 1 && slot > PLAYER_SLOTS - 1 && player_work[slot] == NULL) {
        slot = 0;
    }
#endif
    record = player_records[slot];
    if (record == NULL) {
        return;
    }
    if (record->flags & PLAYER_BARRIER) {
        return;
    }
    task = barrier_create(slot);
    if (task == NULL) {
        return;
    }
    record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
    record->flags |= PLAYER_BARRIER;
    voice = -1;
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_BARRIER_SONIC;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_BARRIER_SHADOW;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_BARRIER_KNUCKLES;
        break;
    case PLAYER_ROUGE:
        voice = VOICE_BARRIER_ROUGE;
        break;
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
        voice = VOICE_BARRIER_TAILS;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_EGGMAN:
        voice = VOICE_BARRIER_EGGMAN;
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_BARRIER_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
}

void player_give_invincibility(int slot)
{
    struct player *record;
    struct task *task;
    int voice;

    /* @bug memory: a slot past 7 reads past player_work's end, as the
     * power-ups' comment says (0x8c04b63c) */
    /* @hack redundant: slot >= 1, which the test after it makes
     * needless */
#ifdef NONMATCHING
    /* this side repairs the tag's read: it takes a slot past 7 for slot 0, and
     * leaves any other as the image does */
    if (slot > PLAYER_SLOTS - 1) {
        slot = 0;
    }
#else
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (slot >= 1 && slot > PLAYER_SLOTS - 1 && player_work[slot] == NULL) {
        slot = 0;
    }
#endif
    record = player_records[slot];
    if (record == NULL) {
        return;
    }
    if (record->flags & PLAYER_INVINCIBLE) {
        return;
    }
    task = invincibility_create(slot);
    if (task == NULL) {
        return;
    }
    voice = -1;
    record->flags |= PLAYER_INVINCIBLE;
    jingle_play("invncibl.adx");
    switch (record->kind) {
    case PLAYER_SONIC:
        voice = VOICE_POWER_SONIC;
        break;
    case PLAYER_SHADOW:
        voice = VOICE_POWER_SHADOW;
        break;
    case PLAYER_KNUCKLES:
        voice = VOICE_POWER_KNUCKLES;
        break;
    case PLAYER_ROUGE:
        voice = VOICE_POWER_ROUGE;
        break;
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
        voice = VOICE_POWER_TAILS;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_EGGMAN:
        voice = VOICE_POWER_EGGMAN;
        break;
    case PLAYER_KIND_AMY:
        voice = VOICE_POWER_AMY;
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
}

/* the steps a moment's invincibility lasts. its task clears it, and ends, on
 * the next step */
#define MOMENT_STEPS 120

/* the step of a MomentMuteki task, whose work's tick holds the player's
 * slot */
static void moment_invincibility_step(struct task *task)
{
    struct task_work *work = task->work;
    struct player *record = player_records[work->tick];

    work->timer++;
    if (work->timer > MOMENT_STEPS) {
        if (record != NULL) {
            record->flags &= ~PLAYER_INVINCIBLE;
        }
        task_kill(task);
    } else if (record != NULL) {
        /* @hack spelling: the record read from its table again, where
         * record would do */
        player_records[work->tick]->flags |= PLAYER_INVINCIBLE;
    }
}

void player_moment_invincibility(int slot)
{
    struct task *task;

    /* @bug memory: a slot past 7 reads past player_work's end, as the
     * power-ups' comment says (0x8c04b796) */
    /* @hack redundant: slot >= 1, which the test after it makes
     * needless */
#ifdef NONMATCHING
    /* this side repairs the tag's read: it takes a slot past 7 for slot 0, and
     * leaves any other as the image does */
    if (slot > PLAYER_SLOTS - 1) {
        slot = 0;
    }
#else
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (slot >= 1 && slot > PLAYER_SLOTS - 1 && player_work[slot] == NULL) {
        slot = 0;
    }
#endif
    task = task_create(TASK_WORK, 0, moment_invincibility_step, "MomentMuteki");
    if (task != NULL) {
        task->work->tick = slot;
    }
}

void player_set_held_kind(int slot, int kind)
{
    struct player *record = player_records[slot];

    held_kinds[record->character] = kind;
}

int player_get_held_kind(int slot)
{
    struct player *record = player_records[slot];

    return held_kinds[record->character];
}

void player_hand_task(int slot, struct task *task)
{
    struct task_work *work;
    struct player *record;

    work = player_work[slot];
    record = player_records[slot];
    if (record->handed == NULL) {
        work->flags |= PLAYER_WORK_REQUEST;
        work->submode = PLAYER_REQUEST_HAND;
        record->handed = task;
    }
}

void player_let_go(int slot)
{
    struct task_work *work;
    struct player *record;

    work = player_work[slot];
    record = player_records[slot];
    if (work != NULL && (work->flags & PLAYER_WORK_HOLDING) &&
        record->held != NULL) {
        work->flags &= ~PLAYER_WORK_HOLDING;
        if (record->held->work != NULL) {
            record->held->work->flags &= ~WORK_HELD_BY_PLAYER;
        }
        record->held = NULL;
    }
}

void player_targets_alloc(struct player *record)
{
    /* @hack type: the size as the int 32, where sizeof (struct
     * player_target) would do */
    record->targets = syCalloc(PLAYER_TARGETS, 32);
    record->target_count = 0;
}

/* free the room of player_targets_alloc and empty targets, so that a second
 * free does nothing */
static void player_targets_free(struct player *record)
{
    if (record->targets != NULL) {
        syFree(record->targets);
        record->targets = NULL;
    }
}

/* player_add_target offers a target to players 0 and 1. a player takes it
 * only within TARGET_REACH of the target's surface */
#define TARGET_PLAYERS 2
#define TARGET_REACH 120.0F
/* steering needs its target within TARGET_ARC of the player's heading, and
 * the stick's heading within TARGET_ARC of the target */
#define TARGET_ARC 0x1800
/* a steered player heading within TARGET_TURN_ARC of its target turns toward
 * it by TARGET_TURN_STEP a step */
#define TARGET_TURN_ARC 0x1000
#define TARGET_TURN_STEP 0x100
/* steering follows the fall for up to FALL_STEPS steps, and gives up on a
 * fall its end test doesn't end within them */
#define FALL_STEPS 120
/* where steering aims a landing, in radii short of the target's center. a
 * landing past the center moves back to LONG_AIM short of it. one more than
 * LONG_AIM short moves up to SHORT_AIM short. a landing between keeps its
 * place, unless the fall is fast */
#define LONG_AIM 0.75F
#define SHORT_AIM 0.5F

void player_add_target(Float x, Float y, Float z, Float radius)
{
    int i;
    int index;
    struct task_work *work;
    struct player *record;
    Float off;
    Float distance;

    /* @hack redundant: radius stored to itself */
    radius = radius; /* NOLINT(clang-diagnostic-self-assign) */
    for (i = 0; i < TARGET_PLAYERS; i++) {
        /* @hack spelling: work set inside its test, where a statement of its
         * own would do */
        if ((work = player_work[i]) == NULL) {
            continue;
        }
        if (y > work->pos.y) {
            continue;
        }
        record = player_records[i];
        /* @hack spelling: one local for the three differences, where each
         * difference written out would do */
        off = x - work->pos.x;
        distance = off * off;
        off = y - work->pos.y;
        distance += off * off;
        off = z - work->pos.z;
        distance += off * off;
        /* @hack spelling: the root and the radius taken in statements of
         * their own, where one statement would do */
        distance = njSqrt(distance);
        distance = distance - radius;
        if (distance > TARGET_REACH) {
            continue;
        }
        if (record->target_count >= PLAYER_TARGETS) {
            continue;
        }
        index = record->target_count;
        record->targets[index].pos.x = x;
        record->targets[index].pos.y = y;
        record->targets[index].pos.z = z;
        record->targets[index].radius = radius;
        record->targets[index].distance = distance;
        record->target_count++;
        return;
    }
}

Bool player_steer_to_target(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    int i;
    struct player_target *target;
    struct player_target *best;
    Angle ang;
    Angle off;
    Angle turn;
    Angle heading;
    Angle aim;
    Float across;
    Float distance;
    NJS_POINT3 pos;
    NJS_VECTOR gravity;
    NJS_VECTOR spd;

    /* @hack spelling: best set ahead of pos, where either order would do */
    best = NULL;
    pos = work->pos;
    for (i = 0; i < record->target_count; i++) {
        target = &record->targets[i];
        if (pos.y < target->pos.y) {
            continue;
        }
        ang = (Angle)(atan2f(target->pos.z - pos.z, target->pos.x - pos.x) *
                      ANGLE_PER_RADIAN);
        off = angle_difference(work->ang.y, ang);
        /* past TARGET_ARC, a target still competes while the player lies
         * within its radius in x and z */
        if (off > TARGET_ARC || off < -TARGET_ARC) {
            /* @hack spelling: the z difference in a local and the x
             * difference written out twice, where either way for both
             * would do */
            across = target->pos.z - pos.z;
            distance = across * across;
            distance += (target->pos.x - pos.x) * (target->pos.x - pos.x);
            distance = njSqrt(distance);
            if (distance > target->radius) {
                continue;
            }
        }
        if (best != NULL && best->distance < target->distance) {
            continue;
        }
        target->off = off;
        target->ang = ang;
        best = target;
    }
    if (best == NULL) {
        return FALSE;
    }
    /* the nearest must lie within TARGET_ARC of the player's heading, and
     * the player must have been outside its sphere as player_add_target
     * listed it */
    turn = best->off;
    if (turn > TARGET_ARC || turn < -TARGET_ARC || best->distance < 0.0F) {
        return FALSE;
    }
    /* player_turn_to_stick for the stick's aim alone: the player's heading
     * and aim go back as they were */
    heading = work->ang.y;
    aim = motion->aim_y;
    player_turn_to_stick(work, motion, record);
    work->ang.y = heading;
    turn = motion->aim_y;
    motion->aim_y = aim;
    turn = angle_distance(turn, best->ang);
    if (turn > TARGET_ARC) {
        return FALSE;
    }
    ang = best->ang;
    pos.x -= best->pos.x;
    pos.y -= best->pos.y;
    pos.z -= best->pos.z;
    best->across = njSqrt((pos.x * pos.x) + (pos.z * pos.z));
    spd = motion->spd;
    gravity.x = 0.0F;
    gravity.y = -record->consts.gravity;
    gravity.z = 0.0F;
    /* follow the fall a step at a time, by the player's constants, to the end
     * the test finds. the speed in x and z that corrects the landing is spread
     * over those steps. each axis of it is held within ground_accel */
    i = 0;
    do {
        spd.x += (record->consts.air_resist * spd.x) + gravity.x;
        spd.y += (record->consts.resist.y * spd.y) + gravity.y;
        spd.z += (record->consts.resist.z * spd.z) + gravity.z;
        if (spd.y < -record->consts.fall_limit) {
            spd.y = -record->consts.fall_limit;
        }
        pos.x += spd.x;
        pos.y += spd.y;
        pos.z += spd.z;
        /* @bug logic: pos.y holds the height above the target's center, and
         * the test compares it with the center's height in the world, where
         * 0 is meant (0x8c04bc4c) */
        if (pos.y > best->pos.y) {
            continue;
        }
        pos.x += best->pos.x - work->pos.x;
        pos.z += best->pos.z - work->pos.z;
        distance = njSqrt((pos.x * pos.x) + (pos.z * pos.z));
        if (distance > best->across) {
            distance = best->across - (best->radius * LONG_AIM) - distance;
            spd.x = distance * njCos(ang);
            spd.y = 0.0F;
            spd.z = distance * njSin(ang);
        } else if (distance > best->across - (best->radius * LONG_AIM)) {
            /* a landing less than LONG_AIM radii short of the center keeps its
             * place. steering aims a fast fall at the target's near edge. the
             * test compares the square of the fall's speed in x and z with
             * running_speed itself, not its square */
            if ((spd.x * spd.x) + (spd.z * spd.z) >
                record->consts.running_speed) {
                distance = best->across - best->radius - distance;
                spd.x = distance * njCos(ang);
                spd.y = 0.0F;
                spd.z = distance * njSin(ang);
            } else {
                spd.x = spd.y = spd.z = 0.0F;
            }
        } else {
            distance = best->across - (best->radius * SHORT_AIM) - distance;
            spd.x = distance * njCos(ang);
            spd.y = 0.0F;
            spd.z = distance * njSin(ang);
        }
        /* @hack spelling: the steps in distance, where each division could
         * take the sum */
        distance = (Float)i + 1.0F;
        spd.x /= distance;
        spd.z /= distance;
        if (spd.x > record->consts.ground_accel) {
            spd.x = record->consts.ground_accel;
        }
        if (spd.x < -record->consts.ground_accel) {
            spd.x = -record->consts.ground_accel;
        }
        if (spd.z > record->consts.ground_accel) {
            spd.z = record->consts.ground_accel;
        }
        if (spd.z < -record->consts.ground_accel) {
            spd.z = -record->consts.ground_accel;
        }
        motion->acc.x += spd.x;
        motion->acc.z += spd.z;
        if (best->off < TARGET_TURN_ARC && best->off > -TARGET_TURN_ARC) {
            work->ang.y =
                angle_approach(work->ang.y, best->ang, TARGET_TURN_STEP);
        }
        player_accel_roll(work, motion, record);
        return TRUE;
    } while (++i < FALL_STEPS);
    return FALSE;
}

/* how far beyond its own radius an Eggman sets a task down, where another
 * character sets it down at its constants' radius */
#define SET_DOWN_EGGMAN_REACH 5.0F

void player_set_down(struct task_work *work, struct task_work *held,
                     struct player *record)
{
    struct collision_work *collision;
    Float reach;
    NJS_POINT3 center;
    NJS_VECTOR away;

    collision = held->collision;
    if (collision == NULL) {
        return;
    }
    /* @hack spelling: a switch of one case, where an if would do */
    switch (record->character) {
    case PLAYER_EGGMAN:
        reach = collision->shapes[1].radius + SET_DOWN_EGGMAN_REACH;
        break;
    default:
        reach = record->consts.radius + collision->shapes[1].radius;
        break;
    }
    collision_get_shape_center(held, &collision->shapes[1], &center);
    if (reach > vector_distance(&work->pos, &center)) {
        away = held->pos;
        njSubVector(&away, &work->pos);
        away.y = 0.0F;
        njUnitVector(&away);
        held->pos.x += work->pos.x + (reach * away.x) - center.x;
        held->pos.z += work->pos.z + (reach * away.z) - center.z;
    }
}

/* the first frame at which a refused ground or wall dig counts as an attack */
#define ATTACK_FRAME 3.0F

Bool player_attacks_task(struct task *task, struct task *other)
{
    /* @hack type: other_work and slot volatile, where plain locals would
     * do */
    struct task_work *volatile other_work;
    struct player *record;
    struct collision_work *collision;
    struct task *touched;
    volatile int slot;
    int rests;
    int character;
    /* @hack redundant: a point nothing uses, which sizes the frame */
    NJS_POINT3 unused; /* NOLINT(clang-diagnostic-unused-variable) */

    if (other == NULL) {
        return FALSE;
    }
    /* @hack spelling: the work tested again in an else-if chain, where
     * separate ifs would do */
    if ((other_work = other->work) == NULL) {
        return FALSE;
    } else if (other_work != NULL && other_work->id == WORK_ID_ATTACK) {
        return TRUE;
    } else if (other_work->collision == NULL) {
        return FALSE;
    }
    if ((slot = player_slot_of(other)) == -1) {
        return FALSE;
    }
    record = player_records[slot];
    collision = task->work->collision;
    other_work = player_work[slot];
    collision_restart_scans();
    touched = collision_next_player(task);
    /* @hack spelling: the two tests nested, where one || would do */
    if (collision->flags & COLLISION_UNDER) {
        if (record->stood_on == task) {
            rests = 1;
        } else if (collision_next_above(task, 0) == other) {
            rests = 1;
        } else {
            rests = 0;
        }
    } else {
        rests = 0;
    }
    /* @hack spelling: character set inside the first test, where a
     * statement of its own would do */
    if ((((character = record->character) == PLAYER_SONIC ||
          character == PLAYER_SHADOW) &&
         other_work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
         other_work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3 &&
         !(collision->flags & COLLISION_UNDER)) ||
        (character == PLAYER_SONIC &&
         other_work->mode == PLAYER_MODE_SONIC_BOUNCE) ||
        ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
         ((rests == 1 && touched == other) || record->ground_task == task) &&
         ((record->anim.current == ANIM_DIG_FAIL &&
           record->anim.frame >= ATTACK_FRAME) ||
          (record->anim.current == ANIM_DIG_WALL_FAIL &&
           record->anim.frame >= ATTACK_FRAME) ||
          record->anim.next == ANIM_DIVE ||
          record->anim.next == ANIM_ROUGE_DIVE ||
          record->anim.current == ANIM_DIVE ||
          record->anim.current == ANIM_ROUGE_DIVE ||
          record->anim.last == ANIM_ROUGE_DIVE ||
          record->anim.last == ANIM_DIVE)) ||
        ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
         rests == 0 && touched == other &&
         other_work->mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
         other_work->mode <= PLAYER_MODE_HUNTER_RUN_ATTACK_3)) {
        return TRUE;
    }
    return FALSE;
}

Bool player_breaks_task(struct task *task, struct task *other)
{
    int slot;
    struct player *record;
    int character;

    if (player_attacks_task(task, other) != TRUE) {
        return FALSE;
    }
    if ((slot = player_slot_of(other)) == -1) {
        return FALSE;
    }
    record = player_records[slot];
    character = record->character;
    if (character == PLAYER_SONIC &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_FLAMERING))) {
        return TRUE;
    }
    if (character == PLAYER_TAILS_MECH &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON))) {
        return TRUE;
    }
    if (character == PLAYER_KNUCKLES &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES))) {
        return TRUE;
    }
    if (character == PLAYER_SHADOW &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
        return TRUE;
    }
    if (character == PLAYER_EGGMAN_MECH &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA))) {
        return TRUE;
    }
    if (character == PLAYER_ROUGE &&
        (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) {
        return TRUE;
    }
    return FALSE;
}

/* @hack type: task volatile, where a plain parameter would do */
Bool player_hit_attacks(struct task *volatile task, struct task *other)
{
    /* @hack type: other_work and slot volatile, where plain locals would
     * do */
    struct task_work *volatile other_work;
    struct player *record;
    volatile int slot;
    int character;

    if (other == NULL) {
        return FALSE;
    } else if ((other_work = other->work) == NULL) {
        return FALSE;
    } else if (other_work->collision == NULL) {
        return FALSE;
    }
    if ((slot = player_slot_of(other_work->collision->owner)) == -1) {
        return FALSE;
    }
    record = player_records[slot];
    other_work = player_work[slot];
    character = record->character;
    if (character == PLAYER_SONIC || character == PLAYER_SHADOW) {
        if (other_work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
            other_work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
            return TRUE;
        }
        if (other_work->mode == PLAYER_MODE_SONIC_LIGHT_ATTACK) {
            return TRUE;
        }
    }
    if ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
        other_work->mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
        other_work->mode <= PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
        return TRUE;
    }
    if (record->flags & PLAYER_INVINCIBLE) {
        return TRUE;
    }
    return FALSE;
}

/* the work's flags an event takes from its player */
#define EVENT_CLEARS                                                           \
    (PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH)

void player_start_event(int character, struct player_event *event)
{
    int slot;

    slot = player_find_character(character);
    if (slot == -1) {
        if ((slot = player_count()) < PLAYER_SLOTS) {
            switch (character) {
            case PLAYER_SONIC:
                sonic_create(slot);
                break;
            case PLAYER_SHADOW:
                shadow_create(slot);
                break;
            case PLAYER_KNUCKLES:
                knuckles_create(slot);
                break;
            case PLAYER_ROUGE:
                rouge_create(slot);
                break;
            case PLAYER_TAILS:
                tails_create(slot);
                break;
            case PLAYER_EGGMAN:
                eggman_create(slot);
                break;
            case PLAYER_EGGMAN_MECH:
                walker_create_eggman(slot);
                break;
            case PLAYER_TAILS_MECH:
                walker_create_tails(slot);
                break;
            }
        }
        player_records[slot]->flags |= PLAYER_FOR_EVENT;
    }
    player_records[slot]->event = event;
    player_work[slot]->mode = PLAYER_MODE_EVENT;
    player_stop(player_motion[slot], player_records[slot]);
    player_work[slot]->flags &= ~EVENT_CLEARS;
}

void player_end_event(int character)
{
    int slot;

    if ((slot = player_find_character(character)) != -1) {
        if (player_records[slot]->flags & PLAYER_FOR_EVENT) {
            task_kill(player_tasks[slot]);
        } else {
            player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        }
        player_records[slot]->event = NULL;
    }
}

/* the first of the slots of model_table, by character, whose objects' first
 * children player_hide_event_models hides. each character has four such slots,
 * and Rouge two */
#define EVENT_SONIC_MODEL 21
#define EVENT_SHADOW_MODEL 86
#define EVENT_KNUCKLES_MODEL 156
#define EVENT_ROUGE_MODEL 186
#define EVENT_TAILS_MODEL 224

/* the first child of model_table's object in slot */
#define EVENT_NODE(slot) (model_table[slot].object->child)

void player_hide_event_models(int slot)
{
    int character;

    character = player_records[slot]->character;
    switch (character) {
    case PLAYER_SONIC:
        EVENT_NODE(EVENT_SONIC_MODEL)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 1)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 2)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 3)->evalflags |= NJD_EVAL_HIDE;
        break;
    case PLAYER_SHADOW:
        EVENT_NODE(EVENT_SHADOW_MODEL)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 1)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 2)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 3)->evalflags |= NJD_EVAL_HIDE;
        break;
    case PLAYER_KNUCKLES:
        EVENT_NODE(EVENT_KNUCKLES_MODEL)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 1)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 2)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 3)->evalflags |= NJD_EVAL_HIDE;
        break;
    case PLAYER_ROUGE:
        EVENT_NODE(EVENT_ROUGE_MODEL)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_ROUGE_MODEL + 1)->evalflags |= NJD_EVAL_HIDE;
        break;
    case PLAYER_TAILS:
        EVENT_NODE(EVENT_TAILS_MODEL)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 1)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 2)->evalflags |= NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 3)->evalflags |= NJD_EVAL_HIDE;
        break;
    }
}

void player_draw_event_parts(int slot)
{
    struct player *record;
    int character;

    record = player_records[slot];
    character = record->character;
    if (record->event->part0.object != NULL) {
        if (record->event->part0.shape != NULL) {
            njCnkSimpleDrawShapeMotion(
                record->event->part0.object, record->event->part0.motion,
                record->event->part0.shape, mini_event_frame);
        } else if (record->event->part0.motion != NULL) {
            njCnkSimpleDrawMotion(record->event->part0.object,
                                  record->event->part0.motion,
                                  mini_event_frame);
        } else {
            njCnkSimpleDrawObject(record->event->part0.object);
        }
    }
    if (record->event->part1.object != NULL) {
        if (record->event->part1.shape != NULL) {
            njCnkSimpleDrawShapeMotion(
                record->event->part1.object, record->event->part1.motion,
                record->event->part1.shape, mini_event_frame);
        } else if (record->event->part1.motion != NULL) {
            njCnkSimpleDrawMotion(record->event->part1.object,
                                  record->event->part1.motion,
                                  mini_event_frame);
        } else {
            njCnkSimpleDrawObject(record->event->part1.object);
        }
    }
    if (record->character != PLAYER_ROUGE &&
        record->event->part2.object != NULL) {
        if (record->event->part2.shape != NULL) {
            njCnkSimpleDrawShapeMotion(
                record->event->part2.object, record->event->part2.motion,
                record->event->part2.shape, mini_event_frame);
        } else if (record->event->part2.motion != NULL) {
            njCnkSimpleDrawMotion(record->event->part2.object,
                                  record->event->part2.motion,
                                  mini_event_frame);
        } else {
            njCnkSimpleDrawObject(record->event->part2.object);
        }
    }
    if (record->character != PLAYER_ROUGE &&
        record->event->part3.object != NULL) {
        if (record->event->part3.shape != NULL) {
            njCnkSimpleDrawShapeMotion(
                record->event->part3.object, record->event->part3.motion,
                record->event->part3.shape, mini_event_frame);
        } else if (record->event->part3.motion != NULL) {
            njCnkSimpleDrawMotion(record->event->part3.object,
                                  record->event->part3.motion,
                                  mini_event_frame);
        } else {
            njCnkSimpleDrawObject(record->event->part3.object);
        }
    }
    switch (character) {
    case PLAYER_SONIC:
        EVENT_NODE(EVENT_SONIC_MODEL)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 1)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 2)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SONIC_MODEL + 3)->evalflags &= ~NJD_EVAL_HIDE;
        break;
    case PLAYER_SHADOW:
        EVENT_NODE(EVENT_SHADOW_MODEL)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 1)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 2)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_SHADOW_MODEL + 3)->evalflags &= ~NJD_EVAL_HIDE;
        break;
    case PLAYER_KNUCKLES:
        EVENT_NODE(EVENT_KNUCKLES_MODEL)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 1)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 2)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_KNUCKLES_MODEL + 3)->evalflags &= ~NJD_EVAL_HIDE;
        break;
    case PLAYER_ROUGE:
        EVENT_NODE(EVENT_ROUGE_MODEL)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_ROUGE_MODEL + 1)->evalflags &= ~NJD_EVAL_HIDE;
        break;
    case PLAYER_TAILS:
        EVENT_NODE(EVENT_TAILS_MODEL)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 1)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 2)->evalflags &= ~NJD_EVAL_HIDE;
        EVENT_NODE(EVENT_TAILS_MODEL + 3)->evalflags &= ~NJD_EVAL_HIDE;
        break;
    }
}

/* a mech's gauge, full, and with UPGRADE_EWALKER_EXTRASHIELD */
#define GAUGE_FULL 10.0F
#define GAUGE_SHIELDED 12.0F

void player_add_gauge(int slot, Float amount)
{
    struct player *record;
    Float full;

    record = player_records[slot];
    if (record != NULL && (record->character == PLAYER_TAILS_MECH ||
                           record->character == PLAYER_EGGMAN_MECH)) {
        record->gauge += amount;
        full = GAUGE_FULL;
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD)) {
            full = GAUGE_SHIELDED;
        }
        if (record->gauge <= 0.0F) {
            record->gauge = 0.0F;
        } else if (record->gauge >= full) {
            record->gauge = full;
        }
    }
}

Bool player_touch_attacks(struct task *task)
{
    struct task *touched;
    int slot;
    struct player *record;
    struct task_work *work;
    int character;

    /* @hack redundant: touched's address taken, which keeps it in the
     * frame */
    (void)&touched;
    touched = collision_next_player(task);
    if (touched == NULL) {
        return FALSE;
    }
    slot = player_slot_of(touched);
    if (slot == -1) {
        return FALSE;
    }
    record = player_records[slot];
    work = player_work[slot];
    character = record->character;
    if ((character == PLAYER_SONIC || character == PLAYER_SHADOW) &&
        work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
        work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
        return TRUE;
    }
    return FALSE;
}
