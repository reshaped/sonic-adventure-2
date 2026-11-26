/* the SG item box, SG_ITEMBOX in a stage's object list. only the players
 * whose bits are set in game_state.sg_players see it and take it */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/sg_item_box.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a box's modes, in its work's mode */
enum sg_item_box_mode {
    MODE_INIT,
    MODE_GROW,
    MODE_WAIT,
    MODE_GONE, /* it kills itself */
    MODE_RESET /* no code here sets it */
};

/* the placement editor's labels for the work's ang.y and scl.x */
static const char edit_yaw[12] = "<- ANGLE Y";
static const char edit_item[] = "<- ITEM";

/* the index of the box's item in sg_item_box_items. the SHC build keeps it
 * in task->force. task_destroy frees force as a record, so
 * sg_item_box_destroy sets the index back to 0 first */
#ifdef NONMATCHING
/* this side keeps the index in unk_3c, a Sint32 of the task's own. the matching
 * side stores it through force's address */
#define ITEM_INDEX(task) ((task)->unk_3c.l)
#else
#define ITEM_INDEX(task) ((task)->force.l)
#endif

/* the box's scale in all three axes, from 0 to 1, kept in its work's scl.z */
#define SCALE(work) ((work)->scl.z)
#define GROW_STEP 0.11F
/* the icon above the box turns about y by ICON_TURN degrees a step. the box
 * keeps the icon's angle, in degrees, in its work's scl.y */
#define ICON_ANGLE(work) ((work)->scl.y)
#define ICON_TURN 5.0F
#define ICON_HEIGHT 7.5F
#define ICON_SCALE 6.0F

#define TAKE_REACH 15.0F
#define CENTER_HEIGHT 5.0F
#define TAKE_SOUND 0x800d
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 48
#define PUFF_RISE 0.1F
#define PUFF_SCALE 6.0F

static void sg_item_box_init(struct task *task);
static void sg_item_box_grow(struct task *task);
static void sg_item_box_wait(struct task *task);
static void sg_item_box_draw(struct task *task);
static void sg_item_box_destroy(struct task *task);
static void sg_item_box_give_10_rings(struct task *box, int player);
static void sg_item_box_give_1up(struct task *box, int player);
static void sg_item_box_give_5_rings(struct task *box, int player);
static void sg_item_box_give_speed_up(struct task *box, int player);
static void sg_item_box_give_20_rings(struct task *box, int player);
static void sg_item_box_give_barrier(struct task *box, int player);
static void sg_item_box_give_bomb(struct task *box, int player);
static void sg_item_box_skip_life(struct task *box, int player);
static void sg_item_box_give_magnetic_barrier(struct task *box, int player);
static void sg_item_box_give_nothing(struct task *box, int player);
static void sg_item_box_give_invincibility(struct task *box, int player);

static struct item_box_item sg_item_box_items[ITEM_BOX_ITEMS] = {
    {ITEM_SPEED_UP, sg_item_box_give_speed_up},
    {ITEM_5_RINGS, sg_item_box_give_5_rings},
    {ITEM_1UP, sg_item_box_give_1up},
    {ITEM_10_RINGS, sg_item_box_give_10_rings},
    {ITEM_20_RINGS, sg_item_box_give_20_rings},
    {ITEM_BARRIER, sg_item_box_give_barrier},
    {ITEM_BOMB, sg_item_box_give_bomb},
    {ITEM_LIFE, sg_item_box_skip_life},
    {ITEM_MAGNETIC_BARRIER, sg_item_box_give_magnetic_barrier},
    {ITEM_EMPTY, sg_item_box_give_nothing},
    {ITEM_INVINCIBLE, sg_item_box_give_invincibility},
};

void sg_item_box_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the tests in the order MODE_INIT, MODE_WAIT and
     * MODE_GROW, not in the modes' order */
    if (work->mode == MODE_INIT || work->mode == MODE_WAIT ||
        work->mode == MODE_GROW) {
        task->draw = sg_item_box_draw;
    }
    switch (work->mode) {
    case MODE_INIT:
        task->destructor = sg_item_box_destroy;
        /* @hack redundant: the draw set again, where the test above has set
         * it already */
        task->draw = sg_item_box_draw;
        sg_item_box_init(task);
        break;
    case MODE_GROW:
        sg_item_box_grow(task);
        break;
    case MODE_WAIT:
        sg_item_box_wait(task);
        break;
    case MODE_GONE:
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    case MODE_RESET:
        sg_item_box_destroy(task);
    /* fall through */
    default:
        work->mode = MODE_INIT;
        break;
    }
    /* @hack redundant: a read of task->work after the switch, which nothing
     * uses */
    (void)task->work;
}

static void sg_item_box_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    work->mode = MODE_GROW;
    SCALE(work) = 0.0F;
    ICON_ANGLE(work) = 0.0F;
#ifdef NONMATCHING
    /* this side clamps the item's index from both ends. a negative scale, or
     * one past the int's range, gives the matching side an index outside the
     * items */
    if (work->scl.x >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(task) = ITEM_BOX_ITEMS - 1;
    } else if (work->scl.x > 0.0F) {
        ITEM_INDEX(task) = (Sint32)work->scl.x;
    } else {
        ITEM_INDEX(task) = 0;
    }
#else
    ITEM_INDEX(task) = (Sint32)work->scl.x;
    if (ITEM_INDEX(task) >= ITEM_BOX_ITEMS) {
        ITEM_INDEX(task) = ITEM_BOX_ITEMS - 1;
    }
#endif
    if (task->placement != NULL && !placement_used(task) &&
        placement_unk_20(task)) {
        if (sg_item_box_items[ITEM_INDEX(task)].kind == ITEM_1UP) {
            for (i = 0; i < ITEM_BOX_ITEMS; i++) {
                if (sg_item_box_items[i].kind == ITEM_EMPTY) {
                    ITEM_INDEX(task) = i;
                    break;
                }
            }
        }
    }
    if (task->placement != NULL && placement_used(task)) {
        work->mode = MODE_GONE;
    }
}

static void sg_item_box_grow(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.time_stopped) {
        SCALE(work) = 1.0F;
        work->mode = MODE_WAIT;
    } else {
        SCALE(work) += GROW_STEP;
        if (SCALE(work) >= 1.0F) {
            /* @hack spelling: the mode set ahead of the scale, where the
             * scale first would do */
            work->mode = MODE_WAIT;
            SCALE(work) = 1.0F;
        }
        ICON_ANGLE(work) += ICON_TURN;
    }
}

static void sg_item_box_wait(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 center;
    /* @hack spelling: spd declared ahead of pos, not after it */
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    int player = -1;
    int i;

    center = work->pos;
    center.y += CENTER_HEIGHT;
    for (i = 0; i < GAME_PLAYERS && player_work[i] != NULL &&
                player_work[i]->collision != NULL;
         i++) {
#ifdef NONMATCHING
        /* this side skips a player whose collision record holds no shapes,
         * which the test below reads */
        if (!player_work[i]->collision->count ||
            player_work[i]->collision->shapes == NULL) {
            continue;
        }
#endif
        if ((game_state.sg_players & (1 << i)) &&
            njDistanceP2P(&player_work[i]->collision->shapes->center, &center) <
                TAKE_REACH) {
            player = i;
            break;
        }
    }
    if (player >= 0) {
        item_icon_row_add(player, sg_item_box_items[ITEM_INDEX(task)].kind);
        sg_item_box_items[ITEM_INDEX(task)].give(task, player);
        enemy_bounce_player(player);
        work->mode = MODE_GONE;
        sound_play(TAKE_SOUND, NULL, 0, 0);
        vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ, 0);
        pos = work->pos;
        pos.y += ICON_HEIGHT;
        spd.x = 0.0F;
        spd.y = PUFF_RISE;
        spd.z = 0.0F;
        white_breath_fade_create(&pos, &spd, PUFF_SCALE);
        if (task->placement != NULL) {
            placement_set_used(task);
        }
    }
    if (!game_state.time_stopped) {
        ICON_ANGLE(work) += ICON_TURN;
    }
}

static void sg_item_box_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    /* @hack redundant: the test's && 1 */
    if (!(game_state.sg_players & (1 << camera_current_index)) && 1) {
        return;
    }
    njSetTexture(&item_box_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njScale(NULL, SCALE(work), SCALE(work), SCALE(work));
    njCnkSimpleDrawModel(&item_box_lower_model);
    njCnkSimpleDrawModel(&item_box_middle_model);
    njCnkSimpleDrawModel(&item_box_upper_model);
    if (sg_item_box_items[ITEM_INDEX(task)].kind != ITEM_EMPTY) {
        pos.x = 0.0F;
        pos.y = ICON_HEIGHT;
        pos.z = 0.0F;
        item_icon_draw(sg_item_box_items[ITEM_INDEX(task)].kind, &pos,
                       (Angle)(ICON_ANGLE(work) * ANGLE_PER_DEGREE),
                       ICON_SCALE);
    }
    njPopMatrix(1);
}

static void sg_item_box_destroy(struct task *task)
{
    ITEM_INDEX(task) = 0;
}

static void sg_item_box_give_10_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_10_RINGS);
    ring_add(player, 10);
    player_add_gauge(player, 10 * ITEM_GAUGE_PER_RING);
}

static void sg_item_box_give_1up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    lives_add(player, 1);
    /* @bug memory: placement_set_unk_20 writes through null where the box
     * has no placement. sg_item_box_init and sg_item_box_wait test the
     * box's placement first (TST at 0xf0 and 0x302 of the object's code),
     * but no test guards this call (JMP at 0x4be) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a box without a placement sets
     * nothing */
    if (box->placement != NULL) {
        placement_set_unk_20(box);
    }
#else
    placement_set_unk_20(box);
#endif
}

static void sg_item_box_give_5_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_5_RINGS);
    ring_add(player, 5);
    player_add_gauge(player, 5 * ITEM_GAUGE_PER_RING);
}

static void sg_item_box_give_speed_up(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_speed_up(player);
}

static void sg_item_box_give_20_rings(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    ring_add(player, 20);
    player_add_gauge(player, 20 * ITEM_GAUGE_PER_RING);
}

static void sg_item_box_give_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_barrier(player);
}

static void sg_item_box_give_bomb(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    explosion_create(&box->work->pos);
}

static void sg_item_box_skip_life(struct task *box, int player)
{
}

static void sg_item_box_give_magnetic_barrier(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_magnetic_barrier(player);
}

static void sg_item_box_give_nothing(struct task *box, int player)
{
}

static void sg_item_box_give_invincibility(struct task *box, int player)
{
    score_add(ITEM_POINTS_OTHER);
    player_give_invincibility(player);
}
