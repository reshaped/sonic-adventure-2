#include <assert.h>
#include <string.h>

#include "chao/egg.c"
#include "chao/impact.c"

static struct task egg, child, pickup, attacker;
static struct chao_work egg_work, child_work;
static struct task_work attacker_work;
static struct egg_state state;
static struct mover egg_mover, child_mover;
static struct chao_save resident;
static int can_hatch, can_drop;
static int hatches, unregisters, behaviors, attempts, drops, sounds, moves;
static int random_calls, expected_hat;
static chao_behavior_fn last_behavior;
struct chao_tuning chao_tuning;

/* every draw is DRAW, the middle of SHC's range. the shell's top scatters
 * along x and z by DRAW_SCATTER, which is 0 where RAND_MAX is 32767 */
#define DRAW 16384
#define DRAW_SCATTER                                                           \
    (SHELL_SCATTER - DRAW * ((2.0F * SHELL_SCATTER) * RAND_SCALE))
/* the yaw RANDOM_TURN() gives for DRAW: half a turn where RAND_MAX is 32767 */
#define DRAW_YAW ((Angle)(DRAW * (FULL_TURN * RAND_SCALE)))

static void reset_case(void)
{
    memset(&egg_work, 0, sizeof(egg_work));
    memset(&child_work, 0, sizeof(child_work));
    memset(&state, 0, sizeof(state));
    memset(&egg_mover, 0, sizeof(egg_mover));
    egg.work = &egg_work.base;
    egg.extra.p = &state;
    egg.motion.p = &egg_mover;
    egg_work.save = &resident;
    egg_work.base.pos.y = 12.0F;
    child.work = &child_work.base;
    child.motion.p = &child_mover;
    attacker.work = &attacker_work;
    hatches = unregisters = behaviors = attempts = drops = sounds = moves = 0;
    can_hatch = can_drop = random_calls = 0;
    expected_hat = GARDEN_HEADGEAR_SHELL;
    last_behavior = NULL;
}

int rand(void)
{
    random_calls++;
    return DRAW;
}

struct task *chao_create(const struct chao_gene *gene,
                         const struct chao_profile *profile,
                         const struct garden_race_chao *appearance,
                         NJS_POINT3 *position, Angle yaw,
                         struct chao_save *save)
{
    assert(save == &resident && position == &egg_work.base.pos);
    hatches++;
    return can_hatch ? &child : NULL;
}
int garden_unregister_object(struct task *task)
{
    assert(task == &egg);
    unregisters++;
    return 1;
}
void chao_set_behavior(struct task *task, chao_behavior_fn action)
{
    assert(task == &egg || task == &child);
    behaviors++;
    last_behavior = action;
}

struct task *garden_drop_headgear(int kind, NJS_POINT3 *position, Angle yaw,
                                  NJS_VECTOR *velocity)
{
    assert(kind == expected_hat && yaw == DRAW_YAW);
    if (kind == GARDEN_HEADGEAR_SHELL) {
        assert(position->y == 13.5F);
        assert(velocity->y == egg_mover.spd.y + 1.3F);
        assert(velocity->x == egg_mover.spd.x + DRAW_SCATTER);
        assert(velocity->z == egg_mover.spd.z + DRAW_SCATTER);
    } else {
        assert(position == &egg_work.base.pos && velocity == &egg_mover.spd);
        assert(egg_work.profile.parameters.hat == expected_hat);
    }
    attempts++;
    if (!can_drop) {
        return NULL;
    }
    drops++;
    return &pickup;
}
Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *position)
{
    assert(id == 0x1011 && position == &egg_work.base.pos);
    sounds++;
    return 0;
}
void collision_shape_off(struct task *task, int index)
{
    assert(task == &egg);
}
void mover_step(struct task *task)
{
    assert(task == &egg);
    moves++;
}
void mover_save_pos(struct task *task)
{
    assert(task == &egg);
}
int garden_meet_floor(struct task *task)
{
    assert(task == &egg);
    return 1;
}
Float njScalor(NJS_VECTOR *velocity)
{
    return 0.0F;
}
void njMemCopy(void *dst, const void *src, Sint32 length)
{
    memcpy(dst, src, length);
}
void chao_morph_body(struct task *task)
{
    assert(task == &child);
}
int chao_hatch_look_behavior(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
int chao_hatch_pop_behavior(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
int chao_die_behavior(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
int chao_evolve_behavior(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
chao_behavior_fn chao_current_behavior(struct task *task)
{
    return last_behavior;
}
struct task *collision_next_player_by_shape(struct task *task, int index)
{
    assert(task == &egg && index == 1);
    return &attacker;
}
Float njUnitVector(NJS_VECTOR *vector)
{
    return 1.0F;
}
void chao_link_motion_frames(struct task *task, Sint32 index, Uint16 frames)
{
}
void chao_set_eye(struct task *task, int eye)
{
}
void chao_show_mark(struct task *task, Sint8 mark, int frames)
{
}
void chao_player_affection(struct task *task, int delta, Float alignment)
{
}
void chao_fear_player(struct task *task, int delta)
{
}
void chao_emotion_add(struct task *task, Uint32 kind, int delta)
{
}

static void hatch(int mode, void (*step)(struct task *),
                  chao_behavior_fn behavior, int failed_drops)
{
    int i;
    reset_case();
    egg_work.base.mode = mode;
    step(&egg);
    assert(hatches == 1 && !unregisters && !behaviors && !random_calls);
    assert(egg_work.save == &resident && !egg_work.base.submode);
    can_hatch = 1;
    for (i = 0; i < 93; ++i) {
        step(&egg);
    }
    assert(hatches == 2 && unregisters == 1 && behaviors == 1);
    assert(last_behavior == behavior && !egg_work.save && !attempts);
    for (i = 0; i < failed_drops; ++i) {
        step(&egg);
        assert(attempts == i + 1 && !drops && random_calls == 3 * attempts);
        assert(egg_work.base.mode == mode && egg_work.base.submode == 1);
        assert(state.frame <= 15.0F && hatches == 2 && behaviors == 1);
    }
    can_drop = 1;
    step(&egg);
    assert(egg_work.base.mode == EGG_EMPTY && !egg_work.base.submode);
    assert(drops == 1 && attempts == failed_drops + 1 &&
           random_calls == 3 * attempts);
    assert(hatches == 2 && behaviors == 1 && !sounds);
}

static void broken(int failed_drops)
{
    int i;
    reset_case();
    egg_work.base.mode = EGG_BREAK;
    PERSONALITY_GENE(&egg, CHAO_KINDNESS)[0] = 2;
    egg_mover.spd.x = 0.5F;
    egg_mover.spd.y = 2.0F;
    egg_mover.spd.z = -1.0F;
    egg_break(&egg);
    assert(hatches == 1 && !unregisters && !behaviors && !random_calls);
    assert(PERSONALITY_GENE(&egg, CHAO_KINDNESS)[0] == 2);
    can_hatch = 1;
    for (i = 0; i < failed_drops; ++i) {
        egg_break(&egg);
        assert(hatches == 2 && unregisters == 1 && behaviors == 1);
        assert(!drops && !moves && !sounds && random_calls == 3 * (i + 1));
        assert(egg_work.base.mode == EGG_BREAK && egg_mover.spd.y == 2.0F);
    }
    can_drop = 1;
    egg_break(&egg);
    assert(last_behavior == chao_knockback_behavior &&
           child_mover.spd.y == 3.0F);
    assert(PERSONALITY_GENE(&egg, CHAO_KINDNESS)[0] == 1);
    assert(hatches == 2 && unregisters == 1 && behaviors == 1);
    assert(drops == 1 && attempts == failed_drops + 1 &&
           random_calls == 3 * attempts);
    assert(moves == 1 && sounds == 1 && egg_mover.spd.y == 2.7F);
    assert(egg_work.base.mode == EGG_BREAK && egg_work.base.submode == 1);
    egg_work.base.flags |= MOVER_WORK_ON_FLOOR;
    egg_break(&egg);
    assert(egg_work.base.mode == EGG_EMPTY && !egg_work.base.submode);
    assert(moves == 2 && sounds == 1 && drops == 1 && hatches == 2);
}

static void worn_hat(void)
{
    reset_case();
    expected_hat = egg_work.profile.parameters.hat = 10;
    egg_work.base.flags = WORK_HURT;
    assert(chao_try_knockback(&egg));
    assert(egg_work.profile.parameters.hat == 10 && attempts == 1 && !drops);
    last_behavior = NULL;
    can_drop = 1;
    egg_work.base.flags |= WORK_HURT;
    assert(chao_try_knockback(&egg));
    assert(!egg_work.profile.parameters.hat && drops == 1 && random_calls == 2);

    reset_case();
    expected_hat = egg_work.profile.parameters.hat = 12;
    egg_work.behavior.mode = 2;
    egg_mover.flags = MOVER_AT_WALL;
    chao_thrown_behavior(&egg);
    assert(egg_work.profile.parameters.hat == 12 && attempts == 1 && !drops);
    can_drop = 1;
    chao_thrown_behavior(&egg);
    chao_thrown_behavior(&egg);
    assert(!egg_work.profile.parameters.hat && drops == 1 && random_calls == 2);
}

int main(void)
{
    hatch(EGG_HATCH_LOOK, egg_hatch_look, chao_hatch_look_behavior, 0);
    hatch(EGG_HATCH_LOOK, egg_hatch_look, chao_hatch_look_behavior, 3);
    hatch(EGG_HATCH_POP, egg_hatch_pop, chao_hatch_pop_behavior, 0);
    hatch(EGG_HATCH_POP, egg_hatch_pop, chao_hatch_pop_behavior, 3);
    broken(0);
    broken(3);
    worn_hat();
    return 0;
}
