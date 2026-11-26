#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "engine/dc/pvm.h"
#include "engine/dc/sh4zam_splash.h"
#include "otherprint/logos.h"
#include "otherprint/otherprint.h"
#include "otherprint/tento_ending.h"
#include "shared/screen_sprite.h"
#include "shared/staff_roll.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

Bool otherprint_done;
struct game_state game_state;
Uint32 pad_press_repeat[PAD_PORTS];
NJS_TEXLIST staff_roll_texlist;
NJS_TEXANIM staff_roll_cells[STAFF_ROLL_CELLS];

static struct task presentation;
static struct task_work work;
static NJS_TEXLIST *held[3];
static int loads;
static int splash_loads;
static int splash_draws;
static int active;
static int fail_load;
static int fail_task;
static int fail_malloc;
static int fail_splash_upload;
static int outstanding_allocs;

void *syMalloc(Uint32 nbytes)
{
    void *block;

    if (fail_malloc) {
        return NULL;
    }
    block = malloc(nbytes);
    if (block != NULL) {
        outstanding_allocs++;
    }
    return block;
}

void syFree(void *ap)
{
    assert(ap != NULL && outstanding_allocs > 0);
    outstanding_allocs--;
    free(ap);
}

Bool texture_load_pvm(const char *name, NJS_TEXLIST *list)
{
    int slot = list == &staff_roll_texlist;

    assert(!held[slot]);
    assert(!strcmp(
        name, slot ? "staffRoll"
                   : (list->nbTexture == 11 ? "segalogo" : "tentoEnding")));
    if (++loads == fail_load) {
        return FALSE;
    }
    held[slot] = list;
    return TRUE;
}

Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target)
{
    assert(archive != NULL && size == sh4zam_splash_pvm_size());
    assert(target != NULL && target->nbTexture == 1);
    assert(!held[2]);
    splash_loads++;
    if (fail_splash_upload) {
        return FALSE;
    }
    held[2] = target;
    return TRUE;
}

void texture_discard_pvm(NJS_TEXLIST *list)
{
    int slot;

    if (list == held[2]) {
        slot = 2;
    } else {
        slot = list == &staff_roll_texlist;
    }
    assert(held[slot] == list);
    held[slot] = NULL;
}

Sint32 njReleaseTexture(NJS_TEXLIST *list)
{
    texture_discard_pvm(list);
    return 1;
}

void njDrawSprite2D(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr)
{
    assert(sp != NULL && n == 0);
    assert(sp->tlist == held[2] && held[2] != NULL);
    assert(sp->p.x == 320.0F && sp->p.y == 192.0F);
    assert(sp->sx == 0.78125F && sp->sy == 0.78125F);
    assert(sp->tanim[0].sx == 384 && sp->tanim[0].sy == 256);
    assert(sp->tanim[0].u1 == 0 && sp->tanim[0].v1 == 0);
    assert(sp->tanim[0].u2 == 192 && sp->tanim[0].v2 == 255);
    assert(pri == -10.0F);
    assert(attr == (NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA));
    splash_draws++;
}

struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    assert(!active && flags == TASK_WORK && level == 2 && name && step);
    if (fail_task) {
        return NULL;
    }
    memset(&presentation, 0, sizeof(presentation));
    memset(&work, 0, sizeof(work));
    presentation.work = &work;
    presentation.step = step;
    active = 1;
    return &presentation;
}

void task_destroy(struct task *task)
{
    assert(task == &presentation && active);
    if (task->destructor) {
        task->destructor(task);
    }
    active = 0;
}

Bool float_step_toward(Float *value, Float target, Float step)
{
    (void)step;
    if (*value == target) {
        return FALSE;
    }
    *value = target;
    return TRUE;
}

void screen_draw_begin(Float level)
{
    (void)level;
}
void screen_draw_end(void)
{
}
void screen_sprite_draw(struct screen_sprite *sprite)
{
    (void)sprite;
}
void screen_picture_draw(Int id, NJS_TEXLIST *list, Float priority, Uint32 attr)
{
    (void)id;
    (void)list;
    (void)priority;
    (void)attr;
}

static void open_presentation(void (*start)(void), int failing_load,
                              int failing_task, int expected_loads, int ready)
{
    loads = 0;
    splash_loads = 0;
    splash_draws = 0;
    fail_load = failing_load;
    fail_task = failing_task;
    fail_malloc = 0;
    fail_splash_upload = 0;
    otherprint_done = FALSE;
    start();
    assert(loads == expected_loads && active == ready);
    assert(otherprint_done == !ready);
    if (ready) {
        presentation.step(&presentation);
        assert(presentation.draw && presentation.destructor);
        task_destroy(&presentation);
    }
    assert(!active && !held[0] && !held[1] && !held[2] && otherprint_done);
    assert(outstanding_allocs == 0);
}

static int run_logos_to_completion(int press_start_on_splash)
{
    int steps = 0;

    loads = 0;
    splash_loads = 0;
    splash_draws = 0;
    fail_load = 0;
    fail_task = 0;
    pad_press_repeat[0] = 0;
    otherprint_done = FALSE;
    logos_start();
    assert(active && !otherprint_done);
    while (active) {
        assert(steps < 2000);
        if (press_start_on_splash && held[2] != NULL && splash_draws >= 2) {
            pad_press_repeat[0] = PDD_DGT_ST;
        } else {
            pad_press_repeat[0] = 0;
        }
        presentation.step(&presentation);
        if (active && presentation.draw != NULL) {
            presentation.draw(&presentation);
        }
        steps++;
    }
    pad_press_repeat[0] = 0;
    assert(!held[0] && !held[1] && !held[2] && otherprint_done);
    assert(outstanding_allocs == 0);
    return steps;
}

static void test_splash_lifecycle(void)
{
    int full_steps;
    int skipped_steps;

    /* Normal boot sequence: SEGA -> Sonic Team -> SH4ZAM -> done. */
    game_state.edition = 0;
    fail_malloc = 0;
    fail_splash_upload = 0;
    full_steps = run_logos_to_completion(0);
    assert(splash_loads == 1 && splash_draws > 120);

    /* Pressing Start during SH4ZAM skips the 120-step hold. */
    skipped_steps = run_logos_to_completion(1);
    assert(splash_loads == 1 && splash_draws >= 2);
    assert(skipped_steps + 100 < full_steps);

    /* Tento sequence runs SH4ZAM and releases it before Sofdec finishes. */
    game_state.edition = EDITION_TENTO;
    run_logos_to_completion(0);
    assert(splash_loads == 1 && splash_draws > 120);

    /* Allocation or upload failure degrades cleanly without drawing. */
    game_state.edition = 0;
    fail_malloc = 1;
    run_logos_to_completion(0);
    assert(splash_loads == 0 && splash_draws == 0);
    fail_malloc = 0;

    fail_splash_upload = 1;
    run_logos_to_completion(0);
    assert(splash_loads == 1 && splash_draws == 0);
    fail_splash_upload = 0;

    /* Early destruction while SH4ZAM is on screen releases the VRAM texture. */
    loads = 0;
    splash_loads = 0;
    splash_draws = 0;
    otherprint_done = FALSE;
    logos_start();
    while (active && held[2] == NULL) {
        presentation.step(&presentation);
    }
    assert(active && held[2] != NULL);
    task_destroy(&presentation);
    assert(!active && !held[0] && !held[1] && !held[2] && otherprint_done);
    assert(outstanding_allocs == 0);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (!strcmp(argv[1], "empty")) {
        open_presentation(logos_start, 0, 0, 0, 0);
        open_presentation(tento_ending_start, 0, 0, 0, 0);
        return 0;
    }
    game_state.edition = EDITION_TENTO;
    open_presentation(logos_start, 1, 0, 1, 0);
    open_presentation(logos_start, 0, 1, 1, 0);
    open_presentation(logos_start, 2, 0, 2, 0);
    open_presentation(logos_start, 0, 0, 2, 1);
    game_state.edition = 0;
    open_presentation(logos_start, 0, 0, 1, 1);
    open_presentation(tento_ending_start, 1, 0, 1, 0);
    open_presentation(tento_ending_start, 0, 1, 1, 0);
    open_presentation(tento_ending_start, 0, 0, 1, 1);
    test_splash_lifecycle();
    return 0;
}
