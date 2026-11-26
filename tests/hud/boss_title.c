/* Boss title admission and the first letter tick on the real task scheduler. */
#include <assert.h>
#include <stdlib.h>

#include <shinobi.h>

#include "collision/collision.h"
#include "shared/boss_title.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

struct game_state game_state;

static int allocated;
static int allocation_calls;
static NJS_TEXLIST textures;
static struct boss_title_letter letters[127];
static struct boss_title_letter bar[127];

void *syMalloc(Uint32 size)
{
    void *block = malloc(size);
    assert(size != 0 && block != NULL);
    allocated++;
    allocation_calls++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = calloc(count, size);
    assert(count != 0 && size != 0 && block != NULL);
    allocated++;
    allocation_calls++;
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && allocated > 0);
    allocated--;
    free(block);
}

void collision_free(struct task *task)
{
    abort();
}

void sound_detach_task(const struct task *task)
{
}

int stage_current_id(void)
{
    return STAGE_BOSS_BIGFOOT;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    abort();
}

/* This fixture steps an invisible space and never draws a texture. */
void njDrawTexture(NJS_TEXTURE_VTX *vertices, Int count, Int texture, Int flags)
{
    abort();
}

Sint32 njSetTexture(NJS_TEXLIST *list)
{
    abort();
}

void njTextureFilterMode(Int mode)
{
    abort();
}

void njColorBlendingMode(Int target, Int mode)
{
    abort();
}

void njFogEnable(void)
{
    abort();
}

void njFogDisable(void)
{
    abort();
}

Float njInvertSqrt(Float value)
{
    abort();
}

static void rejected(const struct boss_title *title)
{
    int before = allocation_calls;
    assert(boss_title_create(NULL, title) == NULL);
    assert(allocation_calls == before && allocated == 0);
}

int main(void)
{
    static struct boss_title title;
    struct task *task;
    int i;

    task_init();
    title.texlist = &textures;
    title.letters = letters;
    title.bar = bar;
    rejected(&title); /* The disc-free descriptor preserves both pointers. */
    title.letter_count = -1;
    rejected(&title);
    title.letter_count = 128;
    rejected(&title);
    title.letter_count = 1;
    rejected(&title); /* A provided bar must have at least one piece. */
    title.bar_count = -1;
    rejected(&title);
    title.bar_count = 128;
    rejected(&title);
    title.bar_count = 1;
    title.letters = NULL;
    rejected(&title);
    title.letters = letters;

    /* The upper bound admits the full title, including its optional bar. */
    title.letter_count = 127;
    title.bar_count = 127;
    task = boss_title_create(NULL, &title);
    assert(task != NULL && task->child != NULL);
    task_destroy(task);
    assert(allocated == 0);

    /* A one-letter title without a bar advances past its last letter. */
    title.letter_count = 1;
    title.bar = NULL;
    letters[0].wait = 1;
    task = boss_title_create(NULL, &title);
    assert(task != NULL && task->child == NULL);
    for (i = 0; i < 21; i++) {
        task->step(task);
    }
    assert(task->work->submode == 1 && task->work->tick == 0);
    task_destroy(task);
    assert(allocated == 0);
    return 0;
}
