/* the race's fruit trees. race_claim_tree gives a racer its tree, which the
 * racer shakes until the tree drops a fruit */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "chao/garden_field.h"
#include "chao/life.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "chaostgrace/course_models.h"
#include "chaostgrace/props.h"
#include "collision/collision.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum { RACE_TREES = 8, TREE_FRUIT = 3, TREE_BRANCHES = 4 };
/* (int)(rand() * (TREE_DRAW * RAND_SCALE)) is a tree's index at random */
#define TREE_DRAW 7.999F
/* the tree's modes. it waits for the RACE_FIELD_ENTRANT field of a racer's
 * reach. it then shakes, and settles once its fruit drops */
enum { TREE_WAIT, TREE_SHAKE, TREE_SETTLE };
/* the tree's shapes. a racer at its own tree starts to shake it on touching
 * TREE_SHAKE_SHAPE. the tree turns that shape off as the shake starts */
enum { TREE_BULK_SHAPE, TREE_SHAKE_SHAPE, TREE_SHAPES };
/* the index of the hanging fruit that drops. its scale then stays 0, which
 * draws nothing */
#define DROP_FRUIT 1

/* a tree takes SHAKE_WORK of shaking to drop its fruit. a racer's power
 * skill points count toward it, and each frame of shaking adds
 * SHAKE_PER_FRAME */
#define SHAKE_WORK 15000
#define SHAKE_PER_FRAME 30
/* the shake's phase turns SHAKE_STEP a frame, and the tree tilts SHAKE_TILT
 * degrees times its sine. once the fruit drops, the phase turns back to 0 by
 * SETTLE_STEP a frame */
#define SHAKE_STEP 0xa00
#define SHAKE_TILT 3.0F
#define SETTLE_STEP 0x200
/* the shake's sway runs from 0, at the phase's rest, to 1. the shake's sound
 * plays once a swing, as the sway falls below SWAY_LOW after rising past
 * SWAY_HIGH */
#define SWAY_HIGH 0.95F
#define SWAY_LOW 0.1F
#define SHAKE_SOUND 0x1008
#define SHAKE_PRIORITY 1
#define SHAKE_VOLUME (-15)
#define SHAKE_SOUND_FRAMES 60
/* the fruit's swing. each step the sway adds FRUIT_SWAY_GAIN of itself to the
 * swing's amplitude, up to FRUIT_SWAY_MAX degrees. the amplitude then keeps
 * FRUIT_SWAY_KEEP of itself, and the phase turns FRUIT_SWAY_STEP. the branches
 * swing as the fruit do, by BRANCH_SWAY_GAIN, BRANCH_SWAY_MAX,
 * BRANCH_SWAY_KEEP and BRANCH_SWAY_STEP */
#define FRUIT_SWAY_GAIN 0.9F
#define FRUIT_SWAY_MAX 14.0F
#define FRUIT_SWAY_KEEP 0.98F
#define FRUIT_SWAY_STEP 0x720
#define BRANCH_SWAY_GAIN 0.3F
#define BRANCH_SWAY_MAX 6.0F
#define BRANCH_SWAY_KEEP 0.986F
#define BRANCH_SWAY_STEP 0x600
/* the fruit hang FRUIT_HEIGHT above the tree's base. each tilts FRUIT_TILT out
 * and stands FRUIT_OUT from the trunk. they draw at FRUIT_SCALE times their
 * scale. the dropping fruit starts at DROP_SCALE, and the others at
 * HANGING_SCALE */
#define FRUIT_HEIGHT 30.0F
#define FRUIT_TILT NJM_DEG_ANG(30)
#define FRUIT_OUT 0.5F
#define FRUIT_SCALE 1.5F
#define DROP_SCALE 0.1F
#define HANGING_SCALE 0.85F
/* a fruit of scale FRUIT_RIPE or less swings UNRIPE_SWING as far as a riper
 * one. no fruit of these trees grows riper */
#define FRUIT_RIPE 0.9F
#define UNRIPE_SWING 0.2F
/* a dropped fruit leaves at DROP_SPEED along the tree's heading. the
 * RACE_FIELD_FRUIT_DROP field that tells the racer lasts DROP_FIELD_FRAMES */
#define DROP_SPEED 0.2F
#define DROP_FIELD_RADIUS 4.5F
#define DROP_FIELD_FRAMES 10
#define SHADOW_Y 0.5F
#define SHADOW_WIDTH 1.8F

/* a tree the race registered. fruit is the fruit it dropped, or NULL */
struct tree_entry {
    struct task *tree;
    struct task *fruit;
    Bool claimed;
};
struct tree_fruit {
    Float scale;
    Uint8 unk_04[28];
};
struct tree_work {
    struct task_work base;
    Uint8 unk_30[16];
    struct tree_fruit fruit[TREE_FRUIT];
    Uint8 unk_a0[16];
    /* the branches' swing: its phase, and its amplitude in degrees.
     * branch_index is the next branch to draw */
    Angle branch_phase;
    Float branch_amplitude;
    int branch_index;
    /* the fruit's swing, as the branches'. each draw clears fruit_index */
    Angle fruit_phase;
    Float fruit_amplitude;
    int fruit_index;
    Bool shake_armed; /* the sway has passed SWAY_HIGH since the sound */
    Uint8 unk_cc[4];
    Float sway; /* the step's sway */
    Uint8 unk_d4[12];
    Angle shake_phase;
    Uint8 unk_e4[24];
};

static int tree_count;
static struct tree_entry trees[RACE_TREES];

/* a branch's entry adds to branch_phase, and a fruit's to fruit_phase, so
 * that they swing out of step. the tree's TREE_BRANCHES branches and
 * TREE_FRUIT fruit leave each table's last entry unread */
static Angle branch_phases[] = {0x1000, 0x3200, 0x5300, 0xb500, 0xa000};
static Angle fruit_phases[] = {0x1000, 0x3200, 0x5300, 0x8000};

/* the tree's shapes, its yaw's label in the placement editor, and
 * fruit_offset. a hanging fruit's model sits at fruit_offset under the
 * fruit's scale */
struct fruit_tree_description {
    struct collision_shape shapes[TREE_SHAPES];
    char editor_yaw[20];
    NJS_POINT3 fruit_offset;
};
static const struct fruit_tree_description tree_description = {
#include "chaostgrace/tree/tree_description.inc"
};

void race_trees_reset(void)
{
    int i;
    for (i = 0; i < RACE_TREES; i++) {
        trees[i].tree = NULL;
        trees[i].fruit = NULL;
        trees[i].claimed = FALSE;
    }
    tree_count = 0;
}

void race_register_tree(struct task *tree)
{
    trees[tree_count].tree = tree;
    tree_count++;
}

static void set_tree_fruit(struct task *tree, struct task *fruit)
{
    int i;
    for (i = 0; i < RACE_TREES; i++) {
        if (trees[i].tree == tree) {
            trees[i].fruit = fruit;
            return;
        }
    }
}

#ifdef NONMATCHING
/* this side clears the link a tree holds to a fruit that dies */
void race_forget_tree_fruit(struct task *fruit)
{
    int i;
    for (i = 0; i < RACE_TREES; ++i) {
        if (trees[i].fruit == fruit) {
            trees[i].fruit = NULL;
            return;
        }
    }
}
#endif

struct task *race_claim_tree(void)
{
    int i;
    for (;;) {
        i = (int)(rand() * (TREE_DRAW * RAND_SCALE));
        if (!trees[i].claimed) {
            trees[i].claimed = TRUE;
            return trees[i].tree;
        }
    }
}

struct task *race_tree_fruit(struct task *chao)
{
    struct task *tree = CHAO_WORK(chao)->race_target;
    int i;
    if (tree) {
        for (i = 0; i < RACE_TREES; i++) {
            if (trees[i].tree == tree) {
#ifdef NONMATCHING
                /* the fruit's first step registers it and so sets its
                 * force, or destroys it where that fails. until then the
                 * tree has no fruit to give */
                if (trees[i].fruit && !trees[i].fruit->force.p) {
                    return NULL;
                }
#endif
                return trees[i].fruit;
            }
        }
    }
    return NULL;
}

static void tree_step(struct task *tree)
{
    struct tree_work *work = (struct tree_work *)tree->work;
    struct task *contact;
    Uint16 power;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    Float sway;
    Float speed;
    switch (work->base.mode) {
    /* @hack spelling: the default first, where last would do */
    default:
        break;
    case TREE_WAIT:
        contact = collision_first_with_id(tree, RACE_FIELD_ENTRANT);
        if (contact) {
            if (contact->parent) {
                contact = contact->parent;
                power = chao_skill_points(contact, CHAO_POWER);
                work->base.timer = (SHAKE_WORK - power) / SHAKE_PER_FRAME;
                collision_shape_off(tree, TREE_SHAKE_SHAPE);
                work->base.mode = TREE_SHAKE;
                work->base.submode = 0;
            }
        }
        break;
    case TREE_SHAKE:
        if (work->base.timer > 0) {
            work->base.timer--;
            work->shake_phase += SHAKE_STEP;
        } else {
#ifdef NONMATCHING
            /* the racer learns of the fruit only by this field. where the
             * field can't be made, the fruit waits for the next step */
            if (!garden_timed_enemy_field(RACE_FIELD_FRUIT_DROP,
                                          &work->base.pos, DROP_FIELD_RADIUS,
                                          DROP_FIELD_FRAMES)) {
                break;
            }
#else
            garden_timed_enemy_field(RACE_FIELD_FRUIT_DROP, &work->base.pos,
                                     DROP_FIELD_RADIUS, DROP_FIELD_FRAMES);
#endif
            work->fruit[DROP_FRUIT].scale = 0.0F;
            position = work->base.pos;
            position.y += FRUIT_HEIGHT;
            /* @hack spelling: speed, a local holding DROP_SPEED, where
             * DROP_SPEED would do */
            speed = DROP_SPEED;
            velocity.x = njSin(work->base.ang.y) * speed;
            velocity.y = 0.0F;
            velocity.z = njCos(work->base.ang.y) * speed;
            set_tree_fruit(tree, race_fruit_create(&position, 0, &velocity));
            work->base.mode = TREE_SETTLE;
            work->base.submode = 0;
        }
        break;
    case TREE_SETTLE:
        work->shake_phase = angle_approach(work->shake_phase, 0, SETTLE_STEP);
        break;
    }
    sway = 0.5F + (0.5F * njSin(work->shake_phase - QUARTER_TURN));
    work->sway = sway;
    if (sway < SWAY_LOW && work->shake_armed) {
        sound_play_at_for(SHAKE_SOUND, tree, SHAKE_PRIORITY, SHAKE_VOLUME,
                          SHAKE_SOUND_FRAMES, &work->base.pos);
        work->shake_armed = FALSE;
    }
    if (sway > SWAY_HIGH && !work->shake_armed) {
        work->shake_armed = TRUE;
    }
    work->fruit_amplitude += sway * FRUIT_SWAY_GAIN;
    if (work->fruit_amplitude > FRUIT_SWAY_MAX) {
        work->fruit_amplitude = FRUIT_SWAY_MAX;
    }
    work->fruit_phase += FRUIT_SWAY_STEP;
    work->fruit_amplitude *= FRUIT_SWAY_KEEP;
    work->branch_amplitude += sway * BRANCH_SWAY_GAIN;
    if (work->branch_amplitude > BRANCH_SWAY_MAX) {
        work->branch_amplitude = BRANCH_SWAY_MAX;
    }
    work->branch_phase += BRANCH_SWAY_STEP;
    work->branch_amplitude *= BRANCH_SWAY_KEEP;
    collision_register(tree);
}

static void tree_draw_branches(struct tree_work *work, NJS_CNK_OBJECT *node)
{
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)node->pos);
    njRotateXYZ(NULL, node->ang[0], node->ang[1], node->ang[2]);
    if (node->model) {
        njRotateX(NULL, (Angle)(work->branch_amplitude *
                                njSin(work->branch_phase +
                                      branch_phases[work->branch_index]) *
                                ANGLE_PER_DEGREE));
        njCnkEasyDrawModel(node->model);
        work->branch_index++;
    }
    if (node->child) {
        tree_draw_branches(work, node->child);
    }
    njPopMatrixEx();
    if (node->sibling) {
        tree_draw_branches(work, node->sibling);
    }
}

static void tree_draw_canopy(struct tree_work *work, NJS_CNK_OBJECT *node)
{
    unsigned int i;
    struct tree_fruit *fruit;
    Float scale;
    NJS_POINT3 offset;
    work->branch_index = 0;
    work->fruit_index = 0;
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)node->pos);
    njRotateXYZ(NULL, node->ang[0], node->ang[1], node->ang[2]);
    njCnkEasyDrawModel(node->model);
    if (node->child) {
        tree_draw_branches(work, node->child);
    }
    njPopMatrixEx();
    if (node->sibling) {
        tree_draw_branches(work, node->sibling);
    }
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, FRUIT_HEIGHT, 0.0F);
    /* @hack type: i unsigned and compared as an int, where an int would do */
    for (i = 0; (int)i < TREE_FRUIT; i++) {
        fruit = &work->fruit[i];
        scale = fruit->scale * FRUIT_SCALE;
        offset = tree_description.fruit_offset;
        njPushMatrixEx();
        njRotateY(NULL, i * THIRD_TURN);
        njRotateX(NULL, FRUIT_TILT);
        njTranslate(NULL, 0.0F, FRUIT_OUT, 0.0F);
        if (scale > 0.0F) {
            if (fruit->scale > FRUIT_RIPE) {
                njRotateX(NULL,
                          (Angle)(work->fruit_amplitude *
                                  njSin(work->fruit_phase + fruit_phases[i]) *
                                  ANGLE_PER_DEGREE));
            } else {
                njRotateX(NULL,
                          (Angle)(work->fruit_amplitude *
                                  njSin(work->fruit_phase + fruit_phases[i]) *
                                  (ANGLE_PER_DEGREE * UNRIPE_SWING)));
            }
            njScale(NULL, scale, scale, scale);
            njTranslateEx(&offset);
            njCnkEasyDrawModel(race_fruit_model->model);
        }
        njPopMatrixEx();
    }
    njPopMatrixEx();
}

static void tree_draw(struct task *tree)
{
    /* @hack redundant: base, work, push, pop and rotate_x declared register,
     * where plain locals would do */
    /* @hack spelling: base and work, two views of one work, and pointers to
     * the matrix calls, where one work pointer and direct calls would do */
    register struct task_work *base = tree->work;
    register struct tree_work *work = (struct tree_work *)base;
    register Bool (*push)(void);
    register Bool (*pop)(void);
    register void (*rotate_x)(NJS_MATRIX *, Angle);
    Float shadow_origin;
    njSetTexture(&race_fruit_texlist);
    push = njPushMatrixEx;
    push();
    njTranslateEx(&base->pos);
    njRotateY(NULL, base->ang.y);
    rotate_x = njRotateX;
    pop = njPopMatrixEx;
    /* @hack spelling: shadow_origin, a local holding 0, where 0.0F would do */
    shadow_origin = 0.0F;
    push();
    rotate_x(NULL, (Angle)(njSin(work->shake_phase) *
                           (ANGLE_PER_DEGREE * SHAKE_TILT)));
    tree_draw_canopy(work, &race_tree_model);
    pop();
    njTranslate(NULL, shadow_origin, SHADOW_Y, shadow_origin);
    njScale(NULL, SHADOW_WIDTH, 1.0F, SHADOW_WIDTH);
    njCnkModDrawModel(&garden_shadow_model);
    pop();
}

static void tree_destroy(struct task *tree)
{
}

void race_tree_init(struct task *tree)
{
    struct task_work *old_work = tree->work;
    struct tree_work *work;
    tree->step = tree_step;
    tree->draw = tree_draw;
    tree->destructor = tree_destroy;
    work = syCalloc(1, sizeof(struct tree_work));
    work->base = *old_work;
    syFree(tree->work);
    tree->work = &work->base;
    work->fruit[0].scale = HANGING_SCALE;
    work->fruit[DROP_FRUIT].scale = DROP_SCALE;
    work->fruit[2].scale = HANGING_SCALE;
    race_register_tree(tree);
    collision_init(tree, tree_description.shapes, TREE_SHAPES,
                   COLLISION_OBJECT);
}
