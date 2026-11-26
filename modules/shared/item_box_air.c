/* the air item box, ITEMBOXAIR in a stage's object list. a player breaks it
 * for the item inside, and the box then swells, fades out and ends */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/item_icon.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a box's modes, in its work's mode. no code of this source sets MODE_STILL
 * or MODE_RESET */
enum item_box_air_mode {
    MODE_INIT,
    MODE_GROW,
    MODE_WAIT,
    MODE_BREAK,
    MODE_POP,
    MODE_STILL, /* the step only registers the box's collision record */
    MODE_RESET
};

/* the index of the box's item in item_box_air_items, kept in its work's
 * tick */
#define ITEM_INDEX(work) ((work)->tick)

/* the box's scale in all three axes, kept in its work's scl.z. it grows from
 * 0 by GROW_STEP a step, to 1. a broken box swells by POP_STEP a step, to
 * POP_SCALE */
#define SCALE(work) ((work)->scl.z)
#define GROW_STEP 0.05F
#define POP_STEP 0.2F
#define POP_SCALE 5.0F
/* past a scale of 1, the box's alpha falls in a line from POP_ALPHA to 0 at
 * POP_SCALE */
#define POP_ALPHA 0.7F
#define POP_FADE (1.0F / (POP_SCALE - 1.0F))

/* the icon's angle about y, in degrees, kept in its work's scl.y. it turns
 * ICON_TURN degrees a step */
#define ICON_ANGLE(work) ((work)->scl.y)
#define ICON_TURN 5.0F
#define ICON_HEIGHT 14.5F
#define ICON_SCALE 9.0F

#define BREAK_SOUND 0x800d
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
#define PUFF_RISE 0.1F
#define PUFF_SCALE 6.0F
/* the rumble of a break, as vibration_start takes it */
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 48

#define ITEM_BOX_AIR_SHAPES 1

#include "shared/item_box_air/text.inc"
#include "shared/item_box_air_art.h"

static void item_box_air_init(struct task *task);
static void item_box_air_grow(struct task *task);
static void item_box_air_wait(struct task *task);
static void item_box_air_break(struct task *task);
static void item_box_air_pop(struct task *task);
static void item_box_air_draw(struct task *task);
static void item_box_air_destroy(struct task *task);
static void item_box_air_give_10_rings(struct task *box, int player);
static void item_box_air_give_1up(struct task *box, int player);
static void item_box_air_give_5_rings(struct task *box, int player);
static void item_box_air_give_speed_up(struct task *box, int player);
static void item_box_air_give_20_rings(struct task *box, int player);
static void item_box_air_give_barrier(struct task *box, int player);
static void item_box_air_give_bomb(struct task *box, int player);
static void item_box_air_give_life(struct task *box, int player);
static void item_box_air_give_magnetic_barrier(struct task *box, int player);
static void item_box_air_give_nothing(struct task *box, int player);
static void item_box_air_give_invincibility(struct task *box, int player);

/* item_box_air_init gives the record's copy of the shape its center, a world
 * position */
static struct collision_shape item_box_air_shapes[ITEM_BOX_AIR_SHAPES] = {
#include "shared/item_box_air/item_box_air_shapes.inc"
};
NJS_TEXLIST *item_box_air_texlists[4] = {
#include "shared/item_box_air/item_box_air_texlists.inc"
};
static struct item_box_item item_box_air_items[ITEM_BOX_ITEMS] = {
#include "shared/item_box_air/item_box_air_items.inc"
};
/* the sphere's center as an offset from the box's pos, before the box's
 * angles turn it */
static NJS_POINT3 item_box_air_sphere_center = {
#include "shared/item_box_air/item_box_air_sphere_center.inc"
};

/* @hack redundant: task declared register, where a plain parameter would do */
void item_box_air_step(register struct task *task)
{
    /* @hack redundant: work declared register, where a plain local would do */
    register struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its three strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)BOX_ANGLE_Y_LABEL;
        (void)BOX_ANGLE_Z_LABEL;
        (void)BOX_ITEM_LABEL;
    }
    if (!work->submode && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        item_box_air_init(task);
        break;
    case MODE_GROW:
        item_box_air_grow(task);
        break;
    case MODE_WAIT:
        item_box_air_wait(task);
        break;
    case MODE_BREAK:
        /* @hack spelling: a call of item_box_air_break, which SHC expands
         * here, where its store of MODE_POP would do */
        item_box_air_break(task);
        break;
    case MODE_POP:
        item_box_air_pop(task);
        break;
    case MODE_STILL:
        /* @hack redundant: a test of the work's submode whose body is
         * empty */
        if (work->submode) {
        }
        collision_register(task);
        break;
    /* MODE_RESET sends the box back to MODE_INIT, as does any mode without a
     * case. item_box_air_init then gives the box a new collision record and
     * enemy_work, and leaves the old ones unfreed. MODE_RESET's
     * item_box_air_destroy frees neither */
    case MODE_RESET:
        item_box_air_destroy(task);
        /* @hack spelling: MODE_RESET's own store of MODE_INIT, where falling
         * through to the default's would do */
        work->mode = MODE_INIT;
        break;
    default:
        work->mode = MODE_INIT;
        break;
    }
    /* @hack redundant: a test of the work's submode whose body is empty */
    if (work->submode) {
    }
}

static void item_box_air_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (task->placement != NULL && placement_used(task)) {
        placement_kill(task);
        return;
    }
    if (work->submode && task->parent->placement != NULL &&
        placement_used(task->parent)) {
        placement_kill(task->parent);
        return;
    }
    work->mode = MODE_GROW;
    SCALE(work) = 0.0F;
    ICON_ANGLE(work) = 0.0F;
    ITEM_INDEX(work) = (int)work->scl.x;
    /* @bug logic: the test for an index below 0 never holds, since tick is
     * an unsigned byte. a scl.x that cuts to -1 to -246 leaves 255 to 10 in
     * tick, and the test for an index of ITEM_BOX_ITEMS or more holds that
     * to 10. the box then holds ITEM_INVINCIBLE, the last item, not
     * ITEM_SPEED_UP, the first. the test below 0 is at 0x12e of the object's
     * code */
    if (ITEM_INDEX(work) >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(work) = ITEM_BOX_ITEMS - 1;
    } else if (ITEM_INDEX(work) < 0) {
        ITEM_INDEX(work) = 0;
    }
    /* @hack redundant: the tests for PLACEMENT_USED, which the returns above
     * have ruled out */
    if ((task->placement != NULL && !placement_used(task) &&
         placement_unk_20(task)) ||
        (work->submode && task->parent->placement != NULL &&
         !placement_used(task->parent) && placement_unk_20(task->parent))) {
        if (item_box_air_items[ITEM_INDEX(work)].kind == ITEM_1UP) {
            for (i = 0; i < ITEM_BOX_ITEMS; i++) {
                if (item_box_air_items[i].kind == ITEM_EMPTY) {
                    ITEM_INDEX(work) = i;
                    break;
                }
            }
        }
    }
    collision_init(task, item_box_air_shapes, ITEM_BOX_AIR_SHAPES,
                   COLLISION_ITEM);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCalcPoint(NULL, &item_box_air_sphere_center,
                &work->collision->shapes->center);
    njPopMatrixEx();
    collision_fit_radius(task);
    enemy_work_alloc(task);
    task->destructor = item_box_air_destroy;
    task->draw = item_box_air_draw;
}

static void item_box_air_grow(struct task *task)
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

static void item_box_air_wait(struct task *task)
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
                                  item_box_air_items[ITEM_INDEX(work)].kind);
                item_box_air_items[ITEM_INDEX(work)].give(task, player);
                enemy_bounce_player(player);
            }
            work->mode = MODE_BREAK;
            pos = work->pos;
            pos.y += ICON_HEIGHT;
            spd.x = 0.0F;
            spd.y = PUFF_RISE;
            spd.z = 0.0F;
            /* @bug logic: the puff starts at the box's pos. the local pos is
             * set ICON_HEIGHT above it, where item_box.c starts its puff, and
             * nothing reads it. the store is at 0x364 of the object's code,
             * and the call at 0x37c of the object's code */
            white_breath_fade_create(&work->pos, &spd, PUFF_SCALE);
            sound_play(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
            if (task->placement != NULL) {
                placement_set_used(task);
            }
            if (work->submode && task->parent->placement != NULL) {
                placement_set_used(task->parent);
            }
        }
    }
    /* @hack redundant: a test of the work's submode whose body is empty */
    if (work->submode) {
    }
    if (work->mode != MODE_BREAK) {
        collision_register(task);
    }
    if (!game_state.time_stopped) {
        ICON_ANGLE(work) += ICON_TURN;
    }
}

static void item_box_air_break(struct task *task)
{
    struct task_work *work = task->work;

    work->mode = MODE_POP;
}

static void item_box_air_pop(struct task *task)
{
    struct task_work *work = task->work;

    SCALE(work) += POP_STEP;
    if (SCALE(work) > POP_SCALE) {
        SCALE(work) = POP_SCALE;
        if (work->submode) {
            if (task->parent->placement != NULL) {
                placement_kill(task->parent);
            } else {
                task_kill(task->parent);
            }
        } else if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    }
}

static void item_box_air_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Float alpha;

    if (work->mode == MODE_POP) {
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
    njSetTexture(&item_box_air_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    njCnkSimpleDrawModel(&item_box_air_lower_model);
    njCnkSimpleDrawModel(&item_box_air_middle_model);
    njCnkSimpleDrawModel(&item_box_air_upper_model);
    if (item_box_air_items[ITEM_INDEX(work)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(item_box_air_items[ITEM_INDEX(work)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    njPopMatrixEx();
    if (SCALE(work) > 1.0F) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    if (work->mode == MODE_POP) {
        njFogEnable();
    }
}

static void item_box_air_destroy(struct task *task)
{
    enemy_destroy(task);
    /* @hack redundant: the clear of task->force, which stays null in a task
     * created without TASK_FORCE */
    task->force.p = NULL;
}

static void item_box_air_give_10_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_10_RINGS);
    ring_add(player, 10);
    player_add_gauge(player, 10 * ITEM_GAUGE_PER_RING);
}

static void item_box_air_give_1up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    lives_add(player, 1);
    /* @bug memory: sets PLACEMENT_UNK_20 through null for a box without a
     * placement, or for a child box whose parent has none. the give calls
     * placement_set_unk_20 untested, by the jump at 0x70e of the object's
     * code. item_box_air_pop tests both placements for null, at 0x416 and
     * 0x4a2 of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: a box whose placement, or whose
     * parent's for a child box, is null marks none */
    if ((box->work->submode ? box->parent : box)->placement == NULL) {
        return;
    }
#endif
    if (box->work->submode) {
        placement_set_unk_20(box->parent);
    } else {
        placement_set_unk_20(box);
    }
}

static void item_box_air_give_5_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_5_RINGS);
    ring_add(player, 5);
    player_add_gauge(player, 5 * ITEM_GAUGE_PER_RING);
}

static void item_box_air_give_speed_up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_speed_up(player);
}

static void item_box_air_give_20_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    ring_add(player, 20);
    player_add_gauge(player, 20 * ITEM_GAUGE_PER_RING);
}

static void item_box_air_give_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_barrier(player);
}

static void item_box_air_give_bomb(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    explosion_create(&box->work->pos);
}

static void item_box_air_give_life(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_add_gauge(player, ITEM_GAUGE_FULL);
}

static void item_box_air_give_magnetic_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_magnetic_barrier(player);
}

static void item_box_air_give_nothing(struct task *box, int player)
{
}

static void item_box_air_give_invincibility(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_invincibility(player);
}
