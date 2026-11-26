/* the land blocks' step of a Cannon's Core route, SOURCE, while slot 0 holds
 * no player and then while it holds one */
#include <assert.h>

#include SOURCE

struct game_state game_state;
struct task_work *player_work[PLAYER_SLOTS];

int main(void)
{
    struct task_work player;

    LAND_MASK = LAND_BLOCK(2);
    previous_mask = LAND_BLOCK(1);
    land_clip_step(NULL);
    assert(LAND_MASK == LAND_BLOCK(2) && previous_mask == LAND_BLOCK(2));

    /* the route's first block lies toward +z, past every bound */
    player.pos.x = 0.0F;
    player.pos.y = 0.0F;
    player.pos.z = 0.0F;
    player_work[0] = &player;
    land_clip_step(NULL);
    assert(LAND_MASK == LAND_BLOCK(1) && previous_mask == LAND_BLOCK(2));
    return 0;
}
