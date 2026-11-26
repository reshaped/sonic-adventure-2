/* the slam's wide blows where no foothold two slots from the golem's takes
 * one. the image's landed then holds the slot two before the golem's, so the
 * golem shakes from hand B unless that slot is 0. a build that defines
 * GOLEM_E runs boss_golem_e/actions.c, and one that doesn't runs
 * boss_golem/actions.c */
#include <assert.h>
#include <stddef.h>

#ifdef GOLEM_E
#include "boss_golem_e/actions.c"
#define STATE golem_e
#define G(name) golem_e_##name
NJS_POINT3 golem_e_head_pos;
#else
#include "boss_golem/actions.c"
#define STATE golem
#define G(name) golem_##name
NJS_POINT3 golem_flash_pos;
#endif

struct golem *STATE;
NJS_POINT3 G(hand_a_pos);
NJS_POINT3 G(hand_b_pos);
struct task_work *player_work[PLAYER_SLOTS];

static Sint32 index_now;
static Float frame_now;
static int shakes;
static NJS_POINT3 *shaken_from;

Sint32 motion_index(struct motion_control *control)
{
    return index_now;
}

Float motion_frame(struct motion_control *control)
{
    return frame_now;
}

int G(foothold_state)(int slot)
{
    return FOOTHOLD_GONE;
}

void golem_rumble(NJS_POINT3 *pos, Float size, int steps)
{
    shakes++;
    shaken_from = pos;
}

/* calls of the slam's other modes, which no test reaches */
void G(turn)(struct task *task)
{
}

void G(foothold_hit)(int slot)
{
    assert(!"no foothold takes a blow");
}

void G(foothold_break)(int slot)
{
}

Bool motion_ended(struct motion_control *control)
{
    return FALSE;
}

void motion_link(struct motion_control *control, Sint32 index)
{
}

void motion_link_frames(struct motion_control *control, Sint32 index,
                        Uint16 frames)
{
}

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    return 0.0F;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    return 0;
}

void sound_set_volume(const void *key, int id, Sint8 volume)
{
}

/* run the slam's wide blows once, with the golem at slot */
static void slam_wide(int slot)
{
    static struct golem state;
    static struct motion_control control;
    static struct boss_record record;
    static struct task task;

    STATE = &state;
    state.slot = slot;
    state.slam_state = FOOTHOLD_WHOLE;
    state.action_mode = MODE_SLAM_WIDE;
    record.block = &control;
    task.extra.p = &record;
    index_now = slam_motions[GOLEM_SLAM_BLOWS][FOOTHOLD_WHOLE];
    frame_now = slam_wide_frame;
    shakes = 0;
    shaken_from = NULL;
    action_slam(&task);
    assert(state.action_mode == MODE_SLAM_NEAR);
}

int main(void)
{
    /* the slot two before slot 0 is 10 */
    slam_wide(0);
    assert(shakes == 1 && shaken_from == &G(hand_b_pos));
    slam_wide(2);
    assert(shakes == 0);
    return 0;
}
