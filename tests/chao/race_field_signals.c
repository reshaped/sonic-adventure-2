/* Exercise the private course callbacks on ordinary tree and lane records.
 * Publish simulated contacts only when their field allocation succeeds. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/carry.h"
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/motion.h"
#include "chao/movement.h"
#include "chaostgrace/movement.h"
#include "chaostgrace/obstacles.h"
#include "chaostgrace/race.h"
#include "chaostgrace/scavenger.c"
#include "chaostgrace/tree.c"

static struct task racer, tree, panel, fruit, field;
static struct chao_work racer_work;
static struct tree_work growing_tree;
static struct task_work panel_work, fruit_work;
static NJS_CNK_MODEL panel_model;
static int blocked_kind, tries[256], issued[256];
static int shake_motions, submit_motions, resistance, slowing, drops;
static int textures, correct_sounds, victories, queued;
static chao_behavior_fn next_behavior;

NJS_CNK_OBJECT scavenger_panel_a, scavenger_panel_b, scavenger_panel_c,
    scavenger_panel_d;

static struct task *try_field(int kind)
{
    ++tries[kind];
    if (kind == blocked_kind) {
        return NULL;
    }
    ++issued[kind];
    return &field;
}

struct task *garden_create_timed_field(struct task *parent, int kind,
                                       NJS_POINT3 *pos, Float radius,
                                       int frames)
{
    assert(parent == &racer && kind == RACE_FIELD_ENTRANT && radius == 1.5F &&
           frames == 2);
    assert(pos->x == racer_work.base.pos.x &&
           pos->y == racer_work.base.pos.y + 1.0F);
    assert(pos->z == racer_work.base.pos.z + 3.0F);
    field.parent = parent;
    return try_field(kind);
}

struct task *garden_timed_enemy_field(int kind, NJS_POINT3 *pos, Float radius,
                                      int frames)
{
    assert(frames == 10);
    if (kind == RACE_FIELD_FRUIT_DROP) {
        assert(pos == &growing_tree.base.pos && radius == 4.5F);
    } else {
        assert(kind == RACE_FIELD_RIGHT_ANSWER && pos == &panel_work.pos &&
               radius == 3.0F);
    }
    return try_field(kind);
}

struct task *garden_hit_kind(struct task *task, int shape, Uint8 kind)
{
    assert(task == &racer && shape == 1);
    return issued[kind] ? &field : NULL;
}

struct task *collision_first_with_id(struct task *task, Uint8 kind)
{
    assert(kind == RACE_FIELD_ENTRANT);
    return task == &tree && issued[kind] ? &field : NULL;
}
Bool collision_register(struct task *task)
{
    return TRUE;
}
void collision_shape_off(struct task *task, int shape)
{
    assert(task == &tree && shape == 1);
}
int chao_skill_points(const struct task *task, Uint16 skill)
{
    assert(task == &racer && skill == CHAO_POWER);
    return 14970; /* one shaking frame */
}
Float njSin(Angle angle)
{
    return (Float)sin(angle / ANGLE_PER_RADIAN);
}
Float njCos(Angle angle)
{
    return (Float)cos(angle / ANGLE_PER_RADIAN);
}
Angle angle_approach(Angle from, Angle to, Angle step)
{
    return to;
}
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    return 0;
}
struct task *race_fruit_create(NJS_POINT3 *pos, Angle yaw, NJS_VECTOR *velocity)
{
    assert(pos->x == growing_tree.base.pos.x &&
           pos->y == growing_tree.base.pos.y + 30.0F);
    assert(pos->z == growing_tree.base.pos.z && yaw == 0);
    assert(velocity->x == 0.0F && velocity->y == 0.0F && velocity->z == 0.2F);
    ++drops;
    return &fruit;
}
void chao_resist_displacement(struct task *task)
{
    assert(task == &racer);
    ++resistance;
}
void chao_slow_on_floor(struct task *task, Float scale)
{
    assert(task == &racer && scale == 0.9F);
    ++slowing;
}
void chao_link_motion(struct task *task, Sint32 index)
{
    assert(task == &racer);
    if (index == 213) {
        ++shake_motions;
    } else {
        assert(index == 190);
        ++submit_motions;
    }
}
void chao_set_behavior(struct task *task, chao_behavior_fn action)
{
    assert(task == &racer);
    next_behavior = action;
}
int race_run(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
void race_run_forward(struct task *task)
{
    assert(0);
}
void chao_link_walk_motion(struct task *task)
{
    assert(0);
}
int garden_turn_to_object(struct task *task, struct task *other, int step)
{
    assert(0);
    return 0;
}
void chao_set_timed_eye(struct task *task, int eye, int frames)
{
    assert(task == &racer && eye == 4 && frames == -1);
}
Bool chao_place_held_item(struct task *task)
{
    return garden_mutual_partner(task) != NULL;
}
void chao_set_timed_behavior(struct task *task, chao_behavior_fn action,
                             int frames)
{
    assert(task == &racer && action == race_victory_pose && frames == 120);
    ++victories;
}
void chao_queue_behavior(struct task *task, chao_behavior_fn action)
{
    assert(task == &racer && action == race_run);
    ++queued;
}
int race_victory_pose(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
void mover_set_spd(struct task *task, NJS_VECTOR *spd)
{
    assert(0);
}
void chao_set_timed_mouth(struct task *task, int mouth, int frames)
{
    assert(0);
}
void chao_show_mark(struct task *task, Sint8 mark, int frames)
{
    assert(0);
}

void garden_animate_texture(NJS_CNK_MODEL *mesh, int texture)
{
    assert(mesh == scavenger_panel_a.model);
    assert(texture == scavenger_answers[textures == 0 ? 0 : 1]);
    ++textures;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    assert(id == 3);
    ++correct_sounds;
    return 0;
}

/* This runner submits the right toy. An attempt to allocate a penalty is a
 * separate failed outcome even if the panel also displayed the right answer. */
struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    abort();
}

static void shake(void)
{
    int before = slowing;
    assert(race_shake_tree(&racer) == BEHAVIOR_CONTINUE);
    assert(slowing == before + 1 && shake_motions == 1 && resistance == 1);
}

static void run_tree(int failed)
{
    int i;
    tree.work = &growing_tree.base;
    growing_tree.fruit[1].scale = 0.1F;
    growing_tree.base.pos.z = 3.0F;
    race_trees_reset();
    race_register_tree(&tree);
    racer_work.race_target = &tree;
    blocked_kind = failed;
    if (failed == RACE_FIELD_ENTRANT) {
        for (i = 0; i < 3; ++i) {
            shake();
            tree_step(&tree);
            assert(!growing_tree.base.mode && tries[failed] == i + 1);
            assert(!issued[failed] && !next_behavior);
        }
        blocked_kind = 0;
    }
    shake();
    tree_step(&tree);
    assert(issued[RACE_FIELD_ENTRANT] == 1 && growing_tree.base.mode == 1);
    tree_step(&tree);
    assert(growing_tree.base.timer == 0 && !tries[RACE_FIELD_FRUIT_DROP]);
    if (failed == RACE_FIELD_FRUIT_DROP) {
        for (i = 0; i < 3; ++i) {
            tree_step(&tree);
            shake();
            assert(growing_tree.base.mode == 1 &&
                   growing_tree.fruit[1].scale == 0.1F);
            assert(tries[failed] == i + 1 && !issued[failed] && !drops);
            assert(!race_tree_fruit(&racer) &&
                   racer_work.race_target == &tree && !next_behavior);
        }
        blocked_kind = 0;
    }
    tree_step(&tree);
    assert(growing_tree.base.mode == 2 && growing_tree.fruit[1].scale == 0.0F);
    assert(drops == 1 && issued[RACE_FIELD_FRUIT_DROP] == 1);
    fruit.work = &fruit_work;
    assert(garden_register_object(GARDEN_OBJECT_FRUIT, &fruit, 0));
    assert(race_tree_fruit(&racer) == &fruit);
    shake();
    assert(next_behavior == race_approach_tree_item &&
           racer_work.race_target == &fruit);
    tree_step(&tree);
    assert(drops == 1 && issued[RACE_FIELD_ENTRANT] == 1 &&
           issued[RACE_FIELD_FRUIT_DROP] == 1);
    assert(garden_unregister_object(&fruit));
}

static void run_answer(int failures)
{
    struct mover racer_motion = {0};
    struct task toy = {0};
    struct task_work toy_work = {0};
    int i;
    NJS_POINT3 lane = {4.0F, 6.0F, -8.0F};
    scavenger_lane_positions[0] = lane;
    toy.work = &toy_work;
    toy_work.tick = race_scavenger_target = 2;
    racer.motion.p = &racer_motion;
    assert(garden_register_object(GARDEN_OBJECT_TOY, &toy, 2));
    assert(garden_pair_objects(&racer, &toy));
    assert(race_join_scavenger_lane(&racer) && racer_work.race_slot == 0);
    assert(racer_motion.target.x == lane.x && racer_motion.target.y == lane.y &&
           racer_motion.target.z == lane.z);
    scavenger_panel_a.model = &panel_model;
    panel.work = &panel_work;
    panel_work.pos = scavenger_lane_positions[0];
    panel_work.timer = 1801;
    answer_step(&panel);
    assert(panel_work.mode == 1);
    assert(race_submit_answer(&racer) == BEHAVIOR_CONTINUE);
    assert(race_scavenger_submitted[0]);
    answer_step(&panel);
    assert(panel_work.mode == 2 && !race_scavenger_submitted[0] &&
           textures == 1);
    for (i = 0; i < 61; ++i) {
        answer_step(&panel);
    }
    assert(panel_work.timer == 61 && !tries[RACE_FIELD_RIGHT_ANSWER]);
    blocked_kind = RACE_FIELD_RIGHT_ANSWER;
    for (i = 0; i < failures; ++i) {
        answer_step(&panel);
        assert(race_submit_answer(&racer) == BEHAVIOR_CONTINUE);
        assert(panel_work.mode == 2 && panel_work.timer == 61);
        assert(tries[blocked_kind] == i + 1 && !issued[blocked_kind]);
        assert(race_scavenger_occupant(0) == &racer &&
               !race_scavenger_submitted[0]);
        assert(textures == 1 && !correct_sounds && !victories && !queued &&
               !next_behavior);
    }
    blocked_kind = 0;
    answer_step(&panel);
    assert(panel_work.mode == 3 && panel_work.timer == 62 && textures == 2 &&
           correct_sounds == 1);
    assert(race_submit_answer(&racer) == BEHAVIOR_CONTINUE);
    assert(submit_motions == 1 && resistance == 1 && victories == 1 &&
           queued == 1 && !next_behavior);
    for (i = 62; i < 149; ++i) {
        answer_step(&panel);
        assert(race_scavenger_occupant(0) == &racer);
    }
    answer_step(&panel);
    assert(panel_work.timer == 150 && !race_scavenger_occupant(0));
    assert(issued[RACE_FIELD_RIGHT_ANSWER] == 1 && textures == 2 &&
           correct_sounds == 1);
    assert(garden_unregister_object(&toy));
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    racer.work = &racer_work.base;
    assert(garden_register_object(GARDEN_OBJECT_CHAO, &racer, 0));
    if (!strcmp(argv[1], "answer")) {
        run_answer(200);
    } else if (!strcmp(argv[1], "normal-tree")) {
        run_tree(0);
    } else if (!strcmp(argv[1], "normal-answer")) {
        run_answer(0);
    } else if (!strcmp(argv[1], "fruit")) {
        run_tree(RACE_FIELD_FRUIT_DROP);
    } else {
        assert(!strcmp(argv[1], "entrant"));
        run_tree(RACE_FIELD_ENTRANT);
    }
    assert(garden_unregister_object(&racer));
    return 0;
}
