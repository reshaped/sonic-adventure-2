/* the item balloon, ITEMBOXBALLOON in a stage's object list. the player who
 * breaks a balloon gets the item it holds. its generator, ITEMBOXBAL GEN in
 * an object list, makes balloons of random items, one at a time */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/item_balloon.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/object_builds.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"
#include "shared/item_balloon/text.inc"

/* a balloon's modes, in its work's mode. the step runs each of them, but
 * nothing in this source puts a balloon in MODE_STILL or MODE_RESET */
enum item_balloon_mode {
    MODE_INIT,
    MODE_GROW,
    MODE_WAIT,
    MODE_BREAK,
    MODE_POP, /* it grows and fades, then ends */
    MODE_STILL, /* the step only registers the balloon's collision record */
    MODE_RESET
};

/* the index of the balloon's item in item_balloon_items, kept in task->force
 * in place of a pointer. task_destroy frees a non-null force, so the
 * destructor clears force first */
#define ITEM_INDEX(task) ((task)->force.l)

/* the balloon's scale in all three axes, kept in its work's scl.z. the scale
 * grows by GROW_STEP a step from 0 to 1. a pop then grows it by POP_STEP a
 * step to POP_SCALE */
#define SCALE(work) ((work)->scl.z)
#define GROW_STEP 0.11F
#define POP_STEP 0.05F
#define POP_SCALE 3.0F
/* past a scale of 1, a balloon fades. its alpha falls in a line from
 * POP_ALPHA at a scale of 1 to 0 at POP_SCALE */
#define POP_ALPHA 0.7F
#define POP_FADE 0.5F
/* what each step of a growing or waiting balloon adds to its work's scl.y,
 * which nothing reads */
#define SCL_Y_STEP 5.0F

/* the balloon bobs, sways about x and wobbles in scale. each wave is a sine
 * or cosine of its timer. a _STEP is the Angle its wave turns a step. a _LEAD
 * is a head start, in steps */
#define BOB_HEIGHT 4.0F
#define BOB_STEP 256
#define BOB_LEAD 800
#define SWAY 3.5F /* the most it sways each way, in degrees */
#define SWAY_STEP 640
#define WOBBLE 0.01F
#define WOBBLE_STEP_X 2304
#define WOBBLE_STEP_Y 2048
#define WOBBLE_STEP_Z 2128
#define WOBBLE_LEAD_Z 4096

/* a player in the air that gets the item takes the speed BOUNCE_SPEED,
 * straight up. the break's puff of white breath rises at PUFF_RISE, at the
 * scale PUFF_SCALE */
#define BOUNCE_SPEED 2.0F
#define PUFF_RISE 0.3F
#define PUFF_SCALE 26.0F
#define BREAK_SOUND 0x800d
#define POP_SOUND 0x1006
/* the break's and the pop's sounds play at these */
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 48

#define GENERATOR_ITEMS 12
#define ITEM_BALLOON_SHAPES 1

/* a kind of item, and the call that gives it to the player in slot player */
struct item_balloon_item {
    Sint32 kind;
    void (*give)(struct task *balloon, int player);
};

static void item_balloon_init(struct task *task);
static void item_balloon_grow(struct task *task);
static void item_balloon_check_hit(struct task *task);
static void item_balloon_break(struct task *task);
static void item_balloon_pop(struct task *task);
static void item_balloon_draw(struct task *task);
static void item_balloon_destroy(struct task *task);
static void item_balloon_give_10_rings(struct task *balloon, int player);
static void item_balloon_give_1up(struct task *balloon, int player);
static void item_balloon_give_5_rings(struct task *balloon, int player);
static void item_balloon_give_speed_up(struct task *balloon, int player);
static void item_balloon_give_20_rings(struct task *balloon, int player);
static void item_balloon_give_barrier(struct task *balloon, int player);
static void item_balloon_give_bomb(struct task *balloon, int player);
static void item_balloon_give_life(struct task *balloon, int player);
static void item_balloon_give_magnetic_barrier(struct task *balloon,
                                               int player);
static void item_balloon_give_nothing(struct task *balloon, int player);
static void item_balloon_give_invincibility(struct task *balloon, int player);
static void item_balloon_generator_step(struct task *task);
static void item_balloon_generator_draw(struct task *task);

#define ITEM_BALLOON_TEXTURES 1
static NJS_TEXNAME item_balloon_texname[ITEM_BALLOON_TEXTURES] = {
#include "shared/item_balloon/item_balloon_texname.inc"
};
NJS_TEXLIST item_balloon_texlist = {
#include "shared/item_balloon/item_balloon_texlist.inc"
};

/* the balloon's model, a chunk model of ITEM_BALLOON_POINTS points, each
 * with its normal */
#define ITEM_BALLOON_POINTS 49
static Sint16 item_balloon_polygons[489] = {
#include "shared/item_balloon/item_balloon_polygons.inc"
};
static Sint32 item_balloon_vertices[297] = {
#include "shared/item_balloon/item_balloon_vertices.inc"
};
static NJS_CNK_MODEL item_balloon_model = {
#include "shared/item_balloon/item_balloon_model.inc"
};
NJS_CNK_OBJECT item_balloon_object = {
#include "shared/item_balloon/item_balloon_object.inc"
};

/* clang-format off */
static struct collision_shape item_balloon_shapes[ITEM_BALLOON_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_800000,
     {0.0F, -4.13F, 0.0F}, 38.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the texture lists of the balloon, the item icons and the break's puff,
 * ended by null */
static NJS_TEXLIST *item_balloon_texlists[4] = {
#include "shared/item_balloon/item_balloon_texlists.inc"
};
static struct item_balloon_item item_balloon_items[ITEM_BOX_ITEMS] = {
    {ITEM_SPEED_UP, item_balloon_give_speed_up},
    {ITEM_5_RINGS, item_balloon_give_5_rings},
    {ITEM_1UP, item_balloon_give_1up},
    {ITEM_10_RINGS, item_balloon_give_10_rings},
    {ITEM_20_RINGS, item_balloon_give_20_rings},
    {ITEM_BARRIER, item_balloon_give_barrier},
    {ITEM_BOMB, item_balloon_give_bomb},
    {ITEM_LIFE, item_balloon_give_life},
    {ITEM_MAGNETIC_BARRIER, item_balloon_give_magnetic_barrier},
    {ITEM_EMPTY, item_balloon_give_nothing},
    {ITEM_INVINCIBLE, item_balloon_give_invincibility},
};
/* the generator's items. it picks an entry at random, so an item's odds are
 * its entries in GENERATOR_ITEMS */
/* clang-format off */
static Sint8 item_balloon_generator_items[GENERATOR_ITEMS] = {
    BOX_SPEED_UP, BOX_5_RINGS,
    BOX_10_RINGS, BOX_10_RINGS, BOX_10_RINGS, BOX_10_RINGS,
    BOX_20_RINGS, BOX_20_RINGS, BOX_20_RINGS,
    BOX_BARRIER, BOX_BARRIER,
    BOX_5_RINGS,
};
/* clang-format on */

void item_balloon_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the tests in the order MODE_INIT, MODE_WAIT and
     * MODE_GROW, not in the modes' order */
    if (work->mode == MODE_INIT || work->mode == MODE_WAIT ||
        work->mode == MODE_GROW) {
        task->draw = item_balloon_draw;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = item_balloon_destroy;
        /* @hack redundant: the draw set again, where the test above has set
         * it already */
        task->draw = item_balloon_draw;
        item_balloon_init(task);
        break;
    case MODE_GROW:
        item_balloon_grow(task);
        break;
    case MODE_WAIT:
        item_balloon_check_hit(task);
        break;
    case MODE_BREAK:
        item_balloon_break(task);
        break;
    case MODE_POP:
        item_balloon_pop(task);
        break;
    case MODE_STILL:
        collision_register(task);
        break;
    /* MODE_RESET sends the balloon back to MODE_INIT, as does any mode
     * without a case. item_balloon_init then gives the balloon a new
     * collision record, and leaves the old one unfreed */
    case MODE_RESET: {
        /* @hack spelling: reset, a copy of task, where task would do */
        struct task *reset = task;

        ITEM_INDEX(reset) = 0;
        /* @hack redundant: a block no path reaches, kept for its two strings,
         * which SHC still places in this source's constants. the image holds
         * none of the block's code. without its call, SHC writes the code
         * around the block differently */
        /* @hack spelling: the test of 0.0F, where 0 would do */
        if (0.0F) {
            (void)"<- ANGLE Y";
            (void)"<- ITEM";
            item_balloon_grow(task);
            /* @hack redundant: an empty else */
        } else {
        }
        work->mode = MODE_INIT;
        break;
    }
    default:
        work->mode = MODE_INIT;
        break;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
}

static void item_balloon_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    work->mode = MODE_GROW;
    SCALE(work) = 0.0F;
    work->scl.y = 0.0F;
    ITEM_INDEX(task) = (Sint32)work->scl.x;
    if (ITEM_INDEX(task) >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(task) = ITEM_BOX_ITEMS - 1;
    } else if (ITEM_INDEX(task) < 0) {
        ITEM_INDEX(task) = 0;
    }
    if (task->placement != NULL && !placement_used(task) &&
        placement_unk_20(task)) {
        if (item_balloon_items[ITEM_INDEX(task)].kind == ITEM_1UP) {
            for (i = 0; i < ITEM_BOX_ITEMS; i++) {
                if (item_balloon_items[i].kind == ITEM_EMPTY) {
                    ITEM_INDEX(task) = i;
                    break;
                }
            }
        }
    }
    collision_init(task, item_balloon_shapes, ITEM_BALLOON_SHAPES,
                   COLLISION_ITEM);
    if (task->placement != NULL && placement_used(task)) {
        work->mode = MODE_BREAK;
    }
    work->timer = 0;
}

static void item_balloon_grow(struct task *task)
{
    struct task_work *work = task->work;

    SCALE(work) += GROW_STEP;
    if (SCALE(work) >= 1.0F) {
        work->mode = MODE_WAIT;
        SCALE(work) = 1.0F;
    }
    work->scl.y += SCL_Y_STEP;
}

static void item_balloon_check_hit(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision = work->collision;
    int player;
    NJS_VECTOR spd;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((collision->flags & COLLISION_TOUCHED) &&
        (collision->hit->kind == COLLISION_PLAYER ||
         collision->hit->kind == COLLISION_ATTACK) &&
        (player = player_slot_of(collision->hit->owner)) != -1) {
        if (!(player_records[player]->flags & PLAYER_MISSED)) {
            item_icon_row_add(player,
                              item_balloon_items[ITEM_INDEX(task)].kind);
            item_balloon_items[ITEM_INDEX(task)].give(task, player);
            if (!(player_work[player]->flags & PLAYER_WORK_ON_GROUND)) {
                player_set_speed(player, 0.0F, BOUNCE_SPEED, 0.0F);
            }
        }
        work->mode = MODE_BREAK;
        spd.x = 0.0F;
        spd.y = PUFF_RISE;
        spd.z = 0.0F;
        white_breath_fade_create(&work->pos, &spd, PUFF_SCALE);
        sound_play(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
        vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                        VIBRATION_UNTIMED);
        if (task->placement != NULL) {
            placement_set_used(task);
        }
    }
    collision_register(task);
    work->scl.y += SCL_Y_STEP;
}

static void item_balloon_break(struct task *task)
{
    struct task_work *work = task->work;

    work->mode = MODE_POP;
    sound_play(POP_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
}

static void item_balloon_pop(struct task *task)
{
    struct task_work *work = task->work;

    SCALE(work) += POP_STEP;
    if (SCALE(work) > POP_SCALE) {
        SCALE(work) = POP_SCALE;
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    }
}

static void item_balloon_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float alpha;

    njSetTexture(&item_balloon_texlist);
    if (work->mode == MODE_POP || objects_fog_off) {
        njFogDisable();
    }
    if (SCALE(work) > 1.0F) {
        alpha = 1.0F - ((SCALE(work) - 1.0F) * POP_FADE);
        if (alpha > 1.0F) {
            alpha = 1.0F;
        }
        if (alpha < 0.0F) {
            alpha = 0.0F;
        }
        alpha *= POP_ALPHA;
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        constant_material_set(alpha, 1.0F, 1.0F, 1.0F);
    }
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x,
                work->pos.y +
                    (njSin((work->timer + BOB_LEAD) * BOB_STEP) * BOB_HEIGHT),
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, (Angle)(njSin(work->timer * SWAY_STEP) *
                            (SWAY * ANGLE_PER_DEGREE)));
    njScale(
        NULL, SCALE(work) + (njCos(work->timer * WOBBLE_STEP_X) * WOBBLE),
        SCALE(work) + (njSin(work->timer * WOBBLE_STEP_Y) * WOBBLE),
        SCALE(work) +
            (njCos((work->timer + WOBBLE_LEAD_Z) * WOBBLE_STEP_Z) * WOBBLE));
    njCnkSimpleDrawModel(&item_balloon_model);
    njPopMatrixEx();
    if (SCALE(work) > 1.0F) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    if (work->mode == MODE_POP || objects_fog_off) {
        njFogEnable();
    }
}

static void item_balloon_destroy(struct task *task)
{
    ITEM_INDEX(task) = 0;
}

static void item_balloon_give_10_rings(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_10_RINGS);
    ring_add(player, 10);
    player_add_gauge(player, 10 * ITEM_GAUGE_PER_RING);
}

static void item_balloon_give_1up(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    lives_add(player, 1);
    /* @bug memory: placement_set_unk_20 writes through null where the
     * balloon has no placement (the JMP at 0x63e of the object's code).
     * item_balloon_init, item_balloon_check_hit and item_balloon_pop test the
     * placement first (the TSTs at 0x11e, 0x2b2 and 0x314 of the object's
     * code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a balloon without a placement marks
     * none */
    if (balloon->placement == NULL) {
        return;
    }
#endif
    placement_set_unk_20(balloon);
}

static void item_balloon_give_5_rings(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_5_RINGS);
    ring_add(player, 5);
    player_add_gauge(player, 5 * ITEM_GAUGE_PER_RING);
}

static void item_balloon_give_speed_up(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_speed_up(player);
}

static void item_balloon_give_20_rings(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    ring_add(player, 20);
    player_add_gauge(player, 20 * ITEM_GAUGE_PER_RING);
}

static void item_balloon_give_barrier(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_barrier(player);
}

static void item_balloon_give_bomb(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    explosion_create(&balloon->work->pos);
}

static void item_balloon_give_life(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_add_gauge(player, ITEM_GAUGE_FULL);
}

static void item_balloon_give_magnetic_barrier(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_magnetic_barrier(player);
}

static void item_balloon_give_nothing(struct task *balloon, int player)
{
}

static void item_balloon_give_invincibility(struct task *balloon, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_invincibility(player);
}

static void item_balloon_generator_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *balloon;
    struct task_work *balloon_work;

    if (placement_try_leave(task)) {
        return;
    }
    if (task->child == NULL) {
        if (work->timer++ > work->ang.x) {
            balloon = task_create_child(TASK_WORK, item_balloon_step, task);
            if (balloon != NULL) {
                balloon_work = balloon->work;
                /* @hack redundant: the copy of the generator's ang.y, which
                 * task_create_child has made already */
                balloon_work->ang.y = work->ang.y;
                balloon_work->scl.x = item_balloon_generator_items[(
                    int)(rand() * (GENERATOR_ITEMS * RAND_SCALE))];
            }
            work->timer = 0;
        }
    } else {
        work->timer = 0;
    }
}

static void item_balloon_generator_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR vectors[2];

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. its locals and calls give
     * the function the image's stack frame, though the image holds none of
     * their code */
    if (0) {
        (void)"<- INTERVAL";
        (void)"SEC";
        (void)"<- ANG Y";
        (void)"<- START COUNT";
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        vectors[0].x = vectors[0].y = vectors[0].z = work->scl.x;
        njScaleEx(&vectors[0]);
        vectors[1].x = vectors[1].z = 0.0F;
        vectors[1].y = 10.0F;
        njTranslateEx(&vectors[1]);
        njScaleEx(&vectors[0]);
        njPopMatrixEx();
    }
}

void item_balloon_generator_init(struct task *task)
{
    struct task_work *work = task->work;

    work->timer += work->ang.z;
    task->step = item_balloon_generator_step;
    task->draw = item_balloon_generator_draw;
}
