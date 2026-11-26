/* Included after transfer_machine.c to run its receive and delivery states. */
#include <assert.h>
#include <math.h>
#include <string.h>

static struct task machine, delivered[2];
static struct task_work machine_base, player_base;
static struct machine_work transport;
static struct player_motion standing;
static int attempts, samples, sounds, deliveries, capacity, allocations;
static int expected_kind, expected_category;

struct task_work *player_work[PLAYER_SLOTS] = {&player_base};
struct player_motion *player_motion[PLAYER_SLOTS] = {&standing};
struct player *player_records[PLAYER_SLOTS];
Sint32 garden_machine_in_use;

static int sample(int index)
{
    return (RAND_MAX / 8) * index;
}

int rand(void)
{
    ++samples;
    assert(samples == attempts + 1 && samples <= 6);
    return sample(samples);
}

Float njSin(Angle angle)
{
    return (Float)sin(angle / ANGLE_PER_RADIAN);
}
Float njCos(Angle angle)
{
    return (Float)cos(angle / ANGLE_PER_RADIAN);
}

static struct task *offer(int category, int kind, NJS_POINT3 *door, Angle yaw,
                          NJS_VECTOR *speed)
{
    Angle facing = machine_base.ang.y + QUARTER_TURN;
    ++attempts;
    assert(category == expected_category && kind == expected_kind);
    assert(samples == attempts);
    assert(yaw == facing - (Angle)(sample(attempts) *
                                   (128.0F * ANGLE_PER_DEGREE * RAND_SCALE)));
    assert(door->x == machine_base.pos.x - njSin(facing) * 13.3F);
    assert(door->y == machine_base.pos.y);
    assert(door->z == machine_base.pos.z - njCos(facing) * 13.3F);
    assert(speed->x == njSin(yaw) * 0.5F && speed->y == 2.0F);
    assert(speed->z == njCos(yaw) * 0.5F);
    if (!capacity) {
        return NULL;
    }
    capacity--;
    assert(deliveries < 2);
    return &delivered[deliveries++];
}

struct task *garden_seed_create(int kind, NJS_POINT3 *door, Angle yaw,
                                NJS_VECTOR *speed)
{
    return offer(GARDEN_OBJECT_SEED, kind, door, yaw, speed);
}
struct task *garden_fruit_create(int kind, NJS_POINT3 *door, Angle yaw,
                                 NJS_VECTOR *speed)
{
    return offer(GARDEN_OBJECT_FRUIT, kind, door, yaw, speed);
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = calloc(count, size);
    assert(block);
    ++allocations;
    return block;
}
void syFree(void *block)
{
    assert(block && allocations > 0);
    free(block);
    --allocations;
}
int garden_open_vmu_receive(struct garden_transfer *transfer)
{
    assert(transfer->chao && transfer->egg);
    transfer->items[0] = GARDEN_FRUIT_HEART - 3;
    transfer->items[1] = 2;
    transfer->item_count = 2;
    transfer->status = GARDEN_TRANSFER_BUSY;
    return 0;
}
Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *position)
{
    assert(position == &machine_base.pos);
    if (id == 0x1019) {
        ++sounds;
    } else {
        assert(id == 0); /* Opening door. */
    }
    return 0;
}
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *position)
{
    assert(id == 0x1018 && key == &machine);
    return 0;
}
struct task *collision_next_player_by_shape(struct task *task, int shape)
{
    assert(task == &machine && shape == MACHINE_BUTTON);
    return NULL;
}
Bool collision_register(struct task *task)
{
    assert(task == &machine);
    return TRUE;
}
Bool pad_is_enabled(void)
{
    return FALSE;
}
Angle angle_approach(Angle from, Angle to, Angle step)
{
    return to;
}

/* These paths require sending a resident or receiving a Chao. This receipt
 * contains garden items only. */
int garden_open_vmu_send(struct garden_transfer *transfer)
{
    abort();
}
void garden_pack_egg(struct task *egg, struct chao_save *save)
{
    abort();
}
void garden_pack_chao(struct task *chao, struct chao_save *save)
{
    abort();
}
int garden_object_category(struct task *task)
{
    abort();
}
struct chao_save *garden_unused_chao(void)
{
    abort();
}
struct task *chao_create(const struct chao_gene *gene,
                         const struct chao_profile *profile,
                         const struct garden_race_chao *appearance,
                         NJS_POINT3 *position, Angle yaw,
                         struct chao_save *save)
{
    abort();
}
struct task *chao_create_egg(struct chao_gene *gene, NJS_POINT3 *position,
                             enum chao_egg_origin origin,
                             struct chao_save *save)
{
    abort();
}
struct task *chao_create_egg_with_profile(struct chao_gene *gene,
                                          struct chao_profile *profile,
                                          NJS_POINT3 *position,
                                          enum chao_egg_origin origin,
                                          struct chao_save *save)
{
    abort();
}
void mover_set_spd(struct task *task, NJS_VECTOR *speed)
{
    abort();
}
void chao_remove_resident(struct task *chao)
{
    abort();
}
int chao_wait_behavior(struct task *chao)
{
    abort();
}
void chao_set_behavior(struct task *chao, chao_behavior_fn behavior)
{
    abort();
}
void player_throw_at_touch(struct task_work *work, struct player *player)
{
    abort();
}
void collision_shape_on(struct task *task, int shape)
{
    abort();
}
void collision_shape_off(struct task *task, int shape)
{
    abort();
}
void pad_enable(void)
{
    abort();
}
void pad_disable(void)
{
    abort();
}
void pause_enable(void)
{
    abort();
}
void pause_disable(void)
{
    abort();
}

static void release_item(int category, int kind)
{
    struct garden_transfer before;
    int initial = deliveries;
    int frame, trial;
    expected_category = category;
    expected_kind = kind;
    memcpy(&before, current_transfer, sizeof(before));
    for (trial = 0; trial < 3; ++trial) {
        capacity = trial == 2;
        assert(machine_base.mode == MACHINE_RELEASE_ITEM);
        machine_step(&machine);
        assert(machine_base.mode == MACHINE_WAIT_ITEM);
        if (trial == 2) {
            --before.item_count;
        }
        assert(!memcmp(&before, current_transfer, sizeof(before)));
        assert(deliveries == initial + (trial == 2) && sounds == deliveries);
        for (frame = 0; frame < 32; ++frame) {
            assert(machine_base.mode == MACHINE_WAIT_ITEM);
            machine_step(&machine);
        }
    }
}

int main(void)
{
    int frame;
    machine.work = &machine_base;
    machine.extra.p = &transport;
    machine_base.pos.x = 21.0F;
    machine_base.pos.y = -5.0F;
    machine_base.pos.z = 42.0F;
    machine_base.ang.y = 0x1200;
    machine_base.mode = MACHINE_RECEIVE_START;
    transport.door = DOOR_SHUT;
    machine_step(&machine);
    assert(current_transfer && machine_base.mode == MACHINE_RECEIVE_READY);
    machine_step(&machine);
    assert(machine_base.mode == MACHINE_RECEIVE);
    current_transfer->status = GARDEN_TRANSFER_DONE;
    for (frame = 0; frame < 240 && machine_base.mode != MACHINE_RELEASE_ITEM;
         ++frame) {
        machine_step(&machine);
    }
    assert(transport.door == DOOR_OPEN && current_transfer->item_count == 2);
    assert(!attempts && !sounds);
    release_item(GARDEN_OBJECT_SEED, 2);
    release_item(GARDEN_OBJECT_FRUIT, GARDEN_FRUIT_HEART);
    assert(attempts == 6 && samples == 6 && deliveries == 2 && sounds == 2);
    machine_step(&machine);
    assert(machine_base.mode == MACHINE_FINISH);
    machine_step(&machine);
    assert(machine_base.mode == MACHINE_RESET && !current_transfer &&
           !allocations);
    return 0;
}
