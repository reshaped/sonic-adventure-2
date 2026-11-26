/* the item boxes, ITEMBOX and ITEMBOX2_B in a stage's object list. a player
 * breaks a box for the item inside. ITEM_BOMB's explosions are here too */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/object_builds.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a box's modes, in its work's mode. no code of this source sets MODE_STILL
 * or MODE_RESET */
enum item_box_mode {
    MODE_INIT,
    MODE_GROW,
    MODE_WAIT,
    MODE_BREAK,
    MODE_BROKEN,
    MODE_STILL, /* the step only registers the box's collision record */
    MODE_RESET
};

/* the index of the box's item in item_box_items, kept in task->force as a
 * word. task_destroy would free force as a record, so item_box_destroy
 * clears it */
#define ITEM_INDEX(task) ((task)->force.l)

/* the box's scale in all three axes, kept in its work's scl.z. it grows from
 * 0 by GROW_STEP a step, to 1 */
#define SCALE(work) ((work)->scl.z)
#define GROW_STEP 0.11F
/* the icon's angle about y, in degrees, kept in its work's scl.y. it turns
 * ICON_TURN degrees a step */
#define ICON_ANGLE(work) ((work)->scl.y)
#define ICON_TURN 5.0F
#define ICON_HEIGHT 7.5F
#define ICON_SCALE 6.0F

#define BREAK_SOUND 0x800d
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
/* the rumble of a break, as vibration_start takes it */
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 48
#define PUFF_RISE 0.1F
#define PUFF_SCALE 6.0F

/* a box's parts in item_box_builds. ITEMBOX2_B's lie PARTS past ITEMBOX's */
#define PART_LOWER 0
#define PART_MIDDLE 1
#define PART_UPPER 2
#define PARTS 3
#define ITEM_BOX_BUILDS (2 * PARTS)
#define ITEM_BOX_SHAPES 2

#define EXPLOSION_LEVEL 3
#define EXPLOSION_GROWTH 10.0F
#define EXPLOSION_LIMIT 500.0F
#define EXPLOSION_TURN 288 /* in Angle units a step */

/* an explosion, a sphere about pos. the explosions form a list through next,
 * newest first. explosion_task's force heads it */
struct explosion {
    NJS_POINT3 pos;
    Float radius;
    Float limit; /* the radius past which the explosion ends */
    struct explosion *next;
};

static void item_box_init(struct task *task);
static void item_box_b_init(struct task *task);
static void item_box_grow(struct task *task);
static void item_box_wait(struct task *task);
static void item_box_break(struct task *task);
static void item_box_draw(struct task *task);
static void item_box_draw_built(struct task *task);
static void item_box_b_draw(struct task *task);
static void item_box_b_draw_built(struct task *task);
static void item_box_broken_draw(struct task *task);
static void item_box_broken_draw_built(struct task *task);
static void item_box_b_broken_draw(struct task *task);
static void item_box_b_broken_draw_built(struct task *task);
static void item_box_destroy(struct task *task);
static void item_box_give_10_rings(struct task *box, int player);
static void item_box_give_1up(struct task *box, int player);
static void item_box_give_5_rings(struct task *box, int player);
static void item_box_give_speed_up(struct task *box, int player);
static void item_box_give_20_rings(struct task *box, int player);
static void item_box_give_barrier(struct task *box, int player);
static void item_box_give_bomb(struct task *box, int player);
static void item_box_give_life(struct task *box, int player);
static void item_box_give_magnetic_barrier(struct task *box, int player);
static void item_box_give_nothing(struct task *box, int player);
static void item_box_give_invincibility(struct task *box, int player);
static void explosion_step(struct task *task);
static void explosion_draw(struct task *task);
static void explosion_destroy(struct task *task);
static int explosion_contains(NJS_POINT3 *pos);

/* clang-format off */
static struct collision_shape item_box_shapes[ITEM_BOX_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_800000 | SHAPE_HURTABLE | SHAPE_TURN_YZX,
     {0.0F, 5.0F, 0.0F}, 5.0F, 5.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL, SHAPE_UNK_800000 | SHAPE_TURN_YZX, {0.0F, 10.0F, 0.0F},
     5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape item_box_broken_shapes[ITEM_BOX_SHAPES] = {
    {10, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, SHAPE_UNK_800000 | SHAPE_UNTURNED | SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F},
     4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {11, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_ENEMIES |
         SHAPE_IGNORE_PLAYERS | SHAPE_OFF,
     {0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
NJS_TEXLIST *item_box_texlists[] = {
    &item_box_texlist,     &item_box_b_texlist, &item_icon_texlist,
    &white_breath_texlist, &bomb_hat_texlist,   NULL,
};
static Bool item_box_built = FALSE;
static struct model_build item_box_builds[ITEM_BOX_BUILDS] = {
    {MODEL_BUILD_ALONE, &item_box_texlist, &item_box_lower_object, NULL},
    {MODEL_BUILD_ALONE, &item_box_texlist, &item_box_middle_object, NULL},
    {MODEL_BUILD_ALONE, &item_box_texlist, &item_box_upper_object, NULL},
    {MODEL_BUILD_ALONE, &item_box_b_texlist, &item_box_b_lower_object, NULL},
    {MODEL_BUILD_ALONE, &item_box_b_texlist, &item_box_b_middle_object, NULL},
    {MODEL_BUILD_ALONE, &item_box_b_texlist, &item_box_b_upper_object, NULL},
};
static struct item_box_item item_box_items[ITEM_BOX_ITEMS] = {
    {ITEM_SPEED_UP, item_box_give_speed_up},
    {ITEM_5_RINGS, item_box_give_5_rings},
    {ITEM_1UP, item_box_give_1up},
    {ITEM_10_RINGS, item_box_give_10_rings},
    {ITEM_20_RINGS, item_box_give_20_rings},
    {ITEM_BARRIER, item_box_give_barrier},
    {ITEM_BOMB, item_box_give_bomb},
    {ITEM_LIFE, item_box_give_life},
    {ITEM_MAGNETIC_BARRIER, item_box_give_magnetic_barrier},
    {ITEM_EMPTY, item_box_give_nothing},
    {ITEM_INVINCIBLE, item_box_give_invincibility},
};
/* the explosions' task, or null while there is none */
static struct task *explosion_task = NULL;

Uint8 *item_box_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, item_box_builds, ITEM_BOX_BUILDS) ==
        ITEM_BOX_BUILDS) {
        item_box_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void item_box_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its three strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- ITEM";
    }
    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the tests in the order MODE_INIT, MODE_WAIT and
     * MODE_GROW, not in the modes' order */
    if (work->mode == MODE_INIT || work->mode == MODE_WAIT ||
        work->mode == MODE_GROW) {
        if (item_box_built) {
            task->draw = item_box_draw_built;
        } else {
            task->draw = item_box_draw;
        }
    } else if (work->mode == MODE_BREAK || work->mode == MODE_BROKEN) {
        if (item_box_built) {
            task->draw = item_box_broken_draw_built;
        } else {
            task->draw = item_box_broken_draw;
        }
    }
    /* @hack spelling: a jump to the one return after each case, where break
     * would do */
    switch (work->mode) {
    case MODE_INIT:
        item_box_init(task);
        goto done;
    case MODE_GROW:
        item_box_grow(task);
        goto done;
    case MODE_WAIT:
        item_box_wait(task);
        goto done;
    case MODE_BREAK:
        item_box_break(task);
        goto done;
    case MODE_BROKEN:
    case MODE_STILL:
        collision_register(task);
        goto done;
    /* MODE_RESET sends the box back to MODE_INIT, as does any mode without a
     * case. item_box_init then gives the box a new collision record and
     * enemy_work, and leaves the old ones unfreed. MODE_RESET's enemy_destroy
     * frees neither */
    case MODE_RESET: {
        /* @hack redundant: reset, a copy of task, and two clears of
         * ITEM_INDEX(reset), where one clear of ITEM_INDEX(task) would do */
        struct task *reset = task;

        enemy_destroy(task);
        ITEM_INDEX(reset) = 0;
        ITEM_INDEX(reset) = 0;
    }
    /* fall through */
    default:
        work->mode = MODE_INIT;
        break;
    }
done:
    return;
}

void item_box_b_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the tests in the order MODE_INIT, MODE_WAIT and
     * MODE_GROW, not in the modes' order */
    if (work->mode == MODE_INIT || work->mode == MODE_WAIT ||
        work->mode == MODE_GROW) {
        if (item_box_built) {
            task->draw = item_box_b_draw_built;
        } else {
            task->draw = item_box_b_draw;
        }
    } else if (work->mode == MODE_BREAK || work->mode == MODE_BROKEN) {
        if (item_box_built) {
            task->draw = item_box_b_broken_draw_built;
        } else {
            task->draw = item_box_b_broken_draw;
        }
    }
    /* @hack spelling: a jump to the one return after each case, where break
     * would do */
    switch (work->mode) {
    case MODE_INIT:
        item_box_b_init(task);
        goto done;
    case MODE_GROW:
        item_box_grow(task);
        goto done;
    case MODE_WAIT:
        item_box_wait(task);
        goto done;
    case MODE_BREAK:
        item_box_break(task);
        goto done;
    case MODE_BROKEN:
    case MODE_STILL:
        collision_register(task);
        goto done;
    /* MODE_RESET and any mode without a case send the box back to MODE_INIT, as
     * in item_box_step */
    case MODE_RESET: {
        /* @hack redundant: reset, a copy of task, and two clears of
         * ITEM_INDEX(reset), where one clear of ITEM_INDEX(task) would do */
        struct task *reset = task;

        enemy_destroy(task);
        ITEM_INDEX(reset) = 0;
        ITEM_INDEX(reset) = 0;
    }
    /* fall through */
    default:
        work->mode = MODE_INIT;
        break;
    }
done:
    return;
}

static void item_box_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    work->mode = MODE_GROW;
    SCALE(work) = 0.0F;
    ICON_ANGLE(work) = 0.0F;
    ITEM_INDEX(task) = (Sint32)work->scl.x;
    if (ITEM_INDEX(task) >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(task) = ITEM_BOX_ITEMS - 1;
    } else if (ITEM_INDEX(task) < 0) {
        ITEM_INDEX(task) = 0;
    }
    if ((task->placement != NULL && !placement_used(task) &&
         placement_unk_20(task)) ||
        (work->submode && task->parent->placement != NULL &&
         !placement_used(task->parent) && placement_unk_20(task->parent))) {
        if (item_box_items[ITEM_INDEX(task)].kind == ITEM_1UP) {
            for (i = 0; i < ITEM_BOX_ITEMS; i++) {
                if (item_box_items[i].kind == ITEM_EMPTY) {
                    ITEM_INDEX(task) = i;
                    break;
                }
            }
        }
    }
    if ((task->placement != NULL && placement_used(task)) ||
        (work->submode && task->parent->placement != NULL &&
         placement_used(task->parent))) {
        collision_init_in_place(task, item_box_broken_shapes, ITEM_BOX_SHAPES,
                                COLLISION_OBJECT);
        work->mode = MODE_BROKEN;
    } else {
        collision_init_in_place(task, item_box_shapes, ITEM_BOX_SHAPES,
                                COLLISION_ITEM);
    }
    enemy_work_alloc(task);
    task->destructor = item_box_destroy;
    if (item_box_built) {
        task->draw = item_box_draw_built;
    } else {
        task->draw = item_box_draw;
    }
}

static void item_box_b_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    work->mode = MODE_GROW;
    SCALE(work) = 0.0F;
    ICON_ANGLE(work) = 0.0F;
    ITEM_INDEX(task) = (Sint32)work->scl.x;
    if (ITEM_INDEX(task) >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(task) = ITEM_BOX_ITEMS - 1;
    } else if (ITEM_INDEX(task) < 0) {
        ITEM_INDEX(task) = 0;
    }
    if ((task->placement != NULL && !placement_used(task) &&
         placement_unk_20(task)) ||
        (work->submode && task->parent->placement != NULL &&
         !placement_used(task->parent) && placement_unk_20(task->parent))) {
        if (item_box_items[ITEM_INDEX(task)].kind == ITEM_1UP) {
            for (i = 0; i < ITEM_BOX_ITEMS; i++) {
                if (item_box_items[i].kind == ITEM_EMPTY) {
                    ITEM_INDEX(task) = i;
                    break;
                }
            }
        }
    }
    if ((task->placement != NULL && placement_used(task)) ||
        (work->submode && task->parent->placement != NULL &&
         placement_used(task->parent))) {
        collision_init_in_place(task, item_box_broken_shapes, ITEM_BOX_SHAPES,
                                COLLISION_OBJECT);
        work->mode = MODE_BROKEN;
    } else {
        collision_init_in_place(task, item_box_shapes, ITEM_BOX_SHAPES,
                                COLLISION_ITEM);
    }
    enemy_work_alloc(task);
    task->destructor = item_box_destroy;
    if (item_box_built) {
        task->draw = item_box_b_draw_built;
    } else {
        task->draw = item_box_b_draw;
    }
}

static void item_box_grow(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.time_stopped) {
        SCALE(work) = 1.0F;
        work->mode = MODE_WAIT;
    } else {
        SCALE(work) += GROW_STEP;
        if (SCALE(work) >= 1.0F) {
            /* @hack spelling: mode set ahead of the scale, not after it as
             * in the time_stopped branch */
            work->mode = MODE_WAIT;
            SCALE(work) = 1.0F;
        }
        ICON_ANGLE(work) += ICON_TURN;
    }
}

static void item_box_wait(struct task *task)
{
    /* @hack spelling: hit declared ahead of work and enemy, not after them */
    struct task *hit = NULL;
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    int player;
    /* @hack spelling: spd declared ahead of pos, not after it */
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    /* @hack spelling: the assignments inside the test, where a statement of
     * its own for each would do */
    if (((work->flags & WORK_HURT) && enemy->shot_by != -1) ||
        (hit = collision_next_player(task)) != NULL ||
        (hit = collision_next_attack(task)) != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (hit == NULL || (player = player_slot_of(hit)) < 0) {
            player = enemy->shot_by;
        }
        if (player >= 0) {
            if (!(player_records[player]->flags & PLAYER_MISSED)) {
                item_icon_row_add(player,
                                  item_box_items[ITEM_INDEX(task)].kind);
                item_box_items[ITEM_INDEX(task)].give(task, player);
                enemy_bounce_player(player);
            }
            work->mode = MODE_BREAK;
            pos = work->pos;
            pos.y += ICON_HEIGHT;
            spd.x = 0.0F;
            spd.y = PUFF_RISE;
            spd.z = 0.0F;
            white_breath_fade_create(&pos, &spd, PUFF_SCALE);
            sound_play(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
            if (task->placement != NULL) {
                placement_set_used(task);
            } else if (work->submode && task->parent->placement != NULL) {
                placement_set_used(task->parent);
            }
        }
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        ICON_ANGLE(work) += ICON_TURN;
    }
}

static void item_box_break(struct task *task)
{
    struct task_work *work = task->work;

    collision_free(task);
    collision_init_in_place(task, item_box_broken_shapes, ITEM_BOX_SHAPES,
                            COLLISION_OBJECT);
    work->mode = MODE_BROKEN;
}

static void item_box_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    if (objects_fog_off) {
        njFogDisable();
    }
    njCnkSimpleDrawModel(&item_box_lower_model);
    njCnkSimpleDrawModel(&item_box_middle_model);
    njCnkSimpleDrawModel(&item_box_upper_model);
    if (item_box_items[ITEM_INDEX(task)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(item_box_items[ITEM_INDEX(task)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrixEx();
}

static void item_box_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    if (objects_fog_off) {
        njFogDisable();
    }
    njDirectDrawObject(item_box_builds[PART_LOWER].built);
    njDirectDrawObject(item_box_builds[PART_MIDDLE].built);
    njDirectDrawObject(item_box_builds[PART_UPPER].built);
    if (item_box_items[ITEM_INDEX(task)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(item_box_items[ITEM_INDEX(task)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrixEx();
}

static void item_box_b_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    if (objects_fog_off) {
        njFogDisable();
    }
    njCnkSimpleDrawModel(&item_box_b_lower_model);
    njCnkSimpleDrawModel(&item_box_b_middle_model);
    njCnkSimpleDrawModel(&item_box_b_upper_model);
    if (item_box_items[ITEM_INDEX(task)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(item_box_items[ITEM_INDEX(task)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrixEx();
}

static void item_box_b_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    if (objects_fog_off) {
        njFogDisable();
    }
    njDirectDrawObject(item_box_builds[PARTS + PART_LOWER].built);
    njDirectDrawObject(item_box_builds[PARTS + PART_MIDDLE].built);
    njDirectDrawObject(item_box_builds[PARTS + PART_UPPER].built);
    if (item_box_items[ITEM_INDEX(task)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(item_box_items[ITEM_INDEX(task)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrixEx();
}

static void item_box_broken_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCnkSimpleDrawModel(&item_box_lower_model);
    njPopMatrixEx();
}

static void item_box_broken_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njDirectDrawObject(item_box_builds[PART_LOWER].built);
    njPopMatrixEx();
}

static void item_box_b_broken_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCnkSimpleDrawModel(&item_box_b_lower_model);
    njPopMatrixEx();
}

static void item_box_b_broken_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&item_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njDirectDrawObject(item_box_builds[PARTS + PART_LOWER].built);
    njPopMatrixEx();
}

static void item_box_destroy(struct task *task)
{
    enemy_destroy(task);
    ITEM_INDEX(task) = 0;
}

static void item_box_give_10_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_10_RINGS);
    ring_add(player, 10);
    player_add_gauge(player, 10 * ITEM_GAUGE_PER_RING);
}

static void item_box_give_1up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    lives_add(player, 1);
    if (box->work->submode && box->parent->placement != NULL) {
        placement_set_unk_20(box->parent);
    } else if (box->placement != NULL) {
        placement_set_unk_20(box);
    }
}

static void item_box_give_5_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_5_RINGS);
    ring_add(player, 5);
    player_add_gauge(player, 5 * ITEM_GAUGE_PER_RING);
}

static void item_box_give_speed_up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_speed_up(player);
}

static void item_box_give_20_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    ring_add(player, 20);
    player_add_gauge(player, 20 * ITEM_GAUGE_PER_RING);
}

static void item_box_give_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_barrier(player);
}

static void item_box_give_bomb(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    explosion_create(&box->work->pos);
}

static void item_box_give_life(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_add_gauge(player, ITEM_GAUGE_FULL);
}

static void item_box_give_magnetic_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_magnetic_barrier(player);
}

static void item_box_give_nothing(struct task *box, int player)
{
}

static void item_box_give_invincibility(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_invincibility(player);
}

void item_give(int player, int kind)
{
    int i;

    if (player_records[player]->flags & PLAYER_MISSED) {
        return;
    }
    if (kind == ITEM_EMPTY) {
        return;
    }
    for (i = 0; i < ITEM_BOX_ITEMS; i++) {
        if (kind == item_box_items[i].kind) {
            /* @hack spelling: the entry's kind, where kind, equal to it,
             * would do */
            item_icon_row_add(player, item_box_items[i].kind);
            item_box_items[i].give(NULL, player);
            break;
        }
    }
}

struct explosion *explosion_create(NJS_POINT3 *pos)
{
    struct explosion *explosion = NULL;
    /* @hack spelling: task, a local for explosion_task, where explosion_task
     * read in place would do */
    struct task *task;

    if (explosion_task == NULL) {
        explosion_task = task_create(TASK_WORK, EXPLOSION_LEVEL, explosion_step,
                                     "ExpManExec");
        if (explosion_task != NULL) {
            explosion_task->draw = explosion_draw;
            explosion_task->destructor = explosion_destroy;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (explosion_task != NULL &&
        (explosion = syCalloc(1, sizeof(struct explosion))) != NULL) {
        if (enemy_hit_hook == NULL) {
            enemy_hit_hook = explosion_contains;
        }
        task = explosion_task;
        explosion->pos = *pos;
        explosion->radius = 0.0F;
        /* @hack spelling: next set ahead of limit, not in the members'
         * order */
        explosion->next = task->force.p;
        explosion->limit = EXPLOSION_LIMIT;
        task->force.p = explosion;
    }
    return explosion;
}

struct explosion *explosion_create_sized(NJS_POINT3 *pos, Float limit)
{
    struct explosion *explosion = NULL;
    /* @hack spelling: task, a local for explosion_task, where explosion_task
     * read in place would do */
    struct task *task;

    if (explosion_task == NULL) {
        explosion_task = task_create(TASK_WORK, EXPLOSION_LEVEL, explosion_step,
                                     "ExpManExec");
        if (explosion_task != NULL) {
            explosion_task->draw = explosion_draw;
            explosion_task->destructor = explosion_destroy;
        }
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (explosion_task != NULL &&
        (explosion = syCalloc(1, sizeof(struct explosion))) != NULL) {
        if (enemy_hit_hook == NULL) {
            enemy_hit_hook = explosion_contains;
        }
        task = explosion_task;
        explosion->pos = *pos;
        explosion->radius = 0.0F;
        /* @hack spelling: next set ahead of limit, not in the members'
         * order */
        explosion->next = task->force.p;
        explosion->limit = limit;
        task->force.p = explosion;
    }
    return explosion;
}

static void explosion_step(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;
    struct explosion *explosion = owner->force.p;
    struct explosion *previous = NULL;
    struct explosion *next;

    while (explosion != NULL) {
        explosion->radius += EXPLOSION_GROWTH;
        if (explosion->radius > explosion->limit) {
            /* @hack redundant: radius set to limit just before the free */
            explosion->radius = explosion->limit;
            next = explosion->next;
            if (owner->force.p == explosion) {
                owner->force.p = next;
            }
            if (previous != NULL) {
                previous->next = next;
            }
            syFree(explosion);
            explosion = next;
        } else {
            previous = explosion;
            explosion = explosion->next;
        }
    }
    if (owner->force.p == NULL) {
        task_destroy(owner);
    }
}

static void explosion_draw(struct task *task)
{
    struct explosion *explosion = task->force.p;
    /* @hack spelling: fade, a local for radius over limit, where the quotient
     * passed in place would do */
    Float fade;

    while (explosion != NULL) {
        fade = explosion->radius / explosion->limit;
        njPushMatrixEx();
        njTranslateEx(&explosion->pos);
        njRotateY(NULL, game_state.steps * EXPLOSION_TURN);
        bomb_hat_draw_ring(explosion->radius, fade);
        njPopMatrixEx();
        explosion = explosion->next;
    }
}

static void explosion_destroy(struct task *task)
{
    struct explosion *next;

    while (task->force.p != NULL) {
        next = ((struct explosion *)task->force.p)->next;
        syFree(task->force.p);
        task->force.p = next;
    }
    if (task == explosion_task) {
        explosion_task = NULL;
    }
    if (enemy_hit_hook == explosion_contains) {
        enemy_hit_hook = NULL;
    }
}

static int explosion_contains(NJS_POINT3 *pos)
{
    /* @hack spelling: task and owner, two locals for explosion_task, where
     * explosion_task read in place would do */
    struct task *task = explosion_task;
    struct task *owner = task;
    struct explosion *explosion;

    if (owner == NULL) {
        return FALSE;
    }
    explosion = owner->force.p;
    while (explosion != NULL) {
        if (explosion->radius > njDistanceP2P(&explosion->pos, pos)) {
            return TRUE;
        }
        explosion = explosion->next;
    }
    return FALSE;
}
