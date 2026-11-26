/* the stages' small animals, as animal.h declares them */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef NONMATCHING
/* the types, the copies and the PVM checks of this side's reader of an
 * animal's file */
#include <stdint.h>
#include <string.h>

#include "engine/dc/pvm.h"
#endif

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/mesh.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/color.h"
#include "hud/hud.h"
#include "hud/item_stock.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the counter's work */
#define PLAYER(work) ((work)->tick)
#define COUNT(work) ((work)->ang.x)
#define LIMIT(work) ((work)->ang.y)
/* the phase of a full count's blink. the number shows while it is below
 * HALF_TURN */
#define BLINK(work) ((work)->ang.z)
#define SCALE(work) ((work)->scl.x)
#define ALPHA(work) ((work)->scl.y)

/* the counter's modes. a new count shows large, shrinks back to a scale of
 * 1, holds and fades out. a full count does the same in red, at its own
 * rates. it gives a life as its shrink ends */
#define COUNTER_IDLE 0
#define COUNTER_SHOW 1
#define COUNTER_SHRINK 2
#define COUNTER_HOLD 3
#define COUNTER_FADE 4
#define COUNTER_FULL_SHOW 5
#define COUNTER_FULL_SHRINK 6
#define COUNTER_FULL_HOLD 7
#define COUNTER_FULL_FADE 8

#define COUNTER_LEVEL 1
#define COUNTER_POP 1.5F /* the scale a count shows at, before it shrinks */
/* what each step takes from the scale and from the alpha */
#define SHRINK 0.025F
#define FADE 0.025F
#define FULL_SHRINK 0.035F
#define FULL_FADE 0.045F
/* a hold ends once the work's timer, counting its steps, passes these */
#define HOLD_STEPS 200
#define FULL_HOLD_STEPS 300
#define BLINK_STEP 0x800 /* what each step of a full hold adds to the phase */
#define NUMBER_SCALE 0.8F /* the number's scale, times the counter's */
#define WHITE 0xffffff
#define RED 0xff0000
/* the number's color takes ALPHA times ALPHA_SCALE, shifted left by
 * ALPHA_SHIFT */
#define ALPHA_SCALE 255.0F

/* each character's limit. Shadow's is Sonic's, and Rouge's is Knuckles's */
#define SONIC_LIMIT 20
#define KNUCKLES_LIMIT 10
#define MECH_LIMIT 15

/* an animal's record, in its task's extra */
struct animal {
    Sint32 mode; /* an ANIMAL_ mode below */
    Sint32 slot; /* of the stage's five, and of animal_models */
    Sint32 kind; /* an enum animal_kind */
    Angle angle; /* where an ANIMAL_CIRCLE animal stands round its circle */
    Float speed; /* along y, under ANIMAL_LEAP and ANIMAL_RUN */
    /* no player can take the animal while it is above 0. the animal draws
     * only while it is even, and each step counts it down to 0 */
    Sint32 timer;
    Float frame; /* the motion's frame */
    Sint32 up_axis; /* collision_get_up_axis's result at the last step */
};

/* the record's modes */
/* it circles its work's pos, RADIUS from it, and turns TURN a step */
#define ANIMAL_CIRCLE 0
#define ANIMAL_STAND 1
/* it draws nothing until a player within RADIUS takes the action
 * ANIMAL_ACTION offers. it then runs */
#define ANIMAL_HIDDEN 2
#define ANIMAL_BUBBLE 3 /* it floats in a bubble for BUBBLE_STEPS steps */
/* it leaps along y, up under COLLISION_UP_Y and down under the other up
 * axes. it runs once its speed turns back */
#define ANIMAL_LEAP 4
/* it runs to the nearest player, falling to the floor. a peacock, a parrot
 * or a condor flies instead, to BIRD_HEIGHT above the player. under the
 * other up axes it flies below */
#define ANIMAL_RUN 5

/* the animal's work. RADIUS is the radius of its circle, or of a hidden
 * animal's reach */
#define RADIUS(work) ((work)->scl.x)
#define TURN(work) ((work)->ang.z)
/* in the work's flags: the animal counts on no counter */
#define ANIMAL_UNCOUNTED 0x100
/* the work's modes, of which a capture effect's work takes the first two */
#define WORK_START 0
#define WORK_LIVE 1
#define WORK_TAKEN 2 /* a player took it, and the next step kills it */

/* the slots the release kinds take from */
#define FIRST_SLOT 0
#define SECOND_SLOT 1
#define THIRD_SLOT 2
#define FOURTH_SLOT 3
#define FIFTH_SLOT 4
/* animal_random_slot picks one of the first RANDOM_SLOTS slots */
#define RANDOM_SLOTS 4

#define ANIMAL_LEVEL 3
#define CAPTURE_LEVEL 3
/* the action a hidden animal offers a player within reach. a player who
 * takes it is in the mode of the same number */
#define ANIMAL_ACTION 42
#define ANIMAL_RELEASE_SOUND 0x8010
#define ANIMAL_RELEASE_VOLUME 120
#define RELEASE_TIMER 60 /* a released animal's timer */
/* the speed of a leap, and of a hidden animal's hop */
#define LEAP_SPEED 2.5F
/* under COLLISION_UP_Y, each step takes GRAVITY from the speed along y.
 * under the other up axes, it adds GRAVITY. TOP_SPEED is the most the speed
 * reaches either way */
#define GRAVITY 0.1F
#define TOP_SPEED 2.5F
/* a leap stops CEILING_GAP below a ceiling, or under the other up axes,
 * FLOOR_GAP above a floor */
#define CEILING_GAP 2.0F
#define FLOOR_GAP 2.0F
/* how far a run moves each step, across x and z */
#define RUN_STEP 0.2F
/* a run looks for a floor below a point RUN_PROBE above its next place. under
 * the other up axes it looks for a ceiling above a point RUN_PROBE below */
#define RUN_PROBE 0.2F
/* how far a runner moves along y the step after the up axis changes */
#define AXIS_HOP 2.0F
#define BIRD_HEIGHT 5.0F
/* a bubble's steps, and the steps before a player can take its animal */
#define BUBBLE_STEPS 600
#define BUBBLE_SAFE_STEPS 10
#define BUBBLE_HEIGHT 2.5F /* where the bubble draws above the animal */
/* a player takes an animal whose squared distance from it across x and z
 * lies below TAKE_SQUARED. the animal's height over the player must also lie
 * between TAKE_BELOW and TAKE_ABOVE */
#define TAKE_SQUARED 49.0F
#define TAKE_BELOW (-5.0F)
#define TAKE_ABOVE 10.0F
/* each mech's TAKE_SQUARED and TAKE_ABOVE, which reach further */
#define EGGMAN_TAKE_SQUARED 144.0F
#define EGGMAN_TAKE_ABOVE 18.0F
#define TAILS_TAKE_SQUARED 121.0F
#define TAILS_TAKE_ABOVE 15.0F

#define MODEL_SCALE 1.4F
/* the shadow's height and scale, under the model's scale */
#define SHADOW_HEIGHT 2.0F
#define SHADOW_SCALE_X 1.2F
#define SHADOW_SCALE_Y 0.5F
#define SHADOW_SCALE_Z 1.8F

/* the radius find_ceiling and find_floor gather the world's objects in, and
 * their results where they find none */
#define PROBE_RADIUS 10.0F
#define NO_CEILING_FOUND 1.0e10F
#define NO_FLOOR_FOUND (-1.0e10F)

#ifdef NONMATCHING
/* Word zero counts itself and the section sizes that follow. The first two
 * sections hold a chunk model and its PVM. Of the remaining motions the
 * stage plays the first. All the sections follow this 32-byte header. */
struct animal_file_head {
    uint32_t words;
    uint32_t sizes[7];
};
#else
/* the head of an animal's file. the model's chunks follow it. the textures
 * and the motion lie at the offsets it gives */
struct animal_file_head {
    Sint32 unk_00;
    Sint32 pvm_offset; /* from MODEL_CHUNKS to the textures' PVM */
    Sint32 motion_offset; /* from the PVM to the motion's chunks */
    Uint8 unk_0c[0x14];
};
#endif
/* animals_start reads each file at the second megabyte of WORK_AREA, and
 * MODEL_CHUNKS lies past its head */
#ifdef NONMATCHING
/* the hack's plain form: this side's objects don't define animal_model_file,
 * which only symbols.txt places */
#define ANIMAL_FILE ((struct animal_file_head *)WORK_AREA_FILE)
#else
/* @hack spelling: SHC loads the header through its own relocation. */
extern struct animal_file_head animal_model_file;
#define ANIMAL_FILE (&animal_model_file)
#endif
#define MODEL_CHUNKS                                                           \
    ((Sint8 *)(WORK_AREA_FILE + sizeof(struct animal_file_head)))
#define MODEL_TEXTURES 16 /* the textures a model's list has room for */

/* a kind's file, MINIMAL_<name>.PRS, and the frames its motion plays a
 * step */
struct animal_file {
    const char *name;
    Float speed;
};

/* a slot's model, as animals_start loads it from its kind's file. a slot
 * whose object is null is empty */
struct animal_model {
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion; /* or null */
    Float speed; /* the motion's frames a step */
};

/* The bubble around a captive animal uses CHAOSDRIVE_TEX texture 3.
 * Its mirrored U coordinates cross the texture seam. */

static Sint16 animal_bubble_polygons[419] = {
#include "object/animal/animal_bubble_polygons.inc"
};
static Sint32 animal_bubble_vertices[351] = {
#include "object/animal/animal_bubble_vertices.inc"
};
static NJS_CNK_MODEL animal_bubble_model = {
#include "object/animal/animal_bubble_model.inc"
};
static NJS_CNK_OBJECT animal_bubble_object = {
#include "object/animal/animal_bubble_object.inc"
};

/* clang-format off */
static const struct collision_shape animal_shape = {
    10, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_FIRM(PUSH_PLAYERS | PUSH_PICKUPS),
    SHAPE_GUARD_LEVEL(3), SHAPE_UNTURNED, {0.0F, 2.0F, 0.0F}, 2.5F, 0.0F, 0.0F,
    0, {0, 0, 0},
};
/* the whole of CHAOSDRIVE_TEX's texture 5, as a cell 14 pixels square about
 * its center */
static const NJS_TEXANIM animal_glow_cell = {
#include "object/animal/animal_glow_cell.inc"
};
/* clang-format on */

#include "object/animal/text.inc"

static struct animal_file animal_files[ANIMAL_KINDS] = {
#include "object/animal/animal_files.inc"
};

/* each stage's five kinds, by stage, under the name stage/stage.h gives the
 * stage. a row of 0s names ANIMAL_PEN five times */
static Sint32 stage_animals[STAGE_STG_END][STAGE_ANIMALS] = {
#include "object/animal/stage_animals.inc"
};

/* a sprite of animal_glow_cell */
/* clang-format off */
static NJS_SPRITE animal_glow = {
    {0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 0, &chaosdrive_tex_texlist,
    (NJS_TEXANIM *)&animal_glow_cell,
};
/* clang-format on */

/* the kept count, which the next counter starts from */
static Sint8 kept_animals = 0;
/* each player's counter, or null */
static struct task *animal_counters[2] = {NULL, NULL};
/* clang-format off */
/* the count's number on a full screen */
static struct hud_number counter_number = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED, HUD_NUMBER_RIGHT, 0, 99, 0,
    {560.0F, 392.0F, 0.0F}, 0.5F, 0xffffffff,
};
/* player 0's and player 1's numbers on a split screen. the draw never sets
 * their value, scale or color */
static struct hud_number counter_number_left = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS, 0, 0, 99, 0,
    {270.0F, 400.0F, 0.0F}, 0.5F, 0xffffffff,
};
static struct hud_number counter_number_right = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS, 0, 0, 99, 0,
    {550.0F, 400.0F, 0.0F}, 0.5F, 0xffffffff,
};
/* clang-format on */
/* how a bubble moves. each step moves it bubble_rise along y, plus
 * bubble_bob times the sine of bubble_bob_turn times its work's timer. it
 * also moves bubble_drift along its heading, which turns by bubble_turn */
/* @hack type: volatile on all five, where plain Floats and Angles would do */
static volatile Float bubble_rise = -0.008F;
static volatile Float bubble_bob = 0.1F;
static volatile Float bubble_drift = 0.1F;
static volatile Angle bubble_bob_turn = 0x222;
static volatile Angle bubble_turn = 0xda;

/* the stage's five models, by slot */
static struct animal_model animal_models[STAGE_ANIMALS];

static void animal_counter_step(struct task *task);
static void animal_counter_draw(struct task *task);
static void animal_counter_destroy(struct task *task);
static void capture_effect_step(struct task *task);
static void animal_step(struct task *task);
static void animal_draw(struct task *task);
static void animal_destroy(struct task *task);

void kept_animals_clear(void)
{
    kept_animals = 0;
}

/* count an animal on player's counter, where player has one. a count at its
 * limit stays there, and shows as full */
static void animal_counter_add(int player)
{
    struct task_work *work;

    if (animal_counters[player] != NULL) {
        work = animal_counters[player]->work;
        COUNT(work)++;
        if (COUNT(work) >= LIMIT(work)) {
            COUNT(work) = LIMIT(work);
            work->mode = COUNTER_FULL_SHOW;
        } else {
            work->mode = COUNTER_SHOW;
        }
    }
}

static void animal_counter_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case COUNTER_IDLE:
        break;
    case COUNTER_SHOW:
        SCALE(work) = COUNTER_POP;
        ALPHA(work) = 1.0F;
        work->mode = COUNTER_SHRINK;
        /* fall through */
    case COUNTER_SHRINK:
        SCALE(work) -= SHRINK;
        if (SCALE(work) < 1.0F) {
            SCALE(work) = 1.0F;
            work->timer = 0;
            work->mode = COUNTER_HOLD;
        }
        break;
    case COUNTER_HOLD:
        if (work->timer++ > HOLD_STEPS) {
            work->mode = COUNTER_FADE;
            BLINK(work) = 0;
        }
        break;
    case COUNTER_FADE:
        ALPHA(work) -= FADE;
        if (ALPHA(work) < 0.0F) {
            ALPHA(work) = 0.0F;
            work->mode = COUNTER_IDLE;
        }
        break;
    case COUNTER_FULL_SHOW:
        SCALE(work) = COUNTER_POP;
        ALPHA(work) = 1.0F;
        work->mode = COUNTER_FULL_SHRINK;
        /* fall through */
    case COUNTER_FULL_SHRINK:
        SCALE(work) -= FULL_SHRINK;
        if (SCALE(work) < 1.0F) {
            SCALE(work) = 1.0F;
            work->timer = 0;
            work->mode = COUNTER_FULL_HOLD;
            lives_add(PLAYER(work), 1);
        }
        break;
    case COUNTER_FULL_HOLD:
        BLINK(work) = (Uint16)(BLINK(work) + BLINK_STEP);
        if (work->timer++ > FULL_HOLD_STEPS) {
            work->mode = COUNTER_FULL_FADE;
            BLINK(work) = 0;
        }
        break;
    case COUNTER_FULL_FADE:
        ALPHA(work) -= FULL_FADE;
        if (ALPHA(work) < 0.0F) {
            ALPHA(work) = 0.0F;
            work->mode = COUNTER_IDLE;
        }
        break;
    }
}

static void animal_counter_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct hud_number *number;
    Uint32 color;
    Uint16 alpha;

    switch (work->mode) {
    case COUNTER_FULL_SHOW:
    case COUNTER_FULL_SHRINK:
    case COUNTER_FULL_HOLD:
    case COUNTER_FULL_FADE:
        color = RED;
        break;
    default:
        color = WHITE;
        break;
    }
    alpha = ALPHA(work) * ALPHA_SCALE;
    number = &counter_number;
    number->value = COUNT(work);
    number->color = (alpha << ALPHA_SHIFT) | color;
    number->scale = SCALE(work) * NUMBER_SCALE;
    if (BLINK(work) < HALF_TURN) {
        /* never: a two-player game starts no counter */
        if (game_state.two_players) {
            switch (PLAYER(work)) {
            case 0:
                if (camera_current_index == 0) {
                    hud_number_draw(&counter_number_left);
                }
                break;
            case 1:
                if (camera_current_index == 1) {
                    hud_number_draw(&counter_number_right);
                }
                break;
            }
        } else {
            hud_number_draw(&counter_number);
        }
    }
}

static void animal_counter_destroy(struct task *task)
{
    struct task_work *work = task->work;

    animal_counters[PLAYER(work)] = NULL;
    if (game_state.cleared == 1) {
        kept_animals = COUNT(work);
    }
}

/* @hack type: an int result that no caller reads, where void would do */
static int animal_counter_start(int player)
{
    struct task *task;
    struct task_work *work;

#ifdef NONMATCHING
    /* A later start can retry an animal file while the counter is live. */
    if (animal_counters[player] != NULL) {
        return 0;
    }
#endif
    if (game_state.two_players || stage_current_id() == STAGE_GREEN_HILL) {
        return 0;
    }
    task = task_create(TASK_WORK, COUNTER_LEVEL, animal_counter_step,
                       "MinimalCounterExecutor");
    if (task != NULL) {
        work = task->work;
        PLAYER(work) = player;
        COUNT(work) = kept_animals;
        switch (player_character(player)) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
            LIMIT(work) = SONIC_LIMIT;
            break;
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            LIMIT(work) = KNUCKLES_LIMIT;
            break;
        case PLAYER_TAILS_MECH:
        case PLAYER_EGGMAN_MECH:
            LIMIT(work) = MECH_LIMIT;
            break;
        }
        ALPHA(work) = 0.0F;
        BLINK(work) = 0;
        task->draw = animal_counter_draw;
        task->destructor = animal_counter_destroy;
        animal_counters[PLAYER(work)] = task;
    }
#ifdef NONMATCHING
    /* a result for GCC, which no caller reads */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* animal_counter_start starts no counter in a two-player game, so only
 * player 0's ever starts */
static void animal_counters_start(void)
{
    if (game_state.two_players) {
        animal_counter_start(0);
        animal_counter_start(1);
    } else {
        animal_counter_start(0);
    }
}

static void capture_effect_draw(struct task *task)
{
}

static void capture_effect_destroy(struct task *task)
{
}

static void capture_effect_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == WORK_START) {
        task->draw = capture_effect_draw;
        task->destructor = capture_effect_destroy;
        work->mode = WORK_LIVE;
    }
    task_destroy(task);
}

/* the effect a taken animal leaves at pos, which draws nothing and ends on
 * its first step. it returns the effect's task, or null where the heap can't
 * give it */
static struct task *capture_effect_create(NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, CAPTURE_LEVEL, capture_effect_step,
                       "MinimalCaptureEffect_Exec");
    if (task != NULL) {
        work = task->work;
        work->pos = *pos;
    }
    return task;
}

/* the height of the lowest ceiling above pos among the world's objects within
 * PROBE_RADIUS of it, or NO_CEILING_FOUND */
static Float find_ceiling(NJS_POINT3 *pos)
{
    struct height_probe probe;
    Float ceiling = NO_CEILING_FOUND;
    Float height;
    int i;

    world_list_near(pos->x, pos->y, pos->z, PROBE_RADIUS);
    /* @hack spelling: probe.pos set a part at a time, where probe.pos = *pos
     * would do */
    probe.pos.x = pos->x;
    probe.pos.y = pos->y;
    probe.pos.z = pos->z;
    for (i = 0; i < world_count; i++) {
        if (polygon_find_heights(&probe, world_objects[i].object, 0) &&
            probe.above.found) {
            height = probe.above.pos;
            if (height < ceiling) {
                ceiling = height;
            }
        }
    }
    return ceiling;
}

/* the height of the highest floor below pos among the world's objects within
 * PROBE_RADIUS of it, or NO_FLOOR_FOUND */
static Float find_floor(NJS_POINT3 *pos)
{
    struct height_probe probe;
    Float ground = NO_FLOOR_FOUND;
    Float height;
    int i;

    world_list_near(pos->x, pos->y, pos->z, PROBE_RADIUS);
    /* @hack spelling: probe.pos set a part at a time, where probe.pos = *pos
     * would do */
    probe.pos.x = pos->x;
    probe.pos.y = pos->y;
    probe.pos.z = pos->z;
    for (i = 0; i < world_count; i++) {
        if (polygon_find_heights(&probe, world_objects[i].object, 0) &&
            probe.below.found) {
            height = probe.below.pos;
            if (height > ground) {
                ground = height;
            }
        }
    }
    return ground;
}

/* the current stage's kind in slot. a stage from STAGE_STG_END on takes
 * stage 0's row */
static int stage_animal(int slot)
{
    int stage = stage_current_id();

    /* @hack spelling: the bound tested in bytes, stage times a row's size
     * against the table's, where stage against STAGE_STG_END would do */
    if (stage * sizeof(stage_animals[0]) >= sizeof(stage_animals)) {
        stage = 0;
    }
    return stage_animals[stage][slot];
}

static int animal_pick_slot(int kind)
{
    int slot;

    /* @hack spelling: the default ahead of ANIMAL_FIFTH's case, where it
     * would follow the others */
    switch (kind) {
    default:
    case ANIMAL_FIFTH:
        slot = FIFTH_SLOT;
        break;
    case ANIMAL_THIRD_OR_FOURTH:
        if (rand() * RAND_SCALE < 0.5F) {
            slot = THIRD_SLOT;
        } else {
            slot = FOURTH_SLOT;
        }
        break;
    case ANIMAL_FIRST_OR_SECOND:
        if (rand() * RAND_SCALE < 0.5F) {
            slot = FIRST_SLOT;
        } else {
            slot = SECOND_SLOT;
        }
        break;
    }
    return slot;
}

/* @hack spelling: the record read through an inline function, where
 * task->extra would do */
#pragma inline(animal_of)
static struct animal *animal_of(struct task *task)
{
    return task->extra.p;
}

static void animal_step(struct task *task)
{
    struct task_work *work = task->work;
    struct animal *animal = animal_of(task);
    struct animal_model *model = animal_models;
    struct task_work *player_task_work;
    struct player *record;
    NJS_POINT3 pos;
    Float dx;
    Float dy;
    Float dz;
    Float vx;
    Float vy;
    Float vz;
    Float rate;
    Float distance;
    Float radius;
    Float step;
    Float height;
    int player;
    int taken;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (animal->timer > 0) {
        animal->timer--;
    }
    switch (work->mode) {
    case WORK_START:
        work->scl.z = 1.0F;
        work->mode = WORK_LIVE;
        work->ang.x = kept_animals; /* animal.c doesn't read ang.x again */
        /* fall through */
    case WORK_LIVE:
        if (animal->mode == ANIMAL_HIDDEN || animal->timer != 0 ||
            (animal->mode == ANIMAL_BUBBLE &&
             work->timer < BUBBLE_SAFE_STEPS)) {
            break;
        }
        player = 0;
        pos = work->pos;
        if (animal->mode == ANIMAL_CIRCLE) {
            pos.z += RADIUS(work) * njCos(animal->angle);
            pos.x += RADIUS(work) * njSin(animal->angle);
        }
        /* @hack type: game_state's two_players read as game_state_two_players,
         * where game_state.two_players would do */
        if (game_state_two_players) {
            player = player_nearest(&pos);
        }
        player_task_work = player_work[player];
        dx = pos.x - player_task_work->pos.x;
        /* @hack redundant: two tests of player_task_work that never run */
        if (0) {
            if (player_task_work) {
            }
            if (player_task_work) {
            }
        }
        dy = pos.y - player_task_work->pos.y;
        dz = pos.z - player_task_work->pos.z;
        taken = 0;
        distance = (dx * dx) + (dz * dz);
        /* @hack redundant: a test of dz and dx that never runs */
        if (0) {
            if (dz + dx) {
            }
        }
        /* @hack redundant: two stores of the work's mode that never run */
        if (0) {
            work->mode = WORK_LIVE;
            work->mode = WORK_LIVE;
        }
        switch (player_records[player]->character) {
        case PLAYER_EGGMAN_MECH:
            if (distance < EGGMAN_TAKE_SQUARED && dy > TAKE_BELOW &&
                dy < EGGMAN_TAKE_ABOVE) {
                taken = 1;
            }
            break;
        case PLAYER_TAILS_MECH:
            if (distance < TAILS_TAKE_SQUARED && dy > TAKE_BELOW &&
                dy < TAILS_TAKE_ABOVE) {
                taken = 1;
            }
            break;
        default:
            if (distance < TAKE_SQUARED && dy > TAKE_BELOW && dy < TAKE_ABOVE) {
                taken = 1;
            }
            break;
        }
        if (taken) {
            item_stock_push(player, animal->kind);
            /* the placement gives no task again until a reset:
             * set_reactivate passes it over */
            if (task->placement != NULL) {
                placement_keep(task);
                placement_stop(task);
            }
            capture_effect_create(&work->pos);
            if (!(work->flags & ANIMAL_UNCOUNTED)) {
                animal_counter_add(player);
            }
            work->mode = WORK_TAKEN;
            return;
        }
        /* @hack spelling: a case of its own for ANIMAL_CIRCLE and for
         * ANIMAL_STAND, where one case would do */
        switch (animal->mode) {
        case ANIMAL_CIRCLE:
            break;
        case ANIMAL_STAND:
            break;
        default:
            collision_register(task);
            break;
        }
        break;
    case WORK_TAKEN:
        task_kill(task);
        break;
    }
    if (game_state.time_stopped) {
        return;
    }
    switch (animal->mode) {
    case ANIMAL_CIRCLE:
        animal->angle += TURN(work);
        break;
    case ANIMAL_STAND:
        break;
    case ANIMAL_HIDDEN:
        for (i = 0; i < PLAYER_SLOTS; i++) {
            if (player_tasks[i]) {
                player_task_work = player_work[i];
                record = player_records[i];
                dx = work->pos.x - player_task_work->pos.x;
                dy = work->pos.y - player_task_work->pos.y;
                dz = work->pos.z - player_task_work->pos.z;
                radius = RADIUS(work);
                /* @hack redundant: tests of dy, dz, record and radius that
                 * never run */
                if (0) {
                    if (dy) {
                    }
                    if (dz) {
                    }
                    if (record) {
                    }
                    if (record) {
                    }
                    if (radius) {
                    }
                    if (radius) {
                    }
                }
                if ((dx * dx) + (dy * dy) + (dz * dz) < radius * radius) {
                    record->action = ANIMAL_ACTION;
                    if (player_task_work->mode == ANIMAL_ACTION) {
                        animal->mode = ANIMAL_RUN;
                        animal->timer = RELEASE_TIMER;
                        if (collision_get_up_axis() == COLLISION_UP_Y) {
                            animal->speed = LEAP_SPEED;
                        } else {
                            animal->speed = -LEAP_SPEED;
                        }
                    }
                }
            }
        }
        break;
    case ANIMAL_BUBBLE:
        if (++work->timer >= BUBBLE_STEPS) {
            work->timer = 0;
            animal->mode = ANIMAL_LEAP;
        } else {
            work->pos.y += bubble_rise +
                           (bubble_bob * njSin(work->timer * bubble_bob_turn));
            work->pos.z += bubble_drift * njCos(work->ang.y);
            work->pos.x += bubble_drift * njSin(work->ang.y);
            work->ang.y = work->ang.y + bubble_turn;
        }
        break;
    case ANIMAL_LEAP:
        if (collision_get_up_axis() == COLLISION_UP_Y) {
            if ((animal->speed += -GRAVITY) < -TOP_SPEED) {
                animal->speed = -TOP_SPEED;
            }
            work->pos.y += animal->speed;
            height = find_ceiling(&work->pos);
            if (work->pos.y + CEILING_GAP > height) {
                work->pos.y = height - CEILING_GAP;
            }
            if (animal->speed < 0.0F) {
                animal->mode = ANIMAL_RUN;
            }
        } else {
            if ((animal->speed -= -GRAVITY) > TOP_SPEED) {
                animal->speed = TOP_SPEED;
            }
            work->pos.y += animal->speed;
            height = find_floor(&work->pos);
            if (work->pos.y - FLOOR_GAP < height) {
                work->pos.y = height + FLOOR_GAP;
            }
            if (animal->speed > 0.0F) {
                animal->mode = ANIMAL_RUN;
            }
        }
        break;
    case ANIMAL_RUN:
        switch (animal->kind) {
        default:
            player_task_work = player_work[player_nearest(&work->pos)];
            dz = player_task_work->pos.z - work->pos.z;
            dx = player_task_work->pos.x - work->pos.x;
            step = RUN_STEP / njSqrt((dz * dz) + (dx * dx));
            work->ang.y = atan2f(dx, dz) * ANGLE_PER_RADIAN;
            pos.z = work->pos.z + (step * dz);
            pos.x = work->pos.x + (step * dx);
            if (collision_get_up_axis() == COLLISION_UP_Y) {
                if (collision_get_up_axis() != animal->up_axis) {
                    work->pos.y -= AXIS_HOP;
                }
                if ((animal->speed += -GRAVITY) < -TOP_SPEED) {
                    animal->speed = -TOP_SPEED;
                }
                pos.y = work->pos.y + RUN_PROBE;
                height = find_floor(&pos);
                if (!(height > work->pos.y)) {
                    work->pos.z = pos.z;
                    work->pos.x = pos.x;
                    work->pos.y += animal->speed;
                    if (work->pos.y < height) {
                        animal->speed = 0.0F;
                        work->pos.y = height;
                    }
                }
            } else {
                if (collision_get_up_axis() != animal->up_axis) {
                    work->pos.y += AXIS_HOP;
                }
                /* @bug logic: the cap tests the speed against -TOP_SPEED, so
                 * each step of a run under these up axes sets the speed to
                 * TOP_SPEED. FCMP/GT at 0x8c0a87d8 compares it with FR15,
                 * which holds -2.5, as COLLISION_UP_Y's test at 0x8c0a8728
                 * does */
                if ((animal->speed -= -GRAVITY) > -TOP_SPEED) {
                    animal->speed = TOP_SPEED;
                }
                pos.y = work->pos.y - RUN_PROBE;
                height = find_ceiling(&pos);
                if (!(work->pos.y > height)) {
                    work->pos.z = pos.z;
                    work->pos.x = pos.x;
                    work->pos.y += animal->speed;
                    if (work->pos.y > height) {
                        animal->speed = 0.0F;
                        work->pos.y = height;
                    }
                }
            }
            break;
        case ANIMAL_KUJA:
        case ANIMAL_OUM:
        case ANIMAL_KON:
            player_task_work = player_work[player_nearest(&work->pos)];
            vz = player_task_work->pos.z - work->pos.z;
            vx = player_task_work->pos.x - work->pos.x;
            /* @bug logic: the step's length counts z twice and leaves out y,
             * so a bird's step isn't RUN_STEP long. FMAC at 0x8c0a8852 and at
             * 0x8c0a8854 each add z's square, from FR15. vy holds the step's
             * part along y only from 0x8c0a8866 on */
            vy = player_task_work->pos.z - work->pos.z;
            rate = RUN_STEP / njSqrt((vy * vy) + (vx * vx) + (vz * vz));
            /* @hack redundant: tests of rate, vy and vx that never run */
            if (0) {
                if (rate + rate + vy + vy + vy + vy + vx + vx + vx) {
                }
            }
            if (collision_get_up_axis() == COLLISION_UP_Y) {
                vy = (player_task_work->pos.y - work->pos.y) + BIRD_HEIGHT;
            } else {
                vy = (player_task_work->pos.y - work->pos.y) - BIRD_HEIGHT;
            }
            work->ang.y = atan2f(vx, vz) * ANGLE_PER_RADIAN;
            work->pos.z += rate * vz;
            work->pos.x += rate * vx;
            work->pos.y += rate * vy;
            /* @hack spelling: no break at the switch's end, where one
             * would do */
        }
        break;
    }
    /* @bug logic: each animal's frame runs by the first slot's motion and
     * speed, while animal_draw_model draws the animal's own slot's motion. a
     * frame past the end of the animal's own motion draws nothing. MOV.L at
     * 0x8c0a80b0 sets model to animal_models, and MOV.L at 0x8c0a88b4 reads
     * its motion with no slot added. animal_draw_model adds the slot that
     * MOV.L at 0x8c0a8932 reads */
    if (model->motion != NULL) {
        if ((animal->frame += model->speed) >= model->motion->nbFrame) {
            animal->frame -= model->motion->nbFrame;
        }
    }
    animal->up_axis = collision_get_up_axis();
}

static void animal_draw_model(struct task *task)
{
    struct animal *animal = animal_of(task);
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *work = task->work;
    struct animal_model *model = &animal_models[animal->slot];

#ifdef NONMATCHING
    /* this side draws no slot whose file didn't load, which animal_load_model
     * leaves with no object */
    if (model->object == NULL) {
        return;
    }
#endif
    njSetTexture(model->texlist);
    njScale(NULL, MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
    if (collision_get_up_axis() != COLLISION_UP_Y) {
        njRotateZ(NULL, HALF_TURN);
    }
    if (model->motion != NULL) {
        njCnkEasyDrawMotion(model->object, model->motion, animal->frame);
    } else {
        njCnkEasyDrawObject(model->object);
    }
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, SHADOW_HEIGHT, 0.0F);
    njScale(NULL, SHADOW_SCALE_X, SHADOW_SCALE_Y, SHADOW_SCALE_Z);
    njCnkModDrawModel(shadow_model);
    njPopMatrixEx();
}

static void animal_draw(struct task *task)
{
    struct animal *animal = animal_of(task);
    struct task_work *work = task->work;
    Float x;
    Float z;

    switch (animal->mode) {
    case ANIMAL_BUBBLE:
        njSetTexture(&chaosdrive_tex_texlist);
        njPushMatrixEx();
        njTranslate(NULL, work->pos.x, work->pos.y + BUBBLE_HEIGHT,
                    work->pos.z);
        njCnkEasyDrawObject(&animal_bubble_object);
        njPopMatrixEx();
        /* fall through */
    case ANIMAL_STAND:
    case ANIMAL_LEAP:
    case ANIMAL_RUN:
    default:
        if (!(animal->timer & 1)) {
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            /* @hack redundant: a switch on the mode whose arms do nothing */
            switch (animal->mode) {
            case ANIMAL_CIRCLE:
                break;
            case ANIMAL_STAND:
                break;
            }
            njRotateY(NULL, work->ang.y);
            animal_draw_model(task);
            njPopMatrixEx();
        }
        break;
    case ANIMAL_CIRCLE:
        njPushMatrixEx();
        z = work->pos.z + (RADIUS(work) * njCos(animal->angle));
        x = work->pos.x + (RADIUS(work) * njSin(animal->angle));
        njTranslate(NULL, x, work->pos.y, z);
        njRotateY(NULL, animal->angle + QUARTER_TURN);
        animal_draw_model(task);
        njPopMatrixEx();
        break;
    case ANIMAL_HIDDEN:
        break;
    }
}

static void animal_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

#ifdef NONMATCHING
/* the word at data, which needn't be aligned */
static Uint32 animal_word(const Uint8 *data)
{
    uint32_t value;

    memcpy(&value, data, sizeof(value));
    return value;
}

/* njReadBinary copies the first chunk, then walks its POF0 relocations.
 * A delta counts four-byte words from the previous pointer in that copy.
 * The first pointer may be at zero, as it is in an NMDM motion header. */
static Bool animal_chunk_valid(const Uint8 *chunk, Uint32 bytes,
                               const char *tag, Uint32 root_size)
{
    const Uint8 *p;
    Uint32 size, left, offset, words, tail;
    Bool first = TRUE;

    if (bytes < 16 || memcmp(chunk, tag, 4) != 0) {
        return FALSE;
    }
    size = animal_word(chunk + 4);
    if (size < root_size || (size & 3) || size > bytes - 16) {
        return FALSE;
    }
    p = chunk + 8 + size;
    left = animal_word(p + 4);
    if (memcmp(p, "POF0", 4) != 0 || left > bytes - 16 - size) {
        return FALSE;
    }
    p += 8;
    offset = 0;
    while (left != 0) {
        unsigned int lead = *p++;

        left--;
        if (lead < 0x40) {
            continue;
        }
        tail = lead < 0x80 ? 0 : lead < 0xc0 ? 1 : 3;
        if (tail > left) {
            return FALSE;
        }
        words = lead & 0x3f;
        left -= tail;
        while (tail != 0) {
            words = (words << 8) | *p++;
            tail--;
        }
        if ((!first && words == 0) || words > (size - 4 - offset) / 4) {
            return FALSE;
        }
        offset += words * 4;
        if (animal_word(chunk + 8 + offset) >= size) {
            return FALSE;
        }
        first = FALSE;
    }
    return !first;
}

static void animal_free_model(struct animal_model *model)
{
    if (model->texlist != NULL) {
        texture_discard_pvm(model->texlist);
        syFree(model->texlist);
    }
    if (model->motion != NULL) {
        syFree(model->motion);
    }
    if (model->object != NULL) {
        syFree(model->object);
    }
    memset(model, 0, sizeof(*model));
}

/* this side repairs the tag in the matching animals_start: no slot holds a
 * motion without its object */

/* read file's model, and give model its object, motion and textures together.
 * it returns FALSE, with model as it was, where the file doesn't read whole or
 * a check fails */
static Bool animal_load_model(struct animal_model *model,
                              const struct animal_file *file)
{
    struct animal_model next = {0};
    const struct animal_file_head *head = ANIMAL_FILE;
    Uint8 *chunks = (Uint8 *)MODEL_CHUNKS;
    Uint8 *pvm;
    Uint8 *motion;
    Uint32 left, model_bytes, pvm_bytes, count, cursor, type, i;
    Sint32 size;
    char path[32];

    if (file->name == NULL || file->name[0] < 'A' || file->name[0] > 'Z') {
        return FALSE;
    }
    sprintf(path, "MINIMAL_%s.PRS", file->name);
    size = loader_read_prs_buffer(path, ANIMAL_FILE,
                                  (Uint32)(WORK_AREA_END - WORK_AREA_FILE));
    if (size < (Sint32)sizeof(*head) || head->words < 4 ||
        head->words > 1 + sizeof(head->sizes) / sizeof(head->sizes[0])) {
        return FALSE;
    }
    left = (Uint32)size - sizeof(*head);
    for (i = 0; i < head->words - 1; i++) {
        if (head->sizes[i] > left) {
            return FALSE;
        }
        left -= head->sizes[i];
    }
    if (left != 0) {
        return FALSE;
    }
    model_bytes = head->sizes[0];
    pvm_bytes = head->sizes[1];
    pvm = chunks + model_bytes;
    motion = pvm + pvm_bytes;
    if (!animal_chunk_valid(chunks, model_bytes, "NJCM",
                            sizeof(*next.object)) ||
        !animal_chunk_valid(motion, head->sizes[2], "NMDM",
                            sizeof(*next.motion))) {
        return FALSE;
    }
    count = texture_pvm_count(pvm, pvm_bytes);
    if (!count || count > MODEL_TEXTURES) {
        return FALSE;
    }
    cursor = 0;
    next.object = njReadBinary((Sint8 *)chunks, &cursor, &type);
    if (next.object == NULL) {
        return FALSE;
    }
    cursor = 0;
    next.motion = njReadBinary((Sint8 *)motion, &cursor, &type);
    if (next.motion == NULL) {
        animal_free_model(&next);
        return FALSE;
    }
    next.texlist = syCalloc(1, sizeof(*next.texlist) +
                                   count * sizeof(*next.texlist->textures));
    if (next.texlist == NULL) {
        animal_free_model(&next);
        return FALSE;
    }
    next.texlist->textures = (NJS_TEXNAME *)(next.texlist + 1);
    next.texlist->nbTexture = count;
    if (!texture_load_pvm_memory(pvm, pvm_bytes, next.texlist)) {
        animal_free_model(&next);
        return FALSE;
    }
    next.speed = file->speed;
    *model = next;
    return TRUE;
}

void animals_start(void)
{
    int slot;

    animal_counters_start();
    if ((Uint32)stage_current_id() > STAGE_STG_END) {
        return;
    }
    for (slot = 0; slot < STAGE_ANIMALS; slot++) {
        if (animal_models[slot].object == NULL) {
            animal_load_model(&animal_models[slot],
                              &animal_files[stage_animal(slot)]);
        }
    }
}

void animals_free(void)
{
    int slot;

    for (slot = 0; slot < STAGE_ANIMALS; slot++) {
        animal_free_model(&animal_models[slot]);
    }
}
#else
void animals_start(void)
{
    struct animal_model *model = animal_models;
    int i;
    int j;
    Sint8 *data;
    Sint8 *base;
    struct animal_file_head *head;
    struct animal_file *file;
    const char *name;
    Uint32 offset;
    Uint32 type;
    char path[32];
    char texname[28];

    animal_counters_start();
    if (stage_current_id() > STAGE_STG_END) {
        return;
    }
    for (i = 0; i < STAGE_ANIMALS; i++, model++) {
        if (!model->object) {
            file = &animal_files[stage_animal(i)];
            /* @hack spelling: name, a local for file->name, where the field
             * as the argument would do */
            name = file->name;
            sprintf(path, "MINIMAL_%s.PRS", name);
            /* @hack spelling: head, a local set before the read, where
             * ANIMAL_FILE as the argument would do */
            head = ANIMAL_FILE;
            loader_read_prs(path, head);
            /* @hack spelling: base, a local for MODEL_CHUNKS, where the
             * constant as the argument would do */
            base = MODEL_CHUNKS;
            offset = 0;
            model->object = njReadBinary(base, &offset, &type);
            data = MODEL_CHUNKS + head->pvm_offset;
            model->texlist =
                syCalloc(1, sizeof(NJS_TEXLIST) +
                                (MODEL_TEXTURES * sizeof(NJS_TEXNAME)));
            model->texlist->textures = (NJS_TEXNAME *)(model->texlist + 1);
            /* each texture's name points into texname, which dies with this
             * call */
            for (j = 0; j < MODEL_TEXTURES; j++) {
                model->texlist->textures[j].filename = texname;
            }
            njLoadTexturePvmMemory((Uint8 *)data, model->texlist);
            /* @hack redundant: a test of head that never runs */
            if (0) {
                if (head) {
                }
            }
            /* @hack spelling: data = data + ..., where data += ... would do */
            data = data + head->motion_offset;
            /* @bug memory: drops a motion it finds in the slot without
             * freeing it, and leaves the slot no motion. a slot holds a
             * motion but no object only after njReadBinary failed for the
             * object at an earlier start. TST at 0x8c0a8c4a finds the motion,
             * and MOV.L R13,@(8,R14) at 0x8c0a8c62 stores null over it */
            if (!model->motion) {
                offset = 0;
                model->motion = njReadBinary(data, &offset, &type);
            } else {
                model->motion = NULL;
            }
            model->speed = file->speed;
        }
    }
}

void animals_free(void)
{
    struct animal_model *model = animal_models;
    int i;

    for (i = STAGE_ANIMALS - 1; i >= 0; i--, model++) {
        if (model->texlist) {
            syFree(model->texlist);
            model->texlist = NULL;
        }
        if (model->object) {
            syFree(model->object);
            model->object = NULL;
        }
        if (model->motion) {
            syFree(model->motion);
            model->motion = NULL;
        }
    }
}
#endif

/* @hack spelling: the record allocated through an inline function, where
 * syMalloc(sizeof(struct animal)) would do */
#pragma inline(animal_alloc)
static struct animal *animal_alloc(void)
{
    return syMalloc(sizeof(struct animal));
}

/* @hack spelling: the random slot drawn through an inline function, where
 * rand() * (RANDOM_SLOTS * RAND_SCALE) would do */
#pragma inline(animal_random_slot)
static int animal_random_slot(void)
{
    return rand() * (RANDOM_SLOTS * RAND_SCALE);
}

/* make an animal of slot at (x, y, z) in mode. it returns the animal's task,
 * or null where the heap can't give the task or the record */
static struct task *animal_create(Float x, Float y, Float z, int slot, int mode)
{
    struct task *task;
    struct animal *animal;
    struct task_work *work;

    task = task_create(TASK_WORK, ANIMAL_LEVEL, animal_step, "Minimal_Exec");
    if (task) {
        animal = animal_alloc();
        if (!animal) {
            task_destroy(task);
            task = NULL;
        } else {
            work = task->work;
            animal->mode = mode;
            animal->slot = slot;
            animal->kind = stage_animal(slot);
            animal->speed = 0.0F;
            animal->timer = 0;
            animal->frame = 0.0F;
            work->pos.x = x;
            work->pos.y = y;
            work->pos.z = z;
            /* @hack type: the heading cast to a Uint16, where the Angle it
             * is stored as would do */
            work->ang.y = (Uint16)RANDOM_TURN();
            work->scl.z = 1.0F;
            work->timer = 0;
            task->extra.p = animal;
            task->step = animal_step;
            task->draw = animal_draw;
            task->destructor = animal_destroy;
            collision_init(task, &animal_shape, 1, COLLISION_ENEMY);
        }
    }
    return task;
}

struct task *animal_release(Float x, Float y, Float z, int kind)
{
    struct animal *animal;
    struct task *task;
    NJS_POINT3 pos;
    int slot;

    /* @hack spelling: slot, a local for the slot the kind picks, where the
     * pick in the call would do */
    slot = animal_pick_slot(kind);
    task = animal_create(x, y, z, slot, ANIMAL_LEAP);
    if (task) {
        animal = animal_of(task);
        animal->speed = collision_get_up_axis() == COLLISION_UP_Y ? LEAP_SPEED
                                                                  : -LEAP_SPEED;
        animal->timer = RELEASE_TIMER;
    }
    pos.x = x;
    pos.y = y;
    pos.z = z;
    sound_play_at(ANIMAL_RELEASE_SOUND, NULL, 0, ANIMAL_RELEASE_VOLUME, &pos);
    return task;
}

struct task *animal_release_uncounted(Float x, Float y, Float z, int kind)
{
    struct task *task;

    animal_pick_slot(kind);
    task = animal_release(x, y, z, kind);
    if (task) {
        task->work->flags |= ANIMAL_UNCOUNTED;
    }
    return task;
}

static struct task *animal_bubble_create(Float x, Float y, Float z)
{
    struct task *task;
    NJS_POINT3 pos;
    int slot;

    /* @hack spelling: slot, a local for the random slot, where the pick in
     * the call would do */
    slot = animal_random_slot();
    task = animal_create(x, y, z, slot, ANIMAL_BUBBLE);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    sound_play_at(ANIMAL_RELEASE_SOUND, NULL, 0, ANIMAL_RELEASE_VOLUME, &pos);
    return task;
}

struct task *animal_bubble_release(Float x, Float y, Float z)
{
    struct task *task;

    task = animal_bubble_create(x, y, z);
    if (task) {
        task->work->flags |= ANIMAL_UNCOUNTED;
    }
    return task;
}

/* no header declares it, and nothing calls it. as a static, SHC would leave
 * it out. it releases an animal as animal_release does, but at rest and
 * with no timer. the animal runs from its second step, and a player can take
 * it at once */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
struct task *animal_release_in_place(Float x, Float y, Float z, int kind)
{
    struct task *task;
    NJS_POINT3 pos;
    int slot;

    /* @hack spelling: slot, a local for the slot the kind picks, where the
     * pick in the call would do */
    slot = animal_pick_slot(kind);
    task = animal_create(x, y, z, slot, ANIMAL_LEAP);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    sound_play_at(ANIMAL_RELEASE_SOUND, NULL, 0, ANIMAL_RELEASE_VOLUME, &pos);
    return task;
}

/* the fields of the work's ang.x that animal_init reads. SET_PICK picks one
 * of the two slots of SET_RELEASE's release kind. PICK_FIRST picks the
 * first, and PICK_SECOND the second. any other value picks either at
 * random */
#define SET_MODE(ang) (((ang) >> 8) & 15)
#define SET_RELEASE(ang) (((ang) >> 4) & 15)
#define SET_PICK(ang) ((ang) & 15)
#define PICK_FIRST 0
#define PICK_SECOND 1

void animal_init(struct task *task)
{
    struct task_work *work = task->work;
    struct animal *animal;
    int mode_bits;
    int release_bits;
    int pick_bits;
    int mode;
    int slot;

    animal = animal_alloc();
    if (!animal) {
        task_destroy(task);
    } else {
        task->step = animal_step;
        task->draw = animal_draw;
        task->destructor = animal_destroy;
        task->extra.p = animal;
        mode_bits = SET_MODE(work->ang.x);
        release_bits = SET_RELEASE(work->ang.x);
        pick_bits = SET_PICK(work->ang.x);
        /* @hack spelling: the default ahead of ANIMAL_CIRCLE's case, where it
         * would follow the others */
        switch (mode_bits) {
        default:
        case ANIMAL_CIRCLE:
            mode = ANIMAL_CIRCLE;
            break;
        case ANIMAL_STAND:
            mode = ANIMAL_STAND;
            break;
        case ANIMAL_HIDDEN:
            mode = ANIMAL_HIDDEN;
            break;
        }
        switch (release_bits) {
        case ANIMAL_FIFTH:
            slot = FIFTH_SLOT;
            break;
        case ANIMAL_THIRD_OR_FOURTH:
            switch (pick_bits) {
            case PICK_FIRST:
                slot = THIRD_SLOT;
                break;
            case PICK_SECOND:
                slot = FOURTH_SLOT;
                break;
            default:
                slot = animal_pick_slot(ANIMAL_THIRD_OR_FOURTH);
                break;
            }
            break;
        case ANIMAL_FIRST_OR_SECOND:
            switch (pick_bits) {
            case PICK_FIRST:
                slot = FIRST_SLOT;
                break;
            case PICK_SECOND:
                slot = SECOND_SLOT;
                break;
            default:
                slot = animal_pick_slot(ANIMAL_FIRST_OR_SECOND);
                break;
            }
            break;
        default:
            slot = animal_random_slot();
            break;
        }
        animal->mode = mode;
        animal->slot = slot;
        animal->kind = stage_animal(slot);
        animal->angle = work->ang.y;
        animal->speed = 0.0F;
        animal->timer = 0;
        animal->frame = 0.0F;
        animal->up_axis = collision_get_up_axis();
        work->timer = 0;
    }
}
