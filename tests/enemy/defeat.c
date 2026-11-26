/* An enemy in a group, without a collision record, beaten where it bursts and
 * leaves an animal and a chaos drive: enemy_defeat credits no player, and
 * each thing it leaves starts at the enemy's pos. */
#include <assert.h>

#include "enemy/enemy.c"

struct game_state game_state;
static NJS_POINT3 piece_pos;
static int piece_player = 0;
static int pieces;
static int bursts;
static int drives;
static int animals;
static NJS_POINT3 at = {1.0F, 2.0F, 3.0F};

static int at_pos(const NJS_POINT3 *pos)
{
    return pos->x == at.x && pos->y == at.y && pos->z == at.z;
}

void collision_restart_scans(void)
{
}

/* enemy_defeat must read no record to find the player */
struct task *collision_next_player(struct task *task)
{
    assert(0);
    return NULL;
}

int player_slot_of(struct task *task)
{
    assert(0);
    return -1;
}

void collision_get_shape_center(struct task_work *work,
                                const struct collision_shape *shape,
                                NJS_POINT3 *pos)
{
    assert(0);
}

void hunt_take_piece(int kind, Sint8 id, int player, NJS_POINT3 *pos)
{
    pieces++;
    piece_player = player;
    piece_pos = *pos;
}

void lockon_count_blast(struct task_work *work, struct enemy_work *enemy)
{
}

struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                       Float scale)
{
    assert(at_pos(pos));
    bursts++;
    return NULL;
}

/* the heap gives no dmyEnemy task */
struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    return NULL;
}

void task_kill(struct task *task)
{
}

Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos)
{
    return 0;
}

void chaos_drive_create(NJS_POINT3 *pos)
{
    assert(at_pos(pos));
    drives++;
}

struct task *animal_bubble_release(Float x, Float y, Float z)
{
    assert(x == at.x && y == at.y && z == at.z);
    animals++;
    return NULL;
}

int main(void)
{
    struct task_work work = {0};
    struct enemy_work enemy = {0};

    work.pos = at;
    enemy.group = 0;
    enemy.flags =
        ENEMY_BURST | ENEMY_RELEASES_ANIMAL | ENEMY_LEAVES_CHAOS_DRIVE;
    enemy_defeat(&work, &enemy);
    assert(pieces == 1 && piece_player == -1 && at_pos(&piece_pos));
    assert(bursts == 1 && drives == 1 && animals == 1);
    assert(work.flags & WORLD_ADDED);
    return 0;
}
