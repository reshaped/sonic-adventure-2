/* golem_explosion_create where the heap can't give the explosion's record:
 * the call returns null and leaves no task, so no step reads the record. with
 * the record, the call returns a task that steps and ends */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "boss_golem/explosion.c"
#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "sound/sound.h"
#include "system/timer.h"
#include "system/util.h"

NJS_CNK_MODEL blast_model_1;
NJS_TEXLIST blast_texlist;
struct texture_anim blast_texture_anim;
struct camera_place *camera_views[CAMERAS];
int camera_current_index;

static int refuse_record, live;

void *syMalloc(Uint32 size)
{
    void *block;

    if (refuse_record && size == sizeof(struct golem_explosion)) {
        return NULL;
    }
    block = malloc(size);
    assert(block != NULL);
    memset(block, 0xa5, size);
    live++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = syMalloc(count * size);

    memset(block, 0, count * size);
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live > 0);
    live--;
    free(block);
}

void collision_free(struct task *task)
{
}

void sound_detach_task(const struct task *task)
{
}

void profile_step_start(void (*step)(struct task *task), const char *name)
{
}

void profile_step_end(void (*step)(struct task *task))
{
}

void texture_anim_apply(NJS_CNK_MODEL *model, struct texture_anim *anim,
                        Uint32 time)
{
}

void black_smoke_rising_burst(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                              Float scale, int count)
{
}

/* the draw's calls, which no test reaches */
void golem_simple_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
}

void constant_material_set(Float alpha, Float red, Float green, Float blue)
{
}

void draw_attr_save(void)
{
}

void draw_attr_restore(void)
{
}

void draw_control_save(void)
{
}

void draw_control_restore(void)
{
}

void draw_control_set(Uint32 bits)
{
}

void draw_control_clear(Uint32 bits)
{
}

Bool njPushMatrixEx(void)
{
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    return TRUE;
}

void njTranslateEx(NJS_VECTOR *v)
{
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    return 0;
}

void njSetCnkBlendMode(Uint32 attr)
{
}

void njSetConstantAttr(Uint32 and_attr, Uint32 or_attr)
{
}

int main(void)
{
    NJS_POINT3 pos = {1.0F, 2.0F, 3.0F};
    struct task *task;
    int step;

    refuse_record = 1;
    assert(golem_explosion_create(&pos, 1.0F, 2.0F, 4.0F) == NULL);
    assert(task_levels[EXPLOSION_LEVEL] == NULL && live == 0);
    refuse_record = 0;
    task = golem_explosion_create(&pos, 1.0F, 2.0F, 4.0F);
    assert(task != NULL && task_levels[EXPLOSION_LEVEL] == task);
    for (step = 0; step < 5; step++) {
        task_step_level(EXPLOSION_LEVEL);
    }
    assert(task_levels[EXPLOSION_LEVEL] == NULL && live == 0);
    return 0;
}
