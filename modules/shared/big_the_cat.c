/* Big the Cat, BIG THE CAT in an object list: Big stands and plays one of
 * his actions */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/big_the_cat.h"
#include "shared/big_art/motions.h"
#include "shared/big_art/textures.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"

/* Big's modes, in his work's mode. nothing in this source sets MODE_LEAVE */
enum big_mode { MODE_INIT, MODE_PLAY, MODE_PLAY_SHAKE, MODE_LEAVE, MODE_LEFT };

/* a type is the index in big_actions of the action Big plays. a Big of
 * TYPE_CHEAP draws big_cheap_object, with no motion and no shadow */
#define TYPE_SHAKE 1
#define TYPE_CHEAP 23

#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.2F
#define BIG_LIGHT 2
/* the cheap model's angle about y keeps only these bits. it turns by whole
 * 256ths of a turn */
#define CHEAP_TURN_BITS 0xff00

#include "shared/big_the_cat/text.inc"

static void big_draw(struct task *task);
static void big_destroy(struct task *task);

static NJS_TEXNAME textures[BIG_TEXTURE_COUNT] = {
#include "shared/big_the_cat/textures.inc"
};
NJS_TEXLIST big_texlist = {
#include "shared/big_the_cat/big_texlist.inc"
};

/* Big's cylinder, which no code passes to collision_init */
static struct collision_shape unused_collision_cylinder = {
#include "shared/big_the_cat/unused_collision_cylinder.inc"
};

struct enemy_action big_actions[BIG_ACTION_COUNT] = {
#include "shared/big_the_cat/big_actions.inc"
};

/* the task that keeps Big's texture list loaded, or null while the list isn't.
 * its extra holds big_cheap_object, as njCnkDirectObjectCompile compiled it, or
 * null where the heap had no room */
static struct task *big_textures = NULL;

/* the set editor's names for Big's actions, in big_actions' order */
static const char *big_action_names[BIG_ACTION_COUNT] = {
#include "shared/big_the_cat/big_action_names.inc"
};

static void big_textures_release(struct task *task)
{
    njReleaseTexture(&big_texlist);
    big_textures = NULL;
}

static void big_textures_step(struct task *task)
{
}

static void big_init(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy = enemy_work_alloc(task);
    /* @hack spelling: size, a local for njCnkDirectObjectCompileSize's
     * result, where the call passed in place would do */
    Uint32 size;

    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= BIG_ACTION_COUNT || work->submode < 0) {
        work->submode = 0;
    }
    enemy_set_home(work, enemy);
    enemy->motion.table = big_actions;
    enemy->motion.object = &big_object;
    /* @hack spelling: a switch of one case and a default, where if and else
     * would do */
    switch (work->submode) {
    case TYPE_SHAKE:
        work->mode = MODE_PLAY_SHAKE;
        enemy->motion.next = TYPE_SHAKE;
        break;
    default:
        work->mode = MODE_PLAY;
        enemy->motion.next = work->submode;
        break;
    }
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    task->draw = big_draw;
    task->destructor = big_destroy;
    if (big_textures == NULL) {
        texture_load_pvm(BIG_ARCHIVE_NAME, &big_texlist);
        big_textures = task_create(0, 0, big_textures_step, BIG_TASK_NAME);
        njSetTexture(&big_texlist);
        light_apply(BIG_LIGHT);
        njFogEnable();
        njSetDirectClip();
        size = njCnkDirectObjectCompileSize(big_cheap_object);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((big_textures->extra.p = syCalloc(1, size)) != NULL) {
            /* @hack spelling: compiled, a local for the holder's extra, where
             * extra passed in place would do */
            NJS_DIRECT_HEAD *compiled = big_textures->extra.p;

            njCnkDirectObjectCompile(big_cheap_object, compiled);
        }
        big_textures->destructor = big_textures_release;
    }
}

static void big_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* @hack redundant: the parameters task and enemy, which the function doesn't
 * read */
static void big_count_timer(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    if (work->mode != MODE_LEAVE && !game_state.time_stopped) {
        work->timer++;
    }
}

static void big_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    njSetTexture(&big_texlist);
    /* @hack redundant: a test of scl.x's type after 0 &&, which never passes,
     * where motion.next's test alone would do. the image holds the test's
     * code */
    /* @hack spelling: TYPE_CHEAP before the call in the test, where the call
     * first would do */
    if ((0 && TYPE_CHEAP == njRoundOff(work->scl.x)) ||
        enemy->motion.next == TYPE_CHEAP) {
        /* @hack redundant: if (1) and an else no path reaches, where the cheap
         * draw alone would do. the image holds the else's code from its do
         * loop on */
        if (1) {
            /* @hack spelling: compiled, a local for the holder's extra, where
             * extra passed in place would do */
            NJS_DIRECT_HEAD *compiled = big_textures->extra.p;

            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.x);
            njRotateX(NULL, work->ang.z);
            njRotateY(NULL, (work->ang.y + QUARTER_TURN) & CHEAP_TURN_BITS);
            /* @bug memory: njDirectDrawObject reads through a null model
             * where big_init's syCalloc found no room, though big_init tests
             * the result for null: 0xf8 and 0x23a of the object's code */
#ifdef NONMATCHING
            /* this side repairs the tag above: without a compiled model, the
             * cheap draw draws nothing */
            if (compiled != NULL) {
                njDirectDrawObject(compiled);
            }
#else
            njDirectDrawObject(compiled);
#endif
            njPopMatrixEx();
        } else {
            /* @hack spelling: action, a local for the type, where
             * work->submode read at each use would do */
            int action;
            /* @hack spelling: entry, a local for the type's action, where
             * big_actions[action] read at each use would do */
            struct enemy_action *entry;
            /* @hack spelling: frame, a local for motion.frame, where
             * motion.frame worked in place would do */
            Float frame;

            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.x);
            njRotateX(NULL, work->ang.z);
            /* @hack redundant: calls no path reaches, where nothing would
             * do. the image holds none of their code, but SHC counts their
             * uses as it assigns the draw's registers */
            if (0) {
                njRotateX(NULL, work->ang.z);
                njRotateX(NULL, work->ang.z);
                njPopMatrixEx();
            }
            action = work->submode;
            entry = &big_actions[action];
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
            frame = enemy->motion.frame;
            /* @hack spelling: a loop that tests at its end, where a while
             * would do. no path reaches the loop, so the two behave the
             * same */
            do {
                frame -= entry->motion->nbFrame;
            } while (frame >= entry->motion->nbFrame - 1);
            enemy->motion.frame = frame;
            njCnkSimpleDrawMotion(enemy->motion.object,
                                  big_actions[action].motion, frame);
            njPopMatrixEx();
        }
    } else {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.x);
        njRotateX(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            njCnkSimpleDrawMotion(
                enemy->motion.object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        enemy_draw_shadow(work, enemy);
    }
    /* @hack redundant: a block no path reaches, where nothing would do. the
     * image holds its four calls of njPrintC */
    if (0) {
        /* @hack redundant: a test no path reaches, where nothing would do.
         * the image holds none of its code, but SHC counts its uses as it
         * assigns the draw's registers */
        if (0 && 1.0F) {
        }
        /* @hack redundant: the same, a second such test */
        if (0 && 1.0F) {
        }
        /* @hack redundant: the same, a third such test */
        if (0 && 1.0F) {
        }
        /* @hack spelling: the calls as a switch's cases, each running on
         * into the next, where the four calls in a row would do */
        switch (work->submode) {
        case 0:
            njPrintC(NJM_LOCATION(17, 16), BIG_ANGLE_X_LABEL);
        case 1:
            njPrintC(NJM_LOCATION(17, 17), BIG_ANGLE_Y_LABEL);
        case 2:
            njPrintC(NJM_LOCATION(17, 18), BIG_ANGLE_Z_LABEL);
        case 3:
            njPrintC(NJM_LOCATION(17, 19), BIG_TYPE_LABEL);
        }
    }
}

void big_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (game_state.unk_53 == 0 || game_state.two_players != 0 ||
        game_state.unk_5b != 0) {
        placement_kill(task);
        return;
    }
    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        big_init(task, work);
        return;
    case MODE_PLAY:
    case MODE_PLAY_SHAKE:
        big_count_timer(task, work, enemy);
        break;
    case MODE_LEAVE:
        work->mode = MODE_LEFT;
        /* fall through */
    case MODE_LEFT:
        placement_kill(task);
        /* @hack spelling: a goto to the function's end, where return would
         * do */
        goto end;
    }
    /* @hack spelling: an empty then and the steps in the else, where
     * !game_state.time_stopped as the test would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
    }
    /* Big has no collision record, so the call enters nothing */
    collision_register(task);
    return;
end:
    /* @hack redundant: a call of big_init in a switch no path reaches, where
     * nothing would do. the image holds the call's code */
    if (0) {
        switch (work->mode) {
        case MODE_INIT:
            big_init(task, work);
            break;
        }
    }
}
