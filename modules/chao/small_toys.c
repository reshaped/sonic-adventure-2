/* a Chao's small toys and its play with them alone. the bubble wand and the
 * book make tasks of their own */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/face.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/parts.h"
#include "chao/small_toys.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "draw/lines.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

/* a bubble bursts after BUBBLE_FRAMES frames */
#define BUBBLE_FRAMES 1800

enum book_mode {
    BOOK_OPEN = 0,
    BOOK_WAIT = 1,
    BOOK_TURN_PAGE = 2,
    BOOK_CLOSE = 4
};
#define BOOK_SIZE(work) ((work)->scl.y)
#define BOOK_FRAME(work) ((work)->scl.x)
#define PAGE_REST_FRAME 15.0F
#define PAGE_TURNED_FRAME 23.0F

static NJS_TEXANIM bubble_cell = {
#include "chao/small_toys/bubble_cell.inc"
};

static void bubble_draw(struct task *bubble)
{
    /* @hack redundant: work declared register, where a plain local would do */
    register struct task_work *work = bubble->work;
    NJS_SPRITE sprite;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&garden_toy_texlist);
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    sprite.tlist = &garden_toy_texlist;
    sprite.tanim = &bubble_cell;
    sprite.ang = 0;
    sprite.sx = 0.2F;
    sprite.sy = 0.2F;
    sprite.p.x = 0.0F;
    sprite.p.y = 0.0F;
    sprite.p.z = 0.0F;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njDrawSprite3D(&sprite, 0, NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void bubble_step(struct task *bubble)
{
    struct task_work *work = bubble->work;
    struct mover *mover = bubble->motion.p;

    if (njScalor(&mover->spd) > 0.05F) {
        mover->spd.x *= 0.9F;
        mover->spd.y *= 0.9F;
        mover->spd.z *= 0.9F;
    }
    work->scl.y += 0.15F;
    if (work->scl.y > 1.0F) {
        work->scl.y = 0.0F;
    }
    if (work->timer++ > BUBBLE_FRAMES) {
        task_kill(bubble);
    }
    work->pos.y += (njSin(work->ang.y) * 0.02F) + 0.01F;
    work->ang.y += 0x100;
    mover_step(bubble);
}

static void launch_bubble(NJS_POINT3 *position, NJS_VECTOR *velocity)
{
    struct task *bubble;
    struct task_work *work;
    struct mover *mover;

    bubble = task_create(TASK_WORK, 4, bubble_step, "ALO_ShabonExecutor");
#ifdef NONMATCHING
    /* this side makes no bubble where task_create fails. the matching side
     * writes through the null */
    if (bubble == NULL) {
        return;
    }
#endif
    work = bubble->work;
    bubble->draw = bubble_draw;
    mover = mover_alloc(bubble);
#ifdef NONMATCHING
    /* this side destroys the bubble where mover_alloc fails. the matching side
     * writes through the null */
    if (mover == NULL) {
        task_destroy(bubble);
        return;
    }
#endif
    work->pos = *position;
    if (velocity) {
        mover->spd = *velocity;
    }
    mover_set_gravity(bubble, 0.0F);
    mover->probe_height = 3.0F;
    mover->floor_bounce = 0.8F;
    mover->friction = 0.7F;
    work->ang.y = RANDOM_TURN();
    work->scl.x = (rand() * (0.6F * RAND_SCALE)) + 0.7F;
    work->scl.y = 0.0F;
}

int chao_blow_bubbles(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct chao_shape_work *shape;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    Float speed;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_SYABON_JYABU);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &bubble_bottle,
                        &garden_toy_texlist);
        chao_equip_node(chao, CHAO_NODE_HAND_11, &bubble_wand,
                        &garden_toy_texlist);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer++ > 60 && behavior->timer % 10 == 0 &&
            RANDOM_UNIT() < 0.4F) {
            shape = &CHAO_WORK(chao)->shape;
            speed = (rand() * (0.1F * RAND_SCALE)) + 0.2F;
            velocity.x = (shape->mouth_direction.x * speed) +
                         (rand() * (0.1F * RAND_SCALE));
            velocity.y = (shape->mouth_direction.y * speed) + 0.1F;
            velocity.z = (shape->mouth_direction.z * speed) +
                         (rand() * (0.1F * RAND_SCALE));
            position.x = shape->hand_11.x + (shape->mouth_direction.x * 0.5F);
            position.y =
                shape->hand_11.y + (shape->mouth_direction.y * 0.5F) + 0.05F;
            position.z = shape->hand_11.z + (shape->mouth_direction.z * 0.5F);
            launch_bubble(&position, &velocity);
        }
        if (chao_motion_held(chao)) {
            if (RANDOM_UNIT() < 0.25F) {
                return BEHAVIOR_FINISH;
            } else {
                chao_link_motion(chao, CHAO_ANIM_SYABON_JYABU);
                behavior->timer = 0;
            }
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_sweep(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HOUKI);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_equip_node(chao, CHAO_NODE_HAND_11, &chao_broom,
                        &garden_toy_texlist);
        behavior->timer = RANDOM_COUNT(600, 201);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

#define HOLD_TOY(name, motion, toy, spread)                                    \
    static int name(struct task *chao)                                         \
    {                                                                          \
        struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;           \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            chao_link_motion(chao, (motion));                                  \
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);           \
            chao_equip_node(chao, CHAO_NODE_HAND_11, &(toy),                   \
                            &garden_toy_texlist);                              \
            behavior->timer = RANDOM_COUNT(600, (spread));                     \
            behavior->mode++;                                                  \
            /* fall through */                                                 \
        case 1:                                                                \
            if (behavior->timer-- <= 0) {                                      \
                return BEHAVIOR_FINISH;                                        \
            }                                                                  \
        }                                                                      \
        return BEHAVIOR_CONTINUE;                                              \
    }

HOLD_TOY(shake_rattle, CHAO_ANIM_GARAGARA, chao_rattle, 201.0F)
HOLD_TOY(hold_stick, CHAO_ANIM_HANABI, chao_stick, 201.0F)

static void book_step(struct task *book)
{
    struct task_work *work = book->work;

    switch (work->mode) {
    case BOOK_OPEN:
        BOOK_SIZE(work) += 0.1F;
        if (BOOK_SIZE(work) > 1.0F) {
            BOOK_SIZE(work) = 1.0F;
            work->mode = BOOK_WAIT;
        }
        break;
    case BOOK_WAIT:
        BOOK_FRAME(work) = PAGE_REST_FRAME;
        break;
    case BOOK_TURN_PAGE:
        BOOK_FRAME(work) += 0.15F;
        if (BOOK_FRAME(work) > PAGE_TURNED_FRAME) {
            BOOK_FRAME(work) = PAGE_TURNED_FRAME;
            work->mode = BOOK_WAIT;
        }
        break;
    case BOOK_CLOSE:
        BOOK_SIZE(work) -= 0.1F;
        if (BOOK_SIZE(work) < 0.0F) {
            BOOK_SIZE(work) = 0.0F;
            task_kill(book);
        }
        break;
    }
}

static void book_draw(struct task *book)
{
    struct task_work *work = book->work;
    Float scale = BOOK_SIZE(work);

    if (scale < 0.01F) {
        return;
    }
    njSetTexture(&garden_toy_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawMotion(&chao_picture_book, &book_page_motion,
                        BOOK_FRAME(work));
    njPopMatrixEx();
}

#ifdef NONMATCHING
/* this side kills the book and clears the links between it and its reader. so
 * neither reads the other once one of them dies */
void chao_release_book(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);

    if (work->book != NULL) {
        work->book->extra.task = NULL;
        task_kill(work->book);
        work->book = NULL;
    }
}
#endif

static void book_destroy(struct task *book)
{
#ifdef NONMATCHING
    /* this side clears the reader's link to the book where it still names this
     * book */
    struct task *reader = book->extra.task;

    if (reader != NULL && CHAO_WORK(reader)->book == book) {
        CHAO_WORK(reader)->book = NULL;
    }
    book->extra.task = NULL;
#endif
}

#ifdef NONMATCHING
/* this side takes the book that read_book made. so a book that can't be made
 * starts no reading */
static void create_book(struct task *chao, struct task *book)
#else
static void create_book(struct task *chao)
#endif
{
#ifdef NONMATCHING
    /* this side lets go of an earlier book by chao_release_book, and links the
     * new one to its reader */
    chao_release_book(chao);
    book->extra.task = chao;
#else
    struct task *book;

    if (CHAO_WORK(chao)->book) {
        task_kill(CHAO_WORK(chao)->book);
    }
    book = task_create(TASK_WORK, 2, book_step, "BookExecutor");
#endif
    CHAO_WORK(chao)->book = book;
    book->work->pos = chao->work->pos;
    book->work->ang.y = chao->work->ang.y;
    book->draw = book_draw;
    book->destructor = book_destroy;
}

static int read_book(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
#ifdef NONMATCHING
    /* this side finishes the behavior where the book died before the reading
     * ended */
    struct task *book;

    if (behavior->mode != 0 && CHAO_WORK(chao)->book == NULL) {
        return BEHAVIOR_FINISH;
    }
#endif

    switch (behavior->mode) {
    case 0:
#ifdef NONMATCHING
        /* this side makes the book before the reading starts, and tries again
         * the next step where task_create fails. the matching side writes
         * through the null */
        book = task_create(TASK_WORK, 2, book_step, "BookExecutor");
        if (book == NULL) {
            return BEHAVIOR_CONTINUE;
        }
#endif
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_BOOK_START);
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        }
        behavior->timer = RANDOM_COUNT(300, 101);
#ifdef NONMATCHING
        /* this side hands create_book the book made above */
        create_book(chao, book);
#else
        create_book(chao);
#endif
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- > 0) {
            break;
        }
        if (RANDOM_UNIT() < 0.3F) {
            CHAO_WORK(chao)->book->work->mode = BOOK_CLOSE;
            return BEHAVIOR_FINISH;
        } else {
            CHAO_WORK(chao)->book->work->mode = BOOK_TURN_PAGE;
            chao_link_motion(chao, CHAO_ANIM_BOOK_MEKURI);
            behavior->mode++;
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            chao_link_motion(chao, CHAO_ANIM_BOOK_YOMU);
            behavior->timer = RANDOM_COUNT(120, 181);
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int play_with_car(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_MINICAR);
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        }
        chao_equip_hand_11(chao, &chao_toy_car, &garden_toy_texlist);
        behavior->timer = RANDOM_COUNT(300, 101);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- > 0) {
            break;
        }
        if (RANDOM_UNIT() < 0.2F) {
            return BEHAVIOR_FINISH;
        }
        if (RANDOM_UNIT() < 0.5F) {
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        }
        chao_link_motion(chao, CHAO_ANIM_MINICAR_MIAGE);
        behavior->mode++;
        break;
    case 2:
        if (chao_motion_held(chao)) {
            chao_link_motion(chao, CHAO_ANIM_MINICAR);
            if (RANDOM_UNIT() < 0.5F) {
                chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
            }
            behavior->timer = RANDOM_COUNT(380, 181);
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int play_with_doll(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DOLL_YOSHIYOSHI);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_equip_node(chao, CHAO_NODE_HAND_4, &chao_doll,
                        &garden_toy_texlist);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            if (RANDOM_UNIT() < 0.2F) {
                return BEHAVIOR_FINISH;
            } else if (RANDOM_UNIT() < 0.6F) {
                chao_link_motion(chao, CHAO_ANIM_DOLL_YOSHIYOSHI);
            } else {
                chao_link_motion(chao, CHAO_ANIM_DOLL_TAKAITAKAI);
            }
        }
    }
    return BEHAVIOR_CONTINUE;
}

HOLD_TOY(ride_pogo_stick, CHAO_ANIM_HOPPING, chao_pogo_stick, 301.0F)

chao_behavior_fn small_toy_actions[CHAO_SMALL_TOYS] = {
    shake_rattle,   play_with_car,     read_book,  NULL,
    play_with_doll, chao_sweep,        hold_stick, ride_pogo_stick,
    NULL,           chao_blow_bubbles, NULL,       NULL};

int chao_choose_small_toy(struct task *actor)
{
    /* @hack redundant: chao, a copy of actor, where the parameter would do */
    struct task *chao = actor;
    /* @hack redundant: room for 32 toys in available, where CHAO_SMALL_TOYS
     * would do */
    int available[32];
    int count = 0;
    int toy;
    int selected;

    for (toy = 0; toy < CHAO_SMALL_TOYS; toy++) {
        /* @hack spelling: the toy and its behavior tested by two ifs, where
         * one && would do */
        if (chao_has_small_toy(chao, toy)) {
            if (small_toy_actions[toy]) {
                available[count] = toy;
                count++;
            }
        }
    }
    if (count > 0) {
        selected = available[(Uint32)RANDOM_BELOW(count)];
        chao_set_behavior(chao, small_toy_actions[selected]);
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}
