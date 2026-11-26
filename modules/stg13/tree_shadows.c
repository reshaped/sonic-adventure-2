/* the trees' shadows, TREESHADOWS in City Escape's object list. a player
 * inside one of its six cylinders takes the shadows' light */
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "stg13/tree_shadows.h"
#include "system/game_state.h"
#include "system/task.h"

#define TREE_SHADOWS_SHAPES 6
/* the cylinders touch players and attacks alone. the checks pass them over
 * for every other kind */
#define SHADOW_FLAGS (SHAPE_UNK_2000 | SHAPE_IGNORE_ALL_BUT_PLAYERS)
/* the low byte of ang.y, which tree_shadows_init clears */
#define SETTING_BYTE 0xff

/* where tree_shadows_init keeps the shadows' light in the work */
#define LIGHT(work) ((work)->tick)

/* the shadows' record, in task->extra */
struct tree_shadows_state {
    Sint8 inside[GAME_PLAYERS]; /* TRUE from a player's entering to leaving */
    Sint8 light_before[GAME_PLAYERS]; /* each player's light before entering */
};

static void tree_shadows_step(struct task *task);
static void tree_shadows_draw(struct task *task);
static void tree_shadows_destroy(struct task *task);

/* clang-format off */
static struct collision_shape tree_shadows_shapes[TREE_SHADOWS_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {75.0F, -150.0F, 250.0F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {-75.0F, -150.0F, 250.0F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {-75.0F, 20.0F, 7.5F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {75.0F, 20.0F, 7.5F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {-75.0F, 180.0F, -236.0F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHADOW_FLAGS,
     {75.0F, 180.0F, -236.0F}, 30.0F, 50.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

void tree_shadows_init(struct task *task)
{
    struct task_work *work = task->work;
    struct tree_shadows_state *state;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((state = syCalloc(1, sizeof(struct tree_shadows_state))) == NULL) {
        return;
    }
    task->extra.p = state;
    LIGHT(work) = work->ang.x;
    work->ang.x = 0;
    /* @hack spelling: the mask written out, where &= would do */
    work->ang.y = work->ang.y & ~SETTING_BYTE;
    work->ang.z = 0;
    /* @hack redundant: both players' flags cleared in one assignment, though
     * syCalloc cleared the record */
    state->inside[0] = state->inside[1] = FALSE;
    collision_init(task, tree_shadows_shapes, TREE_SHADOWS_SHAPES,
                   COLLISION_OBJECT);
    collision_fit_record_radius(work->collision);
    task->step = tree_shadows_step;
    task->draw = tree_shadows_draw;
    task->destructor = tree_shadows_destroy;
}

static void tree_shadows_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the registers SHC
     * gives the draw. the image holds none of its code, so its calls are
     * stand-ins */
    if (0) {
        (void)"<- LIGHT:";
        (void)"<- ANG Y:";
        light_select(LIGHT(work), 0);
        light_select(work->ang.y, 0);
    }
}

static void tree_shadows_destroy(struct task *task)
{
}

/* whether collision keeps a touch of the first shape of the player in slot
 * player */
static Bool tree_shadows_touched_by(struct collision_work *collision,
                                    int player)
{
    struct collision_hit *hit = collision->hits;
    struct collision_hit *end = &collision->hits[COLLISION_HITS];

    for (; hit < end; hit++) {
        if (hit->hit_shape == HIT_UNUSED) {
            break;
        }
        if (hit->task->work->collision->kind == COLLISION_PLAYER &&
            hit->hit_shape == 0 && player_slot_of(hit->task) == player) {
            return TRUE;
        }
    }
    return FALSE;
}

static void tree_shadows_step(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision = work->collision;
    struct tree_shadows_state *state = task->extra.p;
    int player;
    /* @hack spelling: left, a local the test of the call sets and the next
     * test compares with TRUE, where the call tested in place would do */
    Bool left;

    if (placement_try_leave(task)) {
        return;
    }
    for (player = 0; player < GAME_PLAYERS; player++) {
        /* @hack spelling: the flag compared with TRUE, where
         * state->inside[player] alone would do */
        if (state->inside[player] == TRUE) {
            left = FALSE;
            if (!tree_shadows_touched_by(collision, player)) {
                left = TRUE;
            }
            if (left == TRUE) {
                state->inside[player] = FALSE;
                light_select(light_camera, player);
            }
        } else if (tree_shadows_touched_by(collision, player)) {
            state->light_before[player] = light_selected[player];
            light_select(LIGHT(work), player);
            state->inside[player] = TRUE;
        }
    }
    collision_register(task);
}
