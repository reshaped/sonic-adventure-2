/* Draw a ricochet whose player's slot is empty. */
#include <assert.h>
#include <stdlib.h>

#include <ninja.h>
#include <sg_xpt.h>

#include "player/player.h"
#include "system/task.h"

/* the draw tests it only where the slot holds a task */
#define LASER_UPGRADES 0

struct task *player_tasks[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];

static NJS_SPRITE ricochet_sprite[1];
/* njFogEnable's calls less njFogDisable's */
static int fog;

void njFogDisable(void)
{
    fog--;
}
void njFogEnable(void)
{
    fog++;
}

/* the draw reaches none of these at an empty slot */
void njColorBlendingMode(Int target, Int mode)
{
    abort();
}
Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    abort();
}
void njDrawSprite3D(NJS_SPRITE *sp, Int n, Uint32 attr)
{
    abort();
}

#include "extracted.inc"

int main(void)
{
    struct ricochet ricochet = {0, 1};
    struct task task = {0};
    struct task_work work = {0};

    task.work = &work;
    task.extra.p = &ricochet;
    ricochet_draw(&task);
    assert(fog == 0);
    return 0;
}
