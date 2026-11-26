/* Mad Space's step while no slot holds a player: it reads no player's pos for
 * the low wind, and leaves the gravity as it was */
#include <assert.h>

#include "modules/stg44/mad_space.c"

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];
struct camera_place *camera_views[CAMERAS];
NJS_VECTOR world_gravity;
Angle world_gravity_ang_x, world_gravity_ang_z;
struct path mad_space_first_carriage_path, mad_space_second_carriage_path,
    mad_space_platform_path;

/* nothing the step reaches without a player runs */
static void unreached(void)
{
    assert(!"the step read a player");
}

Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    (void)id;
    (void)key;
    (void)priority;
    (void)volume;
    (void)duration;
    (void)pos;
    unreached();
    return 0;
}

void field_sample_at(NJS_POINT3 *pos, struct field_sample *sample)
{
    (void)pos;
    (void)sample;
    unreached();
}

Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2)
{
    (void)v1;
    (void)v2;
    unreached();
    return 0.0F;
}

void vibration_start(int player, int power, int freq, int time)
{
    (void)player;
    (void)power;
    (void)freq;
    (void)time;
    unreached();
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    (void)flags;
    (void)level;
    (void)step;
    (void)name;
    unreached();
    return NULL;
}

struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent)
{
    (void)flags;
    (void)step;
    (void)parent;
    unreached();
    return NULL;
}

void mad_space_carriage_init(struct task *task)
{
    (void)task;
}

void mad_space_platform_init(struct task *task)
{
    (void)task;
}

int main(void)
{
    struct task manager = {0};
    struct task_work work = {0};
    struct task stage = {0};

    /* the carriages' manager is up, so the step makes none */
    carriage_manager = &manager;
    stage.work = &work;
    world_gravity.y = -1.0F;
    mad_space_step(&stage);
    assert(world_gravity.y == -1.0F);
    return 0;
}
